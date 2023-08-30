TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu

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
COMMON_FLAGS += -DITERATIONS=800 -DPERFORMANCE_RUN=1
ifeq ($(TOOLCHAIN),terapines)
COMMON_FLAGS +=
else
COMMON_FLAGS += -DFLAGS_STR=\""$(COMMON_FLAGS)"\"
endif

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
