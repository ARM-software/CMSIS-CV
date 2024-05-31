/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_yuv240_to_rgb24
 * Description:  Convertion of yuv240 to rgb24
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

/**
  @ingroup colorTransform
 */


#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
#include "arm_cv_common.h"

#define COMP(A,B,FA,FB,FC)                                    \
        tmp_half = vmovl##A##q_u8(y);                         \
        yw = vmovl##B##q_s16(vreinterpretq_u16_s16(tmp_half));\
        yw = vsubq_n_s32(yw,16);                              \
                                                              \
        tmp_half = vmovl##A##q_u8(u);                         \
        uw = vmovl##B##q_s16(vreinterpretq_u16_s16(tmp_half));\
        uw = vsubq_n_s32(uw,128);                             \
                                                              \
        tmp_half = vmovl##A##q_u8(v);                         \
        vw = vmovl##B##q_s16(vreinterpretq_u16_s16(tmp_half));\
        vw = vsubq_n_s32(vw,128);                             \
                                                              \
        tmp0 = vdupq_n_s32(0);                                \
        tmp0 = vmlaq_n_s32(tmp0,yw,(FA));                     \
        tmp0 = vmlaq_n_s32(tmp0,uw,(FB));                     \
        tmp0 = vmlaq_n_s32(tmp0,vw,(FC));                     \
                                                              \
        p = vcmpltq_n_s32(tmp0,0);                            \
        tmp0 = vdupq_m_n_s32(tmp0,0,p);                       \
        tmp0 = vshlq(tmp0,vdupq_n_s32(-17));

#define COMPRES(FA,FB,FC)                                                    \
        res = vdupq_n_u8(0);                                                 \
        tmp_half_res = vdupq_n_u16(0);                                       \
                                                                             \
        COMP(b,b,FA,FB,FC);                                                  \
        tmp_half_res = vmovnbq_u32(tmp_half_res,vreinterpretq_s32_u32(tmp0));\
                                                                             \
        COMP(b,t,FA,FB,FC);                                                  \
        tmp_half_res = vmovntq_u32(tmp_half_res,vreinterpretq_s32_u32(tmp0));\
        res = vqmovnbq_u16(res,tmp_half_res);                             \
        tmp_half_res = vdupq_n_u16(0);                                       \
                                                                             \
        COMP(t,b,FA,FB,FC);                                                  \
        tmp_half_res = vmovnbq_u32(tmp_half_res,vreinterpretq_s32_u32(tmp0));\
                                                                             \
        COMP(t,t,FA,FB,FC);                                                  \
        tmp_half_res = vmovntq_u32(tmp_half_res,vreinterpretq_s32_u32(tmp0));\
        res = vqmovntq_u16(res,tmp_half_res);

#define PACKRGB(OUT)                                                     \
        vstrbq_scatter_offset_u8((uint8_t*)cv_sgbuf,redOffsetV,redV);    \
        vstrbq_scatter_offset_u8((uint8_t*)cv_sgbuf,greenOffsetV,greenV);\
        vstrbq_scatter_offset_u8((uint8_t*)cv_sgbuf,blueOffsetV,blueV);  \
                                                                         \
        res = vld1q_u8((uint8_t*)cv_sgbuf);                              \
        vst1q(OUT, res);                                                 \
        OUT += 16;                                                       \
                                                                         \
        res = vld1q_u8((uint8_t*)cv_sgbuf+16);                           \
        vst1q(OUT, res);                                                 \
        OUT += 16;                                                       \
                                                                         \
        res = vld1q_u8((uint8_t*)cv_sgbuf+32);                           \
        vst1q(OUT, res);                                                 \
        OUT += 16;
#endif

