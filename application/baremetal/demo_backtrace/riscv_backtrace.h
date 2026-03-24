/**
 * @file riscv_backtrace.h
 * @brief Struct-based stack unwinding for RISC-V using frame pointers.
 *
 * This module provides a lightweight, debug-info-free stack backtrace mechanism
 * for embedded RISC-V systems (bare-metal, RTOS, etc.). It relies solely on the
 * frame pointer (s0/fp) chain established when compiling with
 * `-fno-omit-frame-pointer`.
 *
 * <h2>Frame Pointer Convention</h2>
 * The implementation strictly follows the specific convention provided:
 *
 * 1. <b>CFA (Canonical Frame Address):</b>
 *    The frame pointer register (s0/x8) holds the CFA, which is defined as
 *    the value of the stack pointer (sp) upon entry to the current procedure.
 *
 * 2. <b>Frame Record Location:</b>
 *    The "frame record" (Return Address and Previous Frame Pointer) resides
 *    stack-downwards relative to the CFA.
 *
 * 3. <b>Layout:</b>
 *    - <b>Return Address (RA):</b> Located at `CFA - XLEN_BYTES`.
 *    - <b>Previous FP:</b> Located at `CFA - 2 * XLEN_BYTES`.
 *
 * 4. <b>Termination:</b>
 *    The chain ends when the "Next Link" (the read Previous FP) is zero.
 *
 * <h2>Requirements</h2>
 * - All code must be compiled with `-fno-omit-frame-pointer`.
 * - Leaf functions may omit frame records; backtrace will stop at such functions
 *   or skip them depending on the exact prologue generation.
 * - The stack must be XLEN-aligned (RISC-V hardware/ABI requirement).
 *
 * <h2>Usage Example</h2>
 * @code
 * // In an exception handler (e.g., Illegal Instruction, Load/Store Fault)
 * void exception_handler(unsigned long mcause, unsigned long sp) {
 *     // Get current frame pointer
 *     unsigned long fp;
 *     asm volatile ("mv %0, s0" : "=r"(fp));
 *     extern char _sp;             // stack top, Symbol from linker script
 *     size_t stack_size = 2048;    // stack size in bytes
 *
 *     // Print backtrace with exception frame info
 *     // Pass sp (exception stack pointer) to show exception context
 *     riscv_backtrace_print(fp, (unsigned long)&_sp, stack_size, sp, PRV_M);
 * }
 * @endcode
 *
 * <h2>Customization</h2>
 * - Define @c BACKTRACE_PRINT_FN before including this header to override
 *   the default @c printf (e.g., @c #define BACKTRACE_PRINT_FN my_uart_printf).
 * - Define @c BACKTRACE_MAX_DEPTH to adjust maximum unwinding depth
 *   (default: 16) to limit stack usage during the trace.
 */

#ifndef RISCV_BACKTRACE_H
#define RISCV_BACKTRACE_H

#include <stdint.h>
#include <stddef.h>

/* ================================================================== */
/* Configuration Macros                                               */
/* ================================================================== */

/**
 * @brief Print function used by riscv_backtrace_print().
 *
 * Defaults to standard @c printf. Override by defining before inclusion:
 * @code
 * #define BACKTRACE_PRINT_FN my_uart_printf
 * #include "riscv_backtrace.h"
 * @endcode
 *
 * The function must support @c %lx format for @c unsigned long.
 */
#ifndef BACKTRACE_PRINT_FN
#include <stdio.h>
#define BACKTRACE_PRINT_FN printf
#endif

/**
 * @brief Maximum number of stack frames to unwind.
 *
 * Default: 16. Reduce to save stack space in deeply nested handlers.
 * Must be > 0.
 */
#ifndef BACKTRACE_MAX_DEPTH
#define BACKTRACE_MAX_DEPTH 16
#endif

#if BACKTRACE_MAX_DEPTH <= 0
#error "BACKTRACE_MAX_DEPTH must be greater than zero."
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================== */
/* Data Structures                                                    */
/* ================================================================== */

