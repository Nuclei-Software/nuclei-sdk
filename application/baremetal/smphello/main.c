#include <stdio.h>
#include "nuclei_sdk_soc.h"

#if !defined(__riscv_atomic)
#error "RVA(atomic) extension is required for SMP"
#endif

#if !defined(SMP_CPU_CNT)
#warning "This example require CPU SMP feature!"
#error "SMP_CPU_CNT macro is not defined, please set SMP_CPU_CNT to integer value > 1"
#endif

typedef struct {
    uint32_t state;
} spinlock;

spinlock lock;
volatile uint32_t lock_ready = 0;
volatile uint32_t cpu_count = 0;
volatile uint32_t finished = 0;

// comment SPINLOCK_CAS to use AMOSWAP as spinlock
#define SPINLOCK_CAS

__STATIC_FORCEINLINE void spinlock_init(spinlock *lock)
{
    lock->state = 0;
}

__STATIC_FORCEINLINE void spinlock_lock(spinlock *lock)
{
    uint32_t old;
    uint32_t backoff = 10;
    do {
#ifndef SPINLOCK_CAS
        // Use amoswap as spinlock
        old = __AMOSWAP_W((&(lock->state)), 1);
#else
        // use lr.w & sc.w to do CAS as spinlock
        old = __CAS_W((&(lock->state)), 0, 1);
#endif
        if (old == 0) {
            break;
        }
        for (volatile int i = 0; i < backoff; i ++) {
            __NOP();
        }
        backoff += 10;
    } while (1);
}

__STATIC_FORCEINLINE void spinlock_unlock(spinlock *lock)
{
    lock->state = 0;
}

int boot_hart_main(unsigned long hartid);
int other_harts_main(unsigned long hartid);
int main(void);

/* Reimplementation of smp_main for multi-harts */
int smp_main(void)
{
    return main();
}

int main(void)
{
    int ret;
    // get hart id in current cluster
    unsigned long hartid = __get_hart_id();
    if (hartid == BOOT_HARTID) { // boot hart
        spinlock_init(&lock);
        lock_ready = 1;
        finished = 0;
        __SMP_RWMB();
        ret = boot_hart_main(hartid);
    } else { // other harts
        // wait for lock initialized
        while (lock_ready == 0);
        ret = other_harts_main(hartid);
    }
    return ret;
}

int boot_hart_main(unsigned long hartid)
{
    volatile unsigned long waitcnt = 0;
    spinlock_lock(&lock);
    printf("Hello world from hart %lu\n", hartid);
    cpu_count += 1;
    spinlock_unlock(&lock);
    // wait for all harts boot and print hello
    while (cpu_count < SMP_CPU_CNT) {
        waitcnt ++;
        __NOP();
        // The waitcnt compare value need to be adjust according
        // to cpu frequency
        if (waitcnt >= SystemCoreClock) {
            break;
        }
    }
    if (cpu_count == SMP_CPU_CNT) {
        printf("All harts boot successfully!\n");
        finished = 1;
        return 0;
    } else {
        printf("Some harts boot failed, only %d/%d booted!\n", cpu_count, SMP_CPU_CNT);
        return -1;
    }
}

int other_harts_main(unsigned long hartid)
{
    spinlock_lock(&lock);
    printf("Hello world from hart %lu\n", hartid);
    cpu_count += 1;
    spinlock_unlock(&lock);
    // wait for all harts boot and print hello
    while (cpu_count < SMP_CPU_CNT);
    // wait for boot hart to set finished flag
    while (finished == 0);
    return 0;
}
