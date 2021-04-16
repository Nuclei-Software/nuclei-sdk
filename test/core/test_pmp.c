#include <stdlib.h>
#include <stdio.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

#define PMPADDR(x)      _AC(CSR_PMPADDR, x)
#define PMPCFG(x)       _AC(CSR_PMPCFG, x)

#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
static uint32_t memory_pmp1[4];
CTEST(pmp, pmpreg)
{
    CTEST_LOG("PMPCFG0 : 0x%x", __get_PMPCFGx(0));
    CTEST_LOG("PMP0CFG - PMP3CFG: 0x%x 0x%x 0x%x 0x%x", \
              __get_PMPxCFG(0), __get_PMPxCFG(1), __get_PMPxCFG(2), __get_PMPxCFG(3));

    for (int i = 0; i < __PMP_ENTRY_NUM; i ++) {
        CTEST_LOG("%d PMPADDRx  PMPxCFG   0x%x 0x%x", i, __get_PMPADDRx(i), __get_PMPxCFG(i));
    }
    ASSERT_EQUAL(__get_PMPADDRx(0), __RV_CSR_READ(PMPADDR(0)));
    ASSERT_EQUAL(__get_PMPADDRx(4), __RV_CSR_READ(PMPADDR(4)));
#if __RISCV_XLEN == 32
    for (int i = 0; i < __PMP_ENTRY_NUM / 4; i ++) {
        CTEST_LOG("%d PMPCFGx  0x%x", i, __get_PMPCFGx(i));
    }
#elif __RISCV_XLEN == 64
    for (int i = 0; i < __PMP_ENTRY_NUM / 8; i ++) {
        CTEST_LOG("%d PMPCFGx  0x%x", i,  __get_PMPCFGx(i * 2));
    }
#endif
    __set_PMPADDRx(0, (rv_csr_t)(&memory_pmp1[0]) >> 2);
    __set_PMPxCFG(0, PMP_A_NA4 | PMP_R | PMP_W);
    CTEST_LOG("PMPADDR0  PMP0CFG   0x%x 0x%x", __get_PMPADDRx(0), __get_PMPxCFG(0));
    __set_PMPADDRx(1, (rv_csr_t)(&memory_pmp1[4]) >> 2);
    __set_PMPxCFG(1, PMP_A_NA4 | PMP_R | PMP_W);
    CTEST_LOG("PMPADDR1  PMP1CFG   0x%x 0x%x", __get_PMPADDRx(1), __get_PMPxCFG(1));
}


CTEST(pmp, pmpset)
{

}
#endif
