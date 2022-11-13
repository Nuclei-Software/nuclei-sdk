#!/bin/env python3

import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))
try:
    import json
    import argparse
    import shutil
    import time

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
        runcfgdict = gen_runcfg(caseconfig.get("cpucfg"), runcfg)
    else:
        runcfgdict = gen_coreruncfg(core, runcfg, choice)

    save_json(os.path.join(genloc, "hw.json"), runcfgdict)
    shutil.copy(appcfg, os.path.join(genloc, "app.json"))

    locs = caseconfig.get("locations", dict())
    locs["cfgloc"] = "."
    fpga_serial = caseconfig.get("fpga_serial", "xxxxxx")
    ftdi_serial = caseconfig.get("ftdi_serial", "xxxxxx")
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
    ocdcfg = caseconfig.get("ocdcfg", "SoC/demosoc/Board/nuclei_fpga_eval/openocd_demosoc.cfg")
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

    print("Using sdk path in %s" % (args.sdk))

    ret, caseconfig = load_json(args.casecfg)
    if ret != JSON_OK:
        print("Invalid case configuration file %s, please check!" % (args.casecfg))
        sys.exit(1)
    passed_cases = []
    torun_cases = args.cases.split(",")
    tot_cases = []
    start_time = time.time()
    ret = True
    try:
        for case in torun_cases:
            case = case.strip()
            if case == "":
                continue
            casedir = os.path.join(args.caseroot, case)
            if os.path.isdir(casedir) == False:
                print("Can't find case %s, ignore it!" % (casedir))
                continue
            tot_cases.append(case)
            caselogdir = os.path.join(args.logdir, case)
            casecfgdir = os.path.join(caselogdir, "gencfgs")
            if gen_runner_configs(casedir, caseconfig, casecfgdir) == False:
                print("No correct case configurations found in %s" % (casedir))
                ret = False
                break
            runneryaml = os.path.join(casecfgdir, "core.yaml")
            shutil.copy(args.casecfg, os.path.join(casecfgdir, "casecfg.json"))
            locations = dict()
            nsdk_ext = nsdk_runner(args.sdk, args.make_options, runneryaml, locations, args.verbose, args.timeout)
            casepassed = True
            for config in nsdk_ext.get_configs():
                print("Run case %s for configuration %s specified in yaml %s" % (case, config, runneryaml))
                if nsdk_ext.run_config(config, caselogdir, runon=args.runon, createsubdir=False) == False:
                    print("Configuration %s failed" % (config))
                    ret = False
                if ret == False:
                    print("Case %s for configuration %s specified in yaml %s: FAILED" % (case, config, runneryaml))
                    casepassed = False
                    break
                else:
                    print("Case %s for configuration %s specified in yaml %s: PASSED" % (case, config, runneryaml))

            if casepassed == False:
                ret = False
                break
            passed_cases.append(case)
    except Exception as exc:
        print("Unexpected Error: %s" % (exc))
        ret = False

    runtime = round(time.time() - start_time, 2)
    print("Cost about %s seconds to do this running!" % (runtime))
    print("Case %s passed out of %s" % (passed_cases, tot_cases))

    # Exit with ret value
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)

