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
#ifndef __CORE_FEATURE_CIDU_H__
#define __CORE_FEATURE_CIDU_H__
/*!
 * @file     core_feature_cidu.h
 * @brief    Cluster Interrupt Distribution Unit feature API header file for Nuclei N/NX Core
 */
/*
 * CIDU Feature Configuration Macro:
 * 1. __CIDU_PRESENT:  Define whether Cluster Interrupt Distribution Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 2. __CIDU_BASEADDR:  Define the base address of the CIDU.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"
#include <math.h>

#if defined(__CIDU_PRESENT) && (__CIDU_PRESENT == 1)

/* ##########################  CIDU functions  #################################### */
/**
 * \defgroup NMSIS_Core_CIDU         CIDU functions
 * \brief    Functions that manage external interrupts, inter core interrupts and semaphores .
 * @{
 * 
 * Nuclei provide Cluster Interrupt Distribution Unit (CIDU) for scenarios that a SMP system is designed for real
 * time application or both Linux and real time application, and Nuclei processor core can optionally support CIDU.
 * The CIDU is used to distribute external interrupts to the core’s ECLIC, also it provides Inter Core Interrupt (ICI)
 * and Semaphores Mechanism. Its features are as follows:
 * 
 * * Support up to 16 Cores in one cluster
 * * Support up to 4096 external interrupts sources
 * * Support up to 16 Inter Core Interrupts
 * * Support 32 Semaphores
 * 
 */

#ifndef __CIDU_BASEADDR
/* Base address of CIDU(__CIDU_BASEADDR) should be defined in <Device.h> */
#error "__CIDU_BASEADDR is not defined, please check!"
#endif

#define CIDU_BASE                                __CIDU_BASEADDR
#define CORE_RECEIVE_INTERRUPT_ENABLE(core_id)   (1 << core_id)   /*!< indicates the core can receive corresponding interrupt */

/**
 * \brief CIDU Register offset
 */
#define CORE_INT_STATUS_OFS                      0x0
#define SEMAPHORE_OFS                            0x80
#define ICI_SHADOW_OFS                           0x3FFC
#define INT_INDICATOR_OFS                        0x4000
#define INT_MASK_OFS                             0x8000
#define CORE_NUM_OFS                             0xC084
#define INT_NUM_OFS                              0xC090

/**
 * \brief CIDU Register address
 */
#define CORE_INT_STATUS_ADDR(n)                  (unsigned long)((CIDU_BASE) + (CORE_INT_STATUS_OFS) + ((n) << 2))
#define SEMAPHORE_ADDR(n)                        (unsigned long)((CIDU_BASE) + (SEMAPHORE_OFS) + ((n) << 2))
#define ICI_SHADOW_ADDR                          (unsigned long)((CIDU_BASE) + (ICI_SHADOW_OFS))
#define INT_INDICATOR_ADDR(n)                    (unsigned long)((CIDU_BASE) + (INT_INDICATOR_OFS) + ((n) << 2))
#define INT_MASK_ADDR(n)                         (unsigned long)((CIDU_BASE) + (INT_MASK_OFS) + ((n) << 2))
#define CORE_NUM_ADDR                            (unsigned long)((CIDU_BASE) + (CORE_NUM_OFS))
#define INT_NUM_ADDR                             (unsigned long)((CIDU_BASE) + (INT_NUM_OFS))

/* SEND_CORE_ID position in ICI_SHADOW_REG register*/
#define ICI_SHADOW_SEND_CORE_ID_POS              16

/**
 * \brief  Get core number in the cluster
 * \details
 * Indicate the static configuration core num in the cluster.
 * \return core number configured
 * \remarks
 * - In a Nulcei multi-core system, each core has an identifiable serial number, the serial number starts from 0 and is
 *   continuous, also the number is static and has a direct mapping with the CSR mhartID.
 * - CORE_NUM register is read only.
*/
__STATIC_FORCEINLINE uint32_t CIDU_GetCoreNum(void)
{
    uint32_t val;
    uint32_t* addr = (uint32_t*)CORE_NUM_ADDR;

    val = __LW(addr);
    return val;
}

