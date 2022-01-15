TARGET = coremark

NUCLEI_SDK_ROOT = ../../../..

CPU_SERIES ?= 300

$(info Using Nuclei $(CPU_SERIES) series optimization compiling flags!)

ifeq ($(CPU_SERIES),900)
# Benchmark options for 900
BENCH_FLAGS ?= -Ofast -funroll-loops -funroll-all-loops -fno-aggressive-loop-optimizations -fgcse-las \
	-fno-delete-null-pointer-checks -fno-rename-registers -fno-tree-vrp -fwrapv -fno-toplevel-reorder \
	-fipa-pta -finline-functions -falign-functions=4 -falign-jumps=4 -falign-loops=4 -fno-if-conversion \
	-fno-if-conversion2 -fno-tree-loop-distribute-patterns -fno-sched-rank-heuristic -fselective-scheduling \
	--param unroll-jam-min-percent=0 --param large-unit-insns=0 --param max-loop-header-insns=2 \
	--param max-average-unrolled-insns=128 --param max-inline-insns-size=128 --param max-cse-path-length=3 \
	--param inline-min-speedup=1 --param max-sched-region-insns=7 --param max-goto-duplication-insns=0 \
	--param max-gcse-insertion-ratio=0 --param max-partial-antic-length=1 --param loop-max-datarefs-for-datadeps=0 \
	--param hot-bb-frequency-fraction=1 --param max-jump-thread-duplication-stmts=2 --param max-tail-merge-iterations=0 \
	--param uninlined-function-insns=16 --param max-grow-copy-bb-insns=16 -mbranch-cost=1
else ifeq ($(CPU_SERIES),600)
# Benchmark options for 600
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-tree-vectorize -fno-common -finline-functions \
	-falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-limit=200  -fno-if-conversion \
	-fno-if-conversion2 -fipa-pta -fselective-scheduling -fno-tree-loop-distribute-patterns \
	-funroll-loops -funroll-all-loops -fno-delete-null-pointer-checks -fno-rename-registers \
	-mbranch-cost=1 --param fsm-scale-path-stmts=5 --param max-average-unrolled-insns=200 \
	--param max-grow-copy-bb-insns=16 --param max-jump-thread-duplication-stmts=14 \
	--param hot-bb-frequency-fraction=4 --param unroll-jam-min-percent=0
else ifeq ($(CPU_SERIES),300)
# Benchmark options for 300
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-tree-vectorize -fno-common -finline-functions -falign-functions=4 \
	-falign-jumps=4 -falign-loops=4 -finline-limit=200 -fno-if-conversion -fno-if-conversion2 -fipa-pta \
	-fselective-scheduling -fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	-fno-delete-null-pointer-checks -fno-rename-registers -mbranch-cost=1 --param fsm-scale-path-stmts=5 \
	--param max-average-unrolled-insns=200 --param max-grow-copy-bb-insns=16 --param max-jump-thread-duplication-stmts=8 \
	--param hot-bb-frequency-fraction=4 --param unroll-jam-min-percent=0
else
# Benchmark options for 200
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-tree-vectorize -fno-common -finline-functions -falign-functions=4 \
	-falign-jumps=4 -falign-loops=4 -finline-limit=200 -fno-if-conversion -fno-if-conversion2 -fipa-pta \
	-fselective-scheduling -fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	-fno-delete-null-pointer-checks -fno-rename-registers -mbranch-cost=1 --param fsm-scale-path-stmts=5 \
	--param max-average-unrolled-insns=200 --param max-grow-copy-bb-insns=16 --param max-jump-thread-duplication-stmts=8 \
	--param hot-bb-frequency-fraction=4 --param unroll-jam-min-percent=0
endif

# Backup options
#BENCH_FLAGS ?= -Ofast -msave-restore -fno-code-hoisting -fno-tree-vectorize -fno-common \
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
COMMON_FLAGS += -DITERATIONS=800 -DPERFORMANCE_RUN=1
COMMON_FLAGS += -DFLAGS_STR=\""$(COMMON_FLAGS)"\"

STDCLIB ?= newlib_small

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
