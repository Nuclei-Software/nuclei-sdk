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

#ifndef __CORE_FEATURE_BASE__
#define __CORE_FEATURE_BASE__
/*!
 * @file     core_feature_base.h
 * @brief    Base core feature API for Nuclei N100 Core
 */
/*
 * Core Base Feature Configuration Macro:
 * 1. __HARTID_OFFSET:  Optional, define this macro when your cpu system first hart hartid and hart index is different.
 *    eg. If your cpu system, first hart hartid is 2, hart index is 0, then set this macro to 2
 *
 */
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#include "nmsis_compiler.h"

/**
 * \defgroup NMSIS_Core_Registers     Register Define and Type Definitions
 * \brief   Type definitions and defines for core registers.
 *
 * @{
 */
#ifndef __RISCV_XLEN
  /** \brief Refer to the width of an integer register in bits(either 32 or 64) */
  #ifndef __riscv_xlen
    #define __RISCV_XLEN    32
  #else
    #define __RISCV_XLEN    __riscv_xlen
  #endif
#endif /* __RISCV_XLEN */

/** \brief Type of Control and Status Register(CSR), depends on the XLEN defined in RISC-V */
typedef unsigned long rv_csr_t;

/** @} */ /* End of Doxygen Group NMSIS_Core_Registers */
/**
 * \defgroup NMSIS_Core_Base_Registers     Base Register Define and Type Definitions
 * \ingroup NMSIS_Core_Registers
 * \brief   Type definitions and defines for base core registers.
 *
 * @{
 */
/**
 * \brief  Union type to access MISA CSR register.
 */
typedef union {
    struct {
        rv_csr_t a:1;                           /*!< bit:     0  Atomic extension */
        rv_csr_t b:1;                           /*!< bit:     1  Tentatively reserved for Bit-Manipulation extension */
        rv_csr_t c:1;                           /*!< bit:     2  Compressed extension */
        rv_csr_t d:1;                           /*!< bit:     3  Double-precision floating-point extension */
        rv_csr_t e:1;                           /*!< bit:     4  RV32E base ISA */
        rv_csr_t f:1;                           /*!< bit:     5  Single-precision floating-point extension */
        rv_csr_t g:1;                           /*!< bit:     6  Additional standard extensions present */
        rv_csr_t h:1;                           /*!< bit:     7  Hypervisor extension */
        rv_csr_t i:1;                           /*!< bit:     8  RV32I/64I/128I base ISA */
        rv_csr_t j:1;                           /*!< bit:     9  Tentatively reserved for Dynamically Translated Languages extension */
        rv_csr_t _reserved1:1;                  /*!< bit:     10 Reserved  */
        rv_csr_t l:1;                           /*!< bit:     11 Tentatively reserved for Decimal Floating-Point extension  */
        rv_csr_t m:1;                           /*!< bit:     12 Integer Multiply/Divide extension */
        rv_csr_t n:1;                           /*!< bit:     13 User-level interrupts supported  */
        rv_csr_t _reserved2:1;                  /*!< bit:     14 Reserved  */
        rv_csr_t p:1;                           /*!< bit:     15 Tentatively reserved for Packed-SIMD extension  */
        rv_csr_t q:1;                           /*!< bit:     16 Quad-precision floating-point extension  */
        rv_csr_t _resreved3:1;                  /*!< bit:     17 Reserved  */
        rv_csr_t s:1;                           /*!< bit:     18 Supervisor mode implemented  */
        rv_csr_t t:1;                           /*!< bit:     19 Tentatively reserved for Transactional Memory extension  */
        rv_csr_t u:1;                           /*!< bit:     20 User mode implemented  */
        rv_csr_t v:1;                           /*!< bit:     21 Tentatively reserved for Vector extension  */
        rv_csr_t _reserved4:1;                  /*!< bit:     22 Reserved  */
        rv_csr_t x:1;                           /*!< bit:     23 Non-standard extensions present  */
#if defined(__RISCV_XLEN) && __RISCV_XLEN == 64
        rv_csr_t _reserved5:38;                 /*!< bit:     24..61 Reserved  */
        rv_csr_t mxl:2;                         /*!< bit:     62..63 Machine XLEN  */
#else
        rv_csr_t _reserved5:6;                  /*!< bit:     24..29 Reserved  */
        rv_csr_t mxl:2;                         /*!< bit:     30..31 Machine XLEN  */
#endif
    } b;                                        /*!< Structure used for bit  access */
    rv_csr_t d;                                 /*!< Type      used for csr data access */
} CSR_MISA_Type;

