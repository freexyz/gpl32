#include "task_video_decoder.h"

/* global varaible */
static vid_dec_buf_struct g_audio;
static vid_dec_buf_struct  g_video;
static vid_dec_buf_struct  g_deblock;
static vid_dec_buf_struct  g_scaler;
static volatile INT32U g_lock_vid_addr;
static volatile INT32U g_jpeg_y_addr;
static volatile INT32U g_jpeg_cb_addr;
static volatile INT32U g_jpeg_cr_addr;
static DMA_STRUCT g_aud_dec_dma_dbf;

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
static INT32U mjpeg_output_h_cnt, mjpeg_scale_h_size;
static FP32   mjpeg_scale_h_fifo, mjpeg_CorrectScaleHSize;
#endif

// timer and sync
void vid_dec_timer_isr(void)
{
	if(p_vid_dec_para->audio_flag) {
		if(p_vid_dec_para->tv - p_vid_dec_para->ta < p_vid_dec_para->time_range) {
			p_vid_dec_para->tv += p_vid_dec_para->tick_time2;
		}
		
		if(p_vid_dec_para->ta - p_vid_dec_para->tv > p_vid_dec_para->time_range) {
			// Make video faster if ta is greater tv more than 32ms
			// This branch will only occur when ICE stop on debug point 
			p_vid_dec_para->tv += p_vid_dec_para->tick_time2;
		}
	} else {
		p_vid_dec_para->tv += p_vid_dec_para->tick_time2;
	}
	
	if(p_vid_dec_para->tv - p_vid_dec_para->Tv >= 0) {
		if(p_vid_dec_para->post_cnt == p_vid_dec_para->pend_cnt) {
			if(OSQPost(vid_dec_q, (void*)MSG_VID_DEC_ONE_FRAME) == OS_NO_ERR) {
				p_vid_dec_para->post_cnt++;
			}
		}
	}
}

void vid_dec_stop_timer(void)
{
	timer_stop(VIDEO_DECODE_TIMER);
}

void vid_dec_start_timer(void)
{
	INT32U temp, freq_hz;
	
	
	if(p_vid_dec_para->audio_flag) {
		//temp = 0x10000 -((0x10000 - (R_TIMERE_PRELOAD & 0xFFFF)) * p_vid_dec_para->n);
		temp = (0x10000 - (R_TIMERE_PRELOAD & 0xFFFF)) * p_vid_dec_para->n;
		freq_hz = MCLK/2/temp;
		if(MCLK%(2*temp)) {
			freq_hz++;
		}
	} else {
		freq_hz = TIME_BASE_TICK_RATE;
	}
	timer_freq_setup(VIDEO_DECODE_TIMER, freq_hz, 0, vid_dec_timer_isr);
}

//memory
static void vid_dec_buffer_number_init(void)
{
	g_lock_vid_addr = 0xFFFFFFFF;
	gp_memset((INT8S*)&g_audio, 0x00, sizeof(vid_dec_buf_struct));
	gp_memset((INT8S*)&g_video, 0x00, sizeof(vid_dec_buf_struct));
	gp_memset((INT8S*)&g_deblock, 0x00, sizeof(vid_dec_buf_struct));
	gp_memset((INT8S*)&g_scaler, 0x00, sizeof(vid_dec_buf_struct));
		
	//audio
	g_audio.total_number = AUDIO_FRAME_NO; 	
	
	//video
	if(p_vid_dec_para->video_format == C_MJPG_FORMAT) {
		//mjpg
		g_video.total_number = VIDEO_FRAME_NO;
	} else {
		//mpeg4
		if(p_vid_dec_para->scaler_flag && p_vid_dec_para->deblock_flag) {
			g_video.total_number = 2;
			g_deblock.total_number = 2;
			g_scaler.total_number = SCALER_FRAME_NO;
		} else if(p_vid_dec_para->scaler_flag) {
			g_video.total_number = 2;
			g_deblock.total_number = 0;
			g_scaler.total_number = SCALER_FRAME_NO;
		} else if(p_vid_dec_para->deblock_flag) {
			g_video.total_number = 2;
			g_deblock.total_number = DEBLOCK_FRAME_NO;
			g_scaler.total_number = 0;
		} else {
			g_video.total_number = VIDEO_FRAME_NO;
			g_deblock.total_number = 0;
			g_scaler.total_number = 0;
		}	
	}
}

static INT32S vid_dec_audio_memory_alloc(void)
{
	INT32S i, nRet;
	
	if(p_vid_dec_para->audio_flag == 0) {
		RETURN(0);
	}
		
	for(i=0; i<g_audio.total_number; i++) {
		if(p_vid_dec_para->audio_decode_addr[i] == 0) {
			p_vid_dec_para->audio_decode_addr[i] = (INT32U)gp_malloc_align(p_vid_dec_para->aud_frame_size, 4);
			if(p_vid_dec_para->audio_decode_addr[i] == 0) {
				RETURN(-1);
			}
			gp_memset((INT8S*)p_vid_dec_para->audio_decode_addr[i], 0x80, p_vid_dec_para->aud_frame_size);	
		}
	}
	
	nRet = STATUS_OK;
Return:
	for(i=0; i<g_audio.total_number; i++) {
		DEBUG_MSG(DBG_PRINT("AudioFrame = 0x%x\r\n", p_vid_dec_para->audio_decode_addr[i]));
	}	
	return nRet;
}

static void vid_dec_audio_memory_free(void)
{
	INT32S i;
	
	for(i=0; i<g_audio.total_number; i++) {
		if(p_vid_dec_para->audio_decode_addr[i]) {
			gp_free((void*)p_vid_dec_para->audio_decode_addr[i]);
			p_vid_dec_para->audio_decode_addr[i] = 0;
		}
	}
}

static INT32S vid_dec_video_memory_alloc(void)
{
	INT16U *pdata;
	INT32S i, j, temp, nRet;
	
	if(p_vid_dec_para->video_flag == 0) {
		RETURN(0);
	}
	
	if(p_vid_dec_para->video_format == C_MJPG_FORMAT) {
		//mjpeg video
		if(p_vid_dec_para->scaler_flag) {	
			temp = p_vid_dec_para->buffer_output_width * p_vid_dec_para->buffer_output_height * 2;
		} else {
			//height must be 16-align
			if(p_bitmap_info->biHeight % 16) {
				pdata = (INT16U*)& p_bitmap_info->biHeight;
				*pdata = (*pdata + 16) & 0xFFF0;
			}
			temp = p_bitmap_info->biWidth * p_bitmap_info->biHeight * 2;
		}	
			
		for(i=0; i<g_video.total_number; i++) {
			if(p_vid_dec_para->video_decode_addr[i] == 0) {
				p_vid_dec_para->video_decode_addr[i] = (INT32U)gp_malloc_align(temp, 32);
				if(p_vid_dec_para->video_decode_addr[i] == 0) {
					RETURN(-1);
				}
				pdata = (INT16U*)p_vid_dec_para->video_decode_addr[i];
				for(j=0; j<(temp>>1); j++)
					*pdata++ = 0x0080; 
			}
		}
	} else {
		//mpeg4 video
		temp = p_bitmap_info->biWidth * p_bitmap_info->biHeight * 2;
		for(i=0; i<g_video.total_number; i++) {
			if(p_vid_dec_para->video_decode_addr[i] == 0) {
				p_vid_dec_para->video_decode_addr[i] = (INT32U)gp_malloc_align(temp, 32);
				if(p_vid_dec_para->video_decode_addr[i] == 0) {
					RETURN(-1);
				}
				pdata = (INT16U*)p_vid_dec_para->video_decode_addr[i];
				for(j=0; j<(temp>>1); j++) {
					*pdata++ = 0x0080; 
				}
			}
		}
		
		//deblock
		temp = p_bitmap_info->biWidth * p_bitmap_info->biHeight * 2;
		for(i=0; i<g_deblock.total_number; i++) {
			if(p_vid_dec_para->deblock_addr[i] == 0) {
				p_vid_dec_para->deblock_addr[i] = (INT32U)gp_malloc_align(temp, 32);
				if(p_vid_dec_para->deblock_addr[i] == 0) {
					RETURN(-1);
				}
				pdata = (INT16U*)p_vid_dec_para->deblock_addr[i];
				for(j=0; j<(temp>>1); j++) {
					*pdata++ = 0x0080; 
				}
			}
		}
		
		//scaler	
		temp = p_vid_dec_para->buffer_output_width * p_vid_dec_para->buffer_output_height * 2;
		for(i=0; i<g_scaler.total_number; i++) {
			if(p_vid_dec_para->scaler_output_addr[i] == 0) {
				p_vid_dec_para->scaler_output_addr[i] = (INT32U)gp_malloc_align(temp, 32);
				if(p_vid_dec_para->scaler_output_addr[i] == 0) {
					RETURN(-1);
				}
				pdata = (INT16U*)p_vid_dec_para->scaler_output_addr[i];
				for(j=0; j<(temp>>1); j++) {
					*pdata++ = 0x0080; 	
				}
			}
		}		
	}

	nRet = STATUS_OK;
Return:
	for(i=0; i<g_video.total_number; i++) {
		DEBUG_MSG(DBG_PRINT("VideoFrame = 0x%x\r\n", p_vid_dec_para->video_decode_addr[i]));
	}
	for(i=0; i<g_deblock.total_number; i++) {
		DEBUG_MSG(DBG_PRINT("DeblockFrame = 0x%x\r\n", p_vid_dec_para->deblock_addr[i]));
	}
	for(i=0; i<g_scaler.total_number; i++) {
		DEBUG_MSG(DBG_PRINT("ScalerFrame = 0x%x\r\n", p_vid_dec_para->scaler_output_addr[i]));
	}
	return nRet;
}

static void vid_dec_video_memory_free(void)
{
	INT8U  video_n, scaler_n, deblock_n;
	INT32U i;
		
	if(p_vid_dec_para->video_format == C_MJPG_FORMAT) {
		if(p_vid_dec_para->user_define_flag) {
			video_n = 2; 
			scaler_n = deblock_n = 0;
		} else {
			video_n = scaler_n = deblock_n = 0;
		}
	} else {
		if(p_vid_dec_para->user_define_flag) {
			if(p_vid_dec_para->deblock_flag && p_vid_dec_para->scaler_flag) {
				scaler_n = 2;
				video_n = deblock_n = 0;
			} else if(p_vid_dec_para->deblock_flag) {
				deblock_n = 2;
				video_n = scaler_n = 0;
			} else if(p_vid_dec_para->scaler_flag) {
				scaler_n = 2;
				video_n = deblock_n = 0;
			} else {
				//no deblock and scaler
				video_n = 2;
				scaler_n = deblock_n = 0;
			}
		} else {
			video_n = scaler_n = deblock_n = 0;
		}
	}
		
	for(i=video_n; i<g_video.total_number; i++) {
		if(p_vid_dec_para->video_decode_addr[i]) {
			gp_free((void*)p_vid_dec_para->video_decode_addr[i]);
			p_vid_dec_para->video_decode_addr[i] = 0;
		}
	}
	
	for(i=deblock_n; i<g_deblock.total_number; i++) {
		if(p_vid_dec_para->deblock_addr[i]) {
			gp_free((void*)p_vid_dec_para->deblock_addr[i]);
			p_vid_dec_para->deblock_addr[i] = 0;
		}
	}
	
	for(i=scaler_n; i<g_scaler.total_number; i++) {
		if(p_vid_dec_para->scaler_output_addr[i]) {
			gp_free((void*)p_vid_dec_para->scaler_output_addr[i]);
			p_vid_dec_para->scaler_output_addr[i] = 0;
		}
	}
}

INT32S vid_dec_memory_alloc(void)
{
	//init index
	vid_dec_buffer_number_init();
	if(vid_dec_audio_memory_alloc() < 0) {
		return -1;
	}	
	
	if(vid_dec_video_memory_alloc() < 0) {
		return -1;
	}	
	return 0;
}

void vid_dec_memory_free(void)
{
	vid_dec_audio_memory_free();
	vid_dec_video_memory_free();
}

INT32S vid_dec_memory_realloc(void)
{	
	INT32U addr[2];
	
	//free memory
	vid_dec_video_memory_free();
	
	//set scaler flag
	vid_dec_set_scaler(p_vid_dec_para->user_scaler_flag, 
					p_vid_dec_para->video_decode_out_format,
					p_vid_dec_para->image_output_width,
					p_vid_dec_para->image_output_height, 
					p_vid_dec_para->buffer_output_width, 
					p_vid_dec_para->buffer_output_height);
	
	//user define buffer				
	if(p_vid_dec_para->user_define_flag) {
		if(p_vid_dec_para->deblock_addr[0] && p_vid_dec_para->deblock_addr[1]) {
			addr[0] = p_vid_dec_para->deblock_addr[0];
			addr[1] = p_vid_dec_para->deblock_addr[1];
			p_vid_dec_para->deblock_addr[0] = 0;
			p_vid_dec_para->deblock_addr[1] = 0;
		} else if(p_vid_dec_para->scaler_output_addr[0] && p_vid_dec_para->scaler_output_addr[1]) {
			addr[0] = p_vid_dec_para->scaler_output_addr[0];
			addr[1] = p_vid_dec_para->scaler_output_addr[1];
			p_vid_dec_para->scaler_output_addr[0] = 0;
			p_vid_dec_para->scaler_output_addr[1] = 0;
		} else {
			addr[0] = p_vid_dec_para->video_decode_addr[0];
			addr[1] = p_vid_dec_para->video_decode_addr[1];
			p_vid_dec_para->video_decode_addr[0] = 0;
			p_vid_dec_para->video_decode_addr[1] = 0;
		}	
		vid_dec_set_user_define_buffer(p_vid_dec_para->user_define_flag, addr[0], addr[1]);	
	}
	
	//init index
	vid_dec_buffer_number_init();
	
	//alloc memory
	if(vid_dec_video_memory_alloc() < 0) {
		return -1;
	}	
	return 0;
}

