#include "common.h"
#include "load.hpp"
#include "test_config.h"
#include <vector>

extern "C"
{
#include "cv/linear_filters.h"
}

#if defined(TESTGROUP0)

void test_gauss(const unsigned char *inputs, unsigned char *&outputs, uint32_t &total_bytes, uint32_t test_id,
                long &cycles, int8_t border_type, int8_t funcid)
{
    long start, end;
    uint32_t width, height;
    int bufid = TENSOR_START + test_id - funcid * (STANDART_NB_GRAY_SIZES);
    get_img_dims(inputs, bufid, &width, &height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height, width), kIMG_GRAY8_TYPE)};

    outputs = create_write_buffer(desc, total_bytes);
    q15_t *Buffer_tmp = (q15_t *)malloc(arm_get_linear_scratch_size_buffer_15(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs, bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs, 0);

    const arm_cv_image_gray8_t input = {(uint16_t)width, (uint16_t)height, (uint8_t *)src};
    arm_cv_image_gray8_t output = {(uint16_t)width, (uint16_t)height, (uint8_t *)dst};

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gaussian_filter_3x3_fixp(&input, &output, Buffer_tmp, border_type);
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
}

void test_gauss_5(const unsigned char *inputs, unsigned char *&outputs, uint32_t &total_bytes, uint32_t test_id,
                  long &cycles, int8_t border_type, int8_t funcid)
{
    long start, end;
    uint32_t width, height;
    int bufid = TENSOR_START + test_id - funcid * (STANDART_NB_GRAY_SIZES);
    get_img_dims(inputs, bufid, &width, &height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height, width), kIMG_GRAY8_TYPE)};

    outputs = create_write_buffer(desc, total_bytes);
    q15_t *Buffer_tmp = (q15_t *)malloc(arm_get_linear_scratch_size_buffer_15(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs, bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs, 0);

    const arm_cv_image_gray8_t input = {(uint16_t)width, (uint16_t)height, (uint8_t *)src};
    arm_cv_image_gray8_t output = {(uint16_t)width, (uint16_t)height, (uint8_t *)dst};

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gaussian_filter_5x5_fixp(&input, &output, Buffer_tmp, border_type);
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
}

void test_gauss_7(const unsigned char *inputs, unsigned char *&outputs, uint32_t &total_bytes, uint32_t test_id,
                  long &cycles, int8_t border_type, int8_t funcid)
{
    long start, end;
    uint32_t width, height;
    int bufid = TENSOR_START + test_id - funcid * (STANDART_NB_GRAY_SIZES);
    get_img_dims(inputs, bufid, &width, &height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height, width), kIMG_GRAY8_TYPE)};

    outputs = create_write_buffer(desc, total_bytes);
    q15_t *Buffer_tmp = (q15_t *)malloc(arm_get_linear_scratch_size_buffer_15(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs, bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs, 0);

    const arm_cv_image_gray8_t input = {(uint16_t)width, (uint16_t)height, (uint8_t *)src};
    arm_cv_image_gray8_t output = {(uint16_t)width, (uint16_t)height, (uint8_t *)dst};

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gaussian_filter_7x7_buffer_15_fixp(&input, &output, Buffer_tmp, border_type);
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
}

void test_gauss_7_31(const unsigned char *inputs, unsigned char *&outputs, uint32_t &total_bytes, uint32_t test_id,
                     long &cycles, int8_t border_type, int8_t funcid)
{
    long start, end;
    uint32_t width, height;
    int bufid = TENSOR_START + test_id - funcid * (STANDART_NB_GRAY_SIZES);
    get_img_dims(inputs, bufid, &width, &height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height, width), kIMG_GRAY8_TYPE)};

    outputs = create_write_buffer(desc, total_bytes);
    q31_t *Buffer_tmp = (q31_t *)malloc(arm_get_linear_scratch_size_buffer_31(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs, bufid);
    uint8_t *dst = Buffer<uint8_t>::write(outputs, 0);

    const arm_cv_image_gray8_t input = {(uint16_t)width, (uint16_t)height, (uint8_t *)src};
    arm_cv_image_gray8_t output = {(uint16_t)width, (uint16_t)height, (uint8_t *)dst};

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    arm_gaussian_filter_7x7_buffer_31_fixp(&input, &output, Buffer_tmp, border_type);
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
}

