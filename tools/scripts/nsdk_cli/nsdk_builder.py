#!/usr/bin/env python3

import os
import sys
import time
import copy
import shutil
import glob
import serial
import tempfile
import json
import argparse
from threading import Thread
import subprocess

from nsdk_utils import *

VALID_MAKEFILE_NAMES = ['Makefile', 'makefile', "GNUMakefile"]


class nsdk_builder(object):
    def __init__(self):
        pass

    @staticmethod
    def is_app(appdir):
        if os.path.isdir(appdir) == False:
            return False
        appdir = os.path.realpath(appdir)
        for mkname in VALID_MAKEFILE_NAMES:
            mkfile_path = os.path.join(appdir, mkname)
            if os.path.isfile(mkfile_path):
                return True
        return False

    @staticmethod
    def copy_objects(appsts, copydir):
        if isinstance(appsts, dict) and "objects" in appsts:
            os.makedirs(copydir, exist_ok=True)
            objects = appsts["objects"]
            if "saved_objects" not in appsts:
                appsts["saved_objects"] = dict()
            for obj in objects:
                obj_file = objects[obj]
                if os.path.isfile(obj_file): # only copy when exist
                    filename = os.path.basename(obj_file)
                    newfile = os.path.join(copydir, filename)
                    shutil.copyfile(obj_file, newfile)
                    appsts["saved_objects"][obj] = newfile
        pass

    @staticmethod
    def get_objects(appdir, timestamp=None):
        if nsdk_builder.is_app(appdir) == False:
            return None

        def find_app_object(pattern):
            files = find_files(appdir, pattern)
            found_file = ""
            latest_timestamp = 0
            for fl in files:
                flct = os.stat(fl).st_ctime
                if timestamp:
                    if flct >= timestamp:
                        found_file = fl
                        break
                else:
                    # find a latest file
                    if flct > latest_timestamp:
                        latest_timestamp = flct
                        found_file = fl
            return found_file
        build_objects = dict()
        build_objects["elf"] = find_app_object("*.elf")
        build_objects["map"] = find_app_object("*.map")
        build_objects["dump"] = find_app_object("*.dump")
        build_objects["dasm"] = find_app_object("*.dasm")
        build_objects["verilog"] = find_app_object("*.verilog")
        return build_objects

    def build_target_only(self, appdir, make_options="", target="clean", show_output=True, logfile=None, parallel=""):
        if self.is_app(appdir) == False:
            return COMMAND_NOTAPP, 0

        # Parallel must start with -j
        if isinstance(parallel, str):
            parallel = parallel.strip()
            if parallel != "" and parallel.startswith("-j") == False:
                parallel = ""
        else:
            parallel = ""
        if parallel != "": # need to split targets
            build_targets = target.strip().split()
            print("Target \"%s\" are split to seperated targets %s in parallel mode." %(target, build_targets))
        else:
            build_targets = [target]
        if os.path.isfile(logfile):
            os.remove(logfile)
        total_ticks = 0
        for btg in build_targets:
            build_cmd = "make %s -C %s %s %s" % (parallel, appdir, make_options, btg)
            if not ((show_output == False) and (btg == "info")):
                print("Build application %s, with target: %s" % (appdir, btg))
                print("Build command: %s" % (build_cmd))
            ret, ticks = run_command(build_cmd, show_output, logfile=logfile, append=True)
            if not ((show_output == False) and (btg == "info")):
                print("Build command return value: %s" % (ret))
            total_ticks += ticks
            if ret != 0: # if one target failed, then stop
                break

        return ret, ticks

    def get_build_info(self, appdir, make_options=""):
        infolog = tempfile.mktemp()
        ret, _ = self.build_target_only(appdir, make_options, "info", False, infolog)
        build_info = None
        if ret != COMMAND_RUNOK:
            os.remove(infolog)
            return build_info
        with open(infolog, "r") as inf:
            for line in inf.readlines():
                line = line.strip()
                INFO_TAG = "Current Configuration:"
                if line.startswith(INFO_TAG):
                    build_info = dict()
                    infos = line.strip(INFO_TAG).split()
                    for info in infos:
                        splits = info.split("=")
                        if len(splits) == 2:
                            build_info[splits[0]] = splits[1]
        os.remove(infolog)
        return build_info

    def build_target(self, appdir, make_options="", target="clean", show_output=True, logfile=None, parallel=""):
        if self.is_app(appdir) == False:
            return False, None
        build_status = dict()
        ret, ticks = self.build_target_only(appdir, make_options, target, show_output, logfile, parallel)
        cmdsts = True
        if ret == COMMAND_INTERRUPTED:
            print("%s: Exit program due to CTRL - C pressed" % (sys._getframe().f_code.co_name))
            sys.exit(1)
        elif ret == COMMAND_RUNOK:
            cmdsts = True
        else:
            cmdsts = False

        build_status["app"] = { "path": appdir, \
                                "make_options": make_options, \
                                "target": target }
        build_status["status"] = {"build": cmdsts}
        build_status["status_code"] = {"build": ret}
        build_status["logs"] = {"build": logfile}
        build_status["time"] = {"build": round(ticks, 2)}
        build_status["objects"] = nsdk_builder.get_objects(appdir)
        build_status["info"] = self.get_build_info(appdir, make_options)
        build_status["size"] = get_elfsize(build_status["objects"].get("elf", ""))
        return cmdsts, build_status

    def clean_app(self, appdir, make_options="", show_output=True, logfile=None):
        return self.build_target(appdir, make_options, "clean", show_output, logfile)

    def compile_app(self, appdir, make_options="", show_output=True, logfile=None, parallel=""):
        return self.build_target(appdir, make_options, "all", show_output, logfile, parallel)

    def upload_app(self, appdir, make_options="", show_output=True, logfile=None):
        if logfile is None:
            uploadlog = tempfile.mktemp()
        else:
            uploadlog = logfile
        cmdsts, build_status = self.build_target(appdir, make_options, "upload", show_output, uploadlog)
        uploader = dict()
        if cmdsts:
            upload_sts = False
            with open(uploadlog, 'r') as uf:
                for line in uf.readlines():
                    if "-ex" in line or "\\" in line:
                        # strip extra newline and \
                        uploader["cmd"] = uploader.get("cmd", "") + line.strip().strip("\\")
                    if "On-Chip Debugger" in line:
                        uploader["version"] = line.strip()
                    if "Start address" in line:
                        upload_sts = True
                        break
            if upload_sts == False: # actually not upload successfully
                cmdsts = False
        if "app" in build_status:
            build_status["app"]["uploader"] = uploader
        if logfile is None:
            os.remove(uploadlog)
        print("Upload application %s status: %s" % (appdir, cmdsts))
        return cmdsts, build_status

