TARGET = threadx_module_manager
RTOS = ThreadX
THREADX_MODULE := 1

# define TX_INCLUDE_USER_DEFINE_FILE to include user defines in tx_user.h
# -fno-tree-tree-merge is required for optimize level > O1
# OS/ThreadX/common/src/tx_mutex_delete.c code compiled is not correct
COMMON_FLAGS := -O2 -fno-tree-tail-merge -DTX_INCLUDE_USER_DEFINE_FILE

NUCLEI_SDK_ROOT = ../../../..

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
