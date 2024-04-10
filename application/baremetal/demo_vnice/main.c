#include <stdio.h>
#include "insn.h"
#include "nuclei_sdk_soc.h"

// TODO this is vnice internal region private address
#define VNICE_PRIVATE_ADDR  0x0

int main(void)
{
    int ret = 0;

    uint32_t array_normal_in1[DATASIZE] = {0};
    uint32_t array_normal_in2[DATASIZE] = {0};
    uint32_t array_normal_out[DATASIZE] = {0};

    uint32_t array_vnice_in1[DATASIZE] = {0};
    uint32_t array_vnice_in2[DATASIZE] = {0};
//    unsigned int array_vnice_addr[DATASIZE] = {0};
    uint32_t array_vnice_out[DATASIZE] = {0};

    uint32_t initdata = 0;

    uint32_t begin_instret, end_instret, instret_normal, instret_vnice;
    uint32_t begin_cycle, end_cycle, cycle_normal, cycle_vnice;

    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_XS);
    __enable_minstret_counter();
    __enable_mcycle_counter();

    printf("1. Set array_normal_in1 array_normal_in1 array_vnice_in1 array_vnice_in2\r\n");
    for (int i = 0; i < DATASIZE; i++) {
        initdata = (i%16) + (i%16) << 8 + (i%16) << 16 + (i%16) << 24;
        array_normal_in1[i] = initdata;
        array_normal_in2[i] = initdata;
        array_vnice_in1[i] = initdata;
        array_vnice_in2[i] = initdata;
    }

    printf("2. Do reference vector complex mul, store, load\r\n");

    begin_instret = __get_rv_instret();
    begin_cycle = __get_rv_cycle();

    normal_case(array_normal_in1, array_normal_in2, array_normal_out);
    end_instret = __get_rv_instret();
    end_cycle = __get_rv_cycle();

    instret_normal = end_instret - begin_instret;
    cycle_normal = end_cycle - begin_cycle;

    printf("3. Do vector nice complex mul, store, load\r\n");

    begin_instret = __get_rv_instret();
    begin_cycle = __get_rv_cycle();

    vnice_case(array_vnice_in1, array_vnice_in2, VNICE_PRIVATE_ADDR, array_vnice_out);

    end_instret = __get_rv_instret();
    end_cycle = __get_rv_cycle();

    instret_vnice = end_instret - begin_instret;
    cycle_vnice = end_cycle - begin_cycle;

    printf("4. Compare reference and vnice result\r\n");

    if (compare_result(array_normal_out, array_vnice_out) == 0) {
        printf("PASS\r\n");
    } else {
        printf("FAIL\r\n");
        ret = 1;
    }

    printf("5. Performance summary\r\n");

    printf("\t normal: \r\n");
    printf("\t      instret: %lu, cycle: %lu\r\n", instret_normal, cycle_normal);
    printf("\t vnice  : \r\n");
    printf("\t      instret: %lu, cycle: %lu\r\n", instret_vnice, cycle_vnice);

    return ret;
}