/**
 * \brief  Get external interrupt number
 * \details
 * Indicate the static configuration external interrupt number
 * \return interrupt number configured
 * \remarks
 * - INT_NUM register is read only.
*/
__STATIC_FORCEINLINE uint32_t CIDU_GetIntNum(void)
{
    uint32_t val;
    uint32_t* addr = (uint32_t*)INT_NUM_ADDR;

    val = __LW(addr);
    return val;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_CIDU */

/**
 * \defgroup NMSIS_Core_Distribute_Interrupt      External Interrupt Distribution Functions
 * \ingroup  NMSIS_Core_CIDU
 * \brief    Functions that distribute external interrupts to cores.
 * @{
 */

/**
 * \brief  Broadcast external interrupt to cores
 * \details
 * This function broadcasts external interrupt_n to some/all target cores
 * \param [in]    interrupt_n    external interrupt id
 * \param [in]    to_cores       target cores which can receive interrupt, use bitwise inclusive or
 *                               of \ref CORE_RECEIVE_INTERRUPT_ENABLE(core_id)
 * \remarks
 * - External IRQn ID(interrupt_n) is from the hard-wired persperctive,
 *   which has an offset mapped to the ECLIC IRQn, see Interrupt Number Definition in <Device.h>
 * - By default on reset, only core 0 can receive interrupt_n
*/
__STATIC_FORCEINLINE void CIDU_SetBroadcastMode(uint32_t interrupt_n, uint32_t to_cores)
{
    uint32_t* addr = (uint32_t*)INT_INDICATOR_ADDR(interrupt_n);

    __SW(addr, (uint32_t)to_cores);
}

/**
 * \brief  get broadcast mode status
 * \details
 * Just query the INTn_INDICATOR register value
 * \param [in]    interrupt_n    external interrupt id
 * \return INTn_INDICATOR register value
 * \remarks
 * - External IRQn ID(interrupt_n) is from the hard-wired persperctive,
 *   which has an offset mapped to the ECLIC IRQn, see Interrupt Number Definition in <Device.h>
 * - By default on reset, only core 0 can receive interrupt_n
*/
__STATIC_FORCEINLINE uint32_t CIDU_GetBroadcastModeStaus(uint32_t interrupt_n)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)INT_INDICATOR_ADDR(interrupt_n);

    val = __LW(addr);
    return val;
}

/**
 * \brief  Let the first coming core to first claim the interrupt
 * \details
 * In external interrupt broadcast mode, make the first coming core to claim this interrupt and then handle it.
 * When it claims the interrupt, it will mask the other core's corresponding bit in the INTn_INDICATOR register.
 * \param [in]    interrupt_n    external interrupt id
 * \param [in]    core_id        core id that receive the interrupt
 * \return -1 if it fails to claim the interrupt, else it can continue to handle the interrupt
 * \remarks
 * - External IRQn ID(interrupt_n) is from the hard-wired persperctive,
 *   which has an offset mapped to the ECLIC IRQn, see Interrupt Number Definition in <Device.h>.
 * - If it fails to claim the interrupt, it should quit the interrupt n's handler of all cores
 * - When a core claims the interrupt successfully and has handled it, it must call \ref CIDU_ResetFirstClaimMode to reset the claim.
 * \sa
 * - \ref CIDU_SetBroadcastMode
 * - \ref CIDU_ResetFirstClaimMode
*/
__STATIC_FORCEINLINE int32_t CIDU_SetFirstClaimMode(uint32_t interrupt_n, uint32_t core_id)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)INT_MASK_ADDR(interrupt_n);
    uint32_t mask = 1 << core_id;

    __SW(addr, mask);
    val = __LW(addr);
    if (mask == val) {
        return 0;
    }
    return -1;
}

/**
 * \brief  Reset the claim mode mask
 * \details
 * Reset the claim mode mask by Writing the reset value (all 1) to it
 * \param [in] interrupt_n    external interrupt id
 * \remarks
 * - External IRQn ID(interrupt_n) is from the hard-wired persperctive,
 *   which has an offset mapped to the ECLIC IRQn, see Interrupt Number Definition in <Device.h>
 * - When a core claims the interrupt successfully and handle it, it must call \ref CIDU_ResetFirstClaimMode to reset the claim
 * \sa
 * - \ref CIDU_SetFirstClaimMode
*/
__STATIC_FORCEINLINE void CIDU_ResetFirstClaimMode(uint32_t interrupt_n)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)INT_MASK_ADDR(interrupt_n);

    /* clear by writing all 1 */
    __SW(addr, 0xFFFFFFFF);
}

/**
 * \brief  Get the claim mask status
 * \details
 * Get the claim mode INTn_MASK register value, each bit indicates it should mask the INTn_INDICATOR's bit or not,
 * 1 means no, 0 means yes.
 * \param [in]    interrupt_n    external interrupt id
 * \return claim mode register INTn_MASK value
 * \remarks
 * - External IRQn ID(interrupt_n) is from the hard-wired persperctive,
 *   which has an offset mapped to the ECLIC IRQn, see Interrupt Number Definition in <Device.h>
 * \sa
 * - \ref CIDU_ResetFirstClaimMode
 * - \ref CIDU_SetFirstClaimMode
*/
__STATIC_FORCEINLINE int32_t CIDU_GetClaimStatus(uint32_t interrupt_n)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)INT_MASK_ADDR(interrupt_n);

    val = __LW(addr);
    return val;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_Distribute_Interrupt */

