/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>
#include <sys/types.h>

#undef errno
extern int errno;

__WEAK int _chown(const char *path, uid_t owner, gid_t group)
{
    errno = ENOSYS;
    return -1;
}
