#ifndef __DRV_L1_DEFLICKER_H__
#define __DRV_L1_DEFLICKER_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


// Deflicker control register
#define C_DEFLICKER_CTRL_320_240			0x00000000
#define C_DEFLICKER_CTRL_640_480			0x00000001
#define C_DEFLICKER_CTRL_720_480			0x00000002
#define C_DEFLICKER_CTRL_START				0x00000100
#define C_DEFLICKER_CTRL_RESET				0x00000200
#define C_DEFLICKER_CTRL_INT				0x00000800

// Deflicker interrupt pending register
#define C_DEFLICKER_INT_PEND_DONE			0x00000001

// Deflicker driver control
#define C_DEFLICKER_DRV_MAX_REQUEST_NUM		8

#define C_DEFLICKER_CONTROL_INITIATED		0x00000001
#define C_DEFLICKER_CONTROL_WAIT_DONE		0x00000002

typedef struct {
	INT32U resolution;
	INT32U *input_buffer;
	INT32U *output_buffer;
	OS_EVENT *reply_queue;
} DEFLICKER_REQUEST_STRUCT;

extern void deflicker_init(void);
extern INT32S deflicker_start(INT32U resolution, INT32U wait_mode, INT32U *input_buffer, INT32U *output_buffer, OS_EVENT *reply_queue);


#endif		// __DRV_L1_DEFLICKER_H__