NMSIS_LIB_SORTED := $(sort $(NMSIS_LIB))
NMSIS_LIB_ARCH ?= $(RISCV_ARCH)

INCDIRS += $(NUCLEI_SDK_NMSIS)/Core/Include

ifeq ($(filter nmsis_nn,$(NMSIS_LIB_SORTED)),nmsis_nn)
INCDIRS += $(NUCLEI_SDK_NMSIS)/NN/Include
LIBDIRS += $(NUCLEI_SDK_NMSIS)/Library/NN/GCC
LDLIBS += -lnmsis_nn_$(NMSIS_LIB_ARCH)
endif

ifeq ($(filter nmsis_dsp,$(NMSIS_LIB_SORTED)),nmsis_dsp)
INCDIRS += $(NUCLEI_SDK_NMSIS)/DSP/Include \
	$(NUCLEI_SDK_NMSIS)/DSP/PrivateInclude
LIBDIRS += $(NUCLEI_SDK_NMSIS)/Library/DSP/GCC
LDLIBS += -lnmsis_dsp_$(NMSIS_LIB_ARCH)
endif

ifneq ($(findstring p,$(ARCH_EXT)),)
COMMON_FLAGS += -D__RISCV_FEATURE_DSP=1
endif

ifneq ($(findstring v,$(ARCH_EXT)),)
COMMON_FLAGS += -D__RISCV_FEATURE_VECTOR=1
endif
