#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

void eclic_msip_handler(void)
{
  SysTimer_ClearSWIRQ();
}

CTEST(timer, timer_irq){	
  __disable_irq();	
  ECLIC_ClearPendingIRQ(SysTimerSW_IRQn);
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);

  ECLIC_Register_IRQ(SysTimerSW_IRQn,ECLIC_NON_VECTOR_INTERRUPT,ECLIC_LEVEL_TRIGGER,1,0,eclic_msip_handler);    /* register system timer SW interrupt */
  SysTimer_SetSWIRQ();
  ASSERT_NOT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);
  __enable_irq();
  for(uint32_t i = 10; i > 0; --i) {}
  ASSERT_EQUAL(ECLIC_GetPendingIRQ(SysTimerSW_IRQn), 0);

  ASSERT_NOT_EQUAL(SysTimer_GetMstopValue(), 1);
  SysTimer_SetMstopValue(1);
  ASSERT_EQUAL(SysTimer_GetMstopValue(), 1);
  SysTimer_SetMstopValue(0);
  ECLIC_DisableIRQ(SysTimerSW_IRQn);
  __disable_irq();
  
}

