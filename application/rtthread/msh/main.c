/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-24     Wu Han    RTT MSH Demo
 * 2020-11-27     Wu Han    Adapted for Nuclei SDK
 */

#include "nuclei_sdk_soc.h"
#include <rtthread.h>
#include <stdio.h>

void nsdk(int argc, char* argv[])
{
    rt_kprintf("Hello Nuclei SDK!\n");
}
MSH_CMD_EXPORT(nsdk, msh nuclei sdk demo)


int main(void)
{
    CSR_MCFGINFO_Type mcfg_info;

#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg_info.b.clic = 1;
#else
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
#endif

    if (0 == mcfg_info.b.clic) {
        printf("ECLIC is not present, will not run this example!\r\n");
        while (1);
    }

    rt_kprintf("Hello RT-Thread!\n");
    while (1) {
        rt_thread_mdelay(100);
    }
}
