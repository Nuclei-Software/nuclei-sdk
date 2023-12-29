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
/******************************************************************************
 * \file     startup_gd32vw55x.S
 * \brief    NMSIS Nuclei N/NX Class Core based Core Device Startup File for
 *  Device gd32vw55x
 * \version  V1.00
 * \date     25 Dec 2023
 *
 *
 ******************************************************************************/

#include "riscv_encoding.h"

    .section .init

    .weak     eclic_msip_handler                # Software interrupt
    .weak     eclic_mtip_handler                # CPU Timer interrupt
    .weak     WWDGT_IRQHandler                  # Window Watchdog Timer interrupt
    .weak     LVD_IRQHandler                    # LVD through EXTI Line detect
    .weak     TAMPER_STAMP_IRQHandler           # Tamper and TimeStamp through EXTI Line detect
    .weak     RTC_WKUP_IRQHandler               # RTC Wakeup through EXTI Line
    .weak     FMC_IRQHandler                    # FMC
    .weak     RCU_IRQHandler                    # RCU
    .weak     EXTI0_IRQHandler                  # EXTI Line 0
    .weak     EXTI1_IRQHandler                  # EXTI Line 1
    .weak     EXTI2_IRQHandler                  # EXTI Line 2
    .weak     EXTI3_IRQHandler                  # EXTI Line 3
    .weak     EXTI4_IRQHandler                  # EXTI Line 4
    .weak     DMA_Channel0_IRQHandler           # DMA Channel0
    .weak     DMA_Channel1_IRQHandler           # DMA Channel1
    .weak     DMA_Channel2_IRQHandler           # DMA Channel2
    .weak     DMA_Channel3_IRQHandler           # DMA Channel3
    .weak     DMA_Channel4_IRQHandler           # DMA Channel4
    .weak     DMA_Channel5_IRQHandler           # DMA Channel5
    .weak     DMA_Channel6_IRQHandler           # DMA Channel6
    .weak     DMA_Channel7_IRQHandler           # DMA Channel7
    .weak     ADC_IRQHandler                    # ADC
    .weak     EXTI5_9_IRQHandler                # EXTI5 to EXTI9
    .weak     TIMER0_BRK_IRQHandler             # TIMER0 Break
    .weak     TIMER0_UP_IRQHandler              # TIMER0 Update
    .weak     TIMER0_CMT_IRQHandler             # TIMER0 Commutation
    .weak     TIMER0_Channel_IRQHandler         # TIMER0 Channel Capture Compare
    .weak     TIMER1_IRQHandler                 # TIMER1
    .weak     TIMER2_IRQHandler                 # TIMER2
    .weak     I2C0_EV_IRQHandler                # I2C0 Event
    .weak     I2C0_ER_IRQHandler                # I2C0 Error
    .weak     I2C1_EV_IRQHandler                # I2C1 Event
    .weak     I2C1_ER_IRQHandler                # I2C1 Error
    .weak     SPI_IRQHandler                    # SPI
    .weak     USART0_IRQHandler                 # USART0
    .weak     UART1_IRQHandler                  # UART1
    .weak     UART2_IRQHandler                  # UART2
    .weak     EXTI10_15_IRQHandler              # EXTI10 to EXTI15
    .weak     RTC_Alarm_IRQHandler              # RTC Alarm
    .weak     TIMER15_IRQHandler                # TIMER15
    .weak     TIMER16_IRQHandler                # TIMER16
    .weak     I2C0_WKUP_IRQHandler              # I2C0 Wakeup
    .weak     USART0_WKUP_IRQHandler            # USART0 Wakeup
    .weak     TIMER5_IRQHandler                 # TIMER5
    .weak     WIFI_TRIGGER_IRQHandler           # WIFI Protocol trigger interrupt
    .weak     WIFI_MAC_IRQHandler               # WIFI MAC interrupt
    .weak     WIFI_TX_IRQHandler                # WIFI Tx interrupt
    .weak     WIFI_RX_IRQHandler                # WIFI Rx interrupt
    .weak     LA_IRQHandler                     # LA interrupt
    .weak     WIFI_WKUP_IRQHandler              # WIFI wakeup interrupt
    .weak     BLE_WKUP_IRQHandler               # BLE wakeup interrupt
    .weak     PLATFORM_WAKE_IRQHandler          # Platform wake interrupt
    .weak     ISO_BT_STAMP0_IRQHandler          # ISO Bluetooth TimeStamp interrupt0
    .weak     ISO_BT_STAMP1_IRQHandler          # ISO Bluetooth TimeStamp interrupt1
    .weak     ISO_BT_STAMP2_IRQHandler          # ISO Bluetooth TimeStamp interrupt2
    .weak     ISO_BT_STAMP3_IRQHandler          # ISO Bluetooth TimeStamp interrupt3
    .weak     ISO_BT_STAMP4_IRQHandler          # ISO Bluetooth TimeStamp interrupt4
    .weak     ISO_BT_STAMP5_IRQHandler          # ISO Bluetooth TimeStamp interrupt5
    .weak     ISO_BT_STAMP6_IRQHandler          # ISO Bluetooth TimeStamp interrupt6
    .weak     ISO_BT_STAMP7_IRQHandler          # ISO Bluetooth TimeStamp interrupt7
    .weak     PMU_IRQHandler                    # PMU
    .weak     CAU_IRQHandler                    # CAU
    .weak     HAU_TRNG_IRQHandler               # HAU and TRNG
    .weak     WIFI_INT_IRQHandler               # WIFI interrupt
    .weak     WIFI_SW_TRIG_IRQHandler           # SW triggered interrupt
    .weak     WIFI_FINE_TIMER_TARGET_IRQHandler # Fine Timer Target interrupt
    .weak     WIFI_STAMP_TARGET1_IRQHandler     # Timestamp Target 1 interrupt
    .weak     WIFI_STAMP_TARGET2_IRQHandler     # Timestamp Target 2 interrupt
    .weak     WIFI_STAMP_TARGET3_IRQHandler     # Timestamp Target 3 interrupt
    .weak     WIFI_ENCRYPTION_ENGINE_IRQHandler # Encryption engine Interrupt
    .weak     WIFI_SLEEP_MODE_IRQHandler        # Sleep Mode Interrupt
    .weak     WIFI_HALF_SLOT_IRQHandler         # Half Slot interrupt
    .weak     WIFI_FIFO_ACTIVITY_IRQHandler     # FIFO Activity interrupt
    .weak     WIFI_ERROR_IRQHandler             # Error Interrupt
    .weak     WIFI_FREQ_SELECT_IRQHandler       # Frequency Selection Interrupt
    .weak     EFUSE_IRQHandler                  # EFUSE
    .weak     QSPI_IRQHandler                   # QUADSPI
    .weak     PKCAU_IRQHandler                  # PKCAU

