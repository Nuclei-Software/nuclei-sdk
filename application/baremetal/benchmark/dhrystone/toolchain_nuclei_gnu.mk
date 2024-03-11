ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=20 -falign-loops=8
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -flto -fwhole-program -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
endif
