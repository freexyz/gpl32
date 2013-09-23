#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "gplib.h"
#include "application_cfg.h"
#include "drv_l1_sfr.h"

/* Pseudo Header Include there*/
#include "turnkey_sys_msg.h"
#include "turnkey_drv_msg.h"

/* for UART debug message on/off */
#define UART_MSG		1

/* Golbal define*/
#if 0  /*remove by jandy, 2008/10/15*/
#define ALIGN32 __align(32)
#define ALIGN16 __align(16)
#define ALIGN8 __align(8)
#define ALIGN4 __align(4)
#endif

extern struct STFileNodeInfo *pstFNodeInfo;

/* Filesrv Task */
extern INT32S FileSrvScanFileContinue(void);
extern void file_scan_task_entry(void *parm);
extern void filesrv_task_entry(void *p_arg);

extern OS_EVENT	*hFileSrvTaskQ;

/* UMI Task */
extern OS_EVENT	*hUmiTaskQ;
extern OS_EVENT	*hUmiQ;
extern void umi_task_entry(void *para);
extern INT32U umi_photo_nums;

/* AUDIO Task */
extern void audio_send_stop(void);
extern INT8U audio_bg_getstatus(void);

/* audio */
#define MAX_DAC_BUFFERS   5
#define C_DMA_DBF_START   2
#define C_DMA_WIDX_CLEAR  3

#define AUDIO_FORMAT_JUDGE_AUTO  1

#define AUDIO_IDLE        0
#define AUDIO_PLAYING     1
#define AUDIO_PLAY_PAUSE  2
#define AUDIO_PLAY_STOP   3

#if _DRV_L2_EXT_RTC == 1
#define	DATESET_START_YEAR		2000
#else
#define	DATESET_START_YEAR		1980
#endif
#define	DATESET_END_YEAR		2099

// Audio Codec ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern INT32S g_audio_data_offset;	// added by Bruce, 2008/10/28
extern INT32S (*audio_move_data)(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);	// added by Bruce, 2008/10/27
extern INT32S (*audio_bg_move_data)(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);


typedef struct
{
	INT8U	*ring_buf;
	INT8S   *work_mem;
	INT8U   reading;
	INT16S  file_handle;
	INT32U  frame_size;
	INT32U  ring_size;
	INT32U  ri;
	INT32U  wi;
	INT32U   f_last;
	INT32U  file_len;
	INT32U  file_cnt;
	INT32U  try_cnt;
	INT32U  read_secs;
	INT32U	data_offset;
	INT8U	*data_start_addr;
}AUDIO_CTRL;

typedef enum
{
	AUDIO_CMD_START,
	AUDIO_CMD_STOP,
	AUDIO_CMD_NEXT,
	AUDIO_CMD_PREV,
	AUDIO_CMD_PAUSE,
	AUDIO_CMD_RESUME,
	AUDIO_CMD_AVI_START,
	AUDIO_CMD_DECODE_NEXT_FRAME
}AUDIO_CMD;

typedef struct
{
    INT32S curr_play_time;
    INT32S total_play_time;
}ST_AUDIO_PLAY_TIME;

extern void audio_task_entry(void *p_arg);
extern void audio_send_stop(void);
extern OS_EVENT	*hAudioTaskQ;
extern OS_EVENT *SemAudio;
extern AUDIO_CTRL audio_ctrl;		// added by Bruce, 2008/09/18
extern AUDIO_CTRL audio_bg_ctrl;
extern INT32U g_MIDI_index;		// added by Bruce, 2008/10/09
extern void (*decode_end)(INT32U audio_decoder_num);	// modified by Bruce, 2008/11/20

// USB
typedef struct   
{
    INT32U FrameSize;
    INT32U AddrFrame;
    INT8U IsoSendState;
}ISOTaskMsg;

/*==================== Turnkey Solution Application header declare */
/* turn key message manager */
#define MSG_PRI_NORMAL		0
#define MSG_PRI_URGENT		1

typedef struct 
{
	INT32U		maxMsgs;			/* max messages that can be queued */
	INT32U		maxMsgLength;		/* max bytes in a message */
	OS_EVENT*	pEvent;
	void*		pMsgQ;
	INT8U*		pMsgPool;
} *MSG_Q_ID;

//========================================================
//Function Name:msgQCreate
//Syntax:		MSG_Q_ID msgQCreate(INT32U maxQSize, INT32U maxMsgs, INT32U maxMsgLength)
//Purpose:		create and initialize a message queue
//Note:			
//Parameters:   INT32U maxQSize			/* max queue can be creat */
//				INT32U	maxMsgs			/* max messages that can be queued */
//				INT32U	maxMsgLength	/* max bytes in a message */
//Return:		NULL if faile
//=======================================================
extern MSG_Q_ID msgQCreate(INT32U maxQSize, INT32U maxMsgs, INT32U maxMsgLength);

//========================================================
//Function Name:msgQDelete
//Syntax:		void msgQDelete (MSG_Q_ID msgQId)
//Purpose:		delete a message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId		/* message queue to delete */
//Return:		
//=======================================================
extern void msgQDelete (MSG_Q_ID msgQId);

//========================================================
//Function Name:msgQSend
//Syntax:		INT32S msgQSend(MSG_Q_ID msgQId, INT32U msg_id, void *para, INT32U nParaByte, INT32U priority)
//Purpose:		send a message to a message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U msg_id			/* message id */
//				void *para				/* message to send */
//				INT32U nParaByte		/* byte number of para buffer */
//				INT32U priority			/* MSG_PRI_NORMAL or MSG_PRI_URGENT */
//Return:		-1 if faile 
//				0 success
//=======================================================
extern INT32S msgQSend(MSG_Q_ID msgQId, INT32U msg_id, void *para, INT32U nParaByte, INT32U priority);

//========================================================
//Function Name:msgQReceive
//Syntax:		INT32S msgQReceive(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
//Purpose:		receive a message from a message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U *msg_id			/* message id */
//				void *para				/* message and type received */
//				INT32U maxNByte			/* message size */
//Return:		-1: if faile
//				0: success
//=======================================================
extern INT32S msgQReceive(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte);

//========================================================
//Function Name:msgQAccept
//Syntax:		INT32S msgQAccept(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte)
//Purpose:		Check whether a message is available from a message queue
//Note:
//Parameters:   MSG_Q_ID msgQId			/* message queue on which to send */
//				INT32U *msg_id			/* message id */
//				void *para				/* message and type received */
//				INT32U maxNByte			/* message size */
//Return:		-1: queue is empty or fail
//				0: success
//=======================================================
extern INT32S msgQAccept(MSG_Q_ID msgQId, INT32U *msg_id, void *para, INT32U maxParaNByte);

//========================================================
//Function Name:msgQFlush
//Syntax:		void msgQFlush(MSG_Q_ID msgQId)
//Purpose:		flush message queue
//Note:			
//Parameters:   MSG_Q_ID msgQId
//Return:		
//=======================================================
extern void msgQFlush(MSG_Q_ID msgQId);

//========================================================
//Function Name:msgQQuery
//Syntax:		INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata)
//Purpose:		get current Q message information
//Note:			
//Parameters:   MSG_Q_ID msgQId, OS_Q_DATA *pdata
//Return:		
//=======================================================
extern INT8U msgQQuery(MSG_Q_ID msgQId, OS_Q_DATA *pdata);

//========================================================
//Function Name:msgQSizeGet
//Syntax:		INT32U msgQSizeGet(MSG_Q_ID msgQId)
//Purpose:		get current Q message number
//Note:			
//Parameters:   MSG_Q_ID msgQId
//Return:		
//=======================================================
extern INT32U msgQSizeGet(MSG_Q_ID msgQId);


/* TurnKey Task entry declare */
/* Turn Key Audio DAC task */
extern void audio_dac_task_entry(void * p_arg);
extern INT32U      last_send_idx;	// added by Bruce, 2008/12/01

typedef struct 
{
    BOOLEAN   mute;
    INT8U     volume;
    INT16S    fd;
    INT32U    src_id;
    INT32U    src_type;
    INT32U    audio_format;
    INT32U    file_len;
} STAudioTaskPara;

typedef enum
{
	AUDIO_SRC_TYPE_FS=0,
	AUDIO_SRC_TYPE_SDRAM,
	AUDIO_SRC_TYPE_SPI,
	AUDIO_SRC_TYPE_RS,			// added by Bruce, 2009/02/19
	AUDIO_SRC_TYPE_GPRS,
	AUDIO_SRC_TYPE_APP_RS,
	AUDIO_SRC_TYPE_APP_PACKED_RS,
	AUDIO_SRC_TYPE_USER_DEFINE,	// added by Bruce, 2008/10/27
	AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE, // added by Bruce, 2010/01/22
	AUDIO_SRC_TYPE_MAX
}AUDIO_SOURCE_TYPE;

typedef enum
{
	AUDIO_TYPE_NONE = -1,
	AUDIO_TYPE_MIDI,//080903
	AUDIO_TYPE_MP3,
	AUDIO_TYPE_WMA,
	AUDIO_TYPE_WAV,
	AUDIO_TYPE_A1800,//080722
	AUDIO_TYPE_S880,	// added by Bruce, 2008/09/25
	AUDIO_TYPE_AVI,
	AUDIO_TYPE_A1600,	// added by Bruce, 2008/09/23
	AUDIO_TYPE_A6400,	// added by Bruce, 2008/09/25
	AUDIO_TYPE_AVI_MP3,
	AUDIO_TYPE_AAC
}AUDIO_TYPE;

typedef enum
{
	AUDIO_CHANNEL_SPU = 0,
	AUDIO_CHANNEL_DAC,
	AUDIO_CHANNEL_MIDI
}AUDIO_CHANNEL_TYPE;

typedef struct 
{
    MSG_AUD_ENUM result_type;
    INT32S result; 
} STAudioConfirm;

