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

static const q15_t grad[9] = {   0x0800, 0x1000, 0x0800,
                                 0x1000, 0x2000, 0x1000,
                                 0x0800, 0x1000, 0x0800
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
void arm_gaussian_filter_3x3_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                        arm_cv_image_q15_t* ImageOut)
{
    /* To be optimized */
    const int w =ImageIn->width;
    int indice;
    for( int y = 1; y< ImageIn -> height- 1; y++)
    {
        for( int x = 1; x< ImageIn-> width- 1; x++)
        {
            indice = y*ImageIn->width+x;
            q63_t res;
            q15_t matrix_in[9] = { ImageIn->pData[indice-w-1], ImageIn->pData[indice-w], ImageIn->pData[indice-w+1], ImageIn->pData[indice - 1], ImageIn->pData[indice], ImageIn->pData[indice + 1], ImageIn->pData[indice +w -1], ImageIn->pData[ indice + w], ImageIn->pData[indice + w+1]};
            //this dot product output on 34.30, so a shift of 15 is enought to bring us back into q15 but because we didn't convert our input Image previously, we have to do a shift to the left by 7, this is possible due to the increase of precisionn of the multiplication/dotproduct, so we only need a shif of 15-7 8
            arm_dot_prod_q15(&grad[0], &matrix_in[0], 9, &res);
            res = res>>8;
            ImageOut->pData[indice] =(q15_t)(res);
        }
        
        
    }
    
    /*

    To rework. Incorrect padding.

    */
    int x = 0;
    int y = 0;
    for( int y =0; y < ImageIn->width; y++)
	{
		ImageOut->pData[x*ImageOut->width +y] = ImageIn->pData[x*ImageOut->width +y] <<7;
	}
	x = ImageIn->width-1;
	for( int y =0; y < ImageIn->width; y++)
	{
		ImageOut->pData[x*ImageOut->width +y] = ImageIn->pData[x*ImageOut->width +y] <<7;
	}
	y=0;
	for( int x =1; x < ImageIn->width; x++)
	{
		ImageOut->pData[x*ImageOut->width +y] = ImageIn->pData[x*ImageOut->width +y] <<7;
	}
    y = ImageIn->width-1;
	for( int x =1; x < ImageIn->width; x++)
	{
		ImageOut->pData[x*ImageOut->width +y] = ImageIn->pData[x*ImageOut->width +y] <<7;
	}
}


