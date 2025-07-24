#!/usr/bin/env python3

import os
import sys
import time
import copy
import shutil
import glob
import traceback

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))

try:
    import serial
    import tempfile
    import json
    import argparse
    from threading import Thread
    import subprocess
except:
    print("Please install requried packages using: pip3 install -r %s" % (requirement_file))
    sys.exit(1)

from nsdk_utils import *

VALID_MAKEFILE_NAMES = ['Makefile', 'makefile', "GNUMakefile"]

def is_nuclei_evalsoc(soc):
    if soc == "hbird" or soc == "demosoc" or soc == "evalsoc" or soc == "xlspike":
        return True
    else:
        return False

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
            cp_keys = get_sdk_copyobjects()
            if cp_keys != None:
                cp_keys = cp_keys.strip().split(",")
            for obj in objects:
                obj_file = objects[obj]
                if os.path.isfile(obj_file): # only copy when exist
                    filename = os.path.basename(obj_file)
                    filesuffix = os.path.splitext(filename)[-1].strip(".")
                    newfile = os.path.join(copydir, filename)
                    if cp_keys is None or filesuffix in cp_keys:
                        shutil.copyfile(obj_file, newfile)
                        appsts["saved_objects"][obj] = newfile
        pass

    @staticmethod
    def get_objects(appdir, target=None, timestamp=None):
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
        if target:
            build_objects["elf"] = find_app_object("*%s.elf" % (target))
            build_objects["map"] = find_app_object("*%s.map" % (target))
            build_objects["dump"] = find_app_object("*%s.dump" % (target) )
            build_objects["dasm"] = find_app_object("*%s.dasm" % (target))
            build_objects["verilog"] = find_app_object("*%s.verilog" % (target))
        else:
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
            if get_sdk_verb_buildmsg():
                print("Target \"%s\" are split to seperated targets %s in parallel mode." %(target, build_targets))
        else:
            build_targets = [target]
        if os.path.isfile(logfile):
            os.remove(logfile)
        total_ticks = 0
        ignore_targets = ["info", "showtoolver", "showflags", "clean", "bin", "size"]
        for btg in build_targets:
            build_cmd = "make %s -C %s %s %s" % (parallel, appdir, make_options, btg)
            if get_sdk_verb_buildmsg() and (not ((show_output == False) and (btg in ignore_targets))):
                print("Build application %s, with target: %s" % (appdir, btg))
                print("Build command: %s" % (build_cmd))
            ret, ticks = run_command(build_cmd, show_output, logfile=logfile, append=True)
            if get_sdk_verb_buildmsg() and (not ((show_output == False) and (btg in ignore_targets))):
                print("Build command return value: %s" % (ret))
            total_ticks += ticks
            if ret != 0: # if one target failed, then stop
                break

        return ret, ticks

    def get_build_info(self, appdir, make_options=""):
        infolog = tempfile.mktemp()
        ret, _ = self.build_target_only(appdir, make_options, "info", False, infolog)
        build_info = dict()
        if ret != COMMAND_RUNOK:
            os.remove(infolog)
            return build_info
        build_info = dict()
        with open(infolog, "r", errors='ignore') as inf:
            for line in inf.readlines():
                line = line.strip()
                INFO_TAG = "Current Configuration:"
                if line.startswith(INFO_TAG):
                    infos = line.replace(INFO_TAG, "").strip().split()
                    for info in infos:
                        splits = info.split("=")
                        if len(splits) == 2:
                            build_info[splits[0]] = splits[1]

        os.remove(infolog)
        return build_info

    def get_build_flags(self, appdir, make_options=""):
        flagslog = tempfile.mktemp()
        ret, _ = self.build_target_only(appdir, make_options, "showflags", False, flagslog)
        build_flags = dict()
        if ret != COMMAND_RUNOK:
            os.remove(flagslog)
            return build_flags
        build_flags = dict()
        with open(flagslog, "r", errors='ignore') as inf:
            for line in inf.readlines():
                line = line.strip()
                if ":" in line:
                    splits = line.split(":")
                    if len(splits) != 2:
                        continue
                    key, value = splits
                    key = key.strip()
                    value = value.strip()
                    build_flags[key] = value

        os.remove(flagslog)
        return build_flags

    def get_build_toolver(self, appdir, make_options=""):
        log = tempfile.mktemp()
        ret, _ = self.build_target_only(appdir, make_options, "showtoolver", False, log)
        buildout = dict()
        if ret != COMMAND_RUNOK:
            os.remove(log)
            return buildout
        buildout = dict()
        toolname = ""
        with open(log, "r", errors='ignore') as inf:
            for line in inf.readlines():
                line = line.strip()
                if line.startswith("Show"):
                    toolname = line.split()[1].strip()
                    buildout[toolname] = ""
                elif line.startswith("make:") == False:
                    if toolname != "":
                        buildout[toolname] += line + "\n"

        os.remove(log)
        return buildout

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
                                "ci": get_ci_info(), \
                                "target": target }
        build_status["status"] = {"build": cmdsts}
        build_status["status_code"] = {"build": ret}
        build_status["logs"] = {"build": logfile}
        build_status["time"] = {"build": round(ticks, 2)}
        build_status["info"] = self.get_build_info(appdir, make_options)
        build_status["toolver"] = self.get_build_toolver(appdir, make_options)
        build_status["flags"] = self.get_build_flags(appdir, make_options)
        apptarget = None
        if build_status["flags"]:
            apptarget = build_status["flags"].get("TARGET", None)
        build_status["objects"] = nsdk_builder.get_objects(appdir, apptarget)
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
        upload_sts = False
        # some error might happened error, try catch protect it
        try:
            with open(uploadlog, 'r', errors='ignore') as uf:
                for line in uf.readlines():
                    if "-ex" in line or "\\" in line:
                        # strip extra newline and \
                        uploader["cmd"] = uploader.get("cmd", "") + line.strip().strip("\\")
                    if "On-Chip Debugger" in line:
                        uploader["version"] = line.strip()
                    if "A problem internal to GDB has been detected" in line:
                        uploader["gdbstatus"] = "hang"
                    if "Quit this debugging session?" in line:
                        uploader["gdbstatus"] = "hang"
                    if "Remote communication error" in line:
                        uploader["gdbstatus"] = "lostcon"
                    if "Start address" in line:
                        uploader["gdbstatus"] = "ok"
                        upload_sts = True
                        break
            # append openocd log to upload log
            openocd_log = os.path.join(appdir, "openocd.log")
            if os.path.isfile(openocd_log):
                with open(uploadlog, 'a', errors='ignore') as uf:
                    uf.write("\n=====OpenOCD log content dumped as below:=====\n")
                    with open(openocd_log, "r", errors='ignore') as of:
                        for line in of.readlines():
                            if "Error: Target not examined yet" in line:
                                uploader["cpustatus"] = "hang"
                            if "Examined RISC-V core" in line:
                                uploader["cpustatus"] = "ok"
                            if "Unable to halt hart" in line:
                                uploader["cpustatus"] = "hang"
                            uf.write(line)
        except Exception as exc:
            print("Some error happened during upload application, %s" % (str(exc)))
            traceback.print_exc()
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
        self.tty_iserr = False
        self.sdk_check = sdk_check
        self.logfile = logfile
        self.show_output = show_output
        self._exit_req = False
        self._check_sdk = False
        self._check_sdk_timeout = 10
        self.result = False
        self.reason = ""
        pass

    def get_result(self):
        try:
            return self.result
        except Exception:
            return False

    def get_reason(self):
        try:
            return self.reason
        except Exception:
            return ""

    def get_tty_iserror(self):
        return self.tty_iserr

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
        serial_prelog = ""
        serial_log = ""
        check_status = False
        check_reason = ""
        pass_checks = self.checks.get("PASS", [])
        fail_checks = self.checks.get("FAIL", [])
        def test_in_check(string, checks):
            if type(checks) == list:
                for check in checks:
                    if check in string:
                        return True
            return False

        NSDK_CHECK_TAG = get_sdk_checktag()
        if get_sdk_verb_buildmsg():
            print("Read serial log from %s, baudrate %s" %(self.port, self.baudrate))
            print("Checker used: ", self.checks)
            print("SDK Checker Tag \"%s\", checker enable %s" % (NSDK_CHECK_TAG, self.sdk_check))
        print("SDK run timeout %s, banner timeout %s" % (self.timeout, self._check_sdk_timeout))
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
                            print("No SDK banner found in %s s, quit now!" % (self._check_sdk_timeout))
                            check_reason = BANNER_TMOUT
                            break
                    if NSDK_CHECK_TAG in line:
                        timestr = line.split(NSDK_CHECK_TAG)[-1].strip()
                        if "Download" in timestr:
                            print("Warning: Download and SDK tag in same line which should not happen!")
                            #timestr = timestr.split("Download")[0].strip()
                        try:
                            cur_time = time.mktime(time.strptime(timestr, "%b %d %Y, %H:%M:%S"))
                        except:
                            print("Warning: Failed to parse time string %s" % (timestr))
                            cur_time = 0
                        if int(cur_time) >= int(self.checktime):
                            self.sdk_check = False
                            line = NSDK_CHECK_TAG + " " + timestr + "\n"
                            serial_log = serial_log + str(line)
                        else:
                            # record previous log of this application
                            serial_prelog = serial_prelog + str(line)
                else:
                    serial_log = serial_log + str(line)
                    if self.show_output:
                        print(line, end='')
                    if check_finished == False:
                        if test_in_check(line, fail_checks):
                            check_reason = "FAIL line: %s" % (line)
                            check_status = False
                            check_finished = True
                        if test_in_check(line, pass_checks):
                            check_reason = "PASS line: %s" % (line)
                            check_status = True
                            check_finished = True
                        if check_finished:
                            # record another 2 seconds by reset start_time and timeout to 2
                            start_time = time.time()
                            self.timeout = 2

        except serial.serialutil.SerialException:
            # https://stackoverflow.com/questions/21050671/how-to-check-if-device-is-connected-pyserial
            print("serial port %s might not exist or in use" % self.port)
            # set tty is error
            self.tty_iserr = True
            check_reason = TTY_OP_ERR
        except Exception as exc:
            print("Some error happens during serial operations, %s" % (str(exc)))
            check_reason = TTY_UNKNOWN_ERR
        finally:
            if ser:
                ser.close()
        if self.logfile:
            with open(self.logfile, 'w') as lf:
                lf.write(serial_log)
            prelogfile = os.path.splitext(self.logfile)[0] + "pre.log"
            # record prestep serial log
            if serial_prelog.strip() != "":
                with open(prelogfile, 'w') as lf:
                    lf.write(serial_prelog)
        self.result = check_status
        self.reason = check_reason
        return check_status