//buffer
void vid_dec_buffer_lock(INT32U addr)
{
	g_lock_vid_addr = addr;
}

void vid_dec_buffer_unlock(void)
{
	g_lock_vid_addr = 0xFFFFFFFF;
}

INT32U vid_dec_get_next_vid_buffer(void)
{
	INT32U addr;
	
	do{
		addr = p_vid_dec_para->video_decode_addr[g_video.current_index++];
		if(g_video.current_index >= g_video.total_number)		
			g_video.current_index = 0;
	}while(addr == g_lock_vid_addr);
	return addr;
}

INT32U vid_dec_get_next_deblock_buffer(void)
{
	INT32U addr;
	
	do{
		addr = p_vid_dec_para->deblock_addr[g_deblock.current_index++];
		if(g_deblock.current_index >= g_deblock.total_number)	
			g_deblock.current_index = 0;
	}while(addr == g_lock_vid_addr);
	return addr;
}

INT32U vid_dec_get_next_aud_buffer(void)
{
	INT32U addr;
	
	do{
		addr = p_vid_dec_para->audio_decode_addr[g_audio.current_index++];
		if(g_audio.current_index >= g_audio.total_number)		
			g_audio.current_index = 0;	
	}while(addr == g_lock_vid_addr);
	return addr;
}

INT32U vid_dec_get_next_scaler_buffer(void)
{
	INT32U addr;
	
	do{	
		addr = p_vid_dec_para->scaler_output_addr[g_scaler.current_index++];
		if(g_scaler.current_index >= g_scaler.total_number)	
			g_scaler.current_index = 0;
	}while(addr == g_lock_vid_addr);
	return addr;
}

//status
void vid_dec_set_status(INT32S flag)
{
	p_vid_dec_para->status |= flag;
}

void vid_dec_clear_status(INT32S flag)
{
	p_vid_dec_para->status &= ~flag;
}

INT32S vid_dec_get_status(void)
{
	return p_vid_dec_para->status;
}

//video info
void vid_dec_set_video_flag(INT8S video_flag)
{
	if(video_flag)
		p_vid_dec_para->video_flag = TRUE;
	else
		p_vid_dec_para->video_flag = FALSE;
}

INT8S vid_dec_get_video_flag(void)
{
	return p_vid_dec_para->video_flag;
}

INT32S vid_dec_get_video_format(INT32U biCompression)
{
	INT8U data[4];
	
	data[0] = (biCompression >> 0) & 0xFF; //X
	data[1] = (biCompression >> 8) & 0xFF; //X
	data[2] = (biCompression >> 16) & 0xFF; //X
	data[3] = (biCompression >> 24) & 0xFF; //X
	
	if( (data[0] == 'X' || data[0] == 'x') &&
		(data[1] == 'V' || data[1] == 'v') &&
		(data[2] == 'I' || data[2] == 'i') &&
		(data[3] == 'D' || data[3] == 'd')) {
		DEBUG_MSG(DBG_PRINT("VidFormat = C_XVID_FORMAT\r\n"));
		return C_XVID_FORMAT;
	} else if((data[0] == 'M' || data[0] == 'm') &&
			(data[1] == '4') &&
			(data[2] == 'S' || data[2] == 's') &&
			(data[3] == '2')) {
		DEBUG_MSG(DBG_PRINT("VidFormat = C_M4S2_FORMAT\r\n"));
		return C_M4S2_FORMAT;
	} else if((data[0] == 'F' || data[0] == 'f') &&
			(data[1] == 'M' || data[1] == 'm') &&
			(data[2] == 'P' || data[2] == 'p') &&
			(data[3] == '4')) {	
		DEBUG_MSG(DBG_PRINT("VidFormat = C_FMP4_FORMAT\r\n"));
		return C_M4S2_FORMAT;
	} else if((data[0] == 'H' || data[0] == 'h') &&
			(data[1] == '2') &&
			(data[2] == '6') &&
			(data[3] == '3')) {
		DEBUG_MSG(DBG_PRINT("VidFormat = C_H263_FORMAT\r\n"));
		return C_H263_FORMAT;
	} else if((data[0] == 'M' || data[0] == 'm') &&
			(data[1] == 'J' || data[1] == 'j') &&
			(data[2] == 'P' || data[2] == 'p') &&
			(data[3] == 'G' || data[3] == 'g')) {
		DEBUG_MSG(DBG_PRINT("VidFormat = C_MJPG_FORMAT\r\n"));
		return C_MJPG_FORMAT;
	}
	
	DEBUG_MSG(DBG_PRINT("NotSupportVidFormat = 0x%x\r\n", biCompression)); 
	return STATUS_FAIL;	 
}

INT32S vid_dec_get_file_format(INT8S *pdata)
{
	if( (*(pdata+0) == 'A' || *(pdata+0) == 'a') &&
		(*(pdata+1) == 'V' || *(pdata+1) == 'v') &&
		(*(pdata+2) == 'I' || *(pdata+2) == 'i'))
	{
		return FILE_TYPE_AVI;
	}
	else if((*(pdata+0) == 'M' || *(pdata+0) == 'm') &&
			(*(pdata+1) == 'O' || *(pdata+1) == 'o') &&
			(*(pdata+2) == 'V' || *(pdata+2) == 'v'))
	{
		return FILE_TYPE_MOV;
	}
	else if((*(pdata+0) == 'M' || *(pdata+0) == 'm') &&
			(*(pdata+1) == 'P' || *(pdata+1) == 'p') &&
			(*(pdata+2) == '4' || *(pdata+2) == '4'))
	{
		return FILE_TYPE_MOV;
	}
	else if((*(pdata+0) == '3' || *(pdata+0) == '3') &&
			(*(pdata+1) == 'G' || *(pdata+1) == 'g') &&
			(*(pdata+2) == 'P' || *(pdata+2) == 'p'))
	{
		return FILE_TYPE_MOV;
	}
	else if((*(pdata+0) == 'M' || *(pdata+0) == 'm') &&
			(*(pdata+1) == '4' || *(pdata+1) == '4') &&
			(*(pdata+2) == 'A' || *(pdata+2) == 'a'))
	{
		return FILE_TYPE_MOV;
	}
	 
	return STATUS_FAIL;	 
}

void vid_dec_get_size(INT16U *width, INT16U *height)
{
	*width = p_bitmap_info->biWidth;
	*height = p_bitmap_info->biHeight;
}

//deblock
void vid_dec_set_deblock_flag(INT8S deblock_flag)
{
	if(deblock_flag)
		p_vid_dec_para->deblock_flag = TRUE;
	else
		p_vid_dec_para->deblock_flag = FALSE;	
}

INT8S vid_dec_get_deblock_flag(void)
{
	return p_vid_dec_para->deblock_flag;
}

//audio info
void vid_dec_set_audio_flag(INT8S audio_flag)
{
	if(audio_flag)
		p_vid_dec_para->audio_flag = TRUE;
	else
		p_vid_dec_para->audio_flag = FALSE;
}

INT8S vid_dec_get_audio_flag(void)
{
	return p_vid_dec_para->audio_flag;
}

//audio decoder
INT32S vid_dec_set_aud_dec_work_mem(INT32U work_mem_size)
{
	p_vid_dec_para->work_mem_size = work_mem_size;
	p_vid_dec_para->work_mem_addr = (INT8U *)gp_malloc_align(work_mem_size, 4);
	if(!p_vid_dec_para->work_mem_addr)
		return -1;
	
	gp_memset((INT8S*)p_vid_dec_para->work_mem_addr, 0, work_mem_size);
	return 0;
}

INT32S vid_dec_set_aud_dec_ring_buffer(void) 
{
	p_vid_dec_para->ring_buffer_size = MultiMediaParser_GetAudRingLen(p_vid_dec_para->media_handle);
	p_vid_dec_para->ring_buffer_addr = (INT8U *)MultiMediaParser_GetAudRing(p_vid_dec_para->media_handle);
	if(!p_vid_dec_para->ring_buffer_addr)
		return -1;
		
	return 0;
}

void vid_dec_aud_dec_memory_free(void)
{
	if(p_vid_dec_para->work_mem_addr)
	{
		gp_free((void*)p_vid_dec_para->work_mem_addr);
	}
	p_vid_dec_para->work_mem_size = 0;
	p_vid_dec_para->work_mem_addr = 0;
	p_vid_dec_para->ring_buffer_size = 0;
	p_vid_dec_para->ring_buffer_addr = 0;
}

//audio dac
INT32S aud_dec_double_buffer_put(INT16U *pdata,INT32U cwlen, OS_EVENT *os_q)
{
	INT32S status;
	
	g_aud_dec_dma_dbf.s_addr = (INT32U) pdata;
	g_aud_dec_dma_dbf.t_addr = (INT32U) P_DAC_CHA_DATA;
	g_aud_dec_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
	g_aud_dec_dma_dbf.count = (INT32U) cwlen;
	g_aud_dec_dma_dbf.notify = NULL;
	g_aud_dec_dma_dbf.timeout = 0;
	
	status = dma_transfer_with_double_buf(&g_aud_dec_dma_dbf, os_q);
	if (status != 0)
		return status;
		
	return STATUS_OK;
}

INT32U aud_dec_double_buffer_set(INT16U *pdata, INT32U cwlen)
{
	INT32S status;
	
	g_aud_dec_dma_dbf.s_addr = (INT32U) pdata;
	g_aud_dec_dma_dbf.t_addr = (INT32U) P_DAC_CHA_DATA;
	g_aud_dec_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
	g_aud_dec_dma_dbf.count = (INT32U) cwlen;
	g_aud_dec_dma_dbf.notify = NULL;
	g_aud_dec_dma_dbf.timeout = 0;
	status = dma_transfer_double_buf_set(&g_aud_dec_dma_dbf);
	if(status != 0)
		return status;

	return STATUS_OK;
}

INT32S aud_dec_dma_status_get(void)
{
	if (g_aud_dec_dma_dbf.channel == 0xff) 
		return -1;
	
	return dma_status_get(g_aud_dec_dma_dbf.channel);	
}

void aud_dec_double_buffer_free(void)
{
	dma_transfer_double_buf_free(&g_aud_dec_dma_dbf);
	g_aud_dec_dma_dbf.channel = 0xff;
}

//dac init
void aud_dec_dac_start(INT8U channel_no, INT32U sample_rate)
{
	if(p_wave_info->wFormatTag == WAVE_FORMAT_MPEGLAYER3) //2 channel
		dac_stereo_set();
	else if(channel_no == 1)
		dac_mono_set();
	else
		dac_stereo_set();
		
	dac_sample_rate_set(sample_rate);	
}

void aud_dec_dac_stop(void)
{
	dac_timer_stop();	
}

void aud_dec_ramp_down_handle(INT8U channel_no)
{
	INT16S  last_ldata,last_rdata;
	INT16S  i, temp;
	
	temp = 0 - DAC_RAMP_DOWN_STEP;
	last_ldata = R_DAC_CHA_DATA;
	last_rdata = R_DAC_CHB_DATA;
	//unsigned to signed 
	last_ldata ^= 0x8000;
	last_rdata ^= 0x8000;
	
	//change timer to 44100
	dac_sample_rate_set(44100);
	while(1)
	{
		if (last_ldata > 0x0) 
		{
			last_ldata -= DAC_RAMP_DOWN_STEP;
		}
		else if (last_ldata < 0x0) 
		{
			last_ldata += DAC_RAMP_DOWN_STEP;
		}
			
		if ((last_ldata < DAC_RAMP_DOWN_STEP)&&(last_ldata > temp)) 
		{ 
			last_ldata = 0;
		}

		if (channel_no == 2) 
		{
			if (last_rdata > 0x0) 
			{
				last_rdata -= DAC_RAMP_DOWN_STEP;
		    }
			else if (last_rdata < 0x0) 
			{
				last_rdata += DAC_RAMP_DOWN_STEP;
		    }
		        
		    if ((last_rdata < DAC_RAMP_DOWN_STEP)&&(last_rdata > temp)) 
		    {  
				last_rdata = 0;
			}
		}
		    
		for(i=0;i<DAC_RAMP_DOWN_STEP_HOLD;i++) 
		{
			if (channel_no == 2)
			{
				while(R_DAC_CHA_FIFO & 0x8000);
				R_DAC_CHA_DATA = last_ldata;
				while(R_DAC_CHB_FIFO & 0x8000);
				R_DAC_CHB_DATA = last_rdata;
			} 
			else 
			{
				while(R_DAC_CHA_FIFO & 0x8000);
				R_DAC_CHA_DATA = last_ldata;
			}
		}
		
		if (channel_no == 2) 
		{
			if ((last_ldata == 0x0) && (last_rdata == 0x0)) 
				break;
		}
		else
		{
			if (last_ldata == 0x0)
				break;
		}
	}
	dac_timer_stop();
}

