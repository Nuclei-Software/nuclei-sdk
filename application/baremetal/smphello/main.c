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

// volatile required
volatile spinlock lock;
volatile uint32_t lock_ready = 0;
volatile uint32_t cpu_count = 0;
volatile uint32_t finished = 0;

/* Define SPINLOCK_CAS to use the lr.w/sc.w compare and swap instead of the
 * single amoswap.w, only useful to compare the two atomic primitives */
// #define SPINLOCK_CAS

__STATIC_FORCEINLINE void spinlock_init(spinlock *lock)
{
    lock->state = 0;
    __RWMB();
}

__STATIC_FORCEINLINE void spinlock_lock(spinlock *lock)
{
    do {
        /* Poll with a plain load first: while the lock is taken this loop only
         * issues loads and does not touch the reservation/first claim of the
         * cache line, so N harts spinning here do not fight each other on the
         * bus. Only once the lock looks free do we pay for an atomic access. */
        while (lock->state != 0) {
            __NOP();
        }
#ifdef SPINLOCK_CAS
        /* lr.w/sc.w: the sc.w may fail if another hart wrote the line in
         * between, in that case spin again from the top */
        if (__CAS_W(&lock->state, 0, 1) == 0) {
#else
        /* amoswap.w: unconditional, it either takes the lock or returns the
         * value the other hart just stored */
        if (__AMOSWAP_W(&lock->state, 1) == 0) {
#endif
            /* Make the acquire visible: no load/store inside the critical
             * section may be reordered before this point */
            __RWMB();
            break;
        }
    } while (1);
}

__STATIC_FORCEINLINE void spinlock_unlock(spinlock *lock)
{
    /* Make the stores done inside the critical section visible to the next
     * hart that takes the lock, before handing the lock back */
    __RWMB();
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
    // get local hart id inside current cluster, BOOT_HARTID is defined as the local hart id
    // (returned by __get_local_hart_id(), see core_feature_base.h for the mhartid layout), so
    // compare against it, not the hart index (hart index is for per-hart resources like timer)
    unsigned long hartid = __get_local_hart_id();
    if (hartid == BOOT_HARTID) { // boot hart
        spinlock_init((spinlock *)&lock);
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
    spinlock_lock((spinlock *)&lock);
    printf("Hello world from hart %lu\n", hartid);
    cpu_count += 1;
    spinlock_unlock((spinlock *)&lock);
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
    spinlock_lock((spinlock *)&lock);
    printf("Hello world from hart %lu\n", hartid);
    cpu_count += 1;
    spinlock_unlock((spinlock *)&lock);
    // wait for all harts boot and print hello
    while (cpu_count < SMP_CPU_CNT);
    // wait for boot hart to set finished flag
    // Attention: must wait until all the harts stop print,
    // because the _postmain_fini will print some dummy '\0', which has no lock-protecting
    while (finished == 0);
    return 0;
}
