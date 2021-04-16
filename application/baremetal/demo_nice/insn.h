#ifndef __INSN_H__
#define __INSN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nuclei_sdk_soc.h>

/*
 ******************************************************************************************
 * NICE Extension Instruction Format:
 *  .insn and r indicates this is a pseudo and R-type instruction.
 *  0x7b is the value of the opcode field, which means it is a
 *  NICE instruction belonging to custom3.
 * Supported format: only R type here
 * This NICE Demo implements the following 3 instructions for NICE-Core:
 * * CLW or lbuf: Load 12-byte data from memory to row buffer.
 * * CSW or sbuf: Store 12-byte data from row buffer to memory.
 * * CACC or rowsum: Sums a row of the matrix, and columns are accumulated automatically.
 * Supported instructions for this nice demo:
 *  1. custom3 lbuf: burst 4 load(4 cycles) data in memory to row_buf
 *     lbuf (a1)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  2. custom3 sbuf: burst 4 store(4 cycles) row_buf to memory
 *     sbuf (a1)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  3. custom3 acc rowsum: load data from memory(@a1), accumulate row data and write back
 *     rowsum rd, a1, x0(N cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 ******************************************************************************************
 */

#define ROW_LEN     3
#define COL_LEN     3

/** custom nice instruction lbuf */
__STATIC_FORCEINLINE void custom_lbuf(unsigned long* addr)
{
    int zero = 0;

    asm volatile(".insn r 0x7b, 2, 1, x0, %1, x0" : "=r"(zero) : "r"(addr));
}

/** custom nice instruction sbuf */
__STATIC_FORCEINLINE void custom_sbuf(unsigned long* addr)
{
    int zero = 0;

    asm volatile(".insn r 0x7b, 2, 2, x0, %1, x0" : "=r"(zero) : "r"(addr));
}

/** custom nice instruction rowsum */
__STATIC_FORCEINLINE int custom_rowsum(unsigned long* addr)
{
    int rowsum;

    asm volatile(".insn r 0x7b, 6, 6, %0, %1, x0" : "=r"(rowsum) : "r"(addr));

    return rowsum;
}
/** normal test case without NICE accelerator. */
void normal_case(unsigned int array[ROW_LEN][COL_LEN], unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN]);

/** teat case using NICE accelerator. */
void nice_case(unsigned int array[ROW_LEN][COL_LEN], unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN]);

/** print input array */
void print_array(unsigned int array[ROW_LEN][COL_LEN]);

/** print matrix result */
void print_result(unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN]);

/** compare result of reference and nice */
int compare_result(unsigned int ref_cs[COL_LEN], unsigned int ref_rs[ROW_LEN], \
                   unsigned int nice_cs[COL_LEN], unsigned int nice_rs[ROW_LEN]);

#ifdef __cplusplus
}
#endif

#endif /* __INSN_H__ */
