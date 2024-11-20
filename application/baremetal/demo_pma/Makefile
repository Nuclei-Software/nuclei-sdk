TARGET = demo_pma

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

# different optimization level such as -O2 effects cache miss
COMMON_FLAGS := -O0

# DOWNLOAD mode must be a mode
# such as external ddr/sram, core local ilm is not ok which will bypass cache
DOWNLOAD ?= sram

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
