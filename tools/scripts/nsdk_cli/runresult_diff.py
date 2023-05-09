#!/usr/bin/env python3

from nsdk_utils import *
from functools import total_ordering
import os
from posixpath import split
import sys
import time
import copy
import glob
import argparse
import shutil
import subprocess
import csv
import json
import re
import datetime
import operator
import collections
from openpyxl import load_workbook
from openpyxl.utils import get_column_letter, column_index_from_string

SCRIPTDIR = sys.path[0]

# key name as a line,value is two-level nested list, generated csv file is where json file is
def dump_jsonfile2csv(jsonfile, csvfile):
    valid, csvtable = load_json(jsonfile)
    csvsummary = []
    csvlist = []
    if len(csvtable) == 0:
        print("No data found, no need to generate csv!")
        return False
    for key in csvtable:
        # must has one row title and at least one row content
        if len(csvtable[key]) > 1:
            csvsummary.append([key])
            csvsummary.extend(csvtable[key])

    # turn into csv format
    for value in csvsummary:
        csvlist.append(",".join(map(str, value)))

    print(">>> Dumping %s into %s\n" %(jsonfile, csvfile))
    save_csv(csvfile, csvlist, display=False)
    return True

# Merge barebench different dir's "runresult.xlsx.csv" into one
def merge_csvs(srcdir: str, srcfilename: str, mergedcsv: str):
    with open(mergedcsv, "w+") as merged_file:
        subdirlist = os.listdir(srcdir)
        subdirlist.sort()
        for subdirname in subdirlist:
            bare_rstfile = os.path.join(srcdir, subdirname, srcfilename)
            if not os.path.isfile(bare_rstfile):
                continue
            print(">>> Merge %s into %s\n" %(bare_rstfile, mergedcsv))
            with open(bare_rstfile, "r") as bf:
                lineslist = bf.readlines()
                lineslist.insert(0, "Case type:    " + subdirname + "\n")
                merged_file.writelines(lineslist)

def performance_diff_bare(base_value: str, opt_value: str, nounit = True):
    # filter: not a number
    if re.compile("_*[A-Za-z]+[0-9]*|-").search(base_value) or \
        re.compile("_*[A-Za-z]+[0-9]*|-").search(opt_value):
        # todo: check base and new in case of wrong item
        return "%s" %(base_value)
    opt_value_str = opt_value.strip()
    base_value_str = base_value.strip()

    # get unit
    unit = re.compile("[A-Za-z/%]+").findall(opt_value)[-1] if not nounit else " "
    # get value
    base_value_num = float(base_value_str.split(unit)[0].strip())
    opt_value_num = float(opt_value_str.split(unit)[0].strip())
    diff_value = float(opt_value_num - base_value_num)
    # big is better
    #percent = "%f%%" %(abs(diff_value/base_value_num)*100) if base_value != 0 else "%s vs %s" %(diff_value, base_value)

    # small is better (opt-base)/opt
    percent = "%f%%" %(abs(diff_value/opt_value_num)*100) if base_value != 0 else "%s vs %s" %(diff_value, base_value)
    # small means performance increase (↑)
    diff_sign = '↑' if 0 > diff_value else '↓'
    diff_sign = "Δ=" if 0 == diff_value else diff_sign
    diff_content = "%s %f %s   %s %s" %(diff_sign, abs(diff_value), unit, diff_sign, percent)
    return diff_content

# two-level nested list
def perf_diff_list_bare(base_value_list: list, opt_value_list: list, nounit = True, start_idx = 0):
    writecontentlist = []
    for baselist, newlist in zip(base_value_list, opt_value_list):
        writecontent = []
        for basevalue, newvalue in zip(baselist[start_idx:], newlist[start_idx:]):
            writecontent.append(performance_diff_bare(str(basevalue).strip(), str(newvalue).strip(), nounit))
        if start_idx > 0:
            temp = []
            temp = baselist[:start_idx]
            temp.extend(writecontent)
            writecontent = temp
        writecontentlist.append(writecontent)
# todo: return[] if only one list, not [[]]
    return writecontentlist

def generate_diff_json(base_value_list: list, opt_value_list: list, nounit, start_idx):
    diff_list = perf_diff_list_bare(base_value_list, opt_value_list, nounit, start_idx)
    return diff_list

