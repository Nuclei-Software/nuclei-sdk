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
#ifndef __RISCV_ENCODING_H__
#define __RISCV_ENCODING_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "riscv_bits.h"

/**
 * \defgroup NMSIS_Core_CSR_Encoding    Core CSR Encodings
 * \ingroup  NMSIS_Core
 * \brief    NMSIS Core CSR Encodings
 * \details
 *
 * The following macros are used for CSR encodings
 *   @{
 */
/* === Standard CSR bit mask === */
#define MSTATUS_UIE         0x00000001
#define MSTATUS_SIE         0x00000002
#define MSTATUS_HIE         0x00000004
#define MSTATUS_MIE         0x00000008
#define MSTATUS_UPIE        0x00000010
#define MSTATUS_SPIE        0x00000020
#define MSTATUS_UBE         0x00000040
#define MSTATUS_MPIE        0x00000080
#define MSTATUS_SPP         0x00000100
#define MSTATUS_VS          0x00000600
#define MSTATUS_MPP         0x00001800
#define MSTATUS_FS          0x00006000
#define MSTATUS_XS          0x00018000
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_SUM         0x00040000
#define MSTATUS_MXR         0x00080000
#define MSTATUS_TVM         0x00100000
#define MSTATUS_TW          0x00200000
#define MSTATUS_TSR         0x00400000
#define MSTATUS32_SD        0x80000000
#define MSTATUS_UXL         0x0000000300000000
#define MSTATUS_SXL         0x0000000C00000000
#define MSTATUS_SBE         0x0000001000000000
#define MSTATUS_MBE         0x0000002000000000
#define MSTATUS_GVA         0x0000004000000000
#define MSTATUS_MPV         0x0000008000000000
#define MSTATUS64_SD        0x8000000000000000

#define MSTATUS_FS_INITIAL  0x00002000
#define MSTATUS_FS_CLEAN    0x00004000
#define MSTATUS_FS_DIRTY    0x00006000

#define MSTATUS_VS_INITIAL  0x00000200
#define MSTATUS_VS_CLEAN    0x00000400
#define MSTATUS_VS_DIRTY    0x00000600

#define MSTATUSH_SBE        0x00000010
#define MSTATUSH_MBE        0x00000020
#define MSTATUSH_GVA        0x00000040
#define MSTATUSH_MPV        0x00000080

#define MIP_SSIP            (1 << IRQ_S_SOFT)
#define MIP_HSIP            (1 << IRQ_H_SOFT)
#define MIP_MSIP            (1 << IRQ_M_SOFT)
#define MIP_STIP            (1 << IRQ_S_TIMER)
#define MIP_HTIP            (1 << IRQ_H_TIMER)
#define MIP_MTIP            (1 << IRQ_M_TIMER)
#define MIP_SEIP            (1 << IRQ_S_EXT)
#define MIP_HEIP            (1 << IRQ_H_EXT)
#define MIP_MEIP            (1 << IRQ_M_EXT)

#define MIE_SSIE            MIP_SSIP
#define MIE_HSIE            MIP_HSIP
#define MIE_MSIE            MIP_MSIP
#define MIE_STIE            MIP_STIP
#define MIE_HTIE            MIP_HTIP
#define MIE_MTIE            MIP_MTIP
#define MIE_SEIE            MIP_SEIP
#define MIE_HEIE            MIP_HEIP
#define MIE_MEIE            MIP_MEIP

#define MCAUSE_INTR         (1ULL << (__riscv_xlen - 1))
#define MCAUSE_CAUSE        0x00000FFFUL

#define WFE_WFE                     (0x1)
#define TXEVT_TXEVT                 (0x1)
#define SLEEPVALUE_SLEEPVALUE       (0x1)

#define MCOUNTINHIBIT_IR            (1<<2)
#define MCOUNTINHIBIT_CY            (1<<0)

#define PRV_U 0
#define PRV_S 1
#define PRV_H 2
#define PRV_M 3

/**
 * \defgroup NMSIS_Core_CSR_Registers    Core CSR Registers
 * \ingroup  NMSIS_Core
 * \brief    NMSIS Core CSR Register Definitions
 * \details
 *
 * The following macros are used for CSR Register Defintions.
 *   @{
 */
