
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Predefined macro __riscv_flen is equal to:
 * 64: if D toolchain is used
 * 32: if F toolchain is used
 */
#if defined(__riscv_flen) && __riscv_flen == 64
#define SPDP double
#define Precision "Double"
#define DSIN	sin
#define DCOS	cos
#define DATAN	atan
#define DLOG	log
#define DEXP	exp
#define DSQRT	sqrt
#else //__riscv_flen == 32
#define SPDP float
#define Precision "Single"
#define DSIN	sinf
#define DCOS	cosf
#define DATAN	atanf
#define DLOG	logf
#define DEXP	expf
#define DSQRT	sqrtf
#endif

#endif
