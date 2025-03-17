TARGET = freertos_fprvv_demo
RTOS = FreeRTOS

NUCLEI_SDK_ROOT = ../../..

COMMON_FLAGS := -Os
CORE := nx900fd
ARCH_EXT := v_zvl128b
DOWNLOAD := ddr
STDCLIB := newlib_small

SRCDIRS = .
INCDIRS = .

LDLIBS := -lm

TOOLCHAIN ?= nuclei_gnu
# Disable autovectorization due to timer task
# code will generate rvv code
AUTOVEC := 0

# Include toolchain specified options
-include toolchain_$(TOOLCHAIN).mk

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
