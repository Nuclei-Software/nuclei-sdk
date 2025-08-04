ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -ffast-math -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -flate-loop-unroll -malign-branch
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -O3 -ffast-math -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions -flate-loop-unroll -malign-branch
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions -flate-loop-unroll -malign-branch
endif
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
