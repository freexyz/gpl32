#ifndef __TASK_VIDEO_DECODER_CFG_H__
#define __TASK_VIDEO_DECODER_CFG_H__

#define VIDEO_DECODE_TIMER			TIMER_C
#define VIDEO_FRAME_NO				3	//video frame number
#define DEBLOCK_FRAME_NO			3	//deblock frame number for mpeg4 use
#define SCALER_FRAME_NO				3 	//scaler frame number for mpeg4 use
#define AUDIO_FRAME_NO				3	//audio frame number

#define TIME_BASE_TICK_RATE			100	//video timer frequency
#define DAC_RAMP_DOWN_STEP			128
#define DAC_RAMP_DOWN_STEP_HOLD 	4

#if (MCU_VERSION >= GPL326XX && MCU_VERSION < GPL327XX)
	#define MPEG4_DECODE_ENABLE		1	//0: disable, 1:enable
#else
	#define MPEG4_DECODE_ENABLE		0	//fixed 0
#endif

#define S263_DECODE_ENABLE			0	//fix 0: disable
#define AAC_DECODE_ENABLE			1	//0: disable, 1:enable	
#if APP_MP3_DECODE_EN == 1
	#define MP3_DECODE_ENABLE		1	//0: disable, 1:enable
#else
	#define MP3_DECODE_ENABLE		0	//fix 0: disable
#endif

#define VOICE_CHANGER_EN			0	//fix 0: disable
#if APP_UP_SAMPLE_EN == 1
	#define UP_SAMPLE_EN			0	//0: disable, 1:enable 
#else
	#define UP_SAMPLE_EN			0	//fix 0: disable
#endif

#define PARSER_AUD_BITSTREAM_SIZE	WAV_DEC_BITSTREAM_BUFFER_SIZE //MP3_DEC_BITSTREAM_BUFFER_SIZE
#define PARSER_VID_BITSTREAM_SIZE	16384 //byte
#define AUDIO_PCM_BUFFER_SIZE		4096<<1 //byte

#define PARSER_DROP					0x00
#define DECODE_DROP					0x01
#define DROP_MENTHOD				PARSER_DROP

#define C_MP4_OUTPUT_VY1UY0			0x0	//ppu type 0
#define C_MP4_OUTPUT_Y1UY0V			0x1	//ppu type 3
#define C_MP4_OUTPUT_Y1VY0U			0x2	//ppu type 1
#define C_MP4_OUTPUT_UY1VY0			0x3	//ppu type 2 
/////////////////////////////////////////////////////////////
//define video_decode_status
#define C_VIDEO_DECODE_IDLE     	0x00000000
#define C_VIDEO_DECODE_PLAY			0x00000001		  	
#define C_VIDEO_DECODE_PARSER		0x00000002
#define C_VIDEO_DECODE_PAUSE		0x00000004
#define C_VIDEO_DECODE_PLAYING		0x00000010
#define C_AUDIO_DECODE_PLAYING		0x00000020
#define C_VIDEO_DECODE_PARSER_NTH	0x00000040
#define C_VIDEO_DECODE_ERR      	0x80000000
#define C_VIDEO_DECODE_ALL			C_VIDEO_DECODE_PLAY|C_VIDEO_DECODE_PARSER|C_VIDEO_DECODE_PAUSE|C_VIDEO_DECODE_PLAYING|C_AUDIO_DECODE_PLAYING|C_VIDEO_DECODE_ERR

//mpeg4 decoder flag
#define C_MP4_STATUS_STOP			0x00000001
#define C_MP4_STATUS_DECODEING		0x00000002
#define C_MP4_STATUS_ENDECODEING	0x00000002
#define C_MP4_STATUS_END_OF_FRAME	0x00000004
#define C_MP4_STATUS_START_OF_FRAME	0x00000008
#define C_MP4_STATUS_DEC_ERR		0x00000010
#define C_MP4_STATUS_VLD_RESTART	0x00000020
#define C_MP4_STATUS_BUSY			0x00000040

#define C_MP4_STATUS_INIT_ERR		0x80001000
#define C_MP4_STATUS_TIMEOUT		0x80002000
#define C_MP4_STATUS_UNDEF			0x80004000

//////////////////////////////////////////////////////////////
//mepg4 bit stream define
#define VIDOBJ_START_CODE			0x00000100	// ..0x0000011f  
#define VIDOBJLAY_START_CODE		0x00000120	// ..0x0000012f 
#define VISOBJSEQ_START_CODE		0x000001b0
#define VISOBJSEQ_STOP_CODE			0x000001b1	// ??? 
#define USERDATA_START_CODE			0x000001b2
#define GRPOFVOP_START_CODE			0x000001b3
#define VIDSESERR_ERROR_CODE  		0x000001b4 
#define VISOBJ_START_CODE			0x000001b5
#define VOP_START_CODE				0x000001b6
#define STUFFING_START_CODE			0x000001c3 

#define VIDOBJ_START_CODE_MASK		0x0000001F
#define VIDOBJLAY_START_CODE_MASK	0x0000000F

#define VISOBJ_TYPE_VIDEO			1
#define VIDOBJLAY_AR_EXTPAR			15

#define VIDOBJLAY_SHAPE_RECTANGULAR	0
#define VIDOBJLAY_SHAPE_BINARY		1
#define VIDOBJLAY_SHAPE_BINARY_ONLY	2
#define VIDOBJLAY_SHAPE_GRAYSCALE	3

