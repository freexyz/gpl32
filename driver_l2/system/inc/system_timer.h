#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include "driver_l2.h"

#define MAX_SYSTEM_TIMER_ISR	4

#define TIMER0_ENABLE		0x1
#define TIMER1_ENABLE		0x2
#define TIMER2_ENABLE		0x4
#define TIMER3_ENABLE		0x8
#define TIMER4_ENABLE		0x10
#define TIMER5_ENABLE		0x20
#define TIMER6_ENABLE		0x40

#define USER_TIMER0_ENABLE	0x080
#define USER_TIMER1_ENABLE	0x100
#define USER_TIMER2_ENABLE	0x200
#define USER_TIMER3_ENABLE	0x400


#define TIMER_ID_INVALID 	-1


typedef struct
{
	fp_msg_qsend	msg_qsend;
	void			*msg_q_id;
	INT32U			msg_id;
	INT32U			Interval;
	INT32U			CurrentTick;
} STSYSTIMER;

extern void sys_init_timer(void);
extern INT32S sys_set_timer(void* msg_qsend, void* msg_q_id, INT32U msg_id, INT32U timerid, INT32U interval);
extern INT32S sys_kill_timer(INT32U timerid);
extern INT32S sys_kill_all_timer(void);
extern INT32S sys_128Hz_timer_init(void);
extern INT32S sys_registe_timer_isr(void (*TimerIsrFunction)(void));
extern INT32S sys_release_timer_isr(void (*TimerIsrFunction)(void));

#endif		// __SYSTEM_TIMER_H__
