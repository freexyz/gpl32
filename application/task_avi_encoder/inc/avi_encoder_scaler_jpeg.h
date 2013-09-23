#ifndef AVI_ENCODER_SCALER_JPEG_H_
#define AVI_ENCODER_SCALER_JPEG_H_

#include "application.h"
//=====================================================================================================
/* avi encode configure set */
//video encode mode
//jpeg encode fifo mode only support by GPL32600, when use fifo mode sensor must set fifo mode at same time
#define C_VIDEO_ENCODE_FRAME_MODE		0
#define C_VIDEO_ENCODE_FIFO_MODE		1
#if (defined MCU_VERSION) && ((MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA))
#define FACE_DETECTION_MODE                  1   //0; disable, 1;enable
#else
#define FACE_DETECTION_MODE                  0   //0; disable, 1;enable
#endif

#if (CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_IRQ) 
	#define VIDEO_ENCODE_MODE			C_VIDEO_ENCODE_FRAME_MODE
#else	
	#define VIDEO_ENCODE_MODE			C_VIDEO_ENCODE_FIFO_MODE
#endif

//video 
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE 
	#define AVI_ENCODE_DIGITAL_ZOOM_EN		0	//0: disable digital zoom in/out 1: enable digital zoom in/out 
	#define AVI_ENCODE_PREVIEW_DISPLAY_EN	1	//0: disable display preview	 1: enable display preview
#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
	#define AVI_ENCODE_DIGITAL_ZOOM_EN		0	//fix to 0 when use fifo mode
	#define AVI_ENCODE_PREVIEW_DISPLAY_EN	0	//fix to 0 when use fifo mode
#endif
#define AVI_ENCODE_PRE_ENCODE_EN		0		//0: disable, 1:enable audio and video pre-encode 
#define AVI_ENCODE_VIDEO_ENCODE_EN		1		//0: disable, 1:enable; video record enable
#define AVI_ENCODE_FAST_SWITCH_EN		0		//0: disable, 1:enable; video encode fast stop and start 
#define AVI_ENCODE_VIDEO_TIMER			TIMER_C //timer, A,B,C
#define AVI_ENCODE_TIME_BASE			50		//timer frequency must >= frame rate
#define AVI_ENCODE_SHOW_TIME			0		//0: disable, 1:enable

//buffer number 
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE 
	#if FACE_DETECTION_MODE == 1
		#define AVI_ENCODE_CSI_BUFFER_NO	3	//sensor use buffer number
	#else
		#define AVI_ENCODE_CSI_BUFFER_NO	2	//sensor use buffer number
    #endif
#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
	#define AVI_ENCODE_CSI_BUFFER_NO	2  	//fix to 2 when use fifo mode
#endif
#define AVI_ENCODE_CSI_FIFO_NO			32	//sensor fifo mode use buffer number		
#if FACE_DETECTION_MODE == 1
	#define AVI_ENCODE_SCALER_BUFFER_NO		3	//scaler use buffer number
	#define AVI_ENCODE_DISPALY_BUFFER_NO	3	//display use buffer number
#else
	#define AVI_ENCODE_SCALER_BUFFER_NO		2	//scaler use buffer number
	#define AVI_ENCODE_DISPALY_BUFFER_NO	2	//display use buffer number
#endif
#define AVI_ENCODE_VIDEO_BUFFER_NO		2	//jpeg encode use buffer number
#define AVI_ENCODE_PCM_BUFFER_NO		3	//audio record pcm use buffer number

//audio format	
#define AVI_ENCODE_AUDIO_FORMAT			WAVE_FORMAT_PCM //0: no audio, WAVE_FORMAT_PCM, WAVE_FORMAT_IMA_ADPCM and WAVE_FORMAT_ADPCM 
//audio encode buffer size
#define C_WAVE_ENCODE_TIMES				15	//audio encode times
#define USB_AUDIO_PCM_SAMPLES 			1000

