/* ThreadX Regression Test for UP (Uniprocessor)
   This runs the ThreadX validation/regression test suite on Nuclei SDK. */
#include <stdio.h>
#include "tx_api.h"
#include "nuclei_sdk_hal.h"

int main()
{
    CSR_MCFGINFO_Type mcfg_info;

#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg_info.b.clic = 1;
#else
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
#endif

    if (mcfg_info.b.clic == 0) {
        printf("ECLIC is not present, will not run this example!\r\n");
        return 0;
    }

    printf("ThreadX Regression Test for UP (Uniprocessor)\r\n");

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    /* Should never reach here */
    return 0;
}

void external_exit(UINT code)
{
    printf("ThreadX regression exit code: %u\r\n", code);
    while (1) {
        __WFI();
    }
}
