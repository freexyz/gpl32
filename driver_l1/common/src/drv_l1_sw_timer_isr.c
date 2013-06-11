#include "drv_l1_sw_timer.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_SW_TIMER) && (_DRV_L1_SW_TIMER == 1)         //
//================================================================//
/******************************************************************************
 * Gloable Variables
 ******************************************************************************/
INT32U sw_timer_count_H=0;
INT32U sw_timer_count_L=0;
//INT32U sw_schedual_count;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void sw_timer_isr(void);
void sw_schedual_isr(void);
/******************************************************************************
 * Function Bodies
 ******************************************************************************/
void sw_timer_isr(void)
{
 sw_timer_count_L++;
 if (sw_timer_count_L==0x0) {sw_timer_count_H++;};
}
/*
void sw_schedual_isr(void)
{
    sw_schedual_count++;
}
*/
#endif
