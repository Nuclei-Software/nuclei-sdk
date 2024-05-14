
#include <stdio.h>
#include "insn.h"

void normal_case(uint32_t *addr_in1, uint32_t *addr_in2, uint32_t *addr_out)
{
    unsigned char *pin1 = (unsigned char *)addr_in1;
    unsigned char *pin2 = (unsigned char *)addr_in2;
    unsigned char tmp[2];
    unsigned char *pout = (unsigned char *)addr_out;
    vint32m8_t vout;
    size_t l;
    uint32_t blkCnt;

    // complex multiplication
    for (int i = 0; i < DATASIZE; i += 2) {
        tmp[0] = (unsigned char)(pin1[i + 1] * pin2[i] + pin1[i] * pin2[i + 1]);
        tmp[1] = (unsigned char)(pin1[i + 1] * pin2[i + 1] - pin1[i] * pin2[i]);
        pin1[i] = tmp[0];
        pin1[i + 1] = tmp[1];
    }

    // complex mask multiplication
    for (int i = 0; i < DATASIZE; i += 2) {
        // Simulate the mask sequence, skipping complex multiplication calculations every 8 bytes
        if (!(i%8)) {
            pin1[i + 1] = (unsigned char)(pin1[i + 1] * pin2[i + 1] - pin1[i] * pin2[i]);
        } else {
            tmp[0] = (unsigned char)(pin1[i + 1] * pin2[i] + pin1[i] * pin2[i + 1]);
            tmp[1] = (unsigned char)(pin1[i + 1] * pin2[i + 1] - pin1[i] * pin2[i]);
            pin1[i] = tmp[0];
            pin1[i + 1] = tmp[1];
        }
    }

    blkCnt = DATASIZE/4;

    for (; (l = __riscv_vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
        vout = __riscv_vle32_v_i32m8((int32_t *)pin1, l);
        __riscv_vse32_v_i32m8((int32_t *)pin2, vout, l);
        vout = __riscv_vle32_v_i32m8((int32_t *)pin2, l);
        __riscv_vse32_v_i32m8((int32_t *)pout, vout, l);
        pin1 += l*4;
        pin2 += l*4;
        pout += l*4;
    }
}

void vnice_case(uint32_t *addr_in1, uint32_t *addr_in2, uint32_t vnice_addr, uint32_t *addr_out)
{
    unsigned char *pin1 = (unsigned char *)addr_in1;
    unsigned char *pin2 = (unsigned char *)addr_in2;
    unsigned char *pvnice = (unsigned char *)(unsigned long)vnice_addr;
    unsigned char *pout = (unsigned char *)addr_out;
    vint8m8_t vin1, vin2, vmult;
    volatile vint8m8_t vtemp;
    vint32m8_t vout;
    size_t l;
    uint32_t blkCnt;
    // mask sequence
    uint8_t vmgroup[16] = {0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
                           0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe};

    // Testing vnice custom complex multiplication intruction
    blkCnt = DATASIZE;
    for (; (l = __riscv_vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
        vin1 = __riscv_vle8_v_i8m8(pin1, l);
        vin2 = __riscv_vle8_v_i8m8(pin2, l);
        vmult = __custom_vnice_cmpx_mult_v_i8m8(vin1, vin2);
        __riscv_vse8_v_i8m8(pin1, vmult, l);
        pin1 += l;
        pin2 += l;
    }

    pin1 = (unsigned char *)addr_in1;
    pin2 = (unsigned char *)addr_in2;

    // Testing vnice custom complex mask multiplication intruction
    blkCnt = DATASIZE;
    for (; (l = __riscv_vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
        vbool1_t vm = __riscv_vlm_v_b1(vmgroup, l);
        vin1 = __riscv_vle8_v_i8m8(pin1, l);
        vin2 = __riscv_vle8_v_i8m8(pin2, l);
        vtemp = __riscv_vadd_vv_i8m8_m(vm, vin2, vin1, l);  // just mv vm to v0 in order to use v0 as mask vector reg for vnice mask instruction
        vmult = __custom_vnice_cmpx_mult_mask_v_i8m8(vin1, vin2);
        __riscv_vse8_v_i8m8(pin1, vmult, l);
        pin1 += l;
        pin2 += l;
    }

    pin1 = (unsigned char *)addr_in1;
    pin2 = (unsigned char *)addr_in2;
    blkCnt = DATASIZE/4;

    // Testing vnice custom load/store intruction to load or store data to vnice private memory at vnice
    // no relation with above code
    for (; (l = __riscv_vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
        vout = __riscv_vle32_v_i32m8((int32_t *)pin1, l);
        __custom_vnice_store_v_i32m8((int32_t *)pvnice, vout);
        vout = __custom_vnice_load_v_i32m8((int32_t *)pvnice);
        __riscv_vse32_v_i32m8((int32_t *)pout, vout, l);
        pin1 += l*4;
        pvnice += l*4;
        pout += l*4;
    }
}

int compare_result(uint32_t* normal_out, uint32_t* nice_out)
{
    int i, ret = 0;

    for (i = 0; i < DATASIZE/4; i++) {
        if (normal_out[i] != nice_out[i]) {
            printf("num %d not match: %u vs %u\r\n", i, normal_out[i], nice_out[i]);
            ret = -1;
        }
    }
    return ret;
}

