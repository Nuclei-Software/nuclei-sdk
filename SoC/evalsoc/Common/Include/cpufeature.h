#ifndef __CPUFEATURE_H__
#define __CPUFEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif


// NOTE: This header file is used to tell us the Nuclei CPU Feature
// see https://doc.nucleisys.com/nuclei_n100_sdk/design/soc/evalsoc.html#usage
// Currently for the evalsoc's cpu feature, we manually create this header file
// but for our customer, you can use nuclei_gen tool to generate it, and just replace it
// If you want to create it on your own, please modify these files:
// cpufeature.h, cpufeature.mk, openocd_evalsoc.cfg and evalsoc.memory
// and the iar project linker script files *.icf
// TODO: You can replace this file using the generated one by nuclei_gen tool

#define CFG_CPU_SERIES              0xFFFFFF
#define CFG_CPU_VER                 0xFFFFFF

// CPU Features
#define CFG_TMR_PRIVATE
#define CFG_HAS_IRQC
// External Interrupt Number
#define CFG_IRQ_NUM                 16

// EXCP is the exception module, if not defined, it means not present
// If not present, you need to remove .exception section in linker script
#define CFG_HAS_EXCP

// Define it when you have TRAP related CSR such as MTVT and MTVEC
#define CFG_HAS_TRAP_CSR

// PMONITOR is the mcycle and minsret counter, if not defined, it means not present
#define CFG_HAS_PMONITOR

#define CFG_ILM_BASE_ADDR           (0x80000000UL)
#define CFG_DLM_BASE_ADDR           (0x90000000UL)

#define CFG_MTVEC_BASE_ADDR         (0x80000180)
#define CFG_MTVT_BASE_ADDR          (0x80000100)

// EVALSOC Features
#define EVALSOC_FLASH_XIP_BASE      (0x20000000UL)
#define EVALSOC_SYSMEM_BASE         (0xA0000000UL)
#define EVALSOC_PERIPS_BASE         (0x10000000UL)

#ifdef __cplusplus
}
#endif

#endif
