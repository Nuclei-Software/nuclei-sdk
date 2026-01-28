TARGET = demo_clint_timer

NUCLEI_SDK_ROOT = ../../..

# NOTE: When interrupts are working in CLINT/PLIC mode,
# even if ECLIC v2 is configured, the auto context save/restore feature
# must be disabled by overriding ECLIC_HWCTX to 0
override ECLIC_HWCTX := 0

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
