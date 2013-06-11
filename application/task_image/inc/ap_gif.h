#ifndef __AP_GIF_H__
#define __AP_GIF_H__


#include "turnkey_image_task.h"

extern INT32S gif_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S gif_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);


#endif		// __AP_GIF_H__