#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

CTEST(core, riscv_xlen)
{
    ASSERT_EQUAL(__RISCV_XLEN, __riscv_xlen);
}

CTEST(core, csr_type)
{
    ASSERT_EQUAL(sizeof(rv_csr_t), sizeof(unsigned long));
#if __RISCV_XLEN == 32
    ASSERT_EQUAL(sizeof(rv_csr_t), 4);
#else
    ASSERT_EQUAL(sizeof(rv_csr_t), 8);
#endif
}

CTEST(core, csr_swap)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0x12345678);
    ASSERT_EQUAL(__RV_CSR_SWAP(CSR_MSCRATCH, 0xABCDEF12), 0x12345678);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0xABCDEF12);
    rv_csr_t mscratch = 0xDEADBEAF;
    ASSERT_EQUAL(__RV_CSR_SWAP(CSR_MSCRATCH, mscratch), 0xABCDEF12);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), mscratch);
}

CTEST(core, csr_read)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0xDEADBEEF);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0xDEADBEEF);
}

CTEST(core, csr_write)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0xDEADBEEF);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0xDEADBEEF);
    rv_csr_t mscratch = 0x12376812;
    __RV_CSR_WRITE(CSR_MSCRATCH, mscratch);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), mscratch);
}

CTEST(core, csr_read_set)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0x0);
    ASSERT_EQUAL(__RV_CSR_READ_SET(CSR_MSCRATCH, 0xFF), 0x0);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0xFF);
    rv_csr_t random_val = __RV_CSR_READ(CSR_MCYCLE);
    __RV_CSR_WRITE(CSR_MSCRATCH, random_val);
    ASSERT_EQUAL(__RV_CSR_READ_SET(CSR_MSCRATCH, 0x12345678), random_val);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0x12345678 | random_val);
}

CTEST(core, csr_set)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0xFF003456);
    __RV_CSR_SET(CSR_MSCRATCH, 0x12EF);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0x12EF | 0xFF003456);
    rv_csr_t random_val = __RV_CSR_READ(CSR_MCYCLE);
    __RV_CSR_WRITE(CSR_MSCRATCH, random_val);
    __RV_CSR_SET(CSR_MSCRATCH, 0x12345678);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), 0x12345678 | random_val);
}

CTEST(core, csr_read_clear)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0xFF003456);
    ASSERT_EQUAL(__RV_CSR_READ_CLEAR(CSR_MSCRATCH, 0xFF), 0xFF003456);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), ~0xFF & 0xFF003456);
    rv_csr_t random_val = __RV_CSR_READ(CSR_MCYCLE);
    __RV_CSR_WRITE(CSR_MSCRATCH, random_val);
    ASSERT_EQUAL(__RV_CSR_READ_CLEAR(CSR_MSCRATCH, 0x12345678), random_val);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), ~0x12345678 & random_val);
}

CTEST(core, csr_clear)
{
    __RV_CSR_WRITE(CSR_MSCRATCH, 0xFF003456);
    __RV_CSR_CLEAR(CSR_MSCRATCH, 0xFF);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), ~0xFF & 0xFF003456);
    rv_csr_t random_val = __RV_CSR_READ(CSR_MCYCLE);
    __RV_CSR_WRITE(CSR_MSCRATCH, random_val);
    __RV_CSR_READ_CLEAR(CSR_MSCRATCH, 0x12345678);
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSCRATCH), ~0x12345678 & random_val);
}

CTEST(core, irq)
{
    __enable_irq();
    ASSERT_NOT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_MIE, 0);
    __disable_irq();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_MIE, 0);
}

CTEST(core, cycle)
{
    uint64_t cycles = 0;
#if __RISCV_XLEN == 32
    cycles = __RV_CSR_READ(CSR_MCYCLE) | ((uint64_t)__RV_CSR_READ(CSR_MCYCLEH) << 32);
#elif __RISCV_XLEN == 64
    cycles = __RV_CSR_READ(CSR_MCYCLE);
#endif
    ASSERT_EQUAL(__get_rv_cycle() > cycles, 1);
}

CTEST(core, instret)
{
    uint64_t instret = 0;
#if __RISCV_XLEN == 32
    instret = __RV_CSR_READ(CSR_MINSTRET) | ((uint64_t)__RV_CSR_READ(CSR_MINSTRETH) << 32);
#elif __RISCV_XLEN == 64
    instret = __RV_CSR_READ(CSR_MINSTRET);
#endif
    ASSERT_EQUAL(__get_rv_instret() > instret, 1);
}

