#include "opencv2/imgproc.hpp"

using namespace cv;

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class OpenCVGaussian;


template<int inputOutputSize>
class OpenCVGaussian<int8_t,inputOutputSize,
               int8_t,inputOutputSize>: 
      public GenericNode<int8_t,inputOutputSize,
                         int8_t,inputOutputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    OpenCVGaussian(FIFOBase<int8_t> &src,
             FIFOBase<int8_t> &dst,uint32_t w,uint32_t h,
                uint32_t *params):
    GenericNode<int8_t,inputOutputSize,
                int8_t,inputOutputSize>(src,dst),mW(w),mH(h),mParams(params){};

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
        int8_t *a=this->getReadBuffer();
        int8_t *b=this->getWriteBuffer();

        const int kernel_size = 3;

        Mat src(mH,mW,CV_8UC1,(void*)a);
        Mat dst(mH,mW,CV_8UC1,(void*)b);

        GaussianBlur(src,dst, Size(3, 3),0);
       
        return(0);
    };
protected:
     uint32_t mW,mH;
     uint32_t *mParams;
};