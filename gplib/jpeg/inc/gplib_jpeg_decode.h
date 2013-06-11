#ifndef __GPLIB_JPEG_DECODE_H__
#define __GPLIB_JPEG_DECODE_H__


#include "gplib.h"

#define JPEG_PARSE_OK				0
#define JPEG_PARSE_NOT_DONE			1
#define JPEG_PARSE_DONE				2
#define JPEG_PARSE_THUMBNAIL_DONE	3

#define JPEG_PARSE_FAIL				-1
#define JPEG_SET_HW_ERROR			-2
#define JPEG_SIZE_ERROR				-3
#define JPEG_REENTRY_ERROR			-4

typedef enum {
	STATE_SOI = 0,
	STATE_TABLE1,
	STATE_FRAME,
	STATE_TABLE2,
	STATE_SCAN,
	STATE_VLC,
	STATE_PARSING_THUMBNAIL,
	STATE_SKIP_THUMBNAIL
} PARSE_MAIN_STATE_ENUM;

typedef enum {
	STATE_LARVAL = 0,
	STATE_QUANTIZATION_TABLE,
	STATE_HUFFMAN_TABLE,
	STATE_ARITHMETIC,
	STATE_DEFINE_RESTART,
	STATE_COMMENT,
	STATE_APP
} PARSE_TABLE_STATE_ENUM;

typedef enum {
	STATE_Q_LEN = 0,
	STATE_Q_IDENTIFIER,
	STATE_ELEMENT
} PARSE_QUANT_STATE_ENUM;

typedef enum {
	STATE_H_LEN = 0,
	STATE_H_IDENTIFIER,
	STATE_NUMBER_CODELEN,
	STATE_HUFFVAL
} PARSE_HUFFMAN_STATE_ENUM;

typedef enum {
	STATE_ARITHMETIC_LEN = 0,
	STATE_ARITHMETIC_DATA
} PARSE_ARITHMETIC_STATE_ENUM;

typedef enum {
	STATE_DRI_LEN = 0,
	STATE_RESTART_INTERVAL
} PARSE_DRI_STATE_ENUM;

typedef enum {
	STATE_COMMENT_LEN = 0,
	STATE_COMMENT_DATA
} PARSE_COMMENT_STATE_ENUM;

typedef enum {
	STATE_APP_LEN = 0,
	STATE_APP_IDENTIFIER,
	STATE_APP_JFIF,
	STATE_APP_EXIF,
	STATE_APP_SKIP_DATA
} PARSE_APP_STATE_ENUM;

typedef enum {
	STATE_APP_EXIF_IDENTIFIER = 0,
	STATE_APP_EXIF_TIFF_BYTE_ORDER,
	STATE_APP_EXIF_TIFF_42,
	STATE_APP_EXIF_TIFF_IFD0,
	STATE_APP_EXIF_JUMP_IFD0,
	STATE_APP_EXIF_IFD0_FIELD_NUM,
	STATE_APP_EXIF_IFD0_FIELD,
	STATE_APP_EXIF_IFD0_NEXT,
	STATE_APP_EXIF_JUMP_DATE_TIME1,
	STATE_APP_EXIF_JUMP_IFD1,
	STATE_APP_EXIF_IFD1_FIELD_NUM,
	STATE_APP_EXIF_IFD1_FIELD,
	STATE_APP_EXIF_JUMP_DATE_TIME2,
	STATE_APP_EXIF_JUMP_THUMBNAIL,
	STATE_APP_EXIF_THUMBNAIL
} PARSE_APP_EXIF_STATE_ENUM;

typedef enum {
	STATE_FRAME_LEN = 0,
	STATE_PRECISION,
	STATE_LINES,
	STATE_SAMPLES,
	STATE_FRAME_COMP_NUM,
	STATE_FRAME_COMPONENT
} PARSE_FRAME_STATE_ENUM;

typedef enum {
	STATE_SCAN_LEN = 0,
	STATE_SCAN_COMP_NUM,
	STATE_SCAN_COMPONENT,
	STATE_START_SPECTRAL,
	STATE_END_SPECTRAL,
	STATE_APPROXIMATION
} PARSE_SCAN_STATE_ENUM;

// JPEG decoder initiate API
extern void jpeg_decode_init(void);

