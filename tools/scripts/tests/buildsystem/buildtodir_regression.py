#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Nuclei SDK buildtodir branch regression test (reproducible on Windows and Linux/WSL)

Covers the core behavior of the buildtodir branch, all verified through real make invocations:
  1. O= output-directory build and artifact placement
  2. Object split into _obj_sdk / _obj_app
  3. Configuration fingerprint .buildconfig content
  4. Multiple configurations coexisting (different O=)
  5. Configuration-change detection (list changed variables, stop build)
  6. rebuild target (fingerprint check bypass + clean reset)
  7. Fingerprint-check-exempt targets (info/showflags/showtoolver/version/help)
  8. clean only cleans the selected O
  9. __up__ external source path conversion
 10. Object path collision detection
 11. version target
 12. help update
 13. build hint (SOC=evalsoc shows sim)
 14. build hint suppression with SILENT=1
 15. QEMU run of O= artifacts
 16. Default build without O= (regression)
 17. Symmetric-difference and non-CORE variable change detection
 18. make -j clean all serialization
 19. Absolute Windows path O= (regression guard)
 20. REUSE=1 recall of the saved configuration
 21. REUSE=1 keeps the saved configuration across clean (no false positives)

Platforms:
  --platform win    : Windows native, invoking make via cmd /c
  --platform linux  : Linux/WSL, invoking make directly
  --platform auto   : default, chosen by os.name

Toolchain: no absolute tool paths are hardcoded. make/gcc/qemu/openocd are all
resolved from the current PATH (add tool directories to PATH before running,
e.g. under WSL: export PATH=/home/riscv/nuclei/gcc/bin:/home/riscv/nuclei/qemu/bin:$PATH).
Missing required tools (gcc/g++) cause an error exit; missing optional tools
(qemu/openocd) cause related cases to be skipped.

NOTE1: O= uses a relative directory name (e.g. build1), consistent with the
       intended usage of the branch.
NOTE2: Absolute Windows-path O= used to be corrupted by SANITIZED_OBJ's
       subst :,_, which broke the drive letter; this is now fixed (see
       Build/Makefile.rules). Case 19 is the regression guard for that fix.

Known uncovered areas (declared honestly to avoid false alarms):
  - pure OpenOCD server targets in the exempt list (run_openocd/run_xlspike_openocd):
    servers do not exit and require a target board, not included; their exempt
    semantics are the same as those already covered in t7.
  - GDB/debugger-dependent targets in the checked list (run_gdb/debug/run_qemu_debug/
    run_xlspike_rbb): require interactive GDB, not included; their checked
    semantics are the same as t5/t6/t17.

Usage:
  python tools/scripts/tests/buildsystem/buildtodir_regression.py --platform win
  wsl.exe python3 /mnt/c/<path>/tools/scripts/tests/buildsystem/buildtodir_regression.py --platform linux
  Optional --only N runs only case N. Exit code: 0=all pass, 1=some failed.
