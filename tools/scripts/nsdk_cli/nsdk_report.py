#!/usr/bin/env python3

import os
import sys
import time
import copy
import glob
import tempfile
import argparse

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))

MARKDOWN_PLUGIN=True
try:
    import serial
    import json
    import markdown
    import pyexcel as pe
    from prettytable import *
except Exception as exc:
    MARKDOWN_PLUGIN=False
    print("Import Error: %s" % (exc))
    print("Please install requried packages using: pip3 install -r %s" % (requirement_file))
    sys.exit(1)

from nsdk_utils import *

def get_expected_build(expcfg):
    if isinstance(expcfg, dict) == False:
        return False
    return expcfg.get("build", True)

def get_expected_run(expcfg, build=None):
    if isinstance(expcfg, dict) == False:
        return False
    if build is None:
        build = get_expected_build(expcfg)
    # if expected build is false, expected run should be false
    if build == False:
        return False
    return expcfg.get("run", True)

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
        allcfgs_expected = config.get("expecteds", dict()).get(found_app_expecteds)
        # find expecteds config match key startwith
        if allcfgs_expected is not None and len(allcfgs_expected) > 0:
            if cfg_name not in allcfgs_expected:
                app_cfgexpected = dict()
                for key in allcfgs_expected:
                    if cfg_name.startswith(key):
                        app_cfgexpected = allcfgs_expected[key]
                        break
            else:
                app_cfgexpected = allcfgs_expected.get(cfg_name, dict())
        else:
            app_cfgexpected = dict()
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
                expected_build_ret = get_expected_build(app_cfg_expected)
                expected_run_ret = get_expected_run(app_cfg_expected)
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
            expected_build = get_expected_build(app_cfg_expected)
            expected_run = get_expected_run(app_cfg_expected)
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

def gen_mdtxt(key, value=None, bold=True):
    if bold:
        key = "**%s**" % (key)
    return "[%s](%s)" % (key, value) if value else key

def generate_build_run_status_md(appresult, logdir, casefail=True):
    if isinstance(appresult, dict) == False:
        if casefail:
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
            return gen_mdtxt(sts, log, bold)
        bsts_md = gen_sts_md(appbsts, appblog, casefail)
        rsts_md = gen_sts_md(apprsts, apprlog, casefail)
        return bsts_md, rsts_md, appbtm, apprtm