//scaler
INT32S vid_dec_set_scaler(INT32U scaler_flag, INT32U video_output_format, 
						INT16U image_output_width, INT16U image_output_height, 
						INT16U buffer_output_width, INT16U buffer_output_height)
{
	INT8U  enable;
	INT32S nRet;
	
	enable = 0;
	if(p_bitmap_info->biWidth != buffer_output_width || p_bitmap_info->biHeight!= buffer_output_height ||
		image_output_width != buffer_output_width || image_output_height!= buffer_output_height)
	{
		enable = 1;
	} 
	
	if(p_vid_dec_para->video_format != C_MJPG_FORMAT &&
		(video_output_format != C_SCALER_CTRL_OUT_YUYV && video_output_format != C_SCALER_CTRL_OUT_UYVY))
	{
		enable = 1;
	}
	
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	if((p_vid_dec_para->video_format == C_MJPG_FORMAT) && (video_output_format != C_SCALER_CTRL_OUT_YUYV))
	{
		enable = 1;
	}
#endif
	
	if(enable && scaler_flag)
	{
		p_vid_dec_para->scaler_flag = scaler_flag;
		p_vid_dec_para->image_output_width = image_output_width;
		p_vid_dec_para->image_output_height = image_output_height;
		p_vid_dec_para->buffer_output_width = buffer_output_width; 
		p_vid_dec_para->buffer_output_height = buffer_output_height;
	}
	else if(enable && (scaler_flag == C_NO_SCALER))
	{
		p_vid_dec_para->scaler_flag = C_SCALER_FULL_SCREEN;
		p_vid_dec_para->image_output_width = p_bitmap_info->biWidth;
		p_vid_dec_para->image_output_height = p_bitmap_info->biHeight;
		p_vid_dec_para->buffer_output_width = p_bitmap_info->biWidth; 
		p_vid_dec_para->buffer_output_height = p_bitmap_info->biHeight;
	}
	else
	{
		p_vid_dec_para->scaler_flag = C_NO_SCALER;
		p_vid_dec_para->image_output_width = p_bitmap_info->biWidth;
		p_vid_dec_para->image_output_height = p_bitmap_info->biHeight;
		p_vid_dec_para->buffer_output_width = p_bitmap_info->biWidth; 
		p_vid_dec_para->buffer_output_height = p_bitmap_info->biHeight;
	}
		
	if(p_vid_dec_para->video_format == C_MJPG_FORMAT)
	{
		switch(video_output_format)
		{
		case C_SCALER_CTRL_OUT_RGB1555:
		case C_SCALER_CTRL_OUT_RGB565:		
		case C_SCALER_CTRL_OUT_RGBG:			
		case C_SCALER_CTRL_OUT_GRGB:			
		case C_SCALER_CTRL_OUT_YUYV:			
		case C_SCALER_CTRL_OUT_UYVY:			
		case C_SCALER_CTRL_OUT_YUYV8X32:		
		case C_SCALER_CTRL_OUT_YUYV8X64:		
		case C_SCALER_CTRL_OUT_YUYV16X32:		
		case C_SCALER_CTRL_OUT_YUYV16X64:		
		case C_SCALER_CTRL_OUT_YUYV32X32:		
		case C_SCALER_CTRL_OUT_YUYV64X64:		
			p_vid_dec_para->video_decode_out_format = video_output_format;
			break;	
		default:
			DEBUG_MSG(DBG_PRINT("OutputFormatNoSupport!!!\r\n"));
			RETURN(STATUS_FAIL);
		}
	}	
	else //mpeg4
	{
		switch(video_output_format)
		{
		case C_SCALER_CTRL_OUT_RGB1555:
		case C_SCALER_CTRL_OUT_RGB565:		
		case C_SCALER_CTRL_OUT_RGBG:			
		case C_SCALER_CTRL_OUT_GRGB:
		case C_SCALER_CTRL_OUT_YUYV8X32:		
		case C_SCALER_CTRL_OUT_YUYV8X64:		
		case C_SCALER_CTRL_OUT_YUYV16X32:		
		case C_SCALER_CTRL_OUT_YUYV16X64:		
		case C_SCALER_CTRL_OUT_YUYV32X32:		
		case C_SCALER_CTRL_OUT_YUYV64X64:		
			p_vid_dec_para->mpeg4_decode_out_format = C_MP4_OUTPUT_Y1UY0V;
			p_vid_dec_para->video_decode_in_format = C_SCALER_CTRL_IN_YUYV;
			p_vid_dec_para->video_decode_out_format = video_output_format;
			break;			
		case C_SCALER_CTRL_OUT_YUYV:
			p_vid_dec_para->mpeg4_decode_out_format = C_MP4_OUTPUT_Y1UY0V;
			p_vid_dec_para->video_decode_in_format = C_SCALER_CTRL_IN_YUYV;
			p_vid_dec_para->video_decode_out_format = C_SCALER_CTRL_OUT_YUYV;
			break;			
		case C_SCALER_CTRL_OUT_UYVY:			
			p_vid_dec_para->mpeg4_decode_out_format = C_MP4_OUTPUT_UY1VY0;
			p_vid_dec_para->video_decode_in_format = C_SCALER_CTRL_IN_UYVY;
			p_vid_dec_para->video_decode_out_format = C_SCALER_CTRL_OUT_UYVY;
			break;		
		default:
			DEBUG_MSG(DBG_PRINT("OutputFormatNoSupport!!!\r\n"));
			RETURN(STATUS_FAIL);
		}
	}

	nRet = STATUS_OK;
Return:
	return nRet;
}

