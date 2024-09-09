/**************************************************
 *
 * System initialization.
 *
 * Copyright 2019-2022 IAR Systems AB.
 *
 **************************************************/

#include "iarMacros.m"
#include "iarCfi.m"
#include "cpufeature.h"


        MODULE  ?cstartup

        PUBWEAK  __iar_program_start
        PUBLIC   __iar_program_start_metal

        SECTION CSTACK:DATA:NOROOT(4)


        // --------------------------------------------------

        SECTION `.cstartup`:CODE:NOROOT(2)
        CfiCom  ra,1,0
        CfiCom  ra,1,1
        CfiCom  ra,1,2
        CfiCom  ra,1,3
        CfiCom  ra,1,4
        CfiCom  ra,1,6

        CfiBlk 0,__iar_program_start
        CALL_GRAPH_ROOT __iar_program_start, "Reset"
        CODE

__iar_program_start:
        REQUIRE ?cstart_init_sp
        cfi ?RET Undefined
        nop                    // Avoid an empty section
        CfiEnd  0

// ----------
//Init gp (required in by the linker config file, if applicable)
        SECTION `.cstartup`:CODE:NOROOT(1)
        CfiBlk 1,__iar_program_start
        CODE
        PUBLIC __iar_cstart_init_gp
__iar_cstart_init_gp:
        cfi ?RET Undefined
        EXTERN  __iar_static_base$$GPREL
        .option push
        .option norelax
        ;; lui  gp, %hi(__iar_static_base$$GPREL)
        ;; addi gp, gp, %lo(__iar_static_base$$GPREL)
        la      gp, __iar_static_base$$GPREL
        .option pop
        REQUIRE ?cstart_init_sp

        CfiEnd  1

// ----------
// Init sp, note that this MAY be gp relaxed! (since if gp relaxations are
// allowed, __iar_cstart_init_gp is already done
        SECTION `.cstartup`:CODE:NOROOT(1)
#ifdef __riscv_flen
        REQUIRE __iar_program_start_metal
#endif
        REQUIRE call_low_level_init
        CfiBlk 2,__iar_program_start
        CODE
?cstart_init_sp:
        cfi ?RET Undefined

#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
        /* Set correct sp for each cpu
        * each stack size is __STACK_SIZE
        * defined in linker script */
        EXTERN  __STACK_SIZE
        lui t0, %hi(__STACK_SIZE)
        addi t0, t0, %lo(__STACK_SIZE)
        la sp, SFE(CSTACK)
        csrr a0, mhartid
        andi a0, a0, 0xFF
        li a1, 0
_per_cpu_init_sp_cont:
        beq a0, a1, _per_cpu_init_sp_fin
        sub sp, sp, t0
        addi a1, a1, 1
        j _per_cpu_init_sp_cont
_per_cpu_init_sp_fin:
#else
        /* Set correct sp for current cpu */
        la sp, SFE(CSTACK)
#endif

        CfiEnd  2

        SECTION `.cstartup`:CODE:NOROOT(1)
        CfiBlk 6,__iar_program_start
        CODE
__iar_program_start_metal:
#ifdef __riscv_flen
        // Enable the floating-point unit by setting the "fs" field in
        // the "mstatus" register.

        lui     a0, %hi(1 << 13)
        csrs    mstatus, a0

        // Set rounding mode to "round to nearest" and clear
        // the floating-point exception flags.
        csrwi   fcsr, 0
#else
        nop      // avoid empty sections
#endif
        CfiEnd  6

        SECTION `.cstartup`:CODE:NOROOT(1)
        CfiBlk 4,__iar_program_start
        CODE
        EXTERN  __low_level_init
        EXTERN  __iar_data_init2
call_low_level_init:
        cfi ?RET Undefined
        CfiCall __low_level_init
        // __iar_data_init2 is done directly in __low_level_init in Nuclei SDK
        // main function will be called directly in __low_level_init
        call    __low_level_init

        EXTERN  exit

        CfiCall exit
        call    exit
?cstart_end:
        j       ?cstart_end

        CfiEnd  4

        /* This section is required by some devices to handle HW reset */
        SECTION `.alias.hwreset`:CODE:NOROOT(2)
        PUBLIC __alias_hw_reset
        PUBLIC _reset_vector
        EXTERN exc_entry
        /* TODO this is used to handle vector table + reset vector + exception entry */
_reset_vector:
        .option push
        .option norelax
        j __alias_hw_reset

#ifdef CFG_HAS_EXCP
        j exc_entry
#endif
        .option pop

__alias_hw_reset:
        csrci        mstatus, 0x08
        ;; lui     a0, %hi(__iar_program_start)
        ;; addi    a0, a0, %lo(__iar_program_start)
        la       a0, __iar_program_start
        jr       a0

        END
