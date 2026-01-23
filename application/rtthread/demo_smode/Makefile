TARGET = rtthread_demo
RTOS = RTThread

NUCLEI_SDK_ROOT = ../../..

COMMON_FLAGS = -O3 -DSMODE_RTOS

# Better with SSTC extension
# when SSTC extension present, S-Mode present
# You can change to 0, if SSTC not present
XLCFG_SSTC ?= 1
# Require S-Mode
# set to 1 to enable S-Mode if your CPU has
XLCFG_SMODE ?=

SRCDIRS = .
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
