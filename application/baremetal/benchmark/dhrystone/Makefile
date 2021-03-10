TARGET := dhrystone

NUCLEI_SDK_ROOT = ../../../..

BENCH_FLAGS ?= -O2 -flto -fno-inline -funroll-loops -Wno-implicit -mexplicit-relocs  -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4

COMMON_FLAGS := $(BENCH_FLAGS)

C_SRCDIRS = .

INCDIRS = .

#Enable Nano Newlib Float Print
PFLOAT = 1
# Clean generated *.i and *.s
CLEAN_OBJS += *.i *.s

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
