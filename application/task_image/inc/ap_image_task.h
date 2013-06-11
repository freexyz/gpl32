#ifndef __AP_IMAGE_TASK_H__
#define __AP_IMAGE_TASK_H__


#include "turnkey_image_task.h"

extern INT32S image_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S image_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S image_decode_ext_and_scale(IMAGE_DECODE_EXT_STRUCT *img_decode_ext_struct);
extern INT32S image_encode(IMAGE_ENCODE_STRUCT *img_encode_struct);
extern INT32S image_scale(IMAGE_SCALE_STRUCT *img_scale_struct);


#endif		// __AP_IMAGE_TASK_H__