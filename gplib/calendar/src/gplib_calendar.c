/* 
* Purpose: Calendar
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
#include "gplib_graphic_calendar.h"
#include "gplib_calendar.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined GPLIB_CALENDAR_EN) && (GPLIB_CALENDAR_EN == 1)       //
//================================================================//

//#define AUG_8TH_2008        2454687 /* julian date for system init time */
//#define DEC_31TH_1799       2378496 /*julian date at 1799/12/31 for conversion */
//#define JAN_1ST_1900        2415021
#define DEC_31TH_1979        2444239 
#define DEC_31TH_2007        2454466

#if SUPPORT_GPY02XX != GPY02XX_NONE || MCU_VERSION >= GPL326XX 
INT32U LAST_JULIAN_DATE = DEC_31TH_2007;
#else
INT32U LAST_JULIAN_DATE = DEC_31TH_1979;
#endif

#if MCU_VERSION >= GPL326XX 
void     (*jd_store)(INT32U);
INT32U   (*jd_get)(void);
void     (*config_store)(void);
#endif
 
#define INT_RTC             0x80000000

/*global variable */
extern  INT32U    day_count;

INT8S     monday[]= {0,31,29,31,30,31,30,31,31,30,31,30,31};

ALARM_T   alarm_event[ALARM_MAX_NUM] = {0};

//static void alarm_callback(void);
//static INT32S set_next_alarm(void);

#if MCU_VERSION >= GPL326XX || SUPPORT_GPY02XX != GPY02XX_NONE 
static void cal_last_jd_store(INT32U last_jd);
static INT32U cal_last_jd_get(void);
static INT32U cal_day_get(void);
#endif

static void cal_day_set(INT32U day);
static void cal_rtc_time_set(t_rtc *rtc_time);
static void cal_rtc_time_get(t_rtc *rtc_time);
static INT32U date_to_jd(INT32S yyyy,INT32S mm,INT32S dd);

/* time zone adj*/
#define SUPPORT_TIME_ZONE_ADJUST 0

