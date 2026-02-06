#ifndef __CPUFEATURE_H__
#define __CPUFEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif


// NOTE: This header file is used to tell us the Nuclei CPU Feature
// Currently for the evalsoc's cpu feature, we manually create this header file
// but for our customer, you can use nuclei_gen tool to generate it, and just replace it
// If you want to create it on your own, please modify these files:
// cpufeature.h, cpufeature.mk, openocd_evalsoc.cfg and evalsoc.memory
// and the iar project linker script files *.icf

// TODO: You can replace this file using the generated one by `nuclei_gen` tool

// NOTE:
// These `XLCFG_xxx` macros are defined by `SoC/evalsoc/cpuconfig.mk` and it should be only used
// in this not generated `cpufeature.h` file.
// It will control the RTL defined macros when CPU RTL configuration defined, for example:
// - When XLCFG_SMPU is defined as 1, it will define CFG_HAS_SMPU
// - When XLCFG_ECLIC is defined as 2, it will define CFG_HAS_ECLICV2 and CFG_HAS_CLIC
// - When XLCFG_ECLIC is defined as 0, it will undef CFG_HAS_CLIC
// These feature allow dynamic enable/disable cpu feature via make command when this cpufeature.h is not generated,
// it is useful for internal testing for different bitstream file with different CPU RTL configuration

// NOTE: CPU FEATURE CONFIGURATION MAP:
// ENABLED_BY_DEFAULT = [
//   "ECLIC", "SYSTIMER", "SMPCC", "PMP", "SMEPMP", "NICE", "VNICE",
//   "EXCP", "ILM", "DLM", "ICACHE", "DCACHE", "UMODE", "PMA", "PMA_MACRO",
//   "STACK_CHECK", "AMO", "ECC", "MISALIGNED_ACCESS", "SRAM", "DDR", "HPM-v1"
// ]
// DISABLED_BY_DEFAULT = [
//   "CCM", "CIDU", "TEE", "SMPU", "SPMP", "SSTC", "SMODE", "PLIC"
// ]
//
// The CFG_IREGION_BASE_ADDR is only defined in generated cpufeature.h or if you know the
// cpu iregion baseaddr you can set it

// #define CFG_IREGION_BASE_ADDR    YOUR_CPU_IREGION_BASE_ADDR

#define CFG_CPU_SERIES              0xFFFFFF
#define CFG_CPU_VER                 0xFFFFFF

// NOTE: By default enabled features

#if !(defined(XLCFG_ECLIC) && XLCFG_ECLIC == 0)
#define CFG_HAS_CLIC
#if XLCFG_ECLIC == 2
#define CFG_HAS_ECLICV2
#endif
#endif

// External Interrupt Number
#if defined(XLCFG_IRQ_NUM)
#define CFG_IRQ_NUM                 XLCFG_IRQ_NUM
#else
#define CFG_IRQ_NUM                 50
#endif

#if !(defined(XLCFG_SYSTIMER) && XLCFG_SYSTIMER == 0)
#define CFG_TMR_PRIVATE
#endif

#if !(defined(XLCFG_SMPCC) && XLCFG_SMPCC == 0)
#define CFG_HAS_SMP
#endif

#if !(defined(XLCFG_PMP) && XLCFG_PMP == 0)
#define CFG_HAS_PMP
#define CFG_HAS_UMODE
#ifdef XLCFG_PMP_ENTRY_NUM
#define CFG_PMP_ENTRY_NUM           XLCFG_PMP_ENTRY_NUM
#else
#define CFG_PMP_ENTRY_NUM           8
#endif
#ifdef XLCFG_PMP_GRAIN
#define CFG_PMP_GRAIN               XLCFG_PMP_GRAIN
#else
#define CFG_PMP_GRAIN               10
#endif
#endif

#if !(defined(XLCFG_SMEPMP) && XLCFG_SMEPMP == 0)
#define CFG_HAS_SMEPMP
#endif

#if !(defined(XLCFG_NICE) && XLCFG_NICE == 0)
#define CFG_HAS_NICE
#endif

#if !(defined(XLCFG_VNICE) && XLCFG_VNICE == 0)
#define CFG_HAS_VNICE
#endif

#if !(defined(XLCFG_EXCP) && XLCFG_EXCP == 0)
#define CFG_HAS_EXCP
#endif