vector_base:
    j _start                                    /* 0: Reserved, Jump to _start when reset for vector table not remapped cases.*/
    .align LOG_REGBYTES                         /*    Need to align 4 byte for RV32, 8 Byte for RV64 */
    .word     0
    .word     0
    .word     eclic_msip_handler                # Software interrupt
    .word     0                                 # Reserved
    .word     0
    .word     0
    .word     eclic_mtip_handler                # CPU Timer interrupt
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     WWDGT_IRQHandler                  # Window Watchdog Timer
    .word     LVD_IRQHandler                    # LVD through EXTI Line detect
    .word     TAMPER_STAMP_IRQHandler           # Tamper and TimeStamp through EXTI Line detect
    .word     RTC_WKUP_IRQHandler               # RTC Wakeup through EXTI Line
    .word     FMC_IRQHandler                    # FMC
    .word     RCU_IRQHandler                    # RCU
    .word     EXTI0_IRQHandler                  # EXTI Line 0
    .word     EXTI1_IRQHandler                  # EXTI Line 1
    .word     EXTI2_IRQHandler                  # EXTI Line 2
    .word     EXTI3_IRQHandler                  # EXTI Line 3
    .word     EXTI4_IRQHandler                  # EXTI Line 4
    .word     DMA_Channel0_IRQHandler           # DMA Channel0
    .word     DMA_Channel1_IRQHandler           # DMA Channel1
    .word     DMA_Channel2_IRQHandler           # DMA Channel2
    .word     DMA_Channel3_IRQHandler           # DMA Channel3
    .word     DMA_Channel4_IRQHandler           # DMA Channel4
    .word     DMA_Channel5_IRQHandler           # DMA Channel5
    .word     DMA_Channel6_IRQHandler           # DMA Channel6
    .word     DMA_Channel7_IRQHandler           # DMA Channel7
    .word     ADC_IRQHandler                    # ADC
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     EXTI5_9_IRQHandler                # EXTI5 to EXTI9
    .word     TIMER0_BRK_IRQHandler             # TIMER0 Break
    .word     TIMER0_UP_IRQHandler              # TIMER0 Update
    .word     TIMER0_CMT_IRQHandler             # TIMER0 Commutation
    .word     TIMER0_Channel_IRQHandler         # TIMER0 Channel Capture Compare
    .word     TIMER1_IRQHandler                 # TIMER1
    .word     TIMER2_IRQHandler                 # TIMER2
    .word     0                                 # Reserved
    .word     I2C0_EV_IRQHandler                # I2C0 Event
    .word     I2C0_ER_IRQHandler                # I2C0 Error
    .word     I2C1_EV_IRQHandler                # I2C1 Event
    .word     I2C1_ER_IRQHandler                # I2C1 Error
    .word     SPI_IRQHandler                    # SPI
    .word     0                                 # Reserved
    .word     USART0_IRQHandler                 # USART0
    .word     UART1_IRQHandler                  # UART1
    .word     UART2_IRQHandler                  # UART2
    .word     EXTI10_15_IRQHandler              # EXTI10 to EXTI15
    .word     RTC_Alarm_IRQHandler              # RTC Alarm
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     TIMER15_IRQHandler                # TIMER15
    .word     TIMER16_IRQHandler                # TIMER16
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     I2C0_WKUP_IRQHandler              # I2C0 Wakeup
    .word     USART0_WKUP_IRQHandler            # USART0 Wakeup
    .word     0                                 # Reserved
    .word     TIMER5_IRQHandler                 # TIMER5
    .word     WIFI_TRIGGER_IRQHandler           # WIFI Protocol trigger
    .word     WIFI_MAC_IRQHandler               # WIFI MAC
    .word     WIFI_TX_IRQHandler                # WIFI Tx
    .word     WIFI_RX_IRQHandler                # WIFI Rx
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     LA_IRQHandler                     # LA interrupt
    .word     WIFI_WKUP_IRQHandler              # WIFI wakeup interrupt
    .word     BLE_WKUP_IRQHandler               # BLE wakeup interrupt
    .word     PLATFORM_WAKE_IRQHandler          # Platform wake
    .word     ISO_BT_STAMP0_IRQHandler          # ISO Bluetooth TimeStamp interrupt0
    .word     ISO_BT_STAMP1_IRQHandler          # ISO Bluetooth TimeStamp interrupt1
    .word     ISO_BT_STAMP2_IRQHandler          # ISO Bluetooth TimeStamp interrupt2
    .word     ISO_BT_STAMP3_IRQHandler          # ISO Bluetooth TimeStamp interrupt3
    .word     ISO_BT_STAMP4_IRQHandler          # ISO Bluetooth TimeStamp interrupt4
    .word     ISO_BT_STAMP5_IRQHandler          # ISO Bluetooth TimeStamp interrupt5
    .word     ISO_BT_STAMP6_IRQHandler          # ISO Bluetooth TimeStamp interrupt6
    .word     ISO_BT_STAMP7_IRQHandler          # ISO Bluetooth TimeStamp interrupt7
    .word     PMU_IRQHandler                    # PMU
    .word     0                                 # Reserved
    .word     0                                 # Reserved
    .word     CAU_IRQHandler                    # CAU
    .word     HAU_TRNG_IRQHandler               # HAU and TRNG
    .word     0                                 # Reserved
    .word     WIFI_INT_IRQHandler               # WIFI interrupt
    .word     WIFI_SW_TRIG_IRQHandler           # SW triggered interrupt
    .word     WIFI_FINE_TIMER_TARGET_IRQHandler # Fine Timer Target interrupt
    .word     WIFI_STAMP_TARGET1_IRQHandler     # Timestamp Target 1 interrupt
    .word     WIFI_STAMP_TARGET2_IRQHandler     # Timestamp Target 2 interrupt
    .word     WIFI_STAMP_TARGET3_IRQHandler     # Timestamp Target 3 interrupt
    .word     WIFI_ENCRYPTION_ENGINE_IRQHandler # Encryption engine Interrupt
    .word     WIFI_SLEEP_MODE_IRQHandler        # Sleep Mode interrupt
    .word     WIFI_HALF_SLOT_IRQHandler         # Half Slot interrupt
    .word     WIFI_FIFO_ACTIVITY_IRQHandler     # FIFO Activity interrupt
    .word     WIFI_ERROR_IRQHandler             # Error interrupt
    .word     WIFI_FREQ_SELECT_IRQHandler       # Frequency Selection iterrupt
    .word     EFUSE_IRQHandler                  # EFUSE
    .word     QSPI_IRQHandler                   # QUADSPI
    .word     PKCAU_IRQHandler                  # PKCAU

    .globl _start
    .type _start,@function

