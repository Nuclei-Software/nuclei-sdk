TARGET = demo_dsp

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
INCDIRS = .

COMMON_FLAGS ?=
# Select NMSIS Library
## - nmsis_dsp : select dsp library
## - nmsis_nn : select nn library
## - nmsis_dsp nmsis_nn : select dsp and nn library
## see NMSIS/build.mk
NMSIS_LIB ?= nmsis_dsp

STDCLIB ?= newlib_small

# We provide prebuilt optimized NMSIS DSP/NN library
# Optimized for the following extensions:
# B extension: _zba_zbb_zbc_zbs
# P extension: _xxldspn1x
# V extension: _zve32f for rv32, v for rv64
# The naming order use in ARCH_EXT, should be V, B, P
# to get correct NMSIS DSP/NN library
# Example as below:
# If I want to use Nuclei BPV extensions
# For RV32: when ARCH_EXT=_zve32f_zba_zbb_zbc_zbs_xxldspn1x
# For RV64: when ARCH_EXT=v_zba_zbb_zbc_zbs_xxldspn1x
# see NMSIS/build.mk
ARCH_EXT ?=
LDLIBS ?= -lm

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
