#include "common.h"
#include <string.h>

#define NB_BUFFERS (buffers[0])

#define BUFFER_OFFSET_POS(ID) \
   (1 + 7*NB_BUFFERS + (ID))

#define BUFFER_LENGTH_POS(ID) \
   (1 + (ID))

#define BUFFER_DIM_POS(ID) \
   (1 + NB_BUFFERS + (5*ID))

/**
 * @brief      Gets the buffer length in bytes
 *
 * @param[in]  test    The test description
 * @param[in]  buffer_nb  The buffer number
 *
 * @return     The buffer length in bytes.
 */
byte_length_t get_buffer_length(const unsigned char *test,
                                const int buffer_nb)
{
    const uint32_t *buffers = (const uint32_t *)test;
    return(buffers[BUFFER_LENGTH_POS(buffer_nb)]);
}


void get_img_dims(const unsigned char *test,
                  const int buffer_nb,
                  uint32_t *width,
                  uint32_t *height)
{
    const uint32_t *buffers = (const uint32_t *)test;
    *width = buffers[BUFFER_DIM_POS(buffer_nb)+2];
    *height = buffers[BUFFER_DIM_POS(buffer_nb)+1];

}

void get_buffer_shape(const unsigned char *test,
                      const int buffer_nb,
                      uint32_t *nb,
                      uint32_t *dim0,
                      uint32_t *dim1,
                      uint32_t *dim2,
                      uint32_t *dim3)
{
   const uint32_t *buffers = (const uint32_t *)test;

   *nb   = buffers[BUFFER_DIM_POS(buffer_nb)];
   *dim0 = buffers[BUFFER_DIM_POS(buffer_nb+1)];
   *dim1 = buffers[BUFFER_DIM_POS(buffer_nb+2)];
   *dim2 = buffers[BUFFER_DIM_POS(buffer_nb+3)];
   *dim3 = buffers[BUFFER_DIM_POS(buffer_nb+4)];
}

const unsigned char* get_read_buffer(const unsigned char *test,
                                     const int buffer_nb)
{
   const uint32_t *buffers = (const uint32_t *)test;
   const uint32_t offset = buffers[BUFFER_OFFSET_POS(buffer_nb)];
   return(test+offset);
}

unsigned char* get_write_buffer(unsigned char *test,
                                const int buffer_nb)
{
   const uint32_t *buffers = (const uint32_t *)test;
   const uint32_t offset = buffers[BUFFER_OFFSET_POS(buffer_nb)];
   return(test+offset);
}
/**
 * @brief      Memory allocation with alignment
 *
 * @param[in]  size              The size in bytes
 * @param[in]  alignment         The alignment in bytes
 *
 * @return     Pointer to the allocated buffer
 */
void* aligned_malloc(size_t size,
                     size_t alignment)
{
   void *ptr=malloc(size+alignment+sizeof(void*));
   void *aligned = (char*)(((size_t)(ptr)+sizeof(void*)+alignment) & ~(alignment-1));

   *((void**)(aligned) - 1) = ptr;
   return(aligned);
}

/**
 * @brief      Free a buffer allocated with the aligned malloc
 *
 * @param      ptr   The pointer
 */
void aligned_free(void* ptr)
{
    if (ptr) {
        free(*((void**)(ptr) - 1));
    }
};



#define SAFE_FREE(x)\
  if ((x))          \
  {                 \
     free((x));     \
  }

