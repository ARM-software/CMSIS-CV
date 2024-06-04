#pragma once

#include "cv/image_transforms.h"

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray8Cropping;


template<int inputSize,int outputSize>
class Gray8Cropping<int8_t,inputSize,
                    int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray8Cropping(FIFOBase<int8_t> &src,
                  FIFOBase<int8_t> &dst,
                  uint32_t left,
                  uint32_t top,
                  uint32_t right,
                  uint32_t bottom):
    GenericNode<int8_t,inputSize,int8_t,outputSize>(src,dst),
    mLeft(left),
    mTop(top),
    mRight(right),
    mBottom(bottom){};

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
    
   
    int run() final{
        uint8_t *gin=(uint8_t*)this->getReadBuffer();
        uint8_t *gout=(uint8_t*)this->getWriteBuffer();

        arm_cv_image_gray8_t input;
        arm_cv_image_gray8_t output;

        input.width = this->inputWidth();
        input.height = this->inputHeight();
        input.pData = gin;

        output.width = this->outputWidth();
        output.height = this->outputHeight();
        output.pData = gout;

        arm_crop_gray8(&input,&output,mLeft,mTop,mRight,mBottom);     
      
        
        return(0);
    };

protected:
    uint32_t mLeft;
    uint32_t mTop;
    uint32_t mRight;
    uint32_t mBottom;
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class RGB24Cropping;


template<int inputSize,int outputSize>
class RGB24Cropping<int8_t,inputSize,
                    int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    RGB24Cropping(FIFOBase<int8_t> &src,
                  FIFOBase<int8_t> &dst,
                  uint32_t left,
                  uint32_t top,
                  uint32_t right,
                  uint32_t bottom):
    GenericNode<int8_t,inputSize,int8_t,outputSize>(src,dst),
    mLeft(left),
    mTop(top),
    mRight(right),
    mBottom(bottom){};

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
  
    int run() final{
        uint8_t *gin=(uint8_t*)this->getReadBuffer();
        uint8_t *gout=(uint8_t*)this->getWriteBuffer();

        arm_cv_image_rgb24_t input;
        arm_cv_image_rgb24_t output;

        input.width = this->inputWidth();
        input.height = this->inputHeight();
        input.pData = gin;

        output.width = this->outputWidth();
        output.height = this->outputHeight();
        output.pData = gout;

        arm_crop_rgb24(&input,&output,mLeft,mTop,mRight,mBottom);     
      
        
        return(0);
    };

protected:
    uint32_t mLeft;
    uint32_t mTop;
    uint32_t mRight;
    uint32_t mBottom;
};