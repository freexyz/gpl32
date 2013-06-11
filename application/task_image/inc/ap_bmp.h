#ifndef __AP_BMP_H__
#define __AP_BMP_H__


#include "turnkey_image_task.h"

extern INT32S bmp_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_buffer_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S bmp_buffer_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);


#endif		// __AP_BMP_H__





#if 0

#define C_BMP_STATUS_INPUT_EMPTY		0x00000004
#define C_BMP_STATUS_OUTPUT_FULL		0x00000008
#define C_BMP_STATUS_DECODE_DONE		0x00000010
#define C_BMP_STATUS_STOP				0x00000040
#define C_BMP_STATUS_TIMEOUT			0x00000080
#define C_BMP_STATUS_INIT_ERR			0x00000100

// Ring FIFO control register
#define C_BMP_FIFO_DISABLE				0x00000000
#define C_BMP_FIFO_ENABLE				0x00000001
#define C_BMP_FIFO_16LINE				0x00000009
#define C_BMP_FIFO_32LINE				0x00000001
#define C_BMP_FIFO_64LINE				0x00000003
#define C_BMP_FIFO_128LINE				0x00000005
#define C_BMP_FIFO_256LINE				0x00000007
#define C_BMP_FIFO_LINE_MASK			0x0000000F

extern INT32S bmp_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_buffer_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S bmp_buffer_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S bmp_decode_32Line(INT16U *out_bufptr);
extern INT32S bmp_getdata(INT16U Width);

#endif