#if SUPPORT_TIME_ZONE_ADJUST == 1
INT8U     time_zone = 50;
int timezone_adj[] =
{
      0         
    ,-(12*60+00)  /* 1(GMT-12:00) Eniwetok,Kwajalain                                         */
    ,-(11*60+00)  /* 2(GMT-11:00) Midway Island,Samoa                                        */
    ,-(10*60+00)  /* 3(GMT-10:00) Hawaii                                                     */
    ,-( 9*60+00)  /* 4(GMT-09:00) Alaska                                                     */
    ,-( 8*60+00)  /* 5(GMT-08:00) Pacific Time (US & Canada);Tijuana                         */
    ,-( 7*60+00)  /* 6(GMT-07:00) Arizona                                                    */
    ,-( 7*60+00)  /* 7(GMT-07:00) Mountain Time (US & Canada)                                */
    ,-( 6*60+00)  /* 8(GMT-06:00) Central Time (US & Canada)                                 */
    ,-( 6*60+00)  /* 9(GMT-06:00) Mexico City,Tegucigalpa                                    */
    ,-( 6*60+00)  /* 10(GMT-06:00) Saskatchewan                                               */
    ,-( 5*60+00)  /* 11(GMT-05:00) Bogota,Lima,Ouito                                          */
    ,-( 5*60+00)  /* 12(GMT-05:00) Eastern Time (US & Canada)                                 */
    ,-( 5*60+00)  /* 13(GMT-05:00) Indiana (East)                                             */
    ,-( 4*60+00)  /* 14(GMT-04:00) Atlantic Time(Canada)                                      */
    ,-( 4*60+00)  /* 15(GMT-04:00) Caracas,La Paz                                             */
    ,-( 4*60+00)  /* 16(GMT-04:00) Sasntiago                                                  */
    ,-( 3*60+30)  /* 17(GMT-03:30) Newfoundlasnd                                              */
    ,-( 3*60+00)  /* 18(GMT-03:00) Brasilia                                                   */
    ,-( 3*60+00)  /* 19(GMT-03:00) Buenos Aires,Georgetown                                    */
    ,-( 2*60+00)  /* 20(GMT-02:00) Mid-Atlantic                                               */
    ,-( 1*60+00)  /* 21(GMT-01:00) Azores,Cape Verde Is.                                      */
    ,0            /* 22(GMT)       Casablance, Monrovia                                       */
    ,0            /* 23(GMT)       Greenwich Mean Time:Dublin,Edinburgh,Lisbon,London         */
    ,( 1*60+00)   /* 24(GMT+01:00) Amsterdam,Berlin,Bern,Rome,Stockholm,Vienna                */
    ,( 1*60+00)   /* 25(GMT+01:00) Belgrade,Bratislava,Budapest,Ljubljana,Prague              */
    ,( 1*60+00)   /* 26(GMT+01:00) Brussels,Copenhagen,Madrid,Paris,Vilnius                   */
    ,( 1*60+00)   /* 27(GMT+01:00) Sarajevo,Skopje,Sofija,Warsaw,Zagreb                       */
    ,( 2*60+00)   /* 28(GMT+02:00) Athens,Istanbul,Minsk                                      */
    ,( 2*60+00)   /* 29(GMT+02:00) Bucharest                                                  */
    ,( 2*60+00)   /* 30(GMT+02:00) Cairo                                                      */
    ,( 2*60+00)   /* 31(GMT+02:00) Harare,Pretoria                                            */
    ,( 2*60+00)   /* 32(GMT+02:00) Helsinki,riga,Tallinn                                      */
    ,( 2*60+00)   /* 33(GMT+02:00) Jerusalem                                                  */
    ,( 3*60+00)   /* 34(GMT+03:00) Baghdad,kuwait,Riyadh                                      */
    ,( 3*60+00)   /* 35(GMT+03:00) Moscow,St.Petersburg,Volgograd                             */
    ,( 3*60+00)   /* 36(GMT+03:00) Nairobi                                                    */
    ,( 3*60+30)   /* 37(GMT+03:30) Tehran                                                     */
    ,( 4*60+00)   /* 38(GMT+04:00) Abu Dhabi,Muscat                                           */
    ,( 4*60+00)   /* 39(GMT+04:00) Baku,Tbilisi                                               */
    ,( 4*60+30)   /* 40(GMT+04:30) Kabul                                                      */
    ,( 5*60+00)   /* 41(GMT+05:00) Ekaterinburg                                               */
    ,( 5*60+00)   /* 42(GMT+05:00) Islamabad,Karachi,Tashkent                                 */
    ,( 5*60+30)   /* 43(GMT+05:30) Bombay,Calcutta,Madras,New Delhi                           */
    ,( 6*60+00)   /* 44(GMT+06:00) Astana,Almaty,Dhaka                                        */
    ,( 6*60+00)   /* 45(GMT+06:00) Colombo                                                    */
    ,( 7*60+00)   /* 46(GMT+07:00) Bangkok,Hanoi,Jakarta                                      */
    ,( 8*60+00)   /* 47(GMT+08:00) Beijing,Chongqing, Hong Kong,Urumqi                        */
    ,( 8*60+00)   /* 48(GMT+08:00) Perth                                                      */
    ,( 8*60+00)   /* 49(GMT+08:00) Singapore                                                  */
    ,( 8*60+00)   /* 50(GMT+08:00) Taipei                                                     */
    ,( 9*60+00)   /* 51(GMT+09:00) Osaka,Sapporo,Tokyo                                        */
    ,( 9*60+00)   /* 52(GMT+09:00) Seoul                                                      */
    ,( 9*60+00)   /* 53(GMT+09:00) Yakutsk                                                    */
    ,( 9*60+30)   /* 54(GMT+09:30) Adelaide                                                   */
    ,( 9*60+30)   /* 55(GMT+09:30) Darwin                                                     */
    ,(10*60+00)   /* 56(GMT+10:00) Brisbane                                                   */
    ,(10*60+00)   /* 57(GMT+10:00) Canberra,Melbourne,Sydney                                  */
    ,(10*60+00)   /* 58(GMT+10:00) Guarn,Port Moresby                                         */
    ,(10*60+00)   /* 59(GMT+10:00) Hobart                                                     */
    ,(10*60+00)   /* 60(GMT+10:00) Vladivostok                                                */
    ,(11*60+00)   /* 61(GMT+11:00) Magadan,Solomon Is. ,New Calcdonia                         */
    ,(12*60+00)   /* 62(GMT+12:00) Auckland,Wellington                                        */
    ,(12*60+00)   /* 63(GMT+12:00) Fiji,Kamchaka,Marshall Is.                                 */
};
#else
INT8U     time_zone = 0;
int timezone_adj[] =
{
	0
};
#endif

