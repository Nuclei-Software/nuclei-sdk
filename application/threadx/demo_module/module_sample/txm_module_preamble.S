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
    .word      0x4D4F4455                                       // Module ID
    .word      0x6                                              // Module Major Version
    .word      0x1                                              // Module Minor Version
    .word      32                                               // Module Preamble Size in 32-bit words
    .word      0x12345678                                       // Module ID (application defined)
    .word      0x02000007                                       // Module Properties where:
                                                                //   Bits 31-24: Compiler ID
                                                                //           0 -> IAR
                                                                //           1 -> RVDS
                                                                //           2 -> GNU
                                                                //   Bits 23-3: Reserved
                                                                //   Bit 2:  0 -> Disable shared/external memory access
                                                                //           1 -> Enable shared/external memory access
                                                                //   Bit 1:  0 -> No MPU protection
                                                                //           1 -> MPU protection (must have user mode selected - bit 0 set)
                                                                //   Bit 0:  0 -> Privileged mode execution
                                                                //           1 -> User mode execution
    .word      _txm_module_thread_shell_entry - . - 0           // Module Shell Entry Point
    .word      demo_module_start - . - 0                        // Module Start Thread Entry Point
    .word      0                                                // Module Stop Thread Entry Point
    .word      1                                                // Module Start/Stop Thread Priority
    .word      1024                                             // Module Start/Stop Thread Stack Size
    .word      _txm_module_callback_request_thread_entry - . - 0 // Module Callback Thread Entry
    .word      1                                                // Module Callback Thread Priority
    .word      1024                                             // Module Callback Thread Stack Size
    .word      __code_size__                                    // Module Code Size
    .word      __data_size__                                    // Module Data Size
    .word      0                                                // Reserved 0
    .word      0                                                // Reserved 1
    .word      0                                                // Reserved 2
    .word      0                                                // Reserved 3
    .word      0                                                // Reserved 4
    .word      0                                                // Reserved 5
    .word      0                                                // Reserved 6
    .word      0                                                // Reserved 7
    .word      0                                                // Reserved 8
    .word      0                                                // Reserved 9
    .word      0                                                // Reserved 10
    .word      0                                                // Reserved 11
    .word      0                                                // Reserved 12
    .word      0                                                // Reserved 13
    .word      0                                                // Reserved 14
    .word      0                                                // Reserved 15
