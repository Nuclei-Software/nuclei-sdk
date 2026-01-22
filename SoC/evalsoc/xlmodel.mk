# NOTE: options control how to run xlmodel
# Handle XLModel Emulation
XLMODEL_OPT += -M nuclei_evalsoc
XLMODEL_OPT += --cpu=$(CORE) --download=$(DOWNLOAD)
ifneq ($(ARCH_EXT),)
XLMODEL_OPT += --ext=$(ARCH_EXT)
endif
ifneq ($(CPU_CNT),)
XLMODEL_OPT += --smp=$(CPU_CNT)
endif

