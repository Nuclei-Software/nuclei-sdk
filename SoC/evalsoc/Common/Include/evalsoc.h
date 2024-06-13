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

// If you have auto generated cpu configuration files
#ifdef HAS_AUTOGEN_CPUCFG
#include "autogen_nuclei_cpu.h"
#endif

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
    DOWNLOAD_MODE_MAX,
} DownloadMode_Type;

/* Simulation mode macros, internal usage */
#define SIMULATION_MODE_XLSPIKE   0     /*!< xlspike simulation mode */
#define SIMULATION_MODE_QEMU      1     /*!< qemu simulation mode */

/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */


typedef enum IRQn {
    /* =======================================  Nuclei N100 Core Internal Specific Interrupt Numbers  ======================================== */

    SysTimerSW_IRQn           =   0,              /*!<  CPU Timer SW interrupt for m-mode in IRQC */
    SysTimer_IRQn             =   1,              /*!<  CPU Timer Interrupt for m-mode in IRQC */

    /* ===========================================  evalsoc Specific Interrupt Numbers  ========================================= */
    /* ToDo: add here your device specific external interrupt numbers.2~31 is reserved number for user. Maxmum interrupt supported
             could get from irqcinfo.NUM_INTERRUPT. According the interrupt handlers defined in system_<Device>.c
             eg.: Interrupt for Timer#1       irqc_tim1_handler   ->   TIM1_IRQn */
    SOC_INT2_IRQn            = 2,                 /*!< Device Interrupt */
    SOC_INT3_IRQn            = 3,                 /*!< Device Interrupt */
    SOC_INT4_IRQn            = 4,                 /*!< Device Interrupt */
    SOC_INT5_IRQn            = 5,                 /*!< Device Interrupt */
    SOC_INT6_IRQn            = 6,                 /*!< Device Interrupt */
    SOC_INT7_IRQn            = 7,                 /*!< Device Interrupt */
    SOC_INT8_IRQn            = 8,                 /*!< Device Interrupt */
    SOC_INT9_IRQn            = 9,                 /*!< Device Interrupt */
    SOC_INT10_IRQn           = 10,                /*!< Device Interrupt */
    SOC_INT11_IRQn           = 11,                /*!< Device Interrupt */
    SOC_INT12_IRQn           = 12,                /*!< Device Interrupt */
    SOC_INT13_IRQn           = 13,                /*!< Device Interrupt */
    SOC_INT14_IRQn           = 14,                /*!< Device Interrupt */
    SOC_INT15_IRQn           = 15,                /*!< Device Interrupt */
    SOC_INT16_IRQn           = 16,                /*!< Device Interrupt */
    SOC_INT17_IRQn           = 17,                /*!< Device Interrupt */
    SOC_INT18_IRQn           = 18,                /*!< Device Interrupt */
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
    SOC_INT_MAX,
} IRQn_Type;

// TODO need to adapt for n100 soc
#define UART0_IRQn                                 SOC_INT2_IRQn
//#define UART1_IRQn                                 SOC_INT3_IRQn
#define QSPI0_IRQn                                 SOC_INT4_IRQn
#define QSPI1_IRQn                                 SOC_INT5_IRQn
#define QSPI2_IRQn                                 SOC_INT6_IRQn

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

/* ToDo: set the defines according your Device */
/* ToDo: define the correct core revision */
#define __NUCLEI_CPU_REV          0x010000              /*!< Nuclei CPU Core Revision, version X.Y.Z, this is for the CPU Core Version, you get from Nuclei, eg. N300 v3.10.1, it should be 0x030A01 */
#define __NUCLEI_CPU_SERIES       0x0100                /*!< Nuclei CPU Series, such as 100, eg. 100 will be 0x0100 */
#define __IRQC_PRESENT            1                     /*!< Set to 1 if IRQC is present */
#define __TIMER_PRESENT           1                     /*!< Set to 1 if Timer is present */

#define __Vendor_SysTickConfig    0                     /*!< Set to 1 if different SysTick Config is used */
#define __Vendor_EXCEPTION        0                     /*!< Set to 1 if vendor exception hander is present */

/** @} */ /* End of group Configuration_of_NMSIS */

#include <nmsis_core.h>                         /*!< Nuclei N/NX class processor and core peripherals */
/* ToDo: include your system_evalsoc.h file
         replace 'Device' with your device name */
#include "system_evalsoc.h"                    /*!< evalsoc System */


/* ========================================  Start of section using anonymous unions  ======================================== */

#define RTC_FREQ                    32768
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
// Interrupt Numbers

// Interrupt Handler Definitions
#define SOC_MTIMER_HANDLER          irqc_mtip_handler
#define SOC_SOFTINT_HANDLER         irqc_msip_handler

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

#ifndef EVALSOC_PERIPS_BASE
#define EVALSOC_PERIPH_BASE     (0x10000000UL)                /*!< (Peripheral) Base Address */
#else
#define EVALSOC_PERIPH_BASE     (EVALSOC_PERIPS_BASE)         /*!< (Peripheral) Base Address */
#endif

/* Peripheral memory map */
#define UART0_BASE              (EVALSOC_PERIPH_BASE + 0x13000)          /*!< (UART0) Base Address */
#define QSPI0_BASE              (EVALSOC_PERIPH_BASE + 0x14000)          /*!< (QSPI0) Base Address */
//#define UART1_BASE              (EVALSOC_PERIPH_BASE + 0x23000)          /*!< (UART1) Base Address */
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
