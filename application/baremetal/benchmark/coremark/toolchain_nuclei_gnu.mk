# Benchmark options for 100
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-tree-vectorize -fno-common -finline-functions -falign-functions=4 \
	-falign-jumps=4 -falign-loops=4 -finline-limit=200 -fno-if-conversion -fno-if-conversion2 -fipa-pta \
	-fselective-scheduling -fno-tree-loop-distribute-patterns -funroll-loops -funroll-all-loops \
	-fno-delete-null-pointer-checks -fno-rename-registers -mbranch-cost=1 --param fsm-scale-path-stmts=3 \
	--param max-average-unrolled-insns=200 --param max-grow-copy-bb-insns=20 --param max-jump-thread-duplication-stmts=25 \
	--param hot-bb-frequency-fraction=4 --param unroll-jam-min-percent=0
