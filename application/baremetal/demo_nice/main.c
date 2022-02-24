#include <stdio.h>
#include "insn.h"
#include "nuclei_sdk_soc.h"

int main(void)
{
    int ret = 0;
    unsigned int array[ROW_LEN][COL_LEN] = {
        {10, 30, 90},
        {20, 40, 80},
        {30, 90, 120}
    };
    unsigned int col_sum_ref[COL_LEN] = {0};
    unsigned int row_sum_ref[ROW_LEN] = {0};
    unsigned int col_sum_nice[COL_LEN] = {0};
    unsigned int row_sum_nice[ROW_LEN] = {0};
    unsigned int begin_instret, end_instret, instret_normal, instret_nice;
    unsigned int begin_cycle, end_cycle, cycle_normal, cycle_nice;

    __RV_CSR_WRITE(CSR_MSTATUS, MSTATUS_XS);
    __enable_minstret_counter();
    __enable_mcycle_counter();

    printf("\r\nNuclei Nice Acceleration Demonstration\r\n");
    printf("Warning: This demo required CPU to implement Nuclei provided NICE Demo instructions.\r\n");
    printf("         Otherwise this example will trap to cpu core exception!\r\n\r\n");

    printf("1. Print input matrix array\r\n");
    print_array(array);

    printf("2. Do reference matrix column sum and row sum\r\n");

    begin_instret = __get_rv_instret();
    begin_cycle = __get_rv_cycle();

    normal_case(array, col_sum_ref, row_sum_ref);
    end_instret = __get_rv_instret();
    end_cycle = __get_rv_cycle();

    instret_normal = end_instret - begin_instret;
    cycle_normal = end_cycle - begin_cycle;

    printf("2. Do nice matrix column sum and row sum\r\n");

    begin_instret = __get_rv_instret();
    begin_cycle = __get_rv_cycle();

    nice_case(array, col_sum_nice, row_sum_nice);

    end_instret = __get_rv_instret();
    end_cycle = __get_rv_cycle();

    instret_nice = end_instret - begin_instret;
    cycle_nice = end_cycle - begin_cycle;

    printf("3. Compare reference and nice result\r\n");
    printf("  1) Reference result:\r\n");
    print_result(col_sum_ref, row_sum_ref);
    printf("  2) Nice result:\r\n");
    print_result(col_sum_nice, row_sum_nice);
    printf("  3) Compare reference vs nice: ");

    if (compare_result(col_sum_ref, row_sum_ref, col_sum_nice, row_sum_nice) == 0) {
        printf("PASS\r\n");
    } else {
        printf("FAIL\r\n");
        ret = 1;
    }

    printf("4. Performance summary\r\n");

    printf("\t normal: \r\n");
    printf("\t      instret: %u, cycle: %u\r\n", instret_normal, cycle_normal);
    printf("\t nice  : \r\n");
    printf("\t      instret: %u, cycle: %u\r\n", instret_nice, cycle_nice);

    return ret;
}