class MonitorThread(Thread):
    def __init__(self, port:str, baudrate:str, timeout:int, checks:dict, checktime=time.time(), sdk_check=False, logfile=None, show_output=False):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.checks = checks
        self.checktime = checktime
        self.sdk_check = sdk_check
        self.logfile = logfile
        self.show_output = show_output
        self._exit_req = False
        self._check_sdk = False
        self._check_sdk_timeout = 10
        pass

    def get_result(self):
        try:
            return self.result
        except Exception:
            return None

    def exit_request(self):
        self._exit_req = True
        pass

    def set_check_sdk_timeout(self, timeout=10):
        self._check_sdk_timestart = time.time()
        self._check_sdk_timeout = timeout
        self._check_sdk = True # start to check timeout monitor
        pass

    def run(self):
        start_time = time.time()
        serial_log = ""
        check_status = False
        pass_checks = self.checks.get("PASS", [])
        fail_checks = self.checks.get("FAIL", [])
        def test_in_check(string, checks):
            if type(checks) == list:
                for check in checks:
                    if check in string:
                        return True
            return False

        print("Read serial log from %s, baudrate %s" %(self.port, self.baudrate))
        NSDK_CHECK_TAG = "Nuclei SDK Build Time:"
        print("Checker used: ", self.checks)
        check_finished = False
        try:
            ser = None
            ser = serial.Serial(self.port, self.baudrate, timeout=3)
            while (time.time() - start_time) < self.timeout:
                if self._exit_req:
                    break
                # Remove '\r' in serial read line
                sline = ser.readline()
                line = str(try_decode_bytes(sline)).replace('\r', '')
                if self.sdk_check == True:
                    if self.show_output:
                        print("XXX Check " + line, end='')
                    if self._check_sdk:
                        chk_time_cost = time.time() - self._check_sdk_timestart
                        if chk_time_cost > self._check_sdk_timeout:
                            print("No SDK banner found in %s, quit now!", self._check_sdk_timeout)
                            break
                    if NSDK_CHECK_TAG in line:
                        timestr = line.split(NSDK_CHECK_TAG)[-1].strip()
                        cur_time = time.mktime(time.strptime(timestr, "%b %d %Y, %H:%M:%S"))
                        if int(cur_time) >= int(self.checktime):
                            self.sdk_check = False
                            line = NSDK_CHECK_TAG + " " + timestr + "\n"
                            serial_log = serial_log + str(line)
                else:
                    serial_log = serial_log + str(line)
                    if self.show_output:
                        print(line, end='')
                    if check_finished == False:
                        if test_in_check(line, fail_checks):
                            check_status = False
                            check_finished = True
                        if test_in_check(line, pass_checks):
                            check_status = True
                            check_finished = True
                        if check_finished:
                            # record another 2 seconds by reset start_time and timeout to 2
                            start_time = time.time()
                            self.timeout = 2

        except serial.serialutil.SerialException:
            # https://stackoverflow.com/questions/21050671/how-to-check-if-device-is-connected-pyserial
            print("serial port %s might not exist or in use" % self.port)
        except:
            print("Some error happens during serial operations")
        finally:
            if ser:
                ser.close()
        if self.logfile:
            with open(self.logfile, 'w') as lf:
                lf.write(serial_log)
        self.result = check_status
        return check_status

