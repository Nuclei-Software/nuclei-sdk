TARGET = exception_umode

NUCLEI_SDK_ROOT = ../../..

# REQUIRE: PMP
# OPTIONAL: SMPU, SMODE
XLCFG_SMPU ?= 0
XLCFG_SMODE ?= 1

SRCDIRS = . src

INCDIRS = . inc

COMMON_FLAGS := -O0

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
