TARGET := dhrystone

NUCLEI_SDK_ROOT = ../../../..

# supported mode
## ground: ground rules for dhrystone
## best: best effort rules for dhrystone
DHRY_MODE ?= ground

CPU_SERIES ?= 300

ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O2 -flto -fno-inline -funroll-loops -Wno-implicit -mexplicit-relocs  -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -flto -fwhole-program -finline -funroll-loops -Wno-implicit -mexplicit-relocs -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O2 -flto -finline -funroll-loops -Wno-implicit -mexplicit-relocs -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-functions
endif

COMMON_FLAGS := $(BENCH_FLAGS)

C_SRCDIRS = .

INCDIRS = .

STDCLIB ?= newlib_small

# Clean generated *.i and *.s
CLEAN_OBJS += *.i *.s

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
