TARGET = demo_eclic_stress

NUCLEI_SDK_ROOT = ../../..

COMMON_FLAGS := -O3

# REQUIRE: ECLIC, SYSTIMER
XLCFG_ECLIC :=
XLCFG_SYSTIMER :=
# OPTIONAL: SSTC
XLCFG_SMODE := 1
XLCFG_SSTC := 1

# if INTSTACK is 1, will use separated interrupt sp
# only valid for eclicv2
INTSTACK :=

ifeq ($(INTSTACK),1)
COMMON_FLAGS += -DINTSTACK=1
endif

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
