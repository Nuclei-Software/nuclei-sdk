## for gcc13
GCCVER=$(shell riscv64-unknown-elf-gcc --version | head -n1 | cut -d' ' -f3)
$(info Using GCC Version $(GCCVER))
ifeq ($(AUTOVEC),1)
ifneq ($(findstring 13,$(GCCVER)),)
COMMON_FLAGS += --param=riscv-autovec-preference=scalable --param=riscv-autovec-lmul=dynamic
else
## for gcc14
COMMON_FLAGS += -mrvv-max-lmul=dynamic -mrvv-vector-bits=scalable
endif
COMMON_FLAGS += -ftree-loop-vectorize -ftree-slp-vectorize
else ifeq ($(AUTOVEC),0)
ifneq ($(findstring 13,$(GCCVER)),)
COMMON_FLAGS += --param=riscv-autovec-preference=none
endif
COMMON_FLAGS += -fno-tree-loop-vectorize -fno-tree-slp-vectorize
else
COMMON_FLAGS +=
endif
