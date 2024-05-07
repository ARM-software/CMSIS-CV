#pragma once

#include "cv/linear_filters.h"


template<typename IN, int inputSize,
         typename OUT,int outputSize>
class GaussianFilter;

template<int inputSize,int outputSize>
class GaussianFilter<int8_t,inputSize,
                int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    GaussianFilter(FIFOBase<int8_t> &src,
                FIFOBase<int8_t> &dst):
    GenericNode<int8_t,inputSize,int8_t,outputSize>(src,dst){};

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
        int8_t *i=this->getReadBuffer();
        int8_t *o=this->getWriteBuffer();

        arm_cv_image_gray8_t src;
        arm_cv_image_q15_t dst;

        src.height=this->inputHeight();
        src.width=this->inputWidth();
        src.pData = (uint8_t*)i;

        dst.height=this->inputHeight();
        dst.width=this->inputWidth();
        dst.pData = (q15_t*)o;
        
        arm_gaussian_filter_3x3_fixp(&src,&dst);

        
        return(0);
    };
   
};