TARGET := whetstone

NUCLEI_SDK_ROOT = ../../../..
TOOLCHAIN ?= nuclei_gnu

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)

LDFLAGS := -lm

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
