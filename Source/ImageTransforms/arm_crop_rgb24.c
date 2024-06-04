/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_crop_gray8
 * Description:  Cropping of grayscale image
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2024 ARM Limited or its affiliates. All rights reserved.
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
#include "cv/image_transforms.h"

/**
  @ingroup imageTransform
 */


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
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_crop_rgb24(const arm_cv_image_rgb24_t* ImageIn,
                          arm_cv_image_rgb24_t* ImageOut,
                          const uint32_t left,
                          const uint32_t top,
                          const uint32_t right,
                          const uint32_t bottom
                    )
{
   const uint8_t *pSrc = ImageIn->pData + 3*left + 3*top*(ImageIn->width);
   uint8_t *pDst = ImageOut->pData;
   uint8x16_t vec;

   for(uint32_t row=top; row < bottom; row ++)
   {
       int32_t column = (int32_t)(3*left);
       for(; column <= (int32_t)(3*right)-16;column += 16)
       {
          vec = vld1q(pSrc);
          pSrc += 16;

          vst1q(pDst, vec);
          pDst += 16;
       }
       for(;column < (int32_t)(3*right); column ++ )
       {
         *pDst++ = *pSrc++;
       }
       pSrc += 3*ImageIn->width - 3*(right - left);
   }

}
#else
void arm_crop_rgb24(const arm_cv_image_rgb24_t* ImageIn,
                          arm_cv_image_rgb24_t* ImageOut,
                          const uint32_t left,
                          const uint32_t top,
                          const uint32_t right,
                          const uint32_t bottom
                    )
{
    const uint8_t *pSrc = ImageIn->pData + 3*left + 3*top*(ImageIn->width);
    uint8_t *pDst = ImageOut->pData;
    for(uint32_t row=top; row < bottom; row ++)
    {
       for(uint32_t column=3*left;column < 3*right; column ++)
       {
         *pDst++ = *pSrc++;
       }
       pSrc += 3*ImageIn->width - 3*(right - left);
    }
}
#endif