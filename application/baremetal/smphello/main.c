#include <stdio.h>
#include "nuclei_sdk_soc.h"

#if !defined(__riscv_atomic)
#error "RVA(atomic) extension is required for SMP"
#endif

#if !defined(SMP_CPU_CNT)
#error "SMP_CPU_CNT macro is not defined, please set SMP_CPU_CNT to integer value > 1"
#endif

typedef struct {
    uint32_t state;
} spinlock;

spinlock lock;
volatile uint32_t lock_ready = 0;
volatile uint32_t cpu_count = 0;
volatile uint32_t finished = 0;

__STATIC_FORCEINLINE void spinlock_init(spinlock *lock)
{
    lock->state = 0;
}

__STATIC_FORCEINLINE void spinlock_lock(spinlock *lock)
{
    uint32_t old;
    uint32_t backoff = 10;
    do {
        old = __AMOSWAP_W((&(lock->state)), 1);
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

void boot_hart_main(unsigned long hartid);
void other_harts_main(unsigned long hartid);
void main(void);

/* Reimplementation of smp_main for multi-harts */
void smp_main(void)
{
    main();
}

void main(void)
{
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);
    if (hartid == 0) { // boot hart
        spinlock_init(&lock);
        lock_ready = 1;
        finished = 0;
        __SMP_RWMB();
        boot_hart_main(hartid);
    } else { // other harts
        // wait for lock initialized
        while (lock_ready == 0);
        other_harts_main(hartid);
    }
}

void boot_hart_main(unsigned long hartid)
{
    volatile unsigned long waitcnt = 0;
    spinlock_lock(&lock);
    printf("Hello world from hart %d\n", hartid);
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
    } else {
        printf("Some harts boot failed, only %d/%d booted!\n", cpu_count, SMP_CPU_CNT);
    }
}

void other_harts_main(unsigned long hartid)
{
    spinlock_lock(&lock);
    printf("Hello world from hart %d\n", hartid);
    cpu_count += 1;
    spinlock_unlock(&lock);
    // wait for all harts boot and print hello
    while (cpu_count < SMP_CPU_CNT);
    // wait for boot hart to set finished flag
    while (finished == 0);
}
