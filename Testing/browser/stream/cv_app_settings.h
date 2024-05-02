#ifndef CV_APP_SETTINGS_H
#define CV_APP_SETTINGS_H

#ifdef   __cplusplus
extern "C"
{
#endif

extern uint32_t g_frame_available;

#define CG_MALLOC(A) malloc((A));


#define CG_FREE(A) free((A));

//#define CG_BEFORE_NODE_EXECUTION(A) \
//printf("ID = %d\n",(A));

//#define CG_BEFORE_SCHEDULE \
// printf("%d\n",params[0]);

#ifdef   __cplusplus
}
#endif

#endif
