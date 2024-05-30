#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>
#include <stdio.h>

extern "C" {
    #include "cv/color_transforms.h"
}


#if defined(TESTGROUP1)

// arm_yuv420_to_gray8
void test0(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t width,height;
    int bufid = TENSOR_START + testid;

    // YUV is recorded as a bidimensional tensor
    get_img_dims(inputs,bufid,&width,&height);

    // Convert to image height from YUV height
    height = height * 2 / 3;


    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_yuv420_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_gray8_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_yuv420_to_gray8(&input,&output);
    end = time_in_cycles();
    cycles = end - start;
}

// arm_bgr_8U3C_to_gray8
void test1(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t nb,channels,width,height,misc;
    int bufid = TENSOR_START + testid;

    // BGR_8U3C has dimension [3,H,W]
    get_buffer_shape(inputs,bufid,&nb,&channels,&height,&width,&misc);
    //printf("%d %d %d\r\n",channels,height,width);

    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    
    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_bgr_8U3C_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_gray8_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_bgr_8U3C_to_gray8(&input,&output);
    end = time_in_cycles();
    cycles = end - start;
}

// arm_gray8_to_rgb24
void test2(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t width,height;
    int bufid = TENSOR_START + testid;

    get_img_dims(inputs,bufid,&width,&height);

    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_RGB_TYPE)
                                          };

    
    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_gray8_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_rgb24_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gray8_to_rgb24(&input,&output);
    end = time_in_cycles();
    cycles = end - start;
}

// arm_bgr_8U3C_to_rgb24
void test3(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t nb,channels,width,height,misc;
    // Use second group of images 
    int bufid = TENSOR_START + testid 
    - STANDARD_RGB_NB_IMGS
    - STANDARD_GRAY_NB_IMGS;

    // BGR_8U3C has dimension [3,H,W]
    get_buffer_shape(inputs,bufid,&nb,&channels,&height,&width,&misc);
    //printf("%d %d %d\r\n",channels,height,width);

    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_RGB_TYPE)
                                          };

    
    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_bgr_8U3C_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_rgb24_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_bgr_8U3C_to_rgb24(&input,&output);
    end = time_in_cycles();
    cycles = end - start;
}

// arm_yuv420_to_rgb24
void test4(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t width,height;
    // Remove testid of first tests that is computed from the number
    // of tests executed before
    int bufid = TENSOR_START + testid
       - STANDARD_RGB_NB_IMGS
       - STANDARD_GRAY_NB_IMGS
       - STANDARD_YUV_NB_IMGS
       - STANDARD_RGB_NB_IMGS
    ;

    // YUV is recorded as a bidimensional tensor
    get_img_dims(inputs,bufid,&width,&height);

    // Convert to image height from YUV height
    height = height * 2 / 3;


    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_RGB_TYPE)
                                          };

    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_yuv420_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_rgb24_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_yuv420_to_rgb24(&input,&output);
    end = time_in_cycles();
    cycles = end - start;
}

// arm_rgb24_to_gray8
void test5(const unsigned char* inputs,
                 unsigned char* &outputs,
                 uint32_t &total_bytes,
                 uint32_t testid,
                 long &cycles)
{    
    long start,end;
    uint32_t width,height;
    int bufid = TENSOR_START + testid
       - STANDARD_RGB_NB_IMGS
       - STANDARD_GRAY_NB_IMGS
       - STANDARD_YUV_NB_IMGS
       - STANDARD_RGB_NB_IMGS
       - STANDARD_YUV_NB_IMGS
       + STANDARD_YUV_NB_IMGS
       + STANDARD_RGB_NB_IMGS 
       + STANDARD_GRAY_NB_IMGS;
    ;

    get_img_dims(inputs,bufid,&width,&height);

    std::vector<BufferDescription> desc = {BufferDescription(Shape(height,width)
                                                            ,kIMG_GRAY8_TYPE)
                                          };

    
    outputs = create_write_buffer(desc,total_bytes);

    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);

    const arm_cv_image_rgb24_t input={(uint16_t)width,
                                       (uint16_t)height,
                                       (uint8_t*)src};

    arm_cv_image_gray8_t output;
    output.width=width;
    output.height=height;
    output.pData=dst;
    
    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_rgb24_to_gray8(&input,&output);
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
            // arm_yuv420_to_gray8
            test0(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 1:
            // arm_bgr_8U3C_to_gray8
            test1(inputs,wbuf,total_bytes,testid,cycles);
        break;
        case 2:
            // arm_gray8_to_rgb24
            test2(inputs,wbuf,total_bytes,testid,cycles);
        break;
        case 3:
            // arm_bgr_8U3C_to_rgb24
            test3(inputs,wbuf,total_bytes,testid,cycles);
        break;
        case 4:
            // arm_yuv420_to_rgb24
            test4(inputs,wbuf,total_bytes,testid,cycles);
        break;
        case 5:
            // arm_rgb24_to_gray8
            test5(inputs,wbuf,total_bytes,testid,cycles);
        break;
    }

}

#endif