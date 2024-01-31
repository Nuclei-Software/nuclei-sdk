TARGET := dhrystone

NUCLEI_SDK_ROOT = ../../../..

TOOLCHAIN ?= nuclei_gnu
# supported mode
## ground: ground rules for dhrystone
## best: best effort rules for dhrystone
DHRY_MODE ?= ground

-include toolchain_$(TOOLCHAIN).mk

COMMON_FLAGS := $(BENCH_FLAGS)

SRCDIRS = .

C_SRCDIRS = .

INCDIRS = .

STDCLIB ?= newlib_small

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
