/* ----------------------------------------------------------------------
 * Project:      CMSIS CV Library
 * Title:        arm_cv.h
 * Description:  General header from CMSIS-CV
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
#ifndef ARM_CV_H_ 
#define ARM_CV_H_ 

#include "arm_cv_types.h"

/**
 * @defgroup colorTransform Color Transformations
 */
#include "cv/color_transforms.h"


/**
 * @defgroup linearFilter Linear Filters
 */
#include "cv/linear_filters.h"

/**
 * @defgroup featureDetection Feature Detection
 */
#include "cv/feature_detection.h"


#endif 
