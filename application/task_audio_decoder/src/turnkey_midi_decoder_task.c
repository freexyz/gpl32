#include "turnkey_audio_decoder_task.h"

// Constant definitions used in this file only go here
#define AUDIO_QUEUE_MAX  6
#define AUDIO_FS_Q_SIZE  1
#define AUDIO_WRITE_Q_SIZE MAX_DAC_BUFFERS
#define AUDIO_PARA_MAX_LEN  sizeof(STAudioTaskPara)

/* external varaible */
extern STAudioConfirm  aud_con;

/* Task Q declare */
MSG_Q_ID MIDITaskQ;

OS_EVENT	*midi_wq;
void		*write_midi_q[AUDIO_WRITE_Q_SIZE];
OS_EVENT	*midi_fsq;
void		*fs_midi_q[AUDIO_FS_Q_SIZE];
INT8U       midi_para[AUDIO_PARA_MAX_LEN];

INT16S      *pcm_midi_out[MAX_DAC_BUFFERS] = {NULL};
INT32U      pcm_midi_len[MAX_DAC_BUFFERS];

AUDIO_CONTEXT   midi_context;
AUDIO_CONTEXT_P midi_context_p = &midi_context;
AUDIO_CTRL      midi_ctrl;

//INT8U  stopped_bg;
extern INT8U stopped_bg;	// tested by Bruce

INT32U g_MIDI_index;		// modified by Bruce, 2008/10/01

INT32U g_delta_time_start;	// added by Bruce, 2008/10/31
INT32U g_delta_time_end;	// added by Bruce, 2008/10/31

#if SUPPORT_MIDI_PLAY_WITHOUT_TASK == 1
INT8U *SPU_ring_buf;		// added by Bruce, 2008/11/14
#endif

/* Proto types */
void midi_task_init(void);
void midi_task_entry(void *p_arg);

static void    midi_init(void);

#if APP_G_MIDI_DECODE_EN == 1
static INT32S  midi_play_init(void);
static INT32S  audio_midi_process(void);
#endif

static void    midi_queue_clear(void);
static void    midi_stop_unfinished(void);
static void    midi_send_next_frame_q(void);
static void    midi_spu_restart(void);

static void    midi_start(STAudioTaskPara *pAudioTaskPara);
static void    midi_pause(STAudioTaskPara *pAudioTaskPara);
static void    midi_resume(STAudioTaskPara *pAudioTaskPara);
static void    midi_stop(STAudioTaskPara *pAudioTaskPara);
static void    midi_decode_next_frame(STAudioTaskPara *pAudioTaskPara);
static void    midi_mute_set(STAudioTaskPara *pAudioTaskPara);
static void    midi_volume_set(STAudioTaskPara *pAudioTaskPara);

static INT32S  audio_bg_q_check(void);

void midi_task_init(void)
{
    /* Create MsgQueue/MsgBox for TASK */
    MIDITaskQ = msgQCreate(AUDIO_QUEUE_MAX, AUDIO_QUEUE_MAX, AUDIO_PARA_MAX_LEN);
    midi_wq = OSQCreate(write_midi_q, AUDIO_WRITE_Q_SIZE);
	midi_fsq = OSQCreate(fs_midi_q, AUDIO_FS_Q_SIZE);
	
	decode_end = NULL;		// added by Bruce, 2008/10/06

	g_delta_time_start = 0;	// added by Bruce, 2008/11/14
	g_delta_time_end = 0;
}

void midi_task_entry(void *p_arg)
{
    INT32U  msg_id;
	STAudioTaskPara     *pstAudioTaskPara;
	
	midi_task_init();
	midi_init();
	
	while (1) 
	{
	    /* Pend task message */
	    msgQReceive(MIDITaskQ, &msg_id, (void*)midi_para, AUDIO_PARA_MAX_LEN);
	    pstAudioTaskPara = (STAudioTaskPara*) midi_para;
        
		switch(msg_id) {
			case MSG_AUD_PLAY: /* by file handle */
			case MSG_AUD_PLAY_BY_SPI:
				midi_start(pstAudioTaskPara);
				break;
			case MSG_AUD_STOP:
				midi_stop(pstAudioTaskPara);
				break;
			case MSG_AUD_PAUSE:
				midi_pause(pstAudioTaskPara);
				break;
			case MSG_AUD_RESUME:
				midi_resume(pstAudioTaskPara);
				break;
			case MSG_AUD_SET_MUTE:
				midi_mute_set(pstAudioTaskPara);
				break;
			case MSG_AUD_VOLUME_SET:
				midi_volume_set(pstAudioTaskPara);
				break;
			case MSG_AUD_DECODE_NEXT_FRAME:
				midi_decode_next_frame(pstAudioTaskPara);
				break;
			case MSG_AUD_SPU_RESTART:
				midi_spu_restart();
				break;
			default:
				break;
		}
	}		
}

