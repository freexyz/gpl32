#ifndef __VIDEO_CODEC_CALLBACK_H__
#define __VIDEO_CODEC_CALLBACK_H__
#include "application.h"
#include "drv_l1_sfr.h"

//IMAGE_OUTPUT_FORMAT_RGB565
//IMAGE_OUTPUT_FORMAT_RGBG, IMAGE_OUTPUT_FORMAT_GRGB
//IMAGE_OUTPUT_FORMAT_UYVY, IMAGE_OUTPUT_FORMAT_YUYV
#define DISPLAY_OUTPUT_FORMAT			IMAGE_OUTPUT_FORMAT_YUYV
#define VIDEO_DISPALY_WITH_PPU			0 	//0; disable, 1;enable
#if (defined MCU_VERSION) && (MCU_VERSION == GPL326XXB)
#define FACE_DETECTION                  1   //0; disable, 1;enable
#else
#define FACE_DETECTION                  0   //0; disable, 1;enable
#endif
#define PPU_FRAME_NO					2

// tft io pin setting
#define	PPU_YUYV_TYPE3					(3<<20)
#define	PPU_YUYV_TYPE2					(2<<20)
#define	PPU_YUYV_TYPE1					(1<<20)
#define	PPU_YUYV_TYPE0					(0<<20)

#define	PPU_RGBG_TYPE3					(3<<20)
#define	PPU_RGBG_TYPE2					(2<<20)
#define	PPU_RGBG_TYPE1					(1<<20)
#define	PPU_RGBG_TYPE0					(0<<20)

#define PPU_LB							(1<<19)
#define	PPU_YUYV_MODE					(1<<10)
#define	PPU_RGBG_MODE			        (0<<10)

#define TFT_SIZE_1024X768               (7<<16)
#define TFT_SIZE_800X600                (6<<16)
#define TFT_SIZE_800X480                (5<<16)
#define TFT_SIZE_720X480				(4<<16)
#define TFT_SIZE_480X272				(3<<16)
#define TFT_SIZE_480X234				(2<<16)
#define TFT_SIZE_640X480                (1<<16)
#define TFT_SIZE_320X240                (0<<16)

#define	PPU_YUYV_RGBG_FORMAT_MODE		(1<<8)
#define	PPU_RGB565_MODE			        (0<<8)

#define	PPU_FRAME_BASE_MODE			    (1<<7)
#define	PPU_VGA_NONINTL_MODE			(0<<5)

#define	PPU_VGA_MODE					(1<<4)
#define	PPU_QVGA_MODE					(0<<4)

//video decode
extern void video_decode_end(void);
extern void video_decode_FrameReady(INT8U *FrameBufPtr);

//video encode
extern INT32U video_encode_sensor_start(INT16U width, INT16U height, INT32U csi_frame1, INT32U csi_frame2);
extern INT32U video_encode_sensor_stop(void);
extern INT32S video_encode_display_frame_ready(INT32U frame_buffer);
extern INT32S  video_encode_frame_ready(void *workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag);
extern void video_encode_end(void *workmem);

extern INT32U video_encode_audio_sfx(INT16U *PCM_Buf, INT32U cbLen);
extern INT32U video_encode_video_sfx(INT32U buf_addr, INT32U cbLen);

//display 
extern void user_defined_set_sensor_irq(INT32U enable);
extern void user_defined_set_sensor_fea(INT8U action);
extern void user_defined_video_codec_entrance(void);
extern void user_defined_video_codec_isr(void);
extern void csi_tg_irq_handle(void);
extern void video_codec_show_image(INT32U TV_TFT, INT32U BUF,INT32U DISPLAY_MODE ,INT32U SHOW_TYPE);
extern void video_codec_ppu_configure(void);
#endif