CTEST(core, nop)
{
    uint64_t instret = __RV_CSR_READ(CSR_MINSTRET);
    uint64_t diff = __RV_CSR_READ(CSR_MINSTRET) - instret;
    instret = __RV_CSR_READ(CSR_MINSTRET);
    __NOP();
    uint64_t diff2 = __RV_CSR_READ(CSR_MINSTRET) - instret;
    //CTEST_LOG("Diff: %d, %d\r\n", diff, diff2);
    ASSERT_EQUAL(diff2 > diff, 1);
}

void SOC_MTIMER_HANDLER(void)
{
    // Clear Systimer pending flag by set compare value very large
    SysTimer_SetCompareValue(0xFFFFFFFFFFFFFFFFULL);
    ECLIC_ClearPendingIRQ(SysTimer_IRQn);
    ECLIC_DisableIRQ(SysTimer_IRQn);
}

CTEST(core, wfi)
{
    __disable_irq();
    SysTick_Config(1000);
    SysTimer_Start();
    __WFI();
    // Should not enter interrupt handler
#if defined(__NUCLEI_N_REV) && (__NUCLEI_N_REV < 0x0104)
    // For nuclei core version >= 1.4, the pending irq state
    // will be automatically cleared due to auto clear feature
    ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
#endif
    ASSERT_NOT_EQUAL(ECLIC_GetEnableIRQ(SysTimer_IRQn), 0);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_ClearPendingIRQ(SysTimer_IRQn);
    SysTick_Config(1000);
    SysTimer_Start();
    __enable_irq();
    __WFI();
    // Should enter interrupt handler first, then do following steps
    ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
    ASSERT_EQUAL(ECLIC_GetEnableIRQ(SysTimer_IRQn), 0);
    __disable_irq();
}

CTEST(core, wfe)
{
    // TODO Not yet find a way to test it
}

static uint32_t ecall = 0;

void ecall_handler(unsigned long mcause, unsigned long sp)
{
    ecall = 1;
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    // mepc for ecall is the pc where ecall instruction located
    // so we just add 4 to mepc to return to the next instruction
    exc_frame->mepc += 4;
    CTEST_LOG("__ECALL called\n");
}

CTEST(core, ecall)
{
    ecall = 0;
    // working: ecall will not trigger the exception of machine mode ecall
    Exception_Register_EXC(MmodeEcall_EXCn, (unsigned long)ecall_handler);
    __ECALL();
    ASSERT_EQUAL(ecall, 1);
}

CTEST(core, sleepmode)
{
    __disable_irq();
    SysTick_Config(1000);
    CTEST_LOG("Enter to Shallow Sleep Mode\r\n");
    __set_wfi_sleepmode(WFI_SHALLOW_SLEEP);
    __WFI();
    // Should not enter interrupt handler
#if defined(__NUCLEI_N_REV) && (__NUCLEI_N_REV < 0x0104)
    // For nuclei core version >= 1.4, the pending irq state
    // will be automatically cleared due to auto clear feature
    ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
#endif
    ASSERT_NOT_EQUAL(ECLIC_GetEnableIRQ(SysTimer_IRQn), 0);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_ClearPendingIRQ(SysTimer_IRQn);
    SysTick_Config(1000);
    __enable_irq();
    // Uncomment the following line, the CPU will enter to deep sleep
    //CTEST_LOG("Enter to Deep Sleep Mode\r\n");
    //__set_wfi_sleepmode(WFI_DEEP_SLEEP);
    __WFI();
    // Should enter interrupt handler first, then do following steps
    ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
    ASSERT_EQUAL(ECLIC_GetEnableIRQ(SysTimer_IRQn), 0);
    __disable_irq();
}

CTEST(core, txevt)
{
    CTEST_LOG("Send TXEVT\r\n");
    __TXEVT();
}

CTEST(core, mcycle_enable_disable)
{
    CTEST_LOG("Disable MCYCLE Counter\r\n");
    __disable_mcycle_counter();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & MCOUNTINHIBIT_CY, MCOUNTINHIBIT_CY);
    uint64_t cycle_b = __get_rv_cycle();
    __NOP();
    __NOP();
    __NOP();
    uint64_t cycle_a = __get_rv_cycle();
    ASSERT_EQUAL(cycle_b, cycle_a);

    CTEST_LOG("Enable MCYCLE Counter\r\n");
    __enable_mcycle_counter();
    ASSERT_NOT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & MCOUNTINHIBIT_CY, MCOUNTINHIBIT_CY);
    cycle_b = __get_rv_cycle();
    __NOP();
    __NOP();
    __NOP();
    cycle_a = __get_rv_cycle();
    ASSERT_EQUAL(cycle_a > cycle_b, 1);
}

