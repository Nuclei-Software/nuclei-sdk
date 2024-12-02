/* Small demonstration of the ThreadX module manager.  */

#include "tx_api.h"
#include "txm_module.h"
#include <stdio.h>

#define DEMO_STACK_SIZE         1024

/* Define the ThreadX object control blocks...  */

TX_THREAD               module_manager;
TXM_MODULE_INSTANCE     my_module;


/* Define the object pool area.  */

UCHAR                   object_memory[16384];
UCHAR                   module_ram[60000];

/* Define the count of memory faults.  */

ULONG                   memory_faults;

#define DEMO_BYTE_POOL_SIZE     10240
// This is an memory area used by ThreadX to allocate for task stacks and etc
UCHAR                   memory_area[DEMO_BYTE_POOL_SIZE];


/* Define thread prototypes.  */

void    module_manager_entry(ULONG thread_input);


/* Define fault handler.  */

VOID module_fault_handler(TX_THREAD *thread, TXM_MODULE_INSTANCE *module)
{

    /* Just increment the fault counter.   */
    memory_faults++;
}

/* Define main entry point.  */

int main()
{

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = (CHAR*)first_unused_memory;

    pointer = &memory_area[0];
    tx_thread_create(&module_manager, "Module Manager Thread", module_manager_entry, 0,
                     pointer, DEMO_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;
}




/* Define the test threads.  */

void    module_manager_entry(ULONG thread_input)
{
    uint32_t i = 0;
    ULONG *thread_6_counter;

    /* Initialize the module manager.   */
    txm_module_manager_initialize((VOID *) module_ram, sizeof(module_ram));

    txm_module_manager_object_pool_create(object_memory, sizeof(object_memory));

    /* Register a fault handler.  */
    txm_module_manager_memory_fault_notify(module_fault_handler);

    /* Load the module that is already there, in this example it is placed there by the multiple image download.  */
    txm_module_manager_memory_load(&my_module, "my module", (VOID *) 0xA0400000);

    /* Enable 128 byte read/write shared memory region at 0xA0500000.  */
    txm_module_manager_external_memory_enable(&my_module, (void *) 0xA0500000, 128, TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_WRITE);

    /* Start the module.  */
    txm_module_manager_start(&my_module);

    /* Sleep for a while....  */
    tx_thread_sleep(100);

    thread_6_counter = (ULONG *) ((ULONG)my_module.txm_module_instance_module_data_base_address + 6 * sizeof(unsigned long));
    printf("Memory loaded module executed, thread_6_counter %d!\n", *thread_6_counter);

    /* Stop the module.  */
    txm_module_manager_stop(&my_module);

    /* Unload the module.  */
    txm_module_manager_unload(&my_module);

    /* Load the module that is already there.  */
    // TODO in place load still require code got table area is writable, since we need to update got table because data location is changed
    txm_module_manager_in_place_load(&my_module, "my module", (VOID *) 0xA0400000);

    /* Start the module again.  */
    txm_module_manager_start(&my_module);

    // thread_6_counter for 0x18(rv32)/0x30(rv64) offset to .bss section of threadx module app
    thread_6_counter = (ULONG *) ((ULONG)my_module.txm_module_instance_module_data_base_address + 6 * sizeof(unsigned long));
    /* Now just spin...  */
    while(1)
    {
        printf("Hello world from the module manager demo %d, thread_6_counter %d!\n", i, *thread_6_counter);

        tx_thread_sleep(10);
        i ++;
    }
}
