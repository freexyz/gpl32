#ifndef __IMAGE_CODEC_CALLBACK_H__
#define __IMAGE_CODEC_CALLBACK_H__

#include "application.h"

//Image Callback /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void image_decode_end(void);
extern void image_encode_end(INT32U encode_size);
extern CODEC_START_STATUS image_encoded_block_read(IMAGE_SCALE_ARGUMENT *scaler_info,IMAGE_ENCODE_PTR *encode_ptr);

#endif