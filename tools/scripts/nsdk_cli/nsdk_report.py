#!/usr/bin/env python3

import os
import sys
import time
import copy
import glob
import serial
import tempfile
import json
import argparse

MARKDOWN_PLUGIN=True
try:
    import markdown
except:
    print("python markdown plugin not installed, please install it using pip install markdown")
    MARKDOWN_PLUGIN=False


from prettytable import *

from nsdk_utils import *


def get_expected(config, app, cfg_name):
    if isinstance(config, dict) == False:
        return None
    found_app_expected = find_local_appconfig(app, config.get("expected", dict()))
    found_app_expecteds = find_local_appconfig(app, config.get("expecteds", dict()))
    if found_app_expected:
        app_expected = copy.deepcopy(config.get("expected", dict()).get(found_app_expected))
    else:
        app_expected = dict()
    if found_app_expecteds:
        app_cfgexpected = config.get("expecteds", dict()).get(found_app_expecteds).get(cfg_name, dict())
    else:
        app_cfgexpected = dict()
    app_expected = merge_two_config(app_expected, app_cfgexpected)
    return app_expected


def check_expected(build_status, config, run=False):
    if isinstance(build_status, dict) == False or isinstance(config, dict) == False:
        return False
    ret = True
    for app in build_status:
        app_allstatus = build_status[app]
        for cfgname in app_allstatus:
            app_status = app_allstatus[cfgname]["status"]
            build_ret = app_status.get("build", False)
            run_ret = app_status.get("run", False)
            app_cfg_expected = get_expected(config, app, cfgname)
            if isinstance(app_cfg_expected, dict):
                expected_build_ret = app_cfg_expected.get("build", True)
                expected_run_ret = app_cfg_expected.get("run", True)
                if build_ret == False and expected_build_ret != build_ret:
                    ret = False
                if run:
                    if run_ret == False and expected_run_ret != run_ret:
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

def analyze_report(config, result, runapp=False):
    apps_status = dict()
    passed_apps = dict()
    failed_apps = dict()
    build_cfgs = dict()

    glb_buildcfg = config.get("build_config", dict())
    # TODO currently this feature only cover cases that the application build_configs
    # just extend the global build_configs
    # get build configs used per cfgname
    if "build_configs" not in config:
        build_cfgs["default"] = config.get("build_config", dict())
    else:
        sub_configs = config["build_configs"]
        for cfgname in sub_configs:
            bcfg = copy.deepcopy(glb_buildcfg)
            bcfg.update(sub_configs[cfgname])
            build_cfgs[cfgname] = bcfg
    if "appconfig" in config:
        appcfgs = config.get("appconfig", dict())
        for app in appcfgs:
            if "build_configs" in appcfgs[app]:
                appsub_configs = appcfgs[app]["build_configs"]
                for cfgname in appsub_configs:
                    bcfg = copy.deepcopy(glb_buildcfg)
                    bcfg.update(appsub_configs[cfgname])
                    build_cfgs[cfgname] = bcfg

    def check_app_status(status, expected, runapp=False):
        app_sts = {"expected": True, "exp_build": True, "exp_run": True, "build": True, "run": True}
        percase_sts = {"expected": True, "exp_build": True, "exp_run": True, "build": True, "run": True}
        app_percase_sts = dict()
        for cfgname in status:
            app_percase_sts[cfgname] = copy.deepcopy(percase_sts)
            app_cfg_expected = get_expected(config, app, cfgname)
            expected_build = app_cfg_expected.get("build", True)
            expected_run = app_cfg_expected.get("run", True)
            real_build = status[cfgname]["status"].get("build", False)
            real_run = status[cfgname]["status"].get("run", False)
            if real_build == False and expected_build != real_build:
                app_sts["exp_build"] = False
                app_percase_sts[cfgname]["exp_build"] = False
            if real_run == False and expected_run != real_run:
                app_sts["exp_run"] = False
                app_percase_sts[cfgname]["exp_run"] = False
            if real_build == False:
                app_sts["build"] = False
            if real_run == False:
                app_sts["run"] = False
            # get per case expected
            app_percase_sts[cfgname]["expected"] = app_sts["exp_build"]
            if runapp:
                app_percase_sts[cfgname]["expected"] = app_percase_sts[cfgname]["exp_build"] and app_percase_sts[cfgname]["exp_run"]
        if runapp:
            app_sts["expected"] = app_sts["exp_build"] and app_sts["exp_run"]
        else:
            app_sts["expected"] = app_sts["exp_build"]

        analayzed_app_status = {"summary": app_sts, "percase": app_percase_sts}
        return analayzed_app_status

    apps_expected = config.get("expected", dict())


    apps_percase_status = dict()
    apps_percase_failed = dict()
    apps_percase_passed = dict()
    # Get app status compared with expected
    for app in result:
        app_expected = apps_expected.get(app, dict())
        app_status = result[app]
        analayzed_app_status = check_app_status(app_status, app_expected, runapp)
        apps_status[app] = analayzed_app_status["summary"]
        apps_percase_status[app] = analayzed_app_status["percase"]
        apps_percase_failed[app] = list()
        apps_percase_passed[app] = list()
        # per case for 1 app
        for case in analayzed_app_status["percase"]:
            if analayzed_app_status["percase"][case]["expected"] == False:
                apps_percase_failed[app].append(case)
            else:
                apps_percase_passed[app].append(case)
        # per app
        if apps_status[app]["expected"] == True:
            passed_apps[app] = copy.deepcopy(apps_status[app])
        else:
            failed_apps[app] = copy.deepcopy(apps_status[app])

    # Create report_dict
    report_dict = {"passed": passed_apps, "failed": failed_apps, "status": apps_status, "configs": build_cfgs, \
        "percase": {"status": apps_percase_status, "failed": apps_percase_failed, "passed": apps_percase_passed} }
    return report_dict

