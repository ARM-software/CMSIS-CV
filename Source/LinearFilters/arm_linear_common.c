
#include "arm_linear_filter_common.h"
#include "cv/linear_filters.h"
#include "dsp/basic_math_functions.h"


/**
  @ingroup linearFilter
 */

/**
 * @brief      Return the scratch size for generic linear filter function using a q15 buffer
 *
 * @param[in]     width        The width of the image in pixels
 * @return		  Scratch size in bytes
 */
uint16_t arm_get_linear_scratch_size_buffer_15(const int width)
{
    return (width * sizeof(q15_t));
}

/**
  @ingroup linearFilter
 */

/**
 * @brief      Return the scratch size for generic linear filter function using a q31 buffer
 *
 * @param[in]     width        The width of the image in pixels
 * @return		  Scratch size in bytes
 */
uint16_t arm_get_linear_scratch_size_buffer_31(const int width)
{
    return (width * sizeof(q31_t));
}

/**
  @ingroup linearFilter
 */

/**     
 * @brief      Return the scratch size for sobel functions using a q15 buffer
 *
 * @param[in]     width        The width of the image in pixels
 * @return		  Scratch size in bytes
 */
uint16_t arm_get_scratch_size_sobel(const int width)
{
    return (width * sizeof(q15_t));
}