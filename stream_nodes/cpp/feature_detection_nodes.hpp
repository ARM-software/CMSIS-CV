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
                int w,int h,
                uint32_t *params):
    GenericNode<int8_t,inputSize,int8_t,outputSize>(src,dst),
    mW(w),mH(h),mParams(params){
            mInitErrorOccured = false;
            img_tmp_grad1.height=3;
            img_tmp_grad1.width=w;
            img_tmp_grad1.pData = (arm_cv_gradient_q15_t*) CG_MALLOC(3*w*sizeof(arm_cv_gradient_q15_t));
            if (img_tmp_grad1.pData == nullptr)
            {
                mInitErrorOccured = true;
            }

            img_tmp_grad2.height=3;
            img_tmp_grad2.width=w;
            img_tmp_grad2.pData = (arm_cv_gradient_q15_t*)CG_MALLOC(3*w*sizeof(arm_cv_gradient_q15_t));
            if (img_tmp_grad2.pData == nullptr)
            {
                mInitErrorOccured = true;
            }

            img_tmp.height=3;
            img_tmp.width=w;
            img_tmp.pData = (q15_t*)CG_MALLOC(3*w*sizeof(q15_t));

            if (img_tmp.pData == nullptr)
            {
                mInitErrorOccured = true;
            }
    };

    ~CannyEdge()
    {
       CG_FREE(img_tmp_grad1.pData);
       CG_FREE(img_tmp_grad2.pData);
       CG_FREE(img_tmp.pData);
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

        arm_cv_image_q15_t input;
        arm_cv_image_q15_t output;

        input.height=mH;
        input.width=mW;
        input.pData = (q15_t*)i;

        output.height=mH;
        output.width=mW;
        output.pData = (q15_t*)o;

        uint32_t low,high;
        if (mParams == nullptr)
        {
           low = 2500;
           high = 1060;
        }
        else
        {
           low = mParams[0];
           high = mParams[1];
        }
        arm_canny_edge_sobel_fixp(&input, 
                                  &output, 
                                  &img_tmp_grad1, 
                                  &img_tmp, 
                                  &img_tmp_grad2,
                                  low, 
                                  high);
       
        
        return(0);
    };
protected:
    int mW,mH;
    arm_cv_image_gradient_q15_t img_tmp_grad1;
    arm_cv_image_gradient_q15_t img_tmp_grad2;
    arm_cv_image_q15_t img_tmp;
    bool mInitErrorOccured;
    uint32_t *mParams;
};

