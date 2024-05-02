/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        AppNodes.h
 * Description:  Application nodes for Example simple
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
*
 * Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
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
#ifndef _APPNODES_H_
#define _APPNODES_H_

#include <string>

#include "arm_math_types.h"


using namespace arm_cmsis_stream;

#include "color_transform_nodes.hpp"
#include "feature_detection_nodes.hpp"
#include "image_transform_nodes.hpp"
#include "linear_filter_nodes.hpp"

#include "opencv_nodes/canny_edge/node.hpp"
#include "opencv_nodes/gaussian/node.hpp"

template<typename IN1,int inputSize1,typename IN2,int inputSize2>
class GenericSink2:public NodeBase
{
public:
     explicit GenericSink2(FIFOBase<IN1> &src1,FIFOBase<IN2> &src2):mSrc1(src1),mSrc2(src2){};

protected:
     IN1 * getReadBuffer1(int nb = inputSize1) {return mSrc1.getReadBuffer(nb);};
     bool willUnderflow1(int nb = inputSize1) const {return mSrc1.willUnderflowWith(nb);};

     IN2 * getReadBuffer2(int nb = inputSize2) {return mSrc2.getReadBuffer(nb);};
     bool willUnderflow2(int nb = inputSize2) const {return mSrc2.willUnderflowWith(nb);};

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;

};

template<typename IN, int inputSize>
class WebDisplay;

template<int inputSize>
class WebDisplay<int8_t, inputSize>: public GenericSink<int8_t, inputSize>
{
public:
    // The constructor for the sink is only using
    // the input FIFO (coming from the generated scheduler).
    // This FIFO is passed to the GenericSink contructor.
    // Implementation of this Sink constructor is doing nothing
    WebDisplay(FIFOBase<int8_t> &src,uint32_t w,uint32_t h):
    GenericSink<int8_t,inputSize>(src),mW(w),mH(h){};

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

   
    int run() final
    {
        int8_t *b=this->getReadBuffer();
        
        return(0);
    };
protected:
     uint32_t mW,mH;
};

template<typename OUT,int outputSize>
class WebCamera;

template<int outputSize>
class WebCamera<int8_t, outputSize>: public GenericSource<int8_t,outputSize>
{
public:
    WebCamera(FIFOBase<int8_t> &dst,uint32_t w,uint32_t h):
    GenericSource<int8_t,outputSize>(dst),mW(w),mH(h){};

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        if (!g_frame_available)
        {
            return(CG_PAUSED_SCHEDULER);
        }
        g_frame_available=0;
        int8_t *b=this->getWriteBuffer();

        return(0);
    };
protected:
     uint32_t mW,mH;
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Copy;

template<typename IN, int ioSize>
class Copy<IN,ioSize,
                         IN,ioSize>:public GenericNode<IN,ioSize,
                         IN,ioSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Copy(FIFOBase<IN> &src,
             FIFOBase<IN> &dst,uint32_t w,uint32_t h):
    GenericNode<IN,ioSize,
                IN,ioSize>(src,dst),mW(w),mH(h){};

    /* In asynchronous mode, node execution will be 
       skipped in case of underflow on the input 
       or overflow in the output.
    */
    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    /* 
       Node processing
       1 is added to the input
    */
    int run() final{
        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();

        memcpy(b,a,sizeof(IN)*mW*mH);

        return(0);
    };
protected:
     uint32_t mW,mH;
};
#endif