/**
 * \brief  Union type to access MSTATUSH CSR register.
 */
typedef union {
    struct {
        rv_csr_t _reserved0:5;                  /*!< bit:     0..4  Reserved */
        rv_csr_t mbe:1;                         /*!< bit:     5  Machine Mode Big endian control */
        rv_csr_t _reserved1:26;                 /*!< bit:     6..31  Reserved */
    } b;                                        /*!< Structure used for bit  access */
    rv_csr_t d;                                 /*!< Type      used for csr data access */
} CSR_MSTATUSH_Type;

/**
 * \brief  Union type to access MSTATUS CSR register.
 */
typedef union {
    struct {
        rv_csr_t _reserved0:3;                  /*!< bit:     0..2  Reserved */
        rv_csr_t mie:1;                         /*!< bit:     3  Machine mode interrupt enable */
        rv_csr_t _reserved1:3;                  /*!< bit:     4..6  Reserved */
        rv_csr_t mpie:1;                        /*!< bit:     7  Machine mode previous privilege mode interrupt enable */
        rv_csr_t _reserved2:24;                 /*!< bit:     8..31 Reserved */
    } b;                                        /*!< Structure used for bit  access */
    rv_csr_t d;                                 /*!< Type      used for csr data access */
} CSR_MSTATUS_Type;

/**
 * \brief  Union type to access MTVEC CSR register.
 */
typedef union {
    struct {
        rv_csr_t _reserved0:2;                  /*!< bit:     0..1   Reserved */
#if defined(__RISCV_XLEN) && __RISCV_XLEN == 64
        rv_csr_t addr:62;                       /*!< bit:     2..63  mtvec address */
#else
        rv_csr_t addr:30;                       /*!< bit:     2..31  mtvec address */
#endif
    } b;                                        /*!< Structure used for bit  access */
    rv_csr_t d;                                 /*!< Type      used for csr data access */
} CSR_MTVEC_Type;

/**
 * \brief  Union type to access MCAUSE CSR register.
 */
typedef union {
    struct {
        rv_csr_t exccode:12;                    /*!< bit:     11..0  exception or interrupt code */
        rv_csr_t _reserved0:19;                  /*!< bit:     12..30  Reserved */
#if defined(__RISCV_XLEN) && __RISCV_XLEN == 64
        rv_csr_t _reserved2:32;                 /*!< bit:     31..62  Reserved */
        rv_csr_t interrupt:1;                   /*!< bit:     63  trap type. 0 means exception and 1 means interrupt */
#else
        rv_csr_t interrupt:1;                   /*!< bit:     31  trap type. 0 means exception and 1 means interrupt */
#endif
    } b;                                        /*!< Structure used for bit  access */
    rv_csr_t d;                                 /*!< Type      used for csr data access */
} CSR_MCAUSE_Type;

/** @} */ /* End of Doxygen Group NMSIS_Core_Base_Registers */

/* ###########################  Core Function Access  ########################### */
/**
 * \defgroup NMSIS_Core_CSR_Register_Access    Core CSR Register Access
 * \ingroup  NMSIS_Core
 * \brief    Functions to access the Core CSR Registers
 * \details
 *
 * The following functions or macros provide access to Core CSR registers.
 * - \ref NMSIS_Core_CSR_Encoding
 * - \ref NMSIS_Core_CSR_Registers
 *   @{
 */


#ifndef __ASSEMBLY__

#ifndef __ICCRISCV__

/**
 * \brief CSR operation Macro for csrrw instruction.
 * \details
 * Read the content of csr register to __v,
 * then write content of val into csr register, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   value to store into the CSR register
 * \return the CSR register value before written
 */
