/*
* Purpose: system timer management
*
* Author: zhangzha
*
* Date: 2007/11/23
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*         1 2008/4/15 zhangzha create
*/

//Include files
#include "system_timer.h"


// Variables defined in this file
STSYSTIMER	gstSysTimer[MAX_SYSTEM_TIMER];
INT32U		gstSysTimerEnable;

void (*gTimeBaseC_ISR[MAX_SYSTEM_TIMER_ISR])(void);


// Functions defined in this file
void TimeBaseC_ISR(void);
INT32S sys_128Hz_timer_init(void);

// initial TimeBaseC 128Hz isr manager
void sys_init_timer()
{
	int i = 0;

	gp_memset((INT8S*)gstSysTimer, 0, sizeof(gstSysTimer));

	for(i = 0; i < MAX_SYSTEM_TIMER_ISR; i++)
	{
		gTimeBaseC_ISR[i] = NULL;
	}
	gstSysTimerEnable = 0;
}

// setup interval time, the time clock is 128Hz
// every interval time, it will send a message to dispatchQ
// timerid is 0~4
// interval time, equal to: interval * (1 / 128) (second)
INT32S sys_set_timer(void* msg_qsend, void* msg_q_id, INT32U msg_id, INT32U timerid, INT32U interval)
{
	if(timerid > MAX_SYSTEM_TIMER  - 1)
		return TIMER_ID_INVALID;

	gstSysTimer[timerid].msg_qsend = (fp_msg_qsend)msg_qsend;
	gstSysTimer[timerid].msg_q_id = msg_q_id;
	gstSysTimer[timerid].msg_id = msg_id;

	gstSysTimer[timerid].CurrentTick = interval;
	gstSysTimer[timerid].Interval = interval;

	if(gstSysTimerEnable == 0)
	{
		// setup timebase C 128Hz
		time_base_setup(2, 0, 0, TimeBaseC_ISR);
	}

#if MAX_SYSTEM_TIMER > 0
	if(timerid == 0)
		gstSysTimerEnable |= TIMER0_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 1
	if(timerid == 1)
		gstSysTimerEnable |= TIMER1_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 2
	if(timerid == 2)
		gstSysTimerEnable |= TIMER2_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 3
	if(timerid == 3)
		gstSysTimerEnable |= TIMER3_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 4
	if(timerid == 4)
		gstSysTimerEnable |= TIMER4_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 5
	if(timerid == 5)
		gstSysTimerEnable |= TIMER5_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 6
	if(timerid == 6)
		gstSysTimerEnable |= TIMER6_ENABLE;
#endif
	//SysDisableWaitMode(WAITMODE_TIMER);
	return 0;
}

INT32S sys_128Hz_timer_init(void)
{
    time_base_setup(TIME_BASE_C, TMBCS_128HZ, 0, TimeBaseC_ISR);
    return STATUS_OK;
}


// kill timer
INT32S sys_kill_timer(INT32U timerid)
{
	INT32U itemp;

	if(timerid > MAX_SYSTEM_TIMER  - 1)
		return TIMER_ID_INVALID;

#if MAX_SYSTEM_TIMER > 0
	if(timerid == 0)
		itemp = TIMER0_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 1
	if(timerid == 1)
		itemp = TIMER1_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 2
	if(timerid == 2)
		itemp = TIMER2_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 3
	if(timerid == 3)
		itemp = TIMER3_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 4
	if(timerid == 4)
		itemp = TIMER4_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 5
	if(timerid == 5)
		itemp = TIMER5_ENABLE;
#endif

#if MAX_SYSTEM_TIMER > 6
	if(timerid == 6)
		itemp = TIMER6_ENABLE;
#endif

	gstSysTimerEnable &= ~itemp;
	//SysEnableWaitMode(WAITMODE_TIMER);

	if(gstSysTimerEnable == 0)
	{
		// stop timebase C 128Hz
		time_base_stop(2);
	}
	return 0;
}

// kill all timer
INT32S sys_kill_all_timer(void)
{
	gstSysTimerEnable &= ~(TIMER0_ENABLE | TIMER1_ENABLE | TIMER2_ENABLE | TIMER3_ENABLE | TIMER4_ENABLE | TIMER5_ENABLE | TIMER6_ENABLE);
	if(gstSysTimerEnable == 0)
	{
		// stop timebase C 128Hz
		time_base_stop(2);
	}
	return 0;
}

// registe a user isr to 128Hz interrupt
// max can register 5 function
INT32S sys_registe_timer_isr(void (*TimerIsrFunction)(void))
{
	INT32S	i;

	for(i = 0; i < MAX_SYSTEM_TIMER_ISR; i++)
	{
		if(gTimeBaseC_ISR[i] == TimerIsrFunction)
			return 0;
	}

	if(gstSysTimerEnable == 0)
	{
		// setup timebase C 128Hz
		time_base_setup(2, 0, 0, TimeBaseC_ISR);
	}

	for(i = 0; i < MAX_SYSTEM_TIMER_ISR; i++)
	{
		if(gTimeBaseC_ISR[i] == NULL)
		{
			gTimeBaseC_ISR[i] = TimerIsrFunction;
			gstSysTimerEnable |= (USER_TIMER0_ENABLE << i);
			return 0;
		}
	}
	return -1;
}

