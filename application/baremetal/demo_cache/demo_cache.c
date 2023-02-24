// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"

#define DISABLE_NMSIS_HPM
#include "nmsis_bench.h"

#if !defined(__CCM_PRESENT) || (__CCM_PRESENT != 1)
/* __CCM_PRESENT should be defined in <Device>.h */
#warning "__CCM_PRESENT is not defined or equal to 1, please check!"
#endif

// Declare HPMCOUNTER4
HPM_DECLARE_VAR(4);
// Means select the Dcache miss events, record Dcache miss event for all M/S/U mode, using 
#define HPM_EVENT4      HPM_EVENT(EVENT_SEL_MEMORY_ACCESS, EVENT_MEMORY_ACCESS_DCACHE_MISS, MSU_EVENT_ENABLE)

//#define BIG_ROW_SIZE
#ifndef BIG_ROW_SIZE
#define ROW_SIZE         10
#else
#define ROW_SIZE         2048
#endif
/* Column size same as cache line size, for better understanding cache mechanism*/
#define COL_SIZE         64

uint8_t array_test[ROW_SIZE][COL_SIZE] __attribute__((aligned(0x40))) = {0};

void array_update_by_row(void)
{
    int32_t i, j = 0;

    for (i = 0; i < ROW_SIZE; i++)
        for (j = 0; j < COL_SIZE; j++) {
            array_test[i][j] = 0xab;
        }
}

void array_update_by_col(void)
{
    int32_t i, j = 0;

    for (i = 0; i < COL_SIZE; i++)
        for (j = 0; j < ROW_SIZE; j++) {
            array_test[j][i] = 0xab;
        }
}

void array_init(void)
{
    int32_t i, j = 0;

    for (i = 0; i < ROW_SIZE; i++)
        for (j = 0; j < COL_SIZE; j++) {
            array_test[i][j] = 0x34;
        }
}

int main(void)
{
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    int32_t ret = 0;
    int32_t val = 0;
    CacheInfo_Type cacheinfo_type;

    if (!DCachePresent() || !ICachePresent()) {
        printf("DCache or ICache not present in CPU!\n");
        return -1;
    }

    GetDCacheInfo(&cacheinfo_type);
    printf("DCache Linesize is %d bytes, ways is %d, setperway is %d, total size is %d bytes\n\n", cacheinfo_type.linesize, \
            cacheinfo_type.ways, cacheinfo_type.setperway,cacheinfo_type.size);
    printf("array_test %d * %d bytes\n", ROW_SIZE, COL_SIZE);

    printf("\n------Update array in memory------\n");
    EnableDCache();
    EnableICache();

    MFlushDCache();
    MInvalDCache();

    // Init HPM bench, only need to do it once
    HPM_INIT();

    // start to record hpm3 and hpm4
    printf("\n------Update array to all 0xab in cache: array_update_by_row------\n");
    // start to record hpm4
    HPM_START(4, array_update_by_row_dcache_miss, HPM_EVENT4);
    array_update_by_row();
    // finish record and print hpm value
    HPM_END(4, array_update_by_row_dcache_miss, HPM_EVENT4);

    printf("\n-------Keep DCache valid, do array_update_by_row again-------\n");
    HPM_START(4, array_update_by_row_dcache_miss, HPM_EVENT4);
    array_update_by_row();
    HPM_END(4, array_update_by_row_dcache_miss, HPM_EVENT4);

    printf("\n-------Invalidate all the Dcache-------\n");
    MInvalDCache();
    printf("\n------Update array to all 0xab in cache: array_update_by_col ------\n");
    HPM_START(4, array_update_by_col_dcache_miss, HPM_EVENT4);
    array_update_by_col();
    HPM_END(4, array_update_by_col_dcache_miss, HPM_EVENT4);

    printf("Read out array_test[0][0] 0x%x in cache, then disable DCache\n", array_test[0][0]);
    DisableDCache();

    printf("\n------Init array in memory to all 0x34------\n");
    array_init();
    printf("Read out array_test[0][0] 0x%x in memory, then enable Dcache\n", array_test[0][0]);
    EnableDCache();
    MFlushDCache();
    printf("After cache flushed to memory, array_test[0][0] in memory is 0x%x\n", array_test[0][0]);

    printf("\n------Again init array in memory to all 0x34, then enable DCache------\n");
    DisableDCache();
    array_init();
    /* Read from memory */
    printf("Read out array_test[0][0] 0x%x in memory\n", array_test[0][0]);
    EnableDCache();
    printf("Read out array_test[0][0] 0x%x in cache, when mapped value in memory has changed\n", array_test[0][0]);

    MInvalDCache();
    HPM_START(4, dcachemiss_readonebyte, HPM_EVENT4);
    /* Read brings in one cache miss */
    val = *(volatile uint8_t*) &array_test[0][0];
    HPM_END(4, dcachemiss_readonebyte, HPM_EVENT4);
#else
    printf("[ERROR]__CCM_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}

