// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
#else
#error "This example require CPU ECLIC feature"
#endif

#if defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1)
#else
#error "This example require CPU System Timer feature"
#endif

#if !defined(__PMP_PRESENT) || (__PMP_PRESENT == 0)
#error "__PMP_PRESENT is not defined or equal to 0, please check!"
#error "This example require CPU PMP feature!"
#endif

#define TIMER_TICKS             (SOC_TIMER_FREQ / 50)

// 2048 is enough
#define UMODE_STACK_SIZE        2048
/* Create a stack for user mode execution */
uint8_t umode_stack[UMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t umode_sp = (uintptr_t) (umode_stack + sizeof(umode_stack));

volatile unsigned long timer_flag = 0;

// setup timer
void setup_timer(void)
{
    printf("Initialize timer and start timer interrupt\n\r");
    SysTick_Config(TIMER_TICKS);
}

// timer interrupt handler
// non-vector mode interrupt
void eclic_mtip_handler(void)
{
    // Reload Timer Interrupt
    SysTick_Reload(TIMER_TICKS);

    SysTimer_Stop();

    printf("Enter to SysTimer M-Mode interrupt handler\r\n");

    timer_flag = 0xAA;
}

static __attribute__((aligned(0x1000))) void user_mode_entry_point(void)
{
    printf("switch to user mode successfully\r\n");

    while (1) {
        if (timer_flag == 0xAA) {
            printf("go back to user mode from SysTimer IRQ handler\r\n");
            printf("[SUCCESS] Test M-mode drop to U-mode passed!\r\n");
#ifdef CFG_SIMULATION
            // directly exit if in nuclei internally simulation
            SIMULATION_EXIT(0);
#endif
            while (1);
        }
    }
}

void illegal_instruction_handler(unsigned long mcause, unsigned long sp)
{
    // Check whether PMP is present via illegal instruction check
    printf("PMP is not present, will not run this example\r\n");
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while (1);
}

int main(int argc, char** argv)
{
    int32_t returnCode;
    CSR_MCFGINFO_Type mcfg_info;
    CSR_MISA_Type misa;

    misa.d = __RV_CSR_READ(CSR_MISA);
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);

    if (mcfg_info.b.clic == 0) {
        printf("ECLIC is not present, will not run this example!\r\n");
        return 0;
    }

    if (misa.b.u == 0) {
        printf("U-Mode is not present, will not run this example!\r\n");
        return 0;
    }

    // Register illegal instruction handler to check whether PMP present
    // If not present, configure PMP CSR will cause illegal instruction exception
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)illegal_instruction_handler);

    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X | PMP_A_NAPOT,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };
    __set_PMPENTRYx(0, &pmp_cfg);

#if defined(__SMPU_PRESENT) && (__SMPU_PRESENT == 1)
    if (mcfg_info.b.tee == 1) {
        printf("Configure SMPU due to TEE Present\r\n");
        /* Configuration of execution region*/
        smpu_config smpu_config = {
            /*  M mode grants U mode with full permission of the whole address range*/
            .protection = SMPU_R | SMPU_W | SMPU_X | SMPU_A_NAPOT,
            /* Initial protected excutable address range is 2^__RISCV_XLENbytes */
            .order = __RISCV_XLEN,
            /* Initial base address is 0 */
            .base_addr = 0,
        };

        __set_SMPUENTRYx(0, &smpu_config);

        // enable smpu entry0
        __set_SMPUSWITCHx(0x1);
    }
#endif

    // Restore illegal instruction handler as NULL which will go to default exception handler
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)NULL);

    // initialize timer
    setup_timer();

    // inital timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER, 1, 0, (void*)eclic_mtip_handler);

    // Enable interrupts in general.
    __enable_irq();

    /* Drop to U mode */
    __switch_mode(PRV_U, umode_sp, user_mode_entry_point);

    printf("[ERROR] the code should not run here!!!\r\n");
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(1);
#endif
    while (1);

    return 0;
}
