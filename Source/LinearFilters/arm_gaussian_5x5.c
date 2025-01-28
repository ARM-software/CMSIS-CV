/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_generic_filter.c
 * Description:  Generic template for linear filter, this one doing a Gaussian
 * filter
 *
 *
 * Target Processor: Cortex-M
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
#include "dsp/none.h"


// The kernel applied by this filter is [1, 4, 6, 4, 1] /256
//                                      [4,16,24,16, 4]
//                                      [6,24,36,24, 6]
//                                      [4,16,24,16, 4]
//                                      [1, 4, 6, 4, 1]
// it also can be seen as applying the kernel [1,4,6,4,1] one time on the line and one time on the column sum is 256

// Macro dividing the input value by 256, necessary to normalise the kernel of the gaussian
#define DIV_256(a) __SSAT(((a) >> 8), 16)

// Apply the kernel [1, 4, 6, 4, 1] to the input values
#define VERTICAL_COMPUTE_SCALAR(data_0, data_1, data_2, data_3, data_4)                                                \
    (data_0 * 1 + (data_1 * 4) + data_2 * 6 + data_3 * 4 + data_4 * 1)
// Gaussian is a symetrical filter so the operation on the vertical and horizontal part is the same
// but the horizontal part is the last so is also combine with the normalisation of the kernel
#define HORIZONTAL_COMPUTE_SCALAR(data_0, data_1, data_2, data_3, data_4)                                              \
    DIV_256(VERTICAL_COMPUTE_SCALAR(data_0, data_1, data_2, data_3, data_4))

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

// vec 1, 2, 3, 4 and 5 are vectors containing lines of the input image
// Apply a kernel [1,4,6,4,1] to the input values
#define VERTICAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_4, vect_5, vect_res)                                      \
    int16x8x2_t vect_1x2;                                                                                              \
    int16x8x2_t vect_2x2;                                                                                              \
    vect_1x2.val[0] = vmovlbq(vect_1);                                                                                 \
    vect_2x2.val[0] = vmovlbq(vect_5);                                                                                 \
    vect_res.val[0] = vaddq(vect_1x2.val[0], vect_2x2.val[0]);                                                         \
    vect_2x2.val[0] = vshllbq(vect_2, 2);                                                                              \
    vect_res.val[0] = vaddq(vect_res.val[0], vect_2x2.val[0]);                                                         \
    vect_2x2.val[0] = vshllbq(vect_4, 2);                                                                              \
    vect_res.val[0] = vaddq(vect_res.val[0], vect_2x2.val[0]);                                                         \
    vect_2x2.val[0] = vshllbq(vect_3, 1);                                                                              \
    vect_2x2.val[0] = vmulq(vect_2x2.val[0], 3);                                                                       \
    vect_res.val[0] = vaddq(vect_res.val[0], vect_2x2.val[0]);                                                         \
                                                                                                                       \
    vect_1x2.val[1] = vmovltq(vect_1);                                                                                 \
    vect_2x2.val[1] = vmovltq(vect_5);                                                                                 \
    vect_res.val[1] = vaddq(vect_1x2.val[1], vect_2x2.val[1]);                                                         \
    vect_2x2.val[1] = vshlltq(vect_2, 2);                                                                              \
    vect_res.val[1] = vaddq(vect_res.val[1], vect_2x2.val[1]);                                                         \
    vect_2x2.val[1] = vshlltq(vect_4, 2);                                                                              \
    vect_res.val[1] = vaddq(vect_res.val[1], vect_2x2.val[1]);                                                         \
    vect_2x2.val[1] = vshlltq(vect_3, 1);                                                                              \
    vect_2x2.val[1] = vmulq(vect_2x2.val[1], 3);                                                                       \
    vect_res.val[1] = vaddq(vect_res.val[1], vect_2x2.val[1]);

// vec 1, 2, 3, 4 and 5 are vectors containing lines of the input image
// Apply a kernel [1,4,6,4,1] to the input values and do the normalisation of the gaussian kernel
#define HORIZONTAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_4, vect_5, vect_out)                                    \
    vect_out = vdupq_n_s16(0);                                                                                         \
    vect_2.val[0] = vshlq_n_s16(vect_2.val[0], 2);                                                                     \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_1.val[0]);                                                           \
    vect_3.val[0] = vmulq(vect_3.val[0], 6);                                                                           \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_1.val[0] = vshlq_n_s16(vect_4.val[0], 2);                                                                     \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_1.val[0]);                                                           \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_5.val[0]);                                                           \
    vect_2.val[1] = vshlq_n_s16(vect_2.val[1], 2);                                                                     \
    vect_3.val[1] = vmulq(vect_3.val[1], 6);                                                                           \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_1.val[1]);                                                           \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_1.val[1] = vshlq_n_s16(vect_4.val[1], 2);                                                                     \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_1.val[1]);                                                           \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_5.val[1]);                                                           \
    vect_2.val[0] = vshrq(vect_2.val[0], 8);                                                                           \
    vect_2.val[1] = vshrq(vect_2.val[1], 8);                                                                           \
    vect_out = vmovnbq(vect_out, vect_2.val[0]);                                                                       \
    vect_out = vmovntq(vect_out, vect_2.val[1]);

#endif

// ARM_CV_LINEAR_OUTPUT_UINT_8 is defined in arm_linear_filter_common.h
// It is used in arm_linear_filter_common.c to modulate the function generated depending on the output data type
// if needed to add an other output data type, modification will be needed in the file in order to treat the new case
#define ARM_CV_LINEAR_OUTPUT_TYPE ARM_CV_LINEAR_OUTPUT_UINT_8
#define KERNEL_5
#define BUFFER_15

#include "arm_linear_filter_common.h"
#include "arm_linear_filter_generator.h"

/**
  @ingroup linearFilter
 */

/**
 * @brief          Gaussian filter applying a 5x5 kernel and using q15 as intermediate values
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
void arm_gaussian_filter_5x5_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut, q15_t *scratch,
                                  const int8_t borderType)
{
    _ARM_LINEAR_GENERIC(imageIn, imageOut, scratch, borderType)
}

// #undef KERNEL_5