void vid_dec_set_user_define_buffer(INT8U user_flag, INT32U addr0, INT32U addr1)
{
	if(user_flag)	//user use define buffer
	{
		p_vid_dec_para->user_define_flag = TRUE;
		if(p_vid_dec_para->video_format == C_MJPG_FORMAT)
		{
			p_vid_dec_para->video_decode_addr[0] = addr0;
			p_vid_dec_para->video_decode_addr[1] = addr1;
			p_vid_dec_para->scaler_output_addr[0] = 0;
			p_vid_dec_para->scaler_output_addr[1] = 0;
			p_vid_dec_para->deblock_addr[0] = 0;
			p_vid_dec_para->deblock_addr[1] = 0;
		}
		else
		{
			if(p_vid_dec_para->deblock_flag && p_vid_dec_para->scaler_flag)
			{
				p_vid_dec_para->scaler_output_addr[0] = addr0;
				p_vid_dec_para->scaler_output_addr[1] = addr1;
			}
			else if(p_vid_dec_para->deblock_flag)
			{
				p_vid_dec_para->deblock_addr[0] = addr0;
				p_vid_dec_para->deblock_addr[1] = addr1;
			}
			else if(p_vid_dec_para->scaler_flag)
			{
				p_vid_dec_para->scaler_output_addr[0] = addr0;
				p_vid_dec_para->scaler_output_addr[1] = addr1;
			}
			else
			{
				p_vid_dec_para->video_decode_addr[0] = addr0;
				p_vid_dec_para->video_decode_addr[1] = addr1;
			}
		}
	}
	else
	{
		p_vid_dec_para->user_define_flag = FALSE;
		p_vid_dec_para->video_decode_addr[0] = 0;
		p_vid_dec_para->video_decode_addr[1] = 0;
		p_vid_dec_para->scaler_output_addr[0] = 0;
		p_vid_dec_para->scaler_output_addr[1] = 0;
		p_vid_dec_para->deblock_addr[0] = 0;
		p_vid_dec_para->deblock_addr[1] = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//display frame buffer scaler
INT32S scaler_size_start(scaler_info *pScaler)
{
	INT32U temp_x;	
	INT32S scaler_status;
		
	if(pScaler->output_x > pScaler->output_buffer_x)
		pScaler->output_x = pScaler->output_buffer_x;
	
	if(pScaler->output_y > pScaler->output_buffer_y)
		pScaler->output_y = pScaler->output_buffer_y;

  	// Initiate Scaler
  	scaler_init();
  	scaler_input_pixels_set(pScaler->input_x, pScaler->input_y);
	scaler_input_visible_pixels_set(pScaler->input_visible_x, pScaler->input_visible_y);
	  	
	if(pScaler->scaler_mode == C_SCALER_FULL_SCREEN) {	
		// Fit to output buffer width and height
		scaler_output_pixels_set((pScaler->input_x<<16)/pScaler->output_x, 
								(pScaler->input_y<<16)/pScaler->output_y, 
								pScaler->output_buffer_x, 
								pScaler->output_buffer_y);
	} else if((pScaler->scaler_mode == C_NO_SCALER_FIT_BUFFER) && 
			(pScaler->input_x <= pScaler->output_x) && (pScaler->input_y <= pScaler->output_y)) {
		// fit to output buffer but not scaler
		scaler_output_pixels_set(1<<16, 1<<16, pScaler->output_buffer_x, pScaler->output_buffer_y);
	} else {
      	// Fit to output buffer width or height and scaler
      	if (pScaler->output_y * pScaler->input_x > pScaler->output_x * pScaler->input_y) 
      		temp_x = (pScaler->input_x<<16) / pScaler->output_x;
      	else 
      		temp_x = (pScaler->input_y<<16) / pScaler->output_y;
      		
     	scaler_output_pixels_set(temp_x, temp_x, pScaler->output_buffer_x, pScaler->output_buffer_y);
	}

	scaler_input_offset_set(0, 0);
	scaler_input_addr_set(pScaler->input_addr, 0, 0);	
   	scaler_output_addr_set(pScaler->output_addr, 0, 0);
   	scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	scaler_input_format_set(pScaler->input_format);
	scaler_output_format_set(pScaler->output_format);
	scaler_out_of_boundary_color_set(pScaler->boundary_color);
	
	scaler_status = scaler_wait_idle();
	if (scaler_status == C_SCALER_STATUS_STOP) 
		scaler_start();
	
	return scaler_status;
}

INT32S scaler_size_wait_done(void)
{
	INT32S scaler_status;
	
	scaler_status = scaler_wait_idle();
	
	if(scaler_status & C_SCALER_STATUS_DONE)
	{
		scaler_stop();
	}
	else if (scaler_status & C_SCALER_STATUS_TIMEOUT)
	{
		DEBUG_MSG(DBG_PRINT("Scaler Timeout failed to finish its job\r\n"));
	}
	else if(scaler_status & C_SCALER_STATUS_INIT_ERR) 
	{	
		DEBUG_MSG(DBG_PRINT("Scaler INIT ERR failed to finish its job\r\n"));	
	} 
	else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) 
	{
		scaler_restart();
	}
	else if(scaler_status & C_SCALER_STATUS_STOP)
	{
		DEBUG_MSG(DBG_PRINT("ScalerStop.\r\n"));	
	}
	else
	{
  		DEBUG_MSG(DBG_PRINT("Un-handled Scaler status!\r\n"));	
  	}	
	
	return scaler_status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//mjpeg decode api
INT32S mjpeg_seek_to_jpeg_header(INT32U raw_data_addr)
{
	INT8U *pdata;
	INT8S cnt;
	INT16U wdata;
	INT32U i;
	
	cnt = 0;
	//seek to data
	pdata = (INT8U*)raw_data_addr;
	wdata = *pdata++;
	wdata <<= 8;
	wdata |= *pdata++;
	
	for(i=0; i<100; i++)
	{
		if(wdata == 0xFFD8)
			break;
			
		wdata <<= 8;
		wdata |= *pdata++;	
		cnt++;
	} 
	
	if(i == 100)
		return -1;
	
	return cnt;
}

INT32S mjpeg_decode_get_size(INT32U raw_data_addr, INT32U size, INT16U *width, INT16U *height, INT16U *img_yuv_mode)
{
	INT32S nRet;
	
	nRet = mjpeg_seek_to_jpeg_header(raw_data_addr);
	if(nRet < 0) 
		RETURN(STATUS_FAIL);
	
	size -= nRet;
	raw_data_addr += nRet;
	jpeg_decode_init();
	nRet = jpeg_decode_parse_header((INT8U *) raw_data_addr, size);
	if(nRet != JPEG_PARSE_OK) 
		RETURN(STATUS_FAIL);

	*width = jpeg_decode_image_width_get();
	*height = jpeg_decode_image_height_get();
	*img_yuv_mode = jpeg_decode_image_yuv_mode_get();
	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S mjpeg_memory_alloc(mjpeg_info *pMjpeg)
{
	INT32U jpeg_output_y_size;
	INT32U jpeg_output_cb_cr_size;
	INT16U cbcr_shift, factor;

	if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420) {
		cbcr_shift = 2;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411) {
		cbcr_shift = 2;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV422) {
		cbcr_shift = 1;
		factor = 20;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV422V) {
		cbcr_shift = 1;
		factor = 20;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV444) {
		cbcr_shift = 0;
		factor = 30;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_GRAYSCALE) {
		cbcr_shift = 32;
		factor = 10;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411V) {
		cbcr_shift = 2;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420H2) {
		cbcr_shift = 1;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420V2) {
		cbcr_shift = 1;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411H2) {
		cbcr_shift = 1;
		factor = 15;
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411V2) {
		cbcr_shift = 1;
		factor = 15;
	} else {
		return STATUS_FAIL;
	}

	if (pMjpeg->fifo_line) {
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XXB)
		jpeg_output_y_size = pMjpeg->jpeg_extend_w * pMjpeg->fifo_line * 2;
		jpeg_output_cb_cr_size = jpeg_output_y_size >> cbcr_shift;
	#elif (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
		#if JPEG_DEC_UNION_EN == 0
		jpeg_output_y_size = pMjpeg->jpeg_extend_w*pMjpeg->fifo_line*2;
		jpeg_output_cb_cr_size = jpeg_output_y_size >> cbcr_shift;
		#else
		switch(pMjpeg->jpeg_yuv_mode) 
		{
		case C_JPG_CTRL_YUV444:
		case C_JPG_CTRL_YUV422:
		case C_JPG_CTRL_YUV422V:
		case C_JPG_CTRL_YUV420:
		case C_JPG_CTRL_YUV411:
			jpeg_output_y_size = pMjpeg->jpeg_extend_w*pMjpeg->fifo_line*2*2; //YUYV
			jpeg_output_cb_cr_size = 0;
			break;
			
		case C_JPG_CTRL_GRAYSCALE:
		case C_JPG_CTRL_YUV411V:
		case C_JPG_CTRL_YUV420H2:
		case C_JPG_CTRL_YUV420V2:
		case C_JPG_CTRL_YUV411H2:
		case C_JPG_CTRL_YUV411V2:
			jpeg_output_y_size = pMjpeg->jpeg_extend_w*pMjpeg->fifo_line*2;
			jpeg_output_cb_cr_size = jpeg_output_y_size >> cbcr_shift;
		}
		#endif
	#else //GPL327XX
		pMjpeg->fifo_line = ((pMjpeg->fifo_line * factor) / 10) >> 1;
		jpeg_output_y_size = pMjpeg->jpeg_extend_w * pMjpeg->fifo_line * 2 * 2; //YUYV
		jpeg_output_cb_cr_size = 0;
	#endif
	} else {
		jpeg_output_y_size = pMjpeg->jpeg_extend_w * pMjpeg->jpeg_extend_h;
		jpeg_output_cb_cr_size = jpeg_output_y_size >> cbcr_shift;
	}
	
  	g_jpeg_y_addr = (INT32U) gp_malloc_align(jpeg_output_y_size, 8);
  	if (!g_jpeg_y_addr) {
  		return STATUS_FAIL;
  	}
  	  	
  	if(jpeg_output_cb_cr_size) {
  		g_jpeg_cb_addr = (INT32U) gp_malloc_align(jpeg_output_cb_cr_size, 8);
  		if (!g_jpeg_cb_addr) {
  			gp_free((void *) g_jpeg_y_addr);
  			g_jpeg_y_addr = NULL;
  			return STATUS_FAIL;
  		}
  		g_jpeg_cr_addr = (INT32U) gp_malloc_align(jpeg_output_cb_cr_size, 8);
  		if (!g_jpeg_cr_addr) {
  			gp_free((void *) g_jpeg_cb_addr);
  			g_jpeg_cb_addr = NULL;
  			gp_free((void *) g_jpeg_y_addr);
  			g_jpeg_y_addr = NULL;
  			return STATUS_FAIL;
  		}

  	} else {
  		g_jpeg_cb_addr = 0;
  		g_jpeg_cr_addr = 0;
  	}
  	return STATUS_OK;
}

void mjpeg_memory_free(void)
{
	if(g_jpeg_y_addr)	gp_free((void*)g_jpeg_y_addr);
	if(g_jpeg_cb_addr)	gp_free((void*)g_jpeg_cb_addr);
	if(g_jpeg_cr_addr)	gp_free((void*)g_jpeg_cr_addr);

	g_jpeg_y_addr = 0;
	g_jpeg_cb_addr = 0;
	g_jpeg_cr_addr = 0;
}

void mjpeg_decode_set_scaler(mjpeg_info *pMjpeg)
{
	INT32U factor;
#if 0	
	COLOR_MATRIX_CFG color_matrix;

	scaler_color_matrix_switch(1);
	color_matrix.SCM_A11 = 0x0100;
	color_matrix.SCM_A12 = 0x0000;
	color_matrix.SCM_A13 = 0x0000;
	color_matrix.SCM_A21 = 0x0000;
	color_matrix.SCM_A22 = 0x0180;
	color_matrix.SCM_A23 = 0x0000;
	color_matrix.SCM_A31 = 0x0000;
	color_matrix.SCM_A32 = 0x0000;
	color_matrix.SCM_A33 = 0x0180;
	scaler_color_matrix_config(&color_matrix);
#endif
	scaler_input_pixels_set(pMjpeg->jpeg_valid_w, pMjpeg->jpeg_valid_h);
	scaler_input_visible_pixels_set(pMjpeg->jpeg_valid_w, pMjpeg->jpeg_valid_h);
	
	if (!pMjpeg->jpeg_output_w) {
	    pMjpeg->jpeg_output_w = pMjpeg->jpeg_output_buffer_w;
	}
	if (!pMjpeg->jpeg_output_h) {
	    pMjpeg->jpeg_output_h = pMjpeg->jpeg_output_buffer_h;
	}
    
    if (pMjpeg->scaler_mode == C_SCALER_FULL_SCREEN) {
    	// Fit to output buffer width and height
      	scaler_output_pixels_set((pMjpeg->jpeg_valid_w<<16)/pMjpeg->jpeg_output_w, 
      							(pMjpeg->jpeg_valid_h<<16)/pMjpeg->jpeg_output_h, 
      							pMjpeg->jpeg_output_buffer_w, 
      							pMjpeg->jpeg_output_buffer_h);
    } else if ((pMjpeg->scaler_mode == C_NO_SCALER_FIT_BUFFER) && 
    			(pMjpeg->jpeg_valid_w <= pMjpeg->jpeg_output_w) && 
    			(pMjpeg->jpeg_valid_h <= pMjpeg->jpeg_output_h)) {	
    	// fit to output buffer but not scaler
    	scaler_output_pixels_set(1<<16, 1<<16, pMjpeg->jpeg_output_buffer_w, pMjpeg->jpeg_output_buffer_h);
    } else {	
    	// Fit to output buffer width or height and scaler
      	if (pMjpeg->jpeg_output_h*pMjpeg->jpeg_valid_w > pMjpeg->jpeg_output_w*pMjpeg->jpeg_valid_h) 
      		factor = (pMjpeg->jpeg_valid_w<<16)/pMjpeg->jpeg_output_w; 
      	else 
      		factor = (pMjpeg->jpeg_valid_h<<16)/pMjpeg->jpeg_output_h;

      	scaler_output_pixels_set(factor, factor, pMjpeg->jpeg_output_buffer_w, pMjpeg->jpeg_output_buffer_h);
    }

	scaler_input_addr_set(g_jpeg_y_addr, g_jpeg_cb_addr, g_jpeg_cr_addr);
   	scaler_output_addr_set(pMjpeg->output_addr, NULL, NULL);
   	scaler_fifo_line_set(pMjpeg->scaler_fifo);
	scaler_YUV_type_set(C_SCALER_CTRL_TYPE_YCBCR);

#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XXB)
	if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV444) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV444);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV422) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV422V) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV420);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_GRAYSCALE) {
		scaler_input_format_set(C_SCALER_CTRL_IN_Y_ONLY);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411V) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411V);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420H2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV420V2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411H2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (pMjpeg->jpeg_yuv_mode == C_JPG_CTRL_YUV411V2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	}
#elif (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	#if JPEG_DEC_UNION_EN == 0
	switch(pMjpeg->jpeg_yuv_mode) 
	{
	case C_JPG_CTRL_YUV444:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV444);
		break;
	case C_JPG_CTRL_YUV422:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
		break;
	case C_JPG_CTRL_YUV422V:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
		break;			
	case C_JPG_CTRL_YUV420:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV420);
		break;
	case C_JPG_CTRL_YUV411:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411);
		break;
	case C_JPG_CTRL_GRAYSCALE:
		scaler_input_format_set(C_SCALER_CTRL_IN_Y_ONLY);
		break;
	case C_JPG_CTRL_YUV411V:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411V);
		break;
	case C_JPG_CTRL_YUV420H2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
		break;
	case C_JPG_CTRL_YUV420V2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
		break;
	case C_JPG_CTRL_YUV411H2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
		break;
	case C_JPG_CTRL_YUV411V2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
		break;
	}
	#else 
	switch(pMjpeg->jpeg_yuv_mode) 
	{
	case C_JPG_CTRL_YUV444:
	case C_JPG_CTRL_YUV422:
	case C_JPG_CTRL_YUV422V:
	case C_JPG_CTRL_YUV420:
	case C_JPG_CTRL_YUV411:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUYV);	//union mode
		break;
	case C_JPG_CTRL_GRAYSCALE:
		scaler_input_format_set(C_SCALER_CTRL_IN_Y_ONLY);
		break;
	case C_JPG_CTRL_YUV411V:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411V);
		break;
	case C_JPG_CTRL_YUV420H2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
		break;
	case C_JPG_CTRL_YUV420V2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
		break;
	case C_JPG_CTRL_YUV411H2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
		break;
	case C_JPG_CTRL_YUV411V2:
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
		break;
	}
	#endif
#else //GPL327XX
	scaler_input_format_set(C_SCALER_CTRL_IN_YUYV);
#endif
	scaler_output_format_set(pMjpeg->output_format);
	scaler_out_of_boundary_color_set(pMjpeg->boundary_color);
}

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
INT32U scaler_set_fifo_line(INT32U jpeg_fifo, INT32U line)
{
	INT32U scaler_fifo;
	
	if(jpeg_fifo == C_JPG_FIFO_16LINE && line == 16) {
		scaler_fifo = C_SCALER_CTRL_FIFO_16LINE;
	} else if(jpeg_fifo == C_JPG_FIFO_32LINE && line == 32) {
		scaler_fifo = C_SCALER_CTRL_FIFO_32LINE;
	} else if(jpeg_fifo == C_JPG_FIFO_64LINE && line == 64) {
		scaler_fifo = C_SCALER_CTRL_FIFO_64LINE;
	} else if(jpeg_fifo == C_JPG_FIFO_256LINE && line == 256) {
		scaler_fifo = C_SCALER_CTRL_FIFO_256LINE;
	} else {
		scaler_fifo = C_SCALER_CTRL_FIFO_DISABLE;
	}
	return scaler_fifo;
}

INT32S scaler_set_para(mjpeg_info *pMjpeg)
{
	FP32   N;
	INT32U factor;

	mjpeg_output_h_cnt = 0;
	mjpeg_scale_h_size = 0;
	mjpeg_scale_h_fifo = 0;
	mjpeg_CorrectScaleHSize = 0;
	if (!pMjpeg->jpeg_output_w) {
        pMjpeg->jpeg_output_w = pMjpeg->jpeg_output_buffer_w;
    }
    
    if (!pMjpeg->jpeg_output_h) {
        pMjpeg->jpeg_output_h = pMjpeg->jpeg_output_buffer_h;
    }
	
	if (pMjpeg->scaler_mode == C_SCALER_FULL_SCREEN) {      
		// Fit to output buffer width and height
		goto __exit; 
	} else if((pMjpeg->scaler_mode == C_NO_SCALER_FIT_BUFFER) && 
			(pMjpeg->jpeg_valid_w <= pMjpeg->jpeg_output_w) && 
			(pMjpeg->jpeg_valid_h <= pMjpeg->jpeg_output_h)) {
		pMjpeg->jpeg_output_w = pMjpeg->jpeg_valid_w;
		pMjpeg->jpeg_output_h = pMjpeg->jpeg_valid_h;
	} else if(pMjpeg->scaler_mode == C_SCALER_FIT_BUFFER ){						
		// Fit to output buffer width or height
		if((pMjpeg->jpeg_output_h * pMjpeg->jpeg_valid_w) > (pMjpeg->jpeg_output_w * pMjpeg->jpeg_valid_h)) {
  			factor = (pMjpeg->jpeg_valid_w<<16)/pMjpeg->jpeg_output_w;
  			pMjpeg->jpeg_output_h = (pMjpeg->jpeg_valid_h<<16)/factor;
  		} else {
  			factor = (pMjpeg->jpeg_valid_h<<16) / (pMjpeg->jpeg_output_h);
  			pMjpeg->jpeg_output_w = (pMjpeg->jpeg_valid_w<<16) / factor;
  		}
	}
	
__exit:
	/* compute jpeg decode times */
	N = (FP32)pMjpeg->jpeg_valid_h / (FP32)pMjpeg->fifo_line;
	
	/* compute scaler fifo output height with twice jpeg decode */
	mjpeg_scale_h_fifo = (pMjpeg->jpeg_output_h << 1) / N;
	return 0;
}

