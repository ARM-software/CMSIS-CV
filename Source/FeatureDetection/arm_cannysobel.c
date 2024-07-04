/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        cannysobel.c
 * Description:  Last steps of the canny edge detector (after the gaussian
 * filter)
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

// Number of Line of the circular buffer
#define NB_LINE_BUF 3
#define DEG_TO_RAD_Q2_13(angle) (q15_t) roundf(angle *PI / 180.0f * powf(2.0f, 13))
//
#define U8_TO_Q2_13(a) ((a) << 5)
#define Q5_10_TO_Q15(a) ((a) << 5)
#define Q31_TO_Q15(a) ((a) >> 15)
/*
Macro that implement core operation of threshold and hysteresis for the canny edge
The different values of x are angle dependent see helper below, and relative to the 8 neighboring pixels
*/
#define DECISION(x0, x1, x2, x3, x4, x5, x6, x7, y0, thresh, width, datain, dataout, offs, mag, col, row)              \
    if (mag <= datain[((col - x0) % NB_LINE_BUF) * width + row - y0] ||                                                \
        mag <= datain[((col - x1) % NB_LINE_BUF) * width + row + y0])                                                  \
    {                                                                                                                  \
        dataout[offs] = 0;                                                                                             \
        continue;                                                                                                      \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        if (mag < thresh)                                                                                              \
        {                                                                                                              \
            if (datain[((col - (x2)) % NB_LINE_BUF) * width + row - 1] >= thresh ||                                    \
                datain[((col - (x3)) % NB_LINE_BUF) * width + row + (y0 - 1)] >= thresh ||                             \
                datain[((col - (x4)) % NB_LINE_BUF) * width + row + 1] >= thresh ||                                    \
                datain[((col - (x5)) % NB_LINE_BUF) * width + row - 1] >= thresh ||                                    \
                datain[((col - (x6)) % NB_LINE_BUF) * width + row - (y0 - 1)] >= thresh ||                             \
                datain[((col - (x7)) % NB_LINE_BUF) * width + row + 1] >= thresh)                                      \
            {                                                                                                          \
                dataout[offs] = Q8_ONE;                                                                                \
                continue;                                                                                              \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                dataout[offs] = 0;                                                                                     \
                continue;                                                                                              \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            dataout[offs] = Q8_ONE;                                                                                    \
            continue;                                                                                                  \
        }                                                                                                              \
    }                                                                                                                  \
    continue;

#define DECISION_LAST(x0, x2, x3, x4, x5, x6, x7, y0, thresh, width, datain, dataout, offs, mag, col, row)             \
    if (mag <= datain[((col - x0) % NB_LINE_BUF) * width + row - y0])                                                  \
    {                                                                                                                  \
        dataout[offs] = 0;                                                                                             \
        continue;                                                                                                      \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        if (mag < thresh)                                                                                              \
        {                                                                                                              \
            if (datain[((col - (x2)) % NB_LINE_BUF) * width + row - 1] >= thresh ||                                    \
                datain[((col - (x3)) % NB_LINE_BUF) * width + row] >= thresh ||                                        \
                datain[((col - (x4)) % NB_LINE_BUF) * width + row + 1] >= thresh ||                                    \
                datain[((col - (x5)) % NB_LINE_BUF) * width + row - 1] >= thresh ||                                    \
                datain[((col - (x6)) % NB_LINE_BUF) * width + row] >= thresh ||                                        \
                datain[((col - (x7)) % NB_LINE_BUF) * width + row + 1] >= thresh)                                      \
            {                                                                                                          \
                dataout[offs] = Q8_ONE;                                                                                \
                continue;                                                                                              \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                dataout[offs] = 0;                                                                                     \
                continue;                                                                                              \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            dataout[offs] = Q8_ONE;                                                                                    \
            continue;                                                                                                  \
        }                                                                                                              \
    }                                                                                                                  \
    continue;

// Helpers for different decision
#define VERTICAL_CASE(threshold, width, datain, dataout, idx, mag, col, row)                                           \
    DECISION(2, 2, 3, 3, 3, 1, 1, 1, 1, threshold, width, datain, dataout, idx, mag, col, row)
#define DIAGONAL_45_CASE(threshold, width, datain, dataout, idx, mag, col, row)                                        \
    DECISION(1, 3, 3, 3, 2, 2, 1, 1, 1, threshold, width, datain, dataout, idx, mag, col, row)
#define HORIZONTAL_CASE(threshold, width, datain, dataout, idx, mag, col, row)                                         \
    DECISION(3, 1, 3, 3, 2, 2, 1, 1, 0, threshold, width, datain, dataout, idx, mag, col, row)
#define DIAGONAL_135_CASE(threshold, width, datain, dataout, idx, mag, col, row)                                       \
    DECISION(3, 1, 3, 3, 2, 2, 1, 1, 1, threshold, width, datain, dataout, idx, mag, col, row)

#define VERTICAL_CASE_BOT_BORDER(threshold, width, datain, dataout, idx, mag, col, row)                                \
    DECISION(2, 2, 1, 1, 1, 1, 1, 1, 1, threshold, width, datain, dataout, idx, mag, col, row)
#define DIAGONAL_45_CASE_BOT_BORDER(threshold, width, datain, dataout, idx, mag, col, row)                             \
    DECISION_LAST(1, 1, 1, 2, 2, 1, 1, 1, threshold, width, datain, dataout, idx, mag, col, row)
#define HORIZONTAL_CASE_BOT_BORDER(threshold, width, datain, dataout, idx, mag, col, row)                              \
    DECISION(1, 1, 1, 1, 2, 2, 1, 1, 0, threshold, width, datain, dataout, idx, mag, col, row)
#define DIAGONAL_135_CASE_BOT_BORDER(threshold, width, datain, dataout, idx, mag, col, row)                            \
    DECISION_LAST(1, 1, 1, 2, 2, 1, 1, -1, threshold, width, datain, dataout, idx, mag, col, row)

#define THRESHOLDING_HYSTERESIS(angle, thresh, width, data_mag, data_out, idx, mag, x, y)                              \
    if ((angle) < (DEG_TO_RAD_Q2_13(22)))                                                                              \
    {                                                                                                                  \
        VERTICAL_CASE(thresh, width, data_mag, data_out, idx, mag, x, y)                                               \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(67)))                                                                         \
    {                                                                                                                  \
        DIAGONAL_45_CASE(thresh, width, data_mag, data_out, idx, mag, x, y)                                            \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(112)))                                                                        \
    {                                                                                                                  \
        HORIZONTAL_CASE(thresh, width, data_mag, data_out, idx, mag, x, y)                                             \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(160)))                                                                        \
    {                                                                                                                  \
        DIAGONAL_135_CASE(thresh, width, data_mag, data_out, idx, mag, x, y)                                           \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        VERTICAL_CASE(thresh, width, data_mag, data_out, idx, mag, x, y)                                               \
    }

