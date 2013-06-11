#include "drv_l1_matrescan.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_KEYSCAN) && (_DRV_L1_KEYSCAN == 1)           //
//================================================================//


static INT32U R_NowProssKeyID;			
static INT32U R_DebounceTimes;			
static INT32U R_KeyStatus;
static INT32U R_DelayTime;
static INT32U R_KeyUpTimers;
INT32U KS_finish_Flag;
INT32U KS_Number;
INT32U KS_DATA;

static void keyscan_isr(void);
static void check_keyvalue(void);
static void keyscan_isr1(void);
void matre_keyscaninit(void); 
void matre_scaninit(void);

void matre_scaninit(void)
{
	KS_finish_Flag = 0;
	KS_Number = 0;
	KS_DATA = 0;
	
	//R_IOA_DIR = 0x00ff;
	//R_IOA_ATTRIB = 0x00ff;
	//R_IOA_BUFFER = 0x00ff;
	
	R_TIMERD_PRELOAD = 0x10000-(48000000/256/60);
	R_TIMERD_CTRL = 0xa062;
	
	vic_irq_register(VIC_KEY_SCAN, keyscan_isr1);
	vic_irq_enable(VIC_KEY_SCAN);
	R_KEYSCAN_CTRL0 = C_KS_INT_Status | C_KS_INT_Enable | C_KS_AUTO |C_KS_SMART| C_KS_STIME_128 | C_KS_TSEL_TimerD;	
	R_KEYSCAN_CTRL0 = C_KS_Key_Status|C_KS_Velocity_Enable|C_KS_64Key_Enable;
}

void keyscan_isr1(void)
{ 
	INT16U keyport;
	INT32U count;
	INT32U keynum;
	volatile INT32U *portaddr;
	
	keynum = 0;
	portaddr = P_KEYSCAN_DATA0;
	for(count=0;count<8;count++)
	{
		keyport = *(portaddr+count);
		if(keyport != 0)
		{
			KS_finish_Flag |= keyport;
			*(portaddr+count) = keyport;
			keyport &= 0x00ff;
			while(keyport)
			{
				keynum++;
				keyport = keyport>>1;			
			}
			keynum--;
			keynum |= 0x8000;
			R_KEYSCAN_ADDR = keynum;
			
			while(R_KEYSCAN_ADDR&0x8000);
			KS_DATA = R_KEYSCAN_VELOCITY;
		}	
		keynum += 8;
	}		
	R_KEYSCAN_CTRL0 = C_KS_INT_Status | C_KS_INT_Enable | C_KS_AUTO |C_KS_SMART| C_KS_STIME_128 | C_KS_TSEL_TimerD;	
}
//*******************************************************************************//
void matre_keyscaninit(void) 
{
	R_NowProssKeyID = C_ClearBuffer;
	R_DebounceTimes = C_LongTimes;
	R_KeyStatus = C_ClearBuffer;    
	R_DelayTime = C_ClearBuffer;  
	R_KeyUpTimers = C_ConfirmUpTimers;
	  
	R_IOA_DIR = 0x00ff;
	R_IOA_ATTRIB = 0x00ff;
	R_IOA_BUFFER = 0x00ff;
	
	R_TIMERD_PRELOAD = 0x10000-(48000000/256/60);
	R_TIMERD_CTRL = 0xa062;
		
	vic_irq_register(VIC_KEY_SCAN, keyscan_isr);
	vic_irq_enable(VIC_KEY_SCAN);
	R_KEYSCAN_CTRL0 = C_KS_INT_Status | C_KS_INT_Enable | C_KS_AUTO |C_KS_SMART| C_KS_STIME_128 | C_KS_TSEL_TimerD;
}   
    
   
    
