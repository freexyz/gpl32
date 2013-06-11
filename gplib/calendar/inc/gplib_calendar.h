#ifndef __GPL32_CALENDAR_H__
#define __GPL32_CALENDAR_H__

#include "gplib.h"

typedef struct
{
	BOOLEAN usage;
	TIME_T  alarm_time;
	BOOLEAN en_flag;
	INT8U   repeat_mode; /* 0 for once a day ,1 for once a year */
	void    (*alarm_handler)();
}ALARM_T;

#define ALARM_MAX_NUM   5
#define ALARM_FULL     -1

extern INT32S calendar_init(void);
extern INT32S cal_time_get(TIME_T  *tm);
extern INT32S cal_time_set(TIME_T tm);
extern INT32S cal_alarm_set(INT8U alarm_idx, TIME_T at, INT8U rpt_mode, void (*alarm_handler)());
extern INT32S cal_alarm_status_set(INT8U alarm_idx, INT8U status);
extern void  cal_time_zone_set(INT8U t_zone);
extern void cal_factory_date_time_set(TIME_T  *tm);
extern void cal_day_store_get_register(void (*fp_store)(INT32U),INT32U (*fp_get)(void),void (*fp_config_store)(void));


#endif 		/* __GPL32_CALENDAR_H__ */