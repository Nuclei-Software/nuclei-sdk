##
#### SoC Build Settings Notice
# The following variables need be defined according to your SoC and Board
# BOARD: MUST, set this variable to one of the valid folder basename under $(NUCLEI_SDK_SOC)/Board/
# CORE: MUST, set this variable to one of SUPPORTED_CORES defined in $(NUCLEI_SDK_BUILD)/Makefile.core
# VARIANT: OPTIONAL, set this variable to consecutive characters better less than 8 chars, such as lite, v1, v2.
#          It can be used to define different variant of similar board
# NUCLEI_SDK_SOC_BOARD: MUST, set this variable to $(NUCLEI_SDK_SOC)/Board/$(BOARD), this means the
#                       base folder for selected BOARD
# NUCLEI_SDK_SOC_COMMON: MUST, set this variable to $(NUCLEI_SDK_SOC)/Common, this means the base
#                       folder for selected SOC
# OPENOCD_CFG: MUST, set this variable to openocd configuration file used for the Board or SoC
# LINKER_SCRIPT: MUST, set this variable to linker script file for selected board and download mode
# RISCV_ARCH: MUST, set this variable to the riscv arch you are using, see $(NUCLEI_SDK_BUILD)/Makefile.core
# RISCV_ABI: MUST, set this variable to the riscv abi you are using, see $(NUCLEI_SDK_BUILD)/Makefile.core
# RISCV_TUNE: Optional, set this variable to the mtune you are using, see $(NUCLEI_SDK_BUILD)/Makefile.core
# If your SoC used a fixed BOARD, CORE or DOWNLOAD mode, please use override to define a fixed variable value, eg.
# override CORE := n205
# override DOWNLOAD := flashxip
# override BOARD := nuclei_fpga_eval
#### Source Code Management Notice
# you need to declare the following items:
# 1. C/ASM/CPP source code folders which need to be compiled
# 2. Some C/ASM/CPP source code files whihc need to be compiled
# You can use these makefile variables defined in $(NUCLEI_SDK_BUILD)/Makefile.files to achieve this, for details,
# please check https://doc.nucleisys.com/nuclei_sdk/develop/buildsystem.html#build-related-makefile-variables-used-only-in-application-makefile
##

##### Put your SoC build configurations below #####

override BOARD := nuclei_fpga_eval
include $(NUCLEI_SDK_SOC)/cpufeature.mk

JTAGSN ?=

NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(BOARD)
NUCLEI_SDK_SOC_COMMON := $(NUCLEI_SDK_SOC)/Common

OPENOCD_CFG ?= $(NUCLEI_SDK_SOC_BOARD)/openocd_evalsoc.cfg

LDFLAGS += -L $(NUCLEI_SDK_SOC_BOARD)/Source/GCC
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_evalsoc_$(DOWNLOAD).ld
MAKEFILE_PREREQS += $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/evalsoc.memory

# File existence check for OPENOCD_CFG and LINKER_SCRIPT
ifeq ($(wildcard $(OPENOCD_CFG)),)
$(error The openocd configuration file $(OPENOCD_CFG) for $(SOC) doesn't exist, please check!)
endif
# Allow non-existance of LINKER_SCRIPT, it might be generated
ifeq ($(wildcard $(LINKER_SCRIPT)),)
$(warning The link script file $(LINKER_SCRIPT) for $(SOC) doesn't exist, please check!)
endif

# if JTAGSN is not empty, pass it via openocd command
ifneq ($(JTAGSN),)
OPENOCD_CMD_ARGS += set JTAGSN $(JTAGSN);
endif

# If using generated cpu configs done by nuclei_gen
ifeq ($(CPU_CONFIG_K),1)
COMMON_FLAGS += -DHAS_AUTOGEN_CPUCFG
endif

# Set RISCV_ARCH and RISCV_ABI
CORE_UPPER := $(call uc, $(CORE))
include $(NUCLEI_SDK_BUILD)/Makefile.core
# you can override SUPPORTED_CORES defined in Makefile.core to limit the COREs used in this SoC
# eg. override SUPPORTED_CORES := n305 n307

CORE_ARCH_ABI := $($(CORE_UPPER)_CORE_ARCH_ABI)
# Check whether CORE is in SUPPORTED_CORES
ifeq ($(filter $(CORE), $(SUPPORTED_CORES)),)
$(error Here we only support these cores: $(SUPPORTED_CORES))
endif
# Check whether CORE_ARCH_ABI is presented for CORE
ifneq ($(words $(wordlist 1, 2, $(CORE_ARCH_ABI))), 2)
$(error No correct CORE_ARCH_ABI setting for CORE=$(CORE) found in $(realpath $(NUCLEI_SDK_BUILD)/Makefile.core))
endif

# Handle Nuclei RISC-V ARCH/ABI/CMODEL/TUNE
## ARCH_EXT could be combination of in order of bkpv, legal combination is list as below:
## bp: Bitmanip and Packed SIMD Extension present
## bpv: Bitmanip, Packed SIMD and Vector Extension present
## bkpv: Bitmanip, Packed SIMD, Scalar Cryptography and Vector Extension present
### If zc extension passed in ARCH_EXT, remove c in ARCH
TEMP_RISCV_ARCH := $(word 1, $(CORE_ARCH_ABI))
ifneq ($(findstring zc,$(ARCH_EXT)),)
RISCV_ARCH ?= $(TEMP_RISCV_ARCH:c=)$(ARCH_EXT)
else
RISCV_ARCH ?= $(TEMP_RISCV_ARCH)$(ARCH_EXT)
endif
RISCV_ABI ?= $(word 2, $(CORE_ARCH_ABI))
RISCV_TUNE ?= $(word 3, $(CORE_ARCH_ABI))

# Handle QEMU Emulation
QEMU_MACHINE ?= nuclei_evalsoc,download=$(DOWNLOAD)
QEMU_ARCHEXT ?= ${ARCH_EXT}
QEMU_CPU ?= nuclei-$(CORE),ext=$(QEMU_ARCHEXT)
ifneq ($(SEMIHOST),)
QEMU_OPT += -semihosting
endif

##### Put your Source code Management configurations below #####

INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include

C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers

# If semihosting is enabled, no stub function is needed
ifeq ($(SEMIHOST),)
ifneq ($(findstring newlib,$(STDCLIB)),)
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/newlib
else
# no stubs will be used
endif
endif

ifneq ($(findstring libncrt,$(STDCLIB)),)
# semihosting currently not ported to support libncrt
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/libncrt
endif

ASM_SRCS += $(NUCLEI_SDK_SOC_COMMON)/Source/GCC/startup_evalsoc.S \
		$(NUCLEI_SDK_SOC_COMMON)/Source/GCC/intexc_evalsoc.S

# Add extra board related source files and header files
VALID_NUCLEI_SDK_SOC_BOARD := $(wildcard $(NUCLEI_SDK_SOC_BOARD))
ifneq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
INCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Include
C_SRCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Source
endif
