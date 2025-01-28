/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_linear_filter_generator_5.c
 * Description:  common code used by different linear filter functions
 *
 *
 * Target Processor: Cortex-M
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

#ifndef ARM_CV_LINEAR_FILTER_COMMON_SCRATCH_C
#define ARM_CV_LINEAR_FILTER_COMMON_SCRATCH_C
#include "arm_linear_filter_common.h"

#if ARM_CV_LINEAR_OUTPUT_TYPE == ARM_CV_LINEAR_OUTPUT_UINT_8
#define OUTPUT_TYPE uint8_t
#else
#define OUTPUT_TYPE q15_t
#endif

#if defined(KERNEL_5)
#define KERNEL_SIZE 5
#else
#if defined(KERNEL_7)
#define KERNEL_SIZE 7
#else
#define KERNEL_SIZE 3
#endif
#endif

#if defined(BUFFER_15)
#define BUFFER_TYPE q15_t
#define BUFFER_TYPE_DOUBLE_VECTOR int16x8x2_t

#else
#define BUFFER_TYPE q31_t
#define BUFFER_TYPE_DOUBLE_VECTOR int32x4x4_t

#endif

#if defined(KERNEL_5)

// This macro apply the horizontal kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_HORIZONTAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                        \
    dataOut[indexOut] = HORIZONTAL_COMPUTE_SCALAR(dataIn[indexIn + offset[0]], dataIn[indexIn + offset[1]],            \
                                                  dataIn[indexIn + offset[2]], dataIn[indexIn + offset[3]],            \
                                                  dataIn[indexIn + offset[4]]);

// This macro apply the vertical kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_VERTICAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                          \
    dataOut[indexOut] = VERTICAL_COMPUTE_SCALAR(                                                                       \
        dataIn[indexIn + offset[0] * width], dataIn[indexIn + offset[1] * width], dataIn[indexIn + offset[2] * width], \
        dataIn[indexIn + offset[3] * width], dataIn[indexIn + offset[4] * width]);

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

// This macro is a loop applying the vertical kernel on the middle part of a line from the input image  and storing the
// result into the temporary buffer
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset)                                                \
    for (int y = 0; y < width - 15; y += 16)                                                                           \
    {                                                                                                                  \
        uint8x16_t vec1 = vld1q(&dataIn[line * width + y + offset[0] * width]);                                        \
        uint8x16_t vec2 = vld1q(&dataIn[line * width + y + offset[1] * width]);                                        \
        uint8x16_t vec3 = vld1q(&dataIn[line * width + y + offset[2] * width]);                                        \
        uint8x16_t vec4 = vld1q(&dataIn[line * width + y + offset[3] * width]);                                        \
        uint8x16_t vec5 = vld1q(&dataIn[line * width + y + offset[4] * width]);                                        \
        BUFFER_TYPE_DOUBLE_VECTOR vect_res;                                                                            \
        VERTICAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vect_res);                                               \
        vst2q(&scratch[y], vect_res);                                                                                  \
    }                                                                                                                  \
    for (int y = width - (width % 16); y < width; y++)                                                                 \
    {                                                                                                                  \
        _ARM_VERTICAL_ATTRIBUTION(y, line *width + y, offset, scratch, dataIn)                                         \
    }

#if (ARM_CV_LINEAR_OUTPUT_TYPE == ARM_CV_LINEAR_OUTPUT_UINT_8)

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 2; y < width - 16; y += 16)                                                                           \
    {                                                                                                                  \
        BUFFER_TYPE_DOUBLE_VECTOR vec1 = vld2q(&scratch[y + offset[0]]);                                               \
        BUFFER_TYPE_DOUBLE_VECTOR vec2 = vld2q(&scratch[y + offset[1]]);                                               \
        BUFFER_TYPE_DOUBLE_VECTOR vec3 = vld2q(&scratch[y + offset[2]]);                                               \
        BUFFER_TYPE_DOUBLE_VECTOR vec4 = vld2q(&scratch[y + offset[3]]);                                               \
        BUFFER_TYPE_DOUBLE_VECTOR vec5 = vld2q(&scratch[y + offset[4]]);                                               \
        int8x16_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vect_out)                                              \
        vst1q((int8_t *)&dataOut[line * width + y], vect_out);                                                         \
    }                                                                                                                  \
    for (int y = width - ((width - 2) % 16); y < width - 2; y++)                                                       \
    {                                                                                                                  \
        _ARM_HORIZONTAL_ATTRIBUTION(line *width + y, y, offset, dataOut, scratch)                                      \
    }

