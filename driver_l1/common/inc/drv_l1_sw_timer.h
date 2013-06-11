#ifndef __GP_TIMER_H__
#define __GP_TIMER_H__
/******************************************************************************
 * Paresent Header Include
 ******************************************************************************/
#include "driver_l1.h"

/* Each Tiny count time is 2.666... us*/
#define TINY_WHILE(BREAK_CONDITION,TIMEOUT_TINY_COUNT)  \
{\
   INT32S ttt1 = tiny_counter_get();\
   INT32S dt=0;\
   while(!((BREAK_CONDITION) || (dt>TIMEOUT_TINY_COUNT)))\
   {\
        dt = ((tiny_counter_get() | 0x10000) - ttt1) & (0xFFFF);\
   }\
}

#define MINY_WHILE(BREAK_CONDITION,TIMEOUT_MINY_COUNT)  \
{\
   INT32S dtm=0;\
   INT32S dtn = tiny_counter_get();\
   while(!((BREAK_CONDITION) || (dtm>TIMEOUT_MINY_COUNT)))\
   {\
        if ((((tiny_counter_get() | 0x10000) - dtn) & (0xFFFF))>=375)\
        {dtm++;dtn=tiny_counter_get();}\
   }\
}

#ifndef _SW_TIMER_ID
#define _SW_TIMER_ID

typedef enum
{
    SW_TIMER_0=0,
    SW_TIMER_1,
    SW_TIMER_2,
    SW_TIMER_3,
    SW_TIMER_4,
    SW_TIMER_5,
    SW_TIMER_6,
    SW_TIMER_7,
    SW_TIMER_8,
    SW_TIMER_9,
    SW_TIMER_MAX
} SW_TIMER_ID;

#endif


extern INT32U sw_timer_init(INT32U TimerId, INT32U freq_hz);
extern INT32U sw_timer_get_counter_L(void);
extern INT32U sw_timer_get_counter_H(void);
extern void sw_timer_isr(void);
extern void sw_schedual_isr(void);
extern void sw_timer_ms_delay(INT32U msec);
extern INT32U sw_timer_count_H;
extern INT32U sw_timer_count_L;
extern INT32U sw_schedual_count;
extern void sw_timer_with_isr_phy_init(TIMER_ID_ENUM phy_timer_id, INT16U freq_Hz_base);
extern void sw_timer_with_isr_phy_uninit(TIMER_ID_ENUM phy_timer_id);
extern void sw_timer_freq_setup(SW_TIMER_ID sw_timer_id, INT16U freq_Hz, void (*TimerIsrFunction)(void));
extern void sw_timer_start_without_isr(SW_TIMER_ID sw_timer_id, INT16U freq_Hz);
extern INT32U sw_timer_counter_get(SW_TIMER_ID sw_timer_id);
extern void sw_timer_counter_reset(SW_TIMER_ID sw_timer_id);
extern void sw_timer_stop(SW_TIMER_ID sw_timer_id);
#endif
