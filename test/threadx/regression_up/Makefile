TARGET = threadx_regression_up
RTOS = ThreadX

# REQUIRE: ECLIC, SYSTIMER
XLCFG_SYSTIMER :=
XLCFG_ECLIC :=

DOWNLOAD ?= ddr

CTEST ?=

# Build the ThreadX UP regression suite as an all-in-one runner.
# -DTX_THREAD_ENABLE_PERFORMANCE_INFO
# -DTX_TIMER_PROCESS_IN_ISR
COMMON_FLAGS := -O2 -DEXTERNAL_MAIN -DEXTERNAL_EXIT -DTEST_STACK_SIZE_PRINTF=2048 -DTX_REGRESSION_TEST

# -fno-tree-tail-merge option is required with >O1 for ThreadX source code correct compiling for gcc
-include toolchain_$(TOOLCHAIN).mk

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
ifeq ($(CTEST),)
SRCDIRS += cases
EXCLUDE_SRCS = cases/threadx_initialize_kernel_setup_test.c
else
COMMON_FLAGS += -DCTEST=1
C_SRCS += $(CTEST)
endif
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