#else // ARM_CV_LINEAR_OUTPUT_UINT_8
// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 2; y < width - 8; y += 8)                                                                             \
    {                                                                                                                  \
        uint16x8_t vec1 = vld1q(&scratch[y + offset[0]]);                                                              \
        uint16x8_t vec2 = vld1q(&scratch[y + offset[1]]);                                                              \
        uint16x8_t vec3 = vld1q(&scratch[y + offset[2]]);                                                              \
        uint16x8_t vec4 = vld1q(&scratch[y + offset[3]]);                                                              \
        uint16x8_t vec5 = vld1q(&scratch[y + offset[4]]);                                                              \
        int16x8_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vect_out)                                              \
        vst1q(&dataOut[line * width + y], vect_out);                                                                   \
    }                                                                                                                  \
    for (int y = width - ((width - 1) % 8); y < width - 2; y++)                                                        \
    {                                                                                                                  \
        HORIZONTAL_ATTRIBUTION(line *width, y, offset, dataOut, scratch)                                               \
    }

#endif // ARM_CV_LINEAR_OUTPUT_UINT_8
#endif // ARM_MATH_MVEI
#else // KERNEL_5

#if defined(KERNEL_7)

// This macro apply the horizontal kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_HORIZONTAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                        \
    dataOut[indexOut] = HORIZONTAL_COMPUTE_SCALAR(dataIn[indexIn + offset[0]], dataIn[indexIn + offset[1]],            \
                                                  dataIn[indexIn + offset[2]], dataIn[indexIn + offset[3]],            \
                                                  dataIn[indexIn + offset[4]], dataIn[indexIn + offset[5]],            \
                                                  dataIn[indexIn + offset[6]]);

// This macro apply the vertical kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_VERTICAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                          \
    dataOut[indexOut] = VERTICAL_COMPUTE_SCALAR(                                                                       \
        dataIn[indexIn + offset[0] * width], dataIn[indexIn + offset[1] * width], dataIn[indexIn + offset[2] * width], \
        dataIn[indexIn + offset[3] * width], dataIn[indexIn + offset[4] * width], dataIn[indexIn + offset[5] * width], \
        dataIn[indexIn + offset[6] * width]);

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
#if defined(BUFFER_15)

// This macro is a loop applying the vertical kernel on the middle part of a line from the input image  and storing the
// result into the temporary buffer
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset)                                                \
    for (int y = 0; y < width - 15; y += 16)                                                                           \
    {                                                                                                                  \
        uint8x16_t vec1 = vld1q(&dataIn[line * width + y + offset[0] * width]);                                        \
        uint8x16_t vec2 = vld1q(&dataIn[line * width + y + offset[1] * width]);                                        \
        uint8x16_t vec3 = vld1q(&dataIn[line * width + y + offset[2] * width]);                                        \
        uint8x16_t vec4 = vld1q(&dataIn[line * width + y + offset[3] * width]);                                        \
        uint8x16_t vec5 = vld1q(&dataIn[line * width + y + offset[4] * width]);                                        \
        uint8x16_t vec6 = vld1q(&dataIn[line * width + y + offset[5] * width]);                                        \
        uint8x16_t vec7 = vld1q(&dataIn[line * width + y + offset[6] * width]);                                        \
        BUFFER_TYPE_DOUBLE_VECTOR vect_res;                                                                            \
        VERTICAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vec6, vec7, vect_res);                                   \
        vst2q(&scratch[y], vect_res);                                                                                  \
    }                                                                                                                  \
    for (int y = width - (width % 16); y < width; y++)                                                                 \
    {                                                                                                                  \
        _ARM_VERTICAL_ATTRIBUTION(y, line *width + y, offset, scratch, dataIn)                                         \
    }

#else // BUFFER_15

// This macro is a loop applying the vertical kernel on the middle part of a line from the input image  and storing the
// result into the temporary buffer
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
//  offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
//  type
#define _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset)                                                \
    for (int y = 0; y < width - 15; y += 16)                                                                           \
    {                                                                                                                  \
        uint8x16_t vec1 = vld1q(&dataIn[line * width + y + offset[0] * width]);                                        \
        uint8x16_t vec2 = vld1q(&dataIn[line * width + y + offset[1] * width]);                                        \
        uint8x16_t vec3 = vld1q(&dataIn[line * width + y + offset[2] * width]);                                        \
        uint8x16_t vec4 = vld1q(&dataIn[line * width + y + offset[3] * width]);                                        \
        uint8x16_t vec5 = vld1q(&dataIn[line * width + y + offset[4] * width]);                                        \
        uint8x16_t vec6 = vld1q(&dataIn[line * width + y + offset[5] * width]);                                        \
        uint8x16_t vec7 = vld1q(&dataIn[line * width + y + offset[6] * width]);                                        \
        BUFFER_TYPE_DOUBLE_VECTOR vect_res;                                                                            \
        VERTICAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vec6, vec7, vect_res);                                   \
        vst4q(&scratch[y], vect_res);                                                                                  \
    }                                                                                                                  \
    for (int y = width - (width % 16); y < width; y++)                                                                 \
    {                                                                                                                  \
        _ARM_VERTICAL_ATTRIBUTION(y, line *width + y, offset, scratch, dataIn)                                         \
    }

