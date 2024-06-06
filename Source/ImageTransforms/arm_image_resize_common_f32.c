/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_image_resize_common
 * Description:  common code used by different resize functions
 *
 * Target Processor: Cortex-M and Cortex-A cores
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

#include "arm_image_resize_common.h"

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
#include "arm_cv_common.h"
#endif

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_image_resize_common_f32(const _common_resize_t *common,
                                 uint8_t *p_img)
{
    const int input_w = common->input_w;
    //const int input_h = common->input_h;

    const int output_w = common->output_w;
    const int output_h = common->output_h;

 
    channel_uint8_t *pIn  = common->pIn;
    channel_uint8_t *pOut = common->pOut;

    float w_scale = common->w_scale;
    float h_scale = common->h_scale;  

    int32_t r,c,d, loc;

    float32x4_t sx_f32;
    int32x4_t ix_s32, ix2_s32, iy_s32, temp_s32;
    float32x4_t ix_f32;
    float32x4_t dx_f32, dy_f32;

    uint32x4_t index_u32, temp_u32, total_x_u32, temp_y_u32;
    float32x4_t index_f32;
    uint32x4_t pixel_u32;
    float32x4_t val_f32, weight_f32, pixel_f32, temp_f32, total_f32;
    uint32_t p_img_loc;
    // uint8_t *p_img = calloc(2 * output_w, sizeof(uint8_t));
    uint32_t temp_u;
        // float val = 0;
        float32_t sy;
        int32_t iy = 0, pre_iy = -1, _iy;
        float32_t dy;       
        int proc_y_offset;
        for (r = 0; r < output_h - 1 ; ++r) {
            proc_y_offset = output_w * r;
            sy = r * h_scale;
            iy = (int)sy;
            dy = sy - iy;   
            if(iy != pre_iy){
                for(d = 0; d < 2; ++d){
                    for(c = 0; c < output_w - 4; c += 4)
                    {   
                        index_u32 = vidupq_n_u32(0, 1);     
                        temp_u = (uint32_t)c;
                        index_u32 = vaddq_n_u32(index_u32, temp_u);
                        index_f32 = vcvtq_f32_u32(index_u32);   

                        //sx = c * w_scaled
                        sx_f32 = vmulq_n_f32(index_f32, w_scale);
                        ix_f32 = vrndmq_f32(sx_f32);
                        //dx = sx - ix
                        dx_f32 = vsubq_f32(sx_f32, ix_f32);
                        ix_s32 = vcvtq_s32_f32(ix_f32);
                        _iy = iy+d;
                        iy_s32 = vdupq_n_s32(_iy);

                        //val = dx * pixel'
                        ix2_s32 = vaddq_n_s32(ix_s32, 1);
                        pixel_u32 = get_image_pixel_index(ix2_s32, iy_s32, (float32_t)input_w);
                        temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                        val_f32 = vcvtq_f32_u32(temp_u32);
                        total_f32 = vmulq_f32(val_f32, dx_f32);

                        //val += (1+dx) * pixel'
                        temp_f32 = vdupq_n_f32(1.0f);
                        weight_f32 = vsubq_f32(temp_f32, dx_f32);
                        pixel_u32 = get_image_pixel_index(ix_s32, iy_s32, (float32_t)input_w);      
                        temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                        pixel_f32 = vcvtq_f32_u32(temp_u32);
                        val_f32 = vmulq_f32(pixel_f32, weight_f32); 
                        total_f32 = vaddq_f32(total_f32, val_f32);

                        total_f32 = vaddq_n_f32(total_f32, 0.5f);  
                        p_img_loc = output_w * d + c;
                        total_x_u32 = vcvtq_u32_f32(total_f32);
                        vstrbq_u32(&(p_img[p_img_loc]), total_x_u32);
                    }

                    index_u32 = vidupq_n_u32(0, 1);
                    c = output_w - 4;
                    temp_s32 = vdupq_n_s32(c);

                    temp_u32 = vreinterpretq_u32_s32(temp_s32);
                    index_u32 = vaddq_u32(index_u32, temp_u32);                 
                    index_f32 = vcvtq_f32_u32(index_u32);   

                    //sx = c * w_scaled
                    sx_f32 = vmulq_n_f32(index_f32, w_scale);
                    ix_f32 = vrndmq_f32(sx_f32);
                    
                    //dx = sx - ix
                    dx_f32 = vsubq_f32(sx_f32, ix_f32);
                    dx_f32 = vsetq_lane_f32((float32_t)0.0, dx_f32, 3);
                    ix_s32 = vcvtq_s32_f32(ix_f32);
                    _iy = iy+d;
                    iy_s32 = vdupq_n_s32(_iy);

                    //val = dx * pixel'
                    ix2_s32 = vaddq_n_s32(ix_s32, 1);
                    // ix2_s32 = vsetq_lane_s32(0, ix_s32, 3);
                    pixel_u32 = get_image_pixel_index(ix2_s32, iy_s32, (float32_t)input_w);
                    temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                    val_f32 = vcvtq_f32_u32(temp_u32);
                    total_f32 = vmulq_f32(val_f32, dx_f32);

                    //val += (1+dx) * pixel'
                    ix_s32 = vsetq_lane_s32(input_w-1, ix_s32, 3); 
                    temp_f32 = vdupq_n_f32(1.0f);
                    weight_f32 = vsubq_f32(temp_f32, dx_f32);
                    weight_f32 = vsetq_lane_f32((float32_t)1.0, weight_f32, 3);
                    pixel_u32 = get_image_pixel_index(ix_s32, iy_s32, (float32_t)input_w);
                    temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                    pixel_f32 = vcvtq_f32_u32(temp_u32);
                    val_f32 = vmulq_f32(pixel_f32, weight_f32); 
                    total_f32 = vaddq_f32(total_f32, val_f32);                                                      
                    p_img_loc = output_w * d + c;
                    // total_f32 = vrndnq_f32(total_f32);
                    total_f32 = vaddq_n_f32(total_f32, 0.5f);  
                    total_x_u32 = vcvtq_u32_f32(total_f32);
                    vstrbq_u32(&(p_img[p_img_loc]), total_x_u32);                   
                }
            }

            for (c = 0; c < output_w; c += 4){
                temp_s32 = vdupq_n_s32(c);
                index_u32 = vidupq_n_u32(0, 1); 

                temp_u = (uint32_t)c;
                index_u32 = vaddq_n_u32(index_u32, temp_u);

                /*pixel loc= p_img[1][c, ..., c+3]*/
                temp_u = output_w;  
                pixel_u32 = vaddq_n_u32(index_u32, temp_u);

                /*val = dy*pixel */
                temp_u32 = vldrbq_gather_offset(&(p_img[0]), pixel_u32);
                val_f32 = vcvtq_f32_u32(temp_u32);
                dy_f32 = vdupq_n_f32(dy);
                total_f32 = vmulq_f32(val_f32, dy_f32); 
                
                /*val += (1 - dy) * pixel' 
                  pixel' loc= p_img[0][c,...,c+3]*/
                temp_f32 = vdupq_n_f32(1.0f);
                weight_f32 = vsubq_f32(temp_f32, dy_f32);       
                // pixel_u32 = vcvtq_u32_f32(index_f32);    
                pixel_u32 = index_u32;
                temp_u32 = vldrbq_gather_offset(&(p_img[0]), pixel_u32);
                val_f32 = vcvtq_f32_u32(temp_u32);
                val_f32 = vmulq_f32(val_f32, weight_f32);   
                // val_f32 = vrndmq(val_f32); 
                total_f32 = vaddq_f32(total_f32, val_f32);
                
                
                loc = proc_y_offset + c;
                total_f32 = vaddq_n_f32(total_f32, 0.5f); 
                temp_y_u32 = vcvtq_u32_f32(total_f32);
                vstrbq_u32(&(pOut[loc]), temp_y_u32);   
            }
            
            pre_iy = iy;    
        }

        r = output_h - 1;
        sy = r * h_scale;
        iy = (int)sy;
        dy = sy - iy;   
        proc_y_offset = output_w * r;
        if(iy != pre_iy){
            for(c = 0; c < output_w-4 ; c += 4)
            {   
                index_u32 = vidupq_n_u32(0, 1);

                temp_u = (uint32_t)c;
                index_u32 = vaddq_n_u32(index_u32, temp_u);
                index_f32 = vcvtq_f32_u32(index_u32);   

                //sx = c * w_scaled
                sx_f32 = vmulq_n_f32(index_f32, w_scale);
                ix_f32 = vrndmq_f32(sx_f32);
                //dx = sx - ix
                dx_f32 = vsubq_f32(sx_f32, ix_f32);
                ix_s32 = vcvtq_s32_f32(ix_f32);
                iy_s32 = vdupq_n_s32(iy);

                //val = dx * pixel'
                ix2_s32 = vaddq_n_s32(ix_s32, 1);
                pixel_u32 = get_image_pixel_index(ix2_s32, iy_s32, input_w);
                temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                val_f32 = vcvtq_f32_u32(temp_u32);
                total_f32 = vmulq_f32(val_f32, dx_f32);

                //val += (1+dx) * pixel'
                temp_f32 = vdupq_n_f32(1.0f);
                weight_f32 = vsubq_f32(temp_f32, dx_f32);
                pixel_u32 = get_image_pixel_index(ix_s32, iy_s32, input_w); 
                temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
                pixel_f32 = vcvtq_f32_u32(temp_u32);
                val_f32 = vmulq_f32(pixel_f32, weight_f32); 
                total_f32 = vaddq_f32(total_f32, val_f32);                      
                
                // val_f32 = vaddq_n_f32(val_f32, 0.5f);  //need use vrndq/vrndnq/vrndmq, not use (float)(val+0.5f)
                p_img_loc = c;
                // total_f32 = vrndnq_f32(total_f32);
                total_f32 = vaddq_n_f32(total_f32, 0.5f);  
                total_x_u32 = vcvtq_u32_f32(total_f32);
                vstrbq_u32(&(p_img[p_img_loc]), total_x_u32);   
            }
    
            c = output_w - 4;
            index_u32 = vidupq_n_u32(0, 1);
            temp_s32 = vdupq_n_s32(c);

            temp_u32 = vreinterpretq_u32_s32(temp_s32);
            index_u32 = vaddq_u32(index_u32, temp_u32);                 
            index_f32 = vcvtq_f32_u32(index_u32);   

            //sx = c * w_scaled
            sx_f32 = vmulq_n_f32(index_f32, w_scale);
            ix_f32 = vrndmq_f32(sx_f32);
            //dx = sx - ix
            dx_f32 = vsubq_f32(sx_f32, ix_f32);
            dx_f32 = vsetq_lane_f32((float32_t)0.0, dx_f32, 3);
            ix_s32 = vcvtq_s32_f32(ix_f32);
            iy_s32 = vdupq_n_s32(iy);

            //val = dx * pixel'
            loc = input_w * iy + input_w - 1;
            ix2_s32 = vaddq_n_s32(ix_s32, 1);
            // ix2_s32 = vsetq_lane_s32(0, ix_s32, 3);
            pixel_u32 = get_image_pixel_index(ix2_s32, iy_s32, input_w);
            temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
            val_f32 = vcvtq_f32_u32(temp_u32);
            total_f32 = vmulq_f32(val_f32, dx_f32);

            //val += (1+dx) * pixel'
            ix_s32 = vsetq_lane_s32(input_w-1, ix_s32, 3); 
            temp_f32 = vdupq_n_f32(1.0f);
            weight_f32 = vsubq_f32(temp_f32, dx_f32);
            weight_f32 = vsetq_lane_f32((float32_t)1.0, weight_f32, 3);
            pixel_u32 = get_image_pixel_index(ix_s32, iy_s32, input_w);
            temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
            pixel_f32 = vcvtq_f32_u32(temp_u32);
            val_f32 = vmulq_f32(pixel_f32, weight_f32); 
            total_f32 = vaddq_f32(total_f32, val_f32);                                                      
            p_img_loc = c;
            // total_f32 = vrndnq_f32(total_f32);
            total_f32 = vaddq_n_f32(total_f32, 0.5f);  
            total_x_u32 = vcvtq_u32_f32(total_f32);
            vstrbq_u32(&(p_img[p_img_loc]), total_x_u32);       
    
        }   
        //to cal val in the last colum
        for (c = 0; c < output_w; c += 4){
            temp_s32 = vdupq_n_s32(c);
            index_u32 = vidupq_n_u32(0, 1);

            temp_u32 = vreinterpretq_u32_s32(temp_s32);
            pixel_u32 = vaddq_u32(index_u32, temp_u32);

            ////let the latest row be exactly the same with lastest tmp buffer
             /*val += pixel' 
                pixel' loc= p_img[0][c,...,c+3]*/    
            temp_u32 = vldrbq_gather_offset(&(p_img[0]), pixel_u32);
            val_f32 = vcvtq_f32_u32(temp_u32); 
            
            loc = proc_y_offset + c;
            // val_f32 = vrndnq_f32(val_f32);
            val_f32 = vaddq_n_f32(val_f32, 0.5f);  
            temp_y_u32 = vcvtq_u32_f32(val_f32);
            vstrbq_u32(&(pOut[loc]), temp_y_u32);   
        }       

}
#else


