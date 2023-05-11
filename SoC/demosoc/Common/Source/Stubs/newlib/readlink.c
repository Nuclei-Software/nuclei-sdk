/* See LICENSE of license details. */
#include "nuclei_sdk_hal.h"
#include <errno.h>
#include <sys/types.h>

#undef errno
extern int errno;

__WEAK int _readlink(const char *path, char *buf, size_t bufsize)
{
    errno = ENOSYS;
    return -1;
}
