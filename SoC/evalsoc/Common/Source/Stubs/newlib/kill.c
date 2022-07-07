/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>
#undef errno
extern int errno;

__WEAK int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}
