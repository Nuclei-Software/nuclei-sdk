#!/bin/env python3

import os
import re
import sys


# Error keywords detected in the profiling/coverage log, grouped by category.
# Category A: heap memory (HEAP) insufficient -> malloc fails
HEAP_ERROR_KEYWORDS = [
    "monstartup: out of memory",
    "gprof_collect: unable to malloc enough memory to store gprof data",
    "ERROR: Can't allocate gcda buffer for",
    "Can't allocate gcda buffer for",
    "_mcleanup: tos overflow",
]

# Category B: file / IO failure -> fopen fails
FILE_IO_ERROR_KEYWORDS = [
    "Unable to open",
]


def check_error_patterns(logfile):
    """
    Scan the log file for common gprof/gcov error keywords and print
    diagnostic hints to help the user resolve them.

    Args:
        logfile (str): Path to the log file to process.

    Returns:
        bool: True if any error was detected, False otherwise.
    """
    detected = {}

    try:
        with open(logfile, "r") as lf:
            content = lf.read()
    except OSError:
        print(f"{logfile} does not exist. Please check!")
        return False

    # Scan the log line by line to avoid a single line matching multiple
    # keywords (some keywords contain others, e.g. the "ERROR:" prefixed
    # version of "Can't allocate gcda buffer for"). For each line, pick the
    # longest matching keyword so the message is only reported once.
    seen_lines = set()

    for line in content.splitlines():
        line = line.strip()
        if not line or line in seen_lines:
            continue

        line_keywords = []
        for keyword in HEAP_ERROR_KEYWORDS:
            if keyword in line:
                line_keywords.append(keyword)
        if line_keywords:
            # Only report the most specific (longest) keyword for this line
            detected.setdefault("heap", []).append(max(line_keywords, key=len))
            seen_lines.add(line)
            continue

        # Detect file/IO error keywords (prefix match for "Unable to open")
        if line.startswith("Unable to open"):
            detected.setdefault("file", []).append(line.strip())
            seen_lines.add(line)

    if not detected:
        return False

    print("\n" + "=" * 60)
    print("NOTE: Detected potential profiling/coverage errors in the log:")
    print("=" * 60)

    if "heap" in detected:
        print("\n[Category A] Heap memory (HEAP) insufficient -> malloc fails:")
        for kw in detected["heap"]:
            print(f"    - Detected message: {kw}")
        print("    Please check the following:")
        print("      1. Use DOWNLOAD=sram or DOWNLOAD=ddr, avoid ilm (too small).")
        print("      2. Verify the actual available heap (__heap_start ~ __heap_end),")
        print("         not just __HEAP_SIZE; confirm _sbrk and no stack/heap overlap.")
        print("      3. Apply -pg / -coverage only to the target application sources,")
        print("         not globally to the whole SDK.")
        print("      4. Review PROGRAM_LOWPC / PROGRAM_HIGHPC ranges.")
        print("    Reference: 40KB heap may still be insufficient, 80KB+ is recommended.")

    if "file" in detected:
        print("\n[Category B] File / IO failure -> fopen fails:")
        for line in detected["file"]:
            print(f"    - Detected message: {line}")
        print("    Please check the following:")
        print("      - Using semihosting: ensure the semihosting library is linked")
        print("        and semihosting is enabled in the debugger/simulator.")
        print("      - Using a target filesystem (e.g. FATFS): ensure the driver is")
        print("        integrated, the path is mounted, and the medium is accessible.")
        print("      - Or use interface 0 (GDB dump) / interface 2 (parse.py dump")
        print("        from console) to avoid file write entirely.")

    print("\n" + "=" * 60)
    return True


def generate_binary_from_log(logfile):
    """
    Parses a log file to extract binary data sections and writes them to separate files.

    Args:
        logfile (str): Path to the log file to process.

    Returns:
        bool: True if processing was successful, False otherwise.
    """

    # Check if the log file exists
    if not os.path.isfile(logfile):
        print(f"{logfile} does not exist. Please check!")
        return False

    # Define regular expressions for data start/end patterns (consider making them more specific)
    datastart_pattern = r"Dump\s+.+?\s+start"
    dataend_pattern = r"Dump\s+.+?\s+finish"

    # Track processing state (0: idle, 1: collecting hex data, 2: generating binary file)
    state = 0

    # Initialize variables for storing hex data and generated filename
    hexstr = ""
    genfilename = ""

    # Open the log file in read mode
    with open(logfile, "r") as lf:
        for line in lf.readlines():
            # Remove leading/trailing whitespace
            line = line.strip()

            # Skip empty lines
            if not line:
                continue

            # Check for data start pattern
            if re.search(datastart_pattern, line):
                # Reset state for new data section
                state = 1
                hexstr = ""
                genfilename = ""
                continue

            # Check for data end pattern
            if re.search(dataend_pattern, line):
                # Reset state for idle
                state = 0
                continue

            # Check for "CREATE" line indicating filename
            if line.startswith("CREATE"):
                # Extract filename and reset state for collecting hex data
                state = 2
                genfilename = line.strip("CREATE:").strip()

            # Append hex data to hexstr while collecting data
            if state == 1:
                hexstr += line

            # Process extracted hex data and create binary file
            if state == 2 and genfilename:
                try:
                    # Attempt to convert hex string to bytes (handle potential conversion errors)
                    binarydata = bytes.fromhex(hexstr)
                    print(f"Generating {genfilename}")
                    # Open the binary file in write-binary mode
                    with open(genfilename, "wb") as wf:
                        wf.write(binarydata)
                except ValueError:
                    print(f"Error: Invalid hex data when creating : {genfilename}")

                # Reset variables for next data section
                hexstr = ""
                genfilename = ""
                state = 1

    return True

# Call in Nuclei Studio IDE Terminal in a Project Directory like this
# NOTE: prof.log is the console log print in qemu console or uart console like Dump xxx start ... Dump xxx finish
# python nuclei_sdk/Components/profiling/parse.py prof.log
if __name__ == "__main__":
    if len(sys.argv) > 1:
        logfile = sys.argv[1]
        print(f"Parsing log file {logfile}")
        # First check for common error patterns and print diagnostic hints
        check_error_patterns(logfile)
        print(f"\nParsing {logfile} to generate binary files...")
        generate_binary_from_log(logfile)
    else:
        print(f"Help: {sys.argv[0]} logfile")
