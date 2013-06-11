#ifndef __GPLIB_MOTION_JPEG_H__
#define __GPLIB_MOTION_JPEG_H__

#include "turnkey_image_task.h"

#if 0  /*remove by jandy, 2008/10/15*/
#define ALIGN4  __align(4)
#define ALIGN8  __align(8)
#define ALIGN16 __align(16)
#define ALIGN32 __align(32)
#endif

/*extern functions */
extern INT16S tk_avi_audiofilehandle_get(void);
extern INT32S tk_avi_get_audio_data(INT32U framenum);
extern INT32S tk_avi_read_audio_data(CHAR *buf,INT32U len);
extern INT32U tk_avi_decode_file_name(CHAR* pstring);
extern INT32U tk_avi_decode_file_id(INT32U FileNum);
extern INT32U tk_avi_get_audio_samplerate(void);
extern INT32S tk_avi_get_width(void);
extern INT32S tk_avi_get_height(void);
extern INT32S tk_avi_find_vdpos(INT32U JumpFrameNum);
extern INT32U tk_avi_close_file_id(INT32U FileNum);
extern INT32S tk_avi_get_hassound(void);
extern INT32S tk_avi_get_scale(void);
extern INT32S tk_avi_get_rate(void);
extern INT8U* tk_avi_get_waveformatex(void);
extern INT16U tk_avi_wave_bitformat_get(void);
extern INT16U tk_avi_wave_channelnum_get(void);
extern INT16U tk_avi_wave_format_type_get(void);
extern FP64   tk_avi_get_fps(void);
extern INT32S avi_settime_frame_pos_get(INT32S time);
extern void   avi_idx1_tabel_free(void);
extern INT32S avi_idx1_totalframe_get(void);

extern INT32S mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
extern INT32S mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT32S mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
extern INT16U avi_mov_switchflag;
#ifndef _DPF_PROJECT
extern INT32U tk_avi_decode_file_parsing(INT16S avi_movhandle, INT16S avi_audhandle);
extern INT32S mjpeg_file_decode_nth_frame(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U nth);
#endif

#endif		// __GPLIB_MOTION_JPEG_H__