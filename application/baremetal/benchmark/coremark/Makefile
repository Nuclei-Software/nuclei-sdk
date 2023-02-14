TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu
CPU_SERIES ?= 300

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
