#define TX_SOURCE_CODE
#define TX_THREAD_SMP_SOURCE_CODE

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_initialize.h"

#include "nuclei_sdk_soc.h"

#ifdef TX_REGRESSION_TEST
/* External reference for regression test ISR dispatch. */
extern void test_interrupt_dispatch(void);
#endif

// SOC_TIMER_FREQ should be provided in <Device>.h of NMSIS. eg. evalsoc.h
// TX_TIMER_TICKS_PER_SECOND defined in tx_user.h which can overwrite the default one in tx_api.h if TX_INCLUDE_USER_DEFINE_FILE defined
#define SYSTICK_TICK_CONST          (SOC_TIMER_FREQ / TX_TIMER_TICKS_PER_SECOND)
#define KERNEL_INTERRUPT_PRIORITY   0

// MUST define SysTick_Handler as eclic_mtip_handler, which is registered in vector table
#define SysTick_Handler     eclic_mtip_handler

/* This is the timer interrupt service routine. */
void SysTick_Handler(void)
{
    UINT saved_posture;

    /* Get the protection.  */
    saved_posture = _tx_thread_smp_protect();

    /* Increment the system active counter.  */
    _tx_timer_interrupt_active ++;

#ifdef TX_REGRESSION_TEST
    /* Call regression test interrupt dispatch for ISR testing. */
    test_interrupt_dispatch();
#endif

    // Reload timer
    SysTick_Reload(SYSTICK_TICK_CONST);

    /* Increment system clock. */
    _tx_timer_system_clock++;

    /* Test for timer expiration.  */
    if (*_tx_timer_current_ptr) {
        /* Set expiration flag.  */
        _tx_timer_expired =  TX_TRUE;
    } else {
        /* No timer expired, increment the timer pointer.  */
        _tx_timer_current_ptr++;
        /* Check for wrap-around.  */
        if (_tx_timer_current_ptr == _tx_timer_list_end) {
            /* Wrap to beginning of list.  */
            _tx_timer_current_ptr =  _tx_timer_list_start;
        }
    }

    /* See if anything has expired.  */
    if (_tx_timer_expired) {
        /* Process timer expiration.  */
        _tx_timer_expiration_process();
    }
    /* Call time-slice processing to process time-slice for all threads on each core. */
    _tx_thread_time_slice();
    /* Decrease the system active counter.  */
    _tx_timer_interrupt_active --;
    /* Release the protection.  */
    _tx_thread_smp_unprotect(saved_posture);
}

