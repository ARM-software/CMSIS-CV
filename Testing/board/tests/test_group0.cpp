#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>

extern "C" {
    #include "cv/linear_filters.h"
}

#if defined(TESTGROUP0)

void test_rgb(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{
    long start,end;
    uint32_t width,height;
    // We assume that each input image has a corresponding tests in this suite
    // so the bufid is the testid
    int bufid = TENSOR_START + testid;
   
    
    get_img_dims(inputs,bufid,&width,&height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width))};

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    memcpy(dst,src,get_buffer_length(inputs,bufid));
    end = time_in_cycles();
    cycles = end - start;
}

void test_gray8(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{
    long start,end;
    uint32_t nb_dims,dim0,dim1,dim2,dim3;
    // We assume that each input image has a corresponding tests in this suite
    // so the bufid is the testid
    int bufid = TENSOR_START + testid;
   
    get_buffer_shape(inputs,bufid,&nb_dims,&dim0,&dim1,&dim2,&dim3);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(dim0,dim1)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    memcpy(dst,src,get_buffer_length(inputs,bufid));
    end = time_in_cycles();
    cycles = end - start;
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
            test_rgb(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 1:
            test_gray8(inputs,wbuf,total_bytes,testid,cycles);
            break;
    }

}

#endif