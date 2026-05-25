/**************************************************************************//**
 * @file     regression_stubs.c
 * @brief    ThreadX SMP regression runner compatibility stubs.
 ******************************************************************************/

#include <stdio.h>

#include "tx_api.h"

void test_control_return(UINT status);

void threadx_trace_basic_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    printf("Running Trace Basic Test............................................ SKIPPED\n");
    test_control_return(0);
}
