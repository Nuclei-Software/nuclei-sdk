#ifndef __CPUFEATURE_H__
#define __CPUFEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif


// NOTE: This header file is used to tell us the Nuclei CPU Feature
// Currently for the opensource evalsoc's cpu feature, we manually create this header file
// but for our customer, you can use nuclei_gen tool to generate it, and just replace it
// TODO: You can replace this file using the generated one by nuclei_gen tool

#define CFG_CPU_SERIES              0xFFFFFF
#define CFG_CPU_VER                 0xFFFFFF

// CPU Features
#define CFG_TMR_PRIVATE
#define CFG_HAS_IRQC
// External Interrupt Number
#define CFG_IRQ_NUM                 30

#define CFG_HAS_EXCP
#define CFG_HAS_PMONITOR

// CCM default still set to 0, you can uncomment it when it really exist
// #define CFG_HAS_IOCC
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
