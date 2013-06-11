//* Version : 1.00
//* History :   1 shin modify : add more 4 bytes for video buffer to avoid pre-alignment issue 2008/4/8
//              2 shin modify : a .add more 4 bytes for audio buffer to avoid pre-alignment issue 2008/4/17
//                              b .fixed bug decode buffer shit
//				                c .modify the file position align dispose. issue 2008/6/10

/*Include files */
#include "ap_avi.h"

/* define */
#define AVI_NOT_SUPPORT	    0x0001
#define max_avi_index		2000
#define AUDIO_AVI_RINGBUF_LEN	4000


/* Variables defined in this file */
STAVIBufConfig		*avi_decode_buf;
STAVIBufConfig		*avi_display_buf;
STAVIAUDIOBufConfig *avi_audio_buf_wi;
STAVIAUDIOBufConfig *avi_audio_buf_ri;
#ifdef	_DPF_PROJECT
INT32U 				*charram = NULL;
#endif
STAviShowControl	*pstAviShowControl;
INT32U				g_avi_ignoreframe;
extern INT32S audiolen;

static void AviVideoTimeSetting(void);
static void AviTimeTick(void);
static void	AviAudioPlay(void);

static INT32U AviAudioInitial(void);
static void AviDecodeInitial(void);
static void AviBufferInitial(void);
//static INT32S AviVedioDataFind(INT32U SilpFrameNum);

void   AviPPUInitial(void);
void   AviSetFrameTime(INT32U freq_hz);
void   avi_ppu_set(STAVIBufConfig* bufconfig);
#ifdef	_DPF_PROJECT
INT32S avi_start(INT32S FileNum);
#else
INT32S avi_start(INT16S avi_movhandle, INT16S avi_audhandle);
#endif
INT32S avi_frame_calculate(void);
INT32S avi_memory_free(void);
INT32S tk_avi_vediodata_find(INT32U SilpFrameNum);
INT32S avi_play_init(void);
void   avi_play(void);
INT32U avi_current_time_get(void);
INT32S avi_jumpframe_show(void);
INT32S avi_audio_buf_fill_again(void);
#ifndef	_DPF_PROJECT
extern INT32U tk_avi_decode_file_parsing(INT16S avi_movhandle, INT16S avi_audhandle);
#endif
extern INT32U OutputFlag;

static void AviVideoTimeSetting(void)
{
	FP64 videofps;
	videofps = tk_avi_get_fps();
	pstAviShowControl->Videofps = (INT32U)videofps;
	pstAviShowControl->videofloatframe = videofps - pstAviShowControl->Videofps;
}

void AviSetFrameTime(INT32U freq_hz)
{
	INT32U msec;
	msec = 1000 / freq_hz;
	timer_msec_setup(AVI_TIMER, msec, 0, AviTimeTick);
}

static void AviTimeTick(void)
{
	INT32S ret;
  #if 0//_DRV_L1_GTE == 1
	INT32U Residue;
  #endif

	pstAviShowControl->RealtimeFrame++;
	//calculate current play time
  #if 0//_DRV_L1_GTE == 1
	INT32U_DIVISION(pstAviShowControl->RealtimeFrame,pstAviShowControl->Videofps,pstAviShowControl->CurrentTime,Residue);
  #else
  	pstAviShowControl->CurrentTime = pstAviShowControl->RealtimeFrame / pstAviShowControl->Videofps;
  #endif
	ret = msgQSend(ApQ, MSG_AVI_TICKER, NULL, 0, MSG_PRI_NORMAL);
#ifdef	_DPF_PROJECT
	if(ret == -1) {
		DBG_PRINT("Failed to call msgQSend() in AviTimeTick\r\n");
		return;
	}
#endif
	//DBG_PRINT ("\n\rT 0x%x\n\r", pstAviShowControl->TimeTickFrame);
}