typedef enum
{
	AUDIO_ERR_NONE,
	AUDIO_ERR_FAILED,
	AUDIO_ERR_INVALID_FORMAT,
	AUDIO_ERR_OPEN_FILE_FAIL,
	AUDIO_ERR_GET_FILE_FAIL,
	AUDIO_ERR_DEC_FINISH,
	AUDIO_ERR_DEC_FAIL,
	AUDIO_ERR_READ_FAIL,
	AUDIO_ERR_AVI_READ_FAIL,
	AUDIO_ERR_MEM_ALLOC_FAIL
}AUDIO_STATUS;

extern void audio_task_entry(void * p_arg); 
extern void audio_bg_task_entry(void * p_arg); 
extern MSG_Q_ID	AudioTaskQ;
extern MSG_Q_ID	AudioBGTaskQ;
extern OS_EVENT	*audio_wq;
extern OS_EVENT	*audio_bg_wq;
extern INT16S   *pcm_out[MAX_DAC_BUFFERS];
extern INT32U    pcm_len[MAX_DAC_BUFFERS];
extern INT16S   *pcm_bg_out[MAX_DAC_BUFFERS];
extern INT32U    pcm_bg_len[MAX_DAC_BUFFERS];

// added by Bruce, 2008/10/09
extern void midi_task_entry(void *p_arg);
extern OS_EVENT	*midi_wq;
extern MSG_Q_ID MIDITaskQ;
extern INT16S   *pcm_midi_out[MAX_DAC_BUFFERS];
extern INT32U    pcm_midi_len[MAX_DAC_BUFFERS];
// added by Bruce, 2008/10/09
/*== Turn Key File Server Task ==*/
#define C_FILE_SERVICE_FLUSH_DONE		0xFF010263

typedef struct {
	INT8U   src_id;
	INT8U   src_name[10];
	INT16U  sec_offset;
	INT16U  sec_cnt;
} TK_FILE_SERVICE_SPI_STRUCT, *P_TK_FILE_SERVICE_SPI_STRUCT;

typedef struct {
	INT16S fd;
	INT32U buf_addr;
	INT32U buf_size;
	TK_FILE_SERVICE_SPI_STRUCT spi_para;
	OS_EVENT *result_queue;
	INT32U data_offset;
	INT8U  main_channel;
	INT8U  *data_start_addr;
} TK_FILE_SERVICE_STRUCT, *P_TK_FILE_SERVICE_STRUCT;

typedef struct 
{
	INT16S disk;
	OS_EVENT *result_queue;
} STMountPara;

typedef struct 
{
	struct STFileNodeInfo *pstFNodeInfo;
	OS_EVENT *result_queue;
} STScanFilePara;

typedef struct
{
	INT8U	src_storage_id;
	INT8U	file_type;
	INT32U	file_idx;
	INT8U	dest_storage_id;
	INT8S	dest_name[32];
	INT32S	status;
} TK_FILE_COPY_STRUCT, *P_TK_FILE_COPY_STRUCT;

typedef struct
{
	f_pos	fpos;
	INT32U	file_idx;
	INT32S	status;
} TK_FILE_DELETE_STRUCT, *P_TK_FILE_DELETE_STRUCT;

typedef struct
{
	INT32U	*p_buffer;
	INT32U	size;
	INT8S	ext_name[4];
	INT8U	dest_storage_id;
	INT8S	dest_name[32];
	INT32S	status;
} TK_FILE_SAVE_STRUCT, *P_TK_FILE_SAVE_STRUCT;

extern INT32S file_server_copy(TK_FILE_COPY_STRUCT *para);
extern INT32S file_server_delete(TK_FILE_DELETE_STRUCT *para);
extern INT32S file_server_save(TK_FILE_SAVE_STRUCT *para);
extern void filesrv_task_entry(void * p_arg);

//========================================================
//Function Name:WaitScanFile
//Syntax:		void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index)
//Purpose:		wait for search file
//Note:			
//Parameters:   pstFNodeInfo	/* the point to file node information struct */
//				index			/* the file index you want to find */
//Return:		
//=======================================================
extern void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index);

/*== Turn Key Image processing task ==*/
// The id field of image request is categorized as following: state field(6-bits) + type field(6-bits) + file number(20-bits)
#define IMAGE_CMD_STATE_MASK					0xFC000000
#define IMAGE_CMD_STATE_SHIFT_BITS				26
#define IMAGE_CMD_TYPE_MASK						0x03F00000
#define IMAGE_CMD_TYPE_SHIFT_BITS				20
#define IMAGE_CMD_FILE_INDEX_MASK				0x000FFFFF

typedef enum {
	TK_IMAGE_SOURCE_TYPE_FILE = 0x0,  
	TK_IMAGE_SOURCE_TYPE_BUFFER,
	TK_IMAGE_SOURCE_TYPE_MAX
} TK_IMAGE_SOURCE_TYPE_ENUM;

typedef enum {
	TK_IMAGE_TYPE_JPEG = 0x1,  
	TK_IMAGE_TYPE_PROGRESSIVE_JPEG,
	TK_IMAGE_TYPE_MOTION_JPEG,
	TK_IMAGE_TYPE_BMP,
	TK_IMAGE_TYPE_GIF,
	TK_IMAGE_TYPE_PNG,
	TK_IMAGE_TYPE_GPZP,
	//#ifdef _DPF_PROJECT
	TK_IMAGE_TYPE_MOV_JPEG,		//mov	// added by Bruce, 2009/02/19
	//#endif
	TK_IMAGE_TYPE_MAX
} TK_IMAGE_TYPE_ENUM;

typedef struct {
	INT32U cmd_id;
	INT32S image_source;          	// File handle/resource handle/pointer
	INT32U source_size;             // File size or buffer size
	INT8U source_type;              // TK_IMAGE_SOURCE_TYPE_FILE/TK_IMAGE_SOURCE_TYPE_BUFFER

	INT8S parse_status;				// 0=ok, others=fail
	INT8U image_type;				// TK_IMAGE_TYPE_ENUM
	INT8U orientation;
	INT8U date_time_ptr[20];		// 20 bytes including NULL terminator. Format="YYYY:MM:DD HH:MM:SS"
	INT16U width;
	INT16U height;
} IMAGE_HEADER_PARSE_STRUCT;

typedef struct {
	INT32U cmd_id;
	INT32S image_source;          	// File handle/resource handle/pointer
	INT32U source_size;             // File size or buffer size
	INT8U source_type;              // 0=File System, 1=SDRAM, 2=NVRAM
	INT8S decode_status;            // 0=ok, others=fail
	INT8U reserved;
	INT8U output_ratio;             // 0=Fit to output_buffer_width and output_buffer_height, 1=Maintain ratio and fit to output_buffer_width or output_buffer_height, 2=Same as 1 but without scale up
	INT32U output_format;
	INT16U output_buffer_width;
	INT16U output_buffer_height;
	INT16U output_image_width;
	INT16U output_image_height;
	INT32U out_of_boundary_color;
	INT32U output_buffer_pointer;
} IMAGE_DECODE_STRUCT, *P_IMAGE_DECODE_STRUCT;

typedef struct {
	INT8U	EncodeType;
	INT8U 	QuantizationQuality;		// only 50/70/80/85/90/93/95/97/100.
	INT8U 	InputFormat;				// only C_JPEG_FORMAT_YUYV.
	INT8U 	OutputFormat;				// only C_JPG_CTRL_YUV422.
	INT8S	EncodeState;				// 0 is ok, -N is fail.
	INT16U 	InputWidth;					// must width/16=0.
	INT16U 	InputHeight;				// must height/8=0.
	INT32U 	OutputBuf;					// must align 16-byte, size=encode_size+header_size.
	INT32U 	InputBuf_Y;					// must align 16-byte, size=encode_size+header_size.
	INT32U 	InputBuf_U;					// normal is null.
	INT32U 	InputBuf_V;					// normal is null.
	INT32U	EncodeSize;					// unit is byte.
} IMAGE_ENCODE_STRUCT;

typedef struct {
	IMAGE_DECODE_STRUCT basic;
	INT16U clipping_start_x;
	INT16U clipping_start_y;
	INT16U clipping_width;
	INT16U clipping_height;
} IMAGE_DECODE_EXT_STRUCT, *P_IMAGE_DECODE_EXT_STRUCT;

typedef enum {    
    SCALER_CTRL_IN_RGB1555		=0x00000000,
	SCALER_CTRL_IN_RGB565		=0x00000001,
	SCALER_CTRL_IN_RGBG			=0x00000002,
	SCALER_CTRL_IN_GRGB			=0x00000003,
	SCALER_CTRL_IN_YUYV			=0x00000004,
	SCALER_CTRL_IN_UYVY			=0x00000005,
	SCALER_CTRL_IN_YUV422		=0x00000008,
	SCALER_CTRL_IN_YUV420		=0x00000009,
	SCALER_CTRL_IN_YUV411		=0x0000000A,
	SCALER_CTRL_IN_YUV444		=0x0000000B,
	SCALER_CTRL_IN_Y_ONLY		=0x0000000C,
	SCALER_CTRL_IN_YUV422V		=0x0000000D,
	SCALER_CTRL_IN_YUV411V		=0x0000000E
} SCALER_IN_FORMAT_ENUM;

typedef enum {    
    SCALER_CTRL_OUT_RGB1555=0x00000000,
    SCALER_CTRL_OUT_RGB565 =0x00000010,
    SCALER_CTRL_OUT_RGBG   =0x00000020,
    SCALER_CTRL_OUT_GRGB   =0x00000030,
    SCALER_CTRL_OUT_YUYV   =0x00000040,
    SCALER_CTRL_OUT_UYVY   =0x00000050,
    SCALER_CTRL_OUT_YUYV32 =0x00000060,
    SCALER_CTRL_OUT_YUYV64 =0x00000070,
    SCALER_CTRL_OUT_YUV422 =0x00000080,
    SCALER_CTRL_OUT_YUV420 =0x00000090,
    SCALER_CTRL_OUT_YUV411 =0x000000A0,
    SCALER_CTRL_OUT_YUV444 =0x000000B0,
    SCALER_CTRL_OUT_Y_ONLY =0x000000C0
} SCALER_OUT_FORMAT_ENUM;