/* Find a runnable thread for this core and optionally publish it as current. */
TX_THREAD* _tx_find_ready_thread(UINT set_current)
{
    TX_THREAD *rdy_thread;
    UINT coreid = _tx_thread_smp_core_get();

    rdy_thread = _tx_thread_execute_ptr[coreid];
    do {
        /* If no ready task just go to idle and wait for interrupt */
        while ((!rdy_thread) || (rdy_thread->tx_thread_smp_core_control != 1)) {
            __NOP(); __NOP();
            rdy_thread = _tx_thread_execute_ptr[coreid];
        }
        /* Atomically claim this ready thread so only one core can schedule it. tx_thread_smp_core_control type is ULONG */
#if __RISCV_XLEN == 32
        if (__AMOSWAP_W((volatile uint32_t *)&rdy_thread->tx_thread_smp_core_control, 0) == 1) {
#else
        if (__AMOSWAP_D((volatile uint64_t *)&rdy_thread->tx_thread_smp_core_control, 0) == 1) {
#endif
            break;
        }
    } while (1);
    if (set_current) {
        /* First-thread restore needs to populate _tx_thread_current_ptr here. */
        _tx_thread_current_ptr[coreid] = rdy_thread;
    }
    __RWMB();
    return rdy_thread;
}

// Task Switch code called in eclic_msip_handler
void PortThreadSwitch(void)
{
    TX_THREAD *rdy_thread;
    UINT coreid = _tx_thread_smp_core_get();

    if (_tx_thread_preempt_disable != 0) {
        if (_tx_thread_smp_protection.tx_thread_smp_protect_core == coreid) {
            SysTimer_ClearSWIRQ();
        }
        __RWMB();
        return;
    }

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    _tx_execution_thread_exit();
#endif

    /* Acknowledge the interrupt that brought us here before scheduling.
       Any SWI raised after this point represents a new scheduling request
       and must remain pending for the restored thread. */
    SysTimer_ClearSWIRQ();
    __RWMB();

    /*
     * Magic idle task emulation for threadx
     * ThreadX don't have idle task, so _tx_thread_execute_ptr could be NULL
     * If it is NULL, it means it should goto idle state, and wait for interrupt
     */
    /* Determine if the time-slice is active.  */
    if (_tx_timer_time_slice[coreid] && _tx_thread_current_ptr[coreid]) {
        /* Preserve current remaining time-slice for the thread and clear the current time-slice.  */
        _tx_thread_current_ptr[coreid] -> tx_thread_time_slice = _tx_timer_time_slice[coreid];
        _tx_timer_time_slice[coreid] =  0;
    }
    if (_tx_thread_current_ptr[coreid]) {
        /* Current thread context is saved, ready for scheduling */
        _tx_thread_current_ptr[coreid] -> tx_thread_smp_core_control = 1;
        _tx_thread_current_ptr[coreid] =  TX_NULL;
    }
    __RWMB();

    rdy_thread = _tx_thread_execute_ptr[coreid];
    if ((!rdy_thread) || (rdy_thread -> tx_thread_smp_core_control != 1)) {
        if (coreid == 0) {
            /* Raise the timer interrupt priority temporarily so interrupt
               nesting remains available while searching for a ready thread. */
            ECLIC_SetLevelIRQ(SysTimer_IRQn, KERNEL_INTERRUPT_PRIORITY + 1);
            __RWMB();
        }
        __RWMB();
        /* Save mcause/msubm because they may be overwritten when interrupts
           are re-enabled and nested here. */
        rv_csr_t mcause = __RV_CSR_READ(CSR_MCAUSE);
        rv_csr_t msubm = __RV_CSR_READ(CSR_MSUBM);
        __enable_irq();
        __RWMB();
        rdy_thread = _tx_find_ready_thread(TX_FALSE);
        /* Disable interrupts again after a candidate thread has been found to
           prevent further nesting during the handoff. */
        __disable_irq();
        __RWMB();
        /* Restore mcause/msubm because nested interrupts were allowed
           explicitly in the search window above. */
        __RV_CSR_WRITE(CSR_MSUBM, msubm);
        __RV_CSR_WRITE(CSR_MCAUSE, mcause);
        __RWMB();
        /* Restore the timer interrupt to its normal kernel priority. */
        if (coreid == 0) {
            ECLIC_SetLevelIRQ(SysTimer_IRQn, KERNEL_INTERRUPT_PRIORITY);
            __RWMB();
        }
    } else {
        rdy_thread = _tx_find_ready_thread(TX_FALSE);
    }
    _tx_thread_current_ptr[coreid] = rdy_thread;
    /* Clear the execution control flag.  */
    rdy_thread -> tx_thread_smp_core_control = 0;
    rdy_thread -> tx_thread_run_count ++;
    /* Setup time-slice, if present.  */
    _tx_timer_time_slice[coreid] =  rdy_thread -> tx_thread_time_slice;
    __RWMB();
}

void SetupSysTickInterrupt(void)
{
    uint64_t ticks = SYSTICK_TICK_CONST;

    if (_tx_thread_smp_core_get() == 0) {
        /* Make SWI and SysTick the lowest priority interrupts. */
        /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
        SysTick_Config(ticks);
        ECLIC_DisableIRQ(SysTimer_IRQn);
        ECLIC_SetLevelIRQ(SysTimer_IRQn, KERNEL_INTERRUPT_PRIORITY);
        ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
        ECLIC_EnableIRQ(SysTimer_IRQn);
    }

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, KERNEL_INTERRUPT_PRIORITY);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);
}

