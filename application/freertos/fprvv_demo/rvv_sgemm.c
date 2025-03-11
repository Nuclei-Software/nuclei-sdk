#include <stdio.h>
#include <stdbool.h>
#include "nuclei_sdk_hal.h"
#include <riscv_vector.h>

#define DISABLE_NMSIS_BENCH
#include <nmsis_bench.h>
#include <common.h>

/* The example is : C(M, N) = A(M, K) x  B(K, N)
 * The ROW_MAJOR is exactly equal to COLUMN_MAJOR, default mode is ROW_MAJOR order
 */

#define ROW_MAJOR
//#define COLUMN_MAJOR

#define M 32
#define K 32
#define N 32

#define OUTPUT_LEN (M * N)

BENCH_DECLARE_VAR();


#ifdef ROW_MAJOR
// c = a*b row-major order
void sgemm_opt_golden(int32_t m, int32_t n, int32_t k, float *a, int32_t lda,
               float *b, int ldb,
               float *c, int ldc)
{
    float sum;

    for (int32_t ii = 0; ii < m; ++ii)
    {
        for (int32_t jj = 0; jj < n; ++jj)
        {
            sum = 0;
            for (int32_t kk = 0; kk < k; ++kk)
            {
                sum += a[ii * lda + kk] * b[kk * ldb + jj];
            }
            c[ii * ldc + jj] = sum;
        }
    }
}


