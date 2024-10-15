#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
extern "C" {
    #include "cv/linear_filters.h"
}

#if defined(TESTDEV)

void test_dev(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 long &cycles)
{
    long start,end;
    uint32_t width,height;
    int bufid = TENSOR_START;

    //int8_t border_type = ARM_CV_BORDER_REFLECT;
    int8_t border_type = ARM_CV_BORDER_NEAREST;
    //int8_t border_type = ARM_CV_BORDER_WRAP;
    get_img_dims(inputs,bufid,&width,&height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);
    q31_t* Buffer_tmp = (q31_t*)malloc(arm_get_scratch_size_generic_31(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_gray8_t input={(uint16_t)width,(uint16_t)height,(uint8_t*)src};
    arm_cv_image_gray8_t output={(uint16_t)width,(uint16_t)height,(uint8_t*)dst};
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gaussian_filter_7x7_32_fixp(&input,&output, Buffer_tmp, border_type);
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
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