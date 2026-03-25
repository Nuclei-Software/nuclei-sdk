/**
 * @file main.c
 * @brief Test RISC-V backtrace via illegal instruction exception.
 *
 * This test:
 * 1. Registers an illegal instruction exception handler.
 * 2. Triggers an illegal instruction (e.g., .word 0).
 * 3. In the handler, prints backtrace using riscv_backtrace_print().
 *
 * <h2>Compilation Requirement</h2>
 * <b>MUST</b> be compiled with `-fno-omit-frame-pointer` to ensure
 * frame pointers (s0) are pushed to the stack.
 *
 * <h2>Expected Output</h2>
 * @code
 * RISC-V Backtrace Test (FP=CFA Convention)
 * Stack Top (@_sp): 0x80020000
 * Calling chain: level1() -> level2() -> trigger_illegal()...
 * Inside level1, calling level2...
 * Inside level2, calling trigger_illegal...
 * About to execute illegal instruction in trigger_illegal()...
 *
 * === ILLEGAL INSTRUCTION EXCEPTION ===
 * mcause = 0000000000000002 (expected 2)
 *
 * --- Starting Backtrace ---
 * === Exception Frame Information ===
 * ra: 0x..., tp: 0x..., ...
 * cause: 0x..., epc: 0x..., msubm: 0x...
 *
 * === Stack Backtrace ===
 * Current SP: 0x..., Start FP: 0x...
 * Captured X frame(s):
 *   #0  RA=0x...  CFA=0x...
 *   #1  RA=0x...  CFA=0x...
 *
 * [Addr2Line Hint]
 * Run the following command on your host PC:
 *   riscv64-unknown-elf-addr2line -pfiaC -e <elf> 0x<epc> 0x<ra1> 0x<ra2> ...
 * @endcode
 */

#include <stdio.h>
#include <stdint.h>
#include "riscv_backtrace.h"
#include <nuclei_sdk_soc.h>

/* ================================================================== */
/* External Dependencies                                              */
/* ================================================================== */

/**
 * @brief Stack top address symbol from linker script.
 * @note This points to the highest address of the stack region.
 */
extern char _sp;

/** Stack size definition (adjust to match your linker script or startup code) */
#define TEST_STACK_SIZE 2048

/* ================================================================== */
/* Test Configuration                                                 */
/* ================================================================== */

/**
 * @brief Maximum number of times to re-trigger the illegal instruction.
 *
 * This allows testing nested exception handling. After reaching this limit,
 * the handler will print the backtrace and halt instead of re-triggering.
 *
 * @note Recommended value: 2-5 for testing nested exceptions.
 *       Higher values may cause stack overflow in exception handler.
 */
#ifndef ILLEGAL_RETRY_COUNT
#define ILLEGAL_RETRY_COUNT 2
#endif

/* ================================================================== */
/* Test Logic                                                         */
/* ================================================================== */

/** Global flag to track how many times the illegal exception has been triggered */
static volatile int illegal_triggered = 0;

/**
 * @brief Illegal instruction exception handler.
 *
 * @param mcause Exception cause (2 for illegal instruction)
 * @param sp     Stack pointer at exception entry (pointing to saved context)
 */
void illegal_handler(unsigned long mcause, unsigned long sp)
{
    unsigned long fp;
    asm volatile ("mv %0, s0" : "=r"(fp));
    if (illegal_triggered >= ILLEGAL_RETRY_COUNT) {
        /* Already handled; just halt to avoid loop */
        printf("=== Test completed. Halting. ===\n");
        while (1) __asm__ volatile ("wfi");
    }
    illegal_triggered += 1;

    printf("\n=== ILLEGAL INSTRUCTION EXCEPTION ===\n");
    printf("mcause = %lu (expected 2), triggered %d times\n", mcause & 0xFFF, illegal_triggered);

    if (illegal_triggered < ILLEGAL_RETRY_COUNT) {
        ((EXC_Frame_Type *)sp)->epc += 4;
        __asm__ volatile (
            "nop \n"
            ".word 0x00000000 \n"
            "nop \n"
        );
    } else {
        printf("\n--- Starting Backtrace ---\n");
        riscv_backtrace_print(fp, (unsigned long)&_sp, TEST_STACK_SIZE, sp, PRV_M);
        printf("--- End of Backtrace ---\n");
    }
}

/**
 * @brief Function that deliberately triggers an illegal instruction.
 *
 * We use 'noinline' to ensure this function has its own stack frame.
 */
__attribute__((noinline)) void trigger_illegal(void)
{
    printf("About to execute illegal instruction in trigger_illegal()...\n");

    /*
     * Insert an illegal instruction.
     * 0x00000000 is guaranteed to be illegal in RISC-V.
     */
    __asm__ volatile (
        "nop \n"
        ".word 0x00000000 \n"
        "nop \n"
    );
}

/**
 * @brief Level 2 caller in the call chain.
 */
__attribute__((noinline)) void level2(void)
{
    printf("Inside level2, calling trigger_illegal...\n");
    trigger_illegal();
}

/**
 * @brief Level 1 caller in the call chain.
 */
__attribute__((noinline)) void level1(void)
{
    printf("Inside level1, calling level2...\n");
    level2();
}

int main(void)
{
    CSR_MCFGINFO_Type mcfg_info;

    printf("RISC-V Backtrace Test (FP=CFA Convention)\n");

#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg_info.b.clic = 1;
#else
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
#endif

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT != 0)
    if (0 == mcfg_info.b.clic) {
        printf("You expect ECLIC present, but ECLIC is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_ECLIC=0 if ECLIC not present!\r\n");
        return 0;
    }
#endif

    printf("Stack Top (@_sp): %p\n", (void *)&_sp);

    /*
     * Register handler for Illegal Instruction exception.
     * Exception ID 2 = MCAUSE_ILLEGAL_INSTRUCTION
     */
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)illegal_handler);

    printf("Calling chain: level1() -> level2() -> trigger_illegal()...\n");
    level1();

    /* Should not reach here */
    printf("ERROR: Should not return from level1!\n");
    return -1;
}