class nsdk_runner(nsdk_builder):
    def __init__(self):
        super().__init__()
        self.hangup_action = None
        self.ttyerrcnt = 0
        self.fpgaprogramcnt = 0
        self.gdberrcnt = 0
        self.bannertmoutcnt = 0
        self.uploaderrcnt = 0
        pass

    @staticmethod
    def find_apps(rootdir):
        subdirectories = [x[0] for x in os.walk(rootdir)]
        appdirs = []
        for subdir in subdirectories:
            if nsdk_runner.is_app(subdir):
                appdirs.append(os.path.normpath(subdir))
        return appdirs

    def reset_counters(self):
        self.ttyerrcnt = 0
        self.uploaderrcnt = 0
        self.fpgaprogramcnt = 0
        self.gdberrcnt = 0
        self.bannertmoutcnt = 0
        pass

    def need_exit_now(self):
        if self.ttyerrcnt > get_sdk_ttyerr_maxcnt():
            return True
        if self.fpgaprogramcnt > get_sdk_fpgaprog_maxcnt():
            return True
        if self.gdberrcnt > get_sdk_gdberr_maxcnt():
            return True
        if self.bannertmoutcnt > get_sdk_bannertmout_maxcnt():
            return True
        if self.uploaderrcnt > get_sdk_uploaderr_maxcnt():
            return True
        return False

    def show_counters(self):
        print("TTY Error counter %d, limit count %d" % (self.ttyerrcnt, get_sdk_ttyerr_maxcnt()))
        print("GDB Internal Error counter %d, limit count %d" % (self.gdberrcnt, get_sdk_gdberr_maxcnt()))
        print("Upload Error counter %d, limit count %d" % (self.uploaderrcnt, get_sdk_uploaderr_maxcnt()))
        print("Banner Timeout Error counter %d, limit count %d" % (self.bannertmoutcnt, get_sdk_bannertmout_maxcnt()))
        print("FPGA Program Error counter %d, limit count %d" % (self.fpgaprogramcnt, get_sdk_fpgaprog_maxcnt()))
        pass

    def set_cpu_hangup_action(self, hangaction):
        self.hangup_action = hangaction
        pass

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

    def analyze_runlog(self, logfile, parsescript=None):
        result = {"type": "unknown", "value": {}}
        if os.path.isfile(logfile):
            try:
                result_lines = open(logfile, "r", errors='ignore').readlines()
            except:
                result_lines = []
            program_found, subtype, result_parsed = parse_benchmark_runlog(result_lines, lgf=logfile)
            if program_found == PROGRAM_UNKNOWN:
                program_found, subtype, result_parsed = parse_benchmark_use_pyscript(result_lines, logfile, parsescript)
            if program_found != PROGRAM_UNKNOWN:
                result = {"type": program_found, "subtype": subtype, "value": result_parsed}

        return result

    def run_app_onhw(self, appdir, runcfg:dict(), show_output=True, logfile=None, uploadlog=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        make_options = runcfg["misc"]["make_options"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("hardware", None)
        serport = None
        timeout = 60
        baudrate = 115200
        fpgabit = None
        fpgaserial = None
        run_tmout = get_sdk_run_tmout()
        if hwconfig is not None:
            most_possible_serport = find_most_possible_serport()
            serport = hwconfig.get("serport", most_possible_serport)
            baudrate = hwconfig.get("baudrate", 115200)
            # priority: environment var SDK_RUN_TMOUT > hwconfig in app.json
            timeout = run_tmout if run_tmout is not None else hwconfig.get("timeout", 60)
            fpgabit = hwconfig.get("fpgabit", None)
            fpgaserial = hwconfig.get("fpgaserial", None)
        ser_thread = None
        uploader = None
        sdk_check = get_sdk_check()
        banner_tmout = get_sdk_banner_tmout()
        retry_cnt = 0
        max_retrycnt = 1
        while True:
            try:
                if retry_cnt > max_retrycnt: # do retry
                    break
                retry_cnt += 1
                print("Hardware configuration: serial port %s, baudrate %s, timeout %ds, retry counter %d" % (serport, baudrate, timeout, retry_cnt))
                if serport: # only monitor serial port when port found
                    ser_thread = MonitorThread(serport, baudrate, timeout, app_runchecks, checktime, \
                        sdk_check, logfile, show_output)
                    ser_thread.start()
                else:
                    print("Warning: No available serial port found, please check!")
                    self.ttyerrcnt += 1
                cmdsts, upload_sts = self.upload_app(appdir, make_options, show_output, uploadlog)
                uploader = upload_sts.get("app", dict()).get("uploader", None)
                uploader["retried"] = retry_cnt
                status = True
                check_reason = ""
                if ser_thread:
                    if cmdsts == False:
                        ser_thread.exit_request()
                    else:
                        ser_thread.set_check_sdk_timeout(banner_tmout)
                    while ser_thread.is_alive():
                        ser_thread.join(1)
                    status = ser_thread.get_result()
                    check_reason = ser_thread.get_reason()
                    if ser_thread.get_tty_iserror(): # tty is in use or not exist
                        print("tty serial port error count %d" % (self.ttyerrcnt))
                        self.ttyerrcnt += 1
                    del ser_thread
                if uploader.get("cpustatus", "") == "hang": # cpu hangs then call cpu hangup action and retry this application
                    max_retrycnt = 1 # when upload error happened, do retry once
                    self.uploaderrcnt += 1
                    if self.hangup_action is not None:
                        print("Execute hangup action for hangup case!")
                        if self.hangup_action() == True:
                            print("hangup action success!")
                            continue
                        else:
                            print("hangup action failed!")
                    elif fpgabit and fpgaserial:
                        print("Reprogram fpga bit %s on fpga board serial number %s, total fpga reprogam count %d" % (fpgabit, fpgaserial, self.fpgaprogramcnt))
                        self.fpgaprogramcnt += 1
                        if program_fpga(fpgabit, fpgaserial) == True:
                            print("Reprogram fpga sucessfully!")
                            continue
                        else:
                            print("Reprogram fpga failed!")
                    else:
                        print("No cpu hangup action found, just continue with other cases")
                if uploader.get("gdbstatus", "") == "hang": # gdb hangs with internal error retry upload this application
                    max_retrycnt = 2 # when gdb internal error happened, do retry twice
                    print("GDB internal error happened, re-upload application, total re-upload count %d" % (self.gdberrcnt))
                    self.gdberrcnt += 1
                    continue
                if status == False and check_reason == BANNER_TMOUT: # banner timeout error, so retry upload application
                    max_retrycnt = 1 # when banner timeout error happened, do retry once
                    print("Banner timeout, re-upload application, total re-upload count %d" % (self.bannertmoutcnt))
                    self.bannertmoutcnt += 1
                    continue
                # exit with upload status
                break
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
        build_config = runcfg["misc"]["build_config"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("qemu", dict())

        timeout = 60
        qemu_exe = None
        qemu_extraopt = ""
        if hwconfig is not None:
            qemu32_exe = hwconfig.get("qemu32", "qemu-system-riscv32")
            qemu64_exe = hwconfig.get("qemu64", "qemu-system-riscv64")
            qemu_machine = hwconfig.get("qemu_machine", None)
            qemu_cpu = hwconfig.get("qemu_cpu", None)
            qemu_exe = qemu32_exe
            build_soc = build_info["SOC"]
            build_board = build_info["BOARD"]
            build_core = build_info["CORE"]
            build_download = build_info["DOWNLOAD"]
            build_smp = build_info.get("SMP", "")
            build_arch_ext = build_config.get("ARCH_EXT", "")
            build_semihost = False
            if build_config.get("SEMIHOST", "") != "":
                build_semihost = True

            if build_arch_ext == "":
                build_arch_ext = build_info.get("ARCH_EXT", "")
            if build_smp != "":
                qemu_extraopt = "%s -smp %s" % (qemu_extraopt, build_smp)
            if build_semihost: # use qemu semihosting, if program build with semihost feature
                qemu_extraopt = "%s -semihosting " % (qemu_extraopt)
            if qemu_machine is None:
                if is_nuclei_evalsoc(build_soc):
                    machine = "nuclei_evalsoc"
                else:
                    if build_board == "gd32vf103v_rvstar":
                        machine = "gd32vf103_rvstar"
                    elif build_board == "gd32vf103v_eval":
                        machine = "gd32vf103_eval"
                    else:
                        machine = "nuclei_evalsoc"
                # machine combined with download
                machine = machine + ",download=%s" %(build_download.lower())
            else:
                machine = qemu_machine
            if qemu_cpu is None:
                qemu_sel_cpu = "nuclei-%s" % (build_core.lower())
                if build_arch_ext != "":
                    qemu_sel_cpu = qemu_sel_cpu + ",ext=%s" %(build_arch_ext)
            else:
                qemu_sel_cpu = qemu_cpu

            if "rv64" in build_info["RISCV_ARCH"]:
                qemu_exe = qemu64_exe
            timeout = hwconfig.get("timeout", 60)
        runner = None
        cmdsts = False
        sdk_check = get_sdk_check()
        if qemu_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s --version" % (qemu_exe)
                verchk = "QEMU emulator version"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret:
                    command = "%s %s -M %s -cpu %s -nodefaults -nographic -icount shift=0 -serial stdio -kernel %s" \
                        % (qemu_exe, qemu_extraopt, machine, qemu_sel_cpu, build_objects["elf"])
                    print("Run command: %s" %(command))
                    runner = {"cmd": command, "version": verstr}
                    cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, \
                        sdk_check, logfile, show_output)
                else:
                    print("%s doesn't exist in PATH, please check!" % qemu_exe)
            else:
                print("ELF file %s doesn't exist, can't run on qemu" % (build_objects["elf"]))

        final_status = cmdsts
        return final_status, runner

    def run_app_onxlmodel(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        build_info = runcfg["misc"]["build_info"]
        build_config = runcfg["misc"]["build_config"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("xlmodel", dict())

        timeout = 300 # xlmodel is slow compared with qemu
        xlmodel_exe = None
        xlmodel_extraopt = ""
        if hwconfig is not None:
            xlmodel_exe = hwconfig.get("xlmodel", "xl_cpumodel")
            xlmodel_machine = hwconfig.get("machine", "nuclei_evalsoc")
            xlmodel_cpu = hwconfig.get("xlmodel_cpu", None)
            build_soc = build_info["SOC"]
            build_board = build_info["BOARD"]
            build_core = build_info["CORE"]
            build_download = build_info["DOWNLOAD"]
            build_smp = build_info.get("SMP", "")
            build_arch_ext = build_config.get("ARCH_EXT", "")
            build_semihost = False
            if build_config.get("SEMIHOST", "") != "":
                build_semihost = True

            if build_arch_ext == "":
                build_arch_ext = build_info.get("ARCH_EXT", "")
            if build_smp != "":
                xlmodel_extraopt = "%s --smp=%s" % (xlmodel_extraopt, build_smp)
            #if build_semihost: # use xlmodel semihosting, if program build with semihost feature
            #    xlmodel_extraopt = "%s -semihosting " % (xlmodel_extraopt)
            if xlmodel_cpu is None:
                xlmodel_sel_cpu = "%s" % (build_core.lower())
            xlmodel_archext_opt = ""
            if build_arch_ext and build_arch_ext.strip() != "":
                xlmodel_archext_opt = "--ext=%s" % (build_arch_ext)
            timeout = hwconfig.get("timeout", timeout)
        runner = None
        cmdsts = False
        sdk_check = get_sdk_check()
        if xlmodel_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s -v" % (xlmodel_exe)
                verchk = "xl_cpumodel Version"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret:
                    command = "%s %s -M %s --cpu=%s %s %s" \
                        % (xlmodel_exe, xlmodel_extraopt, xlmodel_machine, xlmodel_sel_cpu, xlmodel_archext_opt, build_objects["elf"])
                    print("Run command: %s" %(command))
                    runner = {"cmd": command, "version": verstr}
                    cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, \
                        sdk_check, logfile, show_output)
                else:
                    print("%s doesn't exist in PATH, please check!" % xlmodel_exe)
            else:
                print("ELF file %s doesn't exist, can't run on xlmodel" % (build_objects["elf"]))

        final_status = cmdsts
        return final_status, runner

    def run_app_onxlspike(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        build_info = runcfg["misc"]["build_info"]
        build_config = runcfg["misc"]["build_config"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("xlspike", dict())

        timeout = 300 # xlspike is slow compared with qemu
        xlspike_exe = None
        xlspike_extraopt = ""
        if hwconfig is not None:
            xlspike_exe = hwconfig.get("xlspike", "xl_spike")
            build_soc = build_info["SOC"]
            build_board = build_info["BOARD"]
            riscv_arch = build_info["RISCV_ARCH"]
            # replace e with i for xlspike
            riscv_arch = riscv_arch.replace("e", "i")
            build_arch_ext = build_info.get("ARCH_EXT", "")
            build_smp = build_info.get("SMP", "")
            if build_smp != "":
                xlspike_extraopt = "%s -p%s" % (xlspike_extraopt, build_smp)
            if not is_nuclei_evalsoc(build_soc):
                xlspike_exe = None
                print("SOC=%s BOARD=%s is not supported by xlspike" % (build_soc, build_board))
            timeout = hwconfig.get("timeout", timeout)
        runner = None
        cmdsts = False
        sdk_check = get_sdk_check()
        if xlspike_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s --help" % (xlspike_exe)
                verchk = "RISC-V ISA Simulator"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret:
                    command = "%s %s --isa %s%s %s" % (xlspike_exe, xlspike_extraopt, riscv_arch, build_arch_ext, build_objects["elf"])
                    print("Run command: %s" %(command))
                    runner = {"cmd": command, "version": verstr}
                    cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, \
                        sdk_check, logfile, show_output)
                else:
                    print("%s doesn't exist in PATH, please check!" % xlspike_exe)
            else:
                print("ELF file %s doesn't exist, can't run on xlspike" % (build_objects["elf"]))
        else:
            print("Can't run on xlspike due to run config not exist or config not supported")

        final_status = cmdsts
        return final_status, runner

    def run_app_onncycm(self, appdir, runcfg:dict(), show_output=True, logfile=None):
        app_runcfg = runcfg.get("run_config", dict())
        app_runchecks = runcfg.get("checks", dict())
        build_info = runcfg["misc"]["build_info"]
        build_config = runcfg["misc"]["build_config"]
        build_objects = runcfg["misc"]["build_objects"]
        checktime = runcfg["misc"]["build_time"]
        hwconfig = app_runcfg.get("ncycm", dict())

        timeout = 600 # ncycm is slow compared with xlmodel or xlspike
        ncycm_exe = None
        if hwconfig is not None:
            ncycm_exe = hwconfig.get("ncycm", "ncycm")
            timeout = hwconfig.get("timeout", timeout)
        runner = None
        cmdsts = False
        sdk_check = get_sdk_check()
        if ncycm_exe:
            if os.path.isfile(build_objects["elf"]):
                vercmd = "%s -v" % (ncycm_exe)
                verchk = "version:"
                ret, verstr = check_tool_version(vercmd, verchk)
                if ret == False:
                    verstr = "v1"

                ret = check_tool_exist(ncycm_exe) or os.path.isfile(ncycm_exe)
                if ret:
                    if (verstr == "v1"):
                        ncycm_verilog = fix_evalsoc_verilog_ncycm(build_objects["verilog"])
                        if ncycm_verilog == "":
                            command = ""
                        else:
                            command = "%s +TESTCASE=%s" % (ncycm_exe, ncycm_verilog)
                    else:
                        command = "%s %s" % (ncycm_exe, build_objects["elf"])

                    if command != "":
                        print("Run command: %s" %(command))
                        runner = {"cmd": command, "version": verstr}
                        cmdsts, _ = run_cmd_and_check(command, timeout, app_runchecks, checktime, \
                            sdk_check, logfile, show_output, 480)
                    else:
                        print("Unable to run cycle model with %s" % (build_objects["elf"]))
                        cmdsts = False
                else:
                    print("%s doesn't exist in PATH, please check!" % ncycm_exe)
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
        copy_objects_required = get_sdk_need_copyobjects(appconfig)
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
        print("Build application %s with target %s, make options %s, time cost %s seconds, passed: %s" %(appdir, target, make_options, buildtime, appcmdsts))
        sys.stdout.flush()

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
        app_parsescript = app_runcfg.get("parsescript", None)
        if app_parsescript is not None:
            if os.path.isfile(app_parsescript) == False:
                app_parsescript = os.path.join(appdir, app_parsescript)
        # get run checks
        DEFAULT_CHECKS = { "PASS": [ ], "FAIL": [ "MCAUSE:" ] }
        app_runchecks = appconfig.get("checks", DEFAULT_CHECKS)
        misc_config = {"make_options": appsts["app"]["make_options"], "build_config": appconfig["build_config"],\
            "build_info": appsts["info"], "build_objects": appsts["objects"], "build_time": build_cktime}
        runcfg = {"run_config": app_runcfg, "checks": app_runchecks, "misc": misc_config}
        # get copy fail objects flags
        copy_objects_required = get_sdk_need_copyobjects(appconfig)
        copy_failobj_required = get_sdk_copy_failobj()

        print("Run application on %s" % app_runtarget)
        runstarttime = time.time()
        runstatus = False
        ignorehw = False
        appsts["status_code"]["run"] = RUNSTATUS_NOTSTART
        if app_runtarget == "hardware":
            uploadlog = None
            if runlog:
                uploadlog = os.path.join(os.path.dirname(runlog), "upload.log")
            runstatus, uploader = self.run_app_onhw(appdir, runcfg, show_output, runlog, uploadlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog, app_parsescript)
            appsts["logs"]["run"] = runlog
            appsts["logs"]["upload"] = uploadlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if uploader:
                appsts["app"]["uploader"] = uploader
        elif app_runtarget == "qemu":
            runstatus, runner = self.run_app_onqemu(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog, app_parsescript)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["qemu"] = runner
        elif app_runtarget == "xlmodel":
            runstatus, runner = self.run_app_onxlmodel(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog, app_parsescript)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["xlmodel"] = runner
        elif app_runtarget == "xlspike":
            runstatus, runner = self.run_app_onxlspike(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog, app_parsescript)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["xlspike"] = runner
        elif app_runtarget == "ncycm":
            runstatus, runner = self.run_app_onncycm(appdir, runcfg, show_output, runlog)
            # If run successfully, then do log analyze
            if runlog and runstatus:
                appsts["result"] = self.analyze_runlog(runlog, app_parsescript)
            appsts["logs"]["run"] = runlog
            appsts["status_code"]["run"] = RUNSTATUS_OK if runstatus else RUNSTATUS_FAIL
            if runner:
                appsts["app"]["ncycm"] = runner
        else:
            print("Unsupported run target %s" %(app_runtarget))
            ignorehw = True

        # only do copy when needed, fail copy enabled and copy objects disabled
        if copy_failobj_required == True and copy_objects_required == False \
                and runstatus == False and ignorehw == False and runlog is not None:
            objs_copydir = os.path.dirname(runlog) # where objects are copied to
            nsdk_builder.copy_objects(appsts, objs_copydir)

        runtime = round(time.time() - runstarttime, 2)
        print("Run application %s on %s, time cost %s seconds, passed: %s" %(appdir, app_runtarget, runtime, runstatus))
        sys.stdout.flush()
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
                if stoponfail == True:
                    cmdsts = appcmdsts
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
                    if stoponfail == True:
                        cmdsts = appcmdsts
                        print("Stop build apps with config due to fail on application %s" %(appdir))
                        return cmdsts, build_status
        return cmdsts, build_status

    def run_apps_with_config(self, config:dict, show_output=True, stoponfail=False):
        # Run all the applications, each application only has one configuration
        # "app" : { the_only_config }
        cmdsts = True
        # reset error counters
        self.reset_counters()
        build_status = dict()
        apps_config = copy.deepcopy(config)
        for appdir in apps_config:
            appconfig = apps_config[appdir]
            # Used to filter application with certain pattern not acceptable
            filtered, reason = filter_app_config(appconfig)
            if filtered:
                print("INFO: Ignore this app config %s for application %s" % (reason, appdir))
                continue
            applogs = appconfig.get("logs", dict())
            app_buildlogfile = applogs.get("build", None)
            app_runlogfile = applogs.get("run", None)
            appcmdsts, appsts = self.run_app_with_config(appdir, appconfig, show_output, app_buildlogfile, app_runlogfile)
            build_status[appdir] = appsts
            if appcmdsts == False:
                if stoponfail == True:
                    cmdsts = appcmdsts
                    print("ERROR: Stop run apps with config due to fail on application %s" %(appdir))
                    return cmdsts, build_status
            # error exit check
            if self.need_exit_now():
                print("ERROR: Need to exit now due to error counter exceed limit!")
                self.show_counters()
                cmdsts = False
                return cmdsts, build_status
        return cmdsts, build_status

    def run_apps_with_configs(self, config:dict, show_output=True, stoponfail=False):
        # Run all the applications, each application has more than one configuration
        # "app" : {"configs": {"case1": case1_config}}
        cmdsts = True
        # reset error counters
        self.reset_counters()
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
                # Used to filter application with certain pattern not acceptable
                filtered, reason = filter_app_config(appconfig)
                if filtered:
                    print("INFO: Ignore this app config %s for application %s" % (reason, appdir))
                    continue
                applogs = appconfig.get("logs", dict())
                app_buildlogfile = applogs.get("build", None)
                app_runlogfile = applogs.get("run", None)
                appcmdsts, appsts = self.run_app_with_config(appdir, appconfig, show_output, app_buildlogfile, app_runlogfile)
                build_status[appdir][cfgname] = appsts
                if appcmdsts == False:
                    if stoponfail == True:
                        cmdsts = appcmdsts
                        print("ERROR: Stop run apps with config due to fail on application %s" %(appdir))
                        return cmdsts, build_status
                # error exit check
                if self.need_exit_now():
                    print("ERROR: Need to exit now due to error counter exceed limit!")
                    self.show_counters()
                    cmdsts = False
                    return cmdsts, build_status
        return cmdsts, build_status
