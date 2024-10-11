/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_generic_filter.c
 * Description:  Generic template for linear filter, this one doing a Gaussian
 * filter
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2014 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cv/linear_filters.h"
#include "dsp/basic_math_functions.h"
#include "arm_acle.h"

// #define BUFFER_TYPE q15_t
//  The kernel applied by this filter is [ 4, 14, 28, 36, 28, 14, 4] /4096
//                                       [14, 49, 98,126, 98, 49,14]
//                                       [28, 98,196,252,196, 98,28]
//                                       [36,126,252,324,252,126,36]
//                                       [28, 90,196,252,196, 98,28]
//                                       [14, 49, 98,126, 98, 49,14]
//                                       [ 4, 14, 24, 36, 28, 14, 4]

// it also can be seen as applying the kernel [2,7,14,18,14,7,2] one time on the line and one time on the column sum is
// 256

// Macro dividing the input value by 256, necessary to normalise the kernel of the gaussian
#define DIV_256(a) __ssat(((a) >> 8), 32)

// Apply the kernel [2, 7, 14, 18, 14, 7, 2] to the input values
#define KERNEL_APPLICATION(data_0, data_1, data_2, data_3, data_4, data_5, data_6)                                     \
    (data_0 * 2 + (data_1 * 7) + data_2 * 14 + data_3 * 18 + data_4 * 14 + data_5 * 7 + data_6 * 2)

// Vertical operation, consist of applying the kernel to the value and a shift to the right by 4 in order to avoid
// overflowing the q15 buffer for the second application of the kernel applying the kernel one time lead to a maximun
// value of 64 * 255, the second application can lead to a max value of 64*64 ** 255 that can overflow int16 this shift
// lead to a loss of precision in some cases
#define VERTICAL_COMPUTE_SCALAR(data_0, data_1, data_2, data_3, data_4, data_5, data_6)                                \
    KERNEL_APPLICATION(data_0, data_1, data_2, data_3, data_4, data_5, data_6)

// The horizontal computation consist of applying the kernel and shifting by 8 on the left. the sum of the coeficient of
// the kernel is 4096, so shift by 12, but we already shifted by 4 in the vertical part
#define HORIZONTAL_COMPUTE_SCALAR(data_0, data_1, data_2, data_3, data_4, data_5, data_6)                              \
    DIV_256((KERNEL_APPLICATION(data_0, data_1, data_2, data_3, data_4, data_5, data_6)) >> 4)

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