static void day_count_callback(void)
{
#if !(defined _DRV_L2_EXT_RTC) || (_DRV_L2_EXT_RTC == 0) 
	day_count++;
#endif

	#if (_ALARM_SETTING == CUSTOM_ON)&&(_POWERTIME_SETTING != CUSTOM_ON)	
	ap_alarm_set();//luowl for alarm update day
	#endif	

	//DBG_PRINT("day_count\r\n");		
}

INT32S calendar_init(void)
{
#if SUPPORT_GPY02XX == GPY02XX_NONE
 #if MCU_VERSION >= GPL326XX
 	rtc_callback_set(IDP_RTC_DAY_INT_INDEX, day_count_callback);
 	/* enable day interrupt */
 	idp_rtc_int_set(GPX_RTC_DAY_IEN, RTC_EN_MASK&GPX_RTC_DAY_IEN);
 #else
 	rtc_callback_set(RTC_DAY_INT_INDEX, day_count_callback);
	/* enable day interrupt */
	rtc_int_set(RTC_DAY_IEN, RTC_EN&RTC_DAY_IEN);
	/* enable HMS function */
	rtc_function_set(RTC_HMSEN, RTC_EN&RTC_HMSEN);
 #endif
#else
	gpy0200_callback_set(GPY0200_USER_ISR_DAY, day_count_callback);
	gpy0200_rtc_function_set(GPY0200_RTC_HMSEN,(RTC_EN&GPY0200_RTC_HMSEN));
	gpy0200_rtc_int_set(GPY0200_RTC_DAY_IEN,(RTC_EN&GPY0200_RTC_DAY_IEN));
#endif
	
    //rtc_callback_set(RTC_ALM_INT_INDEX,alarm_callback);
	
	/* enable alarm interrupt */
	//rtc_int_set(RTC_ALM_IEN, RTC_EN&RTC_ALM_IEN);
	
	/* enable alarm function */
	//rtc_function_set(RTC_ALMEN, RTC_EN&RTC_ALMEN);

	return STATUS_OK;
}

void cal_factory_date_time_set(TIME_T  *tm)
{
	t_rtc   r_time;

#if SUPPORT_GPY02XX == GPY02XX_NONE

  #if MCU_VERSION >= GPL326XX
  	rtc_day_get(&r_time);
  	if (r_time.rtc_day != 0)  {
		LAST_JULIAN_DATE = cal_last_jd_get();
		day_count = cal_day_get();
		return;
	}
	
	LAST_JULIAN_DATE = date_to_jd(tm->tm_year,tm->tm_mon,tm->tm_mday) - 1;
	cal_last_jd_store(LAST_JULIAN_DATE);
	
	r_time.rtc_sec = tm->tm_sec;
	r_time.rtc_min = tm->tm_min;
	r_time.rtc_hour = tm->tm_hour;
	
	r_time.rtc_day = date_to_jd(tm->tm_year,tm->tm_mon,tm->tm_mday) - LAST_JULIAN_DATE;
	
	rtc_time_set(&r_time);
	rtc_day_set(&r_time);
	
	day_count = cal_day_get();
  #else  
    if ((day_count != 0xFFFFFFFF) && (day_count != 0xFFFFFFFE)) {
    	return;
    }
	
	r_time.rtc_sec = tm->tm_sec;
	r_time.rtc_min = tm->tm_min;
	r_time.rtc_hour = tm->tm_hour;
	rtc_time_set(&r_time);
	
	day_count = date_to_jd(tm->tm_year,tm->tm_mon,tm->tm_mday);
  #endif
#else
	//LAST_JULIAN_DATE = date_to_jd(tm->tm_year-1,12,31);
	gpy0200_rtc_day_get(&r_time);
	
	if (r_time.rtc_day != 0)  {
		LAST_JULIAN_DATE = cal_last_jd_get();
		day_count = cal_day_get();
		return;
	}
	
	LAST_JULIAN_DATE = date_to_jd(tm->tm_year,tm->tm_mon,tm->tm_mday) - 1;
	cal_last_jd_store(LAST_JULIAN_DATE);
	
	r_time.rtc_sec = tm->tm_sec;
	r_time.rtc_min = tm->tm_min;
	r_time.rtc_hour = tm->tm_hour;
	
	r_time.rtc_day = date_to_jd(tm->tm_year,tm->tm_mon,tm->tm_mday) - LAST_JULIAN_DATE;
	
	gpy0200_rtc_time_set(&r_time,0xf);
	
	day_count = cal_day_get();
#endif
}

