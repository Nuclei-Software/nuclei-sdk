/******************************************************************************
 * @file     evalsoc.h
 * @brief    NMSIS Core Peripheral Access Layer Header File for
 *           Nuclei Eval SoC which support Nuclei N/NX class cores
 * @version  V1.00
 * @date     22. Nov 2019
 ******************************************************************************/
/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __EVALSOC_H__
#define __EVALSOC_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: this cpufeature.h header file is introduced in Nuclei SDK 0.6.0
#include "cpufeature.h"

/** @addtogroup Nuclei
  * @{
  */


/** @addtogroup evalsoc
  * @{
  */


/** @addtogroup Configuration_of_NMSIS
  * @{
  */

/** \brief SoC Download mode definition */
typedef enum {
    DOWNLOAD_MODE_FLASHXIP = 0,         /*!< Flashxip download mode */
    DOWNLOAD_MODE_FLASH = 1,            /*!< Flash download mode */
    DOWNLOAD_MODE_ILM = 2,              /*!< ilm download mode */
    DOWNLOAD_MODE_DDR = 3,              /*!< ddr download mode */
    DOWNLOAD_MODE_SRAM = 4,             /*!< sram download mode */
    DOWNLOAD_MODE_SRAMXIP = 5,          /*!< sramxip download mode */
    DOWNLOAD_MODE_MAX,
} DownloadMode_Type;

// IRegion_Info_Type structure is removed in Nuclei SDK 0.6.0 release

/* Simulation mode macros */
#define SIMULATION_MODE_XLSPIKE   0     /*!< xlspike simulation mode */
#define SIMULATION_MODE_QEMU      1     /*!< qemu simulation mode */

/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

/* evalsoc's External IRQn ID is from the hard-wired persperctive, which has an offset mapped to the ECLIC IRQn.
   eg.: uart0's external interrupt id in evalsoc is 32, while its ECLIC IRQn is 51 */
#define SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET      19
/* get evalsoc's External IRQn from ECLIC external IRQn which indexs from 19 */
#define IRQn_MAP_TO_EXT_ID(IRQn)                   (IRQn - SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET)

