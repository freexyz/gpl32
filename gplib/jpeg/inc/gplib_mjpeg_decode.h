#ifndef __GPLIB_MJPEG_DECODE_H__
#define __GPLIB_MJPEG_DECODE_H__


#include "gplib.h"

// Motion-JPEG decoder init API
extern void mjpeg_decode_init(void);

// Motion-JPEG decoder APIs for setting output parameters
extern INT32S mjpeg_decode_output_format_set(INT32U format);
extern INT32S mjpeg_decode_output_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment

// Motion-JPEG decoder APIs for start, restart and stop function
extern INT32S mjpeg_decode_once_start(INT8U *buf, INT32U len);		// The complete VLC data is available, decode JPEG image once.
extern INT32S mjpeg_decode_on_the_fly_start(INT8U *buf, INT32U len);		// Partial of the VLC data is available. buf must be 16-byte alignment except the first call to this API
extern void mjpeg_decode_stop(void);

// Motion-JPEG decoder API for status query
extern INT32S mjpeg_decode_status_query(INT32U wait);					// If wait is 0, this function returns immediately. Return value: C_JPG_STATUS_SCALER_DONE/C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR


#endif 		// __GPLIB_MJPEG_DECODE_H__