class nsdk_runner(nsdk_builder):
    def __init__(self):
        super().__init__()
        pass

    @staticmethod
    def find_apps(rootdir):
        subdirectories = [x[0] for x in os.walk(rootdir)]
        appdirs = []
        for subdir in subdirectories:
            if nsdk_runner.is_app(subdir):
                appdirs.append(os.path.normpath(subdir))
        return appdirs

    def build_target_in_directory(self, rootdir, make_options="", target="", \
                                show_output=True, logdir=None, stoponfail=False):
        appdirs = self.find_apps(rootdir)
        if len(appdirs) == 0:
            return False, None
        cmdsts = True
        build_status = dict()
        createlog = False
        if isinstance(logdir, str):
            createlog = True
            if os.path.isdir(logdir) == False:
                os.makedirs(logdir)

        for appdir in appdirs:
            appdir = appdir.replace("\\", "/") # Change windows \\ path to /
            applogfile = None
            if createlog:
                applogfile = get_logfile(appdir, rootdir, logdir, "build.log")
            appcmdsts, appsts = self.build_target(appdir, make_options, \
                            target, show_output, logfile=applogfile)
            build_status[appdir] = appsts
            if appcmdsts == False:
                cmdsts = appcmdsts
                if stoponfail == True:
                    print("Stop build directory due to fail on application %s" %(appdir))
                    return cmdsts, build_status
        return cmdsts, build_status

    def analyze_runlog(self, logfile):
        result = {"type": "unknown", "value": {}}
        if os.path.isfile(logfile):
            result_lines = open(logfile).readlines()
            program_found, result_parsed = parse_benchmark_runlog(result_lines)
            if program_found != PROGRAM_UNKNOWN:
                result = {"type": program_found, "value": result_parsed}
        return result

    def run_app_onhw(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        make_options = runcfg["misc"]["make_options"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("hardware", None)
        serport = None
        timeout = 60
        baudrate = 115200
        if hwconfig is not None:
            most_possible_serport = find_most_possible_serport()
            serport = hwconfig.get("serport", most_possible_serport)
            baudrate = hwconfig.get("baudrate", 115200)
            timeout = hwconfig.get("timeout", 60)
        ser_thread = None
        uploader = None
        try:
            if serport: # only monitor serial port when port found
                ser_thread = MonitorThread(serport, baudrate, timeout, app_runchecks, checktime, True, logfile, show_output)
                ser_thread.start()
            else:
                print("Warning: No available serial port found, please check!")
            cmdsts, upload_sts = self.upload_app(appdir, make_options, show_output, None)
            uploader = upload_sts.get("app", dict()).get("uploader", None)
            status = True
            if ser_thread:
                if cmdsts == False:
                    ser_thread.exit_request()
                else:
                    ser_thread.set_check_sdk_timeout(5)
                while ser_thread.is_alive():
                    ser_thread.join(1)
                status = ser_thread.get_result()
                del ser_thread
        except (KeyboardInterrupt, SystemExit):
            print("%s: Exit program due to CTRL - C pressed or SystemExit" % (sys._getframe().f_code.co_name))
            if ser_thread:
                ser_thread.exit_request()
            sys.exit(1)

        final_status = cmdsts and status
        return final_status, uploader

    def run_app_onqemu(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        build_info = runcfg["misc"]["build_info"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("qemu", dict())

        timeout = 60
        qemu_exe = None
        if hwconfig is not None:
            qemu32_exe = hwconfig.get("qemu32", "qemu-system-riscv32")
            qemu64_exe = hwconfig.get("qemu64", "qemu-system-riscv64")
            qemu_exe = qemu32_exe
            build_soc = build_info["SOC"]
            build_board = build_info["BOARD"]
            if build_soc == "hbird" or build_soc == "demosoc":
                machine = "nuclei_n"
            else:
                if build_board == "gd32vf103v_rvstar":
                    machine = "gd32vf103_rvstar"
                elif build_board == "gd32vf103v_eval":
                    machine = "gd32vf103_eval"
                else:
                    machine = "nuclei_n"
            if "rv64" in build_info["RISCV_ARCH"]:
                qemu_exe = qemu64_exe
            timeout = hwconfig.get("timeout", 60)
        runner = None
        cmdsts = False
        if qemu_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s --version" % (qemu_exe)
                verchk = "QEMU emulator version"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret:
                    command = "%s -M %s -cpu nuclei-%s -nodefaults -nographic -serial stdio -kernel %s" \
                        % (qemu_exe, machine, build_info["CORE"], build_objects["elf"])
                    print("Run command: %s" %(command))
                    runner = {"cmd": command, "version": verstr}
                    cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, True, logfile, show_output)
                else:
                    print("%s doesn't exist in PATH, please check!" % qemu_exe)
            else:
                print("ELF file %s doesn't exist, can't run on qemu" % (build_objects["elf"]))

        final_status = cmdsts
        return final_status, runner

    def run_app_onxlspike(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        build_info = runcfg["misc"]["build_info"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("xlspike", dict())

        timeout = 60
        xlspike_exe = None
        if hwconfig is not None:
            xlspike_exe = hwconfig.get("xlspike", "xl_spike")
            build_soc = build_info["SOC"]
            build_board = build_info["BOARD"]
            if not (build_soc == "hbird" or build_soc == "demosoc"):
                xlspike_exe = None
                print("SOC=%s BOARD=%s is not supported by xlspike" % (build_soc, build_board))
            else:
                riscv_arch = build_info["RISCV_ARCH"]
            timeout = hwconfig.get("timeout", 60)
        runner = None
        cmdsts = False
        if xlspike_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s --help" % (xlspike_exe)
                verchk = "RISC-V ISA Simulator"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret:
                    command = "%s --isa %s %s" % (xlspike_exe, riscv_arch, build_objects["elf"])
                    print("Run command: %s" %(command))
                    runner = {"cmd": command, "version": verstr}
                    cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, True, logfile, show_output)
                else:
                    print("%s doesn't exist in PATH, please check!" % xlspike_exe)
            else:
                print("ELF file %s doesn't exist, can't run on xlspike" % (build_objects["elf"]))
        else:
            print("Can't run on xlspike due to run config not exist or config not supported")

        final_status = cmdsts
        return final_status, runner

    def build_app_with_config(self, appdir, appconfig:dict, show_output=True, logfile=None):
        build_config = appconfig.get("build_config", None)
        target = appconfig.get("build_target", "all")
        parallel = appconfig.get("parallel", "")
        # Copy program objects if copy_objects is true
        copy_objects_required = appconfig.get("copy_objects", False)
        make_options = ""
        if isinstance(build_config, dict):
            for key, value in build_config.items():
                value = str(value).strip()
                if " " in key:
                    continue
                if " " in value:
                    make_options += " %s=\"%s\""%(key, value)
                else:
                    make_options += " %s=%s"%(key, value)
        appcmdsts, appsts = self.build_target(appdir, make_options, target, show_output, logfile, parallel)
        objs_copydir = os.path.dirname(logfile) # where objects are copied to
        # copy objects if copy_objects_required
        if copy_objects_required:
            nsdk_builder.copy_objects(appsts, objs_copydir)
        buildtime = appsts["time"]["build"]
        print("Build application %s, time cost %s seconds, passed: %s" %(appdir, buildtime, appcmdsts))

        appsts["config"] = appconfig
        return appcmdsts, appsts

    def run_app_with_config(self, appdir, appconfig:dict, show_output=True, buildlog=None, runlog=None):
        appconfig["build_target"] = "clean dasm"
        # build application
        build_cktime = time.time()
        appcmdsts, appsts = self.build_app_with_config(appdir, appconfig, show_output, buildlog)
        # run application
        if appcmdsts == False:
            print("Failed to build application %s, so we can't run it!" % (appdir))
            return appcmdsts, appsts
        # get run config
        app_runcfg = appconfig.get("run_config", dict())
        app_runtarget = app_runcfg.get("target", "hardware")
        # get run checks
        DEFAULT_CHECKS = { "PASS": [ ], "FAIL": [ "MCAUSE:" ] }
        app_runchecks = appconfig.get("checks", DEFAULT_CHECKS)
        misc_config = {"make_options": appsts["app"]["make_options"], \
            "build_info": appsts["info"], "build_objects": appsts["objects"], "build_time": build_cktime}
        runcfg = {"run_config": app_runcfg, "checks": app_runchecks, "misc": misc_config}
        print("Run application on %s" % app_runtarget)
        runstarttime = time.time()
        runstatus = False
        appsts["status_code"]["run"] = RUNSTATUS_NOTSTART
        if app_runtarget == "hardware":
            runstatus, uploader = self.run_app_onhw(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if uploader:
                appsts["app"]["uploader"] = uploader
        elif app_runtarget == "qemu":
            runstatus, runner = self.run_app_onqemu(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["qemu"] = runner
        elif app_runtarget == "xlspike":
            runstatus, runner = self.run_app_onxlspike(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["xlspike"] = runner
        else:
            print("Unsupported run target %s" %(app_runtarget))

        runtime = round(time.time() - runstarttime, 2)
        print("Run application %s on %s, time cost %s seconds, passed: %s" %(appdir, app_runtarget, runtime, runstatus))
        appsts["status"]["run"] = runstatus
        appsts["time"]["run"] = runtime
        return runstatus, appsts

    def build_apps_with_config(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        # Build all the applications, each application only has one configuration
        # "app" : { the_only_config }
        cmdsts = True
        build_status = dict()
        apps_config = copy.deepcopy(config)
        for appdir in apps_config:
            appconfig = apps_config[appdir]
            applogs = appconfig.get("logs", dict())
            applogfile = applogs.get("build", None)
            appcmdsts, appsts = self.build_app_with_config(appdir, appconfig, show_output, applogfile)
            build_status[appdir] = appsts
            if appcmdsts == False:
                cmdsts = appcmdsts
                if stoponfail == True:
                    print("Stop build apps with config due to fail on application %s" %(appdir))
                    return cmdsts, build_status
        return cmdsts, build_status

    def build_apps_with_configs(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        # Build all the applications, each application has more than one configuration
        # "app" : {"configs": {"case1": case1_config}}
        cmdsts = True
        build_status = dict()
        apps_config = copy.deepcopy(config)
        for appdir in apps_config:
            appconfigs = apps_config[appdir]
            if "configs" not in appconfigs:
                continue
            build_status[appdir] = dict()
            app_allconfigs = appconfigs["configs"]
            for cfgname in app_allconfigs:
                appconfig = app_allconfigs[cfgname]  # get configuration for each case for single app
                applogs = appconfig.get("logs", dict())
                applogfile = applogs.get("build", None)
                appcmdsts, appsts = self.build_app_with_config(appdir, appconfig, show_output, applogfile)
                build_status[appdir][cfgname] = appsts
                if appcmdsts == False:
                    cmdsts = appcmdsts
                    if stoponfail == True:
                        print("Stop build apps with config due to fail on application %s" %(appdir))
                        return cmdsts, build_status
        return cmdsts, build_status

    def run_apps_with_config(self, config:dict, show_output=True, stoponfail=False):
        # Run all the applications, each application only has one configuration
        # "app" : { the_only_config }
        cmdsts = True
        build_status = dict()
        apps_config = copy.deepcopy(config)
        for appdir in apps_config:
            appconfig = apps_config[appdir]
            applogs = appconfig.get("logs", dict())
            app_buildlogfile = applogs.get("build", None)
            app_runlogfile = applogs.get("run", None)
            appcmdsts, appsts = self.run_app_with_config(appdir, appconfig, show_output, app_buildlogfile, app_runlogfile)
            build_status[appdir] = appsts
            if appcmdsts == False:
                cmdsts = appcmdsts
                if stoponfail == True:
                    print("Stop run apps with config due to fail on application %s" %(appdir))
                    return cmdsts, build_status
        return cmdsts, build_status

    def run_apps_with_configs(self, config:dict, show_output=True, stoponfail=False):
        # Run all the applications, each application has more than one configuration
        # "app" : {"configs": {"case1": case1_config}}
        cmdsts = True
        build_status = dict()
        apps_config = copy.deepcopy(config)
        for appdir in apps_config:
            appconfigs = apps_config[appdir]
            if "configs" not in appconfigs:
                continue
            build_status[appdir] = dict()
            app_allconfigs = appconfigs["configs"]
            for cfgname in app_allconfigs:
                appconfig = app_allconfigs[cfgname] # get configuration for each case for single app
                applogs = appconfig.get("logs", dict())
                app_buildlogfile = applogs.get("build", None)
                app_runlogfile = applogs.get("run", None)
                appcmdsts, appsts = self.run_app_with_config(appdir, appconfig, show_output, app_buildlogfile, app_runlogfile)
                build_status[appdir][cfgname] = appsts
                if appcmdsts == False:
                    cmdsts = appcmdsts
                    if stoponfail == True:
                        print("Stop run apps with config due to fail on application %s" %(appdir))
                        return cmdsts, build_status
        return cmdsts, build_status
