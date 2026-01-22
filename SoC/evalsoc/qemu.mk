# NOTE: options control how to run qemu
## QEMU/XLSPIKE options
ifeq ($(XLCFG_PLIC),1)
QEMU_ELF_OPT := -bios
else
QEMU_ELF_OPT := -kernel
endif

ifneq ($(CPU_CNT),)
QEMU_OPT += -smp $(CPU_CNT)
endif
## xlspike is only valid for nuclei demosoc/evalsoc
ifneq ($(CPU_CNT),)
XLSPIKE_OPT += -p$(CPU_CNT)
endif

# Handle QEMU Emulation
# - QEMU_MC_EXTOPT is used to pass extra options to qemu -M machine options for evalsoc,
#   please dont pass any extra , to this make variable, you can pass such
#   as QEMU_MC_EXTOPT=debug=1 but not pass QEMU_MC_EXTOPT=,debug=1
# - QEMU_CPU_EXTOPT is used to pass extra options to qemu -cpu cpu options for evalsoc,
#   please dont pass any extra , to this make variable, you can pass such
#   as QEMU_CPU_EXTOPT=vlen=512 but not pass QEMU_CPU_EXTOPT=,vlen=512
QEMU_MACHINE ?= nuclei_evalsoc,download=$(DOWNLOAD)
# You can pass customized soc configuration here
ifneq ($(QEMU_SOCCFG),)
QEMU_MACHINE := $(QEMU_MACHINE),soc-cfg=$(NUCLEI_SDK_SOC)/$(QEMU_SOCCFG)
endif
ifneq ($(QEMU_MC_EXTOPT),)
QEMU_MACHINE := $(QEMU_MACHINE),$(QEMU_MC_EXTOPT)
endif

QEMU_ARCHEXT ?= ${ARCH_EXT}
QEMU_CPU ?= nuclei-$(CORE),ext=$(QEMU_ARCHEXT)
ifneq ($(QEMU_CPU_EXTOPT),)
QEMU_CPU := $(QEMU_CPU),$(QEMU_CPU_EXTOPT)
endif

ifneq ($(SEMIHOST),)
QEMU_OPT += -semihosting
endif

