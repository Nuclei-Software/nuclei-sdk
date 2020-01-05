#include <stdio.h>
#include <os_cpu.h>
#include "ucos_ii.h"
#include "nuclei_sdk_soc.h"
#include <trap.h>

void syscall_handler(unsigned long sp[])
{
    extern void task_switch(unsigned long sp[]);
    task_switch(sp);
}

extern void task_switch(unsigned long sp[]);
#define configClockTicks        (RTC_CLOCK_HZ / TICK_RATE_HZ)
void time_handler(unsigned long sp[])
{
    static uint64_t then = 0;

    if (then == 0)  {
        then = SysTimer_GetLoadValue();
    }
    then += configClockTicks;
    SysTimer_SetCompareValue(then);

    OSIntEnter();
    OSTimeTick();
    OSIntExit(); //change OSIntExit , now it must be used with task_switch()
    task_switch(sp);
}


unsigned long trap_handler(unsigned long mcause, unsigned long sp[])
{
    unsigned long mepc;
    switch (mcause&0X00000fff)
    {
        case 11:
            mepc = __RV_CSR_READ(CSR_MEPC);
            sp[31] = mepc+4;
            syscall_handler(sp);
            break;
    }
    mepc = __RV_CSR_READ(CSR_MEPC);
    __RV_CSR_WRITE(CSR_MEPC, mepc+4);
    return mepc;
}

void scheduler_ecall(void )
{
    portYIELD();
}

void SetupTimer()    {
    ECLIC_SetCfgNlbits(4);
    SysTick_Config(configClockTicks);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_VECTOR_INTERRUPT);
    __enable_irq();
}
/*-----------------------------------------------------------*/


void TickSetup()    {
    Exception_Register_EXC(MmodeEcall_EXCn, (unsigned long)trap_handler);
    SetupTimer();
    OSIntNesting = 0;
}


int32_t timer_irq_handler(unsigned long mcause, unsigned long mepc, unsigned long sp[])
{
    sp[31]=mepc;
    time_handler(sp);
}

void halt(unsigned long mcause, unsigned long mepc)
{
    extern void exit(int);
    __disable_irq();
    printf("halt, mcause=%llx, mepc=%llx\n", mcause, mepc);
    exit(0);
}