def generate_build_run_status_md(appresult, logdir, bold_false=True):
    if isinstance(appresult, dict) == False:
        if bold_false:
            return "**False**", "**False**", "-", "-"
        else:
            return "False", "False", "-", "-"
    else:
        appblog = appresult["logs"].get("build", None)
        apprlog = appresult["logs"].get("run", None)
        appbsts = appresult["status"].get("build", False)
        apprsts = appresult["status"].get("run", False)
        appbtm = appresult["time"].get("build", "-")
        apprtm = appresult["time"].get("run", "-")
        def gen_sts_md(sts, log, bold=True):
            if log:
                log = os.path.relpath(log, logdir)
                if bool(bold) ^ bool((not sts)):
                    sts_md = "[%s](%s)" % (sts, log)
                else:
                    sts_md = "[**%s**](%s)" % (sts, log)
            else:
                if bool(bold) ^ bool((not sts)):
                    sts_md = "%s" % (sts)
                else:
                    sts_md = "**%s**" % (sts)
            return sts_md
        bsts_md = gen_sts_md(appbsts, appblog, bold_false)
        rsts_md = gen_sts_md(apprsts, apprlog, bold_false)
        return bsts_md, rsts_md, appbtm, apprtm

def md2html(mdfile, htmlfile):
    if MARKDOWN_PLUGIN == False or os.path.isfile(mdfile) == False:
        return
    css_style = \
"""
<style>
table, th, td {
    border: 1px solid #1132e791 !important;
    border-collapse: collapse;
    padding: 3px;
    text-align: center;
}
td:first-child {
    text-align: left;
}
</style>
"""
    with open(mdfile) as mdf:
        mdtxt = mdf.read()
        mdhtml = markdown.markdown(mdtxt, extensions=["extra"])
        mdhtml = css_style + mdhtml
        with open(htmlfile, 'w') as htf:
            htf.write(mdhtml)

