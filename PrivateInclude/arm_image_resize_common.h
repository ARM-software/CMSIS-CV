/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_image_resize_common.h
 * Description:  Common declarations for CMSIS-CV image resize functions
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2024 Himax Technologies, Inc. or its affiliates. All rights reserved.
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
#ifndef ARM_CV_IMG_RESIZE_COMMON_H
#define ARM_CV_IMG_RESIZE_COMMON_H

#include "arm_math_types.h"
#include "arm_cv_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct {
    int input_w;
    int input_h;

    int output_w;
    int output_h;
 
    channel_uint8_t *pIn;
    channel_uint8_t *pOut;

    float w_scale;
    float h_scale;

} _common_resize_t;

extern void arm_image_resize_common_f32(const _common_resize_t *common,
                                        uint8_t *p_img);

__STATIC_FORCEINLINE float get_image_pixel(uint8_t* m, int x, int y, int w)
{
    return m[y * w + x];
}

__STATIC_FORCEINLINE void set_image_pixel(uint8_t* m, int x, int y, float val, int w)
{
    /* x, y, c are already validated by upper layers */
    m[y * w + x] = (uint8_t)(val+0.5f);
}


#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)

__STATIC_FORCEINLINE uint32x4_t get_image_pixel_index(int32x4_t x_s32, int32x4_t y_s32, float32_t w)
{
    uint32x4_t pixel_u32;
    float32x4_t x_f32, y_f32 , pixel_f32;

    x_f32 = vcvtq_f32_s32(x_s32);
    y_f32 = vcvtq_f32_s32(y_s32);
    y_f32 = vmulq_n_f32(y_f32, w);
    pixel_f32 = vaddq_f32(x_f32, y_f32);
    pixel_u32 = vcvtq_u32_f32(pixel_f32);
    return pixel_u32;
}

#endif

#ifdef   __cplusplus
}
#endif

#endif

