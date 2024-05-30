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
#include "cv_canny_edge_ocv_scheduler.h"

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
static uint8_t schedule[10]=
{ 
0,8,7,6,1,9,4,2,3,5,
};

/*

Internal ID identification for the nodes

*/
#define CAMERA_INTERNAL_ID 0
#define CANNY_INTERNAL_ID 1
#define CANNY_CV_INTERNAL_ID 2
#define CV_TO_RGBA_INTERNAL_ID 3
#define DISPLAY1_INTERNAL_ID 4
#define DISPLAY2_INTERNAL_ID 5
#define DUP0_INTERNAL_ID 6
#define GAUSSIAN_INTERNAL_ID 7
#define TO_GRAY8_INTERNAL_ID 8
#define TO_RGBA_INTERNAL_ID 9

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
#define FIFOSIZE0 307200
#define FIFOSIZE1 76800
#define FIFOSIZE2 153600
#define FIFOSIZE3 307200
#define FIFOSIZE4 76800
#define FIFOSIZE5 307200
#define FIFOSIZE6 76800
#define FIFOSIZE7 76800
#define FIFOSIZE8 76800

typedef struct {
uint8_t  *buf0;
uint8_t  *buf1;
} buffers_t;

CG_BEFORE_BUFFER
static buffers_t buffers={0};

