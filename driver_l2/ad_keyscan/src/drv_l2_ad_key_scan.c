#include "drv_l2_ad_key_scan.h"

#define MSG_AD_KEY	1
#define KEY_QUEUE_MAX	2
#define KEY_QUEUE_MAX_MSG_LEN   20

MSG_Q_ID	KeyTaskQ;
INT32U ADKEY_IO1, ADKEY_IO2, ADKEY_IO3, ADKEY_IO4;
INT32U ADKEY_IO5, ADKEY_IO6, ADKEY_IO7, ADKEY_IO8;
INT32U ADKEY_IO1_C, ADKEY_IO2_C, ADKEY_IO3_C, ADKEY_IO4_C;
INT32U ADKEY_IO5_C, ADKEY_IO6_C, ADKEY_IO7_C, ADKEY_IO8_C;

extern void turnkey_adkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);

INT32U adc_key_scan_init(void)
{
	KeyTaskQ = msgQCreate(KEY_QUEUE_MAX, KEY_QUEUE_MAX, KEY_QUEUE_MAX_MSG_LEN);
	turnkey_adkey_resource_register((void*)msgQSend, KeyTaskQ, MSG_AD_KEY);
	
	ad_key_initial();
	
	return 0;
}

INT32U adc_key_scan(void)
{
	INT8U  KeyPara[2];
	INT32U msg_id;
	msgQReceive(KeyTaskQ, &msg_id, KeyPara, sizeof(KeyPara));
	
	if(msg_id != MSG_AD_KEY)
		return -1;
		
	if(KeyPara[1] == RB_KEY_DOWN)
	{
		switch(KeyPara[0])
		{
		case AD_KEY_1:	ADKEY_IO1 = 1;	break;
		case AD_KEY_2:	ADKEY_IO2 = 1;	break;
		case AD_KEY_3:	ADKEY_IO3 = 1;	break;
		case AD_KEY_4:	ADKEY_IO4 = 1;	break;
		case AD_KEY_5:	ADKEY_IO5 = 1;	break;
		case AD_KEY_6:	ADKEY_IO6 = 1;	break;
		case AD_KEY_7:	ADKEY_IO7 = 1;	break;
		case AD_KEY_8:	ADKEY_IO8 = 1;	break;				
		}
		//DBG_PRINT("PD = %d\r\n", KeyPara[0]);
	}
	else if(KeyPara[1] == RB_KEY_REPEAT)
	{
		switch(KeyPara[0])
		{
		case AD_KEY_1:	ADKEY_IO1_C = 1;	break;
		case AD_KEY_2:	ADKEY_IO2_C = 1;	break;
		case AD_KEY_3:	ADKEY_IO3_C = 1;	break;
		case AD_KEY_4:	ADKEY_IO4_C = 1;	break;
		case AD_KEY_5:	ADKEY_IO5_C = 1;	break;
		case AD_KEY_6:	ADKEY_IO6_C = 1;	break;
		case AD_KEY_7:	ADKEY_IO7_C = 1;	break;
		case AD_KEY_8:	ADKEY_IO8_C = 1;	break;
		}
		//DBG_PRINT("PR = %d\r\n", KeyPara[0]); 
	}
	else if(KeyPara[1] == RB_KEY_UP)
	{
		ADKEY_IO1 = 0;
		ADKEY_IO2 = 0;
		ADKEY_IO3 = 0;
		ADKEY_IO4 = 0;				
		ADKEY_IO5 = 0;
		ADKEY_IO6 = 0;
		ADKEY_IO7 = 0;
		ADKEY_IO8 = 0;
	
		ADKEY_IO1_C = 0;
		ADKEY_IO2_C = 0;
		ADKEY_IO3_C = 0;
		ADKEY_IO4_C = 0;				
		ADKEY_IO5_C = 0;
		ADKEY_IO6_C = 0;
		ADKEY_IO7_C = 0;
		ADKEY_IO8_C = 0;
		//DBG_PRINT("PU\r\n"); 	
	}
	return 0;
}