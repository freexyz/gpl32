#include "task_video_decoder.h"

#define C_VID_DEC_STATE_STACK_SIZE		512
#define C_VID_DEC_QUEUE_MAX_LEN    		5

#if _PROJ_TYPE != _PROJ_TURNKEY
#define TSK_PRI_VID_PARSER		VIDEO_PASER_PRIORITY
#define TSK_PRI_VID_VID_STATE	VID_DEC_STATE_PRIORITY
#define TSK_PRI_VID_VID_DEC		VIDEO_DECODE_PRIORITY
#define	TSK_PRI_VID_AUD_DEC		AUDIO_DECODE_PRIORITY
#endif
/* OS stack */
INT32U	vid_dec_state_stack[C_VID_DEC_STATE_STACK_SIZE];

/* global varaible */
OS_EVENT *vid_dec_state_q;
OS_EVENT *vid_dec_state_ack_m;
void *vid_dec_state_q_buffer[C_VID_DEC_QUEUE_MAX_LEN];
video_decode_para_struct vid_dec_para;
video_decode_para_struct *p_vid_dec_para;

const GP_BITMAPINFOHEADER *p_bitmap_info;
const GP_WAVEFORMATEX *p_wave_info;
long  g_bitmap_info_len, g_wave_info_len;

INT32S vid_dec_entry(void)
{
	INT32S nRet;

	nRet = video_decode_state_task_create(TSK_PRI_VID_VID_STATE);
	if(nRet < 0) RETURN(STATUS_FAIL);
	nRet = video_decode_task_create(TSK_PRI_VID_VID_DEC);
	if(nRet < 0) RETURN(STATUS_FAIL);
	nRet = audio_decode_task_create(TSK_PRI_VID_AUD_DEC);
	if(nRet < 0) RETURN(STATUS_FAIL);

	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S vid_dec_exit(void)
{
	INT32S nRet;

	nRet = video_decode_state_task_del(TSK_PRI_VID_VID_STATE);
	if(nRet < 0) RETURN(STATUS_FAIL);
	nRet = video_decode_task_del(TSK_PRI_VID_VID_DEC);
	if(nRet < 0) RETURN(STATUS_FAIL);
	nRet = audio_decode_task_del(TSK_PRI_VID_AUD_DEC);
	if(nRet < 0) RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S vid_dec_parser_start(INT16S fd, INT32S FileTpye, INT64U FileSize)
{
	INT8U  err;
	INT32S nRet, msg;

	if(fd < 0) RETURN(STATUS_FAIL);
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);	
		
	nRet = STATUS_OK;	
	p_vid_dec_para->file_handle = fd;
	p_vid_dec_para->FileType = FileTpye;
	p_vid_dec_para->FileSize = FileSize;
	if(!(vid_dec_get_status() & C_VIDEO_DECODE_PARSER))
    {	
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_PARSER_HEADER_START, vid_dec_state_ack_m, 10000, msg, err); //wait 10 sec
		vid_dec_clear_status(C_VIDEO_DECODE_ALL);
		vid_dec_set_status(C_VIDEO_DECODE_PARSER);	
	}	
Return:	
	return nRet;
} 

INT32S vid_dec_parser_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);	
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER_NTH)
		RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
	if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
    {
    	vid_dec_clear_status(C_VIDEO_DECODE_PARSER);	
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_PARSER_HEADER_STOP, vid_dec_state_ack_m, 10000, msg, err);
	}
Return:	
	return nRet;
}

INT32S vid_dec_start(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER_NTH)
		RETURN(STATUS_FAIL);
		
	nRet = STATUS_OK;
	if(!(vid_dec_get_status() & C_VIDEO_DECODE_PLAY)) 
    {	
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_START, vid_dec_state_ack_m, 5000, msg, err);
		vid_dec_set_status(C_VIDEO_DECODE_PLAY);	
	}
Return:	
	return nRet;
}

INT32S vid_dec_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
		
	nRet = STATUS_OK;
	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAY) 
	{
		vid_dec_clear_status(C_VIDEO_DECODE_PLAY);	
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_STOP, vid_dec_state_ack_m, 5000, msg, err);
	}
Return:
	return nRet;
}

