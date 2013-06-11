#ifndef __AP_JPEG_H__
#define __AP_JPEG_H__


#include "turnkey_image_task.h"

#define IMAGE_ENCODE_JPEG_HEADER_LENGTH				624
#define IMAGE_ENCODE_JPEG_L_TABLE_OFFSET			20
#define IMAGE_ENCODE_JPEG_C_TABLE_OFFSET			20+64+5
#define IMAGE_ENCODE_JPEG_HEIGHT_H_OFFSET			158
#define IMAGE_ENCODE_JPEG_HEIGHT_L_OFFSET			159
#define IMAGE_ENCODE_JPEG_WIDTH_H_OFFSET			160
#define IMAGE_ENCODE_JPEG_WIDTH_L_OFFSET			161

extern INT32S jpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S jpeg_buffer_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S jpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size);
extern INT32S jpeg_buffer_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size);
extern INT32S jpeg_buffer_encode(IMAGE_ENCODE_STRUCT *img_encode_struct);


#endif		// __AP_JPEG_H__