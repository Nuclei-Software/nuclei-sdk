#!/usr/bin/env python3

import os
import sys
import time
import serial
import json
import argparse
from threading import Thread

class NThread(Thread):
    def __init__(self, func, args):
        super(NThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        self.result = self.func(*self.args)

    def get_result(self):
        try:
            return self.result
        except Exception:
            return None

def get_make_csv(app, config):
    make_options = " "
    SUPPORT_KEYS = ["DOWNLOAD", "CORE", "BENCH_UNIT", "DSP_ENABLE", "SOC", "BOARD", "SILENT", "V"]
    csv_print = "CSV, APP=%s" % (app)
    if config:
        for key in config:
            if key not in SUPPORT_KEYS:
                continue
            option = "%s=%s"%(key, config[key])
            make_options = " %s %s " % (make_options, option)
            csv_print = "%s, %s" % (csv_print, option)
    return make_options, csv_print

def nsdk_run_app(app, make_options="", target="clean"):
    if os.path.isdir(app) == False:
        return False
    run_cmd = "make -C %s %s %s" % (app, make_options, target)
    print("NSDK Run application %s using %s" % (app, run_cmd))
    if os.system(run_cmd) == 0:
        return True
    else:
        return False

def upload_sdk_app(app, make_options=""):
    if os.path.isdir(app) == False:
        return None
    clean_cmd = "make -C %s %s clean" % (app, make_options)
    run_cmd = "make -C %s %s upload" % (app, make_options)
    print("Clean application %s using %s" % (app, clean_cmd))
    os.system(clean_cmd)
    print("Run application %s using %s" % (app, run_cmd))
    #print(csv_print)
    os.system(run_cmd)
    return None

def read_serial(port="/dev/ttyUSB1", baudrate=115200, timeout=60, checks=None, sdk_check=False, checktime=time.time()):
    start_time = time.time()
    bench_log = ""
    bench_status = False
    pass_checks = checks.get("PASS", []) if checks else []
    fail_checks = checks.get("FAIL", []) if checks else []
    def test_in_check(string, checks):
        if type(checks) == list:
            for check in checks:
                if check in string:
                    return True
        return False

    print("Read serial log from %s, baudrate %s" %(port, baudrate))
    nsdk_check_tag = "Nuclei SDK Build Time:"
    try:
        ser = serial.Serial(port, baudrate, timeout=5)
        while (time.time() - start_time) < timeout:
            # Remove '\r' in serial read line
            sline = ser.readline()
            line = ""
            try:
                line = str(sline.decode()).replace('\r', '')
            except:
                pass
            if sdk_check== True:
                print("XXX Check " + line)
                if nsdk_check_tag in line:
                    timestr = line.split(nsdk_check_tag)[-1].strip()
                    cur_time = time.mktime(time.strptime(timestr, "%b %d %Y, %H:%M:%S"))
                    if int(cur_time) >= int(checktime):
                        sdk_check = False
                        bench_log = bench_log + str(line)
            else:
                bench_log = bench_log + str(line)
                if test_in_check(line, fail_checks):
                    bench_status = False
                    break
                if test_in_check(line, pass_checks):
                    bench_status = True
                    break
    except serial.serialutil.SerialException:
        # https://stackoverflow.com/questions/21050671/how-to-check-if-device-is-connected-pyserial
        print("serial port %s might not exist or in use" % port)
    else:
        print("Some error happens during serial operations")
    finally:
        ser.close()

    return bench_status, bench_log

def run_sdk_app(app, config=None, logfile=None, dry_run=False):
    if os.path.isdir(app) == False:
        return False, ""
    if config is None:
        config = dict()
    port = config.get("port", "/dev/ttyUSB1")
    baudrate = config.get("baudrate", 115200)
    timeout = config.get("timeout", 60)
    checks = config.get("checks", dict())
    pass_checks = checks.get("PASS", [])
    fail_checks = checks.get("FAIL", [])
    checks = {"PASS": pass_checks, "FAIL": fail_checks}
    appschecks = config.get("appchecks", None)

    if appschecks and app in appschecks:
        print("Add application check for %s" % app)
        appchecks = appschecks[app]
        pass_checks = appchecks.get("PASS", [])
        fail_checks = appchecks.get("FAIL", [])
        config["appchecks"][app] = {"PASS": pass_checks, "FAIL": fail_checks}
        if checks:
            pass_checks.extend(checks.get("PASS", []))
            fail_checks.extend(checks.get("FAIL", []))
        checks = dict()
        checks["PASS"] = pass_checks
        checks["FAIL"] = fail_checks
    else:
        if "appchecks" not in config:
            config["appchecks"] = dict()
        config["appchecks"][app] = {"PASS": [], "FAIL": []}

    print("Here are pass checks: %s" % checks["PASS"])
    print("Here are fail checks: %s" % checks["FAIL"])

    make_options, csv_print = get_make_csv(app, config)

    if dry_run:
        print("Dry run for application %s" % app)
        return True, ""
    # record checktime before build application
    checktime = time.time()
    if nsdk_run_app(app, make_options, "clean") == False:
        return False, ""

    if nsdk_run_app(app, make_options, "all") == False:
        return False, ""

    ser_thread = NThread(read_serial, (port, baudrate, timeout, checks, True, checktime))
    ser_thread.start()
    nsdk_run_app(app, make_options, "upload")
    ser_thread.join()
    status, log = ser_thread.get_result()
    del ser_thread
    log = csv_print + "\n" + log

    print(log)
    if logfile:
        logdir = os.path.dirname(logfile)
        if os.path.isdir(logdir) == False:
            os.makedirs(logdir)
        print("Record serial log to %s" % (logfile))
        with open(logfile, 'w') as lf:
            lf.write(log)
    return status, log

def find_file(path, filename):
    finded = []
    for root, dirs, files in os.walk(path):
        for fln in files:
            if fln.lower() == filename.lower():
                finded.append(root)
                break
    return finded

def run_and_parse_apps(appdir, config=None, logname="runapps", logdir="logs", dry_run=False):
    nsdk_apps = find_file(appdir, "Makefile")
    full_log = ""
    fail_list = []
    pass_list = []
    if os.path.isdir(logdir) == False:
        os.makedirs(logdir)

    appslist = config.get("applist", None)

    for app in nsdk_apps:
        # Force to change windows path to linux path, \ -> /
        app = app.replace("\\", "/")
        if appslist and app not in appslist:
            print("Ignore application %s, which is not defined in applist" % app)
            continue
        print("Build, upload and run application %s" % (app))
        appname = app
        if appname == ".":
            appname = os.path.basename(os.path.relpath(appname))
        applog = appname.replace('/', '_').replace('\\','_') + ".log"
        applog = os.path.join(logdir, applog)
        status, log = run_sdk_app(app, config, applog, dry_run)
        if status:
            full_log = full_log + log
            pass_list.append(app)
        else:
            fail_list.append(app)

    if config:
        if appslist is None:
            config["applist"] = nsdk_apps
        cfg_file = os.path.join(logdir, "cfg.json")
        with open(cfg_file, "w") as cf:
            json.dump(config, cf, indent=4)
    if pass_list:
        print("The following applications are passed:")
    for ps in pass_list:
        print(ps)
    if fail_list:
        print("The following applications are failed:")
    for fail in fail_list:
        print(fail)

    flog_name = os.path.join(logdir, logname + ".log")
    flog_csv = os.path.join(logdir, logname + ".csv")
    run_log = os.path.join(logdir, "runner_report" + ".log")

    with open(flog_name, 'w') as lf:
        lf.write(full_log)
    log_lines = full_log.split("\n")

    with open(flog_csv, 'w') as cf:
        for log_line in log_lines:
            log_line = log_line.strip()
            if "CSV" in log_line:
                cf.write(log_line + "\n")

    with open(run_log, 'w') as rf:
        rf.write("Passed applications as below\n")
        for ps in pass_list:
            rf.write(ps+'\n')
        rf.write("Failed applications as below\n")
        for fl in fail_list:
            rf.write(fl+'\n')

    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Runner")

    parser.add_argument('-a', '--appdir', default="application", help="Root application directories, default application")
    parser.add_argument('-c', '--core', default="n205", help="Benchmark Core, default n205")
    parser.add_argument('-d', '--download', default='ilm', help="Download mode, default ilm")
    parser.add_argument('-p', '--dsp_enable', default='ON', help="DSP_ENABLE ON or OFF, default ON")
    parser.add_argument('-b', '--bench_unit', default='cycles', help="BENCH_UNIT, cycles or instret, default cycles")
    parser.add_argument('-j', '--jsonconfig', default=None, help="JSON Configuration File, default no configuration")
    parser.add_argument('-l', '--logdir', default='logs', help="logs directory, default logs")
    parser.add_argument('--port', help="Serial port for monitor")
    parser.add_argument('--baudrate', help="Serial port baudrate for monitor")
    parser.add_argument('--dry_run', action="store_true", help="Dry run, will generate a empty json configuration file in <logdir>")
    args = parser.parse_args()

    checks =  {"PASS":["all test are passed"], "FAIL":["MEPC:", "test error apprears"]}

    config = {
        "CORE": args.core.lower(),
        "DOWNLOAD": args.download.lower(),
        "DSP_ENABLE": args.dsp_enable.upper(),
        "BENCH_UNIT": args.bench_unit.lower(),
        "checks": checks
    }
    if args.jsonconfig and os.path.isfile(args.jsonconfig):
        jsoncfg = json.load(open(args.jsonconfig, 'r'))
        if "checks" in jsoncfg:
            config["checks"] = jsoncfg["checks"]
        jsoncfg.update(config)
        config = jsoncfg
    if args.port:
        config["port"] = args.port
    if args.baudrate:
        config["baudrate"] = args.baudrate

    app_names = [os.path.basename(os.path.relpath(args.appdir)), args.core.lower(), args.download.lower(), \
            args.dsp_enable.lower(), args.bench_unit.lower()]
    applog = "_".join(app_names)
    print("Log file name prefix is " + applog)
    run_and_parse_apps(args.appdir, config, applog, args.logdir, args.dry_run)

