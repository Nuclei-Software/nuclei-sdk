/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

__WEAK void* _sbrk(ptrdiff_t incr)
{
    extern char __heap_start[];
    extern char __heap_end[];
    static char* curbrk = __heap_start;

    if ((curbrk + incr < __heap_start) || (curbrk + incr > __heap_end)) {
        return (void*)(-1);
    }

    curbrk += incr;
    return (void*)(curbrk - incr);
}
