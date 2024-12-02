TARGET := threadx_module_sample

# Configuration parameters
RISCV_ARCH ?= rv32imafdc
RISCV_ABI ?= ilp32d

COMMON_FLAGS := -Os -g -fPIC -mno-plt -fno-builtin

NUCLEI_SDK_ROOT := ../../../..

THREADX_ROOT := $(NUCLEI_SDK_ROOT)/OS/ThreadX

SRCDIRS = . $(THREADX_ROOT)/common_modules/module_lib/src $(THREADX_ROOT)/ports/nuclei/module_lib/src

INCDIRS = . $(THREADX_ROOT)/common_modules/inc $(THREADX_ROOT)/common/inc $(THREADX_ROOT)/ports/nuclei

LINKER_SCRIPT := sample_threadx_module.ld


# Build System Based on Makefile
COMPILE_PREFIX ?= riscv64-unknown-elf-

CC      := $(COMPILE_PREFIX)gcc
CXX     := $(COMPILE_PREFIX)g++
# CC      := $(COMPILE_PREFIX)clang
# CXX     := $(COMPILE_PREFIX)clang++
OBJDUMP := $(COMPILE_PREFIX)objdump
OBJCOPY := $(COMPILE_PREFIX)objcopy
GDB     := $(COMPILE_PREFIX)gdb
AR      := $(COMPILE_PREFIX)ar
SIZE    := $(COMPILE_PREFIX)size
OPENOCD := openocd

RM=rm -rf
RMD=rm -rf
ECHO=echo
CP=cp -rf
MKD=mkdir -p
Q=
PS=/

TRACE_CREATE_DIR    = @$(ECHO) "Creating Directory : " $(@D)
TRACE_COMPILE       = @$(ECHO) "Compiling  : " $<
TRACE_ASSEMBLE      = @$(ECHO) "Assembling : " $<
TRACE_LINK          = @$(ECHO) "Linking    : " $@
TRACE_ARCHIVE       = @$(ECHO) "Archiving  : " $@


MKDEP_OPT = -MMD -MT $@ -MF $@.d
C_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(C_INCDIRS)),-I$(dir))
CXX_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(CXX_INCDIRS)),-I$(dir))
ASM_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(ASM_INCDIRS)),-I$(dir))

COMMON_FLAGS += -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany -fno-common

CFLAGS += $(COMMON_FLAGS) $(C_INCLUDE_OPT) $(MKDEP_OPT)
CXXFLAGS += $(COMMON_FLAGS) $(CXX_INCLUDE_OPT) $(MKDEP_OPT)
ASMFLAGS += $(COMMON_FLAGS) $(ASM_INCLUDE_OPT) $(MKDEP_OPT)

LIB_OPT = $(addprefix -L, $(sort $(LIBDIRS)))

LDFLAGS += -T $(LINKER_SCRIPT) -e _txm_module_thread_shell_entry -nostartfiles -nodefaultlibs \
	-Wl,-Map=$(TARGET).map $(LIB_OPT) $(LDLIBS)


## Small Functions ##
get_csrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.c $(subdir)/*.C))
get_asmsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.s $(subdir)/*.S))
get_cxxsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.cpp $(subdir)/*.CPP))
check_item_exist = $(strip $(if $(filter 1, $(words $(1))),$(filter $(1), $(sort $(2))),))

TARGET_ELF = $(TARGET).elf

ALL_CSRCS = $(sort $(C_SRCS) $(call get_csrcs, $(SRCDIRS) $(C_SRCDIRS)))
ALL_CXXSRCS = $(sort $(CXX_SRCS) $(call get_cxxsrcs, $(SRCDIRS) $(CXX_SRCDIRS)))
ALL_ASMSRCS = $(sort $(ASM_SRCS) $(call get_asmsrcs, $(SRCDIRS) $(ASM_SRCDIRS)))


ALL_ASM_OBJS := $(ALL_ASMSRCS:=.o)
ALL_C_OBJS := $(ALL_CSRCS:=.o)
ALL_CXX_OBJS := $(ALL_CXXSRCS:=.o)

ALL_OBJS += $(ALL_ASM_OBJS) $(ALL_C_OBJS) $(ALL_CXX_OBJS)

ALL_DEPS := $(ALL_OBJS:=.d)

CLEAN_OBJS += $(TARGET).elf $(TARGET).map $(TARGET).bin $(TARGET).dump $(TARGET).hex $(TARGET).verilog $(ALL_OBJS) $(ALL_DEPS) openocd.log
REAL_CLEAN_OBJS = $(subst /,$(PS), $(CLEAN_OBJS))

ifeq ($(OS),Windows_NT)
SHELL:=$(ComSpec)
endif

# Default goal, placed before dependency includes
all: info $(TARGET).elf

# include dependency files of application
ifneq ($(MAKECMDGOALS),clean)
-include $(ALL_DEPS)
endif

.PHONY: all info help bin dasm upload run_openocd run_gdb clean debug library

info:
	@$(ECHO) Current Configuration: RISCV_ARCH=$(RISCV_ARCH) RISCV_ABI=$(RISCV_ABI)

$(TARGET).elf: $(ALL_OBJS)
	$(TRACE_LINK)
	$(Q)$(CC) $(CFLAGS) $(ALL_OBJS) -o $@ $(LDFLAGS)
	$(Q)$(SIZE) $@

$(ALL_ASM_OBJS): %.o: % $(COMMON_PREREQS)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) $(ASMFLAGS) -c -o $@ $<

$(ALL_C_OBJS): %.o: % $(COMMON_PREREQS)
	$(TRACE_COMPILE)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(ALL_CXX_OBJS): %.o: % $(COMMON_PREREQS)
	$(TRACE_COMPILE)
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

dasm: $(TARGET).elf
	$(OBJDUMP) -S -D $< > $(TARGET).dump
	$(OBJDUMP) -S -d $< > $(TARGET).dasm
	$(OBJCOPY) $< -O ihex $(TARGET).hex

bin: $(TARGET).elf
	$(OBJCOPY) --dump-section  .preamble=tmp_preamble.bin $<
	$(OBJCOPY) -j .text -j .got -j .rodata $< -O binary tmp_content.bin
ifeq ($(OS),Windows_NT)
# Use windows copy to merge two binary
	copy /BY tmp_preamble.bin + tmp_content.bin $(TARGET).bin
else
# Use linux cat to merge two binary
	cat tmp_preamble.bin > $(TARGET).bin
	cat tmp_content.bin >> $(TARGET).bin
endif


clean:
	-@$(ECHO) "Clean all build objects"
	-$(Q)$(RM) $(REAL_CLEAN_OBJS)
