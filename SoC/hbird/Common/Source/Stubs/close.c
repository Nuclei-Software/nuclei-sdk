/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>

#undef errno
extern int errno;

__WEAK int _close(int fd)
{
    errno = EBADF;
    return -1;
}