INT32S vid_dec_pause(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
	
	if((vid_dec_get_status() & C_VIDEO_DECODE_PLAY) == 0)
		RETURN(STATUS_FAIL);
		
	nRet = STATUS_OK;
	if(!(vid_dec_get_status() & C_VIDEO_DECODE_PAUSE))
    {
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_PAUSE, vid_dec_state_ack_m, 5000, msg, err);
		vid_dec_set_status(C_VIDEO_DECODE_PAUSE);		
	}
Return:	
	return nRet;
}

INT32S vid_dec_resume(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
	
	if((vid_dec_get_status() & C_VIDEO_DECODE_PLAY) == 0)
		RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
	if(vid_dec_get_status() & C_VIDEO_DECODE_PAUSE)
    {
    	vid_dec_clear_status(C_VIDEO_DECODE_PAUSE);	
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_RESUME, vid_dec_state_ack_m, 5000, msg, err);
	}
Return:	
	return nRet;
}

INT32S vid_dec_end_callback(void)
{
	INT32S nRet;
	
	nRet = STATUS_OK;
	if(vid_dec_get_video_flag() && (vid_dec_get_status() & C_VIDEO_DECODE_PLAYING) == 0)
	{
		vid_dec_set_video_flag(FALSE);
		vid_dec_stop_timer();
	}
	
	if(vid_dec_get_audio_flag() && (vid_dec_get_status() & C_AUDIO_DECODE_PLAYING) == 0)
	{
		vid_dec_set_audio_flag(FALSE);
	}
	
	if((vid_dec_get_status() & (C_AUDIO_DECODE_PLAYING|C_VIDEO_DECODE_PLAYING)) == 0)
	{	
		INT8U  err;
		INT32S msg;
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_FORCE_STOP, vid_dec_state_ack_m, 5000, msg, err);
	}
Return:
	return nRet;
}

INT32S vid_dec_get_total_samples(void)
{
	return p_vid_dec_para->video_total_sample;
}

INT32S vid_dec_get_total_time(void)
{
	return p_vid_dec_para->video_total_time;
}

INT32S vid_dec_get_current_time(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	if(p_vid_dec_para->video_cur_time >= p_vid_dec_para->video_total_time)
		RETURN(nRet);
		
	if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
    {
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_GET_CUR_TIME, vid_dec_state_ack_m, 5000, msg, err);
	}
Return:	
	if(nRet < 0) 
		return p_vid_dec_para->video_total_time;
	
	return p_vid_dec_para->video_cur_time;
}

INT32S vid_dec_set_play_time(INT32U second)
{
	INT8U  err;
	INT32S nRet, msg;	
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
		
	if((vid_dec_get_status() & C_VIDEO_DECODE_PARSER) == 0) 
		RETURN(STATUS_FAIL);	
	
	if(second > p_vid_dec_para->video_total_time)
		RETURN(STATUS_FAIL);
		
	nRet = STATUS_OK;
	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAY)
	{
		// stop play
		vid_dec_clear_status(C_VIDEO_DECODE_PLAY);
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_STOP, vid_dec_state_ack_m, 5000, msg, err);
		
		// set seek
		p_vid_dec_para->video_cur_time = second;
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_SET_SEEK, vid_dec_state_ack_m, 5000, msg, err);	
		
		// start play
		if(vid_dec_get_audio_flag() || vid_dec_get_video_flag())
		{	
			SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_SET_SEEK_PLAY, vid_dec_state_ack_m, 5000, msg, err);
			vid_dec_set_status(C_VIDEO_DECODE_PLAY);
		}
		else if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
		{
			vid_dec_clear_status(C_VIDEO_DECODE_PARSER);
			SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_PARSER_HEADER_STOP, vid_dec_state_ack_m, 5000, msg, err);		
		}
	}
	else if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
	{
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_SET_SEEK, vid_dec_state_ack_m, 5000, msg, err);
	}
	else
	{
		RETURN(STATUS_FAIL)
	}
	
	nRet = p_vid_dec_para->video_cur_time;	
Return:	
	if(nRet < 0)
		return nRet;
		
	return nRet;
}