"""
import argparse
import os
import shutil
import subprocess
import sys

SDK_ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
APP_DIR = os.path.join(SDK_ROOT, "application", "baremetal", "helloworld")
UPUP_BASE = os.path.join(APP_DIR, "_tmp_upup")

PASS = 0
FAIL = 0
FAILED = []
_last_out = ""


def banner(title):
    print("\n== %s ==" % title)


def get_platform(p):
    if p and p != "auto":
        return p
    return "win" if os.name == "nt" else "linux"


def restore_terminal():
    """Best-effort terminal state restore.

    QEMU with -nographic -serial stdio puts the controlling TTY into raw mode;
    if QEMU is killed on timeout the raw mode is left behind, making the
    terminal appear dead (no echo) until `reset` is run. Redirecting QEMU's
    stdin away from the TTY avoids the problem, and this restores the TTY
    just in case.
    """
    if not sys.stdin.isatty():
        return
    try:
        if os.name == "nt":
            # Reset the console mode via cmd's built-in fallback
            subprocess.run(["cmd", "/c", "stty", "sane"], capture_output=True)
        else:
            subprocess.run(["stty", "sane"], stdin=sys.stdin, capture_output=True)
    except Exception:
        pass


class Platform:
    """Platform difference wrapper: how make is invoked and subprocess env.

    Toolchain (riscv64-unknown-elf-gcc etc.), make, qemu, openocd are all
    resolved from the current PATH; the script hardcodes no absolute tool
    paths. Callers should add required tool directories to PATH before
    running this script; if required tools are missing, check_tools()
    reports a clear error with hints and never tries to guess or auto-install.
    """

    REQUIRED = {
        "gcc": "riscv64-unknown-elf-gcc",
        "gpp": "riscv64-unknown-elf-g++",
        "make": "make",
    }
    OPTIONAL = {
        "qemu": "qemu-system-riscv32",
        "openocd": "openocd",
    }

    def __init__(self, plat):
        self.plat = plat
        self._env = dict(os.environ)

    def _which(self, name):
        if self.plat == "win":
            # Windows: resolve from PATH, likewise no hardcoded paths
            p = shutil.which(name)
            return p
        # Linux/WSL: resolve via sh -c 'command -v' (works without bashenv shells)
        r = subprocess.run(["sh", "-c", "command -v %s" % name],
                           stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        return r.stdout.decode("utf-8", "replace").strip() if r.returncode == 0 else None

    def check_tools(self):
        """Check tools in PATH. Returns (missing required list, missing optional list)."""
        missing_req = []
        missing_opt = []
        for key, name in self.REQUIRED.items():
            if not self._which(name):
                missing_req.append(name)
        for key, name in self.OPTIONAL.items():
            if not self._which(name):
                missing_opt.append(name)
        return missing_req, missing_opt

    def make(self, app_dir, args, timeout=120):
        """Run make in app_dir, returns (returncode, merged output)."""
        global _last_out
        try:
            if self.plat == "win":
                full = ["cmd", "/c", "make"] + args
            else:
                full = ["make"] + args
            p = subprocess.run(
                full, cwd=app_dir, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                text=True, encoding="utf-8", errors="replace",
                env=self._env, timeout=timeout,
            )
            _last_out = p.stdout
            return p.returncode, p.stdout
        except subprocess.TimeoutExpired as e:
            partial = ""
            if e.stdout is not None:
                partial = e.stdout.decode("utf-8", "replace") if isinstance(e.stdout, bytes) else e.stdout
            _last_out = partial
            return 124, partial

    def qemu(self, args, timeout=30):
        """Run QEMU, return merged output. QEMU with -nographic -serial stdio
        keeps running forever, so on timeout its whole process tree must be
        killed, otherwise qemu processes leak (especially visible on Windows).
        Return code is recorded as 124 on timeout.

        NOTE: stdin is detached from the TTY (DEVNULL) so QEMU cannot put the
        terminal into raw mode; otherwise the terminal appears dead (no echo)
        after the test until `reset` is run.
        """
        global _last_out
        cmd = ["qemu-system-riscv32"] + args
        try:
            if self.plat == "win":
                # Windows: CREATE_NEW_PROCESS_GROUP + taskkill /T /F to kill the tree
                flags = subprocess.CREATE_NEW_PROCESS_GROUP
                p = subprocess.Popen(
                    cmd, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                    text=True, encoding="utf-8", errors="replace",
                    env=self._env, creationflags=flags)
                try:
                    out, _ = p.communicate(timeout=timeout)
                    rc = p.returncode
                except subprocess.TimeoutExpired:
                    subprocess.run(["taskkill", "/F", "/T", "/PID", str(p.pid)],
                                   capture_output=True, text=True)
                    try:
                        out, _ = p.communicate(timeout=10)
                    except Exception:
                        out = ""
                    rc = 124
            else:
                # Linux: normal run, kill process group on timeout
                p = subprocess.Popen(
                    cmd, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                    text=True, encoding="utf-8", errors="replace",
                    env=self._env, start_new_session=True)
                try:
                    out, _ = p.communicate(timeout=timeout)
                    rc = p.returncode
                except subprocess.TimeoutExpired:
                    os.killpg(os.getpgid(p.pid), 9)
                    try:
                        out, _ = p.communicate(timeout=10)
                    except Exception:
                        out = ""
                    rc = 124
            _last_out = out
            return rc, out
        except Exception as e:
            _last_out = ""
            return 125, "qemu execution error: %r" % e
        finally:
            restore_terminal()


def check(name, cond):
    """Assertion. cond supports: bool / 'matches:literal' / 'file:PATH' / 'dir:PATH' / 'no:PATH'."""
    global PASS, FAIL
    if isinstance(cond, bool):
        ok = cond
    elif isinstance(cond, str) and cond.startswith("matches:"):
        pat = cond[8:]
        ok = pat in _last_out
    elif isinstance(cond, str) and cond.startswith("file:"):
        ok = os.path.isfile(cond[5:])
    elif isinstance(cond, str) and cond.startswith("dir:"):
        ok = os.path.isdir(cond[4:])
    elif isinstance(cond, str) and cond.startswith("no:"):
        ok = not os.path.exists(cond[3:])
    else:
        ok = bool(cond)
    if ok:
        print("  [PASS] %s" % name)
        PASS += 1
    else:
        print("  [FAIL] %s  (%r)" % (name, cond))
        FAIL += 1
        FAILED.append(name)


def cleanO(name):
    """Clean the directory by relative O name. All O= subdirectories live under APP_DIR."""
    d = os.path.join(APP_DIR, name)
    shutil.rmtree(d, ignore_errors=True)
    return d


def main():
    global PASS, FAIL
    parser = argparse.ArgumentParser()
    parser.add_argument("--platform", default="auto", choices=["auto", "win", "linux"])
    parser.add_argument("--only", type=int, default=None)
    args = parser.parse_args()

    plat = get_platform(args.platform)
    pf = Platform(plat)

    print("===== buildtodir regression test =====")
    print("platform : %s" % plat)
    print("SDK_ROOT : %s" % SDK_ROOT)
    print("APP_DIR  : %s" % APP_DIR)
    print()

    # Pre-flight tool check: exit if required tools are missing, degrade if optional missing
    missing_req, missing_opt = pf.check_tools()
    if missing_req:
        print("[FATAL] missing required tools (add tool directories to PATH and retry):")
        for t in missing_req:
            print("  - %s" % t)
        print("  e.g. WSL: export PATH=/home/riscv/nuclei/gcc/bin:$PATH")
        return 2
    if missing_opt:
        print("[WARN] missing optional tools (related cases will be skipped): %s" % ", ".join(missing_opt))
    global QEMU_OK
    QEMU_OK = "qemu-system-riscv32" not in missing_opt
    OPENOCD_OK = "openocd" not in missing_opt
    print()

    # Clean historical pollution (C_ trees etc. under the source directory)
    for n in os.listdir(APP_DIR):
        if n.startswith("build") or n in ("_obj_sdk", "_obj_app", "C_", "_tmp_upup"):
            p = os.path.join(APP_DIR, n)
            if os.path.isdir(p):
                shutil.rmtree(p, ignore_errors=True)
            else:
                try:
                    os.remove(p)
                except FileNotFoundError:
                    pass
    try:
        os.remove(os.path.join(APP_DIR, ".buildconfig"))
    except FileNotFoundError:
        pass

    tests = []

    # 1. O= output-directory build
    def t1():
        O = cleanO("build1")
        rc, out = pf.make(APP_DIR, ["O=build1", "CORE=n300fd", "all"])
        check("O= build succeeds", "matches:Linking")
        check("elf in O directory", "file:%s" % os.path.join(O, "helloworld.elf"))
        check("map in O directory", "file:%s" % os.path.join(O, "helloworld.map"))
        check(".buildconfig exists", "file:%s" % os.path.join(O, ".buildconfig"))
        check("no artifacts in source directory", "no:%s" % os.path.join(APP_DIR, "helloworld.elf"))
    tests.append((1, "O= output-directory build", t1))

    # 2. Object split
    def t2():
        O = cleanO("build2")
        pf.make(APP_DIR, ["O=build2", "CORE=n300fd", "all"])
        check("application objects into _obj_app", "file:%s" % os.path.join(O, "_obj_app", "main.c.o"))
        check("SDK objects into _obj_sdk", "file:%s" % os.path.join(O, "_obj_sdk", "SoC", "evalsoc", "Common", "Source", "system_evalsoc.c.o"))
    tests.append((2, "object split _obj_sdk/_obj_app", t2))

    # 3. Fingerprint content
    def t3():
        O = cleanO("build3")
        pf.make(APP_DIR, ["O=build3", "CORE=n300fd", "all"])
        fp = os.path.join(O, ".buildconfig")
        content = open(fp, encoding="utf-8", errors="replace").read() if os.path.isfile(fp) else ""
        check("fingerprint contains CORE=n300fd", "CORE=n300fd" in content)
        check("fingerprint contains TOOLCHAIN", "TOOLCHAIN=nuclei_gnu" in content)
        check("fingerprint contains DOWNLOAD=ilm", "DOWNLOAD=ilm" in content)
        for ev in ["SYSCLK", "HARTID_OFS", "CODESIZE", "ECLIC_HWCTX"]:
            check("fingerprint contains evalsoc extended variable %s" % ev, "%s=" % ev in content)
    tests.append((3, "fingerprint .buildconfig content", t3))

    # 4. Multiple configurations coexist
    def t4():
        cleanO("builda"); cleanO("buildb")
        pf.make(APP_DIR, ["O=builda", "CORE=n300fd", "all"])
        pf.make(APP_DIR, ["O=buildb", "CORE=n900fd", "all"])
        check("A artifacts present", "file:%s" % os.path.join(APP_DIR, "builda", "helloworld.elf"))
        check("B artifacts present", "file:%s" % os.path.join(APP_DIR, "buildb", "helloworld.elf"))
    tests.append((4, "multiple configurations coexist (different O= dirs)", t4))

    # 5. Configuration-change detection
    def t5():
        O = cleanO("build5")
        pf.make(APP_DIR, ["O=build5", "CORE=n300fd", "all"])
        rc, out = pf.make(APP_DIR, ["O=build5", "CORE=n900fd", "all"])
        check("change-detection message", "matches:Build configuration changed")
        check("changed variables listed", "matches:CORE: n300fd -> n900fd")
        check("build stopped", "matches:Build stopped")
        check("exit code non-zero", rc != 0)
    tests.append((5, "configuration-change detection", t5))

    # 6. rebuild target
    def t6():
        O = cleanO("build6")
        pf.make(APP_DIR, ["O=build6", "CORE=n300fd", "all"])
        rc, out = pf.make(APP_DIR, ["O=build6", "CORE=n900fd", "all"])
        check("plain make fails after config change", rc != 0)
        check("Build stopped reported after config change", "matches:Build stopped")
        rc2, out2 = pf.make(APP_DIR, ["O=build6", "CORE=n900fd", "rebuild"])
        check("rebuild succeeds (exit=0)", rc2 == 0)
        check("elf present after rebuild", "file:%s" % os.path.join(O, "helloworld.elf"))
    tests.append((6, "rebuild target", t6))

    # 7. Fingerprint-check-exempt targets
    def t7():
        O = cleanO("build7")
        pf.make(APP_DIR, ["O=build7", "CORE=n300fd", "all"])
        for g in ["info", "showflags", "showtoolver", "version", "help"]:
            rc, out = pf.make(APP_DIR, ["O=build7", "CORE=n900fd", g])
            # Core exempt semantics: no "Build stopped" even if the config changed
            exempt_ok = "Build stopped" not in out
            if not exempt_ok:
                check("exempt target %s not blocked by change detection" % g, False)
                continue
            if g in ("showtoolver", "version") and not OPENOCD_OK:
                # Without openocd the target itself fails; that's an environment
                # issue, not buildtodir behavior; only verify exempt semantics
                print("  [SKIP] exempt target %s (openocd missing, rc check skipped)" % g)
                continue
            check("exempt target %s succeeds" % g, rc == 0)
    tests.append((7, "fingerprint-check-exempt targets", t7))

    # 8. clean only cleans the selected O
    def t8():
        cleanO("buildf1"); cleanO("buildf2")
        pf.make(APP_DIR, ["O=buildf1", "CORE=n300fd", "all"])
        pf.make(APP_DIR, ["O=buildf2", "CORE=n900fd", "all"])
        rc, out = pf.make(APP_DIR, ["O=buildf1", "CORE=n300fd", "clean"])
        check("clean F1 succeeds", rc == 0 and "Clean all build objects" in out)
        check("F1 fingerprint removed", "no:%s" % os.path.join(APP_DIR, "buildf1", ".buildconfig"))
        check("F1 _obj_app directory removed", "no:%s" % os.path.join(APP_DIR, "buildf1", "_obj_app"))
        check("F1 _obj_sdk directory removed", "no:%s" % os.path.join(APP_DIR, "buildf1", "_obj_sdk"))
        check("F2 preserved", "file:%s" % os.path.join(APP_DIR, "buildf2", "helloworld.elf"))
    tests.append((8, "clean only cleans the selected O", t8))

    # 9. __up__ external source path conversion
    def t9():
        shutil.rmtree(UPUP_BASE, ignore_errors=True)
        os.makedirs(os.path.join(UPUP_BASE, "shared"))
        os.makedirs(os.path.join(UPUP_BASE, "app"))
        with open(os.path.join(UPUP_BASE, "shared", "helper.c"), "w") as f:
            f.write("int helper(int x) { return x + 1; }\n")
        shutil.copy(os.path.join(APP_DIR, "main.c"), os.path.join(UPUP_BASE, "app", "main.c"))
        with open(os.path.join(UPUP_BASE, "app", "Makefile"), "w") as f:
            f.write("""TARGET = upup_test