typedef enum IRQn {
    /* =======================================  Nuclei Core Specific Interrupt Numbers  ======================================== */

    Reserved0_IRQn            =   0,              /*!<  Internal reserved */
    SysTimerSW_S_IRQn         =   1,              /*!<  System Timer supervisor mode SW interrupt triggered by ssip  */
    Reserved2_IRQn            =   2,              /*!<  Internal reserved */
    SysTimerSW_IRQn           =   3,              /*!<  System Timer machine mode SW interrupt triggered by msip */
    Reserved3_IRQn            =   4,              /*!<  Internal reserved */
    SysTimer_S_IRQn           =   5,              /*!<  System Timer supervisor mode interrupt triggered by stimecmp csr */
    Reserved5_IRQn            =   6,              /*!<  Internal reserved */
    SysTimer_IRQn             =   7,              /*!<  System Timer machine mode interrupt triggered by mtimecmp */
    Reserved6_IRQn            =   8,              /*!<  Internal reserved */
    Reserved7_IRQn            =   9,              /*!<  Internal reserved */
    Reserved8_IRQn            =  10,              /*!<  Internal reserved */
    Reserved9_IRQn            =  11,              /*!<  Internal reserved */
    Reserved10_IRQn           =  12,              /*!<  Internal reserved */
    Reserved11_IRQn           =  13,              /*!<  Internal reserved */
    Reserved12_IRQn           =  14,              /*!<  Internal reserved */
    Reserved13_IRQn           =  15,              /*!<  Internal reserved */
    InterCore_IRQn            =  16,              /*!<  CIDU Inter Core Interrupt */
    Reserved15_IRQn           =  17,              /*!<  Internal reserved */
    Reserved16_IRQn           =  18,              /*!<  Internal reserved */

    /* ===========================================  evalsoc Specific Interrupt Numbers  ========================================= */
    /* ToDo: add here your device specific external interrupt numbers. 19~1023 is reserved number for user. Maxmum interrupt supported
             could get from clicinfo.NUM_INTERRUPT. According the interrupt handlers defined in startup_Device.s
             eg.: Interrupt for Timer#1       eclic_tim1_handler   ->   TIM1_IRQn */
    SOC_INT19_IRQn           = 19,                /*!< Device Interrupt */
    SOC_INT20_IRQn           = 20,                /*!< Device Interrupt */
    SOC_INT21_IRQn           = 21,                /*!< Device Interrupt */
    SOC_INT22_IRQn           = 22,                /*!< Device Interrupt */
    SOC_INT23_IRQn           = 23,                /*!< Device Interrupt */
    SOC_INT24_IRQn           = 24,                /*!< Device Interrupt */
    SOC_INT25_IRQn           = 25,                /*!< Device Interrupt */
    SOC_INT26_IRQn           = 26,                /*!< Device Interrupt */
    SOC_INT27_IRQn           = 27,                /*!< Device Interrupt */
    SOC_INT28_IRQn           = 28,                /*!< Device Interrupt */
    SOC_INT29_IRQn           = 29,                /*!< Device Interrupt */
    SOC_INT30_IRQn           = 30,                /*!< Device Interrupt */
    SOC_INT31_IRQn           = 31,                /*!< Device Interrupt */
    SOC_INT32_IRQn           = 32,                /*!< Device Interrupt */
    SOC_INT33_IRQn           = 33,                /*!< Device Interrupt */
    SOC_INT34_IRQn           = 34,                /*!< Device Interrupt */
    SOC_INT35_IRQn           = 35,                /*!< Device Interrupt */
    SOC_INT36_IRQn           = 36,                /*!< Device Interrupt */
    SOC_INT37_IRQn           = 37,                /*!< Device Interrupt */
    SOC_INT38_IRQn           = 38,                /*!< Device Interrupt */
    SOC_INT39_IRQn           = 39,                /*!< Device Interrupt */
    SOC_INT40_IRQn           = 40,                /*!< Device Interrupt */
    SOC_INT41_IRQn           = 41,                /*!< Device Interrupt */
    SOC_INT42_IRQn           = 42,                /*!< Device Interrupt */
    SOC_INT43_IRQn           = 43,                /*!< Device Interrupt */
    SOC_INT44_IRQn           = 44,                /*!< Device Interrupt */
    SOC_INT45_IRQn           = 45,                /*!< Device Interrupt */
    SOC_INT46_IRQn           = 46,                /*!< Device Interrupt */
    SOC_INT47_IRQn           = 47,                /*!< Device Interrupt */
    SOC_INT48_IRQn           = 48,                /*!< Device Interrupt */
    SOC_INT49_IRQn           = 49,                /*!< Device Interrupt */
    SOC_INT50_IRQn           = 50,                /*!< Device Interrupt */
    SOC_INT51_IRQn           = 51,                /*!< Device Interrupt */
    SOC_INT52_IRQn           = 52,                /*!< Device Interrupt */
    SOC_INT53_IRQn           = 53,                /*!< Device Interrupt */
    SOC_INT54_IRQn           = 54,                /*!< Device Interrupt */
    SOC_INT55_IRQn           = 55,                /*!< Device Interrupt */
    SOC_INT56_IRQn           = 56,                /*!< Device Interrupt */
    SOC_INT57_IRQn           = 57,                /*!< Device Interrupt */
    SOC_INT58_IRQn           = 58,                /*!< Device Interrupt */
    SOC_INT59_IRQn           = 59,                /*!< Device Interrupt */
    SOC_INT60_IRQn           = 60,                /*!< Device Interrupt */
    SOC_INT61_IRQn           = 61,                /*!< Device Interrupt */
    SOC_INT62_IRQn           = 62,                /*!< Device Interrupt */
    SOC_INT63_IRQn           = 63,                /*!< Device Interrupt */
#if defined(CFG_IRQ_NUM)
    SOC_INT_MAX              = CFG_IRQ_NUM + SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET,
#else
    SOC_INT_MAX,
#endif

    PLIC_INT0_IRQn           = 0,
    PLIC_INT1_IRQn           = 1,
    PLIC_INT2_IRQn           = 2,
    PLIC_INT3_IRQn           = 3,
    PLIC_INT4_IRQn           = 4,
    PLIC_INT5_IRQn           = 5,
    PLIC_INT6_IRQn           = 6,
    PLIC_INT7_IRQn           = 7,
    PLIC_INT8_IRQn           = 8,
    PLIC_INT9_IRQn           = 9,
    PLIC_INT10_IRQn           = 10,
    PLIC_INT11_IRQn           = 11,
    PLIC_INT12_IRQn           = 12,
    PLIC_INT13_IRQn           = 13,
    PLIC_INT14_IRQn           = 14,
    PLIC_INT15_IRQn           = 15,
    PLIC_INT16_IRQn           = 16,
    PLIC_INT17_IRQn           = 17,
    PLIC_INT18_IRQn           = 18,
    PLIC_INT19_IRQn           = 19,
    PLIC_INT20_IRQn           = 20,
    PLIC_INT21_IRQn           = 21,
    PLIC_INT22_IRQn           = 22,
    PLIC_INT23_IRQn           = 23,
    PLIC_INT24_IRQn           = 24,
    PLIC_INT25_IRQn           = 25,
    PLIC_INT26_IRQn           = 26,
    PLIC_INT27_IRQn           = 27,
    PLIC_INT28_IRQn           = 28,
    PLIC_INT29_IRQn           = 29,
    PLIC_INT30_IRQn           = 30,
    PLIC_INT31_IRQn           = 31,
    PLIC_INT32_IRQn           = 32,
    PLIC_INT33_IRQn           = 33,
    PLIC_INT34_IRQn           = 34,
    PLIC_INT35_IRQn           = 35,
    PLIC_INT36_IRQn           = 36,
    PLIC_INT37_IRQn           = 37,
    PLIC_INT38_IRQn           = 38,
    PLIC_INT39_IRQn           = 39,
    PLIC_INT40_IRQn           = 40,
    PLIC_INT41_IRQn           = 41,
    PLIC_INT42_IRQn           = 42,
    PLIC_INT43_IRQn           = 43,
#if defined(CFG_IRQ_NUM)
    PLIC_INIT_MAX             = CFG_IRQ_NUM + 1,
#else
    PLIC_INIT_MAX,
#endif
} IRQn_Type;