#pragma GCC push_options
#pragma GCC optimize("no-unroll-loops")
// c = a*b row-major order
void sgemm_opt_rvv(size_t m, size_t n, size_t k,
               const float *a, // m * k matrix
               size_t lda,
               const float *b, // k * n matrix
               size_t ldb,
               float *c, // m * n matrix
               size_t ldc) {

    const float *pInA = a;              /* Input data matrix pointer A */
    const float *pInB = b;              /* Input data matrix pointer B */
    float *px = c;                      /* Temporary output data matrix pointer */
    size_t ii, jj, kk;                  /* How many rowumns and ii are controlled */

    size_t l;
    vfloat32m4_t va0m4, vres0m4, vres1m4, vres2m4, vres3m4;
    vfloat32m8_t va0m8, vres0m8, vres1m8;
    /* ch = 4, mul = 4 */
    for (jj = m / 4; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m4(ii);

            pInA = a;

            vres0m4 = __riscv_vfmv_v_f_f32m4(0.0, l);
            vres1m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            vres2m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            vres3m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            for (kk = 0; kk < k; kk++) {
                va0m4 = __riscv_vle32_v_f32m4(pInB + kk * ldb, l);
                vres0m4 = __riscv_vfmacc_vf_f32m4(vres0m4, *(pInA + 0), va0m4, l);
                vres1m4 = __riscv_vfmacc_vf_f32m4(vres1m4, *(pInA + lda), va0m4, l);
                //vTaskDelay(5);
                vres2m4 = __riscv_vfmacc_vf_f32m4(vres2m4, *(pInA + 2 * lda), va0m4, l);
                vres3m4 = __riscv_vfmacc_vf_f32m4(vres3m4, *(pInA + 3 * lda), va0m4, l);
                pInA++;
            }
            __riscv_vse32_v_f32m4(px, vres0m4, l);
            __riscv_vse32_v_f32m4(px + ldc, vres1m4, l);
            __riscv_vse32_v_f32m4(px + 2 * ldc, vres2m4, l);
            __riscv_vse32_v_f32m4(px + 3 * ldc, vres3m4, l);
            px += l;
            pInB += l;
        }
        a += (lda << 2);
        c += (ldc << 2);
    }
    /* ch = 2, mul = 8 */
    m = m & 3;
    for (jj = m / 2; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m8(ii);

            pInA = a;

            vres0m8 = __riscv_vfmv_v_f_f32m8(0.0, l);
            vres1m8 = __riscv_vmv_v_v_f32m8(vres0m8, l);
            for (kk = 0; kk < k; kk++) {
                va0m8 = __riscv_vle32_v_f32m8(pInB + kk * ldb, l);
                vres0m8 = __riscv_vfmacc_vf_f32m8(vres0m8, *(pInA + 0), va0m8, l);
                vres1m8 = __riscv_vfmacc_vf_f32m8(vres1m8, *(pInA + lda), va0m8, l);
                pInA++;
            }
            __riscv_vse32_v_f32m8(px, vres0m8, l);
            __riscv_vse32_v_f32m8(px + ldc, vres1m8, l);
            px += l;
            pInB += l;
        }
        a += (lda << 1);
        c += (ldc << 1);
    }
    /* ch = 1, mul = 8 */
    m = m & 1;
    for (jj = m; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m8(ii);
            pInA = a;
            vres0m8 = __riscv_vfmv_v_f_f32m8(0.0, l);
            for (kk = 0; kk < k; kk++) {
                va0m8 = __riscv_vle32_v_f32m8(pInB + kk * ldb, l);
                vres0m8 = __riscv_vfmacc_vf_f32m8(vres0m8, *(pInA++), va0m8, l);
            }
            __riscv_vse32_v_f32m8(px, vres0m8, l);
            px += l;
            pInB += l;
        }
        a += (lda);
        c += (ldc);
    }
  return;
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize("no-unroll-loops")
// c = a*b row-major order
void sgemm_opt_rvv_m1(size_t m, size_t n, size_t k,
               const float *a, // m * k matrix
               size_t lda,
               const float *b, // k * n matrix
               size_t ldb,
               float *c,       // m * n matrix
               size_t ldc) {

    const float *pInA = a;              /* Input data matrix pointer A */
    const float *pInB = b;              /* Input data matrix pointer B */
    float *px = c;                      /* Temporary output data matrix pointer */
    size_t ii, jj, kk;                  /* How many rowumns and ii are controlled */

    size_t l;
    vfloat32m1_t va0m1, vres0m1, vres1m1, vres2m1, vres3m1;
    /* ch = 4, mul = 4 */
    for (jj = m / 4; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m1(ii);

            pInA = a;

            vres0m1 = __riscv_vfmv_v_f_f32m1(0.0, l);
            vres1m1 = __riscv_vmv_v_v_f32m1(vres0m1, l);
            vres2m1 = __riscv_vmv_v_v_f32m1(vres0m1, l);
            vres3m1 = __riscv_vmv_v_v_f32m1(vres0m1, l);
            for (kk = 0; kk < k; kk++) {
                va0m1 = __riscv_vle32_v_f32m1(pInB + kk * ldb, l);
                vres0m1 = __riscv_vfmacc_vf_f32m1(vres0m1, *(pInA + 0), va0m1, l);
                vres1m1 = __riscv_vfmacc_vf_f32m1(vres1m1, *(pInA + lda), va0m1, l);
                vres2m1 = __riscv_vfmacc_vf_f32m1(vres2m1, *(pInA + 2 * lda), va0m1, l);
                vres3m1 = __riscv_vfmacc_vf_f32m1(vres3m1, *(pInA + 3 * lda), va0m1, l);
                pInA++;
            }
            __riscv_vse32_v_f32m1(px, vres0m1, l);
            __riscv_vse32_v_f32m1(px + ldc, vres1m1, l);
            __riscv_vse32_v_f32m1(px + 2 * ldc, vres2m1, l);
            __riscv_vse32_v_f32m1(px + 3 * ldc, vres3m1, l);
            px += l;
            pInB += l;
        }
        a += (lda << 2);
        c += (ldc << 2);
    }
    /* ch = 2, mul = 8 */
    m = m & 3;
    for (jj = m / 2; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m1(ii);

            pInA = a;

            vres0m1 = __riscv_vfmv_v_f_f32m1(0.0, l);
            vres1m1 = __riscv_vmv_v_v_f32m1(vres0m1, l);
            for (kk = 0; kk < k; kk++) {
                va0m1 = __riscv_vle32_v_f32m1(pInB + kk * ldb, l);
                vres0m1 = __riscv_vfmacc_vf_f32m1(vres0m1, *(pInA + 0), va0m1, l);
                vres1m1 = __riscv_vfmacc_vf_f32m1(vres1m1, *(pInA + lda), va0m1, l);
                pInA++;
            }
            __riscv_vse32_v_f32m1(px, vres0m1, l);
            __riscv_vse32_v_f32m1(px + ldc, vres1m1, l);
            px += l;
            pInB += l;
        }
        a += (lda << 1);
        c += (ldc << 1);
    }
    /* ch = 1, mul = 8 */
    m = m & 1;
    for (jj = m; jj > 0; jj--) {
        px = c;
        pInB = b;

        for (ii = n; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m1(ii);
            pInA = a;
            vres0m1 = __riscv_vfmv_v_f_f32m1(0.0, l);
            for (kk = 0; kk < k; kk++) {
                va0m1 = __riscv_vle32_v_f32m1(pInB + kk * ldb, l);
                vres0m1 = __riscv_vfmacc_vf_f32m1(vres0m1, *(pInA++), va0m1, l);
            }
            __riscv_vse32_v_f32m1(px, vres0m1, l);
            px += l;
            pInB += l;
        }
        a += (lda);
        c += (ldc);
    }
  return;
}
#pragma GCC pop_options


