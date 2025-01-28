/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_image_resize_bgr_8U3C
 * Description:  image resize of planar BGR
 *
 * Target Processor: Cortex-M
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2024 Himax Technologies, Inc. or its affiliates. All rights reserved.
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
#include "arm_image_resize_common.h"

/**
  @ingroup imageTransform
 */


/**     
 * @brief      Resize BGR 8U3C image (float implementation)
 *
 * @param[in]  ImageIn   The input image
 * @param      ImageOut  The output image
 * @param[in,out]  p_img   Temporary buffer
 * 
 * @par  Temporary buffer sizing:
 * 
 * Size of temporary buffers:
 *   - p_img\n
 *     2 * ImageOut->width*sizeof(uint8_t)
 */
void arm_image_resize_bgr_8U3C_f32(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                         arm_cv_image_bgr_8U3C_t* ImageOut,
                                         uint8_t *p_img)
{
    _common_resize_t common;

    common.input_w = ImageIn->width;
    common.input_h = ImageIn->height;

    common.output_w = ImageOut->width;
    common.output_h = ImageOut->height;

	  common.w_scale = (float)(common.input_w - 1) / (common.output_w - 1);
	  common.h_scale = (float)(common.input_h - 1) / (common.output_h - 1);	
	

    common.pIn  = ImageIn->pData;
    common.pOut = ImageOut->pData;
	  arm_image_resize_common_f32(&common,p_img);

	  common.pIn  = ImageIn->pData + (common.input_w * common.input_h);
    common.pOut = ImageOut->pData + (common.output_w * common.output_h);
	  arm_image_resize_common_f32(&common,p_img);

	  common.pIn  = ImageIn->pData + 2 * (common.input_w * common.input_h);
    common.pOut = ImageOut->pData + 2 * (common.output_w * common.output_h);
	  arm_image_resize_common_f32(&common,p_img);


}

