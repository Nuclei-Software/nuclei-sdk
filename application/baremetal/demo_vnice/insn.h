#ifndef __INSN_H__
#define __INSN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nuclei_sdk_soc.h>
#include <riscv_vector.h>

#define DATASIZE    256

/*
******************************************************************************************
* NICE Extension Instruction Format:
*  .insn and r indicates this is a pseudo and R-type instruction.
* Supported format: only R type here
* This VNICE Demo implements the following 3 instructions for NICE-Core:
* * VNICE_LOAD: memory load.
* * VNICE_STORE: memory store.
* * VNICE CMPX_MUL: Complex multiplication [x * y = (a + b i)*(c + d i) = (ac - bd) + i(ad + bc)].
******************************************************************************************
*/

/** custom nice instruction vnice load */
__STATIC_FORCEINLINE vint32m8_t __custom_vnice_load_v_i32m8 (uint32_t* addr)
{
    vint32m8_t rdata ;
    asm volatile(".insn r 0xb,1,0x7f,%0,%1,x0"
            : "=vr"(rdata)
            : "r"(addr)
            );

    return rdata;
}

/** custom nice instruction vnice store */
__STATIC_FORCEINLINE void __custom_vnice_store_v_i32m8 (uint32_t* addr, vint32m8_t wdata)
{
    int zero = 0;
    asm volatile(".insn r 0xb,2,0x7f,x0,%1,%2"
                : "=r"(zero)
                : "r"(addr)
                , "vr"(wdata)
                );

}

/** custom nice instruction vnice compx multi */
__STATIC_FORCEINLINE vint8m8_t __custom_vnice_cmpx_mult_v_i8m8 (vint8m8_t op1, vint8m8_t op2)
{
    vint8m8_t result;
    asm volatile(".insn r 0xb, 0, 0x7f, %0, %1, %2"
                : "=vr"(result)
                : "vr"(op1)
                , "vr"(op2)
                );
    return result;
}

/** custom nice instruction vnice compx mask multi */
__STATIC_FORCEINLINE vint8m8_t __custom_vnice_cmpx_mult_mask_v_i8m8 (vint8m8_t op1, vint8m8_t op2)
{
    vint8m8_t result;
    asm volatile(".insn r 0xb, 3, 0x7f, %0, %1, %2"
                : "=vr"(result)
                : "vr"(op1)
                , "vr"(op2)
                );
    return result;
}

void normal_case(uint32_t *addr_in1, uint32_t *addr_in2, uint32_t *addr_out);
void vnice_case(uint32_t *addr_in1, uint32_t *addr_in2, uint32_t vnice_addr, uint32_t *addr_out);
int compare_result(uint32_t *normal_out, uint32_t *vnice_out);

#ifdef __cplusplus
}
#endif

#endif /* __INSN_H__ */
