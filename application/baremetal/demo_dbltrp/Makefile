TARGET = demo_dbltrp

NUCLEI_SDK_ROOT = ../../..

# REQUIRE: DBLTRP
# WARNING: only build with XLCFG_DBLTRP=1 when the target CPU really implements
# the Smrnmi/Smdbltrp extensions, otherwise the double trap cannot be recovered
XLCFG_DBLTRP := 1

SRCDIRS = .

INCDIRS = .

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
