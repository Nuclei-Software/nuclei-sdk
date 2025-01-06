ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=15 -falign-loops=6 \
-fno-if-conversion2 -fno-toplevel-reorder -fno-sched-last-insn-heuristic -fno-sched-dep-count-heuristic -mbranch-cost=1 -fipa-pta
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -fipa-pta -flto -fwhole-program -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
endif
