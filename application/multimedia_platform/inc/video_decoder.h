#ifndef __Video_CODEC_APP_H__
#define __Video_CODEC_APP_H__

#include "task_video_decoder.h"

extern void video_decode_entrance(void);
extern void video_decode_exit(void);
extern void avi_audio_decode_rampup(void);
extern void avi_audio_decode_rampdown(void);
extern VIDEO_CODEC_STATUS video_decode_Info(VIDEO_INFO *info);
extern VIDEO_CODEC_STATUS video_decode_paser_header(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src);
extern CODEC_START_STATUS video_decode_start(VIDEO_ARGUMENT arg, MEDIA_SOURCE src);
extern VIDEO_CODEC_STATUS video_decode_stop(void);
extern VIDEO_CODEC_STATUS video_decode_pause(void);
extern VIDEO_CODEC_STATUS video_decode_resume(void);
extern void audio_decode_volume(INT8U volume);
extern VIDEO_CODEC_STATUS video_decode_status(void);
extern VIDEO_CODEC_STATUS video_decode_set_play_time(INT32S SecTime);
extern INT32U video_decode_get_current_time(void);
extern VIDEO_CODEC_STATUS video_decode_set_play_speed(FP32 speed);
extern VIDEO_CODEC_STATUS video_decode_set_reserve_play(INT32U enable);
extern VIDEO_CODEC_STATUS video_decode_get_nth_video_frame(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src, INT32U nth_frame);
extern void video_decode_get_display_size(INT16U *pwidth, INT16U *pheight);
#endif

