/* 
* Purpose: RTC driver/interface
*
* Author: Allen Lin
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_rtc.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_RTC) && (_DRV_L1_RTC == 1)                   //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file

// Functions defined in this file
void (*rtc_user_isr[RTC_INT_MAX])(void);

#if MCU_VERSION < GPL326XX

static void rtc_isr()
{
	INT32U int_status = 0;
	INT32U int_ctrl = 0;
	
	int_status = R_RTC_INT_STATUS;
	int_ctrl = R_RTC_INT_CTRL;
		
	if (int_status & RTC_HALF_SEC_IEN) {
		R_RTC_INT_STATUS = RTC_HALF_SEC_IEN;
		if (rtc_user_isr[RTC_HSEC_INT_INDEX] && (int_ctrl & RTC_HALF_SEC_IEN)) {
			(*rtc_user_isr[RTC_HSEC_INT_INDEX])();
		}
	}	
	
	if (int_status & RTC_SEC_IEN) {
		R_RTC_INT_STATUS = RTC_SEC_IEN;
		if (rtc_user_isr[RTC_SEC_INT_INDEX]&& (int_ctrl & RTC_SEC_IEN)) {
			(*rtc_user_isr[RTC_SEC_INT_INDEX])();
		}
	}
	
	if (int_status & RTC_MIN_IEN) {
		R_RTC_INT_STATUS = RTC_MIN_IEN;
		if (rtc_user_isr[RTC_MIN_INT_INDEX]&& (int_ctrl & RTC_MIN_IEN)) {
			(*rtc_user_isr[RTC_MIN_INT_INDEX])();
		}
	}
	
	if (int_status & RTC_HR_IEN) {
		R_RTC_INT_STATUS = RTC_HR_IEN;
		if (rtc_user_isr[RTC_HR_INT_INDEX]&& (int_ctrl & RTC_HR_IEN)) {
			(*rtc_user_isr[RTC_HR_INT_INDEX])();
		}
	}
	
	if (int_status & RTC_DAY_IEN) {
		R_RTC_INT_STATUS = RTC_DAY_IEN;
		if (rtc_user_isr[RTC_DAY_INT_INDEX]&& (int_ctrl & RTC_DAY_IEN)) {
			(*rtc_user_isr[RTC_DAY_INT_INDEX])();
		}
	}
	
	if (int_status & RTC_SCH_IEN) {
		R_RTC_INT_STATUS = RTC_SCH_IEN;
		if (rtc_user_isr[RTC_SCH_INT_INDEX]) {
			(*rtc_user_isr[RTC_SCH_INT_INDEX])();
		}
	}
		
	if (int_status & RTC_ALM_IEN) {
		R_RTC_INT_STATUS = RTC_ALM_IEN;
		if (rtc_user_isr[RTC_ALM_INT_INDEX]) {
			(*rtc_user_isr[RTC_ALM_INT_INDEX])();
		}
	}	
}

void rtc_init(void)
{
	R_RTC_CTRL= 0; /* disable all RTC function */
	R_RTC_CTRL |= RTC_RTCEN; /* enable RTC */
#if BOARD_TYPE >= BOARD_TURNKEY_BASE	
	R_SYSTEM_CTRL |= 0x1000; /* enable SEL30K */
#endif
	vic_irq_register(VIC_ALM_SCH_HMS, rtc_isr);	/* register RTC isr */
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

void rtc_time_set(t_rtc *rtc_time)
{	
	R_RTC_MIN = (INT32U)rtc_time->rtc_min;
	while(R_RTC_BUSY & RTC_MIN_BUSY);
	
	R_RTC_HOUR = (INT32U)rtc_time->rtc_hour;
	while(R_RTC_BUSY & RTC_HR_BUSY);

	R_RTC_SEC = (INT32U)rtc_time->rtc_sec;
	/* wait until not busy */
	while(R_RTC_BUSY & RTC_SEC_BUSY);
}

void rtc_time_get(t_rtc *rtc_time)
{
	rtc_time->rtc_sec = R_RTC_SEC;
	rtc_time->rtc_min = R_RTC_MIN;
	rtc_time->rtc_hour = R_RTC_HOUR;	
}

void rtc_alarm_set(t_rtc *rtc_time)
{
	R_RTC_ALARM_SEC = (INT32U)rtc_time->rtc_sec;
	R_RTC_ALARM_MIN = (INT32U)rtc_time->rtc_min;
	R_RTC_ALARM_HOUR = (INT32U)rtc_time->rtc_hour;
}

