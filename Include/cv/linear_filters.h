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


//this include contain the specific types of the library
#include "arm_cv_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif




/**     
 * @brief      Gaussian filter for grayscale data computing in q15
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
 extern void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t* ImageIn,
                                                arm_cv_image_gray8_t* ImageOut);



#ifdef   __cplusplus
}
#endif


#endif