#ifdef CFG_HAS_CLIC
/* UART0 Interrupt */
/* NOTE: Take care the external uart irq may not work, it require a correct evalsoc cpu configuration */
/* NOTE: For latest 200/300 cpu, this UART0_IRQn maybe SOC_INT19_IRQn */
#define UART0_IRQn                                 SOC_INT51_IRQn
/* QSPI Interrupt */
#define QSPI0_IRQn                                 SOC_INT53_IRQn
#define QSPI1_IRQn                                 SOC_INT54_IRQn
#define QSPI2_IRQn                                 SOC_INT55_IRQn
#else
/* UART0 Interrupt */
#define UART0_IRQn                                 PLIC_INT33_IRQn
/* QSPI Interrupt */
#define QSPI0_IRQn                                 PLIC_INT35_IRQn
#define QSPI1_IRQn                                 PLIC_INT36_IRQn
#define QSPI2_IRQn                                 PLIC_INT37_IRQn
#endif

#define PLIC_UART0_IRQn                            PLIC_INT33_IRQn


/* =========================================================================================================================== */
/* ================                                  Exception Code Definition                                ================ */
/* =========================================================================================================================== */

typedef enum EXCn {
    /* =======================================  Nuclei N/NX Specific Exception Code  ======================================== */
    InsUnalign_EXCn          =   0,              /*!<  Instruction address misaligned */
    InsAccFault_EXCn         =   1,              /*!<  Instruction access fault */
    IlleIns_EXCn             =   2,              /*!<  Illegal instruction */
    Break_EXCn               =   3,              /*!<  Beakpoint */
    LdAddrUnalign_EXCn       =   4,              /*!<  Load address misaligned */
    LdFault_EXCn             =   5,              /*!<  Load access fault */
    StAddrUnalign_EXCn       =   6,              /*!<  Store or AMO address misaligned */
    StAccessFault_EXCn       =   7,              /*!<  Store or AMO access fault */
    UmodeEcall_EXCn          =   8,              /*!<  Environment call from User mode */
    SmodeEcall_EXCn          =   9,              /*!<  Environment call from S-mode */
    MmodeEcall_EXCn          =  11,              /*!<  Environment call from Machine mode */
    InsPageFault_EXCn        =  12,              /*!<  Instruction page fault */
    LdPageFault_EXCn         =  13,              /*!<  Load page fault */
    StPageFault_EXCn         =  15,              /*!<  Store or AMO page fault */
    StackOverflow_EXCn       =  24,              /*!<  Stack overflow fault */
    StackUnderflow_EXCn      =  25,              /*!<  Stack underflow fault */
    NMI_EXCn                 =  0xfff,           /*!<  NMI interrupt */
} EXCn_Type;

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */
// NOTE: macros __NUCLEI_CORE_REV/__NUCLEI_N_REV/__NUCLEI_NX_REV are removed now

// NOTE: __FPU_PRESENT/__BITMANIP_PRESENT/__DSP_PRESENT/__VECTOR_PRESENT can be probed by compiler's -march= option
// See https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html

/*!< Set to 0, 1, or 2, 0 not present, 1 single floating point unit present, 2 double floating point unit present */
#if !defined(__riscv_flen)
#define __FPU_PRESENT             0
#elif __riscv_flen == 32
#define __FPU_PRESENT             1
#else
#define __FPU_PRESENT             2
#endif

