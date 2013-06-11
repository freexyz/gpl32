#ifndef __TURNKEY_IMAGE_TASK_H__
#define __TURNKEY_IMAGE_TASK_H__


#include "application.h"

extern MSG_Q_ID image_msg_queue_id;

extern OS_EVENT *image_task_fs_queue_a;
extern OS_EVENT *image_task_fs_queue_b;

extern void image_task_entry(void *p_arg);
extern INT32S image_task_handle_remove_request(INT32U current_image_id);
extern INT32S image_task_scale_image(IMAGE_SCALE_STRUCT *para);
extern INT32S image_task_parse_image(IMAGE_HEADER_PARSE_STRUCT *para);
extern INT32S image_task_remove_request(INT32U state_id);

extern INT32S image_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S image_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S image_decode_ext_and_scale(IMAGE_DECODE_EXT_STRUCT *img_decode_ext_struct);
extern INT32S image_encode(IMAGE_ENCODE_STRUCT *img_encode_struct);
extern INT32S image_scale(IMAGE_SCALE_STRUCT *img_scale_struct);

// JPEG relative APIs
extern INT32S jpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S jpeg_buffer_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S jpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size);
extern INT32S jpeg_buffer_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size);
extern INT32S jpeg_buffer_encode(IMAGE_ENCODE_STRUCT *img_encode_struct);

// Progressive JPEG relative APIs
extern INT32S jpeg_decode_progressive(IMAGE_DECODE_STRUCT *img_decode_struct);

// Motion-JPEG relative APIs
extern INT32S mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32U avi_parsing(INT16S fileno, INT16S fileno1);
extern INT16U avi_mov_switchflag;
extern INT32S tk_mov_decode_file_id(INT32U FileNum);
extern INT32S tk_mov_get_audio_samplerate(void);
extern INT32S tk_mov_find_adpos(INT32U framenum);
extern INT32S tk_mov_find_vdpos(INT32U JumpFrameNum);
extern INT32S tk_mov_get_scale(void);
extern INT32S tk_mov_get_duration(void);
extern INT32S tk_mov_get_frame_number(void);
extern INT32S tk_mov_get_hassound(void);
extern INT32S tk_mov_get_width(void);
extern INT32S tk_mov_get_height(void);
extern INT32S tk_mov_wave_bitformat_get(void);
extern INT32S tk_mov_wave_channelnum_get(void);
extern INT16U tk_avi_wave_format_type_get(void);
extern INT32U tk_mov_close_file_id(void);
extern INT32S tk_mov_read_audio_data(CHAR *buf, INT32U len);
extern INT32S mov_parsing(INT16U fin,INT16U fin2);
extern void   tk_mov_release_memory(void);
extern FP64   tk_mov_get_fps(void);
extern INT8U *tk_mov_get_waveformatex(void);
extern INT32S mov_mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mov_mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mov_mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S mov_mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct);

// GIF relative APIs
extern INT32S gif_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S gif_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);

///BMP relative APIs
extern INT32S bmp_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_buffer_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S bmp_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
#endif		// __TURNKEY_IMAGE_TASK_H__
