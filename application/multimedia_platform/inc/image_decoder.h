#ifndef __IMAGE_DECODER_H__
#define __IMAGE_DECODER_H__

#include "application.h"

//Image Decode /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void image_decode_entrance(void);						// peripheral setting, global variable initial, memory allocation
extern void image_decode_exit(void);							// peripheral unset, global variable reset, memory free
extern CODEC_START_STATUS image_decode_Info(IMAGE_INFO *info,MEDIA_SOURCE src);							
extern CODEC_START_STATUS image_decode_start(IMAGE_ARGUMENT arg,MEDIA_SOURCE src);						
extern void image_decode_stop(void); 
extern IMAGE_CODEC_STATUS image_decode_status(void);
extern CODEC_START_STATUS image_scale_start(IMAGE_SCALE_ARGUMENT arg);
extern CODEC_START_STATUS image_block_encode_scale(IMAGE_SCALE_ARGUMENT *scaler_arg,IMAGE_ENCODE_PTR *output_ptr);
extern IMAGE_CODEC_STATUS image_scale_status(void);
 
#endif