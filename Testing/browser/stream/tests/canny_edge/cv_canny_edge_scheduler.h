/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _CV_CANNY_EDGE_SCHEDULER_H_ 
#define _CV_CANNY_EDGE_SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif



extern int init_buffer_cv_canny_edge_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2);
extern void free_buffer_cv_canny_edge_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2);

extern int init_cv_canny_edge_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2);
extern void free_cv_canny_edge_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2);

extern uint32_t cv_canny_edge_scheduler(int *error,unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2);

#ifdef   __cplusplus
}
#endif

#endif