#define THRESHOLDING_HYSTERESIS_BOTTOM_BORDER(angle, thresh, width, data_mag, data_out, idx, mag, x, y)                \
    if ((angle) < (DEG_TO_RAD_Q2_13(22)))                                                                              \
    {                                                                                                                  \
        VERTICAL_CASE_BOT_BORDER(thresh, width, data_mag, data_out, idx, mag, x, y)                                    \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(67)))                                                                         \
    {                                                                                                                  \
        DIAGONAL_45_CASE_BOT_BORDER(thresh, width, data_mag, data_out, idx, mag, x, y)                                 \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(112)))                                                                        \
    {                                                                                                                  \
        HORIZONTAL_CASE_BOT_BORDER(thresh, width, data_mag, data_out, idx, mag, x, y)                                  \
    }                                                                                                                  \
    else if ((angle) < (DEG_TO_RAD_Q2_13(160)))                                                                        \
    {                                                                                                                  \
        DIAGONAL_135_CASE_BOT_BORDER(thresh, width, data_mag, data_out, idx, mag, x, y)                                \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        VERTICAL_CASE_BOT_BORDER(thresh, width, data_mag, data_out, idx, mag, x, y)                                    \
    }

/**
  @ingroup featureDetection
 */

/**
 * @brief      Return the scratch size for canny_sobel function
 *
 * @param[in]     width         The width of the image
 * @return		  Scratch size in bytes
 */
uint16_t arm_cv_get_scratch_size_canny_sobel(int width)
{
    return ((NB_LINE_BUF + 2 * NB_LINE_BUF + 2 * NB_LINE_BUF) * width * sizeof(q15_t));
}

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

#define ELTS_PER_VECT_16 8

#define MASK_16(x) ((x) & 0x0FFE0)

