// SPDX-License-Identifier: GPL-2.0
/*
 *  This code provides functions to handle gcc's coverage info format
 *  introduced with gcc 4.7.
 *
 *  This file is based heavily on gcc_3_4.c file.
 *
 *  For a better understanding, refer to gcc source:
 *  gcc/gcov-io.h
 *  libgcc/libgcov.c
 *
 *  Uses gcc-internal data definitions.
 */
// Modified based on https://github.com/torvalds/linux/blob/master/kernel/gcov/gcc_4_7.c

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

//#define DEBUG

#if BITS_PER_LONG >= 64
typedef long gcov_type;
#else
typedef long long gcov_type;
#endif

typedef uint64_t u64;
typedef uint32_t u32;

/*
 * Profiling data types used for gcc 3.4 and above - these are defined by
 * gcc and need to be kept as close to the original definition as possible to
 * remain compatible.
 */
#define GCOV_DATA_MAGIC             ((unsigned int) 0x67636461)
#define GCOV_TAG_FUNCTION           ((unsigned int) 0x01000000)
#define GCOV_TAG_COUNTER_BASE       ((unsigned int) 0x01a10000)
#define GCOV_TAG_FOR_COUNTER(count) \
    (GCOV_TAG_COUNTER_BASE + ((unsigned int) (count) << 17))

#if (__GNUC__ >= 10)
#define GCOV_COUNTERS               8
#elif (__GNUC__ >= 7)
#define GCOV_COUNTERS               9
#elif (__GNUC__ > 5) || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)
#define GCOV_COUNTERS               10
#else
#define GCOV_COUNTERS               9
#endif

#define GCOV_TAG_FUNCTION_LENGTH    3

/* Since GCC 12.1 sizes are in BYTES and not in WORDS (4B). */
#if (__GNUC__ >= 12)
#define GCOV_UNIT_SIZE              4
#else
#define GCOV_UNIT_SIZE              1
#endif

/**
 * struct gcov_ctr_info - information about counters for a single function
 * @num: number of counter values for this type
 * @values: array of counter values for this type
 *
 * This data is generated by gcc during compilation and doesn't change
 * at run-time with the exception of the values array.
 */
struct gcov_ctr_info {
    unsigned int num;
    gcov_type *values;
};

/**
 * struct gcov_fn_info - profiling meta data per function
 * @key: comdat key
 * @ident: unique ident of function
 * @lineno_checksum: function lineo_checksum
 * @cfg_checksum: function cfg checksum
 * @ctrs: instrumented counters
 *
 * This data is generated by gcc during compilation and doesn't change
 * at run-time.
 *
 * Information about a single function.  This uses the trailing array
 * idiom. The number of counters is determined from the merge pointer
 * array in gcov_info.  The key is used to detect which of a set of
 * comdat functions was selected -- it points to the gcov_info object
 * of the object file containing the selected comdat function.
 */
struct gcov_fn_info {
    const struct gcov_info *key;
    unsigned int ident;
    unsigned int lineno_checksum;
    unsigned int cfg_checksum;
    struct gcov_ctr_info ctrs[];
};

/**
 * struct gcov_info - coverage info per object file
 * @version: gcov version magic indicating the gcc version used for compilation
 * @next: list head for a singly-linked list
 * @stamp: uniquifying time stamp
 * @checksum: unique object checksum
 * @filename: name of the associated gcov data file
 * @merge: merge functions (null for unused counter type)
 * @n_functions: number of instrumented functions
 * @functions: pointer to pointers to function information
 *
 * This data is generated by gcc during compilation and doesn't change
 * at run-time with the exception of the next pointer.
 */
struct gcov_info {
    unsigned int version;
    struct gcov_info *next;
    unsigned int stamp;
 /* Since GCC 12.1 a checksum field is added. */
#if (__GNUC__ >= 12)
    unsigned int checksum;
#endif
    const char *filename;
    void (*merge[GCOV_COUNTERS])(gcov_type *, unsigned int);
    unsigned int n_functions;
    struct gcov_fn_info **functions;
};

/**
 * struct gcov_data - analyzed coverage data
 * @next: list head for a singly-linked list
 * @filename: name of the associated gcov data file
 * @buffer: buffer pointer to save gcda data via convert_to_gcda
 * @size: buffer size in bytes
 *
 */
struct gcov_data {
    struct gcov_data *next;
    const char *filename;
    char *buffer;
    size_t size;
};

struct gcov_info *gcov_info_head = NULL;

/** where coverage data stored in after execute gcov_collect(0) */
struct gcov_data *gcov_data_head = NULL;


/**
 * gcov_info_filename - return info filename
 * @info: coverage info set
 */
