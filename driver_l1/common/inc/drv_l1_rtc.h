#ifndef __drv_l1_RTC_H__
#define __drv_l1_RTC_H__


#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define RTC_RTCEN     (1 << 15)  /* RTC enable */

#define RTC_SCHSEL    (7 <<  0)  /* schedule time period selection */

#define RTC_SEC_BUSY  (1 << 15)  /* RTC second busy flag */
#define RTC_MIN_BUSY  (1 << 14)  /* RTC min busy flag */
#define RTC_HR_BUSY   (1 << 13)  /* RTC hour busy flag */

#define RTC_BUSY   -1

extern INT32S rtc_callback_clear(INT8U int_idx);
extern void rtc_time_get(t_rtc *rtc_time);
extern void rtc_alarm_get(t_rtc *rtc_time);
extern void rtc_init(void);
extern INT32S rtc_callback_set(INT8U int_idx, void (*user_isr)(void));
extern void rtc_alarm_set(t_rtc *rtc_time);
extern void rtc_time_set(t_rtc *rtc_time);
extern void rtc_schedule_set(INT8U freq);
extern INT32U rtc_irq_flag_get(void);
extern BOOLEAN rtc_day_int_get(void);

#if MCU_VERSION < GPL326XX
extern void rtc_function_set(INT32U mask, INT32U value);
extern void rtc_int_set(INT32U mask, INT32U value);
#else
extern void rtc_day_get(t_rtc *rtc_time);
extern void rtc_day_set(t_rtc *rtc_time);
extern void rtc_function_set(INT8U mask, INT8U value);
extern void rtc_int_set(INT8U mask, INT8U value);
extern void rtc_schedule_enable(INT8U freq);
extern void rtc_schedule_disable(void);
extern void rtc_reset_trigger_level_set(INT8U value);
extern void rtc_ext_to_int_set(void);

#endif

INT8U gpx_rtc_read(INT8U addr);
void gpx_rtc_write(INT8U addr,INT8U data);

#endif 		/* __drv_l1_RTC_H__ */