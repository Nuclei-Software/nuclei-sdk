/*-
 * Copyright (c) 1983, 1992, 1993
 *    The Regents of the University of California.  All rights reserved.
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
 */
// Modified based on
// - https://github.com/bminor/newlib/blob/master/winsup/cygwin/gmon.c
// - https://github.com/bminor/newlib/blob/master/winsup/cygwin/local_includes/gmon.h

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "gprof_api.h"

//#define DEBUG

/*
 * Structure prepended to gmon.out profiling data file.
 */
struct gmonhdr {
    size_t lpc; /* base pc address of sample buffer */
    size_t hpc; /* max pc address of sampled buffer */
    int ncnt; /* size of sample buffer (plus this header) */
    int version; /* version number */
    int profrate; /* profiling clock rate */
    int spare[3]; /* reserved */
};

#define GMONVERSION         0x00051879
/*
 * histogram counters are unsigned shorts (according to the kernel).
 */
#define HISTCOUNTER         unsigned short
/*
 * fraction of text space to allocate for histogram counters here, 1/2
 */
#define HISTFRACTION        2
/*
 * Fraction of text space to allocate for from hash buckets.
 * The value of HASHFRACTION is based on the minimum number of bytes
 * of separation between two subroutine call points in the object code.
 * Given MIN_SUBR_SEPARATION bytes of separation the value of
 * HASHFRACTION is calculated as:
 *
 *    HASHFRACTION = MIN_SUBR_SEPARATION / (2 * sizeof(short) - 1);
 *
 * For example, on the VAX, the shortest two call sequence is:
 *
 *    calls    $0,(r0)
 *    calls    $0,(r0)
 *
 * which is separated by only three bytes, thus HASHFRACTION is
 * calculated as:
 *
 *    HASHFRACTION = 3 / (2 * 2 - 1) = 1
 *
 * Note that the division above rounds down, thus if MIN_SUBR_FRACTION
 * is less than three, this algorithm will not work!
 *
 * In practice, however, call instructions are rarely at a minimal
 * distance.  Hence, we will define HASHFRACTION to be 2 across all
 * architectures.  This saves a reasonable amount of space for
 * profiling data structures without (in practice) sacrificing
 * any granularity.
 */
#define HASHFRACTION        2

/*
 * percent of text space to allocate for tostructs with a minimum.
 */
#define ARCDENSITY          2 /* this is in percentage, relative to text size! */
#define MINARCS             50
#define MAXARCS             ((1 << (8 * sizeof(HISTCOUNTER))) - 2)

struct tostruct {
    size_t selfpc; /* callee address/program counter. The caller address is in froms[] array which points to tos[] array */
    long count; /* how many times it has been called */
    unsigned short link; /* link to next entry in hash table. For tos[0] this points to the last used entry */
    unsigned short pad; /* additional padding bytes, to have entries 4byte aligned */
};

/*
 * a raw arc, with pointers to the calling site and
 * the called site and a count.
 */
struct rawarc {
    size_t raw_frompc;
    size_t raw_selfpc;
    long raw_count;
};

/*
 * general rounding functions.
 */
#define ROUNDDOWN(x,y)      (((x)/(y))*(y))
#define ROUNDUP(x,y)        ((((x)+(y)-1)/(y))*(y))

/*
 * The profiling data structures are housed in this structure.
 */
struct gmonparam {
    int state;
    int already_setup;
    unsigned short *kcount; /* histogram PC sample array */
    size_t kcountsize; /* size of kcount[] array in bytes */
    unsigned short *froms; /* array of hashed 'from' addresses. The 16bit value is an index into the tos[] array */
    size_t fromssize; /* size of froms[] array in bytes */
    struct tostruct *tos; /* to struct, contains histogram counter */
    size_t tossize; /* size of tos[] array in bytes */
    long tolimit;
    size_t lowpc; /* low program counter of area */
    size_t highpc; /* high program counter */
    size_t textsize; /* code size */
    size_t scale;
};

/* gprof data structure */
struct gprofdata {
    char *buf;
    uint32_t size;
};

/* convert an addr to an index */
#define PROFIDX(pc, base, scale)                                \
    ({                                                          \
    size_t i = (pc - base) / 2;                                 \
    if (sizeof (unsigned long long int) > sizeof (size_t)) {    \
        i = (unsigned long long int) i * scale / 65536;         \
    } else {                                                    \
        i = i / 65536 * scale + i % 65536 * scale / 65536;      \
    }                                                           \
    i;                                                          \
})

