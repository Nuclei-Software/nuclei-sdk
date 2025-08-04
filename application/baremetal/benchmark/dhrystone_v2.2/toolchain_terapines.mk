BENCH_FLAGS ?= -O3
LDFLAGS += -Wl,-mllvm,--align-all-nofallthru-blocks=2
