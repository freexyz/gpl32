#ifndef __GP_TIMER_H__
#define __GP_TIMER_H__
/******************************************************************************
 * Paresent Header Include
 ******************************************************************************/
#include "driver_l1.h"

extern INT32U gp_timer_init(INT32U TimerId, INT32U freq_hz);
extern INT32U get_gp_timer_counter_L(void);
extern INT32U get_gp_timer_counter_H(void);
extern void gp_timer_isr(void);
extern INT32U Gp_Timer_Count_H;
extern INT32U Gp_Timer_Count_L;


#endif