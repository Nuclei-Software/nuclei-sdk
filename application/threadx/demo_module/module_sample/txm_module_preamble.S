.macro ULONG  VAL
#if defined(__riscv_xlen) && (__riscv_xlen == 32)
    .word \VAL
#else
    .dword \VAL
#endif
.endm

    .text
    .align 4
    .section .preamble

    /* Define public symbols.  */
    .global __txm_module_preamble

    /* Define application-specific start/stop entry points for the module.  */
    .global demo_module_start

    /* Define common external refrences.  */
    .global _txm_module_thread_shell_entry
    .global _txm_module_callback_request_thread_entry

__txm_module_preamble:
    ULONG      0x4D4F4455                                       // Module ID
    ULONG      0x6                                              // Module Major Version
    ULONG      0x1                                              // Module Minor Version
    ULONG      __riscv_xlen                                     // Module Preamble Size in 32-bit words
    ULONG      0x12345678                                       // Module ID (application defined)
    ULONG      0x02000000                                       // Module Properties where:
                                                                //   Bits 31-24: Compiler ID
                                                                //           0 -> IAR (not applicable)
                                                                //           2 -> GNU
                                                                //   Bits 23-3: Reserved
                                                                //   Bit 2:  0 -> Disable shared/external memory access (not applicable)
                                                                //           1 -> Enable shared/external memory access (not applicable)
                                                                //   Bit 1:  0 -> No PMP protection
                                                                //           1 -> PMP protection (not applicable)
                                                                //   Bit 0:  0 -> Privileged mode execution
                                                                //           1 -> User mode execution (not applicable)
    ULONG      _txm_module_thread_shell_entry - . - 0           // Module Shell Entry Point
    ULONG      demo_module_start - . - 0                        // Module Start Thread Entry Point
    ULONG      0                                                // Module Stop Thread Entry Point
    ULONG      1                                                // Module Start/Stop Thread Priority
    ULONG      1024                                             // Module Start/Stop Thread Stack Size
    ULONG      _txm_module_callback_request_thread_entry - . - 0 // Module Callback Thread Entry
    ULONG      1                                                // Module Callback Thread Priority
    ULONG      1024                                             // Module Callback Thread Stack Size
    ULONG      __code_size__                                    // Module Code Size
    ULONG      __data_size__                                    // Module Data Size
    ULONG      0                                                // Reserved 0
    ULONG      0                                                // Reserved 1
    ULONG      0                                                // Reserved 2
    ULONG      0                                                // Reserved 3
    ULONG      0                                                // Reserved 4
    ULONG      0                                                // Reserved 5
    ULONG      0                                                // Reserved 6
    ULONG      0                                                // Reserved 7
    ULONG      0                                                // Reserved 8
    ULONG      0                                                // Reserved 9
    ULONG      0                                                // Reserved 10
    ULONG      0                                                // Reserved 11
    ULONG      0                                                // Reserved 12
    ULONG      0                                                // Reserved 13
    ULONG      0                                                // Reserved 14
    ULONG      0                                                // Reserved 15
