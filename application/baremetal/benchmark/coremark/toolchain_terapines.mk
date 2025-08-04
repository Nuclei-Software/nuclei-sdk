BENCH_FLAGS ?= -O3 -flto -falign-functions=4 -falign-loops=4 -flate-loop-unroll -malign-branch
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
