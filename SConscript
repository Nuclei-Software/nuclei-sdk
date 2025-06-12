# RT-Thread building script for component

Import('rtconfig')
Import('RTT_ROOT')
import os
import re
import rtconfig

from building import *

BUILTIN_ALL_DOWNLOADED_MODES = {
    "gd32vf103": ("flashxip"),
    "evalsoc": ("ilm", "flash", "flashxip", "ddr", "sram")
}

DEFAULT_DOWNLOAD_MODES = ("ilm", "flashxip", "ddr", "sram", "sramxip")

default_arch_abi = ("rv32imac", "ilp32")
cwd = GetCurrentDir()
FRAMEWORK_DIR = cwd

def parse_nuclei_soc_predefined_cores(core_mk):
    if not os.path.isfile(core_mk):
        return dict()
    core_arch_abis = dict()
    core_arch_abi_re = re.compile(r'^([A-Z]+\d+[A-Z]*)_CORE_ARCH_ABI\s*=\s*(rv\d+\w*)\s+(i*lp\d+\w*)')
    with open(core_mk, "r") as core_mk_file:
        for line in core_mk_file.readlines():
            line = line.strip()
            matches = core_arch_abi_re.match(line)
            if matches:
                core_lower = matches.groups()[0].lower()
                core_arch_abis[core_lower] = (matches.groups()[1:3])
    return core_arch_abis


NUCLEI_SOC_CORES_MK = os.path.join(cwd, "Build/Makefile.core")
core_arch_abis = parse_nuclei_soc_predefined_cores(NUCLEI_SOC_CORES_MK)

# Get configurations from rtconfig
build_soc = getattr(rtconfig, "NUCLEI_SDK_SOC").lower().strip()
build_board = getattr(rtconfig, "NUCLEI_SDK_BOARD").lower().strip()
build_download_mode = getattr(rtconfig, "NUCLEI_SDK_DOWNLOAD", "").lower().strip()
build_core = getattr(rtconfig, "NUCLEI_SDK_CORE", "").lower().strip()
build_march = getattr(rtconfig, "NUCLEI_SDK_RISCV_ARCH", "").lower().strip()
build_mabi = getattr(rtconfig, "NUCLEI_SDK_RISCV_ABI", "").lower().strip()
build_mcmodel = getattr(rtconfig, "NUCLEI_SDK_RISCV_MCMODEL", "medany").lower().strip()
build_ldscript = getattr(rtconfig, "NUCLEI_SDK_LDSCRIPT", "").lower().strip()

# Backward compatibility with previous Nuclei SDK releases
if build_soc == "hbird":
    print("Warning! Since Nuclei SDK 0.3.1, SoC hbird is renamed to demosoc, please change NUCLEI_SDK_SOC to demosoc!")
    build_soc = "demosoc"
    if build_board == "hbird_eval":
        print("Warning! Since Nuclei SDK 0.3.1, Board hbird_eval is renamed to nuclei_fpga_eval, please change NUCLEI_SDK_BOARD to nuclei_fpga_eval!")
        build_board = "nuclei_fpga_eval"
if build_soc == "demosoc":
    print("Warning! Since Nuclei SDK 0.5.0, SoC demosoc is removed, please change NUCLEI_SDK_SOC to evalsoc!")
    build_soc = "evalsoc"

if not build_march and not build_mabi and build_core in core_arch_abis:
    build_march, build_mabi = core_arch_abis[build_core]
else:
    if not build_mabi or not build_march:
        build_march, build_mabi = default_arch_abi
        print("No mabi and march specified in rtconfig.py, use default -march=%s -mabi=%s!" % (build_march, build_mabi))

if build_soc in BUILTIN_ALL_DOWNLOADED_MODES:
    supported_download_modes = BUILTIN_ALL_DOWNLOADED_MODES[build_soc]
else:
    if os.path.isfile(os.path.join(cwd, build_soc, "build.mk")) == False:
        print("SoC={} is not supported in Nuclei SDK".format(build_soc))
        exit(0)
    else:
        supported_download_modes = DEFAULT_DOWNLOAD_MODES

SoC_Common = 'SoC/{}/Common'.format(build_soc)
SoC_Board = 'SoC/{}/Board/{}'.format(build_soc, build_board)

build_core_options = " -march=%s -mabi=%s -mcmodel=%s " % (build_march, build_mabi, build_mcmodel)