CTEST(core, minstret_enable_disable)
{
    CTEST_LOG("Disable MINSTRET Counter\r\n");
    __disable_minstret_counter();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & MCOUNTINHIBIT_IR, MCOUNTINHIBIT_IR);
    uint64_t instret_b = __get_rv_instret();
    __NOP();
    __NOP();
    __NOP();
    uint64_t instret_a = __get_rv_instret();
    ASSERT_EQUAL(instret_b, instret_a);

    CTEST_LOG("Enable MINSTRET Counter\r\n");
    __enable_minstret_counter();
    ASSERT_NOT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & MCOUNTINHIBIT_IR, MCOUNTINHIBIT_IR);
    instret_b = __get_rv_instret();
    __NOP();
    __NOP();
    __NOP();
    instret_a = __get_rv_instret();
    ASSERT_EQUAL(instret_a > instret_b, 1);
}

CTEST(core, allcounter_enable_disable)
{
    CTEST_LOG("Disable All Counter\r\n");
    __disable_all_counter();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & (MCOUNTINHIBIT_IR | MCOUNTINHIBIT_CY), MCOUNTINHIBIT_IR | MCOUNTINHIBIT_CY);
    __disable_mcycle_counter();
    uint64_t cycle_b = __get_rv_cycle();
    __NOP();
    __NOP();
    __NOP();
    uint64_t cycle_a = __get_rv_cycle();
    ASSERT_EQUAL(cycle_b, cycle_a);
    uint64_t instret_b = __get_rv_instret();
    __NOP();
    __NOP();
    __NOP();
    uint64_t instret_a = __get_rv_instret();
    ASSERT_EQUAL(instret_b, instret_a);

    CTEST_LOG("Enable All Counter\r\n");
    __enable_all_counter();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MCOUNTINHIBIT) & (MCOUNTINHIBIT_IR | MCOUNTINHIBIT_CY), 0);
    instret_b = __get_rv_instret();
    __NOP();
    __NOP();
    __NOP();
    instret_a = __get_rv_instret();
    ASSERT_EQUAL(instret_a > instret_b, 1);
    cycle_b = __get_rv_cycle();
    __NOP();
    __NOP();
    __NOP();
    cycle_a = __get_rv_cycle();
    ASSERT_EQUAL(cycle_a > cycle_b, 1);
}

CTEST(core, fence)
{
    __FENCE(rw, rw);
    __FENCE(iorw, rw);
    __FENCE_I();
    __RWMB();
    __RMB();
    __WMB();
    __SMP_RWMB();
    __SMP_RMB();
    __SMP_WMB();
    __CPU_RELAX();
}
CTEST(core, lb)
{
    uint32_t data = 0x12345678;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__LB((void*)pdata), 0x78);
    ASSERT_EQUAL(__LB((void*)(pdata + 1)), 0x56);
    ASSERT_EQUAL(__LB((void*)(pdata + 2)), 0x34);
    ASSERT_EQUAL(__LB((void*)(pdata + 3)), 0x12);
}

CTEST(core, sb)
{
    uint32_t data = 0x12345678;
    unsigned long pdata = (unsigned long)(&data);
    __SB((void*)pdata, 0xAB);
    ASSERT_EQUAL(__LB((void*)pdata), 0xAB);
    __SB((void*)(pdata + 1), 0xCD);
    ASSERT_EQUAL(__LB((void*)(pdata + 1)), 0xCD);
    __SB((void*)(pdata + 2), 0xEF);
    ASSERT_EQUAL(__LB((void*)(pdata + 2)), 0xEF);
    __SB((void*)(pdata + 3), 0x00);
    ASSERT_EQUAL(__LB((void*)(pdata + 3)), 0x00);
}

CTEST(core, lh)
{
    uint32_t data = 0x12345678;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__LH((void*)pdata), 0x5678);
    ASSERT_EQUAL(__LH((void*)(pdata + 1)), 0x3456);
    ASSERT_EQUAL(__LH((void*)(pdata + 2)), 0x1234);
}

CTEST(core, sh)
{
    uint32_t data = 0x12345678;
    unsigned long pdata = (unsigned long)(&data);
    __SH((void*)pdata, 0xABCD);
    ASSERT_EQUAL(__LH((void*)pdata), 0xABCD);
    __SH((void*)(pdata + 1), 0xCDEF);
    ASSERT_EQUAL(__LH((void*)(pdata + 1)), 0xCDEF);
    __SH((void*)(pdata + 2), 0xEF00);
    ASSERT_EQUAL(__LH((void*)(pdata + 2)), 0xEF00);
}

