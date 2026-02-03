TARGET = demo_smpcc

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .

INCDIRS = .

COMMON_FLAGS := -O2

# REQUIRE: SMPCC
XLCFG_SMPCC :=
# OPTIONAL: CCM, HPM-v2
XLCFG_CCM :=
XLCFG_HPM :=

# Set MAX_L2_SIZE_KB to the max L2 cache size in KB
# For example: MAX_L2_SIZE_KB=1024 for 1MB
# The default MAX_L2_SIZE is 1MB
MAX_L2_SIZE_KB ?= 1024

COMMON_FLAGS += -DMAX_L2_SIZE_KB=$(MAX_L2_SIZE_KB)

# DOWNLOAD mode must be a mode
# such as external ddr/sram, core local ilm is not ok which will bypass cache
DOWNLOAD ?= ddr

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
