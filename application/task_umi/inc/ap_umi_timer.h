#ifndef __AP_UMI_TIMER_H__
#define __AP_UMI_TIMER_H__

#include "turnkey_umi_task.h"


extern void umi_timer_init(void);
extern INT32S umi_timer_start(INT32U event, INT32U interval, INT8U *timer_id);
extern INT32S umi_timer_stop(INT8U timer_id);


#endif		// __AP_UMI_TIMER_H__