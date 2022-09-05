TARGET := dhrystone

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu
# supported mode
## ground: ground rules for dhrystone
## best: best effort rules for dhrystone
DHRY_MODE ?= ground

CPU_SERIES ?= 300

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)

C_SRCDIRS = .

INCDIRS = .

STDCLIB ?= newlib_small

# Clean generated *.i and *.s
CLEAN_OBJS += *.i *.s

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
