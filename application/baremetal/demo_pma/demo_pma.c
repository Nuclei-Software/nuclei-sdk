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

// PMA Region granule is 4KB (0x1000)
#define REGION_GRANULE    (1UL << 12)
// 64 bytes intended, corresponding to cache line 64 bytes
#define COL_SIZE         64

//#define BIG_ROW_SIZE
#ifndef BIG_ROW_SIZE
// region size 4KB, must be aligned to REGION_GRANULE
#define REGION_SIZE                     (1 * REGION_GRANULE)
// ROW_SIZE is 64
#define ROW_SIZE         (REGION_SIZE/COL_SIZE)
#else
// region size 64KB, must be aligned to REGION_GRANULE
#define REGION_SIZE                     (16 * REGION_GRANULE)
// ROW_SIZE is 1024
#define ROW_SIZE         (REGION_SIZE/COL_SIZE)
#endif

// region base address should be aligined by region_size
uint8_t array_test[ROW_SIZE][COL_SIZE] __attribute__((aligned(REGION_SIZE))) = {0};
uint8_t array_test_r[ROW_SIZE][COL_SIZE];
// Declare HPMCOUNTER4
HPM_DECLARE_VAR(4);
// Means select the Dcache miss events, record Dcache miss event for all M/S/U mode
#define HPM_EVENT4      HPM_EVENT(EVENT_SEL_MEMORY_ACCESS, EVENT_MEMORY_ACCESS_DCACHE_MISS, MSU_EVENT_ENABLE)

static unsigned long check_aligned(unsigned long base_addr, unsigned long region_size)
{
    if ( 0 != (base_addr % REGION_GRANULE))
        return 0;
    if ( 0 != (base_addr % region_size))
        return 0;
    return 1;
}

void array_init(void)
{
    int32_t i, j = 0;

    for (i = 0; i < ROW_SIZE; i++)
        for (j = 0; j < COL_SIZE; j++) {
            array_test[i][j] = 0x34;
        }
}

void array_read_by_row(void)
{
    int32_t i, j = 0;

    for (i = 0; i < ROW_SIZE; i++)
        for (j = 0; j < COL_SIZE; j++) {
            array_test_r[i][j] = array_test[i][j];
        }
}

BENCH_DECLARE_VAR();

int main(void)
{
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    CacheInfo_Type cacheinfo_type;
    pma_config pma_cfg_r;
    pma_config pma_cfg = {
        // Take care to set the region type and address range, which maybe causing function or performance issue!
        .region_type = PMA_REGION_TYPE_CA,
        .region_base = (unsigned long)&array_test,
        .region_size = REGION_SIZE,
        .region_enable = PMA_REGION_ENA
    };
    if (!DCachePresent()) {
        printf("DCache not present in CPU!\n");
        return -1;
    }

    if (!check_aligned(pma_cfg.region_base, pma_cfg.region_size)) {
        printf("Error: Region base address 0x%x must be aligned to region granule 0x%x and region size 0x%x!\n", pma_cfg.region_base, REGION_GRANULE, pma_cfg.region_size);
        return -1;
    }

    GetDCacheInfo(&cacheinfo_type);
    if (COL_SIZE != cacheinfo_type.linesize) {
        printf("\nWarning: change COL_SIZE to %d to make them same!\n\n", cacheinfo_type.linesize);
    }

    printf("DCache Linesize is %d bytes, ways is %d, setperway is %d, total size is %d bytes\n\n", cacheinfo_type.linesize, \
            cacheinfo_type.ways, cacheinfo_type.setperway,cacheinfo_type.size);

    array_init();

    printf("array_test size: %d * %d bytes, addr: 0x%x\r\n", ROW_SIZE, COL_SIZE, (unsigned long)&array_test);
    EnableICache();

    EnableDCache(); // enable dcache here on purpose before setting NonCacheable, which actually takes no effect to array update
    // To ensure cached data consistency, because it's NonCacheable now
    MFlushInvalDCache();

    printf("\nSet to NonCacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_NC;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);

    HPM_START(4, array_read_by_row_dcache_miss_noncacheable, HPM_EVENT4);
    array_read_by_row();
    HPM_END(4, array_read_by_row_dcache_miss_noncacheable, HPM_EVENT4);

    MFlushInvalDCache();
    EnableDCache();
    printf("\nSet to Cacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_CA;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);

    HPM_START(4, array_read_by_row_dcache_miss_cacheable, HPM_EVENT4);
    array_read_by_row();
    HPM_END(4, array_read_by_row_dcache_miss_cacheable, HPM_EVENT4);
#else
    printf("[ERROR]__CCM_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}


