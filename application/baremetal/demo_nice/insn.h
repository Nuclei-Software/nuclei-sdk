#ifndef __INSN_H__
#define __INSN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nuclei_sdk_soc.h>


/**
 *******************************************************************************
 * custom3:
 * Supported format: only R type here
 * Supported instr:
 *  1. custom3 add: 1 cycle add
 *     add a0, a1, a2
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  2. custom3 lbuf: burst 4 load(4 cycles) data in memory to row_buf
 *     lbuf (a1)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  3. custom3 sbuf: burst 4 store(4 cycles) row_buf to memory
 *     sbuf (a1)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  4. custom3 pipe add: 4 stage pileline add, put (rs1+rs2) into pipeline(4
 *  cycles)
 *     pipeadd a0, a1, a2(4 cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  5. custom3 write acc matrix setup: matrix setup write, store rs1 into
 *  matrix_cfg register
 *     wsetup x0, a1, x0(1 cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  6. custom3 read acc matrix setup: matrix setup read, load matrix_cfg
 *  register to rs1
 *     rsetup x0, a1, x0(1 cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  7. custom3 acc rowsum: load data from memory(@a1), accumulate row datas and
 *  write back
 *     rowsum rd, a1, x0(N cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *  8. custom3 clear acc rbuf: clear rowbuf
 *     crowbuf x0, x0, x0(1 cycles)
 *     .insn r opcode, func3, func7, rd, rs1, rs2
 *******************************************************************************
 * matrix_cfg:
 * 32------------------------------------------------1----------0
 * |                        reserved                 |  rownum  |
 * |-------------------------------------------------|----------|
 *
 * row_buf size: 4 word(typical), can be configured through ROWBUF_DP
 */


#define ROW_LEN     3
#define COL_LEN     3

/** custom nice instruction add */
__STATIC_FORCEINLINE int custom_add(unsigned long* add1, unsigned long* add2)
{
    int sum;

    asm volatile(".insn r 0x7b, 7, 0, %0, %1, %2"
                 : "=r"(sum)
                 : "r"(add1), "r"(add2));

    return sum;
}

/** custom nice instruction pipeadd */
__STATIC_FORCEINLINE int custom_pipeadd(unsigned long* add1, unsigned long* add2)
{
    int sum;

    asm volatile(".insn r 0x7b, 7, 3, %0, %1, %2"
                 : "=r"(sum)
                 : "r"(add1), "r"(add2));

    return sum;
}

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

/** custom nice instruction wsetup */
__STATIC_FORCEINLINE void custom_wsetup(int rownum)
{
    int zero = 0;

    asm volatile(".insn r 0x7b, 2, 4, x0, %1, x0" : "=r"(zero) : "r"(rownum));
}

/** custom nice instruction rsetup */
__STATIC_FORCEINLINE int custom_rsetup(void)
{
    int rownum;

    asm volatile(".insn r 0x7b, 4, 5, %0, x0, x0" : "=r"(rownum));

    return rownum;
}

/** custom nice instruction rowsum */
__STATIC_FORCEINLINE int custom_rowsum(unsigned long* addr)
{
    int rowsum;

    asm volatile(".insn r 0x7b, 6, 6, %0, %1, x0" : "=r"(rowsum) : "r"(addr));

    return rowsum;
}

/** custom nice instruction clear rowsum */
__STATIC_FORCEINLINE void custom_crowsum(void)
{
    int zero;

    asm volatile(".insn r 0x7b, 0, 7, x0, x0, x0" : "=r"(zero));
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