INT32S cal_time_get(TIME_T  *tm)
{
    INT32U	s;
    INT32U	j;
    INT32U	gmts;
    t_rtc   r_time;
    INT32S  JD, y, m, d;
    BOOLEAN over;
   
    if (!tm) {
        return STATUS_FAIL;
    }

    cal_rtc_time_get(&r_time);
   
#if MCU_VERSION >= GPL326XX    
    if(day_count == 0xFFFFFFFF) {
    	LAST_JULIAN_DATE = cal_last_jd_get();
		day_count = cal_day_get();
    }
#endif
    
    j = day_count;

    j -= LAST_JULIAN_DATE;
    
    if ((INT32S)j < 0) {
        tm->tm_sec = r_time.rtc_sec;
    	tm->tm_min = r_time.rtc_min;
    	tm->tm_hour = r_time.rtc_hour;
    	tm->tm_year = 2008;
    	tm->tm_mon = 8;
   		tm->tm_mday = 8;
   		tm->tm_wday = 4;
    	return STATUS_FAIL;
    }

    j = (j * 24) + r_time.rtc_hour;
    j = (j * 60) + r_time.rtc_min;
    j = (j * 60) + r_time.rtc_sec;
    
	gmts = j;
    gmts += timezone_adj[time_zone] * 60;
    
    s = gmts;
#if 0
	if ((s & 0x80000000) == 0)
    {
        over = TRUE;
    }
    else
    {
        over = FALSE;
    }
#endif
	
	over = FALSE;
    
    if (over)
    {
        s += 16;
    }

    tm->tm_sec = (s % 60);
    s /= 60;

    if (over)
    {
        s += 28;
    }

    tm->tm_min = (s % 60);
    s /= 60;

    if (over)
    {
        s += 6;
    }

    tm->tm_hour = (s % 24);
    s /= 24;

    if (over)
    {
        s += 49710;
    }

    JD = s + LAST_JULIAN_DATE;

    tm->tm_wday = ((JD + 1) % 7);

    j = JD - 1721119;
    y = (4 * j - 1) / 146097;
    j = 4 * j - 1 - 146097 * y;
    d = j / 4;
    j = (4 * d + 3) / 1461;
    d = 4 * d + 3 - 1461 * j;
    d = (d + 4) / 4;
    m = (5 * d - 3) / 153;
    d = 5 * d - 3 - 153 * m;
    d = (d + 5) / 5;
    y = 100 * y + j;
    
    if (m < 10) {
        m = m + 3;
    }
    else {
        m = m - 9;
        y = y + 1;
    }

    tm->tm_year = y;
    tm->tm_mon = m;
    tm->tm_mday = d;
            
    return STATUS_OK;
}


