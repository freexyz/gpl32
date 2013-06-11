#ifndef __drv_l1_MATRESCAN_H__
#define __drv_l1_MATRESCAN_H__
#include "driver_l1.h"
#include "drv_l1_sfr.h"

// R_KEYSCAN_CTRL0 (0xC0050000)
#define C_KS_INT_Status				0x8000
#define C_KS_INT_Enable				0x4000
#define C_KS_AUTO					0x2000
#define C_KS_FIXS_TIME				0x1000
#define C_KS_INV					0x0800
#define C_KS_SMART					0x0400
#define C_KS_STRSCAN				0x0200
#define C_KS_BUSY					0x0100
#define C_KS_STOP					0x0080
#define C_KS_B74_OFF				0x0040
#define C_KS_B31_OFF				0x0020
#define C_KS_B0_OFF					0x0010
#define C_KS_STIME_16				0x0000		//0= 16 System Clocks
#define C_KS_STIME_32				0x0004		//1= 32 System Clocks
#define C_KS_STIME_64				0x0008		//2= 64 System Clocks
#define C_KS_STIME_128				0x000C		//3= 128 System Clocks
#define C_KS_TSEL_TimerC			0x0000		//0= Trigger Source is TimerC
#define C_KS_TSEL_TimerD			0x0001		//1= Trigger Source is TimerD
#define C_KS_TSEL_TimerE			0x0002		//2= Trigger Source is TimerE
#define C_KS_TSEL_TimerF			0x0003		//3= Trigger Source is TimerF

// R_KEYSCAN_CTRL1 (0xC0050004)
#define C_KS_64Key_Enable			0x0000
#define C_KS_88Key_Enable			0x0001
#define C_KS_Velocity_Disable		0x0000
#define C_KS_Velocity_Enable		0x0002
#define C_KS_Key_Status				0x0004

#define C_UP					0
#define C_DOWN					1
#define C_ClearBuffer			0x0000
#define C_ConfirmUpTimers		0x0008
#define C_ConfirmRepeatCounts	28
#define C_RepeatAgainCounts		64
#define C_LongTimes				64
#define C_GETKEY				0x0001
#define C_CONFIRMPRESSED		0x0002
#define C_REPEAT				0x0003

extern void matre_keyscaninit(void);
extern void matre_scaninit(void);

#endif /* __drv_l1_MATRESCAN_H__ */
