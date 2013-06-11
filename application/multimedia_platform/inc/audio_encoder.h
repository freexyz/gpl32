#ifndef __AUDIO_ENCODER_H__
#define __AUDIO_ENCODER_H__

#include "application.h"
#include "audio_record.h"

CODEC_START_STATUS audio_encode_start(MEDIA_SOURCE src, INT16U SampleRate, INT32U BitRate);
void audio_encode_stop(void);
AUDIO_CODEC_STATUS audio_encode_status(void);
CODEC_START_STATUS audio_encode_set_downsample(INT8U bEnable, INT8U DownSampleFactor);

#endif