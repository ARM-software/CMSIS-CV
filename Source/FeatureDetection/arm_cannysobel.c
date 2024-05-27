/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        cannysobel.c
 * Description:  Last steps of the canny edge detector (after the gaussian filter)
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
#include "cv/feature_detection.h"
#include "dsp/basic_math_functions.h"
#include "dsp/fast_math_functions.h"

#define Q15_ONE 0x7FFF
#define Q8_ONE 0xFF
//function performing canny edge on an image where a gaussian filter has been applied
//this function uses three buffers, one for storing intermediate values for computing the gradient, one for storing the gradient and one for storing the magnitude conputed with the gradient
//exept the buffer for the magnitude, the buffer have two component
//the purpose of this function is to avoid repetition of compute by storing the intermediate part of the compute and by fusing the end of canny edge and sobel
//to avoid repetition of condition for the end of the canny edge

/**
  @ingroup featureDetection
 */

/**
 * @brief      Canny edge with sobel integrated
 *
 * @param[in]     ImageIn         The input image
 * @param[out]    ImageOut        The output image
 * @param[in,out] Img_tmp_grad1   Temporary buffer gradient 1
 * @param[in,out] Img_tmp_mag     Temporary magnitude buffer
 * @param[in,out] Img_tmp_grad2   Temporary buffer gradient 2
 * @param[in]     low_threshold   The low threshold
 * @param[in]     high_threshold  The high threshold
 * 
 * @par  Temporary buffer sizing:
 * 
 * Size of temporary buffers:
 *   - Img_tmp_grad1\n
 *     3*w*sizeof(arm_cv_gradient_q15_t) where w is the input image width
 *   - Img_tmp_mag\n 
 *     3*w*sizeof(q15_t)
 *   - Img_tmp_grad2\n 
 *     3*w*sizeof(arm_cv_gradient_q15_t)
 */