#define __RV_CSR_SWAP(csr, val)                                 \
    ({                                                          \
        rv_csr_t __v = (unsigned long)(val);                    \
        __ASM volatile("csrrw %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrr instruction.
 * \details
 * Read the content of csr register to __v and return it
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \return the CSR register value
 */
#define __RV_CSR_READ(csr)                                      \
    ({                                                          \
        rv_csr_t __v;                                           \
        __ASM volatile("csrr %0, " STRINGIFY(csr)               \
                     : "=r"(__v)                                \
                     :                                          \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrw instruction.
 * \details
 * Write the content of val to csr register
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   value to store into the CSR register
 */
#define __RV_CSR_WRITE(csr, val)                                \
    ({                                                          \
        rv_csr_t __v = (rv_csr_t)(val);                         \
        __ASM volatile("csrw " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })

/**
 * \brief CSR operation Macro for csrrs instruction.
 * \details
 * Read the content of csr register to __v,
 * then set csr register to be __v | val, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrrs instruction
 * \return the CSR register value before written
 */
#define __RV_CSR_READ_SET(csr, val)                             \
    ({                                                          \
        rv_csr_t __v = (rv_csr_t)(val);                         \
        __ASM volatile("csrrs %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrs instruction.
 * \details
 * Set csr register to be csr_content | val
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrs instruction
 */
#define __RV_CSR_SET(csr, val)                                  \
    ({                                                          \
        rv_csr_t __v = (rv_csr_t)(val);                         \
        __ASM volatile("csrs " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })

/**
 * \brief CSR operation Macro for csrrc instruction.
 * \details
 * Read the content of csr register to __v,
 * then set csr register to be __v & ~val, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrrc instruction
 * \return the CSR register value before written
 */
#define __RV_CSR_READ_CLEAR(csr, val)                           \
    ({                                                          \
        rv_csr_t __v = (rv_csr_t)(val);                         \
        __ASM volatile("csrrc %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrc instruction.
 * \details
 * Set csr register to be csr_content & ~val
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrc instruction
 */
#define __RV_CSR_CLEAR(csr, val)                                \
    ({                                                          \
        rv_csr_t __v = (rv_csr_t)(val);                         \
        __ASM volatile("csrc " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })
#else

#include <intrinsics.h>

#define __RV_CSR_SWAP         __write_csr
#define __RV_CSR_READ         __read_csr
#define __RV_CSR_WRITE        __write_csr
#define __RV_CSR_READ_SET     __set_bits_csr
#define __RV_CSR_SET          __set_bits_csr
#define __RV_CSR_READ_CLEAR   __clear_bits_csr
#define __RV_CSR_CLEAR        __clear_bits_csr

#endif /* __ICCRISCV__ */

#endif /* __ASSEMBLY__ */

/**
 * \brief   Enable IRQ Interrupts
 * \details Enables IRQ interrupts by setting the MIE-bit in the MSTATUS Register.
 * \remarks
 *          Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_MIE);
}

/**
 * \brief   Disable IRQ Interrupts
 * \details Disables IRQ interrupts by clearing the MIE-bit in the MSTATUS Register.
 * \remarks
 *          Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

/**
 * \brief   Read whole 32 bits value of mcycle counter
 * \details This function will read the whole 32 bits of MCYCLE register for N100
 * \return  The whole 32 bits value of MCYCLE
 * \remarks
 * - mcycle in N100 is only 24 bits long, so upper bits are all zeros
 * - when performance monitor unit not present, this function just return 0
 */
__STATIC_FORCEINLINE uint32_t __get_rv_cycle(void)
{
#if defined(__PMON_PRESENT) && (__PMON_PRESENT == 1)
    volatile uint32_t full;

    full = __RV_CSR_READ(CSR_MCYCLE);
    return full;
#else
    return 0;
#endif
}

/**
 * \brief   Set whole 32 bits value of mcycle counter
 * \details This function will set the whole 32 bits of MCYCLE register
 * \remarks
 * - mcycle in N100 is only 24 bits long, so upper bits are all zeros
 * - when performance monitor unit not present, this function do nothing
 */
__STATIC_FORCEINLINE void __set_rv_cycle(uint32_t cycle)
{
#if defined(__PMON_PRESENT) && (__PMON_PRESENT == 1)
    __RV_CSR_WRITE(CSR_MCYCLE, (uint32_t)(cycle));
#endif
}

/**
 * \brief   Read whole 32 bits value of machine instruction-retired counter
 * \details This function will read the whole 32 bits of MINSTRET register
 * \return  The whole 32 bits value of MINSTRET
 * \remarks
 * - minstret in N100 is only 24 bits long, so upper bits are all zeros
 * - when performance monitor unit not present, this function just return 0
 */
__STATIC_FORCEINLINE uint32_t __get_rv_instret(void)
{
#if defined(__PMON_PRESENT) && (__PMON_PRESENT == 1)
    volatile uint32_t full;

    full = __RV_CSR_READ(CSR_MINSTRET);
    return full;
#else
    return 0;
#endif
}

/**
 * \brief   Set whole 32 bits value of minstret counter
 * \details This function will set the whole 32 bits of MINSTRET register
 * \remarks
 * - minstret in N100 is only 24 bits long, so upper bits are all zeros
 * - when performance monitor unit not present, this function do nothing
 */
__STATIC_FORCEINLINE void __set_rv_instret(uint32_t instret)
{
#if defined(__PMON_PRESENT) && (__PMON_PRESENT == 1)
    __RV_CSR_WRITE(CSR_MINSTRET, (uint32_t)(instret));
#endif
}

/**
 * \brief   Read whole 32 bits value of real-time clock
 * \details This function will read the whole 32 bits of MTIME register
 * \return  The whole 32 bits value of MTIME CSR
 * \remarks
 * - time in N100 is only 24 bits long, so upper bits are all zeros
 * - when timer unit not present, this function just return 0
 * \attention this is using N100 MTIME CSR
 */
__STATIC_FORCEINLINE uint32_t __get_rv_time(void)
{
#if defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1)
    volatile uint32_t full;

    full = __RV_CSR_READ(CSR_MTIME);
    return full;
#else
    return 0;
#endif
}

/**
 * \brief   Get hart index of current cluster
 * \details This function will get hart index of current cluster in a multiple cluster system,
 * hart index is hartid - hartid offset, for example if your hartid is 1, and offset is 1, then
 * hart index is 0
 * \return  The hart index of current cluster
 * \attention function is allowed in machine mode only
 */
__STATIC_FORCEINLINE unsigned long __get_hart_index(void)
{
    unsigned long id;
#ifdef __HARTID_OFFSET
    id = __RV_CSR_READ(CSR_MHARTID) - __HARTID_OFFSET;
#else
    id = __RV_CSR_READ(CSR_MHARTID);
#endif
    return id;
}

/**
 * \brief   Get hart id of current cluster
 * \details This function will get hart id of current cluster in a multiple cluster system
 * \return  The hart id of current cluster
 * \remarks it will return full hartid not part of it for reference subsystem design,
 * if your reference subsystem design has hartid offset, please define __HARTID_OFFSET in
 * <Device>.h
 * \attention function is allowed in machine mode only
 */
__STATIC_FORCEINLINE unsigned long __get_hart_id(void)
{
    unsigned long id;
    id = __RV_CSR_READ(CSR_MHARTID);
    return id;
}

/** @} */ /* End of Doxygen Group NMSIS_Core_CSR_Register_Access */

/* ###########################  CPU Intrinsic Functions ########################### */
/**
 * \defgroup NMSIS_Core_CPU_Intrinsic   Intrinsic Functions for CPU Intructions
 * \ingroup  NMSIS_Core
 * \brief    Functions that generate RISC-V CPU instructions.
 * \details
 *
 * The following functions generate specified RISC-V instructions that cannot be directly accessed by compiler.
 *   @{
 */

/**
 * \brief   NOP Instruction
 * \details
 * No Operation does nothing.
 * This instruction can be used for code alignment purposes.
 */
__STATIC_FORCEINLINE void __NOP(void)
{
    __ASM volatile("nop");
}

/**
 * \brief   Wait For Interrupt
 * \details
 * Wait For Interrupt is is executed using CSR_WFE.WFE=0 and WFI instruction.
 * It will suspends execution until interrupt, NMI or Debug happened.
 * When Core is waked up by interrupt, if
 * 1. mstatus.MIE == 1(interrupt enabled), Core will enter ISR code
 * 2. mstatus.MIE == 0(interrupt disabled), Core will resume previous execution
 */
__STATIC_FORCEINLINE void __WFI(void)
{
    __RV_CSR_CLEAR(CSR_WFE, WFE_WFE);
    __ASM volatile("wfi");
}

/**
 * \brief   Wait For Event
 * \details
 * Wait For Event is executed using CSR_WFE.WFE=1 and WFI instruction.
 * It will suspends execution until event, NMI or Debug happened.
 * When Core is waked up, Core will resume previous execution
 */
__STATIC_FORCEINLINE void __WFE(void)
{
    __RV_CSR_SET(CSR_WFE, WFE_WFE);
    __ASM volatile("wfi");
    __RV_CSR_CLEAR(CSR_WFE, WFE_WFE);
}

/**
 * \brief   Breakpoint Instruction
 * \details
 * Causes the processor to enter Debug state.
 * Debug tools can use this to investigate system state
 * when the instruction at a particular address is reached.
 */
__STATIC_FORCEINLINE void __EBREAK(void)
{
    __ASM volatile("ebreak");
}

/**
 * \brief   Environment Call Instruction
 * \details
 * The ECALL instruction is used to make a service request to
 * the execution environment.
 */
__STATIC_FORCEINLINE void __ECALL(void)
{
    __ASM volatile("ecall");
}

/**
 * \brief WFI Sleep Mode enumeration
 */
typedef enum WFI_SleepMode {
    WFI_SHALLOW_SLEEP = 0,      /*!< Shallow sleep mode, the core_clk will poweroff */
    WFI_DEEP_SLEEP = 1          /*!< Deep sleep mode, the core_clk and core_ano_clk will poweroff */
} WFI_SleepMode_Type;

/**
 * \brief   Set Sleep mode of WFI
 * \details
 * Set the SLEEPVALUE CSR register to control the
 * WFI Sleep mode.
 * \param[in] mode      The sleep mode to be set
 */
__STATIC_FORCEINLINE void __set_wfi_sleepmode(WFI_SleepMode_Type mode)
{
    __RV_CSR_WRITE(CSR_SLEEPVALUE, mode);
}

/**
 * \brief   Send TX Event
 * \details
 * Set the CSR TXEVT to control send a TX Event.
 * The Core will output signal tx_evt as output event signal.
 */
__STATIC_FORCEINLINE void __TXEVT(void)
{
    __RV_CSR_SET(CSR_TXEVT, 0x1);
}

/**
 * \brief   Enable MCYCLE counter
 * \details
 * Clear the CY bit of MCOUNTINHIBIT to 0 to enable MCYCLE Counter
 */
__STATIC_FORCEINLINE void __enable_mcycle_counter(void)
{
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, MCOUNTINHIBIT_CY);
}

/**
 * \brief   Disable MCYCLE counter
 * \details
 * Set the CY bit of MCOUNTINHIBIT to 1 to disable MCYCLE Counter
 */
__STATIC_FORCEINLINE void __disable_mcycle_counter(void)
{
    __RV_CSR_SET(CSR_MCOUNTINHIBIT, MCOUNTINHIBIT_CY);
}

/**
 * \brief   Enable MINSTRET counter
 * \details
 * Clear the IR bit of MCOUNTINHIBIT to 0 to enable MINSTRET Counter
 */
__STATIC_FORCEINLINE void __enable_minstret_counter(void)
{
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, MCOUNTINHIBIT_IR);
}

/**
 * \brief   Disable MINSTRET counter
 * \details
 * Set the IR bit of MCOUNTINHIBIT to 1 to disable MINSTRET Counter
 */
__STATIC_FORCEINLINE void __disable_minstret_counter(void)
{
    __RV_CSR_SET(CSR_MCOUNTINHIBIT, MCOUNTINHIBIT_IR);
}

/**
 * \brief   Enable all MCYCLE & MINSTRET & MHPMCOUNTER counter
 * \details
 * Clear all to zero to enable all counters,
 * such as cycle, instret, high performance monitor counters
 */
__STATIC_FORCEINLINE void __enable_all_counter(void)
{
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, 0xFFFFFFFF);
}

/**
 * \brief   Disable all MCYCLE & MINSTRET & MHPMCOUNTER counter
 * \details
 * Set all to one to disable all counters,
 * such as cycle, instret, high performance monitor counters
 */
__STATIC_FORCEINLINE void __disable_all_counter(void)
{
    __RV_CSR_SET(CSR_MCOUNTINHIBIT, 0xFFFFFFFF);
}


/**
 * \brief Execute fence instruction, p -> pred, s -> succ
 * \details
 * the FENCE instruction ensures that all memory accesses from instructions preceding
 * the fence in program order (the `predecessor set`) appear earlier in the global memory order than
 * memory accesses from instructions appearing after the fence in program order (the `successor set`).
 * For details, please refer to The RISC-V Instruction Set Manual
 * \param p     predecessor set, such as iorw, rw, r, w
 * \param s     successor set, such as iorw, rw, r, w
 **/
#define __FENCE(p, s) __ASM volatile ("fence " #p "," #s : : : "memory")

/**
 * \brief   Fence.i Instruction
 * \details
 * The FENCE.I instruction is used to synchronize the instruction
 * and data streams.
 */
__STATIC_FORCEINLINE void __FENCE_I(void)
{
    __ASM volatile("fence.i");
}

/** \brief Read & Write Memory barrier */
#define __RWMB()        __FENCE(iorw,iorw)

/** \brief Read Memory barrier */
#define __RMB()         __FENCE(ir,ir)

/** \brief Write Memory barrier */
#define __WMB()         __FENCE(ow,ow)

/** \brief SMP Read & Write Memory barrier */
#define __SMP_RWMB()    __FENCE(rw,rw)

/** \brief SMP Read Memory barrier */
#define __SMP_RMB()     __FENCE(r,r)

/** \brief SMP Write Memory barrier */
#define __SMP_WMB()     __FENCE(w,w)

/** \brief CPU relax for busy loop */
#define __CPU_RELAX()   __ASM volatile ("" : : : "memory")


/* ===== Load/Store Operations ===== */
/**
 * \brief  Load 8bit value from address (8 bit)
 * \details Load 8 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint8_t at (*addr)
 */
__STATIC_FORCEINLINE uint8_t __LB(volatile void *addr)
{
    uint8_t result;

    __ASM volatile ("lb %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

/**
 * \brief  Load 16bit value from address (16 bit)
 * \details Load 16 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint16_t at (*addr)
 */
__STATIC_FORCEINLINE uint16_t __LH(volatile void *addr)
{
    uint16_t result;

    __ASM volatile ("lh %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

/**
 * \brief  Load 32bit value from address (32 bit)
 * \details Load 32 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint32_t at (*addr)
 */
__STATIC_FORCEINLINE uint32_t __LW(volatile void *addr)
{
    uint32_t result;

    __ASM volatile ("lw %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

#if __RISCV_XLEN != 32
/**
 * \brief  Load 64bit value from address (64 bit)
 * \details Load 64 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint64_t at (*addr)
 * \remarks RV64 only macro
 */
__STATIC_FORCEINLINE uint64_t __LD(volatile void *addr)
{
    uint64_t result;
    __ASM volatile ("ld %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}
#endif

/**
 * \brief  Write 8bit value to address (8 bit)
 * \details Write 8 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SB(volatile void *addr, uint8_t val)
{
    __ASM volatile ("sb %0, 0(%1)" : : "r" (val), "r" (addr));
}

/**
 * \brief  Write 16bit value to address (16 bit)
 * \details Write 16 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SH(volatile void *addr, uint16_t val)
{
    __ASM volatile ("sh %0, 0(%1)" : : "r" (val), "r" (addr));
}

/**
 * \brief  Write 32bit value to address (32 bit)
 * \details Write 32 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SW(volatile void *addr, uint32_t val)
{
    __ASM volatile ("sw %0, 0(%1)" : : "r" (val), "r" (addr));
}

#if __RISCV_XLEN != 32
/**
 * \brief  Write 64bit value to address (64 bit)
 * \details Write 64 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SD(volatile void *addr, uint64_t val)
{
    __ASM volatile ("sd %0, 0(%1)" : : "r" (val), "r" (addr));
}
#endif

/** @} */ /* End of Doxygen Group NMSIS_Core_CPU_Intrinsic */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_BASE__ */