def diff_with(base_resultjson: dict, new_resultjson: dict, diff_resultname: str):
    baseresult_jsondata_sorted, newresult_jsondata_sorted = sort_dict_netstedlist(base_resultjson, new_resultjson, \
                                        sortfrom_listindex = 1)
    if baseresult_jsondata_sorted is None or newresult_jsondata_sorted is None:
        print("Error: Empty dict!")
        return False
    diff_case_results = dict()
    for key in newresult_jsondata_sorted:
        if key in baseresult_jsondata_sorted:
            case_result_new = get_specific_key_value(newresult_jsondata_sorted, key)
            case_result_base = get_specific_key_value(baseresult_jsondata_sorted, key)
            diff_case_result = generate_diff_json(case_result_base, case_result_new, nounit = True, start_idx = 0)
            # generate diff result json from compared jsons
            diff_case_results.update({key: diff_case_result})

    print("Compare done, start generating report!\n")
    diff_resultcsv = os.path.join(SCRIPTDIR, diff_resultname)
    diff_resultjson = diff_resultname.rstrip(".csv") + ".json"

    save_json(diff_resultjson, diff_case_results)
    ret = dump_jsonfile2csv(diff_resultjson, diff_resultcsv)
    return ret, diff_resultcsv

# todo: may need to get overlap_set 1st-level keys of two dicts later, now suppose at least 1st-level keys are same(ordered dict)
# two-level nest list, like nmsis_dsp_tests in runresult.xlsx.csvtable.json
def sort_dict_netstedlist(basedict: dict, newdict: dict, sortfrom_listindex: int):
    for targetkey in basedict:
        if targetkey not in basedict or targetkey not in newdict:
            continue
        rest_list_base = basedict[targetkey][:sortfrom_listindex]
        rest_list_new = newdict[targetkey][:sortfrom_listindex]
        if len(basedict[targetkey]) > 1 and len(newdict[targetkey]) > 1:
            resultlist_base = basedict[targetkey][sortfrom_listindex:]
            resultlist_new = newdict[targetkey][sortfrom_listindex:]

            resultlist_sorted_base, resultlist_sorted_new = sort_twolist_by_commonkey(resultlist_base, resultlist_new)

            rest_list_base.extend(resultlist_sorted_base)
            rest_list_new.extend(resultlist_sorted_new)

            basedict.update({targetkey: rest_list_base})
            newdict.update({targetkey: rest_list_new})
    return basedict, newdict

# two-level nest list, like nmsis_dsp_tests in runresult.xlsx.csvtable.json
# base has the fewer elements, sort by the first two element(0: sub-type, 1: sub-function)
def sort_list_by_baseone_key(base: list, tobe_sorted: list):
    new_sortedlist = []
    index_to_remove = []

    base_keys = [value[0] for value in base]
    tobe_sorted_keys = [value[0] for value in tobe_sorted]

    for sublist_index, sublist_value in enumerate(base):
        found = False
        for sublist in tobe_sorted:
            if sublist_value[0] == sublist[0] and sublist_value[1] == sublist[1]:
                found = True
                new_sortedlist.append(sublist)
        if not found:
            index_to_remove.append(sublist_index)

    # remove the element which still doesn't exist in the list "tobe_sorted"
    for index in reversed(index_to_remove):
        base.pop(index)
    
    union_set = set(tobe_sorted_keys) | set(base_keys)
    overlap_set = set(tobe_sorted_keys) & set(base_keys)

    if union_set - overlap_set:
        print("Warning: these functions as below belongs to no overlap_set, will not show in the diff report: %s\n"
             %(union_set - overlap_set))

    return base, new_sortedlist

def sort_twolist_by_commonkey(list_1: list, list_2: list):
    if len(list_1) < len(list_2):
        base, new_sortedlist = sort_list_by_baseone_key(list_1, list_2)
        return base, new_sortedlist
    else:
        base, new_sortedlist = sort_list_by_baseone_key(list_2, list_1)
        # corresponding to order of input
        return new_sortedlist, base

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = "Compare two runresult.xlsx.csvtable.json to get benchmark result difference")
    parser.add_argument('--base', required = True, help = "Path to base one, suffix with .csvtable.json")
    parser.add_argument('--new', required = True, help = "Path to new one, suffix with .csvtable.json")
    parser.add_argument('--name', default = "diff_runresult", help = "Name of the diff result without suffix")
    args = parser.parse_args()

    base_json = args.base
    new_json = args.new
    diff_resultname = "%s.csv" %(args.name.rstrip(".csv"))

    valid, baseresult_jsondata = load_json(base_json)
    if JSON_OK != valid:
        print("Invalid json file %s, please check!" % (base_json))
        sys.exit(1)

    valid, newresult_jsondata = load_json(new_json)
    if JSON_OK != valid:
        print("Invalid json file %s, please check!" % (new_json))
        sys.exit(1)

    print("Start to compare %s with %s\n" %(new_json, base_json))

    ret, diff_result = diff_with(baseresult_jsondata, newresult_jsondata, diff_resultname)
    if False == ret:
        print("Diff failed!\n")
        sys.exit(1)
    else:
        print("Diff succeed! See report %s" %(diff_result))
        sys.exit(0)
