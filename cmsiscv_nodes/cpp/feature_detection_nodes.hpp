#pragma once


#include "cv/feature_detection.h"

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class CannyEdge;

template<int inputSize,int outputSize>
class CannyEdge<int8_t,inputSize,
                int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    CannyEdge(FIFOBase<int8_t> &src,
                FIFOBase<int8_t> &dst,
                uint32_t *params):
    GenericNode<int8_t,inputSize,int8_t,outputSize>(src,dst),
    mParams(params){
            
            cannyBuffer = (q15_t*)CG_MALLOC(arm_cv_get_scratch_size_canny_sobel(src.width()));

            mInitErrorOccured = false;
            if (cannyBuffer == nullptr)
            {
                mInitErrorOccured = true;
            }
    };

    ~CannyEdge()
    {
       CG_FREE(cannyBuffer);
    }

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

        if (mInitErrorOccured)
        {
            return(CG_MEMORY_ALLOCATION_FAILURE);
        }

        arm_cv_image_gray8_t input;
        arm_cv_image_gray8_t output;

        input.height=this->inputHeight();
        input.width=this->inputWidth();
        input.pData = (uint8_t*)i;

        output.height=this->inputHeight();
        output.width=this->inputWidth();
        output.pData = (uint8_t*)o;

        uint32_t low,high;
        if (mParams == nullptr)
        {
           low = 78;
           high = 33;
        }
        else
        {
           low = mParams[0];
           high = mParams[1];
        }
        arm_cv_canny_edge_sobel(&input, 
                                  &output, 
                                  cannyBuffer,
                                  low, 
                                  high);
       
        
        return(0);
    };
protected:
    q15_t* cannyBuffer;
    bool mInitErrorOccured;
    uint32_t *mParams;
};

