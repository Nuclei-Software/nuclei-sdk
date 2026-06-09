// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

/*
 * ECLIC (Enhanced Core Local Interrupt Controller) Stress Test Case
 *
 * USER GUIDE: This test case can be customized by adjusting interrupt levels and shadow levels
 * to test different interrupt nesting and tail-chaining scenarios. Follow the TODO markers
 * throughout this file to modify interrupt level configurations and shadow level settings
 * for comprehensive stress testing of the ECLIC system.
 *
 * Key configurable aspects:
 * 1. Interrupt Levels: Modify the level values in the #define statements marked with TODO
 *    to test different interrupt priority scenarios and nesting behaviors.
 * 2. Shadow Levels: Adjust shadow register configurations to test hardware context switching
 *    with shadow register.
 * 3. Tail-Chaining: Modify interrupt triggering patterns to test tail-chaining performance.
 * 4. Nested Interrupts: Change level assignments to validate proper interrupt preemption.
 *
 * The test case validates the functionality of the Nuclei RISC-V processor's
 * Enhanced Core Local Interrupt Controller (ECLIC) in both Machine Mode (M-Mode)
 * and Supervisor Mode (S-Mode) environments. The test performs comprehensive
 * validation of interrupt handling capabilities including:
 *
 * 1. M-Mode Interrupt Handling:
 *    - Timer interrupts (SysTimer_IRQn) with non-vector mode
 *    - Software interrupts (SysTimerSW_IRQn) with vector mode
 *    - External interrupts (SOC_INT20_IRQn to SOC_INT29_IRQn) with mixed vector/non-vector modes
 *    - Nested interrupt handling with different levels
 *    - Context saving/restoring for vector interrupts
 *
 * 2. S-Mode Interrupt Handling:
 *    - Timer interrupts (SysTimer_S_IRQn) with non-vector mode
 *    - Software interrupts (SysTimerSW_S_IRQn) with vector mode
 *    - External interrupts (SOC_INT30_IRQn to SOC_INT39_IRQn) with mixed vector/non-vector modes
 *    - Proper privilege level transitions
 *
 * 3. Interrupt Nesting:
 *    - Different interrupt levels (0-4) to test different interrupt level handling
 *    - Ability to interrupt lower-level handlers with higher-level interrupts
 *    - Proper restoration of interrupted contexts
 *
 * 4. Cross-Mode Communication:
 *    - M-Mode and S-Mode interrupt counters to validate both modes are functioning
 *    - Proper isolation between M-Mode and S-Mode interrupt handling
 *
 * 5. Stress Testing:
 *    - Continuous computation in interrupt handlers to stress the system
 *    - Background computation in main threads to ensure continuous load
 *    - Verification that all interrupt counters increment consistently
 *
 * 6. ECLIC Features Validation:
 *    - Level-triggered and edge-triggered interrupt configurations
 *    - Vector vs non-vector interrupt modes
 *    - Interrupt pending/clear operations
 *    - CSR (Control and Status Register) context management
 *
 * 7. ECLICv2 Automatic Stack Switching (when applicable):
 *    - When ECLICv2 is present with INTSTACK macro defined and hardware context auto-switching enabled,
 *      the test validates automatic stack switching functionality for both M-Mode and S-Mode interrupts
 *    - Separate interrupt stacks are allocated and configured via MTSP/STSP CSRs
 *    - Hardware automatically switches to dedicated interrupt stacks during interrupt processing
 *
 * The test passes when both M-Mode and S-Mode interrupt counters reach a consistent
 * threshold, indicating that all interrupt sources are properly handled and the
 * ECLIC system is functioning correctly under stress conditions.
 */

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
#else
#error "This example require CPU ECLIC feature"
#endif

#if defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1)
#else
#error "This example require CPU System Timer feature"
#endif

#define IRQ3_PARAMS             50, 3, 1, 2
#define IRQ3_EXPECTED_RESULT    5666
#define IRQ7_PARAMS             45, 2, 1, 2
#define IRQ7_EXPECTED_RESULT    3729

#define IRQ20_PARAMS            50, 3, 1, 2
#define IRQ20_EXPECTED_RESULT   5666
#define IRQ21_PARAMS            45, 2, 1, 2
#define IRQ21_EXPECTED_RESULT   3729
#define IRQ22_PARAMS            42, 3, 1, 1
#define IRQ22_EXPECTED_RESULT   4086
#define IRQ23_PARAMS            38, 2, 2, 1
#define IRQ23_EXPECTED_RESULT   4088
#define IRQ24_PARAMS            35, 4, 1, 2
#define IRQ24_EXPECTED_RESULT   3456
#define IRQ25_PARAMS            48, 3, 2, 1
#define IRQ25_EXPECTED_RESULT   7414
#define IRQ26_PARAMS            40, 2, 1, 1
#define IRQ26_EXPECTED_RESULT   2736
#define IRQ27_PARAMS            30, 4, 2, 1
#define IRQ27_EXPECTED_RESULT   3206
#define IRQ28_PARAMS            35, 3, 2, 1
#define IRQ28_EXPECTED_RESULT   4046
#define IRQ29_PARAMS            45, 5, 1, 2
#define IRQ29_EXPECTED_RESULT   6859

#define M_BG_PARAMS             56, 4, 2, 3
#define M_BG_EXPECTED_RESULT    11899

/*
 * TODO: Define interrupt level ranges - individual macros for each interrupt level
 * These levels can be adjusted to test interrupt nesting and tail-chaining functionality
 * NOTE: The level values must be within the range 0 to (1<<nlbits)-1, where nlbits is obtained from ECLIC configuration
 * Higher level numbers represent higher priority interrupts that can preempt lower level interrupts
 * All level values below are USER ADJUSTABLE: Range 0->(1<<nlbits)-1
 */
#define SYSTIMER_SW_LEVEL    1    // Level for SysTimerSW_IRQn (non-vector software interrupt)
#define SYSTIMER_LEVEL       0    // Level for SysTimer_IRQn (timer interrupt)
#define INT20_LEVEL    2    // Level for interrupt 20 (non-vector)
#define INT21_LEVEL    3    // Level for interrupt 21 (non-vector)
#define INT22_LEVEL    3    // Level for interrupt 22 (non-vector)
#define INT23_LEVEL    4    // Level for interrupt 23 (non-vector)
#define INT24_LEVEL    2    // Level for interrupt 24 (non-vector)
#define INT25_LEVEL    1    // Level for interrupt 25 (non-vector)
#define INT26_LEVEL    2    // Level for interrupt 26 (vector)
#define INT27_LEVEL    3    // Level for interrupt 27 (vector)
#define INT28_LEVEL    2    // Level for interrupt 28 (vector)
#define INT29_LEVEL    4    // Level for interrupt 29 (vector)

