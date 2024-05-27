/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        feature_detection.h
 * Description:  Feature detection for CMSIS-CV
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
#ifndef ARM_CV_FEATURE_DETECTION_H_
#define ARM_CV_FEATURE_DETECTION_H_

#include "arm_math_types.h"
#include "dsp/matrix_functions.h"

#include "arm_cv_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @brief      Canny edge with sobel integrated
 *
 * @param[in]     ImageIn         The input image
 * @param[out]    ImageOut        The output image
 * @param[in,out] Img_tmp_grad1   Temporary buffer gradient 1
 * @param[in,out] Img_tmp_mag     Temporary magnitude buffer
 * @param[in,out] Img_tmp_grad2   Temporary buffer gradient 2
 * @param[in]     low_threshold   The low threshold
 * @param[in]     high_threshold  The high threshold
 *
 */
extern void arm_canny_edge_sobel_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                            arm_cv_image_q15_t* ImageOut, 
                                            arm_cv_image_gradient_q15_t* Img_tmp_grad1, 
                                            arm_cv_image_q15_t* Img_tmp_mag, 
                                            arm_cv_image_gradient_q15_t* Img_tmp_grad2,
                                            int low_threshold,
                                            int high_threshold);

#ifdef   __cplusplus
}
#endif

#endif