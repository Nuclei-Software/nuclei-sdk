# Benchmark options for 100
BENCH_FLAGS ?= -Ofast -fno-code-hoisting -fno-tree-vectorize -finline-functions -falign-functions=4 \
	-falign-jumps=4 -falign-loops=4 -finline-limit=200 -fselective-scheduling -funroll-loops
