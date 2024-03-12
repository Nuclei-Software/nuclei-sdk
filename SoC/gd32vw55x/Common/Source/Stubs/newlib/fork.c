/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>

#undef errno
extern int errno;

__WEAK int _fork(void)
{
    errno = ENOSYS;
    return -1;
}
