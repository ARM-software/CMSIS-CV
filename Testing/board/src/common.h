#ifndef ML_HELIUM_COMMON_H
#define ML_HELIUM_COMMON_H

#include <stdint.h>
#include <stdlib.h>


#include "arm_math_types.h"
#include "arm_math_types_f16.h"


typedef uint32_t byte_index_t;
typedef uint32_t byte_length_t;

#ifdef   __cplusplus
extern "C"
{
#endif

// Where input tensor buffers are starting in the buffer set
// Before input tensors there are tensors used to describe
// the test
#define TENSOR_START 2

#ifndef BUFFER_ALIGNMENT
#define BUFFER_ALIGNMENT 16
#endif

#ifndef INPUT_ADDR
#define INPUT_ADDR 0x70000000
#endif 

#ifndef OUTPUT_ADDR
#define OUTPUT_ADDR 0x90000000
#endif

extern byte_length_t get_buffer_length(const unsigned char *,
                                       const int buffer_nb);


extern void get_img_dims(const unsigned char *test,
                         const int buffer_nb,
                         uint32_t *width,
                         uint32_t *height);

extern void get_buffer_shape(const unsigned char *test,
                             const int buffer_nb,
                             uint32_t *nb,
                             uint32_t *dim0,
                             uint32_t *dim1,
                             uint32_t *dim2,
                             uint32_t *dim3);

extern const unsigned char* get_read_buffer(const unsigned char *test,
                                            const int buffer_nb);

extern unsigned char* get_write_buffer(unsigned char *test,
                                       const int buffer_nb);

extern void* aligned_malloc(size_t size,
                            size_t alignment);
extern void  aligned_free(void* ptr);


#ifdef   __cplusplus
}
#endif

#endif