#if ((!defined(ARM_MATH_MVEI)) ||(defined(FORCE_SCALAR)))
void arm_canny_edge_sobel_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                     arm_cv_image_gray8_t* ImageOut, 
                                     arm_cv_image_gradient_q15_t* Img_tmp_grad1, 
                                     arm_cv_image_q15_t* Img_tmp_mag, 
                                     arm_cv_image_gradient_q15_t* Img_tmp_grad2,
                                     int low_threshold,
                                     int high_threshold)
{
	
	int indice;	
	q63_t gradx;
	q63_t grady;
	int x = 0;
	//Shifting the value of threshold to have them in q15
	low_threshold = low_threshold <<5;
	high_threshold = high_threshold <<5;
	//ensure the buffers are empty
	for(int t = 0; t<Img_tmp_grad1->width*3; t++)
	{
		Img_tmp_grad1->pData[t].x=0;
		Img_tmp_mag->pData[t]=0;
		Img_tmp_grad2->pData[t].x=0;
	}
	//first initialisation of the temporary buffer, for the first line we cannot compute the component on y, so we only do the component on x 
	for(int y = 1; y<ImageIn -> width- 1; y++)
	{
		Img_tmp_grad2->pData[x*Img_tmp_grad2->width +y].x = (ImageIn->pData[x*ImageIn->width+(y-1)] + (ImageIn->pData[x*ImageIn->width+(y)]<<1) + ImageIn->pData[x*ImageIn->width+(y+1)])<<5;
	}
	//for the second line we compute both component of the temporary buffer
	x = 1;
	Img_tmp_grad2->pData[x*Img_tmp_grad2->width].y = (ImageIn->pData[(x-1)*ImageIn->width] + (ImageIn->pData[x*ImageIn->width]<<1) + ImageIn->pData[(x+1)*ImageIn->width])<<5;
	for(int y = 1; y<ImageIn -> width- 1; y++)
	{
		Img_tmp_grad2->pData[x*Img_tmp_grad2->width +y].y = (ImageIn->pData[(x-1)*ImageIn->width+y] + (ImageIn->pData[x*ImageIn->width+y]<<1) + ImageIn->pData[(x+1)*ImageIn->width+y])<<5;//possibility to >>2/3 to reduce buffer size to int8 //6value in the buffer aren't used the six on the dorder vertical to painful to adapt code for it
		Img_tmp_grad2->pData[x*Img_tmp_grad2->width +y].x = (ImageIn->pData[x*ImageIn->width+(y-1)] + (ImageIn->pData[x*ImageIn->width+(y)]<<1) + ImageIn->pData[x*ImageIn->width+(y+1)])<<5;
	}
	Img_tmp_grad2->pData[x*Img_tmp_grad2->width+Img_tmp_grad2->width-1].y = (ImageIn->pData[(x-1)*ImageIn->width+Img_tmp_grad2->width-1] + (ImageIn->pData[x*ImageIn->width+Img_tmp_grad2->width-1]<<1) + ImageIn->pData[(x+1)*ImageIn->width+Img_tmp_grad2->width-1])<<5;
	//third line, we compute a third line for the temporary buffer, so it is now full and we are now able to start the computation of the gradient buffer so we also compute the first line of the gradient buffer
	//also if we have the gradient, we can compute the magnitude too
	x=2;
	for( int y =0; y < ImageIn->width; y++)
	{
		//Computation of the edge case
		int xm = x%3;
		if((y==0||y == ImageIn->width-1)&& x != ImageIn->height-1)
		{
			Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].y = (ImageIn->pData[(x-1)*ImageIn->width+y] + (ImageIn->pData[x*ImageIn->width+y]<<1) + ImageIn->pData[(x+1)*ImageIn->width+y])<<5;
			indice = x*ImageIn->width + y;
			ImageOut->pData[indice] = 0;
			continue;
		}
		//Computation of the intermediate gradient buffer
		Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].y = (ImageIn->pData[(x-1)*ImageIn->width+y] + (ImageIn->pData[x*ImageIn->width+y]<<1) + ImageIn->pData[(x+1)*ImageIn->width+y])<<5;
		Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].x = (ImageIn->pData[x*ImageIn->width+(y-1)] + (ImageIn->pData[x*ImageIn->width+(y)]<<1) + ImageIn->pData[x*ImageIn->width+(y+1)])<<5;

		//computation of the gradient
		indice = (x-1)*ImageIn->width + y;

		gradx = Img_tmp_grad2->pData[((x-2)%3)*Img_tmp_grad2->width +y].x - Img_tmp_grad2->pData[(xm)*Img_tmp_grad2->width +y].x;
		grady = Img_tmp_grad2->pData[((x-1)%3)*Img_tmp_grad2->width +(y-1)].y - Img_tmp_grad2->pData[((x-1)%3)*Img_tmp_grad2->width +(y+1)].y;
		if(gradx==0&&grady==0)
		{
			ImageOut->pData[indice] = 0;
			continue;
		}
		//Computation of the magnitude
		q15_t vect[2] = { (q15_t)gradx, (q15_t)grady};
		q15_t out;

		q31_t in[2] = {((q31_t)vect[0]), ((q31_t)vect[1])};
		q31_t out2[2];
		q31_t out3;
		q31_t root;
		//multiplication of two q15 give a q31 in output
		out2[0]=(in[0]*in[0]);
		out2[1]=(in[1]*in[1]);
		//addition of two q1.30 give a q2.29 shift by one a q1.30
		out3 = (out2[0]+out2[1])>>1;
		//root q31 give in output a q31 shit by 15, back to a q15 because of the previous shift by one 
		arm_sqrt_q31(out3, &root);
		out = root>>15;
		Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].y = grady;
		Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].x = gradx;
		Img_tmp_mag->pData[(x-1)%3 * Img_tmp_grad2->width + y]	= out;	
	}
	//here we continue our process but it's the main loop, 
	//as the previous loop we will compute a line of the temporary buffer and the associated line in gradient buffer and magnitude buffer
	//so we have two line of the gradient and magnitude buffer
	//for the computing of an output pixel we would need three line of those buffers but we chose to put all the border to 0 in our implementation of sobel, so the value are know, if the buffers are empty when passed to function
	//Here it's the case so we can start the computing of the output image
	for( int x = 3; x < ImageIn->height; x++)
    {
		//first loop on the line to compute the three buffers
        for( int y =0; y < ImageIn->width; y++)
        {
			//computation of the temporary buffer
			int xm = x%3;
			//ensure the border are initialized at 0
			if((y==0||y == ImageIn->width-1)&& x != ImageIn->height-1)
			{
				Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].y = (ImageIn->pData[(x-1)*ImageIn->width+y] + (ImageIn->pData[x*ImageIn->width+y]<<1) + ImageIn->pData[(x+1)*ImageIn->width+y])<<5;
				indice = x*ImageIn->width + y;
				ImageOut->pData[indice] = 0;
				continue;
			}
			
			Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].y = (ImageIn->pData[(x-1)*ImageIn->width+y] + (ImageIn->pData[x*ImageIn->width+y]<<1) + ImageIn->pData[(x+1)*ImageIn->width+y])<<5;
			Img_tmp_grad2->pData[xm*Img_tmp_grad2->width +y].x = (ImageIn->pData[x*ImageIn->width+(y-1)] + (ImageIn->pData[x*ImageIn->width+(y)]<<1) + ImageIn->pData[x*ImageIn->width+(y+1)])<<5;
			//computation of the gradient buffer
			indice = (x-1)*ImageIn->width + y;
			gradx = Img_tmp_grad2->pData[((x-2)%3)*Img_tmp_grad2->width +y].x - Img_tmp_grad2->pData[(xm)*Img_tmp_grad2->width +y].x;
			grady = Img_tmp_grad2->pData[((x-1)%3)*Img_tmp_grad2->width +(y-1)].y - Img_tmp_grad2->pData[((x-1)%3)*Img_tmp_grad2->width +(y+1)].y;
			//computation of the magnitude buffer
			if(gradx==0&&grady==0)
			{
				ImageOut->pData[indice] = 0;
				Img_tmp_mag->pData[(x-1)%3 * Img_tmp_grad2->width + y]	= 0;
				Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].y = grady;
				Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].x = gradx;
				continue;
			}
			q15_t vect[2] = { (q15_t)gradx, (q15_t)grady};
			q15_t out;

      		q31_t in[2] = {((q31_t)vect[0]), ((q31_t)vect[1])};
      		q31_t out2[2];
      		q31_t out3;
      		q31_t root;
      		out2[0]=(in[0]*in[0]);
      		out2[1]=(in[1]*in[1]);
      		out3 = (out2[0]+out2[1])>>1;
      		arm_sqrt_q31(out3, &root);
      		out = root>>15;
			//setting all the data in the buffers, if not done properly, errors will occurs
			Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].y = grady;
			Img_tmp_grad1->pData[(x-1)%3 * Img_tmp_grad2->width + y].x = gradx;
			Img_tmp_mag->pData[(x-1)%3 * Img_tmp_grad2->width + y]	= out>>5<<5;	
		}
		//second loop on the line to compute the output data
		for( int y =1; y < ImageIn->width-1; y++)
		{
			int indice = (x-2)*ImageIn->width +y;
			int mag = Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y];
			//First thing to test is the magnitude, if it's under the lowthreshold, then there is no need to test further, the output value will be 0
			if(mag < low_threshold)
			{
				ImageOut->pData[indice] = 0;
				continue;
			}
			//we need to compare the magnitude with two other one, determine by the angle
			//if the magnitude is higher than those two, we check if the magnitude is higer the the high threshold 
			//if yes, the output value is 255
			//if no, we need to check in the 3x3 square of magnitude if ther is at least one that is higher than the value
			//if yes 255, else 0
			//This is the step where checking the angle is unavoidable and where we used the gradient buffer, in scalar because of the atan
			else
			{
				q15_t angle;
				arm_atan2_q15(Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].x, Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].y, &angle);
				arm_abs_q15( &angle, &angle, 1);
				//decision based on the angle
				if((angle ) < (0xC4A))//22 in rad in 2.13
				{
					if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+1])
					{
						ImageOut->pData[indice] = 0;
						continue;
					}
					else
					{
						if(mag < high_threshold)
						{
							if(Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
							else
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
						}
						else
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
					}
					continue;
				}
				else if((angle ) < (0x256C))//67 in rad in 2.13
				{
					
						if( mag <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1] || mag <= Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y+1])
						{
							ImageOut->pData[indice] = 0;
							continue;
						}
						else
						{
							if( mag < low_threshold)
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
							else if(mag < high_threshold)
							{
								if (Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
								{
									ImageOut->pData[indice] = Q8_ONE;
									continue;
								}
								else
								{
									ImageOut->pData[indice] = 0;
									continue;
								}
							}
							else
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
						}
						continue;
				}
				else if((angle ) < (0x3E8E))//112 in rad in 2.13
				{
					if(  0 <= Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y] - mag||  0 <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y] - mag)
					{
						ImageOut->pData[indice] = 0;
						continue;
					}
					else
					{
						if(mag < high_threshold)
						{
							if (Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
							else
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
						}
						else
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
					}
					continue;
				}
				else if((angle ) < (0x595C))//160 in rad in 2.13
				{
					
						if( mag <= Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y-1] || mag <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1])
						{
							ImageOut->pData[indice] = 0;
							continue;
						}
						else
						{
							if(mag < high_threshold)
							{
								if (Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
								{
									ImageOut->pData[indice] = Q8_ONE;
									continue;
								}
								else
								{
									ImageOut->pData[indice] = 0;
									continue;
								}
							}
							else
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
						}
						continue;
				}
				else//case tan between 135+22.5 and 180
				{
					if( mag <= Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1])
					{
						ImageOut->pData[indice] = 0;
						continue;
					}
					else
					{
						if(mag < high_threshold)
						{
							if(Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-3)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
							else
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
						}
						else
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
					}
					continue;
				}
			}
		}
	}
	//last line of the outup image to be computed the conditions are simpler because we know that the last line of the image is 0, such as the gradient and magnitude for this line
	x = ImageIn->height;
	for( int y =1; y < ImageIn->width-1; y++)
	{
		
		int indice = (x-2)*ImageIn->width +y;
		int mag = Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y];
		//First thing to test is the magnitude, if it's under the lowthreshold, then there is no need to test further, out will be 0
		if(mag < low_threshold)
		{
			ImageOut->pData[indice] = 0;
			continue;
		}
		//we need to compare the magnitude with two other one, determine by the angle
		//if the magnitude is higher than those two, we check if the magnitude is higer the the high threshold 
		//if yes, the output value is 255
		//if no, we need to check in the 3x3 square of magnitude if ther is at least one that is higher than the value
		//if yes 255, else 0
		//This is the step where checking the angle is unavoidable and where we used the gradient buffer, in scalar because of the atan
		else
		{
			q15_t angle;
			arm_atan2_q15(Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].x, Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].y, &angle);
			if(Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].x ==0&& Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].y==0)
			{
				angle = 0;
			}
			arm_abs_q15( &angle, &angle, 1);
			if((angle ) < (0xC4A))//22 in rad in 2.13
			{
				if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+1])
				{
					ImageOut->pData[indice] = 0;
					continue;
				}
				else
				{
					if(mag < high_threshold)
					{
						if(Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
						else
						{
							ImageOut->pData[indice] = 0;
							continue;
						}
					}
					else
					{
						ImageOut->pData[indice] = Q8_ONE;
						continue;
					}
				}
				continue;
			}
			else if((angle ) < (0x256C))//67 in rad in 2.13
			{
					if( mag <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1] )
					{
						ImageOut->pData[indice] = 0;
						continue;
					}
					else
					{
						if(mag < high_threshold)
						{
							if (Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
							else
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
						}
						else
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
					}
					continue;
				//}	
			}
			else if((angle ) < (0x3E8E))//112 in rad in 2.13
			{
				if(  0 <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y] - mag)
				{
					ImageOut->pData[indice] = 0;
					continue;
				}
				else
				{
					if(mag < high_threshold)
					{
						if (Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
						else
						{
							ImageOut->pData[indice] = 0;
							continue;
						}
					}
					else
					{
						ImageOut->pData[indice] = Q8_ONE;
						continue;
					}
				}
				continue;
			}
			else if((angle ) < (0x595C))//160 in rad in 2.13
			{
				
					if( mag <= Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1])
					{
						ImageOut->pData[indice] = 0;
						continue;
					}
					else
					{
						if(mag < high_threshold)
						{
							if (Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
							{
								ImageOut->pData[indice] = Q8_ONE;
								continue;
							}
							else
							{
								ImageOut->pData[indice] = 0;
								continue;
							}
						}
						else
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
					}
					continue;
			}
			else//case tan between 135+22.5 and 180
			{
				if( mag <= Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y+1])
				{
					ImageOut->pData[indice] = 0;
					continue;
				}
				else
				{
					if(mag < high_threshold)
					{
						if(Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[((x-1)%3) * (ImageIn->width)+y-1]>=(int)(high_threshold))
						{
							ImageOut->pData[indice] = Q8_ONE;
							continue;
						}
						else
						{
							ImageOut->pData[indice] = 0;
							continue;
						}
					}
					else
					{
						ImageOut->pData[indice] = Q8_ONE;
						continue;
					}
				}
				continue;
			}
		}
	}
}