//mic phone input source  
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
#define C_ADC_LINE_IN					0	//use adc hardware 
#define C_GPY0050_IN					1	//use external chip via spi interface
#define C_BUILDIN_MIC_IN				2	//use build-in mic, GPL32600 and GPL32700 support
#define MIC_INPUT_SRC					C_BUILDIN_MIC_IN
#else
#define C_BUILDIN_MIC_IN				0xFF//no built-in MIC for GPL32_A/B/C
#define C_ADC_LINE_IN					0	//use adc hardware 
#define C_GPY0050_IN					1	//use external chip via spi interface
#define MIC_INPUT_SRC					C_ADC_LINE_IN
#endif

#if (MIC_INPUT_SRC == C_ADC_LINE_IN) || (MIC_INPUT_SRC == C_BUILDIN_MIC_IN)
	#define AVI_AUDIO_RECORD_TIMER		ADC_AS_TIMER_F  //adc use timer, C ~ F
	#define AVI_AUDIO_RECORD_ADC_CH		ADC_LINE_1		//adc use channel, 0 ~ 3
#elif MIC_INPUT_SRC == C_GPY0050_IN
	#define AVI_AUDIO_RECORD_TIMER		TIMER_B		 	//timer, A,B,C
	#define C_GPY0050_SPI_CS_PIN		IO_F6			//gpy0500 spi interface cs pin
#endif

//avi file max size
#define AVI_ENCODE_CAL_DISK_SIZE_EN		1				//0: disable, 1: enable
#define AVI_FILE_MAX_RECORD_SIZE		2000000000		//2GB
#define C_MIN_DISK_FREE_SIZE			512*1024		//512K

//avi packer buffer
#define FileWriteBuffer_Size			16*32*1024		//avi pack buffer size, must multiple 32k 
#define IndexBuffer_Size				64*1024			//fix to 64k 		

//mpeg4
#define MPEG4_ENCODE_ENABLE				0				//1: enable, 0: disable
#define C_MP4_INPUT_VY1UY0				0x00000
#define C_MP4_INPUT_Y1UY0V				0x00001
#define C_MP4_INPUT_Y1VY0U				0x00002
#define C_MP4_INPUT_UY1VY0				0x00003
#define MP4_ENC_INPUT_FORMAT			C_MP4_INPUT_Y1UY0V

//video encode fifo mode, fifo line set
#define SENSOR_FIFO_8_LINE				(1<<3)
#define SENSOR_FIFO_16_LINE				(1<<4)
#define SENSOR_FIFO_32_LINE				(1<<5)

#if CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
#define SENSOR_FIFO_LINE				SENSOR_FIFO_8_LINE
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
#define SENSOR_FIFO_LINE				SENSOR_FIFO_16_LINE
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
#define SENSOR_FIFO_LINE				SENSOR_FIFO_32_LINE
#else
#define SENSOR_FIFO_LINE				1
#endif

//video format
#define C_XVID_FORMAT					0x44495658
#define	C_MJPG_FORMAT					0x47504A4D
#define AVI_ENCODE_VIDEO_FORMAT			C_MJPG_FORMAT //only support mjpeg 

//scale setting
#define SCALE_INTP_EN					0
//=====================================================================================================
//=====================================================================================================
/* avi encode status */
#define C_AVI_ENCODE_IDLE       0x00000000
#define C_AVI_ENCODE_PACKER0	0x00000001
#define C_AVI_ENCODE_AUDIO      0x00000002
#define C_AVI_ENCODE_VIDEO      0x00000004
#define C_AVI_ENCODE_SCALER     0x00000008
#define C_AVI_ENCODE_SENSOR     0x00000010
#define C_AVI_ENCODE_PACKER1	0x00000020
#define C_AVI_ENCODE_MD			0x00000040
#define C_AVI_ENCODE_USB_WEBCAM	0x00000080

#define C_AVI_ENCODE_START      0x00000100
#define C_AVI_ENCODE_PAUSE		0x00000200
#define C_AVI_ENCODE_PRE_START	0x00000400
#define C_AVI_ENCODE_JPEG		0x00000800	
#define C_AVI_ENCODE_ERR		0x20000000
#define C_AVI_ENCODE_FRAME_END	0x40000000
#define C_AVI_ENCODE_FIFO_ERR	0x80000000

