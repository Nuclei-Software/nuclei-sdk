#!/bin/env python3

import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))
try:
    import json
    import argparse
    import pprint
    import glob
    import yaml
    import usb
    import yamale
except:
    print("Please install requried packages using: pip3 install -r %s" % (requirement_file))
    sys.exit(1)

from nsdk_utils import *
from nsdk_report import *
from nsdk_bench import nsdk_bench

RUNNER_LIST = ["fpga", "ncycm"]

def yaml_validate(sf, yf):
    try:
        schema = yamale.make_schema(sf)
        data = yamale.make_data(yf)
        yamale.validate(schema, data)
        return True
    except:
        return False

def check_usb_serial(serno):
    busses = usb.busses()
    for bus in busses:
        devices = bus.devices
        try:
            for dev in devices:
                serialnum = usb.util.get_string(dev.dev, dev.iSerialNumber)
                if serialnum == serno:
                    return True
        except:
            continue
    return False

class nsdk_runner(object):
    def __init__(self, sdk, makeopts, runyaml, locations):
        if os.path.isdir(sdk) == False:
            print("Invalid sdk path %s" % (sdk))
            sys.exit(1)
        runner_schema = os.path.join(os.path.dirname(os.path.realpath(__file__)), "runner_schema.yaml")
        if yaml_validate(runner_schema, runyaml) == False:
            print("Invalid runner yaml file %s" % (runyaml))
            sys.exit(1)

        yret, self.runcfg = load_yaml(runyaml)
        self.sdk = sdk
        self.makeopts = makeopts

        if yret != YAML_OK:
            print("Invalid yaml configuration file")
            sys.exit(1)

        if locations["fpgaloc"]:
            self.runcfg["environment"]["fpgaloc"] = locations["fpgaloc"]
        if locations["ncycmloc"]:
            self.runcfg["environment"]["ncycmloc"] = locations["ncycmloc"]
        if locations["cfgloc"]:
            self.runcfg["environment"]["cfgloc"] = locations["cfgloc"]
        self.cpuruncfgs = dict()
        for key in self.runcfg["configs"]:
            self.cpuruncfgs[key] = copy.deepcopy(self.get_runcfg(key))
        pass

    def get_runcfg(self, config):
        if config not in self.runcfg.get("configs", dict()):
            print("ERROR:  %s not found in runner yaml config" % (config))
            return None
        cur_runcfg = self.runcfg["configs"][config]
        fpga = cur_runcfg.get("fpga", None)
        bitstream = cur_runcfg.get("bitstream", None)
        openocdcfg = cur_runcfg.get("openocd_cfg", None)
        appcfg = cur_runcfg.get("appcfg", None)
        hwcfg = cur_runcfg.get("hwcfg", None)
        ncycm = cur_runcfg.get("ncycm", None)

        fpgas2run = dict()
        for runner in self.runcfg["fpga_runners"]:
            if self.runcfg["fpga_runners"][runner]["board_type"] == fpga:
                fpgas2run[runner] = self.runcfg["fpga_runners"][runner]
                fpgas2run[runner]["bitstream"] = bitstream
                fpgas2run[runner]["openocd_cfg"] = openocdcfg
        ncycm2run = dict()
        for runner in self.runcfg["ncycm_runners"]:
            if runner == ncycm:
                ncycm2run = self.runcfg["ncycm_runners"][runner]
                break

        benchcfg = {"appcfg": appcfg, "hwcfg": hwcfg}

        return {"benchcfg": benchcfg, "fpga": fpgas2run, "ncycm": ncycm2run}
        pass

    def get_configs(self):
        return list(self.cpuruncfgs.keys())

    def run_config(self, config, logdir, runon=""):
        runcfg = self.cpuruncfgs.get(config, None)
        if runcfg is None:
            return False
        if runon not in RUNNER_LIST:
            runon = "NOTRUN"
            print("No need to run on any target")
        benchcfg = runcfg["benchcfg"]
        appcfg_file = benchcfg["appcfg"]
        hwcfg_file = benchcfg["hwcfg"]
        cfgloc = self.runcfg["environment"]["cfgloc"]
        def get_file_path(file, loc):
            if os.path.isfile(file) == False:
                file = os.path.join(loc, file)
            return file
        appcfg_file = get_file_path(appcfg_file, cfgloc)
        hwcfg_file = get_file_path(hwcfg_file, cfgloc)

        ret, run_appcfg = load_json(appcfg_file)
        if ret != JSON_OK:
            print("ERROR: appcfg json file is not a valid configuration file!")
        ret, run_hwcfg = load_json(hwcfg_file)

        if ret != JSON_OK:
            print("ERROR: hwcfg json file is not a valid configuration file!")
        final_appcfg = merge_two_config(run_appcfg, run_hwcfg)
        need2run = True
        # check fpga/ncycm runner
        if runon == "fpga":
            if len(runcfg["fpga"]) == 0:
                print("ERROR: No fpga board available for this cpu")
                return False

            fpgaready = False
            ftdi_serial = serport = ""
            for fpga in runcfg["fpga"]:
                # check fpga and bitstream, serial port, ftdi_serial
                ftdi_serial = runcfg["fpga"][fpga]["ftdi_serial"]
                fpga_serial = runcfg["fpga"][fpga]["fpga_serial"]
                bitstream = runcfg["fpga"][fpga]["bitstream"]
                fpgaloc = self.runcfg["environment"]["fpgaloc"]
                serport = runcfg["fpga"][fpga]["serial_port"]
                openocdcfg = os.path.join(self.sdk, runcfg["fpga"][fpga]["openocd_cfg"])
                if (os.path.isfile(openocdcfg)) == False:
                    print("OpenOCD Configuration File %s not found" % (openocdcfg))
                    continue
                if os.path.isfile(bitstream) == False:
                    bitstream = os.path.join(fpgaloc, bitstream)
                    if os.path.isfile(bitstream) == False:
                        print("Bitstream %s not found!" % (bitstream))
                        continue
                if check_usb_serial(ftdi_serial) == False:
                    print("FDTI Serial %s not found!" % (ftdi_serial))
                    continue
                if check_serial_port(serport) == False:
                    print("Serial port %s not found!" % (serport))
                    continue
                # program fpga
                if program_fpga(bitstream, fpga_serial) == False:
                    print("Failed to program fpga using bit %s to target %s" % (bitstream, fpga_serial))
                    continue
                else:
                    print("Successfully program fpga using bit %s to target %s" % (bitstream, fpga_serial))
                    fpgaready = True
                    break
            if fpgaready == False:
                print("FPGA is not ready for running, please check!")
                return False
            # Modify openocd config
            if modify_openocd_cfg(openocdcfg, ftdi_serial) == False:
                print("Not a valid openocd configuration file %s" % (openocdcfg))
                return False
            # set run target to hardware
            final_appcfg["run_config"]["target"] = "hardware"
            if "hardware" not in final_appcfg["run_config"]:
                final_appcfg["run_config"]["hardware"] = {"baudrate": 115200, "timeout": 60, "serport": ""}
            final_appcfg["run_config"]["hardware"]["serport"] = serport
        elif runon == "ncycm":
            # check ncycm
            if len(runcfg["ncycm"]) == 0:
                print("ERROR: No cycle model available for this cpu")
                return False
            ncycm_path = runcfg["ncycm"]["model"]
            ncycm_loc = self.runcfg["environment"]["ncycmloc"]
            if os.path.isfile(ncycm_path) == False:
                ncycm_path = os.path.join(ncycm_loc, ncycm_path)
                if os.path.isfile(ncycm_path) == False:
                    print("cycle model %s not found!" % (ncycm_path))
                    return False
            # set run target to ncycm
            final_appcfg["run_config"]["target"] = "ncycm"
            if "ncycm" not in final_appcfg["run_config"]:
                final_appcfg["run_config"]["ncycm"] = {"timeout": 600, "ncycm": "ncycm"}
            final_appcfg["run_config"]["ncycm"]["ncycm"] = ncycm_path
        else:
            # don't need to run
            need2run = False
        # run on fpga/ncycm
        nsdk_ext = nsdk_bench()
        ret = True

        if self.makeopts is None:
            mkopts = ""
        else:
            mkopts = self.makeopts

        if runon == "ncycm":
            for opts in ("SIMULATION=1", "SIMU=xlspike"):
                if opts not in mkopts:
                    mkopts = "%s %s" % (mkopts, opts)
        subappcfg = final_appcfg
        if mkopts != "":
            subappcfg = merge_config_with_makeopts(subappcfg, mkopts)
        sublogdir = os.path.join(logdir, config)
        start_time = time.time()
        if need2run:
            cmdsts, result = nsdk_ext.run_apps(subappcfg, False, sublogdir, False)
        else:
            cmdsts, result = nsdk_ext.build_apps(subappcfg, False, sublogdir, False)

        runtime = round(time.time() - start_time, 2)
        print("Build or Run application for config %s run status: %s, time cost %s seconds" % (config, cmdsts, runtime))
        locret = check_expected(result, subappcfg, need2run)
        print("Application build as expected: %s" % (locret))
        if locret == False:
            ret = False
        save_results(subappcfg, None, subappcfg, result, sublogdir)
        if result:
            # Generate build or run report
            save_report_files(sublogdir, subappcfg, result, need2run)

        return ret
        pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Benchmark and Report Tool")
    parser.add_argument('--appyaml', required=True, help="Application YAML Configuration File")
    parser.add_argument('--logdir', default='logs', help="logs directory, default logs")
    parser.add_argument('--fpgaloc', help="Where fpga bitstream located in")
    parser.add_argument('--ncycmloc', help="Where nuclei cycle model located in")
    parser.add_argument('--cfgloc', help="Where nsdk bench configurations located in")
    parser.add_argument('--sdk', help="Where SDK located in")
    parser.add_argument('--make_options', help="Extra make options passed to overwrite default build configuration passed via appcfg and hwcfg")
    parser.add_argument('--config', help="run configuration")
    parser.add_argument('--runon', default='hardware', help="Where to run these application")
    parser.add_argument('--show', action='store_true', help="Show configurations")
    args = parser.parse_args()

    if args.sdk is None:
        args.sdk =  os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "../../../"))
    if (os.path.isfile(args.appyaml) == False):
        print("Invalid application yaml, please check!")
        sys.exit(1)
    pp = pprint.PrettyPrinter(compact=True)
    ret = True
    locations = {"fpgaloc": args.fpgaloc, "ncycmloc": args.ncycmloc, "cfgloc": args.cfgloc }
    nsdk_ext = nsdk_runner(args.sdk, args.make_options, args.appyaml, locations)
    if args.show:
        print("Here are the supported configs:")
        pp.pprint(nsdk_ext.get_configs())
    else:
        start_time = time.time()
        if args.config:
            sel_configs = list(set([ key.strip() for key in args.config.split(',')]))
            print("Run selected configurations: %s" % (sel_configs))
        else:
            sel_configs = nsdk_ext.get_configs()
            print("Run all the configurations as below:")
            pp.pprint(sel_configs)
        runcnt = 0
        for config in sel_configs:
            if config in nsdk_ext.get_configs():
                runcnt += 1
                print("Run for configuration %s now" % (config))
                if nsdk_ext.run_config(config, args.logdir, runon=args.runon) == False:
                    print("Configuration %s failed" % (config))
                    ret = False
        if runcnt > 1:
            # generate total results for all the configs
            print("Generate all the reports for this run")
            need2run = False
            if args.runon in RUNNER_LIST:
                need2run = True
            generate_report_for_logs(args.logdir, True, need2run)
        runtime = round(time.time() - start_time, 2)
        print("Cost about %s seconds to do this running!" % (runtime))
    # Exit with ret value
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)
