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
    uint32_t width,height;
    int bufid = TENSOR_START + 0;
    get_img_dims(inputs,bufid,&width,&height);

    uint8_t *p_img = (uint8_t*)malloc(2*32);                               \



    std::vector<BufferDescription> desc = {BufferDescription(Shape(32,32)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_gray8_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_gray8_t output;
    output.width=32;
    output.height=32;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_image_resize_gray8(&input,&output,p_img);
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