/* convert an index into an address */
#define PROFADDR(idx, base, scale)                              \
    ((base) + ((((unsigned long long)(idx) << 16)               \
            / (unsigned long long)(scale)) << 1))

/*
 * Possible states of profiling.
 */
#define GMON_PROF_ON            0
#define GMON_PROF_BUSY          1
#define GMON_PROF_ERROR         2
#define GMON_PROF_OFF           3

#define ERR(s)                  write(STDERR_FILENO, s, sizeof(s))


static struct gmonparam _gmonparam = { GMON_PROF_OFF, 0, NULL,
        0, NULL, 0, NULL, 0, 0L, 0, 0, 0, 0 };

/* see profil(2) where this is described (incorrectly) */
#define SCALE_1_TO_1            0x10000L
#define GMONPARAM               (&_gmonparam)

/* Where the gprof data stored after execute gprof_collect(0) */
struct gprofdata gprof_data = {NULL, 0};

/*
 * Control profiling
 *    profiling is what mcount checks to see if
 *    all the data structures are ready.
 */
static void moncontrol(int mode)
{
    struct gmonparam *p = GMONPARAM;

    if (mode) {
        /* start */
        gprof_on();
        p->state = GMON_PROF_ON;
    } else {
        /* stop */
        gprof_off();
        p->state = GMON_PROF_OFF;
    }
}

static void monstartup(size_t lowpc, size_t highpc)
{
    register size_t o;
    char *cp;
    struct gmonparam *p = GMONPARAM;

    /*
     * round lowpc and highpc to multiples of the density we're using
     * so the rest of the scaling (here and in gprof) stays in ints.
     */
    p->lowpc = ROUNDDOWN(lowpc, HISTFRACTION * sizeof(HISTCOUNTER));
    p->highpc = ROUNDUP(highpc, HISTFRACTION * sizeof(HISTCOUNTER));
    p->textsize = p->highpc - p->lowpc;
    p->kcountsize = p->textsize / HISTFRACTION;
    p->fromssize = p->textsize / HASHFRACTION;
    p->tolimit = p->textsize * ARCDENSITY / 100;
    if (p->tolimit < MINARCS) {
        p->tolimit = MINARCS;
    } else if (p->tolimit > MAXARCS) {
        p->tolimit = MAXARCS;
    }
    p->tossize = p->tolimit * sizeof(struct tostruct);

    cp = malloc(p->kcountsize + p->fromssize + p->tossize);
    if (cp == (char*) NULL) {
        p->state = GMON_PROF_ERROR;
        ERR("monstartup: out of memory\n");
        return;
    }

    /* zero out cp as value will be added there */
    memset(cp, 0, p->kcountsize + p->fromssize + p->tossize);

    p->tos = (struct tostruct*) cp;
    cp += p->tossize;
    p->kcount = (unsigned short*) cp;
    cp += p->kcountsize;
    p->froms = (unsigned short*) cp;

    p->tos[0].link = 0;

    o = p->highpc - p->lowpc;
    if (p->kcountsize < o) {
#ifndef notdef
        p->scale = ((float) p->kcountsize / o) * SCALE_1_TO_1;
#else /* avoid floating point */
        int quot = o / p->kcountsize;

        if (quot >= 0x10000)
            p->scale = 1;
        else if (quot >= 0x100)
            p->scale = 0x10000 / quot;
        else if (o >= 0x800000)
            p->scale = 0x1000000 / (o / (p->kcountsize >> 8));
        else
            p->scale = 0x1000000 / ((o << 8) / p->kcountsize);
#endif
    } else {
        p->scale = SCALE_1_TO_1;
    }
    moncontrol(1); /* start */
}