//avi encode
typedef enum
{
	MSG_AVI_START_SENSOR = 0x1000,
	MSG_AVI_STOP_SENSOR,
	MSG_AVI_START_ENCODE,
	MSG_AVI_STOP_ENCODE,
	MSG_AVI_RESUME_ENCODE,
	MSG_AVI_PAUSE_ENCODE,
	MSG_AVI_CAPTURE_PICTURE,
	MSG_AVI_STORAGE_FULL,
	MSG_AVI_STORAGE_ERR,
	MSG_AVI_ENCODE_STATE_EXIT,
	MSG_AVI_ONE_FRAME_ENCODE,
	MSG_AVI_START_USB_WEBCAM,
	MSG_AVI_STOP_USB_WEBCAM  
}AVI_ENCODE_STATE_ENUM;

//scaler message
typedef enum
{
	MSG_SCALER_TASK_INIT = 0x2000,
	MSG_SCALER_TASK_STOP,
	MSG_SCALER_TASK_EXIT
}AVI_ENCODE_SCALER_ENUM;

//video encode message
typedef enum
{
	MSG_VIDEO_ENCODE_TASK_MJPEG_INIT = 0x3000,
	MSG_VIDEO_ENCODE_TASK_MPEG4_INIT,
	MSG_VIDEO_ENCODE_TASK_STOP,
	MSG_VIDEO_ENCODE_TASK_EXIT
}AVI_ENCODE_VIDEO_ENUM;

typedef enum
{
	AVI_AUDIO_DMA_DONE = 1,//C_DMA_STATUS_DONE
	AVI_AUDIO_RECORD_START = 0x4000,
	AVI_AUDIO_RECORD_STOP,
	AVI_AUDIO_RECORD_STOPING,
	AVI_AUDIO_RECORD_RESTART,
	AVI_AUDIO_RECORD_EXIT
}AVI_ENCODE_AUDIO_ENUM;

//gpy0500 command
#define C_CMD_RESET_IN0				0x81
#define C_CMD_RESET_IN1				0x83
#define C_CMD_RESET_IN2				0x85
#define C_CMD_RESET_IN3				0x87
#define C_CMD_RESET_IN4				0x89		
#define C_CMD_ENABLE_ADC			0x98
#define C_CMD_ENABLE_MIC_AGC_ADC	0x9B
#define C_CMD_ENABLE_MIC_ADC		0x99
#define C_CMD_ENABLE_MIC_AGC		0x93
#define C_CMD_DUMMY_COM				0xC0
#define C_CMD_ADC_IN0				0x80
#define C_CMD_ADC_IN1				0x82
#define C_CMD_ADC_IN2				0x84
#define C_CMD_ADC_IN3				0x86
#define C_CMD_ADC_IN4				0x88
#define C_CMD_ZERO_COM				0x00
#define C_CMD_POWER_DOWN			0x90
#define C_CMD_TEST_MODE				0xF0

//other
#define C_ACK_SUCCESS	0x00000001
#define C_ACK_FAIL		0x80000000

//scaler
#define C_SCALER_FULL_SCREEN				0
#define C_SCALER_BY_RATIO		  			1
#define C_SCALER_FULL_SCREEN_BY_RATIO  		2
#define C_SCALER_FULL_SCREEN_BY_DIGI_ZOOM	3

#ifndef DBG_MESSAGE
	#define DBG_MESSAGE 0
#endif

#if DBG_MESSAGE
	#define DEBUG_MSG(x)	{x;}
#else
	#define DEBUG_MSG(x)	{}
#endif

#define RETURN(x)	{nRet = x; goto Return;}
#define POST_MESSAGE(msg_queue, message, ack_mbox, msc_time, msg, err)\
{\
	msg = (INT32S) OSMboxAccept(ack_mbox);\
	err = OSQPost(msg_queue, (void *)message);\
	if(err != OS_NO_ERR)\
	{\
		DEBUG_MSG(DBG_PRINT("OSQPost Fail!!!\r\n"));\
		RETURN(STATUS_FAIL);\
	}\
	msg = (INT32S) OSMboxPend(ack_mbox, msc_time/10, &err);\
	if(err != OS_NO_ERR || msg == C_ACK_FAIL)\
	{\
		DEBUG_MSG(DBG_PRINT("OSMbox ack Fail!!!\r\n"));\
		RETURN(STATUS_FAIL);\
	}\
}

#endif //AVI_ENCODER_SCALER_JPEG_H_
