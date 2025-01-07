ifeq ($(DHRY_MODE),ground)
ifneq ($(CPU_SERIES),1000)
BENCH_FLAGS ?= -O3 -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=15 -falign-loops=6 \
-fno-if-conversion2 -fno-toplevel-reorder -fno-sched-last-insn-heuristic -fno-sched-dep-count-heuristic -mbranch-cost=1 -fipa-pta
else
BENCH_FLAGS ?= -O3 -fno-if-conversion -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=6 -falign-jumps=16 -falign-loops=8
endif
else ifeq ($(DHRY_MODE),best)
ifneq ($(CPU_SERIES),1000)
BENCH_FLAGS ?= -Ofast -fipa-pta -flto -fwhole-program -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
else
BENCH_FLAGS ?= -Ofast -flto -fwhole-program -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
endif
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
endif
