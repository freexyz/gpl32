/* created by allenlin */
#include "drv_l1_gpy0200.h"

INT16U ALARM_Trigger_By_GY0200 = 0;
INT16U Day_Trigger_By_GY0200 = 0;
#if _DRV_L1_GPY0200 == 1

INT8U irq_mask;
INT8U int_status;

//extern ap_alarm_value;

static void gpy0200_rtc_reset_time_set(void);
void (*gpy0200_user_isr[GPY0200_USER_ISR_MAX])(void);

void gpy0200_critical_section_protect(void);
void gpy0200_critical_section_unprotect(void);

static void gpy0200_isr(void)
{
	INT8U status;
	INT8U data;
	
	status = gpy0200_i2c_read(0x02);
	
	if (status == 0xFF) {
		status = gpy0200_i2c_read(0x02);
		if (status == 0xFF) {
			return;
		}
	}
	
    if (status & GPY0200_INT_ON_KEY1) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ON_KEY1);
    	if (gpy0200_user_isr[GPY0200_USER_ISR_ONKEY1]) {
			(*gpy0200_user_isr[GPY0200_USER_ISR_ONKEY1])();
		}
    }
    
    if (status & GPY0200_INT_ADP) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ADP);
    	if (gpy0200_user_isr[GPY0200_USER_ISR_ADAPTER]) {
			(*gpy0200_user_isr[GPY0200_USER_ISR_ADAPTER])();
		}
    }
    
    if (status & GPY0200_INT_BAT34) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_BAT34);
   		if (gpy0200_user_isr[GPY0200_USER_ISR_BAT34]) {
			(*gpy0200_user_isr[GPY0200_USER_ISR_BAT34])();
		}
    }
    
    if (status & GPY0200_INT_ALM) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ALM);
    	if (gpy0200_user_isr[GPY0200_USER_ISR_ALARM]) {
			(*gpy0200_user_isr[GPY0200_USER_ISR_ALARM])();
		}
    }
    
    if (status & GPY0200_INT_HMS) {
    	data = gpy0200_i2c_read(0x72);
    	if (data & GPY0200_RTC_HALF_SEC_IEN) {
			gpy0200_i2c_write(0x72,GPY0200_RTC_HALF_SEC_IEN);
			if (gpy0200_user_isr[GPY0200_USER_ISR_HSEC]) {
				(*gpy0200_user_isr[GPY0200_USER_ISR_HSEC])();
			}
		}
		if (data & GPY0200_RTC_SEC_IEN) {
			gpy0200_i2c_write(0x72,GPY0200_RTC_SEC_IEN);
			if (gpy0200_user_isr[GPY0200_USER_ISR_SEC]) {
				(*gpy0200_user_isr[GPY0200_USER_ISR_SEC])();
			}
		}
		if (data & GPY0200_RTC_MIN_IEN) {
			gpy0200_i2c_write(0x72,GPY0200_RTC_MIN_IEN);
			if (gpy0200_user_isr[GPY0200_USER_ISR_MIN]) {
				(*gpy0200_user_isr[GPY0200_USER_ISR_MIN])();
			}
		}
		if (data & GPY0200_RTC_HR_IEN) {
			gpy0200_i2c_write(0x72,GPY0200_RTC_HR_IEN);
			if (gpy0200_user_isr[GPY0200_USER_ISR_HOUR]) {
				(*gpy0200_user_isr[GPY0200_USER_ISR_HOUR])();
			}
		}
		if (data & GPY0200_RTC_DAY_IEN) {
			gpy0200_i2c_write(0x72,GPY0200_RTC_DAY_IEN);
			if (gpy0200_user_isr[GPY0200_USER_ISR_DAY]) {
				(*gpy0200_user_isr[GPY0200_USER_ISR_DAY])();
			}
		}
    }
		
    if (status & GPY0200_INT_SCH) {
		gpy0200_i2c_write(0x72,GPY0200_RTC_SCH_IEN);
		if (gpy0200_user_isr[GPY0200_USER_ISR_SCH]) {
			(*gpy0200_user_isr[GPY0200_USER_ISR_SCH])();
		}
	}
}

