TARGET = threadx_smpdemo
RTOS = ThreadX

SMP := 2

DOWNLOAD ?= sram

STACKSZ ?= 4K

# define TX_INCLUDE_USER_DEFINE_FILE to include user defines in tx_user.h
COMMON_FLAGS := -O2 -DTX_INCLUDE_USER_DEFINE_FILE

# -fno-tree-tail-merge option is required with >O1 for ThreadX source code correct compiling for gcc
# eg. OS/ThreadX/common/src/tx_mutex_delete.c
-include toolchain_$(TOOLCHAIN).mk

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
