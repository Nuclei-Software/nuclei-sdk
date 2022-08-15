// See LICENSE for license details.
#include <stdio.h>
#include <string.h>
#include "nuclei_sdk_soc.h"

/* different trigger condition */
#define INSTRUCTION_FETCH_EXCEPTION    0
#define LOAD_EXCEPTION                 1
#define STORE_EXCEPTION                2
#define RUN_WITH_NO_PMP_CHECK          3

/* Just choose one from above defines to test PMP */
#define TRIGGER_PMP_VIOLATION_MODE     RUN_WITH_NO_PMP_CHECK

volatile uint8_t protected_data[0x2000] __attribute__((aligned(0x2000))) =    \
{0xaa, 0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0xaa};

static void __attribute__((section (".text"), aligned(0x2000))) protected_execute(void)
{
    printf("----protected_execute succeed!----\r\n");

    printf("Attempting to read protected_data[0] \r\n");
    printf("protected_data[0]: 0x%0X succeed \r\n", protected_data[0]);

    printf("Attempting to write protected_data[0] \r\n");
    protected_data[0] = 0xFF;

    printf("Won't run here if violates L R\\W\\X permission check! \r\n");
}

static void pmp_violation_fault_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

    switch (mcause & MCAUSE_CAUSE) {
        case InsAccFault_EXCn: 
            printf("Instruction access fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case LdFault_EXCn:
            printf("Load access fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case StAccessFault_EXCn:
            printf("Store/AMO access fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        default: break;
    }
    while(1);
}

int main(void)
{
    /* Configuration of execution region*/
    pmp_config pmp_config_x = {
        /* 
         * Locked PMP entries remain locked until the hart is reset,
         * the L bit also indicates whether the R/W/X permissions are enforced on M-mode accesses 
         */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Initial protected excutable address range is 2^12 = 4K bytes*/
        .order = 12,
        /* initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    /* configuration of read/write region*/
    pmp_config pmp_config_rw = {
        /* 
         * Locked PMP entries remain locked until the hart is reset,
         * the L bit also indicates whether the R/W/X permissions are enforced on M-mode accesses 
         */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* initial protected readable/writable address range is 2^12 = 4K bytes */
        .order = 12,
        /* initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    pmp_config_x.base_addr = (unsigned long)protected_execute;
    pmp_config_rw.base_addr = (unsigned long)protected_data;

#if (INSTRUCTION_FETCH_EXCEPTION == TRIGGER_PMP_VIOLATION_MODE)
    /* Remove X permission of protected_execute region */
    pmp_config_x.protection = PMP_L | PMP_R | PMP_W;

    pmp_config_rw.protection = PMP_L | PMP_R | PMP_W;
#elif (LOAD_EXCEPTION == TRIGGER_PMP_VIOLATION_MODE)
    /* Retrive X permission of protected_execute region */
    pmp_config_x.protection = PMP_L | PMP_R | PMP_W | PMP_X;

    /* Remove R permission of protected_data region */
    pmp_config_rw.protection = PMP_L | PMP_W;
#elif (STORE_EXCEPTION == TRIGGER_PMP_VIOLATION_MODE)
    /* Retrive X permission of protected_execute region */
    pmp_config_x.protection = PMP_L | PMP_R | PMP_W | PMP_X;

    /* Remove W permission of protected_data region */
    pmp_config_rw.protection = PMP_L | PMP_R ;
#elif (RUN_WITH_NO_PMP_CHECK == TRIGGER_PMP_VIOLATION_MODE)
    /* Unset Locking bit, any M-mode access matching the PMP entry will succeed */
    pmp_config_x.protection = PMP_R | PMP_W | PMP_X;

    pmp_config_rw.protection = PMP_R | PMP_W ;
#endif

    printf("------PMP demo------\r\n");

    __set_PMPENTRYx(0, &pmp_config_x);
    /* Verify the configuration takes effect */
    memset(&pmp_config_x, 0, sizeof(pmp_config));
    __get_PMPENTRYx(0, &pmp_config_x);
    printf("Get pmp entry: index %d, prot_out: %0x, addr_out: %x, order_out: %d\r\n", \
        0, pmp_config_x.protection, pmp_config_x.base_addr, pmp_config_x.order);

    __set_PMPENTRYx(1, &pmp_config_rw);
    /* Verify the configuration takes effect */
    memset(&pmp_config_rw, 0, sizeof(pmp_config));
    __get_PMPENTRYx(1, &pmp_config_rw);
    printf("Get pmp entry: index %d, prot_out: %0x, addr_out: %x, order_out: %d\r\n", \
        1, pmp_config_rw.protection, pmp_config_rw.base_addr, pmp_config_rw.order);

    /* register corresponding exception */
    Exception_Register_EXC(InsAccFault_EXCn, (unsigned long)pmp_violation_fault_handler);
    Exception_Register_EXC(LdFault_EXCn, (unsigned long)pmp_violation_fault_handler);
    Exception_Register_EXC(StAccessFault_EXCn, (unsigned long)pmp_violation_fault_handler);

    /* In case compiler use inline optimization of protected_execute */
    void (*fncptr)(void) = ((void *)protected_execute);
    printf("Attempting to fetch instruction from protected address\n");
    fncptr();

    while(1);
    return 0;
}