#endif /* ROW_MAJOR */

#ifdef COLUMN_MAJOR
// c = a*b column-major order
void sgemm_opt_golden(int32_t m, int32_t n, int32_t k, float *a, int32_t lda,
               float *b, int ldb,
               float *c, int ldc)
{
    float sum;

    for (int32_t jj = 0; jj < n; ++jj)
    {
        for (int32_t ii = 0; ii < m; ++ii)
        {
            sum = 0;
            for (int32_t kk = 0; kk < k; ++kk)
            {
                sum += a[kk * lda + ii] * b[jj * ldb + kk];
            }
            c[jj * ldc + ii] = sum;
        }
    }
}

#pragma GCC push_options
#pragma GCC optimize("no-unroll-loops")
// c = a*b column-major order
void sgemm_opt_rvv(size_t m, size_t n, size_t k,
               const float *a, // m * k matrix
               size_t lda,
               const float *b, // k * n matrix
               size_t ldb,
               float *c, // m * n matrix
               size_t ldc) {

    const float *pInA = a;              /* Input data matrix pointer A */
    const float *pInB = b;              /* Input data matrix pointer B */
    float *px = c;                      /* Temporary output data matrix pointer */
    size_t ii, jj, kk;                  //  How many rowumns and ii are controlled

    size_t l;
    vfloat32m4_t va0m4, vres0m4, vres1m4, vres2m4, vres3m4;
    vfloat32m8_t va0m8, vres0m8, vres1m8;
    /* ch = 4, mul = 4 */
    for (jj = n / 4; jj > 0; jj--) {
        px = c;
        pInA = a;

        for (ii = m; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m4(ii);

            pInB = b;

            vres0m4 = __riscv_vfmv_v_f_f32m4(0.0, l);
            vres1m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            vres2m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            vres3m4 = __riscv_vmv_v_v_f32m4(vres0m4, l);
            for (kk = k; kk > 0; kk--) {
                va0m4 = __riscv_vle32_v_f32m4(pInA, l);
                vres0m4 = __riscv_vfmacc_vf_f32m4(vres0m4, *(pInB + 0), va0m4, l);
                vres1m4 = __riscv_vfmacc_vf_f32m4(vres1m4, *(pInB + ldb), va0m4, l);
                vres2m4 = __riscv_vfmacc_vf_f32m4(vres2m4, *(pInB + 2 * ldb), va0m4, l);
                vres3m4 = __riscv_vfmacc_vf_f32m4(vres3m4, *(pInB + 3 * ldb), va0m4, l);
                pInA += lda;
                pInB += 1;

            }
            __riscv_vse32_v_f32m4(px, vres0m4, l);
            __riscv_vse32_v_f32m4(px + ldc, vres1m4, l);
            __riscv_vse32_v_f32m4(px + 2 * ldc, vres2m4, l);
            __riscv_vse32_v_f32m4(px + 3 * ldc, vres3m4, l);
            px += l;
            pInA = a + m - ii + l;
        }
        b += (ldb << 2);
        c += (ldc << 2);
    }
    /* ch = 2, mul = 8 */
    n = n & 3;
    for (jj = n / 2; jj > 0; jj--) {
        px = c;
        pInA = a;

        for (ii = m; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m8(ii);

            pInB = b;

            vres0m8 = __riscv_vfmv_v_f_f32m8(0.0, l);
            vres1m8 = __riscv_vmv_v_v_f32m8(vres0m8, l);
            for (kk = k; kk > 0; kk--) {
                va0m8 = __riscv_vle32_v_f32m8(pInA, l);
                vres0m8 = __riscv_vfmacc_vf_f32m8(vres0m8, *(pInB + 0), va0m8, l);
                vres1m8 = __riscv_vfmacc_vf_f32m8(vres1m8, *(pInB + ldb), va0m8, l);
                pInA += lda;
                pInB += 1;

            }
            __riscv_vse32_v_f32m8(px, vres0m8, l);
            __riscv_vse32_v_f32m8(px + ldc, vres1m8, l);
            px += l;
            pInA = a + m - ii + l;
        }
        b += (ldb << 1);
        c += (ldc << 1);
    }
    /* ch = 1, mul = 8 */
    n = n & 1;
    for (jj = n; jj > 0; jj--) {
        px = c;
        pInA = a;

        for (ii = m; ii > 0; ii -= l) {
            l = __riscv_vsetvl_e32m8(ii);
            pInB = b;
            vres0m8 = __riscv_vfmv_v_f_f32m8(0.0, l);
            for (kk = k; kk > 0; kk--) {
                va0m8 = __riscv_vle32_v_f32m8(pInA, l);
                vres0m8 = __riscv_vfmacc_vf_f32m8(vres0m8, *(pInB + 0), va0m8, l);
                pInA += lda;
                pInB += 1;
            }
            __riscv_vse32_v_f32m8(px, vres0m8, l);
            px += l;
            pInA = a + m - ii + l;
        }
        b += (ldb);
        c += (ldc);
    }
  return;
}
#pragma GCC pop_options