INT32S cal_time_set(TIME_T tm)
{
    INT32S c;
    INT32U j;
    t_rtc  r_time;
    #if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC == 1) 
    EXTERNAL_RTC_TIME	g_rtc_time;
    #endif
    
	#if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC == 1) 
	// update ext rtc
	//drv_l2_rtc_first_power_on_check();
	g_rtc_time.tm_sec = tm.tm_sec;
	g_rtc_time.tm_min = tm.tm_min;
	g_rtc_time.tm_hour = tm.tm_hour;
	g_rtc_time.tm_mday = tm.tm_mday;
	g_rtc_time.tm_mon = tm.tm_mon; 
	g_rtc_time.tm_year = tm.tm_year - 2000;
	g_rtc_time.tm_wday = tm.tm_wday;
	drv_l2_rtc_total_set(&g_rtc_time); 
	#endif

    if ((tm.tm_mon > 12 || tm.tm_mon < 1) || (tm.tm_mday > monday[tm.tm_mon] || tm.tm_mday < 1) ||
        (tm.tm_hour > 23 || tm.tm_hour < 0) || (tm.tm_min > 59 || tm.tm_min < 0) ||
        (tm.tm_sec > 59 || tm.tm_sec < 0) || (tm.tm_year < 1800)) {
        return STATUS_FAIL;
    }

	j = date_to_jd(tm.tm_year,tm.tm_mon,tm.tm_mday);

    c = tm.tm_sec + tm.tm_min*60 + tm.tm_hour*3600;
    c -= timezone_adj[time_zone] * 60;

	if (c < 0) {
		j--;
		c += 86400;
    }

    r_time.rtc_sec = c%60;
    c /= 60;
	r_time.rtc_min = c%60;
	c /= 60;
	r_time.rtc_hour = c;
    
	cal_rtc_time_set(&r_time);
	
    cal_day_set(j);
    
    return STATUS_OK;
}

#if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC == 1) 
INT32S cal_internal_time_set(TIME_T tm)
{
    INT32S c;
    INT32U j;
    t_rtc  r_time;
   
    if ((tm.tm_mon > 12 || tm.tm_mon < 1) || (tm.tm_mday > monday[tm.tm_mon] || tm.tm_mday < 1) ||
        (tm.tm_hour > 23 || tm.tm_hour < 0) || (tm.tm_min > 59 || tm.tm_min < 0) ||
        (tm.tm_sec > 59 || tm.tm_sec < 0) || (tm.tm_year < 1800)) {
        return STATUS_FAIL;
    }

	j = date_to_jd(tm.tm_year,tm.tm_mon,tm.tm_mday);

    c = tm.tm_sec + tm.tm_min*60 + tm.tm_hour*3600;
    c -= timezone_adj[time_zone] * 60;

	if (c < 0) {
		j--;
		c += 86400;
    }

    r_time.rtc_sec = c%60;
    c /= 60;
	r_time.rtc_min = c%60;
	c /= 60;
	r_time.rtc_hour = c;
    
	cal_rtc_time_set(&r_time);
	
    cal_day_set(j);
    
    return STATUS_OK;
}
#endif

static INT32U date_to_jd(INT32S yyyy,INT32S mm,INT32S dd)
{
	INT32S c,ya;
	INT32U j;
	
	if (mm > 2) {
        mm = mm - 3;
    }
    else {
        mm = mm + 9;
        yyyy = yyyy - 1;
    }

    c = yyyy / 100;
    ya = yyyy - 100 * c;
    j = (146097L * c) / 4 + (1461L * ya) / 4 + (153L * mm + 2) / 5 + dd + 1721119L;
    
    return j;		
}

