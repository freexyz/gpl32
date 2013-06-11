/*
* Purpose: IR driver/interface
*
* Author: sunxw
*         allenlin modify 2008/06/10
*
* Date: 2008/3/29
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01         
*/

//Include files 
#include "drv_l2_ir.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_IR) && (_DRV_L2_IR == 1)                     //
//================================================================//

#define C_CodeValue_Get_None	0
#define C_CodeValue_Get_First	1
#define C_CodeValue_Get_Repeat	2

// Variables defined in this file 
INT16S iTimetick;
INT32S iReceivecount;
INT32S IR_Status;
INT32S Repeat_Flag;
INT32S Repeatcount;
INT32S OperationCodeValue;
INT32S CodeValueGetStatus = C_CodeValue_Get_None;
INT8S *pSavetimedataaddr;
INT8S  Savetimebuffer[32];


#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)

typedef struct 
{
    INT8U  key_code;
    INT8U   key_type;
} IRkeySYSPara;
IRkeySYSPara ir_key_para;
fp_msg_qsend ir_msg_QSend;
void   *ir_msgQId;
INT32U ir_msgId;

#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

#define IR_KEY_NONE_TYPE    MAX_IR_KEY_TYPES
#define KEY_CODE_NONE   0x0

IR_KEY_TYPES_ENUM  bin_ir_key_type=IR_KEY_NONE_TYPE;
INT8U  bin_ir_key_code=KEY_CODE_NONE;

IR_KEY_TYPES_ENUM bin_ir_press_status_get(void);
INT8U bin_ir_keycode_get(void);
#endif

// Functions defined in this file 
void drv_l2_ir_init(void);
INT32S drv_l2_getirkey(void);
static void 	ir_time_tick(void);
static void 	drv_l2_ir_start_time_with_irq(INT32U freg);
static void 	drv_l2_ir_start_time_without_irq(void);
static INT16S 	drv_l2_ir_get_time_value(void);
//static void 	drv_l2_ir_delay(INT32U delay_time);
static void 	drv_l2_ir_stop_time(void);
static void 	drv_l2_ir_set_extB(INT32S inputedge);
static void 	drv_l2_ir_clr_extB(void);
static INT32S	drv_l2_ir_check_io(void);
static void 	drv_l2_ir_set_io(void);
//static void 	drv_l2_ir_int_set(void);
static void 	ir_extab_int(void);
static void 	drv_l2_ir_decode(void);
static void 	drv_l2_ir_reinit(void);
static void 	drv_l2_ir_sentrepeatcmd(void);

// Function Call Definition Area
#ifdef DRV_L1_SW_ISR_PHY
static INT32U ir_time_start;
#endif

static void drv_l2_ir_start_time_with_irq(INT32U freg)
{
#ifndef DRV_L1_SW_ISR_PHY
	timer_freq_setup(IR_TIMER,freg,0,ir_time_tick);
#else
    sw_timer_freq_setup(IR_TIMER,freg,ir_time_tick);
#endif
}

static void drv_l2_ir_start_time_without_irq(void)
{
#ifndef DRV_L1_SW_ISR_PHY
    timer_counter_reset(IR_TIMER);
	timer_start_without_isr(IR_TIMER,TIMER_SOURCE_4096HZ);
#else
    ir_time_start=tiny_counter_get();
#endif
}

static INT16S drv_l2_ir_get_time_value(void)
{
#ifndef DRV_L1_SW_ISR_PHY
	INT32U time;
	time = TimerCountReturn(IR_TIMER);
	timer_stop(IR_TIMER);
	return (INT16S)time;
#else
    /* the same as dt=dt/915, unit:0.25ms (each tiny is 2.66us) */
    return ((((tiny_counter_get() | 0x10000) - ir_time_start) & (0xFFFF))*10/915);
#endif
}

#if 0
static void drv_l2_ir_delay(INT32U delay_time)
{
	INT32U time;
	do{
		time = TimerCountReturn(IR_TIMER);
	}while(delay_time>=time);	
	timer_stop(IR_TIMER);
}
#endif

static void drv_l2_ir_stop_time(void)
{
#ifndef DRV_L1_SW_ISR_PHY
	timer_stop(IR_TIMER);
#else
    sw_timer_stop(IR_TIMER);
#endif
}
#if 0
static void drv_l2_ir_int_set(void)
{
	vic_irq_register(VIC_EXT_AB,ir_extab_int);
	vic_irq_enable(VIC_EXT_AB);
}
#endif
static void drv_l2_ir_set_extB(INT32S inputedge)
{
	INT32U value;
	value = R_INT_KECON;
	value &= 0x6413;     //keep other flag status.
	if(inputedge==rising_edge)
		value |= 0xa2;	
	else
		value |= 0x82;		
	R_INT_KECON = value;	
}

static void drv_l2_ir_clr_extB(void)
{
	INT32U value;
	value = R_INT_KECON;
	value &= 0x6411;
	value |= 0x0080;
	R_INT_KECON = value;
}

