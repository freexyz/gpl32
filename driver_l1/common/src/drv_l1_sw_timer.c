/******************************************************************************
 * Paresent Header Include
 ******************************************************************************/
#include "drv_l1_sw_timer.h"
#include "drv_l1_rtc.h"
#include "drv_l1_sfr.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_SW_TIMER) && (_DRV_L1_SW_TIMER == 1)         //
//================================================================//

#define SW_ISR_TIMER_EN      1


#if SW_ISR_TIMER_EN == 1

#define SW_TIMER_ISR(idx) \
{\
    sw_timer_trigger[idx]++;\
    if (sw_timer_trigger[idx]>=sw_timer_freq[idx]) {\
        (*gloable_sw_timer_isr[idx])();\
        sw_timer_trigger[idx]=0;\
        sw_timer_counter[idx]++;\
    }\
}

#ifndef _SW_TIMER_ID
#define _SW_TIMER_ID

typedef enum
{
    SW_TIMER_0=0,
    SW_TIMER_1,
    SW_TIMER_2,
    SW_TIMER_3,
    SW_TIMER_4,
    SW_TIMER_5,
    SW_TIMER_6,
    SW_TIMER_7,
    SW_TIMER_8,
    SW_TIMER_9,
    SW_TIMER_MAX
} SW_TIMER_ID;

#endif

typedef enum
{
    SW_TIMER_DISABLE,
    SW_TIMER_ENABLE,
    SW_TIMER_ISR,
    SW_TIMER_NOISR,
    SW_TIMER_STATUS_MAX
} SW_TIMER_STATUS;

static INT16U Hz_base=2048;
INT8U sw_timer_status[SW_TIMER_MAX]={0};
INT16U sw_timer_freq[SW_TIMER_MAX]={0};
INT16U sw_timer_trigger[SW_TIMER_MAX]={0};
INT32U sw_timer_counter[SW_TIMER_MAX]={0};
static void (*gloable_sw_timer_isr[SW_TIMER_MAX])(void); 
#endif //#if SW_ISR_TIMER_EN == 1

void sw_timer_delay(INT32U timer_delay_count);
void sw_timer_ms_delay(INT32U msec);
INT8U sw_timer_source_rtc=0;
TIMER_ID_ENUM sw_ref_timer=TIMER_B;

/******************************************************************************
 * Defines
 ******************************************************************************/
#define GP_TIMER_OK   0
#define GP_TIMER_FAIL 0xFFFF

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
INT32U sw_timer_init(INT32U TimerId, INT32U freq_hz);
INT32U sw_timer_get_counter_L(void);
INT32U sw_timer_get_counter_H(void);

#if SW_ISR_TIMER_EN == 1
static void sw_time_pseudo_isr(void);
static void sw_timer_null_isr(void);
void sw_timer_with_isr_phy_init(TIMER_ID_ENUM phy_timer_id, INT16U freq_Hz_base);
void sw_timer_with_isr_phy_uninit(TIMER_ID_ENUM phy_timer_id);
void sw_timer_freq_setup(SW_TIMER_ID sw_timer_id, INT16U freq_Hz, void (*TimerIsrFunction)(void));
void sw_timer_start_without_isr(SW_TIMER_ID sw_timer_id, INT16U freq_Hz);
INT32U sw_timer_counter_get(SW_TIMER_ID sw_timer_id);
void sw_timer_counter_reset(SW_TIMER_ID sw_timer_id);
static INT32U sw_phy_timer_work_status_get(void);
#endif
/******************************************************************************
 * Function Bodies
 ******************************************************************************/
INT32U sw_timer_init(INT32U TimerId, INT32U freq_hz)
{
    sw_timer_count_H=0;
    sw_timer_count_L=0;
    sw_timer_source_rtc=0;
    if (TimerId<4)
    {
        // if (freq_hz>8192 || freq_hz<2) {return GP_TIMER_FAIL;}
        timer_freq_setup(TimerId, freq_hz, 0, sw_timer_isr);
        sw_timer_source_rtc=0; 
    }
    else if (TimerId == 6)  /* RTC schedual timer */
    {
        rtc_callback_set(RTC_SCH_INT_INDEX, sw_timer_isr);
       #if MCU_VERSION < GPL326XX
        rtc_schedule_set(RTC_SCH_1024HZ); 
	    rtc_int_set(RTC_SCH_IEN,(RTC_EN&RTC_SCH_IEN));
	    rtc_function_set(RTC_SCHEN,(RTC_EN&RTC_SCHEN));
       #else
       	rtc_schedule_enable(RTC_SCH_1024HZ);
       #endif
        sw_timer_source_rtc=1;    
    }     
    return GP_TIMER_OK;
}

