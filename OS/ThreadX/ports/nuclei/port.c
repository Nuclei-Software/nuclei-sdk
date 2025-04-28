#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_initialize.h"

#include "nuclei_sdk_soc.h"

// SOC_TIMER_FREQ should be provided in <Device>.h of NMSIS. eg. evalsoc.h
// TX_TIMER_TICKS_PER_SECOND defined in tx_user.h which can overwrite the default one in tx_api.h if TX_INCLUDE_USER_DEFINE_FILE defined
#define SYSTICK_TICK_CONST          (SOC_TIMER_FREQ / TX_TIMER_TICKS_PER_SECOND)
#define KERNEL_INTERRUPT_PRIORITY   0

// MUST define SysTick_Handler as eclic_mtip_handler, which is registered in vector table
#define SysTick_Handler     eclic_mtip_handler

/* This is the timer interrupt service routine. */
void SysTick_Handler(void)
{
    // Reload timer
    SysTick_Reload(SYSTICK_TICK_CONST);

    /* Increment system clock. */
    _tx_timer_system_clock++;

    /* Test for time-slice expiration. */
    if (_tx_timer_time_slice) {
        /* Decrement the time_slice.  */
        _tx_timer_time_slice--;
        /* Check for expiration.  */
        if (_tx_timer_time_slice == 0) {
           /* Set the time-slice expired flag.  */
           _tx_timer_expired_time_slice =  TX_TRUE;
        }
    }

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
    if ((_tx_timer_expired_time_slice) || (_tx_timer_expired)) {
        /* Did a timer expire?  */
        if (_tx_timer_expired) {
            /* Process timer expiration.  */
            _tx_timer_expiration_process();
        }
        /* Did time slice expire?  */
        if (_tx_timer_expired_time_slice) {
            /* Time slice interrupted thread.  */
            _tx_thread_time_slice();
        }
    }
}

// Task Switch code called in eclic_msip_handler
void PortThreadSwitch(void)
{
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    _tx_execution_thread_exit();
#endif
    /* Determine if the time-slice is active.  */
    if (_tx_timer_time_slice) {
        /* Preserve current remaining time-slice for the thread and clear the current time-slice.  */
        _tx_thread_current_ptr -> tx_thread_time_slice = _tx_timer_time_slice;
        _tx_timer_time_slice =  0;
    }
    _tx_thread_current_ptr = _tx_thread_execute_ptr;
    /* Clear Software IRQ, A MUST */
    SysTimer_ClearSWIRQ();
}

void SetupSysTickInterrupt(void)
{
    uint64_t ticks = SYSTICK_TICK_CONST;

    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
    SysTick_Config(ticks);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, KERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_EnableIRQ(SysTimer_IRQn);

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
    _tx_thread_interrupt_control(0);
}

UINT _tx_thread_interrupt_control(UINT new_posture)
{
    ULONG temp;

    if (new_posture == TX_INT_DISABLE) {
        // clear interrupt
        temp = __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
    } else {
        temp = __RV_CSR_SWAP(CSR_MSTATUS, new_posture);
    }
    return (UINT)temp;
}

// _tx_thread_schedule function implemented in context.S
// _tx_thread_system_return implemented in tx_port.h

VOID _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
{

    struct thread_stack_frame *frame;
    uint8_t *stk;
    int i;

    stk  = thread_ptr -> tx_thread_stack_end;
    stk  = (uint8_t *)(((unsigned long)stk) & (~(unsigned long)(sizeof(ALIGN_TYPE) - 1)));
    stk -= sizeof(struct thread_stack_frame);

    frame = (struct thread_stack_frame*)stk;

    for (i = 0; i < sizeof(struct thread_stack_frame) / sizeof(unsigned long); i++) {
        ((unsigned long*)frame)[i] = 0xdeadbeef;
    }

    frame->epc     = (unsigned long)function_ptr;
    frame->mstatus = THREAD_INITIAL_MSTATUS;

    thread_ptr -> tx_thread_stack_ptr = stk;
}

