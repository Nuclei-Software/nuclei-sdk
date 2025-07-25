#!/usr/bin/env python3

import os
import sys
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
requirement_file = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "requirements.txt"))

try:
    import time
    import datetime
    import random
    import shutil
    import signal
    import psutil
    import re
    import copy
    import serial
    import serial.tools.list_ports
    import tempfile
    import collections
    from collections import OrderedDict
    from threading import Thread
    import subprocess
    import asyncio
    import glob
    import json
    import yaml
    import importlib.util
    if sys.platform != "win32":
        import fcntl
    import stat
except Exception as exc:
    print("Import Error: %s" % (exc))
    print("Please install requried packages using: pip3 install -r %s" % (requirement_file))
    sys.exit(1)

try:
    from collections.abc import Mapping
except ImportError: # Python 2.7 compatibility
    from collections import Mapping

SDK_GLOBAL_VARIABLES = {
    "sdk_checktag": "Nuclei SDK Build Time:",
    "sdk_check": True,
    "sdk_banner_tmout": 15,
    "sdk_copy_objects": "elf,map",
    "sdk_copy_objects_flag": False,
    "sdk_ttyerr_maxcnt": 3,
    "sdk_fpgaprog_maxcnt": 3,
    "sdk_gdberr_maxcnt": 10,
    "sdk_uploaderr_maxcnt": 10,
    "sdk_bannertmout_maxcnt": 100,
    "sdk_verb_buildmsg": True,
    "sdk_copy_failobj": True
    }

INVAILD_SERNO = "xxxxx"
BANNER_TMOUT = "banner_timeout"
TTY_OP_ERR = "tty_operate_error"
TTY_UNKNOWN_ERR = "tty_unknown_error"
FILE_LOCK_NAME = "fpga_program.lock"

DATE_FORMATE = "%Y-%m-%d %H:%M:%S"

def get_tmpdir():
    tempdir = tempfile.gettempdir()
    if sys.platform == "win32":
        wintempdir = "C:\\Users\\Public\\Temp"
        if os.path.isdir(wintempdir) == False:
            os.makedirs(wintempdir)
        tempdir = wintempdir
    return tempdir

# get ci url information
def get_ci_info():
    cijoburl = os.environ.get("CI_JOB_URL")
    cipipelineurl = os.environ.get("CI_PIPELINE_URL")
    if cijoburl and cipipelineurl:
        return {"joburl": cijoburl, "pipelineurl": cipipelineurl}
    else:
        return {}

def get_global_variables():
    return SDK_GLOBAL_VARIABLES

def get_sdk_checktag():
    checktag = os.environ.get("SDK_CHECKTAG")
    if checktag is None:
        checktag = SDK_GLOBAL_VARIABLES.get("sdk_checktag")
    return checktag

def get_sdk_copyobjects():
    cpobjs = os.environ.get("SDK_COPY_OBJECTS")
    if cpobjs is None:
        cpobjs = SDK_GLOBAL_VARIABLES.get("sdk_copy_objects")
    return cpobjs

def get_env_flag(envar, deft=None):
    flag = os.environ.get(envar)
    if flag is None:
        return deft
    return flag.lower() in ('true', '1', 't')

def get_sdk_check():
    check = get_env_flag("SDK_CHECK")
    if check is None:
        check = SDK_GLOBAL_VARIABLES.get("sdk_check")
    return check

def get_sdk_verb_buildmsg():
    check = get_env_flag("SDK_VERB_BUILDMSG")
    if check is None:
        check = SDK_GLOBAL_VARIABLES.get("sdk_verb_buildmsg")
    return check

def get_sdk_copyobjects_flag():
    cpflag = get_env_flag("SDK_COPY_OBJECTS_FLAG")
    if cpflag is None:
        cpflag = SDK_GLOBAL_VARIABLES.get("sdk_copy_objects_flag")
    return cpflag

def get_sdk_need_copyobjects(appconfig):
    try:
        needed = appconfig.get("copy_objects")
    except:
        needed = False
    if needed != True:
        # use global flag
        needed = get_sdk_copyobjects_flag()
    return needed

def get_sdk_copy_failobj():
    cpflag = get_env_flag("SDK_COPY_FAILOBJ")
    if cpflag is None:
        cpflag = SDK_GLOBAL_VARIABLES.get("sdk_copy_failobj")
    return cpflag

def get_sdk_banner_tmout():
    tmout = os.environ.get("SDK_BANNER_TMOUT")
    if tmout is not None:
        tmout = int(tmout)
    else:
        tmout = SDK_GLOBAL_VARIABLES.get("sdk_banner_tmout")

    return tmout

# some case may run more than default timeout in app.json
def get_sdk_run_tmout():
    tmout = os.environ.get("SDK_RUN_TMOUT")
    if tmout is not None:
        tmout = int(tmout)
    return tmout

def get_sdk_fpga_prog_tmout():
    tmout = os.environ.get("FPGA_PROG_TMOUT")
    return tmout

def get_sdk_ttyerr_maxcnt():
    num = os.environ.get("SDK_TTYERR_MAXCNT")
    if num is not None:
        num = int(num)
    else:
        num = SDK_GLOBAL_VARIABLES.get("sdk_ttyerr_maxcnt")

    return num

def get_sdk_fpgaprog_maxcnt():
    num = os.environ.get("SDK_FPGAPROG_MAXCNT")
    if num is not None:
        num = int(num)
    else:
        num = SDK_GLOBAL_VARIABLES.get("sdk_fpgaprog_maxcnt")

    return num

def get_sdk_gdberr_maxcnt():
    num = os.environ.get("SDK_GDBERR_MAXCNT")
    if num is not None:
        num = int(num)
    else:
        num = SDK_GLOBAL_VARIABLES.get("sdk_gdberr_maxcnt")

    return num

def get_sdk_bannertmout_maxcnt():
    num = os.environ.get("SDK_BANNERTMOUT_MAXCNT")
    if num is not None:
        num = int(num)
    else:
        num = SDK_GLOBAL_VARIABLES.get("sdk_bannertmout_maxcnt")

    return num

