/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_bgr_8U3C_to_rgb24
 * Description:  Convertion of planar BGR to RGB24
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
#include "cv/color_transforms.h"

/**
  @ingroup colorTransform
 */

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
#include "arm_cv_common.h"
#endif
/**     
 * @brief      Unpacked BGR 8U3C to RGB24
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_bgr_8U3C_to_rgb24(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                 arm_cv_image_rgb24_t* ImageOut)
{
    const uint32_t nb = ImageIn->height * ImageIn->width;

    const channel_uint8_t *pBlue  = ImageIn->pData;
    const channel_uint8_t *pGreen = pBlue + nb;
    const channel_uint8_t *pRed   = pGreen + nb;

    channel_uint8_t *pOut = ImageOut->pData;

    uint32_t  blkCnt;           /* loop counters */
    uint8x16_t tmp;
    int8x16_t redOffsetV;
    int8x16_t greenOffsetV;
    int8x16_t blueOffsetV;

    uint8x16_t redV;
    uint8x16_t greenV;
    uint8x16_t blueV;


    redOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
    redOffsetV = vmulq_n_s8(redOffsetV,3);

    greenOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
    greenOffsetV = vmulq_n_s8(greenOffsetV,3);
    greenOffsetV = vaddq_n_s8(greenOffsetV,1);

    blueOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
    blueOffsetV = vmulq_n_s8(blueOffsetV,3);
    blueOffsetV = vaddq_n_s8(blueOffsetV,2);

    /* Compute 16 outputs at a time */
    blkCnt = nb >> 4;
    while (blkCnt > 0U)
    {
       
        blueV = vld1q(pBlue);
        greenV = vld1q(pGreen);
        redV = vld1q(pRed);

        vstrbq_scatter_offset_s8(cv_sgbuf,redOffsetV,redV);
        vstrbq_scatter_offset_s8(cv_sgbuf,greenOffsetV,greenV);
        vstrbq_scatter_offset_s8(cv_sgbuf,blueOffsetV,blueV);
        
        tmp = vld1q(cv_sgbuf);
        vst1q(pOut, tmp);
        pOut += 16;

        tmp = vld1q(cv_sgbuf+16);
        vst1q(pOut, tmp);
        pOut += 16;

        tmp = vld1q(cv_sgbuf+32);
        vst1q(pOut, tmp);
        pOut += 16;

        /*
         * Decrement the blockSize loop counter
         */
        blkCnt--;
        /*
         * advance vector source and destination pointers
         */
        pBlue += 16;
        pGreen += 16;
        pRed += 16;
    }
    /*
     * tail
     */
    blkCnt = nb & 0xF;
    blkCnt = 3 * blkCnt;
    if (blkCnt > 0U)
    {
        blueV = vld1q(pBlue);
        greenV = vld1q(pGreen);
        redV = vld1q(pRed);

        vstrbq_scatter_offset_s8(cv_sgbuf,redOffsetV,redV);
        vstrbq_scatter_offset_s8(cv_sgbuf,greenOffsetV,greenV);
        vstrbq_scatter_offset_s8(cv_sgbuf,blueOffsetV,blueV);

        if (blkCnt > 32)
        {
            mve_pred16_t p0 = vctp8q(blkCnt-32);
    
            tmp = vld1q(cv_sgbuf);
            vst1q(pOut, tmp);
            pOut += 16;
    
            tmp = vld1q(cv_sgbuf+16);
            vst1q(pOut, tmp);
            pOut += 16;
    
            tmp = vld1q(cv_sgbuf+32);
            vstrbq_p(pOut, tmp, p0);
        }
        else if (blkCnt > 16)
        {
            mve_pred16_t p0 = vctp8q(blkCnt-16);
    
            tmp = vld1q(cv_sgbuf);
            vst1q(pOut, tmp);
            pOut += 16;
    
            tmp = vld1q(cv_sgbuf+16);
            vstrbq_p(pOut, tmp, p0);
        }
        else 
        {
            mve_pred16_t p0 = vctp8q(blkCnt);
    
            tmp = vld1q(cv_sgbuf);
            vstrbq_p(pOut, tmp, p0);
        }
    }
}

#else
void arm_bgr_8U3C_to_rgb24(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                 arm_cv_image_rgb24_t* ImageOut)
{
  const int nb = ImageIn->height * ImageIn->width;

  const channel_uint8_t *pBlue  = ImageIn->pData;
  const channel_uint8_t *pGreen = pBlue + nb;
  const channel_uint8_t *pRed   = pGreen + nb;

  channel_uint8_t *pOut = ImageOut->pData;

  for(uint32_t height=0; height<ImageIn->height;height++)
  {
     for(uint32_t width=0; width<ImageIn->width;width++)
     {
        *pOut++ = *pRed++;   // r
        *pOut++ = *pGreen++;   // g
        *pOut++ = *pBlue++; // b
     }
  }
}

#endif