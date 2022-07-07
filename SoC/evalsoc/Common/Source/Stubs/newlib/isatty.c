/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <unistd.h>

__WEAK int _isatty(int fd)
{
    return 1;
}