static void arm_cv_gradient_intermediate(int offsIn, int offsOut, int width, const uint8_t *dataIn,
                                         arm_cv_gradient_q15_t *dataOut)
{
    q15x8x2_t vect_2x2;
    q15x8x2_t vect_1x2;
    q15x8x2_t vect_3x2;
    q15x8x4_t vect_1x4;
    q15x8x2_t vect_lowx2;
    q15x8x2_t vect_highx2;

    uint8x16_t vect_1 = vld1q(&dataIn[offsIn - 1]);

    vect_1x2.val[0] = vshllbq(vect_1, 5);

    uint8x16_t vect_3 = vld1q(&dataIn[offsIn + 1]);

    vect_3x2.val[0] = vshllbq(vect_3, 5);
    vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
    vect_1x2.val[1] = vshlltq(vect_1, 5);

    uint8x16_t vect_2 = vld1q(&dataIn[offsIn]);

    vect_3x2.val[1] = vshlltq(vect_3, 5);
    vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
    vect_2x2.val[0] = vshllbq(vect_2, 6);
    vect_1x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
    vect_2x2.val[1] = vshlltq(vect_2, 6);
    vect_1x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

    uint8x16_t vecth = vld1q(&dataIn[offsIn - width]);

    vect_highx2.val[0] = vshllbq(vecth, 5);
    vect_highx2.val[1] = vshlltq(vecth, 5);

    uint8x16_t vectl = vld1q(&dataIn[offsIn + width]);

    vect_lowx2.val[0] = vshllbq(vectl, 5);
    vect_lowx2.val[1] = vshlltq(vectl, 5);
    vect_2x2.val[0] = vaddq(vect_lowx2.val[0], vect_2x2.val[0]);
    vect_2x2.val[0] = vaddq(vect_highx2.val[0], vect_2x2.val[0]);
    vect_2x2.val[1] = vaddq(vect_lowx2.val[1], vect_2x2.val[1]);
    vect_2x2.val[1] = vaddq(vect_highx2.val[1], vect_2x2.val[1]);
    vect_1x4.val[0] = vect_1x2.val[0];
    vect_1x4.val[1] = vect_2x2.val[0];
    vect_1x4.val[2] = vect_1x2.val[1];
    vect_1x4.val[3] = vect_2x2.val[1];
    vst4q((&dataOut[offsOut].x), vect_1x4);
}

static void arm_cv_gradient_magnitude(int rowIdx, int colIdx, int width, int offsOut, arm_cv_gradient_q15_t *dataGrad1,
                                      arm_cv_gradient_q15_t *dataGrad2, q15_t *magOut)
{
    int16x8_t vect_res_1;
    for (int p = 0; p < 2; p++)
    {
        q15x8x2_t vect_grad_1;

        colIdx += p * ELTS_PER_VECT_16;
        offsOut += p * ELTS_PER_VECT_16;
        // Apply [1,2,1] kernel for computation of a part of the sobel gradient
        q15x8x2_t vect_buff_di = vld2q(&dataGrad2[offsOut].x);
        q15x8x2_t vec_x_y_1 = vld2q(&dataGrad2[((rowIdx - 2) % NB_LINE_BUF) * width + colIdx].x);
        q15x8x2_t vec_y_1 = vld2q(&dataGrad2[((rowIdx - 1) % NB_LINE_BUF) * width + colIdx - 1].x);
        q15x8_t vectgradx = vsubq(vec_x_y_1.val[0], vect_buff_di.val[0]);
        q15x8x2_t vec_y_2 = vld2q(&dataGrad2[((rowIdx - 1) % NB_LINE_BUF) * width + colIdx + 1].x);
        q31x4_t vect_gradx_1 = vmullbq_int(vectgradx, vectgradx);
        q15x8_t vectgrady = vsubq(vec_y_1.val[1], vec_y_2.val[1]);
        q31x4_t vect_grady_1 = vmullbq_int(vectgrady, vectgrady);
        vect_gradx_1 = vaddq(vect_gradx_1, vect_grady_1);
        q31x4_t vect_gradx_2 = vmulltq_int(vectgradx, vectgradx);
        vect_gradx_1 = vshrq(vect_gradx_1, 1);
        q31x4_t vect_grady_2 = vmulltq_int(vectgrady, vectgrady);
        vect_gradx_2 = vaddq(vect_gradx_2, vect_grady_2);
        vect_gradx_2 = vshrq(vect_gradx_2, 1);
        vect_grad_1.val[0] = vectgradx;
        vect_grad_1.val[1] = vectgrady;
        vst2q(&dataGrad1[(rowIdx - 1) % NB_LINE_BUF * width + colIdx].x, vect_grad_1);

        for (int j = 0; j < ELTS_PER_VECT_16; j += 2)
        {
            if (vectgradx[j] == 0 && vectgrady[j] == 0)
            {
                vect_res_1[j] = 0;
                continue;
            }
            q15_t out;
            q31_t root;
            arm_sqrt_q31(vect_gradx_1[j >> 1], &root);
            out = Q31_TO_Q15(root);
            vect_res_1[j] = out;
        }
        for (int j = 1; j < ELTS_PER_VECT_16; j += 2)
        {
            if (vectgradx[j] == 0 && vectgrady[j] == 0)
            {
                vect_res_1[j] = 0;
                continue;
            }
            q15_t out;
            q31_t root;
            arm_sqrt_q31(vect_gradx_2[j >> 1], &root);
            out = Q31_TO_Q15(root);
            vect_res_1[j] = out;
        }
        vst1q((int16_t *)&magOut[((rowIdx - 1) % NB_LINE_BUF) * width + colIdx], vect_res_1);
        colIdx -= p * ELTS_PER_VECT_16;
    }
}