void test_sobel(const unsigned char *inputs, unsigned char *&outputs, uint32_t &total_bytes, uint32_t test_id,
                long &cycles, int8_t border_type, int8_t axis, int funcid)
{
    long start, end;
    uint32_t width, height;
    int bufid = TENSOR_START + test_id - funcid * (STANDART_NB_GRAY_SIZES);

    get_img_dims(inputs, bufid, &width, &height);
    std::vector<BufferDescription> desc = {BufferDescription(Shape(height, width), kIMG_NUMPY_TYPE_SINT16)};

    outputs = create_write_buffer(desc, total_bytes);
    q15_t *Buffer_tmp;
    Buffer_tmp = (q15_t *)malloc(arm_get_scratch_size_sobel(width));
    const uint8_t *src = Buffer<uint8_t>::read(inputs, bufid);
    int16_t *dst = Buffer<int16_t>::write(outputs, 0);

    const arm_cv_image_gray8_t input = {(uint16_t)width, (uint16_t)height, (uint8_t *)src};
    arm_cv_image_q15_t output;
    output.width = width;
    output.height = height;
    output.pData = dst;

    // The test to run is executed with some timing code.
    start = time_in_cycles();
    if (axis == 0)
    {
        arm_sobel_vertical(&input, &output, Buffer_tmp, border_type);
    }
    else
    {
        arm_sobel_horizontal(&input, &output, Buffer_tmp, border_type);
    }
    end = time_in_cycles();
    cycles = end - start;
    free(Buffer_tmp);
}

void run_test(const unsigned char *inputs, const uint32_t testid, const uint32_t funcid, unsigned char *&wbuf,
              uint32_t &total_bytes, long &cycles)
{

    wbuf = nullptr;
    // funcind is used in the test function to compute the number of image previously generated to process the correct
    // images
    switch (funcid)
    {
    case 0:
        //arm_gaussian_filter_3x3_fixp 
        test_gauss(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, funcid);
        break;
    case 1:
        //arm_gaussian_filter_3x3_fixp 
        test_gauss(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, funcid);
        break;
    case 2:
        //arm_gaussian_filter_3x3_fixp 
        test_gauss(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, funcid);
        break;
    case 3:
        //arm_sobel_vertical
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, 0, funcid);
        break;
    case 4:
        //arm_sobel_vertical
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, 0, funcid);
        break;
    case 5:
        //arm_sobel_vertical
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, 0, funcid);
        break;
    case 6:
        //arm_sobel_horizontal
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, 1, funcid);
        break;
    case 7:
        //arm_sobel_horizontal
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, 1, funcid);
        break;
    case 8:
        //arm_sobel_horizontal
        test_sobel(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, 1, funcid);
        break;
    case 9:
        //arm_gaussian_filter_5x5_fixp 
        test_gauss_5(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, funcid);
        break;
    case 10:
        //arm_gaussian_filter_5x5_fixp 
        test_gauss_5(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, funcid);
        break;
    case 11:
        //arm_gaussian_filter_5x5_fixp 
        test_gauss_5(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, funcid);
        break;
    case 12:
        //arm_gaussian_filter_7x7_fixp buffer q15 
        test_gauss_7(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, funcid);
        break;
    case 13:
        //arm_gaussian_filter_7x7_fixp buffer q15
        test_gauss_7(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, funcid);
        break;
    case 14:
        //arm_gaussian_filter_7x7_fixp buffer q15
        test_gauss_7(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, funcid);
        break;
    case 15:
        //arm_gaussian_filter_7x7_fixp_32 buffer q31
        test_gauss_7_31(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_NEAREST, funcid);
        break;
    case 16:
        //arm_gaussian_filter_7x7_fixp_32 buffer q31
        test_gauss_7_31(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_REFLECT, funcid);
        break;
    case 17:
        //arm_gaussian_filter_7x7_fixp_32 buffer q31
        test_gauss_7_31(inputs, wbuf, total_bytes, testid, cycles, ARM_CV_BORDER_WRAP, funcid);
        break;
    }
}

#endif