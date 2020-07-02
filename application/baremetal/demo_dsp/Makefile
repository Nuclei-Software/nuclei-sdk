TARGET = demo_dsp

NUCLEI_SDK_ROOT = ../../..
NUCLEI_SDK_NMSIS_ROOT ?= $(NUCLEI_SDK_ROOT)/NMSIS

SRCDIRS = .

INCDIRS = . $(NUCLEI_SDK_NMSIS_ROOT)/DSP/Include

LIBDIRS = $(NUCLEI_SDK_NMSIS_ROOT)/Library/DSP/GCC

DSP_ENABLE ?= ON
COMMON_FLAGS :=
PFLOAT ?= 1

ifeq ($(DSP_ENABLE),ON)
$(warning "Use and link with RISC-V DSP enabled library")
COMMON_FLAGS += -D__RISCV_FEATURE_DSP=1
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH)p -lm
else
$(warning "Use and link with RISC-V DSP disabled library")
LDLIBS = -lnmsis_dsp_$(RISCV_ARCH) -lm
endif

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
