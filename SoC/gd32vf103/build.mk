##### Put your SoC build configurations below #####

BOARD ?= gd32vf103v_rvstar
# Variant for Board or SoC
VARIANT ?=

# override DOWNLOAD and CORE variable for GD32VF103 SoC
# even though it was set with a command argument
override CORE := n205
override DOWNLOAD := flashxip

NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(BOARD)
NUCLEI_SDK_SOC_COMMON := $(NUCLEI_SDK_SOC)/Common

OPENOCD_CFG ?= $(NUCLEI_SDK_SOC_BOARD)/openocd_gd32vf103.cfg

# Misc fixup for board variant
ifeq ($(BOARD), gd32vf103c_longan_nano)
# Board gd32vf103c_longan_nano
ifeq ($(VARIANT), lite)
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_gd32vf103x8_flashxip.ld
else
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_gd32vf103xb_flashxip.ld
endif
else # other boards except gd32vf103c_longan_nano
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_gd32vf103_$(DOWNLOAD).ld
endif

# File existence check for OPENOCD_CFG and LINKER_SCRIPT
ifeq ($(wildcard $(OPENOCD_CFG)),)
$(error The openocd configuration file $(OPENOCD_CFG) for $(SOC) doesn't exist, please check!)
endif
ifeq ($(wildcard $(LINKER_SCRIPT)),)
$(error The link script file $(LINKER_SCRIPT) for $(SOC) doesn't exist, please check!)
endif

# Set RISCV_ARCH and RISCV_ABI
CORE_UPPER := $(call uc, $(CORE))

RISCV_ARCH ?= rv32imac
RISCV_ABI ?= ilp32

##### Put your Source code Management configurations below #####

INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include

C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers

ifneq ($(findstring libncrt,$(STDCLIB)),)
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/libncrt
else ifneq ($(findstring newlib,$(STDCLIB)),)
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Stubs/newlib
else
# no stubs will be used
endif

ifeq ($(USB_DRV_SUPPORT), 1)
INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include/Usb
C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers/Usb
endif

ASM_SRCS += $(NUCLEI_SDK_SOC_COMMON)/Source/GCC/startup_gd32vf103.S \
		$(NUCLEI_SDK_SOC_COMMON)/Source/GCC/intexc_gd32vf103.S

# Add extra board related source files and header files
VALID_NUCLEI_SDK_SOC_BOARD := $(wildcard $(NUCLEI_SDK_SOC_BOARD))
ifneq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
INCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Include
C_SRCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Source
endif
