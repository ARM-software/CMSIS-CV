#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>

#if defined(TESTGROUP2)

void test1(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes)
{    
    std::vector<BufferDescription> desc = {BufferDescription(Shape(20,10,4)
                                                            ,kIMG_NUMPY_TYPE_SINT16)
                                          };
    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,TENSOR_START + 0);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);
    memcpy(dst,src,get_buffer_length(inputs,TENSOR_START + 0));
}



void run_test(const unsigned char* inputs,
              const uint32_t testid,
              const uint32_t funcid,
              unsigned char* &wbuf,
              uint32_t &total_bytes)
{
    (void)testid;
    switch(funcid)
    {
        case 0:
            test1(inputs,wbuf,total_bytes);
    }

}

#endif