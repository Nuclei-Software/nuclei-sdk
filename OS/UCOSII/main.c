#include<ucos_ii.h>
#include<stdint.h>
#include<cpu.h>

#include "drivers/func.h"
#include "soc/drivers/soc.h"
#include "soc/drivers/board.h"
#include "drivers/riscv_encoding.h"
#include "drivers/timer.h"
#include "drivers/eclic.h"


#define BUTTON_1_GPIO_OFFSET 30
#define BUTTON_2_GPIO_OFFSET 31

#define ECLIC_INT_DEVICE_BUTTON_1 (SOC_ECLIC_INT_GPIO_BASE + BUTTON_1_GPIO_OFFSET)
#define ECLIC_INT_DEVICE_BUTTON_2 (SOC_ECLIC_INT_GPIO_BASE + BUTTON_2_GPIO_OFFSET)
// The real value is:
//#define ECLIC_INT_DEVICE_BUTTON_1 49 // 30+19
//#define ECLIC_INT_DEVICE_BUTTON_2 50 // 31+19

// Since the BUTTON_1 ECLIC IRQ number is 49, and BUTTON_2 is 50, we need to overriede the irq49/50 handler 
#define BUTTON_1_HANDLER eclic_irq49_handler
#define BUTTON_2_HANDLER eclic_irq50_handler


#define mainQUEUE_LENGTH                    ( 1 )
static void prvSetupHardware( void );
extern void idle_task(void);



void gpio_init(){
  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_GPIO_OFFSET)| (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;

  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << RED_LED_GPIO_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< BLUE_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET)) ;
}


void button_init(){

    GPIO_REG(GPIO_OUTPUT_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_PULLUP_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_INPUT_EN)   |=  ((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    
    GPIO_REG(GPIO_RISE_IE) |= ((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_FALL_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_HIGH_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_LOW_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));

}




void prvSetupHardware( void )
{
    button_init();
    gpio_init();
}


#define STK_LEN 1024 

OS_STK task1_stk[STK_LEN];
OS_STK task2_stk[STK_LEN];
OS_STK task3_stk[STK_LEN];
OS_STK start_stk[STK_LEN];

#define TASK1_PRIO       			13 
#define TASK2_PRIO       			12 
#define TASK3_PRIO       			11
#define TASK_START_PRIO       			10 


void task1(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task1 is runing... %d\n",cnt);
     GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << BLUE_LED_GPIO_OFFSET);
		OSTimeDly(200);
	}
}

void task2(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task2 is runing... %d\n",cnt);
     GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << GREEN_LED_GPIO_OFFSET);
		OSTimeDly(100);
	}
}

void task3(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task3 is runing... %d\n",cnt);
		OSTimeDly(100);
	}
}

void start_task(void *args)
{

	printf("start all task...\n");
  OSTaskCreate(task1,NULL,&task1_stk[STK_LEN-1],TASK1_PRIO);
	OSTaskCreate(task2,NULL,&task2_stk[STK_LEN-1],TASK2_PRIO);
  OSTaskCreate(task3,NULL,&task3_stk[STK_LEN-1],TASK3_PRIO);
  OSTaskSuspend(TASK_START_PRIO);
}

int main(void)
{
	printf("Start ucosii...\n");
	prvSetupHardware();
	OSInit();
	OSTaskCreate(start_task,NULL,&start_stk[STK_LEN-1],TASK_START_PRIO);
	printf("create start task success \r\n");
	OSStart();
	while(1){}
}


