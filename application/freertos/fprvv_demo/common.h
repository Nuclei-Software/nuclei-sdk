// common.h
// common utilites for the test code under exmaples/
#ifndef __COMMON_H__
#define __COMMON_H__

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"
#include "nmsis_bench.h"
#include "FreeRTOS.h" /* Must come first. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */


typedef float float32_t;
typedef double float64_t;
#define SMP_CTRLREG(base, ofs)      (*(volatile uint32_t *)((uintptr_t)((base) + (ofs))))


static void gen_rand_1d(float *a, int n) {
  for (int i = 0; i < n; ++i)
    a[i] = (float)rand() / (float)RAND_MAX + (float)(rand() % 1000);
}

static void gen_string(char *s, int n) {
  // char value range: -128 ~ 127
  for (int i = 0; i < n - 1; ++i)
    s[i] = (char)(rand() % 127) + 1;
  s[n - 1] = '\0';
}

static void gen_rand_2d(float **ar, int n, int m) {
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      ar[i][j] = (float)rand() / (float)RAND_MAX + (float)(rand() % 1000);
}

static void print_string(const char *a, const char *name) {
  printf("const char *%s = \"", name);
  int i = 0;
  while (a[i] != 0)
    putchar(a[i++]);
  printf("\"\n");
  puts("");
}

static void print_array_1d(float *a, int n, const char *type, const char *name) {
  printf("%s %s[%d] = {\n", type, name, n);
  for (int i = 0; i < n; ++i) {
    printf("%06.2f%s", a[i], i != n - 1 ? "," : "};\n");
    if (i % 10 == 9)
      puts("");
  }
  puts("");
}

static void print_array_2d(float **a, int n, int m, const char *type,
                    const char *name) {
  printf("%s %s[%d][%d] = {\n", type, name, n, m);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      printf("%06.2f", a[i][j]);
      if (j == m - 1)
        puts(i == n - 1 ? "};" : ",");
      else
        putchar(',');
    }
  }
  puts("");
}

static bool float_eq(float golden, float actual, float relErr) {
  return (fabs(actual - golden) < relErr);
}

static bool compare_1d(float *golden, float *actual, int n) {
  for (int i = 0; i < n; ++i) {
    if (!float_eq(golden[i], actual[i], 1e-5)) {
      printf("i = %d golden = %f actual = %f\r\n", i, golden[i], actual[i]);
      return false;
    }
  }
  return true;
}

static bool compare_string(const char *golden, const char *actual, int n) {
  for (int i = 0; i < n; ++i)
    if (golden[i] != actual[i])
      return false;
  return true;
}

static bool compare_2d(float **golden, float **actual, int n, int m) {
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      if (!float_eq(golden[i][j], actual[i][j], 1e-6)) {
          printf("i = %d golden = %f actual = %f\r\n", i, golden[i][j], actual[i][j]);
          return false;
      }
  return true;
}

static float **alloc_array_2d(int n, int m) {
  float **ret;
  ret = (float **)malloc(sizeof(float *) * n);
  for (int i = 0; i < n; ++i)
    ret[i] = (float *)malloc(sizeof(float) * m);
  return ret;
}

static void init_array_one_1d(float *ar, int n) {
  for (int i = 0; i < n; ++i)
    ar[i] = 1;
}

static void init_array_one_2d(float **ar, int n, int m) {
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      ar[i][j] = 1;
}

static int csrr_vlenb()
{
    int a = 0;
    asm volatile("csrr %0, vlenb" : "=r"(a) : : "memory");
    return a;
}

#endif