// play speed = speed / 0x10000, normal speed = 0x10000 
INT32S vid_dec_set_play_speed(INT32U speed)
{	
	INT8U  err;
	INT32S nRet, msg;	
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
		
	if((vid_dec_get_status() & C_VIDEO_DECODE_PLAY) == 0) 
		RETURN(STATUS_FAIL);	
	
	if(speed == 0) RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;	
	p_vid_dec_para->play_speed = speed;	
	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_SET_PLAY_SPEED, vid_dec_state_ack_m, 5000, msg, err);	
Return:	
	return nRet;
}

INT32S vid_dec_set_reverse_play(INT32U reverse_play_flag)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
	
	if((vid_dec_get_status() & C_VIDEO_DECODE_PLAY) == 0) 
		RETURN(STATUS_FAIL);	
	
	if(vid_dec_get_video_flag() == 0)
		RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;	
	p_vid_dec_para->reverse_play_set = reverse_play_flag;
	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_SET_REVERSE, vid_dec_state_ack_m, 5000, msg, err);
Return:
	return nRet;
}

INT32S vid_dec_nth_frame(INT32U nth)
{
	INT8U  err;
	INT32S nRet, msg;
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) 
		RETURN(STATUS_FAIL);
	
	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAY)
		RETURN(STATUS_FAIL);
	
	if(vid_dec_get_video_flag() == 0)
		RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
	if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
	{
		vid_dec_set_status(C_VIDEO_DECODE_PARSER_NTH);
		SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_NTH_FRAME, vid_dec_state_ack_m, 5000, msg, err);
	}
Return:
	vid_dec_clear_status(C_VIDEO_DECODE_PARSER_NTH);
	return nRet;	
}

INT32S video_decode_state_task_create(INT8U pori)
{
	INT8U  err;
	INT32S nRet;
	
	p_vid_dec_para = &vid_dec_para;
	gp_memset((INT8S*)p_vid_dec_para, 0, sizeof(video_decode_para_struct));
    vid_dec_state_q = OSQCreate(vid_dec_state_q_buffer, C_VID_DEC_QUEUE_MAX_LEN);
    if(!vid_dec_state_q) RETURN(STATUS_FAIL);
    	
    vid_dec_state_ack_m = OSMboxCreate(NULL);
	if(!vid_dec_state_ack_m) RETURN(STATUS_FAIL);
	
	err = OSTaskCreate(video_decode_state_task_entry, (void*) NULL, &vid_dec_state_stack[C_VID_DEC_STATE_STACK_SIZE - 1], pori);
	if(err != OS_NO_ERR) RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
Return:		
	return nRet;	
}

INT32S video_decode_state_task_del(INT8U pori)
{ 
    INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK; 
    if(vid_dec_get_status() & C_VIDEO_DECODE_PLAY) 
    {	
    	vid_dec_clear_status(C_VIDEO_DECODE_PLAY);	
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_STOP, vid_dec_state_ack_m, 5000, msg, err);
    }
    
    if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER)
    {
    	vid_dec_clear_status(C_VIDEO_DECODE_PARSER);
    	SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_PARSER_HEADER_STOP, vid_dec_state_ack_m, 5000, msg, err);
    }
    
    SEND_MESSAGE(vid_dec_state_q, MSG_VIDEO_DECODE_EXIT, vid_dec_state_ack_m, 5000, msg, err);
Return:    		
    OSQFlush(vid_dec_state_q);
   	OSQDel(vid_dec_state_q, OS_DEL_ALWAYS, &err);
	OSMboxDel(vid_dec_state_ack_m, 0, &err);
	return nRet;
}

//////////////////////////////////////////////////////////////////////////////////////
static void vid_dec_parser_end(void)
{
	void *hMedia;
	
	hMedia = p_vid_dec_para->media_handle;
	p_vid_dec_para->media_handle = NULL;
	if(hMedia) MultiMediaParser_Delete(hMedia);
    if(p_vid_dec_para->file_handle >= 0)
    	close(p_vid_dec_para->file_handle);
	
	p_vid_dec_para->file_handle = -1;
	p_bitmap_info = NULL;
	p_wave_info = NULL;
} 

