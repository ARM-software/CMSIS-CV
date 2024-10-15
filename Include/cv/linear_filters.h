/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        linear_filters.h
 * Description:  Linear 2D filters for CMSIS-CV
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
#ifndef ARM_CV_LINEAR_FILTER_H
#define ARM_CV_LINEAR_FILTER_H

// this include contain the specific types of the library
#include "arm_cv_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ARM_CV_BORDER_NEAREST 1
#define ARM_CV_BORDER_WRAP 2
#define ARM_CV_BORDER_REFLECT 3

/**
 * @brief      Return the scratch size for generic linear filter function using a q15 buffer
 *
 * @param[in]     width        The width of the image
 * @return		  Scratch size in bytes
 */
extern uint16_t arm_get_linear_scratch_size_buffer_15(const int width);

/**
 * @brief      Return the scratch size for generic linear filter function using a q31 buffer
 *
 * @param[in]     width        The width of the image
 * @return		  Scratch size in bytes
 */
extern uint16_t arm_get_linear_scratch_size_buffer_31(const int width);

/**
 * @brief          Gaussian filter applying a 3x3 kernel and using q15 as intermediate values
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Temporary buffer
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                         q15_t *scratch, const int8_t borderType);

/**
 * @brief          Gaussian filter applying a 5x5 kernel and using q15 as intermediate values
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Temporary buffer
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_gaussian_filter_5x5_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                         q15_t *scratch, const int8_t borderType);

/**
 * @brief          Gaussian filter applying a 7x7 kernel and using q15 as intermediate values
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Temporary buffer
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_gaussian_filter_7x7_buffer_15_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                         q15_t *scratch, const int8_t borderType);

/**
 * @brief          Gaussian filter applying a 7x7 kernel and using q31 as intermediate values
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Temporary buffer
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_gaussian_filter_7x7_buffer_31_fixp(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                            q31_t *scratch, const int8_t borderType);

/**
 * @brief      Return the scratch size for sobel functions using a q15 buffer
 *
 * @param[in]     width         The width of the image
 * @return		  Scratch size in bytes
 */
extern uint16_t arm_get_scratch_size_sobel(const int width);

/**
 * @brief          Sobel filter computing the gradient on the vertical axis
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Buffer of q15
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_sobel_vertical(const arm_cv_image_gray8_t *ImageIn, arm_cv_image_q15_t *ImageOut, q15_t *Buffer,
                               const int8_t borderType);

/**
 * @brief          Sobel filter computing the gradient on the horizontal axis
 *
 * @param[in]      imageIn     The input image
 * @param[out]     imageOut    The output image
 * @param[in,out]  scratch     Buffer of q15
 * @param[in]      borderType  Type of border to use, supported are Nearest, Wrap and Reflect
 *
 */
extern void arm_sobel_horizontal(const arm_cv_image_gray8_t *imageIn, arm_cv_image_q15_t *imageOut, q15_t *scratch,
                                 const int8_t borderType);

#ifdef __cplusplus
}
#endif

#endif