def get_sdk_uploaderr_maxcnt():
    num = os.environ.get("SDK_UPLOADERR_MAXCNT")
    if num is not None:
        num = int(num)
    else:
        num = SDK_GLOBAL_VARIABLES.get("sdk_uploaderr_maxcnt")

    return num


def parse_riscv_arch(arch_str):
    """Parse RISC-V architecture string to standardized format"""
    if not arch_str:
        return None
    arch_str = arch_str.lower()
    if not arch_str.startswith('rv32') and not arch_str.startswith('rv64'):
        return None

    features = {
        'xlen': arch_str[:4],
        'base': '',
        'exts': set()
    }

    # Parse standard ISA string
    std_isa = arch_str[4:].split('_')[0]
    for c in std_isa:
        if c in 'iemafdcbpkv':
            # don't add b k p into base architecture
            if c == 'b':
                # for nuclei b extension contains zba/zbb/zbc/zbs
                features['exts'].add('zba')
                features['exts'].add('zbb')
                features['exts'].add('zbc')
                features['exts'].add('zbs')
            elif c == 'k':
                # for nuclei k extension contains zba/zbb/zbc/zbs
                features['exts'].add('zk') # zk -> zkn zkr zkt
                features['exts'].add('zks') # zks -> zbkb-sc zbkc-sc zbkx-sc zksed zksh
                features['exts'].add('zkn') # zkn -> zbkb-sc zbkc-sc zbkx-sc zkne zknd zknh
                features['exts'].add('zkr')
                features['exts'].add('zkt')
                features['exts'].add('zkne')
                features['exts'].add('zknd')
                features['exts'].add('zknh')
                features['exts'].add('zksed')
                features['exts'].add('zksh')
                features['exts'].add('zbkb-sc')
                features['exts'].add('zbkc-sc')
                features['exts'].add('zbkx-sc')
            elif c == 'v':
                features['exts'].add('zve64d')
                features['exts'].add('zvl128b')
                features['base'] += 'v'
            elif c == 'p':
                features['exts'].add('xxldsp')
            else:
                features['base'] += c

    # when base architecture has i extension, then e extension is implied
    if 'i' in features['base']:
        features['base'] += 'e'

    # Parse extensions
    if '_' in arch_str:
        exts = arch_str.split('_')[1:]
        for ext in exts:
            ext = ext.strip()
            if ext == "":
                continue
            if ext in ('zvl128', 'zvl256', 'zvl512', 'zvl1024'):
                ext = ext + 'b'
            elif ext in ('zvb', 'zvk', 'zc'):
                ext = ext + '*'
            elif ext in ('dsp'):
                ext = 'xxl' + ext
            elif ext in ('dspn1', 'dspn2', 'dspn3'):
                ext = 'xxl' + ext + 'x'
            features['exts'].add(ext)
    # For nuclei zc* can also configured as c extension via mmisc_ctl csr ZCMT_ZCMP_EN bit
    if 'zc*' in features['exts']:
        features['base'] += 'c'
    # For nuclei cpu, zifencei and zicsr are implied
    features['exts'].add('zicsr')
    features['exts'].add('zifencei')
    # zve64d imply zve64f, zve64f imply zve64x and zve32f
    # zve64x imply zve32x, zve32f imply zve32x
    if 'zve64d' in features['exts']:
        features['exts'].add('zve64f')
    if 'zve64f' in features['exts']:
        features['exts'].add('zve32f')
        features['exts'].add('zve64x')
    if 'zve64x' in features['exts']:
        features['exts'].add('zve32x')
    if 'zve32f' in features['exts']:
        features['exts'].add('zve32x')
    if 'xxldspn3x' in features['exts']:
        features['exts'].add('xxldspn2x')
    if 'xxldspn2x' in features['exts']:
        features['exts'].add('xxldspn1x')
    if 'xxldspn1x' in features['exts']:
        features['exts'].add('xxldsp')
    if 'zvl1024b' in features['exts']:
        features['exts'].add('zvl512b')
    if 'zvl512b' in features['exts']:
        features['exts'].add('zvl256b')
    if 'zvl256b' in features['exts']:
        features['exts'].add('zvl128b')

    if 'zve64d' in features['exts'] and 'zvl128b' in features['exts']:
        features['base'] += 'v'

    return features

def get_nuclei_sdk_root():
    sdk_root = os.environ.get("NUCLEI_SDK_ROOT")
    if not sdk_root:
        sdk_root = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "..", ".."))
    return sdk_root

