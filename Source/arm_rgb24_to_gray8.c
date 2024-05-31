/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_rbg24_to_gray8
 * Description:  Convertion of rgb24 to gray8
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
#include "dsp/none.h"


#define RED_FACTOR_Q7 38 // 0.299
#define GREEN_FACTOR_Q7 75 // 0.587
#define BLUE_FACTOR_Q7 15 // 0.114

/**
  @ingroup colorTransform
 */

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
#include "arm_cv_common.h"
#endif

/**     
 * @brief      RGB24 to Grayscale
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * 
 * @par  Formula used
 * 
 * \f[
 * gray = 0.299  * red + 0.587 * green + 0.114 * blue ;
 * \f]
 * 
 */
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_rgb24_to_gray8(const arm_cv_image_rgb24_t* ImageIn,
                              arm_cv_image_gray8_t* ImageOut)
{
    const channel_uint8_t *pRGB  = ImageIn->pData;
    const uint32_t nb = ImageIn->height * ImageIn->width;
    uint32_t  blkCnt;           /* loop counters */
    int8x16_t colorOffsetV;
    int8x16_t currentOffsetV;

    uint8x16_t redV;
    uint8x16_t greenV;
    uint8x16_t blueV;

    uint16x8_t gray = vdupq_n_u16(0);
    uint16x8_t tmp;
    uint8x16_t out;

    channel_uint8_t *pOut = ImageOut->pData;

    colorOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
    colorOffsetV = vmulq_n_s8(colorOffsetV,3);


    blkCnt = nb >> 4;
    while (blkCnt > 0U)
    {
       
        currentOffsetV = colorOffsetV;
        redV=vldrbq_gather_offset_u8(pRGB,currentOffsetV);

        currentOffsetV = vaddq_n_u8(currentOffsetV,1);
        greenV = vldrbq_gather_offset_u8(pRGB,currentOffsetV);

        currentOffsetV = vaddq_n_u8(currentOffsetV,1);
        blueV = vldrbq_gather_offset_u8(pRGB,currentOffsetV);

        pRGB += 3*16;

        out = vdupq_n_u8(0);

        // Process bottom part
        gray = vdupq_n_u16(0);
        tmp = vmullbq_int_u8(redV,vdupq_n_u8(RED_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmullbq_int_u8(greenV,vdupq_n_u8(GREEN_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmullbq_int_u8(blueV,vdupq_n_u8(BLUE_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        gray = vaddq(gray,vdupq_n_u16(0x7F));

        out = vshrnbq_n_u16(out,gray,7);


        // Process top part
        gray = vdupq_n_u16(0);
        tmp = vmulltq_int_u8(redV,vdupq_n_u8(RED_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmulltq_int_u8(greenV,vdupq_n_u8(GREEN_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmulltq_int_u8(blueV,vdupq_n_u8(BLUE_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        gray = vaddq(gray,vdupq_n_u16(0x7F));

        out = vshrntq_n_u16(out,gray,7);

        vst1q(pOut, out);

        /*
         * Decrement the blockSize loop counter
         */
        blkCnt--;
        /*
         * advance vector source and destination pointers
         */
        
        pOut += 16;
    }

    blkCnt = nb & 0xF;
    if (blkCnt > 0U)
    {
        mve_pred16_t p0 = vctp8q(blkCnt);

        currentOffsetV = colorOffsetV;
        redV=vldrbq_gather_offset_z_u8(pRGB,currentOffsetV,p0);

        currentOffsetV = vaddq_n_u8(currentOffsetV,1);
        greenV = vldrbq_gather_offset_z_u8(pRGB,currentOffsetV,p0);

        currentOffsetV = vaddq_n_u8(currentOffsetV,1);
        blueV = vldrbq_gather_offset_z_u8(pRGB,currentOffsetV,p0);
       
        out = vdupq_n_u8(0);

        // Process bottom part
        gray = vdupq_n_u16(0);
        tmp = vmullbq_int_u8(redV,vdupq_n_u8(RED_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmullbq_int_u8(greenV,vdupq_n_u8(GREEN_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmullbq_int_u8(blueV,vdupq_n_u8(BLUE_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        gray = vaddq(gray,vdupq_n_u16(0x7F));

        out = vshrnbq_n_u16(out,gray,7);


        // Process top part
        gray = vdupq_n_u16(0);
        tmp = vmulltq_int_u8(redV,vdupq_n_u8(RED_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmulltq_int_u8(greenV,vdupq_n_u8(GREEN_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        tmp = vmulltq_int_u8(blueV,vdupq_n_u8(BLUE_FACTOR_Q7));
        gray = vaddq(gray,tmp);

        gray = vaddq(gray,vdupq_n_u16(0x7F));

        out = vshrntq_n_u16(out,gray,7);

        vstrbq_p(pOut, out,p0);
    }
}

#else
void arm_rgb24_to_gray8(const arm_cv_image_rgb24_t* ImageIn,
                              arm_cv_image_gray8_t* ImageOut)
{
  const channel_uint8_t *pRGB  = ImageIn->pData;

  channel_uint8_t *pOut = ImageOut->pData;

  uint8_t r,g,b;
  q15_t gray;

  for(uint32_t height=0; height<ImageIn->height;height++)
  {
     for(uint32_t width=0; width<ImageIn->width;width++)
     {
         r =  *pRGB++;
         g =  *pRGB++;
         b =  *pRGB++;

         gray = (q15_t) (RED_FACTOR_Q7) * (r);
         gray += (q15_t) (GREEN_FACTOR_Q7) * (g);
         gray += (q15_t) (BLUE_FACTOR_Q7) * (b);
         
         *pOut++ = (q7_t) __USAT((gray + 0x7F)>> 7, 8);
     }
  }
}
#endif