void _mcount(uint32_t *frompcindex)
{
    register uint32_t *selfpc asm("ra");
    register struct tostruct *top;
    register struct tostruct *prevtop;
    register long toindex;
    struct gmonparam *p = GMONPARAM;

    if (!p->already_setup) {
        p->already_setup = 1;
        monstartup((size_t) PROGRAM_LOWPC, (size_t) PROGRAM_HIGHPC);
    }
    /*
     *    check that we are profiling
     *    and that we aren't recursively invoked.
     */
    if (p->state != GMON_PROF_ON) {
        goto out;
    }
    p->state++;
    /*
     *  check that frompcindex is a reasonable pc value.
     *  for example: signal catchers get called from the stack,
     *      not from text space.  too bad.
     */
    frompcindex = (uint32_t*) ((unsigned long) frompcindex - (unsigned long) p->lowpc);
    if ((unsigned long) frompcindex > p->textsize) {
        goto done;
    }
    frompcindex = (uint32_t*) &p->froms[((unsigned long) frompcindex)
            / (HASHFRACTION * sizeof(*p->froms))];
    toindex = *((unsigned short*) frompcindex); /* get froms[] value */
    if (toindex == 0) {
        /*
         *  first time traversing this arc
         */
        toindex = ++p->tos[0].link; /* the link of tos[0] points to the last used record in the array */
        if (toindex >= p->tolimit) { /* more tos[] entries than we can handle! */
            goto overflow;
        }
        *((unsigned short*) frompcindex) = (unsigned short) toindex; /* store new 'to' value into froms[] */
        top = &p->tos[toindex];
        top->selfpc = (size_t) selfpc;
        top->count = 1;
        top->link = 0;
        goto done;
    }
    top = &p->tos[toindex];
    if (top->selfpc == (size_t) selfpc) {
        /*
         * arc at front of chain; usual case.
         */
        top->count++;
        goto done;
    }
    /*
     * have to go looking down chain for it.
     * top points to what we are looking at,
     * prevtop points to previous top.
     * we know it is not at the head of the chain.
     */
    for (; /* goto done */;) {
        if (top->link == 0) {
            /*
             * top is end of the chain and none of the chain
             * had top->selfpc == selfpc.
             * so we allocate a new tostruct
             * and link it to the head of the chain.
             */
            toindex = ++p->tos[0].link;
            if (toindex >= p->tolimit) {
                goto overflow;
            }
            top = &p->tos[toindex];
            top->selfpc = (size_t) selfpc;
            top->count = 1;
            top->link = *((unsigned short*) frompcindex);
            *(unsigned short*) frompcindex = (unsigned short) toindex;
            goto done;
        }
        /*
         * otherwise, check the next arc on the chain.
         */
        prevtop = top;
        top = &p->tos[top->link];
        if (top->selfpc == (size_t) selfpc) {
            /*
             * there it is.
             * increment its count
             * move it to the head of the chain.
             */
            top->count++;
            toindex = prevtop->link;
            prevtop->link = top->link;
            top->link = *((unsigned short*) frompcindex);
            *((unsigned short*) frompcindex) = (unsigned short) toindex;
            goto done;
        }
    }
    done: p->state--;
    /* and fall through */
    out: return; /* normal return restores saved registers */
    overflow: p->state++; /* halt further profiling */
#define TOLIMIT         "mcount: tos overflow\n"
    write(STDOUT_FILENO, TOLIMIT, sizeof(TOLIMIT));
    goto out;
}

#define NUM_OCTETS_PER_LINE 20
#define FLUSH_OUTPUT()      fflush(stdout)
static void hexdumpbuf(char *buf, unsigned long sz)
{
    unsigned long rem, cur = 0, i = 0;

    FLUSH_OUTPUT();

    while (cur < sz) {
        rem = ((sz - cur) < NUM_OCTETS_PER_LINE) ? (sz - cur) : NUM_OCTETS_PER_LINE;
        for (i = 0; i < rem; i++) {
            printf("%02x", buf[cur + i]);
        }
        printf("\n");
        FLUSH_OUTPUT();
        cur += rem;
    }
}

