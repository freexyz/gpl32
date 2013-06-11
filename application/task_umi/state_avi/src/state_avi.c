//* Version : 1.00
//* History ;   1. 2008/6/17 the base code

/*Include files */
#include "state_avi.h"

/* define */
#define AVI_DECODE_OK	 		0x0000
#define AVI_DECODE_ERROR	 	0x0001
#define AVI_DEVICE_PLUGOUT		0x0002
#define	AVI_DECODE_PAUSE		0x0003
#define AVI_DECODE_STOP			0x0004
#define AVI_STATE_SUSPEND		0x0005

#define R_PPU_TEXT1_CTRL       	(*((volatile INT32U *) 0xD050004C))
#define AP_AVI_FS_Q_SIZE 	1

#define C_RUBBERKEY_FLAG_AVI_RESUME		0x01
#define C_RUBBERKEY_FLAG_AVI_PAUSE		0x02


#define DBG_MODE_ON			CUSTOM_ON
#define DBG_MODE_OFF		CUSTOM_OFF
#define AVI_DBG_MODE		DBG_MODE_OFF

#if	AVI_PLAY_MODE
#define	C_CONTINUOUS_PLAY	0
#define	C_STOP_PLAYING		1
#endif

#if (DPF_H_V==DPF_800x600)
#define mjpeg_width				800
#define mjpeg_height			600
#elif (DPF_H_V==DPF_800x480)
#define mjpeg_width				800
#define mjpeg_height			480
#elif (DPF_H_V==DPF_480x234)
#define mjpeg_width				480
#define mjpeg_height			234
#elif (DPF_H_V==DPF_320x240)
#define mjpeg_width				320
#define mjpeg_height			240

#endif
void				*audio_fs_q[AP_AVI_FS_Q_SIZE];
OS_EVENT			*audio_fs_result_q;

INT16U avi_mov_switchflag;
INT32U save_file_num;
INT32U avi_status;
INT16U osd_avi_status;

#if	AVI_PLAY_MODE
INT32U avi_play_status = C_CONTINUOUS_PLAY;
INT32U avi_file_num = 0;
#endif

#if VOLUME_CONTROL_STATE_MODE
// 0:	no volume control key press down.
// 1:	avi state need suspend to volume state.
// 2:	avi state need resume the avi.
// 3: 	avi state need keep avi pause status.
INT32U avi_volume_control_state = 0;
#endif

/* Variables defined in this file */
void state_avi_entry(STATE_ENTRY_PARA* para1);
static void state_avi_init(void);
static void state_avi_exit(void);
void state_avi_resume(void);
void state_avi_suspend(EXIT_FLAG_ENUM exit_flag);


#ifdef	_DPF_PROJECT
#else
INT8U  ApQ_para[AP_QUEUE_MSG_MAX_LEN];
extern OS_EVENT	*avi_status_q;
extern void video_decode_end(void);	//Roy add
#endif
extern INT8U osd_timer_id;
void test(void)
{
	if(!(pstAviShowControl->display_mode & AVI_USER_SIZE_USE))
	{
		if(pstAviShowControl->avi_image_width == 640)
		{
			pstAviShowControl->display_width = 320;
			pstAviShowControl->display_height = 240;
		}
		else
		{
			pstAviShowControl->display_width = 640;
			pstAviShowControl->display_height = 480;
		}

		pstAviShowControl->display_mode |= AVI_USER_SIZE_USE;
	}
	else
	{
		pstAviShowControl->display_mode &= AVI_DEFAULT_SIZE_USE;
	}
}

void testkey(void)
{
	msgQSend(ApQ, MSG_AVI_DISPLAYSIZE_CHANGE, NULL, 0, MSG_PRI_NORMAL);
}

#if	AVI_PLAY_MODE
/*
static INT32S check_avi_file(INT8U *extname)
{
	INT32S	i;
	INT8U	name[4];

	gp_memset((INT8S*)name, 0, 4);
	for(i = 0; i < 4; i++)
	{
		if(extname[i] == 0)
		{
			break;
		}
		name[i] = gp_toupper(extname[i]);
	}
	return (gp_strcmp((INT8S*)name, (INT8S*)"AVI") && gp_strcmp((INT8S*)name, (INT8S*)"MOV"));
}
*/
void state_avi_reinit(void)
{
	INT32U i;
	audio_fs_result_q = OSQCreate(audio_fs_q, AP_AVI_FS_Q_SIZE);
	//gp_memset((void *)pstAviShowControl, 0x00, sizeof(STAviShowControl));

	pstAviShowControl->AivStatus = Avi_Prep;
	for(i=0;i<Avi_Buffer_TotalNum;i++)
	{
		//pstAviShowControl->stAviBuf[i].avibufptr = NULL;
		if(i != (Avi_Buffer_TotalNum-1))
			pstAviShowControl->stAviBuf[i].next = &pstAviShowControl->stAviBuf[i+1];
		else
			pstAviShowControl->stAviBuf[i].next = &pstAviShowControl->stAviBuf[0];
	}
	for(i=0;i<Avi_Audio_Buffer_Num;i++)
	{
		pstAviShowControl->stAviAudioBuf[i].audiobufptr = NULL;
		if(i != (Avi_Audio_Buffer_Num-1))
			pstAviShowControl->stAviAudioBuf[i].next = &pstAviShowControl->stAviAudioBuf[i+1];
		else
			pstAviShowControl->stAviAudioBuf[i].next = &pstAviShowControl->stAviAudioBuf[0];
	}
	avi_decode_buf = &pstAviShowControl->stAviBuf[0];
	avi_display_buf = avi_decode_buf->next;
	osd_avi_status = 0;
	avi_mov_switchflag = C_AVI_MOV_DEFAULT;
	avi_status = AVI_DECODE_STOP;
}
#endif

