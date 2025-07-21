# RT-Thread building script for component
# Nuclei 100 series not yet supported by RT-Thread build system
# please take a try with the RT-Thread Nano integrated in Nuclei N100 SDK

Import('rtconfig')
Import('RTT_ROOT')
import os
import re
import rtconfig

from building import *

BUILTIN_ALL_DOWNLOADED_MODES = {
    "evalsoc": ("ilm", "flash", "flashxip", "ddr", "sram")
}

DEFAULT_DOWNLOAD_MODES = ("ilm", "flashxip", "ddr", "sram", "sramxip")

default_arch_abi = ("rv32imc", "ilp32")
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
rtconfig.DEVICE = build_core_options

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

src += Glob(SoC_Common + '/Source/Stubs/newlib/*.c')
src += Glob(SoC_Common + '/Source/GCC/*.S')

src += Glob(SoC_Board + '/Source/*.c')

CPPPATH = [ cwd + '/NMSIS/Core/Include',
            cwd + '/' + SoC_Common + '/Include',
            cwd + '/' + SoC_Common + '/Source/Stubs/newlib',
            cwd + '/' + SoC_Board + '/Include']

CPPDEFINES = [ '-DDOWNLOAD_MODE={}'.format(DOWNLOAD_MODE),
               '-DDOWNLOAD_MODE_STRING=\"{}\"'.format(build_download_mode_upper),
               '-DRTOS_RTTHREAD', '-DNUCLEI_BANNER=0' ]

extra_flags = build_core_options
extra_lflags = "{} -T {}".format(build_core_options, build_ldscript)


group = DefineGroup('nuclei_n100_sdk', src, depend = [''], \
    CCFLAGS=extra_flags, ASFLAGS=extra_flags, LINKFLAGS=extra_lflags, \
    CPPPATH = CPPPATH, CPPDEFINES=CPPDEFINES, LIBPATH=LIBPATH)

Return('group')