// JPEG decoder header parser APIs
extern INT32S jpeg_decode_parse_header(INT8U *buf_start, INT32U len);	// buf_start points to buffer address of JPEG file
extern INT16U jpeg_decode_image_width_get(void);					// Get image width after header parsing finish
extern INT16U jpeg_decode_image_height_get(void);					// Get image height after header parsing finish
extern INT16U jpeg_decode_image_extended_width_get(void);			// Get image extended width(according to YUV mode)
extern INT16U jpeg_decode_image_extended_height_get(void);			// Get image extended height(according to YUV mode)
extern INT16U jpeg_decode_image_yuv_mode_get(void);					// Get image YUV mode after header parsing finish
extern INT8U jpeg_decode_image_progressive_mode_get(void);			// Get image progressive mode (0=baseline, 1=progressive)
extern INT8U jpeg_decode_image_orientation_get(void);				// Get image orientation(1~8)
extern INT32S jpeg_decode_date_time_get(INT8U *target);
extern INT8U jpeg_decode_thumbnail_exist_get(void);					// Get information about thumbnail image(0=No thumbnail image, 1=Thumbnail image exists)
extern INT16U jpeg_decode_thumbnail_width_get(void);
extern INT16U jpeg_decode_thumbnail_height_get(void);
extern INT16U jpeg_decode_thumbnail_extended_width_get(void);
extern INT16U jpeg_decode_thumbnail_extended_height_get(void);
extern INT16U jpeg_decode_thumbnail_yuv_mode_get(void);
extern INT8U * jpeg_decode_image_vlc_addr_get(void);				// Get start address of entropy encoded data after header parsing finish

// JPEG decoder input relative APIs
extern INT32S jpeg_decode_vlc_maximum_length_set(INT32U len);		// Specify the maximum length that JPEG will read to decode image. Using this API is optional. It is used to prevent JPEG engine from hangging when bad JPEG file is read. The value of length should be >= real VLC length(eg: using file size as its value).

// JPEG decoder output relative APIs
extern INT32S jpeg_decode_output_set(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U line);	// Addresses must be 8-byte alignment, line:C_JPG_FIFO_DISABLE/C_JPG_FIFO_ENABLE/C_JPG_FIFO_16LINE/C_JPG_FIFO_32LINE/C_JPG_FIFO_64LINE/C_JPG_FIFO_128LINE/C_JPG_FIFO_256LINE
extern void jpeg_decode_thumbnail_image_enable(void);				// Decode thumbnail image of file instead of original image
extern void jpeg_decode_thumbnail_image_disable(void);				// Decode original image of file
extern INT32S jpeg_decode_clipping_range_set(INT16U start_x, INT16U start_y, INT16U width, INT16U height);	// x, y, width, height must be at lease 8-byte alignment
extern INT32S jpeg_decode_clipping_mode_enable(void);
extern INT32S jpeg_decode_clipping_mode_disable(void);
extern INT32S jpeg_decode_level2_scaledown_enable(void);			// When scale-down level 2 mode is enabled, output size will be 1/2 in width and height
extern INT32S jpeg_decode_level2_scaledown_disable(void);
extern INT32S jpeg_decode_level8_scaledown_enable(void);			// When scale-down level 8 mode is enabled, output size will be 1/8 in width and height
extern INT32S jpeg_decode_level8_scaledown_disable(void);

// JPEG decoder start, restart and stop APIs
extern INT32S jpeg_decode_once_start(INT8U *buf, INT32U len);		// When the complete VLC data is available, using this API to decode JPEG image once.
extern INT32S jpeg_decode_on_the_fly_start(INT8U *buf, INT32U len);	// When only partial of the VLC data is available, using this API to decode it. buf must be 16-byte alignment except the first one
extern INT32S jpeg_decode_output_restart(void);						// Restart decompression when output FIFO buffer is full. This function should be called after Scaler has finished handling the previous FIFO buffer.
extern void jpeg_decode_stop(void);

// JPEG decoder status query API
extern INT32S jpeg_decode_status_query(INT32U wait);				// If wait is 0, this function returns immediately. Return value:C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_OUTPUT_FULL/C_JPG_STATUS_DECODE_DONE/C_JPG_STATUS_ENCODE_DONE/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR


#endif 		// __GPLIB_JPEG_DECODE_H__