#endif // BUFFER_15

#if (ARM_CV_LINEAR_OUTPUT_TYPE == ARM_CV_LINEAR_OUTPUT_UINT_8)

#if defined(BUFFER_15)

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 3; y < width - 16; y += 16)                                                                           \
    {                                                                                                                  \
        int16x8x2_t vec1 = vld2q(&scratch[y + offset[0]]);                                                             \
        int16x8x2_t vec2 = vld2q(&scratch[y + offset[1]]);                                                             \
        int16x8x2_t vec3 = vld2q(&scratch[y + offset[2]]);                                                             \
        int16x8x2_t vec4 = vld2q(&scratch[y + offset[3]]);                                                             \
        int16x8x2_t vec5 = vld2q(&scratch[y + offset[4]]);                                                             \
        int16x8x2_t vec6 = vld2q(&scratch[y + offset[5]]);                                                             \
        int16x8x2_t vec7 = vld2q(&scratch[y + offset[6]]);                                                             \
        int8x16_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vec6, vec7, vect_out)                                  \
        vst1q((int8_t *)&dataOut[line * width + y], vect_out);                                                         \
    }                                                                                                                  \
    for (int y = width - ((width - 3) % 16); y < width - 3; y++)                                                       \
    {                                                                                                                  \
        _ARM_HORIZONTAL_ATTRIBUTION(line *width + y, y, offset, dataOut, scratch)                                      \
    }
#else // BUFFER_15

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 3; y < width - 16; y += 16)                                                                           \
    {                                                                                                                  \
        int32x4x4_t vec1 = vld4q(&scratch[y + offset[0]]);                                                             \
        int32x4x4_t vec2 = vld4q(&scratch[y + offset[1]]);                                                             \
        int32x4x4_t vec3 = vld4q(&scratch[y + offset[2]]);                                                             \
        int32x4x4_t vec4 = vld4q(&scratch[y + offset[3]]);                                                             \
        int32x4x4_t vec5 = vld4q(&scratch[y + offset[4]]);                                                             \
        int32x4x4_t vec6 = vld4q(&scratch[y + offset[5]]);                                                             \
        int32x4x4_t vec7 = vld4q(&scratch[y + offset[6]]);                                                             \
        int8x16_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vec6, vec7, vect_out)                                  \
        vst1q((int8_t *)&dataOut[line * width + y], vect_out);                                                         \
    }                                                                                                                  \
    for (int y = width - ((width - 3) % 16); y < width - 3; y++)                                                       \
    {                                                                                                                  \
        _ARM_HORIZONTAL_ATTRIBUTION(line *width + y, y, offset, dataOut, scratch)                                      \
    }
#endif // BUFFER_15

#else  // ARM_CV_LINEAR_OUTPUT_UINT_8

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 3; y < width - 8; y += 8)                                                                             \
    {                                                                                                                  \
        q15x8_t vec1 = vld1q(&scratch[y + offset[0]]);                                                                 \
        q15x8_t vec2 = vld1q(&scratch[y + offset[1]]);                                                                 \
        q15x8_t vec3 = vld1q(&scratch[y + offset[2]]);                                                                 \
        q15x8_t vec4 = vld1q(&scratch[y + offset[3]]);                                                                 \
        q15x8_t vec5 = vld1q(&scratch[y + offset[4]]);                                                                 \
        q15x8_t vec6 = vld1q(&scratch[y + offset[5]]);                                                                 \
        q15x8_t vec7 = vld1q(&scratch[y + offset[6]]);                                                                 \
        q15x8_t vect_out;                                                                                              \
        _ARM_HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vec4, vec5, vec6, vec7, vect_out)                             \
        vst1q(&dataOut[line * width + y], vect_out);                                                                   \
    }                                                                                                                  \
    for (int y = width - ((width - 3) % 8); y < width - 3; y++)                                                        \
    {                                                                                                                  \
        _ARM_HORIZONTAL_ATTRIBUTION(line *width, y, offset, dataOut, scratch)                                          \
    }