#if !(defined(XLCFG_ILM) && XLCFG_ILM == 0)
#define CFG_HAS_ILM
#endif

#if !(defined(XLCFG_DLM) && XLCFG_DLM == 0)
#define CFG_HAS_DLM
#endif

#if !(defined(XLCFG_ICACHE) && XLCFG_ICACHE == 0)
#define CFG_HAS_ICACHE
#endif

#if !(defined(XLCFG_DCACHE) && XLCFG_DCACHE == 0)
#define CFG_HAS_DCACHE
#endif

#if !(defined(XLCFG_ECC) && XLCFG_ECC == 0)
#define CFG_HAS_ECC
#if XLCFG_ECC == 2
#define CFG_ECC_CODE_XOR
#endif
#endif

#if !(defined(XLCFG_PMA) && XLCFG_PMA == 0)
#define CFG_HAS_PMA
#if defined(XLCFG_PMA_CSR_NUM)
#define CFG_PMA_CSR_NUM             XLCFG_PMA_CSR_NUM
#else
#define CFG_PMA_CSR_NUM             8
#endif
#if defined(XLCFG_PMA_SEC_CSR_NUM)
#define CFG_PMA_SEC_CSR_NUM         XLCFG_PMA_SEC_CSR_NUM
#else
#define CFG_PMA_SEC_CSR_NUM         8
#endif
#endif

#if !(defined(XLCFG_PMA_MACRO) && XLCFG_PMA_MACRO == 0)
#define CFG_HAS_PMA_MACRO
#endif

#if !(defined(XLCFG_STACK_CHECK) && XLCFG_STACK_CHECK == 0)
#define CFG_HAS_STACK_CHECK
#endif

#if !(defined(XLCFG_AMO) && XLCFG_AMO == 0)
#define CFG_HAS_AMO
#endif

#if !(defined(XLCFG_MISALIGNED_ACCESS) && XLCFG_MISALIGNED_ACCESS == 0)
#define CFG_HAS_MISALIGNED_ACCESS
#endif

#if !(defined(XLCFG_SRAM) && XLCFG_SRAM == 0)
#define CFG_HAS_SRAM
#endif

#if !(defined(XLCFG_DDR) && XLCFG_DDR == 0)
#define CFG_HAS_DDR
#endif

#if !(defined(XLCFG_HPM) && XLCFG_HPM == 0)
#define CFG_HAS_HPM
#define CFG_HPM_VER                 XLCFG_HPM
#endif

// NOTE: By default disabled features

#if defined(XLCFG_CCM) && XLCFG_CCM != 0
#define CFG_HAS_CCM
#endif

#if defined(XLCFG_CIDU) && XLCFG_CIDU != 0
#define CFG_HAS_IDU
#endif

#if defined(XLCFG_PLIC) && XLCFG_PLIC != 0
#define CFG_HAS_PLIC
#endif

#if defined(XLCFG_TEE) && XLCFG_TEE != 0
#define CFG_HAS_TEE
#endif

#if defined(XLCFG_UMODE) && XLCFG_UMODE != 0
#ifndef CFG_HAS_UMODE
#define CFG_HAS_UMODE
#endif
#endif

#if defined(XLCFG_SMPU) && XLCFG_SMPU != 0
#define CFG_HAS_SMPU
#endif

#if defined(XLCFG_SPMP) && XLCFG_SPMP != 0
#define CFG_HAS_SPMP
#endif

#if defined(XLCFG_SSTC) && XLCFG_SSTC != 0
#define CFG_HAS_ISA_SSTC
#endif

#if defined(XLCFG_SMODE) && XLCFG_SMODE != 0
#define CFG_HAS_SMODE
#endif

#if defined(XLCFG_CCM) && XLCFG_CCM != 0
#define CFG_HAS_IOCC
#endif


#define CFG_ILM_BASE_ADDR           (0x80000000UL)
#define CFG_DLM_BASE_ADDR           (0x90000000UL)

// EVALSOC Features
#define EVALSOC_FLASH_XIP_BASE      (0x20000000UL)
#define EVALSOC_SYSMEM_BASE         (0xA0000000UL)
#define EVALSOC_PERIPS_BASE         (0x10000000UL)

#ifdef __cplusplus
}
#endif

#endif