_start:
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE
    /* Initialize GP and Stack Pointer SP */
    .option push
    .option norelax
    la gp, __global_pointer$
    la tp, __tls_base
    .option pop
    la sp, _sp

    /*
     * Set the the NMI base mnvec to share
     * with mtvec by setting CSR_MMISC_CTL
     * bit 9 NMI_CAUSE_FFF to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /*
     * Intialize ECLIC vector interrupt
     * base address mtvt to vector_base
     */
    la t0, vector_base
    csrw CSR_MTVT, t0

    /*
     * Set ECLIC non-vector entry to be controlled
     * by mtvt2 CSR register.
     * Intialize ECLIC non-vector interrupt
     * base address mtvt2 to irq_entry.
     */
    la t0, irq_entry
    csrw CSR_MTVT2, t0
    csrs CSR_MTVT2, 0x1

    /*
     * Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0

    /* Set the interrupt processing mode to ECLIC mode */
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

    /* ===== Startup Stage 2 ===== */

#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU, and set state to initial */
    li t0, MSTATUS_FS
    csrc mstatus, t0
    li t0, MSTATUS_FS_INITIAL
    csrs mstatus, t0
#endif

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

    /* ===== Startup Stage 3 ===== */
    /* Load data section */
    la a0, _data_lma
    la a1, _data
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Clear bss section */
    la a0, __bss_start
    la a1, _end
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     */
    call SystemInit

    /* Call global constructors */
    la a0, __libc_fini_array
    call atexit
    /* Call C/C++ constructor start up code */
    call __libc_init_array

    /* do pre-init steps before main */
    call _premain_init

    /*
     * When all initialization steps done
     * set exception entry to correct exception
     * entry and jump to main.
     * And set the interrupt processing mode to
     * ECLIC mode
     */
    la t0, exc_entry
    csrw CSR_MTVEC, t0
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

    /* ===== Call Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

#ifdef RTOS_RTTHREAD
    // Call entry function when using RT-Thread
    call entry
#else
    call main
#endif
    /* do post-main steps after main */
    call _postmain_fini

1:
    j 1b

/* Early boot exception entry before main */
.align 6
.global early_exc_entry
early_exc_entry:
    wfi
    j early_exc_entry
