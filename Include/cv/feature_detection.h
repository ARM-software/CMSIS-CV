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
 * @brief      Function returning the scratch size for canny sobel
 *
 * @param[in]     imageIn         The input image
 * @return		  Scratch size in bytes
 *
 */
extern uint16_t arm_cv_get_scratch_size_canny_sobel(int width);

/**
 * @brief      Canny edge with sobel integrated
 *
 * @param[in]     imageIn         The input image
 * @param[out]    imageOut        The output image
 * @param[in,out] scratch   Temporary buffer 
 * @param[in]     lowThreshold   The low threshold
 * @param[in]     highThreshold  The high threshold
 *
 */
extern void arm_cv_canny_edge_sobel(const arm_cv_image_gray8_t* imageIn, 
                                            arm_cv_image_gray8_t* imageOut, 
                                            q15_t* scratch,
                                            uint8_t lowThreshold,
                                            uint8_t highThreshold);

#ifdef   __cplusplus
}
#endif

#endif