def generate_build_cfgname_md(cfgname, appresult, logdir, casefail=False):
    if isinstance(appresult, dict) == False or appresult["logs"].get("build", None) is None:
        caselogdir = None
    else:
        appblog = appresult["logs"].get("build", None)
        caselogdir = os.path.dirname(os.path.relpath(appblog, logdir))
    return gen_mdtxt(cfgname, caselogdir, casefail)

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
        return None
    report = analyze_report(config, result, runapp)
    rpt_passtxt = os.path.join(os.path.dirname(rptfile), "app_passed.txt")
    rpt_failtxt = os.path.join(os.path.dirname(rptfile), "app_failed.txt")
    rpt_summary = dict()
    # generate markdown file
    with open(rptfile, "w") as rf:
        # generate overall status
        rf.write("# Overall Status\n\n")
        passcnt = len(report["passed"])
        failcnt = len(report["failed"])
        totalcnt = passcnt + failcnt
        passrate = str(round(((passcnt / totalcnt) * 100), 2)) + "%"
        x = PrettyTable()
        x.set_style(MARKDOWN)
        x.field_names = ["Total", "Pass", "Fail", "PassRate"]
        x.add_row([totalcnt, passcnt, failcnt, passrate])
        rf.write(str(x))
        rf.write("\n\n")

        passed_appsstr = "* **Passed**: %s\n" % (",".join(report["passed"].keys()))
        failed_appsstr = "* **Failed**: %s\n" % (",".join(report["failed"].keys()))
        #rf.write(passed_appsstr)
        if failcnt > 0:
            rf.write(failed_appsstr)
            rf.write("\n\n")

        # generate detailed status
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
        sts_title = ["App/Test Case", "Case Name", "Build Status", "Run Status", "Build Time", \
                    "Run Time", "Type", "Value", "Total", "Text", "Data", "Bss"]
        x.field_names = sts_title
        apps_buildsts = result
        percase_status = report["percase"]
        # failed status
        failed_sts = []
        # summary of all app cases
        tot_cases_count = 0
        tot_cases_btm = []
        tot_cases_rtm = []
        tot_cases_sztot = []
        tot_cases_sztext = []
        tot_cases_szdata = []
        tot_cases_szbss = []
        for app in apps_buildsts:
            app_sts = apps_buildsts[app]
            for cfgname in app_sts:
                tot_cases_count += 1
                caseisfail = False
                if cfgname in percase_status["failed"][app]:
                    caseisfail = True
                size = app_sts[cfgname]["size"]
                apprsttype, apprstval = get_app_runresult(app_sts[cfgname].get("result", dict()))
                bsts_md, rsts_md, appbtm, apprtm = generate_build_run_status_md(app_sts[cfgname], logdir, caseisfail)
                cfgname_md = generate_build_cfgname_md(cfgname, app_sts[cfgname], logdir, caseisfail)
                sts_row = [app, cfgname_md, bsts_md, rsts_md, appbtm, apprtm, apprsttype, apprstval, \
                    size["total"], size["text"], size["data"], size["bss"]]
                x.add_row(sts_row)
                # add failed status into list
                if caseisfail:
                    failed_sts.append(sts_row)
                # record total cases build and run time
                appwithcfg = "%s:%s" % (app, cfgname)
                tot_cases_btm.append((str(appbtm), appwithcfg))
                tot_cases_rtm.append((str(apprtm), appwithcfg))
                tot_cases_sztot.append((str(size["total"]), appwithcfg))
                tot_cases_sztext.append((str(size["text"]), appwithcfg))
                tot_cases_szdata.append((str(size["data"]), appwithcfg))
                tot_cases_szbss.append((str(size["bss"]), appwithcfg))

        rf.write(str(x))
        rf.write("\n")
        # save report summary
        rpt_summary["count"] = tot_cases_count
        rpt_summary["buildtime"] = tot_cases_btm
        rpt_summary["runtime"] = tot_cases_rtm
        rpt_summary["sztotal"] = tot_cases_sztot
        rpt_summary["sztext"] = tot_cases_sztext
        rpt_summary["szdata"] = tot_cases_szdata
        rpt_summary["szbss"] = tot_cases_szbss

        # show only failed cases
        if len(failed_sts) > 0:
            rf.write("\n# Failed Cases Details\n\n")
            x = PrettyTable()
            x.set_style(MARKDOWN)
            x.field_names = sts_title
            for row in failed_sts:
                x.add_row(row)
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
                expected_build = get_expected_build(app_cfg_expected)
                expected_run = get_expected_run(app_cfg_expected)
                if expected_build == False or expected_run == False:
                    with_expect = True
                    x.add_row([app, cfgname, expected_build, expected_run])
        if with_expect:
            rf.write("\n# Expected Build or Run Failed Cases\n\n")
            rf.write(str(x))
            rf.write("\n")
    # generate html from markdown
    md2html(rptfile, rpthtml)
    return rpt_summary

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

def update_list_items(list1, list2):
    for i in range(0, len(list2)):
        if list2[i] not in list1:
            list1.append(list2[i])
    return list1