NUCLEI_SDK_ROOT = ../../../../../
SRCDIRS = . ../shared
INCDIRS = .
CORE ?= n300fd
include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
""")
        rc, out = pf.make(os.path.join(UPUP_BASE, "app"), ["O=build", "CORE=n300fd", "all"])
        check("__up__ external source app builds", "matches:upup_test.elf")
        check("external source objects into __up__", "file:%s" % os.path.join(UPUP_BASE, "app", "build", "_obj_app", "__up__", "shared", "helper.c.o"))
    tests.append((9, "__up__ external source path conversion", t9))

    # 10. Object collision detection (local __up__/sharedA collides with external ../sharedA)
    def t10():
        shutil.rmtree(UPUP_BASE, ignore_errors=True)
        os.makedirs(os.path.join(UPUP_BASE, "app", "__up__", "sharedA"))
        os.makedirs(os.path.join(UPUP_BASE, "sharedA"))
        with open(os.path.join(UPUP_BASE, "app", "__up__", "sharedA", "helper.c"), "w") as f:
            f.write("int hA(int x){return x+1;}\n")
        with open(os.path.join(UPUP_BASE, "sharedA", "helper.c"), "w") as f:
            f.write("int hA(int x){return x+2;}\n")
        shutil.copy(os.path.join(APP_DIR, "main.c"), os.path.join(UPUP_BASE, "app", "main.c"))
        with open(os.path.join(UPUP_BASE, "app", "Makefile"), "w") as f:
            f.write("""TARGET = upup_test
