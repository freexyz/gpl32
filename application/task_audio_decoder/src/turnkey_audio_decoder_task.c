#include <string.h>
#include "turnkey_audio_decoder_task.h"

// Constant definitions used in this file only go here
#define AUDIO_QUEUE_MAX  64
#define AUDIO_FS_Q_SIZE  1
#define AUDIO_WRITE_Q_SIZE MAX_DAC_BUFFERS
#define AUDIO_PARA_MAX_LEN  sizeof(STAudioTaskPara)
#define AUDIO_AVI_RINGBUF_LEN	4000
#define RAMP_DOWN_STEP 4
#define RAMP_DOWN_STEP_HOLD 4
#define RAMP_DOWN_STEP_LOW_SR	4*16
#define USE_RAMP_DOWN_RAPID		0
#define SKIP_ID3_TAG            0
	
extern OS_EVENT  *Mbox_WMA_Play_Seek_Flag;         //add WMA_Seek
extern OS_EVENT  *Mbox_WMA_Play_Seek_Offset;
/* Task Q declare */
MSG_Q_ID AudioTaskQ;

OS_EVENT	*audio_wq;
void		*write_q[AUDIO_WRITE_Q_SIZE];
OS_EVENT	*audio_fsq;
void		*fs_q[AUDIO_FS_Q_SIZE];
INT8U       audio_para[AUDIO_PARA_MAX_LEN];

INT16S          *pcm_out[MAX_DAC_BUFFERS] = {NULL};
INT32U          pcm_len[MAX_DAC_BUFFERS];

INT8S		avi_work_mem[WAV_DEC_MEMORY_SIZE];
AUDIO_CONTEXT   audio_context;
AUDIO_CONTEXT_P audio_context_p = &audio_context;
AUDIO_CTRL      audio_ctrl;
STAudioConfirm  aud_con;
volatile INT8U  *avi_state_mon;

static struct sfn_info aud_sfn_file;
INT8U	stopped;
INT8U	audio_rampdown_disable_flag;
void	(*decode_end)(INT32U audio_decoder_num);	// added by Bruce, 2008/10/03
INT32S	(*audio_move_data)(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);	// added by Bruce, 2008/10/27
INT32S	(*audio_bg_move_data)(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);
INT32S  g_audio_data_offset;
static  INT8U   channel;
static  INT16U   g_audio_sample_rate;	

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32U mp3_VBR_flag = 0;//1:VBR  0:CBR
INT32S mp3_total_frame = 0;
INT32S audio_total_time = 0;
INT32U  mp3_ID3V2_length = 0;
INT32S  audio_play_start_time = 0;
INT32S  audio_decode_cnt = 0;
INT32U	audio_samplerate = 0;
INT32U  audio_bitrate = 0;
INT32U  audio_data_size = 0;
INT32S	audio_decode_sample_number = 0;
INT32U	audio_decode_perframe_sample_number = 0;
INT8U	audio_channel_number = 0;

static INT32S	(*pfnGetOutput)(void*, short*, int);
static void     *hSrc;
#endif

/* Proto types */
void audio_task_init(void);
void audio_task_entry(void *p_arg);

static void    audio_init(void);
#if APP_WAV_CODEC_FG_EN == 1
static INT32S  audio_avi_play_init(void);
static INT32S  audio_avi_process(void);
#endif

#if APP_MP3_DECODE_FG_EN == 1 
static INT32S  audio_avi_mp3_play_init(void);	
static INT32S  audio_avi_mp3_process(void);
#endif

#if APP_WAV_CODEC_FG_EN == 1
static INT32S  audio_wav_dec_play_init(void);
static INT32S  audio_wav_dec_process(void);
#endif

#if APP_MP3_DECODE_FG_EN == 1
static INT32S  audio_mp3_play_init(void);
static INT32S  audio_mp3_process(void);
int MP3Parser_Init(INT16S );
int MP3Parser_Seek( INT32U *p_msec,INT32U vbr);
#endif

#if APP_A1800_DECODE_FG_EN == 1
static INT32S  audio_a1800_play_init(void);//080724
static INT32S  audio_a1800_process(void);//080724
#endif

#if APP_WMA_DECODE_FG_EN == 1
static INT32S fg_error_cnt;
static INT32S  audio_wma_play_init(void);
static INT32S  audio_wma_process(void);
#endif

// added by Bruce, 2008/09/23
#if APP_A1600_DECODE_FG_EN == 1
static INT32S  audio_a16_play_init(void);
static INT32S  audio_a16_process(void);
#endif

#if APP_A6400_DECODE_FG_EN == 1
static INT32S  audio_a64_play_init(void);
static INT32S  audio_a64_process(void);
#endif

#if APP_S880_DECODE_FG_EN == 1
static INT32S  audio_s880_play_init(void);
static INT32S  audio_s880_process(void);
#endif

#if APP_AAC_DECODE_FG_EN == 1
static INT32S  audio_aac_play_init(void);
static INT32S  audio_aac_process(void);
#endif

//static INT32U  audio_write_buffer(INT16S fd, INT8U *ring_buf, INT32U wi, INT32U ri);
static INT32S  audio_send_to_dma(void);

#if APP_MP3_DECODE_FG_EN == 1 || APP_WMA_DECODE_FG_EN == 1 || APP_WAV_CODEC_FG_EN == 1 || APP_A1600_DECODE_FG_EN == 1 || APP_A1800_DECODE_FG_EN == 1 || APP_A6400_DECODE_FG_EN == 1 || APP_S880_DECODE_FG_EN == 1 
static INT32S  audio_write_with_file_srv(INT8U *ring_buf, INT32U wi, INT32U ri);
static INT32S  audio_check_wi(INT32S wi_in, INT32U *wi_out);
#endif

//static INT32S  audio_avi_read(INT32U buf_addr, INT32U len);
static INT32S  audio_avi_write(INT8U *ring_buf, INT32U wi, INT32U ri);
#if APP_MP3_DECODE_FG_EN == 1
static INT32S audio_avi_mp3_write(INT8U *ring_buf, INT32U wi, INT32U ri);
#endif
static void    audio_queue_clear(void);
static void    audio_stop_unfinished(void);
static void    audio_send_next_frame_q(void);
static void    audio_ramp_down(void);
static void    audio_start(STAudioTaskPara *pAudioTaskPara);
static void    audio_pause(STAudioTaskPara *pAudioTaskPara);
static void    audio_resume(STAudioTaskPara *pAudioTaskPara);
static void    audio_stop(STAudioTaskPara *pAudioTaskPara);
static void    audio_avi_start(STAudioTaskPara *pAudioTaskPara);
static void    audio_decode_next_frame(STAudioTaskPara *pAudioTaskPara);
static void    audio_mute_set(STAudioTaskPara *pAudioTaskPara);
static void    audio_volume_set(STAudioTaskPara *pAudioTaskPara);

#if (defined AUDIO_FORMAT_JUDGE_AUTO) && (AUDIO_FORMAT_JUDGE_AUTO == 1)
static INT32S audio_get_type(INT16S fd,INT8S* file_name, OS_EVENT *ack_fsq);
#else
static INT32S  audio_get_type(INT8S* file_name);
#endif

static INT32S  audio_q_check(void);

#if USE_RAMP_DOWN_RAPID == 1
static void audio_ramp_down_rapid(void);
#endif

#if (defined SKIP_ID3_TAG) && (SKIP_ID3_TAG == 1)
static INT8U audio_get_id3_type(INT8U *data, INT32U length);
static void audio_parse_id3_header(INT8U *header, INT32U *version, INT32S *flags, INT32U *size);
static INT32U audio_id3_get_size(INT8U *ptr);
INT32S audio_id3_get_tag_len(INT8U *data, INT32U length);
#endif

void audio_task_init(void)
{
    /* Create MsgQueue/MsgBox for TASK */
    AudioTaskQ = msgQCreate(AUDIO_QUEUE_MAX, AUDIO_QUEUE_MAX, AUDIO_PARA_MAX_LEN);
    audio_wq = OSQCreate(write_q, AUDIO_WRITE_Q_SIZE);
	audio_fsq = OSQCreate(fs_q, AUDIO_FS_Q_SIZE);
	
	avi_state_mon = &audio_context.state; 
}

void audio_task_entry(void *p_arg)
{
    INT32U  msg_id;
	STAudioTaskPara     *pstAudioTaskPara;


	audio_task_init();
	audio_init();

	while (1)
	{
	    /* Pend task message */
	    msgQReceive(AudioTaskQ, &msg_id, (void*)audio_para, AUDIO_PARA_MAX_LEN);
	    pstAudioTaskPara = (STAudioTaskPara*) audio_para;

		switch(msg_id) {
			case MSG_AUD_PLAY: /* by file handle */
			case MSG_AUD_PLAY_BY_SPI:
				audio_start(pstAudioTaskPara);
				break;
			case MSG_AUD_STOP:
				audio_stop(pstAudioTaskPara);
				break;
			case MSG_AUD_PAUSE:
				audio_pause(pstAudioTaskPara);
				break;
			case MSG_AUD_RESUME:
				audio_resume(pstAudioTaskPara);
				break;
			case MSG_AUD_AVI_PLAY:
				audio_avi_start(pstAudioTaskPara);
				break;
			case MSG_AUD_SET_MUTE:
				audio_mute_set(pstAudioTaskPara);
				break;
			case MSG_AUD_VOLUME_SET:
				audio_volume_set(pstAudioTaskPara);
				break;
			case MSG_AUD_DECODE_NEXT_FRAME:
				audio_decode_next_frame(pstAudioTaskPara);
				break;
			default:
				break;
		}
	}
}

static void audio_init(void)
{
	audio_ctrl.ring_buf = (INT8U*) gp_iram_malloc(RING_BUF_SIZE);
	if (audio_ctrl.ring_buf == NULL) {
		audio_ctrl.ring_buf = (INT8U*) gp_malloc(RING_BUF_SIZE);
	}	
	audio_ctrl.ring_size = RING_BUF_SIZE;

	audio_ctrl.wi = 0;
	audio_ctrl.ri = 0;
	audio_context_p->state = AUDIO_PLAY_STOP;
	stopped = 1;
	audio_rampdown_disable_flag = 0;
	decode_end = NULL;
}

static void audio_start(STAudioTaskPara *pAudioTaskPara)
{
	INT32S ret;

	if (audio_context_p->state != AUDIO_PLAY_STOP) {
		audio_stop_unfinished();
	}

	stopped = 1;
	audio_queue_clear();

	audio_context_p->source_type = pAudioTaskPara->src_type;

	if (audio_context_p->source_type == AUDIO_SRC_TYPE_FS) {
		ret = audio_play_file_set(pAudioTaskPara->fd, audio_context_p, &audio_ctrl, audio_fsq);
	}
	else if (audio_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)	// added by Bruce, 2008/10/27
	{
		//ret = audio_play_file_set(pAudioTaskPara->fd, audio_context_p, &audio_ctrl, audio_fsq);
		
		// modified by Bruce, 2008/10/30
		audio_context_p->audio_format = pAudioTaskPara->audio_format;
	   	if (audio_context_p->audio_format == AUDIO_TYPE_NONE) 
	   	{
		   DBG_PRINT("audio_play_file_set find not support audio.\r\n");
	   	}
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	   	audio_ctrl.file_handle = pAudioTaskPara->fd;//081127
#endif
		audio_ctrl.file_len = pAudioTaskPara->file_len;
		ret = AUDIO_ERR_NONE;
	}
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	else if(audio_context_p->source_type == AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE)	// added by Bruce, 2010/01/22
	{
		//DBG_PRINT("Play an Audio in a big file...\n\r");
		audio_context_p->audio_format = pAudioTaskPara->audio_format;
	   	if (audio_context_p->audio_format == AUDIO_TYPE_NONE) 
	   	{
		   DBG_PRINT("audio_play_file_set find not support audio.\r\n");
	   	}
		audio_ctrl.file_handle = pAudioTaskPara->fd;
		audio_ctrl.file_len = pAudioTaskPara->file_len;
		ret = AUDIO_ERR_NONE;
	}
#endif
	else {
		ret = AUDIO_ERR_NONE;
		audio_context_p->audio_format = pAudioTaskPara->audio_format;
		audio_ctrl.file_len = pAudioTaskPara->file_len;
		audio_ctrl.file_handle = pAudioTaskPara ->fd;  
	}

	if (ret == AUDIO_ERR_NONE) {
		if (audio_mem_alloc(audio_context_p->audio_format,&audio_ctrl,pcm_out) != AUDIO_ERR_NONE) {
			DBG_PRINT("audio memory allocate fail\r\n");
			audio_send_result(MSG_AUD_PLAY_RES,AUDIO_ERR_MEM_ALLOC_FAIL);
			return;
		}
		audio_context_p->state = AUDIO_PLAYING;

		switch(audio_context_p->audio_format) {
		#if APP_MP3_DECODE_FG_EN == 1
			case AUDIO_TYPE_MP3:
				audio_context_p->fp_deocde_init = audio_mp3_play_init;
				audio_context_p->fp_deocde = audio_mp3_process;
				//audio_ctrl.ring_size = MP3_RING_BUF_SIZE; /* change ring size */
				audio_ctrl.frame_size = MP3_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_WAV_CODEC_FG_EN == 1
			case AUDIO_TYPE_WAV:
				audio_context_p->fp_deocde_init = audio_wav_dec_play_init;
				audio_context_p->fp_deocde = audio_wav_dec_process;
				audio_ctrl.frame_size = WAV_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_WMA_DECODE_FG_EN == 1
			case AUDIO_TYPE_WMA:
				audio_context_p->fp_deocde_init = audio_wma_play_init;
				audio_context_p->fp_deocde = audio_wma_process;
				//audio_ctrl.ring_size = RING_BUF_SIZE; /* change ring size */
				audio_ctrl.frame_size = WMA_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_A1800_DECODE_FG_EN == 1
			case AUDIO_TYPE_A1800://080722
				audio_context_p->fp_deocde_init = audio_a1800_play_init;
				audio_context_p->fp_deocde = audio_a1800_process;
				audio_ctrl.frame_size = A18_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_A1600_DECODE_FG_EN == 1
			case AUDIO_TYPE_A1600:			// added by Bruce, 2008/09/23
				audio_context_p->fp_deocde_init = audio_a16_play_init;
				audio_context_p->fp_deocde = audio_a16_process;
				audio_ctrl.frame_size = A16_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_A6400_DECODE_FG_EN == 1
			case AUDIO_TYPE_A6400:			// added by Bruce, 2008/09/25
				audio_context_p->fp_deocde_init = audio_a64_play_init;
				audio_context_p->fp_deocde = audio_a64_process;
				audio_ctrl.frame_size = A6400_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_S880_DECODE_FG_EN == 1
			case AUDIO_TYPE_S880:			// added by Bruce, 2008/09/25
				audio_context_p->fp_deocde_init = audio_s880_play_init;
				audio_context_p->fp_deocde = audio_s880_process;
				audio_ctrl.frame_size = S880_DEC_FRAMESIZE;
				break;
		#endif
		#if APP_AAC_DECODE_FG_EN == 1
			case AUDIO_TYPE_AAC:			// added by George, 2008/09/25
				audio_context_p->fp_deocde_init = audio_aac_play_init;
				audio_context_p->fp_deocde = audio_aac_process;
				audio_ctrl.frame_size = AAC_DEC_FRAMESIZE;
				audio_ctrl.ring_size = RING_BUF_SIZE; /* change ring size */

				break;
		#endif
			default:
				audio_send_result(MSG_AUD_BG_PLAY_RES,AUDIO_ERR_INVALID_FORMAT);
				return;
		}
		audio_ctrl.ring_size = RING_BUF_SIZE;
		
		//dac_enable_set(TRUE); /* enable dac */
		ret = audio_context_p->fp_deocde_init();
		if (ret != AUDIO_ERR_NONE) {
			audio_stop_unfinished();
			DBG_PRINT("audio play init failed\r\n");
        }
        else {
        	audio_send_next_frame_q();
		}
	}
	else {
   		ret = AUDIO_ERR_INVALID_FORMAT;
	}

	audio_send_result(MSG_AUD_PLAY_RES,ret);
}

static void audio_stop(STAudioTaskPara *pAudioTaskPara)
{
	if ((audio_context_p->state == AUDIO_PLAY_STOP)||(audio_context_p->state == AUDIO_IDLE)) {
//#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_STOP_RES,AUDIO_ERR_NONE);
//#endif
		return;
	}
	
	if(audio_rampdown_disable_flag == 0) {
		if (audio_context_p->state == AUDIO_PLAYING) {
	#if USE_RAMP_DOWN_RAPID == 0	
			audio_ramp_down();		//ramp down speed normal
	#else
			if((audio_context_p->audio_format == AUDIO_TYPE_AVI_MP3)||(audio_context_p->audio_format == AUDIO_TYPE_AVI))
				audio_ramp_down();	//ramp down speed normal
			else
				audio_ramp_down_rapid();
	#endif
		}
	}
	
	audio_stop_unfinished();
	audio_send_result(MSG_AUD_STOP_RES,AUDIO_ERR_NONE);
}

static void audio_ramp_down(void)
{
	INT8U   wb_idx;
	INT8U   err, ramp_down_step;
	INT16S  *ptr;
	INT16S  last_ldata,last_rdata;
	INT32U  i,j;
	
	if(g_audio_sample_rate > 16000)
		ramp_down_step = RAMP_DOWN_STEP;
	else
		ramp_down_step = RAMP_DOWN_STEP_LOW_SR;
	
	OSQFlush(aud_send_q);

	last_ldata = *(pcm_out[last_send_idx] + pcm_len[last_send_idx]-2);
	last_rdata = *(pcm_out[last_send_idx] + pcm_len[last_send_idx]-1);
	
	last_ldata = last_ldata & ~(ramp_down_step-1);
	if (channel == 2) {
		last_rdata = last_rdata & ~(ramp_down_step-1);
	}
	else {
		last_rdata = 0;
	}
	
	DBG_PRINT("ldata = 0x%x\r\n",last_ldata);
	DBG_PRINT("rdata = 0x%x\r\n",last_rdata);

	while(1) {
		
		wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
		ptr = (INT16S*) pcm_out[wb_idx];
		audio_send_to_dma();
		
		for (i=0;i<audio_ctrl.frame_size/RAMP_DOWN_STEP_HOLD;i++) {
			for (j=0;j<RAMP_DOWN_STEP_HOLD;j++) {
				*ptr++ = last_ldata;
				if (channel == 2) {
					*ptr++ = last_rdata;
		    }
	    }

			if (last_ldata > 0x0) {
				last_ldata -= ramp_down_step;
			}
			else if (last_ldata < 0x0) {
				last_ldata += ramp_down_step;
			}

			if (channel == 2) {
				if (last_rdata > 0x0) {
					last_rdata -= ramp_down_step;
		        }
				else if (last_rdata < 0x0) {
					last_rdata += ramp_down_step;
		        }
		    }
	    }

		pcm_len[wb_idx] = audio_ctrl.frame_size*channel;
		
		OSQPost(aud_send_q, (void *) wb_idx);
		if (dac_dma_status_get() == 0) {
			OSQPost(hAudioDacTaskQ,(void *) MSG_AUD_DMA_DBF_RESTART);
		}
		
		if ((last_ldata == 0x0) && (last_rdata == 0x0)) {
			break;
		}
	}
}