void gpy0200_init(void)
{
	INT8U data;	
	INT8U status;

#if (BOARD_TYPE == BOARD_TK_V4_1) || (BOARD_TYPE == BOARD_GPY0200_EMU_V2_0) || (BOARD_TYPE == BOARD_TK35_V1_0) || (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) || (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY)
	R_MEM_IO_CTRL &= ~0x80; /* disable XA12 to IOG9 */
#else
	gpio_set_bkueb(FALSE); /* disable BKUEB to IOG0 */
#endif
	/* release all cs pin to io, BKOEB -> IOH0 */	
	gpio_set_memcs(0,0);
	gpio_set_memcs(1,0);
	gpio_set_memcs(2,0);
	gpio_set_memcs(3,0);
	
	drv_msec_wait(2);

#if (BOARD_TYPE == BOARD_TK_V4_1) || (BOARD_TYPE == BOARD_GPY0200_EMU_V2_0) || (BOARD_TYPE == BOARD_TK35_V1_0) || (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) || (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY)
	gpy0200_i2c_init(IO_H0,IO_G9); /* SCL(IOH0),SDA(IOG9) */
#elif (BOARD_TYPE == BOARD_EMU_V2_0)
	gpy0200_i2c_init(IO_B8,IO_B10); /* SCL(IOB8),SDA(IOB10) */
#else
	gpy0200_i2c_init(IO_H0,IO_G0); /* SCL(IOH0),SDA(IOG0) */
#endif

	data = gpy0200_i2c_read(0x0);
	data = gpy0200_i2c_read(0x0);
	
	//DBG_PRINT("GPY0200 chip id: %x\r\n",gpy0200_i2c_read(0x0));
	//DBG_PRINT("status A: 0x%x\r\n",gpy0200_i2c_read(0x3));
	//DBG_PRINT("status B: 0x%x\r\n",gpy0200_i2c_read(0x4));
	//DBG_PRINT("status C: 0x%x\r\n",gpy0200_i2c_read(0x5));
	//DBG_PRINT("status D: 0x%x\r\n",gpy0200_i2c_read(0x6));
	
	data = gpy0200_i2c_read(0x2);
	gpy0200_i2c_write(0x02,GPY0200_INT_ON_KEY1); /* clear INT_ONKEY1 */
	
	
	if (data & GPY0200_INT_ON_KEY2) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ON_KEY2); /* clear INT_ON_KEY2 */
    }	
	if (data & GPY0200_INT_ADP) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ADP); /* clear INT_ADP */
    	if ((gpy0200_i2c_read(0x04) & 0x4) == 0) { /* plug-out */
    		if (sys_ir_opcode_get() == 0xFFFF0000) {
    			system_enter_halt_mode();
    		}
    	}
    }	
    if (data & GPY0200_INT_ALM) {
    	gpy0200_i2c_write(0x02,GPY0200_INT_ALM); /* clear INT_ALM */
    	ALARM_Trigger_By_GY0200 = TRIGGER_ALARM;
    }
    
     status = gpy0200_i2c_read(0x02);
     if (status & GPY0200_INT_HMS) 
     {
     	data = gpy0200_i2c_read(0x72);
     	if (data & GPY0200_RTC_DAY_IEN) 
     	{
			gpy0200_i2c_write(0x72,GPY0200_RTC_DAY_IEN);
			Day_Trigger_By_GY0200 = TRIGGER_DAY;
		}
     }
    
    irq_mask = gpy0200_i2c_read(0x1);
    
	data = gpy0200_i2c_read(0x06);
	
	if (data & OVT_OFF) {
		gpy0200_i2c_write(0x06,OVT_OFF);
	}
	if (data & BAT32_OFF) {
		gpy0200_i2c_write(0x06,BAT32_OFF);
	}
	if (data & KEY_SD) {
		gpy0200_i2c_write(0x06,KEY_SD);
	}
	
	if (data & POR_ON) {
		gpy0200_i2c_write(0x06,POR_ON);
		gpy0200_rtc_reset_time_set();
	}
	
	gpy0200_i2c_write(0x09,0x30); /* enable LDOA,B and BAT_SEL=2.0V */

	gpy0200_i2c_write(0x0a,0x01); /* enable audio */
	
	gpy0200_i2c_write(0x71,0x0); //clear RTC ctrl(RTC bit7 is always 1,so RTC can not disable */
	gpy0200_i2c_write(0x73,0x0); //disable all interrupt
	gpy0200_i2c_write(0x72,0xFF); //clear all interrupt flag

