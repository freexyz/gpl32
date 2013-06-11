/*
* Purpose: ad key driver
*
* Author: zhangzha
*
* Date: 2008/04/28
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*          
* Note: AD key driver use system timer manager api, so if you want to use ad key, 
* you must initial the system timer first.
* The system time tick is 128Hz
*
* Version : 1.01 
* History :
          1. 2008/4/28 zhangzha create.
          2. 2008/6/10 allen lin modify.
*/

//Include files
#include "ad_key.h"

INT32U ad_key_count;
INT32U ad_key_value;
INT8U  is_key_down;
INT8U  ad_key_start_repeat;
INT8U  ad_key_repeat_count;

// Variables defined in this file
#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
static INT32U key_press;
static INT32U key_repeat;
INT32U adkey_press_get(INT32U key_mask);
INT32U adkey_repeat_get(INT32U key_mask);
#endif
// Functions defined in this file
static void ad_key_timer_isr(void);
static void ad_key_adc_isr(INT16U ad_val);

typedef struct 
{
    INT8U  key_code;
    INT8U  key_type;
} ADkeySYSPara;

ADkeySYSPara key_para;
  
fp_msg_qsend msg_QSend;
void *msgQId;
INT32U msgId;

void ad_key_initial(void)
{
	ad_key_count = 0;
	is_key_down = 0;
    ad_key_value = C_INVALID_KEY;
#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
    key_press = 0;
	key_repeat = 0;
#endif
	ad_key_start_repeat = C_AD_KEY_START_REPEAT;
	ad_key_repeat_count = C_AD_KEY_REPEAT_COUNT;
	
	adc_init();
	adc_manual_ch_set(C_AD_KEY_CH);
	adc_manual_callback_set(ad_key_adc_isr);
#if _OPERATING_SYSTEM == 1
	sys_registe_timer_isr(ad_key_timer_isr);
#endif
}

void ad_key_uninitial(void)
{
#if _OPERATING_SYSTEM == 1
	sys_release_timer_isr(ad_key_timer_isr);
#endif
}

void ad_key_set_repeat_time(INT8U start_repeat, INT8U repeat_count)
{
	ad_key_start_repeat = start_repeat;
	ad_key_repeat_count = repeat_count;
}

void ad_key_get_repeat_time(INT8U *start_repeat, INT8U *repeat_count)
{
	*start_repeat = ad_key_start_repeat;
	*repeat_count = ad_key_repeat_count;
}

#if _OPERATING_SYSTEM == 1
static void ad_key_timer_isr(void)
{
	adc_manual_sample_start();	
}

