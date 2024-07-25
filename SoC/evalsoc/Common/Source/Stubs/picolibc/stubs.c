/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2022 Keith Packard
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <nuclei_sdk_hal.h>

#undef errno
#ifndef NEWLIB_THREAD_LOCAL_ERRNO
#define NEWLIB_THREAD_LOCAL_ERRNO
#endif
NEWLIB_THREAD_LOCAL_ERRNO int errno;

#undef putchar

int putchar(int dat)
{
    if (dat == '\n') {
        uart_write(SOC_DEBUG_UART, '\r');
    }
    uart_write(SOC_DEBUG_UART, dat);
    return dat;
}

// #define UART_AUTO_ECHO

#undef getchar

int getchar(void)
{
    int dat;

    dat = (int)uart_read(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    uart_write(SOC_DEBUG_UART, (uint8_t)dat);
#endif
    return dat;
}

#ifdef TINY_STDIO

static int __stdio_putc(char c, FILE *file)
{
    (void)file;
    return putchar((int)c);
}

// #define UART_AUTO_ECHO

static int __stdio_getc(FILE *file)
{
    (void)file;
    return getchar();
}

static FILE __stdio =
    FDEV_SETUP_STREAM(__stdio_putc, __stdio_getc, NULL, _FDEV_SETUP_RW);

#ifdef __strong_reference
#define STDIO_ALIAS(x) __strong_reference(stdin, x);
#else
#define STDIO_ALIAS(x) FILE *const x = &__stdio;
#endif

FILE *const stdin = &__stdio;
STDIO_ALIAS(stdout);
STDIO_ALIAS(stderr);

#endif

ssize_t read(int fd, void *buf, size_t count)
{
    (void)fd;
    ssize_t cnt = 0;
    uint8_t* readbuf = (uint8_t*)buf;

    for (cnt = 0; cnt < count; cnt ++) {
        readbuf[cnt] = getchar();
        /* Return partial buffer if we get EOL */
        if (readbuf[cnt] == '\n') {
            return cnt;
        }
    }

    return cnt;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    (void)buf;

    const uint8_t* writebuf = (const uint8_t*)buf;
    for (size_t i = 0; i < count; i++) {
        putchar((int)writebuf[i]);
    }
    return (ssize_t)count;
}


void _ATTRIBUTE((__noreturn__)) _exit(int code)
{
    (void)code;
    while (1) {
    }
}

pid_t getpid(void)
{
    return 1;
}

int kill(pid_t pid, int sig)
{
    if (pid == 1)
        _exit(128 + sig);
    errno = ESRCH;
    return -1;
}

int open(const char *pathname, int flags, ...)
{
    (void)pathname;
    (void)flags;
    return -1;
}

int close(int fd)
{
    (void)fd;
    return 0;
}

off_t lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return (off_t)-1;
}

_off64_t lseek64(int fd, _off64_t offset, int whence)
{
    return (_off64_t)lseek(fd, (off_t)offset, whence);
}

int unlink(const char *pathname)
{
    (void)pathname;
    return 0;
}

int fstat(int fd, struct stat *sbuf)
{
    (void)fd;
    (void)sbuf;
    return -1;
}

int isatty(int fd)
{
    (void)fd;
    return 1;
}