#if USE_RAMP_DOWN_RAPID == 1	
static void audio_ramp_down_rapid(void)
{
	INT16S  last_ldata,last_rdata;
	INT16S  i, temp;
	
	OSQFlush(aud_send_q);
	while(dac_dma_status_get() == 1) {
		OSTimeDly(2);
	}
	//free and reset DMA channel 
	dac_dbf_free();
	
	temp = 0 - RAMP_DOWN_STEP;
	last_ldata = R_DAC_CHA_DATA;
	last_rdata = R_DAC_CHB_DATA;
	//unsigned to signed 
	last_ldata ^= 0x8000;
	if(channel == 2) {
		last_rdata ^= 0x8000;
	}
	else {
		last_rdata = 0x0;
	}
	//change timer to 44100
	dac_sample_rate_set(44100);
	
	while(1)
	{
		if (last_ldata > 0x0) {
			last_ldata -= RAMP_DOWN_STEP;
		}
		else if (last_ldata < 0x0) {
			last_ldata += RAMP_DOWN_STEP;
		}
			
		if ((last_ldata < RAMP_DOWN_STEP)&&(last_ldata > temp)) { 
			last_ldata = 0;
		}

		if (channel == 2) {
			if (last_rdata > 0x0) {
				last_rdata -= RAMP_DOWN_STEP;
		    }
			else if (last_rdata < 0x0) {
				last_rdata += RAMP_DOWN_STEP;
		    }
		        
		    if ((last_rdata < RAMP_DOWN_STEP)&&(last_rdata > temp)) {  
				last_rdata = 0;
			}
		}
		    
		for(i=0;i<RAMP_DOWN_STEP_HOLD;i++) {
			if (channel == 2){
				while(R_DAC_CHA_FIFO & 0x8000);
				R_DAC_CHA_DATA = last_ldata;
				while(R_DAC_CHB_FIFO & 0x8000);
				R_DAC_CHB_DATA = last_rdata;
			} else {
				while(R_DAC_CHA_FIFO & 0x8000);
				R_DAC_CHA_DATA = last_ldata;
			}
		}
		
		if ((last_ldata == 0x0) && (last_rdata == 0x0)) {
			break;
		}
	}
}
#endif //USE_RAMP_DOWN_RAPID
/*== AVI audio ctrl Setting explain ==*/
// audio_ctrl.f_last => 1:it mean buffer read file over. 0: it mean buffer read file continue
// when audio play over ,the audio_ctrl.ri == audio_ctrl.wi.
/*== ==*/

static void audio_avi_start(STAudioTaskPara *pAudioTaskPara)
{
	INT32U i;
	
	if (audio_context_p->state != AUDIO_PLAY_STOP) {
		audio_stop_unfinished();
	}

	stopped = 1;
	audio_queue_clear();

	audio_ctrl.file_handle = -1;
	audio_ctrl.f_last = 0;
	
	for(i = 0;i< MAX_DAC_BUFFERS;i++)
		pcm_len[i]=0;
	
#if APP_MP3_DECODE_FG_EN == 1
	if(tk_avi_wave_format_type_get()==0x0055)
	{
		audio_context_p->audio_format = AUDIO_TYPE_AVI_MP3;
		audio_ctrl.frame_size = MP3_DEC_FRAMESIZE;
		audio_context_p->fp_deocde_init = audio_avi_mp3_play_init;
		audio_context_p->fp_deocde = audio_avi_mp3_process;
	}
	else
	{
		audio_context_p->audio_format = AUDIO_TYPE_AVI;
		audio_ctrl.frame_size = WAV_DEC_FRAMESIZE;
		audio_context_p->fp_deocde_init = audio_avi_play_init;
		audio_context_p->fp_deocde = audio_avi_process;
	}
#else
		audio_context_p->audio_format = AUDIO_TYPE_AVI;
		audio_ctrl.frame_size = WAV_DEC_FRAMESIZE;
		audio_context_p->fp_deocde_init = audio_avi_play_init;
		audio_context_p->fp_deocde = audio_avi_process;
#endif	
	
	if (audio_mem_alloc(audio_context_p->audio_format,&audio_ctrl,pcm_out) != AUDIO_ERR_NONE) {
		DBG_PRINT("audio memory allocate fail\r\n");
		audio_send_result(MSG_AUD_PLAY_RES,AUDIO_ERR_MEM_ALLOC_FAIL);
		return;
	}
	audio_context_p->state = AUDIO_PLAYING;

	//dac_enable_set(TRUE);
	audio_context_p->fp_deocde_init();
	audio_send_next_frame_q();
	audio_send_result(MSG_AUD_PLAY_RES,AUDIO_ERR_NONE);
}

static void audio_pause(STAudioTaskPara *pAudioTaskPara)
{
	if (audio_context_p->state != AUDIO_PLAYING) {
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_PAUSE_RES, AUDIO_ERR_NONE);
#endif	
		return;
	}
	
	OSQPost(hAudioDacTaskQ, (void *)MSG_AUD_DMA_PAUSE);
	
	while(dac_dma_status_get() == 1) {
		OSTimeDly(2);
	}
	
	stopped = 1;
	dac_dbf_free();
	//dac_enable_set(FALSE);
	
	audio_context_p->state = AUDIO_PLAY_PAUSE;
	audio_send_result(MSG_AUD_PAUSE_RES, AUDIO_ERR_NONE);
}

static void audio_resume(STAudioTaskPara *pAudioTaskPara)
{
	if (audio_context_p->state != AUDIO_PLAY_PAUSE) {
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_RESUME_RES, AUDIO_ERR_NONE);
#endif		
		return;
	}
	//dac_enable_set(TRUE);
	audio_context_p->state = AUDIO_PLAYING;
	audio_send_next_frame_q();
	audio_send_result(MSG_AUD_RESUME_RES,AUDIO_ERR_NONE);
}

static void audio_mute_set(STAudioTaskPara *pAudioTaskPara)
{
	if (pAudioTaskPara->mute == TRUE) {
		dac_pga_set(0);
		dac_vref_set(FALSE);
	}
	else {
		dac_pga_set(pAudioTaskPara->volume);
		dac_vref_set(TRUE);
	}
	audio_send_result(MSG_AUD_MUTE_SET_RES,AUDIO_ERR_NONE);
}

static void audio_volume_set(STAudioTaskPara *pAudioTaskPara)
{
	if (pAudioTaskPara->volume < 64) {
		dac_pga_set(pAudioTaskPara->volume);
	}
	audio_send_result(MSG_AUD_VOLUME_SET_RES,AUDIO_ERR_NONE);
}

static void audio_decode_next_frame(STAudioTaskPara *pAudioTaskPara)
{
	INT32S ret;
	if (audio_context_p->state != AUDIO_PLAYING) {
		return;
	}
	ret = audio_context_p->fp_deocde();
	if (ret != 0) {
		audio_stop_unfinished();
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
		audio_send_result(MSG_AUD_PLAY_RES,ret);
#endif
		if (decode_end != NULL) {
			decode_end(1);	// added by Bruce, 2008/10/03
		}
	}
}

static void audio_send_next_frame_q(void)
{
	msgQSend(AudioTaskQ, MSG_AUD_DECODE_NEXT_FRAME, NULL, 0, MSG_PRI_NORMAL);
}

void audio_send_result(INT32S res_type,INT32S result)
{
	aud_con.result_type = res_type;
	aud_con.result = result;
	DBG_PRINT("audio_send_result :res_type =  %x,result = %d\r\n",res_type,result);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY) 
	msgQSend(ApQ, EVENT_APQ_ERR_MSG, (void *)&aud_con, sizeof(STAudioConfirm), MSG_PRI_NORMAL);
#else
	msgQSend(Audio_FG_status_Q, EVENT_APQ_ERR_MSG, (void *)&aud_con, sizeof(STAudioConfirm), MSG_PRI_NORMAL);
#endif
}

static void audio_stop_unfinished(void)
{
	INT32S i;

	/* wait until dma finish */
	while(dac_dma_status_get() == 1) {
		OSTimeDly(2);
	}

	if (  audio_context_p->source_type == AUDIO_SRC_TYPE_FS
	    ||audio_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE) {
		if (audio_ctrl.file_handle >= 0) {
			//close(audio_ctrl.file_handle); //who open ,who close
		}
	}
	dac_timer_stop();
	dac_dbf_free(); /* release dma channel */
	//dac_enable_set(FALSE);

	/* free memory */
	if(audio_context_p->audio_format != AUDIO_TYPE_AVI)
	{
		gp_free(audio_ctrl.work_mem);
	}
	audio_ctrl.work_mem = NULL;

	for (i=0;i<MAX_DAC_BUFFERS;i++) {
		gp_free(*(pcm_out+i));
		*(pcm_out+i) = NULL;
	}

	audio_context_p->state = AUDIO_PLAY_STOP;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
	audio_play_start_time = 0;
#endif
}

INT32S audio_mem_alloc(INT32U audio_format,AUDIO_CTRL *aud_ctrl, INT16S *pcm[])
{
	INT32S i;
	INT32U pcm_size;
	INT32U wm_size;

	switch(audio_format) {
	#if APP_MP3_DECODE_FG_EN == 1 || APP_MP3_DECODE_BG_EN == 1
		case AUDIO_TYPE_MP3:
		#if GPLIB_MP3_HW_EN == 1
			wm_size = MP3_DEC_MEMORY_SIZE;
		#else
			wm_size = MP3_DEC_MEMORY_SIZE + MP3_DECODE_RAM;
		#endif
			pcm_size = MP3_PCM_BUF_SIZE;
			break;
	#endif
	#if APP_WMA_DECODE_FG_EN == 1 || APP_WMA_DECODE_BG_EN == 1
		case AUDIO_TYPE_WMA:
			wm_size = WMA_DEC_MEMORY_SIZE;
			pcm_size = WMA_PCM_BUF_SIZE;
			break;
	#endif
	#if APP_WAV_CODEC_FG_EN == 1 || APP_WAV_CODEC_BG_EN == 1
		case AUDIO_TYPE_WAV:
			wm_size = WAV_DEC_MEMORY_SIZE;
			pcm_size = WAV_PCM_BUF_SIZE;
			break;
	#endif
		case AUDIO_TYPE_AVI:
			wm_size = WAV_DEC_MEMORY_SIZE;
			pcm_size = WAV_PCM_BUF_SIZE;
			break;
		case AUDIO_TYPE_AVI_MP3:
			wm_size = MP3_DEC_MEMORY_SIZE;
			pcm_size = MP3_PCM_BUF_SIZE;
			break;		
	#if APP_A1800_DECODE_FG_EN == 1 || APP_A1800_DECODE_BG_EN == 1
		case AUDIO_TYPE_A1800://080722
			wm_size = A1800DEC_MEMORY_BLOCK_SIZE;
			pcm_size = A1800_PCM_BUF_SIZE;
			break;
	#endif
	#if APP_A1600_DECODE_FG_EN == 1 || APP_A1600_DECODE_BG_EN == 1
 	  	case AUDIO_TYPE_A1600:			// added by Bruce, 2008/09/23
  	  		wm_size = A16_DEC_MEMORY_SIZE;
  	  		pcm_size = A16_DEC_BITSTREAM_BUFFER_SIZE;
  	  		break;
	#endif
	#if APP_A6400_DECODE_FG_EN == 1 || APP_A6400_DECODE_BG_EN == 1
 	  	case AUDIO_TYPE_A6400:			// added by Bruce, 2008/09/25
 	  	#if GPLIB_MP3_HW_EN == 1
  	  		wm_size = A6400_DEC_MEMORY_SIZE;
		#else
			wm_size = A6400_DEC_MEMORY_SIZE + A6400_DECODE_RAM;
		#endif
  	  		pcm_size = A6400_DEC_BITSTREAM_BUFFER_SIZE;
  	  		break;
	#endif
	#if APP_S880_DECODE_FG_EN == 1 || APP_S880_DECODE_BG_EN == 1
 	  	case AUDIO_TYPE_S880:			// added by Bruce, 2008/09/25
  	  		wm_size = S880_DEC_MEMORY_SIZE;
  	  		pcm_size = S880_DEC_BITSTREAM_BUFFER_SIZE;
  	  		break;
	#endif
	#if APP_AAC_DECODE_FG_EN == 1 || APP_AAC_DECODE_BG_EN == 1
 	  	case AUDIO_TYPE_AAC:			// added by George, 2009/8/13
  	  		wm_size = AAC_DEC_MEMORY_BLOCK_SIZE;
  	  		pcm_size = AAC_PCM_BUF_SIZE;
  	  		break;  	  		
	#endif
		default:
			//return AUDIO_ERR_FAILED;
			return AUDIO_ERR_NONE;//0809003
	}

	if(audio_format == AUDIO_TYPE_AVI)
	{
		aud_ctrl->work_mem = avi_work_mem;
	}
	else
	{
		aud_ctrl->work_mem = (INT8S*) gp_malloc(wm_size);
		if (aud_ctrl->work_mem == NULL) {
			return AUDIO_ERR_FAILED;
		}
	}
	gp_memset(aud_ctrl->work_mem,0,wm_size);
	DBG_PRINT("decode memory -- 0x%x\r\n",aud_ctrl->work_mem);

	pcm_size += 2 /* add for SPU end data */;

	for (i=0;i<MAX_DAC_BUFFERS;i++) {
		pcm[i] = (INT16S*) gp_malloc(pcm_size*2);
		//DBG_PRINT("pcm buffer[%d] = 0x%x (%d)\r\n",i,pcm[i],pcm_size*2);
		if (pcm[i] == NULL) {
			while(i>0) {
				gp_free(pcm[--i]);
				pcm[i] = NULL;
			}
			return AUDIO_ERR_FAILED;
		}
	}

	return AUDIO_ERR_NONE;
}

INT32S audio_play_file_set(INT16S fd, AUDIO_CONTEXT *aud_context, AUDIO_CTRL *aud_ctrl, OS_EVENT *ack_fsq)
{
#if (defined SKIP_ID3_TAG) && (SKIP_ID3_TAG == 1)
	INT8U id3[10];
	INT32U len;
#endif
	
	sfn_stat(fd,&aud_sfn_file);
#if (defined AUDIO_FORMAT_JUDGE_AUTO) && (AUDIO_FORMAT_JUDGE_AUTO == 1) 
	aud_context->audio_format = audio_get_type(fd, aud_sfn_file.f_extname, ack_fsq);
#else
	aud_context->audio_format = audio_get_type(aud_sfn_file.f_extname);
#endif
   	if (aud_context->audio_format == AUDIO_TYPE_NONE) {
	   DBG_PRINT("audio_play_file_set find not support audio.\r\n");
   	   return AUDIO_ERR_INVALID_FORMAT;
   	}
	aud_ctrl->file_handle = fd;
	
#if (defined SKIP_ID3_TAG) && (SKIP_ID3_TAG == 1) /* skip id3 */
    if (aud_context->audio_format == AUDIO_TYPE_MP3) {
    	read(fd,(INT32U)id3,10);
    	len = audio_id3_get_tag_len(id3, 10);
    	aud_ctrl->file_len = aud_sfn_file.f_size-(len&~3);
   	   	lseek(fd,0,SEEK_SET);
   	   	lseek(fd,(len&~3),SEEK_SET); /* 4 byte alignment */
   	}
   	else {
    	aud_ctrl->file_len = aud_sfn_file.f_size;
    }
#else
	aud_ctrl->file_len = aud_sfn_file.f_size;
#endif	

	return AUDIO_ERR_NONE;
}

#if (defined AUDIO_FORMAT_JUDGE_AUTO) && (AUDIO_FORMAT_JUDGE_AUTO == 1)
#if APP_WAV_CODEC_FG_EN == 1 || APP_WAV_CODEC_BG_EN == 1 || APP_MP3_DECODE_FG_EN == 1 || APP_MP3_DECODE_BG_EN == 1 || APP_WMA_DECODE_FG_EN == 1 || APP_WMA_DECODE_BG_EN == 1
static  INT8S parse_mp3_file_head(INT8S *p_audio_file_head_ptr)
{
	INT8S  mpeg_version, layer,sample_rate;
	INT8S j =0; 
	INT32U ID3V2_length = 0;
	INT32U cnt  = AUDIO_PASER_BUFFER_SIZE;
	MP3_FILE_HEAD  mp3_file_head[2];
	//INT8S *p_audio_file_head;
	INT8U *p_audio_file_head;

	p_audio_file_head = (INT8U *)p_audio_file_head_ptr;
	
	if(*(p_audio_file_head) == (INT8S)'I' && *(p_audio_file_head + 1) == (INT8S)'D' && *(p_audio_file_head + 2) == (INT8S)'3' )
	{
		ID3V2_length = 10 + ((*(p_audio_file_head + 9)& 0x7f)|((*(p_audio_file_head + 8) & 0x7f)<<7)|((*(p_audio_file_head + 7) & 0x7f)<<14)|((*(p_audio_file_head + 6) & 0x7f)<<21));
		//ID3V2_length += 10 + ((*(p_audio_file_head + 6) & 0x7f)<<21);
		//ID3V2_length += ((*(p_audio_file_head + 7) & 0x7f)<<14);
		//ID3V2_length += ((*(p_audio_file_head + 8) & 0x7f)<<7);
		//ID3V2_length +=  (*(p_audio_file_head + 9)& 0x7f);
		
		/*if(ID3V2_length > 1024)
		{
			DBG_PRINT("audio file ID3V2 too long \r\n");
			return AUDIO_PARSE_SUCCS ;
		}
		else
		{
			p_audio_file_head +=  ID3V2_length;
			cnt -= ID3V2_length;
		}
		*/
		DBG_PRINT("audio file parse succes \r\n");
		return AUDIO_PARSE_SUCCS ;
	}

	while (cnt > 4)  
	{
		if((*(p_audio_file_head) == 0xFF ) && ((*(p_audio_file_head + 1)&0xE0) == 0xE0 )  && ((*(p_audio_file_head + 2)&0xF0 )!= 0xF0 ))  // first 11 bits should be 1
		{

			mpeg_version =( *(p_audio_file_head + 1)&0x18)>>3;
			layer = (*(p_audio_file_head + 1)&0x06)>>1;
			sample_rate = (*(p_audio_file_head + 2)&0x0c)>>2;

			if((mpeg_version != 0x01) && (layer != 0x00) && (layer != 0x03) && (sample_rate != 0x03))   // != RESERVERD 
			{
				if(j<2)
				{
					mp3_file_head[j].mpeg_version = mpeg_version;
					mp3_file_head[j].layer = layer;
					mp3_file_head[j].sample_rate = sample_rate;

					j++;
				}
				else if ((mp3_file_head[0].mpeg_version == mp3_file_head[1].mpeg_version) && (mp3_file_head[0].layer == mp3_file_head[1].layer) && (mp3_file_head[0].sample_rate == mp3_file_head[1].sample_rate))
				{		
					DBG_PRINT("audio file parse succes \r\n");
					return  AUDIO_PARSE_SUCCS ;		
				}
		
			}
			else
			{
				p_audio_file_head +=  4;
				cnt -= 4 ;
			}
			
		}
		else
		{
			p_audio_file_head +=  4;
			cnt -= 4 ;

		}

	}

	return AUDIO_PARSE_FAIL ;

}