#if defined(__riscv_bitmanip)
#define __BITMANIP_PRESENT        1                     /*!< Set to 1 if Bitmainpulation extension is present */
#else
#define __BITMANIP_PRESENT        0                     /*!< Set to 1 if Bitmainpulation extension is present */
#endif
#if defined(__riscv_dsp)
#define __DSP_PRESENT             1                     /*!< Set to 1 if Partial SIMD(DSP) extension is present */
#else
#define __DSP_PRESENT             0                     /*!< Set to 1 if Partial SIMD(DSP) extension is present */
#endif
#if defined(__riscv_vector)
#define __VECTOR_PRESENT          1                     /*!< Set to 1 if Vector extension is present */
#else
#define __VECTOR_PRESENT          0                     /*!< Set to 1 if Vector extension is present */
#endif


// CPU IREGION Private Peripherals Offset against IREGION base specified by mirgb_info CSR
// These offset information can be found in selected cpu series databook's IREGION section.
// mirgb_info CSR can be found in Nuclei RISC-V ISA Spec
// You can also probe the cpu information using the general application/baremetal/cpuinfo case
// You can find the offset macros(IREGION_*_OFS) in riscv_encoding.h via search IREGION Offsets

// NORMAL CPU Configuration: From Nuclei SDK 0.6.0, we recommend you to update the CPU configuration macros defined in cpufeature.h
// Please don't modify the macros below directly

// NOTE: We use macros defined in cpufeature.h
// WARNING: Please dont modify macros directly below, you can change in cpufeature.h

// CPU Series and Version Configuration
// To set CPU REV and SERIES, just define CFG_CPU_VER/CFG_CPU_SERIES macros in cpufeature.h
#define __NUCLEI_CPU_REV            CFG_CPU_VER           /*!< Nuclei CPU Core Revision, version X.Y.Z, this is for the CPU Core Version, you get from Nuclei, eg. N300 v3.10.1, it should be 0x030A01 */
#define __NUCLEI_CPU_SERIES         CFG_CPU_SERIES        /*!< Nuclei CPU Series, such as 200/300/600/900, eg. 900 will be 0x0900 */

// CPU IREGION Base Address
// To set IREGION base, just define macro CFG_IREGION_BASE_ADDR in cpufeature.h
#ifndef CFG_IREGION_BASE_ADDR
// it is defined in system_evalsoc.c, you should not use this variable CpuIRegionBase
// SystemIRegionInfo variable in previous release is removed, you should avoid to use it
// you should use macro __IREGION_BASEADDR defined in evalsoc.h
extern volatile unsigned long CpuIRegionBase;
#define CPU_IREGION_BASE            CpuIRegionBase
#else
#define CPU_IREGION_BASE            CFG_IREGION_BASE_ADDR
#endif
#define __IREGION_BASEADDR          (CPU_IREGION_BASE)
#define __IINFO_BASEADDR            (__IREGION_BASEADDR + IREGION_IINFO_OFS)
#define __IINFO_MPASIZE_OFS         0
#define __IINFO_MPASIZE_ADDR        (__IINFO_BASEADDR + __IINFO_MPASIZE_OFS)

// ECLIC Configuration
// To enable ECLIC, just define macro CFG_HAS_CLIC/CFG_CLICINTCTLBITS/CFG_IRQ_NUM in cpufeature.h
#ifdef CFG_HAS_CLIC
#define __ECLIC_PRESENT             1
#ifdef CFG_CLICINTCTLBITS
#define __ECLIC_INTCTLBITS          CFG_CLICINTCTLBITS
#endif
#define __ECLIC_INTNUM              (CFG_IRQ_NUM + SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET)
#else
#define __ECLIC_PRESENT             0
#endif
#define __ECLIC_BASEADDR            (__IREGION_BASEADDR + IREGION_ECLIC_OFS)

// PLIC Configuration
// To enable PLIC, just define macro CFG_HAS_PLIC/CFG_IRQ_NUM in cpufeature.h
#ifdef CFG_HAS_PLIC
#define __PLIC_PRESENT              1
#define __PLIC_INTNUM               (CFG_IRQ_NUM + 1)
#else
#define __PLIC_PRESENT              0
#endif
#define __PLIC_BASEADDR             (__IREGION_BASEADDR + IREGION_PLIC_OFS)

