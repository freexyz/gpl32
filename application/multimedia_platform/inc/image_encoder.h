#ifndef __IMAGE_ENCODER_H__
#define __IMAGE_ENCODER_H__

#include "application.h"

typedef struct {
        INT32U       encodebufferptr;
        INT32U       encodesize;        
} ENCODE_BLOCK_WRITE_ARGUMENT;

//Image Encode /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void image_encode_entrance(void);
extern void image_encode_exit(void);
extern INT32U image_encode_start(IMAGE_ENCODE_ARGUMENT arg);
extern void image_encode_stop(void);
extern IMAGE_CODEC_STATUS image_encode_status(void);

#endif