#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

CTEST(core, vector)
{
#ifdef __riscv_vector
    __disable_vector();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_VS, 0);
    __enable_vector();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_VS, MSTATUS_VS_INITIAL);
#endif
}