def generate_report(config, result, rptfile, rpthtml, logdir, runapp=False):
    if not(isinstance(config, dict) and isinstance(result, dict) and isinstance(rptfile, str)):
        return False
    report = analyze_report(config, result, runapp)
    rpt_passtxt = os.path.join(os.path.dirname(rptfile), "app_passed.txt")
    rpt_failtxt = os.path.join(os.path.dirname(rptfile), "app_failed.txt")
    # generate markdown file
    with open(rptfile, "w") as rf:
        rf.write("# Tested Nuclei SDK Applications/Test Cases\n\n")
        if len(report["passed"]) > 0:
            rf.write("\n## Passed\n\n")
            x = PrettyTable()
            x.set_style(MARKDOWN)
            x.field_names = ["App/Test Case", "All as Expected", "Build As Expected", "Run As Expected", "Build Status", "Run Status"]
            for app in report["passed"]:
                app_sts = report["passed"][app]
                x.add_row([app, app_sts["expected"], app_sts["exp_build"], app_sts["exp_run"], \
                    app_sts["build"], app_sts["run"]])
            rf.write(str(x))
            rf.write("\n")
        if len(report["failed"]) > 0:
            rf.write("\n## Failed\n\n")
            x = PrettyTable()
            x.set_style(MARKDOWN)
            x.field_names = ["App/Test Case", "All as Expected", "Build As Expected", "Run As Expected", "Build Status", "Run Status"]
            for app in report["failed"]:
                app_sts = report["failed"][app]
                x.add_row([app, app_sts["expected"], app_sts["exp_build"], app_sts["exp_run"], \
                    app_sts["build"], app_sts["run"]])
            rf.write(str(x))
            rf.write("\n")
        # Build configurations
        rf.write("\n# Build configurations\n\n")
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["Case Name", "Make Options"]
        for cfgname in report["configs"]:
            make_options = " ".join([ "%s=%s"%(key, value) for key, value in report["configs"][cfgname].items() ])
            x.add_row([cfgname, make_options])
        rf.write(str(x))
        rf.write("\n")
        # Build and run status
        rf.write("\n# Build and run status\n\n")
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["App/Test Case", "Case Name", "Build Status", "Run Status", "Build Time", \
                    "Run Time", "Type", "Value", "Total", "Text", "Data", "Bss"]
        apps_buildsts = result
        for app in apps_buildsts:
            app_sts = apps_buildsts[app]
            for cfgname in app_sts:
                size = app_sts[cfgname]["size"]
                apprsttype, apprstval = get_app_runresult(app_sts[cfgname].get("result", dict()))
                bsts_md, rsts_md, appbtm, apprtm = generate_build_run_status_md(app_sts[cfgname], logdir, True)
                x.add_row([app, cfgname, bsts_md, rsts_md, appbtm, apprtm, apprsttype, apprstval, \
                    size["total"], size["text"], size["data"], size["bss"]])
        rf.write(str(x))
        rf.write("\n")
        # Real expected pass or fail cases
        percase_status = report["percase"]
        rf.write("\n# Passed Cases(as expected) Per Applications\n\n")
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["App", "Passed Cases"]
        with open(rpt_passtxt, "w") as rpt_pf:
            for app in percase_status["passed"]:
                tmptxt = ", ".join(percase_status["passed"][app])
                if (len(tmptxt) > 0):
                    rpt_pf.write("- %s : %s\n" % (app, tmptxt))
                x.add_row([app, tmptxt])
        rf.write(str(x))
        rf.write("\n")
        rf.write("\n# Failed Cases(as expected) Per Applications\n\n")
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["App", "Failed Cases"]
        with open(rpt_failtxt, "w") as rpt_ff:
            for app in percase_status["failed"]:
                tmptxt = ", ".join(percase_status["failed"][app])
                if (len(tmptxt) > 0):
                    rpt_ff.write("- %s : %s\n" % (app, tmptxt))
                x.add_row([app, tmptxt])
        rf.write(str(x))
        rf.write("\n")
        # expected build or run failed cases
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["App/Test Case", "Case Name", "Expected Build", "Expected Run"]
        apps_buildsts = result
        with_expect = False
        for app in apps_buildsts:
            app_sts = apps_buildsts[app]
            for cfgname in app_sts:
                app_cfg_expected = get_expected(config, app, cfgname)
                expected_build = app_cfg_expected.get("build", True)
                expected_run = app_cfg_expected.get("run", True)
                if expected_build == False or expected_run == False:
                    with_expect = True
                    x.add_row([app, cfgname, expected_build, expected_run])
        if with_expect:
            rf.write("\n# Expected Build or Run Failed Cases\n\n")
            rf.write(str(x))
            rf.write("\n")
    # generate html from markdown
    md2html(rptfile, rpthtml)
    pass

