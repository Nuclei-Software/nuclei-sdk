## NUCLEI RISCV GCC COMPILER
## NUCLEI RISCV OPENOCD
#
#!< Nuclei SDK Tools Root
COMPILE_PREFIX ?= riscv64-unknown-elf-
NUCLEI_SDK_TOOL_ROOT ?= $(NUCLEI_SDK_ROOT)/prebuilt_tools
NUCLEI_RISCV_GCC_ROOT ?= $(NUCLEI_SDK_TOOL_ROOT)/gcc
NUCLEI_OPENOCD_ROOT ?= $(NUCLEI_SDK_TOOL_ROOT)/openocd

NUCLEI_SDK_TOOL_ROOT_EXIST = 0
ifneq ($(wildcard $(NUCLEI_RISCV_GCC_ROOT)),)
ifneq ($(wildcard $(NUCLEI_OPENOCD_ROOT)),)
NUCLEI_SDK_TOOL_ROOT_EXIST = 1
endif
endif

ifeq ($(NUCLEI_SDK_TOOL_ROOT_EXIST),1)
CC      := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)gcc)
CXX     := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)g++)
OBJDUMP := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)objdump)
OBJCOPY := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)objcopy)
GDB     := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)gdb)
AR      := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)ar)
SIZE    := $(abspath $(NUCLEI_RISCV_GCC_ROOT)/bin/$(COMPILE_PREFIX)size)
OPENOCD := $(abspath $(NUCLEI_OPENOCD_ROOT)/bin/openocd)
else
CC      := $(COMPILE_PREFIX)gcc
CXX     := $(COMPILE_PREFIX)g++
OBJDUMP := $(COMPILE_PREFIX)objdump
OBJCOPY := $(COMPILE_PREFIX)objcopy
GDB     := $(COMPILE_PREFIX)gdb
AR      := $(COMPILE_PREFIX)ar
SIZE    := $(COMPILE_PREFIX)size
OPENOCD := openocd
endif

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring newlib,$(STDCLIB)),)
### Handle cases when STDCLIB variable has newlib in it
ifeq ($(STDCLIB),newlib_full)
LDLIBS += -lc -lgcc
else ifeq ($(STDCLIB),newlib_fast)
LDLIBS += -lc_nano -lgcc
STDCLIB_LDFLAGS += -u _printf_float -u _scanf_float
COMMON_FLAGS += -isystem=/include/newlib-nano
else ifeq ($(STDCLIB),newlib_small)
LDLIBS += -lc_nano -lgcc
STDCLIB_LDFLAGS += -u _printf_float
COMMON_FLAGS += -isystem=/include/newlib-nano
else ifeq ($(STDCLIB),newlib_nano)
LDLIBS += -lc_nano -lgcc
COMMON_FLAGS += -isystem=/include/newlib-nano
else
LDLIBS += -lc_nano -lgcc
COMMON_FLAGS += -isystem=/include/newlib-nano
endif
###
else ifneq ($(findstring libncrt,$(STDCLIB)),)
### Handle cases when STDCLIB variable has libncrt in it
LDLIBS += -l$(patsubst lib%,%,$(STDCLIB))
ifneq ($(NCRTHEAP),)
ifeq ($(COMPILE_PREFIX),riscv64-unknown-elf-)
LDLIBS += -lheapops_$(NCRTHEAP)
endif
endif
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
LDLIBS += -lc_nano -lgcc
###
endif

ifneq ($(SEMIHOST),)
ifneq ($(findstring libncrt,$(STDCLIB)),)
ifeq ($(COMPILE_PREFIX),riscv64-unknown-elf-)
LDLIBS += -lfileops_semi
endif
else
LDLIBS += -lsemihost
endif
else
ifneq ($(findstring libncrt,$(STDCLIB)),)
ifneq ($(NCRTIO),)
ifeq ($(COMPILE_PREFIX),riscv64-unknown-elf-)
LDLIBS += -lfileops_$(NCRTIO)
endif
endif
endif
endif

## Link with standard c++ library
LDLIBS += -lstdc++

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
COMMON_FLAGS += -mtune=$(RISCV_TUNE)
endif

# Generate zcmp related asm by passing extra options
ifneq ($(findstring zc,$(ARCH_EXT)),)
COMMON_FLAGS += -fomit-frame-pointer -fno-shrink-wrap-separate
endif

# TODO Workaround: when v extension is enabled, disable memcpy generated using vector instruction
# upstream just rename this -mmemcpy-strategy option to -mstringop-strategy option now
# see https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=4ae5a7336ac8e1ba57ee1e885b5b76ed86cdbfd5
ifneq ($(findstring v,$(ARCH_EXT)),)
#COMMON_FLAGS += -mmemcpy-strategy=scalar
COMMON_FLAGS += -mstringop-strategy=scalar
endif

# Disable auto vectorization when AUTOVEC=0
# see https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-ftree-vectorize
ifeq ($(AUTOVEC),0)
COMMON_FLAGS += -fno-tree-vectorize -fno-tree-loop-vectorize -fno-tree-slp-vectorize
endif

LDFLAGS += -nodefaultlibs
# For binutils 2.40 in Nuclei GNU Toolchain, need add below option
# to remove link warning
ifeq ($(COMPILE_PREFIX),riscv64-unknown-elf-)
LDFLAGS += -Wl,--no-warn-rwx-segments
endif

ifneq ($(findstring newlib,$(STDCLIB)),)
# when compiled with -flto, need to pass the following options
ifeq ($(CODESIZE),)
LDFLAGS += -u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek -u errno
endif
endif

