/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>

__WEAK int getpid(void)
{
    return 1;
}