static void arm_cv_gradient_magnitude_tail(int rowIdx, int rowIdxOut, int width, arm_cv_gradient_q15_t *dataGrad1,
                                           arm_cv_gradient_q15_t *dataGrad2, const arm_cv_image_gray8_t *imageIn,
                                           q15_t *magOut, uint8_t *dataOut)
{
    uint8_t *data_in = imageIn->pData;
    for (int y = MASK_16(width - 1); y < width; y++)
    {
        if ((y == 0 || y == width - 1) && rowIdx != 0 && rowIdx != imageIn->height - 1)
        {
            dataGrad2[rowIdxOut * width + y].y =
                Q5_10_TO_Q15(data_in[(rowIdx - 1) * width + y] + (data_in[rowIdx * width + y] << 1) +
                             data_in[(rowIdx + 1) * width + y]);
            magOut[((((rowIdx - 1) % NB_LINE_BUF * width)) + y)] = 0;
            dataOut[rowIdx * width + y] = 0;
            continue;
        }
        if (rowIdx == 0 || y == 0 || y == width - 1)
        {
            magOut[((((rowIdx - 1) % NB_LINE_BUF * width)) + y)] = 0;
            dataOut[rowIdx * width + y] = 0;
            continue;
        }
        dataGrad2[rowIdxOut * width + y].y = Q5_10_TO_Q15(
            data_in[(rowIdx - 1) * width + y] + (data_in[rowIdx * width + y] << 1) + data_in[(rowIdx + 1) * width + y]);
        dataGrad2[rowIdxOut * width + y].x =
            Q5_10_TO_Q15(data_in[rowIdx * width + (y - 1)] + (data_in[rowIdx * width + (y)] << 1) +
                         data_in[rowIdx * width + (y + 1)]);
        if (rowIdx == 1)
        {
            continue;
        }
        q15_t gradx = dataGrad2[((rowIdx - 2) % NB_LINE_BUF) * width + y].x - dataGrad2[(rowIdxOut)*width + y].x;
        q15_t grady = dataGrad2[(rowIdx - 1) % NB_LINE_BUF * width + (y - 1)].y -
                      dataGrad2[(rowIdx - 1) % NB_LINE_BUF * width + (y + 1)].y;
        dataGrad1[(rowIdx - 1) % NB_LINE_BUF * width + y].x = gradx;
        dataGrad1[(rowIdx - 1) % NB_LINE_BUF * width + y].y = grady;
        if (gradx == 0 && grady == 0)
        {
            magOut[((((rowIdx - 1) % NB_LINE_BUF * width)) + y)] = 0;
            dataOut[(rowIdx - 1) * width + y] = 0;
            continue;
        }
        q31_t out;
        q31_t root;
        out = (gradx * gradx + grady * grady) >> 1;
        arm_sqrt_q31(out, &root);
        out = Q31_TO_Q15(root);
        magOut[(rowIdx - 1) % NB_LINE_BUF * width + y] = (q15_t)out;
    }
}
#else
static void arm_cv_compute_buffer_line_canny_sobel(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut,
                                                   arm_cv_gradient_q15_t *grad1, arm_cv_gradient_q15_t *grad2,
                                                   q15_t *magOut, int rowIdx)
{
    const int width = imageIn->width;
    int xm = rowIdx % NB_LINE_BUF;
    int idx;
    uint8_t *data_in = imageIn->pData;
    uint8_t *data_out = imageOut->pData;

    grad2[xm * width].x = 0;
    grad2[xm * width].y =
        Q5_10_TO_Q15((data_in[(rowIdx - 1) * width] + (data_in[rowIdx * width] << 1) + data_in[(rowIdx + 1) * width]));
    data_out[(rowIdx - 1) * width] = 0;
    for (int y = 1; y < width - 1; y++)
    {
        idx = (rowIdx - 1) * width + y;

        grad2[xm * width + y].y = Q5_10_TO_Q15(data_in[(rowIdx - 1) * width + y] + (data_in[rowIdx * width + y] << 1) +
                                               data_in[(rowIdx + 1) * width + y]);
        grad2[xm * width + y].x =
            Q5_10_TO_Q15(data_in[rowIdx * width + (y - 1)] + (data_in[rowIdx * width + (y)] << 1) +
                         data_in[rowIdx * width + (y + 1)]);
        q15_t gradx = grad2[((rowIdx - 2) % NB_LINE_BUF) * width + y].x - grad2[(xm)*width + y].x;
        q15_t grady = grad2[((rowIdx - 1) % NB_LINE_BUF) * width + (y - 1)].y -
                      grad2[((rowIdx - 1) % NB_LINE_BUF) * width + (y + 1)].y;
        if (gradx == 0 && grady == 0)
        {
            int idxp = (rowIdx - 1) % NB_LINE_BUF * width + y;

            data_out[idx] = 0;
            magOut[idxp] = 0;
            grad1[idxp].y = grady;
            grad1[idxp].x = gradx;
            continue;
        }
        q31_t out;
        q31_t root;
        out = (gradx * gradx + grady * grady) >> 1;
        arm_sqrt_q31(out, &root);
        out = Q31_TO_Q15(root);
        grad1[(rowIdx - 1) % NB_LINE_BUF * width + y].y = grady;
        grad1[(rowIdx - 1) % NB_LINE_BUF * width + y].x = gradx;
        magOut[(rowIdx - 1) % NB_LINE_BUF * width + y] = (q15_t)out;
    }
    grad2[xm * width].x = 0;
    grad2[xm * width + width - 1].y =
        Q5_10_TO_Q15(data_in[(rowIdx - 1) * width + width - 1] + (data_in[rowIdx * width + width - 1] << 1) +
                     data_in[(rowIdx + 1) * width + width - 1]);
    data_out[(rowIdx - 1) * width + width - 1] = 0;
}
#endif
/**
  @ingroup featureDetection
 */

