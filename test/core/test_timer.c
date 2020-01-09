#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

void eclic_msip_handler(void)
{
    ASSERT_NOT_EQUAL(SysTimer_GetMsipValue() & SysTimer_MSIP_MSIP_Msk, 0);
    SysTimer_ClearSWIRQ();
}

CTEST(timer, timer_irq) {
  __disable_irq();
  ECLIC_ClearPendingIRQ(SysTimerSW_IRQn);
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);

  ECLIC_Register_IRQ(SysTimerSW_IRQn,ECLIC_NON_VECTOR_INTERRUPT,ECLIC_LEVEL_TRIGGER,1,0,eclic_msip_handler);    /* register system timer SW interrupt */
  SysTimer_SetSWIRQ();
  ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);
  __enable_irq();
  for(uint32_t i = 10; i > 0; --i) {}
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);

  ASSERT_NOT_EQUAL(SysTimer_GetControlValue(), 1);
  SysTimer_SetControlValue(1);
  ASSERT_EQUAL(SysTimer_GetControlValue(), 1);
  SysTimer_SetControlValue(0);
  ECLIC_DisableIRQ(SysTimerSW_IRQn);
  __disable_irq();
}

CTEST(timer, timer_start_stop) {
    SysTimer_Stop();
    uint64_t cur_value = SysTimer_GetLoadValue();
    uint32_t cur_ctrl = SysTimer_GetControlValue();
    ctest_delay(20);
    ASSERT_EQUAL(cur_value, SysTimer_GetLoadValue());
    ASSERT_NOT_EQUAL(cur_ctrl & SysTimer_MTIMECTL_TIMESTOP_Msk, 0);
    SysTimer_Start();
    cur_ctrl = SysTimer_GetControlValue();
    ctest_delay(20);
    ASSERT_EQUAL(cur_ctrl & SysTimer_MTIMECTL_TIMESTOP_Msk, 0);
    ASSERT_NOT_EQUAL(cur_value, SysTimer_GetLoadValue());
}

CTEST(timer, timer_set_msip) {
    __disable_irq();
    SysTimer_SetMsipValue(SysTimer_MSIP_Msk);
    ASSERT_NOT_EQUAL(SysTimer_GetMsipValue() & SysTimer_MSIP_MSIP_Msk, 0);

    SysTimer_SetMsipValue(0);
    ASSERT_EQUAL(SysTimer_GetMsipValue() & SysTimer_MSIP_MSIP_Msk, 0);
    SysTimer_ClearSWIRQ();
}

// CTEST(timer, timer_softreset) {
//     SysTimer_SoftwareReset();
//     Should reset here
// }