static void midi_init(void)
{
	midi_ctrl.ring_buf = (INT8U*) gp_malloc(RING_BUF_SIZE);;
	midi_ctrl.ring_size = RING_BUF_SIZE;
	SPU_Set_midi_ring_buffer_addr((INT32U)midi_ctrl.ring_buf);//081115
	
	midi_ctrl.wi = 0;
	midi_ctrl.ri = 0;
	midi_context_p->state = AUDIO_PLAY_STOP;
	stopped_bg = 1;
}

static void midi_start(STAudioTaskPara *pAudioTaskPara)
{
	INT32S ret;
	
	if (midi_context_p->state != AUDIO_PLAY_STOP) {
		midi_stop_unfinished();
	}
	
	stopped_bg = 1;
	midi_queue_clear();
	
	midi_context_p->source_type = pAudioTaskPara->src_type;
	
	if (midi_context_p->source_type == AUDIO_SRC_TYPE_FS) {
		DBG_PRINT("Play FS Audio...\n\r");
		ret = audio_play_file_set(pAudioTaskPara->fd, midi_context_p, &midi_ctrl, midi_fsq);
	}
	else if (midi_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)	// added by Bruce, 2008/10/28
	{
		DBG_PRINT("Play User Define Audio...\n\r");
		//ret = audio_play_file_set(pAudioTaskPara->fd, midi_context_p, &midi_ctrl, midi_fsq);
		
		// modified by Bruce, 2008/10/30
		midi_context_p->audio_format = pAudioTaskPara->audio_format;
	   	if (midi_context_p->audio_format == AUDIO_TYPE_NONE) 
	   	{
		   DBG_PRINT("audio_play_file_set find not support audio.\r\n");
	   	}
	   	midi_ctrl.file_handle = pAudioTaskPara->fd;//081127
		midi_ctrl.file_len = pAudioTaskPara->file_len;
		g_MIDI_index = pAudioTaskPara ->src_id;	// added by Bruce, 2008/11/04
		ret = AUDIO_ERR_NONE;
	}
	else {
			#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1//For nv midi play.
			if(pAudioTaskPara ->audio_format == AUDIO_TYPE_MIDI)
			{
				midi_ctrl.file_len = pAudioTaskPara->file_len;
				midi_ctrl.file_handle = pAudioTaskPara ->fd;   
				midi_context_p ->audio_format = AUDIO_TYPE_MIDI;
				g_MIDI_index = pAudioTaskPara ->src_id;
				DBG_PRINT("g_MIDI_index = %d\r\n",g_MIDI_index);
		    	ret = AUDIO_ERR_NONE;
			}
			else
			#endif
			{
		    	ret = AUDIO_ERR_NONE;
		    	midi_ctrl.file_handle = pAudioTaskPara ->fd;  
		    	midi_context_p->audio_format = pAudioTaskPara->audio_format;
		    	midi_ctrl.file_len = pAudioTaskPara->file_len;
			}
	}
	
	if (ret == AUDIO_ERR_NONE) {
		if (audio_mem_alloc(midi_context_p->audio_format,&midi_ctrl,pcm_midi_out) != AUDIO_ERR_NONE) {
			DBG_PRINT("audio memory allocate fail\r\n");
		#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
			audio_send_result(MSG_AUD_BG_PLAY_RES,AUDIO_ERR_MEM_ALLOC_FAIL);
		#else
			MIDI_send_result(MSG_AUD_BG_PLAY_RES,AUDIO_ERR_MEM_ALLOC_FAIL);
		#endif
			return;
		}
		dac_enable_set(TRUE); /* enable dac */
		midi_context_p->state = AUDIO_PLAYING;
		
		midi_ctrl.ring_size = RING_BUF_SIZE;
		midi_ctrl.reading = 0;
		
		switch(midi_context_p->audio_format) {
		#if APP_G_MIDI_DECODE_EN == 1
			case AUDIO_TYPE_MIDI:
				midi_context_p->fp_deocde_init = midi_play_init;
				midi_context_p->fp_deocde = audio_midi_process;
				break;
		#endif
			default:
			#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
				audio_send_result(MSG_AUD_BG_PLAY_RES,AUDIO_ERR_INVALID_FORMAT);//080904
			#else
				MIDI_send_result(MSG_AUD_BG_PLAY_RES,AUDIO_ERR_INVALID_FORMAT);//080904
			#endif
				return;
				break;
		}
		ret = midi_context_p->fp_deocde_init();
		if (ret != AUDIO_ERR_NONE) {
			midi_stop_unfinished();
			DBG_PRINT("audio play init failed\r\n");
        }
        else {
        	midi_send_next_frame_q();
		}
	} 
	else {
    	ret = AUDIO_ERR_INVALID_FORMAT;
	}
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
	audio_send_result(MSG_AUD_BG_PLAY_RES,ret);
#else
	MIDI_send_result(MSG_AUD_BG_PLAY_RES,ret);
#endif
}

