#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>
#include <stdio.h>

extern "C" {
    #include "cv/feature_detection.h"
}

#if defined(TESTGROUP3)


void test_sobel(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t test_id,
                 long &cycles)
{
    long start,end;
    uint32_t width,height;
    int bufid = TENSOR_START + test_id;

    get_img_dims(inputs,bufid,&width,&height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_gray8_t input={(uint16_t)width,(uint16_t)height,(uint8_t*)src};
    arm_cv_image_gray8_t output={(uint16_t)width,(uint16_t)height,(uint8_t*)dst};

    q15_t* Buffer_tmp_mag = (q15_t*)malloc(arm_cv_get_scratch_size_canny_sobel(input.width));

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_cv_canny_edge_sobel(&input,&output, Buffer_tmp_mag, 78,33);
    end = time_in_cycles();
    cycles = end - start;

    free(Buffer_tmp_mag);
}
void run_test(const unsigned char* inputs,
              const uint32_t testid,
              const uint32_t funcid,
              unsigned char* &wbuf,
              uint32_t &total_bytes,
              long &cycles)
{

    wbuf = nullptr;
    switch(funcid)
    {
        case 0:
            test_sobel(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 1:
            test_sobel(inputs,wbuf,total_bytes,testid,cycles);
            break;
    }

}

#endif