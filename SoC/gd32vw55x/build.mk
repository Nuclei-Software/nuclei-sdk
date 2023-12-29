##### Put your SoC build configurations below #####
CORE := n307fd
ARCH_EXT ?= _zba_zbb_zbc_zbs_xxldspn1x
DOWNLOAD ?= flashxip

BOARD ?= gd32vw553h_eval
# Variant for Board or SoC
VARIANT ?=
# Please overwrite following variable in Makefile of your application
# System Clock Frequency
# 16Mhz or 48Mhz or 160MHz in pure int value such as 160000000
# leave empty will use default 160MHz
SYSCLK ?=
# Clock source
## hxtal: using high speed crystal oscillator (HXTAL)
## irc16m: using internal 16MHz RC oscillator (IRC16M)
## leave empty will use default settings in system_gd32vw55x.c
CLKSRC ?=
# Don't set HXTAL_VALUE unless you know what you are doing
# when not empty it will define macro HXTAL_VALUE
# see gd32vw55x.h
HXTAL_VALUE ?=

# even though it was set with a command argument
DEFAULT_BOARD := gd32vw553h_eval

NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(BOARD)
NUCLEI_SDK_SOC_COMMON := $(NUCLEI_SDK_SOC)/Common

VALID_NUCLEI_SDK_SOC_BOARD := $(wildcard $(NUCLEI_SDK_SOC_BOARD))
ifeq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
$(warning $(BOARD) for $(SOC) doesn't exist, change it to default $(DEFAULT_BOARD)!)
override NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(DEFAULT_BOARD)
override VALID_NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC_BOARD)
endif

# If DOWNLOAD passed is ilm, force it to sram
ifeq ($(DOWNLOAD),ilm)
override DOWNLOAD := sram
endif

OPENOCD_CFG ?= $(NUCLEI_SDK_SOC_BOARD)/openocd_gd32vw55x.cfg
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_gd32vw55x_$(DOWNLOAD).ld

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
RISCV_ARCH ?= $(TEMP_RISCV_ARCH)$(ARCH_EXT)
RISCV_ABI ?= $(word 2, $(CORE_ARCH_ABI))
RISCV_TUNE ?= $(word 3, $(CORE_ARCH_ABI))

##### Extra compiler options
ifneq ($(SYSCLK),)
COMMON_FLAGS += -DSYSTEM_CLOCK=$(SYSCLK)
endif
ifneq ($(CLKSRC),)
COMMON_FLAGS += -DSYSCLK_USING_$(call uc, $(CLKSRC))
endif
ifneq ($(HXTAL_VALUE),)
COMMON_FLAGS += -DHXTAL_VALUE=$(HXTAL_VALUE)
endif

##### Put your Source code Management configurations below #####

INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include

C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers

# If semihosting is enabled, no stub function is needed
ifeq ($(SEMIHOST),)
ifneq ($(findstring libncrt,$(STDCLIB)),)
# semihosting currently not ported to support libncrt
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/libncrt
else ifneq ($(findstring newlib,$(STDCLIB)),)
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/newlib
else
# no stubs will be used
endif
endif

ASM_SRCS += $(NUCLEI_SDK_SOC_COMMON)/Source/GCC/startup_gd32vw55x.S \
		$(NUCLEI_SDK_SOC_COMMON)/Source/GCC/intexc_gd32vw55x.S

# Add extra board related source files and header files
ifneq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
INCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Include
C_SRCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Source
endif
