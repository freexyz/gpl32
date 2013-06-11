#ifndef __AP_UMI_IMAGE_CACHE_H__
#define __AP_UMI_IMAGE_CACHE_H__

#include "turnkey_umi_task.h"


#define C_IMAGE_CACHE_HASH_NUM		64
#define C_IMAGE_CACHE_WAY_NUM		2

typedef enum {
  	ENUM_IMAGE_CACHE_IDLE = 0x0,
	ENUM_IMAGE_CACHE_PARSING,
	ENUM_IMAGE_CACHE_DECODING,
	ENUM_IMAGE_CACHE_SCALING
} IMAGE_CACHE_BUSY_ENUM;

#define C_IMAGE_CACHE_CTRL_SEARCH_METHOD			0x0001		// 0=Ring mode, 1=Line mode
#define C_IMAGE_CACHE_CTRL_FILE_TYPE				0x0002		// 0=Photo+Video, 1=Video only
#define C_IMAGE_CACHE_CTRL_SCALE_TWICE				0x0004		// 0=disable, 1=enable
#define C_IMAGE_CACHE_CTRL_SAVE_INFO				0x0008		// 0=disable, 1=enable


extern INT32S image_cache_init(void);
extern void image_cache_ctrl_flag_set(INT16U flag);
extern void image_cache_ctrl_flag_clear(INT16U flag);
extern void image_cache_total_number_set(INT32U num);
extern void image_cache_current_index_set(INT32U index);
extern INT32U image_cache_index_diff_get(INT32U index1, INT32U index2);
extern void image_cache_skip_type_set(INT16U type);
extern void image_cache_filename_size_set(INT32U pixel);
extern void image_cache_parse_number_set(INT32U num);
extern void image_cache_decode_number_set(INT32U num);
extern INT32U image_cache_decode_number_get(void);

extern void image_cache_scale_method_set(INT8U method, INT32U width, INT32U height);
extern void image_cache_scale_format_set(INT32U format);
extern INT32S image_cache_max_limited_size_get(INT32U original_width, INT32U original_height, INT32U limited_width, INT32U limited_height, INT32U *zoom_width, INT32U *zoom_height);
extern INT32S image_cache_mim_limited_size_get(INT32U original_width, INT32U original_height, INT32U limited_width, INT32U limited_height, INT32U *zoom_width, INT32U *zoom_height);

extern INT32S image_cache_query(INT32U index, INT32U *cache_ptr);
extern INT32S image_cache_forward_search(INT32U search_count, INT32U *cache_ptr);
extern INT32S image_cache_backward_search(INT32U search_count, INT32U *cache_ptr);
extern INT32S image_cache_buffer_lock(INT32U index, INT32U *buffer_ptr);
extern INT32S image_cache_buffer_unlock(INT32U index, INT32U *buffer_ptr);
extern INT32S image_cache_update_check(void);
extern INT32S image_cache_video_extension_check(INT32U index);
extern INT32S image_cache_parse_cmd_done(IMAGE_HEADER_PARSE_STRUCT *parse_image_reply);
extern INT32S image_cache_decode_cmd_done(IMAGE_DECODE_STRUCT *decode_image_reply);
extern INT32S image_cache_scale_cmd_done(IMAGE_SCALE_STRUCT *scale_reply);
extern INT8U image_cache_busy_status_get(void);

extern void image_cache_flush(void);


#endif 		// __AP_UMI_IMAGE_CACHE_H__