TARGET := dhrystone

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu
# supported mode
## ground: ground rules for dhrystone
## best: best effort rules for dhrystone
DHRY_MODE ?= ground

# Adjust CPU_SERIES according to passed CORE
ifneq ($(findstring 20,$(CORE)),)
CPU_SERIES ?= 200
else ifneq ($(findstring 30,$(CORE)),)
CPU_SERIES ?= 300
else ifneq ($(findstring 60,$(CORE)),)
CPU_SERIES ?= 600
else ifneq ($(findstring 90,$(CORE)),)
CPU_SERIES ?= 900
else
CPU_SERIES ?= 300
endif

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)

SRCDIRS = .

C_SRCDIRS = .

INCDIRS = .

STDCLIB ?= newlib_small

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