#if 0//BOARD_TYPE == BOARD_GPY0200_EMU_V2_0
    data = gpy0200_i2c_read(0xb); /* switch to PWM1 */
	data |= 0x10;
	gpy0200_i2c_write(0xb,data);
	
    data = gpy0200_i2c_read(0xb); /* enable boost */
	data |= 0x1;
	gpy0200_i2c_write(0xb,data);
#endif

	extab_user_isr_set(EXTA,gpy0200_isr);
}

void gpy0200_rtc_time_set(t_rtc *rtc_time,INT8U mask)
{
	gpy0200_critical_section_protect();
	
	if (mask & 0x2) {
		gpy0200_i2c_write(0x61,(INT8U)rtc_time->rtc_min);
	}
	
	if (mask & 0x4) {
		gpy0200_i2c_write(0x62,(INT8U)rtc_time->rtc_hour);
	}

	if (mask & 0x1) {
		gpy0200_i2c_write(0x60,(INT8U)rtc_time->rtc_sec);
	}
	
	if (mask & 0x8) {
		gpy0200_i2c_write(0x63,(INT8U)(rtc_time->rtc_day & 0xff));
		gpy0200_i2c_write(0x64,(INT8U)((rtc_time->rtc_day >> 8) & 0xf));
	}
	gpy0200_critical_section_unprotect();
}

void gpy0200_rtc_hms_get(t_rtc *rtc_time)
{
	gpy0200_critical_section_protect();
	rtc_time->rtc_sec = gpy0200_i2c_read(0x60);
	rtc_time->rtc_min = gpy0200_i2c_read(0x61);
	rtc_time->rtc_hour = gpy0200_i2c_read(0x62);
	gpy0200_critical_section_unprotect();
}

void gpy0200_rtc_day_get(t_rtc *rtc_time)
{
	gpy0200_critical_section_protect();
	rtc_time->rtc_day = gpy0200_i2c_read(0x63);	
	rtc_time->rtc_day |= (gpy0200_i2c_read(0x64) & 0xf) << 8;
	gpy0200_critical_section_unprotect();
}

void gpy0200_rtc_alarm_set(t_rtc *rtc_time, INT8U mask)
{
	gpy0200_critical_section_protect();
	if (mask & 0x1) {
		gpy0200_i2c_write(0x65,(INT8U)rtc_time->rtc_sec);
	}
	
	if (mask & 0x2) {
		gpy0200_i2c_write(0x66,(INT8U)rtc_time->rtc_min);
	}
	
	if (mask & 0x4) {
		gpy0200_i2c_write(0x67,(INT8U)rtc_time->rtc_hour);
	}
	
	if (mask & 0x8) {
		gpy0200_i2c_write(0x68,(INT8U)(rtc_time->rtc_day & 0xff));
		gpy0200_i2c_write(0x69,(INT8U)((rtc_time->rtc_day >> 8) & 0xf));
	}
	gpy0200_critical_section_unprotect();
}

void gpy0200_rtc_alarm_get(t_rtc *rtc_time)
{
	gpy0200_critical_section_protect();
	rtc_time->rtc_sec = gpy0200_i2c_read(0x65);
	rtc_time->rtc_min = gpy0200_i2c_read(0x66);
	rtc_time->rtc_hour = gpy0200_i2c_read(0x67);	
	rtc_time->rtc_day = gpy0200_i2c_read(0x68);	
	rtc_time->rtc_day |= (gpy0200_i2c_read(0x69) & 0xf) << 8;
	gpy0200_critical_section_unprotect();
}