#endif /* COLUMN_MAJOR */


int do_sgemm() {
  const uint32_t seed = 0xdeadbeef;
  uint32_t vlen = csrr_vlenb() * 8;
  srand(seed);

  BENCH_INIT();

  float *a_array;
  float *b_array;
  float *golden;
  float *actual;

  vTaskSuspendAll();
  a_array = (float *)malloc(M*K*sizeof(float));
  b_array = (float *)malloc(N*K*sizeof(float));
  golden = (float *)malloc(OUTPUT_LEN*sizeof(float));
  actual = (float *)malloc(OUTPUT_LEN*sizeof(float));
  ( void  ) xTaskResumeAll();

  if (!(a_array && b_array && golden && actual)) {
      printf("Malloc required memory failed for sgemm case!\n");
      return -1;
  }
  gen_rand_1d(a_array, M * K);
  gen_rand_1d(b_array, K * M);

 // printf("vlen = %d bits\n", vlen); // vlen = vlenb * 8
 // printf("The example is : C(%d, %d) = A(%d, %d) x  B(%d, %d)\n", M, N, M, K, K, N);

#ifdef ROW_MAJOR
  // golden
  BENCH_START(sgemm_opt_golden);
  sgemm_opt_golden(M, N, K, a_array, K, b_array, N, golden, N);
  BENCH_END(sgemm_opt_golden);
  /* 1. vlen = 128 or vlen = 256, lmul = 4
   * 2. vlen = 512, datalen <= 16 (512/32), lmul = 1; datalen > 16, lmul = 4
   * 3. vlen = 1024, datalen <= 32 (1024/32), lmul = 1; datalen > 32, lmul = 4
   */
  if ((vlen >= 512) && (N <= vlen / 32)) {
    // vector
    BENCH_START(sgemm_opt_rvv);
    sgemm_opt_rvv_m1(M, N, K, a_array, K, b_array, N, actual, N);
    BENCH_END(sgemm_opt_rvv);
  } else {
    // vector
    BENCH_START(sgemm_opt_rvv);
    sgemm_opt_rvv(M, N, K, a_array, K, b_array, N, actual, N);
    BENCH_END(sgemm_opt_rvv);
  }
  // compare
  bool ret = compare_1d(golden, actual, OUTPUT_LEN);
#endif

#ifdef COLUMN_MAJOR
  // golden
  BENCH_START(sgemm_opt_golden);
  sgemm_opt_golden(M, N, K, a_array, M, b_array, K, golden, M);
  BENCH_END(sgemm_opt_golden);

  // vector
  BENCH_START(sgemm_opt_rvv);
  sgemm_opt_rvv(M, N, K, a_array, M, b_array, K, actual, M);
  BENCH_END(sgemm_opt_rvv);

  // compare
  bool ret = compare_1d(golden, actual, OUTPUT_LEN);
#endif

  vTaskSuspendAll();
   free(golden);
   free(actual);
   free(a_array);
   free(b_array);
  ( void  ) xTaskResumeAll();
  if (ret) {
//    printf("pass\n");
    return 0;
  } else {
//    printf("fail\n");
    return -1;
  }

}

