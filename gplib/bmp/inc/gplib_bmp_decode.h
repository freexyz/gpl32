#ifndef __GPLIB_BMP_DECODE_H__
#define __GPLIB_BMP_DECODE_H__


#include "gplib.h"

#define BMP_PARSE_OK					0
#define BMP_PARSE_NOT_DONE				1
#define BMP_PARSE_DONE					2
#define BMP_PARSE_FAIL					-1

#define C_BMP_STATUS_INPUT_EMPTY		0x00000001
#define C_BMP_STATUS_OUTPUT_FULL		0x00000002
#define C_BMP_STATUS_DECODE_DONE		0x00000004
#define C_BMP_STATUS_STOP				0x00000008
#define C_BMP_STATUS_INIT_ERR			0x00000010

typedef enum {
	STATE_BMP_HEADER = 0,
	STATE_DIB_FORMAT,					// 40=Windows V3, 12=OS/2 V1, 64=OS/2 V2, 108=Windows V4, 124=Windows V5
	STATE_DIB_HEADER,					// Device independent bitmap header
	STATE_COLOR_PALETTE,
	STATE_BITMAP_DATA
} PARSE_MAIN_STATE_ENUM;

typedef enum {
	STATE_MIGIC_NUMBER = 0,				// 0x424D ("BM")
	STATE_FILE_SIZE,					// bytes
	STATE_RESERVE,
	STATE_BITMAP_DATA_OFFSET			// offset of bitmap data
} PARSE_BMP_HEADER_STATE_ENUM;

typedef enum {
	STATE_HEADER_SIZE = 0,
	STATE_BMP_WIDTH,					// Pixels
	STATE_BMP_HEIGHT,					// Pixels
	STATE_COLOR_PLANE_NUMBER,
	STATE_BITS_PER_PIXEL,				// 1, 4, 8, 16, 24 or 32
	STATE_COMPRESSION_METHOD,			// 0=None, 1=RLE(Run-Length-Encode) 8-bit, 2=RLE 4-bit, 3=Bit field, 4=JPEG, 5=PNG
	STATE_IMAGE_SIZE,
	STATE_H_RESOLUTION,
	STATE_V_RESOLUTION,
	STATE_PALETTE_COLOR_NUMBER,			// 0=2^(bits per pixel)
	STATE_IMPORTANT_COLOR
} PARSE_DIB_STATE_ENUM;


extern void bmp_decode_init(void);
extern INT32S bmp_decode_parse_header(INT8U *buf_start, INT32U len);
extern INT16U bmp_decode_image_width_get(void);
extern INT16U bmp_decode_image_height_get(void);
extern INT8U bmp_decode_line_direction_get(void);				// 0=Bottom to top, 1=Top to bottom
extern INT8U * bmp_decode_data_addr_get(void);
extern INT32S bmp_decode_output_set(INT32U *output_addr, INT32U fifo_line_num);
extern INT32S bmp_decode_on_the_fly_start(INT8U *input_addr, INT32U len);
extern INT32S bmp_decode_output_restart(void);
extern void bmp_decode_stop(void);
extern INT32S bmp_decode_status_query(void);

#endif 		// __GPLIB_BMP_DECODE_H__


















#if 0


#define BMP_SCALER_USE_32LINE     		0  //  1 means scaler use 32line   add by Lion for change scaler from 32 line to 16 line

#define BMP_PARSE_OK					0
#define BMP_PARSE_NOT_DONE				1
#define BMP_PARSE_FAIL					-1

// Define BMP engine status
#define C_BMP_STATUS_INPUT_EMPTY		0x00000004
#define C_BMP_STATUS_OUTPUT_FULL		0x00000008
#define C_BMP_STATUS_DECODE_DONE		0x00000010
#define C_BMP_STATUS_STOP				0x00000040
#define C_BMP_STATUS_TIMEOUT			0x00000080
#define C_BMP_STATUS_INIT_ERR			0x00000100
#define C_BMP_STATUS_INPUT_ERR			0x00010000


// Ring FIFO control register
#define C_BMP_FIFO_DISABLE				0x00000000
#define C_BMP_FIFO_ENABLE				0x00000001
#define C_BMP_FIFO_16LINE				0x00000009
#define C_BMP_FIFO_32LINE				0x00000001
#define C_BMP_FIFO_64LINE				0x00000003
#define C_BMP_FIFO_128LINE				0x00000005
#define C_BMP_FIFO_256LINE				0x00000007
#define C_BMP_FIFO_LINE_MASK			0x0000000F

typedef enum {
	STATE_FILE = 0,
	STATE_HEADER,
	STATE_CHUNKSETUP
} PARSE_MAIN_STATE_ENUM;


// bmp decoder initiate API
extern void bmp_decode_init(void);

// bmp decoder header parser APIs
extern INT32S bmp_decode_parse_header(INT8U *buf_start, INT32U len);	// buf_start points to buffer address of bmp file
extern INT16U bmp_decode_image_width_get(void);					// Get image width after header parsing finish
extern INT16U bmp_decode_image_extended_width_get(void);			// Get image extended width(according to YUV mode)
extern INT16U bmp_decode_image_height_get(void);					// Get image height after header parsing finish
extern INT16U bmp_decode_image_extended_height_get(void);			// Get image extended height(according to YUV mode)
extern INT32U bmp_header_length_get(void);							//get BMP file header length

// bmp decoder start, restart and stop APIs
extern INT32S bmp_decode_on_the_fly_start(INT8U *line_bufptr, INT16U *out_bufptr);	// When only partial of the VLC data is available, using this API to decode it. buf must be 16-byte alignment except the first one


#endif