/**     
 * @brief      YUV420 to packed RGB24
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * 
 * @par YUV format
 * Y,U and V are in different planes with different dimensions
 *      
 * @par Details about the transform ITU-R BT.601
 *
 * \f[
 *   r = 1.16414 * (y-16) - 0.0017889 * (u-128) + 1.59579    * (v-128) \\
 *   g = 1.16414 * (y-16) - 0.391443  * (u-128) - 0.813482   * (v-128) \\
 *   b = 1.16414 * (y-16) + 2.01783   * (u-128) - 0.00124584 * (v-128) \\
 * \f]
 *

 */
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_yuv420_to_rgb24(const arm_cv_image_yuv420_t* ImageIn,
                               arm_cv_image_rgb24_t* ImageOut)
{
  static const int8_t dup_bottom[16]={0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7};
  static const int8_t dup_top[16]={8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15};

  const uint32_t nb = ImageIn->width * ImageIn->height; 
  const channel_uint8_t *pYE = ImageIn->pData;
  const channel_uint8_t *pYO = ImageIn->pData + ImageIn->width;
  const channel_uint8_t *pU = pYE + nb;
  const channel_uint8_t *pV = pU + (nb>>2);
  uint32_t  blkCnt;  

  int8x16_t redOffsetV;
  int8x16_t greenOffsetV;
  int8x16_t blueOffsetV;

  redOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
  redOffsetV = vmulq_n_s8(redOffsetV,3);

  greenOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
  greenOffsetV = vmulq_n_s8(greenOffsetV,3);
  greenOffsetV = vaddq_n_s8(greenOffsetV,1);

  blueOffsetV = vreinterpretq_u8_s8(vidupq_n_u8(0,1));
  blueOffsetV = vmulq_n_s8(blueOffsetV,3);
  blueOffsetV = vaddq_n_s8(blueOffsetV,2);

  channel_uint8_t *pOutE = ImageOut->pData ;
  channel_uint8_t *pOutO = ImageOut->pData + 3*ImageIn->width;

  for(uint32_t height=0; height<ImageIn->height;height+=2)
  {
     blkCnt = ImageIn->width >> 5;
     while (blkCnt > 0U)
     {
        int32x4_t tmp0;

        uint8x16_t y ;
        uint8x16_t ru   = vld1q(pU);
        uint8x16_t rv   = vld1q(pV);

        uint8x16_t u,v;

        
        pU += 16;
        pV += 16;


        uint8x16_t redV;
        uint8x16_t greenV;
        uint8x16_t blueV;
        uint8x16_t offsetV;


        uint16x8_t tmp_half;

        uint16x8_t tmp_half_res;
        uint8x16_t res;

        mve_pred16_t p;


        int32x4_t yw;
        int32x4_t uw;
        int32x4_t vw;

        /**********/

        y = vld1q(pYE);
        pYE += 16;

        vst1q((uint8_t*)cv_sgbuf,ru);
        offsetV = vld1q(dup_bottom);
        u = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        vst1q((uint8_t*)cv_sgbuf,rv);
        v = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        // Fixed point coefficients are in Q17 format
        COMPRES(152586,-234,209163);
        redV=res;

        COMPRES(152586,-51307,-106625);
        greenV=res;
       
        COMPRES(152586,264481,-163);
        blueV = res;

        PACKRGB(pOutE)

        /**********/

        y = vld1q(pYE);
        pYE += 16;

        vst1q((uint8_t*)cv_sgbuf,ru);
        offsetV = vld1q(dup_top);
        u = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        vst1q((uint8_t*)cv_sgbuf,rv);
        v = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);
        
        COMPRES(152586,-234,209163);
        redV=res;

        COMPRES(152586,-51307,-106625);
        greenV=res;
       
        COMPRES(152586,264481,-163);
        blueV = res;

        PACKRGB(pOutE)


        /**********/

        y = vld1q(pYO);
        pYO += 16;

        vst1q((uint8_t*)cv_sgbuf,ru);
        offsetV = vld1q(dup_bottom);
        u = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        vst1q((uint8_t*)cv_sgbuf,rv);
        v = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        COMPRES(152586,-234,209163);
        redV=res;

        COMPRES(152586,-51307,-106625);
        greenV=res;
       
        COMPRES(152586,264481,-163);
        blueV = res;

        PACKRGB(pOutO);

        y = vld1q(pYO);
        pYO += 16;

        vst1q((uint8_t*)cv_sgbuf,ru);
        offsetV = vld1q(dup_top);
        u = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        vst1q((uint8_t*)cv_sgbuf,rv);
        v = vldrbq_gather_offset_u8((uint8_t*)cv_sgbuf,offsetV);

        COMPRES(152586,-234,209163);
        redV=res;

        COMPRES(152586,-51307,-106625);
        greenV=res;
       
        COMPRES(152586,264481,-163);
        blueV = res;

        PACKRGB(pOutO);

       

        blkCnt--;

     }
     blkCnt = ImageIn->width & 0x1F;
     for(uint32_t width=0; width<blkCnt;width+=2)
     {
        int32_t tmp;
        int16_t y00 = (int16_t)(*pYE++) - 16;
        int16_t y01 = (int16_t)(*pYE++) - 16;
        int16_t y10 = (int16_t)(*pYO++) - 16;
        int16_t y11 = (int16_t)(*pYO++) - 16;
        int16_t u   = (int16_t)(*pU++) - 128;
        int16_t v   = (int16_t)(*pV++) - 128;

        //float r = 1.16414f * y00 - 0.0017889f * u + 1.59579f * v;
        //float g = 1.16414f * y00 - 0.391443f  * u - 0.813482f * v;
        //float b = 1.16414f * y00 + 2.01783f   * u - 0.00124584f * v;
        //
        // Fixed point coefficients are in Q17 format
        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y01 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y01 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y01 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y10 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y10 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y10 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y11 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y11 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y11 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


     }

     pYE += ImageIn->width;
     pYO += ImageIn->width;

     pOutE += 3*ImageIn->width;
     pOutO += 3*ImageIn->width;
  }
}
#else
void arm_yuv420_to_rgb24(const arm_cv_image_yuv420_t* ImageIn,
                               arm_cv_image_rgb24_t* ImageOut)
{
  const uint32_t nb = ImageIn->width * ImageIn->height; 
  const channel_uint8_t *pYE = ImageIn->pData;
  const channel_uint8_t *pYO = ImageIn->pData + ImageIn->width;
  const channel_uint8_t *pU = pYE + nb;
  const channel_uint8_t *pV = pU + (nb>>2);
  
  channel_uint8_t *pOutE = ImageOut->pData ;
  channel_uint8_t *pOutO = ImageOut->pData + 3*ImageIn->width;

  for(uint32_t height=0; height<ImageIn->height;height+=2)
  {
     for(uint32_t width=0; width<ImageIn->width;width+=2)
     {
        int32_t tmp;
        int16_t y00 = (int16_t)(*pYE++) - 16;
        int16_t y01 = (int16_t)(*pYE++) - 16;
        int16_t y10 = (int16_t)(*pYO++) - 16;
        int16_t y11 = (int16_t)(*pYO++) - 16;
        int16_t u   = (int16_t)(*pU++) - 128;
        int16_t v   = (int16_t)(*pV++) - 128;

        //float r = 1.16414f * y00 - 0.0017889f * u + 1.59579f * v;
        //float g = 1.16414f * y00 - 0.391443f  * u - 0.813482f * v;
        //float b = 1.16414f * y00 + 2.01783f   * u - 0.00124584f * v;
        //
        // Fixed point coefficients are in Q17 format
        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y00;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y01 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y01 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y01 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y01;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutE++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y10 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y10 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y10 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y10;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        //r = 1.16414f * y11 - 0.0017889f * u + 1.59579f * v;
        //g = 1.16414f * y11 - 0.391443f  * u - 0.813482f * v;
        //b = 1.16414f * y11 + 2.01783f   * u - 0.00124584f * v;

        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp -= (int32_t)234 * u;
        tmp += (int32_t)209163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);

        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp -= (int32_t)51307 * u;
        tmp -= (int32_t)106625 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


        tmp = 0;
        tmp += (int32_t)152586 * y11;
        tmp += (int32_t)264481 * u;
        tmp -= (int32_t)163 * v;
        tmp = tmp < 0 ? 0 : tmp;
        *pOutO++ = __USAT((uint16_t)(tmp >> 17),8);


     }
     pYE += ImageIn->width;
     pYO += ImageIn->width;

     pOutE += 3*ImageIn->width;
     pOutO += 3*ImageIn->width;
  }
}
#endif