void rtc_alarm_get(t_rtc *rtc_time)
{
	rtc_time->rtc_sec = R_RTC_ALARM_SEC;
	rtc_time->rtc_min = R_RTC_ALARM_MIN;
	rtc_time->rtc_hour = R_RTC_ALARM_HOUR;
}

/*******************************************************************************
* DESCRIPTION: This function set a specific RTC function
*      
* INPUT: mask  - 32bit mask value. Each set bit in the mask means that the 
*                value of corresponding rtc function will be set accordingly. Other 
*                function are not affected.
*        value - 32bit value that describes enable or disable per rtc function.
*
* OUTPUT: None.
*
* EXAMPLE:
*       enable alarm function and disable HMS function.
*       set_rtc_function((RTC_ALMEN | RTC_HMSEN), 
*                        ((RTC_EN & RTC_ALMEN)|(RTC_DIS & RTC_HMSEN)));
*
* RETURN: None.
*
*******************************************************************************/
void rtc_function_set(INT32U mask, INT32U value)
{
	R_RTC_CTRL &= ~mask;
	R_RTC_CTRL |= (mask & value);		
}

/*******************************************************************************
* DESCRIPTION: This function set a specific RTC interrupt
*      
* INPUT: mask  - 32bit mask value. Each set bit in the mask means that the 
*                value of corresponding interrupt will be set accordingly. Other 
*                interrupt are not affected.
*        value - 32bit value that describes enable or disable per interrupt.
*
* OUTPUT: None.
*
* EXAMPLE:
*       enable alarm interrupt and disable scheduler interrupt.
*       set_rtc_int((RTC_ALM_IEN | RTC_SCH_IEN), 
*                   ((RTC_EN & RTC_ALM_IEN)|(RTC_DIS & RTC_SCH_IEN)));
*
* RETURN: None.
*
*******************************************************************************/
void rtc_int_set(INT32U mask, INT32U value)
{
	R_RTC_INT_CTRL &= ~mask;
	R_RTC_INT_CTRL |= (mask & value);		
}

void rtc_schedule_set(INT8U freq)
{
	R_RTC_CTRL |= RTC_RTCEN;
	R_RTC_CTRL &= ~RTC_SCHSEL;
	R_RTC_CTRL |= freq;
}

INT32U rtc_irq_flag_get(void)
{
	return R_RTC_INT_STATUS;	
}

BOOLEAN rtc_day_int_get(void)
{
	INT32U int_status = 0;
	
	int_status = R_RTC_INT_STATUS;
	if (int_status & RTC_DAY_IEN) {
		return TRUE;
	}
	
	return FALSE;
}

#else

static void idp_rtc_isr(void)
{
	INT32U int_status =  gpx_rtc_read(0x9);
	INT32U int_ctrl = gpx_rtc_read(0xA);
	INT32U int_en = int_status & int_ctrl; 
	
	if (int_en & GPX_RTC_HALF_SEC_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_HALF_SEC_IEN);
		if (rtc_user_isr[IDP_RTC_HSEC_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_HSEC_INT_INDEX])();
		}
	}

	if (int_en & GPX_RTC_SEC_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_SEC_IEN);
		if (rtc_user_isr[IDP_RTC_SEC_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_SEC_INT_INDEX])();
		}
	}

	if (int_en & GPX_RTC_MIN_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_MIN_IEN);
		if (rtc_user_isr[IDP_RTC_MIN_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_MIN_INT_INDEX])();
		}
	}

	if (int_en & GPX_RTC_HR_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_HR_IEN);
		if (rtc_user_isr[IDP_RTC_HR_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_HR_INT_INDEX])();
		}
	}

	if (int_en & GPX_RTC_DAY_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_DAY_IEN);
		if (rtc_user_isr[IDP_RTC_DAY_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_DAY_INT_INDEX])();
		}
	}

	if (int_en & GPX_RTC_ALM_IEN) {
		gpx_rtc_write(0x9, GPX_RTC_ALM_IEN);
		if (rtc_user_isr[IDP_RTC_ALM_INT_INDEX]) {
			(*rtc_user_isr[IDP_RTC_ALM_INT_INDEX])();
		}
	}
}

