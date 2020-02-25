
#ifndef __REF_MEAN_H__
#define __REF_MEAN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <riscv_math.h>
#include <stdint.h>

void ref_mean_f32(float32_t *pSrc, uint32_t blockSize, float32_t *pResult);
void ref_mean_q31(q31_t *pSrc, uint32_t blockSize, q31_t *pResult);
void ref_mean_q15(q15_t *pSrc, uint32_t blockSize, q15_t *pResult);
void ref_mean_q7(q7_t *pSrc, uint32_t blockSize, q7_t *pResult);

#ifdef __cplusplus
}
#endif

#endif /** __REF_MEAN_H__  */
