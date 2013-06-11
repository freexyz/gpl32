#ifndef __STATE_SLIDE_SHOW_H__
#define __STATE_SLIDE_SHOW_H__
/* parents header include */
#include "turnkey_umi_task.h"

#define Avi_Buffer_TotalNum  0x2
#define Avi_Audio_Buffer_Num 0x4
// AVI Status Flow
#define Avi_Playing			 0x0
#define Avi_Prep			 0x1
#define Avi_AudioFileEnd	 0x2
#define Avi_VideoFileEnd	 0x4
#define Avi_PlayFinish		 0x6
#define Avi_Stop			 0x8

//AVI display mode status
#define AVI_FILE_NUM_USE	 		0xFE
#define AVI_FILE_NAME_USE	 		0x1
#define AVI_DEFAULT_SIZE_USE 		0xFD
#define AVI_USER_SIZE_USE			0x2
#define AVI_CENTRAL_POS_USE			0xFB
#define AVI_USER_POS_USE			0x4
#define AVI_FULL_SCREEN_SHOW_OFF	0x7F
#define AVI_FULL_SCREEN_SHOW_ON		0x8


typedef struct 
{
	INT32U				FileId;
	INT8U				*pstring;
	INT32U              AudioFlag;
	INT32U              Videofps;
	INT32U				RealtimeFrame;
	INT32U				CurrentTime;
	INT32U              CurrentFrame;
	INT32U 				TimeTickFrame;
	FP64				Videomodifyframe;
	FP64				videofloatframe;
	INT16S              AivStatus;
#ifndef _DPF_PROJECT	
	INT16S              MovFileHandle;	//Add by Roy
	INT16S				AudFileHandle;	//ADD by Roy
#endif	
	INT32U				avi_image_width;
	INT32U				avi_image_height;
	INT32U 				display_mode;
	INT32U				display_width;
	INT32U 				display_height;
	INT32S				display_position_x;
	INT32S				display_position_y;
	INT32S				avi_image_buf_malloc_flag;
	STAVIBufConfig		stAviBuf[Avi_Buffer_TotalNum];
	STAVIAUDIOBufConfig stAviAudioBuf[Avi_Audio_Buffer_Num];
} STAviShowControl;



extern STAviShowControl *pstAviShowControl;
extern STAVIBufConfig	*avi_decode_buf;
extern STAVIBufConfig	*avi_display_buf;

extern void   state_avi_entry(STATE_ENTRY_PARA* para1);
extern void   avi_ppu_set(STAVIBufConfig* bufconfig);
extern INT32S avi_frame_calculate(void);
extern INT32S avi_memory_free(void);
extern INT32S tk_avi_vediodata_find(INT32U SilpFrameNum);
extern void   AviSetFrameTime(INT32U freq_hz);
extern INT32S avi_play_init(void);
extern void   avi_play(void);
extern INT32U avi_current_time_get(void);
#ifdef _DPF_PROJECT
	extern INT32S avi_start(INT32S FileNum);
	extern INT32S umi_Apq_Filter(INT32U filter_msg_id);
#else
	extern INT32S avi_start(INT16S avi_movhandle, INT16S avi_audhandle);
#endif
extern INT32S avi_jumpframe_show(void);
extern INT32S avi_audio_buf_fill_again(void);

#ifdef _DPF_PROJECT
extern INT32S mov_start(INT32S FileNum);
#else
extern INT32S mov_start(INT16S mov_movhandle, INT16S mov_audhandle);
#endif
extern INT32S mov_play_init(void);
extern void   mov_play(void);
extern INT32S mov_memory_free(void);
extern INT32S tk_mov_vediodata_find(INT32U SilpFrameNum);

extern void AviPPUInitial(void);
extern void AviPPUUnInitial(void);

extern STAVIBufConfig		*avi_decode_buf;
extern STAVIBufConfig		*avi_display_buf;
extern STAVIAUDIOBufConfig *avi_audio_buf_wi;
extern STAVIAUDIOBufConfig *avi_audio_buf_ri;
extern INT32U 				*charram;
extern STAviShowControl	*pstAviShowControl;

extern INT32S avi_mp3_settime_frame_pos_get(INT32S time);
extern void Media_Task_Prio_Up(void);
extern void Media_Task_Prio_Down(void);
#endif  /*__STATE_SLIDE_SHOW_H__*/