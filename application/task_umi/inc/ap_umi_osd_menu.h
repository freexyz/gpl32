#ifndef __AP_UMI_OSD_MENU_H__
#define __AP_UMI_OSD_MENU_H__

#include "turnkey_umi_task.h"


extern void umi_osd_menu_init(void);
extern INT32S umi_osd_menu_enter(void);
extern INT32S umi_osd_menu_state_load(void);
extern INT32S umi_osd_menu_process_message(INT32U *message, KEY_EVENT_ST *key_sts);
extern INT32S umi_osd_menu_process_timer(void);
extern INT32S umi_osd_menu_assign_image(INT32U message, INT8U image_index);
extern INT32S umi_osd_menu_exit(void);
extern void umi_osd_menu_icon_operation(INT32U icon_num,INT32U mode);
#if (defined OSD_MENU_HINT_MODE) && (OSD_MENU_HINT_MODE == 1)
extern INT32S umi_osd_menu_hint_disappear_time_set(INT32S time);
extern void umi_osd_menu_hint_mode_set(INT32S mode);
extern INT32S umi_osd_menu_hint_mode_get(void);
#endif

#endif		// __AP_UMI_OSD_MENU_H__