typedef struct {
	INT32U cmd_id;
	INT8S status;
	INT8U special_effect;
	INT8U scaler_input_format;
	INT8U scaler_output_format;

	INT16U input_width;
	INT16U input_height;
	INT16U input_visible_width;
	INT16U input_visible_height;
	INT32U input_offset_x;
	INT32U input_offset_y;
	void *input_buf1;
//	void *input_buf2;					// This parameter is valid only when separate YUV input format is used
//	void *input_buf3;					// This parameter is valid only when separate YUV input format is used

	INT16U output_buffer_width;
	INT16U output_buffer_height;
	INT32U output_width_factor;			// factor = (input_size<<16)/output_size
	INT32U output_height_factor;
	INT32U out_of_boundary_color;
	void *output_buf1;
//	void *output_buf2;					// This parameter is valid only when separate YUV output format is used
//	void *output_buf3;					// This parameter is valid only when separate YUV output format is used
} IMAGE_SCALE_STRUCT, *P_IMAGE_SCALE_STRUCT;

typedef struct st_avi_buf {
	INT32U buf_status;	
	INT32U display_mode;
	INT32U avi_image_width;
	INT32U avi_image_height;
	INT32U avi_framenum_silp;
	INT8U *avibufptr; 	
	struct st_avi_buf *next;
}STAVIBufConfig;

typedef struct {
	INT8U  	display_mode;		//bit0: 0 =>use the file num open this file. 1=> user the file name open this file.
								//bit1:	0 =>default size. 1=> user define size				
								//bit2: 0 =>central show. 1=> user define position
	INT8U   *string_ptr;
	#ifndef _DPF_PROJECT
	INT8U 	*Avi_Frame_buf1;
  	INT8U 	*Avi_Frame_buf2;
	#endif
	INT16S	MovFileHandle;
	INT16S  AudFileHandle;
	INT32U	filenum;
	INT32U	display_width;		//user define size.(use this setting , display_mode must be 1)
	INT32U  display_height;		//user define size.(use this setting , display_mode must be 1)
	INT32S	display_position_x; //tft or tv display area central position x.
	INT32S	display_position_y; //tft or tv display area central position y.
	INT32U  first_action;
}AVI_MESSAGE;

typedef enum {
	AUDIO_BUF_FIRST_FILL = 0,
	AUDIO_BUF_FILL_ALL_AGAIN,
	AUDIO_BUF_EMPTY,
	AUDIO_BUF_FILLING,
	AUDIO_BUF_GETING,
	AUDIO_BUF_OVER
}AUDIO_BUF_STATUS;

typedef struct tk_audiobuf{
	INT32U status;
	INT32U ri;
	INT32U wi;
	INT32U length;
	INT8U  *audiobufptr; 	
	struct tk_audiobuf *next;
}STAVIAUDIOBufConfig;

extern MSG_Q_ID    image_msg_queue_id;
extern void image_task_entry(void * p_arg);
extern INT32S image_task_scale_image(IMAGE_SCALE_STRUCT *para);
extern INT32S image_task_parse_image(IMAGE_HEADER_PARSE_STRUCT *para);
extern INT32S image_task_remove_request(INT32U state_id);


/*== Turn Key SYSTEM Task ==*/
typedef enum
{
	STORAGE_DEV_CF=0,
	STORAGE_DEV_NAND,
	STORAGE_DEV_SD,
	STORAGE_DEV_MS,
	STORAGE_DEV_USBH,
	STORAGE_DEV_XD,
	STORAGE_DEV_MAX
}STORAGE_DEV_ID_ENUM;

typedef enum
{
	MEDIA_AUDIO,
	MEDIA_PHOTO_AVI,
	MEDIA_VIDEO_ONLY
}MEDIA_TYPE_ENUM;

typedef enum
{
	MEDIA_PHOTO_CHK, /* photo only */
	MEDIA_PHOTO_AUDIO_CHK, /* photo or audio */
	MEDIA_PHOTO_AUDIO_VIDEO_CHK, /* photo or audio or video */
	MEDIA_VIDEO_CHK /* video only */
} MEDIA_TYPE_CHECK_ENUM;

typedef enum
{
	SCAN_FILE_COMPLETE,
	SCAN_FILE_NOT_COMPLETE,
	SCAN_FILE_COMPLETE_NO_PHOTO
} SCAN_FILE_STATUS_ENUM;

typedef enum
{
	STORAGE_PLGU_OUT,
	STORAGE_PLGU_IN
}STORAGE_PLUG_STATUS_ENUM;

typedef struct 
{
	INT8U storage_id;
	INT8U plug_status;
} PLUG_STATUS_ST;

#define SOURCE_AD_KEY 0
#define SOURCE_IR_KEY 1
#define SOURCE_IT_KEY 2

typedef struct 
{
	INT8U key_source;
	INT8U key_index;
	INT8U key_type;
} KEY_EVENT_ST;


typedef struct 
{
	INT16S fd;
	INT8U  f_name[256];
	INT8U  f_extname[4];
	INT32U f_size;
	INT16U f_time;
	INT16U f_date;
}STORAGE_FINFO;

typedef enum {
	IT_KEY_SINGLE_TAP = 0,
	IT_KEY_SLIDE,
	IT_KEY_RELEASE
}ITOUCH_KEY_ENUM;

typedef struct
{
  INT8U time_enable;
  INT8U time_hour;
  INT8U time_minute;
}ALARM_FORMAT;

#define ALARM_NUM 2

typedef struct
{
  ALARM_FORMAT power_on_time;
  ALARM_FORMAT power_off_time;
  ALARM_FORMAT alarm_time[ALARM_NUM];
}ALARM_INFO;

typedef struct
{
  INT8U alarm_onoff;
  ALARM_FORMAT alarm_time[7];
}ALARM_PARAMETER;


typedef struct{
	INT8U b_alarm_flag;
	INT8U B_alarm_hour;
	INT8U B_alarm_minute;
	INT8U Alarm_Music_idx[3];
} ALARM_STRUCT,*pALARM_STRUCT;

// turnkey setting in NVRAM
typedef struct{
INT8U mode;
INT8U on_hour;
INT8U on_minute;
INT8U off_hour;
INT8U off_minute;
} POWERONOFF_STRUCT,*pPOWERONOFF_STRUCT;

typedef struct
{
  INT32U Frequency;

}FM_STATION_FORMAT;

#define FM_STATION_NUM	20

typedef struct
{
  FM_STATION_FORMAT 	FM_Station[FM_STATION_NUM];
}FM_INFO;

extern INT16U ALARM_Trigger_By_GY0200;
extern INT32U Alarm_Trriger_By_GPL32;
extern INT32U Day_Trriger_By_GPL32;
extern INT32U Day_Trigger_By_GY0200;
typedef struct {
                 INT8U  language;
                 INT8U  date_format_display;
                 INT8U  time_format_display;
                 INT8U  week_format_display;
                 INT8U  lcd_backlight;
                 INT8U  sound_volume;
                 INT8U  current_storage;
                 INT8U  current_photo_index[3];
                 INT8U  slideshow_duration;
                 INT8U  slideshow_bg_music[3];
                 INT8U  slideshow_transition;
                 INT8U  slideshow_photo_date_on;
                 INT8U  calendar_duration;
                 INT8U  calendar_displaymode;
                 INT8U  thumbnail_mode;
                 INT8U  music_play_mode;
                 INT8U  music_on_off;
                 INT8U  midi_exist_mode;
                 INT8U  factory_date[3];                //yy,mm,dd
                 INT8U  factory_time[3];                //hh:mm:ss
                 ALARM_STRUCT	nv_alarm_struct1;
                 ALARM_STRUCT	nv_alarm_struct2;
                 INT8U	alarm_modual;
                 INT8U	full_screen;
                 INT8U	sleep_time;
                 POWERONOFF_STRUCT powertime_onoff_struct1;
                 POWERONOFF_STRUCT powertime_onoff_struct2;
                 INT8U powertime_modual;
				 INT8S powertime_active_id;
				 INT8U powertime_happened;
                 INT32U copy_file_counter;
                 INT32U Pre_Pow_off_state;
                 FM_INFO 	FM_struct;
		   		 INT8U	save_as_logo_flag;
		   		 INT8S	alarm_volume1;
		   		 INT8S	alarm_volume2;
		   		 INT8U	alarm_mute1;
		   		 INT8U	alarm_mute2;
		         INT8U	ui_style;
		         INT32U base_day;
                 INT8U  ifdirty;

} USER_ITEMS;

typedef struct {
    USER_ITEMS item ;
	INT8U crc[4];
	INT8U  DUMMY_BYTE[512-sizeof(USER_ITEMS)-4];
} SYSTEM_USER_OPTION, *pSYSTEM_USER_OPTION;


#ifndef _STORAGE_STATUS
#define _STORAGE_STATUS
typedef struct
{
	INT8U   card_insert;
	INT8U   audio_present;
	INT8U   photo_present;
#if FS_SCAN_STYLE == FS_SCAN_SEPARATION
	INT8U   video_present;
#endif
	INT8U   mount_status;
}STORAGE_STATUS;
#endif //_STORAGE_STATUS


#define NO_STORAGE 0xFF

extern STORAGE_STATUS  storage_status[STORAGE_DEV_MAX];
extern MSG_Q_ID SysTaskQ;
extern INT8U curr_storage_id;
extern INT8U newest_storage;
extern OS_EVENT    *SemScanFile;
typedef void (*func_ptr)(void);

