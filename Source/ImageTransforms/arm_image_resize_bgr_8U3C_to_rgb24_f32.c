/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_image_resize_bgr_8U3C_to_rgb24_f32
 * Description:  image resize of planar BGR8U3C to RGB24
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
#include "cv/image_transforms.h"
#include "arm_image_resize_common.h"

#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
inline uint32x4_t get_pixel_index(int32x4_t x_s32, int32x4_t y_s32, float32_t w, float c_offset)
{
  uint32x4_t pixel_u32;
  float32x4_t x_f32, y_f32 , pixel_f32;

  x_f32 = vcvtq_f32_s32(x_s32);
  y_f32 = vcvtq_f32_s32(y_s32);
  y_f32 = vmulq_n_f32(y_f32, w);
  pixel_f32 = vaddq_f32(x_f32, y_f32);
  pixel_f32 = vaddq_n_f32(pixel_f32, c_offset);
  pixel_u32 = vcvtq_u32_f32(pixel_f32);
  return pixel_u32;
}

#else
static inline float get_bgr_8U3C_image_pixel(uint8_t* m, int x, int y, int c, int w, int h)
{
  return m[c*h*w + y * w + x];
}
static inline void set_tmp_image_pixel(uint8_t* m, int x, int y, int c, float val, int w)
{
  /* x, y, c are already validated by upper layers */
  m[c*2*w + y * w + x] = (uint8_t)(val+0.5f);
}

static inline void set_rgb24_image_pixel(uint8_t* m, int x, int y, int c, float val, int w)
{
  /* x, y, c are already validated by upper layers */
  m[(y * w + x) * 3 + (2 - c) ] = (uint8_t)(val+0.5f);
}
#endif
/**
  @ingroup imageTransform
 */