const char *gcov_info_filename(struct gcov_info *info)
{
    return info->filename;
}

/**
 * gcov_info_version - return info version
 * @info: coverage info set
 */
unsigned int gcov_info_version(struct gcov_info *info)
{
    return info->version;
}

/**
 * gcov_info_next - return next coverage info set
 * @info: coverage info set
 *
 * Returns next gcov_info following @info or first gcov_info in the chain if
 * @info is %NULL.
 */
struct gcov_info *gcov_info_next(struct gcov_info *info)
{
    if (!info)
        return gcov_info_head;

    return info->next;
}

/**
 * gcov_info_link - link/add coverage info set to the list
 * @info: coverage info set
 */
void gcov_info_link(struct gcov_info *info)
{
    info->next = gcov_info_head;
    gcov_info_head = info;
}

/**
 * gcov_info_unlink - unlink/remove coverage info set from the list
 * @prev: previous coverage info set
 * @info: coverage info set
 */
void gcov_info_unlink(struct gcov_info *prev, struct gcov_info *info)
{
    if (prev)
        prev->next = info->next;
    else
        gcov_info_head = info->next;
}

/**
 * gcov_data_link - link/add coverage data set to the list
 * @info: coverage data set
 */
void gcov_data_link(struct gcov_data *data)
{
    data->next = gcov_data_head;
    gcov_data_head = data;
}

/*
 * Determine whether a counter is active. Doesn't change at run-time.
 */
static int counter_active(struct gcov_info *info, unsigned int type)
{
    return info->merge[type] ? 1 : 0;
}

/**
 * store_gcov_u32 - store 32 bit number in gcov format to buffer
 * @buffer: target buffer or NULL
 * @off: offset into the buffer
 * @v: value to be stored
 *
 * Number format defined by gcc: numbers are recorded in the 32 bit
 * unsigned binary form of the endianness of the machine generating the
 * file. Returns the number of bytes stored. If @buffer is %NULL, doesn't
 * store anything.
 */
size_t store_gcov_u32(void *buffer, size_t off, u32 v)
{
    u32 *data;

    if (buffer) {
        data = buffer + off;
        *data = v;
    }

    return sizeof(*data);
}

/**
 * store_gcov_u64 - store 64 bit number in gcov format to buffer
 * @buffer: target buffer or NULL
 * @off: offset into the buffer
 * @v: value to be stored
 *
 * Number format defined by gcc: numbers are recorded in the 32 bit
 * unsigned binary form of the endianness of the machine generating the
 * file. 64 bit numbers are stored as two 32 bit numbers, the low part
 * first. Returns the number of bytes stored. If @buffer is %NULL, doesn't store
 * anything.
 */
size_t store_gcov_u64(void *buffer, size_t off, u64 v)
{
    u32 *data;

    if (buffer) {
        data = buffer + off;

        data[0] = (v & 0xffffffffUL);
        data[1] = (v >> 32);
    }

    return sizeof(*data) * 2;
}

/**
 * convert_to_gcda - convert coverage info set to gcda file format
 * @buffer: the buffer to store file data or %NULL if no data should be stored
 * @info: coverage info set to be converted
 *
 * Returns the number of bytes that were/would have been stored into the buffer.
 */
size_t convert_to_gcda(char *buffer, struct gcov_info *info)
{
    struct gcov_fn_info *fi_ptr;
    struct gcov_ctr_info *ci_ptr;
    unsigned int fi_idx;
    unsigned int ct_idx;
    unsigned int cv_idx;
    size_t pos = 0;

    /* File header. */
    pos += store_gcov_u32(buffer, pos, GCOV_DATA_MAGIC);
    pos += store_gcov_u32(buffer, pos, info->version);
    pos += store_gcov_u32(buffer, pos, info->stamp);

#if (__GNUC__ >= 12)
    /* Use zero as checksum of the compilation unit. */
    pos += store_gcov_u32(buffer, pos, 0);
#endif

    for (fi_idx = 0; fi_idx < info->n_functions; fi_idx++) {
        fi_ptr = info->functions[fi_idx];

        /* Function record. */
        pos += store_gcov_u32(buffer, pos, GCOV_TAG_FUNCTION);
        pos += store_gcov_u32(buffer, pos,
            GCOV_TAG_FUNCTION_LENGTH * GCOV_UNIT_SIZE);
        pos += store_gcov_u32(buffer, pos, fi_ptr->ident);
        pos += store_gcov_u32(buffer, pos, fi_ptr->lineno_checksum);
        pos += store_gcov_u32(buffer, pos, fi_ptr->cfg_checksum);

        ci_ptr = fi_ptr->ctrs;

        for (ct_idx = 0; ct_idx < GCOV_COUNTERS; ct_idx++) {
            if (!counter_active(info, ct_idx))
                continue;

            /* Counter record. */
            pos += store_gcov_u32(buffer, pos,
                          GCOV_TAG_FOR_COUNTER(ct_idx));
            pos += store_gcov_u32(buffer, pos,
                ci_ptr->num * 2 * GCOV_UNIT_SIZE);

            for (cv_idx = 0; cv_idx < ci_ptr->num; cv_idx++) {
                pos += store_gcov_u64(buffer, pos,
                              ci_ptr->values[cv_idx]);
            }

            ci_ptr++;
        }
    }

    return pos;
}