def parse_makefile_core():
    sdk_root = get_nuclei_sdk_root()
    makefile_core = os.path.join(sdk_root, "Build", "Makefile.core")
    core_archs = {}

    if not os.path.exists(makefile_core):
        return core_archs

    with open(makefile_core, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if '_CORE_ARCH_ABI' in line:
                parts = line.split('=')
                if len(parts) == 2:
                    core_name = parts[0].split('_CORE_ARCH_ABI')[0].lower()
                    arch_parts = parts[1].strip().split()
                    if len(arch_parts) >= 2:
                        core_archs[core_name] = arch_parts[0]
    return core_archs

def check_arch_compatibility(core_arch, arch_ext, supported_arch):
    """Check if core architecture with extensions is compatible with supported architecture"""
    if not supported_arch:
        return True

    supported = parse_riscv_arch(supported_arch)
    if not supported:
        return True

    # Combine core_arch with arch_ext
    full_arch = core_arch
    if arch_ext:
        full_arch += arch_ext

    current = parse_riscv_arch(full_arch)
    if not current:
        return False

    # Check XLEN compatibility
    if current['xlen'] != supported['xlen']:
        return False

    # Check base ISA compatibility
    for c in current['base']:
        if c not in supported['base']:
            return False

    # Check extensions compatibility
    # For current['exts'] containing extensions (no * suffix)
    # For supported['exts'] containing extensions (may have * suffix)
    # Extension matching should handle wildcards (*) in supported extensions
    for ext in current['exts']:
        found_match = False
        for supported_ext in supported['exts']:
            if supported_ext.endswith('*'):
                # Handle wildcard matching
                if ext.startswith(supported_ext[:-1]):
                    found_match = True
                    break
            elif ext == supported_ext:
                # Handle exact matching
                found_match = True
                break
        if not found_match:
            return False

    return True
def filter_app_config(appconfig):
    """
    Filter application configurations based on architecture and extension compatibility.

    This function examines the build configuration of an application and determines if it should
    be filtered out based on architecture support and extension compatibility.

    Parameters:
        appconfig (dict): A dictionary containing application configuration.
                         Expected to have a 'build_config' key with CORE, ARCH_EXT details.

    Returns:
        tuple: A pair of (bool, str) where:
            - bool: True if the configuration should be filtered out, False otherwise
            - str: A message explaining why the configuration was filtered (empty if not filtered)

    Environment Variables Used:
        - SDK_SUPPORT_ARCH: Supported architecture specifications
        - SDK_IGNORED_EXTS: Underscore-separated list of extensions to ignore

    Example:
        >>> config = {
        ...     "build_config": {
        ...         "CORE": "n307",
        ...         "ARCH_EXT": "p_zfh"
        ...     }
        ... }
        >>> filter_app_config(config)
        (False, "")

    Notes:
        - The function handles both single-letter and multi-letter extensions
        - Architecture extensions can be specified with or without leading underscore
        - Returns (False, "") if any required configuration is missing or in case of errors
    """
    if not isinstance(appconfig, dict):
        return False, ""

    try:
        build_config = appconfig.get("build_config", None)
        if build_config is None or len(build_config) == 0:
            return False, ""

        # Check SDK_SUPPORT_ARCH compatibility
        core = build_config.get("CORE", "").lower()
        arch_ext = build_config.get("ARCH_EXT", "")
        supported_arch = os.environ.get("SDK_SUPPORT_ARCH")

        if core and supported_arch:
            core_archs = parse_makefile_core()
            if core in core_archs:
                core_arch = core_archs[core]
                if not check_arch_compatibility(core_arch, arch_ext, supported_arch):
                    return True, f"Core {core} with extensions {arch_ext} not supported by {supported_arch}"

        # Continue with existing extension filtering
        archext = build_config.get("ARCH_EXT", None)
        if archext is None or archext.strip() == "":
            return False, ""
        first_part = None
        rest_part = None
        if archext.startswith("_") == False:
            if "_" in archext:
                first_part, rest_part = archext.split("_", 1)
            else:
                if archext.startswith("z"):
                    rest_part = archext
                else:
                    first_part = archext
        else:
            rest_part = archext
        ignored_exts = os.environ.get("SDK_IGNORED_EXTS")
        if ignored_exts is None:
            return False, ""
        unique_exts = list(
            OrderedDict.fromkeys(part.strip() for part in ignored_exts.split('_'))
        )
        if len(unique_exts) == 1 and unique_exts[0] == "":
            return False, ""
        for ext in unique_exts:
            if len(ext) == 0:
                continue
            if len(ext) == 1:
                # handle single letter
                if first_part and ext in first_part:
                    return True, "Filtered by %s extension" %(ext)
            else:
                # handle multi letter
                if rest_part and ext in rest_part:
                    return True, "Filtered by %s extension" % (ext)
    except:
        pass
    return False, ""

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

YAML_OK=0
YAML_NOFILE=1
YAML_INVAILD=2
def load_yaml(file):
    if isinstance(file, str) == False or os.path.isfile(file) == False:
        return YAML_NOFILE, None
    try:
        data = yaml.load(open(file, 'r'), Loader=yaml.FullLoader)
        return YAML_OK, data
    except:
        print("Error: %s is an invalid yaml file!" % (file))
        return YAML_INVAILD, None

def save_yaml(file, data):
    if isinstance(file, str) == False:
        return False
    try:
        with open(file, "w") as cf:
            yaml.dump(data, cf, indent=4)
        return True
    except:
        print("Error: Data can't be serialized to yaml file!")
        return False

def get_specific_key_value(dictdata:dict, key):
    if not dictdata:
        print("Error: dictdata doesn't exist!")
        return None
    value = dictdata.get(key, None)
    if not value:
        print("Error, key %s has no value!" % (key))
        return None
    return value

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

def save_csv(file, csvlines, display=True):
    if isinstance(csvlines, list) == False:
        return False
    # Flush stdout buffer
    sys.stdout.flush()
    try:
        with open(file, "w") as cf:
            for line in csvlines:
                csvline = line + "\n"
                cf.write(csvline)
                cf.flush()
                if display:
                    try:
                        # sometimes facing issue BlockingIOError: [Errno 11] write could not complete without blocking here
                        # maybe related to https://bugs.python.org/issue40634 since we are using async in this tool
                        sys.stdout.flush()
                        print("CSV, %s" % line)
                    except:
                        pass
        return True
    except:
        print("Error: Data can't be saved to file!")
        return False

# Return possible serports, return a list of possible serports
def find_possible_serports():
    comports = serial.tools.list_ports.comports()
    serports = [ port.device for port in comports ]
    return serports

def find_serport_by_no(serno):
    comports = serial.tools.list_ports.comports()
    serport = None
    for port in comports:
        cur_serno = port.serial_number
        cur_dev = port.device
        cur_loc = port.location
        if cur_serno is None:
            continue
        if sys.platform == "win32":
            if (serno + 'B') == cur_serno:
                serport = cur_dev
                break
        else:
            if serno != cur_serno:
                continue
            # serial is the second device of the composite device
            if cur_loc.endswith(".1"):
                serport = cur_dev
                break
    # serport founded
    return serport

def find_most_possible_serport():
    serports = find_possible_serports()
    if len(serports) > 0:
        # sort the ports
        serports.sort()
        # get the biggest port
        # for /dev/ttyUSB0, /dev/ttyUSB1, get /dev/ttyUSB1
        # for COM16, COM17, get COM17
        return serports[-1]
    else:
        return None

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
                and isinstance(merge_dct[k], Mapping)):
            dict_merge(dct[k], merge_dct[k])
        else:
            dct[k] = merge_dct[k]