static  INT8S parse_wma_file_head(INT8S *p_audio_file_head)
{

	INT32U *pData = (INT32U *)p_audio_file_head;
	
	if(*pData++ != 0x75B22630)
		return AUDIO_PARSE_FAIL;
	else if(*pData++ != 0x11CF668E)
		return AUDIO_PARSE_FAIL;
	else if(*pData++ != 0xAA00D9A6)
		return AUDIO_PARSE_FAIL;
	else if(*pData++ != 0x6CCE6200)
		return AUDIO_PARSE_FAIL;
	
	return AUDIO_PARSE_SUCCS ;
/*	        
	if( *(p_audio_file_head) != 0x30 ||*(p_audio_file_head + 1) != 0x26 ||*(p_audio_file_head + 2) != 0xB2 ||*(p_audio_file_head + 3) != 0x75  )
	{
		return AUDIO_PARSE_FAIL ;
	}
	else if( *(p_audio_file_head + 4) != 0x8E  ||*(p_audio_file_head + 5) != 0x66 ||*(p_audio_file_head + 6) != 0xCF ||*(p_audio_file_head + 7) != 0x11 )
	{		
		return AUDIO_PARSE_FAIL ;	
	}
	else if( *(p_audio_file_head + 8) != 0xA6  ||*(p_audio_file_head + 9) != 0xD9 ||*(p_audio_file_head + 10) != 0x00 ||*(p_audio_file_head + 11) != 0xAA )
	{		
		return AUDIO_PARSE_FAIL ;	
	}
	else if(*(p_audio_file_head + 12) != 0x00 ||*(p_audio_file_head + 13) != 0x62 ||*(p_audio_file_head + 14) != 0xCE ||*(p_audio_file_head + 15) != 0x6C )
	{
		return AUDIO_PARSE_FAIL ;
	}
	else
	{
		return AUDIO_PARSE_SUCCS ;
	}
*/	
}

static  INT8S parse_wav_file_head(INT8S *p_audio_file_head)
{
	INT16S wav_format_type;
	
	if( *(p_audio_file_head) != (INT8S)'R' || *(p_audio_file_head + 1) != (INT8S)'I' || *(p_audio_file_head + 2) != (INT8S)'F' || *(p_audio_file_head + 3) != (INT8S)'F' )
	{
		return AUDIO_PARSE_FAIL ;
	}
	else if( *(p_audio_file_head + 8) != (INT8S)'W' || *(p_audio_file_head + 9) !=(INT8S)'A' || *(p_audio_file_head + 10) != (INT8S)'V'|| *(p_audio_file_head + 11) != (INT8S)'E')
	{		
		return AUDIO_PARSE_FAIL ;	
	}
	else if(*(p_audio_file_head + 12) != (INT8S)'f' || *(p_audio_file_head + 13) != (INT8S)'m' || *(p_audio_file_head + 14) != (INT8S)'t' || *(p_audio_file_head + 15) != 0x20 )
	{
		return AUDIO_PARSE_FAIL ;
	}
	else
	{		//add for mantis #9116  090612 by Lion
		wav_format_type = (INT16S)((*(p_audio_file_head + 21))<<8);
		wav_format_type += *(p_audio_file_head + 20);  
		
		if( wav_format_type == WAVE_FORMAT_PCM || wav_format_type == WAVE_FORMAT_ADPCM || wav_format_type == WAVE_FORMAT_ALAW ||wav_format_type == WAVE_FORMAT_MULAW ||wav_format_type == WAVE_FORMAT_IMA_ADPCM )
		{
					
			return AUDIO_PARSE_SUCCS ;
		}
		else        //for example  WAVE_FORMAT_MPEGLAYER3   
		{
			return AUDIO_PARSE_FAIL ;
		}
		
	}
	
	/*
	INT8S  temp[9] = {0};
	INT16U 	i;
	gp_memcpy(temp,p_audio_file_head,8);

	for (i=0;i<gp_strlen(temp);i++) {
		temp[i] = gp_toupper(temp[i]);
	}

	if((gp_strcmp(temp, (INT8S *)"RIFFWAVE")==0)&&(gp_strcmp((p_audio_file_head + 12), (INT8S *)"fmt ")==0))
	{
		return AUDIO_PARSE_SUCCS ;
	}
	else
	{
		return AUDIO_PARSE_FAIL ;
	}*/
}

static INT32S audio_real_type_get(INT16S fd, INT8S type_index, OS_EVENT *ack_fsq)
{
	INT8U err;
	INT16U 	i;
	INT32U  audio_file_head;
	INT32S len;
	AUDIO_TYPE audio_real_type;
	TK_FILE_SERVICE_STRUCT audio_file_head_para;
	AUDIO_FILE_PARSE audio_file_parse_head[AUDIO_MAX_TYPE];

	if (( 0 == type_index ) || (type_index > AUDIO_MAX_TYPE -1))
	{
		return type_index;
	}

	audio_file_parse_head[AUDIO_TYPE_MP3].audio_file_real_type = AUDIO_TYPE_MP3;
	audio_file_parse_head[AUDIO_TYPE_MP3].parse_audio_file_head = parse_mp3_file_head;

	audio_file_parse_head[AUDIO_TYPE_WMA].audio_file_real_type = AUDIO_TYPE_WMA;
	audio_file_parse_head[AUDIO_TYPE_WMA].parse_audio_file_head = parse_wma_file_head;
	
	audio_file_parse_head[AUDIO_TYPE_WAV].audio_file_real_type = AUDIO_TYPE_WAV;
	audio_file_parse_head[AUDIO_TYPE_WAV].parse_audio_file_head = parse_wav_file_head;

	audio_file_head_para.fd = fd;
	audio_file_head_para.result_queue = ack_fsq;
	audio_file_head_para.buf_size = AUDIO_PASER_BUFFER_SIZE;

	audio_file_head = (INT32U) gp_malloc_align((audio_file_head_para.buf_size), 4);
	//audio_file_head = (INT32U) gp_malloc(audio_file_head_para.buf_size);
	if (audio_file_head == NULL) 
	{
		DBG_PRINT("audio file parse failed to allocate memory\r\n");
		
		return type_index;   //what i can do if malloc error?
	}
	
	audio_file_head_para.buf_addr = (INT32U) audio_file_head;

	lseek((INT16S)audio_file_head_para.fd, 0, SEEK_SET);
	msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *)&audio_file_head_para, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_URGENT);

	len = (INT32S)OSQPend(ack_fsq, 200, &err);
	if ((err != OS_NO_ERR) || len < 0) 
	{
		gp_free((void *) audio_file_head);
		return type_index;  // AUDIO_READ_FAIL;     //how to handle it  if read error?
	}
	lseek((INT16S)audio_file_head_para.fd, 0, SEEK_SET);

	// 1. check it is real the type_index,according the file extension to judge the file real type.
	if((audio_file_parse_head[type_index].parse_audio_file_head((INT8S *)audio_file_head )) == AUDIO_PARSE_SUCCS )
	{	
		gp_free((void *) audio_file_head);
		return audio_file_parse_head[type_index].audio_file_real_type;
	}

	// else then enum all support file type except current extension type
	for (i=1;i<AUDIO_MAX_TYPE;i++) 
	{
		if(( i == type_index) && (i < AUDIO_MAX_TYPE -1))
		{
			i++;
		}
		else if ((i == type_index) && (i >= AUDIO_MAX_TYPE -1))
		{
			audio_real_type = AUDIO_TYPE_NONE;
			break;  // if type_index = AUDIO_MAX_TYPE & i the same as type_index ,so should be break.
		}
		
		if((audio_file_parse_head[i].parse_audio_file_head((INT8S *)audio_file_head ) )== AUDIO_PARSE_SUCCS )
		{
			audio_real_type = audio_file_parse_head[i].audio_file_real_type;
			break;
		}
		else
		{
			audio_real_type = AUDIO_TYPE_NONE;
		}

	}

	gp_free((void *) audio_file_head);
	
	return audio_real_type;

}
#endif

static INT32S audio_get_type(INT16S fd,INT8S* file_name, OS_EVENT *ack_fsq)
{
   	INT8S  	temp[5] = {0};
	INT16U 	i;
	
	gp_strcpy(temp,file_name);

	for (i=0;i<gp_strlen(temp);i++) {
		temp[i] = gp_toupper(temp[i]);
	}

#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
	if(gp_strcmp(temp, (INT8S *)"GA")==0) {
		return AUDIO_TYPE_WMA;
   	}
   	else {
   	    return AUDIO_TYPE_NONE;
   	}
#endif
   	
#if APP_WAV_CODEC_FG_EN == 1 || APP_WAV_CODEC_BG_EN == 1
   	if(gp_strcmp(temp, (INT8S *)"WAV")==0)
	{
		
		//return AUDIO_TYPE_WAV;
		return audio_real_type_get(fd,AUDIO_TYPE_WAV, ack_fsq);
   	}

#endif

#if APP_MP3_DECODE_FG_EN == 1 || APP_MP3_DECODE_BG_EN == 1
   	if(gp_strcmp(temp, (INT8S *)"MP3")==0)
	{
		//return AUDIO_TYPE_MP3;
		return audio_real_type_get(fd,AUDIO_TYPE_MP3, ack_fsq);
	}
 
#endif

#if APP_WMA_DECODE_FG_EN == 1 || APP_WMA_DECODE_BG_EN == 1
	if(gp_strcmp(temp, (INT8S *)"WMA")==0)
	{
		//return AUDIO_TYPE_WMA;
		return audio_real_type_get(fd,AUDIO_TYPE_WMA, ack_fsq);
	}
#endif

#if APP_A1800_DECODE_FG_EN == 1 || APP_A1800_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A18")==0)//080722
    	return AUDIO_TYPE_A1800;
#endif

    if(gp_strcmp(temp, (INT8S *)"IDI")==0)//080903
    	return AUDIO_TYPE_MIDI;
    	
    if(gp_strcmp(temp, (INT8S *)"GMD")==0)
    	return AUDIO_TYPE_MIDI;

#if APP_A1600_DECODE_FG_EN == 1 || APP_A1600_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A16")==0)	// added by Bruce, 2008/09/23
    	return AUDIO_TYPE_A1600;
#endif

#if APP_A6400_DECODE_FG_EN == 1 || APP_A6400_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A64")==0)	// added by Bruce, 2008/09/25
    	return AUDIO_TYPE_A6400;
#endif

#if APP_S880_DECODE_FG_EN == 1 || APP_S880_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"S88")==0)	// added by Bruce, 2008/09/25
    	return AUDIO_TYPE_S880;
#endif
#if APP_AAC_DECODE_FG_EN == 1 || APP_AAC_DECODE_BG_EN == 1
	if(gp_strcmp(temp, (INT8S *)"AAC")==0)
		return AUDIO_TYPE_AAC;
#endif

   	return AUDIO_TYPE_NONE;

}
#else
static INT32S audio_get_type(INT8S* file_name)
{
   	INT8S  	temp[5] = {0};
   	INT16U 	i;

	gp_strcpy(temp,file_name);

	for (i=0;i<gp_strlen(temp);i++) {
		temp[i] = gp_toupper(temp[i]);
	}

#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
    if(gp_strcmp(temp, (INT8S *)"GA")==0) {
		return AUDIO_TYPE_WMA;
   	}
   	else {
   	    return AUDIO_TYPE_NONE;
   	}
#endif

#if APP_WAV_CODEC_FG_EN == 1 || APP_WAV_CODEC_BG_EN == 1
   	if(gp_strcmp(temp, (INT8S *)"WAV")==0)
      return AUDIO_TYPE_WAV;
#endif

#if APP_MP3_DECODE_FG_EN == 1 || APP_MP3_DECODE_BG_EN == 1
   	if(gp_strcmp(temp, (INT8S *)"MP3")==0)
    	return AUDIO_TYPE_MP3;
#endif

#if APP_WMA_DECODE_FG_EN == 1 || APP_WMA_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"WMA")==0)
    	return AUDIO_TYPE_WMA;
#endif

#if APP_A1800_DECODE_FG_EN == 1 || APP_A1800_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A18")==0)//080722
    	return AUDIO_TYPE_A1800;
#endif

    if(gp_strcmp(temp, (INT8S *)"IDI")==0)//080903
    	return AUDIO_TYPE_MIDI;
    	
    if(gp_strcmp(temp, (INT8S *)"GMD")==0)
    	return AUDIO_TYPE_MIDI;

#if APP_A1600_DECODE_FG_EN == 1 || APP_A1600_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A16")==0)	// added by Bruce, 2008/09/23
    	return AUDIO_TYPE_A1600;
#endif

#if APP_A6400_DECODE_FG_EN == 1 || APP_A6400_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"A64")==0)	// added by Bruce, 2008/09/25
    	return AUDIO_TYPE_A6400;
#endif

#if APP_S880_DECODE_FG_EN == 1 || APP_S880_DECODE_BG_EN == 1
    if(gp_strcmp(temp, (INT8S *)"S88")==0)	// added by Bruce, 2008/09/25
    	return AUDIO_TYPE_S880;
#endif
#if APP_AAC_DECODE_FG_EN == 1 || APP_AAC_DECODE_BG_EN == 1
	if(gp_strcmp(temp, (INT8S *)"AAC")==0)	
		return AUDIO_TYPE_AAC;
#endif

   	return AUDIO_TYPE_NONE;

}
#endif

static void audio_queue_clear(void)
{
	//OSQFlush(hAudioDacTaskQ);
	OSQFlush(aud_send_q);
	OSQFlush(audio_wq);
	OSQFlush(audio_fsq);
	OSQPost(hAudioDacTaskQ, (void *)MSG_AUD_DMA_WIDX_CLEAR);
}

#if 0
static INT32U audio_write_buffer(INT16S fd, INT8U *ring_buf, INT32U wi, INT32U ri)
{
	INT32S t;
	INT32S len;

	if(wi == 0 && ri == 0) {
		len = read(fd,(INT32U)ring_buf, audio_ctrl.ring_size/2);
        wi += len;
        return wi;
    }

    len = ri - wi;
    if (len < 0) {
    	len += audio_ctrl.ring_size;
    }
    if(len < audio_ctrl.ring_size/2) {
    	return wi;
    }

	t = wi;
	wi += audio_ctrl.ring_size/2;
	if(wi == audio_ctrl.ring_size) {
		wi = 0;
	}
	if(wi == ri) {
		return t;
	}

	len = read(fd,(INT32U)ring_buf+t, audio_ctrl.ring_size/2);
	if (len<0) {
		DBG_PRINT("error read file \r\n");
	}
	wi = t + len;
	if(wi == audio_ctrl.ring_size) {
		wi = 0;
	}
	return wi;
}
#endif

#if APP_MP3_DECODE_FG_EN == 1 || APP_WMA_DECODE_FG_EN == 1 || APP_WAV_CODEC_FG_EN == 1 || APP_A1600_DECODE_FG_EN == 1 || APP_A1800_DECODE_FG_EN == 1 || APP_A6400_DECODE_FG_EN == 1 || APP_S880_DECODE_FG_EN == 1 
INT32S audio_write_with_file_srv(INT8U *ring_buf, INT32U wi, INT32U ri)
{
	INT32S t;
	INT32S len;
	INT8U  err;
	INT32U msg_id;
	TK_FILE_SERVICE_STRUCT audio_fs_para;

	switch(audio_context_p->source_type) {
		case AUDIO_SRC_TYPE_FS:
			msg_id = MSG_FILESRV_FS_READ;
			break;
		case AUDIO_SRC_TYPE_GPRS:
			msg_id = MSG_FILESRV_NVRAM_AUDIO_GPRS_READ;
			audio_fs_para.spi_para.sec_offset = audio_ctrl.read_secs;
			break;
		case AUDIO_SRC_TYPE_APP_RS:
			msg_id = MSG_FILESRV_NVRAM_AUDIO_APP_READ;
			break;
		case AUDIO_SRC_TYPE_APP_PACKED_RS:
			msg_id = MSG_FILESRV_NVRAM_AUDIO_APP_PACKED_READ;
			break;
		case AUDIO_SRC_TYPE_USER_DEFINE:	// added by Bruce, 2008/10/27
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)	
			if(G_SACM_Ctrl.Offsettype != SND_OFFSET_TYPE_NONE)
				msg_id = MSG_FILESRV_MAX;
			else
#endif
			msg_id = MSG_FILESRV_USER_DEFINE_READ;
			audio_fs_para.data_start_addr = audio_ctrl.data_start_addr;
			audio_fs_para.data_offset = audio_ctrl.data_offset;
			break;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
		case AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE:	// added by Bruce, 2010/01/22
			msg_id = MSG_FILESRV_FS_READ;
#endif
		default:
			break;
	}

	audio_fs_para.fd = audio_ctrl.file_handle;
	audio_fs_para.result_queue = audio_fsq;
	audio_fs_para.main_channel = AUDIO_CHANNEL_DAC;

	if(wi == 0 && ri == 0) {
		audio_fs_para.buf_addr = (INT32U)ring_buf;
		audio_fs_para.buf_size = audio_ctrl.ring_size/2;
		audio_fs_para.spi_para.sec_cnt = audio_ctrl.ring_size/1024;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)	
		if(msg_id == MSG_FILESRV_MAX)
		{
			if((G_SACM_Ctrl.Offsettype = SND_OFFSET_TYPE_SIZE)||(G_SACM_Ctrl.Offsettype = SND_OFFSET_TYPE_TIME))
			{	
				if(G_SACM_Ctrl.AudioFormat == MP3)
				{
					if(G_SACM_Ctrl.Offset > audio_ctrl.file_len) 
						return AUDIO_READ_FAIL;
					
					if(Snd_Lseek(G_SACM_Ctrl.Offset,SEEK_CUR) >= 0)
						audio_ctrl.file_cnt += G_SACM_Ctrl.Offset;
					else 
						return AUDIO_READ_FAIL;
						
					len = Snd_GetData(audio_fs_para.buf_addr,audio_fs_para.buf_size);
					audio_ctrl.read_secs += (len/512);
			        wi += len; 
				}
				else if(G_SACM_Ctrl.AudioFormat == WMA)                  //add WMA_Seek
				{
				    len = Snd_GetData(audio_fs_para.buf_addr,audio_fs_para.buf_size);
					audio_ctrl.read_secs += (len/512);
			        wi += len; 
				}
				else
				{	// use jmp to play
					len = Snd_GetData(audio_fs_para.buf_addr,6);
					audio_fs_para.buf_addr += 6;
					audio_fs_para.buf_size -= 6;
					/*
					if(G_SACM_Ctrl.Offsettype == SND_OFFSET_TYPE_TIME)
					{
						//calc offset size by time,sample rate,ALG,data rate
						t = G_SACM_Ctrl.Offset;
						switch(G_SACM_Ctrl.AudioFormat) 
						{
							case AUDIO_TYPE_MP3:
								break;
							case AUDIO_TYPE_WAV:
									break;
							case AUDIO_TYPE_WMA:
								break;
							case AUDIO_TYPE_A1800:
								break;
							case AUDIO_TYPE_A1600:
								break;
							case AUDIO_TYPE_A6400:
								break;
							case AUDIO_TYPE_S880:	
								break;
							default:
									return;
						}				
					}
					else if(G_SACM_Ctrl.Offsettype == SND_OFFSET_TYPE_SIZE)
						t = G_SACM_Ctrl.Offset;
					else
						t = 0;
					*/
					if(G_SACM_Ctrl.Offset > audio_ctrl.file_len) 
						return AUDIO_READ_FAIL;
					
					if(Snd_Lseek(G_SACM_Ctrl.Offset,SEEK_CUR) >= 0)
						audio_ctrl.file_cnt += G_SACM_Ctrl.Offset + 6;
					else 
						return AUDIO_READ_FAIL;
						
					len = Snd_GetData(audio_fs_para.buf_addr,audio_fs_para.buf_size);
					len += 6;
					audio_ctrl.read_secs += (len/512);
			        wi += len; 
				}
			}
			else
			{	// not use jmp to play
				len = Snd_GetData(audio_fs_para.buf_addr,audio_fs_para.buf_size);
				audio_ctrl.read_secs += (len/512);
	        	wi += len; 
			}
			return wi;
		}
		else