/**
 * @brief      Canny edge with sobel integrated
 *
 * @param[in]     imageIn         The input image
 * @param[out]    imageOut        The output image
 * @param[in,out] scratch   Temporary buffer
 * @param[in]     lowThreshold   The low threshold
 * @param[in]     highThreshold  The high threshold
 *
 * @par  Temporary buffer sizing:
 *
 * Will use a temporary buffer to store intermediate values of gradient and magnitude.
 *
 * Size of temporary buffer is given by
 * arm_cv_get_scratch_size_canny_sobel(int width)
 */
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

void arm_cv_canny_edge_sobel(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut, q15_t *scratch,
                             uint8_t lowThreshold, uint8_t highThreshold)
{
    q15x8_t vect_mag;
    int x = 0;
    const int width = imageIn->width;
    q31_t low_threshold = U8_TO_Q2_13(lowThreshold);
    q31_t high_threshold = U8_TO_Q2_13(highThreshold);
    arm_cv_gradient_q15_t *data_grad1 = (arm_cv_gradient_q15_t *)&scratch[3 * NB_LINE_BUF * width];
    arm_cv_gradient_q15_t *data_grad2 = (arm_cv_gradient_q15_t *)&scratch[NB_LINE_BUF * width];
    q15_t *data_mag = scratch;

    const uint8_t *data_in = imageIn->pData;
    uint8_t *data_out = imageOut->pData;
    for (int t = 0; t < width * 3; t++)
    {
        data_grad1[t].x = 0;
        data_mag[t] = 0;
        data_grad2[t].x = 0;
    }
    // First line of buffer
    data_out[x * width] = 0;
    for (int y = 1; y < ((width) >> 4) + 1; y++)
    {
        int idx = ((y - 1) << 4) + 1;
        // Apply [1,2,1] kernel for computation of a part of the sobel gradient, only for column direction
        q15x8x2_t vect_2x2;
        q15x8x2_t vect_1x2;
        q15x8x2_t vect_3x2;
        q15x8x4_t vect_1x4;

        uint8x16_t vect_1 = vld1q(&data_in[idx - 1]);
        vect_1x2.val[0] = vshllbq(vect_1, 5);

        uint8x16_t vect_3 = vld1q(&data_in[idx + 1]);
        vect_3x2.val[0] = vshllbq(vect_3, 5);
        vect_1x2.val[0] = vaddq(vect_1x2.val[0], vect_3x2.val[0]);
        vect_1x2.val[1] = vshlltq(vect_1, 5);

        uint8x16_t vect_2 = vld1q(&data_in[idx]);
        vect_3x2.val[1] = vshlltq(vect_3, 5);
        vect_1x2.val[1] = vaddq(vect_1x2.val[1], vect_3x2.val[1]);
        vect_2x2.val[0] = vshllbq(vect_2, 6);
        vect_2x2.val[0] = vaddq(vect_2x2.val[0], vect_1x2.val[0]);
        vect_2x2.val[1] = vshlltq(vect_2, 6);
        vect_2x2.val[1] = vaddq(vect_2x2.val[1], vect_1x2.val[1]);

        q15x8_t vect_void = vdupq_n_s16(0);
        vect_1x4.val[0] = vect_2x2.val[0];
        vect_1x4.val[1] = vect_void;
        vect_1x4.val[2] = vect_2x2.val[1];
        vect_1x4.val[3] = vect_void;
        vst4q((&data_grad2[idx].x), vect_1x4);
        vst1q(&data_out[idx], (uint8x16_t)vect_void);
        vst1q(&data_out[idx + 16], (uint8x16_t)vect_void);
    }
    // tail
    int numtail = (width - 1) - MASK_16(width - 1);
    if (numtail > 0)
    {
        for (int j = 0; j < numtail + 1; j++)
        {
            int y = ((width)-numtail - 1);
            data_grad2[y + j].x = Q5_10_TO_Q15(data_in[(y - 1) + j] + (data_in[(y) + j] << 1) + data_in[(y + 1) + j]);
            data_grad2[y + j].y = 0;
            data_out[y + j] = 0;
        }
    }
    data_out[x * width + width - 1] = 0;
    // Second line for buffer
    x = 1;
    data_grad2[x * width].x = 0;
    data_out[x * width] = 0;
    data_grad2[x * width].y =
        Q5_10_TO_Q15(data_in[(x - 1) * width] + (data_in[x * width] << 1) + data_in[(x + 1) * width]);
    for (int y = 1; y < ((width) >> 4) + 1; y++)
    {
        int idx = width + ((y - 1) << 4) + 1;
        arm_cv_gradient_intermediate(idx, idx, width, data_in, data_grad2);
    }
    // Tail
    if (numtail > 0)
    {
        for (int j = 0; j < numtail + 1; j++)
        {
            int x = 1;
            int y = ((width)-numtail - 1);
            data_grad2[x * width + y + j].y =
                Q5_10_TO_Q15(data_in[(x - 1) * width + y + j] + (data_in[x * width + y + j] << 1) +
                             data_in[(x + 1) * width + y + j]);
            data_grad2[x * width + y + j].x =
                Q5_10_TO_Q15(data_in[x * width + (y - 1) + j] + (data_in[x * width + (y) + j] << 1) +
                             data_in[x * width + (y + 1) + j]);
        }
    }
    data_grad2[width + width - 1].x = 0;
    data_grad2[width + width - 1].y =
        Q5_10_TO_Q15(data_in[(x - 1) * width + width - 1] + (data_in[x * width + width - 1] << 1) +
                     data_in[(x + 1) * width + width - 1]);
    for (int x = 2; x < 3; x++)
    {
        int x3 = x % NB_LINE_BUF;
        data_grad2[x3 * width].y =
            Q5_10_TO_Q15(data_in[(x - 1) * width] + (data_in[x * width] << 1) + data_in[(x + 1) * width]);
        data_out[(x - 2) * width] = 0;
        for (int y = 1; y < width - 15; y += 16)
        {
            int idx = x * width + y;
            int idx3 = x3 * width + y;
            arm_cv_gradient_intermediate(idx, idx3, width, data_in, data_grad2);
            arm_cv_gradient_magnitude(x, y, width, idx3, data_grad1, data_grad2, data_mag);
        }
        // Tail
        arm_cv_gradient_magnitude_tail(x, x3, width, data_grad1, data_grad2, imageIn, data_mag, data_out);
    }
    // Core loop
    for (int x = 3; x < imageIn->height; x++)
    {
        int x3 = x % NB_LINE_BUF;
        data_grad2[x3 * width].y =
            Q5_10_TO_Q15(data_in[(x - 1) * width] + (data_in[x * width] << 1) + data_in[(x + 1) * width]);
        data_out[(x - 2) * width] = 0;
        for (int y = 1; y < width - 16; y += 16)
        {
            int idx = x * width + y;
            int idx3 = x3 * width + y;
            arm_cv_gradient_intermediate(idx, idx3, width, data_in, data_grad2);
            arm_cv_gradient_magnitude(x, y, width, idx3, data_grad1, data_grad2, data_mag);
        }
        arm_cv_gradient_magnitude_tail(x, x3, width, data_grad1, data_grad2, imageIn, data_mag, data_out);
        for (int y = 1; y < MASK_16(width - 2); y += ELTS_PER_VECT_16)
        {
            int idx = (x - 2) * width + y;
            vect_mag = vld1q(&data_mag[((x - 2) % NB_LINE_BUF) * (width) + y]);
            uint8x16_t vect_out;
            int16x8x2_t vect_grad = vld2q(&data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x);
            for (int j = 0; j < ELTS_PER_VECT_16; j += 1)
            {
                int mag = vect_mag[j];
                if (mag < low_threshold)
                {
                    vect_out[j] = 0;
                    continue;
                }
                else
                {
                    q15_t angle = 0;
                    arm_atan2_q15(vect_grad.val[0][j], vect_grad.val[1][j], &angle);

                    arm_abs_q15(&angle, &angle, 1);
                    THRESHOLDING_HYSTERESIS(angle, high_threshold, width, data_mag, vect_out, j, mag, x, y + j)
                }
            }
            vst1q((uint8_t *)&data_out[idx], vect_out);
        }
        // tail
        for (int y = MASK_16(width - 1); y < width; y++)
        {
            int idx = (x - 2) * width + y;
            int mag = data_mag[((x - 2) % NB_LINE_BUF) * (width) + y];
            if (mag != 0)
            {
                if (mag < low_threshold)
                {
                    data_out[idx] = 0;
                    continue;
                }
                else
                {
                    q15_t angle;
                    arm_cv_gradient_q15_t grad = data_grad1[((x - 2) % NB_LINE_BUF) * width + y];
                    arm_atan2_q15(grad.x, grad.y, &angle);
                    arm_abs_q15(&angle, &angle, 1);
                    THRESHOLDING_HYSTERESIS(angle, high_threshold, width, data_mag, data_out, idx, mag, x, y)
                }
            }
            else
            {
                data_out[idx] = 0;
            }
        }
    }
    // Last lines
    for (int x = imageIn->height; x < imageIn->height + 1; x++)
    {
        for (int y = 0; y < MASK_16(width - 2); y += ELTS_PER_VECT_16)
        {
            data_out[(x - 1) * width] = 0;
            int idx = (x - 2) * width + y;
            vect_mag = vld1q(&data_mag[((x - 2) % NB_LINE_BUF) * (width) + y]);
            uint8x16_t vect_out;
            if (y + 16 < width - 1)
            {
                uint8x16_t vect_void = vdupq_n_u8(0);
                vst1q(&data_out[(x - 1) * width + y], vect_void);
            }
            int16x8x2_t vect_grad = vld2q(&data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x);
            for (int j = 0; j < ELTS_PER_VECT_16; j++)
            {
                int mag = vect_mag[j];
                if (mag != 0)
                {
                    if (mag < low_threshold)
                    {
                        vect_out[j] = 0;
                        continue;
                    }
                    else
                    {
                        q15_t angle;
                        arm_atan2_q15(vect_grad.val[0][j], vect_grad.val[1][j], &angle);
                        arm_abs_q15(&angle, &angle, 1);
                        THRESHOLDING_HYSTERESIS_BOTTOM_BORDER(angle, high_threshold, width, data_mag, vect_out, j, mag,
                                                              x, y + j)
                    }
                }
                else
                {
                    vect_out[j] = 0;
                }
            }
            vst1q((uint8_t *)&data_out[idx], vect_out);
        }
        // tail
        for (int y = MASK_16(width - 1); y < width; y++)
        {
            int idx = (x - 2) * width + y;
            int mag = data_mag[((x - 2) % NB_LINE_BUF) * (width) + y];
            data_out[idx + width] = 0;
            q15_t angle;
            arm_cv_gradient_q15_t grad = data_grad1[((x - 2) % NB_LINE_BUF) * width + y];
            arm_atan2_q15(grad.x, grad.y, &angle);
            arm_abs_q15(&angle, &angle, 1);
            if (mag < low_threshold)
            {
                data_out[idx] = 0;
                continue;
            }
            else
            {
                q15_t angle;
                arm_atan2_q15(data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x,
                              data_grad1[((x - 2) % NB_LINE_BUF) * width + y].y, &angle);
                if (data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x == 0 &&
                    data_grad1[((x - 2) % NB_LINE_BUF) * width + y].y == 0)
                {
                    angle = 0;
                }
                arm_abs_q15(&angle, &angle, 1);
                THRESHOLDING_HYSTERESIS_BOTTOM_BORDER(angle, high_threshold, width, data_mag, data_out, idx, mag, x, y)
            }
        }
    }
}
#else