// release isr
INT32S sys_release_timer_isr(void (*TimerIsrFunction)(void))
{
	INT32S	i;

	for(i = 0; i < MAX_SYSTEM_TIMER_ISR; i++)
	{
		if(gTimeBaseC_ISR[i] == TimerIsrFunction)
		{
			gTimeBaseC_ISR[i] = NULL;
			gstSysTimerEnable &= ~(USER_TIMER0_ENABLE << i);
			if(gstSysTimerEnable == 0)
			{
				// stop timebase C 128Hz
				time_base_stop(2);
			}
			return 0;
		}
	}
	return -1;
}

void TimeBaseC_ISR(void)
{
	INT32S	i;

	// execute use registe isr
	for(i = 0; i < MAX_SYSTEM_TIMER_ISR; i++)
	{
		if(gTimeBaseC_ISR[i] != NULL)
			gTimeBaseC_ISR[i]();
	}

	if(gstSysTimerEnable == 0)
		return;

#if MAX_SYSTEM_TIMER > 0
	if(gstSysTimerEnable & TIMER0_ENABLE )
	{
		gstSysTimer[0].CurrentTick--;
		if(gstSysTimer[0].CurrentTick == 0)
		{
			//SendMSG(hApMessageQ,MSG_USER_TIMER0,0);
			gstSysTimer[0].msg_qsend(gstSysTimer[0].msg_q_id, gstSysTimer[0].msg_id, NULL, 0, 0);
			gstSysTimer[0].CurrentTick = gstSysTimer[0].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 1
	if(gstSysTimerEnable & TIMER1_ENABLE )
	{
		gstSysTimer[1].CurrentTick--;
		if(gstSysTimer[1].CurrentTick == 0)
		{
			//SendMSG(hApMessageQ,MSG_USER_TIMER1,0);
			gstSysTimer[1].msg_qsend(gstSysTimer[1].msg_q_id, gstSysTimer[1].msg_id, NULL, 0, 0);
			gstSysTimer[1].CurrentTick = gstSysTimer[1].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 2
	if(gstSysTimerEnable & TIMER2_ENABLE )
	{
		gstSysTimer[2].CurrentTick--;
		if(gstSysTimer[2].CurrentTick == 0)
		{
			//SendMSG(hApMessageQ,MSG_USER_TIMER2,0);
			gstSysTimer[2].msg_qsend(gstSysTimer[2].msg_q_id, gstSysTimer[2].msg_id, NULL, 0, 0);
			gstSysTimer[2].CurrentTick = gstSysTimer[2].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 3
	if(gstSysTimerEnable & TIMER3_ENABLE )
	{
		gstSysTimer[3].CurrentTick--;
		if(gstSysTimer[3].CurrentTick == 0)
		{
			//SendMSG(hApMessageQ,MSG_USER_TIMER3,0);
			gstSysTimer[3].msg_qsend(gstSysTimer[3].msg_q_id, gstSysTimer[3].msg_id, NULL, 0, 0);
			gstSysTimer[3].CurrentTick = gstSysTimer[3].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 4
	if(gstSysTimerEnable & TIMER4_ENABLE )
	{
		gstSysTimer[4].CurrentTick--;
		if(gstSysTimer[4].CurrentTick == 0)
		{
			//SendMSG(hApMessageQ,MSG_USER_TIMER4,0);
			gstSysTimer[4].msg_qsend(gstSysTimer[4].msg_q_id, gstSysTimer[4].msg_id, NULL, 0, 0);
			gstSysTimer[4].CurrentTick = gstSysTimer[4].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 5
	if(gstSysTimerEnable & TIMER5_ENABLE )
	{
		gstSysTimer[5].CurrentTick--;
		if(gstSysTimer[5].CurrentTick == 0)
		{
			gstSysTimer[5].msg_qsend(gstSysTimer[5].msg_q_id, gstSysTimer[5].msg_id, NULL, 0, 0);
			gstSysTimer[5].CurrentTick = gstSysTimer[5].Interval;
		}
	}
#endif

#if MAX_SYSTEM_TIMER > 6
	if(gstSysTimerEnable & TIMER6_ENABLE )
	{
		gstSysTimer[6].CurrentTick--;
		if(gstSysTimer[6].CurrentTick == 0)
		{
			gstSysTimer[6].msg_qsend(gstSysTimer[6].msg_q_id, gstSysTimer[6].msg_id, NULL, 0, 0);
			gstSysTimer[6].CurrentTick = gstSysTimer[6].Interval;
		}
	}
#endif
}
