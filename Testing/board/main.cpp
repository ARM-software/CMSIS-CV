#include "RTE_Components.h"
#include  CMSIS_device_header
#include "stdio.h"

#if defined(MPS3)
#include "cmsis_driver_config.h"
#include "stdout_USART.h"
#endif 

#include <cstdint>
#include "common.h"
#include "load.hpp"

#include <vector>
#include <sstream> // for ostringstream

volatile uint32_t nb_sys_tick_round=0;
#define MAX_SYSTICK 0xFFFFFFUL

extern "C" {
    extern void SysTick_Handler(void);
    extern long time_in_cycles();
}

void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */
  nb_sys_tick_round++;                                                /* See startup file startup_LPC17xx.s for SysTick vector */ 
}
   

extern "C" uint32_t SystemCoreClock;


float time_in_ms() {
    return(1000.0*(MAX_SYSTICK-SysTick->VAL + MAX_SYSTICK * nb_sys_tick_round)/SystemCoreClock);
}

long time_in_cycles() {
    return(MAX_SYSTICK - SysTick->VAL + MAX_SYSTICK * nb_sys_tick_round);
}



extern void run_test(const unsigned char* inputs,
                     const uint32_t testid,
                     const uint32_t funcid,
                     unsigned char* &wbuf,
                     uint32_t &total_bytes,
                     long &cycles);

void run_test_group()
{
    long cycles,start,stop,overhead;
    const unsigned char* inputs = load_test_mem("test.dat");
    // List of tests
    const uint32_t *tests_id = Buffer<uint32_t>::read(inputs,0);
    const uint32_t nb_tests = get_buffer_length(inputs,0) >> 2;
    const uint32_t *enabled = Buffer<uint32_t>::read(inputs,1);

    cycles = 0;
    for(int i=0;i<10;i++)
    {
#if defined (__ICACHE_PRESENT)
          SCB_InvalidateICache();    
#endif
#if defined (__DCACHE_PRESENT)
          SCB_InvalidateDCache();
#endif
        start = time_in_cycles();
        stop = time_in_cycles();
        cycles += (stop - start);
    }

    overhead = cycles / 10;

    
    for(uint32_t id = 0;id < nb_tests; id++)
    {
      if (enabled[id])
      {
          uint32_t func_id = tests_id[id];
          unsigned char *outputs = nullptr;
          uint32_t total_bytes;
    
          printf("Test %d\r\n",id);
#if defined (__ICACHE_PRESENT)
          SCB_InvalidateICache();    
#endif
#if defined (__DCACHE_PRESENT)
          SCB_InvalidateDCache();
#endif
          run_test(inputs,id,func_id,outputs,total_bytes,cycles);
          printf("Cycles = %ld\r\n",cycles - overhead);
   
          if (outputs)
          {
#if !defined(NOOUTPUT)
             std::ostringstream out;
             out << "results/output_" << id << ".dat";
             save_mem(out.str().c_str(),outputs,total_bytes);
#endif
             aligned_free((void*)outputs);
    
          }
          else
          {
             printf("No output generated for %d\r\n",id);
          }
          printf("\r\n");
      }
    }


}



int main(void)
{
    int systick_status = 0;

#if defined(MPS3)
    stdout_init();
#endif


#if defined (__ICACHE_PRESENT)
    SCB_EnableICache();
#endif

#if defined (__DCACHE_PRESENT)
    SCB_EnableDCache();
#endif

    systick_status = SysTick_Config(MAX_SYSTICK);
    if (systick_status != 0)
    {
        printf("Can't enable SysTick\r\n");
    }

    run_test_group();


    printf(__TIME__"\r\n");
    printf("END_TEST\r\n");


}