// CPU System Timer Configuration
// To enable CPU System Timer, just define macro CFG_TMR_PRIVATE in cpufeature.h
#if defined(CFG_TMR_PRIVATE) || defined(CFG_TMR_BASE_ADDR)
#define __SYSTIMER_PRESENT          1
#else
#define __SYSTIMER_PRESENT          0
#endif
#ifdef CFG_TMR_BASE_ADDR    // Maybe using timer out of cpu for evalsoc
#define __SYSTIMER_BASEADDR         (CFG_TMR_BASE_ADDR)
#else
#define __SYSTIMER_BASEADDR         (__IREGION_BASEADDR + IREGION_TIMER_OFS)
#endif
#define __CLINT_TIMER_BASEADDR      (__SYSTIMER_BASEADDR + 0x1000)

// CPU System Timer SSTC Configuration
#if defined(CFG_HAS_ISA_SSTC)
#define __SSTC_PRESENT              1
#else
#define __SSTC_PRESENT              0
#endif

// CIDU Configuration
// To enable CIDU, just define macro CFG_HAS_IDU in cpufeature.h
#ifdef CFG_HAS_IDU
#define __CIDU_PRESENT              1
#else
#define __CIDU_PRESENT              0
#endif

#define __CIDU_BASEADDR             (__IREGION_BASEADDR + IREGION_IDU_OFS)

// SMP & CC Configuration
// To enable SMP & CC, just define macro CFG_HAS_SMP in cpufeature.h
#ifdef CFG_HAS_SMP
#define __SMPCC_PRESENT             1
#else
#define __SMPCC_PRESENT             0
#endif

#define __SMPCC_BASEADDR            (__IREGION_BASEADDR + IREGION_SMP_OFS)

// PMP Configuration
// To enable PMP, just define macro CFG_HAS_PMP/__PMP_ENTRY_NUM in cpufeature.h
#ifdef CFG_HAS_PMP
#define __PMP_PRESENT               1
#define __PMP_ENTRY_NUM             CFG_PMP_ENTRY_NUM
#else
#define __PMP_PRESENT               0
#define __PMP_ENTRY_NUM             0
#endif

// TEE/sPMP Configuration
// To enable TEE, just define macro CFG_HAS_TEE in cpufeature.h
// TEE required PMP, please also make sure CFG_HAS_PMP defined
#ifdef CFG_HAS_TEE
#define __TEE_PRESENT               1
#define __SPMP_PRESENT              1
#define __SPMP_ENTRY_NUM            CFG_PMP_ENTRY_NUM
#ifdef CFG_HAS_SMPU
#define __SMPU_PRESENT              1
#define __SMPU_ENTRY_NUM            __SPMP_ENTRY_NUM
#endif
#else
#define __TEE_PRESENT               0
#define __SPMP_PRESENT              0
#define __SMPU_PRESENT              0
#define __SPMP_ENTRY_NUM            0
#endif

// ICache Configuration
// To enable ICACHE, just define macro CFG_HAS_ICACHE in cpufeature.h
#ifdef CFG_HAS_ICACHE
#define __ICACHE_PRESENT            1
#else
#define __ICACHE_PRESENT            0
#endif

// DCache Configuration
// To enable DCACHE, just define macro CFG_HAS_DCACHE in cpufeature.h
#ifdef CFG_HAS_DCACHE
#define __DCACHE_PRESENT            1
#else
#define __DCACHE_PRESENT            0
#endif

// CCM Configuration
// To enable CCM, just define macro CFG_HAS_IOCC in cpufeature.h
#ifdef CFG_HAS_IOCC
#define __CCM_PRESENT               1
#else
#define __CCM_PRESENT               0
#endif

// PMA Configuration
// To enable PMA, just define macro CFG_HAS_PMA in cpufeature.h
#ifdef CFG_HAS_PMA
#define __PMA_PRESENT               1
#define __PMA_CSR_NUM               CFG_PMA_CSR_NUM
#define __PMA_SEC_CSR_NUM           CFG_PMA_SEC_CSR_NUM

#ifdef CFG_HAS_PMA_MACRO
#define __PMA_MACRO_PRESENT         1
#else
#define __PMA_MACRO_PRESENT         0
#endif
#else
#define __PMA_PRESENT               0
#define __PMA_MACRO_PRESENT         0
#endif

// NICE Configuration
// To enable NICE, just define macro CFG_HAS_NICE in cpufeature.h
#ifdef CFG_HAS_NICE
#define __NICE_PRESENT              1
#else
#define __NICE_PRESENT              0
#endif

// VNICE Configuration
// To enable Vector NICE, just define macro CFG_HAS_VNICE in cpufeature.h
#ifdef CFG_HAS_VNICE
#define __VNICE_PRESENT             1
#else
#define __VNICE_PRESENT             0
#endif

