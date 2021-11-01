TARGET = demo_dsp

NUCLEI_SDK_ROOT = ../../..
NUCLEI_SDK_NMSIS_ROOT ?= $(NUCLEI_SDK_ROOT)/NMSIS

SRCDIRS = .

INCDIRS = . $(NUCLEI_SDK_NMSIS_ROOT)/DSP/Include \
	$(NUCLEI_SDK_NMSIS_ROOT)/DSP/PrivateInclude

LIBDIRS = $(NUCLEI_SDK_NMSIS_ROOT)/Library/DSP/GCC

DSP_ENABLE ?= ON
VECTOR_ENABLE ?= OFF
COMMON_FLAGS :=
PFLOAT ?= 1

ifeq ($(VECTOR_ENABLE),ON)
ifeq ($(DSP_ENABLE),ON)
$(warning "Use and link with Nuclei DSP and Vector optimized DSP library")
COMMON_FLAGS += -D__RISCV_FEATURE_DSP=1 -D__RISCV_FEATURE_VECTOR=1
ARCH_EXT := pv
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH) -lm
else
$(warning "Use and link with Nuclei Vector optimized library")
COMMON_FLAGS += -D__RISCV_FEATURE_VECTOR=1
ARCH_EXT := v
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH) -lm
endif
else
ifeq ($(DSP_ENABLE),ON)
$(warning "Use and link with Nuclei DSP optimized DSP library")
COMMON_FLAGS += -D__RISCV_FEATURE_DSP=1
ARCH_EXT := p
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH) -lm
else
$(warning "Use and link with pure C DSP library")
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH) -lm
endif
endif

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
