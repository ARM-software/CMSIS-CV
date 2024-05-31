/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        color_transforms.h
 * Description:  Color transforms for CMSIS-CV
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

#ifndef ARM_CV_COLOR_TRANSFORM_H
#define ARM_CV_COLOR_TRANSFORM_H


//this include contain the specific types of the library
#include "arm_cv_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif




/**     
 * @brief      YUV420 to Grayscale
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_yuv420_to_gray8(const arm_cv_image_yuv420_t* ImageIn,
                                      arm_cv_image_gray8_t* ImageOut);

/**     
 * @brief      YUV420 to packed RGB24
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_yuv420_to_rgb24(const arm_cv_image_yuv420_t* ImageIn,
                                      arm_cv_image_rgb24_t* ImageOut);


/**     
 * @brief      Unpacked BGR 8U3C to Grayscale
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_bgr_8U3C_to_gray8(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                        arm_cv_image_gray8_t* ImageOut);


/**     
 * @brief      Unpacked BGR 8U3C to RGB24
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_bgr_8U3C_to_rgb24(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                        arm_cv_image_rgb24_t* ImageOut);

/**     
 * @brief      Grayscale to RGB24
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_gray8_to_rgb24(const arm_cv_image_gray8_t* ImageIn,
                                     arm_cv_image_rgb24_t* ImageOut);



/**     
 * @brief      RGB24 to Grayscale
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
extern void arm_rgb24_to_gray8(const arm_cv_image_rgb24_t* ImageIn,
                                     arm_cv_image_gray8_t* ImageOut);

#ifdef   __cplusplus
}
#endif


#endif
