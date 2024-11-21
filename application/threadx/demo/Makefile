TARGET = threadx_demo
RTOS = ThreadX

# define TX_INCLUDE_USER_DEFINE_FILE to include user defines in tx_user.h
# -fno-tree-tail-merge option is required with >O1 for ThreadX source code correct compiling
# eg. OS/ThreadX/common/src/tx_mutex_delete.c
COMMON_FLAGS := -O2 -fno-tree-tail-merge -DTX_INCLUDE_USER_DEFINE_FILE

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
