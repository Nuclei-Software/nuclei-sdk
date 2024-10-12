ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
endif