static INT32U AviAudioInitial(void)
{
	INT32U  i;
	INT32U	SampleRate;
	INT32S  status;
	INT8U   err;
	INT16U 	wave_bit;
	INT16U  channel_num;
	FP64    temp;


	SampleRate = tk_avi_get_audio_samplerate();

	channel_num = tk_avi_wave_channelnum_get();

	g_avi_synchroframe = 0;
	temp = tk_avi_get_fps() / (FP64)SampleRate;
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

	wave_bit = tk_avi_wave_bitformat_get();
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

static void	AviAudioPlay(void)
{
	INT32S ret;
	ret = msgQSend(AudioTaskQ,MSG_AUD_AVI_PLAY,NULL,0,MSG_PRI_NORMAL);

}
void AviPPUInitial(void)
{
	#ifdef	_DPF_PROJECT
	charram = (INT32U *) gp_malloc(8192);
		
	if(charram == NULL) {
		DBG_PRINT ("Failed to allocate charram in AviPPUInitial\r\n");
		return;
	}
	//gplib_ppu_text_number_array_ptr_set(turnkey_ppu_register_set, C_PPU_TEXT1, (INT32U) charram);
	//gplib_ppu_text_number_array_ptr_set(turnkey_ppu_register_set, C_PPU_TEXT1, (INT32U) charram+4096);
	ppu_text_charnum_array_add(C_PPU_TEXT1, (INT32U) charram);
	ppu_text_charnum_array_add(C_PPU_TEXT1, (INT32U) charram+4096);
	#endif
}

void AviPPUUnInitial(void)
{
	#ifdef	_DPF_PROJECT
	if(charram != NULL)
	{
		ppu_text_charnum_array_release(C_PPU_TEXT1);
		charram = NULL;
	}	
	#endif
}
void avi_ppu_set(STAVIBufConfig* bufconfig)
{
	#ifdef	_DPF_PROJECT
	INT32U window_index;
	INT32U pos_x0;
	INT32U pos_y0;
	INT32U width;
	INT32U height;
	INT32U mask;

	INT32U number_array;

	//gplib_ppu_sprite_enable_set(turnkey_ppu_register_set, 0);
	number_array =turnkey_ppu_register_set->text[C_PPU_TEXT1].n_ptr;
	gplib_ppu_text_init(turnkey_ppu_register_set, C_PPU_TEXT1);								// Initiate TEXT
	gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1);					// Enable TEXT
	gplib_ppu_text_mode_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1);						// Set TEXT to rotate mode
	gplib_ppu_text_attribute_source_select(turnkey_ppu_register_set, C_PPU_TEXT1, 1);		// Get TEXT attribute from register
	#if(DPF_H_V!=DPF_800x600)
	gplib_ppu_text_size_set(turnkey_ppu_register_set, C_PPU_TEXT1, 2);						// Set TEXT size to 1024x512
	#else
	gplib_ppu_text_size_set(turnkey_ppu_register_set, C_PPU_TEXT1, 3);						// Set TEXT size to 1024x1024
	#endif
	#if 0
	gplib_ppu_text_character_size_set(turnkey_ppu_register_set, C_PPU_TEXT1, 2, 2);			// Set TEXT character size to 32x32
	#else
	gplib_ppu_text_bitmap_mode_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1);				// Enable bitmap mode
	#endif

	gplib_ppu_text_color_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1, 3);					// Set TEXT color to YUYV


	gplib_ppu_text_segment_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);	// Set TEXT segment address
	gplib_ppu_text_number_array_ptr_set(turnkey_ppu_register_set, C_PPU_TEXT1, number_array);// Set TEXT number array address
	gplib_ppu_text_calculate_number_array(turnkey_ppu_register_set, C_PPU_TEXT1, bufconfig->avi_image_width, bufconfig->avi_image_height, (INT32U)bufconfig->avibufptr);	// Calculate Number array

	//window register config
	window_index = 3;
	
	if(ap_umi_display_device_get() != C_CURRENT_DISPLAY_DEVICE_TFT)
	{
		switch(ap_umi_display_device_get())
		{
			case C_CURRENT_DISPLAY_DEVICE_TV_VGA:
				if(bufconfig->avi_image_width > 640)
				{
					pos_x0 = 0;
					if(bufconfig->avi_image_height > 480)
						pos_y0 = 0;
					else
						pos_y0 = (480 - bufconfig->avi_image_height)/2;
				}
				else if(bufconfig->avi_image_height > 480)
				{
					pos_y0 = 0;
					pos_x0 = (640 - bufconfig->avi_image_width)/2;		
				}
				else
				{
					pos_x0 = (640 - bufconfig->avi_image_width)/2;
					pos_y0 = (480 - bufconfig->avi_image_height)/2;
				}
				break;
			case C_CURRENT_DISPLAY_DEVICE_TV_QVGA:
				if(bufconfig->avi_image_width > 320)
				{
					pos_x0 = 0;
					if(bufconfig->avi_image_height > 240)
						pos_y0 = 0;
					else
						pos_y0 = (240 - bufconfig->avi_image_height)/2;
				}
				else if(bufconfig->avi_image_height > 240)
				{
					pos_y0 = 0;
					pos_x0 = (320 - bufconfig->avi_image_width)/2;		
				}
				else
				{
					pos_x0 = (320 - bufconfig->avi_image_width)/2;
					pos_y0 = (240 - bufconfig->avi_image_height)/2;
				}
				break;

			case C_CURRENT_DISPLAY_DEVICE_TV_D1:
				if(bufconfig->avi_image_width > 720)
				{
					pos_x0 = 0;
					if(bufconfig->avi_image_height > 480)
						pos_y0 = 0;
					else
						pos_y0 = (480 - bufconfig->avi_image_height)/2;
				}
				else if(bufconfig->avi_image_height > 480)
				{
					pos_y0 = 0;
					pos_x0 = (720 - bufconfig->avi_image_width)/2;		
				}
				else
				{
					pos_x0 = (720 - bufconfig->avi_image_width)/2;
					pos_y0 = (480 - bufconfig->avi_image_height)/2;
				}
				break;
			 default:
			 	break;
		}
		
		width = bufconfig->avi_image_width;
		height = bufconfig->avi_image_height;
		mask = 0;	
	}
	else
	{
		#if((DPF_H_V==DPF_800x480)||(DPF_H_V==DPF_800x600))
		if(bufconfig->avi_image_width > 800)
		#elif(DPF_H_V==DPF_480x234)
		if(bufconfig->avi_image_width > 480)
		#elif(DPF_H_V==DPF_320x240)
		if(bufconfig->avi_image_width > 320)
		#elif(DPF_H_V==DPF_640x480)
		if(bufconfig->avi_image_width > 640)
		#endif
		{
			pos_x0 = 0;
			#if((DPF_H_V==DPF_800x480)||(DPF_H_V==DPF_640x480))
			if(bufconfig->avi_image_height > 480)
				pos_y0 = 0;
			else
				pos_y0 = (480 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_800x600)
			if(bufconfig->avi_image_height > 600)
				pos_y0 = 0;
			else
				pos_y0 = (600 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_480x234)
			if(bufconfig->avi_image_height > 234)
				pos_y0 = 0;
			else
				pos_y0 = (234 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_320x240)
			if(bufconfig->avi_image_height > 240)
				pos_y0 = 0;
			else
				pos_y0 = (240 - bufconfig->avi_image_height)/2;
			#endif
		}
		#if((DPF_H_V==DPF_800x480)||(DPF_H_V==DPF_640x480))
		else if(bufconfig->avi_image_height > 480)
		#elif(DPF_H_V==DPF_800x600)
		else if(bufconfig->avi_image_height > 600)
		#elif(DPF_H_V==DPF_480x234)
		else if(bufconfig->avi_image_height > 234)
		#elif(DPF_H_V==DPF_320x240)
		else if(bufconfig->avi_image_height > 240)
		#endif
		{
			pos_y0 = 0;
			#if(DPF_H_V==DPF_800x480)
			pos_x0 = (800 - bufconfig->avi_image_width)/2;
			#elif(DPF_H_V==DPF_800x600)
			pos_x0 = (800 - bufconfig->avi_image_width)/2;
			#elif(DPF_H_V==DPF_480x234)
			pos_x0 = (480 - bufconfig->avi_image_width)/2;
			#elif(DPF_H_V==DPF_320x240)
			pos_x0 = (320 - bufconfig->avi_image_width)/2;
			#elif(DPF_H_V==DPF_640x480)
			pos_x0 = (640 - bufconfig->avi_image_width)/2;		
			#endif
		}
		else
		{
			#if(DPF_H_V==DPF_800x480)
			pos_x0 = (800 - bufconfig->avi_image_width)/2;
			pos_y0 = (480 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_800x600)
			pos_x0 = (800 - bufconfig->avi_image_width)/2;
			pos_y0 = (600 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_480x234)
			pos_x0 = (480 - bufconfig->avi_image_width)/2;
			pos_y0 = (234 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_320x240)
			pos_x0 = (320 - bufconfig->avi_image_width)/2;
			pos_y0 = (240 - bufconfig->avi_image_height)/2;
			#elif(DPF_H_V==DPF_640x480)
			pos_x0 = (640 - bufconfig->avi_image_width)/2;
			pos_y0 = (480 - bufconfig->avi_image_height)/2;
			#endif
		}
		width = bufconfig->avi_image_width;
		height = bufconfig->avi_image_height;
		mask = 0;
	}
	turnkey_ppu_window_set(window_index,pos_x0,pos_y0,width,height,mask);
	gplib_ppu_text_window_select(turnkey_ppu_register_set, C_PPU_TEXT1, 3);

	//gplib_ppu_text_position_set(turnkey_ppu_register_set, C_PPU_TEXT1, 800/2, 480/2);
	//gplib_ppu_text_offset_set(turnkey_ppu_register_set, C_PPU_TEXT1, ((800 - bufconfig->avi_image_width)/2)+400, ((480 - bufconfig->avi_image_height)/2)+240);
	gplib_ppu_text_position_set(turnkey_ppu_register_set, C_PPU_TEXT1, bufconfig->avi_image_width/2, bufconfig->avi_image_height/2);
	gplib_ppu_text_offset_set(turnkey_ppu_register_set, C_PPU_TEXT1, pstAviShowControl->display_position_x, pstAviShowControl->display_position_y);
	gplib_ppu_text_rotate_zoom_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0, 1);
	issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
	#endif
}