static INT32S drv_l2_ir_check_io(void)
{
	if(gpio_read_io(IOC10)==DATA_HIGH)	
		return 1;
	else
		return 0;	
}

static void drv_l2_ir_set_io(void)
{
	gpio_init_io(IOC10, GPIO_INPUT);
	gpio_set_port_attribute(IOC10,GPIO_INPUT);
	gpio_write_io(IOC10, DATA_HIGH);
}

static void	drv_l2_ir_decode(void)
{
	INT8S  timevalue;
	INT16U check_value;
	INT32S i;
	INT32S CustomCodeValue;
	
	timevalue = 0;
	CustomCodeValue = 0;
	OperationCodeValue = 0;
	
	for(i=0;i<16;i++)
	{
		timevalue = *(INT8S*)(pSavetimedataaddr+i);
		if(timevalue >= 7)
			CustomCodeValue |= (1<<i);
	}
	//DBG_PRINT ("\n\r CustomCodeValue: 0x%x\n\r", CustomCodeValue);
	
	check_value = 0;
	for(i=0;i<16;i++)
	{
		timevalue = *(INT8S*)(pSavetimedataaddr+i+16);
		if(timevalue >= 7)
			OperationCodeValue |= (1<<i);
	}
	check_value = OperationCodeValue & 0x0000ff00; 
	check_value = ~(check_value >>8);
	OperationCodeValue &= 0x00000ff; 

	if(OperationCodeValue == (check_value&0xff))
	{
		CodeValueGetStatus = C_CodeValue_Get_First;
	  #if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)	
	//	DBG_PRINT ("\n\r Key Number ID: 0x%x\n\r", OperationCodeValue);
		//SendMSG( hApMessageQ, MSG_IR_KEY, (OperationCodeValue + MSG_IR_KEY) );
		ir_key_para.key_code = OperationCodeValue;
		ir_key_para.key_type = IR_KEY_DOWN_TYPE;
		ir_msg_QSend(ir_msgQId,ir_msgId,(void*)&ir_key_para,sizeof(IRkeySYSPara),0);
	  #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
	    bin_ir_key_code = OperationCodeValue;
	    bin_ir_key_type = (IR_KEY_TYPES_ENUM) IR_KEY_DOWN_TYPE;
	  #endif
  	}
}
static void drv_l2_ir_sentrepeatcmd(void)
{
	INT32S repeat_count;
	if(CodeValueGetStatus != C_CodeValue_Get_Repeat)
	{
		repeat_count = 10;
	}	
	else
		repeat_count = 2;
		
	Repeatcount++;
	if(Repeatcount >= repeat_count)
	{
    	CodeValueGetStatus = C_CodeValue_Get_Repeat;	
      #if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)	
//		DBG_PRINT ("\n\r Key Number ID: 0x%x\n\r", OperationCodeValue);
		//SendMSG( hApMessageQ, MSG_IR_KEY, (OperationCodeValue + MSG_IR_KEY) );
		ir_key_para.key_code = OperationCodeValue;
		ir_key_para.key_type = IR_KEY_REPEAT_TYPE;
		ir_msg_QSend(ir_msgQId,ir_msgId,(void*)&ir_key_para,sizeof(IRkeySYSPara),0);
		Repeatcount = 0;
      #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
        bin_ir_key_code = OperationCodeValue;
        bin_ir_key_type = (IR_KEY_TYPES_ENUM) IR_KEY_REPEAT_TYPE;
      #endif
	}	
}