INT32S mjpeg_scaler_fifo_once(mjpeg_info *pMjpeg)
{	
	BOOLEAN bFinal;
	INT32S scaler_status;
	INT32S temp, N;
	INT32S jpeg_valid_h;
	INT32S output_h, output_buf_h;
	INT32U output_addr;
	
	/* add jpeg fifo decode count */ 
	mjpeg_output_h_cnt += pMjpeg->fifo_line;
	N = mjpeg_output_h_cnt / pMjpeg->fifo_line;
	
	if(mjpeg_output_h_cnt >= pMjpeg->jpeg_valid_h) {
		/* fine truning jpeg decode output height */
		bFinal = TRUE; 
	} else if((N % 2) == 0) {
		/* jpeg decode twice and scale once */ 
		bFinal = FALSE;
	} else {
		return C_SCALER_STATUS_DONE;
	}
	
	/* cal output address, for YUYV, RGRB or RGB565 */ 
	output_addr = pMjpeg->output_addr + pMjpeg->jpeg_output_buffer_w * mjpeg_scale_h_size * 2;
	
	/* check final scale output height */
	if(bFinal) {
		temp = pMjpeg->jpeg_valid_h - (mjpeg_output_h_cnt - pMjpeg->fifo_line);
		if(temp > 0) {
			if(N % 2) {
				N -= 1;
				jpeg_valid_h = temp;
			} else {
				N -= 2;
				jpeg_valid_h = temp + pMjpeg->fifo_line;
			}
		} else {
			return C_SCALER_STATUS_DONE;
		}
		
		/* set the remainder height to output_h and output_buf_h */
		output_h = pMjpeg->jpeg_output_h - mjpeg_scale_h_size;
		output_buf_h = pMjpeg->jpeg_output_buffer_h - mjpeg_scale_h_size;
	} else {
		jpeg_valid_h = pMjpeg->fifo_line << 1;
		
		mjpeg_CorrectScaleHSize += mjpeg_scale_h_fifo;
		mjpeg_scale_h_size += (INT32U)mjpeg_scale_h_fifo;
		
		output_h = (INT32S)mjpeg_scale_h_fifo;
		output_buf_h = (INT32S)mjpeg_scale_h_fifo;
		
		//adject offset
		temp = mjpeg_CorrectScaleHSize - mjpeg_scale_h_size;
		if(temp > 0) {
			mjpeg_scale_h_size += temp;
			output_h += temp;
			output_buf_h += temp;
		}
	}
	
	scaler_input_pixels_set(pMjpeg->jpeg_extend_w, jpeg_valid_h);
	scaler_input_visible_pixels_set(pMjpeg->jpeg_valid_w, jpeg_valid_h);				
	scaler_output_pixels_set((pMjpeg->jpeg_valid_w<<16) / pMjpeg->jpeg_output_w, 
							(jpeg_valid_h<<16) / output_h, 
							pMjpeg->jpeg_output_buffer_w, 
							output_buf_h);	
							
	scaler_input_addr_set(g_jpeg_y_addr, NULL, NULL);
   	scaler_output_addr_set(output_addr, NULL, NULL);  	
   	scaler_fifo_line_set(C_SCALER_CTRL_IN_FIFO_DISABLE);
	scaler_YUV_type_set(C_SCALER_CTRL_TYPE_YCBCR);
	scaler_input_format_set(C_SCALER_CTRL_IN_YUYV);
	scaler_output_format_set(pMjpeg->output_format);
	scaler_out_of_boundary_mode_set(1);	
	scaler_out_of_boundary_color_set(pMjpeg->boundary_color);
	
	while(1)
	{
		scaler_status = scaler_wait_idle();
	  	if (scaler_status == C_SCALER_STATUS_STOP) {
			if (scaler_start()) {
				DBG_PRINT("Failed to call scaler_start\r\n");
				break;
			}
	  	} else if (scaler_status & C_SCALER_STATUS_DONE) {
	  		// Scaler might finish its job before JPEG does when jpeg is zoomed in.
	  		scaler_stop();
	  		break;
	  	} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
			DBG_PRINT("Scaler failed to finish its job\r\n");
			break;
	  	} else {
	  		DBG_PRINT("Un-handled Scaler status!\r\n");
	  		break;
	  	}
  	}
  	
  	return scaler_status;
}
#endif

INT32S mjpeg_decode_and_scaler(mjpeg_info *pMjpeg)
{	
	#define	C_READ_SIZE			40*1024

	INT8U  *p_vlc;
	INT8U  scaler_done;
	INT32S jpeg_status, scaler_status;
	INT32S fly_len, nRet;
	
	nRet = mjpeg_seek_to_jpeg_header(pMjpeg->raw_data_addr);
	if(nRet < 0) {
		return -1;
	}
	pMjpeg->raw_data_size -= nRet;
	pMjpeg->raw_data_addr += nRet;
	
	jpeg_decode_init();
	jpeg_status = jpeg_decode_parse_header((INT8U *) pMjpeg->raw_data_addr, pMjpeg->raw_data_size);
	if(jpeg_status != JPEG_PARSE_OK) { 
		RETURN(STATUS_FAIL);
	}

	pMjpeg->jpeg_fifo = C_JPG_FIFO_256LINE;
	pMjpeg->scaler_fifo = C_SCALER_CTRL_FIFO_256LINE;
	pMjpeg->fifo_line = 256;
	pMjpeg->jpeg_yuv_mode = jpeg_decode_image_yuv_mode_get();
	pMjpeg->jpeg_valid_w = jpeg_decode_image_width_get();
	pMjpeg->jpeg_valid_h = jpeg_decode_image_height_get();
	pMjpeg->jpeg_extend_w = jpeg_decode_image_extended_width_get();
	pMjpeg->jpeg_extend_h = jpeg_decode_image_extended_height_get();

#if ((defined MCU_VERSION) && (MCU_VERSION < GPL326XXB))
	jpeg_using_scaler_mode_disable();
	if((pMjpeg->jpeg_output_w <= (pMjpeg->jpeg_valid_w>>1)) && 
		(pMjpeg->jpeg_output_h <= (pMjpeg->jpeg_valid_h>>1))) {
		jpeg_decode_level2_scaledown_enable();	// Use 1/2 scale-down mode
		pMjpeg->jpeg_valid_w >>= 1;
		pMjpeg->jpeg_valid_h >>= 1;
		pMjpeg->jpeg_extend_w >>= 1;
		pMjpeg->jpeg_extend_h >>= 1;
	}
#elif (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	#if JPEG_DEC_UNION_EN == 0
	jpeg_using_scaler_mode_disable();
	if((pMjpeg->jpeg_output_w <= (pMjpeg->jpeg_valid_w>>1)) && 
		(pMjpeg->jpeg_output_h <= (pMjpeg->jpeg_valid_h>>1))) {
		jpeg_decode_level2_scaledown_enable();	// Use 1/2 scale-down mode
		pMjpeg->jpeg_valid_w >>= 1;
		pMjpeg->jpeg_valid_h >>= 1;
		pMjpeg->jpeg_extend_w >>= 1;
		pMjpeg->jpeg_extend_h >>= 1;
	}
	#else
	switch(pMjpeg->jpeg_yuv_mode)
	{
	case C_JPG_CTRL_YUV444:
	case C_JPG_CTRL_YUV422:
	case C_JPG_CTRL_YUV422V:
	case C_JPG_CTRL_YUV420:
	case C_JPG_CTRL_YUV411:
		jpeg_using_scaler_mode_enable();
		if((pMjpeg->jpeg_output_w <= (pMjpeg->jpeg_valid_w>>2)) && 
			(pMjpeg->jpeg_output_h <= (pMjpeg->jpeg_valid_h>>2))) {
			jpeg_level4_scaledown_union_mode_enable(1);	// Use 1/4 scale-down mode
			pMjpeg->jpeg_valid_w >>= 2;
			pMjpeg->jpeg_valid_h >>= 2;
			pMjpeg->jpeg_extend_w >>= 2;
			pMjpeg->jpeg_extend_h >>= 2;
		} else if((pMjpeg->jpeg_output_w <= (pMjpeg->jpeg_valid_w>>1)) && 
			(pMjpeg->jpeg_output_h <= (pMjpeg->jpeg_valid_h>>1))) {
			jpeg_level2_scaledown_union_mode_enable(1);	// Use 1/2 scale-down mode
			pMjpeg->jpeg_valid_w >>= 1;
			pMjpeg->jpeg_valid_h >>= 1;
			pMjpeg->jpeg_extend_w >>= 1;
			pMjpeg->jpeg_extend_h >>= 1;
		}
		break;
	
	case C_JPG_CTRL_GRAYSCALE:
	case C_JPG_CTRL_YUV411V:
	case C_JPG_CTRL_YUV420H2:
	case C_JPG_CTRL_YUV420V2:
	case C_JPG_CTRL_YUV411H2:
	case C_JPG_CTRL_YUV411V2: 
		jpeg_using_scaler_mode_disable();
		if((pMjpeg->jpeg_output_w <= (pMjpeg->jpeg_valid_w>>1)) && 
			(pMjpeg->jpeg_output_h <= (pMjpeg->jpeg_valid_h>>1))) {
			jpeg_decode_level2_scaledown_enable();	// Use 1/2 scale-down mode
			pMjpeg->jpeg_valid_w >>= 1;
			pMjpeg->jpeg_valid_h >>= 1;
			pMjpeg->jpeg_extend_w >>= 1;
			pMjpeg->jpeg_extend_h >>= 1;
		}
		break;
	}
	#endif	
#else //GPL327XX
   	jpeg_using_scaler_mode_enable();
#endif

	if(mjpeg_memory_alloc(pMjpeg) < 0) {
		RETURN(STATUS_FAIL);
	}
	
	if(jpeg_decode_output_set(g_jpeg_y_addr, g_jpeg_cb_addr, g_jpeg_cr_addr, pMjpeg->jpeg_fifo) < 0) {
		RETURN(STATUS_FAIL);
	}

	p_vlc = jpeg_decode_image_vlc_addr_get();
	fly_len = pMjpeg->raw_data_addr + pMjpeg->raw_data_size - ((INT32U)p_vlc);	
	jpeg_decode_vlc_maximum_length_set(fly_len);	// Set maximum VLC length to prevent JPEG from hangging too long

	// Now start JPEG decoding on the fly
	nRet = ((INT32U)p_vlc) + C_READ_SIZE;
	nRet &= ~0x0F;			
	fly_len = nRet - ((INT32U)p_vlc);	//16 align
	if(jpeg_decode_on_the_fly_start(p_vlc, fly_len) < 0) { 
		RETURN(STATUS_FAIL);
	}
	
  	// Initiate Scaler
  	scaler_init();
  	scaler_done = 0;

	// Setup Scaler
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	pMjpeg->scaler_fifo = scaler_set_fifo_line(pMjpeg->jpeg_fifo, pMjpeg->fifo_line);
	if(pMjpeg->scaler_fifo != C_SCALER_CTRL_FIFO_DISABLE) {
#endif
		mjpeg_decode_set_scaler(pMjpeg);
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	} else { 
		scaler_set_para(pMjpeg);
	}
#endif  
  
  	while(1) 
  	{
  		jpeg_status = jpeg_decode_status_query(1);
  		
		if(jpeg_status & C_JPG_STATUS_DECODE_DONE) 
		{	// Wait until scaler finish its job
		#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
			if(pMjpeg->scaler_fifo != C_SCALER_CTRL_FIFO_DISABLE) {
		#endif
		  	while (1) 
		  	{
		  		scaler_status = scaler_wait_idle();
		  		if(scaler_status == C_SCALER_STATUS_STOP)
		  		{
					scaler_start();
				}
				else if(scaler_status & C_SCALER_STATUS_DONE) 
				{
					break;
				}
				else if(scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) 
				{
					DEBUG_MSG(DBG_PRINT("Scaler failed to finish its job\r\n"));
					break;
				} 
				else if(scaler_status & C_SCALER_STATUS_INPUT_EMPTY) 
				{
		  			scaler_restart();
		  		}
		  		else 
		  		{
			  		DEBUG_MSG(DBG_PRINT("Un-handled Scaler status!\r\n"));
			  		break;
			  	}
		  	}
		#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)	  	
			} else {
				scaler_status = mjpeg_scaler_fifo_once(pMjpeg);
			}
		#endif	
			break;
		}

  		if(jpeg_status & C_JPG_STATUS_INPUT_EMPTY) 
  		{
			p_vlc += fly_len;
			nRet = ((INT32U)p_vlc) + C_READ_SIZE;
			nRet &= ~0x0F;
			fly_len = nRet - ((INT32U)p_vlc);
			
			// Now restart JPEG decoding on the fly
		  	if(jpeg_decode_on_the_fly_start(p_vlc, fly_len) < 0) 
		  		RETURN(STATUS_FAIL);
		}

		if(jpeg_status & C_JPG_STATUS_OUTPUT_FULL) 
		{	// Start scaler to handle the full output FIFO now
		#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
			if(pMjpeg->scaler_fifo != C_SCALER_CTRL_FIFO_DISABLE) {
		#endif
		  	if(!scaler_done) 
		  	{
			  	scaler_status = scaler_wait_idle();
			  	if(scaler_status == C_SCALER_STATUS_STOP)
			  	{
					scaler_start();
			  	}
			  	else if(scaler_status & C_SCALER_STATUS_DONE)
			  	{
			  		scaler_done = 1;
			  	}
			  	else if(scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) 
			  	{
					DEBUG_MSG(DBG_PRINT("Scaler failed to finish its job\r\n"));
					break;
				} 
				else if(scaler_status & C_SCALER_STATUS_INPUT_EMPTY) 
				{
			  		scaler_restart();
			  	}
			  	else 
			  	{
			  		DEBUG_MSG(DBG_PRINT("Un-handled Scaler status!\r\n"));
			  		break;
			  	}
			}
		#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)	  	
			} else {
				scaler_status = mjpeg_scaler_fifo_once(pMjpeg);
			}
		#endif	
		
	  		// Now restart JPEG to output to next FIFO
	  		if(jpeg_decode_output_restart()) 
	  		{
	  			DEBUG_MSG(DBG_PRINT("Failed to call jpeg_decode_output_restart()\r\n"));
	  			break;
	  		}
		}		

		if(jpeg_status & C_JPG_STATUS_STOP) 
		{
			DEBUG_MSG(DBG_PRINT("JPEG is not started!\r\n"));
			break;
		}
		if(jpeg_status & C_JPG_STATUS_TIMEOUT)
		{
			DEBUG_MSG(DBG_PRINT("JPEG execution timeout!\r\n"));
			break;
		}
		if(jpeg_status & C_JPG_STATUS_INIT_ERR) 
		{
			DEBUG_MSG(DBG_PRINT("JPEG init error!\r\n"));
			break;
		}
		if(jpeg_status & C_JPG_STATUS_RST_VLC_DONE) 
		{
			DEBUG_MSG(DBG_PRINT("JPEG Restart marker number is incorrect!\r\n"));
			break;
		}
		if(jpeg_status & C_JPG_STATUS_RST_MARKER_ERR) 
		{
			DEBUG_MSG(DBG_PRINT("JPEG Restart marker sequence error!\r\n"));
			break;
		}
  	}
  	
  	nRet = STATUS_OK;
Return:  	
	jpeg_decode_stop();
	scaler_stop();
  	mjpeg_memory_free();
	return nRet;
}

