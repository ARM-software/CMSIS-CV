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
#ifndef ARM_CV_LINEAR_FILTER_COMMON_H
#define ARM_CV_LINEAR_FILTER_COMMON_H

#include "cv/linear_filters.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define ARM_CV_LEFT_TOP 0
#define ARM_CV_MIDDLE 1
#define ARM_CV_RIGHT_BOT 2

#define ARM_CV_LEFT_TOP_5 0
#define ARM_CV_LEFT_TOP_MIDDLE_5 1
#define ARM_CV_MIDDLE_5 2
#define ARM_CV_RIGHT_BOT_MIDDLE_5 3
#define ARM_CV_RIGHT_BOT_5 4

#define ARM_CV_LEFT_TOP_BORDER_7 0
#define ARM_CV_LEFT_TOP_7 1
#define ARM_CV_LEFT_TOP_MIDDLE_7 2
#define ARM_CV_MIDDLE_7 3
#define ARM_CV_RIGHT_BOT_MIDDLE_7 4
#define ARM_CV_RIGHT_BOT_7 5
#define ARM_CV_RIGHT_BOT_BORDER_7 6

#define ARM_CV_LINEAR_OUTPUT_UINT_8 1
#define ARM_CV_LINEAR_OUTPUT_Q15 2

#ifndef KERNEL_5
#ifndef KERNEL_7
// Give the offset for the border case Replicate
#define _ARM_BORDER_OFFSET_REPLICATE(list, position)                                                                   \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE:                                                                                                \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT:                                                                                             \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = 0;                                                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = 0;                                                                                                   \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Wrap
#define _ARM_BORDER_OFFSET_WRAP(list, position, dim)                                                                   \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE:                                                                                                \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT:                                                                                             \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = -dim + 1;                                                                                            \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = dim - 1;                                                                                             \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Reflect
#define _ARM_BORDER_OFFSET_REFLECT(list, position)                                                                     \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE:                                                                                                \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT:                                                                                             \
        list[0] = -1;                                                                                                  \
        list[1] = 0;                                                                                                   \
        list[2] = -1;                                                                                                  \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = 1;                                                                                                   \
        list[1] = 0;                                                                                                   \
        list[2] = 1;                                                                                                   \
        break;                                                                                                         \
    }

// This macro give the offset depending on the border type specified
#define _ARM_BORDER_OFFSET(list, position, dim, border_type)                                                           \
    switch (border_type)                                                                                               \
    {                                                                                                                  \
    case ARM_CV_BORDER_NEAREST:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REPLICATE(list, position)                                                                   \
        break;                                                                                                         \
    case ARM_CV_BORDER_WRAP:                                                                                           \
        _ARM_BORDER_OFFSET_WRAP(list, position, dim)                                                                   \
        break;                                                                                                         \
    case ARM_CV_BORDER_REFLECT:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REFLECT(list, position)                                                                     \
        break;                                                                                                         \
    default:                                                                                                           \
        break;                                                                                                         \
    }
#else
// Give the offset for the border case Replicate
#define _ARM_BORDER_OFFSET_REPLICATE_7(list, position)                                                                 \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_7:                                                                                              \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_7:                                                                                           \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 1;                                                                                                   \
        list[6] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 2;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_7:                                                                                            \
        list[0] = -1;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_BORDER_7:                                                                                     \
        list[0] = 0;                                                                                                   \
        list[1] = 0;                                                                                                   \
        list[2] = 0;                                                                                                   \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_BORDER_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 0;                                                                                                   \
        list[5] = 0;                                                                                                   \
        list[6] = 0;                                                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = -2;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Wrap
#define _ARM_BORDER_OFFSET_WRAP_7(list, position, dim)                                                                 \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_7:                                                                                              \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_7:                                                                                           \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = -dim + 2;                                                                                            \
        list[6] = -dim + 3;                                                                                            \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = -dim + 3;                                                                                            \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_7:                                                                                            \
        list[0] = dim - 3;                                                                                             \
        list[1] = dim - 2;                                                                                             \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_BORDER_7:                                                                                     \
        list[0] = dim - 3;                                                                                             \
        list[1] = dim - 2;                                                                                             \
        list[2] = dim - 1;                                                                                             \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_BORDER_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = -dim + 1;                                                                                            \
        list[5] = -dim + 2;                                                                                            \
        list[6] = -dim + 3;                                                                                            \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = dim - 3;                                                                                             \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Reflect