def merge_runconfig(all_mergedcfg, config, reldir):
    if "run_config" not in all_mergedcfg:
        all_mergedcfg["run_config"] = config["run_config"]
    if "build_target" not in all_mergedcfg:
        all_mergedcfg["build_target"] = config["build_target"]
    if "parallel" not in all_mergedcfg:
        all_mergedcfg["parallel"] = config.get("parallel", "-j")
    if "build_config" not in all_mergedcfg:
        all_mergedcfg["build_config"] = dict()
    if "build_configs" not in all_mergedcfg:
        all_mergedcfg["build_configs"] = dict()

    if "expecteds" not in all_mergedcfg:
        all_mergedcfg["expecteds"] = dict()

    if "checks" not in all_mergedcfg:
        all_mergedcfg["checks"] = config.get("checks", dict())
    if "appdirs" not in all_mergedcfg:
        all_mergedcfg["appdirs"] = config.get("appdirs", [])
    if "appdirs_ignore" not in all_mergedcfg:
        all_mergedcfg["appdirs_ignore"] = config.get("appdirs_ignore", [])
    if "appconfig" not in all_mergedcfg:
        all_mergedcfg["appconfig"] = config.get("appconfig", dict())

    # TODO handle expecteds and expected
    if "expected" in config:
        for app in config["expected"]:
            if app not in all_mergedcfg["expecteds"]:
                all_mergedcfg["expecteds"][app] = dict()
            newcfgname = reldir.replace("\\", "/")
            all_mergedcfg["expecteds"][app][newcfgname] = config["expected"][app]

    if "expecteds" in config:
        for app in config["expecteds"]:
            if app not in all_mergedcfg["expecteds"]:
                all_mergedcfg["expecteds"][app] = dict()
            for cfgname in config["expecteds"][app]:
                newcfgname = os.path.join(reldir, cfgname).replace("\\", "/")
                all_mergedcfg["expecteds"][app][newcfgname] = config["expecteds"][app][cfgname]

    def merge_buildcfgs(dstcfg, srccfg, rel):
        if "build_configs" in srccfg:
            for bcfg in srccfg["build_configs"]:
                new_bcfg = os.path.join(rel, bcfg).replace("\\", "/")
                dstcfg["build_configs"][new_bcfg] = merge_two_config(srccfg.get("build_config", None), srccfg["build_configs"][bcfg])


    merge_buildcfgs(all_mergedcfg, config, reldir)

    all_mergedcfg["appdirs"] = update_list_items(all_mergedcfg.get("appdirs", []), config.get("appdirs", []))
    all_mergedcfg["appdirs_ignore"] = update_list_items(all_mergedcfg.get("appdirs_ignore", []), config.get("appdirs_ignore", []))
    appconfigs = config.get("appconfig", dict())
    for app in appconfigs:
        if app not in all_mergedcfg["appconfig"]:
            all_mergedcfg["appconfig"][app] = {"build_config": {}, "build_configs": {}, "checks": appconfigs[app].get("checks", dict())}
        merge_buildcfgs(all_mergedcfg["appconfig"][app], appconfigs[app], reldir)
    return

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
                        app_cfg = os.path.join(reldir, cfg).replace("\\", "/")
                        if app not in all_result:
                            all_result[app] = dict()
                        all_result[app][app_cfg] = result[app][cfg]
                merge_runconfig(all_mergedcfg, config, reldir)
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

def parse_result2dict(result):
    if not(isinstance(result, dict)):
        return None
    csvdict = dict()
    for app in result:
        appresult = result[app]
        for cfg in appresult:
            if cfg not in csvdict:
                csvdict[cfg] = dict()
            runsts = appresult[cfg]["status"].get("run", False)
            if runsts == False:
                continue
            apptype = appresult[cfg]["result"]["type"]
            appsubtype = appresult[cfg]["result"].get("subtype", "")
            if appsubtype == "":
                appsubtype = "default"
            if apptype == "unknown":
                continue
            if apptype not in csvdict[cfg]:
                csvdict[cfg][apptype] = dict()
            if appsubtype not in csvdict[cfg][apptype]:
                csvdict[cfg][apptype][appsubtype] = dict()
            csvdict[cfg][apptype][appsubtype]["meta"] = appresult[cfg].get("toolver", dict())
            if csvdict[cfg][apptype][appsubtype]["meta"] == None:
                csvdict[cfg][apptype][appsubtype]["meta"] = dict()
            csvdict[cfg][apptype][appsubtype]["meta"].update(appresult[cfg].get("flags", dict()))
            if "value" not in csvdict[cfg][apptype][appsubtype]:
                csvdict[cfg][apptype][appsubtype]["value"] = dict()
            csvdict[cfg][apptype][appsubtype]["value"].update(appresult[cfg]["result"]["value"])
            csvdict[cfg][apptype][appsubtype]["size"] = appresult[cfg]["size"]
    return csvdict

