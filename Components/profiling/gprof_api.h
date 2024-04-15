/*-
 * Copyright (c) 1982, 1986, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef _GPROF_API_H_
#define _GPROF_API_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


/* profiling frequency, eg. 1000 means 1ms, 10000 means 100us */
#define PROF_HZ             1000


// TODO please customize the lowpc and highpc according to your link script
extern char _text; /* end of text/code symbol, defined by linker */
extern char __etext; /* end of text/code symbol, defined by linker */
#define PROGRAM_LOWPC       (&_text)
#define PROGRAM_HIGHPC      (&__etext)

/* - if interface == 0, it will dump gprof data in buffer called gprof_data
 * - if interface == 1, it will write gmon.out file using open/write api
 * - otherwise it will dump gprof data in console
 */
long gprof_collect(unsigned long interface);

/* Do gprof sample, you can place it in a PROF_HZ period timer interrupt called, the pc should be sampled program pc */
void gprof_sample(unsigned long pc);

void gprof_off(void);
void gprof_on(void);


#ifdef __cplusplus
}
#endif

#endif /* !_GPROF_API_H_ */