/* === Standard RISC-V CSR Registers === */
#define CSR_JVT 0x17
#define CSR_MTVT 0x307
#define CSR_MSTATUS 0x300
#define CSR_MSTATUSH 0x310
#define CSR_MISA 0x301
#define CSR_MIE 0x304
#define CSR_MTVEC 0x305
#define CSR_MCOUNTEREN 0x306
#define CSR_MCOUNTINHIBIT 0x320
#define CSR_MSCRATCH 0x340 // Not present in N100
#define CSR_MEPC 0x341
#define CSR_MCAUSE 0x342
#define CSR_MTVAL 0x343
#define CSR_MBADADDR 0x343
#define CSR_MIP 0x344
#define CSR_TSELECT 0x7a0
#define CSR_TDATA1 0x7a1
#define CSR_TDATA2 0x7a2
#define CSR_TDATA3 0x7a3
#define CSR_TINFO 0x7a4
#define CSR_TCONTROL 0x7a5
#define CSR_DCSR 0x7b0
#define CSR_DPC 0x7b1
#define CSR_DSCRATCH0 0x7b2
#define CSR_DSCRATCH1 0x7b3
#define CSR_MCYCLE 0xb00
#define CSR_MINSTRET 0xb02
#define CSR_MVENDORID 0xf11
#define CSR_MARCHID 0xf12
#define CSR_MIMPID 0xf13
#define CSR_MHARTID 0xf14

/* === Nuclei custom CSR Registers === */
#define CSR_SLEEPVALUE          0x811
#define CSR_TXEVT               0x812
#define CSR_WFE                 0x810

/* === Nuclei N100 TIMER */
#define CSR_MSIP                0xBD8
#define CSR_MTIMECMP            0xBD9
#define CSR_MTIME               0xBDA
#define CSR_MSTOP               0xBDB

/* === Nuclei N100 IRQC */
#define CSR_IRQCIP              0xBD0
#define CSR_IRQCIE              0xBD1

/** @} */ /** End of Doxygen Group NMSIS_Core_CSR_Registers **/

/* Exception Code in MCAUSE CSR */
#define CAUSE_MISALIGNED_FETCH 0x0
#define CAUSE_FAULT_FETCH 0x1
#define CAUSE_ILLEGAL_INSTRUCTION 0x2
#define CAUSE_BREAKPOINT 0x3
#define CAUSE_MISALIGNED_LOAD 0x4
#define CAUSE_FAULT_LOAD 0x5
#define CAUSE_MISALIGNED_STORE 0x6
#define CAUSE_FAULT_STORE 0x7
#define CAUSE_USER_ECALL 0x8
#define CAUSE_SUPERVISOR_ECALL 0x9
#define CAUSE_HYPERVISOR_ECALL 0xa
#define CAUSE_MACHINE_ECALL 0xb
#define CAUSE_FETCH_PAGE_FAULT 0xc
#define CAUSE_LOAD_PAGE_FAULT 0xd
#define CAUSE_STORE_PAGE_FAULT 0xf

/* Delegatable Exception Code Mask in MCAUSE CSR*/
#define MISALIGNED_FETCH            (1 << CAUSE_MISALIGNED_FETCH)
#define FAULT_FETCH                 (1 << CAUSE_FAULT_FETCH)
#define ILLEGAL_INSTRUCTION         (1 << CAUSE_ILLEGAL_INSTRUCTION)
#define BREAKPOINT                  (1 << CAUSE_BREAKPOINT)
#define MISALIGNED_LOAD             (1 << CAUSE_MISALIGNED_LOAD)
#define FAULT_LOAD                  (1 << CAUSE_FAULT_LOAD)
#define MISALIGNED_STORE            (1 << CAUSE_MISALIGNED_STORE)
#define FAULT_STORE                 (1 << CAUSE_FAULT_STORE)
#define USER_ECALL                  (1 << CAUSE_USER_ECALL)
#define FETCH_PAGE_FAULT            (1 << CAUSE_FETCH_PAGE_FAULT)
#define LOAD_PAGE_FAULT             (1 << CAUSE_LOAD_PAGE_FAULT)
#define STORE_PAGE_FAULT            (1 << CAUSE_STORE_PAGE_FAULT)

/** @} */ /** End of Doxygen Group NMSIS_Core_CSR_Encoding **/

#ifdef __cplusplus
}
#endif
#endif /* __RISCV_ENCODING_H__ */