void state_avi_init(void)
{
	INT32U i;

#if(SLIDESHOW_WITH_VIDEO == CUSTOM_ON)
	#if(SLIDEVIDEO_KEY_MODE == SLIDEVIDEO_KEY_NORMAL)
		#ifdef _DPF_PROJECT
		umi_osd_menu_enter();
	  	#endif
	#endif
#else
	#ifdef _DPF_PROJECT
	umi_osd_menu_enter();
  	#endif
#endif

#ifdef _DPF_PROJECT
	if(umi_music_play_onoff_get())
	{
		umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 0);
	}
	else
	{
		umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 1);
	}
 #endif
	audio_fs_result_q = OSQCreate(audio_fs_q, AP_AVI_FS_Q_SIZE);

	pstAviShowControl = (STAviShowControl*)gp_malloc(sizeof(STAviShowControl));
	gp_memset((void *)pstAviShowControl, 0x00, sizeof(STAviShowControl));

	pstAviShowControl->AivStatus = Avi_Prep;
	for(i=0;i<Avi_Buffer_TotalNum;i++)
	{
		#ifdef _DPF_PROJECT
		pstAviShowControl->stAviBuf[i].avibufptr = (INT8U *)gp_malloc_align(mjpeg_width*mjpeg_height*2,64);
		#else
		pstAviShowControl->stAviBuf[i].avibufptr = NULL;
		#endif
		if(i != (Avi_Buffer_TotalNum-1))
			pstAviShowControl->stAviBuf[i].next = &pstAviShowControl->stAviBuf[i+1];
		else
			pstAviShowControl->stAviBuf[i].next = &pstAviShowControl->stAviBuf[0];
	}
	for(i=0;i<Avi_Audio_Buffer_Num;i++)
	{
		pstAviShowControl->stAviAudioBuf[i].audiobufptr = NULL;
		if(i != (Avi_Audio_Buffer_Num-1))
			pstAviShowControl->stAviAudioBuf[i].next = &pstAviShowControl->stAviAudioBuf[i+1];
		else
			pstAviShowControl->stAviAudioBuf[i].next = &pstAviShowControl->stAviAudioBuf[0];
	}
	avi_decode_buf = &pstAviShowControl->stAviBuf[0];
	avi_display_buf = avi_decode_buf->next;
	gplib_jpeg_default_huffman_table_load();
	AviPPUInitial();

	osd_avi_status = 0;
	avi_mov_switchflag = C_AVI_MOV_DEFAULT;
	avi_status = AVI_DECODE_STOP;
	#if VOLUME_CONTROL_STATE_MODE
	avi_volume_control_state = 0;
	#endif
}
extern INT8U           *avi_state_mon;
//osd_avi_status	0: playing
//osd_avi_status	1: pause holding
//osd_avi_status	2: pause OK;
//osd_avi_status	3: resume holding