static void rtc_isr(void)
{
	INT32U int_status = R_RTC_INT_STATUS;
	INT32U int_ctrl = R_RTC_INT_CTRL;
	INT32U int_en = int_status & int_ctrl;
	
	if (int_en & RTC_HALF_SEC_IEN) {
		R_RTC_INT_STATUS = RTC_HALF_SEC_IEN;
		if (rtc_user_isr[RTC_HSEC_INT_INDEX]) {
			(*rtc_user_isr[RTC_HSEC_INT_INDEX])();
		}
	}

	if (int_en & RTC_SEC_IEN) {
		R_RTC_INT_STATUS = RTC_SEC_IEN;
		if (rtc_user_isr[RTC_SEC_INT_INDEX]) {
			(*rtc_user_isr[RTC_SEC_INT_INDEX])();
		}
	}

	if (int_en & RTC_MIN_IEN) {
		R_RTC_INT_STATUS = RTC_MIN_IEN;
		if (rtc_user_isr[RTC_MIN_INT_INDEX]) {
			(*rtc_user_isr[RTC_MIN_INT_INDEX])();
		}
	}

	if (int_en & RTC_HR_IEN) {
		R_RTC_INT_STATUS = RTC_HR_IEN;
		if (rtc_user_isr[RTC_HR_INT_INDEX]) {
			(*rtc_user_isr[RTC_HR_INT_INDEX])();
		}
	}

	if (int_en & RTC_DAY_IEN) {
		R_RTC_INT_STATUS = RTC_DAY_IEN;
		if (rtc_user_isr[RTC_DAY_INT_INDEX]) {
			(*rtc_user_isr[RTC_DAY_INT_INDEX])();
		}
	}

	if (int_en & RTC_SCH_IEN) {
		R_RTC_INT_STATUS = RTC_SCH_IEN;
		if (rtc_user_isr[RTC_SCH_INT_INDEX]) {
			(*rtc_user_isr[RTC_SCH_INT_INDEX])();
		}
	}

	if (int_en & RTC_ALM_IEN) {
		R_RTC_INT_STATUS = RTC_ALM_IEN;
		if (rtc_user_isr[RTC_ALM_INT_INDEX]) {
			(*rtc_user_isr[RTC_ALM_INT_INDEX])();
		}
	}

#if MCU_VERSION < GPL327XX	
	idp_rtc_isr();
#endif		
}

/**
* @brief	init rtc and idp rtc
* @param 	none
* @return 	none
*/
void rtc_init(void)
{
	INT8U data;

	/* disable all RTC function */
	R_RTC_CTRL= 0;

	/* enable RTC */
	R_RTC_CTRL |= RTC_HMSEN | RTC_RTCEN;
	
	/* clear int */
	R_RTC_INT_STATUS = 0x3F;

	/* disable & clear idp int */
	gpx_rtc_write(0xA, 0x00);
	gpx_rtc_write(0x9, 0x3F);
	
	/* enable external RTC */
	data = gpx_rtc_read(0x08);
	data |= 0x18;
	gpx_rtc_write(0x08,data);

	/* reset level=1.7V */
	rtc_reset_trigger_level_set(0);
	
	/* register RTC isr */
	vic_irq_register(VIC_ALM_SCH_HMS, rtc_isr);
	vic_irq_enable(VIC_ALM_SCH_HMS);
#if MCU_VERSION >= GPL327XX
	vic_irq_register(VIC_FD, idp_rtc_isr);
	vic_irq_enable(VIC_FD);
#endif
}

