// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nuclei_sdk_soc.h"

#include "nmsis_bench.h"

#if !defined(__CCM_PRESENT) || (__CCM_PRESENT != 1)
/* __CCM_PRESENT should be defined in <Device>.h */
#warning "This example require CPU CCM feature!"
#endif

#if !defined(__DCACHE_PRESENT) || (__DCACHE_PRESENT != 1)
/* __DCACHE_PRESENT should be defined in <Device>.h */
#error "This example require CPU DCACHE feature!"
#endif


#if !defined(__PMA_PRESENT) || (__PMA_PRESENT != 1)
/* __PMA_PRESENT should be defined in <Device>.h */
#error "This example require CPU PMA feature!"
#endif

//#define BIG_ROW_SIZE
#ifndef BIG_ROW_SIZE
#define ROW_SIZE         10
#else
#define ROW_SIZE         1024
#endif
#define COL_SIZE         64
// 64KB
#define REGION_SIZE                     (1024 * 64)

// 4K aligned
uint8_t array_test[ROW_SIZE][COL_SIZE] __attribute__((aligned(0x1000))) = {0};

// Declare HPMCOUNTER4
HPM_DECLARE_VAR(4);
// Means select the Dcache miss events, record Dcache miss event for all M/S/U mode
#define HPM_EVENT4      HPM_EVENT(EVENT_SEL_MEMORY_ACCESS, EVENT_MEMORY_ACCESS_DCACHE_MISS, MSU_EVENT_ENABLE)


void array_update_by_row(void)
{
    memset(array_test, 0xab, sizeof(array_test));
}

BENCH_DECLARE_VAR();

int main(void)
{
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    CacheInfo_Type cacheinfo_type;
    pma_config pma_cfg_r;
    pma_config pma_cfg = {
        // Take care to set the region type and address range, which maybe causing function or performance issue!
        .region_type = PMA_REGION_TYPE_DEV,
        .region_base = (unsigned long)&array_test,
        .region_size = REGION_SIZE,
        .region_enable = PMA_REGION_ENA
    };
    BENCH_INIT();
    if (!DCachePresent()) {
        printf("DCache not present in CPU!\n");
        return -1;
    }
    GetDCacheInfo(&cacheinfo_type);
    printf("DCache Linesize is %d bytes, ways is %d, setperway is %d, total size is %d bytes\n\n", cacheinfo_type.linesize, \
            cacheinfo_type.ways, cacheinfo_type.setperway,cacheinfo_type.size);
    printf("array_test size: %d * %d bytes, addr: 0x%x\r\n", ROW_SIZE, COL_SIZE, (unsigned long)&array_test);

    EnableDCache(); // enable dcache here on purpose before setting NonCacheable, which actually takes no effect to array update
    // To ensure cached data consistency, because it's NonCacheable now
    MFlushInvalDCache();

    printf("\nSet to NonCacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_NC;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);
    // hot the cache here takes no effect if it's NonCacheable
    array_update_by_row();

    HPM_START(4, array_update_by_row_dcache_miss_noncacheable, HPM_EVENT4);
    BENCH_START(NonCacheable);
    array_update_by_row();
    BENCH_END(NonCacheable);
    HPM_END(4, array_update_by_row_dcache_miss_noncacheable, HPM_EVENT4);

    printf("\nSet to Cacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_CA;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);
    // hot the cache
    array_update_by_row();

    HPM_START(4, array_update_by_row_dcache_miss_cacheable, HPM_EVENT4);
    BENCH_START(Cacheable);
    array_update_by_row();
    BENCH_END(Cacheable);
    HPM_END(4, array_update_by_row_dcache_miss_cacheable, HPM_EVENT4);
#else
    printf("[ERROR]__CCM_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}


