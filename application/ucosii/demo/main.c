#include <stdint.h>
#include <ucos_ii.h>

#include "nuclei_sdk_hal.h"

#define mainQUEUE_LENGTH (1)
static void prvSetupHardware(void);
extern void idle_task(void);

void wait_seconds(uint32_t seconds)
{
    uint64_t start_mtime, delta_mtime;
    uint64_t wait_ticks = ((uint64_t)seconds) * SOC_TIMER_FREQ;

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < wait_ticks);

    printf("-----------------Waited %d seconds.\n", seconds);
}

// Vector interrupt
__INTERRUPT void SOC_BUTTON_1_HANDLER(void)
{
    // save mepc,mcause,msubm enable interrupts
    SAVE_IRQ_CSR_CONTEXT();

    printf("%s", "--------Higher level\n");
    printf("%s", "----Begin button1 handler----Vector mode\n");

    // Green LED toggle
    gpio_toggle(GPIO, SOC_LED_GREEN_GPIO_MASK);

    // Clear the GPIO Pending interrupt by writing 1.
    gpio_clear_interrupt(GPIO, SOC_BUTTON_1_GPIO_OFS, GPIO_INT_RISE);

    //wait_seconds(1); // Wait for a while

    printf("%s", "----End button1 handler\n");

    // disable interrupts,restore mepc,mcause,msubm
    RESTORE_IRQ_CSR_CONTEXT();
}

void SOC_BUTTON_2_HANDLER(void)
{
    printf("%s", "--------Begin button2 handler----NonVector mode\n");

    // Blue LED toggle
    gpio_toggle(GPIO, SOC_LED_BLUE_GPIO_MASK);

    // Clear pending interrupt of Button 2
    gpio_clear_interrupt(GPIO, SOC_BUTTON_2_GPIO_OFS, GPIO_INT_RISE);

    //wait_seconds(1); // Wait for a while

    printf("%s", "--------End button2 handler\n");
}

void board_gpio_init(void)
{
    gpio_enable_input(GPIO, SOC_BUTTON_GPIO_MASK);
    gpio_set_pue(GPIO, SOC_BUTTON_GPIO_MASK, GPIO_BIT_ALL_ONE);

    gpio_enable_output(GPIO, SOC_LED_GPIO_MASK);
    gpio_write(GPIO, SOC_LED_GPIO_MASK, GPIO_BIT_ALL_ZERO);

    // Initialize the button as rising interrupt enabled
    gpio_enable_interrupt(GPIO, SOC_BUTTON_GPIO_MASK, GPIO_INT_RISE);
}

void prvSetupHardware(void)
{
    board_gpio_init();
}

int IRQ_register(void)
{
    int ret;
    printf("register button1 interrupt as vector mode, rising edge and level 3\r\n");
    ret = ECLIC_Register_IRQ(SOC_BUTTON_1_IRQn, ECLIC_VECTOR_INTERRUPT, ECLIC_POSTIVE_EDGE_TRIGGER, 3, 0, SOC_BUTTON_1_HANDLER);
    if (ret < 0) {
        return ret;
    }
    printf("register button2 interrupt as non_vector mode, rising edge and level 2\r\n");
    ret = ECLIC_Register_IRQ(SOC_BUTTON_2_IRQn, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_POSTIVE_EDGE_TRIGGER, 2, 0, SOC_BUTTON_2_HANDLER);
    return ret;
}

#define STK_LEN 1024

OS_STK task1_stk[STK_LEN];
OS_STK task2_stk[STK_LEN];
OS_STK task3_stk[STK_LEN];
OS_STK start_stk[STK_LEN];

#define TASK1_PRIO 13
#define TASK2_PRIO 12
#define TASK3_PRIO 11
#define TASK_START_PRIO 10

void task1(void *args)
{
    int cnt = 0;
    for (;;) {
        cnt++;
        printf("task1 is running... %d\r\n", cnt);
        OSTimeDly(200);
    }
}

void task2(void *args)
{
    int cnt = 0;
    for (;;) {
        cnt++;
        printf("task2 is running... %d\r\n", cnt);
        OSTimeDly(100);
    }
}

void task3(void *args)
{
    int cnt = 0;
    for (;;) {
        cnt++;
        printf("task3 is running... %d\r\n", cnt);
        gpio_toggle(GPIO, SOC_LED_RED_GPIO_MASK);
        OSTimeDly(100);
    }
}

void start_task(void *args)
{
    printf("start all task...\r\n");
    OSTaskCreate(task1, NULL, &task1_stk[STK_LEN - 1], TASK1_PRIO);
    OSTaskCreate(task2, NULL, &task2_stk[STK_LEN - 1], TASK2_PRIO);
    OSTaskCreate(task3, NULL, &task3_stk[STK_LEN - 1], TASK3_PRIO);
    OSTaskSuspend(TASK_START_PRIO);
}

int main(void)
{
    printf("Start ucosii...\r\n");
    prvSetupHardware();
    IRQ_register();
    OSInit();
    OSTaskCreate(start_task, NULL, &start_stk[STK_LEN - 1], TASK_START_PRIO);
    printf("create start task success \r\n");
    OSStart();
    while (1) {
    }
}
