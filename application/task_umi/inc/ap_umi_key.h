#ifndef __AP_UMI_KEY_H__
#define __AP_UMI_KEY_H__

#include "turnkey_umi_task.h"


extern void umi_key_init(void);
extern INT32S umi_key_register(void);
extern void umi_tv_out_key_register(void);
extern void umi_key_transfer(INT32U *message, INT32U key_up, INT32U key_down, INT32U key_left, INT32U key_right, INT32U key_ok);


#endif		// __AP_UMI_KEY_H__