static INT32S play_speed_set(void)
{
	INT32S nRet;
	OS_CPU_SR cpu_sr;
	
	nRet = STATUS_OK;
	if(p_vid_dec_para->play_speed == 0x10000 && p_vid_dec_para->reverse_play_flag == 0)
	{
		if(p_vid_dec_para->ff_audio_flag)
		{
			p_vid_dec_para->ff_audio_flag = FALSE;
			vid_dec_set_audio_flag(TRUE);
			MultiMediaParser_EnableAudStreaming(p_vid_dec_para->media_handle, 1);
   			if(audio_decode_task_start() < 0)
   				RETURN(STATUS_FAIL);
   						
   			vid_dec_set_status(C_AUDIO_DECODE_PLAYING);
		}
	}
	else
	{
		/* no video, only audio, so skip set play speed */
		if(vid_dec_get_video_flag() == 0)
		{
			DEBUG_MSG(DBG_PRINT("NoVidData.\r\n"));
			RETURN(STATUS_FAIL);
		}
		
		if(vid_dec_get_audio_flag())
    	{
   			p_vid_dec_para->ff_audio_flag = TRUE;
   			vid_dec_set_audio_flag(FALSE);
   			if(audio_decode_task_stop() < 0)
   				RETURN(STATUS_FAIL);	
   			
   			vid_dec_clear_status(C_AUDIO_DECODE_PLAYING);
			MultiMediaParser_EnableAudStreaming(p_vid_dec_para->media_handle, 0);   				
		}
	}
	
	OS_ENTER_CRITICAL();
	MultiMediaParser_SetFrameDropLevel(p_vid_dec_para->media_handle, 0);
	p_vid_dec_para->tick_time2 = (p_vid_dec_para->tick_time * p_vid_dec_para->play_speed) >> 16;
	p_vid_dec_para->fail_cnt = 0;
	p_vid_dec_para->ta = 0;
	p_vid_dec_para->tv = 0;
	p_vid_dec_para->Tv = 0;
	OS_EXIT_CRITICAL();
	vid_dec_stop_timer(); 
	vid_dec_start_timer();
Return:
	return nRet;
}

static INT32S reserved_play_set(void)
{
	INT32S nRet;
	OS_CPU_SR cpu_sr;
	
	nRet = STATUS_OK;
	if(p_vid_dec_para->reverse_play_flag != p_vid_dec_para->reverse_play_set)
	{
		vid_dec_stop_timer();
		if(video_decode_task_stop() < 0)
			RETURN(STATUS_FAIL);
		
		p_vid_dec_para->reverse_play_flag = p_vid_dec_para->reverse_play_set;
		if(MultiMediaParser_SetReversePlay(p_vid_dec_para->media_handle, p_vid_dec_para->reverse_play_set) < 0)
			RETURN(STATUS_FAIL);
		
		if(p_vid_dec_para->play_speed == 0x10000 && p_vid_dec_para->reverse_play_flag == 0)
		{
			OS_ENTER_CRITICAL();
			MultiMediaParser_SetFrameDropLevel(p_vid_dec_para->media_handle, 0);
			p_vid_dec_para->fail_cnt = 0;
			p_vid_dec_para->ta = 0;
			p_vid_dec_para->tv = 0;
			p_vid_dec_para->Tv = 0;
			OS_EXIT_CRITICAL();
			if(p_vid_dec_para->ff_audio_flag)
			{
				p_vid_dec_para->ff_audio_flag = FALSE;
				vid_dec_set_audio_flag(TRUE);
				MultiMediaParser_EnableAudStreaming(p_vid_dec_para->media_handle, 1);
				if(video_decode_task_restart() < 0)
					RETURN(STATUS_FAIL);	
				
				vid_dec_start_timer();
				if(audio_decode_task_start() < 0)
					RETURN(STATUS_FAIL);
				
				vid_dec_set_status(C_AUDIO_DECODE_PLAYING);
			}
			else
			{
				if(video_decode_task_restart() < 0)
					RETURN(STATUS_FAIL);	
				
				vid_dec_start_timer();
			} 
		}
		else
		{
			if(vid_dec_get_audio_flag())
	    	{
	   			p_vid_dec_para->ff_audio_flag = TRUE;
	   			vid_dec_set_audio_flag(FALSE);
	   			if(audio_decode_task_stop() < 0)
	   				RETURN(STATUS_FAIL);
	   				
	   			vid_dec_clear_status(C_AUDIO_DECODE_PLAYING);
				MultiMediaParser_EnableAudStreaming(p_vid_dec_para->media_handle, 0);      				
			}
			
			OS_ENTER_CRITICAL();
			MultiMediaParser_SetFrameDropLevel(p_vid_dec_para->media_handle, 0);
			p_vid_dec_para->fail_cnt = 0;
			p_vid_dec_para->ta = 0;
			p_vid_dec_para->tv = 0;
			p_vid_dec_para->Tv = 0;
			OS_EXIT_CRITICAL(); 
			if(video_decode_task_restart() < 0)
				RETURN(STATUS_FAIL);	
			
			vid_dec_start_timer();
		}
	}
	
Return:
	return nRet;
}