def show_failed_apps(logdir):
    rpt_failtxt = os.path.join(logdir, "app_failed.txt")
    if os.path.isfile(rpt_failtxt) == False:
        return
    with open(rpt_failtxt, "r") as rpt_ff:
        failed_lines = rpt_ff.readlines()
        if len(failed_lines) > 0:
            print("Here are the failed applications list below")
            for line in failed_lines:
                print(line)
    return

def show_report_summary(summary, sfl):
    if not(isinstance(summary, dict)):
        return
    if len(summary) == 0:
        return
    def decsort(key):
        return key[0]

    summary["buildtime"].sort(reverse=True, key=decsort)
    summary["runtime"].sort(reverse=True, key=decsort)
    summary["sztotal"].sort(reverse=True, key=decsort)
    summary["sztext"].sort(reverse=True, key=decsort)
    summary["szdata"].sort(reverse=True, key=decsort)
    summary["szbss"].sort(reverse=True, key=decsort)
    mostnum = min(10, summary["count"])
    with open(sfl, "w") as sflh:
        sflh.write("There are %d cases executed\r\n" % (summary["count"]))
        sflh.write("The most %d build time costing cases are %s\r\n" % (mostnum, summary["buildtime"][:mostnum]))
        sflh.write("The most %d run time costing cases are %s\r\n" % (mostnum, summary["runtime"][:mostnum]))
        sflh.write("The most %d program total size costing cases are %s\r\n" % (mostnum, summary["sztotal"][:mostnum]))
        sflh.write("The most %d program text section size costing cases are %s\r\n" % (mostnum, summary["sztext"][:mostnum]))
        sflh.write("The most %d program data section size costing cases are %s\r\n" % (mostnum, summary["szdata"][:mostnum]))
        sflh.write("The most %d program bss section size costing cases are %s\r\n" % (mostnum, summary["szbss"][:mostnum]))
    print("\r\n=====Here is the report summary:=====")
    with open(sfl, "r") as sflh:
        for line in sflh.readlines():
            print(line)
    pass

def save_report_files(logdir, config, result, run=False):
    if os.path.isdir(logdir) == False:
        os.makedirs(logdir)
    rptfile = os.path.join(logdir, "report.md")
    rpthtml = os.path.join(logdir, "report.html")
    rptsumfile = os.path.join(logdir, "summary.txt")
    rptsum = generate_report(config, result, rptfile, rpthtml, logdir, run)
    csvfile = os.path.join(logdir, "result.csv")
    save_bench_csv(result, csvfile)
    print("Generate report csv file to %s" % (csvfile))
    print("Generate report markdown file to %s" % (rptfile))
    if run:
        csvdata = parse_result2dict(result)
        csvdatafile = os.path.join(logdir, "runresult.json")
        save_json(csvdatafile, csvdata)
        runresultexcel = os.path.join(logdir, "runresult.xlsx")
        save_runresult(csvdata, runresultexcel)
    show_report_summary(rptsum, rptsumfile)
    # show failed apps
    show_failed_apps(logdir)
    pass

