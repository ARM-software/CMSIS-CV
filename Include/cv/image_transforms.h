/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        image_transforms.h
 * Description:  Image transformations for CMSIS-CV
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

#ifndef ARM_CV_IMAGE_TRANSFORM_H
#define ARM_CV_IMAGE_TRANSFORM_H


//this include contain the specific types of the library
#include "arm_cv_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif




/**
 * @brief      Gray8 image cropping
 *
 * @param[in]  ImageIn   The image in
 * @param      ImageOut  The image out
 * @param[in]  left      The left boundary
 * @param[in]  top       The top boundary
 * @param[in]  right     The right boundary
 * @param[in]  bottom    The bottom boundary
 */
extern void arm_crop_gray8(const arm_cv_image_gray8_t* ImageIn,
                           arm_cv_image_gray8_t* ImageOut,
                           const uint32_t left,
                           const uint32_t top,
                           const uint32_t right,
                           const uint32_t bottom
                           );

/**
 * @brief      RGB24 image cropping
 *
 * @param[in]  ImageIn   The image in
 * @param      ImageOut  The image out
 * @param[in]  left      The left boundary
 * @param[in]  top       The top boundary
 * @param[in]  right     The right boundary
 * @param[in]  bottom    The bottom boundary
 */
extern void arm_crop_rgb24(const arm_cv_image_rgb24_t* ImageIn,
                           arm_cv_image_rgb24_t* ImageOut,
                           const uint32_t left,
                           const uint32_t top,
                           const uint32_t right,
                           const uint32_t bottom
                           );

#ifdef   __cplusplus
}
#endif


#endif