#endif // ARM_CV_LINEAR_OUTPUT_UINT_8
#endif // ARM_MATH_MVEI
#else  // KERNEL_7

// This macro apply the horizontal kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_HORIZONTAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                        \
    dataOut[indexOut] = HORIZONTAL_COMPUTE_SCALAR(dataIn[indexIn + offset[0]], dataIn[indexIn + offset[1]],            \
                                                  dataIn[indexIn + offset[2]]);

// This macro apply the vertical kernel on the pixel respecting the offset for the corresponding border type
// indexOut Index to find the place to store the output value
// indexIn Index where to find the value of the pixel treated
// offset List of offset to apply to the index of the treated pixel to found the other pixel involved in the compute
// dataOut Pointer to the memory where the output value will have to be store
// dataIn Pointer to the memory where the input value is stored
#define _ARM_VERTICAL_ATTRIBUTION(indexOut, indexIn, offset, dataOut, dataIn)                                          \
    dataOut[indexOut] =                                                                                                \
        VERTICAL_COMPUTE_SCALAR(dataIn[indexIn + offset[0] * width], dataIn[indexIn + offset[1] * width],              \
                                dataIn[indexIn + offset[2] * width]);

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
// This macro is a loop applying the vertical kernel on the middle part of a line from the input image  and storing the
// result into the temporary buffer
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset)                                                \
    for (int y = 0; y < width - 15; y += 16)                                                                           \
    {                                                                                                                  \
        uint8x16_t vec1 = vld1q(&dataIn[line * width + y + offset[0] * width]);                                        \
        uint8x16_t vec2 = vld1q(&dataIn[line * width + y + offset[1] * width]);                                        \
        uint8x16_t vec3 = vld1q(&dataIn[line * width + y + offset[2] * width]);                                        \
        int16x8x2_t vect_res;                                                                                          \
        VERTICAL_COMPUTE_VECTOR(vec1, vec2, vec3, vect_res);                                                           \
        vst2q(&scratch[y], vect_res);                                                                                  \
    }                                                                                                                  \
    for (int y = width - (width % 16); y < width; y++)                                                                 \
    {                                                                                                                  \
        scratch[y] = VERTICAL_COMPUTE_SCALAR(dataIn[line * width + y + offset[0] * width],                             \
                                             dataIn[line * width + y + offset[1] * width],                             \
                                             dataIn[line * width + y + offset[2] * width]);                            \
    }

#if (ARM_CV_LINEAR_OUTPUT_TYPE == ARM_CV_LINEAR_OUTPUT_UINT_8)

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 1; y < width - 16; y += 16)                                                                           \
    {                                                                                                                  \
        int16x8x2_t vec1 = vld2q(&scratch[y + offset[0]]);                                                             \
        int16x8x2_t vec2 = vld2q(&scratch[y + offset[1]]);                                                             \
        int16x8x2_t vec3 = vld2q(&scratch[y + offset[2]]);                                                             \
        int8x16_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vect_out)                                                          \
        vst1q((int8_t *)&dataOut[line * width + y], vect_out);                                                         \
    }                                                                                                                  \
    for (int y = width - ((width - 1) % 16); y < width - 1; y++)                                                       \
    {                                                                                                                  \
        dataOut[line * width + y] =                                                                                    \
            HORIZONTAL_COMPUTE_SCALAR(scratch[y + offset[0]], scratch[y + offset[1]], scratch[y + offset[2]]);         \
    }

// output q15
#else // ARM_CV_LINEAR_OUTPUT_UINT_8

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = 1; y < width - 8; y += 8)                                                                             \
    {                                                                                                                  \
        int16x8_t vec1 = vld1q(&scratch[y + offset[0]]);                                                               \
        int16x8_t vec2 = vld1q(&scratch[y + offset[1]]);                                                               \
        int16x8_t vec3 = vld1q(&scratch[y + offset[2]]);                                                               \
        int16x8_t vect_out;                                                                                            \
        HORIZONTAL_COMPUTE_VECTOR(vec1, vec2, vec3, vect_out)                                                          \
        vst1q(&dataOut[line * width + y], vect_out);                                                                   \
    }                                                                                                                  \
    for (int y = width - ((width - 1) % 8); y < width - 1; y++)                                                        \
    {                                                                                                                  \
        dataOut[line * width + y] =                                                                                    \
            HORIZONTAL_COMPUTE_SCALAR(scratch[y + offset[0]], scratch[y + offset[1]], scratch[y + offset[2]]);         \
    }

#endif // ARM_CV_LINEAR_OUTPUT_UINT_8
#endif
#endif // KERNEL_7

