/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include <cstdint>
#include "cv_app_settings.h"
#include "cg_status.h"
#include "GenericNodes.h"
#include "AppNodes.h"
#include "cv_gaussian_ocv_scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_MALLOC)
#define CG_MALLOC(A) malloc((A))
#endif 

#if !defined(CG_FREE)
#define CG_FREE(A) free((A))
#endif 

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION(ID)
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION(ID)
#endif



#if !defined(CG_RESTORE_STATE_MACHINE_STATE)
#define CG_RESTORE_STATE_MACHINE_STATE
#endif

#if !defined(CG_SAVE_STATE_MACHINE_STATE)
#define CG_SAVE_STATE_MACHINE_STATE
#endif


CG_AFTER_INCLUDES


using namespace arm_cmsis_stream;

/*

Description of the scheduling. 

*/
static uint8_t schedule[9]=
{ 
0,6,3,4,7,1,5,8,2,
};

/*

Internal ID identification for the nodes

*/
#define CAMERA_INTERNAL_ID 0
#define DISPLAY1_INTERNAL_ID 1
#define DISPLAY2_INTERNAL_ID 2
#define DUP0_INTERNAL_ID 3
#define GAUSSIAN_INTERNAL_ID 4
#define GAUSSIAN_CV_INTERNAL_ID 5
#define TO_GRAY8_INTERNAL_ID 6
#define TO_RGBA_INTERNAL_ID 7
#define TO_RGBA_CV_INTERNAL_ID 8

/* For callback management */

#define CG_PAUSED_SCHEDULER_ID 1
#define CG_SCHEDULER_NOT_STARTED_ID 2
#define CG_SCHEDULER_RUNNING_ID 3

struct cb_state_t
{
    unsigned long scheduleStateID;
    unsigned long nbSched;
    int status;
    kCBStatus running;
};

static cb_state_t cb_state;


static void init_cb_state()
{
    cb_state.status = CG_SCHEDULER_NOT_STARTED_ID;
    cb_state.nbSched = 0;
    cb_state.scheduleStateID = 0;
    cb_state.running = kNewExecution;
}


CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 1228800
#define FIFOSIZE1 614400
#define FIFOSIZE2 1228800
#define FIFOSIZE3 307200
#define FIFOSIZE4 1228800
#define FIFOSIZE5 307200
#define FIFOSIZE6 307200
#define FIFOSIZE7 307200

typedef struct {
uint8_t  *buf0;
uint8_t  *buf1;
} buffers_t;

CG_BEFORE_BUFFER
static buffers_t buffers={0};

int init_buffer_cv_gaussian_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
    buffers.buf0 = (uint8_t *)CG_MALLOC(614400 * sizeof(uint8_t));
    if (buffers.buf0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    buffers.buf1 = (uint8_t *)CG_MALLOC(307200 * sizeof(uint8_t));
    if (buffers.buf1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    return(CG_SUCCESS);
}

void free_buffer_cv_gaussian_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
    if (buffers.buf0!=NULL)
    {
        CG_FREE(buffers.buf0);
    }
    if (buffers.buf1!=NULL)
    {
        CG_FREE(buffers.buf1);
    }
}


typedef struct {
FIFO<int8_t,FIFOSIZE0,1,0> *fifo0;
FIFO<int8_t,FIFOSIZE1,1,0> *fifo1;
FIFO<int8_t,FIFOSIZE2,1,0> *fifo2;
FIFO<int8_t,FIFOSIZE3,1,0> *fifo3;
FIFO<int8_t,FIFOSIZE4,1,0> *fifo4;
FIFO<int8_t,FIFOSIZE5,1,0> *fifo5;
FIFO<int8_t,FIFOSIZE6,1,0> *fifo6;
FIFO<int8_t,FIFOSIZE7,1,0> *fifo7;
} fifos_t;

typedef struct {
    WebCamera<int8_t,1228800> *camera;
    WebDisplay<int8_t,1228800> *display1;
    WebDisplay<int8_t,1228800> *display2;
    Duplicate<int8_t,307200,int8_t,307200> *dup0;
    GaussianFilter<int8_t,307200,int8_t,614400> *gaussian;
    OpenCVGaussian<int8_t,307200,int8_t,307200> *gaussian_cv;
    RGBAToGray8<int8_t,1228800,int8_t,307200> *to_gray8;
    Gray16ToRGBA<int8_t,614400,int8_t,1228800> *to_rgba;
    Gray8ToRGBA<int8_t,307200,int8_t,1228800> *to_rgba_cv;
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};


