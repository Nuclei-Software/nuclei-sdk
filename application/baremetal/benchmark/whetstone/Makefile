TARGET := whetstone

NUCLEI_SDK_ROOT = ../../../..

BENCH_FLAGS ?= -O3 -funroll-loops -fsingle-precision-constant

COMMON_FLAGS := $(BENCH_FLAGS)

LDFLAGS := -lm

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
