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
    printf("usecyc:%lu, lpcnt:%lu, sumcyc:%lu\n", (unsigned long)BENCH_GET_USECYC(), (unsigned long)BENCH_GET_LPCNT(), (unsigned long)BENCH_GET_SUMCYC());

}