#ifndef __INC_INTRINSIC_API
#define __INC_INTRINSIC_API         0                     /*!< Set to 1 if intrinsic api header files need to be included */
#endif

#define __Vendor_SysTickConfig      0                     /*!< Set to 1 if different SysTick Config is used */
#define __Vendor_EXCEPTION          0                     /*!< Set to 1 if vendor exception hander is present */


/*!< Set this timer hartid if you have only 1 hart in your cpu, and you know the timer hartid, just set it */
//#define __SYSTIMER_HARTID           0


#ifndef __HARTID_OFFSET
/**
 * If your hart index is different to your hartid, you must define this __HARTID_OFFSET macro.
 * For example, if your cpu has 4 harts, and hartid start from 3, so the __HARTID_OFFSET should set to 3.
 * Which means hartid 3-6 means hart index 0-3, this is useful for the timer software interrupt and timer interrupt trigger register location
 */
#define __HARTID_OFFSET             0
#endif

/* Define boot hart id */
#ifndef BOOT_HARTID
/**
 * Choosen boot hart id in current cluster when in soc system, need to align with the value defined in startup_<Device>.S,
 * should start from 0, taken the mhartid bit 0-7 value
 */
#define BOOT_HARTID                 0
#endif

/** @} */ /* End of group Configuration_of_NMSIS */

// NOTE: Run Mode Control internally used Nuclei
// RUNMODE_* macros are defined in SoC/evalsoc/runmode.mk, for internal usage not intend for widely usage
#ifdef RUNMODE_IC_EN
#undef __ICACHE_PRESENT
#define __ICACHE_PRESENT            RUNMODE_IC_EN         /*!< Controlled by macro RUNMODE_IC_EN */
#endif

#ifdef RUNMODE_DC_EN
#undef __DCACHE_PRESENT
#define __DCACHE_PRESENT            RUNMODE_DC_EN         /*!< Controlled by macro RUNMODE_DC_EN */
#endif

#ifdef RUNMODE_CCM_EN
#undef __CCM_PRESENT
#define __CCM_PRESENT               RUNMODE_CCM_EN        /*!< Controlled by macro RUNMODE_CCM_EN */
#endif

#include <nmsis_core.h>                         /*!< Nuclei N/NX class processor and core peripherals */
#include "system_evalsoc.h"                     /*!< evalsoc System */

/* ========================================  Start of section using anonymous unions  ======================================== */

#ifdef EVALSOC_RTC_FREQ
#define RTC_FREQ                    EVALSOC_RTC_FREQ
#else
#define RTC_FREQ                    32768
#endif
// The TIMER frequency is just the RTC frequency
#define SOC_TIMER_FREQ              RTC_FREQ


/* =========================================================================================================================== */
/* ================                            Device Specific Peripheral Section                             ================ */
/* =========================================================================================================================== */


/** @addtogroup Device_Peripheral_peripherals
  * @{
  */

/****************************************************************************
 * Platform definitions
 *****************************************************************************/
// Interrupt Numbers, will be removed in future, please use SOC_INT_MAX - SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET
#define SOC_ECLIC_NUM_INTERRUPTS    (SOC_INT_MAX - SOC_EXTERNAL_MAP_TO_ECLIC_IRQn_OFFSET)

// Interrupt Handler Definitions
#define SOC_MTIMER_HANDLER          eclic_mtip_handler
#define SOC_SOFTINT_HANDLER         eclic_msip_handler

/**
  * @brief UART
  */
typedef struct {
    __IOM uint32_t TXFIFO;
    __IOM uint32_t RXFIFO;
    __IOM uint32_t TXCTRL;
    __IOM uint32_t RXCTRL;
    __IOM uint32_t IE;
    __IOM uint32_t IP;
    __IOM uint32_t DIV;
} UART_TypeDef;

/**
  * @brief QSPI
  */
typedef struct {
    __IOM uint32_t SCKDIV;
    __IOM uint32_t SCKMODE;
    __IOM uint32_t RESERVED0[1];
    __IOM uint32_t FORCE;
    __IOM uint32_t CSID;
    __IOM uint32_t CSDEF;
    __IOM uint32_t CSMODE;
    __IOM uint32_t VERSION;
    __IOM uint32_t RESERVED1[2];
    __IOM uint32_t DELAY0;
    __IOM uint32_t DELAY1;
    __IOM uint32_t RESERVED2[4];
    __IOM uint32_t FMT;
    __IOM uint32_t RESERVED3;
    __IOM uint32_t TXDATA;
    __IOM uint32_t RXDATA;
    __IOM uint32_t TXMARK;
    __IOM uint32_t RXMARK;
    __IOM uint32_t RESERVED4[2];
    __IOM uint32_t FCTRL;
    __IOM uint32_t FFMT;
    __IOM uint32_t RESERVED5[2];
    __IOM uint32_t IE;
    __IOM uint32_t IP;
    __IOM uint32_t FFMT1;
    __IOM uint32_t STATUS;
} QSPI_TypeDef;