int init_cv_gaussian_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
init_cb_state();

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new FIFO<int8_t,FIFOSIZE0,1,0>(src);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new FIFO<int8_t,FIFOSIZE1,1,0>(buffers.buf0);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo2 = new FIFO<int8_t,FIFOSIZE2,1,0>(dst1);
    if (fifos.fifo2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo3 = new FIFO<int8_t,FIFOSIZE3,1,0>(buffers.buf0);
    if (fifos.fifo3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo4 = new FIFO<int8_t,FIFOSIZE4,1,0>(dst2);
    if (fifos.fifo4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo5 = new FIFO<int8_t,FIFOSIZE5,1,0>(buffers.buf0);
    if (fifos.fifo5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo6 = new FIFO<int8_t,FIFOSIZE6,1,0>(buffers.buf1);
    if (fifos.fifo6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo7 = new FIFO<int8_t,FIFOSIZE7,1,0>(buffers.buf1);
    if (fifos.fifo7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.camera = new WebCamera<int8_t,1228800>(*(fifos.fifo0),640,480);
    if (nodes.camera==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.display1 = new WebDisplay<int8_t,1228800>(*(fifos.fifo2),640,480);
    if (nodes.display1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.display2 = new WebDisplay<int8_t,1228800>(*(fifos.fifo4),640,480);
    if (nodes.display2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.dup0 = new Duplicate<int8_t,307200,int8_t,307200>(*(fifos.fifo5),{fifos.fifo6});
    if (nodes.dup0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.gaussian = new GaussianFilter<int8_t,307200,int8_t,614400>(*(fifos.fifo6),*(fifos.fifo1),640,480);
    if (nodes.gaussian==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.gaussian_cv = new OpenCVGaussian<int8_t,307200,int8_t,307200>(*(fifos.fifo7),*(fifos.fifo3),640,480,nullptr);
    if (nodes.gaussian_cv==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.to_gray8 = new RGBAToGray8<int8_t,1228800,int8_t,307200>(*(fifos.fifo0),*(fifos.fifo5),640,480);
    if (nodes.to_gray8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.to_rgba = new Gray16ToRGBA<int8_t,614400,int8_t,1228800>(*(fifos.fifo1),*(fifos.fifo2),640,480);
    if (nodes.to_rgba==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.to_rgba_cv = new Gray8ToRGBA<int8_t,307200,int8_t,1228800>(*(fifos.fifo3),*(fifos.fifo4),640,480);
    if (nodes.to_rgba_cv==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    return(CG_SUCCESS);

}

void free_cv_gaussian_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }
    if (fifos.fifo2!=NULL)
    {
       delete fifos.fifo2;
    }
    if (fifos.fifo3!=NULL)
    {
       delete fifos.fifo3;
    }
    if (fifos.fifo4!=NULL)
    {
       delete fifos.fifo4;
    }
    if (fifos.fifo5!=NULL)
    {
       delete fifos.fifo5;
    }
    if (fifos.fifo6!=NULL)
    {
       delete fifos.fifo6;
    }
    if (fifos.fifo7!=NULL)
    {
       delete fifos.fifo7;
    }

    if (nodes.camera!=NULL)
    {
        delete nodes.camera;
    }
    if (nodes.display1!=NULL)
    {
        delete nodes.display1;
    }
    if (nodes.display2!=NULL)
    {
        delete nodes.display2;
    }
    if (nodes.dup0!=NULL)
    {
        delete nodes.dup0;
    }
    if (nodes.gaussian!=NULL)
    {
        delete nodes.gaussian;
    }
    if (nodes.gaussian_cv!=NULL)
    {
        delete nodes.gaussian_cv;
    }
    if (nodes.to_gray8!=NULL)
    {
        delete nodes.to_gray8;
    }
    if (nodes.to_rgba!=NULL)
    {
        delete nodes.to_rgba;
    }
    if (nodes.to_rgba_cv!=NULL)
    {
        delete nodes.to_rgba_cv;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t cv_gaussian_ocv_scheduler(int *error,unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

if (cb_state.status==CG_PAUSED_SCHEDULER_ID)
   {
      nbSchedule = cb_state.nbSched;

   }


    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
CG_RESTORE_STATE_MACHINE_STATE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        unsigned long id=0;
        if (cb_state.status==CG_PAUSED_SCHEDULER_ID)
        {
            id = cb_state.scheduleStateID;
            cb_state.status = CG_SCHEDULER_RUNNING_ID;
        }
        for(; id < 9; id++)
        {
            CG_BEFORE_NODE_EXECUTION(schedule[id]);
            switch(schedule[id])
            {
                case 0:
                {
                    nodes.camera->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.camera->run();
                }
                break;

                case 1:
                {
                    nodes.display1->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.display1->run();
                }
                break;

                case 2:
                {
                    nodes.display2->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.display2->run();
                }
                break;

                case 3:
                {
                    nodes.dup0->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.dup0->run();
                }
                break;

                case 4:
                {
                    nodes.gaussian->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.gaussian->run();
                }
                break;

                case 5:
                {
                    nodes.gaussian_cv->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.gaussian_cv->run();
                }
                break;

                case 6:
                {
                    nodes.to_gray8->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.to_gray8->run();
                }
                break;

                case 7:
                {
                    nodes.to_rgba->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.to_rgba->run();
                }
                break;

                case 8:
                {
                    nodes.to_rgba_cv->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.to_rgba_cv->run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION(schedule[id]);
            cb_state.running = kNewExecution;
            if (cgStaticError == CG_PAUSED_SCHEDULER)
            {
                CG_SAVE_STATE_MACHINE_STATE;
                cb_state.status = CG_PAUSED_SCHEDULER_ID;
                cb_state.nbSched = nbSchedule;
                cb_state.scheduleStateID = id;
                cb_state.running = kResumedExecution;
            }
            CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