#if SUPPORT_TIME_ZONE_ADJUST == 1
void cal_set_time_zone(INT8U t_zone)
{
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif	
	time_zone = t_zone;
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif

}
#endif
#if 0
static void alarm_callback(void)
{
	INT32S  i;
	t_rtc   alm_time;
	
	rtc_alarm_get(&alm_time);
	
	for (i=0;i<ALARM_MAX_NUM;i++) {
		if (!alarm_event[i].usage) {
			continue;
		}
		if ((alarm_event[i].alarm_time.tm_sec != alm_time.rtc_sec) ||
			(alarm_event[i].alarm_time.tm_min != alm_time.rtc_min) ||
			(alarm_event[i].alarm_time.tm_hour != alm_time.rtc_hour)) {
			continue;
		}
		
		if (alarm_event[i].en_flag && (alarm_event[i].repeat_mode==ALARM_DAY)) {
			if (alarm_event[i].alarm_handler) {
				(*alarm_event[i].alarm_handler)();
			}		
		}/*
		else if (alarm_event[i].en_flag && (alarm_event[i].repeat_mode==ALARM_ANNUAL)) {
			if ((alarm_event[i].alarm_time.tm_mday != tm.tm_mday) ||
			    (alarm_event[i].alarm_time.tm_mon != tm.tm_mon) ||
			    (alarm_event[i].alarm_time.tm_year != tm.tm_year)) {
			    	continue;
			}
			if (alarm_event[i].alarm_handler) {
				(*alarm_event[i].alarm_handler)();
			}
		}*/
	}
	set_next_alarm();	
	
}

INT32S cal_set_alarm(INT8U alarm_idx, TIME_T at, INT8U rpt_mode, void (*alarm_handler)())
{
	if (alarm_idx>ALARM_MAX_NUM) {
		return STATUS_FAIL;
	}
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif
	alarm_event[alarm_idx].usage = TRUE;
	alarm_event[alarm_idx].alarm_time.tm_sec = at.tm_sec;
	alarm_event[alarm_idx].alarm_time.tm_min = at.tm_min;
	alarm_event[alarm_idx].alarm_time.tm_hour = at.tm_hour;
	alarm_event[alarm_idx].alarm_time.tm_mday = at.tm_mday;
	alarm_event[alarm_idx].alarm_time.tm_mon = at.tm_mon;
	alarm_event[alarm_idx].alarm_time.tm_year = at.tm_year;
	alarm_event[alarm_idx].repeat_mode = rpt_mode;
	alarm_event[alarm_idx].alarm_handler = alarm_handler;	
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif

	set_next_alarm();
	
	return STATUS_OK;	
}

INT32S cal_clear_alarm(INT8U alarm_idx)
{
	if (alarm_idx>ALARM_MAX_NUM) {
		return STATUS_FAIL;
	}
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif
	alarm_event[alarm_idx].usage = FALSE;
	alarm_event[alarm_idx].alarm_handler = 0;	
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif

	return STATUS_OK;	
}

INT32S cal_set_alarm_status(INT8U alarm_idx, INT8U status)
{
	if (alarm_idx>ALARM_MAX_NUM) {
		return STATUS_FAIL;
	}
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif
	if (status) {
		alarm_event[alarm_idx].en_flag = TRUE;
	}
		
	else {
		alarm_event[alarm_idx].en_flag = FALSE;
	}
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif
	return STATUS_OK;
}

static void swap(INT32S *a, INT32S *b)
{ 
  INT32S t=*a;
  *a=*b;
  *b=t; 
}

static void sort(INT32S *array,INT8U len) {
    
    INT32S i, j;
    for(i=len-1;i>=0;i--) {
        for (j=0;j<=i-1;j++){ 
            if (array[j] > array[j+1])
                swap(&array[j], &array[j+1]);
        }
    }
}

