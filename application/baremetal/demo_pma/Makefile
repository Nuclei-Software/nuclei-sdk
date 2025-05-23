TARGET = demo_pma

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

COMMON_FLAGS := -O2

# DOWNLOAD mode must be a mode
# such as external ddr/sram, core local ilm is not ok which will bypass cache
DOWNLOAD ?= sram

ifeq ($(DOWNLOAD),ddr)
COMMON_FLAGS += -DBIG_ROW_SIZE
endif

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
