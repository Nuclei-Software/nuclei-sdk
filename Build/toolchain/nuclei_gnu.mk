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

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring newlib,$(STDCLIB)),)
LDLIBS += -lstdc++
### Handle cases when STDCLIB variable has newlib in it
ifeq ($(STDCLIB),newlib_full)
COMMON_FLAGS += --specs=nosys.specs
else ifeq ($(STDCLIB),newlib_fast)
COMMON_FLAGS += --specs=nano.specs --specs=nosys.specs
STDCLIB_LDFLAGS += -u _printf_float -u _scanf_float
else ifeq ($(STDCLIB),newlib_small)
COMMON_FLAGS += --specs=nano.specs --specs=nosys.specs
STDCLIB_LDFLAGS += -u _printf_float
else ifeq ($(STDCLIB),newlib_nano)
COMMON_FLAGS += --specs=nano.specs --specs=nosys.specs
else
COMMON_FLAGS += --specs=nosys.specs
endif
###
else ifneq ($(findstring libncrt,$(STDCLIB)),)
### Handle cases when STDCLIB variable has libncrt in it
COMMON_FLAGS += --specs=$(STDCLIB).specs
LDLIBS += -l$(patsubst lib%,%,$(STDCLIB)) -lheapops_$(NCRTHEAP)
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
LDLIBS += -lstdc++
### To be back compatiable with NEWLIB and PFLOAT variable
ifeq ($(NEWLIB),nano)
STDCLIB_LDFLAGS = --specs=nano.specs --specs=nosys.specs
ifeq ($(PFLOAT),1)
STDCLIB_LDFLAGS += -u _printf_float
endif
else
STDCLIB_LDFLAGS += --specs=nosys.specs
endif
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
#COMMON_FLAGS += -mtune=$(RISCV_TUNE)
endif

ifneq ($(findstring newlib,$(STDCLIB)),)
LDFLAGS += -u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek
endif

