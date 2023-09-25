// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

// Reserve 0x200 bytes for exception stack push/pop
#ifndef __ICCRISCV__
extern char __StackTop[];
extern char __StackBottom[];
#define STACK_TOP                   (unsigned long)(__StackTop)
#define STACK_BOTTOM                ((unsigned long)(__StackBottom) + 0x200)
#define STACK_SIZE                  ((STACK_TOP) - (STACK_BOTTOM))
#else
extern char CSTACK$$Base[];
extern char CSTACK$$Limit[];
#define STACK_TOP                   (unsigned long)(CSTACK$$Limit)
#define STACK_BOTTOM                ((unsigned long)(CSTACK$$Base) + 0x200)
#define STACK_SIZE                  ((STACK_TOP) - (STACK_BOTTOM))
#endif

#define STACK_CHECK_OVF_UDF_MODE    0
#define STACK_TRACK_MODE            1
#define STACK_CHECK_DIS_MODE        0xFF

__STATIC_FORCEINLINE void set_stack_check_mode(uint8_t mode);

static unsigned long stack_check_detected = 0;
static unsigned long factorial_iter = 0;

static void stack_corrupt_exeception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    // Note that the sp has grown downwardly 0x50 bytes in the exeception entry saving context
    // In this demo, add sp by 0x50 is the sp value that triggered overflow/underflow
    set_stack_check_mode(STACK_CHECK_DIS_MODE);
    switch (mcause & MCAUSE_CAUSE) {
        case StackOverflow_EXCn:
            stack_check_detected = 1;
            printf("Stack overflow fault occurs at iteration %ld, cause: 0x%lx, epc: 0x%lx, sp: 0x%lx\r\n", factorial_iter, exc_frame->cause, exc_frame->epc, sp);
            break;
        case StackUnderflow_EXCn:
            stack_check_detected = 2;
            printf("Stack underflow fault occurs at iteration %ld, cause: 0x%lx, epc: 0x%lx, sp: 0x%lx\r\n", factorial_iter, exc_frame->cause, exc_frame->epc, sp);
            break;
        default: break;
    }
    factorial_iter = 0;
    // Comment it on purpose, continue to excute
    // while(1);
}

// Must use __STATIC_FORCEINLINE, to avoid stack use in underflow check,
// or else it will cause underflow itself after decreasing the stack base value
__STATIC_FORCEINLINE void set_stack_check_mode(uint8_t mode)
{
    if (STACK_TRACK_MODE == mode) {
        // Track the stack top mode
        __RV_CSR_SET(CSR_MSTACK_CTRL, MSTACK_CTRL_MODE);
        __RV_CSR_SET(CSR_MSTACK_CTRL, MSTACK_CTRL_OVF_TRACK_EN);
    } else if (STACK_CHECK_OVF_UDF_MODE == mode) {
        // Overflow and Underflow check (Default) mode
        __RV_CSR_CLEAR(CSR_MSTACK_CTRL, MSTACK_CTRL_MODE);
        // Enable underflow and overflow check
        __RV_CSR_SET(CSR_MSTACK_CTRL, MSTACK_CTRL_UDF_EN | MSTACK_CTRL_OVF_TRACK_EN);
    } else {
        __RV_CSR_CLEAR(CSR_MSTACK_CTRL, 0xFF);
    }
}

// User should set the mstack_bound register before mstack_ctrl to check stack overflow.
__STATIC_FORCEINLINE void set_stack_bound(unsigned long bound)
{
    __RV_CSR_WRITE(CSR_MSTACK_BOUND, bound);
    printf("BOUND register set to: 0x%lx\r\n", (unsigned long)__RV_CSR_READ(CSR_MSTACK_BOUND));
}

// User should set the mstack_base register before mstack_ctrl to check stack underflow.
__STATIC_FORCEINLINE void set_stack_base(unsigned long base)
{
    __RV_CSR_WRITE(CSR_MSTACK_BASE, base);
    printf("BASE register set to: 0x%lx\r\n", (unsigned long)__RV_CSR_READ(CSR_MSTACK_BASE));
}

// A simple recursive function of calculating factorial
// It will casue the stack to grow downwards, maybe overflow if only n is big enough
static long factorial(int n, int log_en, int change_base)
{
    static int iter_cnt = 0;

    factorial_iter = (unsigned long)n;
    if (1 == log_en) {
        // In stack track mode, BOUND register will update to sp value
        printf("Iterations: %d, stack bound: 0x%lx\n", ++iter_cnt, (unsigned long)__RV_CSR_READ(CSR_MSTACK_BOUND));
    }
    if ((1 == n) || stack_check_detected != 0) {
        if (1 == change_base) {
            // Decrease the stack base value to make the underflow condition on purpose
            set_stack_base((unsigned long)STACK_TOP - ((unsigned long)(STACK_SIZE) >> 1));
            set_stack_check_mode(STACK_CHECK_OVF_UDF_MODE);
        }
        iter_cnt = 0;
        stack_check_detected = 0;
        return 1;
    } else {
        return factorial(n - 1, log_en, change_base) * n;
    }
}

int main(void)
{
    unsigned long stack_bound = 0;
    unsigned long stack_base = 0;

    /* In runtime, when download mode is ILM in evalsoc, stack top value(high address) is 0x90010000,
       stack bottom value(low address) is 0x9000f800 by default when using gnu gcc */
    printf("Stack's top high address: 0x%lx, stack's bottom low address: 0x%lx, stack size: 0x%lx\n", STACK_TOP, STACK_BOTTOM, STACK_SIZE);

    /* register corresponding exception */
    Exception_Register_EXC(StackOverflow_EXCn, (unsigned long)stack_corrupt_exeception_handler);
    Exception_Register_EXC(StackUnderflow_EXCn, (unsigned long)stack_corrupt_exeception_handler);

    printf("\n--------OVERFLOW CHECK MODE--------\r\n");
    /* set the stack bound for overflow check to the default */
    stack_bound = (unsigned long)STACK_BOTTOM;
    /* set the stack base for underflow check to the default */
    stack_base = (unsigned long)STACK_TOP;
    // Must set the BOUND and BASE before setting the check mode
    set_stack_bound(stack_bound);
    set_stack_base(stack_base);
    /* enable overflow and underflow check */
    set_stack_check_mode(STACK_CHECK_OVF_UDF_MODE);

    // In practice, n=100's factorial will trigger overflow when stack pushes
    stack_check_detected = 0;
    factorial(100, 0, 0);

    printf("\n--------UNDERFLOW CHECK MODE--------\r\n");

    // Will trigger underflow when stack pops
    stack_check_detected = 0;
    factorial(8, 0, 1);
    // Restore the BASE
    set_stack_base((unsigned long)STACK_TOP);

    printf("\n--------TRACK SP MODE--------\r\n");
    // Change to track mode first
    set_stack_check_mode(STACK_TRACK_MODE);
    // If the sp is smaller than BOUND, the BOUND's value will be updated to sp
    // So here set it to the default top high address
    set_stack_bound((unsigned long)STACK_TOP);
    // You will see the BOUND register track the sp value
    stack_check_detected = 0;
    factorial(18, 1, 0);
    printf("Calculate factorial over, the max stack used downwards to: 0x%lx\r\n", (unsigned long)__RV_CSR_READ(CSR_MSTACK_BOUND));
    printf("\nRerun it. The BOUND won't track sp if sp is bigger: \r\n");
    stack_check_detected = 0;
    factorial(5, 1, 0);
    printf("\nStack check demo over!\r\n");
    return 0;
}
