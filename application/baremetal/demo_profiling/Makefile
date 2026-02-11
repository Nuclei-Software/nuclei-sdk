# This demo is intended for use in Nuclei Studio IDE
# For more information, visit: https://doc.nucleisys.com/nuclei_sdk/design/app.html#demo-profiling
# Name of the executable target for this application
TARGET = demo_profiling

# Enable profiling middleware for this application
# Middleware components extend SDK functionality
MIDDLEWARE := profiling

# Path to the Nuclei SDK root directory
NUCLEI_SDK_ROOT = ../../..

# Source directories containing application source files
SRCDIRS = . src

# Include directories containing header files
INCDIRS = . inc

# Standard C library selection for this application
# Use newlib_small to ensure printf formatters (e.g., %02) work properly with profiling middleware
# Note: newlib_nano does not support certain printf formatters used by the profiling middleware
STDCLIB ?= newlib_small

# Memory region for downloading and running the application
# NOTE: Profiling and Coverage need a lot of memory, so please make sure
# your RAM is enough, here we recommend to use sram or ddr download mode
DOWNLOAD ?= sram

# Optimization level set to none for accurate profiling results
# Higher optimization levels can interfere with profiling accuracy
COMMON_FLAGS := -O0

# Application-specific compilation options that can be used for
# profiling, coverage, and other app-specific settings
# -pg enables profiling instrumentation
# -coverage enables coverage analysis (requires more memory)
# NOTE: when -coverage is passed, then heap will not be enough
# you need to change DOWNLOAD to ddr
# when Zc extension used, -fomit-frame-pointer is passed but
#  -pg and -fomit-frame-pointer are incompatible
APP_COMMON_FLAGS :=

# App-specific C compilation flags
APP_CFLAGS :=
# App-specific C++ compilation flags
APP_CXXFLAGS :=
# App-specific Assembly compilation flags
APP_ASMFLAGS :=

# Specify directories that need app-specific compilation flags
# If left empty, APP_XXXFLAGS will be applied to all source files
APPDIRS := . src

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