// TODO
// No need to do it here, in void tx_application_define(void *first_unused_memory) function
// You don't need to use this first_unused_memory since it is not available in our port
// You should allocate an memory by your self such as memory_area array in threadx demo

//static uint64_t s_threadx_heap[TX_HEAP_SIZE/sizeof(uint64_t)];

VOID _tx_initialize_low_level(VOID)
{
//    _tx_initialize_unused_memory = s_threadx_heap;
    _tx_initialize_unused_memory = NULL;
    SetupSysTickInterrupt();
    _tx_thread_interrupt_control(TX_INT_DISABLE);
}

UINT _tx_thread_interrupt_control(UINT new_posture)
{
    ULONG temp;

    if (new_posture == TX_INT_DISABLE) {
        // clear interrupt
        temp = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
    } else {
        temp = __RV_CSR_READ_SET(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
    }
    __RWMB();
    return (UINT)temp;
}

/*    This function forcefully releases previously obtained protection, regardless of the protection count. */
/*    The supplied previous interrupt posture is restored.                                                  */
void _tx_thread_smp_force_unprotect(UINT new_interrupt_posture)
{
    UINT core_id;

    __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
    core_id = _tx_thread_smp_core_get();
    if (_tx_thread_smp_protection.tx_thread_smp_protect_core == core_id) {
        _tx_thread_smp_protection.tx_thread_smp_protect_count = 0;
        /* Publish the owner-clear state before handing the ticket to the
           next hart. Otherwise ticket_owner can become visible first, let the
           successor publish itself as owner, and then have this delayed clear
           overwrite the new owner's protect_core field. */
        _tx_thread_smp_protection.tx_thread_smp_protect_core = ((ULONG) 0xFFFFFFFFUL);
        __RWMB(); /* ensure prior stores visible */
        _tx_thread_smp_protection.tx_thread_smp_protect_ticket_owner ++;
        __RWMB();
    }
    __RV_CSR_SET(CSR_MSTATUS, new_interrupt_posture);
    __RWMB();
}

extern volatile UINT _tx_thread_preempt_disable;
#ifndef TXM_MODULE
void _tx_thread_system_return(void)
{
    _tx_thread_preempt_disable = 0;
    __RWMB();
    _tx_thread_smp_force_unprotect(MSTATUS_MIE);

    /* Set a software interrupt(SWI) request to request a context switch. */
    SysTimer_SetSWIRQ();
    /* Barriers are normally not required but do ensure the code is completely
    within the specified behaviour for the architecture. */
    __RWMB();
}

void _tx_thread_smp_core_preempt(UINT core)
{
    /* Set a software interrupt(SWI) request to request a context switch. */
    SysTimer_SetHartSWIRQ(core);
    /* Barriers are normally not required but do ensure the code is completely
    within the specified behaviour for the architecture. */
    __RWMB();
}

void _tx_thread_irq_exit_schedule_check(void)
{
    UINT coreid;

    coreid = _tx_thread_smp_core_get();

    /* Only request scheduling when the IRQ nesting for this core has fully
       unwound and ThreadX is about to return to normal thread context. */
    if (_tx_thread_system_state[coreid] != ((ULONG)0)) {
        return;
    }

    /* No scheduling request is needed if the current thread already matches
       the scheduler's execute target for this core. */
    if (_tx_thread_current_ptr[coreid] == _tx_thread_execute_ptr[coreid]) {
        return;
    }

    /* Honor ThreadX global preemption disable state.  If scheduling is
       deferred here, the normal ThreadX resume/preempt paths are expected
       to request it later. */
    if (_tx_thread_preempt_disable != ((UINT)0)) {
        return;
    }

    SysTimer_SetSWIRQ();
    __RWMB();
}
#endif

// _tx_thread_schedule function implemented in context.S

VOID _tx_thread_exit(VOID)
{
    while (1) {
        __WFI();
    }
}

// This function is gets the current state of the calling core.
ULONG _tx_thread_smp_current_state_get(void)
{
    ULONG temp;
    ULONG current_state;

    temp = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
    /* Pickup the current state.  */
    current_state =  _tx_thread_system_state[_tx_thread_smp_core_get()];

    /* Only restore the saved MIE bit here.  A full CSR write may clobber
       unrelated MSTATUS state established by nested trap handling. */
    __RV_CSR_SET(CSR_MSTATUS, temp);
    __RWMB();

    /* Now return the state for the core.  */
    return current_state;
}

// This function is gets the current thread of the calling core.
TX_THREAD *_tx_thread_smp_current_thread_get(void)
{
    ULONG temp;
    TX_THREAD *current_thread;

    temp = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
    /* Pickup the current state.  */
    current_thread =  _tx_thread_current_ptr[_tx_thread_smp_core_get()];

    /* Only restore the saved MIE bit here.  A full CSR write may clobber
       unrelated MSTATUS state established by nested trap handling. */
    __RV_CSR_SET(CSR_MSTATUS, temp);
    __RWMB();

    /* Now return the current thread for the core.  */
    return current_thread;
}

/*    This function gets protection for running inside the ThreadX        */
/*    source. This is accomplished by a FIFO ticket lock.                */
UINT _tx_thread_smp_protect(void)
{
    UINT old_posture;
    UINT core_id;
    UINT my_ticket;

    old_posture = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE) & MSTATUS_MIE;
    core_id = _tx_thread_smp_core_get();
    if (_tx_thread_smp_protection.tx_thread_smp_protect_core == core_id) {
        _tx_thread_smp_protection.tx_thread_smp_protect_count ++;
        __RWMB();   /* mem-barrier    */
        return old_posture;
    }

    my_ticket = (UINT)__AMOADD_W((volatile int32_t *)&(_tx_thread_smp_protection.tx_thread_smp_protect_ticket_next), 1);

    while (_tx_thread_smp_protection.tx_thread_smp_protect_ticket_owner != my_ticket) {
        __NOP();
    }

    _tx_thread_smp_protection.tx_thread_smp_protect_core = core_id;
    _tx_thread_smp_protection.tx_thread_smp_protect_count = 1;
    __RWMB();   /* mem-barrier    */
    return old_posture;
}