#define IRQ1_PARAMS             55, 3, 2, 1
#define IRQ1_EXPECTED_RESULT    9538
#define IRQ5_PARAMS             55, 3, 2, 1
#define IRQ5_EXPECTED_RESULT    9538

#define IRQ30_PARAMS            60, 4, 1, 2
#define IRQ30_EXPECTED_RESULT   10120
#define IRQ31_PARAMS            58, 3, 2, 1
#define IRQ31_EXPECTED_RESULT   10941
#define IRQ32_PARAMS            55, 2, 1, 3
#define IRQ32_EXPECTED_RESULT   5634
#define IRQ33_PARAMS            52, 5, 1, 1
#define IRQ33_EXPECTED_RESULT   8516
#define IRQ34_PARAMS            48, 3, 2, 2
#define IRQ34_EXPECTED_RESULT   7574
#define IRQ35_PARAMS            50, 4, 1, 3
#define IRQ35_EXPECTED_RESULT   7352
#define IRQ36_PARAMS            45, 2, 3, 1
#define IRQ36_EXPECTED_RESULT   9256
#define IRQ37_PARAMS            40, 5, 1, 2
#define IRQ37_EXPECTED_RESULT   5195
#define IRQ38_PARAMS            53, 3, 1, 2
#define IRQ38_EXPECTED_RESULT   6629
#define IRQ39_PARAMS            47, 4, 2, 1
#define IRQ39_EXPECTED_RESULT   8119

#define S_BG_PARAMS             56, 3, 2, 3
#define S_BG_EXPECTED_RESULT    10538

/*
 * TODO: S-Mode interrupt level definitions
 * These levels can also be adjusted to test interrupt nesting and tail-chaining in S-Mode
 * NOTE: The level values must be within the range 0 to (1<<nlbits)-1, where nlbits is obtained from ECLIC configuration
 * Higher level numbers represent higher priority interrupts that can preempt lower level interrupts
 * All level values below are USER ADJUSTABLE: Range 0->(1<<nlbits)-1
 */
#define SYSTIMER_SW_S_LEVEL    3    // Level for SysTimerSW_IRQn (non-vector software interrupt)
#define SYSTIMER_S_LEVEL       1    // Level for SysTimer_IRQn (timer interrupt)

#define INT30_LEVEL    3    // Level for interrupt 30 (non-vector)
#define INT31_LEVEL    3    // Level for interrupt 31 (non-vector)
#define INT32_LEVEL    2    // Level for interrupt 32 (non-vector)
#define INT33_LEVEL    4    // Level for interrupt 33 (non-vector)
#define INT34_LEVEL    3    // Level for interrupt 34 (non-vector)
#define INT35_LEVEL    2    // Level for interrupt 35 (non-vector)
#define INT36_LEVEL    3    // Level for interrupt 36 (vector)
#define INT37_LEVEL    1    // Level for interrupt 37 (vector)
#define INT38_LEVEL    4    // Level for interrupt 38 (vector)
#define INT39_LEVEL    2    // Level for interrupt 39 (vector)

/* Loop count for printing execution statistics */
#ifdef CFG_SIMULATION
#define PRINT_LOOP_COUNT                5
#else
#define PRINT_LOOP_COUNT                100
#endif

// NOTE: SSUBM is introduced from ECLIC v2
#if __ECLIC_VER == 2
#define READ_SSUBM()            __RV_CSR_READ(CSR_SSUBM)
#else
#define READ_SSUBM()            0
#endif

#define SMODE_TICK_MS           20
#define MMODE_TICK_MS           10

#define MMODE_TIMER_TICKS       (SOC_TIMER_FREQ / (1000 / (MMODE_TICK_MS)))
#define SMODE_TIMER_TICKS       (SOC_TIMER_FREQ / (1000 / (SMODE_TICK_MS)))

/* Create separated interrupt stack M-Mode and S-Mode execution when INTSTACK macro defined */
#define INTERRUPT_STACK_SIZE        20480
uint8_t mmode_int_stack[INTERRUPT_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t mmode_int_sp = (uintptr_t)(mmode_int_stack + sizeof(mmode_int_stack));

uint8_t smode_int_stack[INTERRUPT_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_int_sp = (uintptr_t)(smode_int_stack + sizeof(smode_int_stack));

/* Create a stack for supervisor mode execution */
#define SMODE_STACK_SIZE            20480
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_sp = (uintptr_t)(smode_stack + sizeof(smode_stack));

// Counters for additional interrupts
static volatile uint32_t mmode_eclic_int_cnt[12] = {0}; // For SOC_INT20_IRQn to SOC_INT29_IRQn, and M-Mode Timer and SwTimer IRQ
static volatile uint32_t smode_eclic_int_cnt[12] = {0};  // For SOC_INT30_IRQn to SOC_INT39_IRQn, and S-Mode Timer and SwTimer IRQ

/* Global test passed counter */
static volatile uint32_t test_passed = 0;


#define MMODE_INTERRUPT_MASK        0x80000000
#define INTERRUPT_NUMBER_MASK       0x0000FFFF

#define MMODE_IRQNUM(level)         (MMODE_INTERRUPT_MASK | (level))
#define SMODE_IRQNUM(level)         (level)

#define GET_IRQNUM(value)           (INTERRUPT_NUMBER_MASK & (value))

__STATIC_INLINE uint32_t perform_computation(uint32_t iterations, uint32_t mult_factor, uint32_t shift1, uint32_t shift2, uint32_t expected_result, const char* handler_name, uint32_t irq_to_trigger)
{
    volatile uint32_t computation_result = 0;
    volatile uint32_t i, j;

    // Perform some computation to stress test the interrupt handling
    for (i = 0; i < iterations; i++) {
        computation_result += (i * mult_factor) + (i << shift1) - (i >> shift2);
    }

    // Trigger interrupt in the middle of computation if irq_to_trigger > 0
    if (GET_IRQNUM(irq_to_trigger) > 0) {
        if (irq_to_trigger & MMODE_INTERRUPT_MASK) {
            ECLIC_SetPendingIRQ(GET_IRQNUM(irq_to_trigger));
#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
        } else {
            ECLIC_SetPendingIRQ_S(GET_IRQNUM(irq_to_trigger));
#endif
        }
    }

    // Additional computation with different operations
    for (j = 1; j < (iterations/2 + 10); j++) {
        computation_result ^= (j * (mult_factor + 1)) + (j << (shift1 - 1));
        computation_result &= 0xFFFF; // Keep within 16-bit range
    }

    // Only verify if expected_result is not 0 (for calculation mode in main)
    if (expected_result != 0 && computation_result != expected_result) {
        // Disable all interrupts
        if (irq_to_trigger & MMODE_INTERRUPT_MASK) {
            __disable_irq();
            __disable_irq_s();
            printf("\n[M] ");
        } else {
            __disable_irq_s();
            printf("\n[S] ");
        }
        printf("ERROR in %s: Expected %u, got %u\n", handler_name, expected_result, computation_result);
#ifdef CFG_SIMULATION
        // directly exit with error if in nuclei internally simulation
        SIMULATION_EXIT(1);
#endif
        while(1);         // Halt execution
    }

    return computation_result;
}

/* Generic helper function to check if all interrupt counters in an array are equal */
static uint32_t check_interrupt_counters(volatile uint32_t *counter_array, uint32_t threshold)
{
    uint32_t i;
    uint32_t first_value = counter_array[0];
    uint32_t all_equal = 1;

    // Only check if the first counter has reached at least 1 (meaning interrupts are firing)
    if (first_value > 0) {
        for (i = 1; i < 12; i++) {
            if (counter_array[i] != first_value) {
                all_equal = 0;
                break;
            }
        }

        if (all_equal && first_value > threshold) {
            return 1; // All counters are equal and greater than threshold
        }
    }

    return 0; // Not all equal or not greater than threshold
}

void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    if (((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0]))) {
        printf("[S] Current sp is 0x%lx, so it is in Supervisor Mode!\r\n", (unsigned long)sp);
    } else {
        printf("[M] Current sp is 0x%lx, so it is in Machine Mode!\r\n", (unsigned long)sp);
    }
}

