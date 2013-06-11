#ifndef __GPID_AUDIO_H__
#define __GPID_AUDIO_H__

#include "application.h"

//=============================================================================
// Define
//=============================================================================
// for gpid_audio_get_status()
#define C_GPID_AUDIO_STATUS_STOP	0
#define C_GPID_AUDIO_STATUS_PLAY	1
#define C_GPID_AUDIO_STATUS_PAUSE	2

#define C_GPID_AUDIO_FORMAT_MP3		1
#define C_GPID_AUDIO_FORMAT_A1800	3

//=============================================================================
// Function Declaration
//=============================================================================
void gpid_audio_init(INT16S fd);
INT8U gpid_audio_get_status(void);
void gpid_audio_play(void);
void gpid_audio_ext_res_play(unsigned long index);
void gpid_audio_stop(void);
void gpid_audio_pause(void);
void gpid_audio_resume(void);
void gpid_audio_set_audio_format(AUDIO_FORMAT format);
AUDIO_FORMAT gpid_audio_get_audio_format(void);
void gpid_audio_set_volume(INT32U volume);
void gpid_audio_volume_up(void);
void gpid_audio_volume_down(void);
BOOLEAN gpid_audio_is_mute(void);
void gpid_audio_mute(void);
void gpid_audio_unmute(void);
void gpid_audio_to_n(INT32U idx);

#endif