void state_avi_entry(STATE_ENTRY_PARA* para1)
{
   EXIT_FLAG_ENUM exit_flag = EXIT_RESUME;
   EXIT_FLAG_ENUM exit_tempflag = EXIT_RESUME;
   INT32U         msg_id;
   INT32S 		  ret;
   AVI_MESSAGE*	  avi_m_ptr;
   STATE_ENTRY_PARA *state_para;
   MJPEG_HEADER_PARSE_STRUCT *p_mjpeg_para;
   STAudioConfirm *aud_confirm;
   #ifdef _DPF_PROJECT   
   static INT8U  audio_volume_status = 0;
   #endif
   #if AVI_PLAY_MODE
   INT32S    	total_file = 0;
   INT8U		err;
   INT32S		avi_action_mode = 0;
   #endif

   #if VOLUME_CONTROL_STATE_MODE
   STATE_ENUM		state_next;
   STATE_ENTRY_PARA state_entry_para;
   INT8U volume;
   #endif

#if (defined SLIDESHOW_WITH_VIDEO)  && (SLIDESHOW_WITH_VIDEO == CUSTOM_ON)
	#if(SLIDEVIDEO_MUSIC_MUTE == CUSTOM_ON)
	audio_volume_status = 1;
	#endif
#endif

   #if	AVI_DBG_MODE
   INT32U	testtemp = 0;
   INT8U*	testprt = NULL;
   testprt = gp_malloc(0x280000);
   if(testprt != NULL)
   		gp_free((void *)testprt);
   else
   {
		DBG_PRINT ("avi allcote fail \r\n");
		while(1)
		{
			OSTimeDly(2);
			msgQFlush(ApQ);
		}
   }
   #endif
//#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)      
	#ifdef	_DPF_PROJECT    
	if(umi_music_play_onoff_get())
	    audio_volume_status = 0;
    else
    	audio_volume_status = 1;

	if(audio_volume_status == 1)
		audio_mute_ctrl_set(TRUE);
	else if(audio_volume_status == 0)
		audio_mute_ctrl_set(FALSE);
	#endif
//#endif     
   #if AVI_PLAY_MODE
    avi_play_status = C_CONTINUOUS_PLAY;
   #endif
   #ifdef _DPF_PROJECT
   Media_Task_Prio_Up();
   #endif
   state_para = (STATE_ENTRY_PARA*) para1;
   if (state_para->state_entry_mode==STATE_NORMAL_MODE) {  /*  NORMAL Entry */
       state_avi_init();
	   avi_m_ptr = (AVI_MESSAGE*)para1->state_entry_para;
	   pstAviShowControl->FileId = avi_m_ptr->filenum;
	   #if AVI_PLAY_MODE
	   avi_file_num = avi_m_ptr->filenum;
	   #endif
	   pstAviShowControl->pstring = avi_m_ptr->string_ptr;
	   #ifndef	_DPF_PROJECT
	   pstAviShowControl->MovFileHandle = avi_m_ptr->MovFileHandle;
	   pstAviShowControl->AudFileHandle = avi_m_ptr->AudFileHandle;
	   if((avi_m_ptr->Avi_Frame_buf1!=NULL)&&(avi_m_ptr->Avi_Frame_buf2!=NULL))
	   {
		   pstAviShowControl->stAviBuf[0].avibufptr = avi_m_ptr->Avi_Frame_buf1;
		   pstAviShowControl->stAviBuf[1].avibufptr = avi_m_ptr->Avi_Frame_buf2;
	   	   pstAviShowControl->avi_image_buf_malloc_flag = 1;
	   }
	   #endif
       pstAviShowControl->display_mode = avi_m_ptr->display_mode;
       pstAviShowControl->display_width = avi_m_ptr->display_width;
	   pstAviShowControl->display_height = avi_m_ptr->display_height;
       pstAviShowControl->display_position_x = avi_m_ptr->display_position_x;
	   pstAviShowControl->display_position_y = avi_m_ptr->display_position_y;

	if(avi_m_ptr->first_action == 0)
	  	  msgQSend(ApQ, MSG_AVI_PARSE_HEADER, NULL, 0, MSG_PRI_NORMAL);
    }
   else if (state_para->state_entry_mode==STATE_RESUME_MODE) {    /*=== RESUME MODE ===*/
	   #if VOLUME_CONTROL_STATE_MODE
	   #ifdef _DPF_PROJECT
       suspend_status_finished();
	   #endif
	   if(avi_volume_control_state == 2)
	   {
		  #ifdef _DPF_PROJECT
		  umi_osd_menu_enter();
  	      #endif

		   if(umi_previous_state_get() == STATE_STORAGE)
		   {
	   			avi_volume_control_state = 0;
				osd_avi_status = 0;
		   		msgQSend(ApQ,EVENT_KEY_BACK,NULL,0,MSG_PRI_NORMAL);
		   }
		   else
		   {
			   if(pstAviShowControl->AudioFlag != 0)
		       {
			   		osd_avi_status = 3;
			   		msgQSend(AudioTaskQ,MSG_AUD_RESUME,NULL,0,MSG_PRI_NORMAL);
			   }
			   else
			   {
					// no audio
					avi_volume_control_state = 0;
					osd_avi_status = 0;
					AviSetFrameTime(pstAviShowControl->Videofps);
					#ifdef _DPF_PROJECT
					ap_umi_only_ppu_go_enable();
			   		#endif
			   }
		   }
	   }
	   else if(avi_volume_control_state == 3)
	   {
			#ifdef _DPF_PROJECT
			umi_osd_menu_enter();
	  	    #endif

	   		osd_avi_status = 2;
	   		avi_volume_control_state = 0;
	   }
	   else
		   return;
	   #else
	   #ifdef _DPF_PROJECT
       suspend_status_finished();
       #endif
       return;
       #endif
    }
   else if (state_para->state_entry_mode==STATE_NULL_ENTRY_MODE) {
        state_avi_resume();
        state_avi_exit();
        return;
    }

#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_OFF)
	#ifdef _DPF_PROJECT
	if(audio_volume_status == 1)
		audio_mute_ctrl_set(TRUE);
#endif
#endif

//	#if VOLUME_CONTROL_STATE_MODE
	#ifdef _DPF_PROJECT
	audio_vol_set(umi_sound_volume_get());
	#endif
//	#endif

    while(1)
    {
		msgQReceive(ApQ, &msg_id, ApQ_para, AP_QUEUE_MSG_MAX_LEN);
      	#ifdef _DPF_PROJECT
		if(msg_id == EVENT_KEY_MEDIA_PLAY_PAUSE)
		{
			if((osd_avi_status == 1)||(osd_avi_status == 3))
				continue;
		}

       	if (umi_osd_menu_process_message(&msg_id, (KEY_EVENT_ST *) ApQ_para)) {
			continue;
		}
		
	#if (OSD_MENU_MODE == OSD_MENU_M3 || OSD_MENU_MODE == OSD_MENU_M2)
		if (((KEY_EVENT_ST*)ApQ_para)->key_source == SOURCE_AD_KEY) {
			umi_key_transfer(&msg_id, EVENT_KEY_MEDIA_PREV, EVENT_KEY_MEDIA_NEXT, EVENT_KEY_MEDIA_VOL_DOWN, EVENT_KEY_MEDIA_VOL_UP, EVENT_KEY_MEDIA_PLAY_PAUSE);
		}
	#endif
		#endif
	#if(SLIDESHOW_WITH_VIDEO == CUSTOM_ON)
		#if(SLIDEVIDEO_KEY_MODE == SLIDEVIDEO_KEY_NOACTION)
		// Key Event Filter......
		if(msg_id & EVENT_KEY_BASE){
			continue;
			}
		#elif(SLIDEVIDEO_KEY_MODE == SLIDEVIDEO_KEY_EXIT)
		if(msg_id & EVENT_KEY_BASE){
			// Exit to Main Menu......
			msg_id = EVENT_KEY_BACK;
			if(umi_previous_state_get()== STATE_SLIDE_SHOW){
				g_key_exit_flag =1;
				}
			}
		#else	//(SLIDEVIDEO_KEY_MODE == SLIDEVIDEO_KEY_NORMAL)
		//......
		#endif
	#endif

		switch (msg_id)
  	   	{
			case EVENT_APQ_ERR_MSG:
				aud_confirm = (STAudioConfirm *)ApQ_para;
				switch(aud_confirm->result_type)
				{
					case MSG_AUD_PLAY_RES:	// normal over
						if(aud_confirm->result == AUDIO_ERR_NONE)
						{
							avi_status = AVI_DECODE_OK;
						}
						else if(aud_confirm->result == AUDIO_ERR_DEC_FINISH)
						{
							pstAviShowControl->AivStatus = Avi_PlayFinish;
							timer_stop(AVI_TIMER);
							#ifdef _DPF_PROJECT
	        	    		ap_umi_only_ppu_go_disable();
	        	    		umi_Apq_Filter(MSG_AVI_TICKER);
	        	    		#endif
							avi_status = AVI_DECODE_STOP;
							msgQSend(ApQ, MSG_AVI_DECODE_END, NULL, 0, MSG_PRI_NORMAL);
						}
						else if(aud_confirm->result == AUDIO_ERR_DEC_FAIL)
						{
							pstAviShowControl->AivStatus = Avi_PlayFinish;
							timer_stop(AVI_TIMER);
							#ifdef _DPF_PROJECT
							ap_umi_only_ppu_go_disable();
	        	    		umi_Apq_Filter(MSG_AVI_TICKER);
	        	    		#endif
							avi_status = AVI_DECODE_STOP;
							msgQSend(ApQ, MSG_AVI_DECODE_END, NULL, 0, MSG_PRI_NORMAL);
						}
						break;

					case MSG_AUD_STOP_RES:
	        			pstAviShowControl->AivStatus = Avi_PlayFinish;
    		   			msgQSend(ApQ,MSG_AVI_DECODE_END,NULL,0,MSG_PRI_NORMAL);
						break;

					case MSG_AUD_PAUSE_RES:
						if(osd_avi_status == 1)
						{
							osd_avi_status++;	// pause ok
							#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)
							if(avi_volume_control_state == 1)
							{
								avi_volume_control_state = 2;
								image_task_remove_request(umi_current_state_get());
							}
							#endif
						}
						break;

					case MSG_AUD_RESUME_RES:
						if(osd_avi_status == 3)
						{
							#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)
							avi_volume_control_state = 0;
							#endif
							osd_avi_status = 0;	// pause ok
							AviSetFrameTime(pstAviShowControl->Videofps);
							#ifdef _DPF_PROJECT
							ap_umi_only_ppu_go_enable();
							#endif
						}
						break;
					default:
						break;
				}
				break;

			case MSG_AVI_PARSE_HEADER:
				#ifdef	_DPF_PROJECT
				ret = avi_start(pstAviShowControl->FileId);
				if(ret != 0)
				{
					ret = mov_start(pstAviShowControl->FileId);
					if(ret != 0)
						exit_flag = EXIT_BREAK;
					else
						avi_mov_switchflag = C_MOV_MODE_PLAY;
				}
				else
					avi_mov_switchflag = C_AVI_MODE_PLAY;
				#else
				{
					struct sfn_info fd_info;
					sfn_stat(pstAviShowControl->MovFileHandle, &fd_info);

					if(gp_strncmp((INT8S*)fd_info.f_extname, (INT8S*)"AVI",4) == 0)
					{
						ret = avi_start(pstAviShowControl->MovFileHandle, pstAviShowControl->AudFileHandle);
						avi_mov_switchflag = C_AVI_MODE_PLAY;
					}
					else if(gp_strncmp((INT8S*)fd_info.f_extname, (INT8S*)"MOV",4) == 0)
					{
						ret = mov_start(pstAviShowControl->MovFileHandle, pstAviShowControl->AudFileHandle);
						avi_mov_switchflag = C_MOV_MODE_PLAY;
					}
					else
						exit_flag = EXIT_BREAK;
				}
				#endif
				break;

			case MSG_MJPEG_PARSE_HEADER_REPLY:
				p_mjpeg_para = (MJPEG_HEADER_PARSE_STRUCT *)ApQ_para;
				if(p_mjpeg_para->status == 0)
				{
					osd_avi_status = 0;
					if(avi_mov_switchflag == C_AVI_MODE_PLAY)
						ret=avi_play_init();
					else if(avi_mov_switchflag == C_MOV_MODE_PLAY)
						ret=mov_play_init();

					if(ret == 0)
					{
					#ifdef	_DPF_PROJECT
						msgQSend(ApQ, MSG_AVI_PLAY, NULL, 0, MSG_PRI_NORMAL);
					#else
						OSQPost(avi_status_q, (void *)0);
					#endif
					}
					else
					{
					#ifndef _DPF_PROJECT
						//added statusQ
						OSQPost(avi_status_q, (void *)1);
					#endif
					msgQSend(ApQ, MSG_IMAGE_STOP_REPLY, NULL, 0, MSG_PRI_NORMAL);
					}
				}
				else
				{
				#ifndef _DPF_PROJECT
					//added statusQ
					OSQPost(avi_status_q, (void *)1);
				#endif
					msgQSend(ApQ, MSG_IMAGE_STOP_REPLY, NULL, 0, MSG_PRI_NORMAL);
				}
				break;
			case MSG_AVI_DISPLAYSIZE_CHANGE:
				test();
				break;
			case  MSG_AVI_PLAY:
				pstAviShowControl->AivStatus = Avi_Playing;
				if(avi_mov_switchflag == C_AVI_MODE_PLAY)
					avi_play();
				else if(avi_mov_switchflag == C_MOV_MODE_PLAY)
					mov_play();
				if(pstAviShowControl->AudioFlag == 0)
					avi_status = AVI_DECODE_OK;
				#if AVI_PLAY_MODE
				avi_action_mode = 0;
				#endif
				break;

			#if 0
			case  MSG_AVI_AUDIO_END:
				pstAviShowControl->AivStatus |= Avi_AudioFileEnd;
				//if(dac_dma_status_get() == 1)
				if(*avi_state_mon == 1)
				{
					if(pstAviShowControl->AivStatus & Avi_VideoFileEnd)
					{
						msgQSend(ApQ, MSG_AVI_AUDIO_END, NULL, 0, MSG_PRI_NORMAL);
					}
				}
				else
				{
					pstAviShowControl->AivStatus = Avi_PlayFinish;
					timer_stop(AVI_TIMER);
					ap_umi_only_ppu_go_disable();
					msgQSend(ApQ, MSG_AVI_DECODE_END, NULL, 0, MSG_PRI_NORMAL);
				}
				break;
			#endif

			case  MSG_AVI_MJPEG_END:
				pstAviShowControl->AivStatus |= Avi_VideoFileEnd;
				if(pstAviShowControl->AudioFlag == 0)
					msgQSend(ApQ, MSG_AVI_DECODE_END, NULL, 0, MSG_PRI_NORMAL);
				#ifndef	_DPF_PROJECT
				video_decode_end();		//play end, Roy ADD
				#endif
				break;

			case  MSG_AVI_DECODE_END:
				#ifdef _DPF_PROJECT
				timer_stop(AVI_TIMER);
				ap_umi_only_ppu_go_disable();
				umi_Apq_Filter(MSG_AVI_TICKER);
				image_task_remove_request(umi_current_state_get());
				ap_umi_only_ppu_go_disable();
				//OSTimeDly(1);
				//msgQSend(ApQ, MSG_AVI_DECODE_END, NULL, 0, MSG_PRI_NORMAL);
				#else
				if(avi_mov_switchflag == C_AVI_MODE_PLAY)
				{
					avi_memory_free();
					avi_idx1_tabel_free();
				}
				else if(avi_mov_switchflag == C_MOV_MODE_PLAY)
					mov_memory_free();
				avi_mov_switchflag = C_AVI_MOV_DEFAULT;
				if(exit_tempflag !=EXIT_RESUME)
					exit_flag = exit_tempflag;
				else
					exit_flag = EXIT_BREAK;
				#endif
				break;

			case  MSG_AVI_TICKER:
				if((avi_status == AVI_DECODE_OK)&&(!(pstAviShowControl->AivStatus&Avi_VideoFileEnd)))
				{
					avi_frame_calculate();
				}

				#if AVI_DBG_MODE
				if((osd_timer_id == 0xff)&&(testtemp > 3))
				{
					msg_id = EVENT_KEY_MEDIA_VOL_UP;
					umi_osd_menu_process_message(&msg_id, (KEY_EVENT_ST *) ApQ_para);
					testtemp = 0;
				}
				else if(osd_timer_id == 0xff)
				{
					testtemp++;
				}
				#endif
				break;

			case  MSG_MJPEG_DECODE_OK:

				break;

			case  MSG_MJPEG_DECODE_FAIL:
				if(avi_status == AVI_DECODE_OK)
				{
					avi_status = AVI_DECODE_ERROR;
					timer_stop(AVI_TIMER);
					#ifdef _DPF_PROJECT
					ap_umi_only_ppu_go_disable();
					#endif
					msgQSend(ApQ,MSG_AVI_STOP,NULL,0,MSG_PRI_URGENT);
				}
				break;

        	case  MSG_AVI_STOP:
        	    #ifdef _DPF_PROJECT
        	    ap_umi_only_ppu_go_disable();
        	    umi_Apq_Filter(MSG_AVI_TICKER);
        	    #endif
				if(avi_status == AVI_DECODE_OK)
					avi_status = AVI_DECODE_STOP;
        		pstAviShowControl->AivStatus |= Avi_VideoFileEnd;
        		if(pstAviShowControl->AudioFlag != 0)
	        	{
	        		msgQSend(AudioTaskQ,MSG_AUD_STOP,NULL,0,MSG_PRI_NORMAL);
        		}
        		else
	    		{
	        		pstAviShowControl->AivStatus = Avi_PlayFinish;
    	    		msgQSend(ApQ,MSG_AVI_DECODE_END,NULL,0,MSG_PRI_NORMAL);
        		}
        		break;

        	#if 0
        	case  MSG_AVI_PAUSE:
       			if(pstAviShowControl->AudioFlag != 0)
        		{
	        		if(*avi_state_mon == AUDIO_PLAYING)
		        		msgQSend(AudioTaskQ,MSG_AUD_PAUSE,NULL,0,MSG_PRI_NORMAL);
        		}
        		break;


        	case  MSG_AVI_RESUME:
        	    #ifdef _DPF_PROJECT
        	    umi_Apq_Filter(MSG_AVI_TICKER);
        	    if(audio_volume_status == 0)
					audio_mute_ctrl_set(FALSE);
				#endif
				AviSetFrameTime(pstAviShowControl->Videofps);
				#ifdef _DPF_PROJECT
				ap_umi_only_ppu_go_enable();
        		#endif
        		if(pstAviShowControl->AudioFlag != 0)
        		{
	        		if(*avi_state_mon == AUDIO_PLAY_PAUSE)
		        		msgQSend(AudioTaskQ,MSG_AUD_RESUME,NULL,0,MSG_PRI_NORMAL);
				}
				osd_avi_status = 0;
        		break;
			#endif
#if CONTROL_SLIDESHOW_EFFECT_AND_MUSIC == SYNCHRONOUS_MODE
            case EVENT_KEY_MEDIA_STOP:
#endif
            case EVENT_KEY_BACK:
            	#ifdef _DPF_PROJECT
	            audio_mute_ctrl_set(TRUE);
				#endif
				timer_stop(AVI_TIMER);
				#ifdef _DPF_PROJECT
				ap_umi_only_ppu_go_disable();
				#endif
				msgQSend(ApQ, MSG_AVI_STOP, NULL, 0, MSG_PRI_URGENT);
				#if	AVI_PLAY_MODE
				avi_play_status = C_STOP_PLAYING;
				#endif
             	break;

            case EVENT_KEY_MEDIA_PAUSE:
#if SUPPORT_UNITED_UI_MODE == 0
            	if(osd_avi_status == 0)
            	{
	       			timer_stop(AVI_TIMER);
	       			#ifdef _DPF_PROJECT
	       			ap_umi_only_ppu_go_disable();
					audio_mute_ctrl_set(TRUE);
					#endif
            		msgQSend(ApQ, MSG_AVI_PAUSE, NULL, 0, MSG_PRI_URGENT);
           			osd_avi_status++;
           		  #ifdef _DPF_PROJECT
           			umi_osd_menu_assign_image(EVENT_KEY_MEDIA_PLAY_PAUSE, 1);	// Show pause image
           		  #endif
            	}
#endif
            	break;

            case EVENT_KEY_MEDIA_PLAY:
#if SUPPORT_UNITED_UI_MODE == 0
            	if(osd_avi_status == 2)
            	{
            		msgQSend(ApQ, MSG_AVI_RESUME, NULL, 0, MSG_PRI_URGENT);
           			osd_avi_status++;
           		  #ifdef _DPF_PROJECT
           			umi_osd_menu_assign_image(EVENT_KEY_MEDIA_PLAY_PAUSE, 0);	// Show play image
           		  #endif
            	}
#endif
            	break;

			#ifdef _DPF_PROJECT
			#if	AVI_PLAY_MODE
			case EVENT_KEY_MEDIA_PREV:
			case EVENT_KEY_UP:
				if(avi_action_mode == 0)
				{
					audio_mute_ctrl_set(TRUE);
					timer_stop(AVI_TIMER);
					ap_umi_only_ppu_go_disable();
					msgQSend(ApQ, MSG_AVI_STOP, NULL, 0, MSG_PRI_URGENT);
					avi_action_mode = -1;
				}
				break;

			case EVENT_KEY_MEDIA_NEXT:
			case EVENT_KEY_DOWN:
				if(avi_action_mode == 0)
				{
					audio_mute_ctrl_set(TRUE);
					timer_stop(AVI_TIMER);
					ap_umi_only_ppu_go_disable();
					msgQSend(ApQ, MSG_AVI_STOP, NULL, 0, MSG_PRI_URGENT);
					avi_action_mode = 1;
				}
				break;

			#endif
			#endif

		 	#ifdef	_DPF_PROJECT
			case EVENT_KEY_MEDIA_PLAY_PAUSE:
				if(avi_status == AVI_DECODE_OK)
				{
					if(osd_avi_status == 0) // playing
					{
		       			timer_stop(AVI_TIMER);
		       			ap_umi_only_ppu_go_disable();
						audio_mute_ctrl_set(TRUE);
						osd_avi_status++;	// pause holding
						if(pstAviShowControl->AudioFlag != 0)
        				{
		        			msgQSend(AudioTaskQ,MSG_AUD_PAUSE,NULL,0,MSG_PRI_NORMAL);
        				}
						else
						{
							// no audio
							osd_avi_status++;
						}
#if SUPPORT_UNITED_UI_MODE == 0
						umi_osd_menu_assign_image(EVENT_KEY_MEDIA_PLAY_PAUSE, 1);	// Show pause image
#endif
						pop_up_msg_start((void *)STR_VEDIO_PAUSE, MSG_NORMAL);
					}
					else if(osd_avi_status == 2) // pause OK
					{
						#ifdef _DPF_PROJECT
        	    		umi_Apq_Filter(MSG_AVI_TICKER);
        	    		if(audio_volume_status == 0)
							audio_mute_ctrl_set(FALSE);
						#endif
						osd_avi_status++;	// resume holding
        				if(pstAviShowControl->AudioFlag != 0)
        				{
	        				msgQSend(AudioTaskQ,MSG_AUD_RESUME,NULL,0,MSG_PRI_NORMAL);
						}
						else
						{
							// no audio
							osd_avi_status = 0;
							AviSetFrameTime(pstAviShowControl->Videofps);
							#ifdef _DPF_PROJECT
							ap_umi_only_ppu_go_enable();
							#endif
						}
#if SUPPORT_UNITED_UI_MODE == 0
						umi_osd_menu_assign_image(EVENT_KEY_MEDIA_PLAY_PAUSE, 0);	// Show play image
#endif
						pop_up_msg_start((void *)STR_VEDIO_PLAY, MSG_NORMAL);
					}
				}
				break;
		  #else
            case EVENT_KEY_SLIDESHOW_PLAY_PAUSE:
            	if(osd_avi_status == 0)
            	{
            		msgQSend(ApQ, MSG_AVI_PAUSE, NULL, 0, MSG_PRI_URGENT);
            		osd_avi_status = 1;		//pause
            	}
            	else
            	{
            		msgQSend(ApQ, MSG_AVI_RESUME, NULL, 0, MSG_PRI_URGENT);
            		osd_avi_status = 0;	    //resume
            	}
	           	break;
          #endif

            case MSG_START_AVI_STOP://EVENT_KEY_BACK:
	            timer_stop(AVI_TIMER);
	            #ifdef _DPF_PROJECT
	            ap_umi_only_ppu_go_disable();
            	#endif
            	msgQSend(ApQ, MSG_AVI_STOP, NULL, 0, MSG_PRI_URGENT);
            	break;

			case EVENT_KEY_MEDIA_FF:
			case EVENT_KEY_MEDIA_BF:
				//first step: stop the audio task.
				timer_stop(AVI_TIMER);
				#ifdef _DPF_PROJECT
				ap_umi_only_ppu_go_disable();
				#endif
				if(pstAviShowControl->AudioFlag != 0)
	        	{
	        		msgQSend(AudioTaskQ,MSG_AUD_STOP,NULL,0,MSG_PRI_NORMAL);
					while(*avi_state_mon == 1);
				}

				if(tk_avi_wave_format_type_get()!=0x0055)
					ret = avi_settime_frame_pos_get(*(INT32U *)ApQ_para);
				else
					ret = avi_mp3_settime_frame_pos_get(*(INT32U *)ApQ_para);
				if(ret != -1)
				{
					pstAviShowControl->TimeTickFrame = (INT32U)ret;
					pstAviShowControl->RealtimeFrame = pstAviShowControl->TimeTickFrame;
					pstAviShowControl->CurrentTime = *(INT32U *)ApQ_para;
					avi_jumpframe_show();
					avi_audio_buf_fill_again();
				}
				else
				{
					pstAviShowControl->TimeTickFrame = (INT32U)g_avi_synchroframe;
					pstAviShowControl->RealtimeFrame = pstAviShowControl->TimeTickFrame;
				}
			#ifndef	_DPF_PROJECT
				//added statusQ
				OSQPost(avi_status_q, (void *)0);
			#endif
				break;
			#ifdef	_DPF_PROJECT
			#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)
			case EVENT_KEY_MEDIA_VOL_DOWN:
			case EVENT_KEY_MEDIA_VOL_UP:
#if SUPPORT_UNITED_UI_MODE == 0
			case EVENT_KEY_LEFT:
			case EVENT_KEY_RIGHT:
#endif
				#ifdef _DPF_PROJECT
				umi_music_play_onoff_set(1);
				audio_bg_mute_ctrl_set(FALSE);
				audio_mute_ctrl_set(FALSE);
	        	volume = umi_sound_volume_get();
        		if (msg_id == EVENT_KEY_MEDIA_VOL_UP)
        		{
	    			if(volume < 16)
	    			{
						volume++;
					}
   				}
   				else if (msg_id == EVENT_KEY_MEDIA_VOL_DOWN)
   				{
   					if(volume > 0)
   					{
						volume--;
					}
   				}
	        	umi_sound_volume_set(volume);
				audio_vol_set(umi_sound_volume_get());
				#endif
				if(avi_status == AVI_DECODE_OK)
 				{
	 				if(osd_avi_status == 0) // playing
					{
		    			avi_volume_control_state = 1;
			       		timer_stop(AVI_TIMER);
			       		#ifdef _DPF_PROJECT
			       		ap_umi_only_ppu_go_disable();
						audio_mute_ctrl_set(TRUE);
						umi_Apq_Filter(MSG_AVI_TICKER);
						#endif
						osd_avi_status++;	// pause holding
						if(pstAviShowControl->AudioFlag != 0)
	        			{
			        		msgQSend(AudioTaskQ,MSG_AUD_PAUSE,NULL,0,MSG_PRI_NORMAL);
	        			}
						else
						{
							// no audio
							osd_avi_status++;
						}
					}
					else if(osd_avi_status == 2)
					{
						// need suspend
						avi_volume_control_state = 3;
						image_task_remove_request(umi_current_state_get());
					}
				}
            	break;
	        case EVENT_KEY_MEDIA_SOUND_ON_OFF:
	        	#ifdef _DPF_PROJECT
           		if(audio_volume_status == 0)
           		{
					audio_bg_mute_ctrl_set(TRUE);
					audio_mute_ctrl_set(TRUE);
       				audio_volume_status = 1;
//					umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 1);
					umi_music_play_onoff_set(0);
       			}
       			else
       			{
					audio_bg_mute_ctrl_set(FALSE);
					audio_mute_ctrl_set(FALSE);
					audio_volume_status = 0;
//					umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 0);
					umi_music_play_onoff_set(1);
				}
				#endif

				if(avi_status == AVI_DECODE_OK)
 				{
	 				if(osd_avi_status == 0) // playing
					{
		    			avi_volume_control_state = 1;
			       		timer_stop(AVI_TIMER);
			       		#ifdef _DPF_PROJECT
			       		ap_umi_only_ppu_go_disable();
			       		audio_mute_ctrl_set(TRUE);
						umi_Apq_Filter(MSG_AVI_TICKER);
						#endif
						osd_avi_status++;	// pause holding
						if(pstAviShowControl->AudioFlag != 0)
	        			{
			        		msgQSend(AudioTaskQ,MSG_AUD_PAUSE,NULL,0,MSG_PRI_NORMAL);
	        			}
						else
						{
							// no audio
							osd_avi_status++;
						}
					}
					else if(osd_avi_status == 2)
					{
						// need suspend
						avi_volume_control_state = 3;
						image_task_remove_request(umi_current_state_get());
					}
				}
            	break;
			#else
			case EVENT_KEY_MEDIA_VOL_DOWN:
			case EVENT_KEY_LEFT:
				#ifdef _DPF_PROJECT
				audio_mute_ctrl_set(FALSE);
				audio_volume_status = 0;

				audio_vol_dec_set();
				pop_up_msg_start(NULL, MSG_VOLUME);
				umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 0);
				#endif
				break;

			case EVENT_KEY_MEDIA_VOL_UP:
			case EVENT_KEY_RIGHT:
				#ifdef _DPF_PROJECT
				audio_mute_ctrl_set(FALSE);
				audio_volume_status = 0;

           		audio_vol_inc_set();
           		pop_up_msg_start(NULL, MSG_VOLUME);
           		umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 0);
            	#endif
            	break;

	        case EVENT_KEY_MEDIA_SOUND_ON_OFF:
           		#ifdef _DPF_PROJECT
           		if(audio_volume_status == 0)
           		{
					audio_mute_ctrl_set(TRUE);
					audio_bg_mute_ctrl_set(TRUE);
       				audio_volume_status = 1;
					umi_music_play_onoff_set(0);					
       				pop_up_msg_start((void *)STR_MUTE_ON, MSG_NORMAL);
					umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 1);
       			}
       			else
       			{
					audio_mute_ctrl_set(FALSE);
					audio_bg_mute_ctrl_set(FALSE);
					audio_volume_status = 0;
					umi_music_play_onoff_set(1);					
					pop_up_msg_start((void *)STR_MUTE_OFF, MSG_NORMAL);
					umi_osd_menu_assign_image(EVENT_KEY_MEDIA_SOUND_ON_OFF, 0);
				}
                #endif
                break;
			#endif

			case MSG_IMAGE_STOP_REPLY:

				#if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)

				if((avi_volume_control_state == 2)||(avi_volume_control_state == 3))
				{
					state_entry_para.state_entry_mode = STATE_NORMAL_MODE;
					state_next = STATE_VOLUME_CONTROL;
					exit_flag = EXIT_SUSPEND;
					umi_next_state_reg(state_next, &state_entry_para, sizeof(STATE_ENTRY_PARA),exit_flag);
					break;
				}

				#endif

				if(avi_mov_switchflag == C_AVI_MODE_PLAY)
				{
					avi_memory_free();
					avi_idx1_tabel_free();
				}
				else if(avi_mov_switchflag == C_MOV_MODE_PLAY)
					mov_memory_free();
				avi_mov_switchflag = C_AVI_MOV_DEFAULT;

			    //gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
			    //issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);

				#if	AVI_PLAY_MODE
				if(avi_play_status == C_CONTINUOUS_PLAY)
				{
					#if (FS_SCAN_STYLE == FS_SCAN_COMBINATION)
					total_file = storage_file_nums_get(curr_storage_id,MEDIA_PHOTO_AVI);
					while(1)
					{
						if(curr_storage_id == 0xff)
							break;

						if(avi_action_mode == -1)
						{
							if(avi_file_num == 0)
								avi_file_num = 	total_file - 1;
							else
								avi_file_num --;
						}
						else
						{
							avi_file_num++;
							if(avi_file_num >= total_file)
							{
								avi_file_num = 0;
							}
						}

						ret = storage_finfo_get(curr_storage_id, MEDIA_PHOTO_AVI, avi_file_num, &photo_finfo);
						if (ret != STATUS_OK)
						{
							if(exit_tempflag !=EXIT_RESUME)
								exit_flag = exit_tempflag;
							else
								exit_flag = EXIT_BREAK;
						}

						//close(photo_finfo.fd);

						if(!(check_avi_file(photo_finfo.f_extname)))
						{
							avi_action_mode = 0x5a; //ready paurse avi head
							break;
						}

					}
					#else
					total_file = storage_file_nums_get(curr_storage_id, MEDIA_VIDEO_ONLY);
					while(1)
					{
						if(curr_storage_id == 0xff)
							break;

						if(avi_action_mode == -1)
						{
							if(avi_file_num == 0)
								avi_file_num = 	total_file - 1;
							else
								avi_file_num --;
						}
						else
						{
							avi_file_num++;
							if(avi_file_num >= total_file)
							{
								avi_file_num = 0;
							}
						}

						ret = storage_finfo_get(curr_storage_id, MEDIA_VIDEO_ONLY, avi_file_num, &photo_finfo);
						if (ret != STATUS_OK)
						{
							if(exit_tempflag !=EXIT_RESUME)
								exit_flag = exit_tempflag;
							else
								exit_flag = EXIT_BREAK;
						}
						//close(photo_finfo.fd);
						avi_action_mode = 0x5a; //ready paurse avi head
						break;
					}
					#endif

					if(curr_storage_id == 0xff)
					{
						if(exit_tempflag !=EXIT_RESUME)
							exit_flag = exit_tempflag;
						else
							exit_flag = EXIT_BREAK;
					}
					else
					{
						POS_STRUCT	pos;
						msgQFlush(ApQ);
						OSQDel(audio_fs_result_q,OS_DEL_ALWAYS,&err);
						state_avi_reinit();
						#ifdef _DPF_PROJECT
						if(audio_volume_status == 1)
							audio_mute_ctrl_set(TRUE);
						else
							audio_mute_ctrl_set(FALSE);
						#endif
						pstAviShowControl->FileId = avi_file_num;

						gplib_ppu_sprite_visible_position_get(turnkey_ppu_register_set, &pos.x0, &pos.x2, &pos.y0, &pos.y2);
						pstAviShowControl->display_mode = AVI_FULL_SCREEN_SHOW_ON;
						pstAviShowControl->display_width = pos.x2 - pos.x0 + 1;;
						pstAviShowControl->display_height = pos.y0 - pos.y2 + 1;

						/*
						pstAviShowControl->display_mode = avi_m_ptr->display_mode;
					    pstAviShowControl->display_width = avi_m_ptr->display_width;
					    pstAviShowControl->display_height = avi_m_ptr->display_height;
				        pstAviShowControl->display_position_x = avi_m_ptr->display_position_x;
					    pstAviShowControl->display_position_y = avi_m_ptr->display_position_y;
					    */
						msgQSend(ApQ, MSG_AVI_PARSE_HEADER, NULL, 0, MSG_PRI_NORMAL);
						exit_tempflag = EXIT_RESUME;
						exit_flag = EXIT_RESUME;

					  #ifdef _DPF_PROJECT
           				umi_osd_menu_assign_image(EVENT_KEY_MEDIA_PLAY_PAUSE, 0);	// Show play image
           		  	  #endif
					}
				}
				else
				{
					if(exit_tempflag !=EXIT_RESUME)
						exit_flag = exit_tempflag;
					else
						exit_flag = EXIT_BREAK;
				}
				#else
				if(exit_tempflag !=EXIT_RESUME)
					exit_flag = exit_tempflag;
				else
					exit_flag = EXIT_BREAK;

				#endif
				break;

			case EVENT_STORAGE_LOADING_START:
				timer_stop(AVI_TIMER);
        	    #ifdef _DPF_PROJECT
        	    ap_umi_only_ppu_go_disable();
        	    umi_Apq_Filter(MSG_AVI_TICKER);
        	    audio_mute_ctrl_set(TRUE);
        	    #endif
				pstAviShowControl->AivStatus |= Avi_VideoFileEnd;
			default: /* default procedure */
       			if(exit_tempflag == EXIT_RESUME)
       			{
       				exit_tempflag = apq_sys_msg_handler(msg_id);
	       	 		if(exit_tempflag != EXIT_RESUME)
	       	 		{
						if(avi_status == AVI_DECODE_OK)
        				{
							#if	AVI_PLAY_MODE
							avi_play_status = C_STOP_PLAYING;
							#endif
							#ifdef _DPF_PROJECT
							audio_mute_ctrl_set(TRUE);
							#endif
							timer_stop(AVI_TIMER);
							#ifdef	_DPF_PROJECT
							ap_umi_only_ppu_go_disable();
							#endif
							msgQSend(ApQ,MSG_AVI_STOP,NULL,0,MSG_PRI_URGENT);
	                  	}
	       	 		}

       	 		}
       	 		else
       	 		{
					if(avi_status == AVI_DECODE_OK)
        			{
        				#ifdef _DPF_PROJECT
						audio_mute_ctrl_set(TRUE);
						#endif
						timer_stop(AVI_TIMER);
						#ifdef _DPF_PROJECT
						ap_umi_only_ppu_go_disable();
        				#endif
        				msgQSend(ApQ,MSG_AVI_STOP,NULL,0,MSG_PRI_URGENT);
	                }
       	 			apq_sys_msg_handler(msg_id);
       	 		}

           	 	break;
           	 #else
           	 default:
           	 	break;
           	 #endif
       }

	   if(exit_flag != EXIT_RESUME)
		  break;
    }
	#ifdef _DPF_PROJECT
	audio_mute_ctrl_set(FALSE);
	#if MUTE_FUNCTION_MODE == MEDIA_MUTE_OFF
	audio_volume_status = 0;
	#endif
	#endif
	#if	AVI_PLAY_MODE
	#ifdef _DPF_PROJECT
	#if (FS_SCAN_STYLE == FS_SCAN_COMBINATION)
		umi_current_photo_index_set(avi_file_num);
	#else
		umi_current_video_index_set(avi_file_num);
	#endif
	#endif
	#endif
    if ((exit_flag == EXIT_BREAK) || (exit_flag == EXIT_TO_SHORTCUT)) {
    	state_avi_exit();
	  #ifdef	_DPF_PROJECT
		umi_osd_menu_exit();
	  #endif
    } else {
	  #if (VOLUME_CONTROL_STATE_MODE == VOLUME_CONTROL_STATE_ON)
		if((avi_volume_control_state == 2)||(avi_volume_control_state == 3))
		{
			suspend_state_stack_push(umi_current_state_get());
		}
		else
 	  #endif
		{
	  		state_avi_suspend(exit_flag);
	  	}

	  #ifdef	_DPF_PROJECT
	   #if CARD_PLUG_MODE == CARD_PLUG_M0	// Pop-up window
		umi_osd_menu_exit();
	   #endif
	  #endif
	}
