#include "nuclei_sdk_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include <riscv_vector.h>
#define DISABLE_NMSIS_BENCH
#include <nmsis_bench.h>
#include <string.h>
#include <common.h>

BENCH_DECLARE_VAR();

// reference https://github.com/riscv/riscv-v-spec/blob/master/example/strcmp.s
int strcmp_rvv(const char *src1, const char *src2) {
  size_t vlmax = __riscv_vsetvlmax_e8m8();
  long first_set_bit = -1;
  size_t vl, vl1;
  while (first_set_bit < 0) {
    vint8m8_t vec_src1 = __riscv_vle8ff_v_i8m8(src1, &vl, vlmax);
    vint8m8_t vec_src2 = __riscv_vle8ff_v_i8m8(src2, &vl1, vlmax);

    vbool1_t string_terminate = __riscv_vmseq_vx_i8m8_b1(vec_src1, 0, vl);
    //vTaskDelay(10);
    vbool1_t no_equal = __riscv_vmsne_vv_i8m8_b1(vec_src1, vec_src2, vl);
    vbool1_t vec_terminate = __riscv_vmor_mm_b1(string_terminate, no_equal, vl);

    first_set_bit = __riscv_vfirst_m_b1(vec_terminate, vl);
    src1 += vl;
    src2 += vl;
  }
  src1 -= vl - first_set_bit;
  src2 -= vl - first_set_bit;
  return *src1 - *src2;
}

int do_strcmp() {
  const int N = 1023;
  const uint32_t seed = 0xdeadbeef;
  BENCH_INIT();
  srand(seed);

 // printf("vlen = %d bits\n", csrr_vlenb() * 8); // vlen = vlenb * 8

  // data gen
  char s0[N], s1[N];
  gen_string(s0, N);
  gen_string(s1, N);
  // in order to compare more char
  memcpy(s1, s0, N - 2);

  // compute
  int golden, actual;

  BENCH_START(strcmp_golden);
  golden = strcmp(s0, s1);
  BENCH_END(strcmp_golden);
  BENCH_START(strcmp_rvv);
  actual = strcmp_rvv(s0, s1);
  BENCH_END(strcmp_rvv);

  // compare
  // b extension will return difference value or -1 of strcmp, so the pass may fail
  bool ret = golden == actual;
  //printf("golden: %d; actual: %d\n", golden, actual);
  // puts pass for the CI

  if (ret) {
//      puts("pass");
      return 0;
  } else {
//      puts("fail");
      return -1;
  }
}