int do_sgemm_c() {
  const uint32_t seed = 0xdeadbeef;
  srand(seed);

  float *a_array;
  float *b_array;
  float *golden;
  float *actual;

  vTaskSuspendAll();
  a_array = (float *)malloc(M*K*sizeof(float));
  b_array = (float *)malloc(N*K*sizeof(float));
  golden = (float *)malloc(OUTPUT_LEN*sizeof(float));
  actual = (float *)malloc(OUTPUT_LEN*sizeof(float));
  ( void  ) xTaskResumeAll();

  if (!(a_array && b_array && golden && actual)) {
      printf("Malloc required memory failed for sgemm case!\n");
      return -1;
  }

  gen_rand_1d(a_array, M * K);
  gen_rand_1d(b_array, K * M);

#ifdef ROW_MAJOR
  // golden
  BENCH_START(sgemm_opt_golden);
  sgemm_opt_golden(M, N, K, a_array, K, b_array, N, golden, N);
  BENCH_END(sgemm_opt_golden);
  BENCH_START(sgemm_opt_c);
  sgemm_opt_golden(M, N, K, a_array, K, b_array, N, actual, N);
  BENCH_END(sgemm_opt_c);
  // compare
  bool ret = compare_1d(golden, actual, OUTPUT_LEN);
#endif

#ifdef COLUMN_MAJOR
  // golden
  BENCH_START(sgemm_opt_golden);
  sgemm_opt_golden(M, N, K, a_array, M, b_array, K, golden, M);
  BENCH_END(sgemm_opt_golden);

  // vector
  BENCH_START(sgemm_opt_c);
  sgemm_opt_golden(M, N, K, a_array, M, b_array, K, actual, M);
  BENCH_END(sgemm_opt_c);

  // compare
  bool ret = compare_1d(golden, actual, OUTPUT_LEN);
#endif

  vTaskSuspendAll();
   free(golden);
   free(actual);
   free(a_array);
   free(b_array);
  ( void  ) xTaskResumeAll();
  if (ret) {
//    printf("pass\n");
    return 0;
  } else {
//    printf("fail\n");
    return -1;
  }

}
