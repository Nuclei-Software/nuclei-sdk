TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

BENCH_FLAGS ?= -Ofast -msave-restore -fno-code-hoisting -fno-tree-vectorize -fno-common \
	-finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4 \
	-finline-limit=200 -fno-if-conversion -fno-if-conversion2 -fno-tree-dominator-opts \
	-fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	--param fsm-scale-path-stmts=5 --param max-average-unrolled-insns=200 \
	--param max-grow-copy-bb-insns=16

#BENCH_FLAGS ?= -Ofast -msave-restore -fno-code-hoisting -fschedule-insns -fschedule-insns2 \
	-fno-tree-vectorize -fno-common -funroll-all-loops -finline-functions \
	-falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-limit=1000 \
	-fno-if-conversion -fno-if-conversion2 -fno-tree-dominator-opts \
	-fno-tree-loop-distribute-patterns --param fsm-scale-path-stmts=5

#BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fschedule-insns -fschedule-insns2 \
	-fno-tree-vectorize -fno-common -funroll-loops -finline-functions \
	-falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-limit=1000 \
	-fno-if-conversion -fno-if-conversion2 -fselective-scheduling -fno-tree-dominator-opts

#BENCH_FLAGS ?= -O2 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -fno-common -funroll-loops -finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4
#BENCH_FLAGS ?= -O3 -funroll-loops -fpeel-loops -fgcse-sm -fgcse-las
# Optimize just for code size
#BENCH_FLAGS ?= -Os -flto
#BENCH_FLAGS ?= -Os

COMMON_FLAGS := $(BENCH_FLAGS)
COMMON_FLAGS += -DITERATIONS=600 -DPERFORMANCE_RUN=1
COMMON_FLAGS += -DFLAGS_STR=\""$(COMMON_FLAGS)"\"

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