//////////////////////////////////////////////////////////////////////////////////////
void video_decode_error_callback(void)
{
	DEBUG_MSG(DBG_PRINT("video_decode_error_callback\r\n"));
}

int video_parser_error_handle(INT32S error_id)
{
	DEBUG_MSG(DBG_PRINT("ParserErrID =0x%x\r\n", error_id));
	vid_dec_set_status(C_VIDEO_DECODE_ERR);
#if 1
	/* error happen close video parser automatic */
	// stop video 
	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)
    {
        vid_dec_clear_status(C_VIDEO_DECODE_PLAYING);
		vid_dec_stop_timer();
		video_decode_task_stop();
    }
    
    // wait audio stop
    if(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
    {
    	while(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
			OSTimeDly(1);
  	}
  	
	// stop parser and free memory
	OSQPost(vid_dec_state_q, (void*)MSG_VIDEO_DECODE_ERROR);
	return 1;
#else
	/* error happen close video parser by user, call video decode stop */
	return 0;
#endif
}

void video_decode_state_task_entry(void *para)
{
	void    *hMedia;
	INT8U   err;
    INT32S  i, error_id, nRet;
    INT32U  msg_id;
 	
    while(1)
    {      
        msg_id = (INT32U) OSQPend(vid_dec_state_q, 0, &err);
        if((!msg_id) || (err != OS_NO_ERR))
        	continue;
        
        switch(msg_id)
        {
        case MSG_VIDEO_DECODE_PARSER_HEADER_START:
        	//clear count
        	p_vid_dec_para->n = 0;
			p_vid_dec_para->ta = 0;
			p_vid_dec_para->tv = 0;
			p_vid_dec_para->Tv = 0;
			p_vid_dec_para->fail_cnt = 0;
        	p_vid_dec_para->pend_cnt = 0;
        	p_vid_dec_para->post_cnt = 0;
        	p_vid_dec_para->video_flag = 0;
        	p_vid_dec_para->user_define_flag = 0;
        	p_vid_dec_para->video_format = 0;
        	p_vid_dec_para->deblock_flag = 0;
        	p_vid_dec_para->play_speed = 0x10000;
        	p_vid_dec_para->reverse_play_set = 0;
        	p_vid_dec_para->reverse_play_flag = 0;
        	p_vid_dec_para->ff_audio_flag = 0;	
        	p_vid_dec_para->audio_flag = 0;
        	p_vid_dec_para->scaler_flag = 0;
        	p_vid_dec_para->image_output_width = 0;
        	p_vid_dec_para->image_output_height = 0;
        	p_vid_dec_para->buffer_output_width = 0;
        	p_vid_dec_para->buffer_output_height = 0;
        	p_vid_dec_para->mpeg4_decode_out_format = C_MP4_OUTPUT_Y1UY0V;
			p_vid_dec_para->video_decode_in_format =  C_SCALER_CTRL_IN_YUYV;
			p_vid_dec_para->video_decode_out_format = C_SCALER_CTRL_OUT_YUYV;
           	
           	for(i=0; i<AUDIO_FRAME_NO; i++)
				p_vid_dec_para->audio_decode_addr[i] = 0;
	
           	for(i=0; i<VIDEO_FRAME_NO; i++)
				p_vid_dec_para->video_decode_addr[i] = 0;
			
			for(i=0; i<SCALER_FRAME_NO; i++)
				p_vid_dec_para->scaler_output_addr[i] = 0;
			
			for(i=0; i<DEBLOCK_FRAME_NO; i++)
				p_vid_dec_para->deblock_addr[i] = 0;
			
           	hMedia = MultiMediaParser_Create(
           		p_vid_dec_para->file_handle,
           		p_vid_dec_para->FileSize,
           		PARSER_AUD_BITSTREAM_SIZE,
           		PARSER_VID_BITSTREAM_SIZE,
           		TSK_PRI_VID_PARSER,
           		p_vid_dec_para->FileType==FILE_TYPE_AVI ? AviParser_GetFcnTab() : QtffParser_GetFcnTab(),
           		video_parser_error_handle,
           		&error_id);
           
           	if((error_id & 0x80000000) || !hMedia)
           	{
           		close(p_vid_dec_para->file_handle);
	           	p_vid_dec_para->file_handle = -1;
	           	p_wave_info = NULL;
           		p_bitmap_info = NULL;
           		DEBUG_MSG(DBG_PRINT("ParserErrID = 0x%x\r\n", error_id));
           		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL);
           	}
           	else
           	{	
           		p_vid_dec_para->media_handle = hMedia;
           		p_vid_dec_para->VidTickRate = MultiMediaParser_GetVidTickRate(p_vid_dec_para->media_handle);
           		p_vid_dec_para->video_total_sample = MultiMediaParser_GetVidTotalSamples(p_vid_dec_para->media_handle);
			 	p_vid_dec_para->video_total_time = MultiMediaParser_GetVidTrackDur(p_vid_dec_para->media_handle) >> 8;
				p_vid_dec_para->video_cur_time = 0;
				
           		p_bitmap_info = MultiMediaParser_GetVidInfo(hMedia, &g_bitmap_info_len);
           		p_wave_info = MultiMediaParser_GetAudInfo(hMedia, &g_wave_info_len);
				
           		switch(p_wave_info->wFormatTag)
           		{
           		case WAVE_FORMAT_PCM:
           		case WAVE_FORMAT_MULAW:
           		case WAVE_FORMAT_ALAW:
           		case WAVE_FORMAT_ADPCM:
           		case WAVE_FORMAT_DVI_ADPCM:
           		#if MP3_DECODE_ENABLE == 1
           		case WAVE_FORMAT_MPEGLAYER3:
           		#endif
           		#if AAC_DECODE_ENABLE == 1
           		case WAVE_FORMAT_RAW_AAC1:
           		case WAVE_FORMAT_MPEG_ADTS_AAC:
           		#endif
           			if(p_wave_info->wFormatTag == WAVE_FORMAT_MPEGLAYER3) {
           				DEBUG_MSG(DBG_PRINT("AudFormat = MP3\r\n"));
	   				} else if(p_wave_info->wFormatTag == WAVE_FORMAT_RAW_AAC1 || 
	   						p_wave_info->wFormatTag == WAVE_FORMAT_MPEG_ADTS_AAC) {	
           				DEBUG_MSG(DBG_PRINT("AudFormat = AAC\r\n"));
           			} else if(p_wave_info->wFormatTag == WAVE_FORMAT_PCM) {	
           				DEBUG_MSG(DBG_PRINT("AudFormat = WAVE\r\n"));
           			} else if(p_wave_info->wFormatTag == WAVE_FORMAT_ADPCM || 
	   						p_wave_info->wFormatTag == WAVE_FORMAT_DVI_ADPCM) {	
           				DEBUG_MSG(DBG_PRINT("AudFormat = ADPCM\r\n"));	
           			} else {
           				DEBUG_MSG(DBG_PRINT("AudFormat = 0x%x\r\n", p_wave_info->wFormatTag));
           			}
           			
	   				vid_dec_set_audio_flag(TRUE);
	   				p_vid_dec_para->aud_frame_size = AUDIO_PCM_BUFFER_SIZE;
	   			#if UP_SAMPLE_EN == 1
	   				p_vid_dec_para->aud_frame_size <<= 1;
	   			#endif	
	   				break;
           			
           		default:
           			DEBUG_MSG(DBG_PRINT("NotSupportAudFormat!!!\r\n"));
           			vid_dec_set_audio_flag(FALSE);
           			p_wave_info = NULL;
           			break;
           		}
           		
           		p_vid_dec_para->video_format = vid_dec_get_video_format(p_bitmap_info->biCompression);
       			switch(p_vid_dec_para->video_format)
       			{
       			case C_MJPG_FORMAT:
       				vid_dec_set_video_flag(TRUE);
       				break;
       				
       			#if MPEG4_DECODE_ENABLE == 1
       			case C_XVID_FORMAT:
       			case C_M4S2_FORMAT:
       			case C_H263_FORMAT:
       				if(p_bitmap_info->biWidth%16 != 0 || p_bitmap_info->biWidth > 768)
       				{
       					DEBUG_MSG(DBG_PRINT("Width is not 16-Align or > 768\r\n"));
       					vid_dec_set_video_flag(FALSE);
       				}
       				else
       				{
       					vid_dec_set_video_flag(TRUE);
       				}
       				break;
				#endif
				
       			default:
       				vid_dec_set_video_flag(FALSE);
       				p_bitmap_info = NULL;
       				break;
       			}
				
				// no audio and no video
           		if(vid_dec_get_audio_flag() == FALSE && vid_dec_get_video_flag() == FALSE)
           		{	
           			vid_dec_parser_end();	
	       			OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL);
	       			continue;
           		}
				
           		if(p_wave_info) 
           		{
           			p_vid_dec_para->n = p_wave_info->nSamplesPerSec*2/TIME_BASE_TICK_RATE; 
           			p_vid_dec_para->tick_time = (INT64S)p_vid_dec_para->VidTickRate * p_vid_dec_para->n;
           			p_vid_dec_para->time_range = p_vid_dec_para->tick_time * TIME_BASE_TICK_RATE >> 2; // about 256ms
           		}
           		else
           		{
           			p_vid_dec_para->n = 1;
           			p_vid_dec_para->tick_time = (INT64S)p_vid_dec_para->VidTickRate * p_vid_dec_para->n;
           			p_vid_dec_para->time_range = p_vid_dec_para->tick_time * TIME_BASE_TICK_RATE >> 2; // about 256ms
           		}
           		p_vid_dec_para->tick_time2 = p_vid_dec_para->tick_time;
           		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS);
           	}
           	break; 	
           	
        case MSG_VIDEO_DECODE_PARSER_HEADER_STOP:
           	vid_dec_parser_end();
           	vid_dec_memory_free();
           	OSQFlush(vid_dec_state_q);
           	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 
           	break;
           
        case MSG_VIDEO_DECODE_START:
        	if(vid_dec_memory_alloc() < 0)
        	{
        		vid_dec_memory_free();
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL);
        		continue; 
        	}	
        
        	if(vid_dec_get_video_flag())
    		{
    			if(video_decode_task_start() < 0)
    			{
    				OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
    			}		
    			vid_dec_set_status(C_VIDEO_DECODE_PLAYING);
    		}
        		
    		if(vid_dec_get_audio_flag())
    		{
    			if(audio_decode_task_start() < 0)	
    			{
    				OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
    			}	
    			vid_dec_set_status(C_AUDIO_DECODE_PLAYING);
    		}
    			
        	if(vid_dec_get_video_flag())			
        		vid_dec_start_timer();

        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 
        	break;
        	
        case MSG_VIDEO_DECODE_STOP:
        	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)
        	{
        		vid_dec_clear_status(C_VIDEO_DECODE_PLAYING);
        		vid_dec_stop_timer();
        		if(video_decode_task_stop() < 0)
        		{
        			OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
        		}
        	}
        		
        	if(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
        	{
        		vid_dec_clear_status(C_AUDIO_DECODE_PLAYING);
        		if(audio_decode_task_stop() < 0)
        		{
        			OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
        		}
			}
				
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;
        
        case MSG_VIDEO_DECODE_PAUSE:
        	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)
        		vid_dec_stop_timer();
        				
        	if(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
        		audio_decode_task_pause();	
        			
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;
        
        case MSG_VIDEO_DECODE_RESUME:
			if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)
				vid_dec_start_timer();
			
			if(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
				audio_decode_task_resume();
				
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;
        	
		case MSG_VIDEO_DECODE_GET_CUR_TIME:
        	if(p_vid_dec_para->media_handle == 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}
        	
			p_vid_dec_para->video_cur_time = MultiMediaParser_GetVidCurTime(p_vid_dec_para->media_handle) >> 8;
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;
        	
        case MSG_VIDEO_DECODE_SET_SEEK:
        	if(p_vid_dec_para->media_handle == 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}

        	nRet = p_vid_dec_para->video_cur_time << 8;
			nRet = MultiMediaParser_SeekTo(p_vid_dec_para->media_handle, nRet);
			if(nRet < 0)
			{
				OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL);
				continue; 
			}
			
			p_vid_dec_para->video_cur_time = nRet >> 8;	
	   		if(MultiMediaParser_IsEOA(hMedia))
			{
				vid_dec_set_audio_flag(FALSE);	
				vid_dec_clear_status(C_AUDIO_DECODE_PLAYING);
			}

			if(MultiMediaParser_IsEOV(hMedia))
			{
				vid_dec_set_video_flag(FALSE);
				vid_dec_clear_status(C_VIDEO_DECODE_PLAYING);
			}

			vid_dec_stop_timer();
			vid_dec_start_timer();
			OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS);
			break;
			
		case MSG_VIDEO_DECODE_SET_SEEK_PLAY:
        	if(vid_dec_get_video_flag())
    		{
    			if(video_decode_task_restart() < 0)
    			{
    				OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
    			}		
    			vid_dec_set_status(C_VIDEO_DECODE_PLAYING);
    		}
        		
        	if(vid_dec_get_audio_flag())
        	{
        		if(audio_decode_task_start() < 0)
        		{
        			OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    				continue;
        		}	
        		vid_dec_set_status(C_AUDIO_DECODE_PLAYING);
        	}	
        	
        	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)		
        		vid_dec_start_timer();
        	
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 		
        	break;
        
        case MSG_VIDEO_DECODE_SET_PLAY_SPEED:
        	if(p_vid_dec_para->media_handle == 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}
        	
        	if(play_speed_set() < 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;
        	
         case MSG_VIDEO_DECODE_SET_REVERSE:
        	if(p_vid_dec_para->media_handle == 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}
        	
        	if(reserved_play_set() < 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
    			continue;
        	}
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;	
        	
        case MSG_VIDEO_DECODE_NTH_FRAME:
        	if(vid_dec_memory_alloc() < 0)
        	{
        		vid_dec_memory_free();
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
        		continue;
        	}	
        	
        	if(video_decode_task_nth_frame() < 0)
        	{
        		OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_FAIL); 
        		continue;
        	}
        	
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 	
        	break;	
        
        case MSG_VIDEO_DECODE_ERROR:
        	error_id = 0x80000000;	
        case MSG_VIDEO_DECODE_FORCE_STOP:
        	if(vid_dec_get_status() & C_VIDEO_DECODE_PLAYING)
            	video_decode_task_stop();
            
            if(vid_dec_get_status() & C_AUDIO_DECODE_PLAYING)
            	audio_decode_task_stop();
   			
   			vid_dec_clear_status(C_VIDEO_DECODE_ALL);
        	vid_dec_parser_end();
           	vid_dec_memory_free();
           	OSQFlush(vid_dec_state_q);
           	if(error_id) 
           	{
           		video_decode_error_callback();
           	}
           	else
           	{
				OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS);
           	#if _PROJ_TYPE == _PROJ_TURNKEY	
				msgQSend(ApQ, MSG_MJPEG_END, NULL, NULL, MSG_PRI_NORMAL);
			#else	
				video_decode_end();	
			#endif
			}
        	break;
        			 	
        case MSG_VIDEO_DECODE_EXIT:
        	OSMboxPost(vid_dec_state_ack_m, (void*)C_ACK_SUCCESS); 
   			OSTaskDel(OS_PRIO_SELF);
        	break;
        }  
    }
}
