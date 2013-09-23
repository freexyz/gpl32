#ifndef __drv_l1_RTC_H__
#define __drv_l1_RTC_H__


#include "driver_l1.h"
#include "drv_l1_sfr.h"

//internal rtc
#define RTC_RTCEN     (1 << 15)  /* RTC enable */

#define RTC_SCHSEL    (7 <<  0)  /* schedule time period selection */

#define RTC_SEC_BUSY  (1 << 15)  /* RTC second busy flag */
#define RTC_MIN_BUSY  (1 << 14)  /* RTC min busy flag */
#define RTC_HR_BUSY   (1 << 13)  /* RTC hour busy flag */

#define RTC_ALMEN     (1 << 10)  /* alarm function enable */
#define RTC_HMSEN     (1 <<  9)  /* H/M/S function enable */
#define RTC_SCHEN     (1 <<  8)  /* scheduler function enbale */

#define RTC_ALM_IEN       (1 << 10)  /* alarm interrupt enbale */
#define RTC_SCH_IEN       (1 <<  8)  /* scheduler interrupt enbale */
#define RTC_DAY_IEN      (1 <<  4)  /* hour interrupt enbale */
#define RTC_HR_IEN        (1 <<  3)  /* hour interrupt enbale */
#define RTC_MIN_IEN       (1 <<  2)  /* min interrupt enbale */
#define RTC_SEC_IEN       (1 <<  1)  /* alarm interrupt enbale */
#define RTC_HALF_SEC_IEN  (1 <<  0)  /* alarm interrupt enbale */

//independ power rtc
#define GPX_RTC_ALMOEN    (1 << 0)  /* alarm output singnal enable */
#define GPX_RTC_EN        (1 << 3)  /* RTC function enable */
#define GPX_RTC_VAEN      (1 << 4)  /* Voltage comparator enbale */
#define GPX_RTC_PWREN     (1 << 5)  /* set alarm output signal to high */

#define GPX_RTC_ALM_IEN       (1 <<  4)  /* alarm interrupt enbale */
#define GPX_RTC_DAY_IEN       (1 <<  5)  /* hour interrupt enbale */
#define GPX_RTC_HR_IEN        (1 <<  3)  /* hour interrupt enbale */
#define GPX_RTC_MIN_IEN       (1 <<  2)  /* min interrupt enbale */
#define GPX_RTC_SEC_IEN       (1 <<  1)  /* alarm interrupt enbale */
#define GPX_RTC_HALF_SEC_IEN  (1 <<  0)  /* alarm interrupt enbale */

#define RTC_EN				0xFFFFFFFF
#define RTC_DIS				0
#define RTC_EN_MASK			0xFF
#define RTC_BUSY			-1

#if MCU_VERSION < GPL326XX
extern void rtc_init(void);
extern void rtc_time_set(t_rtc *rtc_time);
extern void rtc_time_get(t_rtc *rtc_time);
extern void rtc_alarm_set(t_rtc *rtc_time);
extern void rtc_alarm_get(t_rtc *rtc_time);
extern void rtc_function_set(INT32U mask, INT32U value);
extern void rtc_int_set(INT32U mask, INT32U value);
extern void rtc_schedule_set(INT8U freq);
extern INT32U rtc_irq_flag_get(void);
extern BOOLEAN rtc_day_int_get(void);
#else
extern void rtc_init(void);
extern void rtc_time_set(t_rtc *rtc_time);
extern void rtc_time_get(t_rtc *rtc_time);
extern void rtc_day_set(t_rtc *rtc_time);
extern void rtc_day_get(t_rtc *rtc_time);
extern void rtc_alarm_set(t_rtc *rtc_time);
extern void rtc_alarm_get(t_rtc *rtc_time);
extern void idp_rtc_alm_pin_set(INT32U level);
extern void idp_rtc_alm_pin_wakeup_set(INT32U enable);
extern void rtc_function_set(INT8U mask, INT8U value);
extern void rtc_reset_trigger_level_set(INT8U value);
extern void rtc_int_set(INT32U mask, INT32U value);
extern void rtc_int_clear(INT32U value);
extern void idp_rtc_int_set(INT8U mask, INT8U value);
extern void idp_rtc_int_clear(INT8U value);
extern INT8U gpx_rtc_read(INT8U addr);
extern void gpx_rtc_write(INT8U addr,INT8U data);
extern void rtc_schedule_enable(INT8U freq);
extern void rtc_schedule_disable(void);
extern void rtc_ext_to_int_set(void);
#endif

extern INT32S rtc_callback_clear(INT8U int_idx);
extern INT32S rtc_callback_set(INT8U int_idx, void (*user_isr)(void));

#endif 		/* __drv_l1_RTC_H__ */