// M-Mode setup timer
void setup_timer_mmode(void)
{
    printf("[M] Initialize timer and start timer interrupt %d ms periodically in M-Mode\r\n", MMODE_TICK_MS);
    SysTick_Config(MMODE_TIMER_TICKS);
}

// M-Mode timer interrupt handler
// non-vector mode interrupt
void eclic_mtip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */

    int_t_cnt += 1;
    if (int_t_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_mtip_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               SYSTIMER_LEVEL, SOC_INT20_IRQn, INT20_LEVEL, int_t_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ7_PARAMS, IRQ7_EXPECTED_RESULT, "eclic_mtip_handler", MMODE_IRQNUM(SOC_INT20_IRQn));

    // Reload Timer Interrupt
    SysTick_Reload(MMODE_TIMER_TICKS);

    // trigger software interrupt
    SysTimer_SetSWIRQ();

    // Mark mtip handler done
    if (int_t_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_mtip_handler - non-vector (level %u) run %u times done!\r\n", SYSTIMER_LEVEL, int_t_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[10] = int_t_cnt;
}

// M-Mode software interrupt handler
// vector mode interrupt
__INTERRUPT void eclic_msip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    int_sw_cnt += 1;
    SysTimer_ClearSWIRQ();
    if (int_sw_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_msip_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               SYSTIMER_LEVEL, SOC_INT26_IRQn, INT26_LEVEL, int_sw_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ3_PARAMS, IRQ3_EXPECTED_RESULT, "eclic_msip_handler", MMODE_IRQNUM(SOC_INT26_IRQn));
    SysTimer_ClearSWIRQ();

    // Mark msip handler done
    if (int_sw_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_msip_handler - vector (level %u) run %u times done!\r\n", SYSTIMER_SW_LEVEL, int_sw_cnt);
    }

    // Update global counter
    mmode_eclic_int_cnt[11] = int_sw_cnt;
    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

