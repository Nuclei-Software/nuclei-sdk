TARGET = demo_smpu

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

COMMON_FLAGS := -O2

# Require sMPU extension
XLCFG_SMPU ?= 1

CHK_MODE ?=

ifneq ($(CHK_MODE),)
COMMON_FLAGS += -DCHK_MODE=$(CHK_MODE)
endif

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
