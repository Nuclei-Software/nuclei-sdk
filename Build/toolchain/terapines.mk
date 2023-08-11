CC      := zcc
CXX     := z++
OBJDUMP := llvm-objdump
OBJCOPY := llvm-objcopy
# use gnu gdb to debug or upload
GDB     := riscv64-unknown-elf-gdb
AR      := llvm-ar
SIZE    := llvm-size
OPENOCD := openocd

ifeq ($(NOGC),1)
GC_CFLAGS =
GC_LDFLAGS =
else
GC_CFLAGS = -ffunction-sections -fdata-sections
GC_LDFLAGS = -Wl,--gc-sections -Wl,--check-sections
endif

ifeq ($(SIMULATION),1)
COMMON_FLAGS += -DCFG_SIMULATION
endif

ifeq ($(BANNER),0)
COMMON_FLAGS += -DNUCLEI_BANNER=0
endif

ifneq ($(findstring libncrt,$(STDCLIB)),)
$(error terapine toolchain don't provide libncrt library support)
endif

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring newlib,$(STDCLIB)),)
### Handle cases when STDCLIB variable has newlib in it
ifeq ($(STDCLIB),newlib_full)
COMMON_FLAGS += --config nosys.cfg
else ifeq ($(STDCLIB),newlib_fast)
COMMON_FLAGS += --config nano-nosys.cfg
STDCLIB_LDFLAGS += -u _printf_float -u _scanf_float
else ifeq ($(STDCLIB),newlib_small)
COMMON_FLAGS += --config nano-nosys.cfg
STDCLIB_LDFLAGS += -u _printf_float
else ifeq ($(STDCLIB),newlib_nano)
COMMON_FLAGS += --config nano-nosys.cfg
else
COMMON_FLAGS += --config nosys.cfg
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
COMMON_FLAGS += --config nosys.cfg
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
LDLIBS += -lfileops_uart
endif
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
## Append mtune options when RISCV_TUNE is defined
## It might be defined in SoC/<SOC>/build.mk, and can be overwritten by make
ifneq ($(RISCV_TUNE),)
COMMON_FLAGS +=
endif

ifneq ($(findstring newlib,$(STDCLIB)),)
LDFLAGS += -u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek -u __on_exit_args
endif

