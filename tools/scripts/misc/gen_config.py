#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_config.py
┌----------------------------------------------------------------------------┐
│ Nuclei SDK CLI test-configuration generator                                │
│                                                                            │
│ Automatically produces a JSON test matrix from a list of ARCH/ABI strings  │
│ with the following built-in rules:                                         │
│  1. Extensions are randomly appended (_zba, _zbb …).  Extensions that      │
│     end with "_x" (_xxlcz, _xxldsp) are always placed last.                │
│  2. 32-/64-bit CORE is selected automatically (rv64* is only paired        │
│     with a core whose name contains the letter "x").                       │
│  3. DOWNLOAD type "ddr" is restricted to the 600/900-series cores.         │
└----------------------------------------------------------------------------┘

Usage examples
--------------
# Use the built-in list, add 2 random extensions, pretty-print
$ python gen_config.py -e 2 -p

# Supply your own list and write the result to disk
$ python gen_config.py -i my.list -o config.json -p

# Pipe the list through stdin
$ cat my.list | python gen_config.py -e 1 -p

# Full configuration with custom template
$ python gen_config.py -t custom.json -f -o full.json -e 3 -p
"""

import json
import random
import argparse
import sys
import re
from typing import List, Dict, Tuple

# --------------------------------------------------------------------------- #
# Constants                                                                   #
# --------------------------------------------------------------------------- #

# Valid download memory types for the build configuration.
DOWNLOAD_OPTS = ["ilm", "flash", "flashxip", "sram", "ddr"]

# Extension pools for random selection.
# Note: Extensions ending in "_x" must appear at the end of the architecture string.
X_EXT = ["_xxlcz", "_xxldsp"]  # Special extensions that must be last.
Z_EXT = ["_zba", "_zbb", "_zbc", "_zbs", "_zicond", "_zk", "_zks"]  # Standard Z extensions.

# --------------------------------------------------------------------------- #
# Default Configuration Template
# This serves as the base structure when --full is used without a custom template.
# --------------------------------------------------------------------------- #
DEFAULT_TEMPLATE = {
    "run_config": {
        "target": "qemu",
        "hardware": {"baudrate": 115200, "timeout": 240},
        "qemu": {"timeout": 240},
    },
    "parallel": "-j",
    "build_target": "clean all",
    "build_config": {"SOC": "evalsoc", "BOARD": "nuclei_fpga_eval", "ARCH_EXT": ""},
    "build_configs": {},  # Will be populated by generated configurations.
    "appconfig": {},
    "expected": {
        "application/baremetal/demo_nice": {"run": True, "build": True},
        "application/baremetal/demo_dsp": {"run": False, "build": False}
    }
}

# Default list of ARCH/ABI pairs to use if no input is provided.
DEFAULT_ARCH_ABI = [
    "rv32emc_zfinx/ilp32e",
    "rv32emc_zdinx/ilp32e",
    "rv32emac_zfinx/ilp32e",
    "rv32emac_zdinx/ilp32e",
    "rv32imc_zfinx/ilp32",
    "rv32imc_zdinx/ilp32",
    "rv32em_zfinx_zca_zcb_zcmp/ilp32e",
    "rv32em_zdinx_zca_zcb_zcmp/ilp32e",
    "rv32ema_zfinx_zca_zcb_zcmp/ilp32e",
    "rv32ema_zdinx_zca_zcb_zcmp/ilp32e",
    "rv32im_zfinx_zca_zcb_zcmp/ilp32",
    "rv32im_zdinx_zca_zcb_zcmp/ilp32",
    "rv32imac_zfinx/ilp32",
    "rv32imac_zdinx/ilp32",
    "rv32ima_zfinx_zca_zcb_zcmp/ilp32",
    "rv32ima_zdinx_zca_zcb_zcmp/ilp32",
    "rv64imac_zfinx/lp64",
    "rv64imac_zdinx/lp64",
]

# Mapping from base architecture strings to corresponding Nuclei CPU core names.
# Used to select an appropriate CORE based on the RISC-V ISA string.
CORE_ARCH_MAP = {
    "rv32imc": "n200",
    "rv32emc": "n200e",
    "rv32iac": "n201",
    "rv32eac": "n201e",
    "rv32ic": "n202",
    "rv32ec": "n202e",
    "rv32emac": "n203e",
    "rv32imac": "n300",
    "rv32imafc": "n300f",
    "rv32imafdc": "n300fd",
    "rv64imac": "nx900",
    "rv64imafc": "nx900f",
    "rv64imafdc": "nx900fd",
}

# --------------------------------------------------------------------------- #
# Helper functions                                                           #
# --------------------------------------------------------------------------- #

def pick_extensions(max_cnt: int) -> str:
    """
    Randomly select up to `max_cnt` extensions from Z_EXT and X_EXT.

    Rules:
      - Total number of selected extensions <= max_cnt.
      - Z extensions are chosen first (random subset, then sorted).
      - X extensions (ending in '_x') are chosen from remaining count and appended last.
      - Result is a concatenated string (e.g., '_zba_zbb_xxldsp').
    """
    if max_cnt <= 0:
        return ""

    # Randomly decide how many Z extensions to pick (0 to min(len(Z_EXT), max_cnt))
    n_z = random.randint(0, min(len(Z_EXT), max_cnt))
    picked_z = sorted(random.sample(Z_EXT, n_z))  # Sort for deterministic ordering

    # Remaining slots for X extensions
    remain = max_cnt - n_z
    n_x = random.randint(0, min(len(X_EXT), remain))
    picked_x = sorted(random.sample(X_EXT, n_x))  # Also sorted

    # Z extensions come first, X extensions last (as required)
    return "".join(picked_z + picked_x)


def pick_core_archext(arch: str) -> Tuple[str, str]:
    """
    Given a full RISC-V architecture string (e.g., 'rv32imacbv_zfinx'),
    this function:
      1. Separates base architecture from standard extensions (after '_').
      2. Handles special suffixes like 'b', 'v', 'bv', 'vb' that are part of the base ISA.
      3. Maps the cleaned base architecture to a Nuclei CORE using CORE_ARCH_MAP.
      4. Returns the matched CORE name and the formatted ARCH_EXT string.

    Example:
      Input:  "rv32imacbv_zfinx"
      Output: ("n300", "bv_zfinx")

    The ARCH_EXT field in the output config will be "_bv_zfinx" (with leading underscore).
    """
    # Split into base (before first '_') and extension part (after first '_')
    arch_base, _, archext = arch.partition('_')

    # Check if base ends with special suffixes: 'b', 'v', 'bv', or 'vb'
    match = re.match(r'^(.*?)(bv|vb|b|v)$', arch_base)
    if match:
        # Extract the true base (without suffix) and the suffix
        arch_new, suffix = match.groups()
        # Combine suffix with original extension (if any)
        combined_ext = f"{suffix}_{archext}" if archext else suffix
        final_archext = f"_{combined_ext}"  # Always include leading underscore for non-empty
    else:
        # No special suffix; base is unchanged
        arch_new = arch_base
        combined_ext = archext
        final_archext = f"_{combined_ext}" if combined_ext else ""

    # Find the best matching core in CORE_ARCH_MAP
    best_match_core = None
    best_match_len = float('inf')  # Prefer shortest matching prefix (more specific)

    for map_arch, core_name in CORE_ARCH_MAP.items():
        # Only consider map entries that start with our cleaned base
        if map_arch.startswith(arch_new):
            # Among matches, pick the one with the shortest key (more precise match)
            if len(map_arch) < best_match_len:
                best_match_core = core_name
                best_match_len = len(map_arch)

    if best_match_core is None:
        raise ValueError(f"Could not find a matching core for arch: {arch_new}")

    return best_match_core, final_archext


def pick_download(core: str) -> str:
    """
    Select a random DOWNLOAD type from DOWNLOAD_OPTS.

    Constraint:
      - 'ddr' is only allowed for cores containing '600' or '900' in their name
        (e.g., 'nx900', 'n600' — though only 900 appears in current map).
    """
    pool = DOWNLOAD_OPTS.copy()
    # Remove 'ddr' if core is not in 600/900 series
    if "600" not in core and "900" not in core:
        pool.remove("ddr")
    return random.choice(pool)


def optimize_archext(archext: str) -> str:
    """
    Optimize archext string for compatibility with Nuclei Qemu.
    - Replace '_zdinx' with '_zfinx_zdinx' if present for Nuclei Qemu 2025.02.
    """
    if "_zdinx" in archext and "_zfinx" not in archext:
        archext = archext.replace("_zdinx", "_zfinx_zdinx")
    return archext

def build_one(arch_abi: str, max_ext: int) -> Dict[str, Dict[str, str]]:
    """
    Process a single "ARCH/ABI" string (e.g., "rv32imc_zfinx/ilp32") into a full config entry.

    Steps:
      1. Split into ARCH and ABI.
      2. Append up to `max_ext` random extensions to ARCH.
      3. Determine CORE and ARCH_EXT using `pick_core_archext`.
      4. Choose DOWNLOAD type based on CORE.
      5. Return a dict with a unique key: "{final_arch}-{download}".
    """
    if "/" not in arch_abi:
        raise ValueError(f"Malformed arch_abi: {arch_abi}")

    arch, abi = arch_abi.strip().split("/", 1)
    # Append randomly selected extensions to the base architecture string
    final_arch = arch + pick_extensions(max_ext)

    # Determine core and formatted extension string
    core, archext = pick_core_archext(final_arch)
    download = pick_download(core)
    archext = optimize_archext(archext)

    # Create a unique key for this configuration
    key = f"{final_arch}-{download}"
    return {
        key: {
            "DOWNLOAD": download,
            "CORE": core,
            "RISCV_ARCH": final_arch,
            "RISCV_ABI": abi,
            "ARCH_EXT": archext,
        }
    }


def build_all(lines: List[str], max_ext: int) -> Dict:
    """
    Process a list of ARCH/ABI strings (one per line), skipping empty lines and comments.
    Returns a dictionary of all generated configurations.
    """
    result = {}
    for line in lines:
        line = line.strip()
        if not line or line.startswith("#"):
            continue  # Skip blank lines and comments
        result.update(build_one(line, max_ext))
    return result


# --------------------------------------------------------------------------- #
# Command-line interface                                                     #
# --------------------------------------------------------------------------- #
def main(argv=None):
    """
    Main entry point. Parses arguments, reads input, generates config, and outputs JSON.
    """
    parser = argparse.ArgumentParser(
        description="Nuclei SDK CLI test-configuration generator — "
        "randomly pick CORE/DOWNLOAD/extension suffixes while "
        "respecting built-in constraints.",
        epilog=(
            "Examples:\n"
            "  python gen_config.py -e 2 -p\n"
            "        # Use built-in ARCH list, append 2 random extensions, pretty-print to stdout\n\n"
            "  python gen_config.py -i my.list -o config.json -p\n"
            "        # Read ARCH/ABI pairs from my.list, write pretty JSON to config.json\n\n"
            "  cat my.list | python gen_config.py -e 1 -p\n"
            "        # Read ARCH/ABI from stdin, append 1 random extension, pretty-print\n\n"
            "  python gen_config.py -t custom.json -f -o full.json -e 3\n"
            "        # Load custom template, generate full config with 3 random extensions\n\n"
            "Built-in rules:\n"
            "  * RV64* architectures are only paired with CORE names containing 'x'.\n"
            "  * RV32* architectures must use CORE names without 'x'.\n"
            "  * DOWNLOAD 'ddr' is restricted to 600/900-series cores.\n"
            "  * Extensions ending with '_x' are always placed last.\n"
        ),
        formatter_class=argparse.RawTextHelpFormatter,
    )
    parser.add_argument(
        "-i", "--input",
        type=str,
        help="Text file containing ARCH/ABI pairs (one per line). "
        "If omitted and stdin is empty, the built-in list is used.",
    )
    parser.add_argument(
        "-o", "--output",
        type=str,
        help="Output JSON file. If omitted, the result is printed to stdout.",
    )
    parser.add_argument(
        "-e", "--ext",
        type=int,
        default=0,
        help="Maximum number of random extensions to append (default: 0).",
    )
    parser.add_argument(
        "-p", "--pretty",
        action="store_true",
        help="Pretty-print the resulting JSON (indent 2).",
    )
    parser.add_argument(
        "-q", "--quiet",
        action="store_true",
        help="Suppress runtime messages.",
    )
    parser.add_argument(
        "-t", "--template",
        metavar="FILE",
        help="Path to custom JSON template. If omitted, a built-in template is used.",
    )
    parser.add_argument(
        "-f", "--full",
        action="store_true",
        help="Emit the full configuration object (template + build_configs) "
        "instead of only the build_configs section.",
    )
    args = parser.parse_args(argv)

    # Load configuration template (custom or default)
    if args.template:
        with open(args.template, encoding="utf-8") as f:
            template = json.load(f)
    else:
        template = DEFAULT_TEMPLATE

    # Read input ARCH/ABI list
    if args.input:
        # Read from specified file
        with open(args.input, encoding="utf-8") as f:
            lines = f.readlines()
    else:
        # If stdin has data (e.g., piped input), use it; otherwise use built-in list
        lines = sys.stdin.readlines() if not sys.stdin.isatty() else DEFAULT_ARCH_ABI

    if not args.quiet:
        print(f"/* Generating configuration for {len(lines)} ARCH/ABI entries ... */")

    # Generate all build configurations
    build_configs = build_all(lines, args.ext)

    # Decide what to output: full template or just build_configs
    if args.full:
        template["build_configs"] = build_configs
        data = template
    else:
        data = build_configs

    # Serialize to JSON
    out = json.dumps(data, indent=2) if args.pretty else json.dumps(data)

    # Write to file or stdout
    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(out)
        print(f"/* Written to {args.output} */")
    else:
        print(out)


if __name__ == "__main__":
    main()
