#include <stdlib.h>
#include <string.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

// uncomment below to disable nmsis bench api
//#define DISABLE_NMSIS_BENCH

#include "nmsis_bench.h"


BENCH_DECLARE_VAR();

unsigned long test_mem[32];

CTEST(bench, bench)
{
    BENCH_INIT();
    BENCH_START(memset);
    memset(test_mem, 0xa5, sizeof(test_mem));
    BENCH_END(memset);
    BENCH_ERROR(memset);
    BENCH_STATUS(memset);

    BENCH_RESET(memsetloop);
    for (int i = 0; i < 10; i ++) {
        BENCH_START(memsetloop);
        memset(test_mem, 0xa5, sizeof(test_mem));
        BENCH_SAMPLE(memsetloop);
    }
    BENCH_STOP(memsetloop);
    BENCH_STAT(memsetloop);
    printf("usecyc:%lu, lpcnt:%lu, sumcyc:%lu\n", (uint32_t)BENCH_GET_USECYC(), (uint32_t)BENCH_GET_LPCNT(), (uint32_t)BENCH_GET_SUMCYC());

}
// Declare HPMCOUNTER3 and HPMCOUNTER4
HPM_DECLARE_VAR(3);
HPM_DECLARE_VAR(4);

// Define HPMEVENT3 and HPMEVENT4
// HPMEVENT3 value macro: event_sel: 0, event_idx: 8, m/s/uevent_enable: 0xf
// means Select the instruction commit events, record conditional branch event for all M/S/U mode
#define HPM_EVENT3      HPM_EVENT(0, 8, 0xf)

// HPMEVENT4 value macro: event_sel: 0, event_idx: 4, m/s/uevent_enable: 0x1
// means Select the instruction commit events, record integer store instruction (includes SC) event for only U mode
#define HPM_EVENT4      HPM_EVENT(0, 4, 0x1)

CTEST(bench, hpm)
{
    // Init HPM bench, only need to do it once
    HPM_INIT();

    // start to record hpm3 and hpm4
    HPM_START(3, memset, HPM_EVENT3);
    HPM_START(4, memset, HPM_EVENT4);
    memset(test_mem, 0x5a, sizeof(test_mem));
    // finish record and print hpm value
    HPM_END(3, memset, HPM_EVENT3);
    HPM_END(4, memset, HPM_EVENT4);

    HPM_RESET(3, memsetloop, HPM_EVENT3);
    HPM_RESET(4, memsetloop, HPM_EVENT4);
    for (int i = 0; i < 10; i ++) {
        HPM_START(3, memsetloop, HPM_EVENT3);
        HPM_START(4, memsetloop, HPM_EVENT4);
        memset(test_mem, 0x5a, sizeof(test_mem));
        // finish record and print hpm value
        HPM_SAMPLE(3, memsetloop, HPM_EVENT3);
        HPM_SAMPLE(4, memsetloop, HPM_EVENT4);
    }
    HPM_STOP(3, memsetloop, HPM_EVENT3);
    HPM_STOP(4, memsetloop, HPM_EVENT4);

    HPM_STAT(3, memsetloop, HPM_EVENT3);
    HPM_STAT(4, memsetloop, HPM_EVENT4);
    printf("hpm3, usecyc:%lu, lpcnt:%lu, sumcyc:%lu\n", (uint32_t)HPM_GET_USECYC(3), (uint32_t)HPM_GET_LPCNT(3), (uint32_t)HPM_GET_SUMCYC(3));
    printf("hpm4, usecyc:%lu, lpcnt:%lu, sumcyc:%lu\n", (uint32_t)HPM_GET_USECYC(4), (uint32_t)HPM_GET_LPCNT(4), (uint32_t)HPM_GET_SUMCYC(4));
}

