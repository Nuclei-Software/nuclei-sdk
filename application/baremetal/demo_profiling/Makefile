# This demo is intended for use in Nuclei Studio IDE
# For more information, visit: https://doc.nucleisys.com/nuclei_sdk/design/app.html#demo-profiling
TARGET = demo_profiling

# Enable profiling middleware for this application
MIDDLEWARE := profiling

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

# Use newlib_small to ensure printf formatters (e.g., %02) work properly with profiling middleware
# Note: newlib_nano does not support certain printf formatters used by the profiling middleware
STDCLIB ?= newlib_small

DOWNLOAD ?= sram

# Optimization level set to none for accurate profiling results
COMMON_FLAGS := -O0

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
