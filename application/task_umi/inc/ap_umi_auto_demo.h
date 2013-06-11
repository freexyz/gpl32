#ifndef __AP_UMI_AUTO_DEMO_H__
#define __AP_UMI_AUTO_DEMO_H__

#include "turnkey_umi_task.h"

#ifndef _AUTO_RUN_STATUS_ENUM
#define _AUTO_RUN_STATUS_ENUM
typedef enum {
  	ENUM_AUTO_RUN_STATUS_STOP = 0x0,
  	ENUM_AUTO_RUN_STATUS_PLAY,
	ENUM_AUTO_RUN_STATUS_PAUSE
} AUTO_RUN_STATUS_ENUM;
#endif

extern INT32S ap_umi_auto_demo_start(void);
extern INT32S ap_umi_auto_demo_check_sd(void);
extern INT32S ap_umi_auto_demo_handle_timer(void);
extern INT32S ap_umi_auto_demo_pause(void);
extern INT32S ap_umi_auto_demo_resume(void);
extern INT32S ap_umi_auto_demo_stop(void);

extern INT8U auto_run_status;

#endif		// __AP_UMI_AUTO_DEMO_H__
