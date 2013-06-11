//* Version : 1.00
//* History :   1 shin modify : add more 4 bytes for video buffer to avoid pre-alignment issue 2008/4/8
//              2 shin modify : a .add more 4 bytes for audio buffer to avoid pre-alignment issue 2008/4/17
//                              b .fixed bug decode buffer shit
//				                c .modify the file position align dispose. issue 2008/6/10

/*Include files */
#include "state_avi.h"

/* define */
#define AVI_NOT_SUPPORT	    0x0001
#define max_avi_index		2000
#define AUDIO_AVI_RINGBUF_LEN	4000


/* Variables defined in this file */
extern STAVIBufConfig		*avi_decode_buf;
extern STAVIBufConfig		*avi_display_buf;
extern STAVIAUDIOBufConfig *avi_audio_buf_wi;
extern STAVIAUDIOBufConfig *avi_audio_buf_ri;
#ifdef	_DPF_PROJECT
extern INT32U 				*charram;
#endif
extern STAviShowControl	*pstAviShowControl;
extern INT32U				g_avi_ignoreframe;

extern void AviPPUInitial(void);
extern void	AviSetFrameTime(INT32U freq_hz);

static void   MovVideoTimeSetting(void);
static void	  MovAudioPlay(void);
static INT32U MovAudioInitial(void);
static void   MovBufferInitial(void);
#ifdef	_DPF_PROJECT
INT32S mov_start(INT32S FileNum);
#else
INT32S mov_start(INT16S mov_movhandle, INT16S mov_audhandle);
#endif
INT32S mov_play_init(void);
void   mov_play(void);
INT32S mov_memory_free(void);
INT32S tk_mov_vediodata_find(INT32U SilpFrameNum);
#ifndef	_DPF_PROJECT
extern INT32U tk_mov_decode_file_parsing(INT16S mov_movhandle, INT16S mov_audhandle);
#endif
extern INT32U OutputFlag;
/*====  mov api start  ====*/
static void MovVideoTimeSetting(void)
{
	FP64 videofps;
	videofps = tk_mov_get_fps();
	pstAviShowControl->Videofps = (INT32U)videofps;
	pstAviShowControl->videofloatframe = videofps - pstAviShowControl->Videofps;
}

static void	MovAudioPlay(void)
{
	INT32S ret;
	ret = msgQSend(AudioTaskQ,MSG_AUD_AVI_PLAY,NULL,0,MSG_PRI_NORMAL);

}

static INT32U MovAudioInitial(void)
{
	INT32U  i;
	INT32U	SampleRate;
	INT32S  status;
	INT8U   err;
	INT16U 	wave_bit;
	INT16U  channel_num;
	FP64    temp;
		
	SampleRate = tk_mov_get_audio_samplerate();
	channel_num = tk_mov_wave_channelnum_get();

	g_avi_synchroframe = 0;
	temp = tk_mov_get_fps() / (FP64)SampleRate;
	if(channel_num == 2)
		temp = temp / 2;
	g_avi_synchrocnt = temp*AUDIO_AVI_RINGBUF_LEN;
	g_avi_datalength = AUDIO_AVI_RINGBUF_LEN;
	g_avi_ignoreframe = SampleRate%AUDIO_AVI_RINGBUF_LEN;
	if(g_avi_ignoreframe!=0)
	{
		temp = temp*g_avi_ignoreframe+1;
		g_avi_ignoreframe = (INT32U)temp;
	}
	else
		g_avi_ignoreframe = g_avi_synchrocnt + 1;

	wave_bit = tk_mov_wave_bitformat_get();
	if(wave_bit == 0x10)
		SampleRate = SampleRate*2;
	if(channel_num == 2)
		SampleRate = SampleRate*2;
	avi_audio_buf_wi = &pstAviShowControl->stAviAudioBuf[0];
	avi_audio_buf_ri = avi_audio_buf_wi;

	for(i=0;i<Avi_Audio_Buffer_Num;i++)
	{
		pstAviShowControl->stAviAudioBuf[i].status = AUDIO_BUF_FIRST_FILL;
		pstAviShowControl->stAviAudioBuf[i].ri = 0;
		pstAviShowControl->stAviAudioBuf[i].wi = 0;
		pstAviShowControl->stAviAudioBuf[i].length = SampleRate;
		pstAviShowControl->stAviAudioBuf[i].audiobufptr = (INT8U *)gp_malloc_align(SampleRate,4);
		if(pstAviShowControl->stAviAudioBuf[i].audiobufptr == NULL)
			DBG_PRINT ("--- !!! RESOURCE NOT ENOUGH !!! ---\r\n");
	}

	status = msgQSend(fs_msg_q_id, MSG_FILESRV_AVIAUDIO_FS_READ, NULL, 0, MSG_PRI_NORMAL);
    if (status != 0) {
		return 1;
    }

	status = (INT32S) OSQPend(audio_fs_result_q, 0, &err);
	if ((err != OS_NO_ERR) || status < 0) {
        	return -1;
    }

	return 0;
}

