/* See LICENSE of license details. */
#include "nuclei_sdk_soc.h"
#include <errno.h>

#undef errno
extern int errno;

int _link(char* old, char* new)
{
    errno = EMLINK;
    return -1;
}
