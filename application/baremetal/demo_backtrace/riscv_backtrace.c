/**
 * @file riscv_backtrace.c
 * @brief Implementation of RISC-V frame-pointer-based stack unwinding.
 *
 * This file implements the core logic for walking the RISC-V frame pointer
 * chain. It adheres to the convention where the frame pointer (x8/s0) points
 * to the Canonical Frame Address (CFA).
 *
 * <h2>Frame Layout Assumptions</h2>
 * According to the provided convention:
 * - The frame pointer (fp) points to the CFA (stack pointer on entry).
 * - The Frame Record (RA and Prev FP) is stored *just prior* to the CFA.
 * - XLEN denotes the register width in bits (converted to bytes below).
 *
 * Layout (XLEN = 8 bytes for RV64):
 * @code
 * High Address
 *   +------------------+
 *   | Caller's stack   |
 *   +------------------+  <- CFA (Current fp points here)
 *   | saved ra         |  <- CFA - XLEN_BYTES
 *   | saved fp (prev)  |  <- CFA - 2 * XLEN_BYTES
 *   +------------------+
 *   | Local variables  |
 *   +------------------+  <- sp (current stack pointer)
 * Low Address
 * @endcode
 *
 * The chain is traversed by:
 * 1. Reading ra from (fp - XLEN_BYTES).
 * 2. Reading the previous fp from (fp - 2 * XLEN_BYTES).
 * 3. Repeating until the previous fp is zero.
 */

#include "riscv_backtrace.h"
#include <stdint.h>
#include <nuclei_sdk_soc.h>

/* ================================================================== */
/* Constants and Helpers                                              */
/* ================================================================== */

/** Size of a register in bytes (4 for RV32, 8 for RV64) */
#define XLEN_BYTES (sizeof(unsigned long))

/**
 * @brief Validates a candidate frame pointer for safe dereferencing.
 *
 * A frame pointer is considered valid if:
 * - It is non-zero.
 * - It is aligned to XLEN bytes.
 * - It lies within the current stack bounds.
 *
 * @param[in] fp           Candidate frame pointer (CFA).
 * @param[in] stack_bottom Lowest valid stack address (inclusive).
 * @param[in] stack_top    Highest valid stack address (exclusive).
 * @return 1 if valid, 0 otherwise.
 */
static inline int is_valid_fp(unsigned long fp, unsigned long stack_bottom, unsigned long stack_top)
{
    if (fp == 0) {
        return 0;
    }
    /* Check XLEN alignment */
    if ((fp & (XLEN_BYTES - 1)) != 0) {
        return 0;
    }
    /*
     * Check bounds: fp points to the top of the frame.
     * We must ensure fp is within bounds.
     * Note: We access memory *below* fp, so strict checking might
     * verify (fp - 2*XLEN) >= stack_bottom, but checking fp is usually sufficient.
     */
    if (fp < stack_bottom || fp >= stack_top) {
        return 0;
    }
    return 1;
}

/* ================================================================== */
/* Public Functions                                                   */
/* ================================================================== */

int riscv_backtrace_frames(unsigned long fp, backtrace_frame_t *frames, int max_depth,
                           unsigned long stack_top, size_t stack_size)
{
    /* Input validation */
    if (!frames || max_depth <= 0 || stack_size == 0) {
        return 0;
    }
    if (max_depth > BACKTRACE_MAX_DEPTH) {
        max_depth = BACKTRACE_MAX_DEPTH;
    }

    const unsigned long stack_bottom = stack_top - stack_size;
    int count = 0;
    unsigned long curr_fp = fp;

    /* Walk the frame pointer chain */
    for (int i = 0; i < max_depth; i++) {
        /* Validate current frame pointer */
        if (!is_valid_fp(curr_fp, stack_bottom, stack_top)) {
            break;
        }

        /*
         * Rule: "This puts the return address at fp - XLEN/8"
         * (XLEN/8 is XLEN_BYTES)
         */
        const unsigned long ra = *(const unsigned long *)(curr_fp - XLEN_BYTES);

        /*
         * Store frame info.
         * CFA is the value of fp (stack pointer on entry).
         */
        frames[count].ra = ra;
        frames[count].cfa = curr_fp;

        if (ra == 0) {
            /*
             * If RA is 0, we've likely hit the start of the thread/context.
             * Include this frame and break.
             */
            count++;
            break;
        }

        /*
         * Rule: "...and the previous frame pointer at fp - 2 * XLEN/8"
         * Rule: "The end of the frame record chain is indicated by the address zero"
         */
        const unsigned long prev_fp = *(const unsigned long *)(curr_fp - 2 * XLEN_BYTES);

        count++;

        if (prev_fp == 0) {
            break; /* End of chain */
        }

        /* Move to the previous frame */
        curr_fp = prev_fp;
    }

    return count;
}

void riscv_backtrace_print(unsigned long fp, unsigned long stack_top, size_t stack_size,
                           unsigned long exc_sp, uint8_t mode)
{
    /* Read current runtime stack pointer */
    register unsigned long current_sp asm("sp");

    /* Capture frames */
    static backtrace_frame_t frames[BACKTRACE_MAX_DEPTH];
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)exc_sp;
    int count = riscv_backtrace_frames(fp, frames, BACKTRACE_MAX_DEPTH,
                                             stack_top, stack_size);

    /* 1. Print Exception Frame Info if exc_sp is provided */
    if (exc_sp != 0) {
        BACKTRACE_PRINT_FN("=== Exception Frame Information ===\n");
        Exception_DumpFrame(exc_sp, mode);
        BACKTRACE_PRINT_FN("\n");
    }

    BACKTRACE_PRINT_FN("=== Stack Backtrace ===\n");
    BACKTRACE_PRINT_FN("SP (current): 0x%lx\n", current_sp);
    BACKTRACE_PRINT_FN("\nBacktrace:\n");
    for (int i = 0; i < count; i++) {
        BACKTRACE_PRINT_FN("#%d  RA=0x%lx, CFA=0x%lx\n",
                           i, frames[i].ra, frames[i].cfa);
    }

    /*
     * 2. Print addr2line friendly string
     * Output format: addr2line -e <elf> -a -f <epc> <ra1> <ra2> ...
     * If exc_sp is provided, the exception EPC is printed first to indicate
     * the exact location where the exception occurred.
     */
    BACKTRACE_PRINT_FN("\n[Addr2Line Hint] Run the following command on PC:\n");
    BACKTRACE_PRINT_FN("riscv64-unknown-elf-addr2line -pfiaC -e *.elf");

    if (exc_sp != 0) {
        /* Print exception EPC to show where the exception occurred */
        BACKTRACE_PRINT_FN(" 0x%lx", (unsigned long)(exc_frame->epc));
    }

    for (int i = 0; i < count; i++) {
        /* Print just the addresses, separated by spaces */
        BACKTRACE_PRINT_FN(" 0x%lx", frames[i].ra);
    }
    BACKTRACE_PRINT_FN("\n\n");
}
