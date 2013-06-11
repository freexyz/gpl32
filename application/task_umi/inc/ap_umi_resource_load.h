#ifndef __AP_UMI_RESOURCE_LOAD_H__
#define __AP_UMI_RESOURCE_LOAD_H__

#include "turnkey_umi_task.h"

typedef enum {
	GPRS_TFT,
	GPRS_TV,
	GPRS_NUM_MAX
}GPRS_TYPE;


extern INT32S resource_init(void);
extern INT32S resource_read(INT32U offset_byte, INT8U *pbuf, INT32U byte_count);
extern void resource_close(void);

#endif  	// __AP_UMI_RESOURCE_LOAD_H__