int init_buffer_cv_canny_edge_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
    buffers.buf0 = (uint8_t *)CG_MALLOC(153600 * sizeof(uint8_t));
    if (buffers.buf0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    buffers.buf1 = (uint8_t *)CG_MALLOC(76800 * sizeof(uint8_t));
    if (buffers.buf1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    return(CG_SUCCESS);
}

void free_buffer_cv_canny_edge_ocv_scheduler(unsigned char * src,
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
ImageFIFO<int8_t,FIFOSIZE0,1,0> *fifo0;
ImageFIFO<int8_t,FIFOSIZE1,1,0> *fifo1;
ImageFIFO<int8_t,FIFOSIZE2,1,0> *fifo2;
ImageFIFO<int8_t,FIFOSIZE3,1,0> *fifo3;
ImageFIFO<int8_t,FIFOSIZE4,1,0> *fifo4;
ImageFIFO<int8_t,FIFOSIZE5,1,0> *fifo5;
ImageFIFO<int8_t,FIFOSIZE6,1,0> *fifo6;
ImageFIFO<int8_t,FIFOSIZE7,1,0> *fifo7;
ImageFIFO<int8_t,FIFOSIZE8,1,0> *fifo8;
} fifos_t;

typedef struct {
    WebCamera<int8_t,307200> *camera;
    CannyEdge<int8_t,76800,int8_t,153600> *canny;
    OpenCVCanny<int8_t,76800,int8_t,76800> *canny_cv;
    Gray8ToRGBA32<int8_t,76800,int8_t,307200> *cv_to_rgba;
    WebDisplay<int8_t,307200> *display1;
    WebDisplay<int8_t,307200> *display2;
<<<<<<< HEAD
<<<<<<< HEAD
    Duplicate<int8_t,153600,int8_t,153600> *dup0;
    Gray16ToGray8<int8_t,153600,int8_t,76800> *g16_to_g8;
    GaussianFilter<int8_t,76800,int8_t,153600> *gaussian;
    RGBA32ToGray8<int8_t,307200,int8_t,76800> *to_gray8;
    Gray16ToRGBA32<int8_t,153600,int8_t,307200> *to_rgba;
=======
=======
>>>>>>> b6dcc048cf255b882b2e93faa3648ec9512cf12d
    Duplicate<int8_t,76800,int8_t,76800> *dup0;
    GaussianFilter<int8_t,76800,int8_t,76800> *gaussian;
    RGBAToGray8<int8_t,307200,int8_t,76800> *to_gray8;
    Gray16ToRGBA<int8_t,153600,int8_t,307200> *to_rgba;
>>>>>>> c84958d (commit before merging with update)
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};


int init_cv_canny_edge_ocv_scheduler(unsigned char * src,
                              unsigned char * dst1,
                              unsigned char * dst2,
                              uint32_t* params1,
                              uint32_t* params2)
{
init_cb_state();

    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new ImageFIFO<int8_t,FIFOSIZE0,1,0>(src,0,320,240);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new ImageFIFO<int8_t,FIFOSIZE1,1,0>(buffers.buf0,0,320,240);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo2 = new ImageFIFO<int8_t,FIFOSIZE2,1,0>(buffers.buf0,0,320,240);
    if (fifos.fifo2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo3 = new ImageFIFO<int8_t,FIFOSIZE3,1,0>(dst1,0,320,240);
    if (fifos.fifo3==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo4 = new ImageFIFO<int8_t,FIFOSIZE4,1,0>(buffers.buf0,0,320,240);
    if (fifos.fifo4==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo5 = new ImageFIFO<int8_t,FIFOSIZE5,1,0>(dst2,0,320,240);
    if (fifos.fifo5==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo6 = new ImageFIFO<int8_t,FIFOSIZE6,1,0>(buffers.buf1,0,320,240);
    if (fifos.fifo6==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo7 = new ImageFIFO<int8_t,FIFOSIZE7,1,0>(buffers.buf1,0,320,240);
    if (fifos.fifo7==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo8 = new ImageFIFO<int8_t,FIFOSIZE8,1,0>(buffers.buf1,0,320,240);
    if (fifos.fifo8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.camera = new WebCamera<int8_t,307200>(*(fifos.fifo0));
    if (nodes.camera==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.canny = new CannyEdge<int8_t,76800,int8_t,153600>(*(fifos.fifo7),*(fifos.fifo2),params1);
    if (nodes.canny==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.canny_cv = new OpenCVCanny<int8_t,76800,int8_t,76800>(*(fifos.fifo8),*(fifos.fifo4),params2);
    if (nodes.canny_cv==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
<<<<<<< HEAD
<<<<<<< HEAD
    nodes.cv_to_rgba = new Gray8ToRGBA32<int8_t,76800,int8_t,307200>(*(fifos.fifo5),*(fifos.fifo6));
=======
    nodes.cv_to_rgba = new Gray8ToRGBA<int8_t,76800,int8_t,307200>(*(fifos.fifo4),*(fifos.fifo5));
>>>>>>> c84958d (commit before merging with update)
=======
    nodes.cv_to_rgba = new Gray8ToRGBA<int8_t,76800,int8_t,307200>(*(fifos.fifo4),*(fifos.fifo5));
>>>>>>> b6dcc048cf255b882b2e93faa3648ec9512cf12d
    if (nodes.cv_to_rgba==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.display1 = new WebDisplay<int8_t,307200>(*(fifos.fifo3));
    if (nodes.display1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.display2 = new WebDisplay<int8_t,307200>(*(fifos.fifo5));
    if (nodes.display2==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.dup0 = new Duplicate<int8_t,76800,int8_t,76800>(*(fifos.fifo6),{});
    if (nodes.dup0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.gaussian = new GaussianFilter<int8_t,76800,int8_t,76800>(*(fifos.fifo1),*(fifos.fifo6));
    if (nodes.gaussian==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.to_gray8 = new RGBA32ToGray8<int8_t,307200,int8_t,76800>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.to_gray8==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    nodes.to_rgba = new Gray16ToRGBA32<int8_t,153600,int8_t,307200>(*(fifos.fifo2),*(fifos.fifo3));
    if (nodes.to_rgba==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    return(CG_SUCCESS);

}

void free_cv_canny_edge_ocv_scheduler(unsigned char * src,
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
    if (fifos.fifo8!=NULL)
    {
       delete fifos.fifo8;
    }

    if (nodes.camera!=NULL)
    {
        delete nodes.camera;
    }
    if (nodes.canny!=NULL)
    {
        delete nodes.canny;
    }
    if (nodes.canny_cv!=NULL)
    {
        delete nodes.canny_cv;
    }
    if (nodes.cv_to_rgba!=NULL)
    {
        delete nodes.cv_to_rgba;
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
    if (nodes.to_gray8!=NULL)
    {
        delete nodes.to_gray8;
    }
    if (nodes.to_rgba!=NULL)
    {
        delete nodes.to_rgba;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t cv_canny_edge_ocv_scheduler(int *error,unsigned char * src,
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
        for(; id < 10; id++)
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
                    nodes.canny->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.canny->run();
                }
                break;

                case 2:
                {
                    nodes.canny_cv->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.canny_cv->run();
                }
                break;

                case 3:
                {
                    nodes.cv_to_rgba->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.cv_to_rgba->run();
                }
                break;

                case 4:
                {
                    nodes.display1->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.display1->run();
                }
                break;

                case 5:
                {
                    nodes.display2->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.display2->run();
                }
                break;

                case 6:
                {
                    nodes.dup0->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.dup0->run();
                }
                break;

                case 7:
                {
                    nodes.gaussian->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.gaussian->run();
                }
                break;

                case 8:
                {
                    nodes.to_gray8->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.to_gray8->run();
                }
                break;

                case 9:
                {
                    nodes.to_rgba->setExecutionStatus(cb_state.running);
                   cgStaticError = nodes.to_rgba->run();
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