INT32S mjpeg_decode_without_scaler(mjpeg_info *pMjpeg)
{
	INT8U *p_vlc;
	INT32U fly_len, header_len;
	INT32S nRet;
	
	nRet = mjpeg_seek_to_jpeg_header(pMjpeg->raw_data_addr);
	if(nRet < 0) return -1;
	pMjpeg->raw_data_size -= nRet;
	pMjpeg->raw_data_addr += nRet;

#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XXB)
	mjpeg_decode_init();
	nRet = jpeg_decode_parse_header((INT8U *) pMjpeg->raw_data_addr, pMjpeg->raw_data_size);
	if(nRet != JPEG_PARSE_OK) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = mjpeg_decode_output_format_set(pMjpeg->output_format);
	nRet = mjpeg_decode_output_addr_set(pMjpeg->output_addr, 0, 0);
	p_vlc = jpeg_decode_image_vlc_addr_get();
	header_len = ((INT32U) p_vlc) - pMjpeg->raw_data_addr;
	fly_len = pMjpeg->raw_data_addr + pMjpeg->raw_data_size - ((INT32U) p_vlc);
	nRet = jpeg_decode_vlc_maximum_length_set(fly_len);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}	
	
	nRet = mjpeg_decode_once_start(p_vlc, fly_len);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}	
	
	while (1) {
		nRet = mjpeg_decode_status_query(TRUE);
		if(nRet & C_JPG_STATUS_SCALER_DONE) {
			break;
		}
		if(nRet & (C_JPG_STATUS_STOP|C_JPG_STATUS_TIMEOUT|C_JPG_STATUS_INIT_ERR)) {
			RETURN(STATUS_FAIL);
		}
	}
#elif (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	#if JPEG_DEC_UNION_EN == 0
	mjpeg_decode_init();
	nRet = jpeg_decode_parse_header((INT8U *) pMjpeg->raw_data_addr, pMjpeg->raw_data_size);
	if(nRet != JPEG_PARSE_OK) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = mjpeg_decode_output_format_set(pMjpeg->output_format);
	nRet = mjpeg_decode_output_addr_set(pMjpeg->output_addr, 0, 0);
	p_vlc = jpeg_decode_image_vlc_addr_get();
	header_len = ((INT32U) p_vlc) - pMjpeg->raw_data_addr;
	fly_len = pMjpeg->raw_data_addr + pMjpeg->raw_data_size - ((INT32U) p_vlc);
	nRet = jpeg_decode_vlc_maximum_length_set(fly_len);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}	
		
	nRet = mjpeg_decode_once_start(p_vlc, fly_len);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}	
	
	while (1) {
		nRet = mjpeg_decode_status_query(TRUE);
		if(nRet & C_JPG_STATUS_SCALER_DONE) {
			break;
		}
		if(nRet & (C_JPG_STATUS_STOP|C_JPG_STATUS_TIMEOUT|C_JPG_STATUS_INIT_ERR)) {
			RETURN(STATUS_FAIL);
		}
	}
	#else
	jpeg_decode_init();
	nRet = jpeg_decode_parse_header((INT8U *) pMjpeg->raw_data_addr, pMjpeg->raw_data_size);
	if(nRet != JPEG_PARSE_OK) {
		RETURN(STATUS_FAIL);
	}
	
	if(jpeg_decode_output_set(pMjpeg->output_addr, NULL, NULL, C_JPG_FIFO_DISABLE) < 0) { 
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_addr_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
		
	jpeg_using_scaler_mode_enable();
	p_vlc = jpeg_decode_image_vlc_addr_get();
	header_len = ((INT32U) p_vlc) - pMjpeg->raw_data_addr;
	fly_len = pMjpeg->raw_data_addr + pMjpeg->raw_data_size - ((INT32U) p_vlc);
	
	if (jpeg_vlc_addr_set((INT32U) p_vlc)) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_addr_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	if (jpeg_vlc_maximum_length_set(fly_len)) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_maximum_length_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	if (jpeg_decompression_start()) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_decompression_start() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	while (1) {
		nRet = jpeg_status_polling(TRUE);
		if(nRet & C_JPG_STATUS_DECODE_DONE) {
			break;
		}
		if(nRet & (C_JPG_STATUS_STOP|C_JPG_STATUS_TIMEOUT|C_JPG_STATUS_INIT_ERR)) { 
			RETURN(STATUS_FAIL);
		}
	}
	#endif
#else //GPL327XX 
	jpeg_decode_init();
	nRet = jpeg_decode_parse_header((INT8U *) pMjpeg->raw_data_addr, pMjpeg->raw_data_size);
	if(nRet != JPEG_PARSE_OK) {
		RETURN(STATUS_FAIL);
	}
	
	if(jpeg_decode_output_set(pMjpeg->output_addr, NULL, NULL, C_JPG_FIFO_DISABLE) < 0) { 
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_addr_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
		
	jpeg_using_scaler_mode_enable();
	p_vlc = jpeg_decode_image_vlc_addr_get();
	header_len = ((INT32U) p_vlc) - pMjpeg->raw_data_addr;
	fly_len = pMjpeg->raw_data_addr + pMjpeg->raw_data_size - ((INT32U) p_vlc);
	
	if (jpeg_vlc_addr_set((INT32U) p_vlc)) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_addr_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	if (jpeg_vlc_maximum_length_set(fly_len)) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_vlc_maximum_length_set() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	if (jpeg_decompression_start()) {
		DEBUG_MSG(DBG_PRINT("Calling to jpeg_decompression_start() failed\r\n"));
		RETURN(STATUS_FAIL);
	}
	
	while (1) {
		nRet = jpeg_status_polling(TRUE);
		if(nRet & C_JPG_STATUS_DECODE_DONE) {
			break;
		}
		if(nRet & (C_JPG_STATUS_STOP|C_JPG_STATUS_TIMEOUT|C_JPG_STATUS_INIT_ERR)) { 
			RETURN(STATUS_FAIL);
		}
	}
#endif

	nRet = STATUS_OK;	
Return:
	mjpeg_decode_stop();	
	return nRet;
}

INT32S mjpeg_decode_stop_all(INT32U flag)
{
	if(flag)
	{
		jpeg_decode_stop();
		scaler_stop();
		mjpeg_memory_free();
	}
	else
	{
		mjpeg_decode_stop();
	}
	
	return STATUS_OK;
}

///////////////////////////////////////////////////////////////////////////////////
#if MPEG4_DECODE_ENABLE == 1
//paser mpeg4 bit stream infomation
typedef struct  
{   
	INT8U is_visual_object_start_code;
	INT8U is_vo_start_code;
    INT8U profile_and_level_indication;
    INT8U visual_object_verid;
    
    INT8U video_object_type_indication;
    INT8U video_object_layer_verid;
   	INT8U visual_object_type;
    INT8U is_object_layer_identifier;
	
	INT8U aspect_ratio_info;
	INT8U vol_control_par;
	INT8U chroma_format;
	INT8U low_delay;
	
	INT8U vbv_par;
	INT8U fixed_vop_rate;
	INT8U video_object_layer_shape;
    INT8U vopTimeIncResLen;
    
    INT32U vop_time_increment_resolution;
    INT16U par_width; 
	INT16U par_height;
    INT16U video_object_layer_width;
    INT16U video_object_layer_height;
    
    INT8U interlaced;
    INT8U obmc_disable;
    INT8U sprite_enable;
    INT8U not_8_bit;
    
    INT8U quant_bits;
    INT8U quant_type;
    INT8U quarter_sample;
    INT8U complexity_estimation_disable;
	
	INT8U resync_marker_disable;
	INT8U data_partitioned;
	INT8U reversible_vlc;
	INT8U newpred_enable;
	
	INT8U reduced_resolution_enable;
	INT8U scalability;
	INT8U vop_quant;
	INT8U time_increment;
	
	INT32U vop_id;
	INT32U vop_id_for_prediction;
	CHAR   user_data[256];
} Mpeg4ParaStruct;

static INT8U  g_bs_pos;
static INT8U  *g_bs_ptr;
static INT32U g_bs_buffer[2];
static Mpeg4ParaStruct *p_mp4_para, mp4_para;

static void bs_skip(INT8U bits);
static void bs_byte_align(void);
static INT32U bs_show_bits(INT8U bits);
static INT32U bs_get_bits(INT8U bits);

static void bs_skip(INT8U bits)
{	
	g_bs_pos += bits;
	if(g_bs_pos >= 32)
	{
		g_bs_buffer[0] = g_bs_buffer[1];
		g_bs_buffer[1] = (INT32U)(*g_bs_ptr++ << 24);
		g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 16);
		g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 8);
		g_bs_buffer[1] |= *g_bs_ptr++;
		g_bs_pos -= 32;
	}
}

static void bs_byte_align(void)
{
	INT32U remainder = g_bs_pos % 8;
	if (remainder) 
		bs_skip(8 - remainder);
}

static INT32U bs_show_bits(INT8U bits)
{
	INT32S nbit = (bits + g_bs_pos) - 32;
	if (nbit > 0) 
		return ((g_bs_buffer[0] & (0xffffffff >> g_bs_pos)) << nbit) | (g_bs_buffer[1] >> (32 - nbit));
	else 
		return (g_bs_buffer[0] & (0xffffffff >> g_bs_pos)) >> (32 - g_bs_pos - bits);
}

static INT32U bs_get_bits(INT8U bits)
{
	INT32U ret = bs_show_bits(bits); 
	bs_skip(bits);
	return ret;
}