// vec 1, 2, 3, 4 5 6 and 7 are vectors containing lines of the input image
// Apply the kernel [2, 7, 14, 18, 14, 7, 2] to the input values
// and divide the result by 16, in order for the intermediate computation of the horizontal part to not saturate
#define VERTICAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_4, vect_5, vect_6, vect_7, vect_res)                      \
    vect_res.val[0] = vdupq_n_s32(0);                                                                                  \
    vect_res.val[1] = vdupq_n_s32(0);                                                                                  \
    vect_res.val[2] = vdupq_n_s32(0);                                                                                  \
    vect_res.val[3] = vdupq_n_s32(0);                                                                                  \
    int16x8x2_t vect_16_1;                                                                                             \
    int32x4_t vect_32;                                                                                                 \
    int16x8x2_t vect_1x2;                                                                                              \
    int16x8x2_t vect_2x2;                                                                                              \
    vect_1x2.val[0] = vshllbq(vect_1, 1);                                                                              \
    vect_2x2.val[0] = vshllbq(vect_7, 1);                                                                              \
    vect_16_1.val[0] = vaddq(vect_1x2.val[0], vect_2x2.val[0]);                                                        \
    vect_2x2.val[0] = vshllbq(vect_3, 1);                                                                              \
    vect_2x2.val[0] = vmulq(vect_2x2.val[0], 7);                                                                       \
    vect_16_1.val[0] = vaddq(vect_16_1.val[0], vect_2x2.val[0]);                                                       \
    vect_2x2.val[0] = vshllbq(vect_5, 1);                                                                              \
    vect_2x2.val[0] = vmulq(vect_2x2.val[0], 7);                                                                       \
    vect_16_1.val[0] = vaddq(vect_16_1.val[0], vect_2x2.val[0]);                                                       \
                                                                                                                       \
    vect_1x2.val[0] = vshllbq(vect_4, 1);                                                                              \
    vect_1x2.val[0] = vmulq(vect_1x2.val[0], 9);                                                                       \
    vect_2x2.val[0] = vmovlbq(vect_2);                                                                                 \
    vect_2x2.val[0] = vmulq(vect_2x2.val[0], 7);                                                                       \
    vect_16_1.val[1] = vaddq(vect_1x2.val[0], vect_2x2.val[0]);                                                        \
    vect_2x2.val[0] = vmovlbq(vect_6);                                                                                 \
    vect_2x2.val[0] = vmulq(vect_2x2.val[0], 7);                                                                       \
    vect_16_1.val[1] = vaddq(vect_16_1.val[1], vect_2x2.val[0]);                                                       \
                                                                                                                       \
    vect_res.val[0] = vmovlbq(vect_16_1.val[1]);                                                                       \
    vect_32 = vmovlbq(vect_16_1.val[0]);                                                                               \
    vect_res.val[0] = vaddq(vect_res.val[0], vect_32);                                                                 \
    vect_res.val[2] = vmovltq(vect_16_1.val[1]);                                                                       \
    vect_32 = vmovltq(vect_16_1.val[0]);                                                                               \
    vect_res.val[2] = vaddq(vect_res.val[2], vect_32);                                                                 \
                                                                                                                       \
    vect_1x2.val[1] = vshlltq(vect_1, 1);                                                                              \
    vect_2x2.val[1] = vshlltq(vect_7, 1);                                                                              \
    vect_16_1.val[0] = vaddq(vect_1x2.val[1], vect_2x2.val[1]);                                                        \
    vect_2x2.val[1] = vshlltq(vect_3, 1);                                                                              \
    vect_2x2.val[1] = vmulq(vect_2x2.val[1], 7);                                                                       \
    vect_16_1.val[0] = vaddq(vect_16_1.val[0], vect_2x2.val[1]);                                                       \
    vect_2x2.val[1] = vshlltq(vect_5, 1);                                                                              \
    vect_2x2.val[1] = vmulq(vect_2x2.val[1], 7);                                                                       \
    vect_16_1.val[0] = vaddq(vect_16_1.val[0], vect_2x2.val[1]);                                                       \
                                                                                                                       \
    vect_1x2.val[1] = vshlltq(vect_4, 1);                                                                              \
    vect_1x2.val[1] = vmulq(vect_1x2.val[1], 9);                                                                       \
    vect_2x2.val[1] = vmovltq(vect_2);                                                                                 \
    vect_2x2.val[1] = vmulq(vect_2x2.val[1], 7);                                                                       \
    vect_16_1.val[1] = vaddq(vect_1x2.val[1], vect_2x2.val[1]);                                                        \
    vect_2x2.val[1] = vmovltq(vect_6);                                                                                 \
    vect_2x2.val[1] = vmulq(vect_2x2.val[1], 7);                                                                       \
    vect_16_1.val[1] = vaddq(vect_16_1.val[1], vect_2x2.val[1]);                                                       \
                                                                                                                       \
    vect_res.val[1] = vmovlbq(vect_16_1.val[1]);                                                                       \
    vect_32 = vmovlbq(vect_16_1.val[0]);                                                                               \
    vect_res.val[1] = vaddq(vect_res.val[1], vect_32);                                                                 \
    vect_res.val[3] = vmovltq(vect_16_1.val[1]);                                                                       \
    vect_32 = vmovltq(vect_16_1.val[0]);                                                                               \
    vect_res.val[3] = vaddq(vect_res.val[3], vect_32);

