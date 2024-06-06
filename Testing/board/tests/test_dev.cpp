#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>
#include <stdlib.h>

extern "C" {
    #include "cv/image_transforms.h"
}

#if defined(TESTDEV)


void test_dev(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 long &cycles)
{
    long start,end;
    uint32_t nb,channels,width,height,misc;
    int bufid = TENSOR_START + 0;

    // BGR_8U3C has dimension [3,H,W]
    get_buffer_shape(inputs,bufid,&nb,&channels,&height,&width,&misc);

    uint8_t *p_img = (uint8_t*)malloc(2*32);
    //printf("%d %d\r\n",width,height);



    std::vector<BufferDescription> desc = {BufferDescription(Shape(3,16,32)
                                                            ,kIMG_NUMPY_TYPE_UINT8)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_bgr_8U3C_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    (void)input;
    arm_cv_image_bgr_8U3C_t output;
    output.width=32;
    output.height=16;
    output.pData=dst;

    //arm_cv_image_gray8_t tmpin;
    //tmpin.width=width;
    //tmpin.height=height;
    //tmpin.pData=(uint8_t*)src;
//
    //arm_cv_image_gray8_t tmpout;
    //tmpout.width=32;
    //tmpout.height=16;
    //tmpout.pData=(uint8_t*)dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    //uint8_t *p = dst;
    arm_image_resize_bgr_8U3C_f32(&input,&output,p_img);
    
    //arm_image_resize_gray8(&tmpin,&tmpout,p_img);
//
    //tmpin.pData=(uint8_t*)src + (width*height);
    //tmpout.pData=dst + (32*16);
    //arm_image_resize_gray8(&tmpin,&tmpout,p_img);
//
    //tmpin.pData=(uint8_t*)src + 2*(width*height);
    //tmpout.pData=dst + 2*(32*16);
    //arm_image_resize_gray8(&tmpin,&tmpout,p_img);
    //for(int ch=0;ch<3;ch++)
    //{
    //    for(int row =0 ;row < 16;row ++)
    //    {
    //        for(int col = 0;col < 32;col++)
    //        {
    //            *p++ = ch+row;
    //        }
    //    }
    //}
    end = time_in_cycles();
    cycles = end - start;

    free(p_img);
}

void run_test(const unsigned char* inputs,
              const uint32_t testid,
              const uint32_t funcid,
              unsigned char* &wbuf,
              uint32_t &total_bytes,
              long &cycles)
{

    wbuf = nullptr;
    (void)testid;
    (void)funcid;
    test_dev(inputs,wbuf,total_bytes,cycles);
}

#endif