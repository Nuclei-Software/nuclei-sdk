TARGET = rtthread_demo
RTOS = RTThread

NUCLEI_SDK_ROOT = ../../..

COMMON_FLAGS = -O3 -DSMODE_RTOS

XLCFG_TEE := 1
# You can change to 0, if SSTC not present
XLCFG_SSTC ?= 1

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
