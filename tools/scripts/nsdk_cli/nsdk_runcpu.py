#!/bin/env python3

import os
import sys
import traceback

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))
try:
    import json
    import argparse
    import shutil
    import time
    import datetime

except:
    print("Please install requried packages using: pip3 install -r %s" % (requirement_file))
    sys.exit(1)

from nsdk_utils import *
from nsdk_report import *
from nsdk_runner import nsdk_runner

FPGACIROOT = os.path.join(SCRIPT_DIR, "configs", "fpgaci")

# caseconfig is a dict
# {
#    "core": "n300",
#    "locations": {
#            "fpgaloc": "",
#            "ncycmloc": "",
#            "cfgloc": ""
#            },
#    "ncycm": "xxxx",
#    "bitstream": "fpga.bit",
#    "fpga_serial": "xxxx",
#    "ftdi_serial": "xxxx",
#    "boardtype": "ddr200t",
#    "ocdcfg": "SoC/demosoc/Board/nuclei_fpga_eval/openocd_demosoc.cfg",
#    "cpucfg": "n300.json"
# }

def gen_runner_configs(casedir, caseconfig, genloc):
    if os.path.isdir(casedir) == False:
        return False
    if "core" not in caseconfig:
        print("No core is specified, please check!")
        return False
    core = caseconfig["core"]
    ocdcfg = caseconfig.get("ocdcfg", "SoC/demosoc/Board/nuclei_fpga_eval/openocd_demosoc.cfg")
    defbldcfg = dict()
    try:
        pathkeys = ocdcfg.replace("\\","/").split("/")
        defbldcfg = {"SOC": pathkeys[1], "BOARD": pathkeys[3]}
    except:
        defbldcfg = {"SOC": "demosoc"}
    # get build_config from caseconfig
    glbldcfg = caseconfig.get("build_config", defbldcfg)
    runcfg = os.path.join(casedir, "%s.json" % (core))
    appcfg = os.path.join(casedir, "app.json")
    cfgcfg = os.path.join(casedir, "config.json")
    _, cfgconfig = load_json(cfgcfg)
    if cfgconfig is None:
        cfgconfig = {"choice": "mini"}

    choice = cfgconfig.get("choice", "mini")
    if os.path.isdir(genloc) == False:
        os.makedirs(genloc)

    # if specified user own cpu config use it
    if caseconfig.get("cpucfg", None):
        runcfgdict = gen_runcfg(caseconfig.get("cpucfg"), runcfg, glbldcfg)
    else:
        runcfgdict = gen_coreruncfg(core, runcfg, choice, glbldcfg)

    caseconfig["gencfgtimestamp"] = str(datetime.datetime.now())
    # save casecfg.json/app.json/hw.json
    save_json(os.path.join(genloc, "casecfg.json"), caseconfig)
    save_json(os.path.join(genloc, "hw.json"), runcfgdict)
    shutil.copy(appcfg, os.path.join(genloc, "app.json"))

    locs = caseconfig.get("locations", dict())
    locs["cfgloc"] = "."
    fpga_serial = caseconfig.get("fpga_serial", INVAILD_SERNO)
    ftdi_serial = caseconfig.get("ftdi_serial", INVAILD_SERNO)
    cycm = caseconfig.get("ncycm", None)
    fpgabit = caseconfig.get("bitstream", "fpga.bit")
    if core in ["n200", "n300"]:
        boardtype = "ddr200t"
    elif core in ["n600", "ux600"]:
        boardtype = "ku060"
    else:
        boardtype = "vcu118"
    for btype in ["mcu200t", "ddr200t", "ku060", "vcu118"]:
        if btype in fpgabit:
            boardtype = btype
            break
    boardtype = caseconfig.get("boardtype", boardtype)
    appcfg = "app.json"
    hwcfg = "hw.json"

    # generate required runner yaml
    runyamldict = gen_runyaml(core, locs, fpga_serial, ftdi_serial, cycm, fpgabit, boardtype, ocdcfg, appcfg, hwcfg)
    save_yaml(os.path.join(genloc, "core.yaml"), runyamldict)

    return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK CPU Runner")
    parser.add_argument('--cases', required=True, help="Case names, splitted by comma, such as barebench,clibbench")
    parser.add_argument('--casecfg', required=True, help="Case configuration for cycle model or fpga")
    parser.add_argument('--caseroot', default=FPGACIROOT, help="Case configuration root")
    parser.add_argument('--logdir', default='logs', help="logs directory, default logs")
    parser.add_argument('--sdk', help="Where SDK located in")
    parser.add_argument('--make_options', help="Extra make options passed to overwrite default build configuration passed via appcfg and hwcfg")
    parser.add_argument('--runon', default='qemu', help="Where to run these application")
    parser.add_argument('--timeout', help="Runner timeout for each application run, if not specified will use default one specified in json configuration")
    parser.add_argument('--verbose', action='store_true', help="If specified, will show detailed build/run messsage")
    args = parser.parse_args()

    if args.sdk is None:
        sdk_path = os.environ.get("NUCLEI_SDK_ROOT")
        if sdk_path is None or os.path.isdir(sdk_path) == False:
            args.sdk = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "../../../"))
        else:
            args.sdk = sdk_path
    if not(args.sdk and os.path.isdir(args.sdk) and os.path.isfile(os.path.join(args.sdk, "npk.yml"))):
        print("SDK location %s is invalid, please check!" % (args.sdk))
        sys.exit(1)

    STOPONFAIL = get_env_flag("STOPONFAIL", True)
    print("Using sdk path in %s" % (args.sdk))

    ret, caseconfig = load_json(args.casecfg)
    if ret != JSON_OK:
        print("Invalid case configuration file %s, please check!" % (args.casecfg))
        sys.exit(1)
    passed_cases = []
    failed_cases = []
    torun_cases = list(set(args.cases.split(",")))
    tot_cases = []
    start_time = time.time()
    ret = True
    try:
        print("Prepare to do cases %s on %s, stop on fail %s" % (torun_cases, args.runon, STOPONFAIL))
        index = 0
        totlen = len(torun_cases)
        for case in torun_cases:
            case = case.strip()
            index += 1
            print("Run baremetal cpu test case %s now, progress %d/%d!" % (case, index, totlen))
            if case == "":
                print("Case %s is invaild, ignore it!" % (case))
                continue
            casedir = os.path.join(args.caseroot, case)
            if os.path.isdir(casedir) == False:
                print("Can't find case %s, ignore it!" % (casedir))
                continue
            tot_cases.append(case)
            caselogdir = os.path.join(args.logdir, case)
            casecfgdir = os.path.join(caselogdir, "gencfgs")
            caseconfig["execase"] = case
            if gen_runner_configs(casedir, caseconfig, casecfgdir) == False:
                print("No correct case configurations found in %s" % (casedir))
                ret = False
                # if stop on fail, it will break current execution and exit
                if STOPONFAIL:
                    print("Stop early due to case %s failed" % (case))
                    failed_cases.append(case)
                    break
                else:
                    print("Continue execution due to not stop on failed case")
                    continue
            runneryaml = os.path.join(casecfgdir, "core.yaml")
            locations = dict()
            nsdk_ext = nsdk_runner(args.sdk, args.make_options, runneryaml, locations, args.verbose, args.timeout)
            casepassed = True
            for config in nsdk_ext.get_configs():
                print("Run case %s for configuration %s specified in yaml %s" % (case, config, runneryaml))
                if nsdk_ext.run_config(config, caselogdir, runon=args.runon, createsubdir=False) == False:
                    print("Configuration %s failed" % (config))
                    print("Case %s for configuration %s specified in yaml %s: FAILED" % (case, config, runneryaml))
                    failed_cases.append(case)
                    casepassed = False
                else:
                    print("Case %s for configuration %s specified in yaml %s: PASSED" % (case, config, runneryaml))
                    passed_cases.append(case)
                # only one configuration should be executed, so just break here
                break

            if casepassed == False:
                ret = False
                # if stop on fail, it will break current execution and exit
                if STOPONFAIL:
                    print("Stop early due to case %s failed" % (case))
                    break
                else:
                    print("Continue execution due to not stop on failed case")
    except Exception as exc:
        print("Unexpected Error: %s during execute case %s" % (exc, case))
        if case not in failed_cases:
            failed_cases.append(case)
        # https://stackoverflow.com/questions/3702675/how-to-catch-and-print-the-full-exception-traceback-without-halting-exiting-the
        traceback.print_exc()
        ret = False

    runtime = round(time.time() - start_time, 2)
    print("Cost about %s seconds to do this running, passed %s!" % (runtime, ret))
    print("All the required cases are %s" % (torun_cases))
    print("Case %s passed out of executed %s" % (passed_cases, tot_cases))
    print("These following cases failed: %s" % (failed_cases))
    # At least passed or failed cases are not zero
    if (len(passed_cases) > 0 or len(failed_cases) > 0) and os.path.isdir(args.logdir):
        runcpustatustxt = os.path.join(args.logdir, "runcpustatus.txt")
        print("Save run cpu report status into %s" % (runcpustatustxt))
        with open(runcpustatustxt, "w") as rf:
            rf.write("PASSED:%s\n" % (",".join(passed_cases)))
            rf.write("FAILED:%s\n" % (",".join(failed_cases)))

    sys.stdout.flush()
    # Exit with ret value
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)