#else

void arm_canny_edge_sobel_fixp(const arm_cv_image_gray8_t* ImageIn, 
                                     arm_cv_image_gray8_t* ImageOut, 
                                     arm_cv_image_gradient_q15_t* Img_tmp_grad1, 
                                     arm_cv_image_q15_t* Img_tmp_mag, 
                                     arm_cv_image_gradient_q15_t* Img_tmp_grad2,
                                     int low_threshold,
                                     int high_threshold)
{
	q15x8_t vect_mag;
    int x = 0;
	int w = ImageIn->width;
	
	//Initialize all the three buffer to 0
	//maybe useless maybe not, security measure
	for(int t = 0; t<320*3; t++)
	{
		Img_tmp_grad1->pData[t].x=0;
		Img_tmp_mag->pData[t]=0;
		Img_tmp_grad2->pData[t].x=0;
	}
	//Computation of the first line of the temporary buffer contaning the partial sum for the gradient computation
	//because we are on the first line we only do the horizontal one and not the vertical one
	for(int y = 1; y<((ImageIn -> width)>>4)+1; y++)
	{
		int indice = ((y-1)<<4) + 1;

		q15x8x2_t vect_2x2;
		q15x8x2_t vect_1x2;
		q15x8x2_t vect_3x2;
		q15x8x4_t vect_1x4;

		uint8x16_t vect_1 = vld1q(&ImageIn->pData[indice-1]);
		vect_1x2.val[0] = vshllbq(vect_1,5);
		
		uint8x16_t vect_3 = vld1q(&ImageIn->pData[indice+1]);
		vect_3x2.val[0] = vshllbq(vect_3,5);
		vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
		vect_1x2.val[1] = vshlltq(vect_1,5);
		
		uint8x16_t vect_2 = vld1q(&ImageIn->pData[indice]);
		vect_3x2.val[1] = vshlltq(vect_3,5);
		vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
		vect_2x2.val[0] = vshllbq(vect_2,6);
		vect_2x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
		vect_2x2.val[1] = vshlltq(vect_2,6);
		vect_2x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

		q15x8_t vect_void = vdupq_n_s16(0);
		vect_1x4.val[0] = vect_2x2.val[0];
		vect_1x4.val[1] = vect_void;
		vect_1x4.val[2] = vect_2x2.val[1];
		vect_1x4.val[3] = vect_void;
		vst4q((&Img_tmp_grad1->pData[indice].x), vect_1x4);
	}
	//tail of the first line for partial sum for the gradient
	int numtail = (w-1)%8;
	if(numtail>0)
	{
		for(int j=0; j<numtail+1; j++)
		{
			int y = ((ImageIn -> width)-numtail-1);
			Img_tmp_grad1->pData[y+j].x = (ImageIn->pData[(y-1)+j] + (ImageIn->pData[(y)+j]<<1) + ImageIn->pData[(y+1)+j])<<5;
			Img_tmp_grad1->pData[y+j].y = 0;
		}
	}
	//Computation of the second line of the partial sum for the gradient computation
	//this time we can do the two part vetical and horizontal
	x =1;
	Img_tmp_grad2->pData[x*w].x =0;
	Img_tmp_grad2->pData[x*w].y = (ImageIn->pData[(x-1)*w] + (ImageIn->pData[x*w]<<1) + ImageIn->pData[(x+1)*w])>>2;
	for(int y =1; y<((w)>>4); y++)
	{
		int indice = w +((y-1)<<4)+1;
		q15x8x2_t vect_2x2;
		q15x8x2_t vect_1x2;
		q15x8x2_t vect_3x2;
		q15x8x4_t vect_1x4;
		q15x8x2_t vect_lowx2;
		q15x8x2_t vect_highx2;

		uint8x16_t vect_1 = vld1q(&ImageIn->pData[indice-1]);
		vect_1x2.val[0] = vshllbq(vect_1,5);
		
		uint8x16_t vect_3 = vld1q(&ImageIn->pData[indice+1]);
		vect_3x2.val[0] = vshllbq(vect_3,5);
		vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
		vect_1x2.val[1] = vshlltq(vect_1,5);
		
		uint8x16_t vect_2 = vld1q(&ImageIn->pData[indice]);
		vect_3x2.val[1] = vshlltq(vect_3,5);
		vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
		vect_2x2.val[0] = vshllbq(vect_2,6);
		vect_1x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
		vect_2x2.val[1] = vshlltq(vect_2,6);
		vect_1x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

		uint8x16_t vecth = vld1q(&ImageIn->pData[indice-w]);
		vect_highx2.val[0] = vshllbq(vecth,5);
		vect_highx2.val[1] = vshlltq(vecth,5);
		uint8x16_t vectl = vld1q(&ImageIn->pData[indice+w]);
		vect_lowx2.val[0] = vshllbq(vectl,5);
		vect_lowx2.val[1] = vshlltq(vectl,5);

		vect_2x2.val[0] = vaddq(vect_lowx2.val[0], vect_2x2.val[0]);
		vect_2x2.val[0] = vaddq(vect_highx2.val[0], vect_2x2.val[0]);
		vect_2x2.val[1] = vaddq(vect_lowx2.val[1], vect_2x2.val[1]);
		vect_2x2.val[1] = vaddq(vect_highx2.val[1], vect_2x2.val[1]);

		vect_1x4.val[0] = vect_1x2.val[0];
		vect_1x4.val[1] = vect_2x2.val[0];
		vect_1x4.val[2] = vect_1x2.val[1];
		vect_1x4.val[3] = vect_2x2.val[1];
		vst4q((&Img_tmp_grad1->pData[indice].x), vect_1x4);
	}
	//Tail of the second line of the partial sum for the gradient computation
	if(numtail>0)
	{
		for(int j=0; j<numtail+1; j++)
		{
			int x=1;
			int y = ((w)-numtail-1);
			Img_tmp_grad1->pData[x*Img_tmp_grad1->width +y+j].y = (ImageIn->pData[(x-1)*w+y+j] + (ImageIn->pData[x*w+y+j]<<1) + ImageIn->pData[(x+1)*w+y+j])<<5;//possibility to >>2/3 to reduce buffer size to int8 //6value in the buffer aren't used the six on the dorder vertical to painful to adapt code for it
			Img_tmp_grad1->pData[x*Img_tmp_grad1->width +y+j].x = (ImageIn->pData[x*w+(y-1)+j] + (ImageIn->pData[x*w+(y)+j]<<1) + ImageIn->pData[x*w+(y+1)+j])<<5;
		}
	}
	Img_tmp_grad2->pData[w+w-1].x =0;
	Img_tmp_grad2->pData[w+w-1].y = (ImageIn->pData[(x-1)*w+w-1] + (ImageIn->pData[x*w+w-1]<<1) + ImageIn->pData[(x+1)*w+w-1])<<5;
	//Last step of the initialisation, we compute thee thrid line of the partial sum and a first line of the gradient value but also the magnitude value
	for(int x =2; x< 3; x++)
	{
		int x3 = x%3;
		//Computation of the partial sum and the gradient at the same time, the gradient have a different offset becase it requieres to have three line of memory for the partial sums
		Img_tmp_grad2->pData[x3*w].y = (ImageIn->pData[(x-1)*w] + (ImageIn->pData[x*w]<<1) + ImageIn->pData[(x+1)*w])<<5;
		for(int y =1; y< w-15;y+=16)
		{
			int indice = x*w + y;
			int indice3 = x3*w + y;

			q15x8x2_t vect_2x2;
			q15x8x2_t vect_1x2;
			q15x8x2_t vect_3x2;
			q15x8x4_t vect_1x4;
			q15x8x2_t vect_lowx2;
			q15x8x2_t vect_highx2;

			uint8x16_t vect_1 = vld1q(&ImageIn->pData[indice-1]);
			vect_1x2.val[0] = vshllbq(vect_1,5);

			uint8x16_t vect_3 = vld1q(&ImageIn->pData[indice+1]);
			vect_3x2.val[0] = vshllbq(vect_3,5);
			vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
			vect_1x2.val[1] = vshlltq(vect_1,5);

			uint8x16_t vect_2 = vld1q(&ImageIn->pData[indice]);
			vect_3x2.val[1] = vshlltq(vect_3,5);
			vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
			vect_2x2.val[0] = vshllbq(vect_2,6);
			vect_1x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
			vect_2x2.val[1] = vshlltq(vect_2,6);
			vect_1x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

			uint8x16_t vecth = vld1q(&ImageIn->pData[indice-w]);
			vect_highx2.val[0] = vshllbq(vecth,5);
			vect_highx2.val[1] = vshlltq(vecth,5);
			uint8x16_t vectl = vld1q(&ImageIn->pData[indice+w]);
			vect_lowx2.val[0] = vshllbq(vectl,5);
			vect_lowx2.val[1] = vshlltq(vectl,5);

			vect_2x2.val[0] = vaddq(vect_lowx2.val[0], vect_2x2.val[0]);
			vect_2x2.val[0] = vaddq(vect_highx2.val[0], vect_2x2.val[0]);
			vect_2x2.val[1] = vaddq(vect_lowx2.val[1], vect_2x2.val[1]);
			vect_2x2.val[1] = vaddq(vect_highx2.val[1], vect_2x2.val[1]);

			vect_1x4.val[0] = vect_1x2.val[0];
			vect_1x4.val[1] = vect_2x2.val[0];
			vect_1x4.val[2] = vect_1x2.val[1];
			vect_1x4.val[3] = vect_2x2.val[1];
			vst4q((&Img_tmp_grad1->pData[indice3].x), vect_1x4);
			for(int p = 0; p<2; p++)
			{
				y += p*8;
				indice += p*8;
				indice3 += p*8;
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_grad1->pData[indice3].x);
=======
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_temporary->pData[indice3].x);
=======
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_grad1->pData[indice3].x);
>>>>>>> 80d0433 (bug fix for cannyedge vect):Source/cannysobel.c

				q15x8x2_t vect_grad_1;
				q15x8_t vectgradx = vsubq_s16(vec_x_y_1.val[0], vect_buff_di.val[0]);
				q15x8x2_t vec_y_2 = vld2q_s16(&Img_tmp_grad2->pData[((x-1)%3)*w + y+1].x);
				int numVect = 8;
				int16x8_t vect_res_1;
				q31x4_t vect_gradx_1 = vmullbq_int_s16(vectgradx, vectgradx);
				q15x8_t vectgrady = vsubq_s16(vec_y_1.val[1], vec_y_2.val[1]);
				q31x4_t vect_grady_1 = vmullbq_int_s16(vectgrady, vectgrady);

				vect_gradx_1 = vaddq_s32(vect_gradx_1, vect_grady_1);
				q31x4_t vect_gradx_2 = vmulltq_int_s16(vectgradx, vectgradx);
				vect_gradx_1 = vshrq(vect_gradx_1, 1);
				q31x4_t vect_grady_2 = vmulltq_int_s16(vectgrady, vectgrady);

				vect_gradx_2 = vaddq_s32(vect_gradx_2, vect_grady_2);
				vect_gradx_2 = vshrq(vect_gradx_2, 1);
				//

				vect_grad_1.val[0] = vectgradx;
				vect_grad_1.val[1] = vectgrady;
            	vst2q(&Img_tmp_grad1->pData[(x-1)%3*w+y].x, vect_grad_1);
				//Computation of the magnitude, due to the square root, we can't keep using vector
            	//split into two loops because we tryed to keep the vector as long as possible so we only have the square root in scalar but 
				//the vector have been split in two with a desinterlive
				for(int j =0; j < numVect; j+=2)
				{
					if(vectgradx[j]==0&&vectgrady[j]==0)
					{
						vect_res_1[j] =  0;
						continue;
					}
					//mag
					q15_t out;
					q31_t root ;
					arm_sqrt_q31(vect_gradx_1[j>>1], &root);
      				out = root>>15;
					vect_res_1[j] = out;
				}
				for(int j =1; j < numVect; j+=2)
				{
					if(vectgradx[j]==0&&vectgrady[j]==0)
					{
						vect_res_1[j] =  0;
						continue;
					}
					//mag
					q15_t out;
					q31_t root ;
					arm_sqrt_q31(vect_gradx_2[j>>1], &root);
      				out = root>>15;
					vect_res_1[j] = out;
				}
				vst1q_s16((int16_t*)&Img_tmp_mag->pData[((x-1)%3)*w + y], vect_res_1);
				y -= p*8;
			}
		}
		//Tail of the third line of partial sum and the first gradient and magnitude May be doable in vector but for now all is in scalar
		for(int j = 0; j<numtail+1; j++)
		{
			int y = ((w-1)&0xFFF0) + j;
			if((y==0||y == w-1)&&x!=0&& x != ImageIn->height-1)
			{
				Img_tmp_grad1->pData[(x-1)%3*w +y].y = (Img_tmp_grad2->pData[(x-1)*w+y].y + (Img_tmp_grad2->pData[x*w+y].y<<1) + Img_tmp_grad2->pData[(x+1)*w+y].y)<<5;
				int indice = x*w + y;
				Img_tmp_mag->pData[indice-w] = 0;
				continue;
			}
			if( x==0 || y==0||y == w-1)
			{
				int indice = (x%3)*w + y;
				Img_tmp_mag->pData[indice-w] = 0;
				Img_tmp_grad1->pData[indice-w].x = 0;
				Img_tmp_grad1->pData[indice-w].y = 0;
				continue;
			}
			Img_tmp_grad2->pData[(x)%3*w +y].y = ((ImageIn->pData[(x-1)*w+y]) + (ImageIn->pData[x*w+y]<<1) + (ImageIn->pData[(x+1)*w+y]))<<5;
			Img_tmp_grad2->pData[(x)%3*w +y].x = ((ImageIn->pData[x*w+(y-1)]) + (ImageIn->pData[x*w+(y)]<<1) + (ImageIn->pData[x*w+(y+1)]))<<5;
			if(x==1)
			{
				continue;
			}
			int indice = (x-1)*w + y;
			q63_t gradx = Img_tmp_grad2->pData[((x-2)%3)*w +y].x - Img_tmp_grad2->pData[(x3)*w +y].x;
			q63_t grady = Img_tmp_grad2->pData[(x-1)%3*w +(y-1)].y - Img_tmp_grad2->pData[(x-1)%3*w +(y+1)].y;
			Img_tmp_grad1->pData[(x-1)%3*w+y].x = gradx;
			Img_tmp_grad1->pData[(x-1)%3*w+y].y = grady;
			if(gradx==0&&grady==0)
			{
				Img_tmp_mag->pData[indice-w] = 0;
				continue;
			}
			//mag
			q15_t vect[2] = { (q15_t)gradx, (q15_t)grady};

			q31_t in[2] = {((q31_t)vect[0]), ((q31_t)vect[1])};
			q31_t out2[2];
			q31_t out3;
			q31_t root;
			out2[0]=(in[0]*in[0]);
			out2[1]=(in[1]*in[1]);
			out3 = (out2[0]+out2[1])>>1;
			arm_sqrt_q31(out3, &root);
			Img_tmp_mag->pData[((x-1)%3)*w + y] = root>>15;
		}
		Img_tmp_grad2->pData[(x)%3*w+w-1].y = (ImageIn->pData[(x-1)*w+w-1] + (ImageIn->pData[x*w+w-1]<<1) + ImageIn->pData[(x+1)*w+w-1])<<5;
	}  
	//Main Loop of the process on all the possible lines
	//This loop do, the update of the oldest line of the partial sum buffer, the computation of a line of gradient and magnitude(I think ther's an issue and I would have to do an other turn of the previous loop because i lack one line Maybe not because the border are forced to be at  I think not)
	//and a computation of a line of the output image using the manitude and the gradient store
	//this last computation used the line of magnitude and in some case, a square 3x3 center around the indice of the considered value in the magnitude buffer(so it will check the magnitude in a 3x3 square)
	for(int x = 3; x< ImageIn->height; x++)
	{
		int x3 = x%3;
		//First the computation of the partial sum, magnitude and gradient
		//I can't put the output wit it because it need the value of the magnitude in a 3x3x square meaning in the case of the last value of a vector willl need this process
		//the value in the bottom right will not have been calculated yet => need to split the loop in two process
		Img_tmp_grad2->pData[x3*w].y = (ImageIn->pData[(x-1)*w] + (ImageIn->pData[x*w]<<1) + ImageIn->pData[(x+1)*w])<<5;
		for(int y =1; y< w-15;y+=16)
		{
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
			int indice = x*w + y;
			int indice3 = x3*w + y;
			
=======
>>>>>>> 0eb2bec (update type for gaussian and cannyedge):Source/cannysobel.c
=======
			int indice = x*w + y;
			int indice3 = x3*w + y;
			
>>>>>>> 80d0433 (bug fix for cannyedge vect):Source/cannysobel.c
			q15x8x2_t vect_2x2;
			q15x8x2_t vect_1x2;
			q15x8x2_t vect_3x2;
			q15x8x4_t vect_1x4;
			q15x8x2_t vect_lowx2;
			q15x8x2_t vect_highx2;

			uint8x16_t vect_1 = vld1q(&ImageIn->pData[indice-1]);
			vect_1x2.val[0] = vshllbq(vect_1,5);

			uint8x16_t vect_3 = vld1q(&ImageIn->pData[indice+1]);
			vect_3x2.val[0] = vshllbq(vect_3,5);
			vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
			vect_1x2.val[1] = vshlltq(vect_1,5);

			uint8x16_t vect_2 = vld1q(&ImageIn->pData[indice]);
			vect_3x2.val[1] = vshlltq(vect_3,5);
			vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
			vect_2x2.val[0] = vshllbq(vect_2,6);
			vect_1x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
			vect_2x2.val[1] = vshlltq(vect_2,6);
			vect_1x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

			uint8x16_t vecth = vld1q(&ImageIn->pData[indice-w]);
			vect_highx2.val[0] = vshllbq(vecth,5);
			vect_highx2.val[1] = vshlltq(vecth,5);
			uint8x16_t vectl = vld1q(&ImageIn->pData[indice+w]);
			vect_lowx2.val[0] = vshllbq(vectl,5);
			vect_lowx2.val[1] = vshlltq(vectl,5);

			vect_2x2.val[0] = vaddq(vect_lowx2.val[0], vect_2x2.val[0]);
			vect_2x2.val[0] = vaddq(vect_highx2.val[0], vect_2x2.val[0]);
			vect_2x2.val[1] = vaddq(vect_lowx2.val[1], vect_2x2.val[1]);
			vect_2x2.val[1] = vaddq(vect_highx2.val[1], vect_2x2.val[1]);

			vect_1x4.val[0] = vect_1x2.val[0];
			vect_1x4.val[1] = vect_2x2.val[0];
			vect_1x4.val[2] = vect_1x2.val[1];
			vect_1x4.val[3] = vect_2x2.val[1];
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
			vst4q((&Img_tmp_grad1->pData[indice3].x), vect_1x4);
=======
			vst4q((&Img_tmp_temporary->pData[indice3].x), vect_1x4);
>>>>>>> 0eb2bec (update type for gaussian and cannyedge):Source/cannysobel.c
=======
			vst4q((&Img_tmp_grad1->pData[indice3].x), vect_1x4);
>>>>>>> 80d0433 (bug fix for cannyedge vect):Source/cannysobel.c

			for(int p = 0; p<2; p++)
			{
				y = y+p*8;
				indice = x*w + y;
				indice3 = x3*w + y;	

<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
<<<<<<< HEAD:Source/FeatureDetection/arm_cannysobel.c
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_grad1->pData[indice3].x);
=======
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_temporary->pData[indice3].x);
>>>>>>> 0eb2bec (update type for gaussian and cannyedge):Source/cannysobel.c
=======
				q15x8x2_t vect_buff_di = vld2q_s16(&Img_tmp_grad1->pData[indice3].x);
>>>>>>> 80d0433 (bug fix for cannyedge vect):Source/cannysobel.c

				q15x8x2_t vec_x_y_1 = vld2q_s16(&Img_tmp_grad2->pData[((x-2)%3)*w + y].x);
				q15x8x2_t vec_y_1 = vld2q_s16(&Img_tmp_grad2->pData[((x-1)%3)*w + y-1].x);

				q15x8x2_t vect_grad_1;
				q15x8_t vectgradx = vsubq_s16(vec_x_y_1.val[0], vect_buff_di.val[0]);
				q15x8x2_t vec_y_2 = vld2q_s16(&Img_tmp_grad2->pData[((x-1)%3)*w + y+1].x);
				int numVect = 8;
				int16x8_t vect_res_1;
				q31x4_t vect_gradx_1 = vmullbq_int_s16(vectgradx, vectgradx);
				q15x8_t vectgrady = vsubq_s16(vec_y_1.val[1], vec_y_2.val[1]);
				q31x4_t vect_grady_1 = vmullbq_int_s16(vectgrady, vectgrady);

				vect_gradx_1 = vaddq_s32(vect_gradx_1, vect_grady_1);
				q31x4_t vect_gradx_2 = vmulltq_int_s16(vectgradx, vectgradx);
				vect_gradx_1 = vshrq(vect_gradx_1, 1);
				q31x4_t vect_grady_2 = vmulltq_int_s16(vectgrady, vectgrady);

				vect_gradx_2 = vaddq_s32(vect_gradx_2, vect_grady_2);
				vect_gradx_2 = vshrq(vect_gradx_2, 1);

				vect_grad_1.val[0] = vectgradx;
				vect_grad_1.val[1] = vectgrady;
            	vst2q(&Img_tmp_grad1->pData[(x-1)%3*w+y].x, vect_grad_1);
            	for(int j =0; j < numVect; j+=2)
				{
					if(vectgradx[j]==0&&vectgrady[j]==0)
					{
						vect_res_1[j] =  0;
						continue;
					}
					//mag
					q15_t out;
					q31_t root ;
					arm_sqrt_q31(vect_gradx_1[j>>1], &root);
      				out = root>>15;
					vect_res_1[j] = out;
				}
				for(int j =1; j < numVect; j+=2)
				{
					if(vectgradx[j]==0&&vectgrady[j]==0)
					{
						vect_res_1[j] =  0;
						continue;
					}
					//mag
					q15_t out;
					q31_t root ;
					arm_sqrt_q31(vect_gradx_2[j>>1], &root);
      				out = root>>15;
					vect_res_1[j] = out;
				}
				vst1q_s16((int16_t*)&Img_tmp_mag->pData[((x-1)%3)*w + y], vect_res_1);
				y = y-p*8;
			}
		}
		//Tail for the process of the temporary image gradient and magnitude
		for(int j = 0; j<numtail+1; j++)
		{
			int y = ((w-1)&0xFFF0) + j;
			if((y==0||y == w-1)&&x!=0&& x != ImageIn->height-1)
			{
				Img_tmp_grad2->pData[x3*w +y].y = (ImageIn->pData[(x-1)*w+y] + (ImageIn->pData[x*w+y]<<1) + ImageIn->pData[(x+1)*w+y])<<5;
				vect_mag[j] =0;
				continue;
			}
			if( x==0 || y==0||y == w-1)
			{
				vect_mag[j] = 0;
				continue;
			}
			Img_tmp_grad2->pData[x3*w +y].y = (ImageIn->pData[(x-1)*w+y] + (ImageIn->pData[x*w+y]<<1) + ImageIn->pData[(x+1)*w+y])<<5;
			Img_tmp_grad2->pData[x3*w +y].x = (ImageIn->pData[x*w+(y-1)] + (ImageIn->pData[x*w+(y)]<<1) + ImageIn->pData[x*w+(y+1)])<<5;
			if(x==1)
			{
				continue;
			}
			q63_t gradx = Img_tmp_grad2->pData[((x-2)%3)*w +y].x - Img_tmp_grad2->pData[(x3)*w +y].x;
			q63_t grady = Img_tmp_grad2->pData[(x-1)%3*w +(y-1)].y - Img_tmp_grad2->pData[(x-1)%3*w +(y+1)].y;
			Img_tmp_grad1->pData[(x-1)%3*w+y].x = gradx;
			Img_tmp_grad1->pData[(x-1)%3*w+y].y = grady;
			if(gradx==0&&grady==0)
			{
				Img_tmp_mag->pData[((((x-1)%3*w)) + y)+j] = 0;
				continue;
			}
			//mag
			q15_t vect[2] = { (q15_t)gradx, (q15_t)grady};

			q31_t in[2] = {((q31_t)vect[0]), ((q31_t)vect[1])};
			q31_t out2[2];
			q31_t out3;
			q31_t root;
			out2[0]=(in[0]*in[0]);
			out2[1]=(in[1]*in[1]);
			out3 = (out2[0]+out2[1])>>1;
			arm_sqrt_q31(out3, &root);
			Img_tmp_mag->pData[((((x-1)%3*w)) + y)+j] = root>>15;
		}
		//Computation of the output value, in vector
		//The focus heve been put on trying to avoid as much unecessary test as possible
        for(int y= 1; y < ((ImageIn-> width-2)&0xFFF8); y+=8)
        {
			int indice = (x-2)*ImageIn->width +y;
            vect_mag = vld1q(&Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y]);
			uint8x16_t vect_out;
            int16x8x2_t vect_grad = vld2q_s16(&Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].x);
			for(int j =0; j< 8; j+=1)
            {
                int mag = vect_mag[j];
				//First thing to test is the magnitude, if it's under the lowthreshold, then there is no need to test further, out will be 0
				if(mag < low_threshold)
				{
					vect_out[j] = 0;
					continue;
				}
				//we need to compare the magnitude with two other one, determine by the angle
				//if the magnitude is higher than those two, we check if the magnitude is higer the the high threshold 
				//if yes, the output value is 255
				//if no, we need to check in the 3x3 square of magnitude if ther is at least one that is higher than the value
				//if yes 255, else 0
				//This is the step where checking the angle is unavoidable and where we used the gradient buffer, in scalar because of the atan
				else
				{
					q15_t angle =0;
					arm_atan2_q15(vect_grad.val[0][j], vect_grad.val[1][j], &angle);

					arm_abs_q15( &angle, &angle, 1);
					//int grady = vect_grad.val[0][j];
					if((angle ) < (0xC4A))//22 in rad in 2.13
					{
						if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+j+1])
						{
							vect_out[j] = 0;
							continue;
						}
						else
						{
							if(mag < high_threshold)
							{
								if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1+j]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
								{
									vect_out[j] = Q8_ONE;
									continue;
								}
								else
								{
									vect_out[j] = 0;
									continue;
								}
							}
							else
							{
								vect_out[j] = Q8_ONE;
								continue;
							}
						}
						continue;
					}
					else if((angle ) < (0x256C))//67 in rad in 2.13
					{
						
							if( mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1])
							{
								vect_out[j] = 0;
								continue;
							}
							else
							{
								if(mag < high_threshold)
								{
									if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
									{
										vect_out[j] = Q8_ONE;
										continue;
									}
									else
									{
										vect_out[j] = 0;
										continue;
									}
								}
								else
								{
									vect_out[j] = Q8_ONE;
									continue;
								}
							}
							continue;
							
					}
					else if((angle ) < (0x3E8E))//112 in rad in 2.13
					{
						if(  0 <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j] - mag||  0 <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j] - mag)
						{
							vect_out[j] = 0;
							continue;
						}
						else
						{
							if(mag < high_threshold)
							{
								if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
								{
									vect_out[j] = Q8_ONE;
									continue;
								}
								else
								{
									vect_out[j] = 0;
									continue;
								}
							}
							else
							{
								vect_out[j] = Q8_ONE;
								continue;
							}
						}
						continue;
					}
					else if((angle ) < (0x595C))//160 in rad in 2.13
					{
							if( mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1])
							{
								vect_out[j] = 0;
								continue;
							}
							else
							{
								if(mag < high_threshold)
								{
									if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
									{
										vect_out[j] = Q8_ONE;
										continue;
									}
									else
									{
										vect_out[j] = 0;
										continue;
									}
								}
								else
								{
									vect_out[j] = Q8_ONE;
									continue;
								}
							}
							continue;
					}
					else//case tan between 135+22.5 and 180
					{
						if( mag <= Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[(x-2)%3*ImageIn->width+y+j+1])
						{
							vect_out[j] = 0;
							continue;
						}
						else
						{
							if(mag < high_threshold)
							{
								if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
								{
									vect_out[j] = Q8_ONE;
									continue;
								}
								else
								{
									vect_out[j] = 0;
									continue;
								}
							}
							else
							{
								vect_out[j] = Q8_ONE;
								continue;
							}
						}
						continue;
					}
				}
            }
			vst1q((uint8_t*)&ImageOut->pData[indice], vect_out);
        }
		//This is the tail of the computation of the output value
		for(int y= ((ImageIn-> width-1)&0xFFF0); y < ImageIn-> width; y++)
        {
			int indice = (x-2)*ImageIn->width +y;
			int indicepcent = (x-2)%3*ImageIn->width+y;
            int mag = Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y];
            if(mag != 0)
			{
				if(mag < low_threshold)
				{
					ImageOut->pData[indice] = 0;
					continue;
				}
				else
				{
					q15_t angle;
					arm_cv_gradient_q15_t grad = Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y];
					arm_atan2_q15(grad.x, grad.y, &angle);
					arm_abs_q15( &angle, &angle, 1);
					//int grady = grad.x;
					if((angle ) < (0xC4A))//22 in rad in 2.13
					{
				        	if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+1])
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
                                    	continue;
									}
								}
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
                                	continue;
								}
                            }
				        	continue;
					}
					else if((angle ) < (0x256C))//67 in rad in 2.13
					{
				        		if( mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1])
                                {
                                    ImageOut->pData[indice] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold)||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                        {
                                            ImageOut->pData[indice] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            ImageOut->pData[indice] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
					}
					else if((angle ) < (0x3E8E))//112 in rad in 2.13
					{
				        	if(  0 <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y] - mag||  0 <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y] - mag)
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
										continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
										continue;
									}
                                }
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
									continue;
								}
                            }
            	        	continue;
					}
					else if((angle ) < (0x595C))//160 in rad in 2.13
					{
				        		if( mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1])
                                {
                                    ImageOut->pData[indice] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                        {
                                            ImageOut->pData[indice] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            ImageOut->pData[indice] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
				        	//}
					}
					else//case tan between 135+22.5 and 180
					{
							if( mag <= Img_tmp_mag->pData[indicepcent-1] || mag <= Img_tmp_mag->pData[indicepcent+1])
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
                                    	continue;
									}
                                }
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
                                	continue;
								}
                            }
            	        	continue;
					}
				}
			}
			else
			{

				ImageOut->pData[indice] = 0;
			}
		}
	}
	//This is the loop to do the last lines
	//For the whole process, the line processeds where using different indice and now we catch up(nee to check for the before last one that the las line of magnitude read is 0 due to the border)
	//only one line to catch
	for(int x = ImageIn->height; x <ImageIn->height+1 ;x++)
	{
		//same proces as previously running on all the line minus the tail
		//to compute the output value
		for(int y= 0; y < ((ImageIn-> width-2)&0xFFF8); y+=8)
		{
			int indice = (x-2)*ImageIn->width +y;
			int indicepcent = (x-2)%3*ImageIn->width+y;
			vect_mag = vld1q(&Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y]);
			uint8x16_t vect_out;
			int16x8x2_t vect_grad = vld2q_s16(&Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y].x);
			for(int j =0; j< 8; j++)
			{
				int mag = vect_mag[j];
				if(mag != 0)
				{
					if(mag < low_threshold)
					{
						vect_out[j] = 0;
						
						continue;
					}
					else
					{
						q15_t angle;
						arm_atan2_q15(vect_grad.val[0][j],  vect_grad.val[1][j], &angle);
						arm_abs_q15( &angle, &angle, 1);
						//int grady = vect_grad.val[0][j];
						if((angle ) < (0xC4A))//22 in rad in 2.13
						{
				        	if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+j+1])
                            {
                                vect_out[j] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1+j]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
                                    {
                                        vect_out[j] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        vect_out[j] = 0;
                                    	continue;
									}
								}
                                else
                                {
                                    vect_out[j] = Q8_ONE;
                                	continue;
								}
                            }
				        	continue;
						}
						else if((angle ) < (0x256C))//67 in rad in 2.13
						{
				        		if( mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1])
                                {
                                    vect_out[j] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold)||Img_tmp_mag->pData[indicepcent+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
                                        {
                                            vect_out[j] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            vect_out[j] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        vect_out[j] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
				        	//}
						}
						else if((angle ) < (0x3E8E))//112 in rad in 2.13
						{
				        	if(  0 <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j] - mag||  0 <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j] - mag)
                            {
                                vect_out[j] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
                                    {
                                        vect_out[j] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        vect_out[j] = 0;
                                    	continue;
									}
                                }
                                else
                                {
                                    vect_out[j] = Q8_ONE;
                                	continue;
								}
                            }   
            	        	continue;
						}
						else if((angle ) < (0x595C))//160 in rad in 2.13
						{
				        		if( mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1] || mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1])
                                {
                                    vect_out[j] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
                                        {
                                            vect_out[j] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            vect_out[j] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        vect_out[j] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
						}
						else//case tan between 135+22.5 and 180
						{
				        	if( mag <= Img_tmp_mag->pData[indicepcent+j-1] || mag <= Img_tmp_mag->pData[indicepcent+j+1])
                            {
                                vect_out[j] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+j+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+j-1]>=(int)(high_threshold))
                                    {
                                        vect_out[j] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        vect_out[j] = 0;
                                    	continue;
									}
                                }
                                else
                                {
                                    vect_out[j] = Q8_ONE;
                                	continue;
								}
                            }   
            	        	continue;
						}
					}
				}
				else
				{
					vect_out[j] = 0;
				}
			}
			vst1q((uint8_t*)&ImageOut->pData[indice], vect_out);
        }
		//tail computation
		for(int y= ((ImageIn-> width-1)&0xFFF0); y < ImageIn-> width; y++)
		{
			int indice = (x-2)*ImageIn->width +y;
			int indicepcent = (x-2)%3*ImageIn->width+y;
			int mag = Img_tmp_mag->pData[((x-2)%3) * (ImageIn->width)+y];
			if(mag != 0)
			{
				if(mag < low_threshold)
				{
					ImageOut->pData[indice] = 0;
					continue;
				}
				else
				{
					q15_t angle;
					arm_cv_gradient_q15_t grad = Img_tmp_grad1->pData[((x-2)%3)*ImageIn->width+y];
					arm_atan2_q15(grad.x, grad.y, &angle);
					arm_abs_q15( &angle, &angle, 1);
					//int grady = grad.x;
					if((angle ) < (0xC4A))//22 in rad in 2.13
					{
				        	if( mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[((x-2)%3)*ImageIn->width+y+1])
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
                                    	continue;
									}
								}
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
                                	continue;
								}
                            }
				        	continue;
					}
					else if((angle ) < (0x256C))//67 in rad in 2.13
					{
				        		if( mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1])
                                {
                                    ImageOut->pData[indice] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold)||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                        {
                                            ImageOut->pData[indice] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            ImageOut->pData[indice] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
				        	//}
							
					}
					else if((angle ) < (0x3E8E))//112 in rad in 2.13
					{
				        	if(  0 <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y] - mag||  0 <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y] - mag)
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
										continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
										continue;
									}
                                }
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
									continue;
								}
                            }   
            	        	continue;
					}
					else if((angle ) < (0x595C))//160 in rad in 2.13
					{
				        		if( mag <= Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1] || mag <= Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1])
                                {
                                    ImageOut->pData[indice] = 0;
                                	continue;
								}
                                else
                                {
                                    if(mag < high_threshold)
                                    {
                                        if (Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[indicepcent+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                        {
                                            ImageOut->pData[indice] = Q8_ONE;
                                        	continue;
										}
                                        else
                                        {
                                            ImageOut->pData[indice] = 0;
                                        	continue;
										}
                                    }
                                    else
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                }
				        		continue;
				        	//}
				        	
					}
					else//case tan between 135+22.5 and 180
					{
							if( mag <= Img_tmp_mag->pData[indicepcent-1] || mag <= Img_tmp_mag->pData[indicepcent+1])
                            {
                                ImageOut->pData[indice] = 0;
                            	continue;
							}
                            else
                            {
                                if(mag < high_threshold)
                                {
                                    if(Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y-1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-3)%3*ImageIn->width+y+1]>=(int)(high_threshold)||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y+1]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y]>=(int)(high_threshold) ||Img_tmp_mag->pData[(x-1)%3*ImageIn->width+y-1]>=(int)(high_threshold))
                                    {
                                        ImageOut->pData[indice] = Q8_ONE;
                                    	continue;
									}
                                    else
                                    {
                                        ImageOut->pData[indice] = 0;
                                    	continue;
									}
                                }
                                else
                                {
                                    ImageOut->pData[indice] = Q8_ONE;
                                	continue;
								}
                            }   
            	        	continue;
					}
				}
			}
			else
			{
				ImageOut->pData[indice] = 0;
			}
		}
	}
}   
#endif
