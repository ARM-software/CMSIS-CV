#include<stdio.h>
#include<math.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

#include "cv_app_settings.h"
#include "cv_canny_edge_scheduler.h"


int canny_edge_stream(unsigned char * src,
           unsigned char * dst1,
           unsigned char * dst2,
           uint32_t *params1,
           uint32_t *params2) 
{
    int error=0;
    uint32_t res;
    g_frame_available = 1;
    res = cv_canny_edge_scheduler(&error,src,dst1,dst2,params1,params2);
    (void)res;
    return(error);
}


int canny_edge_init_cv(unsigned char * src,
            unsigned char * dst1,
            unsigned char * dst2,
            uint32_t *params1,
            uint32_t *params2)
{
    int err = 0;
    g_frame_available = 0;
    err = init_buffer_cv_canny_edge_scheduler(src,dst1,dst2,params1,params2);
    if (err!=0)
    {
        return(err);
    }

    err = init_cv_canny_edge_scheduler(src,dst1,dst2,params1,params2);
    if (err!=0)
    {
        free_buffer_cv_canny_edge_scheduler(src,dst1,dst2,params1,params2);
    }
    return(err);
}











