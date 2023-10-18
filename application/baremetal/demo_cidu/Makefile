TARGET = demo_cidu

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .

INCDIRS = .

COMMON_FLAGS := -O2

# Per-Core HEAP and STACK Size Settings
HEAPSZ ?= 2K
STACKSZ ?= 2K

# DOWNLOAD mode must be a mode
# where all cpus share the same code/data ram
# such as external ddr/sram, core local ilm is not ok
DOWNLOAD ?= ddr
CORE ?= nx900
# SMP CORE Number Settings
SMP ?= 2
CIDU ?= 1

ifneq ($(CIDU),0)
COMMON_FLAGS += -D__CIDU_PRESENT=1
endif

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