/*    This function releases previously obtained protection. The supplied */
/*    previous interrupt posture is restored.                             */
void _tx_thread_smp_unprotect(UINT new_interrupt_posture)
{
    UINT core_id;

    __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
    core_id = _tx_thread_smp_core_get();
    if (_tx_thread_smp_protection.tx_thread_smp_protect_core == core_id) {
        if (_tx_thread_smp_protection.tx_thread_smp_protect_count > 0) {
            _tx_thread_smp_protection.tx_thread_smp_protect_count --;
        }
        if ((_tx_thread_smp_protection.tx_thread_smp_protect_count == 0) && (_tx_thread_preempt_disable == 0)) {
            /* Publish the owner-clear state before advancing the ticket.
               Otherwise another hart can observe ticket_owner first, acquire
               the lock, and then get its protect_core update overwritten by
               this delayed clear from the previous owner. */
            _tx_thread_smp_protection.tx_thread_smp_protect_core = ((ULONG) 0xFFFFFFFFUL);
            __RWMB(); /* ensure prior stores visible */
            _tx_thread_smp_protection.tx_thread_smp_protect_ticket_owner ++;
            __RWMB();
        }
    }
    __RV_CSR_SET(CSR_MSTATUS, new_interrupt_posture);
    __RWMB();
}

/*    This function gets the global time value that is used for debug     */
/*    information and event tracing.                                      */
ULONG _tx_thread_smp_time_get(void)
{
    return ((ULONG) __read_time_csr());
}

/*    This function performs low-level initialization of the booting      */
/*    core.                                                               */
void _tx_thread_smp_low_level_initialize(UINT number_of_cores)
{
    // Enable interrupt and task sp swap
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
    __RV_CSR_SET(CSR_MECLIC_CTL, MECLIC_CTL_TSP_EN);
#endif
}