void keyscan_isr(void)
{   
	INT16U keyport;
	INT32U count;
	INT32U keynum;
	volatile INT32U *portaddr;
	
	R_KEYSCAN_CTRL0 = C_KS_INT_Status|C_KS_STIME_128|C_KS_TSEL_TimerD;

	keynum = 0;
	portaddr = P_KEYSCAN_DATA0;
	for(count=0;count<8;count++)
	{
		keyport = *(portaddr+count);
		if(keyport != 0)
		{
			*(portaddr+count) = keyport;
			keyport &= 0x00ff;
			while(keyport)
			{
				keynum++;
				keyport = keyport>>1;			
			}
			R_NowProssKeyID = keynum;
			time_base_setup(2, 0, 0, check_keyvalue);			
			R_KeyStatus = C_GETKEY;
			R_DebounceTimes = C_LongTimes;			
			return;
		}
			
		keynum += 8;
	}	
	R_NowProssKeyID = 0xffff;
	R_KEYSCAN_CTRL0 = C_KS_INT_Status | C_KS_INT_Enable | C_KS_AUTO | C_KS_SMART | C_KS_STIME_128 | C_KS_TSEL_TimerD;
}   

void check_keyvalue(void)
{
	INT32U comm;
	volatile INT32U * portaddr;
	INT16U keyport;
	
	portaddr = P_KEYSCAN_DATA0;
	comm = 0;
	
	R_KEYSCAN_CTRL0 |= C_KS_STRSCAN;
	while(R_KEYSCAN_CTRL0 & C_KS_BUSY);		
			
	if(R_KeyStatus != C_ClearBuffer)
	{
		comm = R_NowProssKeyID / 9;
		keyport = *(portaddr+comm);	
		comm = (R_NowProssKeyID-1) % 8;
		if(!(keyport & (1<<comm))) //up
		{
			R_KeyUpTimers --;
			if(R_KeyUpTimers == C_ClearBuffer)
			{
				//sysSendMSG	_hDispatchQ,MSG_INT_EXTKEY_UP,[R_NowProssKeyID]
				R_NowProssKeyID = C_ClearBuffer;
				R_DebounceTimes = C_LongTimes;
				R_KeyStatus = C_ClearBuffer;    
				R_DelayTime = C_ClearBuffer;  
				R_KeyUpTimers = C_ConfirmUpTimers;
				R_KEYSCAN_CTRL0 = C_KS_INT_Status | C_KS_INT_Enable | C_KS_AUTO | C_KS_SMART | C_KS_STIME_128 | C_KS_TSEL_TimerD;
				time_base_stop(2);
			}
		}
		else //down
		{
			R_KeyUpTimers = C_ConfirmUpTimers;
			if(R_KeyStatus == C_GETKEY)
			{
				R_DebounceTimes --;
				if(R_DebounceTimes == C_ClearBuffer)
				{
					//SysSendMSG	_hDispatchQ,MSG_INT_EXTKEY_DOWN,[R_NowProssKeyID]
					R_KeyStatus = C_CONFIRMPRESSED;
					R_DelayTime = C_ConfirmRepeatCounts;			
				}		
			}
			else if(R_KeyStatus == C_CONFIRMPRESSED)
			{
				R_DelayTime --;
				if(R_DelayTime == C_ClearBuffer)
				{
					//SysSendMSG	_hDispatchQ,MSG_INT_EXTKEY,[R_NowProssKeyID]
					R_KeyStatus = C_REPEAT;
					R_DelayTime = C_RepeatAgainCounts;			//64		
				}
			}
			else if(R_KeyStatus == C_REPEAT)
			{
				R_DelayTime --;
				if( R_DelayTime == C_ClearBuffer)
				{
					//SysSendMSG	_hDispatchQ,MSG_INT_EXTKEY,[R_NowProssKeyID]
					R_KeyStatus = C_REPEAT;
					R_DelayTime = C_RepeatAgainCounts;			//64
				}	
			}
		}		
	}

}


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_KEYSCAN) && (_DRV_L1_KEYSCAN == 1)      //
//================================================================//