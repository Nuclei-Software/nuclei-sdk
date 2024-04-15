/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Profiling infrastructure declarations.
 *
 *  This file is based on gcc-internal definitions. Data structures are
 *  defined to be compatible with gcc counterparts. For a better
 *  understanding, refer to gcc source: gcc/gcov-io.h.
 *
 *    Copyright IBM Corp. 2009
 *    Author(s): Peter Oberparleiter <oberpar@linux.vnet.ibm.com>
 *
 *    Uses gcc-internal data definitions.
 */
// Modified based on https://github.com/torvalds/linux/blob/master/kernel/gcov/gcov.h

#ifndef _GCOV_API_H_
#define _GCOV_API_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/*
 * - If interface == 0, it will collect coverage data and store in gcov_data_head
 * - If interface == 1, it will dump gcda files in filesystem using open/write API
 * - otherwise, it will execute gcov_dump() to dump in console */
int gcov_collect(unsigned long interface);

/* Use gcov_dump to dump coverage data in console */
/*
 * console dump format like below
Dump coverage data start

616463672a313342184c8b9e0000000000000001
0c00000066470d790f5ddbb2b88330a40000a101
08000000bfd7fc0000000000000000010c000000
c1e7f66aa629cd8f508939c90000a10110000000
0000000000000000000000000000000000000001
0c000000b5ea01374ab48209508939c90000a101
1000000000000000000000000000000000000000
0100000000000000
C:\Users\hqfang\NucleiStudio_workspace_202402\gcov_example\Debug\application/main.gcda
0000000001000000000000000100000000000000
0b000000000000000b0000000000000001000000
0000000000000000000000000000000000000000
0b000000000000000b0000000000000000000000
0000000000000000000000000b00000000000000
0b00000000000000000000000000000000000000
000000000b000000000000000b00000000000000
0b00000000000000000000000000000000000000
000000000b000000000000000000000000000000
C:\Users\hqfang\NucleiStudio_workspace_202402\gcov_example\Debug\nuclei_sdk/SoC/FreeRTOS/Source/portable/MemMang/heap_4.gcda

Dump coverage data finish
 */
void gcov_dump(void);

#ifdef __cplusplus
}
#endif
#endif /* _GCOV_API_H_ */