//extern INT8U card_change;
extern void sys_task_entry(void * p_arg);
extern INT32S storage_scan_file(INT8U storage_id);
extern INT32S storage_file_nums_get(INT8U storage_id, INT8U file_type);
extern INT32S storage_scan_status_get(INT8U storage_id, INT8U file_type, INT8U *status);
extern INT32S storage_fopen(INT8U storage_id, INT8U file_type, INT32U file_idx, STORAGE_FINFO *storage_finfo);
extern INT32S storage_finfo_get(INT8U storage_id, INT8U file_type, INT32U file_idx, STORAGE_FINFO *storage_finfo);
extern INT32S storage_fcopy(INT8U src_storage_id, INT8U file_type, INT32U file_idx, INT8U dest_storage_id, INT8S *dest_name);
extern INT32S sys_save(INT32U *p_buffer, INT32U size, INT8S *ext_name, INT8U dest_storage_id, INT8S *dest_name);
//extern INT32S storage_fdelete(INT8U storage_id, INT8U file_type, INT32U file_idx);
extern INT32S storage_get_fpos(INT8U storage_id, INT8U file_type, INT32U file_idx, f_pos *fpos);
extern INT32S storage_startup_curr_storage_get(INT8U *storage_id,INT8U media_type);
extern INT32S storage_card_change_storage_get(INT8U *storage_id,INT8U media_type);
extern void storage_existence_list_get(INT8U *storage_list,INT8U *storage_nums);
extern void storage_photo_existence_list_get(INT8U *storage_list,INT8U *storage_nums,INT8U media_type);
extern INT8U storage_existence_check(INT8U storage_id);
extern void storage_device_unmount(INT8U new_storage_id);
extern void storage_rescan_device(INT8U dev);
extern void storage_renumeration(void);

extern BOOLEAN storage_existence_check(INT8U storage_id);
extern BOOLEAN storage_media_in_storages_check(INT8U media_type);
extern BOOLEAN storage_audio_in_storage_check(INT8U storage_id);
extern BOOLEAN storage_photo_check(INT8U storage_id);
extern BOOLEAN storage_media_check_by_id(INT8U storage_id,INT8U media_type);
#if FS_SCAN_STYLE == FS_SCAN_SEPARATION
extern BOOLEAN storage_video_check(INT8U storage_id);
#endif
extern void system_rbkey_lock(INT8U rbkey_code);
extern void system_rbkey_unlock(INT8U rbkey_code);
extern void system_irkey_lock(void);
extern void system_irkey_unlock(void);
extern INT32S irkey_message_init(void);
extern INT32S irkey_message_set(INT8U key_code, INT32U msg_id);
extern INT32S rbkey_message_set(INT8U key_index, INT32U message);
extern INT8U storage_lastin_storage_get(void);
extern STORAGE_DEV_ID_ENUM current_working_device_get(void);
extern void sys_storage_status_clear(void);
extern INT32S storage_card_judge(INT8U *storage_id,INT8U media_type);

extern INT8U sys_sd_upgrade_file_flag_get(void);	// Return 0: Not checked yet, 1=file does not exist when startup, 2=file exists
extern INT8U sys_sd_auto_demo_file_flag_get(void);	// Return 0: Not checked yet, 1=file does not exist when startup, 2=file exists
extern INT8U sys_sd_tft_calibrate_get(void);
extern void sys_usbd_state_flag_set(INT8U in_or_out);

extern INT8U sys_card_change_get(void);
extern void sys_card_change_reset(void);
extern void sys_card_change_set(void);
//TFT calibrate mode
extern INT32S usb_test_task_create(INT8U prio);
// battery module
extern INT8U sys_adapter_plug_status_get(void);
extern INT8U sys_battery_lvl_get(void);

//remindar
extern void 	sw_alarm_isr(void);
extern INT32S 	ap_alarm_set(void);
extern void ap_alarm_adjust(void);
extern void recalculate_alarm_day(void);
extern INT32S 	ap_alarm_inti(void);
extern INT32S	ap_alarm_message_dispose(void);

#if((defined _POWERTIME_SETTING)&&(_POWERTIME_SETTING == CUSTOM_ON))
extern INT8S ap_alarm_powertime_set(void);
extern void ap_alarm_powertime_set_rtc(void);
extern INT8S ap_alarm_set_enable_check(void);
extern INT8S ap_alarm_powertime_happen_check(void);
extern POWERONOFF_STRUCT umi_powertime_struct_get(INT8U powertime_id);
extern void umi_powertime_active_mode_set(INT8U mode);
#endif

/* Turn Key UMI Task */
extern INT32U rbkey_message_id_get(INT8U key_num);

#ifndef _TK_UMI_DEFINE_
#define _TK_UMI_DEFINE_
#define AP_QUEUE_MSG_MAX_LEN    60
#endif //_TK_UMI_DEFINE_

typedef struct
{
	INT32U type;
} STException;

extern MSG_Q_ID    UmiTaskQ;
extern void umi_task_entry(void * p_arg);
extern INT8U  ApQ_para[AP_QUEUE_MSG_MAX_LEN];
extern struct STFileNodeInfo PhotoFNodeInfo;
extern STORAGE_FINFO photo_finfo;

extern void ppu_resource_release_table_init(void);
extern void umi_user_config_store(void);
extern void umi_scaler_color_matrix_load(void);

#define SPU_LEFT_CH             0
#define SPU_RIGHT_CH            1

extern OS_EVENT	*hAudioDacTaskQ;
extern OS_EVENT	*aud_send_q;
extern OS_EVENT *aud_bg_send_q;
extern OS_EVENT *aud_right_q;
extern INT32U   last_send_idx;

extern MSG_Q_ID ApQ;
extern MSG_Q_ID	Audio_FG_status_Q, Audio_BG_status_Q, MIDI_status_Q;
extern MSG_Q_ID	audio_status_q;

/*==Turnkey AVI state ==*/
typedef struct
{
	INT16S	vedhandle;
	INT16S	audhandle;
	INT16S	status;
}MJPEG_HEADER_PARSE_STRUCT;

// added by Bruce, 2009/02/19
#if 1//def _DPF_PROJECT
#define C_AVI_MOV_DEFAULT		0x0000
#define C_AVI_MODE_PLAY			0x0001
#define C_MOV_MODE_PLAY			0x0002
#endif
// added by Bruce, 2009/02/19

#define AVI_TIMER 			TIMER_C
extern STAVIBufConfig		*avi_decode_buf;
extern STAVIBufConfig		*avi_display_buf;
extern STAVIAUDIOBufConfig  *avi_audio_buf_wi;
extern STAVIAUDIOBufConfig  *avi_audio_buf_ri;
extern OS_EVENT				*audio_fs_result_q;
extern OS_EVENT    			*aud_avi_q;
extern FP64 				g_avi_synchroframe; 
extern FP64 				g_avi_synchrocnt;
extern INT32S 				g_avi_datalength;
extern OS_EVENT				*avi_status_q;	
extern OS_EVENT 			*image_task_fs_queue_a;
extern OS_EVENT 			*image_task_fs_queue_b;

extern INT16S tk_avi_audiofilehandle_get(void);
extern INT32S tk_avi_get_audio_data(INT32U framenum);
extern INT32S tk_avi_read_audio_data(CHAR *buf,INT32U len);
extern INT32U tk_avi_decode_file_name(CHAR* pstring);
extern INT32U tk_avi_decode_file_id(INT32U FileNum);
extern INT32U tk_avi_get_audio_samplerate(void);
extern INT32S tk_avi_get_width(void);
extern INT32S tk_avi_get_height(void);
extern INT32S tk_avi_find_vdpos(INT32U JumpFrameNum);
extern INT32U tk_avi_close_file_id(INT32U FileNum);
extern INT32S tk_avi_get_hassound(void);
extern INT32S tk_avi_get_scale(void);
extern INT32S tk_avi_get_rate(void);
extern INT8U* tk_avi_get_waveformatex(void);
extern FP64   tk_avi_get_fps(void);
extern INT16U tk_avi_wave_bitformat_get(void);
extern INT16U tk_avi_wave_channelnum_get(void);
extern INT16U tk_avi_wave_format_type_get(void);
extern INT32S tk_avi_vediodata_find(INT32U SilpFrameNum);
extern INT32S avi_memory_free(void);
extern INT32S audio_avi_read(INT32U buf_addr, INT32U len);
extern void   avi_ppu_set(STAVIBufConfig* bufconfig);
extern INT32S avi_settime_frame_pos_get(INT32S time);
extern void   avi_idx1_tabel_free(void);
extern INT32S avi_idx1_totalframe_get(void);
#if 1//def _DPF_PROJECT
extern INT16U avi_mov_switchflag;
extern INT32S tk_mov_decode_file_id(INT32U FileNum);
extern INT32S tk_mov_get_audio_samplerate(void);
extern INT32S tk_mov_find_adpos(INT32U framenum);
extern INT32S tk_mov_find_vdpos(INT32U JumpFrameNum);
extern INT32S tk_mov_get_scale(void);
extern INT32S tk_mov_get_duration(void);
extern INT32S tk_mov_get_frame_number(void);
extern INT32S tk_mov_get_hassound(void);
extern INT32S tk_mov_get_width(void);
extern INT32S tk_mov_get_height(void);
extern INT32S tk_mov_wave_bitformat_get(void);
extern INT32S tk_mov_wave_channelnum_get(void);
extern INT32U tk_mov_close_file_id(void);
extern INT32S tk_mov_vediodata_find(INT32U SilpFrameNum);
extern INT32S tk_mov_read_audio_data(CHAR *buf, INT32U len);
extern INT32S mov_parsing(INT16U fin,INT16U fin2);
extern void   tk_mov_release_memory(void);
extern FP64   tk_mov_get_fps(void);
extern INT8U *tk_mov_get_waveformatex(void);
#endif
/*== apq manager ==*/
typedef enum {
	RB_KEY_1,
	RB_KEY_2,
	RB_KEY_3,
	RB_KEY_4,
	RB_KEY_5,
	RB_KEY_6,
	RB_KEY_7,
	MAX_RB_KEYS
}RB_KEY_ENUM;

typedef enum {
	IR_KEY_POWER,
	IR_KEY_UNDEFIEND_1,
	IR_KEY_UNDEFIEND_2,
	IR_KEY_SOUND_ON_OFF,
	IR_KEY_THUMBNAIL_SHOW,
	IR_KEY_ROTATE,
	IR_KEY_SLIDE_SHOW,
	IR_KEY_CALENDAR,
	IR_KEY_SLIDESHOW_INTERVAL,
	IR_KEY_ZOOM_CYCLE,
	IR_KEY_UNDEFIEND_6,
	IR_KEY_SLIDESHOW_SELECT_MUSIC,
	IR_KEY_MENU,
	IR_KEY_UP,
	IR_KEY_UNDEFIEND_8,
	IR_KEY_UNDEFIEND_9,
	IR_KEY_LEFT,
	IR_KEY_OK,
	IR_KEY_RIGHT,
	IR_KEY_UNDEFIEND_10,
	IR_KEY_UNDEFIEND_11,
	IR_KEY_DOWN,
	IR_KEY_VOL_UP,
	IR_KEY_VOL_DOWN,
	IR_KEY_PLAY_RESUME,
	IR_KEY_PAUSE,
	IR_KEY_STOP,
	IR_KEY_BACK,
	IR_KEY_UNDEFIEND_12,
	IR_KEY_UNDEFIEND_13,
	IR_KEY_UNDEFIEND_14,
	IR_KEY_UNDEFIEND_15,
	MAX_IR_KEYS
}IR_KEY_ENUM;