#define SPRITE_NONE		0
#define SPRITE_STATIC	1
#define SPRITE_GMC		2

#define C_I_VOP			0x00
#define C_P_VOP			0x01
#define C_B_VOP			0x02
#define C_S_VOP			0x03
#define C_N_VOP			0x04
#define C_UNKNOW_VOP	0x10

#define ASP_ENABLE		1	//advance simple profile
/////////////////////////////////////////////////////////////
#define P_DAC_CHA_DATA       		((volatile INT32U *) 0xC00D0004)
#define R_DAC_CHA_DATA              (*((volatile INT32U *) 0xC00D0004))
#define R_DAC_CHB_DATA              (*((volatile INT32U *) 0xC00D0024))
#define R_DAC_CHA_FIFO              (*((volatile INT32U *) 0xC00D0008))
#define R_DAC_CHB_FIFO              (*((volatile INT32U *) 0xC00D0028))
#define R_TIMERE_PRELOAD			(*((volatile INT32U *) 0xC0020088))

/////////////////////////////////////////////////////////////
typedef enum
{
	MSG_VIDEO_DECODE_PARSER_HEADER_START = 0x1000,
	MSG_VIDEO_DECODE_PARSER_HEADER_STOP,
	MSG_VIDEO_DECODE_START,
	MSG_VIDEO_DECODE_STOP,	
	MSG_VIDEO_DECODE_PAUSE,
	MSG_VIDEO_DECODE_RESUME,
	MSG_VIDEO_DECODE_GET_CUR_TIME,
	MSG_VIDEO_DECODE_SET_SEEK,
	MSG_VIDEO_DECODE_SET_SEEK_PLAY,
	MSG_VIDEO_DECODE_SET_PLAY_SPEED,
	MSG_VIDEO_DECODE_SET_REVERSE,
	MSG_VIDEO_DECODE_NTH_FRAME,
	MSG_VIDEO_DECODE_VID_END,
	MSG_VIDEO_DECODE_AUD_END,
	MSG_VIDEO_DECODE_ERROR,
	MSG_VIDEO_DECODE_EXIT 
} MSG_VID_DEC_STATE_ENUM;

typedef enum
{
	MSG_AUD_DEC_DMA_DONE = C_DMA_STATUS_DONE,
	MSG_AUD_DEC_START = 0x2000,
	MSG_AUD_DEC_STOP,
	MSG_AUD_DEC_EXIT
} MSG_AUD_DEC_ENUM;

typedef enum
{
	MSG_VID_DEC_ONE_FRAME = 0x3000,
	MSG_VID_DEC_START,
	MSG_VID_DEC_RESTART,
	MSG_VID_DEC_STOP,
	MSG_VID_DEC_NTH, 
	MSG_VID_DEC_EXIT
} MSG_VID_DEC_ENUM;

enum
{
	ERROR_00 = 0x8000,
	ERROR_01,
	ERROR_02,
	ERROR_03,
	ERROR_04,
	ERROR_05,
	ERROR_06,
	ERROR_07,
	ERROR_08,
	ERROR_09,
	ERROR_0A,
	ERROR_0B,
	ERROR_0C,
	ERROR_0D,
	ERROR_0E,
	ERROR_0F,
	ERROR_10,
	ERROR_11,
	ERROR_12,
	ERROR_13,
	ERROR_14,
	ERROR_15
};

// file type
#define FILE_TYPE_AVI		1
#define FILE_TYPE_MOV		2

// codec type
#define C_MPEG4_CODEC			0x01
#define C_H263_CODEC			0x02 
#define C_SORENSON_H263_CODEC	0x04

// video_format
#define C_XVID_FORMAT		0x44495658
#define C_M4S2_FORMAT		0x3253344D
#define C_H263_FORMAT		0x33363248
#define C_SOREN_H263_FORMAT	0x00000000
#define C_MJPG_FORMAT		0x47504A4D

// scaler mode
#define C_NO_SCALER				0
#define C_SCALER_FULL_SCREEN	1
#define C_NO_SCALER_FIT_BUFFER  2
#define C_SCALER_FIT_BUFFER		3

// other
#define C_ACK_SUCCESS		0x00000001
#define C_ACK_FAIL			0x80000000

#ifndef DBG_MESSAGE 
	#define DBG_MESSAGE 0
#endif

#if DBG_MESSAGE == 1
	#define DEBUG_MSG(x)	{x;}
#else
	#define DEBUG_MSG(x)	{}
#endif

#define RETURN(x)	{nRet = x; goto Return;}
#define SEND_MESSAGE(msg_queue, message, ack_mbox, msc_time, msg, err)\
{\
	msg = (INT32S) OSMboxAccept(ack_mbox);\
	err = OSQPost(msg_queue, (void *)message);\
	if(err != OS_NO_ERR)\
	{\
		DEBUG_MSG(DBG_PRINT("OSQPost Fail!!!\r\n"));\
		RETURN(STATUS_FAIL);\
	}\
	msg = (INT32S) OSMboxPend(ack_mbox, msc_time/10, &err);\
	if(err != OS_NO_ERR)\
	{\
		DEBUG_MSG(DBG_PRINT("OSMbox Ack Fail!!!\r\n"));\
		RETURN(STATUS_FAIL);\
	}\
	if(msg == C_ACK_FAIL)\
	{\
		DEBUG_MSG(DBG_PRINT("Return Fail!!!\r\n"));\
		RETURN(STATUS_FAIL);\
	}\
}
#endif