/**
 * \defgroup NMSIS_Core_ICI      Inter Core Interrupt Functions
 * \ingroup  NMSIS_Core_CIDU
 * \brief    Functions that implement Inter Core Interrupt mechanism.
 * @{
 * Inter Core Interrupt (ICI) means that one core can send interrupt to another core in a multi-core cluster. CIDU ICI belongs
 * to Internal Interrupt.
 * 
 * * CIDU ICI Interrupt ID is fixed to 16.
 */

/**
 * \brief  Send interrupt to another core in a multi-core cluster
 * \details
 * When called by core send_core_id, CIDU will trigger ICI to core recv_core_id automatically.
 * and core recv_core_id could query \ref CIDU_GetCoreIntSenderId to know the sender.
 * \param [in]    send_core_id    the core id which want to send the inter core interrupt
 * \param [in]    recv_core_id    the core id which will receive the inter core interrupt
 * \remarks
 * - The core recv_core_id need to call CIDU_ClearCoreIntStatus to clear the corresponding bit/bits
 *   of its own COREn_INT_STATUS.
 * - It supports that multiple cores call \ref CIDU_SetInterCoreIntShadow simultaneously.
 * \sa
 * - \ref CIDU_GetCoreIntSenderId
 * - \ref CIDU_ClearCoreIntStatus
*/
__STATIC_FORCEINLINE void CIDU_SetInterCoreIntShadow(uint32_t send_core_id, uint32_t recv_core_id)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)ICI_SHADOW_ADDR;

    val = (uint32_t)(send_core_id << ICI_SHADOW_SEND_CORE_ID_POS) | (uint32_t)(recv_core_id);
    __SW(addr, (uint32_t)val);
}

/**
 * \brief  get Coren's Interrupt status register value
 * \details
 * Just query COREn_INT_STATUS value
 * \param [in]    recv_core_id    the core id which receives the inter core interrupt
 * \return register COREn_INT_STATUS value
 * \sa
 * - \ref CIDU_GetCoreIntSenderId
*/
__STATIC_FORCEINLINE uint32_t CIDU_GetCoreIntStatus(uint32_t recv_core_id)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)CORE_INT_STATUS_ADDR(recv_core_id);

    val = __LW(addr);
    return val;
}

/**
 * \brief  Core recv_core_id query out who sends inter core interrupt to itself
 * \details
 * When core recv_core_id receives the ICI and enter the ICI Interrupt Service Routine (ISR), it
 * can query out who triggered the ICI.
 * \param [in]    recv_core_id    the core id which receives the inter core interrupt
 * \return sender core id that triggers the ICI
 * \sa
 * - \ref CIDU_GetCoreIntSenderId
 * - \ref CIDU_ClearCoreIntStatus
*/
__STATIC_FORCEINLINE uint32_t CIDU_GetCoreIntSenderId(uint32_t recv_core_id)
{
    uint32_t val = 0;
    uint32_t core_id = 0;
    val = CIDU_GetCoreIntStatus(recv_core_id);
    while (val != 0) {
        val >>= 1;
        core_id++;
    }
    return (core_id - 1);
}

/**
 * \brief  Clear the corresponding bit/bits of its own COREn_INT_STATUS
 * \details
 * Core recv_core_id write 1 to clear the bit send_core_id of the core recv_core_id's COREn_INT_STATUS.
 * \param [in]    send_core_id    the core id which want to send the inter core interrupt
 * \param [in]    recv_core_id    the core id which will receive the inter core interrupt
 * \remarks
 * - If the ICI ISR has finished the job of send_core_id_n's ICI, then clear bit send_core_id_n;
 *   if it has finished send_core_id_n and send_core_id_m's, then should clear both the bits and etc.
*/
__STATIC_FORCEINLINE void CIDU_ClearCoreIntStatus(uint32_t send_core_id, uint32_t recv_core_id)
{
    uint32_t val = 0;
    uint32_t* addr = (uint32_t*)CORE_INT_STATUS_ADDR(recv_core_id);

    val |= (uint32_t)(1 << send_core_id);
    __SW(addr, val);
}
/** @} */ /* End of Doxygen Group NMSIS_Core_ICI */

/**
 * \defgroup NMSIS_Core_Semaphore      Semaphore Functions
 * \ingroup  NMSIS_Core_CIDU
 * \brief    Functions that configure and use semaphores
 * @{
 * Semaphore is very useful for multi-core cluster without SMP enable.
 * 
 * * All Cores in the cluster agree on using SEMAPHORE_n register to protect a critical resource (an UART device for example).
 * * If Core n wants to access the critical resource, it should try to own the SEMPAPHORE_n register, or else it can’t access the critical resource.
 * * When the Core n owns the register SEMPAPHORE_n and finishes the job related the critical resource, then it should
 *   release the register by write all 1 to it.
 */