def get_make_csv(app, config):
    make_options = " "
    SUPPORT_KEYS = ["SOC", "BOARD", "CORE", "DOWNLOAD", "VARIANT", \
        "BENCH_UNIT", "BENCH_FLAGS", "ARCH_EXT", "STDCLIB", "SILENT", "V"]
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

def kill_async_subprocess(proc):
    startticks = time.time()
    if proc is not None:
        try:
            kill_sig = signal.SIGTERM
            if sys.platform != "win32":
                kill_sig = signal.SIGKILL

            print("Try to Kill process id %d now" %(proc.pid))
            parent_proc = psutil.Process(proc.pid)
            try:
                # This might cause PermissionError: [Errno 1] Operation not permitted: '/proc/1/stat' issue
                child_procs = parent_proc.children(recursive=True)
                for child_proc in child_procs:
                    print("Kill child process %s, pid %d" %(child_proc.name(), child_proc.pid))
                    try:
                        os.kill(child_proc.pid, kill_sig) # kill child process
                    except:
                        continue
            except Exception as exc:
                print("Warning: kill child process failed with %s" %(exc))
            if parent_proc.is_running():
                print("Kill parent process %s, pid %d" %(parent_proc.name(), parent_proc.pid))
                if sys.platform != "win32":
                    try:
                        os.killpg(parent_proc.pid, kill_sig) # kill parent process
                    except:
                        os.kill(parent_proc.pid, kill_sig) # kill parent process
                else:
                    os.kill(parent_proc.pid, kill_sig) # kill parent process
            # kill using process.kill again
            if parent_proc.is_running():
                proc.kill()
        except psutil.NoSuchProcess:
            pass
        except Exception as exc:
            print("Warning: kill process failed with %s" %(exc))
    # show time cost for kill process
    print("kill process used %.2f seconds" %((time.time() - startticks)))
    sys.stdout.flush()
    pass

def kill_subprocess(proc):
    try:
        if proc.poll() is None: # process is still running
            kill_async_subprocess(proc)
    except:
        pass
    pass

