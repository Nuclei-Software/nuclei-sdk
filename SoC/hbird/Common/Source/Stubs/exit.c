/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"

__WEAK void _exit(int fd)
{
    while(1) {
        __WFI();
    }
}