/**
 * \brief  Get SEMAPHOREn's value
 * \details
 * Just query the semaphore n's value
 * \param [in]    semph_n     the semaphore id used to protect a critical resource
 * \return register SEMAPHOREn_STATUS value
*/
__STATIC_FORCEINLINE int32_t CIDU_GetSemaphoreStatus(uint32_t semph_n)
{
    uint32_t val;
    uint32_t* addr = (uint32_t*)SEMAPHORE_ADDR(semph_n);

    val = __LW(addr);
    return val;
}

/**
 * \brief  check SEMAPHOREn's own status
 * \details
 * Query that whether SEMAPHOREn has been owned by one core successfully.
 * \param [in]    semph_n    the semaphore id used to protect a critical resource
 * \param [in]    core_id    the core id that wants to access the critical resource
 * \return 0 if core_id has acquired this semaphore successfully, or else -1 if failed
 * \remarks
 * - When the core n owns the register SEMPAPHORE_n and finishes the job related the critical resource,
 *   it should call \ref CIDU_ReleaseSemaphore to release it.
 * \sa
 * - \ref CIDU_GetSemaphoreStatus
 * - \ref CIDU_ReleaseSemaphore
*/
__STATIC_FORCEINLINE int32_t CIDU_CheckSemaphoreOwnStatus(uint32_t semph_n, uint32_t core_id)
{
    uint32_t val;
    val = CIDU_GetSemaphoreStatus(semph_n);
    if (core_id != val) {
        return -1;
    }
    return 0;
}

/**
 * \brief  Acquire the SEMAPHOREn
 * \details
 * Acuqire the SEMAPHOREn, and check the own status
 * \param [in]    semph_n    the semaphore id used to protect a critical resource
 * \param [in]    core_id    the core id that wants to access the critical resource
 * \return 0 if core_id has acquired this semaphore successfully, or else -1 if failed
 * \remarks
 * - When the core n owns the register SEMPAPHORE_n and finishes the job related the critical resource,
 *   it should call \ref CIDU_ReleaseSemaphore to release it.
 * \sa
 * - \ref CIDU_CheckSemaphoreOwnStatus
 * - \ref CIDU_ReleaseSemaphore
*/
__STATIC_FORCEINLINE int32_t CIDU_AcquireSemaphore(uint32_t semph_n, uint32_t core_id)
{
    int32_t semaphore_status = -1;
    uint32_t* addr = (uint32_t*)SEMAPHORE_ADDR(semph_n);

    __SW(addr, core_id);
    semaphore_status = CIDU_CheckSemaphoreOwnStatus(semph_n, core_id);
    return semaphore_status;
}

/**
 * \brief  Keep acquiring the SEMAPHOREn until it has own this semaphore successfully
 * \details
 * Query that whether SEMAPHOREn has been owned by one core successfully, if not, keep trying.
 * \param [in]    semph_n    the semaphore id used to protect a critical resource
 * \param [in]    core_id    the core id that wants to access the critical resource
 * \remarks
 * - Core n will block here acquiring, so take care that core should release the semaphore when related job done.
 * \sa
 * - \ref CIDU_AcquireSemaphore
 * - \ref CIDU_ReleaseSemaphore
*/
__STATIC_FORCEINLINE void CIDU_AcquireSemaphore_Block(uint32_t semph_n, uint32_t core_id)
{
    int32_t semaphore_status = -1;

    while(0 != semaphore_status) {
        semaphore_status = CIDU_AcquireSemaphore(semph_n, core_id);
    }
}

/**
 * \brief  Release the SEMAPHOREn
 * \details
 * Release the SEMAPHOREn by write all 1 to SEMAPHOREn register.
 * \param [in]    semph_n    the semaphore id used to protect a critical resource
 * \remarks
 * - When the core finishes the job related to the critical resource, it should release the corresponding semaphore.
 * \sa
 * - \ref CIDU_AcquireSemaphore_Block
*/
__STATIC_FORCEINLINE void CIDU_ReleaseSemaphore(uint32_t semph_n)
{
    uint32_t val;
    uint32_t* addr = (uint32_t*)SEMAPHORE_ADDR(semph_n);

    /* Release by writing all 1 */
    __SW(addr, 0xFFFFFFFF);
    val = __LW(addr);
}
/** @} */ /* End of Doxygen Group NMSIS_Core_Semaphore */
#endif /* defined(__CIDU_PRESENT) && (__CIDU_PRESENT == 1) */

#ifdef __cplusplus
}
#endif 
#endif /* __CORE_FEATURE_CIDU_H__ */