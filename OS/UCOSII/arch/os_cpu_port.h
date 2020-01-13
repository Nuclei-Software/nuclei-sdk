#ifndef _OS_CPU_PORT_H_
#define _OS_CPU_PORT_H_

#include <stdint.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

extern void halt(unsigned long, unsigned long);
extern void syscall_handler(unsigned long sp[]);

/*-----------------------------------------------------------*/
/*System Calls												 */
/*-----------------------------------------------------------*/
//ecall macro used to store argument in a3
#define ECALL(arg)                                                             \
    ({                                                                         \
        register uintptr_t a3 asm("a3") = (uintptr_t)(arg);                    \
        asm volatile("ecall" : "+r"(a3) : : "memory");                         \
        a3;                                                                    \
    })

#define PORT_NO  		40

/*-----------------------------------------------------------*/


/* Scheduler utilities. */
/* the return after the ECALL is VERY important */

#define portYIELD() ECALL(PORT_NO);

#endif