long gprof_collect(unsigned long interface)
{
    static const char gmon_out[] = "gmon.out";
    int fd = -1;
    int hz;
    int fromindex;
    int endfrom;
    size_t frompc;
    int toindex;
    struct rawarc rawarc;
    struct gmonparam *p = GMONPARAM;
    struct gmonhdr gmonhdr, *hdr;
    const char *proffile;
    char *bufptr;
#ifdef DEBUG
    int log, len;
    char dbuf[200];
#endif

    if (p->state == GMON_PROF_ERROR) {
        ERR("_mcleanup: tos overflow\n");
        return -1;
    }
    hz = PROF_HZ;
    moncontrol(0); /* stop */
    if (interface == 0) {
        gprof_data.size = 0;
        if (gprof_data.buf == NULL) {
            gprof_data.buf = malloc(sizeof(gmonhdr) + p->kcountsize + p->tolimit * sizeof(struct rawarc));
            if (gprof_data.buf == NULL) {
                ERR("gprof_collect: unable to malloc enough memory to store gprof data\n");
                return -1;
            }
        }
        bufptr = gprof_data.buf;
    } else if (interface == 1) {
        proffile = gmon_out;
        fd = open(proffile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if (fd < 0) {
            printf("Unable to open %s\n", proffile);
            return -1;
        }
    } else {
        printf("\nDump profiling data start\n");
    }

#ifdef DEBUG
    len = sprintf(dbuf, "[mcleanup1] kcount 0x%x ssiz %d\n",
            p->kcount, p->kcountsize);
    write(STDOUT_FILENO, dbuf, len);
#endif

    hdr = (struct gmonhdr*) &gmonhdr;
    hdr->lpc = p->lowpc;
    hdr->hpc = p->highpc;
    hdr->ncnt = p->kcountsize + sizeof(gmonhdr);
    hdr->version = GMONVERSION;
    hdr->profrate = hz;
    if (interface == 0) {
        memcpy(bufptr, (void*) hdr, sizeof *hdr);
        bufptr += sizeof *hdr;
        memcpy(bufptr, (void*) p->kcount, p->kcountsize);
        bufptr += p->kcountsize;
    } else if (interface == 1) {
        write(fd, (const char*) hdr, sizeof *hdr);
        write(fd, (const char*) p->kcount, p->kcountsize);
    } else {
        hexdumpbuf((char*) hdr, sizeof *hdr);
        hexdumpbuf((char *)p->kcount, (unsigned long)p->kcountsize);
    }

#ifdef DEBUG
    len = sprintf(dbuf, "[mcleanup1] lowpc 0x%lx highpc 0x%lx ncnt %d\n",
                hdr->lpc, hdr->hpc, hdr->ncnt);
    write(STDOUT_FILENO, dbuf, len);
#endif
    endfrom = p->fromssize / sizeof(*p->froms);
    for (fromindex = 0; fromindex < endfrom; fromindex++) {
        if (p->froms[fromindex] == 0) {
            continue;
        }
        frompc = p->lowpc;
        frompc += fromindex * HASHFRACTION * sizeof(*p->froms);
        for (toindex = p->froms[fromindex]; toindex != 0; toindex =
                p->tos[toindex].link) {
#ifdef DEBUG
            len = sprintf(dbuf,
            "[mcleanup2] frompc 0x%x selfpc 0x%x count %d\n" ,
                frompc, p->tos[toindex].selfpc,
                p->tos[toindex].count);
            write(STDOUT_FILENO, dbuf, len);
#endif
            rawarc.raw_frompc = frompc;
            rawarc.raw_selfpc = p->tos[toindex].selfpc;
            rawarc.raw_count = p->tos[toindex].count;
            if (interface == 0) {
                memcpy(bufptr, (void*) &rawarc, sizeof rawarc);
                bufptr += sizeof rawarc;
            } else if (interface == 1) {
                write(fd, (const char*)&rawarc, sizeof rawarc);
            } else {
                hexdumpbuf((char *)(&rawarc), (unsigned long)(sizeof rawarc));
            }
        }
    }
    if (interface == 0) {
        gprof_data.size = bufptr - gprof_data.buf;
        printf("Collected gprof data @0x%lx, size %lu bytes\n", gprof_data.buf, gprof_data.size);
    } else if (interface == 1) {
        close(fd);
        printf("Write %s done!\n", gmon_out);
    } else {
        printf("\nCREATE: %s\n", gmon_out);
        printf("\nDump profiling data finished\n");
    }
    return 0;
}

/* sample the current program counter */
void gprof_sample(unsigned long pc)
{
    size_t idx;
    struct gmonparam *p = GMONPARAM;

    if (p->state == GMON_PROF_ON) {
        if (pc >= p->lowpc && pc < p->highpc) {
            idx = PROFIDX(pc, p->lowpc, p->scale);
            p->kcount[idx]++;
        }
    }
}
