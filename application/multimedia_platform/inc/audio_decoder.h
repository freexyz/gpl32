#ifndef __AUDIO_DECODER_H__
#define __AUDIO_DECODER_H__

#include "application.h"

extern void audio_decode_entrance(void);						// peripheral setting, global variable initial, memory allocation
extern void audio_decode_exit(void);							// peripheral unset, global variable reset, memory free
extern void audio_decode_rampup(void);							// Ramp to Middle Level 0x80(Standby)
extern void audio_decode_rampdown(void);						// Ramp to Zero Level 0x0(Power Saving)
extern CODEC_START_STATUS audio_decode_start(AUDIO_ARGUMENT arg, MEDIA_SOURCE src);
extern void audio_decode_stop(AUDIO_ARGUMENT arg);				// resource should be completely released if card accendentially plug out
extern void audio_decode_pause(AUDIO_ARGUMENT arg);
extern void audio_decode_resume(AUDIO_ARGUMENT arg);	
extern void audio_decode_volume_set(AUDIO_ARGUMENT *arg, int volume);
extern void audio_decode_mute(AUDIO_ARGUMENT *arg);
extern void audio_decode_unmute(AUDIO_ARGUMENT *arg);
extern AUDIO_CODEC_STATUS audio_decode_status(AUDIO_ARGUMENT arg);
extern void audio_decode_get_info(AUDIO_ARGUMENT arg, MEDIA_SOURCE src, AUDIO_INFO *audio_info);

#endif