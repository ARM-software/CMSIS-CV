#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>
#include <stdio.h>

extern "C" {
    #include "cv/image_transforms.h"
}


#if defined(TESTGROUP2)


#define GRAY8_CROP(TESTID,LEFT,TOP,RIGHT,BOTTOM)                                                 \
void test##TESTID(const unsigned char* inputs,                                                   \
                        unsigned char* &outputs,                                                 \
                        uint32_t &total_bytes,                                                   \
                        uint32_t testid,                                                         \
                        long &cycles)                                                            \
{                                                                                                \
    long start,end;                                                                              \
    uint32_t width,height;                                                                       \
    int bufid = TENSOR_START + 0;                                                                \
    (void)testid;                                                                                \
                                                                                                 \
    get_img_dims(inputs,bufid,&width,&height);                                                   \
                                                                                                 \
    std::vector<BufferDescription> desc = {BufferDescription(Shape((BOTTOM)-(TOP),(RIGHT)-(LEFT))\
                                                            ,kIMG_GRAY8_TYPE)                    \
                                          };                                                     \
                                                                                                 \
                                                                                                 \
    outputs = create_write_buffer(desc,total_bytes);                                             \
                                                                                                 \
    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);                                    \
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);                                            \
                                                                                                 \
    const arm_cv_image_gray8_t input={(uint16_t)width,                                           \
                                       (uint16_t)height,                                         \
                                       (uint8_t*)src};                                           \
                                                                                                 \
    arm_cv_image_gray8_t output;                                                                 \
    output.width=(RIGHT)-(LEFT);                                                                 \
    output.height=(BOTTOM)-(TOP);                                                                \
    output.pData=dst;                                                                            \
                                                                                                 \
    start = time_in_cycles();                                                                    \
    arm_crop_gray8(&input,&output,(LEFT),(TOP),(RIGHT),(BOTTOM));                                \
    end = time_in_cycles();                                                                      \
    cycles = end - start;                                                                        \
}

#define RGB24_CROP(TESTID,LEFT,TOP,RIGHT,BOTTOM)                                                 \
void test##TESTID(const unsigned char* inputs,                                                   \
                        unsigned char* &outputs,                                                 \
                        uint32_t &total_bytes,                                                   \
                        uint32_t testid,                                                         \
                        long &cycles)                                                            \
{                                                                                                \
    long start,end;                                                                              \
    uint32_t width,height;                                                                       \
    int bufid = TENSOR_START + 1;                                                                \
    (void)testid;                                                                                \
                                                                                                 \
    get_img_dims(inputs,bufid,&width,&height);                                                   \
                                                                                                 \
    std::vector<BufferDescription> desc = {BufferDescription(Shape((BOTTOM)-(TOP),(RIGHT)-(LEFT))\
                                                            ,kIMG_RGB_TYPE)                    \
                                          };                                                     \
                                                                                                 \
                                                                                                 \
    outputs = create_write_buffer(desc,total_bytes);                                             \
                                                                                                 \
    const uint8_t *src = Buffer<uint8_t>::read(inputs,bufid);                                    \
    uint8_t *dst = Buffer<uint8_t>::write(outputs,0);                                            \
                                                                                                 \
    const arm_cv_image_rgb24_t input={(uint16_t)width,                                           \
                                       (uint16_t)height,                                         \
                                       (uint8_t*)src};                                           \
                                                                                                 \
    arm_cv_image_rgb24_t output;                                                                 \
    output.width=(RIGHT)-(LEFT);                                                                 \
    output.height=(BOTTOM)-(TOP);                                                                \
    output.pData=dst;                                                                            \
                                                                                                 \
    start = time_in_cycles();                                                                    \
    arm_crop_rgb24(&input,&output,(LEFT),(TOP),(RIGHT),(BOTTOM));                                \
    end = time_in_cycles();                                                                      \
    cycles = end - start;                                                                        \
}

GRAY8_CROP(0,15,15,113,113);
GRAY8_CROP(1,16,16,112,112);
GRAY8_CROP(2,32,32,96,96);
GRAY8_CROP(3,47,47,81,81);
GRAY8_CROP(4,60,60,68,68);
GRAY8_CROP(5,16,60,112,68);

RGB24_CROP(6,15,15,113,113);
RGB24_CROP(7,16,16,112,112);
RGB24_CROP(8,32,32,96,96);
RGB24_CROP(9,47,47,81,81);
RGB24_CROP(10,60,60,68,68);
RGB24_CROP(11,16,60,112,68);

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
            // gray8
            test0(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 1:
            // gray8
            test1(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 2:
            // gray8
            test2(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 3:
            // gray8
            test3(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 4:
            // gray8
            test4(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 5:
            // gray8
            test5(inputs,wbuf,total_bytes,testid,cycles);
            break;

        case 6:
            // rgb24
            test6(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 7:
            // rgb24
            test7(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 8:
            // rgb24
            test8(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 9:
            // rgb24
            test9(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 10:
            // rgb24
            test10(inputs,wbuf,total_bytes,testid,cycles);
            break;
        case 11:
            // rgb24
            test11(inputs,wbuf,total_bytes,testid,cycles);
            break;
    }

}

#endif