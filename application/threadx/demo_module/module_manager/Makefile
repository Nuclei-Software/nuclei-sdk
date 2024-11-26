TARGET = threadx_module_manager
RTOS = ThreadX
THREADX_MODULE := 1

# define TX_INCLUDE_USER_DEFINE_FILE to include user defines in tx_user.h
# -fno-tree-tree-merge is required for optimize level > O1
# OS/ThreadX/common/src/tx_mutex_delete.c code compiled is not correct
COMMON_FLAGS := -O2 -fno-tree-tail-merge -DTX_INCLUDE_USER_DEFINE_FILE

DOWNLOAD := sram

CORE ?= n900fd

QEMU_OPT += -device loader,file=../module_sample/threadx_module_sample.bin,addr=0xA0400000

NUCLEI_SDK_ROOT = ../../../..

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base

# Load in gdb
# restore ../module_sample/threadx_module_sample.bin binary 0xA0400000

cleanmodule:
	make -C ../module_sample RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI) clean

module:
	make -C ../module_sample RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI) bin

.PHONY: cleanmodule module