#endif		
		{
            msgQSend(fs_msg_q_id, msg_id, (void *)&audio_fs_para, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_URGENT);

            len = (INT32S) OSQPend(audio_fsq, 0, &err);
            if ((err != OS_NO_ERR) || len < 0) {
        	    return AUDIO_READ_FAIL;
            }
			if (audio_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)
				audio_ctrl.data_offset += len;
            audio_ctrl.read_secs += (len/512);
            wi += len;
            return wi;
        }
    }

    len = ri - wi;
    if (len <= 0) {
    	len += audio_ctrl.ring_size;
    }
    if(len < audio_ctrl.ring_size/2) {
    	return wi;
    }

	t = wi;
	wi += audio_ctrl.ring_size/2;
	if(wi == audio_ctrl.ring_size) {
		wi = 0;
	}
	if(wi == ri) {
		return t;
	}

	audio_fs_para.buf_addr = (INT32U)ring_buf+t;
	audio_fs_para.buf_size = audio_ctrl.ring_size/2;

	audio_fs_para.spi_para.sec_cnt = audio_ctrl.ring_size/1024;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	if(msg_id == MSG_FILESRV_MAX)
	{	
		len = Snd_GetData(audio_fs_para.buf_addr,audio_fs_para.buf_size);
		OSQPost(audio_fsq, (void *) len);
	}
	else
#endif	
	msgQSend(fs_msg_q_id, msg_id, (void *)&audio_fs_para, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_URGENT);

	return AUDIO_READ_PEND;
}

INT32S audio_check_wi(INT32S wi_in, INT32U *wi_out)
{
	INT32S len;
	INT8U  err;
	INT32S t;

	if (wi_in >= 0) {
		*wi_out = wi_in;
		return AUDIO_ERR_NONE;
	}
	if (wi_in == AUDIO_READ_FAIL) {
		return AUDIO_READ_FAIL;
	}
	/* AUDIO_READ_PEND */
	t = audio_ctrl.wi;
	len = (INT32S) OSQPend(audio_fsq, 0, &err);
	if ((err != OS_NO_ERR) || len <= 0) {
        return AUDIO_READ_FAIL;
    }
	if (audio_context_p->source_type == AUDIO_SRC_TYPE_USER_DEFINE)
		audio_ctrl.data_offset += len;
    audio_ctrl.read_secs += (len/512);
	t += len;
	if(t == audio_ctrl.ring_size) {
		t = 0;
	}

	*wi_out = t;
	return 	AUDIO_ERR_NONE;
}
#endif

static INT32S audio_send_to_dma(void)
{
	INT32U count;

	if (stopped && (audio_q_check()==AUDIO_SEND_FAIL)) { /* queuq full */
		stopped = 0;
		OSQPost(hAudioDacTaskQ,(void *) MSG_AUD_DMA_DBF_START);
	}
	else if (dac_dma_status_get() == 0) {
		if (!stopped) {
			#if 1 /* dac underrun, wait queue full again and start DMA*/
			stopped = 1;
			dac_dbf_free();
			DBG_PRINT("dac underrun !\r\n");

			audio_queue_clear();
			for (count=0;count<MAX_DAC_BUFFERS;count++) {
				OSQPost(audio_wq, (void *) count);
			}
			#else /* if any buffer into queue, start DMA again */
				OSQPost(hAudioDacTaskQ,(void *) MSG_AUD_DMA_DBF_RESTART);
			#endif
		}
	}
	return STATUS_OK;
}

static INT32S audio_q_check(void)
{
	OS_Q      *pq;
	pq = (OS_Q *)aud_send_q->OSEventPtr;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	if (pq->OSQEntries >= 2) {//for fast 090531
		return AUDIO_SEND_FAIL;
	}
#else
	if (pq->OSQEntries == pq->OSQSize) {
		return AUDIO_SEND_FAIL;
	}
#endif
	return AUDIO_SEND_OK;
}

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
void Set_mp3_play_start_time(INT32U play_start_time)
{
	audio_play_start_time = play_start_time;
}
#endif

#if APP_MP3_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_mp3_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;
	INT8U   *mp3_play_seek_flag;
	INT32U  *mp3_play_seek_offset;
	INT8U   err;
	
	while(1)
	{
		if(G_SACM_Ctrl.Offsettype != SND_OFFSET_TYPE_NONE)                  //add seek_time
		{
			mp3_play_seek_flag = OSMboxAccept(Mbox_WMA_Play_Seek_Flag);
			if((*mp3_play_seek_flag) == 1)
			{
				mp3_play_seek_offset = OSMboxPend(Mbox_WMA_Play_Seek_Offset , 0 , &err);
				MP3Parser_Seek(mp3_play_seek_offset,mp3_VBR_flag);
				audio_ctrl.wi = audio_ctrl.ri = 0;
				mp3_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
				 audio_ctrl.file_cnt =lseek(audio_ctrl.file_handle,0,SEEK_CUR);
				 G_SACM_Ctrl.Offsettype =SND_OFFSET_TYPE_NONE;
			} 
		}
		audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL); //return negtive value
		}

		if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
			if(audio_ctrl.f_last){
				return 0; //AUDIO_ERR_DEC_FINISH;
			}
			else{
				audio_ctrl.f_last = 1;
			}
		}

		in_length = audio_ctrl.ri;
	#if GPLIB_MP3_HW_EN == 1		
		pcm_point = mp3_dec_run(work_mem, Buffer, audio_ctrl.wi,audio_ctrl.f_last);
	#else
		pcm_point = mp3_dec_run(work_mem, Buffer, audio_ctrl.wi);
	#endif
	#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
		audio_decode_sample_number += pcm_point;
	#endif
		audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += MP3_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0) {
			if (pcm_point < 0) {
				if (--audio_ctrl.try_cnt == 0) {
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
#if GPLIB_MP3_HW_EN == 1	
	return pcm_point*2; /* 2 channel */
#else
	return pcm_point*mp3_dec_get_channel((CHAR*)work_mem);
#endif
}
#endif

INT32S audio_mp3_play_init(void)
{
	INT32U  count;
	INT32S  ret = 0;
	INT32U  in_length;
	INT32S  t_wi;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
	INT8U *U8_ptr;
        INT8U  char_temp0,char_temp1,char_temp2,char_temp3;
        INT32S i;
        INT32U temp[2];
#endif
	MP3Parser_Init(audio_ctrl.file_handle);

	// mp3 init
	gp_memset((INT8S*)audio_ctrl.ring_buf,0,RING_BUF_SIZE);
#if GPLIB_MP3_HW_EN == 1
	ret = mp3_dec_init((void*)audio_ctrl.work_mem, (void*)audio_ctrl.ring_buf);
	mp3_dec_set_ring_size((void*)audio_ctrl.work_mem,audio_ctrl.ring_size);
#else
	ret = mp3_dec_init((char*)audio_ctrl.work_mem, (unsigned char*)audio_ctrl.ring_buf, (char*)(audio_ctrl.work_mem + MP3_DEC_MEMORY_SIZE));
	mp3_dec_set_bs_buf_size((void*)audio_ctrl.work_mem,audio_ctrl.ring_size);
#endif
	
	audio_ctrl.file_cnt = 0;
	audio_ctrl.f_last = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;
	fg_error_cnt = 0;

	audio_ctrl.wi = 0;
	audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY) 
	U8_ptr = (INT8U*)audio_ctrl.ring_buf;
	char_temp0 = *U8_ptr++;
	char_temp1 = *U8_ptr++;
	char_temp2 = *U8_ptr++;
	if((char_temp0 == 'I') && (char_temp1 == 'D') && (char_temp2 == '3'))//ID3 header
	{
		U8_ptr += 3;
		char_temp3 = *U8_ptr++;
		char_temp2 = *U8_ptr++;
		char_temp1 = *U8_ptr++;
		char_temp0 = *U8_ptr++;
		mp3_ID3V2_length = 10 + ((char_temp0 & 0x7f)|((char_temp1 & 0x7f)<<7)|((char_temp2 & 0x7f)<<14)|((char_temp3 & 0x7f)<<21));
		switch(audio_context_p->source_type)
		{
			case AUDIO_SRC_TYPE_FS:
				audio_ctrl.file_cnt = mp3_ID3V2_length;
				ret = (INT32S)lseek(audio_ctrl.file_handle, audio_ctrl.file_cnt, SEEK_SET);
				audio_ctrl.wi = 0;
				audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)/* check reading data */
				{ 
					return AUDIO_ERR_DEC_FAIL;
				}
				break;
			case AUDIO_SRC_TYPE_SPI:
				break;
			case AUDIO_SRC_TYPE_USER_DEFINE:
				audio_ctrl.file_cnt = mp3_ID3V2_length;
				audio_ctrl.data_offset = audio_ctrl.file_cnt;
				audio_ctrl.wi = 0;
				audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)/* check reading data */
				{ 
					return AUDIO_ERR_DEC_FAIL;
				}
				break;
		#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
			case AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE:	// added by Bruce, 2010/01/22
				audio_ctrl.file_cnt = mp3_ID3V2_length;
				ret = (INT32S)lseek(audio_ctrl.file_handle, audio_ctrl.file_cnt+audio_ctrl.data_offset, SEEK_SET);
				audio_ctrl.wi = 0;
				audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)/* check reading data */
				{ 
					return AUDIO_ERR_DEC_FAIL;
				}
				break;
		#endif
			default:
				break;
		}
	}
	else
	{
		mp3_ID3V2_length = 0;
	}
	U8_ptr = (INT8U*)audio_ctrl.ring_buf;
	if( (*(U8_ptr+36)=='X') && (*(U8_ptr+37)=='i') && (*(U8_ptr+38)=='n') && (*(U8_ptr+39)=='g') )//VBR flag
		mp3_VBR_flag = 1;//VBR
	else
		mp3_VBR_flag = 0;//CBR
	if(mp3_VBR_flag)//VBR
	{
		if( (*(U8_ptr+43)) | 0x01)//total frame number exist
		{
			mp3_total_frame = (INT32U)(*(U8_ptr+44)<<24) | (*(U8_ptr+45)<<16) | (*(U8_ptr+46)<<8) | (*(U8_ptr+47));
			if(mp3_total_frame == 0)//090819
			{
				mp3_VBR_flag = 0;
			}
		}
		else
		{
			mp3_VBR_flag = 0;
		}
	}
#endif
	count = 400;
	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = mp3_dec_parsing((void*)audio_ctrl.work_mem , audio_ctrl.wi);

		audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			//audio_ctrl.file_cnt += MP3_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		switch(ret)
		{
			case MP3_DEC_ERR_NONE:
				break;
			case MP3_DEC_ERR_LOSTSYNC:		//not found sync word
			case MP3_DEC_ERR_BADSAMPLERATE:	//reserved sample frequency value
			case MP3_DEC_ERR_BADBITRATE:		//forbidden bitrate value
			case MP3_DEC_ERR_BADLAYER:
			case MP3_DEC_ERR_BADMPEGID:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
					return AUDIO_ERR_READ_FAIL;
				}
				if (--count == 0)
					return AUDIO_ERR_DEC_FAIL;
				continue;
			default:
				return AUDIO_ERR_DEC_FAIL;
		}
		if(ret == MP3_DEC_ERR_NONE) {
			break;
		}
	}

	in_length = mp3_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	audio_samplerate = in_length;
	audio_bitrate = mp3_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
	if(audio_bitrate == 0)
		audio_bitrate = 1;//100528 ֵֹΪ0ʱϵͳhangס
	if(mp3_VBR_flag == 0)//	CBR
	{
		audio_total_time = ((audio_ctrl.file_len - mp3_ID3V2_length)*(8*10))/audio_bitrate;		// modified by Bruce, 2010/01/08
	}
	else//VBR
	{
		for(i=0;i<2;i++)
		{
			audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
			t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
			if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)
			{ /* check reading data */
				return (0 - AUDIO_ERR_DEC_FAIL); //return negtive value
			}
			if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
			{
				if(audio_ctrl.f_last)
				{
					return 0; //AUDIO_ERR_DEC_FINISH;
				}
				else
				{
					audio_ctrl.f_last = 1;
				}
			}
			in_length = audio_ctrl.ri;
		#if GPLIB_MP3_HW_EN == 1		
			temp[i] = mp3_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[0], audio_ctrl.wi,audio_ctrl.f_last);
		#else
			temp[i] = mp3_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[0], audio_ctrl.wi);
		#endif
			audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
			audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
			if(in_length > audio_ctrl.ri)
			{
				//audio_ctrl.file_cnt += MP3_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
				audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
			}
		}
		audio_decode_perframe_sample_number = temp[1];
		if(audio_samplerate == 0)
			audio_samplerate = 1;//100528 ֵֹΪ0ʱϵͳhangס
		audio_total_time = (audio_decode_perframe_sample_number * mp3_total_frame * 10 / audio_samplerate);	// modified by Bruce, 2010/01/08
	}
	if(audio_play_start_time >= audio_total_time/10)		// modified by Bruce, 2010/01/08
	{
		audio_play_start_time = audio_total_time/10 -1;
	}
	if(audio_play_start_time < 0)
	{
		audio_play_start_time = 0;
	}
	if(audio_play_start_time == 0)
	{
		audio_ctrl.file_cnt = mp3_ID3V2_length;//100528
	}
	else//100528
	{
		audio_ctrl.file_cnt = mp3_ID3V2_length + (INT32U)((INT64U)(audio_ctrl.file_len - mp3_ID3V2_length)*audio_play_start_time *10 /audio_total_time); // modified by Bruce, 2010/01/08
	}
	audio_decode_sample_number = audio_samplerate * audio_play_start_time;
	switch(audio_context_p->source_type)//100528
	{
		case AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE:// modified by Bruce, 2010/01/22
			ret = (INT32S)lseek(audio_ctrl.file_handle, audio_ctrl.file_cnt+audio_ctrl.data_offset, SEEK_SET);
			break;
		case AUDIO_SRC_TYPE_FS:
			ret = (INT32S)lseek(audio_ctrl.file_handle, audio_ctrl.file_cnt, SEEK_SET);
			break;
		case AUDIO_SRC_TYPE_USER_DEFINE:
			audio_ctrl.data_offset = audio_ctrl.file_cnt;
			break;
		default:
			break;
	}
	audio_ctrl.wi = 0;
	audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)/* check reading data */
	{ 
		return AUDIO_ERR_DEC_FAIL;
	}
	in_length = audio_samplerate;

	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_mp3_get_output;
#if GPLIB_MP3_HW_EN == 1	
	//ret = mp3_dec_get_channel((CHAR*)audio_ctrl.work_mem);
	ret = 2; //mp3 decode force to 2 channel by h/w
    dac_stereo_set();
#else
	ret = mp3_dec_get_channel((CHAR*)audio_ctrl.work_mem);
	if(ret == 1) {
		dac_mono_set();
	} else {
		dac_stereo_set();
	}
#endif

#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ret == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ret, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ret = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ret == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ret, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ret = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100/in_length;
	if(count > 4)	count = 4;
	if(hUpSample && (count >= 2))
	{
		if(ret == 1)
			UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 1);
		else
			UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ret = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
		//if(ret == 1)
		//	dac_mono_set();
		//else
		//	dac_stereo_set();
	}
#endif //APP_UP_SAMPLE_EN
#else
	dac_stereo_set();	
#endif
	
	dac_sample_rate_set(in_length);
	channel = ret;
	g_audio_sample_rate = in_length;
	DBG_PRINT("bps: %d\r\n",mp3_dec_get_bitrate((void*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",mp3_dec_get_channel((void*)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n",mp3_dec_get_samplerate((void*)audio_ctrl.work_mem));
	DBG_PRINT("block size: %d\r\n",mp3_dec_get_mem_block_size());

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}
	return AUDIO_ERR_NONE;
}

INT32S  audio_mp3_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S  pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT32S  t_wi;

	audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
		if(audio_ctrl.f_last){
			return AUDIO_ERR_DEC_FINISH;
		}
		else{
			audio_ctrl.f_last = 1;
		}
	}

	in_length = audio_ctrl.ri;

