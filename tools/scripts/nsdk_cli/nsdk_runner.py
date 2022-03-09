#!/bin/env python3

import os
import json
import pyexcel as pe
import argparse
import pprint
import glob
import yaml
import yamale
from nsdk_utils import *
from nsdk_report import *
from nsdk_bench import nsdk_bench
import usb

RUNNER_LIST = ["fpga", "ncycm"]

def yaml_validate(sf, yf):
    try:
        schema = yamale.make_schema(sf)
        data = yamale.make_data(df)
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
    def __init__(self, sdk, appjson, runyaml):
        if os.path.isdir(sdk) == False:
            print("Invalid sdk path %s" % (sdk))
            sys.exit(1)
        runner_schema = os.path.join(os.path.dirname(os.path.realpath(__file__)), "runner_schema.yaml")
        if yaml_validate(runner_schema, runyaml):
            print("Invalid runner yaml file %s" % (runyaml))
            sys.exit(1)

        jret, self.appcfg = load_json(appjson)
        yret, self.runcfg = load_yaml(runyaml)
        self.sdk = sdk

        if jret != JSON_OK or yret != YAML_OK:
            print("Invalid json file or yaml file")
            sys.exit(1)

        self.cpuruncfgs = dict()
        for key in self.runcfg["configs"]:
            self.cpuruncfgs[key] = self.get_runcfg(key)
        pass

    def get_runcfg(self, config):
        if config not in self.runcfg.get("configs", dict()):
            print("ERROR:  %s not found in runner yaml config" % (config))
            return None
        cur_runcfg = self.runcfg["configs"][config]
        fpga = cur_runcfg.get("fpga", None)
        bitstream = cur_runcfg.get("bitstream", None)
        openocdcfg = cur_runcfg.get("openocd_cfg", None)
        ncycm = cur_runcfg.get("ncycm", None)
        cpu = cur_runcfg.get("cpu", None)

        fpgas2run = dict()
        for runner in self.runcfg["fpga_runners"]:
            if self.runcfg["fpga_runners"][runner]["board_type"] == fpga:
                fpgas2run[runner] = self.runcfg["fpga_runners"][runner]
                fpgas2run[runner]["bitstream"] = bitstream
                fpgas2run[runner]["openocd_cfg"] = openocdcfg
        ncycm2run = dict()
        for runner in self.runcfg["ncycm_runners"]:
            if runner == ncycm:
                ncycm2run[runner] = self.runcfg["ncycm_runners"][runner]
        cpu2run = dict()
        for selcpu in self.runcfg["cpus"]:
            if cpu == selcpu:
                cpu2run = self.runcfg["cpus"][cpu]
        return {"cpu": cpu2run, "fpga": fpgas2run, "ncycm": ncycm2run}
        pass

    def get_configs(self):
        configs = dict()
        for key in self.cpuruncfgs:
            configs[key] = list(self.cpuruncfgs[key]["cpu"]["nsdk_makeopts"].keys())
        return configs

    def run_config(self, config, logdir, subcfgs="", runon=""):
        runcfg = self.cpuruncfgs.get(config, None)
        if runcfg is None:
            return False
        if runon not in RUNNER_LIST:
            runon = self.runcfg["runcfg"]["runner"]
            print("Use default runner %s" % (runon))
        cpu2run = runcfg["cpu"]
        if len(cpu2run) == 0:
            print("ERROR:  Found no cpu for config %s" % (config))
            return False
        selcpucfgs = self.get_configs()[config]
        if subcfgs != "":
            selcpucfgs = list(set(selcpucfgs).union(set(subcfgs.split(','))))
        if len(selcpucfgs) == 0:
            print("ERROR:  no nsdk make options selected for config %s" % (config))
            return False
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
            self.appcfg["run_config"]["target"] = "hardware"
            if "hardware" not in self.appcfg["run_config"]:
                self.appcfg["run_config"]["hardware"] = {"baudrate": 115200, "timeout": 60, "serport": ""}
            self.appcfg["run_config"]["hardware"]["serport"] = serport
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
            self.appcfg["run_config"]["target"] = "ncycm"
            if "ncycm" not in self.appcfg["run_config"]:
                self.appcfg["run_config"]["ncycm"] = {"timeout": 240, "ncycm": "ncycm"}
            self.appcfg["run_config"]["ncycm"]["ncycm"] = ncycm_path
        # run on fpga/ncycm
        nsdk_ext = nsdk_bench()
        for subcfg in selcpucfgs:
            subappcfg = copy.deepcopy(self.appcfg)
            subappcfg = merge_config_with_makeopts(subappcfg, runcfg["cpu"]["nsdk_makeopts"][subcfg])
            sublogdir = os.path.join(logdir, config, subcfg)
            start_time = time.time()
            cmdsts, result = nsdk_ext.run_apps(subappcfg, False, sublogdir, False)
            runtime = round(time.time() - start_time, 2)
            print("Run application for config %s run status: %s, time cost %s seconds" % (subcfg, cmdsts, runtime))
            ret = check_expected(result, subappcfg, True)
            print("Application build as expected: %s" % (ret))
            save_results(subappcfg, None, subappcfg, result, sublogdir)
            if result:
                # Generate build or run report
                save_report_files(sublogdir, subappcfg, result, True)
        return True
        pass

#runner = nsdk_runner(".", "nmsis_hw.json", "nmsis_hw.yaml")

#runner.run_config(1,2)
#print(runner.get_configs())

#modify_openocd_cfg('SoC/demosoc/Board/nuclei_fpga_eval/openocd_demosoc.cfg', 'FT6JGAXS')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Benchmark and Report Tool")
    parser.add_argument('--appcfg', required=True, help="Application JSON Configuration File")
    parser.add_argument('--appyaml', required=True, help="Application YAML Configuration File")
    parser.add_argument('--logdir', default='logs', help="logs directory, default logs")
    parser.add_argument('--sdk', help="Where SDK located in")
    parser.add_argument('--config', help="run configuration")
    parser.add_argument('--runon', default='hardware', help="Where to run these application")
    parser.add_argument('--show', action='store_true', help="Show configurations")
    args = parser.parse_args()

    if args.sdk is None:
        args.sdk =  os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "../../../"))
    if (os.path.isfile(args.appcfg) == False) or (os.path.isfile(args.appyaml) == False):
        print("Invalid application cfg/yaml, please check!")
        sys.exit(1)
    pp = pprint.PrettyPrinter(compact=True)
    ret = True
    nsdk_ext = nsdk_runner(args.sdk, args.appcfg, args.appyaml)
    if args.show:
        print("Here are the supported configs:")
        pp.pprint(nsdk_ext.get_configs())
    else:
        ret = nsdk_ext.run_config(args.config, args.logdir, runon=args.runon)
    # Exit with ret value
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)
