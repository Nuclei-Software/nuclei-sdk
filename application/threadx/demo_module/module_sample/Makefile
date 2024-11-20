TARGET := threadx_module_sample

# Configuration parameters
RISCV_ARCH ?= rv32imac
RISCV_ABI ?= ilp32

COMMON_FLAGS := -Og -g -fPIC -mno-plt

NUCLEI_SDK_ROOT = ../../../..

THREADX_ROOT := $(NUCLEI_SDK_ROOT)/OS/ThreadX

SRCDIRS = . $(THREADX_ROOT)/common_modules/module_lib/src $(THREADX_ROOT)/ports/nuclei/module_lib/src

INCDIRS = . $(THREADX_ROOT)/common_modules/inc $(THREADX_ROOT)/common/inc $(THREADX_ROOT)/ports/nuclei

OPENOCD_CFG :=
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
MKD =mkdir -p
Q=
PS=/

TRACE_CREATE_DIR    = @$(ECHO) "Creating Directory : " $(@D)
TRACE_COMPILE       = @$(ECHO) "Compiling  : " $<
TRACE_ASSEMBLE      = @$(ECHO) "Assembling : " $<
TRACE_LINK          = @$(ECHO) "Linking    : " $@
TRACE_ARCHIVE       = @$(ECHO) "Archiving  : " $@

GC_CFLAGS = #-ffunction-sections -fdata-sections
GC_LDFLAGS = #-Wl,--gc-sections -Wl,--check-sections

MKDEP_OPT = -MMD -MT $@ -MF $@.d
C_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(C_INCDIRS)),-I$(dir))
CXX_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(CXX_INCDIRS)),-I$(dir))
ASM_INCLUDE_OPT = $(foreach dir,$(sort $(INCDIRS) $(ASM_INCDIRS)),-I$(dir))

COMMON_FLAGS += -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany \
		$(GC_CFLAGS) -fno-common

CFLAGS += $(COMMON_FLAGS) $(C_INCLUDE_OPT) $(MKDEP_OPT)
CXXFLAGS += $(COMMON_FLAGS) $(CXX_INCLUDE_OPT) $(MKDEP_OPT)
ASMFLAGS += $(COMMON_FLAGS) $(ASM_INCLUDE_OPT) $(MKDEP_OPT)

LIB_OPT = $(addprefix -L, $(sort $(LIBDIRS)))

LDFLAGS += -T $(LINKER_SCRIPT)  -e _txm_module_thread_shell_entry -nostartfiles -nodefaultlibs \
	-Wl,-Map=$(TARGET).map $(GC_LDFLAGS) $(LIB_OPT) $(LDLIBS)


# Set your GDB port using variable GDB_PORT
GDB_PORT ?= 3333

GDB_UPLOAD_ARGS ?= --batch
GDB_UPLOAD_CMDS += -ex "monitor halt"
GDB_UPLOAD_CMDS += -ex "monitor flash protect 0 0 last off"
GDB_UPLOAD_CMDS += -ex "load"
GDB_UPLOAD_CMDS += -ex "monitor resume"
GDB_UPLOAD_CMDS += -ex "monitor shutdown"
GDB_UPLOAD_CMDS += -ex "quit"

OPENOCD_PORT_ARGS = -c "gdb_port $(GDB_PORT)"

OPENOCD_ARGS += -f $(OPENOCD_CFG)
GDB_CMDS += -ex "set remotetimeout 240"
GDB_CMDS += -ex "target extended-remote localhost:$(GDB_PORT)"

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
#	$(Q)$(SIZE) $@

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
	cat tmp_preamble.bin > $(TARGET).bin
	cat tmp_content.bin >> $(TARGET).bin

upload: $(TARGET).elf
	@$(ECHO) "Download and run $<"
	$(GDB) $< -ex "set remotetimeout 240" \
        -ex "target remote | $(OPENOCD) --pipe $(OPENOCD_ARGS)" \
        $(GDB_UPLOAD_ARGS) $(GDB_UPLOAD_CMDS)
	@$(ECHO) "Successfully uploaded $< "

run_openocd:
	@$(ECHO) "Start openocd server"
	$(OPENOCD) $(OPENOCD_PORT_ARGS) $(OPENOCD_ARGS)

run_gdb: $(TARGET).elf
	@$(ECHO) "Run gdb to connect openocd server and debug"
	$(GDB) $< $(GDB_ARGS) $(GDB_CMDS)

debug: $(TARGET).elf
	@$(ECHO) "Download and debug $<"
	$(GDB) $< -ex "set remotetimeout 240" \
        -ex "target remote | $(OPENOCD) --pipe $(OPENOCD_ARGS)"

clean:
	@$(ECHO) "Clean all build objects"
	$(Q)$(RM) $(REAL_CLEAN_OBJS)