static void AviBufferInitial(void)
{
	INT32U i;
	INT32U jpeg_width;
	INT32U jpeg_height;
	INT32U product1, product2;
	POS_STRUCT	pos;

	pstAviShowControl->avi_image_width = tk_avi_get_width();
	pstAviShowControl->avi_image_height = tk_avi_get_height();
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

	//added code
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

static void AviDecodeInitial(void)
{
	AviBufferInitial();
}

INT32S tk_avi_vediodata_find(INT32U SilpFrameNum)
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
	//filepos = tk_avi_find_vdpos(jumpframe);
	//if(filepos == -1)
	{
		pstAviShowControl->AivStatus |= Avi_VideoFileEnd;
		return 1;
	}
	pstAviShowControl->CurrentFrame++;
	return 0;
}

INT32S avi_audio_buf_fill_again(void)
{
	INT32S	i;
	INT32S	status;
	INT8U	err;
	avi_audio_buf_wi = &pstAviShowControl->stAviAudioBuf[0];
	avi_audio_buf_ri = avi_audio_buf_wi;

	for(i=0;i<Avi_Audio_Buffer_Num;i++)
	{
		if(i == 0)
			pstAviShowControl->stAviAudioBuf[i].status = AUDIO_BUF_FILL_ALL_AGAIN;
		else
			pstAviShowControl->stAviAudioBuf[i].status = AUDIO_BUF_FIRST_FILL;
		pstAviShowControl->stAviAudioBuf[i].ri = 0;
		pstAviShowControl->stAviAudioBuf[i].wi = 0;
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

INT32S avi_jumpframe_show(void)
{
	pstAviShowControl->CurrentFrame = pstAviShowControl->TimeTickFrame;
	#ifdef	_DPF_PROJECT
	avi_decode_buf = avi_decode_buf->next;
	#else
	if(OutputFlag)
	{
		avi_decode_buf = avi_decode_buf->next;
		OutputFlag = 0;
	}
	#endif
	avi_decode_buf->buf_status = 1;
	if(!(pstAviShowControl->display_mode & AVI_USER_SIZE_USE))
	{
		if((avi_decode_buf->avi_image_width != pstAviShowControl->avi_image_width)&&
		   (avi_decode_buf->avi_image_height!= pstAviShowControl->avi_image_height))
		{
			avi_decode_buf->avi_image_width = pstAviShowControl->avi_image_width;
			avi_decode_buf->avi_image_height = pstAviShowControl->avi_image_height;
			gp_free((void *)avi_decode_buf->avibufptr);
			avi_decode_buf->avibufptr = (INT8U *)gp_malloc_align(avi_decode_buf->avi_image_width*avi_decode_buf->avi_image_height*2,4);
			DBG_PRINT ("Video Buffer2 ptr %d s\r\n", avi_decode_buf->avibufptr);
		}
	}
	else if(pstAviShowControl->display_mode & AVI_USER_SIZE_USE)
	{
		if((avi_decode_buf->avi_image_width != pstAviShowControl->display_width)&&
		   (avi_decode_buf->avi_image_height!= pstAviShowControl->display_height))
		{
			avi_decode_buf->avi_image_width = pstAviShowControl->display_width;
			avi_decode_buf->avi_image_height = pstAviShowControl->display_height;
			gp_free((void *)avi_decode_buf->avibufptr);
			avi_decode_buf->avibufptr = (INT8U *)gp_malloc_align(avi_decode_buf->avi_image_width*avi_decode_buf->avi_image_height*2,4);
			DBG_PRINT ("Video Buffer2 ptr %d s\r\n", avi_decode_buf->avibufptr);
		}
	}
	avi_decode_buf->display_mode = pstAviShowControl->display_mode;
	avi_decode_buf->avi_framenum_silp = pstAviShowControl->TimeTickFrame;
	msgQSend(image_msg_queue_id, MSG_MJPEG_DECODE_1, avi_decode_buf, sizeof(STAVIBufConfig), MSG_PRI_URGENT);
	return 0;
}
INT32S avi_frame_calculate(void)
{
	INT8U  err;
  #if 0//_DRV_L1_GTE == 1
	INT32U Quotient;
  #endif
	INT32U Residue;
	INT32U framecnt;

	pstAviShowControl->TimeTickFrame++;

	if(pstAviShowControl->AudioFlag)
	{
		OSQAccept(aud_avi_q,&err);
		if (err == OS_NO_ERR)
		{
			framecnt = (INT32U)g_avi_synchroframe;
			if(pstAviShowControl->TimeTickFrame < framecnt)
				pstAviShowControl->TimeTickFrame = framecnt;
			else if(pstAviShowControl->TimeTickFrame > framecnt+2)
			{
				pstAviShowControl->TimeTickFrame--;
				return 0;
			}
		}
		else
		{
			if((pstAviShowControl->TimeTickFrame > ((INT32U)g_avi_synchroframe+g_avi_ignoreframe))&&(!(pstAviShowControl->AivStatus&Avi_AudioFileEnd)))
			{
				pstAviShowControl->TimeTickFrame--;
				return 0;
			}
		}
	}

  #if 0//_DRV_L1_GTE == 1
	INT32U_DIVISION(pstAviShowControl->TimeTickFrame,pstAviShowControl->Videofps,Quotient,Residue);
  #else
  	Residue = pstAviShowControl->TimeTickFrame % pstAviShowControl->Videofps;
  #endif
	if(Residue == 0)
	{
		pstAviShowControl->Videomodifyframe = pstAviShowControl->Videomodifyframe + pstAviShowControl->videofloatframe;
		if(pstAviShowControl->Videomodifyframe > 1)
		{
			pstAviShowControl->TimeTickFrame++;
			pstAviShowControl->Videomodifyframe = pstAviShowControl->Videomodifyframe - 1;
		}
	}

	if(!(pstAviShowControl->AivStatus & Avi_VideoFileEnd))
	{
		if(pstAviShowControl->TimeTickFrame>=pstAviShowControl->CurrentFrame)
		{
			#ifdef	_DPF_PROJECT
			if(OutputFlag == 0)	//A
			{
				avi_decode_buf = &pstAviShowControl->stAviBuf[0];

			}
			else if(OutputFlag == 1)	//B
			{
				avi_decode_buf = &pstAviShowControl->stAviBuf[1];
			}
			#else
			if(OutputFlag)
			{
				avi_decode_buf = avi_decode_buf->next;
				OutputFlag = 0;
			}
			#endif
			avi_decode_buf->buf_status = 1;
			/*
			if(!(pstAviShowControl->display_mode & AVI_USER_SIZE_USE))
			{
				if((avi_decode_buf->avi_image_width != pstAviShowControl->avi_image_width)&&
				   (avi_decode_buf->avi_image_height!= pstAviShowControl->avi_image_height))
				{
					avi_decode_buf->avi_image_width = pstAviShowControl->avi_image_width;
					avi_decode_buf->avi_image_height = pstAviShowControl->avi_image_height;
					gp_free((void *)avi_decode_buf->avibufptr);
					avi_decode_buf->avibufptr = (INT8U *)gp_malloc_align(avi_decode_buf->avi_image_width*avi_decode_buf->avi_image_height*2,4);
				}
			}
			else if(pstAviShowControl->display_mode & AVI_USER_SIZE_USE)
			{
				if((avi_decode_buf->avi_image_width != pstAviShowControl->display_width)&&
				   (avi_decode_buf->avi_image_height!= pstAviShowControl->display_height))
				{
					avi_decode_buf->avi_image_width = pstAviShowControl->display_width;
					avi_decode_buf->avi_image_height = pstAviShowControl->display_height;
					gp_free((void *)avi_decode_buf->avibufptr);
					avi_decode_buf->avibufptr = (INT8U *)gp_malloc_align(avi_decode_buf->avi_image_width*avi_decode_buf->avi_image_height*2,4);
				}
			}
			*/
			avi_decode_buf->display_mode = pstAviShowControl->display_mode;
			avi_decode_buf->avi_framenum_silp = pstAviShowControl->TimeTickFrame;
			msgQSend(image_msg_queue_id, MSG_MJPEG_DECODE, avi_decode_buf, sizeof(STAVIBufConfig), MSG_PRI_URGENT);
		}
	}
	return 0;
}

#ifdef	_DPF_PROJECT
INT32S avi_start(INT32S FileNum)
#else
INT32S avi_start(INT16S avi_movhandle, INT16S avi_audhandle) //Roy ADD
#endif
{
	INT32U ret;
	#ifdef	_DPF_PROJECT
	if(pstAviShowControl->display_mode&AVI_FILE_NAME_USE)
		ret = tk_avi_decode_file_name((CHAR*) pstAviShowControl->pstring);
	else
		ret = tk_avi_decode_file_id(pstAviShowControl->FileId);
	#else
 	ret = tk_avi_decode_file_parsing(avi_movhandle, avi_audhandle);//Roy Add
	#endif
	return ret;
}

INT32S avi_play_init(void)
{
	INT32S ret=0;
	AviVideoTimeSetting();
	if(pstAviShowControl->Videofps == 0)
		return -1;
	//AviPPUInitial();
	AviDecodeInitial();
	pstAviShowControl->AudioFlag = tk_avi_get_hassound();
 	if(pstAviShowControl->AudioFlag)
		ret = AviAudioInitial();
	pstAviShowControl->RealtimeFrame = 0;
	pstAviShowControl->CurrentTime = 0;
	//#ifndef	_DPF_PROJECT
	OutputFlag = 0;		//0 = A .1 = B
	//#endif
	return ret;
}

void avi_play(void)
{
	#ifdef	_DPF_PROJECT
	ap_umi_only_ppu_go_enable();
	#endif	
	AviSetFrameTime(pstAviShowControl->Videofps);
	if(pstAviShowControl->AudioFlag)
		AviAudioPlay();
}

INT32U avi_current_time_get(void)
{
	return pstAviShowControl->CurrentTime;
}

INT32S avi_memory_free(void)
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
	tk_avi_close_file_id(pstAviShowControl->FileId);
	return 0;
}

INT32S audio_avi_read(INT32U buf_addr, INT32U len)
{
	/* to do read data into ring buffer*/
#if 0
	INT8S *ptr;

	ptr =(INT8S *) wav_fish;
	gp_memcpy((INT8S*)buf_addr,ptr+avi_len,len);
	avi_len += len;
	if (avi_len >= LEN_WAV_FISH) {
		OSQPost(audio_cmdq, (void *) AUDIO_CMD_STOP);
	}
	return len;
#else
	INT8S  *ptr;
	INT32U templen;
	INT32S status;
	INT8U  err;
	INT32U buffer_count;

	/*
	status = (INT32S) OSQAccept(audio_fs_result_q, &err);
	if (err == OS_NO_ERR) {
		status = msgQSend(fs_msg_q_id, MSG_FILESRV_AVIAUDIO_FS_READ, NULL, 0, MSG_PRI_NORMAL);
	    if (status != 0) {
			return 1;
	    }
	}
	*/


	status = msgQSend(fs_msg_q_id, MSG_FILESRV_AVIAUDIO_FS_READ, NULL, 0, MSG_PRI_URGENT);
	if (status != 0) {
		//return -1;
	}

	//OSQPend(audio_fs_result_q,0,&err);
	//OSTimeDly(2);
	// the first time audio task get wmv data
	if(avi_audio_buf_ri->status == AUDIO_BUF_FILLING)
	{
		if(avi_audio_buf_ri->wi > avi_audio_buf_ri->ri + len)
		{
			ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
			gp_memcpy((INT8S*)buf_addr,ptr,len);
			avi_audio_buf_ri->ri += len;
			avi_audio_buf_ri->status = AUDIO_BUF_GETING;
		}
		else if(avi_audio_buf_ri->wi == avi_audio_buf_ri->ri + len)
		{
			ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
			gp_memcpy((INT8S*)buf_addr,ptr,len);
			avi_audio_buf_ri->ri += len;
			avi_audio_buf_ri->status = AUDIO_BUF_EMPTY;//AUDIO_BUF_FILLING;//AUDIO_BUF_EMPTY;
			avi_audio_buf_ri->wi = 0;
			//avi_audio_buf_ri->ri = 0;//debug
			avi_audio_buf_ri = avi_audio_buf_ri->next;
		}
	}
	// the next time audio task get wmv data
	else if(avi_audio_buf_ri->status == AUDIO_BUF_GETING)
	{
		if(avi_audio_buf_ri->wi > avi_audio_buf_ri->ri + len)
		{
			ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
			gp_memcpy((INT8S*)buf_addr,ptr,len);
			avi_audio_buf_ri->ri += len;
		}
		else if(avi_audio_buf_ri->wi == avi_audio_buf_ri->ri + len)
		{
			ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
			gp_memcpy((INT8S*)buf_addr,ptr,len);
			avi_audio_buf_ri->ri += len;
			avi_audio_buf_ri->status = AUDIO_BUF_EMPTY;//AUDIO_BUF_FILLING;//AUDIO_BUF_EMPTY;
			avi_audio_buf_ri->wi = 0;
			//avi_audio_buf_ri->ri = 0;//debug
			avi_audio_buf_ri = avi_audio_buf_ri->next;
		}
		else
		{
			templen = avi_audio_buf_ri->wi - avi_audio_buf_ri->ri;
			ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
			gp_memcpy((INT8S*)buf_addr,ptr,templen);
			avi_audio_buf_ri->ri += templen;
			avi_audio_buf_ri->status = AUDIO_BUF_EMPTY;//AUDIO_BUF_FILLING;//AUDIO_BUF_EMPTY;
			avi_audio_buf_ri->wi = 0;
			//avi_audio_buf_ri->ri = 0;//debug
			buffer_count = 0;
			while(1)
			{
				avi_audio_buf_ri = avi_audio_buf_ri->next;
				if(avi_audio_buf_ri->wi > (len-templen))
				{
					ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
					gp_memcpy((INT8S*)(buf_addr+templen),ptr,(len-templen));
					avi_audio_buf_ri->ri += len-templen;
					if(avi_audio_buf_ri->status == AUDIO_BUF_FILLING)
						avi_audio_buf_ri->status = AUDIO_BUF_GETING;
					break;
				}
				else
				{
				 	ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
					gp_memcpy((INT8S*)(buf_addr+templen),ptr,avi_audio_buf_ri->wi);
					avi_audio_buf_ri->ri = avi_audio_buf_ri->wi;
					templen += avi_audio_buf_ri->wi;
					avi_audio_buf_ri->wi = 0;
					if(avi_audio_buf_ri->status != AUDIO_BUF_OVER)
					{
						avi_audio_buf_ri->status = AUDIO_BUF_EMPTY;
					}
					else
						return -1;
				}
				buffer_count++;
				if(buffer_count >= 4)
				{
					OSQPend(audio_fs_result_q,80,&err);
					if (err != OS_NO_ERR) {
						return -2;
					}
					buffer_count = 0;
				}
			}
		}
	}
	// the last time audio task get wmv data
	else if(avi_audio_buf_ri->status == AUDIO_BUF_OVER)
	{
		if(avi_audio_buf_ri->wi != 0)
		{
			if(avi_audio_buf_ri->wi > avi_audio_buf_ri->ri + len)
			{
				ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
				gp_memcpy((INT8S*)buf_addr,ptr,len);
				avi_audio_buf_ri->ri += len;
			}
			else
			{
				templen = avi_audio_buf_ri->wi - avi_audio_buf_ri->ri;
				ptr = (INT8S *)((INT32U)avi_audio_buf_ri->audiobufptr+avi_audio_buf_ri->ri);
				gp_memcpy((INT8S*)buf_addr,ptr,templen);
				avi_audio_buf_ri->ri += templen;
				avi_audio_buf_ri->wi = 0;
				//avi_audio_buf_ri->ri = 0;//debug
				len = templen;
			}
		}
		else	//data is finish
		{
			return -1;
		}
	}
	//write data point was pursued by read data point.
	else
	{
		OSQPend(audio_fs_result_q,80,&err);
		if (err != OS_NO_ERR) {
			return -2;
		}
	}
	return len;
#endif
}