CTEST(core, lw)
{
    uint32_t data[2] = {0x12345678, 0xABCDEF01};
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__LW((void*)pdata), 0x12345678);
    ASSERT_EQUAL(__LW((void*)(pdata + 1)), 0x01123456);
    ASSERT_EQUAL(__LW((void*)(pdata + 2)), 0xEF011234);
    ASSERT_EQUAL(__LW((void*)(pdata + 3)), 0xCDEF0112);
    ASSERT_EQUAL(__LW((void*)(pdata + 4)), 0xABCDEF01);
}

CTEST(core, sw)
{
    uint32_t data[2] = {0x12345678, 0xABCDEF01};
    unsigned long pdata = (unsigned long)(&data);
    __SW((void*)pdata, 0xDEADBEEF);
    ASSERT_EQUAL(__LW((void*)pdata), 0xDEADBEEF);
    __SW((void*)(pdata + 1), 0xCDEF1234);
    ASSERT_EQUAL(__LW((void*)(pdata + 1)), 0xCDEF1234);
    __SW((void*)(pdata + 2), 0xEF007678);
    ASSERT_EQUAL(__LW((void*)(pdata + 2)), 0xEF007678);
    __SW((void*)(pdata + 3), 0x89282678);
    ASSERT_EQUAL(__LW((void*)(pdata + 3)), 0x89282678);
    __SW((void*)(pdata + 4), 0xDEADBEAF);
    ASSERT_EQUAL(__LW((void*)(pdata + 4)), 0xDEADBEAF);
}

#if __RISCV_XLEN == 64
CTEST(core, ld)
{
    uint32_t data[4] = {0x12345678, 0xABCDEF01, 0x87654321, 0x01FEDCBA};
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__LD((void*)pdata), 0xABCDEF0112345678);
    ASSERT_EQUAL(__LD((void*)(pdata + 1)), 0x21ABCDEF01123456);
    ASSERT_EQUAL(__LD((void*)(pdata + 2)), 0x4321ABCDEF011234);
    ASSERT_EQUAL(__LD((void*)(pdata + 3)), 0x654321ABCDEF0112);
    ASSERT_EQUAL(__LD((void*)(pdata + 4)), 0x87654321ABCDEF01);
    ASSERT_EQUAL(__LD((void*)(pdata + 8)), 0x01FEDCBA87654321);
}

CTEST(core, sd)
{
    uint32_t data[4] = {0x12345678, 0xABCDEF01, 0x87654321, 0x01FEDCBA};
    unsigned long pdata = (unsigned long)(&data);
    __SD((void*)pdata, 0xDEADBEEFDEADBEEF);
    ASSERT_EQUAL(__LD((void*)pdata), 0xDEADBEEFDEADBEEF);
    __SD((void*)(pdata + 1), 0xCDEF123443219876);
    ASSERT_EQUAL(__LD((void*)(pdata + 1)), 0xCDEF123443219876);
    __SD((void*)(pdata + 2), 0xEF00767887665776);
    ASSERT_EQUAL(__LD((void*)(pdata + 2)), 0xEF00767887665776);
    __SD((void*)(pdata + 3), 0x8928267890892912);
    ASSERT_EQUAL(__LD((void*)(pdata + 3)), 0x8928267890892912);
    __SD((void*)(pdata + 4), 0xDEADBEAFBEEFDEAD);
    ASSERT_EQUAL(__LD((void*)(pdata + 4)), 0xDEADBEAFBEEFDEAD);
    __SD((void*)(pdata + 8), 0xBEEFDEADDEADBEAF);
    ASSERT_EQUAL(__LD((void*)(pdata + 8)), 0xBEEFDEADDEADBEAF);
}
#endif

#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT == 1
CTEST(core, icache)
{
    EnableICache();
    ASSERT_NOT_EQUAL(__RV_CSR_READ(CSR_MCACHE_CTL) & CSR_MCACHE_CTL_IE, 0);
    DisableICache();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MCACHE_CTL) & CSR_MCACHE_CTL_IE, 0);
}
#endif

CTEST(core, csrcontext)
{
    SAVE_IRQ_CSR_CONTEXT();
    RESTORE_IRQ_CSR_CONTEXT();
}

CTEST(core, hpmevent)
{
    for (unsigned long i = 0; i < 32; i ++) {
        __set_hpm_event(i, 0xA5A5A5A5);
        __get_hpm_event(i);
    }
}

CTEST(core, hpmcounter)
{
    for (unsigned long i = 0; i < 32; i ++) {
        __set_hpm_counter(i, 0xA5A5A5A5A5A5A5A5ULL);
        __get_hpm_counter(i);
        __enable_mhpm_counter(i);
        __disable_mhpm_counter(i);
        __enable_mhpm_counters(1<<i);
        __disable_mhpm_counters(1<<i);
    }
}