void arm_cv_canny_edge_sobel(const arm_cv_image_gray8_t *imageIn, arm_cv_image_gray8_t *imageOut, q15_t *scratch,
                             uint8_t lowThreshold, uint8_t highThreshold)
{
    int16_t width = imageIn->width;
    q31_t low_threshold = U8_TO_Q2_13(lowThreshold);
    q31_t high_threshold = U8_TO_Q2_13(highThreshold);

    arm_cv_gradient_q15_t *data_grad1 = (arm_cv_gradient_q15_t *)&scratch[3 * NB_LINE_BUF * width];
	arm_cv_gradient_q15_t *data_grad2 = (arm_cv_gradient_q15_t *)&scratch[NB_LINE_BUF * width];
    q15_t *data_mag = scratch;
    uint8_t *data_in = imageIn->pData;
    uint8_t *data_out = imageOut->pData;
    int x = 0;
    for (int t = 0; t < width * NB_LINE_BUF; t++)
    {
        data_grad1[t].x = 0;
        data_mag[t] = 0;
        data_grad2[t].x = 0;
    }
    data_out[x * width] = 0;
    for (int y = 1; y < width - 1; y++)
    {
        // Apply [1,2,1] kernel for computation of a part of the sobel gradient
        data_grad2[x * width + y].x =
            Q5_10_TO_Q15(data_in[x * width + (y - 1)] + (data_in[x * width + (y)] << 1) + data_in[x * width + (y + 1)]);
        data_out[x * width + y] = 0;
    }
    data_out[x * width + width - 1] = 0;
    x = 1;
    data_grad2[x * width].y =
        Q5_10_TO_Q15(data_in[(x - 1) * width] + (data_in[x * width] << 1) + data_in[(x + 1) * width]);
    data_out[x * width] = 0;
    for (int y = 1; y < width - 1; y++)
    {
        data_grad2[x * width + y].y =
            Q5_10_TO_Q15(data_in[(x - 1) * width + y] + (data_in[x * width + y] << 1) + data_in[(x + 1) * width + y]);
        data_grad2[x * width + y].x =
            Q5_10_TO_Q15(data_in[x * width + (y - 1)] + (data_in[x * width + (y)] << 1) + data_in[x * width + (y + 1)]);
    }
    data_grad2[x * width + width - 1].y =
        Q5_10_TO_Q15(data_in[(x - 1) * width + width - 1] + (data_in[x * width + width - 1] << 1) +
                     data_in[(x + 1) * width + width - 1]);
    data_out[x * width + width - 1] = 0;
    x = 2;
    // first line
    arm_cv_compute_buffer_line_canny_sobel(imageIn, imageOut, data_grad1, data_grad2, data_mag, x);
    // core loop
    for (int x = 3; x < imageIn->height; x++)
    {
        arm_cv_compute_buffer_line_canny_sobel(imageIn, imageOut, data_grad1, data_grad2, data_mag, x);

        for (int y = 1; y < width - 1; y++)
        {
            int idx = (x - 2) * width + y;
            int mag = data_mag[((x - 2) % NB_LINE_BUF) * (width) + y];
            if (mag < low_threshold)
            {
                data_out[idx] = 0;
                continue;
            }
            else
            {
                q15_t angle;
                arm_atan2_q15(data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x,
                              data_grad1[((x - 2) % NB_LINE_BUF) * width + y].y, &angle);
                arm_abs_q15(&angle, &angle, 1);
                THRESHOLDING_HYSTERESIS(angle, high_threshold, width, data_mag, data_out, idx, mag, x, y)
            }
        }
    }
    // last line
    x = imageIn->height;
    data_out[(x - 1) * width] = 0;
    for (int y = 1; y < width - 1; y++)
    {
        int idx = (x - 2) * width + y;
        int mag = data_mag[((x - 2) % NB_LINE_BUF) * (width) + y];
        data_out[idx + width] = 0;
        if (mag < low_threshold)
        {
            data_out[idx] = 0;
            continue;
        }
        else
        {
            q15_t angle;
            arm_atan2_q15(data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x,
                          data_grad1[((x - 2) % NB_LINE_BUF) * width + y].y, &angle);
            if (data_grad1[((x - 2) % NB_LINE_BUF) * width + y].x == 0 &&
                data_grad1[((x - 2) % NB_LINE_BUF) * width + y].y == 0)
            {
                angle = 0;
            }
            arm_abs_q15(&angle, &angle, 1);
            THRESHOLDING_HYSTERESIS_BOTTOM_BORDER(angle, high_threshold, width, data_mag, data_out, idx, mag, x, y)
        }
    }
    data_out[(x - 1) * width + width - 1] = 0;
}
#endif