def import_module(module_name, file_path):
    if file_path is None or os.path.isfile(file_path) == False:
        return None
    try:
        spec = importlib.util.spec_from_file_location(module_name, file_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
    except:
        module = None
    return module

def import_function(func_name, file_path):
    module_name = "tempmodule_%s" % (random.randint(0, 10000))
    tmpmodule = import_module(module_name, file_path)
    if tmpmodule is None:
        return None
    if func_name not in dir(tmpmodule):
        return None
    return getattr(tmpmodule, func_name)

COMMAND_RUNOK=0
COMMAND_INVALID=1
COMMAND_FAIL=2
COMMAND_INTERRUPTED=3
COMMAND_EXCEPTION=4
COMMAND_NOTAPP=5
COMMAND_TIMEOUT=6
COMMAND_TIMEOUT_READ=7

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
        if logfh:
            # record command run in log file
            logfh.write(("Execute Command %s\n" % (command)).encode())
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
        ret = COMMAND_TIMEOUT
    except Exception as exc:
        print("Unexpected exception happened: %s" %(str(exc)))
        ret = COMMAND_EXCEPTION
    finally:
        kill_subprocess(process)
        if process:
            del process
        if logfh:
            logfh.close()
    cmd_elapsed_ticks = time.time() - startticks
    return ret, cmd_elapsed_ticks

async def run_cmd_and_check_async(command, timeout:int, checks:dict, checktime=time.time(), sdk_check=False, logfile=None, show_output=False, banner_timeout=3):
    logfh = None
    ret = COMMAND_FAIL
    cmd_elapsed_ticks = 0
    if isinstance(command, str) == False:
        return COMMAND_INVALID, cmd_elapsed_ticks
    startticks = time.time()
    process = None
    check_status = False
    pass_checks = checks.get("PASS", [])
    fail_checks = checks.get("FAIL", [])
    def test_in_check(string, checks):
        if type(checks) == list:
            for check in checks:
                if check in string:
                    return True
        return False
    NSDK_CHECK_TAG = get_sdk_checktag()
    if get_sdk_verb_buildmsg():
        print("Checker used: ", checks)
        print("SDK Checker Tag \"%s\", checker enable %s" % (NSDK_CHECK_TAG, sdk_check))
    print("SDK run timeout %s, banner timeout %s" % (timeout, banner_timeout))
    check_finished = False
    start_time = time.time()
    serial_log = ""
    nsdk_check_timeout = banner_timeout
    sdk_checkstarttime = time.time()
    try:
        if isinstance(logfile, str):
            logfh = open(logfile, "wb")
        if sys.platform != "win32":
            # add exec to running command to avoid create a process called /bin/sh -c
            # and if you kill that process it will kill this sh process not the really
            # command process you want to kill
            process = await asyncio.create_subprocess_shell("exec " + command, \
                    stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
        else:
            process = await asyncio.create_subprocess_shell(command, \
                    stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.STDOUT)
        while (time.time() - start_time) < timeout:
            try:
                linebytes = await asyncio.wait_for(process.stdout.readline(), 1)
            except asyncio.TimeoutError:
                if sdk_check == True:
                    linebytes = None
                else:
                    continue
            except KeyboardInterrupt:
                print("Key CTRL-C pressed, command executing stopped!")
                break
            except:
                break
            if linebytes:
                line = str(try_decode_bytes(linebytes)).replace('\r', '')
            else:
                line = ""

            if sdk_check == True:
                if (time.time() - sdk_checkstarttime) > nsdk_check_timeout:
                    print("No SDK banner found in %s s, quit now!" % (nsdk_check_timeout))
                    ret = COMMAND_TIMEOUT
                    check_status = False
                    break
                if line == "":
                    continue
                if show_output:
                    print("XXX Check " + line, end='')
                if NSDK_CHECK_TAG in line:
                    timestr = line.split(NSDK_CHECK_TAG)[-1].strip()
                    cur_time = time.mktime(time.strptime(timestr, "%b %d %Y, %H:%M:%S"))
                    if int(cur_time) >= int(checktime):
                        sdk_check = False
                        line = NSDK_CHECK_TAG + " " + timestr + "\n"
                        serial_log = serial_log + str(line)
            else:
                serial_log = serial_log + str(line)
                if show_output:
                    print(line, end='')
                if check_finished == False:
                    if test_in_check(line, fail_checks):
                        check_status = False
                        check_finished = True
                    if test_in_check(line, pass_checks):
                        check_status = True
                        check_finished = True
                    if check_finished:
                        ret = COMMAND_RUNOK
                        # record another 2 seconds by reset start_time and timeout to 2
                        start_time = time.time()
                        timeout = 1
            if logfh and linebytes:
                logfh.write(linebytes)
            time.sleep(0.01)
    except (KeyboardInterrupt):
        print("Key CTRL-C pressed, command executing stopped!")
        ret = COMMAND_INTERRUPTED
    except Exception as exc:
        print("Unexpected exception happened: %s" %(str(exc)))
        ret = COMMAND_EXCEPTION
    finally:
        # kill this process
        kill_async_subprocess(process)
        if logfh:
            logfh.close()
    cmd_elapsed_ticks = time.time() - startticks
    return check_status, cmd_elapsed_ticks

def run_cmd_and_check(command, timeout:int, checks:dict, checktime=time.time(), sdk_check=False, logfile=None, show_output=False, banner_timeout=30):
    loop = asyncio.get_event_loop()
    try:
        ret, cmd_elapsed_ticks = loop.run_until_complete( \
            run_cmd_and_check_async(command, timeout, checks, checktime, sdk_check, logfile, show_output, banner_timeout))
    except KeyboardInterrupt:
        print("Key CTRL-C pressed, command executing stopped!")
        ret, cmd_elapsed_ticks = False, 0
    finally:
        if sys.platform != "win32":
            os.system("stty echo 2> /dev/null")

    return ret, cmd_elapsed_ticks

def find_files(fndir, pattern, recursive=False):
    fndir = os.path.normpath(fndir)
    files = glob.glob(os.path.join(fndir, pattern), recursive=recursive)
    return files

def get_logfile(appdir, startdir, logdir, logname):
    relpath = os.path.relpath(appdir, startdir)
    _, startdir_basename = os.path.splitdrive(startdir)
    applogdir = os.path.join(os.path.relpath(logdir + os.sep + startdir_basename), relpath)
    applog = os.path.relpath(os.path.join(applogdir, logname))
    applogdir = os.path.dirname(applog)
    if os.path.isdir(applogdir) == False:
        os.makedirs(applogdir)
    return applog

def strtofloat(value):
    fval = 0.0
    try:
        match = re.search(r'[+-]?\d*\.?\d+([Ee][+-]?\d+)?',  value.strip())
        if match:
            fval = float(match.group())
    except:
        pass
    return fval

def check_tool_version(ver_cmd, ver_check):
    vercmd_log = tempfile.mktemp()
    ret, _ = run_command(ver_cmd, show_output=False, logfile=vercmd_log)
    check_sts = False
    verstr = None
    if ret == COMMAND_RUNOK:
        with open(vercmd_log, 'r', errors='ignore') as vlf:
            for line in vlf.readlines():
                if ver_check in line:
                    verstr = line.strip()
                    check_sts = True
                    break
    os.remove(vercmd_log)
    return check_sts, verstr

def get_elfsize(elf):
    sizeinfo = {"text": -1, "data": -1, "bss": -1, "total": -1}
    if os.path.isfile(elf) == False:
        return sizeinfo
    for sizetool in [ "riscv-nuclei-elf-size", "riscv64-unknown-elf-size", "size" ]:
        sizecmd = "%s %s" % (sizetool, elf)
        sizelog = tempfile.mktemp()
        ret, _ = run_command(sizecmd, show_output=False, logfile=sizelog)
        if ret == COMMAND_RUNOK:
            with open(sizelog, "r", errors='ignore') as sf:
                lines = sf.readlines()
                datas = lines[-1].strip().split()
                sizeinfo["text"] = int(datas[0])
                sizeinfo["data"] = int(datas[1])
                sizeinfo["bss"] = int(datas[2])
                sizeinfo["total"] = int(datas[3])
            os.remove(sizelog)
            break
        else:
            os.remove(sizelog)
    return sizeinfo

def merge_config_with_makeopts(config, make_options):
    opt_splits=make_options.strip().split()
    passed_buildcfg = dict()
    for opt in opt_splits:
        if "=" in opt:
            values = opt.split("=")
            # Make new build config
            if (len(values) == 2):
                passed_buildcfg[values[0]] = values[1]
    build_cfg = config.get("build_config", None)
    if build_cfg is None:
        config["build_config"] = passed_buildcfg
    else:
        # update build_config using parsed config via values specified in make_options
        config["build_config"].update(passed_buildcfg)
    return config

# merge config dict and args dict
# args will overwrite config
def merge_config_with_args(config, args_dict):
    if isinstance(config, dict) == False:
        return None
    if isinstance(args_dict, dict) == False:
        return config
    serport = args_dict.get("serport", None)
    baudrate = args_dict.get("baudrate", None)
    make_options = args_dict.get("make_options", None)
    parallel = args_dict.get("parallel", None)
    build_target = args_dict.get("build_target", None)
    run_target = args_dict.get("run_target", None)
    timeout = args_dict.get("timeout", None)
    ncycm = args_dict.get("ncycm", None)
    if isinstance(config, dict) == False:
        return None
    new_config = copy.deepcopy(config)
    if serport or baudrate or run_target:
        run_cfg = new_config.get("run_config", None)
        if run_cfg is None:
            new_config["run_config"] = {"hardware":{}}
        elif "hardware" not in run_cfg:
            new_config["run_config"]["hardware"] = {}
    if serport:
        new_config["run_config"]["hardware"]["serport"] = str(serport)
    if baudrate:
        new_config["run_config"]["hardware"]["serport"] = int(baudrate)
    if run_target:
        new_config["run_config"]["target"] = str(run_target)
    run_target = new_config["run_config"].get("target", "hardware")
    if run_target not in new_config["run_config"]:
        new_config["run_config"][run_target] = dict()
    if ncycm:
        if "ncycm" not in new_config["run_config"]:
            new_config["run_config"]["ncycm"] = dict()
        new_config["run_config"]["ncycm"]["ncycm"] = os.path.abspath(ncycm)

    if timeout: # set timeout
        try:
            timeout = int(timeout)
        except:
            timeout = 60
        new_config["run_config"][run_target]["timeout"] = timeout
    if build_target is not None:
        new_config["build_target"] = build_target
    if parallel is not None:
        new_config["parallel"] = parallel
    if make_options:
        new_config = merge_config_with_makeopts(new_config, make_options)
    return new_config

# merge two config, now is appcfg, another is hwcfg
# hwcfg will overwrite configuration in appcfg
def merge_two_config(appcfg, hwcfg):
    if isinstance(appcfg, dict) == True and isinstance(hwcfg, dict) == False:
        return appcfg
    if isinstance(appcfg, dict) == False and isinstance(hwcfg, dict) == True:
        return hwcfg
    merged_appcfg = copy.deepcopy(appcfg)
    dict_merge(merged_appcfg, hwcfg)
    return merged_appcfg

def set_global_variables(config):
    global SDK_GLOBAL_VARIABLES
    if isinstance(config, dict) == False:
        return False

    if "global_variables" in config:
        dict_merge(SDK_GLOBAL_VARIABLES, config["global_variables"])
        print("Using global variables: %s" % SDK_GLOBAL_VARIABLES)
    return True

def get_app_runresult(apprst):
    if not isinstance(apprst, dict):
        return "unknown", "-"
    if "type" not in apprst:
        return "unknown", "-"
    rsttype = apprst["type"]
    rstvaluedict = apprst.get("value", dict())
    if rstvaluedict and len(rstvaluedict) < 3:
        rstval = ""
        for key in rstvaluedict:
            rstval += "%s : %s;" %(key, rstvaluedict[key])
        rstval = rstval.rstrip(';')
    else:
        rstval = "-"
    return rsttype, rstval

def save_execute_csv(result, csvfile):
    if isinstance(result, dict) == False:
        return False
    csvlines = ["App, buildstatus, runstatus, buildtime, runtime, type, value, total, text, data, bss"]
    for app in result:
        size = result[app]["size"]
        app_status = result[app]["status"]
        app_time = result[app]["time"]
        apprsttype, apprstval = get_app_runresult(result[app].get("result", dict()))
        csvline ="%s, %s, %s, %s, %s, %s, %s, %d, %d, %d, %d" % (app, app_status["build"], \
            app_status.get("run", False), app_time.get("build", "-"), app_time.get("run", "-"), \
            apprsttype, apprstval, size["total"], size["text"], size["data"], size["bss"])
        csvlines.append(csvline)
    display = get_sdk_verb_buildmsg()
    save_csv(csvfile, csvlines, display)
    return True

def save_bench_csv(result, csvfile):
    if isinstance(result, dict) == False:
        return False
    csvlines = ["App, case, buildstatus, runstatus, buildtime, runtime, type, value, total, text, data, bss"]
    for app in result:
        appresult = result[app]
        for case in appresult:
            size = appresult[case]["size"]
            app_status = appresult[case]["status"]
            app_time = appresult[case]["time"]
            apprsttype, apprstval = get_app_runresult(appresult[case].get("result", dict()))
            csvline = "%s, %s, %s, %s, %s, %s, %s, %s, %d, %d, %d, %d" % (app, case, app_status["build"], \
                app_status.get("run", False), app_time.get("build", "-"), app_time.get("run", "-"), \
                apprsttype, apprstval, size["total"], size["text"], size["data"], size["bss"])
            csvlines.append(csvline)
    # save csv file
    display = get_sdk_verb_buildmsg()
    save_csv(csvfile, csvlines, display)
    return True

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

def fix_evalsoc_verilog_ncycm(verilog):
    if os.path.isfile(verilog) == False:
        return ""
    vfct = ""
    with open(verilog, "r", errors='ignore') as vf:
        for line in vf.readlines():
            line = line.replace("@80", "@00").replace("@90", "@08")
            vfct += line
    verilog_new = verilog + ".ncycm"
    with open(verilog_new, "w") as vf:
        vf.write(vfct)
    return verilog_new

PROGRAM_UNKNOWN="unknown"
PROGRAM_BAREBENCH="barebench"
PROGRAM_COREMARK="coremark"
PROGRAM_DHRYSTONE="dhrystone"
PROGRAM_WHETSTONE="whetstone"

def parse_benchmark_compatiable(lines):
    result = None
    program_type = PROGRAM_UNKNOWN
    subtype = PROGRAM_UNKNOWN
    try:
        for line in lines:
            # Coremark
            if "CoreMark" in line:
                program_type = PROGRAM_BAREBENCH
                subtype = PROGRAM_COREMARK
            if "Iterations*1000000/total_ticks" in line:
                value = line.split("=")[1].strip().split()[0]
                result = dict()
                result["CoreMark/MHz"] = strtofloat(value)
            # Dhrystone
            if "Dhrystone" in line:
                program_type = PROGRAM_BAREBENCH
                subtype = PROGRAM_DHRYSTONE
            if "1000000/(User_Cycle/Number_Of_Runs)" in line:
                value = line.split("=")[1].strip().split()[0]
                result = dict()
                result["DMIPS/MHz"] = strtofloat(value)
            # Whetstone
            if "Whetstone" in line:
                program_type = PROGRAM_BAREBENCH
                subtype = PROGRAM_WHETSTONE
            if "MWIPS/MHz" in line:
                value = line.split("MWIPS/MHz")[-1].strip().split()[0]
                result = dict()
                result["MWIPS/MHz"] = strtofloat(value)
    except:
        return program_type, subtype, result
    return program_type, subtype, result

def parse_benchmark_baremetal(lines):
    result = None
    program_type = PROGRAM_UNKNOWN
    subtype = PROGRAM_UNKNOWN
    try:
        unit = "unknown"
        for line in lines:
            stripline = line.strip()
            if "csv," in stripline.lower():
                csv_values = stripline.split(',')
                if len(csv_values) >= 3:
                    key = csv_values[1].strip()
                    value = csv_values[-1].strip()
                    if key.lower() == "benchmark":
                        program_type = PROGRAM_BAREBENCH
                        unit = value
                    else:
                        subtype = key.lower()
                        result = dict()
                        result[unit] = strtofloat(value)
                        break
    except:
        return program_type, subtype, result
    return program_type, subtype, result

def parse_benchmark_baremetal_csv(lines):
    result = None
    program_type = PROGRAM_UNKNOWN
    try:
        result = dict()
        for line in lines:
            stripline = line.strip()
            if "csv," in stripline.lower():
                csv_values = stripline.split(',')
                if len(csv_values) >= 3:
                    key = csv_values[1].strip()
                    value = csv_values[-1].strip()
                    if "BENCH" not in key.upper():
                        result[key] = value
    except:
        return program_type, result
    return program_type, result

def find_index(key, arr):
    try:
        index = arr.index(key)
    except:
        index = -1
    return index

def parse_benchmark_runlog(lines, lgf=""):
    if isinstance(lines, list) == False:
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None
    if len(lines) == 0:
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None
    subtype = ""
    if lgf.strip() == "": # old style
        program_type, subtype, result = parse_benchmark_compatiable(lines)
    else:
        lgf = lgf.replace("\\", "/")
        appnormdirs = os.path.dirname(os.path.normpath(lgf)).replace('\\', '/').split('/')
        if "baremetal/benchmark" in lgf:
            # baremetal benchmark
            program_type, subtype, result = parse_benchmark_baremetal(lines)
            if program_type == PROGRAM_UNKNOWN:
                # fallback to previous parser
                program_type, subtype, result = parse_benchmark_compatiable(lines)
        elif "baremetal/demo_dsp" in lgf:
            program_type, result = parse_benchmark_baremetal_csv(lines)
            program_type = "demo_dsp"
        elif "DSP/Examples/RISCV" in lgf:
            program_type, result = parse_benchmark_baremetal_csv(lines)
            program_type = "nmsis_dsp_example"
            index = find_index("RISCV", appnormdirs)
            if index >= 0:
                subtype = appnormdirs[index + 1]
        elif "DSP/Test" in lgf:
            program_type, result = parse_benchmark_baremetal_csv(lines)
            program_type = "nmsis_dsp_tests"
            index = find_index("Test", appnormdirs)
            if index >= 0:
                subtype = appnormdirs[index + 1]
        elif "NN/Examples/RISCV" in lgf:
            program_type, result = parse_benchmark_baremetal_csv(lines)
            program_type = "nmsis_nn_example"
            index = find_index("RISCV", appnormdirs)
            if index >= 0:
                subtype = appnormdirs[index + 1]
        elif "NN/Tests" in lgf:
            program_type, result = parse_benchmark_baremetal_csv(lines)
            if "full" in appnormdirs:
                program_type = "nmsis_nn_test_full"
                subtype = "full"
            else:
                program_type = "nmsis_nn_test_percase"
                index = find_index("percase", appnormdirs)
                if index >= 0:
                    subtype = appnormdirs[index + 1]
        else:
            program_type, subtype, result = parse_benchmark_compatiable(lines)
    return program_type, subtype, result

def parse_benchmark_use_pyscript(lines, lgf, pyscript):
    if isinstance(lines, list) == False:
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None
    if len(lines) == 0:
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None
    # function should named parse_benchmark
    # function argument and return like parse_benchmark_runlog
    parsefunc = import_function("parse_benchmark", pyscript)
    if parsefunc is None:
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None
    try:
        program_type, subtype, result = parsefunc(lines, lgf)
        return program_type, subtype, result
    except Exception as exc:
        print("ERROR: Parse using %s script error: %s" %(pyscript, exc))
        return PROGRAM_UNKNOWN, PROGRAM_UNKNOWN, None

def check_tool_exist(tool):
    exist = False
    if sys.platform == 'win32':
        if os.system("where %s" % (tool)) == 0:
            exist = True
    else:
        if os.system("which %s" % (tool)) == 0:
            exist = True
    return exist

def find_vivado_cmd():
    for vivado_cmd in ("vivado", "vivado_lab"):
        if sys.platform == 'win32':
            if os.system("where %s" % (vivado_cmd)) == 0:
                return vivado_cmd
        else:
            if os.system("which %s" % (vivado_cmd)) == 0:
                return vivado_cmd
    return None

def datetime_now():
    return datetime.datetime.now().strftime(DATE_FORMATE)

def program_fpga(bit, target):
    if os.path.isfile(bit) == False:
        print("Can't find bitstream in %s" % (bit))
        return False
    print("Try to program fpga bitstream %s to target board %s" % (bit, target))
    sys.stdout.flush()
    FILE_LOCK = os.path.join(get_tmpdir(), FILE_LOCK_NAME)
    # TODO: use portable filelock for win32
    with open(FILE_LOCK, 'w+') as filelock:
        if sys.platform != "win32":
            print("%s, Wait another board's programing fpga to finished" %(datetime_now()))
            fcntl.flock(filelock, fcntl.LOCK_EX)
            # set to 666, in case that other user can't access this file causing exception
            if os.stat(FILE_LOCK).st_uid == os.getuid():
                os.chmod(FILE_LOCK, stat.S_IWGRP | stat.S_IRGRP | stat.S_IWUSR | stat.S_IRUSR | stat.S_IWOTH | stat.S_IROTH)
            print("%s, Has acquired the chance to do fpga programing!" %(datetime_now()))

        vivado_cmd = find_vivado_cmd()
        # check vivado is found or not
        if vivado_cmd == None:
            print("vivado is not found in PATH, please check!")
            return False
        tcl = os.path.join(os.path.dirname(os.path.realpath(__file__)), "program_bit.tcl")
        target = "*%s" % (target)
        progcmd = "%s -mode batch -nolog -nojournal -source %s -tclargs %s %s" % (vivado_cmd, tcl, bit, target)
        tmout = get_sdk_fpga_prog_tmout()
        if sys.platform != 'win32' and tmout is not None and tmout.strip() != "":
            print("Timeout %s do fpga program" % (tmout))
            progcmd = "timeout --foreground -s SIGKILL %s %s" % (tmout, progcmd)
        print("Do fpga program using command: %s" % (progcmd))
        sys.stdout.flush()
        ret = os.system(progcmd)
        sys.stdout.flush()
        if ret != 0:
            print("Program fpga bit failed, error code %d" % ret)
            return False
        print("Program fpga bit successfully")
        return True

def find_fpgas():
    vivado_cmd = find_vivado_cmd()
    if vivado_cmd == None:
        print("vivado is not found in PATH, please check!")
        return dict()
    tcl = os.path.join(os.path.dirname(os.path.realpath(__file__)), "find_devices.tcl")
    sys.stdout.flush()
    tmp_log = tempfile.mktemp()
    os.system("%s -mode batch -nolog -nojournal -source %s -notrace > %s" % (vivado_cmd, tcl, tmp_log))
    sys.stdout.flush()
    fpgadevices = dict()
    with open(tmp_log, "r", errors='ignore') as tf:
        for line in tf.readlines():
            line = line.strip()
            if line.startswith("CSV,") == False:
                continue
            splits = line.split(",")
            if len(splits) != 3:
                continue
            fpga_serial = "/".join(splits[1].split("/")[2:])
            fpgadevices[fpga_serial] = splits[2].strip()
    return fpgadevices

def check_serial_port(serport):
    if serport in find_possible_serports():
        return True
    return False

def modify_openocd_cfg(cfg, ftdi_serial):
    cfg_bk = cfg + ".backup"
    if (os.path.isfile(cfg)) == False:
        return False
    if os.path.isfile(cfg_bk) == True:
        print("Restore openocd cfg %s" %(cfg))
        shutil.copyfile(cfg_bk, cfg)
    else:
        print("Backup openocd cfg %s" %(cfg))
        shutil.copyfile(cfg, cfg_bk)
    found = False
    contents = []
    index = 0
    with open(cfg, 'r', errors='ignore') as cf:
        contents = cf.readlines()
        for line in contents:
            if line.strip().startswith("transport select"):
                found = True
                break
            index += 1
    if found == False:
        return False
    if sys.platform == 'win32':
        ftdi_serial = "%sA" % (ftdi_serial)
    contents.insert(index, "ftdi_serial %s\ntcl_port disabled\ntelnet_port disabled\n" %(ftdi_serial))
    with open(cfg, 'w') as cf:
        contents = "".join(contents)
        cf.write(contents)
    return True

GL_CPUCFGs = os.path.join(SCRIPT_DIR,  "configs", "cpu")
def gen_runcfg(cpucfg, runcfg, buildconfig=dict()):
    _, cpucfgdict = load_json(cpucfg)
    _, runcfgdict = load_json(runcfg)
    if cpucfgdict is None:
        return  { "build_configs": { "default": {} } }
    if runcfgdict is None:
        return cpucfgdict
    matrixcfgs = runcfgdict.get("matrix", None)
    expectedcfg = runcfgdict.get("expected", dict())
    expectedscfg = runcfgdict.get("expecteds", dict())
    finalruncfg = copy.deepcopy(cpucfgdict)
    # merge buildconfig
    finalruncfg["build_config"] = merge_two_config(finalruncfg.get("build_config", dict()), buildconfig)
    finalruncfg["expected"] = merge_two_config(finalruncfg.get("expected", dict()), expectedcfg)
    finalruncfg["expecteds"] = merge_two_config(finalruncfg.get("expecteds", dict()), expectedscfg)
    if matrixcfgs is None:
        return finalruncfg
    bcfgs = cpucfgdict.get("build_configs", dict())
    newbcfgs = dict()
    for bkey in bcfgs:
        for key in matrixcfgs:
            cfgkey = "%s-%s" % (bkey, key)
            newbcfgs[cfgkey] = merge_two_config(bcfgs[bkey], matrixcfgs[key])
    if len(newbcfgs) > 1:
        finalruncfg["build_configs"] = newbcfgs
    else:
        finalruncfg["build_configs"] = bcfgs
    return finalruncfg

def gen_coreruncfg(core, runcfg, choice="mini", buildconfig=dict(), casedir=None):
    cpucfgsloc = os.path.join(GL_CPUCFGs, choice)
    if casedir is not None:
        tmp = os.path.join(casedir, choice)
        if os.path.isdir(tmp) == True:
            cpucfgsloc = os.path.realpath(tmp)
    print("Use cpu configs in location %s directory" % (cpucfgsloc))
    cpucfg = os.path.join(cpucfgsloc, "%s.json" % (core))
    return gen_runcfg(cpucfg, runcfg, buildconfig)

def gen_coreruncfg_custom(core, runcfg, customcfgdir, buildconfig=dict()):
    cpucfg = os.path.join(customcfgdir, "%s.json" % (core))
    return gen_runcfg(cpucfg, runcfg, buildconfig)

def gen_runyaml(core, locs, fpga_serial, ftdi_serial, cycm, fpgabit, boardtype, ocdcfg, appcfg, hwcfg):
    runyaml = { "runcfg": {"runner": "fpga"},
                "fpga_runners": { core: {
                    "board_type": boardtype, "fpga_serial": fpga_serial,
                    "ftdi_serial": ftdi_serial, "serial_port": ""}
                },
                "ncycm_runners": { core: {
                    "model": cycm if cycm else "" }
                },
                "configs": { core: {
                    "fpga": boardtype, "bitstream": fpgabit,
                    "ncycm": core, "openocd_cfg": ocdcfg,
                    "appcfg": appcfg, "hwcfg": hwcfg }
                },
                "environment": {
                    "fpgaloc": locs.get("fpgaloc", ""),
                    "ncycmloc": locs.get("ncycmloc", ""),
                    "cfgloc": locs.get("cfgloc", "")
                }
            }
    if cycm is not None:
        runyaml["runcfg"]["runner"] = "ncycm"
    return runyaml