#endif // KERNEL_5

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
#else // ARM_MATH_MVEI

// This macro is a loop applying the horizontal kernel on the middle part of a line from the temporary buffer and
// storing the result into the output image
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset)                                              \
    for (int y = KERNEL_SIZE >> 1; y < width - (KERNEL_SIZE >> 1); y++)                                                \
    {                                                                                                                  \
        _ARM_HORIZONTAL_ATTRIBUTION(line *width + y, y, offset, dataOut, scratch);                                     \
    }
// This macro is a loop applying the vertical kernel on the middle part of a line from the input image  and storing the
// result into the temporary buffer
//  width Width of the image
//  scratch Temporary buffer
//  dataOut Pointer to the memory containing the pixel of the output image
//  line Index of the treated line
// offset List containing the offset to apply to read the correct pixel around the treated pixel, depend of the bordure
// type
#define _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset)                                                \
    for (int y = 0; y < width; y++)                                                                                    \
    {                                                                                                                  \
        _ARM_VERTICAL_ATTRIBUTION(y, line *width + y, offset, scratch, dataIn);                                        \
    }

#endif // ARM_MATH_MVEI

// This function call macro in order to process lines of the image
//  borderLocation give the information on if the line is on the border of the image and need to apply special rules
//  regarding the kernel depending on the border type width Width of the targeted image scratch Buffer for storing
//  intermediate values dataIn Pointer to the memory containing the pixels values of the input image dataOut Pointer to
//  the memory containing the pixels values of the output image offset Small buffer of the size of one dimension of the
//  kernel to store the offset require to respect the border type borderType Define the border type to use line Index of
//  the line that will be treated by the function height Height of the image
__STATIC_INLINE void line_processing_linear(const int8_t borderLocation, const int16_t width, BUFFER_TYPE *scratch,
                                            const uint8_t *dataIn, OUTPUT_TYPE *dataOut, int *offset,
                                            const uint8_t borderType, const uint8_t line, const uint8_t height)
{
    _ARM_BORDER_OFFSET(offset, borderLocation, height, borderType);
    _ARM_LOOP_INPUT_TO_BUFFER(width, scratch, dataIn, line, offset);
    for (int y = 0; y < KERNEL_SIZE >> 1; y++)
    {
        _ARM_BORDER_OFFSET(offset, y, width, borderType);
        _ARM_HORIZONTAL_ATTRIBUTION(line * width + y, y, offset, dataOut, scratch);
    }
    _ARM_BORDER_OFFSET(offset, KERNEL_SIZE >> 1, width, borderType);

    _ARM_LOOP_BUFFER_TO_OUTPUT(width, scratch, dataOut, line, offset);
    for (int y = (KERNEL_SIZE >> 1) + 1; y < KERNEL_SIZE; y++)
    {
        _ARM_BORDER_OFFSET(offset, y, width, borderType);
        _ARM_HORIZONTAL_ATTRIBUTION(line * width + width - KERNEL_SIZE + y, width - KERNEL_SIZE + y, offset, dataOut,
                                    scratch);
    }
}

// This macro process the image line by line
//  imageIn is the pointer to the struct containing the input image
//  imageOut is the pointer to the struct containing the output image
//  scratch is the pointer to the temporary buffer used to store intermediate values
//  borderType define the type of bordure that will by use by the computing steps
#define _ARM_LINEAR_GENERIC(imageIn, imageOut, scratch, borderType)                                                    \
    int width = imageOut->width;                                                                                       \
    int height = imageOut->height;                                                                                     \
    uint8_t *dataIn = imageIn->pData;                                                                                  \
    OUTPUT_TYPE *dataOut = imageOut->pData;                                                                            \
    int offset[KERNEL_SIZE];                                                                                           \
    for (int y = 0; y < KERNEL_SIZE >> 1; y++)                                                                         \
    {                                                                                                                  \
        line_processing_linear(y, width, scratch, dataIn, dataOut, offset, borderType, y, height);                     \
    }                                                                                                                  \
    for (int x = KERNEL_SIZE >> 1; x < height - (KERNEL_SIZE >> 1); x++)                                               \
    {                                                                                                                  \
        line_processing_linear((KERNEL_SIZE >> 1), width, scratch, dataIn, dataOut, offset, borderType, x, height);    \
    }                                                                                                                  \
    for (int y = (KERNEL_SIZE >> 1) + 1; y < KERNEL_SIZE; y++)                                                         \
    {                                                                                                                  \
        int x = height - KERNEL_SIZE + y;                                                                              \
        line_processing_linear(y, width, scratch, dataIn, dataOut, offset, borderType, x, height);                     \
    }
#endif
