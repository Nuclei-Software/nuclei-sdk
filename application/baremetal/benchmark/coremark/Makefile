TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)
COMMON_FLAGS += -DITERATIONS=15 -DPERFORMANCE_RUN=1
COMMON_FLAGS += -DFLAGS_STR=\""$(COMMON_FLAGS)"\"

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
