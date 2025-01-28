/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_yuv240_to_gray8
 * Description:  Convertion of yuv240 to gray8
 *
 * Target Processor: Cortex-M
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
#include "cv/color_transforms.h"

/**
  @ingroup colorTransform
 */


/**     
 * @brief      YUV420 to Grayscale
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * 
 * @par YUV Format
 * Y,U and V are in different planes with different dimensions
 */

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_yuv420_to_gray8(const arm_cv_image_yuv420_t* ImageIn,
                               arm_cv_image_gray8_t* ImageOut)
{
    const uint8_t *pIn = (const uint8_t *)ImageIn->pData;
    uint8_t *pOut = (uint8_t*)ImageOut->pData;

    uint32_t  blkCnt;           /* loop counters */
    uint8x16_t vecSrc;
    const uint32_t nb = ImageIn->height * ImageIn->width;

    /* Compute 16 outputs at a time */
    blkCnt = nb >> 4;
    while (blkCnt > 0U)
    {
        /*
         * C = A + offset
         * Add offset and then store the result in the destination buffer.
         */
        vecSrc = vld1q(pIn);
        vst1q(pOut, vecSrc);
        /*
         * Decrement the blockSize loop counter
         */
        blkCnt--;
        /*
         * advance vector source and destination pointers
         */
        pIn += 16;
        pOut += 16;
    }
    /*
     * tail
     */
    blkCnt = nb & 0xF;
    if (blkCnt > 0U)
    {
        mve_pred16_t p0 = vctp8q(blkCnt);
        vecSrc = vld1q(pIn);
        vstrbq_p(pOut, vecSrc, p0);
    }
}
#else
void arm_yuv420_to_gray8(const arm_cv_image_yuv420_t* ImageIn,
                               arm_cv_image_gray8_t* ImageOut)
{
  const channel_uint8_t *pIn = ImageIn->pData;
  channel_uint8_t *pOut = ImageOut->pData;

  for(uint32_t height=0; height<ImageIn->height;height++)
  {
     for(uint32_t width=0; width<ImageIn->width;width++)
     {
         *pOut++ = *pIn++;
     }
  }
}
#endif