/**
* @brief	set rtc and idp rtc time
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_time_set(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

	gpx_rtc_write(0x0,rtc_time->rtc_sec); // sec
	gpx_rtc_write(0x1,rtc_time->rtc_min); // min
	gpx_rtc_write(0x2,rtc_time->rtc_hour); // hour

	R_RTC_MIN = (INT32U)rtc_time->rtc_min;
	while(R_RTC_BUSY & RTC_MIN_BUSY);

	R_RTC_HOUR = (INT32U)rtc_time->rtc_hour;
	while(R_RTC_BUSY & RTC_HR_BUSY);

	R_RTC_SEC = (INT32U)rtc_time->rtc_sec;
	/* wait until not busy */
	while(R_RTC_BUSY & RTC_SEC_BUSY);

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	get rtc or idp rtc time
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_time_get(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

#if 1
	rtc_time->rtc_sec = gpx_rtc_read(0x0);
	rtc_time->rtc_min = gpx_rtc_read(0x1);
	rtc_time->rtc_hour = gpx_rtc_read(0x2);
#else
	rtc_time->rtc_sec = R_RTC_SEC;
	rtc_time->rtc_min = R_RTC_MIN;
	rtc_time->rtc_hour = R_RTC_HOUR;
#endif

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set idp rtc day
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_day_set(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

	gpx_rtc_write(0x3,(INT8U)(rtc_time->rtc_day & 0xff));
	gpx_rtc_write(0x4,(INT8U)((rtc_time->rtc_day >> 8) & 0xf));

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	get idp rtc day
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_day_get(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

	rtc_time->rtc_day = gpx_rtc_read(0x3);
	rtc_time->rtc_day |= (gpx_rtc_read(0x4) & 0xf) << 8;

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set idp rtc alarm time
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_alarm_set(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

#if 1
	gpx_rtc_write(0x5,rtc_time->rtc_sec);
	gpx_rtc_write(0x6,rtc_time->rtc_min);
	gpx_rtc_write(0x7,rtc_time->rtc_hour);
#else
	R_RTC_ALARM_SEC = (INT32U)rtc_time->rtc_sec;
	R_RTC_ALARM_MIN = (INT32U)rtc_time->rtc_min;
	R_RTC_ALARM_HOUR = (INT32U)rtc_time->rtc_hour;
#endif

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	get idp rtc alarm time
* @param 	rtc_time[in]: time 
* @return 	none
*/
void rtc_alarm_get(t_rtc *rtc_time)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

#if 1
	rtc_time->rtc_sec = gpx_rtc_read(0x5);
	rtc_time->rtc_min = gpx_rtc_read(0x6);
	rtc_time->rtc_hour = gpx_rtc_read(0x7);
#else
    rtc_time->rtc_sec = R_RTC_ALARM_SEC;
	rtc_time->rtc_min = R_RTC_ALARM_MIN;
	rtc_time->rtc_hour = R_RTC_ALARM_HOUR;
#endif

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	direct set idp rtc alarm pin high or low
* @param 	level[in]: 1 is high; 0 is low.
* @return 	none
*/
void idp_rtc_alm_pin_set(INT32U level)
{
	INT8U data;

	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

	data = gpx_rtc_read(0x08);
	data &= ~(1 << 0);
	if(level) {
		data |= (1 << 5);
	} else {
		data &= ~(1 << 5);
	}
	gpx_rtc_write(0x08, data);

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set alarm pin enable to high when alarm happen,  
* @brief	this funtion is for system need wake up by alarm pin
* @param 	enable[in]: 1 is enable; 0 is disable.
* @return 	none
*/
void idp_rtc_alm_pin_wakeup_set(INT32U enable)
{
	INT8U data;

	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif

	data = gpx_rtc_read(0x08);
	data &= ~(1 << 5);
	if(enable) {
		data |= (1 << 0);
	} else {
		data &= ~(1 << 0);
	}
	gpx_rtc_write(0x08, data);

	//let alm pin active must enable alarm irq 
	if(enable) {
		data = gpx_rtc_read(0x0A);
		data |= GPX_RTC_ALM_IEN;
		gpx_rtc_write(0x0A,data);
	}

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set idp rtc control register
* @param 	mask[in]: bit
* @param 	value[in]: set value
* @return 	none
*/
void rtc_function_set(INT8U mask, INT8U value)
{
	INT8U data = 0;

	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif
	data = gpx_rtc_read(0x08);
	data &= ~mask;
	data |= (mask & value);

	gpx_rtc_write(0x08,data);
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set idp rtc reset level
* @param 	value[in]: 0: 1.7V, 1: 1.9V, 2: 1.5V, 3: 1.3V
* @return 	none
*/
void rtc_reset_trigger_level_set(INT8U value)
{
	INT8U data = 0;

	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif
	data = gpx_rtc_read(0x08);
	data &= ~0x6;
	data |= (0x3 & value) << 1;

	gpx_rtc_write(0x08,data);
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set rtc interrupt enable
* @param 	mask[in]: bit
* @param 	value[in]: set value
* @return 	none
*/
void rtc_int_set(INT32U mask, INT32U value)
{
    INT32U _mask = 0;
    INT32U _value = 0;

    _mask = mask & 0xF;
    _value = value & 0xF;

    if (mask & RTC_ALM_IEN) {
        _mask |= RTC_ALM_IEN;
    }

    if (mask & RTC_DAY_IEN) {
        _mask |= RTC_DAY_IEN;
    }

    if (value & RTC_ALM_IEN) {
        _value |= RTC_ALM_IEN;
        R_RTC_CTRL |= RTC_ALMEN;
    }
    
    if (value & RTC_DAY_IEN) {
        _value |= RTC_DAY_IEN;
    }
		
    // enable int
    R_RTC_INT_CTRL &= ~_mask;
	R_RTC_INT_CTRL |= (_mask & _value);
}

/**
* @brief	set idp interrupt clear
* @param 	value[in]: clear value
* @return 	none
*/
void rtc_int_clear(INT32U value)
{
	R_RTC_INT_STATUS = value;
}

/**
* @brief	set idp rtc interrupt enable
* @param 	mask[in]: bit
* @param 	value[in]: set value
* @return 	none
*/
void idp_rtc_int_set(INT8U mask, INT8U value)
{
	INT8U data = 0;

	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif	

	data = gpx_rtc_read(0x0A);
	data &= ~mask;
	data |= (mask & value);
	gpx_rtc_write(0x0A,data);

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	set idp rtc interrupt clear
* @param 	value[in]: clear value
* @return 	none
*/
void idp_rtc_int_clear(INT8U value)
{
	vic_irq_disable(VIC_ALM_SCH_HMS);
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif	

	gpx_rtc_write(0x09, value);

#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	vic_irq_enable(VIC_ALM_SCH_HMS);
}

/**
* @brief	read idp rtc register
* @param 	addr[in]: register address
* @return 	register value
*/
INT8U gpx_rtc_read(INT8U addr)
{
	INT8U cmd = 0;
	INT8U data;

	cmd = addr << 3;

	R_RTC_IDPWR_ADDR = cmd;
	R_RTC_IDPWR_CTRL |= 0x1;

#if 1
	TINY_WHILE((R_RTC_IDPWR_CTRL_FLAG & 1) == 0,375);
#else	
	while(R_RTC_IDPWR_CTRL_FLAG & 1);
#endif
	data = (INT8U) R_RTC_IDPWR_RDATA;
	return data;
}

/**
* @brief	write idp rtc register
* @param 	addr[in]: register address
* @param 	data[in]: register value
* @return 	none 
*/
void gpx_rtc_write(INT8U addr,INT8U data)
{
	INT8U cmd = 1;
	cmd |= (addr << 3);

	R_RTC_IDPWR_ADDR = cmd;

	R_RTC_IDPWR_WDATA = data;
	R_RTC_IDPWR_CTRL |= 0x1;

#if 1
	TINY_WHILE((R_RTC_IDPWR_CTRL_FLAG & 1) == 0,375);
#else	
	while(R_RTC_IDPWR_CTRL_FLAG & 1);
#endif
}

/**
* @brief	enable rtc schedule 
* @param 	freq[in]: RTC_SCH_16HZ ~ RTC_SCH_2048HZ
* @return 	none
*/
void rtc_schedule_enable(INT8U freq)
{
	R_RTC_CTRL |= RTC_EN;
	R_RTC_CTRL &= ~RTC_SCHSEL;
	R_RTC_CTRL |= freq;
	R_RTC_CTRL |= RTC_SCHEN;
	R_RTC_INT_CTRL |= RTC_SCH_IEN;
}

/**
* @brief	disable rtc schedule 
* @param 	none
* @return 	none
*/
void rtc_schedule_disable(void)
{
	R_RTC_CTRL &= ~RTC_SCHEN;
	R_RTC_INT_CTRL &= ~RTC_SCH_IEN;
}

void rtc_ext_to_int_set(void)
{
	R_RTC_SEC = gpx_rtc_read(0x0);
	while(R_RTC_BUSY & RTC_SEC_BUSY);
	
	R_RTC_MIN = gpx_rtc_read(0x1);
	while(R_RTC_BUSY & RTC_MIN_BUSY);
	
	R_RTC_HOUR = gpx_rtc_read(0x2);
	while(R_RTC_BUSY & RTC_HR_BUSY);
}
#endif

INT32S rtc_callback_set(INT8U int_idx, void (*user_isr)(void))
{
	INT32S status = STATUS_OK;

	if (int_idx > RTC_INT_MAX) {
		return STATUS_FAIL;
	}

#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif
	if (rtc_user_isr[int_idx] != 0) {
		status = STATUS_FAIL;
	}
	else {
		rtc_user_isr[int_idx] = user_isr;
	}
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif

	return status;
}

INT32S rtc_callback_clear(INT8U int_idx)
{
	if (int_idx > RTC_INT_MAX) {
		return STATUS_FAIL;
	}
#if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
#endif
	rtc_user_isr[int_idx] = 0;
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif
	return STATUS_OK;
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_RTC) && (_DRV_L1_RTC == 1)              //
//================================================================//