// M-Mode external interrupt handlers for SOC_INT20_IRQn to SOC_INT25_IRQn (non-vector) and SOC_INT26_IRQn to SOC_INT29_IRQn (vector)
void eclic_int20_handler(void)
{
    static uint32_t int_ext20_cnt = 0;    /* external interrupt 20 counter */

    int_ext20_cnt += 1;
    if (int_ext20_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int20_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT20_LEVEL, SOC_INT21_IRQn, INT21_LEVEL, int_ext20_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ20_PARAMS, IRQ20_EXPECTED_RESULT, "eclic_int20_handler", MMODE_IRQNUM(SOC_INT21_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT20_IRQn);

    // Mark int20 handler done
    if (int_ext20_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int20_handler - non-vector (level %u) run %u times done!\r\n", INT20_LEVEL, int_ext20_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[0] = int_ext20_cnt;
}

void eclic_int21_handler(void)
{
    static uint32_t int_ext21_cnt = 0;    /* external interrupt 21 counter */

    int_ext21_cnt += 1;
    if (int_ext21_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int21_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT21_LEVEL, SOC_INT22_IRQn, INT22_LEVEL, int_ext21_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ21_PARAMS, IRQ21_EXPECTED_RESULT, "eclic_int21_handler", MMODE_IRQNUM(SOC_INT22_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT21_IRQn);

    // Mark int21 handler done
    if (int_ext21_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int21_handler - non-vector (level %u) run %u times done!\r\n", INT21_LEVEL, int_ext21_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[1] = int_ext21_cnt;
}

void eclic_int22_handler(void)
{
    static uint32_t int_ext22_cnt = 0;    /* external interrupt 22 counter */

    int_ext22_cnt += 1;
    if (int_ext22_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int22_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT22_LEVEL, SOC_INT23_IRQn, INT23_LEVEL, int_ext22_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ22_PARAMS, IRQ22_EXPECTED_RESULT, "eclic_int22_handler", MMODE_IRQNUM(SOC_INT23_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT22_IRQn);

    // Mark int22 handler done
    if (int_ext22_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int22_handler - non-vector (level %u) run %u times done!\r\n", INT22_LEVEL, int_ext22_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[2] = int_ext22_cnt;
}

void eclic_int23_handler(void)
{
    static uint32_t int_ext23_cnt = 0;    /* external interrupt 23 counter */

    int_ext23_cnt += 1;
    if (int_ext23_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int23_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT23_LEVEL, SOC_INT24_IRQn, INT24_LEVEL, int_ext23_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ23_PARAMS, IRQ23_EXPECTED_RESULT, "eclic_int23_handler", MMODE_IRQNUM(SOC_INT24_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT23_IRQn);

    // Mark int23 handler done
    if (int_ext23_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int23_handler - non-vector (level %u) run %u times done!\r\n", INT23_LEVEL, int_ext23_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[3] = int_ext23_cnt;
}

void eclic_int24_handler(void)
{
    static uint32_t int_ext24_cnt = 0;    /* external interrupt 24 counter */

    int_ext24_cnt += 1;
    if (int_ext24_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int24_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT24_LEVEL, SOC_INT25_IRQn, INT25_LEVEL, int_ext24_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ24_PARAMS, IRQ24_EXPECTED_RESULT, "eclic_int24_handler", MMODE_IRQNUM(SOC_INT25_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT24_IRQn);

    // Mark int24 handler done
    if (int_ext24_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int24_handler - non-vector (level %u) run %u times done!\r\n", INT24_LEVEL, int_ext24_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[4] = int_ext24_cnt;
}

void eclic_int25_handler(void)
{
    static uint32_t int_ext25_cnt = 0;    /* external interrupt 25 counter */

    int_ext25_cnt += 1;
    if (int_ext25_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int25_handler (level %u), run counter: %u, msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT25_LEVEL, int_ext25_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ25_PARAMS, IRQ25_EXPECTED_RESULT, "eclic_int25_handler", MMODE_IRQNUM(0)); // No next interrupt in chain
    ECLIC_ClearPendingIRQ(SOC_INT25_IRQn);

    // Mark int25 handler done
    if (int_ext25_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int25_handler - non-vector (level %u) run %u times done!\r\n", INT25_LEVEL, int_ext25_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[5] = int_ext25_cnt;
}

__INTERRUPT void eclic_int26_handler(void)
{
    static uint32_t int_ext26_cnt = 0;   /* external interrupt 26 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    int_ext26_cnt += 1;
    if (int_ext26_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int26_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT26_LEVEL, SOC_INT27_IRQn, INT27_LEVEL, int_ext26_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ26_PARAMS, IRQ26_EXPECTED_RESULT, "eclic_int26_handler", MMODE_IRQNUM(SOC_INT27_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT26_IRQn);

    // Mark int26 handler done
    if (int_ext26_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int26_handler - vector (level %u) run %u times done!\r\n", INT26_LEVEL, int_ext26_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[6] = int_ext26_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

__INTERRUPT void eclic_int27_handler(void)
{
    static uint32_t int_ext27_cnt = 0;   /* external interrupt 27 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    int_ext27_cnt += 1;
    if (int_ext27_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int27_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT27_LEVEL, SOC_INT28_IRQn, INT28_LEVEL, int_ext27_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ27_PARAMS, IRQ27_EXPECTED_RESULT, "eclic_int27_handler", MMODE_IRQNUM(SOC_INT28_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT27_IRQn);

    // Mark int27 handler done
    if (int_ext27_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int27_handler - vector (level %u) run %u times done!\r\n", INT27_LEVEL, int_ext27_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[7] = int_ext27_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

__INTERRUPT void eclic_int28_handler(void)
{
    static uint32_t int_ext28_cnt = 0;   /* external interrupt 28 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    int_ext28_cnt += 1;
    if (int_ext28_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int28_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT28_LEVEL, SOC_INT29_IRQn, INT29_LEVEL, int_ext28_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ28_PARAMS, IRQ28_EXPECTED_RESULT, "eclic_int28_handler", MMODE_IRQNUM(SOC_INT29_IRQn));
    ECLIC_ClearPendingIRQ(SOC_INT28_IRQn);

    // Mark int28 handler done
    if (int_ext28_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int28_handler - vector (level %u) run %u times done!\r\n", INT28_LEVEL, int_ext28_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[8] = int_ext28_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

__INTERRUPT void eclic_int29_handler(void)
{
    static uint32_t int_ext29_cnt = 0;   /* external interrupt 29 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    int_ext29_cnt += 1;
    if (int_ext29_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int29_handler (level %u), run counter: %u, msubm 0x%x, mcause 0x%x, mintstatus 0x%x\r\n",
               INT29_LEVEL, int_ext29_cnt, __RV_CSR_READ(CSR_MSUBM), __RV_CSR_READ(CSR_MCAUSE), __RV_CSR_READ(CSR_MINTSTATUS));
    }
    perform_computation(IRQ29_PARAMS, IRQ29_EXPECTED_RESULT, "eclic_int29_handler", MMODE_IRQNUM(0)); // No next interrupt in chain
    ECLIC_ClearPendingIRQ(SOC_INT29_IRQn);

    // Mark int29 handler done
    if (int_ext29_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[M] eclic_int29_handler - vector (level %u) run %u times done!\r\n", INT29_LEVEL, int_ext29_cnt);
    }
    // Update global counter
    mmode_eclic_int_cnt[9] = int_ext29_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)

// S-Mode setup timer
void setup_timer_smode(void)
{
#if defined(__SSTC_PRESENT) && (__SSTC_PRESENT == 1)
    printf("[S] Initialize timer and start timer interrupt %d ms periodically in S-Mode\r\n", SMODE_TICK_MS);
    SysTick_Config_S(SMODE_TIMER_TICKS);
#endif
}

#if defined(__SSTC_PRESENT) && (__SSTC_PRESENT == 1)
// S-Mode timer interrupt handler
// non-vector mode interrupt
void eclic_stip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */

    int_t_cnt += 1;
    if (int_t_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_stip_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               SYSTIMER_S_LEVEL, SOC_INT30_IRQn, INT30_LEVEL, int_t_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ5_PARAMS, IRQ5_EXPECTED_RESULT, "eclic_stip_handler", SMODE_IRQNUM(SOC_INT30_IRQn));

    // Reload Timer Interrupt
    SysTick_Reload_S(SMODE_TIMER_TICKS);
    // trigger software interrupt
    SysTimer_SetSWIRQ_S();

    // Mark stip handler done
    if (int_t_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_stip_handler - non-vector (level %u) run %u times done!\r\n", SYSTIMER_S_LEVEL, int_t_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[10] = int_t_cnt;
}

// S-Mode software interrupt handler
// vector mode interrupt
__SUPERVISOR_INTERRUPT void eclic_ssip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    int_sw_cnt += 1;
    if (int_sw_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_ssip_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               SYSTIMER_S_LEVEL, SOC_INT36_IRQn, INT36_LEVEL, int_sw_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ1_PARAMS, IRQ1_EXPECTED_RESULT, "eclic_ssip_handler", SMODE_IRQNUM(SOC_INT36_IRQn));
    SysTimer_ClearSWIRQ_S();

    // Mark ssip handler done
    if (int_sw_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_ssip_handler - vector (level %u) run %u times done!\r\n", SYSTIMER_SW_S_LEVEL, int_sw_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[11] = int_sw_cnt;
    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}
#endif

// S-Mode external interrupt handlers for SOC_INT30_IRQn and SOC_INT39_IRQn
void eclic_int30_handler(void)
{
    static uint32_t int_ext30_cnt = 0;   /* external interrupt 30 counter */

    int_ext30_cnt += 1;
    if (int_ext30_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int30_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT30_LEVEL, SOC_INT31_IRQn, INT31_LEVEL, int_ext30_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ30_PARAMS, IRQ30_EXPECTED_RESULT, "eclic_int30_handler", SMODE_IRQNUM(SOC_INT31_IRQn));
    ECLIC_ClearPendingIRQ_S(SOC_INT30_IRQn);

    // Mark int30 handler done
    if (int_ext30_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int30_handler - non-vector (level %u) run %u times done!\r\n", INT30_LEVEL, int_ext30_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[0] = int_ext30_cnt;
}

void eclic_int31_handler(void)
{
    static uint32_t int_ext31_cnt = 0;   /* external interrupt 31 counter */

    int_ext31_cnt += 1;
    if (int_ext31_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int31_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT31_LEVEL, SOC_INT32_IRQn, INT32_LEVEL, int_ext31_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ31_PARAMS, IRQ31_EXPECTED_RESULT, "eclic_int31_handler", SMODE_IRQNUM(SOC_INT32_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT31_IRQn);

    // Mark int31 handler done
    if (int_ext31_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int31_handler - non-vector (level %u) run %u times done!\r\n", INT31_LEVEL, int_ext31_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[1] = int_ext31_cnt;

}

void eclic_int32_handler(void)
{
    static uint32_t int_ext32_cnt = 0;   /* external interrupt 32 counter */

    int_ext32_cnt += 1;
    if (int_ext32_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int32_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT32_LEVEL, SOC_INT32_IRQn, INT32_LEVEL, int_ext32_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ32_PARAMS, IRQ32_EXPECTED_RESULT, "eclic_int32_handler", SMODE_IRQNUM(SOC_INT33_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT32_IRQn);

    // Mark int32 handler done
    if (int_ext32_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int32_handler - non-vector (level %u) run %u times done!\r\n", INT32_LEVEL, int_ext32_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[2] = int_ext32_cnt;

}

void eclic_int33_handler(void)
{
    static uint32_t int_ext33_cnt = 0;   /* external interrupt 33 counter */

    int_ext33_cnt += 1;
    if (int_ext33_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int33_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT33_LEVEL, SOC_INT34_IRQn, INT34_LEVEL, int_ext33_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ33_PARAMS, IRQ33_EXPECTED_RESULT, "eclic_int33_handler", SMODE_IRQNUM(SOC_INT34_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT33_IRQn);

    // Mark int33 handler done
    if (int_ext33_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int33_handler - non-vector (level %u) run %u times done!\r\n", INT33_LEVEL, int_ext33_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[3] = int_ext33_cnt;
}

void eclic_int34_handler(void)
{
    static uint32_t int_ext34_cnt = 0;   /* external interrupt 34 counter */

    int_ext34_cnt += 1;
    if (int_ext34_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int34_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT34_LEVEL, SOC_INT35_IRQn, INT35_LEVEL, int_ext34_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ34_PARAMS, IRQ34_EXPECTED_RESULT, "eclic_int34_handler", SMODE_IRQNUM(SOC_INT35_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT34_IRQn);

    // Mark int34 handler done
    if (int_ext34_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int34_handler - non-vector (level %u) run %u times done!\r\n", INT34_LEVEL, int_ext34_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[4] = int_ext34_cnt;
}

void eclic_int35_handler(void)
{
    static uint32_t int_ext35_cnt = 0;   /* external interrupt 35 counter */

    int_ext35_cnt += 1;
    if (int_ext35_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int35_handler (level %u), run counter: %u, ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT35_LEVEL, int_ext35_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ35_PARAMS, IRQ35_EXPECTED_RESULT, "eclic_int35_handler", SMODE_IRQNUM(0)); // No next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT35_IRQn);

    // Mark int35 handler done
    if (int_ext35_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int35_handler - non-vector (level %u) run %u times done!\r\n", INT35_LEVEL, int_ext35_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[5] = int_ext35_cnt;
}

__SUPERVISOR_INTERRUPT void eclic_int36_handler(void)
{
    static uint32_t int_ext36_cnt = 0;   /* external interrupt 36 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    int_ext36_cnt += 1;
    if (int_ext36_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int36_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT36_LEVEL, SOC_INT37_IRQn, INT37_LEVEL, int_ext36_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ36_PARAMS, IRQ36_EXPECTED_RESULT, "eclic_int36_handler", SMODE_IRQNUM(SOC_INT37_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT36_IRQn);

    // Mark int36 handler done
    if (int_ext36_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int36_handler - vector (level %u) run %u times done!\r\n", INT36_LEVEL, int_ext36_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[6] = int_ext36_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}

__SUPERVISOR_INTERRUPT void eclic_int37_handler(void)
{
    static uint32_t int_ext37_cnt = 0;   /* external interrupt 37 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    int_ext37_cnt += 1;
    if (int_ext37_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int37_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT37_LEVEL, SOC_INT38_IRQn, INT38_LEVEL, int_ext37_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ37_PARAMS, IRQ37_EXPECTED_RESULT, "eclic_int37_handler", SMODE_IRQNUM(SOC_INT38_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT37_IRQn);

    // Mark int37 handler done
    if (int_ext37_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int37_handler - vector (level %u) run %u times done!\r\n", INT37_LEVEL, int_ext37_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[7] = int_ext37_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}

__SUPERVISOR_INTERRUPT void eclic_int38_handler(void)
{
    static uint32_t int_ext38_cnt = 0;   /* external interrupt 38 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    int_ext38_cnt += 1;
    if (int_ext38_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int38_handler (level %u) triggered IRQ %d (level %u, trigger counter: %u), ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT38_LEVEL, SOC_INT39_IRQn, INT39_LEVEL, int_ext38_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ38_PARAMS, IRQ38_EXPECTED_RESULT, "eclic_int38_handler", SMODE_IRQNUM(SOC_INT39_IRQn)); // Trigger next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT38_IRQn);

    // Mark int38 handler done
    if (int_ext38_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int38_handler - vector (level %u) run %u times done!\r\n", INT38_LEVEL, int_ext38_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[8] = int_ext38_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}

__SUPERVISOR_INTERRUPT void eclic_int39_handler(void)
{
    static uint32_t int_ext39_cnt = 0;   /* external interrupt 39 counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    int_ext39_cnt += 1;
    if (int_ext39_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int39_handler (level %u), run counter: %u, ssubm 0x%x, scause 0x%x, sintstatus 0x%x\r\n",
               INT39_LEVEL, int_ext39_cnt, READ_SSUBM(), __RV_CSR_READ(CSR_SCAUSE), __RV_CSR_READ(CSR_SINTSTATUS));
    }
    perform_computation(IRQ39_PARAMS, IRQ39_EXPECTED_RESULT, "eclic_int39_handler", SMODE_IRQNUM(0)); // No next interrupt in chain
    ECLIC_ClearPendingIRQ_S(SOC_INT39_IRQn);

    // Mark int39 handler done
    if (int_ext39_cnt % PRINT_LOOP_COUNT == 0) {
        printf("[S] eclic_int39_handler - vector (level %u) run %u times done!\r\n", INT39_LEVEL, int_ext39_cnt);
    }
    // Update global counter
    smode_eclic_int_cnt[9] = int_ext39_cnt;

    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}

// S-Mode background computation function using S_BG_PARAMS
void smode_background_computation(void)
{
    static volatile uint32_t bg_counter = 0;

    // Perform computation using S_BG_PARAMS
    volatile uint32_t result = perform_computation(S_BG_PARAMS, S_BG_EXPECTED_RESULT, "smode_background_computation", SMODE_IRQNUM(0));

    bg_counter++;
}

// S-Mode entry point
static void supervisor_mode_entry_point(void)
{
    // setup timer and software interrupt , then register the S mode irq
    int32_t returnCode = 0;

    printf("[S] Hello Supervisor Mode!!!\r\n");
    print_sp_judge_privilege_mode();

    // initialize timer
    setup_timer_smode();

    uint32_t nlbits = ECLIC_GetCfgNlbits();

    // NOTE: nlbits determines the number of bits used for interrupt level encoding
    // Valid interrupt levels are from 0 to (1<<nlbits)-1
    // For example, if nlbits=3, valid levels are 0-7 (0 to 7)
    printf("[S] ECLIC nlbits configuration: %u\r\n", nlbits);
#if __ECLIC_VER == 2
    /*
     * TODO: Shadow Level configuration - can be adjusted for different functionality testing
     * The first parameter refers to the shadow register index (SHAD1_CFG, SHAD2_CFG, etc.)
     * The second parameter sets the interrupt level threshold for shadow register group
     */
    uint32_t shadow_num = ECLIC_GetInfoShadowNum();
   if (shadow_num > 0) {
        ECLIC_SetShadowLevel_S(0, 3);
        printf("[S] ECLIC Shadow Register Groups: %u\r\n", shadow_num);
        uint64_t shadow_reg_val = ECLIC_GetShadowLevelReg_S();
        printf("[S] Supervisor Shadow Level Register: 0x%08x%08x\r\n", (uint32_t)(shadow_reg_val >> 32), (uint32_t)shadow_reg_val);
   }
#endif

#if defined(__SSTC_PRESENT) && (__SSTC_PRESENT == 1)
    // initialize software interrupt as vector interrupt
    returnCode = ECLIC_Register_IRQ_S(SysTimerSW_S_IRQn, ECLIC_VECTOR_INTERRUPT,
                                      ECLIC_LEVEL_TRIGGER, SYSTIMER_SW_S_LEVEL, 0, (void *)eclic_ssip_handler);

    // initial timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ_S(SysTimer_S_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_LEVEL_TRIGGER, SYSTIMER_S_LEVEL, 0, (void *)eclic_stip_handler);
#endif

    // Register SOC_INT30_IRQn to SOC_INT39_IRQn to S-Mode
    returnCode = ECLIC_Register_IRQ_S(SOC_INT30_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT30_LEVEL, 0, (void *)eclic_int30_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT31_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT31_LEVEL, 0, (void *)eclic_int31_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT32_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT32_LEVEL, 0, (void *)eclic_int32_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT33_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT33_LEVEL, 0, (void *)eclic_int33_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT34_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT34_LEVEL, 0, (void *)eclic_int34_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT35_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT35_LEVEL, 0, (void *)eclic_int35_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT36_IRQn, ECLIC_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT36_LEVEL, 0, (void *)eclic_int36_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT37_IRQn, ECLIC_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT37_LEVEL, 0, (void *)eclic_int37_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT38_IRQn, ECLIC_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT38_LEVEL, 0, (void *)eclic_int38_handler);
    returnCode = ECLIC_Register_IRQ_S(SOC_INT39_IRQn, ECLIC_VECTOR_INTERRUPT,
                                      ECLIC_POSTIVE_EDGE_TRIGGER, INT39_LEVEL, 0, (void *)eclic_int39_handler);

#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2) && defined(INTSTACK)
    printf("[S] Use separated interrupt stack for s-mode interrupt at 0x%x\r\n", (unsigned long)smode_int_sp);
    __RV_CSR_WRITE(CSR_STSP, (unsigned long)smode_int_sp);
    __RV_CSR_SET(CSR_SECLIC_CTL, SECLIC_CTL_TSP_EN);
#endif
    // Enable s-mode interrupts in general.
    __enable_irq_s();

    unsigned long threshold = PRINT_LOOP_COUNT;
    while (1) {
        // Perform S-mode background computation
        smode_background_computation();

        // Check if both M-mode and S-mode conditions are satisfied and test hasn't passed yet
        if (check_interrupt_counters((volatile uint32_t*)smode_eclic_int_cnt,  threshold) &&
            check_interrupt_counters((volatile uint32_t*)mmode_eclic_int_cnt, threshold)) {
            test_passed += 1;
            printf("[S] PASS: All smode_eclic_int_cnt and mmode_eclic_int_cnt values are equal and greater than %u\r\n", threshold);
            threshold += PRINT_LOOP_COUNT;
#ifdef CFG_SIMULATION
            // directly exit with ok if in nuclei internally simulation
            SIMULATION_EXIT(0);
#endif
        }
    }
}
#endif

// Function to consolidate M-Mode initialization operations
int initialize_mmode_demo(void)
{
    int32_t returnCode;

    uint32_t nlbits = ECLIC_GetCfgNlbits();

    // NOTE: nlbits determines the number of bits used for interrupt level encoding
    // Valid interrupt levels are from 0 to (1<<nlbits)-1
    // For example, if nlbits=3, valid levels are 0-7 (0 to 7)
    printf("[M] ECLIC nlbits configuration: %u\r\n", nlbits);

#if __ECLIC_VER == 2
    /*
     * TODO: Shadow Level configuration - can be adjusted for different functionality testing
     * The first parameter refers to the shadow register index (SHAD1_CFG, SHAD2_CFG, etc.)
     * The second parameter sets the interrupt level threshold for shadow register group
     */
    uint32_t shadow_num = ECLIC_GetInfoShadowNum();
   if (shadow_num > 0) {
        ECLIC_SetShadowLevel(0, 2);
        printf("[M] ECLIC Shadow Register Groups: %u\r\n", shadow_num);
        uint64_t shadow_reg_val = ECLIC_GetShadowLevelReg();
        printf("[M] Machine Shadow Level Register: 0x%08x%08x\r\n", (uint32_t)(shadow_reg_val >> 32), (uint32_t)shadow_reg_val);
   }
#endif

    // Initialize and start M-Mode timer - moved to end of main function
    setup_timer_mmode();

    // initialize software interrupt as vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_LEVEL_TRIGGER, SYSTIMER_SW_LEVEL, 0, (void*)eclic_msip_handler);

    // initial timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_LEVEL_TRIGGER, SYSTIMER_LEVEL, 0, (void*)eclic_mtip_handler);

    // Register SOC_INT20_IRQn to SOC_INT25_IRQn as NON-VECTOR interrupts (20-25)
    returnCode = ECLIC_Register_IRQ(SOC_INT20_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT20_LEVEL, 0, (void*)eclic_int20_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT21_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT21_LEVEL, 0, (void*)eclic_int21_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT22_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT22_LEVEL, 0, (void*)eclic_int22_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT23_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT23_LEVEL, 0, (void*)eclic_int23_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT24_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT24_LEVEL, 0, (void*)eclic_int24_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT25_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT25_LEVEL, 0, (void*)eclic_int25_handler);

    // Register SOC_INT26_IRQn to SOC_INT29_IRQn as VECTOR interrupts (26-29)
    returnCode = ECLIC_Register_IRQ(SOC_INT26_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT26_LEVEL, 0, (void*)eclic_int26_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT27_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT27_LEVEL, 0, (void*)eclic_int27_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT28_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT28_LEVEL, 0, (void*)eclic_int28_handler);
    returnCode = ECLIC_Register_IRQ(SOC_INT29_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, INT29_LEVEL, 0, (void*)eclic_int29_handler);

#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2) && defined(INTSTACK)
    printf("[M] Use separated interrupt stack for m-mode interrupt at 0x%x\r\n", (unsigned long)mmode_int_sp);
    __RV_CSR_WRITE(CSR_MTSP, (unsigned long)mmode_int_sp);
    __RV_CSR_SET(CSR_MECLIC_CTL, MECLIC_CTL_TSP_EN);
#endif

    return 0; // Success
}

// M-Mode background computation function using M_BG_PARAMS
void mmode_background_computation(void)
{
    static volatile uint32_t bg_counter = 0;

    // Perform computation using M_BG_PARAMS
    volatile uint32_t result = perform_computation(M_BG_PARAMS, M_BG_EXPECTED_RESULT, "mmode_background_computation", MMODE_IRQNUM(0));

    bg_counter++;
}

int main(int argc, char** argv)
{
    CSR_MCFGINFO_Type mcfg_info;

#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg_info.b.clic = 1;
#else
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
#endif

    if (0 == mcfg_info.b.clic) {
        printf("ECLIC is not present, will not run this example!\r\n");
        return 0;
    }

    printf("[M] Starting ECLIC Stress Test - M-Mode and S-Mode Integration\r\n");

    /* Calculate expected computation results */
    printf("[M] Calculating expected computation results...\r\n");
    uint32_t result_irq3 = perform_computation(IRQ3_PARAMS, 0, "main calculation - irq3", 0);
    uint32_t result_irq7 = perform_computation(IRQ7_PARAMS, 0, "main calculation - irq7", 0);
    uint32_t result_irq20 = perform_computation(IRQ20_PARAMS, 0, "main calculation - irq20", 0);
    uint32_t result_irq21 = perform_computation(IRQ21_PARAMS, 0, "main calculation - irq21", 0);
    uint32_t result_irq22 = perform_computation(IRQ22_PARAMS, 0, "main calculation - irq22", 0);
    uint32_t result_irq23 = perform_computation(IRQ23_PARAMS, 0, "main calculation - irq23", 0);
    uint32_t result_irq24 = perform_computation(IRQ24_PARAMS, 0, "main calculation - irq24", 0);
    uint32_t result_irq25 = perform_computation(IRQ25_PARAMS, 0, "main calculation - irq25", 0);
    uint32_t result_irq26 = perform_computation(IRQ26_PARAMS, 0, "main calculation - irq26", 0);
    uint32_t result_irq27 = perform_computation(IRQ27_PARAMS, 0, "main calculation - irq27", 0);
    uint32_t result_irq28 = perform_computation(IRQ28_PARAMS, 0, "main calculation - irq28", 0);
    uint32_t result_irq29 = perform_computation(IRQ29_PARAMS, 0, "main calculation - irq29", 0);
    uint32_t result_irq30 = perform_computation(IRQ30_PARAMS, 0, "main calculation - irq30", 0);
    uint32_t result_irq31 = perform_computation(IRQ31_PARAMS, 0, "main calculation - irq31", 0);
    uint32_t result_irq32 = perform_computation(IRQ32_PARAMS, 0, "main calculation - irq32", 0);
    uint32_t result_irq33 = perform_computation(IRQ33_PARAMS, 0, "main calculation - irq33", 0);
    uint32_t result_irq34 = perform_computation(IRQ34_PARAMS, 0, "main calculation - irq34", 0);
    uint32_t result_irq35 = perform_computation(IRQ35_PARAMS, 0, "main calculation - irq35", 0);
    uint32_t result_irq36 = perform_computation(IRQ36_PARAMS, 0, "main calculation - irq36", 0);
    uint32_t result_irq37 = perform_computation(IRQ37_PARAMS, 0, "main calculation - irq37", 0);
    uint32_t result_irq38 = perform_computation(IRQ38_PARAMS, 0, "main calculation - irq38", 0);
    uint32_t result_irq39 = perform_computation(IRQ39_PARAMS, 0, "main calculation - irq39", 0);
    uint32_t result_irq1 = perform_computation(IRQ1_PARAMS, 0, "main calculation - irq1", 0);
    uint32_t result_irq5 = perform_computation(IRQ5_PARAMS, 0, "main calculation - irq5", 0);
    uint32_t result_m_bg = perform_computation(M_BG_PARAMS, 0, "main calculation - m_bg_params", 0);
    uint32_t result_s_bg = perform_computation(S_BG_PARAMS, 0, "main calculation - s_bg_params", 0);

    // Note: In a real scenario, we would update the #define values, but since they're compile-time constants,
    // we're showing what the calculated values should be for reference
    printf("[M] Expected results for interrupt handlers:\r\n");
    printf("[M] eclic_irq3_int_handler: %u\r\n", result_irq3);
    printf("[M] eclic_irq7_int_handler: %u\r\n", result_irq7);
    printf("[M] eclic_irq20_int_handler: %u\r\n", result_irq20);
    printf("[M] eclic_irq21_int_handler: %u\r\n", result_irq21);
    printf("[M] eclic_irq22_int_handler: %u\r\n", result_irq22);
    printf("[M] eclic_irq23_int_handler: %u\r\n", result_irq23);
    printf("[M] eclic_irq24_int_handler: %u\r\n", result_irq24);
    printf("[M] eclic_irq25_int_handler: %u\r\n", result_irq25);
    printf("[M] eclic_irq26_int_handler: %u\r\n", result_irq26);
    printf("[M] eclic_irq27_int_handler: %u\r\n", result_irq27);
    printf("[M] eclic_irq28_int_handler: %u\r\n", result_irq28);
    printf("[M] eclic_irq29_int_handler: %u\r\n", result_irq29);
    printf("[M] eclic_irq30_int_handler: %u\r\n", result_irq30);
    printf("[M] eclic_irq31_int_handler: %u\r\n", result_irq31);
    printf("[M] eclic_irq32_int_handler: %u\r\n", result_irq32);
    printf("[M] eclic_irq33_int_handler: %u\r\n", result_irq33);
    printf("[M] eclic_irq34_int_handler: %u\r\n", result_irq34);
    printf("[M] eclic_irq35_int_handler: %u\r\n", result_irq35);
    printf("[M] eclic_irq36_int_handler: %u\r\n", result_irq36);
    printf("[M] eclic_irq37_int_handler: %u\r\n", result_irq37);
    printf("[M] eclic_irq38_int_handler: %u\r\n", result_irq38);
    printf("[M] eclic_irq39_int_handler: %u\r\n", result_irq39);
    printf("[M] eclic_irq1_int_handler: %u\r\n", result_irq1);
    printf("[M] eclic_irq5_int_handler: %u\r\n", result_irq5);
    // Update the expected results for background computations
    printf("[M] m-mode background computations: %u\r\n", result_m_bg);
    printf("[M] s-mode background computations: %u\r\n", result_s_bg);
    printf("[M] Calculation completed! Use these values to update the interrupt handlers if needed.\r\n");

    // Initialize M-Mode interrupts
    initialize_mmode_demo();

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    printf("[M] S-Mode is present, will run both S-Mode and M-Mode demos\r\n");

    // Configure PMP to allow S-Mode access to memory
#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
    // set pmp, S mode can access all address range
    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };

    __set_PMPENTRYx(0, &pmp_cfg);
    print_sp_judge_privilege_mode();
#endif

    printf("[M] Configuring SysTimerSW_S, SysTimer_S and SOC_INT30-39 to execute in S-Mode\r\n");

    // before drop to S Mode, specifies in which privilege mode the interrupt should be taken
    ECLIC_SetModeIRQ(SysTimerSW_S_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SysTimer_S_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT30_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT31_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT32_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT33_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT34_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT35_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT36_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT37_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT38_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SOC_INT39_IRQn, PRV_S);

#if defined(__SSTC_PRESENT) && (__SSTC_PRESENT == 1)
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_SSTC) {
        /* Disable S-mode access some system timer registers */
        SysTimer_DisableSAccess();
        /* Enable CY,TM,IR in CSR_MCOUNTEREN to allow S-mode access cycle,time,instret csr */
        SysTimer_EnableSSTC();
    } else {
        printf("[M] SSTC extension is not present, so should not run S-Mode demo!\n");
        goto m_mode_bg;
    }
#else
    printf("[M] Warning: SSTC extension is required for S-Mode Timer Interrupt, so should not run S-Mode demo!\n");
    goto m_mode_bg;
#endif
#endif
    // Enable M-Mode interrupts before jumping to S-Mode
    __enable_irq();

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    printf("[M] Dropping to S-Mode now\r\n");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
    /* If dropped to S Mode, it will not return to the following code */
#endif

m_mode_bg:
    unsigned long threshold = PRINT_LOOP_COUNT;
    printf("[M] S-Mode or SSTC not present, running M-Mode demo only\r\n");
    // Enable M-Mode interrupts if not drop to S-Mode
    __enable_irq();
    // Main loop for M-mode background computation
    while (1) {
        mmode_background_computation();
        if (check_interrupt_counters((volatile uint32_t*)mmode_eclic_int_cnt, threshold)) {
            test_passed += 1;
            printf("[M] PASS: All mmode_eclic_int_cnt values are equal and greater than %u\r\n", threshold);
            threshold += PRINT_LOOP_COUNT;
#ifdef CFG_SIMULATION
            // directly exit with ok if in nuclei internally simulation
            SIMULATION_EXIT(0);
#endif
        }
    }

    return 0;
}
