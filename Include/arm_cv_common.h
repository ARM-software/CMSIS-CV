/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_cv_commonh.h
 * Description:  Common declarations for CMSIS-CV
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
#ifndef ARM_CV_COMMON_H
#define ARM_CV_COMMON_H

#include "arm_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

#if (defined(ARM_MATH_MVEI) || defined(ARM_MATH_MVEF)) && !defined(ARM_MATH_AUTOVECTORIZE)

/*

Used to place some very small working memory into
fast memory (like TCM when available).
This memory is used for scatter / gather instructions on Helium.

*/
#ifndef CV_FAST_MEMORY 
#define CV_FAST_MEMORY
#endif

CV_FAST_MEMORY
extern int8_t cv_sgbuf[16*3];

#endif


#ifdef   __cplusplus
}
#endif

#endif

