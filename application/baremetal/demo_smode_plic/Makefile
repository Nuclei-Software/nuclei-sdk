TARGET = demo_smode_plic

NUCLEI_SDK_ROOT = ../../..

# NOTE: When interrupts are working in CLINT/PLIC mode,
# even if ECLIC v2 is configured, the auto context save/restore feature
# must be disabled by overriding ECLIC_HWCTX to 0
override ECLIC_HWCTX := 0

# Require S-Mode and PLIC and PMP extension
# set to 1 if present
XLCFG_PLIC :=

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
