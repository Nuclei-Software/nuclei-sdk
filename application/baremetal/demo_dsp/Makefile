TARGET = demo_dsp

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
INCDIRS = .

COMMON_FLAGS ?=
NMSIS_LIB ?= nmsis_dsp

STDCLIB ?= newlib_small

# when ARCH_EXT=pv P-ext and V-ext will be enabled
# and P-ext/V-ext optimized library will be selected
# see NMSIS/build.mk
ARCH_EXT ?=
LDLIBS ?= -lm

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