/*@}*/ /* end of group evalsoc_Peripherals */


/* =========================================  End of section using anonymous unions  ========================================= */

/* Macros for memory access operations */
#define _REG8P(p, i)                        ((volatile uint8_t *) ((uintptr_t)((p) + (i))))
#define _REG16P(p, i)                       ((volatile uint16_t *) ((uintptr_t)((p) + (i))))
#define _REG32P(p, i)                       ((volatile uint32_t *) ((uintptr_t)((p) + (i))))
#define _REG64P(p, i)                       ((volatile uint64_t *) ((uintptr_t)((p) + (i))))
#define _REG8(p, i)                         (*(_REG8P(p, i)))
#define _REG16(p, i)                        (*(_REG16P(p, i)))
#define _REG32(p, i)                        (*(_REG32P(p, i)))
#define _REG64(p, i)                        (*(_REG64P(p, i)))
#define REG8(addr)                          _REG8((addr), 0)
#define REG16(addr)                         _REG16((addr), 0)
#define REG32(addr)                         _REG32((addr), 0)
#define REG64(addr)                         _REG64((addr), 0)

/* Macros for address type convert and access operations */
#define ADDR16(addr)                        ((uint16_t)(uintptr_t)(addr))
#define ADDR32(addr)                        ((uint32_t)(uintptr_t)(addr))
#define ADDR64(addr)                        ((uint64_t)(uintptr_t)(addr))
#define ADDR8P(addr)                        ((uint8_t *)(uintptr_t)(addr))
#define ADDR16P(addr)                       ((uint16_t *)(uintptr_t)(addr))
#define ADDR32P(addr)                       ((uint32_t *)(uintptr_t)(addr))
#define ADDR64P(addr)                       ((uint64_t *)(uintptr_t)(addr))

/* Macros for Bit Operations */
#if __riscv_xlen == 32
#define BITMASK_MAX                         0xFFFFFFFFUL
#define BITOFS_MAX                          31
#else
#define BITMASK_MAX                         0xFFFFFFFFFFFFFFFFULL
#define BITOFS_MAX                          63
#endif

// BIT/BITS only support bit mask for __riscv_xlen
// For RISC-V 32 bit, it support mask 32 bit wide
// For RISC-V 64 bit, it support mask 64 bit wide
#define BIT(ofs)                            (0x1UL << (ofs))
#define BITS(start, end)                    ((BITMASK_MAX) << (start) & (BITMASK_MAX) >> (BITOFS_MAX - (end)))
#define GET_BIT(regval, bitofs)             (((regval) >> (bitofs)) & 0x1)
#define SET_BIT(regval, bitofs)             ((regval) |= BIT(bitofs))
#define CLR_BIT(regval, bitofs)             ((regval) &= (~BIT(bitofs)))
#define FLIP_BIT(regval, bitofs)            ((regval) ^= BIT(bitofs))
#define WRITE_BIT(regval, bitofs, val)      CLR_BIT(regval, bitofs); ((regval) |= ((val) << bitofs) & BIT(bitofs))
#define CHECK_BIT(regval, bitofs)           (!!((regval) & (0x1UL<<(bitofs))))
#define GET_BITS(regval, start, end)        (((regval) & BITS((start), (end))) >> (start))
#define SET_BITS(regval, start, end)        ((regval) |= BITS((start), (end)))
#define CLR_BITS(regval, start, end)        ((regval) &= (~BITS((start), (end))))
#define FLIP_BITS(regval, start, end)       ((regval) ^= BITS((start), (end)))
#define WRITE_BITS(regval, start, end, val) CLR_BITS(regval, start, end); ((regval) |= ((val) << start) & BITS((start), (end)))
#define CHECK_BITS_ALL(regval, start, end)  (!((~(regval)) & BITS((start), (end))))
#define CHECK_BITS_ANY(regval, start, end)  ((regval) & BITS((start), (end)))

#define BITMASK_SET(regval, mask)           ((regval) |= (mask))
#define BITMASK_CLR(regval, mask)           ((regval) &= (~(mask)))
#define BITMASK_FLIP(regval, mask)          ((regval) ^= (mask))
#define BITMASK_CHECK_ALL(regval, mask)     (!((~(regval)) & (mask)))
#define BITMASK_CHECK_ANY(regval, mask)     ((regval) & (mask))

