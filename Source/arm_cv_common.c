#include "arm_math_types.h"
#include "arm_cv_types.h"
#include "arm_cv_common.h"

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

CV_FAST_MEMORY
int8_t __ALIGNED(16) cv_sgbuf[16*3];

#endif