/*    This function is the place where additional cores wait until        */
/*    initialization is complete before they enter the thread scheduling  */
/*    loop.                                                               */
void _tx_thread_smp_initialize_wait(void)
{
    UINT core_id;

    /*
     * 1. Lockout interrupts.
     * We disable interrupts globally because the kernel is not yet ready
     * to handle vectors on this core.
     */
    __disable_irq();
    /*
     * 2. Pickup the CPU ID.
     * We need to know which core we are to access the correct index in
     * the system state array.
     */
    core_id = _tx_thread_smp_core_get();

    /**
     * If the current core ID exceeds the maximum allowed cores for ThreadX SMP,
     * enter an infinite sleep loop using the WFI (Wait For Interrupt) instruction.
     * This effectively disables the core, preventing it from executing further code.
     *
     * @note This is typically used to ensure that only supported cores are active,
     *       and any extra cores are put into a low-power state.
     */
    /* Sleep */
    if (core_id >= TX_THREAD_SMP_MAX_CORES) {
        while (1) {
            __WFI();
        }
    }

    /*
     * 3. Barrier 1: Wait for initialization state.
     * We wait until the Primary Core (Core 0) explicitly marks this core's
     * state as "IN_PROGRESS". This ensures the master knows we are alive
     * and the memory structures are prepared.
     */
    while (_tx_thread_system_state[core_id] != TX_INITIALIZE_IN_PROGRESS) {
        __NOP();
    }

    /*
     * 4. Barrier 2: Wait for the global release flag.
     * The Primary Core sets this flag only AFTER it has finished initializing
     * all global kernel objects (timers, queues, etc.).
     * We cannot proceed until the kernel is fully built.
     */
    while (_tx_thread_smp_release_cores_flag == 0) {
        __NOP();
    }

    SetupSysTickInterrupt();

    /*
     * 5. Clear this core's system state variable.
     * Setting the state to 0 indicates that this secondary core is now
     * fully initialized and ready to execute threads.
     */
    _tx_thread_system_state[core_id] = 0;

    __RWMB();   /* mem-barrier    */

    /*
     * 6. Barrier 3: Wait for Core 0 to finish.
     * Even though Core 0 released us, it might not have cleared its own
     * state yet. We wait for Core 0's state to become 0 to ensure all
     * cores enter the scheduler phase in sync.
     */
    while (_tx_thread_system_state[0] != 0)
    {
        __NOP();
    }

    // Enable interrupt and task sp swap
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
    __RV_CSR_SET(CSR_MECLIC_CTL, MECLIC_CTL_TSP_EN);
#endif

    __RWMB();
    /*
     * 7. Initialize is complete, enter the scheduling loop!
     * Jump to the main thread scheduler to start running tasks.
     */
    _tx_thread_schedule();
}

VOID _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
{
    struct thread_stack_frame *frame;
    uint8_t *stk;
    int i;

    stk  = thread_ptr -> tx_thread_stack_end;
    /* https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc */
    /* 32-bit boundary for ilp32e, and 128-bit boundary for others */
#ifndef __riscv_32e
    stk  = (uint8_t *)(((unsigned long)stk) & (~(unsigned long)(16 - 1)));
#else
    stk  = (uint8_t *)(((unsigned long)stk) & (~(unsigned long)(4 - 1)));
#endif
    stk -= sizeof(struct thread_stack_frame);

    frame = (struct thread_stack_frame*)stk;

    for (i = 0; i < sizeof(struct thread_stack_frame) / sizeof(unsigned long); i++) {
        ((unsigned long*)frame)[i] = 0xdeadbeef;
    }

    frame->epc     = (unsigned long)function_ptr;
    frame->ra     = (unsigned long)_tx_thread_exit;
    frame->mstatus = THREAD_INITIAL_MSTATUS;

    thread_ptr -> tx_thread_stack_ptr = stk;
    /* Indicate that this thread is now ready for scheduling again by another core.  */
    thread_ptr -> tx_thread_smp_core_control = 1;
    __RWMB();
}