static void ir_extab_int(void)
{
#if 0
	INT32U value;
	value = R_INT_KECON;
	if(value & 0x80)
	{
		value &= 0x64b3;
		R_INT_KECON = value;
#endif	
		switch(IR_Status)
		{
			case FindFirstTrigger:
				drv_l2_ir_clr_extB();			
				drv_l2_ir_start_time_with_irq(IR_FREG);
				IR_Status = Check_IR_Header_1;	
				break;
			case Check_IR_Header_1:
				drv_l2_ir_clr_extB();
				iTimetick = 0;
				IR_Status = Check_IR_Header_2;	
				drv_l2_ir_start_time_with_irq(IR_FREG);
				break;
			case Check_IR_Header_2:	
				drv_l2_ir_stop_time();  //stop time irq;
				if(iTimetick<=6) 
				{
					if(Repeat_Flag == 1)
					{
						iTimetick = 0;
						IR_Status = Receive_IR_Repeat_CMD;
						drv_l2_ir_start_time_with_irq(IR_FREG1);				
					}
					else
					{
						drv_l2_ir_clr_extB();
						iTimetick = 0;	
						IR_Status = FindFirstTrigger;	
						drv_l2_ir_set_extB(falling_edge);			
					}
				}		
				else if(iTimetick>=7) 
				{
					iTimetick = 0;			
					iReceivecount = 0x2;
					IR_Status = Receive_IR_Data;
					drv_l2_ir_start_time_without_irq();
				}
				break;
			case Receive_IR_Data:
				iTimetick = drv_l2_ir_get_time_value();
				if((iTimetick > 12)||(iTimetick<3))			//È¥µôÔÓÑ¶¸ÉÈÅ
				{
					iTimetick = 0;	
					IR_Status = FindFirstTrigger;	
					drv_l2_ir_clr_extB();	
					drv_l2_ir_set_extB(falling_edge);	
				}	
				*(INT8S*)(pSavetimedataaddr+(iReceivecount-2))=(INT8S)iTimetick;
				iTimetick = 0;
				iReceivecount++;
				drv_l2_ir_start_time_without_irq();
				if(iReceivecount >= IOTriggerMaxNum)
				{	
					drv_l2_ir_clr_extB();
					iTimetick = 0;	
					drv_l2_ir_decode();
					Repeat_Flag = 1;	
					//drv_l2_ir_delay(0xa4);	//delay 40ms (why delay 40ms?)*/			
					drv_l2_ir_reinit();
				}
				break;
			case Receive_IR_Repeat_CMD:
				drv_l2_ir_stop_time();
				if((iTimetick>=47)&&(iTimetick<50))
				{
					drv_l2_ir_sentrepeatcmd();	
					drv_l2_ir_clr_extB();
					iTimetick = 1;	
					IR_Status = Check_IR_Header_1;	
					drv_l2_ir_start_time_with_irq(IR_FREG);						
				}
				else
				{
					Repeat_Flag = 0;
					drv_l2_ir_clr_extB();
					iTimetick = 1;	
					IR_Status = Check_IR_Header_1;	
					drv_l2_ir_start_time_with_irq(IR_FREG);						
				}		
				break;
		}	
	//}
}

static void ir_time_tick(void)
{
	INT32S ret;
	
	iTimetick++;
	
	switch(IR_Status)
	{
		case Check_IR_Header_1:
			ret = drv_l2_ir_check_io();
			if(ret==0)
			{
				if(iTimetick >= IR_Header_1)
				{
					drv_l2_ir_stop_time();
					drv_l2_ir_set_extB(rising_edge);
				}	
			}
			else
			{
				iTimetick = 0;	
				IR_Status = FindFirstTrigger;	
				drv_l2_ir_stop_time();
				drv_l2_ir_set_extB(falling_edge);
			}
			break;

		case Check_IR_Header_2:
			ret = drv_l2_ir_check_io();
			if(ret == 1)
			{
				if(iTimetick == IR_Header_2)
				{
					drv_l2_ir_set_extB(falling_edge);
				}
				else if(iTimetick > 10)
				{
					iTimetick = 0;	
					IR_Status = FindFirstTrigger;	
					drv_l2_ir_stop_time();
					drv_l2_ir_set_extB(falling_edge);						
				}
			}
			else
			{
				iTimetick = 0;	
				IR_Status = FindFirstTrigger;	
				drv_l2_ir_stop_time();
				drv_l2_ir_set_extB(falling_edge);							
			}
			break;
			
		case Receive_IR_Repeat_CMD:
			if(iTimetick>=50)
			{
				Repeat_Flag = 0;
				drv_l2_ir_stop_time();
				drv_l2_ir_clr_extB();
				drv_l2_ir_reinit();
			}
			break;

		default:
			break;
	}
}

static void drv_l2_ir_reinit(void)
{
	INT32U i;
	iTimetick = 0;	
	iReceivecount = 0;
	IR_Status = FindFirstTrigger;	
	for(i=0;i<32;i++)
		Savetimebuffer[i]=0;	
	drv_l2_ir_set_extB(falling_edge);	
}

void drv_l2_ir_init(void)
{
	INT32U i;
	iTimetick = 0;
	iReceivecount = 0;
	Repeat_Flag = 0;
	Repeatcount = 0;
	OperationCodeValue = 0;
	pSavetimedataaddr = (INT8S *)Savetimebuffer;
	drv_l2_ir_set_io();
	
	//drv_l2_ir_int_set();
	extab_user_isr_set(EXTB,ir_extab_int);
	
	for(i=0;i<32;i++)
		Savetimebuffer[i]=0;
	IR_Status = FindFirstTrigger;	
	//drv_l2_ir_set_extB(falling_edge);
}

INT32S drv_l2_getirkey(void)
{
	return 0;
}

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
void turnkey_irkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
	ir_msg_QSend = (fp_msg_qsend) msg_qsend;
	ir_msgQId = msgq_id;
	ir_msgId = msg_id;
}


#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

IR_KEY_TYPES_ENUM bin_ir_press_status_get(void)
{
    IR_KEY_TYPES_ENUM ret;

    ret = bin_ir_key_type;
    bin_ir_key_type=IR_KEY_NONE_TYPE;
    return ret;
}

INT8U bin_ir_keycode_get(void)
{
    INT8U ret;

    ret = bin_ir_key_code;
    bin_ir_key_code=KEY_CODE_NONE;
    return ret;
}

#endif
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_IR) && (_DRV_L2_IR == 1)                //
//================================================================//