static void MovBufferInitial(void)
{
	INT32U i;
	INT32U jpeg_width;
	INT32U jpeg_height;
	INT32U product1, product2;
	POS_STRUCT	pos;

	pstAviShowControl->avi_image_width = tk_mov_get_width();
	pstAviShowControl->avi_image_height = tk_mov_get_height();
	#ifdef	_DPF_PROJECT
	gplib_ppu_sprite_visible_position_get(turnkey_ppu_register_set, &pos.x0, &pos.x2, &pos.y0, &pos.y2);
	#endif
	jpeg_width = pos.x2 - pos.x0 + 1;
	jpeg_height = pos.y0 - pos.y2 + 1;
	if((pstAviShowControl->avi_image_width > jpeg_width)||(pstAviShowControl->avi_image_height > jpeg_height))
	{
		pstAviShowControl->display_mode = AVI_FULL_SCREEN_SHOW_ON;
		pstAviShowControl->display_height = jpeg_height;
		pstAviShowControl->display_width = jpeg_width;
	}

	if(!(pstAviShowControl->display_mode & (AVI_USER_SIZE_USE|AVI_FULL_SCREEN_SHOW_ON)))
	{
		jpeg_width = pstAviShowControl->avi_image_width;
		jpeg_height = pstAviShowControl->avi_image_height;
		pstAviShowControl->display_width = jpeg_width;
		pstAviShowControl->display_height = jpeg_height;

	}
	else if(pstAviShowControl->display_mode & AVI_USER_SIZE_USE)
	{
		jpeg_width = pstAviShowControl->display_width;
		jpeg_height = pstAviShowControl->display_height;	
		if((jpeg_width == pstAviShowControl->avi_image_width)&&(jpeg_height == pstAviShowControl->avi_image_height))
		{
			pstAviShowControl->display_mode &= AVI_DEFAULT_SIZE_USE;
		}		
	}
	else
	{
		product1 = pstAviShowControl->avi_image_width*pstAviShowControl->display_height;
		product2 = pstAviShowControl->avi_image_height*pstAviShowControl->display_width;
		if(product1<product2)
		{
			jpeg_height = pstAviShowControl->display_height;
			jpeg_width = product1/pstAviShowControl->avi_image_height;
			#if(DPF_H_V==DPF_480x234)
			//0x2666 => 15%
			jpeg_width = jpeg_width * (0x10000 + 0x2666) >> 16;
			#endif
		}
		else if(product1>product2)
		{
			jpeg_width = pstAviShowControl->display_width;
			jpeg_height = product2/pstAviShowControl->avi_image_width;
			#if(DPF_H_V==DPF_480x234)
			//0x2666 => 15%
			jpeg_height = jpeg_height * (0x10000 - 0x2666) >> 16;
			#endif
		}
		else
		{
			jpeg_width = pstAviShowControl->display_width;
			jpeg_height = pstAviShowControl->display_height;
		}
		
		if((jpeg_width == pstAviShowControl->avi_image_width)&&(jpeg_height == pstAviShowControl->avi_image_height))
		{
			pstAviShowControl->display_mode &= AVI_FULL_SCREEN_SHOW_OFF;
		}
	}

	if(pstAviShowControl->avi_image_width&0x7)
		pstAviShowControl->display_mode = AVI_USER_SIZE_USE;

	jpeg_width = (jpeg_width + 7)& ~7;
	
	if(!(pstAviShowControl->display_mode & AVI_USER_POS_USE))
	{
		#ifdef _DPF_PROJECT
		gplib_ppu_sprite_visible_position_get(turnkey_ppu_register_set, &pos.x0, &pos.x2, &pos.y0, &pos.y2);		
		pstAviShowControl->display_position_x = (pos.x2 - pos.x0)/2+1;
		pstAviShowControl->display_position_y = (pos.y0 - pos.y2)/2+1;
		#endif
	}

	for(i=0;i<Avi_Buffer_TotalNum;i++)
	{
		pstAviShowControl->stAviBuf[i].buf_status = 0;
		pstAviShowControl->stAviBuf[i].display_mode = pstAviShowControl->display_mode;
		pstAviShowControl->stAviBuf[i].avi_image_width = jpeg_width;
		pstAviShowControl->stAviBuf[i].avi_image_height = jpeg_height;
		/*
		if(pstAviShowControl->stAviBuf[i].avibufptr == NULL)
			pstAviShowControl->stAviBuf[i].avibufptr = (INT8U *)gp_malloc_align(jpeg_width*jpeg_height*2,64);
		if(pstAviShowControl->stAviBuf[i].avibufptr == NULL)
			DBG_PRINT ("--- !!! RESOURCE NOT ENOUGH !!! ---\r\n");
		*/	
	}

	pstAviShowControl->CurrentFrame = 0;
	pstAviShowControl->TimeTickFrame = 0;
	pstAviShowControl->Videomodifyframe = 0;
	avi_decode_buf = &pstAviShowControl->stAviBuf[0];
	avi_display_buf = avi_decode_buf->next;
}

