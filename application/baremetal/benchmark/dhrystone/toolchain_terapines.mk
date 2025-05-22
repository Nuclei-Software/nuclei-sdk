ifeq ($(DHRY_MODE),ground)
BENCH_FLAGS ?= -O3 -ffast-math -flto -fno-inline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -flate-loop-unroll -malign-branch -fuse-size-lib
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
else ifeq ($(DHRY_MODE),best)
BENCH_FLAGS ?= -O3 -ffast-math -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions -flate-loop-unroll -malign-branch
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
else ifeq ($(DHRY_MODE),inline)
BENCH_FLAGS ?= -O3 -flto -finline -fno-builtin-printf -funroll-loops -falign-functions=4 -falign-loops=4 -finline-functions -flate-loop-unroll -malign-branch
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
endif