/**
 * @brief Represents a single unwound stack frame.
 *
 * This structure captures the essential information from a RISC-V stack frame:
 * - The return address (@c ra) is the address to which the function will return.
 * - The Canonical Frame Address (@c cfa) represents the stack pointer value
 *   at the moment the function was called. In this convention, the CFA is
 *   equal to the value of the Frame Pointer register (fp).
 */
typedef struct {
    unsigned long ra;  /**< Return address (read from fp - XLEN) */
    unsigned long cfa; /**< Canonical Frame Address (value of fp) */
} backtrace_frame_t;

/* ================================================================== */
/* Function Prototypes                                                */
/* ================================================================== */

/**
 * @brief Unwinds the stack from a given frame pointer and captures frames.
 *
 * This function walks the frame pointer chain starting at @p fp and populates
 * the @p frames array with up to @p max_depth valid stack frames.
 *
 * <h3>Logic</h3>
 * Based on the specified convention:
 * 1. Current FP is validated against stack bounds.
 * 2. RA is read from `(fp - XLEN_BYTES)`.
 * 3. Previous FP is read from `(fp - 2 * XLEN_BYTES)`.
 * 4. Loop continues with the Previous FP.
 *
 * <h3>Safety Guarantees</h3>
 * - Never dereferences unaligned pointers.
 * - Never accesses memory outside [@p stack_top - @p stack_size, @p stack_top).
 * - Stops immediately upon encountering an invalid or zero frame pointer.
 * - Does not perform any dynamic memory allocation.
 *
 * @param[in]  fp          Initial frame pointer (s0 register value).
 *                         Typically obtained from a saved register context.
 * @param[out] frames      User-provided buffer to store captured frames.
 *                         Must be large enough to hold @p max_depth elements.
 * @param[in]  max_depth   Maximum number of frames to capture.
 *                         Automatically clamped to @ref BACKTRACE_MAX_DEPTH.
 * @param[in]  stack_top   Virtual address of the top of the stack
 *                         (highest valid stack address, exclusive).
 * @param[in]  stack_size  Total size of the stack in bytes.
 *                         Used to compute the stack bottom for bounds checking.
 *
 * @return The number of successfully captured frames (0 on error or empty chain).
 */
int riscv_backtrace_frames(unsigned long fp, backtrace_frame_t *frames, int max_depth,
                           unsigned long stack_top, size_t stack_size);

/**
 * @brief Prints a human-readable backtrace including the current SP and CFA values.
 *
 * This convenience function:
 * 1. Reads the current runtime stack pointer (sp) via inline assembly.
 * 2. Calls @ref riscv_backtrace_frames() to capture frames.
 * 3. Prints results using @ref BACKTRACE_PRINT_FN.
 * 4. Prints exception frame information if exc_sp is non-zero.
 * 5. Prints addr2line hint with EPC (if exc_sp is provided) and return addresses
 *    for easy symbol resolution on host PC.
 *
 * Addr2Line Hint output format:
 * @code
 * [Addr2Line Hint] Run the following command on PC:
 * riscv64-unknown-elf-addr2line -pfiaC -e *.elf 0x<epc> 0x<ra1> 0x<ra2> ...
 * @endcode
 * If exc_sp is provided, the exception EPC is included as the first address
 * to indicate the exact location where the exception occurred.
 *
 * @param[in] fp          Frame pointer to start unwinding from.
 * @param[in] stack_top   Top of the stack (high address).
 * @param[in] stack_size  Stack size in bytes.
 * @param[in] exc_sp      Exception stack pointer for Exception_DumpFrame (0 if not available).
 * @param[in] mode        Privilege mode (PRV_M/PRV_S/PRV_U) for Exception_DumpFrame.
 *
 * @note This function uses a local array of @ref BACKTRACE_MAX_DEPTH frames.
 *       Ensure sufficient stack space in your calling context.
 * @note If exc_sp is non-zero, it will call Exception_DumpFrame to print exception
 *       frame information (epc, tval, cause, msubm) before the backtrace.
 */
void riscv_backtrace_print(unsigned long fp, unsigned long stack_top, size_t stack_size,
                           unsigned long exc_sp, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* RISCV_BACKTRACE_H */
