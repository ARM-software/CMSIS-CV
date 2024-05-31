/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        gaussian.c
 * Description:  Gaussian filter CMSIS-CV
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
#include "cv/linear_filters.h"
#include "dsp/basic_math_functions.h"

static const uint8_t gaussian_kernel[9] = {   0x08, 0x10, 0x08,
                                            0x10, 0x20, 0x10,
                                            0x08, 0x10, 0x08
                                        };



/**
  @ingroup linearFilter
 */

/**     
 * @brief      Gaussian filter for grayscale data computing in q15
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 */
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                        arm_cv_image_gray8_t* ImageOut)
{
    /* To be optimized */
    const int w =ImageIn->width;
    int indice;
    mve_pred16_t p0 = vctp8q(9);
    uint32_t res;
    for( int y = 1; y< ImageIn -> height- 1; y++)
    {
        for( int x = 1; x< ImageIn-> width- 1; x++)
        {
            indice = y*ImageIn->width+x;
            res = 0;
            uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};
            
            uint8x16_t vecA = vld1q(gaussian_kernel);
            uint8x16_t vecB = vld1q(matrix_in);
            
            res = vmladavaq_p(res, vecA, vecB, p0);
            //this dot product output on 2.14, so we need to shift by 7 after to return to uint8 values
            res = res + 0x7F;
            res = __USAT(res>>(7),8);
            ImageOut->pData[indice] =(uint8_t)(res);
        }
        
        
    }
    
    /*

    To rework. Incorrect padding.

    */
    
    
    int x = 0;
    int y = 0;
    //top left corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[0] + ImageIn->pData[0 + 1] + ImageIn->pData[0 + w] + ImageIn->pData[0 + w + 1])>>2;
    x = ImageIn->width-1;
    y = ImageIn->height-1;
    //bottom right corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y-1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x-1)] + ImageIn->pData[(y-1)*ImageOut->width +(x-1)])>>2;
    y = ImageIn->height-1;
    x = 0;
    //bottom left corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y-1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x+1)] + ImageIn->pData[(y-1)*ImageOut->width +(x+1)])>>2;
    y = 0;
	x = ImageIn->width-1;
    //top right corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y+1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x-1)] + ImageIn->pData[(y+1)*ImageOut->width +(x-1)])>>2;
    y = 0;
    x = 0;
    // x0y0
    if(ImageIn->width==1)
    {
        if(ImageIn->height==1)
        {
            ImageOut->pData[0] = ImageIn->pData[0];
        }
        else
        {
            y = 0;
            x = 0;
            //lack top and bot alues
            for( int y =1; y < ImageIn->height-1; y++)
	        {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w], ImageIn->pData[indice-w], ImageIn->pData[indice-w], ImageIn->pData[indice ], ImageIn->pData[indice], ImageIn->pData[indice], ImageIn->pData[indice +w], ImageIn->pData[ indice + w], ImageIn->pData[indice + w]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
        }
    }
    else
    {
        if(ImageIn->height==1)
        {
            x = 0;
            y = 0;
            //bottom line
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-1], ImageIn->pData[indice], ImageIn->pData[indice+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice -1], ImageIn->pData[ indice], ImageIn->pData[indice -+1]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
        }
        else
        {
            x = 0;
            y=0;
            //top part
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice+w-1], ImageIn->pData[indice+w], ImageIn->pData[indice+w+1], ImageIn->pData[indice + 1], ImageIn->pData[indice], ImageIn->pData[indice - 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
            x = 0;
            y = ImageIn->height-1;
            //bottom line
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice -w -1], ImageIn->pData[ indice - w], ImageIn->pData[indice - w+1]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
            y = 0;
            x = 0;
            //left border else width 1
            for( int y =1; y < ImageIn->height-1; y++)
	        {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w+1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice +1 ], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice +w +1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
	        x = ImageIn->width-1;
            //right border else width 1
            for( int y =1; y < ImageIn->height-1; y++)
	        {
	        	indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w-1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice - 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w-1]};

                uint8x16_t vecA = vld1q(gaussian_kernel);
                uint8x16_t vecB = vld1q(matrix_in);

                res = vmladavaq_p(res, vecA, vecB, p0);
                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
        }
    }
}
#else
void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                        arm_cv_image_gray8_t* ImageOut)
{
    /* To be optimized */
    const int w =ImageIn->width;
    int indice;
    uint32_t res;
    for( int y = 1; y< ImageIn -> height- 1; y++)
    {
        for( int x = 1; x< ImageIn-> width- 1; x++)
        {
            indice = y*ImageIn->width+x;
            uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};
            res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

            res = res + 0x7F;
            res = __USAT(res>>(7),8);
            ImageOut->pData[indice] =(uint8_t)(res);
        }
        
        
    }
    
    /*

    To rework. Incorrect padding.

    */
    
    
    int x = 0;
    int y = 0;
    //top left corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[0] + ImageIn->pData[0 + 1] + ImageIn->pData[0 + w] + ImageIn->pData[0 + w + 1])>>2;
    x = ImageIn->width-1;
    y = ImageIn->height-1;
    //bottom right corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y-1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x-1)] + ImageIn->pData[(y-1)*ImageOut->width +(x-1)])>>2;
    y = ImageIn->height-1;
    x = 0;
    //bottom left corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y-1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x+1)] + ImageIn->pData[(y-1)*ImageOut->width +(x+1)])>>2;
    y = 0;
	x = ImageIn->width-1;
    //top right corner
    ImageOut->pData[y*ImageOut->width +x] = (ImageIn->pData[y*ImageOut->width +x] + ImageIn->pData[(y+1)*ImageOut->width +x] + ImageIn->pData[y*ImageOut->width +(x-1)] + ImageIn->pData[(y+1)*ImageOut->width +(x-1)])>>2;
    y = 0;
    x = 0;
    // x0y0
    if(ImageIn->width==1)
    {
        if(ImageIn->height==1)
        {
            ImageOut->pData[0] = ImageIn->pData[0];
        }
        else
        {
            y = 0;
            x = 0;
            //lack top and bot alues
            for( int y =1; y < ImageIn->height-1; y++)
	        {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w], ImageIn->pData[indice-w], ImageIn->pData[indice-w], ImageIn->pData[indice ], ImageIn->pData[indice], ImageIn->pData[indice], ImageIn->pData[indice +w], ImageIn->pData[ indice + w], ImageIn->pData[indice + w]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
        }
    }
    else
    {
        if(ImageIn->height==1)
        {
            x = 0;
            y = 0;
            //bottom line
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-1], ImageIn->pData[indice], ImageIn->pData[indice+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice -1], ImageIn->pData[ indice], ImageIn->pData[indice -+1]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
        }
        else
        {
            x = 0;
            y=0;
            //top part
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice+w-1], ImageIn->pData[indice+w], ImageIn->pData[indice+w+1], ImageIn->pData[indice + 1], ImageIn->pData[indice], ImageIn->pData[indice - 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
            x = 0;
            y = ImageIn->height-1;
            //bottom line
            for( int x =1; x < ImageIn->width-1; x++)
            {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice -w -1], ImageIn->pData[ indice - w], ImageIn->pData[indice - w+1]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
            }
            y = 0;
            x = 0;
            //left border else width 1
            for( int y =1; y < ImageIn->height-1; y++)
	        {
                indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w+1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice +1 ], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice +w +1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
	        x = ImageIn->width-1;
            //right border else width 1
            for( int y =1; y < ImageIn->height-1; y++)
	        {
	        	indice = y*ImageIn->width+x;
                res = 0;
                uint8_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w-1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice - 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w-1]};
                res = gaussian_kernel[0]*matrix_in[0]+gaussian_kernel[1]*matrix_in[1]+gaussian_kernel[2]*matrix_in[2]+gaussian_kernel[3]*matrix_in[3]+gaussian_kernel[4]*matrix_in[4]+gaussian_kernel[5]*matrix_in[5]+gaussian_kernel[6]*matrix_in[6]+gaussian_kernel[7]*matrix_in[7]+gaussian_kernel[8]*matrix_in[8];

                res = __USAT(res>>(7),8);

                ImageOut->pData[y*ImageOut->width +x] = res;
	        }
        }
    }
}
#endif