/*
 * These functions may be referenced by gcc-generated profiling code but serve
 * no function for kernel profiling.
 */
void __gcov_flush(void)
{
    /* Unused. */
}

void __gcov_merge_add(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_merge_single(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_merge_delta(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_merge_ior(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_merge_time_profile(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_merge_icall_topn(gcov_type *counters, unsigned int n_counters)
{
    /* Unused. */
}

void __gcov_exit(void)
{
    /* Unused. */
}

/*
 * __gcov_init is called by gcc-generated constructor code for each object
 * file compiled with -fprofile-arcs.
 */
void __gcov_init(struct gcov_info *info)
{
    if (info) {
        gcov_info_link(info);
    }
}

#define NUM_OCTETS_PER_LINE     20
#define FLUSH_OUTPUT()        fflush(stdout)
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

// Modified based on https://github.com/astarasikov/lk/commit/2a4af09a894194dfaff3e05f6fd505241d54d074
static void dump_gcov_info(struct gcov_info *info)
{
    size_t sz = 0;
    char *bufptr = NULL;
    if (!info) {
        return;
    }

    sz = convert_to_gcda(NULL, info);
    bufptr = (char *)malloc(sz);
    if (bufptr == NULL) {
        printf("ERROR: Can't allocate gcda buffer for %s\n", info->filename);
        return;
    }
    sz = convert_to_gcda(bufptr, info);

    hexdumpbuf(bufptr, sz);

    free(bufptr);
    printf("\nCREATE: %s\n", info->filename);
}

void gcov_dump(void)
{
    struct gcov_info *info;

    if (!gcov_info_head) {
        return;
    }

    printf("\nDump coverage data start\n");
    fflush(stdout);

    for (info = gcov_info_head; info != NULL; info = info->next) {
        dump_gcov_info(info);
    }
    printf("\nDump coverage data finish\n");
    fflush(stdout);
}

/**
 * gcov_free - free all coverage data allocated memory
 *
 */
void gcov_free(void)
{
    struct gcov_data *data;
    if (gcov_data_head == NULL) {
        return;
    }
    for (data = gcov_data_head; data != NULL; data = data->next) {
        if (data->buffer) {
            free(data->buffer);
        }
        free(data);
    }
    gcov_data_head = NULL;
}

/**
 * gcov_collect - collect and convert coverage data from gcov_info_head
 *      It need to malloc buffer from heap, so it may fail if your heap is not big enough
 *
 * Return 0, if all coverage data is collected and converted
 */
int gcov_collect(unsigned long interface)
{
    struct gcov_info *info;
    struct gcov_data *data;
    size_t sz = 0, count = 0;
    char *bufptr = NULL;
    int fd = -1;

    // Make sure there are coverage information in it
    if (!gcov_info_head) {
        return -1;
    }

    // if you want to dump in console, just call gcov_dump() function
    if (interface > 1) {
        gcov_dump();
        return 0;
    }

    // Free coverage data if present before
    // to make sure collect latest coverage data
    gcov_free();

    for (info = gcov_info_head; info != NULL; info = info->next) {
        sz = convert_to_gcda(NULL, info);
        bufptr = (char *)malloc(sz);
        data = (struct gcov_data *)malloc(sizeof(struct gcov_data));
        if ((bufptr == NULL) || (data == NULL)) {
            printf("Can't allocate gcda buffer for %s\n", info->filename);
            return -1;
        }
        data->filename = info->filename;
        data->buffer = bufptr;
        data->size = sz;
        convert_to_gcda(bufptr, info);
        gcov_data_link(data);
        if (interface == 1) {
            fd = open(data->filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
            if (fd > 0) {
                printf("Create and store coverage data in %s file\n", data->filename);
                write(fd, (const char*) data->buffer, (size_t)data->size);
                close(fd);
            } else {
                printf("Unable to open %s file\n", data->filename);
            }
        }
        count += 1;
    }
    if (count) {
        printf("%u files coverage data collected, see gcov_data_head=0x%x\n", count, gcov_data_head);
    }

    return 0;
}