void read_vol_complexity_estimation_header(void)
{
	INT8U method;	
	method = bs_get_bits(2);	/* estimation_method */
	if (method == 0 ||method == 1)
	{
		if(!bs_get_bits(1))		/* shape_complexity_estimation_disable */
		{
			bs_skip(1);		/* opaque */
			bs_skip(1);		/* transparent */
			bs_skip(1);		/* intra_cae */
			bs_skip(1);		/* inter_cae */
			bs_skip(1);		/* no_update */
			bs_skip(1);		/* upsampling */
		}

		if (!bs_get_bits(1))	/* texture_complexity_estimation_set_1_disable */
		{
			bs_skip(1);		/* intra_blocks */
			bs_skip(1);		/* inter_blocks */
			bs_skip(1);		/* inter4v_blocks */
			bs_skip(1);		/* not_coded_blocks */
		}
	}

	bs_skip(1);//READ_MARKER();
	if (!bs_get_bits(1))		/* texture_complexity_estimation_set_2_disable */
	{
		bs_skip(1);		/* dct_coefs */
		bs_skip(1);		/* dct_lines */
		bs_skip(1);		/* vlc_symbols */
		bs_skip(1);		/* vlc_bits */
	}

	if (!bs_get_bits(1))		/* motion_compensation_complexity_disable */
	{
		bs_skip(1);		/* apm */
		bs_skip(1);		/* npm */
		bs_skip(1);		/* interpolate_mc_q */
		bs_skip(1);		/* forw_back_mc_q */
		bs_skip(1);		/* halfpel2 */
		bs_skip(1);		/* halfpel4 */
	}

	bs_skip(1);//READ_MARKER();
	if (method == 1)
	{
		if (!bs_get_bits(1))	/* version2_complexity_estimation_disable */
		{
			bs_skip(1);		/* sadct */
			bs_skip(1);		/* quarterpel */
		}
	}
}

void vid_dec_parser_bit_stream_init(void)
{
	p_mp4_para = &mp4_para;
	gp_memset((INT8S*)p_mp4_para, 0, sizeof(Mpeg4ParaStruct)); 
	p_mp4_para->vop_time_increment_resolution = 1;
	p_mp4_para->visual_object_verid = 1;
	p_mp4_para->obmc_disable = 1;
	p_mp4_para->resync_marker_disable = 1;
	p_mp4_para->complexity_estimation_disable = 1;
}

INT32S vid_dec_paser_bit_stream(INT8U *pdata, INT32U bs_size, INT16U *width, INT16U *height, INT8U *time_inc_bits, INT8U *quant)
{		
	INT32S nRet, coding_type;
	INT32U temp, start_code;
	
	//init  
	g_bs_pos = 0;
	g_bs_ptr = pdata;
	pdata += bs_size + 8;
	g_bs_buffer[0] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[0] |= *g_bs_ptr++;	
	g_bs_buffer[1] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[1] |= *g_bs_ptr++;
	
	// init coding type when m4s2 format
	if(p_vid_dec_para->video_format == C_M4S2_FORMAT)
		coding_type = C_I_VOP;
	else 
		coding_type = C_UNKNOW_VOP;
		
	while(1)
	{
		bs_byte_align();
		start_code = bs_show_bits(32);
		if(start_code == VISOBJSEQ_START_CODE) //visual_object_sequence_start_code
		{
			bs_skip(32); //visual_object_sequence_start_code
			p_mp4_para->profile_and_level_indication = bs_get_bits(8); //profile_and_level_indication
			if(p_mp4_para->profile_and_level_indication == 0)
			{
				DEBUG_MSG(DBG_PRINT("profile_and_level_indication = 0\r\n"));
				//RETURN(ERROR_00); //don't care 
			}
		}
		else if(start_code == VISOBJSEQ_STOP_CODE) //visual_object_sequence_stop_code 
		{
			bs_skip(32); //visual_object_sequence_stop_code
		}
		else if(start_code == VISOBJ_START_CODE) //visual_object_start_code
		{
			// vidMP4StartCodeFind
			p_mp4_para->is_visual_object_start_code = 1;
			bs_skip(32); //visual_object_start_code
			
			// vidMP4VisualObjectVeridGet
			if(bs_get_bits(1))	//is_visual_object_identified					
			{
				p_mp4_para->visual_object_verid = bs_get_bits(4); //visual_object_ver_id
				bs_skip(3); //visual_object_priority 
			}
			else
			{
				p_mp4_para->visual_object_verid = 1;
			}
			
			p_mp4_para->visual_object_type = bs_show_bits(4);
			if(p_mp4_para->visual_object_type != VISOBJ_TYPE_VIDEO) //visual_object_type
			{
				DEBUG_MSG(DBG_PRINT("visual_object_type = %d, not support!\r\n", p_mp4_para->visual_object_type));
				RETURN(ERROR_01);
			}	 
			bs_skip(4);
			
			if(bs_get_bits(1))	//video_signal_type
			{
				//DEBUG_MSG(DBG_PRINT("+ video_signal_type!\r\n"));
				bs_skip(3); //video_format
				bs_skip(1); //video_range
				if(bs_get_bits(1))	//color_description
				{
					bs_skip(8);	//color_primaries
					bs_skip(8);	//transfer_characteristics
					bs_skip(8);	//matrix_coefficients
				}
			}	
		}
		else if((start_code & ~VIDOBJ_START_CODE_MASK) == VIDOBJ_START_CODE) 
		{
			// vidMP4StartCodeFind
			p_mp4_para->is_vo_start_code = 1;
			bs_skip(32); //video_object_start_code
		}
		else if((start_code & ~VIDOBJLAY_START_CODE_MASK)== VIDOBJLAY_START_CODE) //video_object_layer_start_code
		{
			// vidMP4StartCodeFind
			p_mp4_para->is_vo_start_code = 1; 
			bs_skip(32); //video_object_layer_start_code 
			
			// vidMP4VideoObjectTypeIndicationGet
			bs_skip(1); //random_accessible_vol
			p_mp4_para->video_object_type_indication = bs_get_bits(8); //video_object_type_indication
			if(p_mp4_para->video_object_type_indication != 1) //simple profile
			{
				DEBUG_MSG(DBG_PRINT("video_object_type_indication = %d, not support!!!\r\n", p_mp4_para->video_object_type_indication));
			#if ASP_ENABLE == 0
				RETURN(ERROR_02);
			#endif
			}
			
			// vidMP4VideoObjectLayerVeridGet
			p_mp4_para->is_object_layer_identifier = bs_get_bits(1); //is_object_layer_identifier
			if(p_mp4_para->is_object_layer_identifier == 1)						
			{
				p_mp4_para->video_object_layer_verid = bs_get_bits(4); //video_object_layer_verid
				bs_skip(3);	//video_object_layer_priority
			}
			else
			{
				p_mp4_para->video_object_layer_verid = p_mp4_para->visual_object_verid;
			}
			
			// vidMP4VolControlParGet
			p_mp4_para->aspect_ratio_info = bs_get_bits(4);	
			if(p_mp4_para->aspect_ratio_info == VIDOBJLAY_AR_EXTPAR)
			{
				p_mp4_para->par_width = bs_get_bits(8);
				p_mp4_para->par_height = bs_get_bits(8);
			}
			
			p_mp4_para->vol_control_par = bs_get_bits(1); //vol_control_parameters
			if(p_mp4_para->vol_control_par != 0)						
			{
				p_mp4_para->chroma_format = bs_get_bits(2); //chroma_format
				if(p_mp4_para->chroma_format != 1)
				{
					DEBUG_MSG(DBG_PRINT("vol_control_par = %d\r\n", p_mp4_para->vol_control_par));
					DEBUG_MSG(DBG_PRINT("chroma_format=%d, not 420 format\r\n", p_mp4_para->chroma_format));
					RETURN(ERROR_03);	
				}
				
				p_mp4_para->low_delay = bs_get_bits(1); //low_delay
				if(p_mp4_para->low_delay == 0) 
				{
					DEBUG_MSG(DBG_PRINT("low_delay=%d, VOL contains B-VOPs\r\n", p_mp4_para->low_delay));
					//RETURN(ERROR_04);	//same as 2900
				} 

				p_mp4_para->vbv_par = bs_get_bits(1); //vbv_parameters
				if(p_mp4_para->vbv_par == 1)					
				{
					INT32U bitrate, buffer_size, occupancy;
					//DEBUG_MSG(DBG_PRINT("vbv_par=%d\n", p_mp4_para->vbv_par));
					bitrate = bs_get_bits(15) << 15; //first_half_bit_rate
					bs_skip(1);	//READ_MARKER
					bitrate |= bs_get_bits(15); //latter_half_bit_rate
					bs_skip(1);	//READ_MARKER
					
					buffer_size = bs_get_bits(15) << 3;	//first_half_vbv_buffer_size
					bs_skip(1);	//READ_MARKER
					buffer_size |= bs_get_bits(3); //latter_half_vbv_buffer_size
					
					occupancy = bs_get_bits(11) << 15; //first_half_vbv_occupancy
					bs_skip(1);	//READ_MARKER
					occupancy |= bs_get_bits(15); //latter_half_vbv_occupancy
					bs_skip(1);	//READ_MARKER
				}
			}
			
			// vidMP4VideoObjectLayerShapeGet
			p_mp4_para->video_object_layer_shape = bs_get_bits(2); //video_object_layer_shape
			if(p_mp4_para->video_object_layer_shape != VIDOBJLAY_SHAPE_RECTANGULAR)
			{
				DEBUG_MSG(DBG_PRINT("video_object_layer_shape=%d, not support\r\n", p_mp4_para->video_object_layer_shape));
				RETURN(ERROR_05);
			}
			
			// vidMP4VopTimeIncrementResolutionGet
			bs_skip(1); //READ_MARKER
			p_mp4_para->vop_time_increment_resolution = bs_get_bits(16); //vop_time_increment_resolution
			temp = p_mp4_para->vop_time_increment_resolution; 
			if(temp > 0)
			{
				p_mp4_para->vopTimeIncResLen = 0;
				do
				{
					temp >>= 1;	
					p_mp4_para->vopTimeIncResLen++;
				}while(temp);
			}
			else
			{
				p_mp4_para->vopTimeIncResLen = 1;
			}
			
			*time_inc_bits = p_mp4_para->vopTimeIncResLen; 
			bs_skip(1); //READ_MARKER
			p_mp4_para->fixed_vop_rate = bs_get_bits(1); //fixed_vop_rate
			if(p_mp4_para->fixed_vop_rate == 1)
			{
				bs_skip(p_mp4_para->vopTimeIncResLen); //fixed_vop_time_increment
			}
			
			// vidMP4WidthHeightGet
			bs_skip(1); //READ_MARKER
			p_mp4_para->video_object_layer_width = bs_get_bits(13); //video_object_layer_width
			bs_skip(1); //READ_MARKER
			p_mp4_para->video_object_layer_height = bs_get_bits(13); //video_object_layer_height
			bs_skip(1); //READ_MARKER	
				
			*width = p_mp4_para->video_object_layer_width;
			*height = p_mp4_para->video_object_layer_height;
			if(p_mp4_para->video_object_layer_width > 768)
			{
				DEBUG_MSG(DBG_PRINT("video_object_layer_width > 768, not support\r\n", p_mp4_para->video_object_layer_width));
				RETURN(ERROR_06);
			}
		
			// vidMP4InterlacedGet
			p_mp4_para->interlaced = bs_get_bits(1);
			if(p_mp4_para->interlaced != 0)
			{
				DEBUG_MSG(DBG_PRINT("interlaced = %d, not support\r\n", p_mp4_para->interlaced));
				RETURN(ERROR_07);
			}
			
			// vidMP4ObmcDisableGet
			p_mp4_para->obmc_disable = bs_get_bits(1); //obmc_disable
			if(p_mp4_para->obmc_disable != 1)			
			{
				DEBUG_MSG(DBG_PRINT("obmc_disable = %d, not support\r\n", p_mp4_para->obmc_disable));
				RETURN(ERROR_08); //not support
			}
			
			// vidMP4SpriteEnableGet
			p_mp4_para->sprite_enable = bs_get_bits(p_mp4_para->video_object_layer_verid == 1 ? 1 : 2); //sprite_enable
			if(p_mp4_para->sprite_enable != 0)						
			{
				DEBUG_MSG(DBG_PRINT("sprite_enable = %d, not support\r\n", p_mp4_para->sprite_enable));
				RETURN(ERROR_09);
			}
			
			// vidMP4Not8BitGet
			p_mp4_para->not_8_bit = bs_get_bits(1);	//not_8_bit
			if(p_mp4_para->not_8_bit != 0) 
			{
				DEBUG_MSG(DBG_PRINT("not_8_bit = %d, not support\r\n", p_mp4_para->not_8_bit));
				RETURN(ERROR_0A);
			}
									
			if(p_mp4_para->not_8_bit)
			{
				p_mp4_para->quant_bits = bs_get_bits(4); //quant_precision
				bs_skip(4);	//bits_per_pixel
			}
			else
			{
				p_mp4_para->quant_bits = 5;
			}
			
			// vidMP4QuantTypeGet
			p_mp4_para->quant_type = bs_get_bits(1); //quant_type
			if(p_mp4_para->quant_type != 0)
			{
				DEBUG_MSG(DBG_PRINT("quant_type = %d, It should be 0\r\n", p_mp4_para->quant_type));
			#if ASP_ENABLE == 0
				RETURN(ERROR_0B);	//not support
			#endif
			}
			
			// vidMP4QuarterSampleGet
			if(p_mp4_para->video_object_layer_verid != 1)
			{
				p_mp4_para->quarter_sample = bs_get_bits(1); //quarter_sample
				if(p_mp4_para->quarter_sample == 1)
				{
					DEBUG_MSG(DBG_PRINT("quarter_sample = %d, It should be 0\r\n", p_mp4_para->quarter_sample));
				#if ASP_ENABLE == 0
					RETURN(ERROR_0C); //not support
				#endif
				}
			}
			
			// vidMP4CompEstDisableGet
			p_mp4_para->complexity_estimation_disable = bs_get_bits(1);
			if(p_mp4_para->complexity_estimation_disable != 1) //complexity estimation disable
			{	
				DEBUG_MSG(DBG_PRINT("complexity_estimation_disable = %d, It should be 1\r\n", p_mp4_para->complexity_estimation_disable));	
				read_vol_complexity_estimation_header();
			#if ASP_ENABLE == 0
				RETURN(ERROR_0D); //not support
			#endif		
			}
			
			// vidMP4DPRVLCGet
			p_mp4_para->resync_marker_disable = bs_get_bits(1);	//resync_marker_disable
			if(p_mp4_para->resync_marker_disable == 0)
			{
				DEBUG_MSG(DBG_PRINT("resync_marker_disable = %d\r\n", p_mp4_para->resync_marker_disable));
				//RETURN(ERROR_0E); //support
			}
			
			p_mp4_para->data_partitioned = bs_get_bits(1);	//data_partitioned
			if(p_mp4_para->data_partitioned == 1)						
			{
				p_mp4_para->reversible_vlc = bs_get_bits(1); //reversible_vlc
				DEBUG_MSG(DBG_PRINT("data_partitioned = %d, not support\r\n", p_mp4_para->data_partitioned));
				DEBUG_MSG(DBG_PRINT("reversible_vlc = %d\r\n", p_mp4_para->reversible_vlc));
				RETURN(ERROR_0F);
			}
			
			if(p_mp4_para->video_object_layer_verid != 1)
			{ 
				p_mp4_para->newpred_enable = bs_get_bits(1); //newpred_enable
				if(p_mp4_para->newpred_enable)					
				{
					bs_skip(2);	//requested_upstream_message_type
					bs_skip(1);	//newpred_segment_type
				}
				p_mp4_para->reduced_resolution_enable = bs_get_bits(1); //reduced_resolution_vop_enable
			}
			
			// vidMP4ScalabilityGet
			p_mp4_para->scalability = bs_get_bits(1); //scalability
			if(p_mp4_para->scalability != 0)			
			{
				bs_skip(1);	//hierarchy_type
				bs_skip(4);	//ref_layer_id
				bs_skip(1);	//ref_layer_sampling_direc
				bs_skip(5); //hor_sampling_factor_n
				bs_skip(5);	//hor_sampling_factor_m
				bs_skip(5);	//vert_sampling_factor_n
				bs_skip(5);	//vert_sampling_factor_m
				bs_skip(1);	//enhancement_type
				if(p_mp4_para->video_object_layer_shape == VIDOBJLAY_SHAPE_BINARY)
				{
					bs_skip(1);	//use_ref_shape 
					bs_skip(1);	//use_ref_texture 
					bs_skip(5);	//shape_hor_sampling_factor_n 
					bs_skip(5);	//shape_hor_sampling_factor_m 
					bs_skip(5);	//shape_vert_sampling_factor_n 
					bs_skip(5);	//shape_vert_sampling_factor_m 
				}
				DEBUG_MSG(DBG_PRINT("scalability = %d, not support\r\n", p_mp4_para->scalability));
				RETURN(ERROR_10);
			}
		} 
		else if (start_code == GRPOFVOP_START_CODE) 
		{
			bs_skip(32);
			{
				INT16U hours, minutes, seconds;
				hours = bs_get_bits(5);
				minutes = bs_get_bits(6);
				bs_skip(1); //READ_MARKE
				seconds = bs_get_bits(6);
			}
			bs_skip(1);	//closed_gov 
			bs_skip(1);	//broken_link 
		}	
		else if(start_code == VOP_START_CODE)
		{
			bs_skip(32); //vop_start_code 
			coding_type = bs_get_bits(2); //vop_coding_type 
			while (bs_get_bits(1) != 0); //time_base 

			bs_skip(1); //READ_MARKER
			if(p_mp4_para->vopTimeIncResLen) 
				p_mp4_para->time_increment = bs_get_bits(p_mp4_para->vopTimeIncResLen);	//vop_time_increment

			bs_skip(1); //READ_MARKER
			if(!bs_get_bits(1))	//vop_coded
				RETURN(C_N_VOP);
			
			if(p_mp4_para->newpred_enable)
			{
				if((p_mp4_para->vopTimeIncResLen + 3) >= 15)
					temp = 15;
					
				p_mp4_para->vop_id = bs_get_bits(temp);
				if(bs_get_bits(1)) //vop_id_for_prediction_indication 
				{
					p_mp4_para->vop_id_for_prediction = bs_get_bits(temp);
				}
				bs_skip(1); //READ_MARKER
			}

			if ((p_mp4_para->video_object_layer_shape != VIDOBJLAY_SHAPE_BINARY_ONLY) && (coding_type == C_P_VOP))
				bs_skip(1);	//rounding_type 
			
			if (p_mp4_para->reduced_resolution_enable && 
				(p_mp4_para->video_object_layer_shape == VIDOBJLAY_SHAPE_RECTANGULAR) &&
				((coding_type == C_P_VOP) || (coding_type == C_I_VOP)))
			{
				if (bs_get_bits(1))	//vop reduced resolution 
				{
					//return -10;	//RRV not supported
				}
			}

			if (p_mp4_para->video_object_layer_shape != VIDOBJLAY_SHAPE_RECTANGULAR) 
			{
				bs_skip(1);	//change_conv_ratio_disable 
				if (bs_get_bits(1))	//vop_constant_alpha 
				{
					bs_skip(8); //vop_constant_alpha_value 
				}
			}

			if (p_mp4_para->video_object_layer_shape != VIDOBJLAY_SHAPE_BINARY_ONLY) 
			{
				//if (!dec->complexity_estimation_disable)
				//{
				//	read_vop_complexity_estimation_header(bs, dec, coding_type);
				//}
				bs_skip(3); //intra_dc_threshold = intra_dc_threshold_table[BitstreamGetBits(bs, 3)]; // intra_dc_vlc_threshold
			}
			
			*quant = p_mp4_para->vop_quant = bs_get_bits(p_mp4_para->quant_bits); //vop_quant
			if(p_mp4_para->vop_quant < 1)			 
				*quant = p_mp4_para->vop_quant = 1;
				
			RETURN(coding_type);
		}
		else if (start_code == USERDATA_START_CODE) 
		{
			bs_skip(32); //user_data_start_code 
			gp_memset((INT8S*)p_mp4_para->user_data, 0, 256);
			for(temp=0; temp < 256; temp++)
			{
				if(temp == 0) 
					p_mp4_para->user_data[temp] = bs_show_bits(8);
				else
					p_mp4_para->user_data[temp] = (bs_show_bits(16) & 0xFF);
			
				if(p_mp4_para->user_data[temp] == 0)
					break;
				bs_skip(8);
			}
		}
		else  
		{
			bs_skip(8);	//start_code == ?
		}
		
		if(g_bs_ptr > pdata)
			RETURN(coding_type);
	}
	
Return:
	return nRet;
}