static void midi_stop(STAudioTaskPara *pAudioTaskPara)
{
	if((midi_context_p->state == AUDIO_PLAY_STOP)||(midi_context_p->state == AUDIO_IDLE)) {
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_BG_STOP_RES,AUDIO_ERR_NONE);
#else
		MIDI_send_result(MSG_AUD_BG_STOP_RES,AUDIO_ERR_NONE);
#endif
		return;
	}
	
	midi_stop_unfinished();
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	audio_send_result(MSG_AUD_BG_STOP_RES,AUDIO_ERR_NONE);
#else
	MIDI_send_result(MSG_AUD_BG_STOP_RES,AUDIO_ERR_NONE);
#endif
}

static void midi_pause(STAudioTaskPara *pAudioTaskPara)
{
	if (midi_context_p->state != AUDIO_PLAYING) {
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		MIDI_send_result(MSG_AUD_BG_PAUSE_RES, AUDIO_ERR_NONE);
#endif		
		return;
	}
	
	while(1) {
		if (audio_bg_q_check() == AUDIO_SEND_OK) { /* queue empty */
			SPU_StopChannel(SPU_LEFT_CH);
			SPU_StopChannel(SPU_RIGHT_CH);
			SPU_Disable_FIQ_Channel(SPU_LEFT_CH);
			SPU_Disable_FIQ_Channel(SPU_RIGHT_CH);
			break;
		}
		OSTimeDly(2);
	}
	
	#if APP_G_MIDI_DECODE_EN == 1
		SPU_MIDI_Pause();	// added by Bruce, 2008/10/01
	#endif
	
	midi_context_p->state = AUDIO_PLAY_PAUSE;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	audio_send_result(MSG_AUD_BG_PAUSE_RES,AUDIO_ERR_NONE);
#else
	MIDI_send_result(MSG_AUD_BG_PAUSE_RES,AUDIO_ERR_NONE);
#endif
}

static void midi_resume(STAudioTaskPara *pAudioTaskPara)
{
	if (midi_context_p->state != AUDIO_PLAY_PAUSE) {
	#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		MIDI_send_result(MSG_AUD_BG_RESUME_RES, AUDIO_ERR_NONE);
	#endif		
		return;
	}
	
	#if APP_G_MIDI_DECODE_EN == 1
		SPU_MIDI_Resume();	// added by Bruce, 2008/10/01
	#endif
	
	midi_context_p->state = AUDIO_PLAYING;
	midi_spu_restart();
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)	
	audio_send_result(MSG_AUD_BG_RESUME_RES,AUDIO_ERR_NONE);
#else
	MIDI_send_result(MSG_AUD_BG_RESUME_RES,AUDIO_ERR_NONE);
#endif
}

static void midi_mute_set(STAudioTaskPara *pAudioTaskPara)
{
	if (pAudioTaskPara->mute == TRUE) {
		SPU_MIDI_Set_MIDI_Volume(0);
	}
	else {
		//SPU_Set_MainVolume(pAudioTaskPara->volume);//081113
		SPU_MIDI_Set_MIDI_Volume(pAudioTaskPara->volume);//081112
	}
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)	
	audio_send_result(MSG_AUD_MUTE_SET_RES,AUDIO_ERR_NONE);
#else
    MIDI_send_result(MSG_AUD_MUTE_SET_RES,AUDIO_ERR_NONE);
#endif
}

