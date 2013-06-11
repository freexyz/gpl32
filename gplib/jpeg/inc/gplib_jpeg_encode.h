#ifndef __GPLIB_JPEG_ENCODE_H__
#define __GPLIB_JPEG_ENCODE_H__


#include "gplib.h"

#define C_JPEG_FORMAT_YUV_SEPARATE		0x0
#define C_JPEG_FORMAT_YUYV				0x1

// JPEG encoder initiate API
extern void jpeg_encode_init(void);

// JPEG encoder input buffer relative APIs
extern INT32S jpeg_encode_input_size_set(INT16U width, INT16U height);			// Width and height of the image that will be compressed
extern INT32S jpeg_encode_input_format_set(INT32U format);						// format: C_JPEG_FORMAT_YUV_SEPARATE or C_JPEG_FORMAT_YUYV

// JPEG encoder output buffer relative APIs
extern INT32S jpeg_encode_yuv_sampling_mode_set(INT32U encode_mode);			// encode_mode: C_JPG_CTRL_YUV422 or C_JPG_CTRL_YUV420
extern INT32S jpeg_encode_output_addr_set(INT32U addr);							// The address that VLC(variable length coded) data will be output

// JPEG encoder start, restart and stop APIs
extern INT32S jpeg_encode_once_start(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// If input format is YUV separate, both y_addr, u_addr and v_addr must be 8-byte alignment. If input format is YUYV, only y_addr(16-byte alignment) is used.
extern INT32S jpeg_encode_on_the_fly_start(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U len);	// If input format is YUV separate, both y_addr, u_addr and v_addr must be 8-byte alignment. If input format is YUYV, only y_addr(16-byte alignment) is used.
extern void jpeg_encode_stop(void);

// JPEG encoder status query API
extern INT32S jpeg_encode_status_query(INT32U wait);
extern INT32U jpeg_encode_vlc_cnt_get(void);

#endif 		// __GPLIB_JPEG_ENCODE_H__