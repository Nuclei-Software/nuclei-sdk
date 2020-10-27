/* See LICENSE of license details. */

#include <errno.h>
#include <sys/times.h>
#include "nuclei_sdk_soc.h"

int _times(struct tms *buf)
{
    buf->tms_cstime = 0;
    buf->tms_cutime = 0;
    buf->tms_stime = __get_rv_cycle();
    buf->tms_utime = 0;

    return  0;
}
