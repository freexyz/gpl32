#ifndef __AUDIO_CODEC_CALLBACK_H__
#define __AUDIO_CODEC_CALLBACK_H__

#include "application.h"

extern void audio_decode_end(INT32U audio_decoder_num);	// modified by Bruce, 2008/11/20
INT32S audio_encoded_data_read(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);	// added by Bruce, 2008/10/27
INT32U audio_encode_data_write(INT8U bHeader, INT32U buffer_addr, INT32U cbLen);

#endif