/**     
 * @brief      Resize BGR 8U3C image and convert to RGB24 (float implementation)
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
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
void arm_image_resize_bgr_8U3C_to_rgb24_f32(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                         arm_cv_image_rgb24_t* ImageOut,
                                         uint8_t *p_img)
{

    const int input_w = ImageIn->width;
    const int input_h = ImageIn->height;

    const int output_w = ImageOut->width;
    const int output_h = ImageOut->height;

	  float w_scale = (float)( input_w - 1) / ( output_w - 1);
	  float h_scale = (float)( input_h - 1) / ( output_h - 1);	

    channel_uint8_t *pIn  =  ImageIn->pData;
    channel_uint8_t *pOut = ImageOut->pData;
    int channels = 3;

    int32_t r,c,k,d;

    float32x4_t sx_f32;
    int32x4_t ix_s32, ix2_s32, iy_s32, temp_s32;
    float32x4_t ix_f32;
    float32x4_t dx_f32, dy_f32;

    uint32x4_t index_u32, temp_u32, total_x_u32, temp_y_u32;
    float32x4_t index_f32;
    uint32x4_t pixel_u32;
    float32x4_t val_f32, weight_f32, pixel_f32, temp_f32, total_f32;
    uint32_t part_loc;
    // uint8_t *p_img = calloc(2 *  output_w, sizeof(uint8_t));
    uint32_t temp_u;
    uint64_t a = 0x0000000300000000; // = {0x0, 0x3}
    uint64_t b = 0x0000000900000006; // = {0x6, 0x9}  
    uint32x4_t str_offset = vcreateq_u32(a, b); //= {0, 3, 6, 9};  //TODO
    for (k = 0; k < channels; ++k) {
      // float val = 0;
      float32_t sy;
      int32_t iy = 0, pre_iy = -1, _iy;
      float32_t dy;	
      float32_t inimg_c_offset = k* input_w* input_h;
      for (r = 0; r <  output_h - 1 ; ++r) {
        sy = r *  h_scale;
        iy = (int)sy;
        dy = sy - iy;	
        if(iy != pre_iy){
          for(d = 0; d < 2; ++d){
            for(c = 0; c <  output_w - 4; c += 4)
            {	
              index_u32 = vidupq_n_u32(0, 1);		
              temp_u = (uint32_t)c;
              index_u32 = vaddq_n_u32(index_u32, temp_u);
              index_f32 = vcvtq_f32_u32(index_u32);	

              //sx = c * w_scaled
              sx_f32 = vmulq_n_f32(index_f32,  w_scale);
              ix_f32 = vrndmq_f32(sx_f32);
              //dx = sx - ix
              dx_f32 = vsubq_f32(sx_f32, ix_f32);
              ix_s32 = vcvtq_s32_f32(ix_f32);
              _iy = iy+d;
              iy_s32 = vdupq_n_s32(_iy);

              //val = dx * pixel'
              ix2_s32 = vaddq_n_s32(ix_s32, 1);
              pixel_u32 = get_pixel_index(ix2_s32, iy_s32, (float32_t) input_w, inimg_c_offset);
              temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
              val_f32 = vcvtq_f32_u32(temp_u32);
              total_f32 = vmulq_f32(val_f32, dx_f32);

              //val += (1+dx) * pixel'
              temp_f32 = vdupq_n_f32(1.0f);
              weight_f32 = vsubq_f32(temp_f32, dx_f32);
              pixel_u32 = get_pixel_index(ix_s32, iy_s32, (float32_t) input_w, inimg_c_offset);		
              temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
              pixel_f32 = vcvtq_f32_u32(temp_u32);
              val_f32 = vmulq_f32(pixel_f32, weight_f32);	
              total_f32 = vaddq_f32(total_f32, val_f32);

              total_f32 = vaddq_n_f32(total_f32, 0.5f);  
              part_loc =  output_w * d + c;
              total_x_u32 = vcvtq_u32_f32(total_f32);
              vstrbq_u32(&(p_img[part_loc]), total_x_u32);
            }

            index_u32 = vidupq_n_u32(0, 1);
            c =  output_w - 4;
            temp_s32 = vdupq_n_s32(c);

            temp_u32 = vreinterpretq_u32_s32(temp_s32);
            index_u32 = vaddq_u32(index_u32, temp_u32);					
            index_f32 = vcvtq_f32_u32(index_u32);	

            //sx = c * w_scaled
            sx_f32 = vmulq_n_f32(index_f32,  w_scale);
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
            pixel_u32 = get_pixel_index(ix2_s32, iy_s32, (float32_t) input_w, inimg_c_offset);
            temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
            val_f32 = vcvtq_f32_u32(temp_u32);
            total_f32 = vmulq_f32(val_f32, dx_f32);

            //val += (1+dx) * pixel'
            ix_s32 = vsetq_lane_s32( input_w-1, ix_s32, 3); 
            temp_f32 = vdupq_n_f32(1.0f);
            weight_f32 = vsubq_f32(temp_f32, dx_f32);
            weight_f32 = vsetq_lane_f32((float32_t)1.0, weight_f32, 3);
            pixel_u32 = get_pixel_index(ix_s32, iy_s32, (float32_t) input_w, inimg_c_offset);
            temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
            pixel_f32 = vcvtq_f32_u32(temp_u32);
            val_f32 = vmulq_f32(pixel_f32, weight_f32);	
            total_f32 = vaddq_f32(total_f32, val_f32);														
            part_loc =  output_w * d + c;
            // total_f32 = vrndnq_f32(total_f32);
            total_f32 = vaddq_n_f32(total_f32, 0.5f);  
            total_x_u32 = vcvtq_u32_f32(total_f32);
            vstrbq_u32(&(p_img[part_loc]), total_x_u32);					
          }
        }

        for (c = 0; c <  output_w; c += 4){
          temp_s32 = vdupq_n_s32(c);
          index_u32 = vidupq_n_u32(0, 1);	

          temp_u = (uint32_t)c;
          index_u32 = vaddq_n_u32(index_u32, temp_u);

          /*pixel loc= p_img[1][c, ..., c+3]*/
          temp_u =  output_w;	
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
          
          total_f32 = vaddq_n_f32(total_f32, 0.5f); 
          temp_y_u32 = vcvtq_u32_f32(total_f32);

          //[[bbb],[ggg],[rrr]]--> [[rgb],[rgb],[rgb]]
          uint32_t loc = ( output_w*r + c)*3 + (2-k);

          //(uint8_t * base, uint8x16_t offset, uint8x16_t value)
          vstrbq_scatter_offset_u32(&(pOut[loc]), str_offset, temp_y_u32);
        }
        
        pre_iy = iy;	
      }

      r =  output_h - 1;
      sy = r *  h_scale;
      iy = (int)sy;
      dy = sy - iy;	
      if(iy != pre_iy){
        for(c = 0; c <  output_w-4 ; c += 4)
        {	
          index_u32 = vidupq_n_u32(0, 1);

          temp_u = (uint32_t)c;
          index_u32 = vaddq_n_u32(index_u32, temp_u);
          index_f32 = vcvtq_f32_u32(index_u32);	

          //sx = c * w_scaled
          sx_f32 = vmulq_n_f32(index_f32,  w_scale);
          ix_f32 = vrndmq_f32(sx_f32);
          //dx = sx - ix
          dx_f32 = vsubq_f32(sx_f32, ix_f32);
          ix_s32 = vcvtq_s32_f32(ix_f32);
          iy_s32 = vdupq_n_s32(iy);

          //val = dx * pixel'
          ix2_s32 = vaddq_n_s32(ix_s32, 1);
          pixel_u32 = get_pixel_index(ix2_s32, iy_s32,  input_w, inimg_c_offset);
          temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
          val_f32 = vcvtq_f32_u32(temp_u32);
          total_f32 = vmulq_f32(val_f32, dx_f32);

          //val += (1+dx) * pixel'
          temp_f32 = vdupq_n_f32(1.0f);
          weight_f32 = vsubq_f32(temp_f32, dx_f32);
          pixel_u32 = get_pixel_index(ix_s32, iy_s32,  input_w, inimg_c_offset);	
          temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
          pixel_f32 = vcvtq_f32_u32(temp_u32);
          val_f32 = vmulq_f32(pixel_f32, weight_f32);	
          total_f32 = vaddq_f32(total_f32, val_f32);						
          
          // val_f32 = vaddq_n_f32(val_f32, 0.5f);  //need use vrndq/vrndnq/vrndmq, not use (float)(val+0.5f)
          part_loc = c;
          // total_f32 = vrndnq_f32(total_f32);
          total_f32 = vaddq_n_f32(total_f32, 0.5f);  
          total_x_u32 = vcvtq_u32_f32(total_f32);
          vstrbq_u32(&(p_img[part_loc]), total_x_u32);	
        }
    
        c =  output_w - 4;
        index_u32 = vidupq_n_u32(0, 1);
        temp_s32 = vdupq_n_s32(c);

        temp_u32 = vreinterpretq_u32_s32(temp_s32);
        index_u32 = vaddq_u32(index_u32, temp_u32);					
        index_f32 = vcvtq_f32_u32(index_u32);	

        //sx = c * w_scaled
        sx_f32 = vmulq_n_f32(index_f32,  w_scale);
        ix_f32 = vrndmq_f32(sx_f32);
        //dx = sx - ix
        dx_f32 = vsubq_f32(sx_f32, ix_f32);
        dx_f32 = vsetq_lane_f32((float32_t)0.0, dx_f32, 3);
        ix_s32 = vcvtq_s32_f32(ix_f32);
        iy_s32 = vdupq_n_s32(iy);

        //val = dx * pixel'
        // loc =  input_w * iy +  input_w - 1;
        ix2_s32 = vaddq_n_s32(ix_s32, 1);
        // ix2_s32 = vsetq_lane_s32(0, ix_s32, 3);
        pixel_u32 = get_pixel_index(ix2_s32, iy_s32,  input_w, inimg_c_offset);
        temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
        val_f32 = vcvtq_f32_u32(temp_u32);
        total_f32 = vmulq_f32(val_f32, dx_f32);

        //val += (1+dx) * pixel'
        ix_s32 = vsetq_lane_s32( input_w-1, ix_s32, 3); 
        temp_f32 = vdupq_n_f32(1.0f);
        weight_f32 = vsubq_f32(temp_f32, dx_f32);
        weight_f32 = vsetq_lane_f32((float32_t)1.0, weight_f32, 3);
        pixel_u32 = get_pixel_index(ix_s32, iy_s32,  input_w, inimg_c_offset);
        temp_u32 = vldrbq_gather_offset_u32(&(pIn[0]), pixel_u32);
        pixel_f32 = vcvtq_f32_u32(temp_u32);
        val_f32 = vmulq_f32(pixel_f32, weight_f32);	
        total_f32 = vaddq_f32(total_f32, val_f32);														
        part_loc = c;
        // total_f32 = vrndnq_f32(total_f32);
        total_f32 = vaddq_n_f32(total_f32, 0.5f);  
        total_x_u32 = vcvtq_u32_f32(total_f32);
        vstrbq_u32(&(p_img[part_loc]), total_x_u32);		
    
      }	
      //to cal val in the last colum
      for (c = 0; c <  output_w; c += 4){
        temp_s32 = vdupq_n_s32(c);
        index_u32 = vidupq_n_u32(0, 1);

        temp_u32 = vreinterpretq_u32_s32(temp_s32);
        pixel_u32 = vaddq_u32(index_u32, temp_u32);


        ////let the latest row be exactly the same with lastest tmp buffer
              /*val += pixel' 
                pixel' loc= p_img[0][c,...,c+3]*/    
        temp_u32 = vldrbq_gather_offset(&(p_img[0]), pixel_u32);
        val_f32 = vcvtq_f32_u32(temp_u32); 
        
        
        // val_f32 = vrndnq_f32(val_f32);
        val_f32 = vaddq_n_f32(val_f32, 0.5f);  
        temp_y_u32 = vcvtq_u32_f32(val_f32);

        //[[bbb],[ggg],[rrr]]--> [[rgb],[rgb],[rgb]]
        uint32_t loc = ( output_w*r + c)*3 + (2-k);

        //(uint8_t * base, uint8x16_t offset, uint8x16_t value)
        vstrbq_scatter_offset_u32(&(pOut[loc]), str_offset, temp_y_u32);
      }		

    }	
}
#else
void arm_image_resize_bgr_8U3C_to_rgb24_f32(const arm_cv_image_bgr_8U3C_t* ImageIn,
                                         arm_cv_image_rgb24_t* ImageOut,
                                         uint8_t *p_img)
{

  const int channels = 3;
  const int input_w = ImageIn->width;
  const int input_h = ImageIn->height;


  const int output_w = ImageOut->width;
  const int output_h = ImageOut->height;


  channel_uint8_t *pIn  = ImageIn->pData;

  channel_uint8_t *pOut = ImageOut->pData;

  float w_scale ;
  float h_scale ;	

  int ch_cnt, d, row,col;

  w_scale = (float)(input_w - 1) / (output_w - 1);
  h_scale = (float)(input_h - 1) / (output_h - 1);

	if(input_w == 1 || input_h == 1)
		return;

	for (ch_cnt = 0; ch_cnt < channels; ++ch_cnt) 
	{	
		float re_v = 0;
		float sy;
		int iy, pre_iy = -1;
		float dy;
		for (row = 0; row < output_h - 1 ; ++row) 
		{
			sy = row * h_scale;
			iy = (int)sy;
			dy = sy - iy;	

			if(iy != pre_iy){
				for(d = 0; d < 2; ++d)
				{
					for(col = 0; col < output_w - 1; ++col)
					{
						float re_v = 0;
						float sx = col * w_scale;
						int ix = (int)sx;
						float dx = sx - ix;		
						re_v = dx * get_bgr_8U3C_image_pixel(pIn, ix + 1, iy + d, ch_cnt, input_w, input_h) + (1 - dx) * get_bgr_8U3C_image_pixel(pIn, ix, iy + d, ch_cnt, input_w, input_h);
						set_tmp_image_pixel(p_img, col, d, 0, re_v, output_w);
					}
					re_v = get_bgr_8U3C_image_pixel(pIn, input_w - 1, iy + d, ch_cnt, input_w, input_h);
					set_tmp_image_pixel(p_img, col, d, 0, re_v, output_w);
				}				
			}
			
			for (col = 0; col < output_w; ++col)
			{
				re_v = (1 - dy) * get_bgr_8U3C_image_pixel(p_img, col, 0, 0, output_w, 2);
				re_v = (uint8_t)re_v + dy * get_bgr_8U3C_image_pixel(p_img, col, 1, 0, output_w, 2);
				set_rgb24_image_pixel(pOut, col, row, ch_cnt, re_v, output_w);
			}	
			pre_iy = iy;		
		}

		row = output_h - 1;
		sy = row * h_scale;
		iy = (int)floor((double)sy);
		dy = sy - iy;	
		if(iy != pre_iy)
		{
			for(col = 0; col < output_w - 1; ++col)
			{
				float re_v = 0;
				float sx = col * w_scale;
				int ix = (int)sx;
				float dx = sx - ix;		
				re_v = (1 - dx) * get_bgr_8U3C_image_pixel(pIn, ix, iy, ch_cnt, input_w, input_h) + dx * get_bgr_8U3C_image_pixel(pIn, ix + 1, iy, ch_cnt, input_w, input_h);
				set_tmp_image_pixel(p_img, col, 0, 0, re_v, output_w);
			}
			re_v = get_bgr_8U3C_image_pixel(pIn, input_w - 1, iy, ch_cnt, input_w, input_h);
			set_tmp_image_pixel(p_img, col, 0, 0, re_v, output_w);			
		}
		for (col = 0; col < output_w; ++col)
		{
      ////let the latest row be exactly the same with lastest tmp buffer
			re_v = (get_bgr_8U3C_image_pixel(p_img, col, 0, 0, output_w, 2));
			set_rgb24_image_pixel(pOut, col, output_h - 1, ch_cnt, re_v, output_w);
		}	
	}


}
#endif