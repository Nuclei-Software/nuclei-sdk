TARGET = threadx_regression_smp
RTOS = ThreadX

# REQUIRE: SMPCC, ECLIC, SYSTIMER
XLCFG_SYSTIMER :=
XLCFG_ECLIC :=
XLCFG_SMPCC :=

SMP ?= 4
THREADX_CORE0_ONLY ?= 1

CORE ?= nx900fd

# Default to DDR because the regression cases are memory hungry.
DOWNLOAD ?= ddr

STACKSZ ?= 2K

# Default to a single SMP case for bring-up and smoke testing.
# Set `CTEST=` explicitly to build/run the whole regression suite, but the
# current all-in-one flow is still known to be unreliable because some cases
# interfere with each other when run together.
CTEST ?= cases/threadx_smp_time_slice_test.c

# Build the imported ThreadX SMP regression suite as an all-in-one runner.
# -DTX_THREAD_ENABLE_PERFORMANCE_INFO
COMMON_FLAGS := -O2 -DBATCH_TEST -DEXTERNAL_MAIN -DEXTERNAL_EXIT -DTEST_STACK_SIZE_PRINTF=4096 -DTX_REGRESSION_TEST -DTX_TIMER_PROCESS_IN_ISR

ifeq ($(THREADX_CORE0_ONLY),1)
COMMON_FLAGS += -DTX_THREAD_SMP_ONLY_CORE_0_DEFAULT
endif

# -fno-tree-tail-merge option is required with >O1 for ThreadX source code correct compiling for gcc
# eg. OS/ThreadX/common/src/tx_mutex_delete.c
-include toolchain_$(TOOLCHAIN).mk

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = .
ifeq ($(CTEST),)
SRCDIRS += cases
EXCLUDE_SRCS = cases/threadx_initialize_kernel_setup_test.c cases/threadx_trace_basic_test.c
else
COMMON_FLAGS += -DCTEST=1
C_SRCS += $(CTEST)
endif
INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
