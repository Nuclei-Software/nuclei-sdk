#!/usr/bin/env python3

import os
import sys
import time
import copy
import serial
import tempfile
import collections
from threading import Thread
import subprocess
import asyncio
import glob
import json

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

JSON_OK=0
JSON_NOFILE=1
JSON_INVAILD=2
def load_json(file):
    if isinstance(file, str) == False or os.path.isfile(file) == False:
        return JSON_NOFILE, None
    try:
        data = json.load(open(file, 'r'))
        return JSON_OK, data
    except:
        print("Error: %s is an invalid json file!" % (file))
        return JSON_INVAILD, None

def save_json(file, data):
    if isinstance(file, str) == False:
        return False
    try:
        with open(file, "w") as cf:
            json.dump(data, cf, indent=4)
        return True
    except:
        print("Error: Data can't be serialized to json file!")
        return False

# get from https://gist.github.com/angstwad/bf22d1822c38a92ec0a9
def dict_merge(dct, merge_dct):
    """ Recursive dict merge. Inspired by :meth:``dict.update()``, instead of
    updating only top-level keys, dict_merge recurses down into dicts nested
    to an arbitrary depth, updating keys. The ``merge_dct`` is merged into
    ``dct``.
    :param dct: dict onto which the merge is executed
    :param merge_dct: dct merged into dct
    :return: None
    """
    for k, v in merge_dct.items():
        if (k in dct and isinstance(dct[k], dict)
                and isinstance(merge_dct[k], collections.Mapping)):
            dict_merge(dct[k], merge_dct[k])
        else:
            dct[k] = merge_dct[k]

def get_make_csv(app, config):
    make_options = " "
    SUPPORT_KEYS = ["SOC", "BOARD", "CORE", "DOWNLOAD", "VARIANT", \
        "BENCH_UNIT", "BENCH_FLAGS", "DSP_ENABLE", "SILENT", "V"]
    csv_print = "CSV, APP=%s" % (app)
    if isinstance(config, dict):
        for key in config:
            if key not in SUPPORT_KEYS:
                continue
            option = "%s=%s"%(key, config[key])
            make_options = " %s %s " % (make_options, option)
            csv_print = "%s, %s" % (csv_print, option)
    return make_options, csv_print

def try_decode_bytes(bytes):
    ENCODING_LIST = ['utf-8', 'gbk', 'gb18030']
    destr = ""
    for encoding in ENCODING_LIST:
        try:
            destr = bytes.decode(encoding)
            break
        except:
            continue
    return destr

COMMAND_RUNOK=0
COMMAND_INVALID=1
COMMAND_FAIL=2
COMMAND_INTERRUPTED=3
COMMAND_EXCEPTION=4
COMMAND_NOTAPP=5
COMMAND_TIMEOUT=6

RUNSTATUS_OK=0
RUNSTATUS_FAIL=1
RUNSTATUS_NOTSTART=2
def run_command(command, show_output=True, logfile=None, append=False):
    logfh = None
    ret = COMMAND_RUNOK
    cmd_elapsed_ticks = 0
    if isinstance(command, str) == False:
        return COMMAND_INVALID, cmd_elapsed_ticks
    startticks = time.time()
    process = None
    try:
        if isinstance(logfile, str):
            if append:
                logfh = open(logfile, "ab")
            else:
                logfh = open(logfile, "wb")
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, \
                            stderr=subprocess.STDOUT)
        while True:
            line = process.stdout.readline()
            if (not line) and process.poll() is not None:
                break
            if show_output:
                print(try_decode_bytes(line), end="")
            if logfh:
                logfh.write(line)
            time.sleep(0.01)
        process.communicate(30)
        if process.returncode != 0:
            ret = COMMAND_FAIL
    except (KeyboardInterrupt):
        print("Key CTRL-C pressed, command executing stopped!")
        ret = COMMAND_INTERRUPTED
    except subprocess.TimeoutExpired:
        process.kill()
        ret = COMMAND_TIMEOUT
    except Exception as exc:
        print("Unexpected exception happened: %s" %(str(exc)))
        ret = COMMAND_EXCEPTION
    finally:
        if process:
            del process
        if logfh:
            logfh.close()
    cmd_elapsed_ticks = time.time() - startticks
    return ret, cmd_elapsed_ticks

# def run_command(command, show_output=True, logfile=None):
#     if sys.platform == "win32":
#         loop = asyncio.ProactorEventLoop() # For subprocess' pipes on Windows
#         asyncio.set_event_loop(loop)
#     else:
#         loop = asyncio.get_event_loop()

#     ret, cmd_elapsed_ticks = loop.run_until_complete(run_command_async(command, show_output, logfile, timeout=10))
#     loop.close()
#     return ret, cmd_elapsed_ticks