static INT32S set_next_alarm(void)
{
	INT32U  temp[ALARM_MAX_NUM] = {0};
	//TIME_T  tm;
	INT8U   i,count = 0;
	INT32U  now_time;
	t_rtc   r_time;
	
	for (i=0;i<ALARM_MAX_NUM;i++) {
		if (alarm_event[i].usage) {
			temp[count] = alarm_event[i].alarm_time.tm_sec +
			          alarm_event[i].alarm_time.tm_min * 60 +
			          alarm_event[i].alarm_time.tm_hour * 3600; 
			count++;
		}
	}
	
	if (count == 0) {
		return STATUS_FAIL;
	}
	
	sort((INT32S *)temp,count);
	//cal_get_time(&tm);
	cal_rtc_time_get(&r_time);
	now_time = r_time.rtc_sec + r_time.rtc_min * 60 + r_time.rtc_hour * 3600; 
	
	for (i=0;i<count;i++) {
		if (temp[i] > now_time) {
			break;
		}
	} 
	
	if (i == count) {
		i = 0;
	}

#if 0 
	r_time.rtc_sec = tm.tm_sec;
	r_time.rtc_min = tm.tm_min;
	r_time.rtc_hour = tm.tm_hour;
	cal_rtc_time_set(&r_time);
#endif
	r_time.rtc_sec = (temp[i] % 60);
    temp[i] /= 60;
    r_time.rtc_min = (temp[i] % 60);
    temp[i] /= 60;
    r_time.rtc_hour = (temp[i] % 24);
	rtc_alarm_set(&r_time);
	
	return STATUS_OK;
	 
}
#endif
#if SUPPORT_GPY02XX != GPY02XX_NONE
static INT32U cal_day_get(void)
{
	t_rtc  r_time;
	
	gpy0200_rtc_day_get(&r_time);
	return (r_time.rtc_day+LAST_JULIAN_DATE); 
}
#elif MCU_VERSION >= GPL326XX
static INT32U cal_day_get(void)
{
	t_rtc  r_time;
	
	rtc_day_get(&r_time);
	return (r_time.rtc_day+LAST_JULIAN_DATE); 
}
#endif

static void cal_day_set(INT32U day)
{
#if SUPPORT_GPY02XX != GPY02XX_NONE
	t_rtc  r_time;
	
	LAST_JULIAN_DATE = day - 1;
	cal_last_jd_store(LAST_JULIAN_DATE);
	
	r_time.rtc_day = day - LAST_JULIAN_DATE;
	gpy0200_rtc_time_set(&r_time, 0x8);
#else
 #if MCU_VERSION >= GPL326XX 
	t_rtc  r_time;
	
	LAST_JULIAN_DATE = day - 1;
	cal_last_jd_store(LAST_JULIAN_DATE);
	
	r_time.rtc_day = day - LAST_JULIAN_DATE;
	rtc_day_set(&r_time);
 #endif
#endif
	day_count = day;
}

static void cal_rtc_time_set(t_rtc *rtc_time)
{
#if SUPPORT_GPY02XX == GPY02XX_NONE
	rtc_time_set(rtc_time);
#else
	gpy0200_rtc_time_set(rtc_time, 0x7);
#endif		
}

static void cal_rtc_time_get(t_rtc *rtc_time)
{
#if SUPPORT_GPY02XX == GPY02XX_NONE
	rtc_time_get(rtc_time);
#else
	gpy0200_rtc_hms_get(rtc_time);
#endif		
	
}

#if MCU_VERSION >= GPL326XX
static void cal_last_jd_store(INT32U last_jd)
{
	if(jd_store) 
		(*jd_store)(last_jd);
	
	if(config_store) 
		(*config_store)();
}

static INT32U cal_last_jd_get(void)
{
	if(jd_get)
		return (*jd_get)();
	else
		return DEC_31TH_2007;
}
#elif SUPPORT_GPY02XX != GPY02XX_NONE
static void cal_last_jd_store(INT32U last_jd)
{
	gpy0200_sram_data_wirte(0,(INT8U*)&last_jd,4);
}

static INT32U cal_last_jd_get(void)
{
	INT8U jd[4];
	INT32U last_jd;
	
	gpy0200_sram_data_read(0,jd,4);
	last_jd = jd[0] | jd[1] << 8 | jd[2] << 16 | jd[3] << 24;
	return last_jd;
}
#endif

#if MCU_VERSION >= GPL326XX 
void cal_day_store_get_register(void (*fp_store)(INT32U),INT32U (*fp_get)(void),void (*fp_config_store)(void))
{
	jd_store = fp_store;
	jd_get = fp_get;
	config_store = fp_config_store;
}
#endif

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined GPLIB_CALENDAR_EN) && (GPLIB_CALENDAR_EN == 1)  //
//================================================================//
