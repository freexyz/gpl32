#ifndef __AP_MOV_MJPEG_H__
#define __AP_MOV_MJPEG_H__
#include "turnkey_image_task.h"

extern	INT32S 	quicktime_read_char(INT16U fin);																										
extern	INT32S 	quicktime_read_int16(INT16U fin);																										
extern	INT32S 	quicktime_read_int24(INT16U fin);																										
extern	INT32S 	quicktime_read_int32(INT16U fin);																										
extern	INT32U 	quicktime_read_uint32(INT16U fin);																										
extern	INT64U 	quicktime_read_int64(INT16U fin);																										
extern	FP32 	quicktime_read_fixed16(INT16U fin);																										
extern	FP32 	quicktime_read_fixed32(INT16U fin);																										
extern	FP64 	quicktime_read_double64(INT16U fin);																										
extern	void 	quicktime_read_pascal(INT16U fin, INT8S *data);	

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
extern INT32U tk_mov_close_file_id(void);
extern INT32S tk_mov_read_audio_data(CHAR *buf, INT32U len);
extern INT32S mov_parsing(INT16U fin,INT16U fin2);
extern void   tk_mov_release_memory(void);
extern FP64   tk_mov_get_fps(void);
extern INT8U * tk_mov_get_waveformatex(void);
extern INT32S mov_mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mov_mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mov_mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S mov_mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct);
#endif		// __AP_MOV_MJPEG_H__