static void midi_volume_set(STAudioTaskPara *pAudioTaskPara)
{
	if (pAudioTaskPara->volume < 128) {
		//SPU_Set_MainVolume(pAudioTaskPara->volume);	
		SPU_MIDI_Set_MIDI_Volume(pAudioTaskPara->volume);//081112
	}
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)	
	audio_send_result(MSG_AUD_VOLUME_SET_RES,AUDIO_ERR_NONE);
#else
	MIDI_send_result(MSG_AUD_VOLUME_SET_RES,AUDIO_ERR_NONE);
#endif
}

static void midi_decode_next_frame(STAudioTaskPara *pAudioTaskPara)
{
	INT32S ret;
	if (midi_context_p->state != AUDIO_PLAYING) {
		return;
	}
	ret = midi_context_p->fp_deocde();
	if (ret != 0) {
		midi_stop_unfinished();
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_BG_PLAY_RES,ret);
#endif		
		if (decode_end != NULL) {
			decode_end(2);	// added by Bruce, 2008/10/03
		}
	}
}

static void midi_send_next_frame_q(void)
{
	msgQSend(MIDITaskQ, MSG_AUD_DECODE_NEXT_FRAME, NULL, 0, MSG_PRI_NORMAL);	
}

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
void MIDI_send_result(INT32S res_type,INT32S result)
{
	aud_con.result_type = res_type;
	aud_con.result = result;
	DBG_PRINT("MIDI_send_result :res_type =  %x,result = %d\r\n",res_type,result);
	msgQSend(MIDI_status_Q, EVENT_APQ_ERR_MSG, (void *)&aud_con, sizeof(STAudioConfirm), MSG_PRI_NORMAL);
}
#endif

static void midi_spu_restart(void)
{
	INT32U count;
	
	DBG_PRINT("SPU restart..\r\n");
	stopped_bg = 1;
	midi_queue_clear();
	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(midi_wq, (void *) count);
	} 
	midi_send_next_frame_q();
}

static void midi_stop_unfinished(void)
{
	INT32S i;
	
	switch(midi_context_p->audio_format)
	{
		case AUDIO_TYPE_MIDI:
				if(midi_ctrl.file_handle >= 0)
					close(midi_ctrl.file_handle);
				SPU_MIDI_Stop();
				SPU_Free_Midi();
				break;
	
		default:
				if(midi_ctrl.file_handle >= 0)
					close(midi_ctrl.file_handle);
				SPU_StopChannel(SPU_LEFT_CH);
				SPU_StopChannel(SPU_RIGHT_CH);
				SPU_Disable_FIQ_Channel(SPU_LEFT_CH);
				SPU_Disable_FIQ_Channel(SPU_RIGHT_CH);
				/* free memory */
				gp_free(midi_ctrl.work_mem);
				midi_ctrl.work_mem = NULL;
	
				for (i=0;i<MAX_DAC_BUFFERS;i++)
				{
					gp_free(*(pcm_midi_out+i));
					*(pcm_midi_out+i) = NULL;
				}
				break;
	}

	midi_context_p->state = AUDIO_PLAY_STOP;
}

static void midi_queue_clear(void)
{
	////OSQFlush(hAudioDacTaskQ);
	//OSQFlush(aud_bg_send_q);
	OSQFlush(midi_wq);
	OSQFlush(midi_fsq);
	//OSQFlush(aud_right_q);
	//OSQPost(hAudioDacTaskQ, (void *)MSG_AUD_SPU_WIDX_CLEAR);	
}

static INT32S audio_bg_q_check(void)
{
	OS_Q      *pq;
	pq = (OS_Q *)aud_bg_send_q->OSEventPtr;
	if (pq->OSQEntries == pq->OSQSize) {
		return AUDIO_SEND_FAIL;
	}
	return AUDIO_SEND_OK;
}

