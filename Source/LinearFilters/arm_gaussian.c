/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_gaussian.c
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


// The kernel applied by this filter is [1,2,1]
//                                      [2,4,2]
//                                      [1,2,1]
// it also can be seen as applying the kernel [1,2,1] one time on the line and one time on the column

// Convert the q6 value from the buffer back to an uint8 value
// This macro is in fact two operation at once,
// first the conversion from q6 to uint8/q0
// second the division per 16, necessary to have the sum of the coeficients in our gaussian equal to 1
#define CONVERSION_GAUSSIAN_Q6TO_U8_AND_DIV_16(a) __SSAT(((a) >> 10),16)

// Apply a kernel [1,2,1] in q3 so [0x08,0x10,0x08] to the input data
// This macro will be applied two time on each pixel.
// the first time it will convert the value from u8 to q3, the second time from q3 to q6
#define VERTICAL_COMPUTE_SCALAR(data_0, data_1, data_2) (data_0 * 0x08 + (data_1 * 0x10) + data_2 * 0x08)
// Gaussian is a symetrical filter, so the vertical and horizontal part of the filter are the same
// but to keep precision we have done the intermediate compute in q3, so basicaly int16 but the output of the filter is
// gray uint8 so we need to add an extra step of conversion for the second application of the filter
#define HORIZONTAL_COMPUTE_SCALAR(data_0, data_1, data_2)                                                              \
    CONVERSION_GAUSSIAN_Q6TO_U8_AND_DIV_16(VERTICAL_COMPUTE_SCALAR(data_0, data_1, data_2))

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

// vec 1,2 and 3 are vector containing lines of the input image
// Apply a kernel [1,2,1] to the input vectors and convert them into q3
#define VERTICAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_res)                                                      \
    int16x8x2_t vect_1x2;                                                                                              \
    int16x8x2_t vect_3x2;                                                                                              \
    vect_1x2.val[0] = vshllbq(vect_1, 3);                                                                              \
    vect_3x2.val[0] = vshllbq(vect_3, 3);                                                                              \
    vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);                                                         \
    vect_1x2.val[1] = vshlltq(vect_1, 3);                                                                              \
    vect_3x2.val[1] = vshlltq(vect_3, 3);                                                                              \
    vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);                                                         \
    vect_res.val[0] = vshllbq(vect_2, 4);                                                                              \
    vect_res.val[0] = vaddq(vect_res.val[0], vect_1x2.val[0]);                                                         \
    vect_res.val[1] = vshlltq(vect_2, 4);                                                                              \
    vect_res.val[1] = vaddq(vect_res.val[1], vect_1x2.val[1]);

// vec 1,2 and 3 are vector containing columns of the input image
// Apply a kernel [1,2,1] to the input vectors and convert them back to uint8, so a shift to the right by three, and in
// order to have the sum of the coefficients of the filter equal to 1
// the shift need to be increase to 7 (divided by 16, shift to 4)
#define HORIZONTAL_COMPUTE_VECTOR(vect_1, vect_2, vect_3, vect_out)                                                    \
    vect_out = vdupq_n_s16(0);                                                                                         \
    vect_2.val[0] = vshlq_n_s16(vect_2.val[0], 1);                                                                     \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_1.val[0]);                                                           \
    vect_2.val[1] = vshlq_n_s16(vect_2.val[1], 1);                                                                     \
    vect_2.val[0] = vaddq_s16(vect_2.val[0], vect_3.val[0]);                                                           \
    vect_2.val[0] = vshrq(vect_2.val[0], 7);                                                                           \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_1.val[1]);                                                           \
    vect_2.val[1] = vaddq_s16(vect_2.val[1], vect_3.val[1]);                                                           \
    vect_2.val[1] = vshrq(vect_2.val[1], 7);                                                                           \
    vect_out = vmovntq(vect_out, vect_2.val[1]);                                                                       \
    vect_out = vmovnbq(vect_out, vect_2.val[0]);

#endif

// ARM_CV_LINEAR_OUTPUT_UINT_8 is defined in arm_linear_filter_common.h
// It is used in arm_linear_filter_common.c to modulate the function generated depending on the output data type
// if needed to add an other output data type, modification will be needed in the file in order to treat the new case
#define ARM_CV_LINEAR_OUTPUT_TYPE ARM_CV_LINEAR_OUTPUT_UINT_8
#define BUFFER_15

#include "arm_linear_filter_common.h"
#include "arm_linear_filter_generator.h"

/**
  @ingroup linearFilter
 */

/**
 * @brief          Gaussian filter applying a 3x3 kernel and using q15 as intermediate values
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
void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut, q15_t *scratch,
                                  const int8_t borderType)
{
    _ARM_LINEAR_GENERIC(imageIn, imageOut, scratch, borderType)
}