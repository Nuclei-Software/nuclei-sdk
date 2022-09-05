ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -flto -fno-inline -funroll-loops -Wno-implicit -mexplicit-relocs -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -Ofast -flto -fwhole-program -finline -funroll-loops -Wno-implicit -mexplicit-relocs -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-functions
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -funroll-loops -Wno-implicit -mexplicit-relocs -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4 -finline-functions
endif
