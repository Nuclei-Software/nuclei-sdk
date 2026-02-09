TARGET = demo_ecc

NUCLEI_SDK_ROOT = ../../..

# REQUIRE: ECC
# XLCFG_ECC defines the ECC operational or injection mode:
# [Leave Empty] - Default: No macro defined. Assumes ECC is present using "Direct Write" mode.
#                 See 'cpufeature.h' in evalsoc for more details.
# 0             - Disabled: Not support ECC.
# 1             - Direct Write Mode: Support ECC error injection with Direct Write mode.
# 2             - XOR Mode: Support ECC error injection with XOR mode.
XLCFG_ECC :=

# OPTIONAL: CCM, PMA, ICACHE, DCACHE, SMPCC
XLCFG_CCM := 1
XLCFG_PMA :=
XLCFG_ICACHE :=
XLCFG_DCACHE :=
XLCFG_SMPCC :=

SRCDIRS = .

INCDIRS = .

COMMON_FLAGS := -O2

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
