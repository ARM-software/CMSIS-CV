#pragma once

#include "cv/color_transforms.h"

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class YUV420ToGray16;


template<int inputSize,int outputSize>
class YUV420ToGray16<int8_t,inputSize,
                  int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    YUV420ToGray16(FIFOBase<int8_t> &src,
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
        int8_t *yuv=this->getReadBuffer();
        int8_t *d=this->getWriteBuffer();

        uint16_t *gray = (uint16_t*)d;

        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           *gray++ = ((uint16_t)*yuv++) << 7;
        }
        
        return(0);
    };
};



template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray16ToRGB24;


template<int inputSize,int outputSize>
class Gray16ToRGB24<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray16ToRGB24(FIFOBase<int8_t> &src,
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
        int8_t *g=this->getReadBuffer();
        int8_t *rgb=this->getWriteBuffer();

        uint16_t *gray = (uint16_t*)g;

        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           int8_t v = *gray++ >> 7;
           *rgb++ = v;
           *rgb++ = v;
           *rgb++ = v;
        }
        
        return(0);
    };
};



template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray8ToRGBA32;


template<int inputSize,int outputSize>
class Gray8ToRGBA32<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray8ToRGBA32(FIFOBase<int8_t> &src,
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
        uint8_t *g=(uint8_t*)this->getReadBuffer();
        uint8_t *rgb=(uint8_t*)this->getWriteBuffer();


        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           uint8_t v = *g++;
           *rgb++ = v;
           *rgb++ = v;
           *rgb++ = v;
           *rgb++ = 255;
        }
        
        return(0);
    };
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class RGBA32ToGray8;


template<int inputSize,int outputSize>
class RGBA32ToGray8<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    RGBA32ToGray8(FIFOBase<int8_t> &src,
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
        uint8_t *src=(uint8_t*)this->getReadBuffer();
        uint8_t *dst=(uint8_t*)this->getWriteBuffer();


        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           uint8_t r = *src++;
           uint8_t b = *src++;
           uint8_t g = *src++;
           src++;

           float c = 0.299f * r + 0.587f * g + 0.114f * b;
          
           *dst++ = (uint8_t)c;
        }
        
        return(0);
    };
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray16ToRGBA32;


template<int inputSize,int outputSize>
class Gray16ToRGBA32<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray16ToRGBA32(FIFOBase<int8_t> &src,
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
        uint16_t *g=(uint16_t*)this->getReadBuffer();
        uint8_t *rgb=(uint8_t*)this->getWriteBuffer();


        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           uint8_t v = (*g++) >> 7;
           *rgb++ = v;
           *rgb++ = v;
           *rgb++ = v;
           *rgb++ = 255;
        }
        
        return(0);
    };
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray8ToGray16;


template<int inputSize,int outputSize>
class Gray8ToGray16<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray8ToGray16(FIFOBase<int8_t> &src,
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
        uint8_t *src=(uint8_t*)this->getReadBuffer();
        uint16_t *dst=(uint16_t*)this->getWriteBuffer();


        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           *dst++ = (uint16_t)(*src++) << 7;
        }
        
        return(0);
    };
};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class Gray16ToGray8;


template<int inputSize,int outputSize>
class Gray16ToGray8<int8_t,inputSize,
               int8_t,outputSize>: 
      public GenericNode<int8_t,inputSize,
                         int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    Gray16ToGray8(FIFOBase<int8_t> &src,
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
        uint16_t *src=(uint16_t*)this->getReadBuffer();
        uint8_t *dst=(uint8_t*)this->getWriteBuffer();


        for(int32_t k=0;k<this->inputWidth()*this->inputHeight();k++)
        {
           *dst++ = *src++ >> 7;
        }
        
        return(0);
    };
};


/**
 Color conversions nodes implemented with CMSIS-CV

 */

template<typename I,typename O>
struct _CVColorConvertFunc;

template<typename I>
struct _CVImageChannel
{
   typedef decltype(I::pData) type;
};

