# buildsystem Regression Tests

This directory hosts regression test scripts for the Nuclei SDK **build
system** (the Makefiles under `Build/`), currently covering the features
introduced by the `hqfang/buildtodir` branch (output directory `O=`,
configuration fingerprint `.buildconfig`, configuration-change detection,
the `rebuild` target, the `version` target, build hints, etc.).

## Files

| File | Description |
|------|-------------|
| `buildtodir_regression.py` | Main regression script, 19 cases / 60+ checks, covering all core behavior of the buildtodir branch |

The script supports `--platform win|linux` with consistent logic; no
absolute tool paths are hardcoded — required tools (make /
riscv64-unknown-elf-gcc / qemu / openocd) are always resolved from the
current **PATH**.

All scripts and their output are English-only.

## Windows usage

Windows needs `make` and the Nuclei toolchain in PATH (e.g. NucleiStudio's
`toolchain/build-tools/bin` and `toolchain/gcc/bin`), with QEMU under
`toolchain/qemu/bin`.

```bash
cd <SDK_ROOT>
python tools/scripts/tests/buildsystem/buildtodir_regression.py --platform win
python tools/scripts/tests/buildsystem/buildtodir_regression.py --platform win --only 5   # run case 5 only
```

## Linux / WSL usage

Linux needs the Linux tools in PATH (`make`, `riscv64-unknown-elf-gcc`,
Nuclei's `qemu-system-riscv32`). Example with the author's WSL tool
directories:

```bash
export PATH=/home/riscv/nuclei/gcc/bin:/home/riscv/nuclei/qemu/bin:$PATH
cd /mnt/c/<path>/nuclei-sdk-clean   # or cd directly into the WSL workspace
python3 tools/scripts/tests/buildsystem/buildtodir_regression.py --platform linux
```

> Do not hardcode specific tool directories (e.g. `/home/riscv/nuclei/...`)
> into the scripts — they only check whether tools exist in PATH; missing
> required tools (gcc/make) cause an error exit, missing optional tools
> (qemu/openocd) cause related cases to be skipped with a notice.

## Tool dependencies

| Tool | Required/Optional | Behavior if missing |
|------|-------------------|---------------------|
| `make` (GNU Make >= 4.0, needed for `$(file)`) | Required | Error exit |
| `riscv64-unknown-elf-gcc` / `riscv64-unknown-elf-g++` | Required | Error exit |
| `qemu-system-riscv32` (with `nuclei_evalsoc` machine support) | Optional | Skips case 15 |
| `openocd` | Optional | Skips the rc check for showtoolver/version in case 7 |

## Exit codes

- `0`: all pass (SKIP cases count as passing but are not counted as PASS)
- `1`: some case FAILED
- `2`: required tools missing

## Case list (buildtodir_regression.py)

1. `O=` output-directory build and artifact placement
2. Object split `_obj_sdk` / `_obj_app`
3. Configuration fingerprint `.buildconfig` content (incl. evalsoc extended variables)
4. Multiple configurations coexisting (different `O=`)
5. Configuration-change detection (CORE change)
6. `rebuild` target
7. Fingerprint-check-exempt targets (info/showflags/showtoolver/version/help)
8. `clean` only cleans the selected `O=`
9. `__up__` external source path conversion
10. Object path collision detection
11. `version` target
12. `help` update
13. build hint (SOC=evalsoc)
14. build hint suppression with `SILENT=1`
15. QEMU run of `O=` artifacts
16. Default build without `O=` (regression)
17. Symmetric-difference and non-CORE variable change detection
18. `make -j clean all` serialization
19. Absolute Windows-path `O=` (regression guard)

## Terminal safety (QEMU)

QEMU is launched with its stdin detached from the TTY (`stdin=DEVNULL`), so
`-nographic -serial stdio` cannot put the terminal into raw mode. On timeout
the QEMU process tree is killed, and a best-effort `stty sane` restore runs
after every QEMU run and in the script's top-level `finally`, so the terminal
keeps working after the test without needing `reset`.
