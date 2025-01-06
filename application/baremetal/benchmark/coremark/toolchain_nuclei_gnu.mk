$(info Using Nuclei $(CPU_SERIES) series optimization compiling flags!)

ifeq ($(CPU_SERIES),1000)
# Benchmark options for 1000
BENCH_FLAGS ?= -Ofast -fgcse-las -ffloat-store -mbranch-cost=1 -mstrict-align -funroll-all-loops \
	-fno-if-conversion -ftree-dominator-opts -fno-selective-scheduling -finline-functions -falign-functions=12 \
	-falign-jumps=8 -finline-limit=400 -fno-common -fno-tree-vectorize -fno-tree-loop-ivcanon \
	--param max-jump-thread-duplication-stmts=8 -fno-tree-partial-pre -fno-tree-copy-prop -fno-associative-math -ftrapping-math -fno-toplevel-reorder \
	-fstack-protector -fno-sched-last-insn-heuristic -fno-sched-dep-count-heuristic -fno-ipa-bit-cp -fno-function-cse \
	-fno-cse-follow-jumps -fno-crossjumping --param max-grow-copy-bb-insns=25
else ifeq ($(CPU_SERIES),900)
ifneq ($(findstring x,$(CORE)),)
#ux900
BENCH_FLAGS ?= -Ofast -fgcse-las -fno-code-hoisting -fipa-pta -mbranch-cost=1 -funroll-all-loops -finline-limit=300 \
	 -fno-crossjumping -fno-version-loops-for-strides \
	-fno-tree-sra -fno-tree-sink -fno-tree-partial-pre \
	-fno-shrink-wrap-separate -ffloat-store -funroll-loops -falign-functions=16 \
	-falign-jumps=8 -falign-loops=6 -fno-if-conversion2 -fno-tree-loop-ivcanon \
	-fstack-protector -fno-tree-loop-distribute-patterns -fno-cse-follow-jumps -fno-sched-dep-count-heuristic --param loop-max-datarefs-for-datadeps=4 \
	--param max-jump-thread-duplication-stmts=30 --param fsm-scale-path-stmts=3 --param max-grow-copy-bb-insns=12
else
#u900
BENCH_FLAGS ?= -Ofast -mno-autovec-dsp -fno-shrink-wrap-separate -fno-tree-partial-pre -fno-version-loops-for-strides -mbranch-cost=1 -mstrict-align -funroll-all-loops -finline-limit=350 \
	-fno-code-hoisting -fno-sched-last-insn-heuristic -fno-toplevel-reorder -fno-tree-copy-prop -fno-crossjumping -fno-bit-tests -ftree-dominator-opts -funroll-loops -finline-functions -falign-functions=4 \
	-fno-ipa-bit-cp -falign-jumps=16 -falign-loops=8 -fno-if-conversion2 -fno-tree-loop-ivcanon \
	-fgcse-las --param=max-loop-header-insns=3 --param loop-max-datarefs-for-datadeps=6 \
	--param max-jump-thread-duplication-stmts=58 --param fsm-scale-path-stmts=3 --param max-grow-copy-bb-insns=12
endif
else ifeq ($(CPU_SERIES),600)
# Benchmark options for 600
ifneq ($(findstring x,$(CORE)),)
#ux600
BENCH_FLAGS ?= -Ofast -fno-version-loops-for-strides -ffloat-store -mbranch-cost=1 -mstrict-align -funroll-all-loops -finline-limit=500 \
	-ftree-dominator-opts -funroll-loops -finline-functions -falign-functions=8 \
	-falign-jumps=8 -falign-loops=6 -fno-if-conversion2 -fno-tree-loop-distribute-patterns  -fno-tree-loop-ivcanon \
	--param=max-loop-header-insns=2 --param=unroll-jam-min-percent=0 \
	--param max-jump-thread-duplication-stmts=12 --param fsm-scale-path-stmts=3 --param max-grow-copy-bb-insns=12
else
#u600
BENCH_FLAGS ?= -Ofast -fno-version-loops-for-strides -ffloat-store -mbranch-cost=1 -mstrict-align -funroll-all-loops -finline-limit=300 \
	-ftree-dominator-opts -funroll-loops -finline-functions -falign-functions=8 \
	-falign-jumps=8 -falign-loops=8 -fipa-pta -fno-code-hoisting -fno-if-conversion \
	-fno-if-conversion2 -fno-tree-loop-distribute-patterns -fno-tree-loop-ivcanon \
	-ftree-vrp --param=max-loop-header-insns=2 --param=unroll-jam-min-percent=0 \
	--param max-jump-thread-duplication-stmts=12 --param fsm-scale-path-stmts=3 --param max-grow-copy-bb-insns=12
endif
else ifeq ($(CPU_SERIES),300)
# Benchmark options for 300
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-common -finline-functions -falign-functions=6 \
	-falign-jumps=6 -falign-loops=4 -finline-limit=200 -fno-if-conversion -fno-if-conversion2 \
	-fselective-scheduling -fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	-fno-delete-null-pointer-checks -fno-rename-registers -mbranch-cost=1 --param fsm-scale-path-stmts=3 \
	--param max-average-unrolled-insns=200 --param max-grow-copy-bb-insns=20 --param max-jump-thread-duplication-stmts=25 \
	--param hot-bb-frequency-fraction=4
else
# Benchmark options for 200
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-common -finline-functions -falign-functions=10 \
	-falign-jumps=8 -falign-loops=8 -finline-limit=300 -fno-if-conversion -fno-if-conversion2 \
	-fselective-scheduling -fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	-fno-delete-null-pointer-checks -mbranch-cost=1 --param fsm-scale-path-stmts=3 \
	--param max-average-unrolled-insns=400 --param max-grow-copy-bb-insns=20 --param max-jump-thread-duplication-stmts=26 \
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

