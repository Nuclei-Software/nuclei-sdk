#!/usr/bin/env python3

import os
import argparse

def get_all_jsonfiles(rootdir):
    jsonfiles = []
    try:
        for root, dirs, files in os.walk(rootdir):
            for file in files:
                if file.endswith(".json") or file.endswith(".json.libncrt"):
                    filepath = "%s/%s" %(root, file)
                    print("Found %s" % (filepath) )
                    jsonfiles.append(filepath)
    except:
        pass
    return jsonfiles

def fix_archext_in_json(jsonfile):
    if os.path.isfile(jsonfile) == False:
        return False
    lines = []
    with open(jsonfile, "r") as f:
        lines = f.readlines()

    fixcnt = 0
    with open(jsonfile, "w") as f:
        for line in lines:
            vext_name = "_zve32f"
            if "\"nx" in line or "\"ux" in line:
                vext_name = "v"
            oldext=''
            if '"bpv"' in line:
                oldext = '"bpv"'
                newext = "\"%s_zba_zbb_zbc_zbs_xxldspn1x\"" % (vext_name)
            elif '"bv"' in line:
                oldext = '"bv"'
                newext = "\"%s_zba_zbb_zbc_zbs\"" % (vext_name)
            elif '"bp"' in line:
                oldext = '"bp"'
                newext = "\"zba_zbb_zbc_zbs_xxldspn1x\""
            elif '"pv"' in line:
                oldext = '"pv"'
                newext = "\"%s_xxldspn1x\"" % (vext_name)
            elif '"v"' in line:
                oldext = '"v"'
                newext = "\"%s\"" % (vext_name)
            elif '"p"' in line:
                oldext = '"p"'
                newext = "\"_xxldspn1x\""
            elif '"b"' in line:
                oldext = '"b"'
                newext = "\"_zba_zbb_zbc_zbs\""
            else:
                oldext = ""
                newext = ""
            if oldext != "":
                line = line.replace(oldext, newext)
                fixcnt = fixcnt + 1
            f.write(line)
    print("Fix json file %s, replace count %s" % (jsonfile, fixcnt))
    return True

def fix_jsonfiles(rootdir):
    if os.path.isdir(rootdir) == False:
        return False
    jsonfiles = get_all_jsonfiles(rootdir)
    for jsfile in jsonfiles:
        #print("Fix file %s" % (jsfile))
        fix_archext_in_json(jsfile)
    return True

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Nuclei SDK CLI Configuration Fixup Tools For GCC 13 upgrade")
    parser.add_argument('--jsondir', '-d', required=True, default='tools/scripts/nsdk_cli/', help="Where json configuration files located")

    args = parser.parse_args()

    fix_jsonfiles(args.jsondir)
    pass