# save excel and csv for selected csv table
def save_excel(csvtable, excelfile):
    newcsvtable = {}
    csvsummary = []
    keylens = [ len(key) for key in  csvtable.keys() ]
    keylens.sort()
    maxlen = keylens[-1]
    for cfg in csvtable:
        if len(csvtable[cfg]) > 1:
            csvsummary.append([cfg])
            csvsummary.extend(csvtable[cfg])
            if maxlen >= 31:
                cfglist = cfg.split('-')
                newcfg = "-".join(cfglist[1:])
                newcsvtable[newcfg] = csvtable[cfg]
            else:
                newcsvtable[cfg] = csvtable[cfg]

    newcsvtable["summary"] = csvsummary
    pe.isave_book_as(bookdict=newcsvtable, dest_file_name=excelfile)
    print("Generate run result excel file to %s" % (excelfile))
    csvfile = excelfile + ".csv"
    with open(csvfile, "w") as cf:
        for row in csvsummary:
            cf.write("%s\n" % (",".join(str(e) for e in row)))
    print("Generate run result csv file to %s" % (csvfile))
    pass

def save_runresult(runresult, excelfile):
    if not(isinstance(runresult, dict)):
        return False

    # Get csv header for each header and column 1 & column 2
    csv_headers = dict()
    csv_col1 = dict()
    csv_col2 = dict()
    for cfg in runresult:
        splitcfgs = cfg.split('/')
        pretype = ""
        if len(splitcfgs) > 1:
            pretype = '-'.join(splitcfgs[:-1])

        runcfg = splitcfgs[-1]
        for apptype in runresult[cfg]:
            for subtype in runresult[cfg][apptype]:
                if pretype != "":
                    final_apptype = pretype + "_" + apptype
                else:
                    final_apptype = apptype
                if final_apptype not in csv_headers:
                    csv_headers[final_apptype] = ["RUNCONFIG", "SUBTYPE"]
                    csv_col1[final_apptype] = []
                    csv_col2[final_apptype] = []
                # fill header and col1 / col2
                if runcfg not in csv_headers[final_apptype]:
                    csv_headers[final_apptype].append(runcfg)
                rstvalues = runresult[cfg][apptype][subtype]["value"]
                for key in rstvalues:
                    if key not in csv_col1[final_apptype]:
                        csv_col1[final_apptype].append(key)
                        csv_col2[final_apptype].append(subtype)

    # Fill the csvtable with -
    csvtable = dict()
    for cfg in csv_headers:
        csvtable[cfg] = [csv_headers[cfg]]
        for i in range(0, len(csv_col1[cfg])):
            rowlist = [csv_col1[cfg][i], csv_col2[cfg][i]]
            for j in range(0, len(csv_headers[cfg]) - 2):
                rowlist.append('-')
            csvtable[cfg].append(rowlist)
    # Fill the csvtable with real value if key found
    for cfg in runresult:
        splitcfgs = cfg.split('/')
        pretype = ""
        if len(splitcfgs) > 1:
            pretype = '-'.join(splitcfgs[:-1])

        runcfg = splitcfgs[-1]
        for apptype in runresult[cfg]:
            for subtype in runresult[cfg][apptype]:
                if pretype != "":
                    final_apptype = pretype + "_" + apptype
                else:
                    final_apptype = apptype
                rstvalues = runresult[cfg][apptype][subtype]["value"]
                header = csvtable[final_apptype][0]
                index = header.index(runcfg)
                for key in rstvalues:
                    for i in range(0, len(csvtable[final_apptype])):
                        if key == csvtable[final_apptype][i][0]:
                            csvtable[final_apptype][i][index] = rstvalues[key]
                            break

    # Fill csvdict using csvtable
    csvdict = dict()
    for cfg in csvtable:
        csvdict[cfg] = dict()
        for csvlist in csvtable[cfg]:
            csvdict[cfg][csvlist[0]] = csvlist[1:]
    # Save to excel
    try:
        csvtable_jf = excelfile + ".csvtable.json"
        csvdict_jf = excelfile + ".csvdict.json"
        save_json(csvtable_jf, csvtable)
        save_json(csvdict_jf, csvdict)
        save_excel(csvtable, excelfile)
    except:
        print("pyexcel package is not installed.!")
        return False

    return True

def generate_report_for_logs(logdir, run=False, split=False):
    if logdir and os.path.isdir(logdir):
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
            save_report_files(logdir, all_mergedcfg, all_result, run)
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