rtconfig.NUCLEI_SDK_OPENOCD_CFG = os.path.join(FRAMEWORK_DIR, \
    "SoC", build_soc, "Board", build_board, "openocd_{}.cfg".format(build_soc))

if build_soc == "evalsoc":
    if build_download_mode not in supported_download_modes:
        # If build.download not defined for Eval SoC, use default "ILM"
        chosen_download_mode = "ilm" if len(supported_download_modes) == 0 else supported_download_modes[0]
        print("Download mode %s is not supported for SOC %s, use default download mode %s" \
             % (build_download_mode, build_soc, chosen_download_mode))
        build_download_mode = chosen_download_mode
else:
    if build_download_mode not in supported_download_modes:
        chosen_download_mode = "flashxip" if len(supported_download_modes) == 0 else supported_download_modes[0]
        print("Download mode %s is not supported for SOC %s, use default download mode %s" \
             % (build_download_mode, build_soc, chosen_download_mode))
        build_download_mode = chosen_download_mode

print("Supported downloaded modes for board %s are %s, chosen downloaded mode is %s" \
    % (build_board, supported_download_modes, build_download_mode))

if not build_ldscript:
    ld_script = "gcc_%s_%s.ld" % (
        build_soc, build_download_mode) if build_download_mode else "gcc_%s.ld" % build_soc
    build_ldscript = os.path.join(
        FRAMEWORK_DIR, "SoC", build_soc, "Board", build_board, "Source", "GCC", ld_script)
else:
    print("Use user defined ldscript %s" % build_ldscript)

# Use correct downloaded modes
DOWNLOAD_MODE = "DOWNLOAD_MODE_%s" % build_download_mode.upper()
build_download_mode_upper = build_download_mode.upper()

src = Glob(SoC_Common + '/Source/*.c')
src += Glob(SoC_Common + '/Source/Drivers/*.c')
if build_soc == "gd32vf103":
    src += Glob(SoC_Common + '/Source/Drivers/Usb/*.c')
src += Glob(SoC_Common + '/Source/Stubs/newlib/*.c')
src += Glob(SoC_Common + '/Source/GCC/*.S')

src += Glob(SoC_Board + '/Source/*.c')

CPPPATH = [ cwd + '/NMSIS/Core/Include',
            cwd + '/NMSIS/DSP/Include',
            cwd + '/NMSIS/DSP/PrivateInclude',
            cwd + '/NMSIS/NN/Include',
            cwd + '/' + SoC_Common + '/Include',
            cwd + '/' + SoC_Common + '/Source/Stubs/newlib',
            cwd + '/' + SoC_Board + '/Include']

LIBPATH = [ cwd + '/NMSIS/Library/DSP/GCC',
            cwd + '/NMSIS/Library/NN/GCC' ]

if build_soc == "gd32vf103":
    CPPPATH.append(cwd + '/' + SoC_Common + '/Include/Usb')

CPPDEFINES = [ '-DDOWNLOAD_MODE={}'.format(DOWNLOAD_MODE),
               '-DDOWNLOAD_MODE_STRING=\"{}\"'.format(build_download_mode_upper),
               '-DRTOS_RTTHREAD', '-DNUCLEI_BANNER=0' ]

# Flash download mode vector table need to remapped
if build_download_mode_upper == "FLASH":
    CPPDEFINES.extend(['-DVECTOR_TABLE_REMAPPED'])

extra_flags = build_core_options
extra_lflags = "{} -L {} -T {}".format(build_core_options, os.path.dirname(build_ldscript), build_ldscript)

# rtconfig.CFLAGS = "{} {}".format(build_core_options, rtconfig.CFLAGS)
# rtconfig.AFLAGS = "{} {}".format(build_core_options, rtconfig.AFLAGS)
# rtconfig.LFLAGS = "{} {} -T {}".format(build_core_options, rtconfig.LFLAGS, build_ldscript)

# print(rtconfig.CFLAGS)
# print(rtconfig.AFLAGS)
# print(rtconfig.LFLAGS)

group = DefineGroup('nuclei_sdk', src, depend = [''], \
    CCFLAGS=extra_flags, ASFLAGS=extra_flags, LINKFLAGS=extra_lflags, \
    CPPPATH = CPPPATH, CPPDEFINES=CPPDEFINES, LIBPATH=LIBPATH)

Return('group')
