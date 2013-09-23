#ifndef __AD_KEY_DRIVER_H__
#define __AD_KEY_DRIVER_H__

#include "driver_l2.h"

#if (MCU_VERSION >= GPL327XX) && ((BOARD_TYPE == BOARD_DEMO_GPL327XX) || (BOARD_TYPE == BOARD_DEMO_GP327XXXA))
	#define C_AD_KEY_CH				1
#elif (BOARD_TYPE == BOARD_DEMO_GPL326XXB) || (BOARD_TYPE == BOARD_DEMO_GP326XXXA)
	#define C_AD_KEY_CH				3
#else
	#define C_AD_KEY_CH				0	
#endif

// 10bit valid value

#if SUPPORT_AD_KEY == KEY_AD_4_MODE
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			77
	#define C_AD_VALUE_2			248
	#define C_AD_VALUE_3			434
	#define C_AD_VALUE_4			651
#elif SUPPORT_AD_KEY == KEY_AD_5_MODE
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			93
	#define C_AD_VALUE_2			310
	#define C_AD_VALUE_3			496
	#define C_AD_VALUE_4			713
	#define C_AD_VALUE_5			930
#elif SUPPORT_AD_KEY == KEY_AD_6_MODE
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			170 - 85
	#define C_AD_VALUE_2			341 - 85
	#define C_AD_VALUE_3			511 - 85
	#define C_AD_VALUE_4			682 - 85
	#define C_AD_VALUE_5			852 - 85
	#define C_AD_VALUE_6			1023 - 85
#elif (SUPPORT_AD_KEY == KEY_AD_8_MODE)
#if (MCU_VERSION >= GPL327XX) && ((BOARD_TYPE == BOARD_DEMO_GPL327XX) || (BOARD_TYPE == BOARD_DEMO_GP327XXXA))
	#define C_AD_VALUE_0			10
	#define C_AD_VALUE_1			130+40
	#define C_AD_VALUE_2			190+40 
	#define C_AD_VALUE_3			350+40
	#define C_AD_VALUE_4			450+40 
	#define C_AD_VALUE_5			630+40
	#define C_AD_VALUE_6			700+40
	#define C_AD_VALUE_7			850+40
	#define C_AD_VALUE_8			1024
#elif 0//(MCU_VERSION <= GPL326XX_C) 
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			70
	#define C_AD_VALUE_2			250+40
	#define C_AD_VALUE_3			480+40 
	#define C_AD_VALUE_4			610+40
	#define C_AD_VALUE_5			720+40 
	#define C_AD_VALUE_6			810+40
	#define C_AD_VALUE_7			890+40
	#define C_AD_VALUE_8			960+40

#elif (MCU_VERSION >= GPL326XXB) && ((BOARD_TYPE == BOARD_DEMO_GPL326XXB) || (BOARD_TYPE == BOARD_DEMO_GP326XXXA))
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			70
	#define C_AD_VALUE_2			127+40
	#define C_AD_VALUE_3			280+40 
	#define C_AD_VALUE_4			410+40
	#define C_AD_VALUE_5			520+40 
	#define C_AD_VALUE_6			650+40
	#define C_AD_VALUE_7			760+40
	#define C_AD_VALUE_8			880+40

#else
	#define C_AD_VALUE_0			0
	#define C_AD_VALUE_1			70
	#define C_AD_VALUE_2			127+40
	#define C_AD_VALUE_3			255+40
	#define C_AD_VALUE_4			380+40
	#define C_AD_VALUE_5			511+40
	#define C_AD_VALUE_6			638+40
	#define C_AD_VALUE_7			766+40
	#define C_AD_VALUE_8			892+40
#endif									  
#endif

#define C_AD_KEY_DEBOUNCE		4//3	// 3 * 1 / 128 = 23ms

#if _PROJ_TYPE == _PROJ_TURNKEY && 0
	#define C_AD_KEY_START_REPEAT	128	// 128 * 1 / 128 = 1s
	#define C_AD_KEY_REPEAT_COUNT	64	// 64 * 1 / 128 = 0.5s
#else
	#define C_AD_KEY_START_REPEAT	96//64	// 128 * 1 / 128 = 0.5s
	#define C_AD_KEY_REPEAT_COUNT	12//8	// 8 * 1 / 128 = 0.5s
#endif

#define C_INVALID_KEY			0xffffffff

extern void ad_key_initial(void);
extern void ad_key_uninitial(void);
extern INT32U ad_key_get_key(void);
#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
extern INT32U adkey_press_get(INT32U key_mask);
extern INT32U adkey_repeat_get(INT32U key_mask);
#endif
extern void ad_key_set_repeat_time(INT8U start_repeat, INT8U repeat_count);
extern void ad_key_get_repeat_time(INT8U *start_repeat, INT8U *repeat_count);

#endif