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
from nsdk_report import *

class nsdk_bench(nsdk_runner):
    def __init__(self):
        super().__init__()
        pass

    def build_apps(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        if isinstance(config, dict) == False:
            return False, None
        global_build_config = config.get("build_config", dict())
        global_build_configs = config.get("build_configs", dict())
        global_target = config.get("build_target", "all")
        global_parallel = config.get("parallel", "")
        global_cpobjs = config.get("copy_objects", False)
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

                app_buildcfg = copy.deepcopy(global_build_config)
                app_buildcfgs = copy.deepcopy(global_build_configs)
                app_buildtarget = copy.deepcopy(global_target)
                app_parallel = copy.deepcopy(global_parallel)
                app_cpobjs = copy.deepcopy(global_cpobjs)
                found_cfg = find_local_appconfig(appdir, appconfigs)
                if found_cfg:
                    appcfg = appconfigs[found_cfg]
                    # merge_global is True if not present
                    merge_global = appcfg.get("merge_global", True)
                    # Merge global config when merge_global is True
                    if merge_global:
                        app_buildcfg.update(appcfg.get("build_config", dict()))
                        app_buildcfgs.update(appcfg.get("build_configs", dict()))
                        if "build_target" in appcfg:
                            app_buildtarget = appcfg["build_target"]
                        if "parallel" in appcfg:
                            app_parallel = appcfg["parallel"]
                        if "copy_objects" in appcfg:
                            app_cpobjs = appcfg["copy_objects"]
                    else: # if merge_global is false, then use app config only
                        app_buildcfg = appcfg.get("build_config", dict())
                        app_buildcfgs = appcfg.get("build_configs", dict())
                        app_buildtarget = appcfg.get("build_target", "all")
                        app_parallel = appcfg.get("parallel", "")
                        app_cpobjs = appcfg.get("copy_objects", False)

                app_allconfigs = {"configs": {}}
                if len(app_buildcfgs) == 0:
                    app_buildcfgs = { "default": {} }
                for cfgname in app_buildcfgs:
                    if createlog:
                        buildlog_name = os.path.join(cfgname, "build.log")
                        applogfile = get_logfile(appdir, rootdir, logdir, buildlog_name)
                    else:
                        applogfile = None
                    percfg_appbuildcfg = copy.deepcopy(app_buildcfg)
                    percfg_appbuildcfg.update(app_buildcfgs[cfgname])
                    app_allconfigs["configs"][cfgname] = {"build_config": percfg_appbuildcfg, "build_target": app_buildtarget, \
                            "copy_objects": app_cpobjs, "parallel": app_parallel, "logs": {"build": applogfile}}

                apps_config[appdir] = copy.deepcopy(app_allconfigs)

        if len(apps_config) == 0:
            print("No applications need to be run according to the configuration")
            return False, None
        # Build all the applications
        print("Build %d applications defined by configuration" % (len(apps_config)))
        cmdsts, build_status = self.build_apps_with_configs(apps_config, show_output, stoponfail)
        return cmdsts, build_status

    def run_apps(self, config:dict, show_output=True, logdir=None, stoponfail=False):
        if isinstance(config, dict) == False:
            return False, None
        global_build_config = config.get("build_config", dict())
        global_build_configs = config.get("build_configs", dict())
        global_target = config.get("build_target", "clean all")
        global_parallel = config.get("parallel", "")
        global_cpobjs = config.get("copy_objects", False)
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

                app_buildcfg = copy.deepcopy(global_build_config)
                app_buildcfgs = copy.deepcopy(global_build_configs)
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
                        app_buildcfgs.update(appcfg.get("build_configs", dict()))
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
                        app_buildcfgs = appcfg.get("build_configs", dict())
                        app_buildtarget = appcfg.get("build_target", "clean all")
                        app_parallel = appcfg.get("parallel", "")
                        app_runcfg = appcfg.get("run_config", dict())
                        app_checks = appcfg.get("checks", dict())
                        app_cpobjs = appcfg.get("copy_objects", False)

                app_allconfigs = {"configs": {}}
                if len(app_buildcfgs) == 0:
                    app_buildcfgs = { "default": {} }
                for cfgname in app_buildcfgs:
                    if createlog:
                        buildlog_name = os.path.join(cfgname, "build.log")
                        runlog_name = os.path.join(cfgname, "run.log")
                        app_buildlogfile = get_logfile(appdir, rootdir, logdir, buildlog_name)
                        app_runlogfile = get_logfile(appdir, rootdir, logdir, runlog_name)
                    else:
                        app_buildlogfile = None
                        app_runlogfile = None
                    percfg_appbuildcfg = copy.deepcopy(app_buildcfg)
                    percfg_appbuildcfg.update(app_buildcfgs[cfgname])
                    app_allconfigs["configs"][cfgname] = {"build_config": percfg_appbuildcfg, "build_target": app_buildtarget, \
                            "copy_objects": app_cpobjs, "parallel": app_parallel, "run_config": app_runcfg, "checks": app_checks, \
                            "logs": {"build": app_buildlogfile, "run": app_runlogfile}}

                apps_config[appdir] = copy.deepcopy(app_allconfigs)

        if len(apps_config) == 0:
            print("No applications need to be run according to the configuration")
            return False, None
        print("Run %d applications defined by configuration" % (len(apps_config)))
        # Run all the applications
        cmdsts, build_status = self.run_apps_with_configs(apps_config, show_output, stoponfail)
        return cmdsts, build_status


def merge_config(appcfg, hwcfg):
    return merge_two_config(appcfg, hwcfg)

def merge_cmd_config(config, args_dict):
    return merge_config_with_args(config, args_dict)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Benchmark and Report Tool")
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
    parser.add_argument('--ncycm',  help="If specified, will use cycle model specified here")
    parser.add_argument("--timeout", help="If specified, will use timeout value specified here")
    parser.add_argument('--verbose', action='store_true', help="If specified, will show detailed build/run messsage")
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
    if args.ncycm and os.path.isfile(args.ncycm) == False:
        print("ERROR: cycle model %s doesn't exist, please check!" % (args.ncycm))
        sys.exit(0)
    # Merge options passed by serport, baudrate, make_options
    config = merge_cmd_config(config, vars(args))

    set_global_variables(config)

    nsdk_ext = nsdk_bench()
    if args.run:
        cmdsts, result = nsdk_ext.run_apps(config, args.verbose, args.logdir, False)
    else:
        cmdsts, result = nsdk_ext.build_apps(config, args.verbose, args.logdir, False)

    runtime = round(time.time() - start_time, 2)
    print("Applications specified in %s build or run status: %s, time cost %s seconds" % (args.appcfg, cmdsts, runtime))
    ret = check_expected(result, config, args.run)
    print("Application build as expected: %s" % (ret))
    save_results(appcfg, hwcfg, config, result, args.logdir)

    if result:
        # Generate build or run report
        save_report_files(args.logdir, config, result, args.run)
    print("This bench cost about %s seconds" % (runtime))
    # Exit with ret value
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)
