#include <stdlib.h>
#include "evalsoc.h"

extern __weak void   eclic_msip_handler();
extern __weak void   eclic_mtip_handler();
extern __weak void   eclic_irq19_handler();
extern __weak void   eclic_irq20_handler();
extern __weak void   eclic_irq21_handler();
extern __weak void   eclic_irq22_handler();
extern __weak void   eclic_irq23_handler();
extern __weak void   eclic_irq24_handler();
extern __weak void   eclic_irq25_handler();
extern __weak void   eclic_irq26_handler();
extern __weak void   eclic_irq27_handler();
extern __weak void   eclic_irq28_handler();
extern __weak void   eclic_irq29_handler();
extern __weak void   eclic_irq30_handler();
extern __weak void   eclic_irq31_handler();
extern __weak void   eclic_irq32_handler();
extern __weak void   eclic_irq33_handler();
extern __weak void   eclic_irq34_handler();
extern __weak void   eclic_irq35_handler();
extern __weak void   eclic_irq36_handler();
extern __weak void   eclic_irq37_handler();
extern __weak void   eclic_irq38_handler();
extern __weak void   eclic_irq39_handler();
extern __weak void   eclic_irq40_handler();
extern __weak void   eclic_irq41_handler();
extern __weak void   eclic_irq42_handler();
extern __weak void   eclic_irq43_handler();
extern __weak void   eclic_irq44_handler();
extern __weak void   eclic_irq45_handler();
extern __weak void   eclic_irq46_handler();
extern __weak void   eclic_irq47_handler();
extern __weak void   eclic_irq48_handler();
extern __weak void   eclic_irq49_handler();
extern __weak void   eclic_irq50_handler();
extern __weak void   eclic_irq51_handler();

typedef void(*__fp)();

#pragma data_alignment = 256
static __fp vector_base[] = {
    0,
    0,
    0,
    eclic_msip_handler,
    0,
    0,
    0,
    eclic_mtip_handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    eclic_irq19_handler,
    eclic_irq20_handler,
    eclic_irq21_handler,
    eclic_irq22_handler,
    eclic_irq23_handler,
    eclic_irq24_handler,
    eclic_irq25_handler,
    eclic_irq26_handler,
    eclic_irq27_handler,
    eclic_irq28_handler,
    eclic_irq29_handler,
    eclic_irq30_handler,
    eclic_irq31_handler,
    eclic_irq32_handler,
    eclic_irq33_handler,
    eclic_irq34_handler,
    eclic_irq35_handler,
    eclic_irq36_handler,
    eclic_irq37_handler,
    eclic_irq38_handler,
    eclic_irq39_handler,
    eclic_irq40_handler,
    eclic_irq41_handler,
    eclic_irq42_handler,
    eclic_irq43_handler,
    eclic_irq44_handler,
    eclic_irq45_handler,
    eclic_irq46_handler,
    eclic_irq47_handler,
    eclic_irq48_handler,
    eclic_irq49_handler,
    eclic_irq50_handler,
    eclic_irq51_handler,
    0
};

extern void exc_entry(void);
extern void irq_entry(void);

extern void _premain_init(void);

extern void early_exc_entry(void);

#ifndef IAR_DATA_INIT
#define IAR_DATA_INIT _GLUE(__iar_data_init, _DLIB_ELF_INIT_INTERFACE_VERSION)
#endif

extern void IAR_DATA_INIT(void);

int __low_level_init()
{
    unsigned long temp;
    __disable_interrupt();

    /* Set the the NMI base to share with mtvec by setting CSR_MMISC_CTL */
    __set_bits_csr(CSR_MMISC_CTL, MMISC_CTL_NMI_CAUSE_FFF);
    /* Enable Zc feature when compiled zcmp & zcmt */
    __set_bits_csr(CSR_MMISC_CTL, MMISC_CTL_ZC);


    /* Intialize ECLIC vector interrupt base address mtvt to vector_base */
    __write_csr(CSR_MTVT, (unsigned long)(&vector_base));
    /*
     * Set ECLIC non-vector entry to be controlled by mtvt2 CSR register.
     * Intialize ECLIC non-vector interrupt base address mtvt2 to irq_entry.
     */
    __write_csr(CSR_MTVT2, 0x1|(unsigned long)(&irq_entry));

    /* Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main */
    __write_csr(CSR_MTVEC, ((unsigned long)&exc_entry));
    __clear_bits_csr(CSR_MTVEC, 0x3f);
    __set_bits_csr(CSR_MTVEC, 0x3);

    /* Enable FPU and Vector Unit if f/d/v exist in march */
#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU, and set state to initial */
    __clear_bits_csr(CSR_MSTATUS, MSTATUS_FS);
    __set_bits_csr(CSR_MSTATUS, MSTATUS_FS_INITIAL);
#endif

#if defined(__riscv_vector)
    /* Enable Vector, and set state to initial */
    __clear_bits_csr(CSR_MSTATUS, MSTATUS_VS);
    __set_bits_csr(CSR_MSTATUS, MSTATUS_VS_INITIAL);
#endif

    /* Enable mcycle and minstret counter */
    __clear_bits_csr(CSR_MCOUNTINHIBIT, 0x5);
    
    /* Call IAR Internal data initial function */
    IAR_DATA_INIT();

    /* Get CPU frequency and initialize uart for print */
    _premain_init();

    /* No need to call it again, since it is initialized */
    return 0;
}