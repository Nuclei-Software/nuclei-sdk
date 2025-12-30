#include <stdio.h>
#include "ucos_ii.h"
#include "nuclei_sdk_soc.h"
#include "os_cpu_port.h"

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ                  SOC_TIMER_FREQ
#endif

#ifndef configKERNEL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY         0
#endif

#define SYSTICK_TICK_CONST                      (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)

/* Constants required to set up the initial stack. */
#define portINITIAL_MSTATUS                     ( MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL)
#define portINITIAL_EXC_RETURN                  ( 0xfffffffd )

/* Let the user override the pre-loading of the initial RA with the address of
prvTaskExit() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS                 configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS                 prvTaskExit
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt(void);

/*
 * Exception handlers.
 */
#define xPortSysTickHandler                     eclic_mtip_handler

void xPortSysTickHandler(void);

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
extern void prvPortStartFirstTask(void) __attribute__((naked));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExit(void);


/*-----------------------------------------------------------*/
/*
 *********************************************************************************************************
 *                                        INITIALIZE A TASK'S STACK
 *
 * Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
 *              stack frame of the task being created.  This function is highly processor specific.
 *
 * Arguments  : task          is a pointer to the task code
 *
 *              p_arg         is a pointer to a user supplied data area that will be passed to the task
 *                            when the task first executes.
 *
 *              p_tos         is a pointer to the top of stack.  It is assumed that 'ptos' points to
 *                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then
 *                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
 *                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
 *                            of the stack.
 *
 *              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
 *                            (see uCOS_II.H for OS_TASK_OPT_xxx).
 *
 * Returns    : Always returns the location of the new top-of-stack once the processor registers have
 *              been placed on the stack in the proper order.
 *
 * Note(s)    : (1) Interrupts are enabled when task starts executing.
 *
 *              (2) There is no need to save register x0 since it is a hard-wired zero.
 *
 *              (3) RISC-V calling convention register usage:
 *
 *                    +-------------+-------------+----------------------------------+
 *                    |  Register   |   ABI Name  | Description                      |
 *                    +-------------+-------------+----------------------------------+
 *                    |  x31 - x28  |   t6 - t3   | Temporaries                      |
 *                    +-------------+-------------+----------------------------------+
 *                    |  x27 - x18  |  s11 - s2   | Saved registers                  |
 *                    +-------------+-------------+----------------------------------+
 *                    |  x17 - x12  |   a7 - a2   | Function arguments               |
 *                    +-------------+-------------+----------------------------------+
 *                    |  x11 - x10  |   a1 - a0   | Function arguments/return values |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x9      |     s1      | Saved register                   |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x8      |    s0/fp    | Saved register/frame pointer     |
 *                    +-------------+-------------+----------------------------------+
 *                    |   x7 - x5   |   t2 - t0   | Temporaries                      |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x4      |     tp      | Thread pointer                   |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x3      |     gp      | Global pointer                   |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x2      |     sp      | Stack pointer                    |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x1      |     ra      | return address                   |
 *                    +-------------+-------------+----------------------------------+
 *                    |     x0      |    zero     | Hard-wired zero                  |
 *                    +-------------+-------------+----------------------------------+
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2. The new top of stack is passed out in a0.
 *
 * The RISC-V context is saved rtos tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * #ifndef __riscv_32e
 * rsv1
 * rsv0
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * #endif
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * pxCode
 */
OS_STK* OSTaskStkInit(void (*task)(void* pd), void* pdata, OS_STK* ptos, INT16U opt)
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */
    /* Stack frame size 32 REGBYTES(4/8) for most cases, but for ilp32e mode, it's 14 REGBYTES(4) */
    // Force stack 8byte align for double floating point case
    OS_STK* pxTopOfStack = (OS_STK*)(((unsigned long)ptos) & (~(unsigned long)(portBYTE_ALIGNMENT - 1)));

    /* Offset added to account for the way the MCU uses the stack on entry/exit
    of interrupts, and to ensure alignment. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_MSTATUS;    /* MSTATUS */

    /* Save code space by skipping register initialisation. */
#ifndef __riscv_32e
    pxTopOfStack -= 24;    /* X11 - X31, and 2 reserved regs space. */
#else
    pxTopOfStack -= 6;    /* X11 - X15. */
#endif
    *pxTopOfStack = (StackType_t) pdata;      /* X10/A0 */
    pxTopOfStack -= 6; /* X5 - X9 */
    *pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;      /* RA, X1 */

    pxTopOfStack --;
    *pxTopOfStack = ((StackType_t) task) ;        /* PC */

    return pxTopOfStack;
}

static void prvTaskExit(void)
{
    while (1) {
        __WFI();
    }
}

/*
 * See header file for description.
 */
void OSStartHighRdy(void)
{
    __disable_irq();
    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    vPortSetupTimerInterrupt();

    /* Mark OSRunning to True */
    OSRunning = OS_TRUE;

#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
    /* Call OSTaskSwHook */
    OSTaskSwHook();
#endif

    // Enable interrupt and task sp swap
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
    __RV_CSR_SET(CSR_MECLIC_CTL, MECLIC_CTL_TSP_EN);
#endif

    /* Start the first task. */
    prvPortStartFirstTask();

    /* Should never get here as the tasks will now be executing! */
    prvTaskExit();

    /* Should not get here! */
    return;
}

/*-----------------------------------------------------------*/

void xPortTaskSwitch(void)
{
    /* Clear Software IRQ, A MUST */
    SysTimer_ClearSWIRQ();
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
    /* Call OSTaskSwHook */
    OSTaskSwHook();
#endif
    OSPrioCur = OSPrioHighRdy;
    OSTCBCur  = OSTCBHighRdy;
}
/*-----------------------------------------------------------*/

/*
*********************************************************************************************************
*                                          SYS TICK HANDLER
*
* Description: Handle the system tick (SysTick) interrupt, which is used to generate the uC/OS-II tick
*              interrupt.
*
* Arguments  : None.
*
* Note(s)    : This function is defined with weak linking in 'riscv_hal_stubs.c' so that it can be
*              overridden by the kernel port with same prototype
*********************************************************************************************************
*/
void xPortSysTickHandler(void)
{
#if OS_CRITICAL_METHOD == 3u                   /* Allocate storage for CPU status register             */
    OS_CPU_SR cpu_sr;
#endif
    /* The SysTick runs at the lowest interrupt priority, so when this interrupt
    executes all interrupts must be unmasked.  There is therefore no need to
    save and then restore the interrupt mask value as its value is already
    known. */
    OS_ENTER_CRITICAL();
    SysTick_Reload(SYSTICK_TICK_CONST);
    OSIntEnter();                              /* Tell uC/OS-II that we are starting an ISR            */
    OS_EXIT_CRITICAL();

    OSTimeTick();                              /* Call uC/OS-II's OSTimeTick()                         */

    OSIntExit();                               /* Tell uC/OS-II that we are leaving the ISR            */
}

/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__((weak)) void vPortSetupTimerInterrupt(void)
{
    TickType_t ticks = SYSTICK_TICK_CONST;

    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
    SysTick_Config(ticks);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_EnableIRQ(SysTimer_IRQn);

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);
}
/*-----------------------------------------------------------*/
