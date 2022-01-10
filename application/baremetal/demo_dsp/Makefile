TARGET = demo_dsp

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
INCDIRS = .

COMMON_FLAGS ?=
# Select NMSIS Library
## - nmsis_dsp : select dsp library
## - nmsis_dsp nmsis_nn : select nn library
## see NMSIS/build.mk
NMSIS_LIB ?= nmsis_dsp

STDCLIB ?= newlib_small

# when ARCH_EXT=pv P-ext and V-ext will be enabled
# and P-ext/V-ext optimized library will be selected
# see NMSIS/build.mk
ARCH_EXT ?=
LDLIBS ?= -lm

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
