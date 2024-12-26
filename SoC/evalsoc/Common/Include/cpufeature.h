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

// TODO: You can replace this file using the generated one by nuclei_gen tool

// The CFG_IREGION_BASE_ADDR is only defined in generated cpufeature.h or if you know the
// cpu iregion baseaddr you can set it

// #define CFG_IREGION_BASE_ADDR    YOUR_CPU_IREGION_BASE_ADDR

#define CFG_CPU_SERIES              0xFFFFFF
#define CFG_CPU_VER                 0xFFFFFF

// CPU Features
#ifndef UNDEF_CFG_TMR_PRIVATE
#define CFG_TMR_PRIVATE
#endif
#ifndef UNDEF_CFG_HAS_CLIC
#define CFG_HAS_CLIC
#endif
//#define CFG_HAS_PLIC

// External Interrupt Number
#define CFG_IRQ_NUM                 50

#ifndef UNDEF_CFG_HAS_SMP
#define CFG_HAS_SMP
#endif

#ifndef UNDEF_CFG_HAS_PMP
#define CFG_HAS_PMP
#endif

#define CFG_PMP_ENTRY_NUM           8

//#define CFG_HAS_HPM
#define CFG_HPM_VER                 1

//#define CFG_HAS_IDU
//#define CFG_HAS_TEE
//#define CFG_HAS_SMPU
#define CFG_HAS_PMA
#define CFG_HAS_PMA_MACRO
#define CFG_PMA_CSR_NUM             8
#define CFG_PMA_SEC_CSR_NUM         8
#define CFG_HAS_STACK_CHECK
#define CFG_HAS_NICE
#define CFG_HAS_VNICE

#define CFG_HAS_ICACHE
#define CFG_HAS_DCACHE

// CCM default still set to 0, you can uncomment it when it really exist
// #define CFG_HAS_IOCC
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