void arm_image_resize_common_f32(const _common_resize_t *common,
                                 uint8_t *p_img)
{
    const int input_w = common->input_w;
    const int input_h = common->input_h;

    const int output_w = common->output_w;
    const int output_h = common->output_h;

 
    channel_uint8_t *pIn  = common->pIn;
    channel_uint8_t *pOut = common->pOut;

    float w_scale = common->w_scale;
    float h_scale = common->h_scale;  

    int row,col, d;

    if(input_w == 1 || input_h == 1)
        return;

    float re_v = 0;
    float sy;
    int iy, pre_iy = -1;
    float dy;
    for (row = 0; row < output_h - 1 ; ++row) {
        sy = row * h_scale;
        iy = (int)sy;
        dy = sy - iy;   
        if(iy != pre_iy){
            for(d = 0; d < 2; ++d){
                for(col = 0; col < output_w - 1; ++col)
                {
                    float re_v = 0;
                    float sx = col * w_scale;
                    int ix = (int)sx;
                    float dx = sx - ix;     
                    re_v = (1 - dx) * get_image_pixel(pIn, ix, iy + d, input_w) + dx * get_image_pixel(pIn, ix + 1, iy + d, input_w);
                    set_image_pixel(p_img, col, d, re_v, output_w);
                }
                re_v = get_image_pixel(pIn, input_w - 1, iy + d, input_w);
                set_image_pixel(p_img, col, d, re_v, output_w);
            }               
        }
        
        for (col = 0; col < output_w; ++col){
            re_v = (1 - dy) * get_image_pixel(p_img, col, 0, output_w);
            re_v = (uint8_t)re_v + dy * get_image_pixel(p_img, col, 1, output_w);
            set_image_pixel(pOut, col, row, re_v, output_w);
        }   
        pre_iy = iy;        
    }
    
    row = output_h - 1;
    sy = row * h_scale;
    //iy = (int)sy;
    iy = (int)floor((double)sy);
    dy = sy - iy;   
    if(iy != pre_iy){
        for(col = 0; col < output_w - 1; ++col)
        {
            float re_v = 0;
            float sx = col * w_scale;
            int ix = (int)sx;
            float dx = sx - ix;     
            re_v = (1 - dx) * get_image_pixel(pIn, ix, iy, input_w) + dx * get_image_pixel(pIn, ix + 1, iy, input_w);
            set_image_pixel(p_img, col, 0, re_v, output_w);
        }
        re_v = get_image_pixel(pIn, input_w - 1, iy, input_w);
        set_image_pixel(p_img, col, 0, re_v, output_w);          
    }
    for (col = 0; col < output_w; ++col){
        ////let the latest row be exactly the same with lastest tmp buffer
        re_v = get_image_pixel(p_img, col, 0, output_w);
        set_image_pixel(pOut, col, output_h - 1, re_v, output_w);
    }   
}
#endif