INT32U timer_counter_init(INT32U TimerId, INT32U freq_hz)
{
    sw_timer_count_H=0;
    sw_timer_count_L=0;
    sw_timer_source_rtc=0;
    if (TimerId<4)
    {
        // if (freq_hz>8192 || freq_hz<2) {return GP_TIMER_FAIL;}
        timer_freq_setup(TimerId, freq_hz, 0, sw_timer_isr);
        sw_timer_source_rtc=0; 
    }
    else if (TimerId == 6)  /* RTC schedual timer */
    {
        rtc_callback_set(RTC_SCH_INT_INDEX, sw_timer_isr);
       #if MCU_VERSION < GPL326XX
        rtc_schedule_set(RTC_SCH_1024HZ); 
	    rtc_int_set(RTC_SCH_IEN,(RTC_EN&RTC_SCH_IEN));
	    rtc_function_set(RTC_SCHEN,(RTC_EN&RTC_SCHEN));
       #else
       	rtc_schedule_enable(RTC_SCH_1024HZ);
       #endif
        sw_timer_source_rtc=1;    
    }
    return GP_TIMER_OK;
}

INT32U sw_timer_get_counter_L(void)
{
    if (sw_timer_source_rtc!=1)
    {return sw_timer_count_L;}
    else if (!(R_RTC_CTRL&RTC_SCH_1024HZ) || !(R_RTC_CTRL&RTC_SCHEN) || !(R_RTC_CTRL&RTC_RTCEN))
    {
        R_RTC_CTRL|=RTC_RTCEN;
        rtc_callback_set(RTC_SCH_INT_INDEX, sw_timer_isr);
       #if MCU_VERSION < GPL326XX
        rtc_schedule_set(RTC_SCH_1024HZ); 
	    rtc_int_set(RTC_SCH_IEN,(RTC_EN&RTC_SCH_IEN));
	    rtc_function_set(RTC_SCHEN,(RTC_EN&RTC_SCHEN));
	   #else
	   	rtc_schedule_enable(RTC_SCH_1024HZ);
       #endif
        return sw_timer_count_L;
    }
    else {return sw_timer_count_L;}
}

INT32U sw_timer_get_counter_H(void)
{
    return sw_timer_count_H;
}

void sw_timer_delay(INT32U timer_delay_count)   /* only used in TimerID=5*/
{
    INT32U timer_count_temp;
    timer_count_temp = sw_timer_count_L;
    while ((sw_timer_count_L-timer_count_temp) < timer_delay_count);
}

void sw_timer_ms_delay(INT32U msec)
{
    INT32S m_sec=(INT32U) msec;
    #if _OPERATING_SYSTEM != _OS_NONE
    //INT32U tick_count;
  
      INT32S timer_counter=0;
      INT32S time_interval=0;
      if (OSRunning==TRUE)
      {
          while ((m_sec>=10) && (m_sec>0))
          {
              timer_counter = sw_timer_get_counter_L();
              OSTimeDly(1);
              time_interval = sw_timer_get_counter_L()-timer_counter;
              m_sec = m_sec - time_interval;
          }
      }
      
    #endif
    
    while (m_sec>0)
    {
        m_sec--;
        TINY_WHILE(0, 375);  /* 1ms = 375 tiny (1ms = 2.666666 x 375)*/
    }
}

void sw_timer_us_delay(INT32U USEC)  //96Mhz
{
#if 1
   INT32S tiny_count = (INT32S) (USEC*1000/2666) + 1;

   while (tiny_count>0)
   {
        tiny_count--;
        TINY_WHILE(0, 1); 
   }
#else
   INT32U usec = USEC*159;
   if (MHZ==48) {usec = 79;}
   else if (MHZ==24) {usec = 39;}

   while (usec>10) {
  	usec-= 10 ;
  }
#endif   
}

#if SW_ISR_TIMER_EN == 1

static void sw_timer_null_isr(void){;;}

void sw_timer_with_isr_phy_init(TIMER_ID_ENUM phy_timer_id, INT16U freq_Hz_base)
{
    INT8U i;

    if (phy_timer_id>TIMER_C || freq_Hz_base>8192 ) {return; /*error config, will cause system ill-work*/}

    sw_ref_timer=phy_timer_id;
    Hz_base = freq_Hz_base;
    for (i=0; i<SW_TIMER_MAX; i++)
    { 
        gloable_sw_timer_isr[i]=sw_timer_null_isr;
        //sw_timer_counter[i]=0;
        sw_timer_status[i]=SW_TIMER_DISABLE;
    }
   // timer_freq_setup(sw_ref_timer, Hz_base, 0, sw_time_pseudo_isr);
    timer_freq_setup_lock(sw_ref_timer);
}

