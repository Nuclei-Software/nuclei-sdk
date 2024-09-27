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
DOWNLOAD ?= sram
CORE ?= nx900
# SMP CORE Number Settings
SMP ?= 2

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