#define COLOR_CVT_FUNC(NAME,I,O)  \
template<> \
struct _CVColorConvertFunc<I,O>   \
{                                 \
    static void convert(I *i,O *o)\
    {                             \
          NAME(i,o);              \
    };                            \
};


COLOR_CVT_FUNC(arm_yuv420_to_gray8,arm_cv_image_yuv420_t,arm_cv_image_gray8_t);
COLOR_CVT_FUNC(arm_yuv420_to_rgb24,arm_cv_image_yuv420_t,arm_cv_image_rgb24_t);
COLOR_CVT_FUNC(arm_bgr_8U3C_to_gray8,arm_cv_image_bgr_8U3C_t,arm_cv_image_gray8_t);
COLOR_CVT_FUNC(arm_bgr_8U3C_to_rgb24,arm_cv_image_bgr_8U3C_t,arm_cv_image_rgb24_t);
COLOR_CVT_FUNC(arm_gray8_to_rgb24,arm_cv_image_gray8_t,arm_cv_image_rgb24_t);
COLOR_CVT_FUNC(arm_rgb24_to_gray8,arm_cv_image_rgb24_t,arm_cv_image_gray8_t);


template<typename IN, int inputSize,
         typename OUT,int outputSize,
         typename IMGIN = arm_cv_image_gray8_t,
         typename IMGOUT = arm_cv_image_rgb24_t>
class _CVColorConvert;


template<int inputSize,int outputSize,
         typename IMGIN,
         typename IMGOUT>
class _CVColorConvert<int8_t,inputSize,
                      int8_t,outputSize,
                      IMGIN,
                      IMGOUT>: 
public GenericNode<int8_t,inputSize,
                   int8_t,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    _CVColorConvert(FIFOBase<int8_t> &src,
                    FIFOBase<int8_t> &dst):
    GenericNode<int8_t,inputSize,
                int8_t,outputSize>(src,dst){};

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
        uint8_t *g=(uint8_t*)this->getReadBuffer();
        uint8_t *rgb=(uint8_t*)this->getWriteBuffer();

        IMGIN input;
        IMGOUT output;

        input.width = this->inputWidth();
        input.height = this->inputHeight();
        input.pData = (typename _CVImageChannel<IMGIN>::type)g;

        output.width = this->inputWidth();
        output.height = this->inputHeight();
        output.pData = (typename _CVImageChannel<IMGOUT>::type)rgb;


        _CVColorConvertFunc<IMGIN,IMGOUT>::convert(&input,&output);
        
        return(0);
    };
};

#define COLOR_CVT_NODE(NAME,I,O)         \
                                         \
template<typename IN, int inputSize,     \
         typename OUT,int outputSize>    \
class NAME;                              \
                                         \
                                         \
template<int inputSize,int outputSize>   \
class NAME<int8_t,inputSize,             \
           int8_t,outputSize>:           \
public _CVColorConvert<int8_t,inputSize, \
                       int8_t,outputSize,\
                       I,                \
                       O>                \
{                                        \
public:                                  \
    NAME(FIFOBase<int8_t> &src,          \
         FIFOBase<int8_t> &dst):         \
    _CVColorConvert<int8_t,inputSize,    \
                    int8_t,outputSize,   \
                    I,                   \
                    O>(src,dst){};       \
};

COLOR_CVT_NODE(YUV420ToGray8,arm_cv_image_yuv420_t,arm_cv_image_gray8_t);
COLOR_CVT_NODE(YUV420ToRGB24,arm_cv_image_yuv420_t,arm_cv_image_rgb24_t);
COLOR_CVT_NODE(BGR_8U3CToGray8,arm_cv_image_bgr_8U3C_t,arm_cv_image_gray8_t);
COLOR_CVT_NODE(BGR_8U3CToRGB24,arm_cv_image_bgr_8U3C_t,arm_cv_image_rgb24_t);
COLOR_CVT_NODE(Gray8ToRGB24,arm_cv_image_gray8_t,arm_cv_image_rgb24_t);
COLOR_CVT_NODE(RGB24ToGray8,arm_cv_image_rgb24_t,arm_cv_image_gray8_t);