/* ex:disable day compare
   gpy0200_rtc_alarm_condition_set(GPY0200_EN_DAY,RTC_DIS&GPY0200_EN_DAY);
*/
void gpy0200_rtc_alarm_condition_set(INT8U mask,INT8U value)
{
	INT8U data = 0;
	
	gpy0200_critical_section_protect();
	data = gpy0200_i2c_read(0x70);
	data &= ~mask;
	data |= (mask & value);	
	
	gpy0200_i2c_write(0x70,data);
	gpy0200_critical_section_unprotect();	
}

INT32S gpy0200_callback_set(INT8U int_idx, void (*user_isr)(void))
{
	INT32S status = STATUS_OK;
	
	if (int_idx > GPY0200_USER_ISR_MAX) {
		return STATUS_FAIL;
	}
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif	

	gpy0200_user_isr[int_idx] = user_isr;
	
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif

	return status;
}

void gpy0200_rtc_function_set(INT8U mask, INT8U value)
{
	INT8U data = 0;
	
	gpy0200_critical_section_protect();
	data = gpy0200_i2c_read(0x71);
	data &= ~mask;
	data |= (mask & value);	
	
	gpy0200_i2c_write(0x71,data);
	gpy0200_critical_section_unprotect();
}

void gpy0200_rtc_int_set(INT8U mask, INT8U value)
{
	INT8U data = 0;
	gpy0200_critical_section_protect();
	data = gpy0200_i2c_read(0x73);
	data &= ~mask;
	data |= (mask & value);	
	
	gpy0200_i2c_write(0x73,data);
	gpy0200_critical_section_unprotect();
}

void gpy0200_irq_mask_set(INT8U mask, INT8U value)
{
	gpy0200_critical_section_protect();
	
	irq_mask &= ~mask;
	irq_mask |= (mask & value);	
	
	gpy0200_critical_section_unprotect();	
}

static void gpy0200_rtc_reset_time_set(void)
{
	//gpy0200_i2c_write(0x60,40);
	//gpy0200_i2c_write(0x61,59);
	//gpy0200_i2c_write(0x62,23);
	gpy0200_i2c_write(0x63,0);
	gpy0200_i2c_write(0x64,0);
}

void gpy0200_ldo33a_turn_off(void)
{ 
	INT8U data;
	
	gpy0200_i2c_write(0x72,0xFF); //clear all interrupt flag of RTC
    
    data = gpy0200_i2c_read(0x09);
    data &= ~0x10;
    
    gpy0200_i2c_write(0x09,data); /* disable gpy0200 LDO33A */
 
}

void gpy0200_ldo33b_set(BOOLEAN status)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x09);
	
	if (status == TRUE) {
		data |= 0x20; /* enable */
	}
	else {
	 	data &= ~0x20; /* disable */
	}
	gpy0200_i2c_write(0x09,data);
 	gpy0200_critical_section_unprotect();	
}

void gpy0200_audio_en_set(BOOLEAN status)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x0a);
	
	if (status == TRUE) {
		data |= 0x1; /* enable */
	}
	else {
	 	data &= ~0x1; /* disable */
	}
	gpy0200_i2c_write(0x0a,data);
 	gpy0200_critical_section_unprotect();	
}

void gpy0200_ldo50_en_set(BOOLEAN status)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x09);
	
	if (status == TRUE) {
		data |= 0x40; /* enable */
	}
	else {
	 	data &= ~0x40; /* disable */
	}
	gpy0200_i2c_write(0x09,data);
 	gpy0200_critical_section_unprotect();	
}