NUCLEI_SDK_ROOT = ../../../../../
SRCDIRS = __up__/sharedA ../sharedA
INCDIRS = .
CORE ?= n300fd
include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
""")
        rc, out = pf.make(os.path.join(UPUP_BASE, "app"), ["O=build", "CORE=n300fd", "all"])
        check("object path collision detection triggered", "matches:Object path collision detected for:")
    tests.append((10, "object path collision detection", t10))

    # 11. version target
    def t11():
        cleanO("build11")
        rc, out = pf.make(APP_DIR, ["O=build11", "CORE=n300fd", "version"])
        check("version prints SDK version", "matches:Nuclei SDK version")
        check("version prints NMSIS version", "matches:NMSIS version")
        check("version prints git version", "matches:git version")
    tests.append((11, "version target", t11))

    # 12. help update
    def t12():
        cleanO("build12")
        rc, out = pf.make(APP_DIR, ["O=build12", "CORE=n300fd", "help"])
        check("help contains O= description", "matches:Output directory")
        check("help contains rebuild", "matches:rebuild")
    tests.append((12, "help update", t12))

    # 13. build hint
    def t13():
        cleanO("build13")
        rc, out = pf.make(APP_DIR, ["O=build13", "CORE=n300fd", "all"])
        check("hint contains run_qemu", "matches:run_qemu")
        check("hint contains upload", "matches:upload")
        # overrides whose value contains spaces cannot be echoed back
        # safely for every shell (MAKEOVERRIDES escapes their spaces as
        # "\\ ", which Windows cmd cannot parse), so the hint omits them
        # while keeping the space-free ones
        rc, out = pf.make(APP_DIR, ["O=build13", "CORE=n300fd", "BENCH_FLAGS=-O2 -flto", "all"])
        check("hint omits overrides containing spaces", "BENCH_FLAGS" not in out)
        check("hint keeps space-free overrides", "matches:make CORE=n300fd")
    tests.append((13, "build hint (SOC=evalsoc)", t13))

    # 14. hint suppression with SILENT
    def t14():
        cleanO("build14")
        rc, out = pf.make(APP_DIR, ["O=build14", "CORE=n300fd", "SILENT=1", "all"])
        check("SILENT=1 suppresses hint", "run_qemu" not in out)
    tests.append((14, "build hint SILENT=1", t14))

    # 15. QEMU run of O= artifacts
    def t15():
        if not QEMU_OK:
            print("  [SKIP] QEMU run (qemu-system-riscv32 missing, please confirm it supports nuclei_evalsoc)")
            return
        cleanO("build15")
        pf.make(APP_DIR, ["O=build15", "CORE=n300fd", "all"])
        elf = os.path.join(APP_DIR, "build15", "helloworld.elf")
        rc, out = pf.qemu(["-M", "nuclei_evalsoc,download=ilm", "-cpu", "nuclei-n300fd,ext=",
                           "-smp", "1", "-nodefaults", "-nographic", "-serial", "stdio",
                           "-kernel", elf], timeout=30)
        check("QEMU artifact prints Hello World", "matches:Hello World From Nuclei RISC-V Processor")
        if "Hello World From Nuclei RISC-V Processor" not in out:
            print("      (diag) QEMU output tail: %s" % out.strip()[-300:].replace("\n", " | "))
    tests.append((15, "QEMU run of O= artifacts", t15))

    # 16. Default build without O= (regression)
    def t16():
        for n in ["_obj_sdk", "_obj_app"]:
            shutil.rmtree(os.path.join(APP_DIR, n), ignore_errors=True)
        try:
            os.remove(os.path.join(APP_DIR, ".buildconfig"))
        except FileNotFoundError:
            pass
        rc, out = pf.make(APP_DIR, ["CORE=n300fd", "all"])
        check("default build without O=", ("Linking" in out) or ("Build complete" in out))
        check("no-O= fingerprint written to source directory", "file:%s" % os.path.join(APP_DIR, ".buildconfig"))
        fp_content = ""
        try:
            fp_content = open(os.path.join(APP_DIR, ".buildconfig"), encoding="utf-8", errors="replace").read()
        except Exception:
            pass
        check("no-O= fingerprint contains CORE=n300fd", "CORE=n300fd" in fp_content)
        check("objects into _obj_sdk", "dir:%s" % os.path.join(APP_DIR, "_obj_sdk"))
        check("objects into _obj_app", "dir:%s" % os.path.join(APP_DIR, "_obj_app"))
        rc2, out2 = pf.make(APP_DIR, ["CORE=n900fd", "all"])
        check("no-O= change detection stops build", rc2 != 0 and "Build stopped" in out2)
        pf.make(APP_DIR, ["CORE=n900fd", "clean"])
        check("clean removes fingerprint after default build", "no:%s" % os.path.join(APP_DIR, ".buildconfig"))
        check("clean removes _obj_sdk directory", "no:%s" % os.path.join(APP_DIR, "_obj_sdk"))
        check("clean removes _obj_app directory", "no:%s" % os.path.join(APP_DIR, "_obj_app"))
        for n in ["_obj_sdk", "_obj_app"]:
            shutil.rmtree(os.path.join(APP_DIR, n), ignore_errors=True)
        try:
            os.remove(os.path.join(APP_DIR, ".buildconfig"))
        except FileNotFoundError:
            pass
    tests.append((16, "default build without O= (regression)", t16))

    # 17. Symmetric-difference and non-CORE variable change detection
    def t17():
        O = cleanO("build17")
        pf.make(APP_DIR, ["O=build17", "CORE=n300fd", "all"])
        rc, out = pf.make(APP_DIR, ["O=build17", "CORE=n300fd", "ARCH_EXT=_zba_zbb_zbc", "all"])
        check("symmetric-difference detection stops build", rc != 0 and "Build stopped" in out)
        check("lists ARCH_EXT: (empty) -> _zba_zbb_zbc", "matches:ARCH_EXT: (empty) -> _zba_zbb_zbc")
        pf.make(APP_DIR, ["O=build17", "CORE=n300fd", "rebuild"])
        rc, out = pf.make(APP_DIR, ["O=build17", "CORE=n300fd", "DOWNLOAD=flash", "all"])
        check("DOWNLOAD change detection stops build", rc != 0 and "Build stopped" in out)
        check("lists DOWNLOAD: ilm -> flash", "matches:DOWNLOAD: ilm -> flash")
    tests.append((17, "symmetric-difference and non-CORE variable change detection", t17))

    # 18. make -j clean all serialization
    def t18():
        O = cleanO("build18")
        pf.make(APP_DIR, ["O=build18", "CORE=n300fd", "all"])
        rc, out = pf.make(APP_DIR, ["-j4", "O=build18", "CORE=n300fd", "clean", "all"])
        check("-j4 clean all succeeds", rc == 0)
        check("elf present after clean all", "file:%s" % os.path.join(APP_DIR, "build18", "helloworld.elf"))
        fp = os.path.join(APP_DIR, "build18", ".buildconfig")
        content = open(fp, encoding="utf-8", errors="replace").read() if os.path.isfile(fp) else ""
        check("fingerprint updated to latest config after clean all", "CORE=n300fd" in content)
    tests.append((18, "make -j clean all serialization", t18))

    # 19. Absolute Windows-path O= (regression guard)
    def t19():
        repo_parent = os.path.dirname(SDK_ROOT)
        absO = os.path.join(repo_parent, "buildtodir_abs_regress")
        shutil.rmtree(absO, ignore_errors=True)
        shutil.rmtree(os.path.join(APP_DIR, "C_"), ignore_errors=True)
        rc, out = pf.make(APP_DIR, ["O=%s" % absO, "CORE=n300fd", "all"])
        check("absolute O= build succeeds", rc == 0 and "Linking" in out)
        check("absolute O= application objects in OUTDIR/_obj_app", "file:%s" % os.path.join(absO, "_obj_app", "main.c.o"))
        check("absolute O= no C_ tree pollution in source directory", "no:%s" % os.path.join(APP_DIR, "C_"))
        check("absolute O= SDK objects in OUTDIR/_obj_sdk", "file:%s" % os.path.join(absO, "_obj_sdk", "SoC", "evalsoc", "Common", "Source", "system_evalsoc.c.o"))
        shutil.rmtree(absO, ignore_errors=True)
    tests.append((19, "absolute Windows-path O= (regression guard)", t19))

    # 20. REUSE=1 recall of the saved configuration
    def t20():
        O = cleanO("build20")
        pf.make(APP_DIR, ["O=build20", "CORE=n300fd", "DOWNLOAD=flash", "all"])
        # without REUSE, dropping the variables is stopped by the change detection
        rc, out = pf.make(APP_DIR, ["O=build20", "all"])
        check("no REUSE stopped by change detection", rc != 0 and "Build stopped" in out)
        # with REUSE=1 the saved configuration is recalled
        rc, out = pf.make(APP_DIR, ["O=build20", "REUSE=1", "all"])
        check("REUSE builds without re-passing variables", rc == 0)
        check("REUSE recalled CORE", "matches: CORE=n300fd")
        check("REUSE recalled DOWNLOAD", "matches:DOWNLOAD=flash")
        check("REUSE artifact present", "file:%s" % os.path.join(APP_DIR, "build20", "helloworld.elf"))
        # an explicit variable which differs from the saved one is still detected
        rc, out = pf.make(APP_DIR, ["O=build20", "REUSE=1", "CORE=n900fd", "all"])
        check("explicit change still detected with REUSE", rc != 0 and "Build stopped" in out)
        rc, out = pf.make(APP_DIR, ["O=build20", "REUSE=1", "CORE=n900fd", "rebuild"])
        check("REUSE rebuild with new explicit variable", rc == 0)
    tests.append((20, "REUSE=1 recall of the saved configuration", t20))

    # 21. REUSE=1 keeps the saved configuration across clean and an empty
    # output directory does not trigger a false configuration-change stop
    def t21():
        O = cleanO("build21")
        pf.make(APP_DIR, ["O=build21", "CORE=n300fd", "DOWNLOAD=flash", "all"])
        rc, out = pf.make(APP_DIR, ["O=build21", "REUSE=1", "clean"])
        check("REUSE clean succeeds", rc == 0 and "Clean all build objects" in out)
        check("REUSE clean keeps the fingerprint", "file:%s" % os.path.join(O, ".buildconfig"))
        check("REUSE clean removes the objects", "no:%s" % os.path.join(O, "_obj_app"))
        # documented workflow: clean then build without re-passing the variables
        rc, out = pf.make(APP_DIR, ["O=build21", "REUSE=1", "all"])
        check("REUSE build after clean succeeds", rc == 0)
        check("REUSE after clean recalled CORE", "matches: CORE=n300fd")
        check("REUSE after clean recalled DOWNLOAD", "matches:DOWNLOAD=flash")
        check("REUSE after clean not blocked by change detection", "Build stopped" not in out)
        # clean and all in one parallel invocation keeps the recalled configuration
        rc, out = pf.make(APP_DIR, ["-j4", "O=build21", "REUSE=1", "clean", "all"])
        check("-j4 REUSE=1 clean all succeeds", rc == 0 and "Build stopped" not in out)
        fp = os.path.join(O, ".buildconfig")
        content = open(fp, encoding="utf-8", errors="replace").read() if os.path.isfile(fp) else ""
        check("fingerprint keeps the recalled configuration after -j4 clean all",
              "CORE=n300fd" in content and "DOWNLOAD=flash" in content)
        # a plain clean still removes the fingerprint
        rc, out = pf.make(APP_DIR, ["O=build21", "clean"])
        check("plain clean still removes the fingerprint", "no:%s" % os.path.join(O, ".buildconfig"))
        # REUSE=1 without a saved configuration warns instead of silently defaulting
        rc, out = pf.make(APP_DIR, ["O=build21", "REUSE=1", "info"])
        check("REUSE=1 without saved configuration warns",
              "no saved build configuration" in out)
        # an empty output directory does not conflict with a new configuration
        pf.make(APP_DIR, ["O=build21", "CORE=n300fd", "DOWNLOAD=flash", "all"])
        pf.make(APP_DIR, ["O=build21", "REUSE=1", "clean"])
        rc, out = pf.make(APP_DIR, ["O=build21", "CORE=n900fd", "all"])
        check("no false change detection in an empty output directory",
              rc == 0 and "Build stopped" not in out)
        fp = os.path.join(O, ".buildconfig")
        content = open(fp, encoding="utf-8", errors="replace").read() if os.path.isfile(fp) else ""
        check("fingerprint refreshed to the new configuration after clean", "CORE=n900fd" in content)
        # with the objects back, the change detection works again
        rc, out = pf.make(APP_DIR, ["O=build21", "CORE=n300fd", "all"])
        check("change detection works again with objects present", rc != 0 and "Build stopped" in out)
    tests.append((21, "REUSE=1 clean keeps the configuration", t21))

    for num, name, fn in tests:
        if args.only is not None and num != args.only:
            continue
        banner("%d. %s" % (num, name))
        try:
            fn()
        except Exception as e:
            print("  [FAIL] case raised exception: %r" % e)
            FAIL += 1
            FAILED.append(name)

    # Clean leftovers
    shutil.rmtree(UPUP_BASE, ignore_errors=True)
    for n in os.listdir(APP_DIR):
        if n.startswith("build"):
            shutil.rmtree(os.path.join(APP_DIR, n), ignore_errors=True)
    for n in ["_obj_sdk", "_obj_app", "C_"]:
        shutil.rmtree(os.path.join(APP_DIR, n), ignore_errors=True)
    for n in ["helloworld.elf", "helloworld.map", ".buildconfig"]:
        try:
            os.remove(os.path.join(APP_DIR, n))
        except FileNotFoundError:
            pass

    print("\n" + "===== summary =====")
    print("PASS: %d" % PASS)
    print("FAIL: %d" % FAIL)
    if FAILED:
        print("failed cases:")
        for n in FAILED:
            print("  - %s" % n)
    return 1 if FAIL else 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    finally:
        restore_terminal()
