#!/usr/bin/env python3

import os
import sys
import time
import copy
import glob
import tempfile
import json
import argparse

from nsdk_builder import *
from nsdk_utils import *

class nsdk_executor(nsdk_runner):
    def __init__(self):
        super().__init__()
        pass

    def build_apps(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        if isinstance(config, dict) == False:
            return False, None
        global_build_config = config.get("build_config", dict())
        global_target = config.get("build_target", "all")
        global_cpobjs = config.get("copy_objects", None)
        global_parallel = config.get("parallel", "")
        rootdirs = config.get("appdirs", [])
        ignored_rootdirs = config.get("appdirs_ignore", [])
        if (isinstance(rootdirs, list) and isinstance(ignored_rootdirs, list)) == False:
            print("appdirs type in config should be list, please check!")
            return False, None
        if len(rootdirs) == 0:
            print("No appdirs specified in config, please check!")
            return False, None
        root_appdirs = dict()
        appnum = 0
        # Find all the apps in rootdirs
        for rootdir in rootdirs:
            root_appdirs[rootdir] = self.find_apps(rootdir)
            appnum += len(root_appdirs[rootdir])
        # Find all the ignored apps in ignored_rootdirs
        ignored_apps = []
        for ig_rootdir in ignored_rootdirs:
            ignored_apps.extend(self.find_apps(ig_rootdir))
        if appnum == 0:
            print("No application found in appdirs specified in config")
            return False, None
        createlog = False
        if isinstance(logdir, str):
            createlog = True
            if os.path.isdir(logdir) == False:
                os.makedirs(logdir)
        appconfigs = config.get("appconfig", dict())

        # Construct all the applications' configuration
        apps_config = dict()
        for rootdir in root_appdirs:
            for appdir in root_appdirs[rootdir]:
                if appdir in ignored_apps:
                    print("%s is ignored by appdirs_ignore section" % (appdir))
                    continue
                appdir = appdir.replace("\\", "/") # Change windows \\ path to /
                applogfile = None
                if createlog:
                    applogfile = get_logfile(appdir, rootdir, logdir, "build.log")
                app_buildcfg = copy.deepcopy(global_build_config)
                app_buildtarget = copy.deepcopy(global_target)
                app_cpobjs = copy.deepcopy(global_cpobjs)
                app_parallel = copy.deepcopy(global_parallel)
                found_cfg = find_local_appconfig(appdir, appconfigs)
                if found_cfg:
                    appcfg = appconfigs[found_cfg]
                    # merge_global is True if not present
                    merge_global = appcfg.get("merge_global", True)
                    # Merge global config when merge_global is True
                    if merge_global:
                        app_buildcfg.update(appcfg.get("build_config", dict()))
                        if "build_target" in appcfg:
                            app_buildtarget = appcfg["build_target"]
                        if "parallel" in appcfg:
                            app_parallel = appcfg["parallel"]
                        if "copy_objects" in appcfg:
                            app_cpobjs = appcfg["copy_objects"]
                    else: # if merge_global is false, then use app config only
                        app_buildcfg = appcfg.get("build_config", dict())
                        app_buildtarget = appcfg.get("build_target", "all")
                        app_parallel = appcfg.get("parallel", "")
                        app_cpobjs = appcfg.get("copy_objects", None)

                appconfig = {"build_config": app_buildcfg, "build_target": app_buildtarget, \
                            "parallel": app_parallel, "logs": {"build": applogfile}}
                if app_cpobjs is not None:
                    appconfig["copy_objects"] = app_cpobjs

                apps_config[appdir] = copy.deepcopy(appconfig)

        if len(apps_config) == 0:
            print("No applications need to be run according to the configuration")
            return False, None
        # Build all the applications
        print("Build %d applications defined by configuration" % (len(apps_config)))
        cmdsts, build_status = self.build_apps_with_config(apps_config, show_output, stoponfail)
        return cmdsts, build_status

    def run_apps(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        if isinstance(config, dict) == False:
            return False, None
        global_build_config = config.get("build_config", dict())
        global_target = config.get("build_target", "clean all")
        global_cpobjs = config.get("copy_objects", None)
        global_parallel = config.get("parallel", "")
        global_run_config = config.get("run_config", dict())
        global_checks = config.get("checks", dict())
        rootdirs = config.get("appdirs", [])
        ignored_rootdirs = config.get("appdirs_ignore", [])
        if (isinstance(rootdirs, list) and isinstance(ignored_rootdirs, list)) == False:
            print("appdirs type in config should be list, please check!")
            return False, None
        if len(rootdirs) == 0:
            print("No appdirs specified in config, please check!")
            return False, None
        root_appdirs = dict()
        appnum = 0
        # Find all the apps in rootdirs
        for rootdir in rootdirs:
            root_appdirs[rootdir] = self.find_apps(rootdir)
            appnum += len(root_appdirs[rootdir])
        # Find all the ignored apps in ignored_rootdirs
        ignored_apps = []
        for ig_rootdir in ignored_rootdirs:
            ignored_apps.extend(self.find_apps(ig_rootdir))
        if appnum == 0:
            print("No application found in appdirs specified in config")
            return False, None
        createlog = False
        if isinstance(logdir, str):
            createlog = True
            if os.path.isdir(logdir) == False:
                os.makedirs(logdir)
        appconfigs = config.get("appconfig", dict())

        # Construct all the applications' configuration
        apps_config = dict()
        for rootdir in root_appdirs:
            for appdir in root_appdirs[rootdir]:
                if appdir in ignored_apps:
                    print("%s is ignored according to appdirs_ignore settings in configuration" % (appdir))
                    continue
                appdir = appdir.replace("\\", "/") # Change windows \\ path to /
                app_buildlogfile = None
                app_runlogfile = None
                if createlog:
                    app_buildlogfile = get_logfile(appdir, rootdir, logdir, "build.log")
                    app_runlogfile = get_logfile(appdir, rootdir, logdir, "run.log")
                app_buildcfg = copy.deepcopy(global_build_config)
                app_buildtarget = copy.deepcopy(global_target)
                app_parallel = copy.deepcopy(global_parallel)
                app_cpobjs = copy.deepcopy(global_cpobjs)
                app_runcfg = copy.deepcopy(global_run_config)
                app_checks = copy.deepcopy(global_checks)
                found_cfg = find_local_appconfig(appdir, appconfigs)
                if found_cfg:
                    appcfg = appconfigs[found_cfg]
                    # merge_global is True if not present
                    merge_global = appcfg.get("merge_global", True)
                    # Merge global config when merge_global is True
                    if merge_global:
                        app_buildcfg.update(appcfg.get("build_config", dict()))
                        app_runcfg.update(appcfg.get("run_config", dict()))
                        app_checks.update(appcfg.get("checks", dict()))
                        if "build_target" in appcfg:
                            app_buildtarget = appcfg["build_target"]
                        if "parallel" in appcfg:
                            app_parallel = appcfg["parallel"]
                        if "copy_objects" in appcfg:
                            app_cpobjs = appcfg["copy_objects"]
                    else: # if merge_global is false, then use app config only
                        app_buildcfg = appcfg.get("build_config", dict())
                        app_buildtarget = appcfg.get("build_target", "clean all")
                        app_parallel = appcfg.get("parallel", "")
                        app_runcfg = appcfg.get("run_config", dict())
                        app_checks = appcfg.get("checks", dict())
                        app_cpobjs = appcfg.get("copy_objects", None)

                appconfig = {"build_config": app_buildcfg, "build_target": app_buildtarget, \
                             "parallel": app_parallel, "run_config": app_runcfg, "checks": app_checks, \
                            "logs": {"build": app_buildlogfile, "run": app_runlogfile}}
                if app_cpobjs is not None:
                    appconfig["copy_objects"] = app_cpobjs
                apps_config[appdir] = copy.deepcopy(appconfig)

        if len(apps_config) == 0:
            print("No applications need to be run according to the configuration")
            return False, None
        print("Run %d applications defined by configuration" % (len(apps_config)))
        # Run all the applications
        cmdsts, build_status = self.run_apps_with_config(apps_config, show_output, stoponfail)
        return cmdsts, build_status

def merge_config(appcfg, hwcfg):
    return merge_two_config(appcfg, hwcfg)

def merge_cmd_config(config, args_dict):
    return merge_config_with_args(config, args_dict)

def check_expected(build_status, expected, run=False):
    if isinstance(build_status, dict) == False:
        return False
    ret = True
    for app in build_status:
        app_status = build_status[app]["status"]
        build_ret = app_status.get("build", False)
        run_ret = app_status.get("run", False)
        if isinstance(expected, dict) and app in expected:
            expected_build_ret = expected[app].get("build", True)
            expected_run_ret = expected[app].get("run", True)
            if expected_build_ret != build_ret:
                ret = False
            if run:
                if expected_run_ret != run_ret:
                    ret = False
        else:
            if build_ret == False:
                ret = False
            if run:
                if run_ret == False:
                    ret = False
    return ret

def save_results(appcfg, hwcfg, mergedcfg, result, savedir):
    if not (isinstance(savedir, str) and os.path.isdir(savedir)):
        return
    if isinstance(appcfg, dict):
        sfn = os.path.join(savedir, "appcfg.json")
        save_json(sfn, appcfg)
    if isinstance(hwcfg, dict):
        sfn = os.path.join(savedir, "hwcfg.json")
        save_json(sfn, hwcfg)
    if isinstance(mergedcfg, dict):
        sfn = os.path.join(savedir, "mergedcfg.json")
        save_json(sfn, mergedcfg)
    if isinstance(result, dict):
        sfn = os.path.join(savedir, "result.json")
        save_json(sfn, result)
    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Executor Tool")
    parser.add_argument('--appcfg', required=True, help="Application JSON Configuration File")
    parser.add_argument('--hwcfg', help="Hardware Target JSON Configuration File, if specified, will overwrite configuration defined in appcfg")
    parser.add_argument('--logdir', default='logs', help="logs directory, default logs")
    parser.add_argument('--serport', help="Serial port for monitor, if not specified, it will use the specified by appcfg and hwcfg")
    parser.add_argument('--baudrate', help="Serial port baudrate for monitor, it will use the specified by appcfg and hwcfg")
    parser.add_argument('--make_options', help="Extra make options passed to overwrite default build configuration passed via appcfg and hwcfg")
    parser.add_argument('--build_target', help="Build target passed to make, to overwrite default build_target defined in configuration file")
    parser.add_argument('--run_target', help="Run target which program will run, such as hardware, qemu or xlspike")
    parser.add_argument('--parallel', help="parallel value, such as -j4 or -j or -j8, default None")
    parser.add_argument('--run', action='store_true', help="If specified, will do run not build process")
    parser.add_argument('--verbose', action='store_true', help="If specified, will show detailed build/run messsage")
    parser.add_argument('--uniqueid', default="", help="Pass pipeline$CI_PIPELINE_ID, such as pipeline123456")
    args = parser.parse_args()

    # Load appcfg and hwcfg
    ret, appcfg = load_json(args.appcfg)
    if ret != JSON_OK:
        print("Please provide valid json config file")
        sys.exit(1)

    # record start time
    start_time = time.time()
    ret, hwcfg = load_json(args.hwcfg)
    # Merge appcfg and hwcfg, hwcfg has higher priority
    config = merge_config(appcfg, hwcfg)
    # Merge options passed by serport, baudrate, make_options
    config = merge_cmd_config(config, vars(args))

    # Set global variables from config file
    set_global_variables(config)

    nsdk_ext = nsdk_executor()
    if args.run:
        cmdsts, result = nsdk_ext.run_apps(config, args.verbose, args.logdir, False)
    else:
        cmdsts, result = nsdk_ext.build_apps(config, args.verbose, args.logdir, False)
    runtime = round(time.time() - start_time, 2)
    print("Applications specified in %s build or run status: %s, time cost %s seconds" % (args.appcfg, cmdsts, runtime))
    expected = config.get("expected", None)
    ret = check_expected(result, expected, args.run)
    print("Applications build or run as expected: %s" % (ret))
    save_results(appcfg, hwcfg, config, result, args.logdir)
    if result:
        csvfile = os.path.join(args.logdir, "result.csv")
        save_execute_csv(result, csvfile)
        print("Generate report csv file to %s" % (csvfile))
    # Exit with ret value
    if ret and cmdsts:
        sys.exit(0)
    else:
        sys.exit(1)