///////////////////////////////////////////////////////////////////////////////////
// h263 bit stream parser 
INT32S vid_dec_paser_h263_bit_stream(INT8U *pdata, INT16U *width, INT16U *height, INT32U *key_word)
{
	INT8U  *psrc, source_format;
	INT16U num_mb_in_gob;
	INT32S nRet, i;
	INT32U h263_header;
	
	//get key word
	psrc = pdata;
	*key_word = *psrc++;
	*key_word <<= 8;
	*key_word |= *psrc++;
	*key_word <<= 8;
	*key_word |= *psrc++;
	
	for(i=0; i<50; i++)
	{
		if((*key_word & 0x000080) == 0x000080)
		{
			*key_word <<= 8;
			*key_word |= *psrc++;
			break;
		}	
		
		pdata++;
		*key_word <<= 8;
		*key_word |= *psrc++;
		*key_word &= 0x00FFFFFF;
	}
	if(i == 50)
		RETURN(ERROR_00);
	
	//parser bs
	g_bs_pos = 0;
	g_bs_ptr = pdata;
	g_bs_buffer[0] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[0] |= *g_bs_ptr++;	
	g_bs_buffer[1] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[1] |= *g_bs_ptr++;	
	
	h263_header = bs_get_bits(22);
	if(h263_header != 0x0020)
		RETURN(ERROR_01);
	
	bs_skip(8); //temporal_reference
	bs_skip(1); //marker_bit
	bs_skip(1); //zero_bit
	bs_skip(1); //split_screen_indicator
	bs_skip(1); //document_camera_indicator
	bs_skip(1); //full_picture_freeze_release
			
	source_format = bs_get_bits(3);
	switch(source_format) 
	{
	case 1:
		*width = 128; *height = 96; num_mb_in_gob = 8;
		break;
	case 2: 
		*width = 176; *height = 144; num_mb_in_gob = 11;
		break;
	case 3:	
		*width = 352; *height = 288;  num_mb_in_gob = 22;
		break;
	case 4:
		*width = 704; *height = 576;  num_mb_in_gob = 88;
		break;
	default:
		RETURN(ERROR_02);
	}
	
	nRet = bs_get_bits(1); //h263_picture_coding_type
Return:
	return nRet;
}

///////////////////////////////////////////////////////////////////////////////////
#if S263_DECODE_ENABLE == 1
INT32S vid_dec_paser_sorenson263_bit_stream(INT8U *pdata, INT16U *width, INT16U *height, INT32U *key_word)
{
	INT8U  *psrc;
	INT32S  nRet, i, temp;
	
	//get key word
	psrc = pdata;
	*key_word = *psrc++;
	*key_word <<= 8;
	*key_word |= *psrc++;
	*key_word <<= 8;
	*key_word |= *psrc++;
	
	for(i=0; i<50; i++)
	{
		if((*key_word & 0x000080) == 0x000080)
		{
			*key_word <<= 8;
			*key_word |= *psrc++;
			break;
		}	
		
		pdata++;
		*key_word <<= 8;
		*key_word |= *psrc++;
		*key_word &= 0x00FFFFFF;
	}
	if(i == 50)
		RETURN(ERROR_00);
	
	g_bs_pos = 0;
	g_bs_ptr = pdata;
	g_bs_buffer[0] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[0] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[0] |= *g_bs_ptr++;	
	g_bs_buffer[1] = (INT32U)(*g_bs_ptr++ << 24);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 16);
	g_bs_buffer[1] |= (INT32U)(*g_bs_ptr++ << 8);
	g_bs_buffer[1] |= *g_bs_ptr++;	
	
	//StartCode
	if(bs_get_bits(17) != 1)       
	{
		DEBUG_MSG(DBG_PRINT("StartCodeMismatch !!!\r\n")); 
		RETURN(ERROR_01);
	}
	
	//Version
	nRet = bs_get_bits(5); //h263_version
	
	//TemporalReference
	bs_get_bits(8);
	
	//PictureSize
	temp = bs_show_bits(3);
	switch(temp)
	{
	case 0:
		bs_skip(3);
	  	*width = bs_get_bits(8);
	  	*height = bs_show_bits(8);
	  	bs_skip(7); 
	  	*key_word = *psrc++;
		*key_word <<= 8;
		*key_word |= *psrc++;
		*key_word <<= 8;
		*key_word |= *psrc++;
	  	*key_word <<= 8;
		*key_word |= *psrc++;
		break;
		
	case 1:
		bs_skip(3);
	  	*width = bs_get_bits(16);
	  	*height = bs_show_bits(16);
	  	bs_skip(15); 
	  	*key_word = *psrc++;
		*key_word <<= 8;
		*key_word |= *psrc++;
		*key_word <<= 8;
		*key_word |= *psrc++;
	  	*key_word <<= 8;
		*key_word |= *psrc++;
		break;
		
	case 2:
		bs_skip(2); 
	  	*width = 352;
	  	*height = 288;
		break;
		
	case 3:
		bs_skip(2); 
	  	*width = 176;
	  	*height = 144;
		break;
		
	case 4:
		bs_skip(2);
	    *width = 128;
	    *height = 96;
		break;
		
	case 5:
		bs_skip(2);
	  	*width = 320;
	  	*height = 240;
		break;
		
	case 6:
		bs_skip(2); 
	  	*width =160;
	  	*height =120;
		break;
		
	default:	
		DEBUG_MSG(DBG_PRINT("InvalidWidth/Height!!!\r\n")); 
		RETURN(ERROR_02);
	}
	
	*width = (*width + 15) & ~0x0F;
	*height = (*height + 15) & ~0x0F;

Return:	
	return nRet;
}
#endif
#endif