# async def run_command_async(command, show_output=True, logfile=None, timeout=30):
#     logfh = None
#     ret = COMMAND_RUNOK
#     cmd_elapsed_ticks = 0
#     if isinstance(command, str) == False:
#         return COMMAND_INVALID, cmd_elapsed_ticks
#     startticks = time.time()
#     process = None
#     try:
#         if isinstance(logfile, str):
#             logfh = open(logfile, "wb")
#         process = await asyncio.create_subprocess_shell(command,  stdout=asyncio.subprocess.PIPE, \
#                             stderr=asyncio.subprocess.STDOUT)
#         while True:
#             try:
#                 line = await asyncio.wait_for(process.stdout.readline(), timeout)
#             except asyncio.TimeoutError:
#                 break
#             except (KeyboardInterrupt):
#                 print("Key CTRL-C pressed, command executing stopped!")
#                 ret = COMMAND_INTERRUPTED
#                 break
#             else:
#                 if not line: #EOF
#                     break
#                 if show_output:
#                     print(try_decode_bytes(line), end="")
#                 if logfh:
#                     logfh.write(line)
#                 continue
#         await process.communicate()
#         if process.returncode != 0:
#             ret = COMMAND_FAIL
#     except (KeyboardInterrupt):
#         print("Key CTRL-C pressed, command executing stopped!")
#         ret = COMMAND_INTERRUPTED
#     except Exception as exc:
#         print("Unexpected exception happened: %s" %(str(exc)))
#         ret = COMMAND_EXCEPTION
#     finally:
#         if process:
#             del process
#         if logfh:
#             logfh.close()
#     cmd_elapsed_ticks = time.time() - startticks
#     return ret, cmd_elapsed_ticks

def find_files(fndir, pattern, recursive=False):
    fndir = os.path.normpath(fndir)
    files = glob.glob(os.path.join(fndir, pattern), recursive=recursive)
    return files

def get_logfile(appdir, startdir, logdir, logname):
    relpath = os.path.relpath(appdir, startdir)
    startdir_basename = os.path.basename(startdir)
    applogdir = os.path.join(logdir, startdir_basename, relpath)
    applog = os.path.join(applogdir, logname)
    applogdir = os.path.dirname(applog)
    if os.path.isdir(applogdir) == False:
        os.makedirs(applogdir)
    return applog

def get_elfsize(elf):
    sizeinfo = {"text": -1, "data": -1, "bss": -1, "total": -1}
    if os.path.isfile(elf) == False:
        return sizeinfo
    sizecmd = "riscv-nuclei-elf-size %s" % (elf)
    sizelog = tempfile.mktemp()
    ret, _ = run_command(sizecmd, show_output=False, logfile=sizelog)
    if ret == COMMAND_RUNOK:
        with open(sizelog, "r") as sf:
            lines = sf.readlines()
            datas = lines[1].strip().split()
            sizeinfo["text"] = int(datas[0])
            sizeinfo["data"] = int(datas[1])
            sizeinfo["bss"] = int(datas[2])
            sizeinfo["total"] = int(datas[3])
    os.remove(sizelog)
    return sizeinfo

def find_local_appconfig(appdir, localcfgs):
    if isinstance(appdir, str) and isinstance(localcfgs, dict):
        if appdir in localcfgs:
            return appdir
        else:
            foundcfg = None
            for localcfg in localcfgs:
                localcfgtp = localcfg.strip('/')
                striped_dir = appdir.split(localcfgtp, 1)
                if len(striped_dir) == 2:
                    striped_dir = striped_dir[1]
                else:
                    striped_dir = appdir
                if striped_dir != appdir:
                    if striped_dir.startswith('/'):
                        if foundcfg is None:
                            foundcfg = localcfg
                        else:
                            if len(foundcfg) < len(localcfg):
                                foundcfg = localcfg
            return foundcfg
    else:
        return None

PROGRAM_UNKNOWN="unknown"
PROGRAM_COREMARK="coremark"
PROGRAM_DHRYSTONE="dhrystone"
PROGRAM_WHETSTONE="whetstone"
def parse_benchmark_runlog(lines):
    if isinstance(lines, list) == False:
        return False, None
    result = None
    program_type = PROGRAM_UNKNOWN
    for line in lines:
        # Coremark
        if "CoreMark" in line:
            program_type = PROGRAM_COREMARK
        if "Iterations*1000000/total_ticks" in line:
            value = line.split("=")[1].strip().split()[0]
            result = dict()
            result["CoreMark/MHz"] = (float)(value)
        # Dhrystone
        if "Dhrystone" in line:
            program_type = PROGRAM_DHRYSTONE
        if "1000000/(User_Cycle/Number_Of_Runs)" in line:
            value = line.split("=")[1].strip().split()[0]
            result = dict()
            result["DMIPS/MHz"] = (float)(value)
        # Whetstone
        if "Whetstone" in line:
            program_type = PROGRAM_WHETSTONE
        if "MWIPS/MHz" in line:
            value = line.strip().split()[1:2]
            result = dict()
            result["MWIPS/MHz"] = (float)(value[0])
    return program_type, result

