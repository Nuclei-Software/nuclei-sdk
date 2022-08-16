#!/bin/env python3

import os
import sys
import time
import tempfile
import random
import argparse

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

SDK_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../../"))

def run_nsdk_bench(appcfg, hwcfg, logdir, makeopts, runtarget):
    nsdk_benchpy = os.path.join(SDK_ROOT, "tools/scripts/nsdk_cli/nsdk_bench.py")
    benchcmd = "%s %s --appcfg %s --hwcfg %s --logdir %s --make_options \"%s\" --run_target %s --run" \
            % (sys.executable, nsdk_benchpy, appcfg, hwcfg, logdir, makeopts, runtarget)
    print("Run command: %s" % (benchcmd))
    sys.stdout.flush()
    ret = os.system(benchcmd)
    sys.stdout.flush()
    if ret != 0:
        return False
    return True

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Sanity Random Checker")
    parser.add_argument('--appcfg', required=True, help="Application JSON Configuration File")
    parser.add_argument('--hwcfg', help="Hardware Target JSON Configuration File, if specified, will overwrite configuration defined in appcfg")
    parser.add_argument('--logdir', required=True, help="logs directory where saved the report json files")
    parser.add_argument('--socs', default="evalsoc,demosoc", help="SOC choices")
    parser.add_argument('--downloads', default="ilm,flash,flashxip,ddr", help="DOWNLOAD choices")
    parser.add_argument('--stdclibs', default="newlib_small,newlib_full,libncrt_small,libncrt_balanced,libncrt_fast", help="STDCLIB choices")
    parser.add_argument('--run_target', default="qemu", help="Run target which program will run, such as hardware, qemu or xlspike")
    parser.add_argument('--randtimes', default=1, help="Random run times")

    args = parser.parse_args()

    soc_choices = args.socs.split(",")
    download_choices = args.downloads.split(",")
    stdclib_choices = args.stdclibs.split(",")

    for i in range(int(args.randtimes)):
        soc = random.choice(soc_choices)
        download = random.choice(download_choices)
        stdclib = random.choice(stdclib_choices)
        # when use newlib full, some case might link fail due size issue, so change to other download mode
        if stdclib == "newlib_full" and download in ("ilm", "flash"):
            download = random.choice(["ddr", "flashxip"])

        makeopts = "SOC=%s DOWNLOAD=%s STDCLIB=%s" % (soc, download, stdclib)
        logdir = args.logdir + "/%s/%s/%s" % (soc, download, stdclib)
        print("Random select choice: %s" % (makeopts))
        sys.stdout.flush()
        appcfg = args.appcfg
        hwcfg = args.hwcfg
        if "libncrt" in stdclib:
            hwcfg = hwcfg + ".libncrt"
        ret = run_nsdk_bench(appcfg, hwcfg, logdir, makeopts, args.run_target)
        if ret == False:
            print("Choice %s Failed!" % (makeopts))
            break
        else:
            print("Choice %s Passed!" % (makeopts))
    # Sanity check exit
    if ret == False:
        sys.exit(1)
    sys.exit(0)
