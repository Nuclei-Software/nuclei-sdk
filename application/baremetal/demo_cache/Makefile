TARGET = demo_cache

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

COMMON_FLAGS := -O0

# DOWNLOAD mode must be a mode
# such as external ddr/sram, core local ilm is not ok which will bypass cache
DOWNLOAD ?= ddr

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