# check whether the result json is generate by nsdk_bench.py
def is_bench_result(result):
    if isinstance(result, dict) == False:
        return False

    check = True
    try:
        for app in result:
            for cfgname in result[app]:
                if "app" not in result[app][cfgname]:
                    check = False
                    break
                if check == False:
                    break
    except:
        check = False

    return check


def merge_split_config_and_result(logdir):
    mergedcfg_files = find_files(logdir, "**/mergedcfg.json", True)
    all_mergedcfg = dict()
    all_result = dict()
    print("Start to merge config and result json files in %s" % (logdir))
    for mergedcfg_file in mergedcfg_files:
        configfile = mergedcfg_file
        resultdir = os.path.dirname(mergedcfg_file)
        reldir = os.path.relpath(resultdir, logdir)
        resultfile = os.path.join(resultdir, "result.json")
        if os.path.isfile(resultfile) == True:
            _, config = load_json(configfile)
            _, result = load_json(resultfile)
            if is_bench_result(result):
                for app in result:
                    for cfg in result[app]:
                        app_cfg = os.path.join(reldir, cfg)
                        if app not in all_result:
                            all_result[app] = dict()
                        all_result[app][app_cfg] = result[app][cfg]
                all_mergedcfg = merge_two_config(all_mergedcfg, config)
                #all_result = merge_two_config(all_result, result)
            else:
                print("result json file %s is not generated by nsdk_bench.py" % (resultfile))

    return all_mergedcfg, all_result

def merge_all_config_and_result(logdir):
    mergedcfg_files = find_files(logdir, "**/mergedcfg.json", True)
    all_mergedcfg = dict()
    all_result = dict()
    print("Start to merge split config and result json files in %s" % (logdir))
    for mergedcfg_file in mergedcfg_files:
        configfile = mergedcfg_file
        resultdir = os.path.dirname(mergedcfg_file)
        resultfile = os.path.join(resultdir, "result.json")
        if os.path.isfile(resultfile) == True:
            _, config = load_json(configfile)
            _, result = load_json(resultfile)
            if is_bench_result(result):
                print("Merging config json file %s, result json file %s" %(configfile, resultfile))
                all_mergedcfg = merge_two_config(all_mergedcfg, config)
                all_result = merge_two_config(all_result, result)
            else:
                print("result json file %s is not generated by nsdk_bench.py" % (resultfile))

    return all_mergedcfg, all_result

def generate_report_for_logs(logdir, run=False, split=False):
    if logdir and os.path.isdir(logdir):
        reportfile =  os.path.join(logdir, "report.md")
        reporthtml =  os.path.join(logdir, "report.html")

        if split == False:
            all_mergedcfg, all_result = merge_all_config_and_result(logdir)
        else:
            all_mergedcfg, all_result = merge_split_config_and_result(logdir)
        if all_mergedcfg and all_result:
            config_file = os.path.join(logdir, "allcfg.json")
            result_file = os.path.join(logdir, "allrst.json")
            print("Save all merged config file to %s" % (config_file))
            print("Save all result file to %s" % (result_file))
            save_json(config_file, all_mergedcfg)
            save_json(result_file, all_result)
            print("Save generated report markdown file to %s" % (reportfile))
            if MARKDOWN_PLUGIN:
                print("Save generated report html file to %s" % (reporthtml))
            generate_report(all_mergedcfg, all_result, reportfile, reporthtml, logdir, run)
        else:
            print("Can't find any valid reports in %s generated by nsdk_bench.py" % (logdir))
    pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK Bench report Generate Tools")
    parser.add_argument('--logdir', required=True, help="logs directory where saved the report json files")
    parser.add_argument('--split', action='store_true', help="Split for different configurations")
    parser.add_argument('--run', action='store_true', help="If specified, it means this is a runner report")

    args = parser.parse_args()

    if os.path.isdir(args.logdir) == False:
        print("The log directory doesn't exist, please check!")
        sys.exit(1)

    generate_report_for_logs(args.logdir, args.run, args.split)
