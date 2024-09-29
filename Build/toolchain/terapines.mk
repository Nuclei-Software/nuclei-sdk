CC      := zcc
CXX     := z++
OBJDUMP := llvm-objdump
OBJCOPY := llvm-objcopy
# use gnu gdb to debug or upload
GDB     := riscv64-unknown-elf-gdb
AR      := llvm-ar
SIZE    := llvm-size
OPENOCD := openocd

ifneq ($(findstring libncrt,$(STDCLIB)),)
$(error Terapines toolchain dont provide libncrt library support)
endif

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring newlib,$(STDCLIB)),)
### Handle cases when STDCLIB variable has newlib in it
ifeq ($(STDCLIB),newlib_full)
LDLIBS += -lc -lclang_rt.builtins
else ifeq ($(STDCLIB),newlib_fast)
LDLIBS += -lc_nano -lclang_rt.builtins
STDCLIB_LDFLAGS += -u _printf_float -u _scanf_float
else ifeq ($(STDCLIB),newlib_small)
LDLIBS += -lc_nano -lclang_rt.builtins
STDCLIB_LDFLAGS += -u _printf_float
else ifeq ($(STDCLIB),newlib_nano)
LDLIBS += -lc_nano -lclang_rt.builtins
# work around for relocation R_RISCV_PCREL_HI20 out of range: -524289 is not in [-524288, 524287]; references _printf_float when compile with rv64
# so with this change below, newlib_nano = newlib_small now
STDCLIB_LDFLAGS += -u _printf_float
else
LDLIBS += -lc_nano -lclang_rt.builtins
STDCLIB_LDFLAGS += -u _printf_float
endif
###
else ifneq ($(findstring libncrt,$(STDCLIB)),)
### Handle cases when STDCLIB variable has libncrt in it
LDLIBS += -l$(patsubst lib%,%,$(STDCLIB)) -lheapops_$(NCRTHEAP)
COMMON_FLAGS += -isystem=/include/libncrt
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
LDLIBS += -lc_nano -lclang_rt.builtins
STDCLIB_LDFLAGS += -u _printf_float
###
endif

ifneq ($(SEMIHOST),)
ifneq ($(findstring libncrt,$(STDCLIB)),)
LDLIBS += -lfileops_semi
else
LDLIBS += -lsemihost
endif
else
ifneq ($(findstring libncrt,$(STDCLIB)),)
LDLIBS += -lfileops_$(NCRTIO)
endif
endif

## Link with standard c++ library
LDLIBS += -lc++ -lunwind

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
# Enable some experimental extension for llvm clang
COMMON_FLAGS += -menable-experimental-extensions
## Append mtune options when RISCV_TUNE is defined
## It might be defined in SoC/<SOC>/build.mk, and can be overwritten by make
ifneq ($(RISCV_TUNE),)
COMMON_FLAGS += -mtune=$(RISCV_TUNE)
endif

ifneq ($(findstring newlib,$(STDCLIB)),)
LDFLAGS += -u __on_exit_args
endif

LDFLAGS += -nodefaultlibs
