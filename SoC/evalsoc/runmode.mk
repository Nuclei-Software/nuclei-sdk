# RUN Mode, to control ILM/DLM/ICACHE/DCACHE enable or disable
# Combined with ILM_EN/DLM_EN/IC_EN/DC_EN/CCM_EN
# only works in evalsoc
RUNMODE ?=

ifneq ($(RUNMODE),) # RUNMODE defined but not empty
# lm mode, ilm/dlm enabled, icache/dcache disabled
ifeq ($(RUNMODE),lm)
ILM_EN ?= 1
DLM_EN ?= 1
IC_EN ?= 0
DC_EN ?= 0
CCM_EN ?= 0
# icache+dlm mode, ilm disabled, icache enabled, dlm enabled, dcache disabled
else ifeq ($(RUNMODE),icdlm)
ILM_EN ?= 0
DLM_EN ?= 1
IC_EN ?= 1
DC_EN ?= 0
CCM_EN ?= 1
# dcache+ilm mode, ilm enabled, icache disabled, dlm disabled, dcache enabled
else ifeq ($(RUNMODE),dcilm)
ILM_EN ?= 1
DLM_EN ?= 0
IC_EN ?= 0
DC_EN ?= 1
CCM_EN ?= 1
# cache mode, ilm/dlm is disabled, icache/dcache enabled
else ifeq ($(RUNMODE),cache)
ILM_EN ?= 0
DLM_EN ?= 0
IC_EN ?= 1
DC_EN ?= 1
CCM_EN ?= 1
# bus mode, ilm/dlm/icache/dcache is disabled by default
else ifeq ($(RUNMODE),bus)
ILM_EN ?= 0
DLM_EN ?= 0
IC_EN ?= 0
DC_EN ?= 0
CCM_EN ?= 0
# all on/cache lm mode, ilm/dlm/icache/dcache enabled
else ifeq ($(RUNMODE),clm)
ILM_EN ?= 1
DLM_EN ?= 1
IC_EN ?= 1
DC_EN ?= 1
CCM_EN ?= 1
# if not match above cases, manual control mode, if not match, default equal lm mode
else
ILM_EN ?= 1
DLM_EN ?= 1
IC_EN ?= 0
DC_EN ?= 0
CCM_EN ?= 0
endif
$(info Do run mode control, RUNMODE=$(RUNMODE), IC_EN=$(IC_EN), DC_EN=$(DC_EN), ILM_EN=$(ILM_EN), DLM_EN=$(DLM_EN), CCM_EN=$(CCM_EN))
COMMON_FLAGS += -DRUNMODE_STRING=\"$(RUNMODE)\" -DRUNMODE_CONTROL
else # RUNMODE is not defined
# please don't define RUNMODE_CONTROL
CCM_EN ?=
ILM_EN ?=
DLM_EN ?=
IC_EN ?=
DC_EN ?=
endif

ifneq ($(ILM_EN),)
COMMON_FLAGS += -DRUNMODE_ILM_EN=$(ILM_EN)
endif

ifneq ($(DLM_EN),)
COMMON_FLAGS += -DRUNMODE_DLM_EN=$(DLM_EN)
endif

ifneq ($(IC_EN),)
COMMON_FLAGS += -DRUNMODE_IC_EN=$(IC_EN)
endif

ifneq ($(DC_EN),)
COMMON_FLAGS += -DRUNMODE_DC_EN=$(DC_EN)
endif

ifneq ($(CCM_EN),)
COMMON_FLAGS += -DRUNMODE_CCM_EN=$(CCM_EN)
endif

LDSPEC_EN ?=
ifneq ($(LDSPEC_EN),)
COMMON_FLAGS += -DRUNMODE_LDSPEC_EN=$(LDSPEC_EN)
endif

L2_EN ?=
ifneq ($(L2_EN),)
COMMON_FLAGS += -DRUNMODE_L2_EN=$(L2_EN)
endif

BPU_EN ?=
ifneq ($(BPU_EN),)
COMMON_FLAGS += -DRUNMODE_BPU_EN=$(BPU_EN)
endif

# NOTE: extra XLCFG_xxx make variables to control
# cpu feature present or not which is only internally used by Nuclei(XinLai)
# This will affect the cpufeature.h and evalsoc.h
XLCFG_SPMP ?=
ifeq ($(XLCFG_SPMP),1)
XLCFG_TEE := 1
COMMON_FLAGS += -DCFG_HAS_SPMP
endif

XLCFG_SMPU ?=
ifeq ($(XLCFG_SMPU),1)
XLCFG_TEE := 1
COMMON_FLAGS += -DCFG_HAS_SMPU
endif

XLCFG_TEE ?=
ifeq ($(XLCFG_TEE),1)
COMMON_FLAGS += -DCFG_HAS_TEE
endif

XLCFG_PMP ?=
ifeq ($(XLCFG_PMP),1)
COMMON_FLAGS += -DCFG_HAS_PMP
endif

XLCFG_CCM ?=
ifeq ($(XLCFG_CCM),1)
COMMON_FLAGS += -DCFG_HAS_IOCC
endif

XLCFG_ECLIC ?=
ifeq ($(XLCFG_ECLIC),1)
COMMON_FLAGS += -DCFG_HAS_CLIC
endif

XLCFG_SYSTIMER ?=
ifeq ($(XLCFG_SYSTIMER),1)
COMMON_FLAGS += -DCFG_TMR_PRIVATE
endif

XLCFG_CIDU ?=
ifeq ($(XLCFG_CIDU),1)
COMMON_FLAGS += -DCFG_HAS_IDU
endif

XLCFG_SMPCC ?=
ifeq ($(XLCFG_SMPCC),1)
COMMON_FLAGS += -DCFG_HAS_SMP
endif