static void ad_key_adc_isr(INT16U ad_val)
{
	INT32U	key;
	
	ad_val >>= 6;

#if SUPPORT_AD_KEY == KEY_AD_4_MODE
	if(ad_val < C_AD_VALUE_1) {
		key = AD_KEY_4;
	} else if(ad_val < C_AD_VALUE_2) {
		key = AD_KEY_3;
	} else if(ad_val < C_AD_VALUE_3) {
		key = AD_KEY_2;
	} else if(ad_val < C_AD_VALUE_4) {
		key = AD_KEY_1;
#elif SUPPORT_AD_KEY == KEY_AD_5_MODE
	if(ad_val < C_AD_VALUE_1) {
		key = AD_KEY_1;
	} else if(ad_val < C_AD_VALUE_2) {
		key = AD_KEY_2;
	} else if(ad_val < C_AD_VALUE_3) {
		key = AD_KEY_3;
	} else if(ad_val < C_AD_VALUE_4) {
		key = AD_KEY_4;
	} else if(ad_val < C_AD_VALUE_5) {
		key = AD_KEY_5;
#else
 #if BOARD_TYPE == BOARD_TK_32600_V1_0
	if(ad_val < C_AD_VALUE_1) {
		key = AD_KEY_6;
	} else if(ad_val < C_AD_VALUE_2) {
		key = AD_KEY_5;
	} else if(ad_val < C_AD_VALUE_3) {
		key = AD_KEY_3;
	} else if(ad_val < C_AD_VALUE_4) {
		key = AD_KEY_2;
 #elif BOARD_TYPE == BOARD_DEMO_GPL327XX
 	if(ad_val < C_AD_VALUE_0) {
 		key = C_INVALID_KEY;	// no key pressed
 	} else if(ad_val < C_AD_VALUE_1) {
 		key = AD_KEY_1;
	} else if(ad_val < C_AD_VALUE_2) {
		key = AD_KEY_2;
	} else if(ad_val < C_AD_VALUE_3) {
		key = AD_KEY_3;
	} else if(ad_val < C_AD_VALUE_4) {
		key = AD_KEY_4;
 #else
 	if(ad_val < C_AD_VALUE_1) {
 		key = AD_KEY_1;
	} else if(ad_val < C_AD_VALUE_2) {
		key = AD_KEY_2;
	} else if(ad_val < C_AD_VALUE_3) {
		key = AD_KEY_3;
	} else if(ad_val < C_AD_VALUE_4) {
		key = AD_KEY_4;
 #endif
#endif
#if (SUPPORT_AD_KEY == KEY_AD_6_MODE) || (SUPPORT_AD_KEY == KEY_AD_8_MODE) 
 #if BOARD_TYPE == BOARD_TK_32600_V1_0
 	} else if(ad_val < C_AD_VALUE_5) {
		key = AD_KEY_1;
	} else if(ad_val < C_AD_VALUE_6) {
		key = AD_KEY_4;
 #else
	} else if(ad_val < C_AD_VALUE_5) {
		key = AD_KEY_5;
	} else if(ad_val < C_AD_VALUE_6) {
		key = AD_KEY_6;
 #endif
#endif
#if SUPPORT_AD_KEY == KEY_AD_8_MODE
	} else if(ad_val < C_AD_VALUE_7) {
		key = AD_KEY_7;
	} else if(ad_val < C_AD_VALUE_8) {
		key = AD_KEY_8;
#endif	
	} else {
		key = C_INVALID_KEY;	// no key pressed
	}

	if(key == C_INVALID_KEY) {
		if (is_key_down) {
			key_para.key_code = ad_key_value;
			key_para.key_type = RB_KEY_UP;
            #if (defined _PROJ_TURNKEY) //&& (_PROJ_TYPE == _PROJ_TURNKEY)	// marked by Bruce, 2009/02/23
			msg_QSend(msgQId,msgId,(void*)&key_para,sizeof(ADkeySYSPara),0);
            #endif
			is_key_down = 0;
		}
		ad_key_count = 0;
		ad_key_value = key;
		return;
	}
	
	if(key == ad_key_value) {
		ad_key_count += 1;
		if(ad_key_count == C_AD_KEY_DEBOUNCE) {
			is_key_down = 1;
			key_para.key_code = key;
			key_para.key_type = RB_KEY_DOWN;
            #if (defined _PROJ_TURNKEY) //&& (_PROJ_TYPE == _PROJ_TURNKEY)	// marked by Bruce, 2009/02/23
			msg_QSend(msgQId,msgId,(void*)&key_para,sizeof(ADkeySYSPara),0);
            #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
            key_press |= (1<<key);
            #endif
        } else if(ad_key_count == ad_key_start_repeat) {
			key_para.key_code = key;
			key_para.key_type = RB_KEY_REPEAT;
            #if (defined _PROJ_TURNKEY) //&& (_PROJ_TYPE == _PROJ_TURNKEY)
			msg_QSend(msgQId,msgId,(void*)&key_para,sizeof(ADkeySYSPara),0);
            #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
            key_repeat |= (1<<key);
            #endif
			ad_key_count -= ad_key_repeat_count;
		}
	} else {
		ad_key_count = 0;
		ad_key_value = key;
	}
}

void turnkey_adkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
	msg_QSend = (fp_msg_qsend) msg_qsend;
	msgQId = msgq_id;
	msgId = msg_id;
}

#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
INT32U adkey_press_get(INT32U key_mask)
{
	vic_irq_disable(VIC_TMB_ABC);
	key_mask &= key_press;                       
	key_press ^= key_mask;  /* clear */                
	vic_irq_enable(VIC_TMB_ABC);
	
	return key_mask;
}


INT32U adkey_repeat_get(INT32U key_mask)
{
	vic_irq_disable(VIC_TMB_ABC);
	key_mask &= key_repeat;             	
	key_repeat ^= key_mask; /* clear */
	vic_irq_enable(VIC_TMB_ABC);
	
	return key_mask;
}
#endif

#endif
