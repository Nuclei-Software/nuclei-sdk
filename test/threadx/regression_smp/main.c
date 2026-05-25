#include <stdio.h>

#include "nuclei_sdk_hal.h"
#include "tx_api.h"

int main(void)
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

    tx_kernel_enter();
    return 0;
}

extern void _tx_thread_smp_initialize_wait(void);

void smp_main(void)
{
    if (__get_hart_id() == 0) {
        main();
    } else {
        _tx_thread_smp_initialize_wait();
    }
}

void external_exit(UINT code)
{
    printf("ThreadX regression exit code: %u\r\n", code);
    while (1) {
        __WFI();
    }
}