/* =========================================================================================================================== */
/* ================                          Device Specific Peripheral Address Map                           ================ */
/* =========================================================================================================================== */

/* ToDo: add here your device peripherals base addresses
         following is an example for timer */
/** @addtogroup Device_Peripheral_peripheralAddr
  * @{
  */
/* ILM/DLM/FLASHXIP and Peripheral base address */
#ifndef EVALSOC_FLASH_XIP_BASE
#define QSPI_FLASH_BASE         (0x20000000UL)                /*!< (FLASH     ) Base Address */
#else
#define QSPI_FLASH_BASE         (EVALSOC_FLASH_XIP_BASE)      /*!< (FLASH     ) Base Address */
#endif

#ifndef CFG_ILM_BASE_ADDR
#define ONCHIP_ILM_BASE         (0x80000000UL)                /*!< (ILM       ) Base Address */
#else
#define ONCHIP_ILM_BASE         (CFG_ILM_BASE_ADDR)           /*!< (ILM       ) Base Address */
#endif

#ifndef CFG_DLM_BASE_ADDR
#define ONCHIP_DLM_BASE         (0x90000000UL)                /*!< (DLM       ) Base Address */
#else
#define ONCHIP_DLM_BASE         (CFG_DLM_BASE_ADDR)           /*!< (DLM       ) Base Address */
#endif

#ifndef EVALSOC_SYSMEM_BASE
#define ONCHIP_SRAM_BASE         (0xA0000000UL)                /*!< (SRAM       ) Base Address */
#else
#define ONCHIP_SRAM_BASE         (EVALSOC_SYSMEM_BASE)          /*!< (SRAM       ) Base Address */
#endif

#ifndef EVALSOC_PERIPS_BASE
#define EVALSOC_PERIPH_BASE     (0x10000000UL)                /*!< (Peripheral) Base Address */
#else
#define EVALSOC_PERIPH_BASE     (EVALSOC_PERIPS_BASE)         /*!< (Peripheral) Base Address */
#endif

/* Peripheral memory map */
#define UART0_BASE              (EVALSOC_PERIPH_BASE + 0x13000)          /*!< (UART0) Base Address */
#define QSPI0_BASE              (EVALSOC_PERIPH_BASE + 0x14000)          /*!< (QSPI0) Base Address */
#define UART1_BASE              (EVALSOC_PERIPH_BASE + 0x23000)          /*!< (UART1) Base Address */
#define QSPI1_BASE              (EVALSOC_PERIPH_BASE + 0x24000)          /*!< (QSPI1) Base Address */
#define QSPI2_BASE              (EVALSOC_PERIPH_BASE + 0x34000)          /*!< (QSPI2) Base Address */

/** @} */ /* End of group Device_Peripheral_peripheralAddr */


/* =========================================================================================================================== */
/* ================                                  Peripheral declaration                                   ================ */
/* =========================================================================================================================== */


/* ToDo: add here your device peripherals pointer definitions
         following is an example for timer */
/** @addtogroup Device_Peripheral_declaration
  * @{
  */
#define UART0                   ((UART_TypeDef *) UART0_BASE)
#define QSPI0                   ((QSPI_TypeDef *) QSPI0_BASE)
#define UART1                   ((UART_TypeDef *) UART1_BASE)
#define QSPI1                   ((QSPI_TypeDef *) QSPI1_BASE)
#define QSPI2                   ((QSPI_TypeDef *) QSPI2_BASE)

#define SPI0_REG(offset)        _REG32(QSPI0_BASE, offset)
#define SPI1_REG(offset)        _REG32(QSPI1_BASE, offset)
#define SPI2_REG(offset)        _REG32(QSPI2_BASE, offset)
#define UART0_REG(offset)       _REG32(UART0_BASE, offset)
#define UART1_REG(offset)       _REG32(UART1_BASE, offset)

// Misc

// Only used by Nuclei Internally, please dont use it
#define SIMULATION_EXIT(ret)    { __WMB(); UART0->RXFIFO = (ret);       \
                                    while (UART0->TXFIFO & (1<<31));    \
                                    UART0->TXFIFO = 4; }

extern uint32_t get_cpu_freq(void);
extern void delay_1ms(uint32_t count);

/** @} */ /* End of group evalsoc */

/** @} */ /* End of group Nuclei */

#ifdef __cplusplus
}
#endif

#endif  /* __EVALSOC_H__ */
