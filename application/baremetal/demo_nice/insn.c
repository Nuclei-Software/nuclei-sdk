#include <stdio.h>
#include "insn.h"

#define _DEBUG_INFO_

// normal test case without NICE accelerator.
void normal_case(unsigned int array[ROW_LEN][COL_LEN], unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN])
{
    int i = 0, j = 0;

    unsigned int tmp = 0;
    for (i = 0; i < ROW_LEN; i++) {
        tmp = 0;
        for (j = 0; j < COL_LEN; j++) {
            col_sum[j] += array[i][j];
            tmp += array[i][j];
        }
        row_sum[i] = tmp;
    }
}


// teat case using NICE accelerator.
void nice_case(unsigned int array[ROW_LEN][COL_LEN], unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN])
{
    int i;
    unsigned long init_buf[COL_LEN] = {0};

    custom_lbuf(init_buf);
    for (i = 0; i < ROW_LEN; i++) {
        row_sum[i] = custom_rowsum((unsigned long*)array[i]);
    }
    custom_sbuf((unsigned long*)col_sum);
}

void print_array(unsigned int array[ROW_LEN][COL_LEN])
{
    int i, j;
    printf("the element of array is :\r\n\t");
    for (i = 0; i < ROW_LEN; i++) {
        printf("%d\t", array[0][i]);
    }
    printf("\r\n\t");
    for (i = 0; i < ROW_LEN; i++) {
        printf("%d\t", array[1][i]);
    }
    printf("\r\n\t");
    for (i = 0; i < ROW_LEN; i++) {
        printf("%d\t", array[2][i]);
    }
    printf("\r\n\r\n");
}

void print_result(unsigned int col_sum[COL_LEN], unsigned int row_sum[ROW_LEN])
{
    int i, j;
    printf("the sum of each row is :\r\n\t\t");
    for (i = 0; i < ROW_LEN; i++) {
        printf("%d\t", row_sum[i]);
    }
    printf("\r\n");
    printf("the sum of each col is :\r\n\t\t");
    for (j = 0; j < COL_LEN; j++) {
        printf("%d\t", col_sum[j]);
    }
    printf("\r\n");
}

int compare_result(unsigned int ref_cs[COL_LEN], unsigned int ref_rs[ROW_LEN], \
                   unsigned int nice_cs[COL_LEN], unsigned int nice_rs[ROW_LEN])
{
    int i, ret = 0;
    for (i = 0; i < COL_LEN; i ++) {
        if (ref_cs[i] != nice_cs[i]) {
#ifdef _DEBUG_INFO_
            printf("Column %d result not match: %d vs %d\r\n", ref_cs[i], nice_cs[i]);
#endif
            ret = -1;
        }
    }
    for (i = 0; i < ROW_LEN; i ++) {
        if (ref_rs[i] != nice_rs[i]) {
#ifdef _DEBUG_INFO_
            printf("Row %d result not match: %d vs %d\r\n", ref_rs[i], nice_rs[i]);
#endif
            ret = -1;
        }
    }
    return ret;
}