//-----------------------------------------------------------------midi play
#if APP_G_MIDI_DECODE_EN == 1
INT32S  midi_play_init(void)
{
	//INT32S MIDI_Index;
	INT32S TotalMidi;
	//INT32S MIDI_Volume = 120;
#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
	INT8U mode;
#endif
	STAudioTaskPara     *pAudioTaskPara;

	pAudioTaskPara = (STAudioTaskPara*) midi_para;

	SPU_MIDI_Set_MIDI_Volume(pAudioTaskPara->volume);//0~127,set MIDI software volume
	
	if (midi_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)
	{
		#if SUPPORT_MIDI_READ_FROM_SDRAM == 1
			TotalMidi = SPU_load_tonecolor_from_SDRAM(midi_ctrl.file_handle, MIDI_Index);
		#else
			TotalMidi = SPU_load_tonecolor(-1, pAudioTaskPara->src_id, 2);//nandflash app area without FS
		#endif
	}
	else if (midi_context_p->source_type == AUDIO_SRC_TYPE_APP_RS)
	{
		TotalMidi = SPU_load_tonecolor(midi_ctrl.file_handle, pAudioTaskPara->src_id, 4);//nv_read 
	}
	else if (midi_context_p->source_type == AUDIO_SRC_TYPE_APP_PACKED_RS)
	{
		TotalMidi = SPU_load_tonecolor(midi_ctrl.file_handle, pAudioTaskPara->src_id, 5);//nvp_read 
	}
	else if (midi_context_p->source_type == AUDIO_SRC_TYPE_SDRAM)
	{
		TotalMidi = SPU_load_tonecolor(-1, pAudioTaskPara->src_id, 6);//from sdram without fs
	}
	else
	{
		#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
		if (midi_context_p->source_type == AUDIO_SRC_TYPE_FS) 
			mode = 0;
		else 
			mode = 1;
		TotalMidi = SPU_load_tonecolor(midi_ctrl.file_handle, MIDI_Index,mode);
		#else
			TotalMidi = SPU_load_tonecolor(midi_ctrl.file_handle, pAudioTaskPara->src_id, 3);
		#endif
	}
	
	if((TotalMidi > 0) && (TotalMidi > pAudioTaskPara->src_id))
	{
		SPU_MIDI_Play(0);
		return AUDIO_ERR_NONE;
	}
	else
	{
		DBG_PRINT("Play Midi Error...\r\n");
		return AUDIO_ERR_GET_FILE_FAIL;
	}
}

INT32S  audio_midi_process(void)
{
	if (midi_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)		// added by Bruce, 2008/10/31
	{
		#if SUPPORT_MIDI_READ_FROM_SDRAM == 1
			SPU_check_fill_midi_ring_buffer_from_SDRAM();
		#else
			SPU_check_fill_midi_ring_buffer();
		#endif
	}
	else
	{
		SPU_check_fill_midi_ring_buffer();
	}

	OSTimeDly(5);
	if( (SPU_MIDI_Get_Status() & 0x00000001) == 0 )//stop
	{
		return AUDIO_ERR_DEC_FINISH;
	}
	
	if (midi_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)		// added by Bruce, 2008/10/31
	{
		#if SUPPORT_MIDI_READ_FROM_SDRAM == 1
			SPU_check_fill_midi_ring_buffer_from_SDRAM();
		#else
			SPU_check_fill_midi_ring_buffer();
		#endif
	}
	else
	{
		SPU_check_fill_midi_ring_buffer();
	}
	
	midi_send_next_frame_q();
	return(AUDIO_ERR_NONE);
}
#endif	// #if APP_G_MIDI_DECODE_EN == 1

// added by Bruce, 2008/11/14
#if SUPPORT_MIDI_PLAY_WITHOUT_TASK == 1
INT32U midi_play_init_for_idi_on_SDRAM_without_task(void)
{
	INT32S TotalMidi;
	INT32S MIDI_Volume = 120;
	
	SPU_Initial();
	SPU_Set_MainVolume(127);	//set spu global volume
	SPU_MIDI_Set_MIDI_Volume(MIDI_Volume);//0~127,set MIDI software volume

	// Assign the callback function for reading MIDI data from SDRAM
	audio_move_data = audio_encoded_data_read;

	SPU_ring_buf = (INT8U*) gp_malloc(4096);
	SPU_Set_midi_ring_buffer_addr((INT32U)SPU_ring_buf);

	TotalMidi = SPU_load_tonecolor_from_SDRAM(0, 0);

	if(TotalMidi <= 0)
	{
		DBG_PRINT("Play Midi Error...\r\n");
		return -1;
	}
	
	return TotalMidi;
}

void midi_play_start_for_idi_on_SDRAM_without_task(INT32U MIDI_Index)
{
	SPU_load_tonecolor_from_SDRAM(0, MIDI_Index);

	if(g_delta_time_start==0 && g_delta_time_end==0)
		SPU_MIDI_Play(0);
	else
		SPU_MIDI_PlayDt(g_delta_time_start, g_delta_time_end);
}
#endif // #if SUPPORT_MIDI_PLAY_WITHOUT_TASK == 1






