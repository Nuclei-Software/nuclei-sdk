##### Put your SoC build configurations below #####

BOARD ?= gd32vf103v_rvstar
# Variant for Board or SoC
VARIANT ?=
# Please overwrite following variable in Makefile of your application
# System Clock Frequency
# 108MHz in pure int value
SYSCLK ?= 108000000
# Clock source
CLKSRC ?= hxtal
# Don't set HXTAL_VALUE unless you know what you are doing
# when not empty it will define macro HXTAL_VALUE
# see SoC/gd32vf103/Common/Include/gd32vf103_rcu.h
HXTAL_VALUE ?=
# Select USB host or device driver
# host: select host driver
# device: select device driver
# both: select both
USB_DRIVER ?=

# override DOWNLOAD and CORE variable for GD32VF103 SoC
# even though it was set with a command argument
override CORE := n203
override DOWNLOAD := flashxip
DEFAULT_BOARD := gd32vf103v_rvstar

NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(BOARD)
NUCLEI_SDK_SOC_COMMON := $(NUCLEI_SDK_SOC)/Common

VALID_NUCLEI_SDK_SOC_BOARD := $(wildcard $(NUCLEI_SDK_SOC_BOARD))
ifeq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
$(warning $(BOARD) for $(SOC) doesn't exist, change it to default $(DEFAULT_BOARD)!)
override NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(DEFAULT_BOARD)
override VALID_NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC_BOARD)
endif

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

RISCV_ARCH ?= rv32imac
RISCV_ABI ?= ilp32

# Handle QEMU Emulation
# NOTE: QEMU emulation for gd32vf103 is no longer supported after nuclei release >= 2023.06
QEMU_MACHINE := ${BOARD}
QEMU_CPU := nuclei-n203

##### Extra compiler options
COMMON_FLAGS += -DSYSTEM_CLOCK=$(SYSCLK)
COMMON_FLAGS += -DSYSCLK_USING_$(call uc, $(CLKSRC))
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

# GD32VF103 USB Driver Handling
USBDRV_ROOT := $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers/Usb
ifeq ($(USB_DRIVER),both)
INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include/Usb
C_SRCDIRS += $(USBDRV_ROOT)
else ifeq ($(USB_DRIVER),host)
INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include/Usb
C_SRCS += $(USBDRV_ROOT)/drv_usb_core.c \
		  $(USBDRV_ROOT)/drv_usb_host.c \
		  $(USBDRV_ROOT)/drv_usbh_int.c \
		  $(USBDRV_ROOT)/usbh_*.c
else ifeq ($(USB_DRIVER),device)
INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include/Usb
C_SRCS += $(USBDRV_ROOT)/drv_usb_core.c \
		  $(USBDRV_ROOT)/drv_usb_dev.c \
		  $(USBDRV_ROOT)/drv_usbd_int.c \
		  $(USBDRV_ROOT)/usbd_*.c
endif

ASM_SRCS += $(NUCLEI_SDK_SOC_COMMON)/Source/GCC/startup_gd32vf103.S \
		$(NUCLEI_SDK_SOC_COMMON)/Source/GCC/intexc_gd32vf103.S

# Add extra board related source files and header files
ifneq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
INCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Include
C_SRCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Source
endif