#if GPLIB_MP3_HW_EN
	pcm_point = mp3_dec_run((void*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi,audio_ctrl.f_last);
#else
	pcm_point = mp3_dec_run((void*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi);
#endif
	audio_ctrl.ri = mp3_dec_get_ri((void*)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += MP3_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0) {
		if (pcm_point == 0 && ++fg_error_cnt > MP3_FRAME_ERROR_CNT) {
	        return AUDIO_ERR_DEC_FAIL;
	    }
		if (pcm_point < 0) {
			if (--audio_ctrl.try_cnt == 0) {
				return AUDIO_ERR_DEC_FAIL;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
	else {
		fg_error_cnt = 0;
	}
		

	pcm_len[wb_idx] = pcm_point*2; /* 2 channel */
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;
	
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], MP3_DEC_FRAMESIZE);	//pcm_point*2
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;

	pcm_len[wb_idx] = pcm_point;
#endif
	
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif // #if APP_MP3_DECODE_FG_EN == 1

#if APP_WMA_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_wma_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S	pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT8U   ch;
	INT32S  t_wi;
	INT32S  offset;
	INT32S  len;
	INT8U   *wma_play_seek_flag;
	INT32U  *wma_play_seek_offset;
	INT32U  seek_offset;
	while(1)
	{
		audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);
		
		offset = wma_dec_get_offset((char *)audio_ctrl.work_mem);
		
		if(offset > 0) {
			len = audio_ctrl.wi - audio_ctrl.ri;
			if(len < 0) {
				len += WMA_DEC_BITSTREAM_BUFFER_SIZE;
			}
			
			audio_ctrl.file_cnt += offset;

			if(len > offset)
			{
				audio_ctrl.ri += offset;
				if(audio_ctrl.ri >= WMA_DEC_BITSTREAM_BUFFER_SIZE) {
					audio_ctrl.ri -= WMA_DEC_BITSTREAM_BUFFER_SIZE;
				}
				wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
			}
			else
			{
				offset -= len;
				if(lseek(audio_ctrl.file_handle, offset, SEEK_CUR) < 0) {
					return (0-AUDIO_ERR_DEC_FAIL);
				}
				audio_ctrl.wi = audio_ctrl.ri = 0;
				wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
			}
			wma_dec_reset_offset((char *)audio_ctrl.work_mem);
		}

	    if(G_SACM_Ctrl.Offsettype != SND_OFFSET_TYPE_NONE)                  //add seek_time
	     {
	      wma_play_seek_flag = OSMboxAccept(Mbox_WMA_Play_Seek_Flag);
	      if((*wma_play_seek_flag) == 1)
	       {
	        wma_play_seek_offset = OSMboxPend(Mbox_WMA_Play_Seek_Offset , 0 , &err);

	        seek_offset = wma_dec_seek((char *)audio_ctrl.work_mem, *wma_play_seek_offset);
	        if(lseek(audio_ctrl.file_handle, seek_offset, SEEK_SET))
	         {
	          audio_ctrl.file_cnt = seek_offset;
	         }

	        audio_ctrl.wi = audio_ctrl.ri = 0;
	        wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri); 
	       }
	     }

		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0-AUDIO_ERR_READ_FAIL);
		}

		in_length = audio_ctrl.ri;
		if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
			if(audio_ctrl.f_last){
				return 0; //AUDIO_ERR_DEC_FINISH;
			}
			else{
				audio_ctrl.f_last = 1;
			}
		}
		pcm_point = wma_dec_run((char *)audio_ctrl.work_mem,Buffer,audio_ctrl.wi);

		audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += WMA_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0) {
			if (pcm_point < 0) {
				if (--audio_ctrl.try_cnt == 0) {
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
		}
		else
			break;
	}
	ch = wma_dec_get_channel((char *)audio_ctrl.work_mem);
	return ch*pcm_point;
}