INT8U gpy0200_battery_level_get(void)
{
	INT8U data;
	INT8U level;
	
	gpy0200_critical_section_protect();
	
	level = gpy0200_i2c_read(0x03);
	level >>= 2;
	level &= 0x3;
	
	if (level == 3) {
		data = gpy0200_i2c_read(0x04);
		if (data & 0x1) {
			level = GPY0200_BAT_LEVEL_FULL;
		}	
	}
 	gpy0200_critical_section_unprotect();
 	
 	return level;		
}

INT8U gpy0200_adapter_plug_status_get(void)
{
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x04);
		
	gpy0200_critical_section_unprotect();
 	
 	if (data & 0x04) {
 		return GPY0200_ADAPTER_IN;
 	}
 	else {
 		return GPY0200_ADAPTER_OUT;
 	}
}

void gpy0200_charger_current_set(INT8U value)
{ 
	gpy0200_critical_section_protect();
	
	gpy0200_i2c_write(0x08,value);
	
 	gpy0200_critical_section_unprotect();	
}

INT8U gpy0200_charger_current_get(void)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x08);
	
 	gpy0200_critical_section_unprotect();
 	
 	return data;	
}

void gpy0200_tft_en_set(BOOLEAN status)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x20);
	
	if (status == TRUE) {
		data |= 0x1; /* enable */
		data &= ~0x40; /* set OEH to hi */
	}
	else {
	 	data &= ~0x1; /* disable */
	 	data |= 0x40; /* set OEH to low to save power */
	}
	gpy0200_i2c_write(0x20,data);
 	gpy0200_critical_section_unprotect();	
}

void gpy0200_vdac_en_set(BOOLEAN status)
{ 
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0xc);
	
	if (status == TRUE) {
		data |= 0x1; /* enable */
	}
	else {
	 	data &= ~0x1; /* disable */
	}
	gpy0200_i2c_write(0xc,data);
 	gpy0200_critical_section_unprotect();	
}

void gpy0200_soft_reset(void)
{
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	data = gpy0200_i2c_read(0x20);
	data |= 0x10; /* soft reset */
	gpy0200_i2c_write(0x20,data);
	drv_msec_wait(1);
	data &= ~0x10;
	gpy0200_i2c_write(0x20,data);
	
 	gpy0200_critical_section_unprotect();		
}

void gpy0200_aging_pattern_en_set(BOOLEAN status,INT8U index)
{
	INT8U data;
	
	gpy0200_critical_section_protect();
	
	if (status == TRUE) {
		if (index > 0xF) {
			gpy0200_critical_section_unprotect();
			return;
		}
		data = index | 0x10;	
	}
	else {
		data = 0;
	}
	gpy0200_i2c_write(0x32,data);
	
 	gpy0200_critical_section_unprotect();		
}

void gpy0200_sram_data_wirte(INT8U offset,INT8U *data,INT8U len)
{
	INT8U i,t; 
	
	if (offset > 127) {
		return;
	}	
	t = offset;
	
	gpy0200_critical_section_protect();
	for (i=0;i<len;i++) {
		gpy0200_i2c_write(0x80+t,*data++);
		if (t++>127) {
			break;
		}
	}
 	gpy0200_critical_section_unprotect();		
}

void gpy0200_sram_data_read(INT8U offset,INT8U *data,INT8U len)
{
	INT8U i,t; 
	
	if (offset > 127) {
		return;
	}	
	t = offset;
	
	gpy0200_critical_section_protect();
	for (i=0;i<len;i++) {
		*data = gpy0200_i2c_read(0x80+t);
		data++;
		if (t++>127) {
			break;
		}
	}
 	gpy0200_critical_section_unprotect();		
}

void gpy0200_critical_section_protect(void)
{
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif	
	
	if (R_INT_KECON & 1) {
		extab_enable_set(EXTA,FALSE);
	}
	else {
		int_status = 1;
	}	
	gpy0200_i2c_write(0x01,0);
}

void gpy0200_critical_section_unprotect(void)
{
	gpy0200_i2c_write(0x01,irq_mask);
	if (int_status == 0) {
		extab_enable_set(EXTA,TRUE);
	}
	else {
		int_status = 0;
	}	
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedUnlock();
#endif	
}

#endif