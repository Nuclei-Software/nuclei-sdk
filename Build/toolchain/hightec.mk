CC      := clang
CXX     := clang
OBJDUMP := llvm-objdump
OBJCOPY := llvm-objcopy
# use gnu gdb to debug or upload
GDB     := riscv64-unknown-elf-gdb
AR      := llvm-ar
SIZE    := llvm-size
OPENOCD := openocd

# Hightec toolchain use picolibc as c library
# TODO: The stub function is totally different from newlib stub function, take care

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring picolibc,$(STDCLIB)),)
### Handle cases when STDCLIB variable has picolibc in it
LDLIBS += -lc
STDCLIB_LDFLAGS +=
###
else ifeq ($(STDCLIB),nostd)
### Handle cases when no standard system directories for header files
COMMON_FLAGS += -nostdinc
###
else ifeq ($(STDCLIB),nospec)
### Handle cases no specs configs are passed
COMMON_FLAGS +=
###
else
LDLIBS += -lc
STDCLIB_LDFLAGS +=
###
endif

ifneq ($(SEMIHOST),)
LDLIBS += -lsemihost
endif

## Link with standard c++ library
LDLIBS +=


## TODO: Specical handling for RV32 or RV64 clang_rt library selection
ifneq ($(findstring 32,$(RISCV_ABI)),)
LDLIBS += -lclang_rt.builtins-riscv32
else
LDLIBS += -lclang_rt.builtins-riscv64
endif

## Heap and stack size settings
## It will define symbols only used in linker script
## __STACK_SIZE and __HEAP_SIZE are not a c marco
## they are ld symbols used by linker
ifneq ($(STACKSZ),)
LDFLAGS += -Wl,--defsym=__STACK_SIZE=$(STACKSZ)
endif
ifneq ($(HEAPSZ),)
LDFLAGS += -Wl,--defsym=__HEAP_SIZE=$(HEAPSZ)
endif

## SIMU=xlspike/qemu
### enable run on xlspike and qemu auto-exit if return from main
ifneq ($(SIMU),)
SIMULATION_MODE=SIMULATION_MODE_$(call uc, $(SIMU))
COMMON_FLAGS += -DSIMULATION_MODE=$(SIMULATION_MODE)
endif

COMMON_FLAGS += -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=$(RISCV_CMODEL)
COMMON_FLAGS += -fno-exceptions
## Append mtune options when RISCV_TUNE is defined
## It might be defined in SoC/<SOC>/build.mk, and can be overwritten by make
ifneq ($(RISCV_TUNE),)
COMMON_FLAGS +=# -mtune=$(RISCV_TUNE)
endif

ifneq ($(findstring picolibc,$(STDCLIB)),)
LDFLAGS += #-u __on_exit_args
endif

LDFLAGS += -nodefaultlibs
