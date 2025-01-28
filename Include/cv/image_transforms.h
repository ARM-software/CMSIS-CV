/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        image_transforms.h
 * Description:  Image transformations for CMSIS-CV
 *
 *
 * Target Processor: Cortex-M
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2014 ARM Limited or its affiliates. All rights reserved.
 * Copyright (C) 2024 Himax Technologies, Inc. or its affiliates. All rights reserved.
 *
 * (Copyright details are available in each C source file for each function)
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

/**     
 * @brief      Resize BGR 8U3C image (float implementation)
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * @param[in,out]  p_img   Temporary buffer
 */
extern void arm_image_resize_bgr_8U3C_f32(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                                arm_cv_image_bgr_8U3C_t* ImageOut,
                                                uint8_t *p_img);


/**     
 * @brief      Resize gray image (float implementation)
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * @param[in,out]  p_img   Temporary buffer
 */
extern void arm_image_resize_gray8_f32(const arm_cv_image_gray8_t* ImageIn,
                                             arm_cv_image_gray8_t* ImageOut,
                                             uint8_t *p_img);

/**     
 * @brief      Resize BGR 8U3C image and convert to RGB24 (float implementation)
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * @param[in,out]  p_img   Temporary buffer
 * 
 */
extern void arm_image_resize_bgr_8U3C_to_rgb24_f32(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                         arm_cv_image_rgb24_t* ImageOut,
                                         uint8_t *p_img);
                                         
#ifdef   __cplusplus
}
#endif


#endif
