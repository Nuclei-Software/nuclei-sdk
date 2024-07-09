/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>

#include <nuclei_sdk_hal.h>

#undef errno
extern int errno;

/* Key stub function for uart io via printf/scanf and heap management */
#undef putchar

extern void __usart_putchar(uint32_t usart_periph, uint16_t data);

int putchar(int dat)
{
    if (dat == '\n') {
        __usart_putchar(SOC_DEBUG_UART, '\r');
    }
    __usart_putchar(SOC_DEBUG_UART, dat);
    return dat;
}

__WEAK ssize_t _write(int fd, const void* ptr, size_t len)
{
    if (!isatty(fd)) {
        return -1;
    }

    const uint8_t* writebuf = (const uint8_t*)ptr;
    for (size_t i = 0; i < len; i++) {
        putchar((int)writebuf[i]);
    }
    return len;
}

// #define UART_AUTO_ECHO

extern int __usart_getchar(uint32_t usart_periph);

#undef getchar

int getchar(void)
{
    int dat;

    dat = (int)__usart_getchar(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    __usart_putchar(SOC_DEBUG_UART, (uint16_t)dat);
#endif
    return dat;
}


__WEAK ssize_t _read(int fd, void* ptr, size_t len)
{
    if (fd != STDIN_FILENO) {
        return -1;
    }
    ssize_t cnt = 0;
    uint8_t* readbuf = (uint8_t*)ptr;
    for (cnt = 0; cnt < len; cnt ++) {
        readbuf[cnt] = getchar();
        /* Return partial buffer if we get EOL */
        if (readbuf[cnt] == '\n') {
            return cnt;
        }
    }

    return cnt;
}

__WEAK void* _sbrk(ptrdiff_t incr)
{
    extern char __heap_start[];
    extern char __heap_end[];
    static char* curbrk = __heap_start;

    if ((curbrk + incr < __heap_start) || (curbrk + incr > __heap_end)) {
        return (void*)(-1);
    }

    curbrk += incr;
    return (void*)(curbrk - incr);
}

/* Other newlib stub functions, see https://sourceware.org/newlib/libc.html#Stubs */

int errno;
__WEAK void* __dso_handle = NULL;
/* version of environ for no OS. */
char *__env[1] = { 0 };
char **environ = __env;

/* Get resolution of clock. */
__WEAK int clock_getres(clockid_t clock_id, struct timespec* res)
{
    res->tv_sec = 0;
    res->tv_nsec = 1000000000 / SystemCoreClock;

    return 0;
}

__WEAK int _gettimeofday(struct timeval* tp, void* tzp)
{
    uint64_t cycles;

    cycles = __get_rv_cycle();

    tp->tv_sec = cycles / SystemCoreClock;
    tp->tv_usec = (cycles % SystemCoreClock) * 1000000 / SystemCoreClock;
    return 0;
}

__WEAK int _isatty(int fd)
{
    return 1;
}

__WEAK int _stat(char* file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

__WEAK int _fstat(int file, struct stat* st)
{
    if ((STDOUT_FILENO == file) || (STDERR_FILENO == file)) {
        st->st_mode = S_IFCHR;
        return 0;
    } else {
        errno = EBADF;
        return -1;
    }
}
__WEAK int _chown(const char *path, uid_t owner, gid_t group)
{
    errno = ENOSYS;
    return -1;
}


__WEAK int _open(const char* name, int flags, int mode)
{
    errno = ENOSYS;
    return -1;
}


__WEAK int _lseek(int file, int offset, int whence)
{
    return 0;
}

__WEAK int _link(char* old, char* new)
{
    errno = EMLINK;
    return -1;
}

__WEAK int _getpid(void)
{
    return 1;
}

__WEAK int _close(int fd)
{
    errno = EBADF;
    return -1;
}

__WEAK int _unlink(const char* name)
{
    return -1;
}

__WEAK int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

__WEAK int _symlink(const char *path1, const char *path2)
{
    errno = ENOSYS;
    return -1;
}
/* Supply a definition of errno if one not already provided.  */


__WEAK int _execve(char* name, char** argv, char** env)
{
    errno = ENOMEM;
    return -1;
}

__WEAK int _readlink(const char *path, char *buf, size_t bufsize)
{
    errno = ENOSYS;
    return -1;
}

__WEAK int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}


__WEAK void _exit(int fd)
{
    while (1) {
        __WFI();
    }
}

__WEAK int _wait(int* status)
{
    errno = ECHILD;
    return -1;
}

extern int _gettimeofday(struct timeval* tp, void* tzp);

__WEAK clock_t _times(struct tms* buf)
{
    static struct timeval t0;
    struct timeval t;
    long long utime;

    /* When called for the first time, initialize t0. */
    if (t0.tv_sec == 0 && t0.tv_usec == 0) {
        _gettimeofday(&t0, 0);
    }

    _gettimeofday(&t, 0);

    utime = (t.tv_sec - t0.tv_sec) * 1000000 + (t.tv_usec - t0.tv_usec);
    buf->tms_utime = utime * CLOCKS_PER_SEC / 1000000;
    buf->tms_stime = buf->tms_cstime = buf->tms_cutime = 0;

    return buf->tms_utime;
}

/* Set CLOCK to value TP. */
__WEAK int clock_settime(clockid_t clock_id, const struct timespec* tp)
{
    return -1;
}

/* Get current value of CLOCK and store it in tp.  */
__WEAK int clock_gettime(clockid_t clock_id, struct timespec* tp)
{
    struct timeval tv;
    int retval = -1;

    retval = _gettimeofday(&tv, NULL);
    if (retval == 0) {
        TIMEVAL_TO_TIMESPEC(&tv, tp);
    }

    return retval;
}

