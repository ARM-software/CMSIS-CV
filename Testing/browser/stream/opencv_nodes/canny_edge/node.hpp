#include "opencv2/imgproc.hpp"

using namespace cv;

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class OpenCVCanny;


template<int inputOutputSize>
class OpenCVCanny<int8_t,inputOutputSize,
               int8_t,inputOutputSize>: 
      public GenericNode<int8_t,inputOutputSize,
                         int8_t,inputOutputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    OpenCVCanny(FIFOBase<int8_t> &src,
             FIFOBase<int8_t> &dst,
                uint32_t *params):
    GenericNode<int8_t,inputOutputSize,
                int8_t,inputOutputSize>(src,dst),mParams(params){};

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

        Mat src(this->inputHeight(),this->inputWidth(),CV_8UC1,(void*)a);
        Mat dst(this->inputHeight(),this->inputWidth(),CV_8UC1,(void*)b);

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

        Canny( src, dst, low, high, kernel_size );
       
        return(0);
    };
protected:
     uint32_t *mParams;
};