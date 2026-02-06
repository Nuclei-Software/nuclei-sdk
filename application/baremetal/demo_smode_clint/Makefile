TARGET = demo_smode_clint

NUCLEI_SDK_ROOT = ../../..

# REQUIRE: SSTC, PMP, SYSTIMER
# set to 1 if present
XLCFG_SSTC := 1
XLCFG_PMP :=
XLCFG_SYSTIMER :=

# NOTE: When interrupts are working in CLINT/PLIC mode,
# even if ECLIC v2 is configured, the auto context save/restore feature
# must be disabled by overriding ECLIC_HWCTX to 0
override ECLIC_HWCTX := 0

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
