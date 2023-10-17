TARGET := whetstone

NUCLEI_SDK_ROOT = ../../../..
TOOLCHAIN ?= nuclei_gnu

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)

# When using libncrt library, no need to link with -lm
LDLIBS := -lm

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