typedef struct 
{
    INT32U result_type;
    INT32S result;
} ST_APQ_HANDLE;

extern void audio_confirm_handler(STAudioConfirm *aud_con);
//extern EXIT_FLAG_ENUM apq_sys_msg_handler(INT32U msg_id);

// message q id define
extern MSG_Q_ID	fs_msg_q_id;
extern MSG_Q_ID	fs_scan_msg_q_id;
extern MSG_Q_ID	ap_msg_q_id;

//-------------------------------------------------------------------------------------------------
//ap_graphic
//extern  char *dpf_str_array[LCD_MAX][STR_MAX];
typedef struct
{
	 INT16U start_x;
	 INT16U start_y;
	 INT16U frame_width;//Only "1","2"width can use,"0" mean no outline
	 INT16U front_color;
	 INT16U frame_color;
}GraphicFrameChar;

extern INT32U cl1_graphic_init(GraphicBitmap *bitmap);
extern void cl1_graphic_finish(GraphicBitmap *bitmap);
extern void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_width_to_char_num(char *strings,INT32U total_width,INT8U language,INT8U font_type);
extern void cl1_graphic_char_draw(GraphicBitmap *bitmap,unsigned short character,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern void cl1_graphic_line_draw(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void cl1_graphic_rectangle_draw(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);
extern INT32U cl1_graphic_font_get_width(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_get_height(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_line_bytes_get(INT16U character_code,INT8U language,INT8U font_type);
extern void cl1_graphic_frame_string_draw(GraphicBitmap *bitmap,char *strings,INT8U language,INT8U font_type,GraphicFrameChar *frame);
extern void cl1_graphic_frame_char_draw(GraphicBitmap *bitmap,	INT16U character,INT8U language,INT8U font_type,GraphicFrameChar *frame);

extern void   cl1_graphic_music_string_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates);

extern void cl1_graphic_bitmap_front_color_set(GraphicBitmap *bitmap,INT16U color);
extern void cl1_graphic_bitmap_background_color_set(GraphicBitmap *bitmap,INT16U color);

// current storage
extern void umi_current_storage_set(INT8U current_storage);
extern INT8U umi_current_storage_get(void);

// added by Bruce, 2008/10/31
extern INT32S SPU_read_a_sector_from_SDRAM(INT16S fd, INT32U data_offset_addr, INT32U buffer_addr);
extern INT32S	SPU_load_tonecolor_from_SDRAM(INT16S fd_idi, INT32S MIDI_Index);

extern INT32S resource_audio_load(INT32U off_sector,  INT32U *buffer, INT32U sector_cnt);
#if 1 //For frame window.
typedef enum{
	TUNE_RESOURE_CREATE = 0,
	TUNE_RESOURE_RELEASE
}TUNE_RESOURE;

#define TUNE_HAVE_WINDWOS   0x01 << 0
#define TUNE_HAVE_HINT      0x01 << 1
#define TUNE_HAVE_CONTROL   0x01 << 2
#define TUNE_HAVE_SCOLL_BAR 0x01 << 4

#define TUNE_CREATE_SUCCESS 1
#define TUNE_CREATE_FAIL -1
/*__________Frame Attribute_____________*/
struct TuneFrameAttribute{
	INT8U depth;
	INT8U blending;
	INT16U width;
	INT16U height;
	INT16S start_x;
	INT16S start_y;
	INT16U bg_color;
	INT16U frame_width;
	INT16U frame_color;
	SPRITE frame_sprite;
	INT8S (*fram_bg_create)(struct TuneFrameAttribute *);
	INT8S (*frame_create)(struct TuneFrameAttribute *);
	void  (*frame_add)(struct TuneFrameAttribute *);
	void  (*frame_position_set)(INT16S,INT16S,struct TuneFrameAttribute*);
	void  (*frame_remove)(struct TuneFrameAttribute *);
	void  (*frame_destroy)(struct TuneFrameAttribute *);
};



/*__________Title Attribute_____________*/

#define TUNE_MSG_CENTER 0x01 << 0
#define TUNE_MSG_RIGHT  0x01 << 1
#define TUNE_MSG_LEFT   0x01 << 2

struct TuneTitleAttribute
{
	INT8U  title_depth;
	INT8U  title_blending;

	INT16S title_start_x;     //base x0
 	INT16S title_start_y;     //base y0

	INT16S title_rel_x;       //rel to x0 ..
	INT16S title_rel_y;       //rel to y0 ..


	INT16U title_width;       //X width can not over width of window.
	INT16U title_height;      //Y width can not over width of window.

	INT8U  title_msg_mode ;
	INT8U  title_bg_mode ;
	INT8U  title_font_size;   //Title text font size.
	INT16U  title_msg_color;  //Title text font color.
	char * title_msg_content; //The chars show in top position.

	SPRITE title_bg_sprite;
	SPRITE title_msg_sprite;

	INT8S (*title_create)(struct TuneTitleAttribute *,char* );
	INT8S (*title_bg_load)(struct TuneTitleAttribute *);
	void  (*title_position_set)(INT16S,INT16S,struct TuneTitleAttribute *);
	void  (*title_add)(struct TuneTitleAttribute *);
	void  (*title_remove)(struct TuneTitleAttribute *);
	void  (*title_destroy)(struct TuneTitleAttribute *);
};

/*__________ICON Attribute_____________*/
struct TuneIconAttribute{
	INT16S icon_start_x;
	INT16S icon_start_y;
	SPRITE icon_sprite;
	INT8S (*icon_create)(struct TuneIconAttribute *);
	void  (*icon_add)(struct TuneIconAttribute *);
	void  (*icon_remove)(struct TuneIconAttribute *);
	void  (*icon_destroy)(struct TuneIconAttribute *);
};

/*__________Text Attribute_____________*/
struct TuneTextAttribute{
	INT8U text_depth;
	INT16S text_start_x;
	INT16S text_start_y;
	INT16U text_front_color;
	INT16U text_bg_color;
	INT16U text_font_size;
	INT8U  text_font_outline;
	INT16U text_outline_color;

	SPRITE text_sprite;
	INT8S (*text_create)(struct TuneTextAttribute *,char *);
	void  (*text_position_set)(INT16S,INT16S,INT16S,INT16S,struct TuneTextAttribute *);
	void  (*text_add)(struct TuneTextAttribute *);
	void  (*text_remove)(struct TuneTextAttribute *);
	void  (*text_destroy)(struct TuneTextAttribute *);
};
/*__________Process Bar Attribute_____________*/
struct TuneProcessBarAttribute{
	INT8U depth;
	INT16S start_x;
	INT16S start_y;

	INT16S rel_x;
	INT16S rel_y;


	INT16U bg_color;
	INT16U bg_width;
	INT16U bg_height;

	INT16U rel_gap;

	INT16U current_level;
	INT16U total_level;

	SPRITE bg_sprite;
	SPRITE total_level_sprite;
	SPRITE current_level_sprite;
	INT8S (*process_bar_bg_create)(struct TuneProcessBarAttribute *);
	INT8S (*process_bar_img_create)(struct TuneProcessBarAttribute *);
	void  (*process_bar_add)(struct TuneProcessBarAttribute *);
	void  (*process_bar_position_set)(INT16S,INT16S,INT16S,INT16S,struct TuneProcessBarAttribute *);
	void  (*process_bar_update)(INT16U,struct TuneProcessBarAttribute *);
	void  (*process_bar_remove)(struct TuneProcessBarAttribute *);
	void  (*process_bar_destroy)(struct TuneProcessBarAttribute *);

};

/*__________Window Attribute_____________*/
struct TuneWindowAttribute{
	struct TuneFrameAttribute *window_frame;
	struct TuneTitleAttribute *window_title;
	void *window_text;
	void *window_picture;

	//void  (*window_create)(struct TuneProcessBarAttribute *);
	//void  (*window_add_to_link)(void *,struct TuneProcessBarAttribute *);
	//void  (*window_update)(INT16U,struct TuneProcessBarAttribute *);

	void  (*window_position_set)(INT16S,INT16S,struct TuneWindowAttribute *);
	void  (*window_add)(struct TuneWindowAttribute *);
	void  (*window_remove)(struct TuneWindowAttribute *);
	void  (*window_destroy)(struct TuneWindowAttribute *);

};

extern struct TuneFrameAttribute* cl2_tune_frame_create(
							INT8U depth,
							INT8U blending,
							INT16S start_x,
							INT16S start_y,
							INT16U width,
							INT16U height,
							INT16U frame_width,
							INT16U frame_color,
							INT8S (*fram_bg_create)(struct TuneFrameAttribute *)
							);

extern struct TuneTextAttribute *cl2_tune_text_create(
							INT8U text_depth,
							INT16S text_start_x,
							INT16S text_start_y,
							INT16U text_front_color,
							INT16U text_bg_color,
							INT16U text_font_size,
							INT8U  text_font_outline,
							INT16U text_outline_color,
							char* text);

extern struct TuneTitleAttribute* cl2_tune_title_create(
							INT8U title_depth,
							INT8U title_blending,
	                        INT16S title_start_x,
							INT16S title_start_y,

							INT16S title_rel_x,
							INT16S title_rel_y,

							INT16U title_width,
							INT16U title_height,
	                        INT8U title_font_size,
	                        INT8U title_msg_mode,
	                        INT16U title_msg_color,
	                        char *title_msg_text,
							INT8S (*title_bg_load)(struct TuneTitleAttribute*));

extern struct TuneProcessBarAttribute *cl2_tune_process_bar_create(
    							 INT8U depth,
    							 INT16S start_x,
    				             INT16S start_y,
    				             INT16S rel_x,
    				             INT16S rel_y,
    				             INT16U bg_color,
    				             INT16U bg_width,
    				             INT16U bg_height,
    				             INT16U rel_gap,
    							 INT16U current_level,
    							 INT16U total_level,
    							 INT8S (*process_bar_img_create)(struct TuneProcessBarAttribute *));
extern struct TuneIconAttribute *cl2_tune_icon_create(
						INT16S icon_start_x,
						INT16S icon_start_y,
						INT8S (*icon_load)(struct TuneIconAttribute *));

extern struct TuneWindowAttribute *cl2_window_create(
	INT8U depth,
	INT8U blending,
	INT16S start_x,
	INT16S start_y,
	INT16U width,
	INT16U height,
	INT16U frame_width,
	INT16U frame_color,
	INT8U title_font_size,
	INT16U title_text_color,
	char *title_name,
	INT8S (*title_bg_load)(struct TuneTitleAttribute*),
	INT8S (*frame_bg_load)(struct TuneFrameAttribute*)
	);
#endif

#if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC == 1)
extern void drv_l2_external_rtc_init(void);
//==================================//
//function: drv_l2_rtc_first_power_on_check
//return: 1 means i2c first power on,
//==================================//
extern INT32S drv_l2_rtc_first_power_on_check(void);
extern INT32S drv_l2_rtc_total_set(EXTERNAL_RTC_TIME *ptr);
extern INT32S drv_l2_rtc_total_get(EXTERNAL_RTC_TIME *ptr);
#if DRV_L2_EXT_ALAEM_EN
extern INT32S drv_l2_rtc_alarm_time_set(EXTERNAL_RTC_ALRAM *ptr);
extern INT32S drv_l2_rtc_alarm_time_get(EXTERNAL_RTC_ALRAM *ptr);
#endif
#endif
//===============PPU OS LOCK==============//
extern void PPU_OS_LCOK (void);
extern void PPU_OS_UNLCOK (void);

extern void _int_to_string(INT32S v, INT8U len, CHAR* p_string);
extern void int_to_string(INT32S v, CHAR* p_string);
extern void _int_to_string_no_end(INT32S v, INT8U len, CHAR* p_string);
extern void file_size_to_string(INT32S file_size, CHAR *p_string);

extern void umi_file_copy_counter_set(INT32U i);
extern INT32U umi_file_copy_counter_get(void);


//SAVE AS LOGO
#define LOGO_FILE_HEAD_SIZE  16
#define C_C_SAVE_AS_LOGO_FILE_TAG     "GPLOGO"
#define C_SAVE_AS_LOGO_TAG1	2
#define C_SAVE_AS_LOGO_TAG2	3
#define C_SAVE_AS_LOGO_FILE_SIZE_MAX      (128*1024)
extern INT32U g_save_as_logo_key_flag ;

extern INT8U ap_save_as_logo_flag_get(void);
extern INT32U nvmemory_user_sector_write(INT16U itag ,INT32U *pbuf , INT16U secter_count);
extern INT32U nvmemory_user_sector_read(INT16U itag ,INT32U *pbuf , INT16U secter_count);

// MutilMedia Codec //////////////////////////////////////////////////////////////////////////////////////////

/* define task pority use  */
#define DAC_PRIORITY			0
#define MIDI_DEC_PRIORITY		2
#define AUD_DEC_PRIORITY		4
#define AUD_DEC_BG_PRIORITY		6
#define FS_PRIORITY				8
#define IMAGE_PRIORITY			10
#define AVI_DEC_PRIORITY  		12

//usb web cam 
#define USB_AUD_ENC_PRIORITY	11
#define USB_AUDIO_PRIORITY      13
#define USB_CAM_PRIORITY        15 
#define USB_CAM_GPL325XX        19 

//video codec
#define JPEG_ENC_PRIORITY		14
#define AUD_ENC_PRIORITY		16
#define SCALER_PRIORITY			18
#define AVI_ENC_PRIORITY		20
#define AVI_PACKER0_PRIORITY	21
#define AVI_PACKER1_PRIORITY	22
#define AVI_PACKER_PRIORITY		23

#define VIDEO_PASER_PRIORITY	16
#define AUDIO_DECODE_PRIORITY   18
#define VIDEO_DECODE_PRIORITY	20
#define VID_DEC_STATE_PRIORITY	22

//========================= Media Format Defintion ============================
// including Audio, Image, Video
//=============================================================================
typedef enum
{
		AUD_AUTOFORMAT=0,
		MIDI,
		WMA,
		MP3,
		WAV,
		A1800,
		S880,
		A6400,
		A1600, 
		IMA_ADPCM,
		MICROSOFT_ADPCM,
		AAC
} AUDIO_FORMAT;

typedef enum
{
		IMG_AUTOFORMAT=0,
		JPEG,
		JPEG_P,		// JPEG Progressive
		MJPEG_S,	// Single JPEG from M-JPEG video
		GIF,
		BMP
} IMAGE_FORMAT;

typedef enum
{
		VID_AUTOFORMAT=0,
		MJPEG,
		MPEG4
} VIDEO_FORMAT;

//====================== Media Information Defintion ==========================
// including Audio, Image, Video
//=============================================================================
typedef struct {
		AUDIO_FORMAT	Format;
		char*		SubFormat;
		
		INT32U		DataRate;				// unit: bit-per-second
		INT32U		SampleRate;				// unit: Hz
		INT8U		Channel;				// if 1, Mono, if 2 Stereo
		INT16U		Duration;				// unit: second
		INT16U		FrameSize;				// unit: sample per single frame

		INT32U		FileSize;				// unit: byte
		char		*FileDate;				// string pointer
		char		*FileName;				// file name		
} AUDIO_INFO;

typedef struct {
		IMAGE_FORMAT	Format;
		char*		SubFormat;
		
		INT32U		Width;					// unit: pixel
		INT32U		Height;					// unit: pixel
		INT8U		Color;					// unit: color number

		INT32U		FileSize;				// unit: byte
		char		*FileDate;				// string pointer
		char		*FileName;				// file name pointer

		INT8U		Orientation;			// Portrait or Landscape
		char		*PhotoDate;				// string pointer
		INT8U		ExposureTime;			// Exporsure Time
		INT8U		FNumber;				// F Number
		INT16U		ISOSpeed;				// ISO Speed Ratings		
} IMAGE_INFO;

typedef struct {
		AUDIO_FORMAT	AudFormat;
		char		AudSubFormat[6];
		
		INT32U		AudBitRate;				// unit: bit-per-second
		INT32U		AudSampleRate;			// unit: Hz
		INT8U		AudChannel;				// if 1, Mono, if 2 Stereo
		INT16U		AudFrameSize;			// unit: sample per single frame

		VIDEO_FORMAT	VidFormat;
		char		VidSubFormat[4];
		INT8U		VidFrameRate;			// unit: FPS
		INT32U		Width;					// unit: pixel
		INT32U		Height;					// unit: pixel

		INT32U		TotalDuration;			// unit: second
		//INT32U	FileSize;				// unit: byte
		//char		*FileDate;				// string pointer
		//char		*FileName;				// file name
} VIDEO_INFO;

//=============================================================================
//======================== Media Argument Defintion ===========================
// including Audio, Image, Video
//=============================================================================
typedef enum
{
		PPUAUTOFORMAT=0,
		BITMAP_YUYV,
		BITMAP_RGRB,
		CHARATER64_YUYV,
		CHARATER64_RGRB,
		CHARATER32_YUYV,
		CHARATER32_RGRB,
		CHARATER16_YUYV,
		CHARATER16_RGRB,
		CHARATER8_YUYV,
		CHARATER8_RGRB
} PPU_FORMAT;

typedef enum
{
		FIT_OUTPUT_SIZE=0,//Fit to output_buffer_width and output_buffer_height
		MAINTAIN_IMAGE_RATIO_1,//Maintain ratio and fit to output_buffer_width or output_buffer_height
		MAINTAIN_IMAGE_RATIO_2//Same as MAINTAIN_IMAGE_RATIO_1 but without scale up
}SCALER_OUTPUT_RATIO;

typedef enum
{
		SCALER_INPUT_FORMAT_RGB1555=0,
		SCALER_INPUT_FORMAT_RGB565,
		SCALER_INPUT_FORMAT_RGBG,
		SCALER_INPUT_FORMAT_GRGB,
		SCALER_INPUT_FORMAT_YUYV,
     	SCALER_INPUT_FORMAT_UYVY			
} IMAGE_INPUT_FORMAT;

typedef enum
{
		IMAGE_OUTPUT_FORMAT_RGB1555=0,
		IMAGE_OUTPUT_FORMAT_RGB565,
		IMAGE_OUTPUT_FORMAT_RGBG,
		IMAGE_OUTPUT_FORMAT_GRGB,
		IMAGE_OUTPUT_FORMAT_YUYV,
     	IMAGE_OUTPUT_FORMAT_UYVY,	
	    IMAGE_OUTPUT_FORMAT_YUYV8X32,		
        IMAGE_OUTPUT_FORMAT_YUYV8X64,		
        IMAGE_OUTPUT_FORMAT_YUYV16X32,		
        IMAGE_OUTPUT_FORMAT_YUYV16X64,
        IMAGE_OUTPUT_FORMAT_YUYV32X32,	
        IMAGE_OUTPUT_FORMAT_YUYV64X64,
        IMAGE_OUTPUT_FORMAT_YUV422,
        IMAGE_OUTPUT_FORMAT_YUV420,
        IMAGE_OUTPUT_FORMAT_YUV411,
        IMAGE_OUTPUT_FORMAT_YUV444,        
        IMAGE_OUTPUT_FORMAT_Y_ONLY        
} IMAGE_OUTPUT_FORMAT;

typedef struct {
		INT8U		Main_Channel;			//0: SPU Channels
											//1: DAC Channel A+B (can't assign MIDI)
											//   only 0 and 1 are available now
		INT8U		L_R_Channel;			//0: Invalid Setting				
											//1: Left Channel only
											//2: Right Channel only
											//3: Left + Right Channel
											//   only Left + Right Channel are avialable now
	    BOOLEAN   	mute;
	    INT8U     	volume;
	    INT8U		midi_index;				// MIDI index in *.idi

		INT8U		*data_start_addr;		// *data_start_addr is used only in one file mode
											// 1: SOURCE_TYPE_USER_DEFINE

	    INT32U		data_len;				// data_len is used only in two file mode
	    									// 1: SOURCE_TYPE_USER_DEFINE
	    									// 2: SOURCE_TYPE_FS_RESOURCE_IN_FILE
	    INT32U		data_offset;
} AUDIO_ARGUMENT;

typedef struct {
		INT8U       *OutputBufPtr;     
		INT16U		OutputBufWidth;
		INT16U		OutputBufHeight;
		INT16U		OutputWidth;
		INT16U		OutputHeight;	
		INT32U      OutBoundaryColor;
		SCALER_OUTPUT_RATIO ScalerOutputRatio;
		IMAGE_OUTPUT_FORMAT	OutputFormat;
} IMAGE_ARGUMENT;

typedef struct
{
		INT8U *yaddr;
		INT8U *uaddr;
		INT8U *vaddr;
} IMAGE_ENCODE_PTR;

typedef enum
{
		IMAGE_ENCODE_INPUT_FORMAT_YUYV=0,
        IMAGE_ENCODE_INPUT_FORMAT_YUV_SEPARATE,
        IMAGE_ENCODE_INPUT_FORMAT_YUYV_SCALE
} IMAGE_ENCODE_INPUT_FORMAT;

typedef enum
{
		IMAGE_ENCODE_OUTPUT_FORMAT_YUV422=0,
		IMAGE_ENCODE_OUTPUT_FORMAT_YUV420
} IMAGE_ENCODE_OUTPUT_FORMAT;

typedef enum
{
		IMAGE_ENCODE_ONCE_READ=0,
		IMAGE_ENCODE_BLOCK_READ,
		IMAGE_ENCODE_BLOCK_READ_WRITE
} IMAGE_ENCODE_MODE;

typedef struct {
        INT8U        *InputBufPtr;
        INT8U        *OutputBufPtr;
		INT16U		 InputWidth;
		INT16U		 InputHeight;
	    INT16U       inputvisiblewidth;
	    INT16U       inputvisibleheight;
	    INT32U       outputwidthfactor;			
	    INT32U       outputheightfactor;	
		INT16U		 OutputWidth;
		INT16U		 OutputHeight;
	    INT32U       inputoffsetx;
	    INT32U       inputoffsety;
		INT16U		 OutputBufWidth;
		INT16U		 OutputBufHeight;
		INT32U       OutBoundaryColor;
		IMAGE_INPUT_FORMAT	InputFormat;
		IMAGE_OUTPUT_FORMAT	OutputFormat;
} IMAGE_SCALE_ARGUMENT;

typedef struct {
        INT8U        *OutputBufPtr;
        INT8U        *quant_luminance_table;
        INT8U        *quant_chrominance_table;
        INT8U        BlockLength;             //BlockLength=16,32..
        INT8U        UseDisk;
		INT16U		 InputWidth;
		INT16U		 InputHeight;
		INT16S       FileHandle;
		INT32U       quantizationquality;    //quality=50,70,80,85,90,95,100       
		IMAGE_ENCODE_MODE EncodeMode;        // 0=read once, 1=block read
		IMAGE_ENCODE_PTR InputBufPtr;
		IMAGE_ENCODE_INPUT_FORMAT	InputFormat;
		IMAGE_ENCODE_OUTPUT_FORMAT	OutputFormat;
		IMAGE_SCALE_ARGUMENT        *scalerinfoptr;
} IMAGE_ENCODE_ARGUMENT;

typedef struct {
		INT8U		bScaler;
		INT8U		bUseDefBuf;			//video decode use user define buffer or not 
		INT8U		*AviDecodeBuf1;		//video decode user define buffer address 
		INT8U		*AviDecodeBuf2;		//video decode user define buffer address
		INT16U		TargetWidth;		//video encode use
		INT16U		TargetHeight;		//video encode use
		INT16U      SensorWidth;        //video encode use
		INT16U      SensorHeight;       //video encode use
		INT16U      DisplayWidth;       
		INT16U      DisplayHeight;      
		INT16U		DisplayBufferWidth;
		INT16U 		DisplayBufferHeight;
		INT32U      VidFrameRate;       //for avi encode only
		INT32U      AudSampleRate;      //for avi encode only
		IMAGE_OUTPUT_FORMAT	OutputFormat;
} VIDEO_ARGUMENT;

//=============================================================================
//======================== Media Status Defintion ============================
// including Audio, Image, Video
//=============================================================================
typedef enum
{
		START_OK=0,
		RESOURCE_NO_FOUND_ERROR,
		RESOURCE_READ_ERROR,
		RESOURCE_WRITE_ERROR,
		CHANNEL_ASSIGN_ERROR,
		REENTRY_ERROR,
		AUDIO_ALGORITHM_NO_FOUND_ERROR,
		CODEC_START_STATUS_ERROR_MAX
} CODEC_START_STATUS;

typedef enum
{
		AUDIO_CODEC_PROCESSING=0,					// Decoding or Encoding
		AUDIO_CODEC_PROCESS_END,					// Decoded or Encoded End
		AUDIO_CODEC_BREAK_OFF,						// Due to unexpended card-plug-in-out
		AUDIO_CODEC_PROCESS_PAUSED,
		AUDIO_CODEC_STATUS_MAX
} AUDIO_CODEC_STATUS;

typedef enum
{
		IMAGE_CODEC_DECODE_END=0,                // Decoded or Encoded End
		IMAGE_CODEC_DECODE_FAIL,
		IMAGE_CODEC_DECODING,					  // Decoding or Encoding				
		IMAGE_CODEC_BREAK_OFF,					// Due to unexpended card-plug-in-out
		IMAGE_CODEC_STATUS_MAX
} IMAGE_CODEC_STATUS;

typedef enum
{
		VIDEO_CODEC_PROCESSING=0,					// Decoding or Encoding
		VIDEO_CODEC_PROCESS_PAUSE,
		VIDEO_CODEC_PROCESS_END,					// Decoded or Encoded End
		VIDEO_CODEC_BREAK_OFF,					// Due to unexpended card-plug-in-out
		VIDEO_CODEC_RESOURCE_NO_FOUND,		
		VIDEO_CODEC_CHANNEL_ASSIGN_ERROR,
		VIDEO_CODEC_RESOURCE_READ_ERROR,
		VIDEO_CODEC_RESOURCE_WRITE_ERROR,
		VIDEO_CODEC_PASER_HEADER_FAIL,
		VIDEO_CODEC_STATUS_ERR,
		VIDEO_CODEC_STATUS_OK, 
		VIDEO_CODEC_STATUS_MAX
} VIDEO_CODEC_STATUS;

//=============================================================================
//========================= Media Source Defintion ============================
// including Audio, Image, Video
//=============================================================================
typedef enum
{
		SOURCE_TYPE_FS=0,
		SOURCE_TYPE_SDRAM,
		SOURCE_TYPE_NVRAM,
		SOURCE_TYPE_USER_DEFINE,
		SOURCE_TYPE_FS_RESOURCE_IN_FILE,	// added by Bruce, 2010/01/22
		SOURCE_TYPE_MAX
} SOURCE_TYPE;


typedef struct {
		SOURCE_TYPE type;					//0: GP FAT16/32 File System by File SYSTEM 
											//1: Directly from Memory Mapping (SDRAM)
											//2: Directly from Memory Mapping (NVRAM)
											//3: User Defined defined by call out function:audio_encoded_data_read								
		
		struct User							//Source File handle and memory address
		{
				INT16S		FileHandle;		//File Number by File System or user Define	
				INT32S      temp;			//Reserve for special use 
				INT8U       *memptr;		//Memory start address						
		}type_ID;
		
		union SourceFormat					//Source File Format
		{
				AUDIO_FORMAT	AudioFormat;		//if NULL,auto detect
				IMAGE_FORMAT	ImageFormat;		//if NULL,auto detect
				VIDEO_FORMAT	VideoFormat;		//if NULL,auto detect
		}Format;
} MEDIA_SOURCE;

//=============================================================================
typedef enum
{
		SND_OFFSET_TYPE_NONE = 0,
		SND_OFFSET_TYPE_HEAD,
		SND_OFFSET_TYPE_SIZE,
		SND_OFFSET_TYPE_TIME,
		SND_OFFSET_TYPE_MAX
} SndOffset_TYPE;

typedef struct {
		INT32U		DataRate;				// unit: bit-per-second
		INT32U		SampleRate;				// unit: Hz
		
		INT8U		Channel;				// if 1, Mono, if 2 Stereo
		INT8U		ConstPitchEn;			// constant pitch enable
		INT8U		EchoEn;					// echo efffect enable
		INT8U		VoiceChangerEn;			// voice changer enable 
		
		INT32U		Pitch_idx;				// constant pitch index 0 ~ 7
		INT32U 		weight_idx;				// echo weight, 0 ~ 2
		INT32U		delay_len;				// echo delay 				
		
		INT16U		Speed;					// voice changer speed, 0 ~ 24 
		INT16U		Pitch;					// voice changer pitch, 0 ~ 24

		INT32U		Offset;					// play offset
		SndOffset_TYPE Offsettype;
		
		AUDIO_FORMAT AudioFormat;

		SOURCE_TYPE Srctype;				//0: GP FAT16/32 File System by File SYSTEM 
											//1: Directly from Memory Mapping (SDRAM)
											//2: Directly from Memory Mapping (NVRAM)
											//3: User Defined defined by call out function:audio_decode_data_fetch								
} SND_INFO;

typedef struct {
		INT32U		Offset;					// play offset
		SndOffset_TYPE Offsettype;
		
		AUDIO_FORMAT AudioFormat;

} SACM_CTRL;
//=============================================================================
//========================= Platform Initialization ===========================
//
// Task Create : Audio, DAC, Image, File, AP(Demo Code) Task
// Global Variable Initialization
// Component Initialization : File system, Memory Manager,
// Driver Initialization: DMA Manager, SDC, USB Host
//=============================================================================
extern void platform_entrance(void* free_memory);
extern void platform_exit(void);

// Audio Decode ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern INT32U Audio_Decoder_Status_0;	// added by Bruce, 2008/11/20
extern INT32U Audio_Decoder_Status_1;	// added by Bruce, 2008/11/20
extern INT32U Audio_Decoder_Status_2;	// added by Bruce, 2008/11/20
extern INT32U g_delta_time_start;	// added by Bruce, 2008/11/14
extern INT32U g_delta_time_end;		// added by Bruce, 2008/11/14
extern void	audio_decode_end(INT32U audio_decoder_num);	// modified by Bruce, 2008/11/20
extern INT32S audio_encoded_data_read(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);	// added by Bruce, 2008/10/27
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
extern INT32U audio_decode_GetTotalTime(INT16S filehandle, AUDIO_FORMAT audioformat);
extern INT32U audio_decode_GetCurTime(INT16S filehandle, AUDIO_FORMAT audioformat);
extern AUDIO_CODEC_STATUS audio_decode_status(AUDIO_ARGUMENT arg);
extern void audio_decode_get_info(AUDIO_ARGUMENT arg, MEDIA_SOURCE src, AUDIO_INFO *audio_info);

// Audio Codec SACM Interface//////////////////////////////////////////////////////////////////////////////////////////////////////
extern INT16U GetSoundLibVersion(void);
extern void Snd_SetVolume(INT8U CodecType,INT8U vol );
extern INT8U Snd_GetVolume( INT8U CodecType);
extern INT16S Snd_Stop( INT8U CodecType );
extern INT16S Snd_Pause( INT8U CodecType );
extern INT16S Snd_Resume( INT8U CodecType );
extern INT16U Snd_GetStatus(INT8U CodecType);
//extern void Snd_Init( int *SACMstack, int SACMstacksize );
extern INT16S Snd_Init(void);
extern INT16S Snd_Play(SND_INFO *sndinfo);	//added by Dexter.Ming 081009
extern INT32S Snd_GetData(INT32U buf_adr,INT32U buf_size);
extern INT32U Snd_GetLen(void);	//read file length
extern SACM_CTRL G_SACM_Ctrl;
extern INT16S G_Snd_UserFd;
extern SND_INFO G_snd_info;
//extern INT32U GetSndTotalTime(INT16S	filehandle,AUDIO_FORMAT audioformat);
//extern INT32U GetSndCurTime(INT16S	filehandle,AUDIO_FORMAT audioformat);
extern INT32U Snd_GetTotalTime(INT16S filehandle, AUDIO_FORMAT audioformat);
extern INT32U Snd_GetCurTime(INT16S filehandle, AUDIO_FORMAT audioformat);

// voice changer /////////////////////////////////////////////////////////////////////////////////////////////
extern void *hVC;		//for voice change
extern void *hUpSample;	//for upsample
extern void *hConstPitch;
extern void *hEcho;

// Audio Encode ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void audio_encode_entrance(void);
void audio_encode_exit(void);
CODEC_START_STATUS audio_encode_start(MEDIA_SOURCE src, INT16U SampleRate, INT32U BitRate);
void audio_encode_stop(void);
AUDIO_CODEC_STATUS audio_encode_status(void);
CODEC_START_STATUS audio_encode_set_downsample(INT8U bEnable, INT8U DownSampleFactor);
INT32U audio_encode_data_write(INT8U bHeader, INT32U buffer_addr, INT32U cbLen);

// Audio Codec SACM Interface//////////////////////////////////////////////////////////////////////////////////////////////////////
extern INT16U GetSoundLibVersion(void);
extern void Snd_SetVolume(INT8U CodecType,INT8U vol );
extern INT8U Snd_GetVolume( INT8U CodecType);
extern INT16S Snd_Stop( INT8U CodecType );
extern INT16S Snd_Pause( INT8U CodecType );
extern INT16S Snd_Resume( INT8U CodecType );
extern INT16U Snd_GetStatus(INT8U CodecType);
extern INT16S Snd_Init(void);
extern INT16S Snd_Play(SND_INFO *sndinfo);	
extern INT32S Snd_GetData(INT32U buf_adr,INT32U buf_size);
extern INT32U Snd_GetLen(void);		//read file length
extern INT32S Snd_Lseek(INT32S offset,INT16S fromwhere);
extern SACM_CTRL G_SACM_Ctrl;
extern INT16S G_Snd_UserFd;

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
extern void image_decode_end(void);		//call-back

//Image Encode /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void image_encode_entrance(void);
extern void image_encode_exit(void);
extern INT32U image_encode_start(IMAGE_ENCODE_ARGUMENT arg);
extern void image_encode_stop(void);
extern IMAGE_CODEC_STATUS image_encode_status(void);
extern void image_encode_end(INT32U encode_size);									//call-back
extern CODEC_START_STATUS image_encoded_block_read(IMAGE_SCALE_ARGUMENT *scaler_info,IMAGE_ENCODE_PTR *encode_ptr);	//call-back
extern void user_defined_image_decode_entrance(INT32U TV_TFT);
extern void image_sensor_start(void);

// Video decode /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void video_decode_entrance(void);
extern void video_decode_exit(void);
extern void avi_audio_decode_rampup(void);
extern void avi_audio_decode_rampdown(void);
extern VIDEO_CODEC_STATUS video_decode_Info(VIDEO_INFO *info);
extern VIDEO_CODEC_STATUS video_decode_paser_header(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src);
extern CODEC_START_STATUS video_decode_start(VIDEO_ARGUMENT arg, MEDIA_SOURCE src);
extern VIDEO_CODEC_STATUS video_decode_stop(void);
extern VIDEO_CODEC_STATUS video_decode_pause(void);
extern VIDEO_CODEC_STATUS video_decode_resume(void);
extern void audio_decode_volume(INT8U volume);
extern VIDEO_CODEC_STATUS video_decode_status(void);
extern void video_decode_end(void);		
extern void video_decode_FrameReady(INT8U *FrameBufPtr);
extern VIDEO_CODEC_STATUS video_decode_set_play_time(INT32S SecTime);
extern INT32U video_decode_get_current_time(void);
extern VIDEO_CODEC_STATUS video_decode_set_play_speed(FP32 speed);
extern VIDEO_CODEC_STATUS video_decode_set_reserve_play(INT32U enable);
extern VIDEO_CODEC_STATUS video_decode_get_nth_video_frame(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src, INT32U nth_frame);
extern void video_decode_get_display_size(INT16U *pwidth, INT16U *pheight);

// Video encode /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void video_encode_entrance(void);
extern void video_encode_exit(void);
extern CODEC_START_STATUS video_encode_preview_start(VIDEO_ARGUMENT arg);
extern CODEC_START_STATUS video_encode_preview_stop(void);
extern CODEC_START_STATUS video_encode_webcam_start(VIDEO_ARGUMENT arg);
extern CODEC_START_STATUS video_encode_webcam_stop(void);
extern CODEC_START_STATUS video_encode_start(MEDIA_SOURCE src);
extern CODEC_START_STATUS video_encode_stop(void);
extern CODEC_START_STATUS video_encode_Info(VIDEO_INFO *info);
extern VIDEO_CODEC_STATUS video_encode_status(void);
extern CODEC_START_STATUS video_encode_auto_switch_csi_frame(void);
extern CODEC_START_STATUS video_encode_auto_switch_csi_fifo_end(void);
extern CODEC_START_STATUS video_encode_auto_switch_csi_fifo_frame_end(void);
extern CODEC_START_STATUS video_encode_set_zoom_scaler(FP32 zoom_ratio);
extern INT32U video_encode_sensor_start(INT16U width, INT16U height, INT32U csi_frame1, INT32U csi_frame2);
extern INT32U video_encode_sensor_stop(void);
extern INT32S video_encode_display_frame_ready(INT32U frame_buffer);
extern INT32S video_encode_frame_ready(void *workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag);
extern void video_encode_end(void *workmem);
extern CODEC_START_STATUS video_encode_pause(void);
extern CODEC_START_STATUS video_encode_resume(void);
extern CODEC_START_STATUS video_encode_set_jpeg_quality(INT8U quality_value);
extern CODEC_START_STATUS video_encode_capture_picture(MEDIA_SOURCE src);
extern CODEC_START_STATUS video_encode_fast_switch_stop_and_start(MEDIA_SOURCE src);
#if AUDIO_SFX_HANDLE
extern INT32U video_encode_audio_sfx(INT16U *PCM_Buf, INT32U cbLen);
#endif
#if VIDEO_SFX_HANDLE
extern INT32U video_encode_video_sfx(INT32U buf_addr, INT32U cbLen); 
#endif
extern void video_encode_get_display_size(INT16U *pwidth, INT16U *pheight);

// display ///////////////////////////////////////////////////////////////
extern void video_codec_show_image(INT32U TV_TFT, INT32U BUF,INT32U DISPLAY_MODE ,INT32U SHOW_TYPE);
extern void user_defined_video_codec_entrance(void);

// card detect /////////////////////////////////////////////////////////////////////////////////////////
extern void card_detect_set_cf_callback(void (*cf_callback)(void));
extern void card_detect_set_sdms_callback(void (*sdms_callback)(void));
extern void card_detect_set_usb_h_callback(void (*usb_h_callback)(void));
extern void card_detect_set_usb_d_callback(void (*usb_d_callback)(void));
extern INT8U card_detect_get_plug_status(void);
extern void card_detect_init(INT16U devicetpye);
extern void sdms_detection(void);
extern void cfc_detection(void);
extern INT16U card_detection(INT16U type);

// usb web cam /////////////////////////////////////////////////////////////////////////////////////////
extern INT32S usb_cam_entrance(INT16U usetask, INT16U jpeg_width, INT16U jpeg_height, INT16U aud_sample_rate);
extern void usb_cam_exit(void);
extern void usb_webcam_state(void);

// power down /////////////////////////////////////////////////////////
extern void PowerDown_Mode(INT8U mode);

#endif 		// __APPLICATION_H__