#define _ARM_BORDER_OFFSET_REFLECT_7(list, position)                                                                   \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_7:                                                                                              \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_7:                                                                                           \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 0;                                                                                                   \
        list[6] = -1;                                                                                                  \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_7:                                                                                            \
        list[0] = 1;                                                                                                   \
        list[1] = 0;                                                                                                   \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_BORDER_7:                                                                                     \
        list[0] = 3;                                                                                                   \
        list[1] = 2;                                                                                                   \
        list[2] = 1;                                                                                                   \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_BORDER_7:                                                                                    \
        list[0] = -3;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = -1;                                                                                                  \
        list[5] = -2;                                                                                                  \
        list[6] = -3;                                                                                                  \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = -1;                                                                                                  \
        list[1] = -2;                                                                                                  \
        list[2] = -1;                                                                                                  \
        list[3] = 0;                                                                                                   \
        list[4] = 1;                                                                                                   \
        list[5] = 2;                                                                                                   \
        list[6] = 3;                                                                                                   \
        break;                                                                                                         \
    }

// This macro give the offset depending on the border type specified
#define _ARM_BORDER_OFFSET(list, position, dim, border_type)                                                           \
    switch (border_type)                                                                                               \
    {                                                                                                                  \
    case ARM_CV_BORDER_NEAREST:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REPLICATE_7(list, position)                                                                 \
        break;                                                                                                         \
    case ARM_CV_BORDER_WRAP:                                                                                           \
        _ARM_BORDER_OFFSET_WRAP_7(list, position, dim)                                                                 \
        break;                                                                                                         \
    case ARM_CV_BORDER_REFLECT:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REFLECT_7(list, position)                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        break;                                                                                                         \
    }

#endif
#else
// Give the offset for the border case Replicate
#define _ARM_BORDER_OFFSET_REPLICATE_5(list, position)                                                                 \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_5:                                                                                              \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_5:                                                                                           \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 0;                                                                                                   \
        list[4] = 0;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_5:                                                                                    \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 1;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_5:                                                                                            \
        list[0] = 0;                                                                                                   \
        list[1] = 0;                                                                                                   \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = -1;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Wrap
#define _ARM_BORDER_OFFSET_WRAP_5(list, position, dim)                                                                 \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_5:                                                                                              \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_5:                                                                                           \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = -dim + 1;                                                                                            \
        list[4] = -dim + 2;                                                                                            \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_5:                                                                                    \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = -dim + 2;                                                                                            \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_5:                                                                                            \
        list[0] = dim - 2;                                                                                             \
        list[1] = dim - 1;                                                                                             \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = dim - 2;                                                                                             \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    }

// Give the offset for the border case Reflect
#define _ARM_BORDER_OFFSET_REFLECT_5(list, position)                                                                   \
    switch (position)                                                                                                  \
    {                                                                                                                  \
    case ARM_CV_MIDDLE_5:                                                                                              \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_5:                                                                                           \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = -1;                                                                                                  \
        list[4] = -2;                                                                                                  \
        break;                                                                                                         \
    case ARM_CV_RIGHT_BOT_MIDDLE_5:                                                                                    \
        list[0] = -2;                                                                                                  \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 0;                                                                                                   \
        break;                                                                                                         \
    case ARM_CV_LEFT_TOP_5:                                                                                            \
        list[0] = 2;                                                                                                   \
        list[1] = 1;                                                                                                   \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        list[0] = 0;                                                                                                   \
        list[1] = -1;                                                                                                  \
        list[2] = 0;                                                                                                   \
        list[3] = 1;                                                                                                   \
        list[4] = 2;                                                                                                   \
        break;                                                                                                         \
    }

// This macro give the offset depending on the border type specified
#define _ARM_BORDER_OFFSET(list, position, dim, border_type)                                                           \
    switch (border_type)                                                                                               \
    {                                                                                                                  \
    case ARM_CV_BORDER_NEAREST:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REPLICATE_5(list, position)                                                                 \
        break;                                                                                                         \
    case ARM_CV_BORDER_WRAP:                                                                                           \
        _ARM_BORDER_OFFSET_WRAP_5(list, position, dim)                                                                 \
        break;                                                                                                         \
    case ARM_CV_BORDER_REFLECT:                                                                                        \
        (void)dim;                                                                                                     \
        _ARM_BORDER_OFFSET_REFLECT_5(list, position)                                                                   \
        break;                                                                                                         \
    default:                                                                                                           \
        break;                                                                                                         \
    }

#endif
#ifdef __cplusplus
}
#endif

#endif