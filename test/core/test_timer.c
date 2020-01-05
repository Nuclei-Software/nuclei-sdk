#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"


void test_mtip_handler(void)
{
  // Clear Systimer pending flag by set compare value very large
  SysTimer_SetCompareValue(0xFFFFFFFFFFFFFFFFULL);
  ECLIC_ClearPendingIRQ(SysTimer_IRQn);
  ECLIC_DisableIRQ(SysTimer_IRQn);
}

void test_msip_handler(void)
{
  SysTimer_ClearSWIRQ();
}

CTEST(timer, timer_irq){

  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);

  ECLIC_Register_IRQ(SysTimer_IRQn,0,2,2,0,test_mtip_handler);    /* register system timer interrupt */
  SysTimer_SetCompareValue(SysTimer_GetLoadValue()+ 0.01*SOC_TIMER_FREQ);
  for(uint32_t i = 10000; i > 0; --i) {}
  ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);
  __enable_irq();
  for(uint32_t i = 10; i > 0; --i) {}
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);

  __disable_irq();

  ECLIC_Register_IRQ(SysTimerSW_IRQn,0,2,1,0,test_msip_handler);    /* register system timer SW interrupt */
  SysTimer_SetSWIRQ();
  ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);
  __enable_irq();
  for(uint32_t i = 10; i > 0; --i) {}
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimer_IRQn), 0);

  ASSERT_NOT_EQUAL(SysTimer_GetMstopValue(), 1);
  SysTimer_SetMstopValue(1);
  ASSERT_EQUAL(SysTimer_GetMstopValue(), 1);
  SysTimer_SetMstopValue(0);
}
