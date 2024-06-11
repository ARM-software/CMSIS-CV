#ifndef CINPUT_H 
#define CINPUT_H 
    
#include "arm_math_types.h"
    
#ifdef   __cplusplus
extern "C"
{
#endif
    
#if defined(CINPUT)
    
#define NB_CINPUT 115096
extern const uint8_t cinput[NB_CINPUT];

#endif

#ifdef   __cplusplus
}
#endif

#endif
