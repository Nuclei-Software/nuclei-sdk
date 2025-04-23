ifeq ($(DHRY_MODE),ground)
ifneq ($(CPU_SERIES),1000)
BENCH_FLAGS ?= -O3 -fno-if-conversion -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=6 -falign-jumps=16 -falign-loops=8
else
BENCH_FLAGS ?= -O3 -fno-if-conversion  -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=12 -falign-jumps=16 -falign-loops=12
endif
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -flto -fwhole-program -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-jumps=4 -falign-loops=8 -finline-functions
endif