#if(SLIDESHOW_WITH_VIDEO == CUSTOM_ON)&&(SLIDEVIDEO_PLAY_MODE == SLIDEVIDEO_PLAY_ONCE)
	if(avi_m_ptr->filenum == (slideshow_total_file -1)){
		 g_key_exit_flag =1;
	}
#endif
   #ifdef _DPF_PROJECT
   Media_Task_Prio_Down();
   #endif
}

void state_avi_exit(void)
{
	INT8U err;
	#ifdef	_DPF_PROJECT
	INT32U i;

    /* allow umi to access statck state */
    suspend_status_finished();
    #endif

    #ifdef	_DPF_PROJECT
    gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
    issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
	while(R_PPU_TEXT1_CTRL & 0x8)
	{
	    gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
	    issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
		OSTimeDly(2);
	}
	for(i=0;i<Avi_Buffer_TotalNum;i++)
	{
		gp_free((void *)pstAviShowControl->stAviBuf[i].avibufptr);
	}   
   
    gp_free((void *)pstAviShowControl);	
	AviPPUUnInitial();
	ap_umi_only_ppu_go_disable();
    //audio_vol_set(8);
	#endif
	OSQDel(audio_fs_result_q,OS_DEL_ALWAYS,&err);

}

void state_avi_resume(void)
{
	/*
	state_avi_init();
	pstAviShowControl->FileId = save_file_num;
	msgQSend(ApQ, MSG_AVI_PARSE_HEADER, NULL, 0, MSG_PRI_NORMAL);
	*/
	if(avi_mov_switchflag == C_AVI_MODE_PLAY)
	{
		avi_memory_free();
		avi_idx1_tabel_free();
	}
	else if(avi_mov_switchflag == C_MOV_MODE_PLAY)
		mov_memory_free();
	avi_mov_switchflag = C_AVI_MOV_DEFAULT;
    #ifdef	_DPF_PROJECT
    gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
	issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
	#endif
}

void avi_enter_menu(void)
{
}

void state_avi_suspend(EXIT_FLAG_ENUM exit_flag)
{
	INT8U err,i;
    /* allow umi to access statck state */
    //gp_free((void *)pstAviShowControl);
    #ifdef	_DPF_PROJECT
    gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
    issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
	while(R_PPU_TEXT1_CTRL & 0x8)
	{
	    gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);
	    issue_ppu_update_command(C_UMI_TASK_START_PPU_WAIT_DONE);
		OSTimeDly(2);
	}
    suspend_state_stack_push(umi_current_state_get());
	#endif
	for(i=0;i<Avi_Buffer_TotalNum;i++)
	{
		gp_free((void *)pstAviShowControl->stAviBuf[i].avibufptr);
	}   
   
    gp_free((void *)pstAviShowControl);	
	AviPPUUnInitial();
	OSQDel(audio_fs_result_q,OS_DEL_ALWAYS,&err);
	#ifdef _DPF_PROJECT
	ap_umi_only_ppu_go_disable();
	#endif
}