#endif
int audio_wma_play_init()
{
	INT32S count;
	INT32U in_length;
	INT32S ret;
	INT8U  ch;
	INT32S  t_wi;

	audio_ctrl.file_cnt = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;
    fg_error_cnt = 0;
    
	gp_memset((INT8S*)audio_ctrl.ring_buf,0,audio_ctrl.ring_size);

	ret = wma_dec_init((char *)audio_ctrl.work_mem, audio_ctrl.ring_buf,(char *)audio_ctrl.work_mem + 8192,audio_ctrl.ring_size);

	audio_ctrl.wi = wma_dec_get_ri((char *)audio_ctrl.work_mem);
	audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}
	count = 50;

	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = wma_dec_parsing((char *)audio_ctrl.work_mem , audio_ctrl.wi);
		audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			//audio_ctrl.file_cnt += WMA_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		switch(ret)
		{
			case WMA_OK:
				break;
			case WMA_E_BAD_PACKET_HEADER:
			case WMA_E_INVALIDHEADER:
			case WMA_E_NOTSUPPORTED:
			case WMA_E_NOTSUPPORTED_CODEC:
				return AUDIO_ERR_INVALID_FORMAT;
			case WMA_E_ONHOLD:
			case WMA_E_DATANOTENOUGH:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
					return AUDIO_ERR_READ_FAIL;
				}
				if (--count == 0)
					return AUDIO_ERR_DEC_FAIL;
				continue;
			default:
				if (--count == 0)
					return AUDIO_ERR_DEC_FAIL;
				break;
		}

		if(ret == WMA_OK) {
			break;
		}
	}

	in_length =wma_dec_get_samplerate((char *)audio_ctrl.work_mem); 

	ch = wma_dec_get_channel((char *)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput=audio_wma_get_output;

	#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ch, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
	#endif
#endif
	dac_sample_rate_set(in_length);
	channel = ch;
	g_audio_sample_rate = in_length;
	if (ch == 1) {
		dac_mono_set();
	}
	else {
		dac_stereo_set();
	}
	audio_total_time = wma_dec_get_playtime((char*)audio_ctrl.work_mem);
	DBG_PRINT("bps: %d\r\n",wma_dec_get_bitrate((char*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",ch);
	DBG_PRINT("sample rate: %d\r\n",wma_dec_get_samplerate((char*)audio_ctrl.work_mem));

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_wma_process()
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S	pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT8U   ch;
	INT32S  t_wi;
	INT32S  offset;
	INT32S  len;
	INT8U   *wma_play_seek_flag;
	INT32U  *wma_play_seek_offset;
	INT32U  seek_offset;

	audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);
	
	offset = wma_dec_get_offset((char *)audio_ctrl.work_mem);
	
	if(offset > 0) {
		len = audio_ctrl.wi - audio_ctrl.ri;
		if(len < 0) {
			len += WMA_DEC_BITSTREAM_BUFFER_SIZE;
		}
		
		audio_ctrl.file_cnt += offset;

		if(len > offset)
		{
			audio_ctrl.ri += offset;
			if(audio_ctrl.ri >= WMA_DEC_BITSTREAM_BUFFER_SIZE) {
				audio_ctrl.ri -= WMA_DEC_BITSTREAM_BUFFER_SIZE;
			}
			wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
		}
		else
		{
			offset -= len;
			if(lseek(audio_ctrl.file_handle, offset, SEEK_CUR) < 0) {
				return AUDIO_ERR_DEC_FAIL;
			}
			audio_ctrl.wi = audio_ctrl.ri = 0;
			wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
		}
		wma_dec_reset_offset((char *)audio_ctrl.work_mem);
	}

    if(G_SACM_Ctrl.Offsettype != SND_OFFSET_TYPE_NONE)                  //add seek_time
     {
      wma_play_seek_flag = OSMboxAccept(Mbox_WMA_Play_Seek_Flag);
      if((*wma_play_seek_flag) == 1)
       {
        wma_play_seek_offset = OSMboxPend(Mbox_WMA_Play_Seek_Offset , 0 , &err);

        seek_offset = wma_dec_seek((char *)audio_ctrl.work_mem, *wma_play_seek_offset);
        if(lseek(audio_ctrl.file_handle, seek_offset, SEEK_SET))
         {
          audio_ctrl.file_cnt = seek_offset;
         }

        audio_ctrl.wi = audio_ctrl.ri = 0;
        wma_dec_set_ri((char *)audio_ctrl.work_mem, audio_ctrl.ri);
       }
     }

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	in_length = audio_ctrl.ri;

	pcm_point = wma_dec_run((char *)audio_ctrl.work_mem,pcm_out[wb_idx],audio_ctrl.wi);

	audio_ctrl.ri = wma_dec_get_ri((char *)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += WMA_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
		return AUDIO_ERR_DEC_FINISH;
	}

	if (pcm_point <= 0) {
	    if (pcm_point == 0 && ++fg_error_cnt > 30) {
	        return AUDIO_ERR_DEC_FAIL;
	    }
	    if (pcm_point < 0) {
			if (wma_dec_get_errno((char *)audio_ctrl.work_mem) == WMA_E_NO_MORE_FRAMES) {
				return AUDIO_ERR_DEC_FINISH;
			}
			else {
				return AUDIO_ERR_DEC_FAIL;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
    else {
        fg_error_cnt = 0;
    }
    
	ch = wma_dec_get_channel((char *)audio_ctrl.work_mem);
	
	pcm_len[wb_idx] = pcm_point*ch;
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], WMA_DEC_FRAMESIZE);	//pcm_point*2
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;


	pcm_len[wb_idx] = pcm_point;
#endif
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();

	return 0;
}
#endif // #if APP_WMA_DECODE_FG_EN == 1

#if APP_WAV_CODEC_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_wav_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S	pcm_point;
	INT32U  in_length;
	INT32S  tmp_len;
	INT8U   ch;
	INT32S  t_wi;
	
	while(1)
	{
		audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}

		if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
		{
			return 0; //AUDIO_ERR_DEC_FINISH;
		}
		else
		{
			in_length = audio_ctrl.wi - audio_ctrl.ri;
			if(audio_ctrl.wi < audio_ctrl.ri) {
				in_length += WAV_DEC_BITSTREAM_BUFFER_SIZE;
			}
			if(in_length > audio_ctrl.file_len-audio_ctrl.file_cnt) {
				tmp_len = audio_ctrl.wi;
				tmp_len -= (in_length - audio_ctrl.file_len + audio_ctrl.file_cnt);
				if(tmp_len < 0) {
					tmp_len += WAV_DEC_BITSTREAM_BUFFER_SIZE;
				}
				audio_ctrl.wi = tmp_len;
			}
		}

		in_length = audio_ctrl.ri;

		pcm_point = wav_dec_run((INT8U *)audio_ctrl.work_mem, Buffer, audio_ctrl.wi);
		audio_decode_sample_number += pcm_point;
		if (pcm_point == -1) {
			return (0 - AUDIO_ERR_DEC_FAIL);
		}

		audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += WAV_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0) {
			if (--audio_ctrl.try_cnt == 0) {
				return (0 - AUDIO_ERR_DEC_FAIL);
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
	ch = wav_dec_get_nChannels((INT8U *)audio_ctrl.work_mem);
	return pcm_point * ch;
}
#endif

INT32S audio_wav_dec_play_init()
{
	INT32U in_length;
	INT8U  ch;
	INT32S count;
	INT32S t_wi;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	audio_decode_sample_number = 0;
#endif
	audio_ctrl.file_cnt = 0;
	audio_ctrl.try_cnt = 10;
	audio_ctrl.read_secs = 0;

	gp_memset((INT8S*)audio_ctrl.ring_buf,0,RING_BUF_SIZE);
	wav_dec_init((INT8U *)audio_ctrl.work_mem, audio_ctrl.ring_buf);
	audio_ctrl.wi = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	in_length = audio_ctrl.ri;

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	audio_ctrl.file_len = wav_dec_parsing((INT8U *)audio_ctrl.work_mem , audio_ctrl.wi);

	if((int)(audio_ctrl.file_len) <= 0) {
		return AUDIO_ERR_DEC_FAIL;
	}

	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += WAV_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	in_length = wav_dec_get_SampleRate((INT8U *)audio_ctrl.work_mem);
	ch = wav_dec_get_nChannels((INT8U *)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_wav_get_output;
	
#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ch == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ch, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ch = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ch == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ch, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ch = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif	
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ch, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ch = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100 / in_length;
	if(count > 4)	count = 4;
	if(hUpSample && count>=2)
	{
		UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ch, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ch = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
	}
#endif
#endif
	
	dac_sample_rate_set(in_length);
	channel = ch;
	g_audio_sample_rate = in_length;
	if (ch == 1) {
		dac_mono_set();
	}
	else {
		dac_stereo_set();
	}

	DBG_PRINT("channel: %d\r\n",ch);
	DBG_PRINT("sample rate: %d\r\n",wav_dec_get_SampleRate((INT8U*)audio_ctrl.work_mem));

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_wav_dec_process()
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S	pcm_point;
	INT32U  in_length;
	INT32S  tmp_len;
	INT8U   ch;
	INT32U  wb_idx;
	INT8U   err;
	INT32S  t_wi;

	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	{
		return AUDIO_ERR_DEC_FINISH;
	}
	else
	{
		in_length = audio_ctrl.wi - audio_ctrl.ri;
		if(audio_ctrl.wi < audio_ctrl.ri) {
			in_length += WAV_DEC_BITSTREAM_BUFFER_SIZE;
		}
		if(in_length > audio_ctrl.file_len-audio_ctrl.file_cnt) {
			tmp_len = audio_ctrl.wi;
			tmp_len -= (in_length - audio_ctrl.file_len + audio_ctrl.file_cnt);
			if(tmp_len < 0) {
				tmp_len += WAV_DEC_BITSTREAM_BUFFER_SIZE;
			}
			audio_ctrl.wi = tmp_len;
		}
	}

	in_length = audio_ctrl.ri;

	pcm_point = wav_dec_run((INT8U *)audio_ctrl.work_mem,pcm_out[wb_idx],audio_ctrl.wi);

	if (pcm_point == -1) {
		return AUDIO_ERR_DEC_FAIL;
	}

	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += WAV_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0) {
		if (--audio_ctrl.try_cnt == 0) {
			return AUDIO_ERR_DEC_FAIL;
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
	ch = wav_dec_get_nChannels((INT8U *)audio_ctrl.work_mem);
	pcm_len[wb_idx] = pcm_point*ch;
#else
	INT32S pcm_point;
	INT8U   err;
	INT32U  wb_idx;
	
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], WAV_DEC_FRAMESIZE);
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;
	pcm_len[wb_idx] = pcm_point;
#endif

	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif // #if APP_WAV_CODEC_FG_EN == 1

#if APP_WAV_CODEC_FG_EN == 1
INT32S audio_avi_play_init(void)
{
	INT8U  ch;
	INT32S count;

	audio_ctrl.file_cnt = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;
	audio_ctrl.ring_size = RING_BUF_SIZE;
	
	gp_memset((void *)audio_ctrl.work_mem,0,WAV_DEC_MEMORY_SIZE);
	wav_dec_init((INT8U *)audio_ctrl.work_mem, audio_ctrl.ring_buf);
	audio_ctrl.wi = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);

	/*call wav_dec_set_param here */
	if(avi_mov_switchflag == C_MOV_MODE_PLAY)
		wav_dec_set_param((INT8U *)audio_ctrl.work_mem, tk_mov_get_waveformatex());
	else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
		wav_dec_set_param((INT8U *)audio_ctrl.work_mem, tk_avi_get_waveformatex());

	dac_sample_rate_set(wav_dec_get_SampleRate((INT8U *)audio_ctrl.work_mem));
	ch = wav_dec_get_nChannels((INT8U *)audio_ctrl.work_mem);
	channel = ch;
	g_audio_sample_rate = wav_dec_get_SampleRate((INT8U *)audio_ctrl.work_mem);	
	if (ch == 1) {
		dac_mono_set();
	}
	else {
		dac_stereo_set();
	}

	DBG_PRINT("channel: %d\r\n",ch);
	DBG_PRINT("sample rate: %d\r\n",wav_dec_get_SampleRate((INT8U*)audio_ctrl.work_mem));

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}
#endif

#if APP_MP3_DECODE_FG_EN == 1
INT32S audio_avi_mp3_play_init(void)
{
	INT32S count;
	INT32S ret;
		
	audio_ctrl.file_cnt = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;
	audio_ctrl.ring_size = RING_BUF_SIZE;
	
	gp_memset((INT8S*)audio_ctrl.ring_buf,0,audio_ctrl.ring_size);
#if GPLIB_MP3_HW_EN == 1
	ret = mp3_dec_init((void*)audio_ctrl.work_mem, (void*)audio_ctrl.ring_buf);
	mp3_dec_set_ring_size((void*)audio_ctrl.work_mem,audio_ctrl.ring_size);
#else
	ret = mp3_dec_init((char*)audio_ctrl.work_mem, (unsigned char*)audio_ctrl.ring_buf, (char*)(audio_ctrl.work_mem + MP3_DEC_MEMORY_SIZE));
	mp3_dec_set_bs_buf_size((void*)audio_ctrl.work_mem,audio_ctrl.ring_size);
#endif

	audio_ctrl.wi = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);

	audio_ctrl.wi = audio_avi_mp3_write(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	ret = mp3_dec_parsing((void*)audio_ctrl.work_mem , audio_ctrl.wi);

	switch(ret)
	{		
		case MP3_DEC_ERR_NONE:
			break;
		case MP3_DEC_ERR_LOSTSYNC:		//not found sync word
		case MP3_DEC_ERR_BADSAMPLERATE:	//reserved sample frequency value
		case MP3_DEC_ERR_BADBITRATE:		//forbidden bitrate value
		case MP3_DEC_ERR_BADLAYER:
		case MP3_DEC_ERR_BADMPEGID:
		default:
			return AUDIO_ERR_DEC_FAIL;
	}

	channel = mp3_dec_get_channel((void*)audio_ctrl.work_mem);
	g_audio_sample_rate = mp3_dec_get_samplerate((void*)audio_ctrl.work_mem);
	dac_sample_rate_set(g_audio_sample_rate);
	dac_stereo_set();

	DBG_PRINT("bps: %d\r\n",mp3_dec_get_bitrate((void*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",mp3_dec_get_channel((void*)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n",mp3_dec_get_samplerate((void*)audio_ctrl.work_mem));
	DBG_PRINT("block size: %d\r\n",mp3_dec_get_mem_block_size());

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}
	return AUDIO_ERR_NONE;
}
#endif

#if APP_WAV_CODEC_FG_EN == 1
INT32S  audio_avi_process(void)
{
	INT32S	pcm_point;
	INT8U   ch;
	INT32U  wb_idx;
	INT8U   err;
	INT32S  wi;
#if 1	
	INT32U	i;
	INT16S	*ptr;
#endif

	audio_ctrl.ri = wav_dec_get_ri((INT8U *)audio_ctrl.work_mem);
	if(audio_ctrl.f_last == 0) {
		wi = audio_avi_write(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (wi == -2) {
			return AUDIO_ERR_AVI_READ_FAIL;
		}	
		audio_ctrl.wi = wi;
	}

	if(audio_ctrl.f_last == 1)
	{
		if(audio_ctrl.ri == audio_ctrl.wi)
		{
#if 1
			if(stopped != 0)
			{
				for(i = 0;i< MAX_DAC_BUFFERS;i++)
				{
					if(pcm_len[i]== 0)
					{
						ptr = pcm_out[i];
						*ptr = 0;
						*(++ptr) = 0;
						pcm_len[i] = 2;
						OSQPost(aud_send_q, (void *) i);
						audio_send_to_dma();
						audio_send_next_frame_q();
						return 0;						
					}				
				}
			}
#endif			
			//msgQSend(ApQ, MSG_AVI_AUDIO_END, NULL, 0, MSG_PRI_URGENT);
			return AUDIO_ERR_DEC_FINISH;
		}
	}
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	pcm_point = wav_dec_run((INT8U *)audio_ctrl.work_mem,pcm_out[wb_idx],audio_ctrl.wi);

	if (pcm_point == -1) {
		return AUDIO_ERR_DEC_FAIL;
	}

	if (pcm_point <= 0) {
		if (--audio_ctrl.try_cnt == 0) {
			return AUDIO_ERR_DEC_FAIL;
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}

	ch = wav_dec_get_nChannels((INT8U *)audio_ctrl.work_mem);
	//last_ldata = *(pcm_out[wb_idx] + pcm_point*ch-2);
	//last_rdata = *(pcm_out[wb_idx] + pcm_point*ch-1);
	
	pcm_len[wb_idx] = pcm_point*ch;

	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();

	audio_send_next_frame_q();

	return 0;
}
#endif

#if APP_MP3_DECODE_FG_EN == 1
INT32S  audio_avi_mp3_process(void)
{
	INT32S	pcm_point;
	INT32U  wb_idx;
	INT8U   err;
#if 1	
	INT32U	i;
	INT16S	*ptr;
#endif

	audio_ctrl.ri = mp3_dec_get_ri((void *)audio_ctrl.work_mem);

	if(audio_ctrl.f_last == 0)
	audio_ctrl.wi = audio_avi_mp3_write(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	if(audio_ctrl.f_last == 1)
	{
		if(audio_ctrl.ri == audio_ctrl.wi)
		{
#if 1
			if(stopped != 0)
			{
				for(i = 0;i< MAX_DAC_BUFFERS;i++)
				{
					if(pcm_len[i]== 0)
					{
						ptr = pcm_out[i];
						*ptr = 0;
						*(++ptr) = 0;
						pcm_len[i] = 2;
						OSQPost(aud_send_q, (void *) i);
						audio_send_to_dma();
						audio_send_next_frame_q();
						return 0;						
					}				
				}
			}
			else
#endif			
			{
				//msgQSend(ApQ, MSG_AVI_AUDIO_END, NULL, 0, MSG_PRI_URGENT);
				return AUDIO_ERR_DEC_FINISH;
			}	
		}
	}
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
#if GPLIB_MP3_HW_EN == 1	
	pcm_point = mp3_dec_run((void*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi,audio_ctrl.f_last);
#else
	pcm_point = mp3_dec_run((void*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi);
#endif	
	if (pcm_point == 0) {
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
	pcm_len[wb_idx] = pcm_point*2;
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}	
#endif

#if APP_WAV_CODEC_FG_EN == 1
INT32S audio_avi_write(INT8U *ring_buf, INT32U wi, INT32U ri)
{
	INT32S len;
	INT32S templen;
	/*
	if((wi == 0 && ri == 0)||(wi == ri)) {
		len = audio_avi_read((INT32U)ring_buf + wi, audio_ctrl.ring_size/2);
        if(len != -1)
	    {
	        wi += len;
    	    if(wi == audio_ctrl.ring_size) {
				wi = 0;
			}
        	return wi;
        }
        else
        	return -1;
    }
	*/
	if(wi == 0 && ri == 0) {
		len = audio_avi_read((INT32U)ring_buf + wi, AUDIO_AVI_RINGBUF_LEN);
		if(len >= 0)
		{
	        wi += len;
       		return wi;
       	}
       	else if (len == -2) 
       	{
       		return len;	
       	}
       	else
       	{
	       	audio_ctrl.f_last = 1;
       		return wi;
       	}
    }

	if(wi == ri)
	{
		if(RING_BUF_SIZE >= wi+AUDIO_AVI_RINGBUF_LEN)
		{
			len = audio_avi_read((INT32U)ring_buf + wi, AUDIO_AVI_RINGBUF_LEN);
			if(len >=0)
		    {
		        wi += len;
		        if(wi == audio_ctrl.ring_size)
		        	wi = 0;
	       	}
	       	else if (len == -2) 
       		{
       			return len;	
       		}
	       	else
	       	{
				audio_ctrl.f_last = 1;
	       		return wi;
	       	}
		}
		else
		{
			templen = RING_BUF_SIZE - wi;
			len = audio_avi_read((INT32U)ring_buf + wi, templen);
			if(len >=0)
		    {
		        wi += len;
		        if(wi == audio_ctrl.ring_size)
		        	wi = 0;
	       	}
	       	else if (len == -2) 
       		{
       			return len;	
       		}
	       	else
	       	{
		       	audio_ctrl.f_last = 1;
	       		return wi;
	       	}
			if(templen == len)
			{
				templen = AUDIO_AVI_RINGBUF_LEN - len;
				len = audio_avi_read((INT32U)ring_buf + wi, templen);
				if(len >=0)
		        	wi += len;
		        else if (len == -2) 
       			{
       				return len;	
       			}
	       		else
	       		{
		       		audio_ctrl.f_last = 1;
	       			return wi;
	       		}
			}
		}
		return wi;
	}
	else
	{
		if(wi<ri)
		{
			if(AUDIO_AVI_RINGBUF_LEN < (ri -wi))
			{
				templen = AUDIO_AVI_RINGBUF_LEN;
				len = audio_avi_read((INT32U)ring_buf + wi, templen);
				if(len >= 0)
			    {
			        wi += len;
			        if(wi == audio_ctrl.ring_size)
			        	wi = 0;
		       	}
		       	else if (len == -2) 
       			{
       				return len;	
       			}
				else
				{
					audio_ctrl.f_last = 1;
					return wi;
				}
			}
		}
		else
		{
			//wi>ri
			if(audio_ctrl.ring_size < AUDIO_AVI_RINGBUF_LEN + wi)
			{
				templen = audio_ctrl.ring_size - wi;
				if(AUDIO_AVI_RINGBUF_LEN <= templen + ri)
				{
					len = audio_avi_read((INT32U)ring_buf + wi, templen);
					if(len >= 0)
					{
						wi += len;
					    if(wi == audio_ctrl.ring_size)
					    	wi = 0;
						if(templen == len)
						{
							templen = AUDIO_AVI_RINGBUF_LEN - len;
							len = audio_avi_read((INT32U)ring_buf + wi, templen);
							if(len >=0)
							{
					        	wi += len;
					        	return wi;
					        }
					        else if (len == -2) 
       						{	
       							return len;	
       						}	
				       		else
				       		{
					       		audio_ctrl.f_last = 1;
				       			return wi;
				       		}
						}					    	
					}
					else if (len == -2) 
       				{
       					return len;	
       				}   
  				    else
					{
						audio_ctrl.f_last = 1;
						return wi;
					}						
				}
			}
			else
				templen = AUDIO_AVI_RINGBUF_LEN;
			
			len = audio_avi_read((INT32U)ring_buf + wi, templen);
			if(len >= 0)
			{
				wi += len;

			    if(wi == audio_ctrl.ring_size)
			    	wi = 0;

			    return wi;
		     }
		     else if (len == -2) 
       		{
       			return len;	
       		}
			 else
			 {
				audio_ctrl.f_last = 1;
				return wi;
			 }
			/*
			if(audio_ctrl.ring_size>= AUDIO_AVI_RINGBUF_LEN + wi)
				templen = AUDIO_AVI_RINGBUF_LEN;
			else
				templen = audio_ctrl.ring_size - wi;

			len = audio_avi_read((INT32U)ring_buf + wi, templen);
			if(len != -1)
			{
				wi += len;

			    if(wi == audio_ctrl.ring_size)
			    	wi = 0;

			    return wi;
		     }
			 else
			 {
				audio_ctrl.f_last = 1;
				return wi;
			 }
			 */
		}
	}
	return wi;
}
#endif

#if APP_MP3_DECODE_FG_EN == 1	
INT32S audio_avi_mp3_write(INT8U *ring_buf, INT32U wi, INT32U ri)
{
	INT32S len;
	INT32S templen;

	if(wi == 0 && ri == 0) 
	{
		len = audio_avi_read((INT32U)ring_buf + wi, (RING_BUF_SIZE>>1));
		if(len !=-1)
		{
		    wi += len;
	    	return wi;
	   	}
	   	else
	   	{
			audio_ctrl.f_last = 1;
	      	return wi;
	    }
  	}

	if(wi == ri)
	{
		if(wi <= (RING_BUF_SIZE>>1))
		{
			len = audio_avi_read((INT32U)ring_buf + wi, (RING_BUF_SIZE>>1));
			if(len !=-1)
			{
			    wi += len;
			    if(wi == audio_ctrl.ring_size)
			    	wi = 0;
		    	return wi;
		   	}
	   		else
	   		{
				audio_ctrl.f_last = 1;
		      return wi;
		    }			
		}
		else
		{
			templen = RING_BUF_SIZE - wi;
			len = audio_avi_read((INT32U)ring_buf + wi, templen);
			if(len !=-1)
			{
			    wi += len;
			    if(wi == audio_ctrl.ring_size)
			    {
			    	wi = 0;
			    	templen = (RING_BUF_SIZE>>1) - templen;
			    	len = audio_avi_read((INT32U)ring_buf + wi, templen);
					if(len !=-1)
					{
			    		wi += len;
			    		return wi;
			    	}
			    	else
			    	{
						audio_ctrl.f_last = 1;
				      	return wi;		    	
			   		}
	    		}
	    		else // data is read end
	    			return wi; 
	   		}
	   		else
	   		{
				audio_ctrl.f_last = 1;
	      		return wi;
	    	}
	    }						
	}
	else if(wi>ri)
	{
		if((ri > 0)&&(wi >= (RING_BUF_SIZE>>1)))
		{
			len = audio_avi_read((INT32U)ring_buf + wi, (RING_BUF_SIZE>>1));
			if(len !=-1)
			{
				wi += len;
				if(wi == audio_ctrl.ring_size)
					wi = 0;
			 	return wi;
			}
			else
			{
				audio_ctrl.f_last = 1;
				return wi;
			}							
		}
	}
	else
	{
		if((ri > (RING_BUF_SIZE>>1))&&(wi < (RING_BUF_SIZE>>1)))
		{
			len = audio_avi_read((INT32U)ring_buf + wi, (RING_BUF_SIZE>>1));
			if(len !=-1)
			{
				wi += len;
				if(wi == audio_ctrl.ring_size)
					wi = 0;
			 	return wi;
			}
			else
			{
				audio_ctrl.f_last = 1;
				return wi;
			}										
		}
	}
	return wi;
}
#endif

#if APP_A1800_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_a1800_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;
	
	while(1)
	{
		audio_ctrl.ri = a1800dec_read_index((INT8U *)audio_ctrl.work_mem);//word to byte
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}

		if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
			return 0; //AUDIO_ERR_DEC_FINISH;
		}

		in_length = audio_ctrl.ri;
		pcm_point = a1800dec_run((CHAR*)audio_ctrl.work_mem, audio_ctrl.wi, Buffer);
		audio_decode_sample_number += pcm_point;
		audio_ctrl.ri = a1800dec_read_index((CHAR*)audio_ctrl.work_mem);//word to byte
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length >= audio_ctrl.ri) {
			//audio_ctrl.file_cnt += A18_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0)
		{
			if (pcm_point < 0)
			{
				if (--audio_ctrl.try_cnt == 0)
				{
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
	return pcm_point;
}
#endif

INT32S audio_a1800_play_init(void)
{
	INT32U  count;
	INT32S  ret;
	INT32U  in_length;
	INT32S  t_wi;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	audio_decode_sample_number = 0;
#endif
	audio_ctrl.file_cnt = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;//080724

	gp_memset((INT8S*)audio_ctrl.ring_buf,0,RING_BUF_SIZE);
	audio_ctrl.ring_size = A18_DEC_BITSTREAM_BUFFER_SIZE;//must be 256 byte for a1800

	ret = a1800dec_GetMemoryBlockSize();//080723
	if(ret != A1800DEC_MEMORY_BLOCK_SIZE)
		return AUDIO_ERR_DEC_FAIL;

	ret = a1800dec_init((void *)audio_ctrl.work_mem, (const unsigned char *)audio_ctrl.ring_buf);//080723
	audio_ctrl.wi = a1800dec_read_index((void *)audio_ctrl.work_mem);//after initial the value is 0
	audio_ctrl.ri = a1800dec_read_index((void *)audio_ctrl.work_mem);
	in_length = (audio_ctrl.ri);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	ret = a1800dec_parsing((void *)audio_ctrl.work_mem, audio_ctrl.wi);
	if(ret != 1)
	{
		return AUDIO_ERR_DEC_FAIL;
	}
	
	audio_ctrl.ri = a1800dec_read_index((INT8U *)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += A18_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}
	
	in_length = A18_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)	
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_a1800_get_output;
	ret = A18_dec_get_channel((CHAR*)audio_ctrl.work_mem);

#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ret == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ret, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ret = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ret == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ret, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ret = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100 / in_length;
	if(count > 4)	count = 4;
	if(hUpSample && count>=2)
	{
		UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ret = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
	}
#endif
#endif
	dac_sample_rate_set(in_length);	//a1800 is 16khz sample rate
	dac_mono_set();//a1800 is mono
	channel = 1;
	g_audio_sample_rate = in_length;
	DBG_PRINT("bps: %d\r\n",A18_dec_get_bitrate((void *)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n",A18_dec_get_samplerate((CHAR*)audio_ctrl.work_mem));

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_a1800_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT32S  t_wi;

	audio_ctrl.ri = a1800dec_read_index((INT8U *)audio_ctrl.work_mem);//word to byte

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	{
			return AUDIO_ERR_DEC_FINISH;
	}

	in_length = audio_ctrl.ri;
	pcm_point = a1800dec_run((CHAR*)audio_ctrl.work_mem, audio_ctrl.wi, pcm_out[wb_idx]);

	audio_ctrl.ri = a1800dec_read_index((CHAR*)audio_ctrl.work_mem);//word to byte
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += A18_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0)
	{
		if (pcm_point < 0)
		{
			if (--audio_ctrl.try_cnt == 0)
			{
				return AUDIO_ERR_DEC_FAIL;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
#else
	INT32S pcm_point;
	INT8U   err;
	INT32U  wb_idx;
	
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], A18_DEC_FRAMESIZE);
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;
#endif

	pcm_len[wb_idx] = pcm_point; /* 1 channel */
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif // #if APP_A1800_DECODE_FG_EN == 1

//-----------------------------------------------------------------
// added by Bruce, 2008/09/23
//===============================================================================================================
//   A1600 Playback
//===============================================================================================================
#if APP_A1600_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_a16_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;

	while(1)
	{
		audio_ctrl.ri = A16_dec_get_ri((void *)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}
		if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
		{
			return 0;	//AUDIO_ERR_DEC_FINISH);
		}

		in_length = audio_ctrl.ri;
		pcm_point = A16_dec_run((CHAR*)audio_ctrl.work_mem, Buffer, audio_ctrl.wi);

		audio_ctrl.ri = A16_dec_get_ri((CHAR*)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += A16_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0)
		{
			if (pcm_point < 0)
			{
				if (--audio_ctrl.try_cnt == 0)
				{
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
	return pcm_point;	
}
#endif

INT32S audio_a16_play_init(void)
{
	INT32U  count;
	INT32S  ret = 0;
	INT32U  in_length;
	INT32S  t_wi;

	audio_ctrl.file_cnt = 0;
	audio_ctrl.f_last = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;//080724

	audio_ctrl.ring_size = A16_DEC_BITSTREAM_BUFFER_SIZE;

	ret = A16_dec_get_mem_block_size();
	if(ret != A16_DEC_MEMORY_SIZE)
		return AUDIO_ERR_DEC_FAIL;

	ret = A16_dec_init((char *)audio_ctrl.work_mem, (unsigned char *)audio_ctrl.ring_buf);//080723
	audio_ctrl.wi = A16_dec_get_ri((void *)audio_ctrl.work_mem);//after initial the value is 0
	audio_ctrl.ri = A16_dec_get_ri((void *)audio_ctrl.work_mem);
	//in_length = (audio_ctrl.ri*2);//word to byte
	in_length = audio_ctrl.ri;

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	count = 500;
	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = A16_dec_parsing((CHAR*)audio_ctrl.work_mem , audio_ctrl.wi);

		audio_ctrl.ri = A16_dec_get_ri((CHAR*)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			//audio_ctrl.file_cnt += A16_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		switch(ret)
		{
			case A16_OK:
				break;
			case A16_E_NO_MORE_SRCDATA:		//not found sync word
			case A16_E_READ_IN_BUFFER:	//reserved sample frequency value
			case A16_CODE_FILE_FORMAT_ERR:		//forbidden bitrate value
			case A16_E_FILE_END:
			case A16_E_MODE_ERR:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = A16_dec_get_ri((CHAR*)audio_ctrl.work_mem);
				//audio_ctrl.wi = audio_write_buffer(audio_ctrl.file_handle, audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)
				{
					return AUDIO_ERR_READ_FAIL;
				}

				if (--count == 0)
					return AUDIO_ERR_FAILED;
				continue;
			default:
				return AUDIO_ERR_FAILED;
		}
		if(ret == A16_OK) {
			break;
		}
	}

	in_length = A16_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_a16_get_output;
	ret = 1;	//channel

#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ret == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ret, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ret = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ret == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ret, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ret = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100 / in_length;
	if(count > 4)	count = 4;
	if(hUpSample && count >= 2)
	{
		UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ret = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
	}
#endif
#endif
	dac_mono_set();
	dac_sample_rate_set(in_length);
	channel = 1;
	g_audio_sample_rate = in_length;
	DBG_PRINT("bps: %d\r\n",A16_dec_get_bitspersample((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",A16_dec_get_channel((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n", A16_dec_get_samplerate((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("block size: %d\r\n",A16_dec_get_mem_block_size());

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_a16_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S  pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT32S  t_wi;

	audio_ctrl.ri = A16_dec_get_ri((void *)audio_ctrl.work_mem);
	//audio_ctrl.ri = A16_dec_get_ri((INT8U *)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}


	if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	{
		//if(audio_ctrl.f_last){
			return AUDIO_ERR_DEC_FINISH;
		//}
		//else{
		//	audio_ctrl.f_last = 1;
		//}
	}

	in_length = audio_ctrl.ri;
	//pcm_point = a1800dec_run((CHAR*)audio_ctrl.work_mem, audio_ctrl.wi/2, pcm_out[wb_idx]);
	pcm_point = A16_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi);

	audio_ctrl.ri = A16_dec_get_ri((CHAR*)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += A16_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0)
	{
		if (pcm_point < 0)
		{
			if (--audio_ctrl.try_cnt == 0)
			{
				return AUDIO_ERR_DEC_FAIL;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], audio_ctrl.ring_size);
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;
#endif
	
	pcm_len[wb_idx] = pcm_point;
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif	// #if APP_A1600_DECODE_FG_EN == 1

//===============================================================================================================
//   A6400 Playback
//===============================================================================================================
#if APP_A6400_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_a64_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;

	while(1)
	{
		audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}
	#if GPLIB_MP3_HW_EN == 1 
		if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	#else
		if(audio_ctrl.file_cnt - audio_ctrl.file_len >= -1)
	#endif	
		{
			if(audio_ctrl.f_last){
				return 0; //AUDIO_ERR_DEC_FINISH);
			}
			else{
				audio_ctrl.f_last = 1;
			}
		}

		in_length = audio_ctrl.ri;
	#if GPLIB_MP3_HW_EN == 1 
		pcm_point = a6400_dec_run((CHAR*)audio_ctrl.work_mem, Buffer, audio_ctrl.wi,audio_ctrl.f_last);
	#else
		pcm_point = a6400_dec_run((CHAR*)audio_ctrl.work_mem, Buffer, audio_ctrl.wi);
	#endif	
		audio_decode_sample_number += pcm_point;
		audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += A6400_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0) {
			if (pcm_point < 0) {
				if (--audio_ctrl.try_cnt == 0) {
					return (0 - AUDIO_ERR_DEC_FINISH);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
#if GPLIB_MP3_HW_EN == 1	
	return pcm_point*2; /* 2 channel */
#else
	return pcm_point*a6400_dec_get_channel((CHAR*)work_mem);
#endif
}
#endif

INT32S audio_a64_play_init(void)
{
	INT32U  count;
	INT32S  ret = 0;
	INT32U  in_length;
	INT32S  t_wi;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	audio_decode_sample_number = 0;
#endif
	audio_ctrl.file_cnt = 0;
	audio_ctrl.f_last = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;

	audio_ctrl.ring_size = A6400_DEC_BITSTREAM_BUFFER_SIZE;

	ret = a6400_dec_get_mem_block_size();
	if(ret != A6400_DEC_MEMORY_SIZE)
		return AUDIO_ERR_DEC_FAIL;

#if GPLIB_MP3_HW_EN == 1
	ret = a6400_dec_init((void *)audio_ctrl.work_mem, (char *)audio_ctrl.ring_buf);
#else
	ret = a6400_dec_init((char*)audio_ctrl.work_mem, (unsigned char*)audio_ctrl.ring_buf, (char*)(audio_ctrl.work_mem + A6400_DEC_MEMORY_SIZE));
	a6400_dec_set_bs_buf_size((char*)audio_ctrl.work_mem, audio_ctrl.ring_size);
#endif	
	audio_ctrl.wi = a6400_dec_get_ri((void *)audio_ctrl.work_mem);//after initial the value is 0
	audio_ctrl.ri = a6400_dec_get_ri((void *)audio_ctrl.work_mem);
	//in_length = (audio_ctrl.ri*2);//word to byte
	in_length = audio_ctrl.ri;

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	count = 500;
	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = a6400_dec_parsing((CHAR*)audio_ctrl.work_mem , audio_ctrl.wi);

		audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			//audio_ctrl.file_cnt += A6400_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		switch(ret)
		{
			case A6400_DEC_ERR_NONE:
				break;
			case A6400_DEC_ERR_LOSTSYNC:		//not found sync word
			case A6400_DEC_ERR_BADSAMPLERATE:	//reserved sample frequency value
			case A6400_DEC_ERR_BADBITRATE:		//forbidden bitrate value
			case A6400_DEC_ERR_BADLAYER:
			case A6400_DEC_ERR_BADMPEGID:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);

				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)
				{
					return AUDIO_ERR_READ_FAIL;
				}

				if (--count == 0)
					return AUDIO_ERR_FAILED;
				continue;
			default:
				return AUDIO_ERR_FAILED;
		}
		if(ret == A6400_DEC_ERR_NONE) {
			break;
		}
	}

	in_length = a6400_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
//	dac_stereo_set();
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_a64_get_output;
	//ret = a6400_dec_get_channel((CHAR*)audio_ctrl.work_mem);
#if GPLIB_MP3_HW_EN == 1	
	//ret = mp3_dec_get_channel((CHAR*)audio_ctrl.work_mem);
	ret = 2; //mp3 decode force to 2 channel by h/w
    dac_stereo_set();
#else
	ret = a6400_dec_get_channel((CHAR*)audio_ctrl.work_mem);
	if(ret == 1) {
		dac_mono_set();
	} else {
		dac_stereo_set();
	}
#endif
#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ret == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ret, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ret = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ret == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ret, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ret = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100 / in_length;
	if(count > 4)  count = 4;
	if(hUpSample && count >= 2)
	{
		if(ret == 1)
			UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 1);
		else
			UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ret = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
		if(ret == 1)
			dac_mono_set();
		else
			dac_stereo_set();
	}
#endif //APP_UP_SAMPLE_EN
#endif
	
	dac_sample_rate_set(in_length);
	channel = ret;
	g_audio_sample_rate = in_length;
	DBG_PRINT("bps: %d\r\n",a6400_dec_get_bitrate((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",a6400_dec_get_channel((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n",a6400_dec_get_samplerate((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("block size: %d\r\n",a6400_dec_get_mem_block_size());

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_a64_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S  pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT32S  t_wi;

	audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	{
		if(audio_ctrl.f_last){
			return AUDIO_ERR_DEC_FINISH;
		}
		else{
			audio_ctrl.f_last = 1;
		}
	}


	in_length = audio_ctrl.ri;
#if GPLIB_MP3_HW_EN == 1 
	pcm_point = a6400_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi,audio_ctrl.f_last);
#else
	pcm_point = a6400_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi);
#endif	

	audio_ctrl.ri = a6400_dec_get_ri((CHAR*)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += A6400_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0) {
		if (pcm_point < 0) {
			if (--audio_ctrl.try_cnt == 0) {
				return AUDIO_ERR_DEC_FINISH;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}

	pcm_len[wb_idx] = pcm_point*2;
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], A6400_DEC_FRAMESIZE);//audio_ctrl.ring_size

	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;
	pcm_len[wb_idx] = pcm_point;
#endif

	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif // #if APP_A6400_DECODE_FG_EN == 1

//===============================================================================================================
//   S880 Playback
//===============================================================================================================
#if APP_S880_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_s880_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;

	while(1)
	{
		audio_ctrl.ri = S880_dec_get_ri((void *)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}

		if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
		{
			return 0; //AUDIO_ERR_DEC_FINISH);
		}

		in_length = audio_ctrl.ri;
		pcm_point = S880_dec_run((CHAR*)audio_ctrl.work_mem, Buffer, audio_ctrl.wi);

		audio_ctrl.ri = S880_dec_get_ri((CHAR*)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			//audio_ctrl.file_cnt += S880_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		if (pcm_point <= 0)
		{
			if (pcm_point < 0)
			{
				if (--audio_ctrl.try_cnt == 0)
				{
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
	return pcm_point;
}
#endif

INT32S audio_s880_play_init(void)
{
	INT32U  count;
	INT32S  ret = 0;
	INT32U  in_length;
	INT32S  t_wi;

	audio_ctrl.file_cnt = 0;
	audio_ctrl.f_last = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;//080724

	audio_ctrl.ring_size = S880_DEC_BITSTREAM_BUFFER_SIZE;

	ret = S880_dec_get_mem_block_size();
	if(ret != S880_DEC_MEMORY_SIZE)
		return AUDIO_ERR_DEC_FAIL;

	ret = S880_dec_init((char *)audio_ctrl.work_mem, (unsigned char *)audio_ctrl.ring_buf);//080723
	audio_ctrl.wi = S880_dec_get_ri((void *)audio_ctrl.work_mem);//after initial the value is 0
	audio_ctrl.ri = S880_dec_get_ri((void *)audio_ctrl.work_mem);
	//in_length = (audio_ctrl.ri*2);//word to byte
	in_length = audio_ctrl.ri;

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}

	count = 500;
	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = S880_dec_parsing((CHAR*)audio_ctrl.work_mem , audio_ctrl.wi);

		audio_ctrl.ri = S880_dec_get_ri((CHAR*)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			//audio_ctrl.file_cnt += S880_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
			audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
		}

		switch(ret)
		{
			case S880_OK:
				break;
			case S880_E_NO_MORE_SRCDATA:		//not found sync word
			case S880_E_READ_IN_BUFFER:	//reserved sample frequency value
			case S880_CODE_FILE_FORMAT_ERR:		//forbidden bitrate value
			case S880_E_FILE_END:
		//	case S880_E_MODE_ERR:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = S880_dec_get_ri((CHAR*)audio_ctrl.work_mem);
				//audio_ctrl.wi = audio_write_buffer(audio_ctrl.file_handle, audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE)
				{
					return AUDIO_ERR_READ_FAIL;
				}

				if (--count == 0)
					return AUDIO_ERR_FAILED;
				continue;
			default:
				return AUDIO_ERR_FAILED;
		}
		if(ret == S880_OK) {
			break;
		}
	}

	in_length = S880_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_s880_get_output;
	ret = S880_dec_get_channel((CHAR*)audio_ctrl.work_mem);

#if APP_CONST_PITCH_EN
	if(hConstPitch && G_snd_info.ConstPitchEn && ret == 1)
	{
		if(in_length == 8000 || in_length == 16000 || in_length == 11025 || in_length == 22050)
		{
			ConstantPitch_Link(hConstPitch, hSrc, pfnGetOutput, in_length, ret, 0);
			ConstantPitch_SetParam(hConstPitch, G_snd_info.Pitch_idx, 0);
			hSrc = hConstPitch;
			in_length = ConstantPitch_GetSampleRate(hConstPitch);
			ret = ConstantPitch_GetChannel(hConstPitch);
			pfnGetOutput = &ConstantPitch_GetOutput;
		}
	}
#endif
#if APP_ECHO_EN
	if(hEcho && G_snd_info.EchoEn && ret == 1)
	{
		Echo_Link(hEcho, hSrc, pfnGetOutput, in_length, ret, 0);
		Echo_SetParam(hEcho, G_snd_info.delay_len, G_snd_info.weight_idx);
		hSrc = hEcho;
		in_length = Echo_GetSampleRate(hEcho);
		ret = Echo_GetChannel(hEcho);
		pfnGetOutput = &Echo_GetOutput;
	}
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
#if APP_UP_SAMPLE_EN
	count = 44100 / in_length;
	if(count > 4)	count = 4;
	if(hUpSample && count >= 2)
	{
		UpSample_Link(hUpSample, hSrc, pfnGetOutput, in_length, ret, count, 0);
		hSrc = hUpSample;
		in_length = UpSample_GetSampleRate(hUpSample);
		ret = UpSample_GetChannel(hUpSample);
		pfnGetOutput = &UpSample_GetOutput;	
	}
#endif
#endif
	
	dac_mono_set();
	dac_sample_rate_set(in_length);
	channel = 1;
	g_audio_sample_rate = in_length;
	DBG_PRINT("bps: %d\r\n",S880_dec_get_bitspersample((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",S880_dec_get_channel((CHAR*)audio_ctrl.work_mem));
	DBG_PRINT("sample rate: %d\r\n", ret);
	DBG_PRINT("block size: %d\r\n",S880_dec_get_mem_block_size());

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;
}

INT32S  audio_s880_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S  pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT32S  t_wi;

	audio_ctrl.ri = S880_dec_get_ri((void *)audio_ctrl.work_mem);
	//audio_ctrl.ri = S880_dec_get_ri((INT8U *)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}


	if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
	{
		//if(audio_ctrl.f_last){
			return AUDIO_ERR_DEC_FINISH;
		//}
		//else{
		//	audio_ctrl.f_last = 1;
		//}
	}

	in_length = audio_ctrl.ri;
	//pcm_point = a1800dec_run((CHAR*)audio_ctrl.work_mem, audio_ctrl.wi/2, pcm_out[wb_idx]);
	pcm_point = S880_dec_run((CHAR*)audio_ctrl.work_mem, pcm_out[wb_idx], audio_ctrl.wi);

	audio_ctrl.ri = S880_dec_get_ri((CHAR*)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		//audio_ctrl.file_cnt += S880_DEC_BITSTREAM_BUFFER_SIZE;//101020 mask
		audio_ctrl.file_cnt += audio_ctrl.ring_size;//101020
	}

	if (pcm_point <= 0)
	{
		if (pcm_point < 0)
		{
			if (--audio_ctrl.try_cnt == 0)
			{
				return AUDIO_ERR_DEC_FAIL;
			}
		}
		OSQPostFront(audio_wq, (void *)wb_idx);
		audio_send_next_frame_q();
		return 0;
	}
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], S880_DEC_FRAMESIZE);//audio_ctrl.ring_size
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH;
#endif

	pcm_len[wb_idx] = pcm_point;
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();
	return 0;
}
#endif // #if APP_S880_DECODE_FG_EN == 1

#if (defined SKIP_ID3_TAG) && (SKIP_ID3_TAG == 1)
static INT8U audio_get_id3_type(INT8U *data, INT32U length)
{
	if (length >= 3 && data[0] == 'T' && data[1] == 'A' && data[2] == 'G') {
    	return ID3_TAG_V1;
	}
	
  	if (length >= 10 && ((data[0] == 'I' && data[1] == 'D' && data[2] == '3') ||
      	(data[0] == '3' && data[1] == 'D' && data[2] == 'I')) && data[3] < 0xff && data[4] < 0xff)
   	{ 
   		if (data[0] == 'I')
   			return ID3_TAG_V2;
   		else
   			return ID3_TAG_V2_FOOTER;
	}
  	return ID3_TAG_NONE;
}

static void audio_parse_id3_header(INT8U *header, INT32U *version, INT32S *flags, INT32U *size)
{
	INT8U *ptr;
	INT32U ver = 0;
	ptr = header;
  	ptr += 3;

  	ver = *ptr++ << 8; 
  	*version = ver | *ptr++;
  	*flags   = *ptr++;
  	*size    = audio_id3_get_size(ptr);
 
}

static INT32U audio_id3_get_size(INT8U *ptr)
{
	INT32U value = 0;
	
	value = (value << 7) | (*ptr++ & 0x7f);
    value = (value << 7) | (*ptr++ & 0x7f);
	value = (value << 7) | (*ptr++ & 0x7f);
	value = (value << 7) | (*ptr++ & 0x7f);
 
  	return value;
}

INT32S audio_id3_get_tag_len(INT8U *data, INT32U length)
{
  INT32U version;
  INT32S flags;
  INT32U len;

  switch (audio_get_id3_type(data, length)) {
 	case ID3_TAG_V1:
    	return 128;
  	case ID3_TAG_V2:
    	audio_parse_id3_header(data, &version, &flags, &len);
    	if (flags & ID3_TAG_FLAG_FOOTER) {
      		len += 10;
		}
    	return 10 + len;
  	case ID3_TAG_NONE:
    	break;
  }

  return 0;
}
#endif
//===============================================================================================================
//  AAC Playback
//===============================================================================================================
#if APP_AAC_DECODE_FG_EN == 1
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
INT32S audio_aac_get_output(void *work_mem, short *Buffer, INT32S MaxLen)
{
	INT32S  pcm_point;
	INT32U  in_length;
	INT32S  t_wi;

	while(1)
	{
		audio_ctrl.ri = aac_dec_get_read_index((void *)audio_ctrl.work_mem);
		t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
		if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
			return (0 - AUDIO_ERR_DEC_FAIL);
		}

		if(audio_ctrl.file_cnt >= audio_ctrl.file_len)
		{
			return -1;
			//return 0; //AUDIO_ERR_DEC_FINISH);
		}

		in_length = audio_ctrl.ri;
		pcm_point = aac_dec_run((CHAR*)audio_ctrl.work_mem, audio_ctrl.wi, Buffer);

		audio_ctrl.ri = aac_dec_get_read_index((CHAR*)audio_ctrl.work_mem);
		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

		if(in_length > audio_ctrl.ri) {
			audio_ctrl.file_cnt += AAC_DEC_BITSTREAM_BUFFER_SIZE;
		}

		if (pcm_point <= 0)
		{
			if (pcm_point < 0)
			{
				if (--audio_ctrl.try_cnt == 0)
				{
					return (0 - AUDIO_ERR_DEC_FAIL);
				}
			}
			//OSQPostFront(audio_wq, (void *)wb_idx);
			//audio_send_next_frame_q();
			//return 0;
		}
		else
			break;
	}
	return pcm_point*2;
}
#endif

INT32S audio_aac_play_init(void)
{
	INT32U  count;
	INT32S  ret = 0;
	INT32U  in_length;
	INT32S  t_wi;
	INT32S downMatrix = 0;	// 1: input 5.1 channels and output 2 channels
	INT8U  ch;


	// mp3 init
	gp_memset((INT8S*)audio_ctrl.ring_buf,0,RING_BUF_SIZE);
	ret = aac_dec_init((void*)audio_ctrl.work_mem, downMatrix, (void*)audio_ctrl.ring_buf);
	aac_dec_SetRingBufferSize((void*)audio_ctrl.work_mem,audio_ctrl.ring_size);
	
	audio_ctrl.file_cnt = 0;
	audio_ctrl.f_last = 0;
	audio_ctrl.try_cnt = 20;
	audio_ctrl.read_secs = 0;

	audio_ctrl.wi = 0;
	audio_ctrl.ri = aac_dec_get_read_index((void*)audio_ctrl.work_mem);

	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}
	
	count = 500;
	while(1)
	{
		in_length = audio_ctrl.ri;
		ret = aac_dec_parsing((void*)audio_ctrl.work_mem , audio_ctrl.wi);

		audio_ctrl.ri = aac_dec_get_read_index((void*)audio_ctrl.work_mem);

		audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);
		if(audio_ctrl.ri < in_length) {
			audio_ctrl.file_cnt += AAC_DEC_BITSTREAM_BUFFER_SIZE;
		}

		if(audio_ctrl.file_cnt>=audio_ctrl.file_len)
		{
			//AACapi->error_msg = AAC_E_UNSUPPORTED_FILE_FORMAT;
			return AUDIO_ERR_FAILED;
		}

		switch(ret)
		{
			case AAC_OK:
				break;

			case  UNABLE_TO_FIND_ADTS_SYNCWORD:
				//feed in DecodeInBuffer;
				audio_ctrl.ri = aac_dec_get_read_index((char *)audio_ctrl.work_mem);
				t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);
				if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
					return AUDIO_ERR_READ_FAIL;
				}
				if (--count == 0)
					return AUDIO_ERR_DEC_FAIL;
				continue;
				
			case UNSUPPORTED_FILE_FORMAT_MP4:		
			case NOT_MONO_OR_STEREO:
			case NOT_LC_OBJECT_TYPE:
			default:
				return AUDIO_ERR_DEC_FAIL;
		}
		if(ret == AAC_OK) {
			break;
		}
	}
	ch = aac_dec_get_channel((char *)audio_ctrl.work_mem);
	channel = ch;
	g_audio_sample_rate = aac_dec_get_samplerate((char *)audio_ctrl.work_mem);	//20090209 Roy
	if (ch == 1) {
		dac_mono_set();
	}
	else {
		dac_stereo_set();
	}
	hSrc = audio_ctrl.work_mem;
	pfnGetOutput = audio_aac_get_output;	
	in_length=aac_dec_get_samplerate((char *)audio_ctrl.work_mem);
#if APP_VOICE_CHANGER_EN
	if(hVC && G_snd_info.VoiceChangerEn)
	{
		VoiceChanger_Link(hVC, hSrc, pfnGetOutput, in_length, ret, 0);
		VoiceChanger_SetParam(hVC, G_snd_info.Speed, G_snd_info.Pitch);
		hSrc = hVC;
		in_length = VoiceChanger_GetSampleRate(hVC);
		ret = VoiceChanger_GetChannel(hVC);
		pfnGetOutput = &VoiceChanger_GetOutput;
	}
#endif
	dac_sample_rate_set(in_length);
	DBG_PRINT("bps: %d\r\n",aac_dec_get_bitspersample((char*)audio_ctrl.work_mem));
	DBG_PRINT("channel: %d\r\n",ch);
	DBG_PRINT("sample rate: %d\r\n",aac_dec_get_samplerate((char*)audio_ctrl.work_mem));

	for (count=0;count<MAX_DAC_BUFFERS;count++) {
		OSQPost(audio_wq, (void *) count);
	}

	return AUDIO_ERR_NONE;

}

INT32S  audio_aac_process(void)
{
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	INT32S	pcm_point;
	INT32U  in_length;
	INT8U   err;
	INT32U  wb_idx;
	INT8U   ch;
	INT32S  t_wi;


	audio_ctrl.ri = aac_dec_get_read_index((char *)audio_ctrl.work_mem);
	t_wi = audio_write_with_file_srv(audio_ctrl.ring_buf, audio_ctrl.wi, audio_ctrl.ri);

	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);

	if (audio_check_wi(t_wi, &audio_ctrl.wi) != AUDIO_ERR_NONE) { /* check reading data */
		return AUDIO_ERR_READ_FAIL;
	}
	
	if(audio_ctrl.file_cnt >= audio_ctrl.file_len){
			if(audio_ctrl.f_last){
				return AUDIO_ERR_DEC_FINISH;
			}
			else{
				audio_ctrl.f_last = 1;
			}
	}

	in_length = audio_ctrl.ri;

	R_IOF_O_DATA |= 0x0200;	//test for aac decoder performance
	pcm_point = aac_dec_run((char *)audio_ctrl.work_mem,audio_ctrl.wi,pcm_out[wb_idx]);
	R_IOF_O_DATA &= ~0x0200;

	audio_ctrl.ri = aac_dec_get_read_index((char *)audio_ctrl.work_mem);
	audio_ctrl.file_cnt += (audio_ctrl.ri - in_length);

	if(in_length > audio_ctrl.ri) {
		audio_ctrl.file_cnt += AAC_DEC_BITSTREAM_BUFFER_SIZE;
	}

	if (pcm_point <= 0) {
			if (pcm_point == 0 && ++fg_error_cnt > MP3_FRAME_ERROR_CNT) {
				return AUDIO_ERR_DEC_FAIL;
			}
			if (pcm_point < 0) {
				if (--audio_ctrl.try_cnt == 0) {
					return AUDIO_ERR_DEC_FAIL;
				}
			}
			OSQPostFront(audio_wq, (void *)wb_idx);
			audio_send_next_frame_q();
			return 0;
		}
	else {
		fg_error_cnt = 0;
	}


	ch = aac_dec_get_channel((char *)audio_ctrl.work_mem);
	
	pcm_len[wb_idx] = pcm_point*ch;//*ch;
#else
	INT32S  pcm_point;
	INT8U   err;
	INT32U  wb_idx;
	wb_idx = (INT32U) OSQPend(audio_wq, 0, &err);
	pcm_point = pfnGetOutput(hSrc, pcm_out[wb_idx], MP3_DEC_FRAMESIZE);	//pcm_point*2
	if (pcm_point <= 0)
		return AUDIO_ERR_DEC_FINISH; 
	pcm_len[wb_idx] = pcm_point;

#endif
	OSQPost(aud_send_q, (void *) wb_idx);
	audio_send_to_dma();
	audio_send_next_frame_q();

	return 0;
}
#endif // #if APP_AAC_DECODE_FG_EN == 1




// MP3 parser for seek
/*========================================================================
 MP3 parser for seek
=========================================================================*/


#if APP_MP3_DECODE_FG_EN==1

#define		CODEC_ID_MP2				0x15000
#define		CODEC_ID_MP3				0x15001


#define ID3_V1_TAG_LEN


#define MAD_FLAG_LSF_EXT			0x1000	// lower sampling freq. extension
//#define MAD_FLAG_MC_EXT				0x2000	// multichannel audio extension
#define MAD_FLAG_MPEG_2_5_EXT		0x4000	// MPEG 2.5 (unofficial) extension


#define MAD_LAYER_I		1			// Layer I
//#define MAD_LAYER_II 	2			// Layer II/
#define MAD_LAYER_III	3			// Layer III/

#define streamRead read
#define streamSeek lseek


typedef INT64S INT64 ;

typedef struct
{
	INT16S  fin;

	int data_start;
	int data_size;
	int data_cur;

	int prev_time;
	int next_time;
	int dec_sample_num;

	unsigned int first_bitrate;
	unsigned int bitrate;
	unsigned int samplerate;
	unsigned int SamplePerFrame;
	unsigned int channels;
	unsigned int layer;
} MP3_PARSER;

MP3_PARSER g_mp3_parser;

static unsigned short const bitrate_table[5][15] = {
  /* MPEG-1 */
  { 0,  32,  64,  96, 128, 160, 192, 224,  /* Layer I   */
       256, 288, 320, 352, 384, 416, 448 },
  { 0,  32,  48,  56,  64,  80,  96, 112,  /* Layer II  */
       128, 160, 192, 224, 256, 320, 384 },
  { 0,  32,  40,  48,  56,  64,  80,  96,  /* Layer III */
       112, 128, 160, 192, 224, 256, 320 },

  /* MPEG-2 LSF */
  { 0,  32,  48,  56,  64,  80,  96, 112,  /* Layer I   */
       128, 144, 160, 176, 192, 224, 256 },
  { 0,   8,  16,  24,  32,  40,  48,  56,  /* Layers II & III */
        64,  80,  96, 112, 128, 144, 160 }
};


static unsigned int SamplePerFrame_table[3] = { 384, 1152, 576 };


static unsigned int const samplerate_table[3] = { 44100, 48000, 32000 };



static unsigned int udiv_32by32(unsigned int d, unsigned int n){
	unsigned q = 0, r= n, N = 32;
	do{
		N--;
		if((r>>N)>=d){
			r -= (d<<N);
			q += (1<<N);
		}
	}while(N);
	return q;
}

static unsigned int find_syncword(MP3_PARSER *Parser, int flag, int *frame_size)
{
	unsigned char byte0,byte1,byte2,byte3;
	int index;
	unsigned int ri;
//begin add by zgq on 20090220 for parsing	
	unsigned int pre_ri = 0;
	unsigned short pre_samplerate = 0;
	short pre_channel = 0;
	short pre_layer = 0;
	short mpeg25;
	short bitrate_index;
	short pad_slot;

	//struct mad_header *header;	
	unsigned char layer;
	int flags, resync = 0;
	unsigned int samplerate;
	unsigned short bitrate;
	

	int N = 0;
	short stereo;
	
	ri = lseek(Parser->fin,0,SEEK_CUR);

	while (1)
	{
		flags = 0;		//add by zgq on 20090402
		if (streamRead(Parser->fin,(INT32U)&byte0, sizeof(char)) != 1)
			break;
		if (streamRead(Parser->fin,(INT32U)&byte1, sizeof(char)) != 1)
			break;

		if (!((byte0 == 0xff) && ((byte1 & 0xe0) == 0xe0)))
		{
			ri++;
			streamSeek(Parser->fin, -1, SEEK_CUR);
			resync = 1;
			continue;
		}
		else
		{
			//mpegid
			if (streamRead(Parser->fin,(INT32U)&byte2, sizeof(char)) != 1)
				break;
			if (streamRead(Parser->fin,(INT32U)&byte3, sizeof(char)) != 1)
				break;

			
			if (byte1 & 0x10)
			{
				mpeg25 = 0;
				if((byte1 & 0x8) == 0)
					flags |= MAD_FLAG_LSF_EXT;
				
				flags &= ~MAD_FLAG_MPEG_2_5_EXT;
			}
			else
			{
				mpeg25 = 1;
				flags |= MAD_FLAG_LSF_EXT;
				flags |= MAD_FLAG_MPEG_2_5_EXT;
				if(byte1 & 0x08)
				{
					ri++;
					streamSeek(Parser->fin, -3, SEEK_CUR);
					resync = 1;
					continue;
				}
			}						

			
			//layer description
			index = (byte1 & 0x06) >> 1;
			if(index == 0x00)
			{
				ri++;
				streamSeek(Parser->fin, -3, SEEK_CUR);
				resync = 1;
				continue;
			}
			else
			{
				layer = 4 - index;
				Parser->layer = layer;
			}
			
			//bitrate
			index = (byte2 & 0xF0)>>4;
			if(index == 0x0F)
			{
				ri++;
				streamSeek(Parser->fin, -3, SEEK_CUR);
				resync = 1;
				continue;
			}
			else
			{
				bitrate_index = index;

				if (flags & MAD_FLAG_LSF_EXT)
					bitrate = bitrate_table[3 + (layer >> 1)][index];
				else
					bitrate = bitrate_table[layer - 1][index];

				Parser->bitrate = bitrate * 1000;//add by zgq on 20090303
				
				if (flag && (pre_samplerate == 0 || pre_channel == 0 || pre_layer == 0))
				{
					Parser->first_bitrate = Parser->bitrate;
					Parser->SamplePerFrame = SamplePerFrame_table[layer - 1];

					if ((!(flags & MAD_FLAG_LSF_EXT)) && (layer == 3))
						Parser->SamplePerFrame = 1152;
				}
			}
			
			//sample rate
			index = (byte2 & 0x0C)>>2;	
			if(index == 0x03)
			{
			    ri++;
				streamSeek(Parser->fin, -3, SEEK_CUR);
				resync = 1;
				continue;			    
			}
			else
			{
				samplerate =  samplerate_table[index];
			
				index = byte1 & 0x18;
				if(index == 0x00)						//MPEG2.5
					samplerate = samplerate >> 2;
				else if(index == 0x10)					//MPEG2
					samplerate = samplerate >> 1;

				Parser->samplerate = samplerate;
			}

			//channel mode
			index = (byte3 >> 6) & 0x03;
			stereo = (index == 0x03 ? 1 : 2);
			Parser->channels = stereo;
			//header->mode = 3 - index;
			
			// check next sample rate, layer, channel ...
			if(pre_samplerate == 0 || pre_channel == 0 || pre_layer == 0)
			{
				int ri_tmp2;	

				/* calculate beginning of next frame */
				pad_slot = (byte2 >> 1) & 0x01;

				if(bitrate_index != 0)
				{ 
					if (layer == MAD_LAYER_I)
					{			
						//N = ((12 * header->bitrate / header->samplerate) + pad_slot) * 4;
						unsigned int temp = 12 * Parser->bitrate;
						N = (udiv_32by32(samplerate, temp) + pad_slot) * 4;		
					}	
					else 
					{
						unsigned int slots_per_frame;
						unsigned int temp;

						slots_per_frame = ((layer == MAD_LAYER_III) &&
							   (flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;
				
						temp = slots_per_frame * Parser->bitrate;
						N = udiv_32by32(samplerate, temp) + pad_slot;	
					}
				}
				else
				{
					int header0 = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
					unsigned int newhead=0;
					int j = 0;		

					streamSeek(Parser->fin, ri, SEEK_SET);
					
					while(1)
					{

						unsigned char b;
						j += 1;
						if (streamRead(Parser->fin,(INT32U)&b, sizeof(char)) != 1)
							break;
						newhead <<= 8;
						newhead |= b;
						newhead &= 0xffffffff;

						if(((header0 & 0xfffefc00) == (newhead & 0xfffefc00)))
						{
							if(layer == 1)
								N = j - 4 - pad_slot *4; // header and padding not included
							else if((layer ==2) || (layer == 3))
								N = j - 4 - pad_slot;

							//mp3_dec_set_ri(mp3dec, ri);
							break;
						}

					} // while(!feof(Parser->fin));

					N = N + pad_slot;
				}

				if (N == 0)
				{
					ri++;
					streamSeek(Parser->fin, -3, SEEK_CUR);
					resync = 1;
					continue;
				}


				if (!flag && !resync)
				{
					*frame_size = N;
					streamSeek(Parser->fin, -4, SEEK_CUR);
					break;
				}
				
				// pass bitstream 
				ri_tmp2 = ri + N;	
				
				streamSeek(Parser->fin, N - 4, SEEK_CUR);			

				if (streamRead(Parser->fin,(INT32U)&byte0, sizeof(char)) != 1)
					break;
				if (streamRead(Parser->fin,(INT32U)&byte1, sizeof(char)) != 1)
					break;
				
				// check next header...
				if ((byte0 == 0xff) && ((byte1 & 0xe0) == 0xe0))
				{
					pre_ri = ri;
					pre_samplerate = samplerate;
					pre_channel = stereo;
					pre_layer = layer;
					ri = ri_tmp2;
				}
				else
				{
					ri++;
					resync = 1;
				}

				streamSeek(Parser->fin, ri, SEEK_SET);
				continue;
			}
			else
			{
				if(pre_samplerate != samplerate || pre_channel != stereo || pre_layer != layer)
				{
					pre_samplerate = 0;
					pre_channel = 0;
					pre_layer = 0;
					ri = pre_ri + 1;
					resync = 1;
					continue;
				}
				else
					ri = pre_ri;
			}
					
			break;			
		}		
	}

	if (!flag)
		*frame_size = N;

	return ri;
}


 int MP3Parser_Init(INT16S fin)
{
	MP3_PARSER *Parser = &g_mp3_parser;
	int ret = 0;
	
	char ape_tag[21];
	unsigned char id3_tag1, id3_tag2, id3_tag3;
	int pos;

	gp_memset((INT8S *)&g_mp3_parser,0,sizeof(MP3_PARSER));	
	Parser->fin = fin;


	streamSeek(Parser->fin, 0, SEEK_SET) ;

	Parser->prev_time = 0;
	Parser->next_time = 0;
	Parser->dec_sample_num = 0;
	
	// ========================= remove ID3v2 @ file start =========================
	
	streamRead(Parser->fin,(INT32U)&id3_tag1, sizeof(char));
	streamRead(Parser->fin,(INT32U)&id3_tag2, sizeof(char));
	streamRead(Parser->fin,(INT32U)&id3_tag3, sizeof(char));

	if ((id3_tag1=='I') && (id3_tag2=='D') && (id3_tag3=='3'))
	{ 
		unsigned int id3_length = 0;
		unsigned char x;

		streamSeek(Parser->fin, 3, SEEK_CUR);

		streamRead(Parser->fin,(INT32U)&x, sizeof(char));
		id3_length |= (x & 0x7F) << 21;

		streamRead(Parser->fin,(INT32U)&x, sizeof(char));
		id3_length |= (x & 0x7F) << 14;
		
		streamRead(Parser->fin,(INT32U)&x, sizeof(char));
		id3_length |= (x & 0x7F) << 7;
		
		streamRead(Parser->fin,(INT32U)&x, sizeof(char));
		id3_length |= (x & 0x7F);

		id3_length += 0x0A;

		streamSeek(Parser->fin, id3_length - 10, SEEK_CUR);
	}
	else
		streamSeek(Parser->fin, 0, SEEK_SET);

	
	// ========================= get bitrate =========================
	Parser->data_start = find_syncword(Parser, 1, NULL);
	
	// ========================= remove APETAG @ file end =========================
	streamSeek(Parser->fin, -500, SEEK_END);

	while (1)
	{
		pos = streamRead(Parser->fin,(INT32U)ape_tag, 20);

		if (pos != 20)
			break;
		else
		{
			pos = strcspn(ape_tag, "APETAGEX");

			if (strncmp(ape_tag + pos, "APETAGEX", 8))
				streamSeek(Parser->fin, -7, SEEK_CUR);
			else
			{
				streamSeek(Parser->fin, -(20-pos) + 12, SEEK_CUR);
				pos = 0xFF;
				break;
			}
		}
	}

	if (pos == 0xFF)	// APETAGEX exists
		Parser->data_size = lseek(Parser->fin,0,SEEK_CUR) - 12 - Parser->data_start;
	else	// ========================= remove ID3v1 @ file end =========================
	{
		streamSeek(Parser->fin, -128, SEEK_END);

		streamRead(Parser->fin,(INT32U)&id3_tag1, sizeof(char));
		streamRead(Parser->fin,(INT32U)&id3_tag2, sizeof(char));
		streamRead(Parser->fin,(INT32U)&id3_tag3, sizeof(char));

		if ((id3_tag1=='T') && (id3_tag2=='A') && (id3_tag3=='G'))
			Parser->data_size = lseek(Parser->fin,0,SEEK_CUR) - 3 - Parser->data_start;
		else
			Parser->data_size = lseek(Parser->fin,0,SEEK_CUR)+ 125 - Parser->data_start;
	}

	streamSeek(Parser->fin, 0, SEEK_SET); // set position to 0.
	Parser->data_cur = 0;



	return ret;
}



#define VBR_SUPPORTED

 int MP3Parser_Seek( INT32U *p_msec,INT32U vbr)
{
	MP3_PARSER *Parser = &g_mp3_parser;
	long msec = *p_msec;
	int ret;

#ifdef VBR_SUPPORTED
	
	int frame = 0, frame_size;
	int target_frame;
	
	DBG_PRINT("MP3Parser_seek entry \r\n");
	target_frame = (long)(((INT64)msec * Parser->samplerate) / (Parser->SamplePerFrame * 1000));
	
	streamSeek(Parser->fin, Parser->data_start, SEEK_SET);
	if(vbr==0)
	{
		find_syncword(Parser, 0, &frame_size);
		streamSeek(Parser->fin,frame_size*target_frame,SEEK_CUR);
	}
	else
	{
		Parser->data_cur = 0;
		while ((frame < target_frame) && (Parser->data_cur < Parser->data_size))
		{
			//frame_size = get_frame_size(Parser);
			find_syncword(Parser, 0, &frame_size);
			streamSeek(Parser->fin, frame_size, SEEK_CUR);
			Parser->data_cur += frame_size;
			frame++;
		}

		msec = (long)(((INT64)frame * Parser->SamplePerFrame * 1000) / Parser->samplerate);
		DBG_PRINT("MP3Parser_seek exit \r\n");
	}
#else

	long t32;
	unsigned int ri;
	
	
	t32 = (long)(((INT64)msec * Parser->first_bitrate) / (8 * 1000));
	
	if (t32 >= Parser->data_size)
	{
		t32 = Parser->data_start + Parser->data_size;
		streamSeek(Parser->fin, t32, SEEK_SET);
		Parser->data_cur = Parser->data_size;
	}
	else
	{
		// ===== find syncword =====
		t32 += Parser->data_start;
		streamSeek(Parser->fin, t32, SEEK_SET);
		ri = find_syncword(Parser, 0);
		
		
		if(streamSeek(Parser->fin, ri, SEEK_SET)!=0) ERROR("File seek failed");
		Parser->data_cur = ri - Parser->data_start;
	}
	
	msec = (long)(((INT64)Parser->data_cur * 8 * 1000) / Parser->first_bitrate);

#endif

	*p_msec = msec;

	Parser->prev_time = Parser->next_time = msec;
	Parser->dec_sample_num = 0;

	ret = 0;
	return ret;
}

int MP3Parser_total_time()
{
	MP3_PARSER *Parser = &g_mp3_parser;
	return (int)((INT64U)Parser->data_size * 8 * 1000 / Parser->bitrate);
}

#endif






