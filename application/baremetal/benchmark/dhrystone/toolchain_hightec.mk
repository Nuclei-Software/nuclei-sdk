ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -Ofast -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions
endif
