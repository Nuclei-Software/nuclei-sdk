TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

COMMON_FLAGS := -O2 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -fno-common -funroll-loops -finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4
#COMMON_FLAGS := -O3 -funroll-loops -fpeel-loops -fgcse-sm -fgcse-las

COMMON_FLAGS += -DFLAGS_STR=\""$(COMMON_FLAGS)"\"
COMMON_FLAGS += -DITERATIONS=100 -DPERFORMANCE_RUN=1

PFLOAT = 1

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