void sw_timer_with_isr_phy_uninit(TIMER_ID_ENUM phy_timer_id)
{
    INT8U i;

    if (phy_timer_id>TIMER_C ) {return; /*error config, will cause system ill-work*/}

    sw_ref_timer=phy_timer_id;
    //Hz_base = freq_Hz_base;
    for (i=0; i<SW_TIMER_MAX; i++)
    { 
        gloable_sw_timer_isr[i]=sw_timer_null_isr;
        //sw_timer_counter[i]=0;
        sw_timer_status[i]=SW_TIMER_DISABLE;
    }

    timer_freq_setup_unlock(sw_ref_timer);
    timer_stop(sw_ref_timer);
}


void sw_timer_freq_setup(SW_TIMER_ID sw_timer_id, INT16U freq_Hz, void (*TimerIsrFunction)(void))
{
    if (freq_Hz==0 || freq_Hz>Hz_base || sw_timer_id>=SW_TIMER_MAX) {return;}

    sw_timer_freq[sw_timer_id]=Hz_base/freq_Hz;
    gloable_sw_timer_isr[sw_timer_id] = TimerIsrFunction;
    if(sw_phy_timer_work_status_get()==FALSE)
    {   
        timer_freq_setup_unlock(sw_ref_timer);
        timer_freq_setup(sw_ref_timer, Hz_base, 0, sw_time_pseudo_isr);
        timer_freq_setup_lock(sw_ref_timer);
    }
    sw_timer_status[sw_timer_id]=SW_TIMER_ENABLE;

}

void sw_timer_start_without_isr(SW_TIMER_ID sw_timer_id, INT16U freq_Hz)
{
    if (freq_Hz==0 || freq_Hz>Hz_base || sw_timer_id>=SW_TIMER_MAX) {return;}

    sw_timer_freq[sw_timer_id]=Hz_base/freq_Hz;
    gloable_sw_timer_isr[sw_timer_id] = sw_timer_null_isr;
    if(sw_phy_timer_work_status_get()==FALSE)
    {
        timer_freq_setup_unlock(sw_ref_timer);
        timer_freq_setup(sw_ref_timer, Hz_base, 0, sw_time_pseudo_isr);
        timer_freq_setup_lock(sw_ref_timer);
    }

        sw_timer_status[sw_timer_id]=SW_TIMER_ENABLE;
}


INT32U sw_timer_counter_get(SW_TIMER_ID sw_timer_id)
{
    return sw_timer_counter[sw_timer_id];
}

void sw_timer_counter_reset(SW_TIMER_ID sw_timer_id)
{
    sw_timer_counter[sw_timer_id]=0;
}
 
void sw_timer_stop(SW_TIMER_ID sw_timer_id)
{
    sw_timer_counter[sw_timer_id]=0;
    sw_timer_status[sw_timer_id]=SW_TIMER_DISABLE;
    gloable_sw_timer_isr[sw_timer_id] = sw_timer_null_isr;
    if(sw_phy_timer_work_status_get()==FALSE) {
        timer_freq_setup_unlock(sw_ref_timer);
        timer_stop(sw_ref_timer);
        timer_freq_setup_lock(sw_ref_timer);
        //timer_freq_setup_unlock(sw_ref_timer);
    }
}

static INT32U sw_phy_timer_work_status_get(void)
{
    INT32U i;
    //return TRUE;
    for (i=0; i<SW_TIMER_MAX; i++)
    { 
        if (sw_timer_status[i]==SW_TIMER_ENABLE)//((gloable_sw_timer_isr[i] != sw_timer_null_isr) ) //|| (sw_timer_counter[i]!=0))
        {
            return TRUE;
        }
    } 
    return FALSE;
}
/* sw time isr */
static void sw_time_pseudo_isr(void)
{
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedLock();
#endif
    SW_TIMER_ISR(SW_TIMER_0);
    SW_TIMER_ISR(SW_TIMER_1);
    SW_TIMER_ISR(SW_TIMER_2);
    SW_TIMER_ISR(SW_TIMER_3);
    SW_TIMER_ISR(SW_TIMER_4);
    SW_TIMER_ISR(SW_TIMER_5);
    SW_TIMER_ISR(SW_TIMER_6);
    SW_TIMER_ISR(SW_TIMER_7);
    SW_TIMER_ISR(SW_TIMER_8);
    SW_TIMER_ISR(SW_TIMER_9);
#if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
#endif    
}
#endif //#if SW_ISR_TIMER_EN == 1    
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_SW_TIMER) && (_DRV_L1_SW_TIMER == 1)    //
//================================================================//