// vec 1, 2, 3, 4 and 5 are vectors containing lines of the input image
// Apply the kernel [2, 7, 14, 18, 14, 7, 2] to the input values
// and shift the result by 8 in order for the sum of the coeficients of the filter to be equal to 1
#define HORIZONTAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_4, vect_5, vect_6, vect_7, vect_out)                    \
    int16x8x2_t vect_16;                                                                                               \
    vect_out = vdupq_n_s16(0);                                                                                         \
    vect_2.val[0] = vmulq(vect_2.val[0], 7);                                                                           \
    vect_1.val[0] = vshlq_n_s16(vect_1.val[0], 1);                                                                     \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_1.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_3.val[0], 14);                                                                          \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_4.val[0], 18);                                                                          \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_5.val[0], 14);                                                                          \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_6.val[0], 7);                                                                           \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_7.val[0], 2);                                                                           \
    vect_2.val[0] = vaddq_s32(vect_2.val[0], vect_3.val[0]);                                                           \
                                                                                                                       \
    vect_2.val[1] = vmulq(vect_2.val[1], 7);                                                                           \
    vect_1.val[1] = vshlq_n_s16(vect_1.val[1], 1);                                                                     \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_1.val[1]);                                                           \
    vect_3.val[1] = vmulq(vect_3.val[1], 14);                                                                          \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_3.val[1] = vmulq(vect_4.val[1], 18);                                                                          \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_3.val[1] = vmulq(vect_5.val[1], 14);                                                                          \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_3.val[1] = vmulq(vect_6.val[1], 7);                                                                           \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_3.val[1] = vmulq(vect_7.val[1], 2);                                                                           \
    vect_2.val[1] = vaddq_s32(vect_2.val[1], vect_3.val[1]);                                                           \
                                                                                                                       \
    vect_2.val[2] = vmulq(vect_2.val[2], 7);                                                                           \
    vect_1.val[2] = vshlq_n_s16(vect_1.val[2], 1);                                                                     \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_1.val[2]);                                                           \
    vect_3.val[2] = vmulq(vect_3.val[2], 14);                                                                          \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_3.val[2]);                                                           \
    vect_3.val[2] = vmulq(vect_4.val[2], 18);                                                                          \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_3.val[2]);                                                           \
    vect_3.val[2] = vmulq(vect_5.val[2], 14);                                                                          \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_3.val[2]);                                                           \
    vect_3.val[2] = vmulq(vect_6.val[2], 7);                                                                           \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_3.val[2]);                                                           \
    vect_3.val[2] = vmulq(vect_7.val[2], 2);                                                                           \
    vect_2.val[2] = vaddq_s32(vect_2.val[2], vect_3.val[2]);                                                           \
                                                                                                                       \
    vect_2.val[3] = vmulq(vect_2.val[3], 7);                                                                           \
    vect_1.val[3] = vshlq_n_s16(vect_1.val[3], 1);                                                                     \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_1.val[3]);                                                           \
    vect_3.val[3] = vmulq(vect_3.val[3], 14);                                                                          \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_3.val[3]);                                                           \
    vect_3.val[3] = vmulq(vect_4.val[3], 18);                                                                          \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_3.val[3]);                                                           \
    vect_3.val[3] = vmulq(vect_5.val[3], 14);                                                                          \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_3.val[3]);                                                           \
    vect_3.val[3] = vmulq(vect_6.val[3], 7);                                                                           \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_3.val[3]);                                                           \
    vect_3.val[3] = vmulq(vect_7.val[3], 2);                                                                           \
    vect_2.val[3] = vaddq_s32(vect_2.val[3], vect_3.val[3]);                                                           \
                                                                                                                       \
    vect_16.val[0] = vshrnbq(vect_16.val[0], vect_2.val[0], 4);                                                        \
    vect_16.val[0] = vshrntq(vect_16.val[0], vect_2.val[2], 4);                                                        \
                                                                                                                       \
    vect_16.val[1] = vshrnbq(vect_16.val[1], vect_2.val[1], 4);                                                        \
    vect_16.val[1] = vshrntq(vect_16.val[1], vect_2.val[3], 4);                                                        \
                                                                                                                       \
    vect_out = vshrnbq(vect_out, vect_16.val[0], 8);                                                                   \
    vect_out = vshrntq(vect_out, vect_16.val[1], 8);

#endif
// ARM_CV_LINEAR_OUTPUT_UINT_8 is defined in arm_linear_filter_common.h
// It is used in arm_linear_filter_common.c to modulate the function generated depending on the output data type
// if needed to add an other output data type, modification will be needed in the file in order to treat the new case
#define ARM_CV_LINEAR_OUTPUT_TYPE ARM_CV_LINEAR_OUTPUT_UINT_8
#define KERNEL_7
#define BUFFER_31

#include "arm_linear_filter_common.h"
#include "arm_linear_filter_generator.h"

/**
  @ingroup linearFilter
 */

/**
 * @brief          Gaussian filter applying a 7x7 kernel and using q31 as intermediate values
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Temporary buffer
 * @param[in]      borderType  Type of border to use, supported are Replicate Wrap and Reflect
 *
 * @par Temporary buffer sizing:
 *
 * Will use a temporary buffer to store intermediate values of gradient and magnitude.
 *
 * Size of temporary buffer is given by
 * arm_cv_get_scratch_size_generic(int width)
 */
void arm_gaussian_filter_7x7_buffer_31_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                     BUFFER_TYPE *scratch, const int8_t borderType)
{
    _ARM_LINEAR_GENERIC(imageIn, imageOut, scratch, borderType)
}