#ifdef	_DPF_PROJECT
INT32S mov_start(INT32S FileNum)
#else
INT32S mov_start(INT16S mov_movhandle, INT16S mov_audhandle) //Roy ADD
#endif
{
	#ifdef	_DPF_PROJECT	
	return tk_mov_decode_file_id(pstAviShowControl->FileId);
	#else
	return tk_mov_decode_file_parsing(mov_movhandle, mov_audhandle);
	#endif
}

INT32S mov_play_init(void)
{
	INT32S ret=0;
	MovVideoTimeSetting();
	if(pstAviShowControl->Videofps == 0)
		return -1;
	//AviPPUInitial();
	MovBufferInitial();
	pstAviShowControl->AudioFlag = tk_mov_get_hassound();
	if(pstAviShowControl->AudioFlag)
		ret = MovAudioInitial();
	pstAviShowControl->RealtimeFrame = 0;
	pstAviShowControl->CurrentTime = 0;
	OutputFlag = 0;
	return ret;
}

void mov_play(void)
{
	#ifdef	_DPF_PROJECT
	ap_umi_only_ppu_go_enable();
	#endif
	AviSetFrameTime(pstAviShowControl->Videofps);	
	if(pstAviShowControl->AudioFlag)
		MovAudioPlay();
}

INT32S tk_mov_vediodata_find(INT32U SilpFrameNum)
{
	INT32S filepos;
	INT32S jumpframe;
	INT32S status;
	INT8U  err;
	jumpframe = SilpFrameNum - pstAviShowControl->CurrentFrame;
	if(jumpframe>=1)
	{
		pstAviShowControl->CurrentFrame = SilpFrameNum;
		jumpframe++;
	}
	else
		jumpframe = 1;

	status = msgQSend(fs_msg_q_id, MSG_FILESRV_AVIVIDEO_FS_READ, &jumpframe, sizeof(INT32U), MSG_PRI_NORMAL);
	if(status != 0)
		return 1;
	filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
   	if ((err!=OS_NO_ERR) || (filepos<=0))
	{
		pstAviShowControl->AivStatus |= Avi_VideoFileEnd;
		return 1;
	}
	pstAviShowControl->CurrentFrame++;
	return 0;
}

INT32S mov_memory_free(void)
{
	INT32U i;
	
	#ifndef	_DPF_PROJECT	
	if(pstAviShowControl->avi_image_buf_malloc_flag != 1)
	{
		for(i=0;i<Avi_Buffer_TotalNum;i++)
		{
			if(pstAviShowControl->stAviBuf[i].avibufptr != NULL)
			{
				gp_free((void *)pstAviShowControl->stAviBuf[i].avibufptr);
				pstAviShowControl->stAviBuf[i].avibufptr = NULL;	
			}
		}
	}
	#endif	
	if(pstAviShowControl->AudioFlag)
	{
		for(i=0;i<Avi_Audio_Buffer_Num;i++)
		{
			if(pstAviShowControl->stAviAudioBuf[i].audiobufptr != NULL)
			{
				gp_free((void *)pstAviShowControl->stAviAudioBuf[i].audiobufptr);
				pstAviShowControl->stAviAudioBuf[i].audiobufptr = NULL;	
			}
		}
	}

	tk_mov_release_memory();
	tk_mov_close_file_id();
	return 0;
}

/*====  mov api end  ====*/