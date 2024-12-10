// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>
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

#define ROW_SIZE         512
#define COL_SIZE         64
// 32KB
#define REGION_SIZE                     (512 * 64)

// 4K aligned
uint8_t array_test[ROW_SIZE][COL_SIZE] __attribute__((aligned(0x1000))) = {0};

void array_update_by_row(void)
{
    int32_t i, j = 0;

    for (i = 0; i < ROW_SIZE; i++)
        for (j = 0; j < COL_SIZE; j++) {
            array_test[i][j] = 0xab;
        }
}

BENCH_DECLARE_VAR();

int main(void)
{
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    pma_config pma_cfg_r;
    pma_config pma_cfg = {
        // Take care to set the region type and address range, which maybe causing function or performance issue!
        .region_type = PMA_REGION_TYPE_DEV,
        .region_base = (unsigned long)&array_test,
        .region_size = REGION_SIZE,
        .region_enable = PMA_REGION_ENA
    };

    if (!DCachePresent()) {
        printf("DCache not present in CPU!\n");
        return -1;
    }
    BENCH_INIT();

    // To ensure data consistency
    DisableDCache();
    MFlushInvalDCache();

    printf("\nSet to NonCacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_NC;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);

    BENCH_START(NonCacheable);
    array_update_by_row();
    BENCH_END(NonCacheable);

    EnableDCache();

    printf("\nSet to Cacheable region\n\r");
    pma_cfg.region_type = PMA_REGION_TYPE_CA;
    PMA_SetRegion(0, &pma_cfg);
    PMA_GetRegion(0, &pma_cfg_r);
    printf("Region type: 0x%x,region base addr: 0x%lx, region size: 0x%lx, region status: %d\n\r",\
        pma_cfg_r.region_type, pma_cfg_r.region_base, pma_cfg_r.region_size, pma_cfg_r.region_enable);

    BENCH_START(Cacheable);
    array_update_by_row();
    BENCH_END(Cacheable);
#else
    printf("[ERROR]__CCM_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}

