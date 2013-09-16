#include <stdlib.h>
#include "avi_encoder_app.h"
#include "jpeg_header.h"
#include "font.h"

/* global varaible */
AviEncPara_t AviEncPara, *pAviEncPara;
AviEncAudPara_t AviEncAudPara, *pAviEncAudPara;
AviEncVidPara_t AviEncVidPara, *pAviEncVidPara;
AviEncPacker_t AviEncPacker0, *pAviEncPacker0;
AviEncPacker_t AviEncPacker1, *pAviEncPacker1;

GP_AVI_AVISTREAMHEADER	avi_aud_stream_header;
GP_AVI_AVISTREAMHEADER	avi_vid_stream_header;
GP_AVI_BITMAPINFO		avi_bitmap_info;
GP_AVI_PCMWAVEFORMAT	avi_wave_info;

#if MIC_INPUT_SRC == C_ADC_LINE_IN || MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	static DMA_STRUCT g_avi_adc_dma_dbf;
#elif MIC_INPUT_SRC == C_GPY0050_IN
	INT8U  g_mic_buffer;
	INT16U g_gpy0050_pre_value;
	INT32U g_mic_cnt;
#endif

static INT8U g_csi_index;
static INT8U g_pcm_index;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// avi encode api	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void avi_encode_init(void)
{
    pAviEncPara = &AviEncPara;
    gp_memset((INT8S *)pAviEncPara, 0, sizeof(AviEncPara_t));
    
    pAviEncAudPara = &AviEncAudPara;
    gp_memset((INT8S *)pAviEncAudPara, 0, sizeof(AviEncAudPara_t));
	pAviEncVidPara = &AviEncVidPara;
    gp_memset((INT8S *)pAviEncVidPara, 0, sizeof(AviEncVidPara_t));

    pAviEncPacker0 = &AviEncPacker0;
    gp_memset((INT8S *)pAviEncPacker0, 0, sizeof(AviEncPacker_t));   
    pAviEncPacker1 = &AviEncPacker1;
    gp_memset((INT8S *)pAviEncPacker1, 0, sizeof(AviEncPacker_t));
    
	pAviEncPacker0->file_handle = -1;
	pAviEncPacker0->index_handle = -1;
	pAviEncPacker1->file_handle = -1;
	pAviEncPacker1->index_handle = -1;
	pAviEncVidPara->scaler_zoom_ratio = 1;
}

static void avi_encode_sync_timer_isr(void)
{
	if(pAviEncPara->AviPackerCur->p_avi_wave_info) {
		if((pAviEncPara->tv - pAviEncPara->ta) < pAviEncPara->delta_ta) {
			pAviEncPara->tv += pAviEncPara->tick;
		}	 
	} else {
		pAviEncPara->tv += pAviEncPara->tick;
	}
	
	if((pAviEncPara->tv - pAviEncPara->Tv) > 0) {
		if(pAviEncPara->post_cnt == pAviEncPara->pend_cnt) {
			if(OSQPost(AVIEncodeApQ, (void*)MSG_AVI_ONE_FRAME_ENCODE) == OS_NO_ERR) {
				pAviEncPara->post_cnt++;
			}
		}
	}
}

void avi_encode_video_timer_start(void)
{
	INT32U temp, freq_hz;
	INT32U preload_value;
	
	pAviEncPara->ta = 0;
	pAviEncPara->tv = 0;
	pAviEncPara->Tv = 0;
	pAviEncPara->pend_cnt = 0;
	pAviEncPara->post_cnt = 0;
#if MIC_INPUT_SRC == C_ADC_LINE_IN 
	if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_C) {
		preload_value = R_TIMERC_PRELOAD;
	} else if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_D) {
		preload_value = R_TIMERD_PRELOAD;
	} else if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_E) {
		preload_value = R_TIMERE_PRELOAD;
	} else {//timerf 
		preload_value = R_TIMERF_PRELOAD;	
	} 
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	/*if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_A) {
		preload_value = R_TIMERA_PRELOAD;	
	} else if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_B) {
		preload_value = R_TIMERB_PRELOAD;	
	} else */if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_C) {
		preload_value = R_TIMERC_PRELOAD;
	} else if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_D) {
		preload_value = R_TIMERD_PRELOAD;
	} else if(AVI_AUDIO_RECORD_TIMER == ADC_AS_TIMER_E) {
		preload_value = R_TIMERE_PRELOAD;
	} else {	//timerf 
		preload_value = R_TIMERF_PRELOAD;	
	} 
#elif MIC_INPUT_SRC == C_GPY0050_IN
	if(AVI_AUDIO_RECORD_TIMER == TIMER_A) {
		preload_value = R_TIMERA_PRELOAD;
    } else if(AVI_AUDIO_RECORD_TIMER == TIMER_B) {
		preload_value = R_TIMERB_PRELOAD;
	} else if(AVI_AUDIO_RECORD_TIMER == TIMER_C) {
		preload_value = R_TIMERC_PRELOAD;	
	} else if(AVI_AUDIO_RECORD_TIMER == TIMER_D) {
		preload_value = R_TIMERD_PRELOAD;	
	} else if(AVI_AUDIO_RECORD_TIMER == TIMER_E) {
		preload_value = R_TIMERE_PRELOAD;	
	} else {	//timer F
		preload_value = R_TIMERF_PRELOAD;	
	}
#endif 	

	if(pAviEncPara->AviPackerCur->p_avi_wave_info) {
		//temp = 0x10000 -((0x10000 - (R_TIMERE_PRELOAD & 0xFFFF)) * p_vid_dec_para->n);
		temp = (0x10000 - (preload_value & 0xFFFF)) * pAviEncPara->freq_div;
		freq_hz = MCLK/2/temp;
		if(MCLK %(2*temp)) {
			freq_hz++;
		}
	} else {
		freq_hz = AVI_ENCODE_TIME_BASE;
	}	
	timer_freq_setup(AVI_ENCODE_VIDEO_TIMER, freq_hz, 0, avi_encode_sync_timer_isr);
}

void avi_encode_video_timer_stop(void)
{
	timer_stop(AVI_ENCODE_VIDEO_TIMER);
}

void avi_encode_audio_timer_start(void)
{
#if MIC_INPUT_SRC == C_ADC_LINE_IN
	adc_sample_rate_set(AVI_AUDIO_RECORD_TIMER, pAviEncAudPara->audio_sample_rate);
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	mic_sample_rate_set(AVI_AUDIO_RECORD_TIMER, pAviEncAudPara->audio_sample_rate);
#elif MIC_INPUT_SRC == C_GPY0050_IN		
	timer_freq_setup(AVI_AUDIO_RECORD_TIMER, pAviEncAudPara->audio_sample_rate, 0, gpy0050_timer_isr);
#endif
}

void avi_encode_audio_timer_stop(void)
{
#if MIC_INPUT_SRC == C_ADC_LINE_IN
	adc_timer_stop(AVI_AUDIO_RECORD_TIMER);
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	mic_timer_stop(AVI_AUDIO_RECORD_TIMER);
#elif MIC_INPUT_SRC == C_GPY0050_IN	
	timer_stop(AVI_AUDIO_RECORD_TIMER);
#endif
}

// file handle
INT32S avi_encode_set_file_handle_and_caculate_free_size(AviEncPacker_t *pAviEncPacker, INT16S FileHandle)
{
	INT16S disk_no;
	INT64U disk_free;
#if AVI_ENCODE_FAST_SWITCH_EN == 1	
	struct stat_t statbuf;
#endif
	
	//SOURCE_TYPE_USER_DEFINE
	if(FileHandle > 100) {
		pAviEncPara->disk_free_size = (INT64U)FileHandle << 48;
   		pAviEncPara->record_total_size = 2*32*1024 + 16; 
		return STATUS_OK;
	}
	
	//create index file handle
    disk_no = GetDiskOfFile(FileHandle);
    if(disk_no == 0) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"A:\\");
    } else if(disk_no == 1) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"B:\\");
    } else if(disk_no == 2) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"C:\\");
    } else if(disk_no == 3) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"D:\\");
    } else if(disk_no == 4) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"E:\\");
    } else if(disk_no == 5) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"F:\\");
    } else if(disk_no == 6) {
    	gp_strcpy((INT8S*)pAviEncPacker->index_path, (INT8S*)"G:\\");
    } else {
    	return STATUS_FAIL;
    }
    
    chdir((CHAR*)pAviEncPacker->index_path);
#if AVI_ENCODE_FAST_SWITCH_EN == 1
    if(stat("index0.tmp", &statbuf) < 0) {
    	gp_strcat((INT8S*)pAviEncPacker->index_path, (INT8S*)"index0.tmp");
    } else { 
    	gp_strcat((INT8S*)pAviEncPacker->index_path, (INT8S*)"index1.tmp");
	}
#else
   	gp_strcat((INT8S*)pAviEncPacker->index_path, (INT8S*)"index0.tmp");
#endif
    
    pAviEncPacker->file_handle = FileHandle;
    pAviEncPacker->index_handle = open((char*)pAviEncPacker->index_path, O_RDWR|O_CREAT|O_TRUNC);
    if(pAviEncPacker->index_handle < 0) {
    	return STATUS_FAIL;
   	}
   	
   	//caculate storage free size 
    disk_free = vfsFreeSpace(disk_no);
    DEBUG_MSG(DBG_PRINT("DISK FREE SIZE = %dMByte\r\n", disk_free/1024/1024));
    if(disk_free < C_MIN_DISK_FREE_SIZE) {
    	avi_encode_close_file(pAviEncPacker);
  		return STATUS_FAIL;
  	}
    	
    pAviEncPara->disk_free_size = disk_free - C_MIN_DISK_FREE_SIZE;
   	pAviEncPara->record_total_size = 2*32*1024 + 16; //avi header + data is 32k align + index header
	return STATUS_OK;
}

INT16S avi_encode_close_file(AviEncPacker_t *pAviEncPacker)
{
	#define C_AVIPACKER_V323	"GP$323 Generalplus AviPackerV3 20110701"
	#define C_AVIPACKER_V324	"GP$324 Generalplus AviPackerV3 20120117"
	#define C_AVIPACKER_V325	"GP$325 Generalplus AviPackerV3 2012.11.29"
	
	INT32S nRet;
	
	if(gp_strcmp((INT8S *)AviPackerV3_GetVersion(), (INT8S *)C_AVIPACKER_V323) == 0) {
		nRet = file_cat(pAviEncPacker->file_handle, pAviEncPacker->index_handle);
		nRet = close(pAviEncPacker->file_handle);
		nRet = close(pAviEncPacker->index_handle);
    	nRet = unlink2((CHAR*)pAviEncPacker->index_path);
	} else if(gp_strcmp((INT8S *)AviPackerV3_GetVersion(), (INT8S *)C_AVIPACKER_V324) == 0) { 
		nRet = close(pAviEncPacker->file_handle);
		nRet = close(pAviEncPacker->index_handle);
    	nRet = unlink2((CHAR*)pAviEncPacker->index_path);
	} else if(gp_strcmp((INT8S *)AviPackerV3_GetVersion(), (INT8S *)C_AVIPACKER_V325) == 0) { 
		nRet = close(pAviEncPacker->file_handle);
		nRet = close(pAviEncPacker->index_handle);
    	nRet = unlink2((CHAR*)pAviEncPacker->index_path);
	} else {
		INT32U buf = (INT32U) gp_malloc_align(32768, 32);
		
		if(!buf) {
			DEBUG_MSG(DBG_PRINT("buf allocate fail.\r\n"));
			return -1;
		}
	
		lseek(pAviEncPacker->file_handle, 0, SEEK_END);
		lseek(pAviEncPacker->index_handle, 0, SEEK_SET);
		while(1) {
			nRet = read(pAviEncPacker->index_handle, buf, 32768);
			if(nRet) {
				write(pAviEncPacker->file_handle, buf, nRet);
			} else {
				break;
			}
		}
		
		gp_free((INT32U*)buf);
		nRet = close(pAviEncPacker->file_handle);
		nRet = close(pAviEncPacker->index_handle);
	    nRet = unlink((CHAR*)pAviEncPacker->index_path);
	}
	
	pAviEncPacker->file_handle = -1;
	pAviEncPacker->index_handle = -1;
#if (NAND1_EN || NAND2_EN)
	nRet = GetDiskOfFile(pAviEncPacker->file_handle);	
	if(nRet == FS_NAND1 || nRet == FS_NAND2) {
		DrvNand_flush_allblk(); 
		DEBUG_MSG(DBG_PRINT("Nand_flush_allblk.\r\n"));	
	}
	nRet = 0;
#endif
	return nRet;
}

void avi_encode_fail_handle_close_file(AviEncPacker_t *pAviEncPacker)
{	
    close(pAviEncPacker->file_handle);
	close(pAviEncPacker->index_handle);
    unlink((CHAR*)pAviEncPacker->index_path);
	pAviEncPacker->file_handle = -1;
	pAviEncPacker->index_handle = -1;
}

INT32S avi_encode_set_avi_header(AviEncPacker_t *pAviEncPacker)
{
	INT16U sample_per_block, package_size;
	
	pAviEncPacker->p_avi_aud_stream_header = &avi_aud_stream_header;
	pAviEncPacker->p_avi_vid_stream_header = &avi_vid_stream_header;
	pAviEncPacker->p_avi_bitmap_info = &avi_bitmap_info;
	pAviEncPacker->p_avi_wave_info = &avi_wave_info;
	gp_memset((INT8S*)pAviEncPacker->p_avi_aud_stream_header, 0, sizeof(GP_AVI_AVISTREAMHEADER));
	gp_memset((INT8S*)pAviEncPacker->p_avi_vid_stream_header, 0, sizeof(GP_AVI_AVISTREAMHEADER));
	gp_memset((INT8S*)pAviEncPacker->p_avi_bitmap_info, 0, sizeof(GP_AVI_BITMAPINFO));
	gp_memset((INT8S*)pAviEncPacker->p_avi_wave_info, 0, sizeof(GP_AVI_PCMWAVEFORMAT));
	
	//audio
	avi_aud_stream_header.fccType[0] = 'a';
	avi_aud_stream_header.fccType[1] = 'u';
	avi_aud_stream_header.fccType[2] = 'd';
	avi_aud_stream_header.fccType[3] = 's';
	
	switch(pAviEncAudPara->audio_format) 
	{
	case WAVE_FORMAT_PCM:
		pAviEncPacker->wave_info_cblen = 16;
		avi_aud_stream_header.fccHandler[0] = 1;
		avi_aud_stream_header.fccHandler[1] = 0;
		avi_aud_stream_header.fccHandler[2] = 0;
		avi_aud_stream_header.fccHandler[3] = 0;
			
		avi_wave_info.wFormatTag = WAVE_FORMAT_PCM;
		avi_wave_info.nChannels = pAviEncAudPara->channel_no;	
		avi_wave_info.nSamplesPerSec =  pAviEncAudPara->audio_sample_rate;
		avi_wave_info.nAvgBytesPerSec =  pAviEncAudPara->channel_no * pAviEncAudPara->audio_sample_rate * 2; 
		avi_wave_info.nBlockAlign = pAviEncAudPara->channel_no * 2;
		avi_wave_info.wBitsPerSample = 16; //16bit
			
		avi_aud_stream_header.dwScale = avi_wave_info.nBlockAlign;
		avi_aud_stream_header.dwRate = avi_wave_info.nAvgBytesPerSec;
		avi_aud_stream_header.dwSampleSize = avi_wave_info.nBlockAlign;;	
		break;
		
	case WAVE_FORMAT_ADPCM:
		pAviEncPacker->wave_info_cblen = 50;
		avi_aud_stream_header.fccHandler[0] = 0;
		avi_aud_stream_header.fccHandler[1] = 0;
		avi_aud_stream_header.fccHandler[2] = 0;
		avi_aud_stream_header.fccHandler[3] = 0;

		package_size = 0x100;
		if(pAviEncAudPara->channel_no == 1) {
			sample_per_block = 2 * package_size - 12;
		} else if(pAviEncAudPara->channel_no == 2) {
			sample_per_block = package_size - 12;
		} else {
			sample_per_block = 1;
		} 
		avi_wave_info.wFormatTag = WAVE_FORMAT_ADPCM;
		avi_wave_info.nChannels = pAviEncAudPara->channel_no;	
		avi_wave_info.nSamplesPerSec =  pAviEncAudPara->audio_sample_rate;
		avi_wave_info.nAvgBytesPerSec =  package_size * pAviEncAudPara->audio_sample_rate / sample_per_block; // = PackageSize * FrameSize / fs
		avi_wave_info.nBlockAlign = package_size; //PackageSize
		avi_wave_info.wBitsPerSample = 4; //4bit
		avi_wave_info.cbSize = 32;
		// extension ...
		avi_wave_info.ExtInfo[0] = 0x01F4;	avi_wave_info.ExtInfo[1] = 0x0007;	
		avi_wave_info.ExtInfo[2] = 0x0100;	avi_wave_info.ExtInfo[3] = 0x0000;
		avi_wave_info.ExtInfo[4] = 0x0200;	avi_wave_info.ExtInfo[5] = 0xFF00;
		avi_wave_info.ExtInfo[6] = 0x0000;	avi_wave_info.ExtInfo[7] = 0x0000;
		
		avi_wave_info.ExtInfo[8] =  0x00C0;	avi_wave_info.ExtInfo[9] =  0x0040;
		avi_wave_info.ExtInfo[10] = 0x00F0; avi_wave_info.ExtInfo[11] = 0x0000;
		avi_wave_info.ExtInfo[12] = 0x01CC; avi_wave_info.ExtInfo[13] = 0xFF30;
		avi_wave_info.ExtInfo[14] = 0x0188; avi_wave_info.ExtInfo[15] = 0xFF18;
		break;
		
	case WAVE_FORMAT_IMA_ADPCM:
		pAviEncPacker->wave_info_cblen = 20;
		avi_aud_stream_header.fccHandler[0] = 0;
		avi_aud_stream_header.fccHandler[1] = 0;
		avi_aud_stream_header.fccHandler[2] = 0;
		avi_aud_stream_header.fccHandler[3] = 0;
		
		package_size = 0x100;
		if(pAviEncAudPara->channel_no == 1) {
			sample_per_block = 2 * package_size - 7;
		} else if(pAviEncAudPara->channel_no == 2) {
			sample_per_block = package_size - 7;
		} else {
			sample_per_block = 1;
		} 
		avi_wave_info.wFormatTag = WAVE_FORMAT_IMA_ADPCM;
		avi_wave_info.nChannels = pAviEncAudPara->channel_no;	
		avi_wave_info.nSamplesPerSec =  pAviEncAudPara->audio_sample_rate;
		avi_wave_info.nAvgBytesPerSec =  package_size * pAviEncAudPara->audio_sample_rate / sample_per_block;
		avi_wave_info.nBlockAlign = package_size;	//PackageSize
		avi_wave_info.wBitsPerSample = 4; //4bit
		avi_wave_info.cbSize = 2;
		// extension ...
		avi_wave_info.ExtInfo[0] = sample_per_block;
		break;
		
	default:
		pAviEncPacker->wave_info_cblen = 0;
		pAviEncPacker->p_avi_aud_stream_header = NULL; 
		pAviEncPacker->p_avi_wave_info = NULL;
	}
	
	avi_aud_stream_header.dwScale = avi_wave_info.nBlockAlign;
	avi_aud_stream_header.dwRate = avi_wave_info.nAvgBytesPerSec;
	avi_aud_stream_header.dwSampleSize = avi_wave_info.nBlockAlign;
	
	//video
	avi_vid_stream_header.fccType[0] = 'v';
	avi_vid_stream_header.fccType[1] = 'i';
	avi_vid_stream_header.fccType[2] = 'd';
	avi_vid_stream_header.fccType[3] = 's';
	avi_vid_stream_header.dwScale = pAviEncVidPara->dwScale;
	avi_vid_stream_header.dwRate = pAviEncVidPara->dwRate;
	avi_vid_stream_header.rcFrame.right = pAviEncVidPara->encode_width;
	avi_vid_stream_header.rcFrame.bottom = pAviEncVidPara->encode_height;
	switch(pAviEncVidPara->video_format)
	{
	case C_MJPG_FORMAT:
		avi_vid_stream_header.fccHandler[0] = 'm';
		avi_vid_stream_header.fccHandler[1] = 'j';
		avi_vid_stream_header.fccHandler[2] = 'p';
		avi_vid_stream_header.fccHandler[3] = 'g';
		
		avi_bitmap_info.biSize = pAviEncPacker->bitmap_info_cblen = 40;
		avi_bitmap_info.biWidth = pAviEncVidPara->encode_width;
		avi_bitmap_info.biHeight = pAviEncVidPara->encode_height;
		avi_bitmap_info.biBitCount = 24;
		avi_bitmap_info.biCompression[0] = 'M';
		avi_bitmap_info.biCompression[1] = 'J';
		avi_bitmap_info.biCompression[2] = 'P';
		avi_bitmap_info.biCompression[3] = 'G';
		avi_bitmap_info.biSizeImage = pAviEncVidPara->encode_width * pAviEncVidPara->encode_height * 3;
		break;
		
	case C_XVID_FORMAT:
		avi_vid_stream_header.fccHandler[0] = 'x';
		avi_vid_stream_header.fccHandler[1] = 'v';
		avi_vid_stream_header.fccHandler[2] = 'i';
		avi_vid_stream_header.fccHandler[3] = 'd';
	
		avi_bitmap_info.biSize = pAviEncPacker->bitmap_info_cblen = 68;
		avi_bitmap_info.biWidth = pAviEncVidPara->encode_width;
		avi_bitmap_info.biHeight = pAviEncVidPara->encode_height;
		avi_bitmap_info.biBitCount = 24;
		avi_bitmap_info.biCompression[0] = 'X';
		avi_bitmap_info.biCompression[1] = 'V';
		avi_bitmap_info.biCompression[2] = 'I';
		avi_bitmap_info.biCompression[3] = 'D';
		avi_bitmap_info.biSizeImage = pAviEncVidPara->encode_width * pAviEncVidPara->encode_height * 3;
		break; 
	} 
	
	return STATUS_OK;
}

void avi_encode_set_curworkmem(void *pAviEncPacker)
{
	 pAviEncPara->AviPackerCur = pAviEncPacker;
}

// status
void avi_encode_set_status(INT32U bit)
{
	pAviEncPara->avi_encode_status |= bit;
}  

void avi_encode_clear_status(INT32U bit)
{
	pAviEncPara->avi_encode_status &= ~bit;
}  

INT32S avi_encode_get_status(void)
{
    return pAviEncPara->avi_encode_status;
}

//memory
INT32U avi_encode_post_empty(OS_EVENT *event, INT32U frame_addr)
{
	INT8U err;
	
	err = OSQPost(event, (void *) frame_addr);
	return err;
}

INT32U avi_encode_get_empty(OS_EVENT *event)
{
	INT8U err;
	INT32U frame_addr;
	
	frame_addr = (INT32U) OSQAccept(event, &err);
	if(err != OS_NO_ERR || frame_addr == 0) {
    	return 0; 
    }	 

	return frame_addr;
}

INT32U avi_encode_get_csi_frame(void)
{
	INT32U addr;
	
	addr =  pAviEncVidPara->csi_frame_addr[g_csi_index++];
	if(g_csi_index >= AVI_ENCODE_CSI_BUFFER_NO) {
		g_csi_index = 0;  
	}
	return addr;
}

static INT32S sensor_mem_alloc(void)
{
	INT32U buffer_addr;
	INT32S i, buffer_size, nRet;
	
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE
	buffer_size = pAviEncVidPara->sensor_capture_width * pAviEncVidPara->sensor_capture_height << 1;
#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
	buffer_size = pAviEncVidPara->sensor_capture_width * SENSOR_FIFO_LINE << 1;
#endif
	buffer_addr = (INT32U) gp_malloc_align(buffer_size * AVI_ENCODE_CSI_BUFFER_NO, 32);
	if(buffer_addr == 0) {
		RETURN(STATUS_FAIL);	
	}
	
	for(i=0; i<AVI_ENCODE_CSI_BUFFER_NO; i++) {
		pAviEncVidPara->csi_frame_addr[i] = buffer_addr + buffer_size * i;
		DEBUG_MSG(DBG_PRINT("sensor_frame_addr[%d] = 0x%x\r\n", i, pAviEncVidPara->csi_frame_addr[i]));
	}	
	nRet = STATUS_OK;	
Return:
	return nRet;
}

static INT32S scaler_mem_alloc(void)
{
	INT32U buffer_addr;
	INT32S i, buffer_size, nRet;
	
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE
	if((AVI_ENCODE_DIGITAL_ZOOM_EN == 1) || pAviEncVidPara->scaler_flag) {
		buffer_size = pAviEncVidPara->encode_width * pAviEncVidPara->encode_height << 1;
		buffer_addr = (INT32U) gp_malloc_align(buffer_size * AVI_ENCODE_SCALER_BUFFER_NO, 32);
		if(buffer_addr == 0) {
			RETURN(STATUS_FAIL);	
		}
		
		for(i=0; i<AVI_ENCODE_SCALER_BUFFER_NO; i++) {
			pAviEncVidPara->scaler_output_addr[i] = (INT32U)buffer_addr + buffer_size * i;
			DEBUG_MSG(DBG_PRINT("scaler_frame_addr[%d] = 0x%x\r\n", i, pAviEncVidPara->scaler_output_addr[i]));
		}
	}
#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
	if(pAviEncVidPara->scaler_flag) {
		nRet = pAviEncVidPara->encode_height /(pAviEncVidPara->sensor_capture_height / SENSOR_FIFO_LINE);
		nRet += 2; //+2 is to fix block line
		buffer_size = pAviEncVidPara->encode_width * nRet << 1; 
		buffer_addr = (INT32U) gp_malloc_align(buffer_size * AVI_ENCODE_SCALER_BUFFER_NO, 32);
		if(buffer_addr == 0) {
			RETURN(STATUS_FAIL);
		}
		
		for(i=0; i<AVI_ENCODE_SCALER_BUFFER_NO; i++) {
			pAviEncVidPara->scaler_output_addr[i] = buffer_addr + buffer_size * i;
			DEBUG_MSG(DBG_PRINT("scaler_frame_addr[%d] = 0x%x\r\n", i, pAviEncVidPara->scaler_output_addr[i]));
		}
	}
#endif
	nRet = STATUS_OK;
Return:
	return nRet;
}

static INT32S display_mem_alloc(void)
{
#if AVI_ENCODE_PREVIEW_DISPLAY_EN == 1
	INT32U buffer_addr;
	INT32S i, buffer_size, nRet;
	
	if(pAviEncVidPara->dispaly_scaler_flag) {
		buffer_size = pAviEncVidPara->display_buffer_width * pAviEncVidPara->display_buffer_height << 1;
		buffer_addr = (INT32U) gp_malloc_align(buffer_size * AVI_ENCODE_DISPALY_BUFFER_NO, 32);
		if(buffer_addr == 0) {
			RETURN(STATUS_FAIL);
		}
		
		for(i=0; i<AVI_ENCODE_DISPALY_BUFFER_NO; i++) {
			pAviEncVidPara->display_output_addr[i] = buffer_addr + buffer_size * i;
			DEBUG_MSG(DBG_PRINT("display_frame_addr[%d]= 0x%x\r\n", i, pAviEncVidPara->display_output_addr[i]));
		}
	}	

	nRet = STATUS_OK;
Return:
	return nRet;
#else
	return STATUS_OK;
#endif
}

static INT32S video_mem_alloc(void)
{
#if AVI_ENCODE_VIDEO_ENCODE_EN == 1
	INT32U buffer_addr;
	INT32S i, buffer_size, nRet;
	
	buffer_size = pAviEncVidPara->encode_width * pAviEncVidPara->encode_height;
	buffer_addr = (INT32U) gp_malloc_align(buffer_size * AVI_ENCODE_VIDEO_BUFFER_NO, 32);	
	if(buffer_addr == 0) {
		RETURN(STATUS_FAIL);
	}
	
	for(i=0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {
		pAviEncVidPara->video_encode_addr[i] = buffer_addr + buffer_size * i;
		DEBUG_MSG(DBG_PRINT("jpeg_frame_addr[%d]   = 0x%x\r\n", i, pAviEncVidPara->video_encode_addr[i]));
	}	
	
	nRet = STATUS_OK;
Return:
	return nRet;
#else
	return STATUS_OK;
#endif
} 

static INT32S AviPacker_mem_alloc(AviEncPacker_t *pAviEncPacker)
{
#if AVI_ENCODE_VIDEO_ENCODE_EN == 1		
	INT32S nRet;
	INT32U buffer_addr, buffer_size;
	
	pAviEncPacker->file_buffer_size = FileWriteBuffer_Size;
	pAviEncPacker->index_buffer_size = IndexBuffer_Size;
	buffer_size = AviPackerV3_GetWorkMemSize() + FileWriteBuffer_Size + IndexBuffer_Size;
	buffer_addr = (INT32U)gp_malloc_align(buffer_size, 32);
	if(buffer_addr == 0) {
		 RETURN(STATUS_FAIL);
	}
	
	pAviEncPacker->file_write_buffer = (void *)buffer_addr;
	pAviEncPacker->index_write_buffer = (void *)(buffer_addr + FileWriteBuffer_Size);
	pAviEncPacker->avi_workmem = (void *)(buffer_addr + FileWriteBuffer_Size + IndexBuffer_Size);

	DEBUG_MSG(DBG_PRINT("file_write_buffer = 0x%x\r\n", pAviEncPacker->file_write_buffer));
	DEBUG_MSG(DBG_PRINT("index_write_buffer= 0x%x\r\n", pAviEncPacker->index_write_buffer));
	DEBUG_MSG(DBG_PRINT("avi_workmem_buffer= 0x%x\r\n", pAviEncPacker->avi_workmem));

	nRet = STATUS_OK;
Return:
	return nRet;
#else
	return STATUS_OK;
#endif
} 

static void sensor_mem_free(void) 
{
	INT32S i;
	
	if(pAviEncVidPara->csi_frame_addr[0]) {
		gp_free((void*)pAviEncVidPara->csi_frame_addr[0]);
	}
	
	for(i=0; i<AVI_ENCODE_CSI_BUFFER_NO; i++) {
		pAviEncVidPara->csi_frame_addr[i] = 0;
	}
}

static void scaler_mem_free(void) 
{
	INT32S i;
	
	if(pAviEncVidPara->scaler_output_addr[0]) {
		gp_free((void*)pAviEncVidPara->scaler_output_addr[0]);
	}
	
	for(i=0; i<AVI_ENCODE_SCALER_BUFFER_NO; i++) {
		pAviEncVidPara->scaler_output_addr[i] = 0;
	}
}

static void display_mem_free(void) 
{
	INT32S i;
	
	if(pAviEncVidPara->display_output_addr[0]) {
		gp_free((void*)pAviEncVidPara->display_output_addr[0]);	
	}
	
	for(i=0; i<AVI_ENCODE_DISPALY_BUFFER_NO; i++) {
		pAviEncVidPara->display_output_addr[i] = 0;
	}
}

static void video_mem_free(void) 
{
	INT32S i;
	
	if(pAviEncVidPara->video_encode_addr[0]) {
		gp_free((void*)pAviEncVidPara->video_encode_addr[0]);
	}
	
	for(i=0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {
		pAviEncVidPara->video_encode_addr[i] = 0;
	}
}

static void AviPacker_mem_free(AviEncPacker_t *pAviEncPacker)
{
	
	if(pAviEncPacker->file_write_buffer) {
		gp_free((void*)pAviEncPacker->file_write_buffer);
	}
			
	pAviEncPacker->file_write_buffer = 0;
	pAviEncPacker->index_write_buffer = 0;
	pAviEncPacker->avi_workmem = 0;
}

INT32S avi_encode_memory_alloc(void)
{
	INT32S nRet;
	
	//inti index
	g_csi_index = 0;
	g_pcm_index = 0;
	if(sensor_mem_alloc() < 0) {
		RETURN(STATUS_FAIL);
	}
	
	if(scaler_mem_alloc() < 0) {
		RETURN(STATUS_FAIL); 
	}
	
	if(display_mem_alloc() < 0) {
		RETURN(STATUS_FAIL);
	}
	     
	if(video_mem_alloc() < 0) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S avi_packer_memory_alloc(void)
{
	INT32S nRet;
	
	if(AviPacker_mem_alloc(pAviEncPacker0) < 0) {
		RETURN(STATUS_FAIL);
	}
#if AVI_ENCODE_FAST_SWITCH_EN == 1	
	if(AviPacker_mem_alloc(pAviEncPacker1) < 0) {
		RETURN(STATUS_FAIL);
	}
#endif
	nRet = STATUS_OK;
Return:	
	return nRet;
}

void avi_encode_memory_free(void)
{
	sensor_mem_free();
	scaler_mem_free();
	display_mem_free();
	video_mem_free();
}

void avi_packer_memory_free(void)
{
	AviPacker_mem_free(pAviEncPacker0);
#if AVI_ENCODE_FAST_SWITCH_EN == 1		
	AviPacker_mem_free(pAviEncPacker1);
#endif
}

#if MPEG4_ENCODE_ENABLE == 1
INT32S avi_encode_mpeg4_malloc(INT16U width, INT16U height)
{
	INT32S size, nRet;
	
	size = 16*width*2*3/4;
	pAviEncPara->isram_addr = (INT32U)gp_iram_malloc_align(size, 32);
	if(!pAviEncPara->isram_addr) {
		RETURN(STATUS_FAIL);
	}
	
	size = width*height*2;
	pAviEncPara->write_refer_addr = (INT32U)gp_malloc_align(size, 32);
	if(!pAviEncPara->write_refer_addr) {
		RETURN(STATUS_FAIL);
	}
	
	size = width*height*2;	
	pAviEncPara->reference_addr = (INT32U)gp_malloc_align(size, 32);
	if(!pAviEncPara->reference_addr) {
		RETURN(STATUS_FAIL);		
	}
	nRet = STATUS_OK;
Return:		
	return nRet;
}

void avi_encode_mpeg4_free(void)
{
	gp_free((void*)pAviEncPara->isram_addr);
	pAviEncPara->isram_addr = 0;
	
	gp_free((void*)pAviEncPara->write_refer_addr);
	pAviEncPara->write_refer_addr = 0;
	
	gp_free((void*)pAviEncPara->reference_addr);
	pAviEncPara->reference_addr = 0;
}
#endif //MPEG4_ENCODE_ENABLE

// format
void avi_encode_set_sensor_format(INT32U format)
{
	if(format == BITMAP_YUYV) {
    	pAviEncVidPara->sensor_output_format = C_SCALER_CTRL_IN_YUYV;
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)    	
    } else if(format ==  BITMAP_RGRB) {	
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_IN_RGBG;
#endif    
    } else {
    	pAviEncVidPara->sensor_output_format = C_SCALER_CTRL_IN_YUYV;
	}
} 

void avi_encode_set_display_format(INT32U format)
{
	if(format == IMAGE_OUTPUT_FORMAT_RGB565) {
		pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_RGB565;
	} else if(format == IMAGE_OUTPUT_FORMAT_YUYV) {
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_YUYV;
    } else if(format == IMAGE_OUTPUT_FORMAT_UYVY) {
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_UYVY;
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
    } else if(format == IMAGE_OUTPUT_FORMAT_RGBG) {
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_RGBG;
    } else if(format == IMAGE_OUTPUT_FORMAT_GRGB) {
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_GRGB;
#endif	
    } else {
    	pAviEncVidPara->display_output_format = C_SCALER_CTRL_OUT_YUYV;	
	} 
}

void avi_encode_set_display_scaler(void)
{
	if((pAviEncVidPara->encode_width != pAviEncVidPara->display_buffer_width)||
		(pAviEncVidPara->encode_height != pAviEncVidPara->display_buffer_height) ||
		(pAviEncVidPara->display_width != pAviEncVidPara->display_buffer_width) ||
		(pAviEncVidPara->display_height != pAviEncVidPara->display_buffer_height) || 
		(pAviEncVidPara->display_output_format != C_SCALER_CTRL_OUT_YUYV)) {
		pAviEncVidPara->dispaly_scaler_flag = 1;
	 } else {
		pAviEncVidPara->dispaly_scaler_flag = 0;
	}
			
	if((pAviEncVidPara->sensor_capture_width != pAviEncVidPara->encode_width) || 
		(pAviEncVidPara->sensor_capture_height != pAviEncVidPara->encode_height)) {
		pAviEncVidPara->scaler_flag = 1;
	} else {
		pAviEncVidPara->scaler_flag = 0;
	}
}

INT32S jpeg_header_generate(INT8U q, INT16U w, INT16U h)
{
	INT16U *plum, *pchr;
	INT32S i;
	
	switch(q)
	{
	case 20: 
		plum = (INT16U *)quant20_luminance_table; 
		pchr = (INT16U *)quant20_chrominance_table;
		break;
		
	case 25: 
		plum = (INT16U *)quant25_luminance_table; 
		pchr = (INT16U *)quant25_chrominance_table;
		break;
	
	case 30:
		plum = (INT16U *)quant30_luminance_table; 
		pchr = (INT16U *)quant30_chrominance_table;
		break;
		
	case 40: 
		plum = (INT16U *)quant40_luminance_table; 
		pchr = (INT16U *)quant40_chrominance_table;
		break;
	
	case 50: 
		plum = (INT16U *)quant50_luminance_table; 
		pchr = (INT16U *)quant50_chrominance_table;
		break;
	
	case 70: 
		plum = (INT16U *)quant70_luminance_table; 
		pchr = (INT16U *)quant70_chrominance_table;
		break;
	
	case 80: 
		plum = (INT16U *)quant80_luminance_table; 
		pchr = (INT16U *)quant80_chrominance_table;
		break;
	
	case 85: 
		plum = (INT16U *)quant85_luminance_table; 
		pchr = (INT16U *)quant85_chrominance_table;
		break;
	
	case 90: 
		plum = (INT16U *)quant90_luminance_table; 
		pchr = (INT16U *)quant90_chrominance_table;
		break;
	
	case 93: 
		plum = (INT16U *)quant93_luminance_table; 
		pchr = (INT16U *)quant93_chrominance_table;
		break;
	
	case 95: 
		plum = (INT16U *)quant95_luminance_table; 
		pchr = (INT16U *)quant95_chrominance_table;
		break;
	
	case 98: 
		plum = (INT16U *)quant98_luminance_table; 
		pchr = (INT16U *)quant98_chrominance_table;
		break;
	
	case 100:
		break;
	
	default: 
		q = 50;
		plum = (INT16U *)quant50_luminance_table; 
		pchr = (INT16U *)quant50_chrominance_table;
		break;
	}
	
	if(q == 100) {
		//Luminance
		for(i=0; i<64; i++) {
			jpeg_422_header[i+0x14] = 0x01; 
		}

		//Chrominance 
		for(i=0; i<64; i++) {
			jpeg_422_header[i+0x59] = 0x01;
		}
	} else {
		//Luminance
		for(i=0; i<64; i++) {
			jpeg_422_header[i+0x14] = (INT8U) (*(plum + zigzag_scan[i]) & 0xFF); 
		}

		//Chrominance 
		for(i=0; i<64; i++) {
			jpeg_422_header[i+0x59] = (INT8U) (*(pchr + zigzag_scan[i]) & 0xFF); 
		}
	}
	
	jpeg_422_header[0x9E] = h >> 8;
	jpeg_422_header[0x9F] = h & 0xFF;
	jpeg_422_header[0xA0] = w >> 8;
	jpeg_422_header[0xA1] = w & 0xFF; 
	
	return q;
}

INT32S avi_encode_set_jpeg_quality(INT8U quality_value)
{
	INT32U i, video_frame;
	
	pAviEncVidPara->quality_value = jpeg_header_generate(quality_value, 
														pAviEncVidPara->encode_width, 
														pAviEncVidPara->encode_height);
    //copy to video buffer
    for(i = 0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {	
    	video_frame = pAviEncVidPara->video_encode_addr[i];
    	gp_memcpy((INT8S*)video_frame, (INT8S*)jpeg_422_header, sizeof(jpeg_422_header));
    }
    
	return sizeof(jpeg_422_header);
}

INT32S avi_encode_set_mp4_resolution(INT16U encode_width, INT16U encode_height)
{
	INT8U *src;
	INT32U i, header_size, video_frame;
	
	if(encode_width == 640 && encode_height == 480) {
		src = mpeg4_header_vga;
 	} else if(encode_width == 320 && encode_height == 240) {
 		src = mpeg4_header_qvga;
 	} else if(encode_width == 160 && encode_height == 120) {
 		src = mpeg4_header_qqvga;
 	} else if(encode_width == 352 && encode_height == 288) {
 		src = mpeg4_header_cif;
 	} else if(encode_width == 176 && encode_height == 144) {
 		src = mpeg4_header_qcif;
 	} else if(encode_width == 128 && encode_height == 96) {
 		src = mpeg4_header_sqcif;
 	} else {
 		DEBUG_MSG(DBG_PRINT("mpeg4 header fail!!!\r\n"));
 		return STATUS_FAIL;	
 	}
 	
	header_size = sizeof(mpeg4_header_vga);
	for(i = 0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {	
	   	video_frame = pAviEncVidPara->video_encode_addr[i];
	  	gp_memcpy((INT8S*)video_frame, (INT8S*)src, header_size);
	}
	return header_size;
}

BOOLEAN avi_encode_is_pause(void)
{
	INT32U status;
	status = pAviEncPara->avi_encode_status & C_AVI_ENCODE_PAUSE;
	if(!status) { 
		return FALSE;
	}
	return TRUE;
}

// check disk free size
BOOLEAN avi_encode_disk_size_is_enough(INT32S cb_write_size)
{
#if AVI_ENCODE_CAL_DISK_SIZE_EN	
	INT32U temp;
	INT32S nRet;
	INT64U disk_free_size;
	
	temp = pAviEncPara->record_total_size;
	disk_free_size = pAviEncPara->disk_free_size;
	temp += cb_write_size;
	if(temp >= AVI_FILE_MAX_RECORD_SIZE) {
		RETURN(FALSE);
	}
	
	if(temp >= disk_free_size) {
		RETURN(FALSE);
	}
	pAviEncPara->record_total_size = temp;
#endif

	nRet = TRUE;
Return:
	return nRet;
}
// csi frame mode switch buffer
void avi_encode_switch_csi_frame_buffer(void)
{
    INT8U err;
	INT32U frame, ready_frame;
	
    frame = (INT32U) OSQAccept(cmos_frame_q, &err);
    if((err == OS_NO_ERR) && frame) {
    	ready_frame = *P_CSI_TG_FBSADDR;
    	*P_CSI_TG_FBSADDR = frame;
	    OSQPost(scaler_task_q, (void *)ready_frame);
    } else {
		DEBUG_MSG(DBG_PRINT("L"));	
	}    	
}
// csi fifo mode switch buffer
void vid_enc_csi_fifo_end(void)
{
	INT8U err;
	INT32U ready_frame;
	
	//R_IOC_O_DATA ^= 0x04;
	pAviEncPara->fifo_irq_cnt++;
	if((pAviEncPara->fifo_irq_cnt - pAviEncPara->vid_pend_cnt) >= 2) {
		pAviEncPara->fifo_enc_err_flag = 1;
		return;
	}

	if(((pAviEncPara->avi_encode_status & C_AVI_ENCODE_PRE_START) == 0) || pAviEncPara->fifo_enc_err_flag) {
		return;
	}

	ready_frame = pAviEncVidPara->csi_fifo_addr[pAviEncPara->fifo_irq_cnt - 1];
	if(pAviEncPara->fifo_irq_cnt >= pAviEncPara->vid_post_cnt) {
		ready_frame |= C_AVI_ENCODE_FRAME_END;
	}
	
	err = OSQPost(vid_enc_task_q, (void *)ready_frame);	
	if(err != OS_NO_ERR) {
		DEBUG_MSG(DBG_PRINT("L"));
		pAviEncPara->fifo_enc_err_flag = 1;
		return;
	}
}
// csi fifo mode frame end, take time: post 30 times 80us
void vid_enc_csi_frame_end(void)
{
	INT8U i;
	
	if(pAviEncPara->fifo_irq_cnt != pAviEncPara->vid_post_cnt) {
		DEBUG_MSG(DBG_PRINT("[%x]\r\n", pAviEncPara->fifo_irq_cnt));
		pAviEncPara->fifo_enc_err_flag = 1;
	}
			
	if(pAviEncPara->vid_pend_cnt && pAviEncPara->fifo_enc_err_flag) {
		OSQFlush(vid_enc_task_q);
		OSQPost(vid_enc_task_q, (void *)C_AVI_ENCODE_FIFO_ERR);
	}

	pAviEncPara->fifo_enc_err_flag = 0;
	pAviEncPara->fifo_irq_cnt = 0;
	for(i = 0; i<pAviEncPara->vid_post_cnt; i++) {
		pAviEncVidPara->csi_fifo_addr[i] = avi_encode_get_csi_frame();		
	}
}

INT32S scaler_clip(INT8U wait_done, gpImage *src, gpImage *dst, gpRect *clip)
{	
	// IMAGE Src, Dst;
	INT32S src_width, dst_width;
	INT32S in_format, out_format;
	ScalerFormat_t scale;
	
	switch(src->format)
	{
	case IMG_FMT_GRAY:
		src_width = src->widthStep >> 1 << 1; //align2
		in_format = C_SCALER_CTRL_IN_Y_ONLY;
		break;
	case IMG_FMT_YUYV:
	case IMG_FMT_YCbYCr:
		src_width = src->widthStep >> 2 << 1; //align2
		in_format = C_SCALER_CTRL_IN_UYVY;
		break;
	case IMG_FMT_UYVY:
	case IMG_FMT_CbYCrY:
		src_width = src->widthStep >> 2 << 1; //align2
		in_format = C_SCALER_CTRL_IN_YUYV;
		break;
	default:
        return STATUS_FAIL;
	}
		
	switch(dst->format)
	{
	case IMG_FMT_GRAY:
		dst_width = dst->widthStep >> 1 << 1; //align2
		out_format = C_SCALER_CTRL_OUT_Y_ONLY;
		break;
	case IMG_FMT_YUYV:
	case IMG_FMT_YCbYCr:
		dst_width = dst->widthStep >> 4 << 3; //align8
		out_format = C_SCALER_CTRL_OUT_UYVY;
		break;
	case IMG_FMT_UYVY:
	case IMG_FMT_CbYCrY:
		dst_width = dst->widthStep >> 4 << 3; //align8
		out_format = C_SCALER_CTRL_OUT_YUYV;
		break;
	default:
		return STATUS_FAIL;
	}
		
	scale.input_format = in_format;
	scale.input_width = src_width;
	scale.input_height = src->height;
	scale.input_visible_width = clip->x + clip->width;
	scale.input_visible_height = clip->y + clip->height;
	scale.input_x_offset = clip->x;
	scale.input_y_offset = clip->y;
	
	scale.input_addr_y = (INT32U)src->ptr;
	scale.input_addr_u = 0;
	scale.input_addr_v = 0;
	
	scale.output_format = out_format;
	scale.output_width = dst_width;
	scale.output_height = dst->height;
	scale.output_buf_width = dst_width;
	scale.output_buf_height = dst->height;
	scale.output_x_offset = 0;
	
	scale.output_addr_y = (INT32U)dst->ptr;
	scale.output_addr_u = 0;
	scale.output_addr_v = 0;
		
	scale.fifo_mode = C_SCALER_CTRL_FIFO_DISABLE;
	scale.scale_mode = C_SCALER_FULL_SCREEN_BY_RATIO;
	scale.digizoom_m = 10;
	scale.digizoom_n = 10;
		
	return scaler_trigger(wait_done, &scale, 0x008080);
}

INT32S scaler_once(INT8U wait_done, gpImage *src, gpImage *dst)
{
	// IMAGE Src, Dst;
	INT32S src_width, dst_width;
	INT32S in_format, out_format;
	ScalerFormat_t scale;
	
	switch(src->format)
	{
	case IMG_FMT_GRAY:
		src_width = src->widthStep >> 1 << 1; //align2
		in_format = C_SCALER_CTRL_IN_Y_ONLY;
		break;
	case IMG_FMT_YUYV:
	case IMG_FMT_YCbYCr:
		src_width = src->widthStep >> 2 << 1; //align2
		in_format = C_SCALER_CTRL_IN_UYVY;
		break;
	case IMG_FMT_UYVY:
	case IMG_FMT_CbYCrY:
		src_width = src->widthStep >> 2 << 1; //align2
		in_format = C_SCALER_CTRL_IN_YUYV;
		break;
	default:
        return STATUS_FAIL;
	}
		
	switch(dst->format)
	{
	case IMG_FMT_GRAY:
		dst_width = dst->widthStep >> 1 << 1; //align2
		out_format = C_SCALER_CTRL_OUT_Y_ONLY;
		break;
	case IMG_FMT_YUYV:
	case IMG_FMT_YCbYCr:
		dst_width = dst->widthStep >> 4 << 3; //align8
		out_format = C_SCALER_CTRL_OUT_UYVY;
		break;
	case IMG_FMT_UYVY:
	case IMG_FMT_CbYCrY:
		dst_width = dst->widthStep >> 4 << 3; //align8
		out_format = C_SCALER_CTRL_OUT_YUYV;
		break;
	default:
		return STATUS_FAIL;
	}
		
	scale.input_format = in_format;
	scale.input_width = src_width;
	scale.input_height = src->height;
	scale.input_visible_width = src->width;
	scale.input_visible_height = src->height;
	scale.input_x_offset = 0;
	scale.input_y_offset = 0;
	
	scale.input_addr_y = (INT32U)src->ptr;
	scale.input_addr_u = 0;
	scale.input_addr_v = 0;
	
	scale.output_format = out_format;
	scale.output_width = dst_width;
	scale.output_height = dst->height;
	scale.output_buf_width = dst_width;
	scale.output_buf_height = dst->height;
	scale.output_x_offset = 0;
	
	scale.output_addr_y = (INT32U)dst->ptr;
	scale.output_addr_u = 0;
	scale.output_addr_v = 0;
		
	scale.fifo_mode = C_SCALER_CTRL_FIFO_DISABLE;
	scale.scale_mode = C_SCALER_FULL_SCREEN_BY_RATIO;
	scale.digizoom_m = 10;
	scale.digizoom_n = 10;
		
	return scaler_trigger(wait_done, &scale, 0x008080);
}

INT32S scaler_wait_done(void)
{
	INT32S scaler_status;
	
	while(1) {	
		scaler_status = scaler_wait_idle();
		if(scaler_status == C_SCALER_STATUS_STOP) { 
			scaler_start();
		} else if(scaler_status & C_SCALER_STATUS_DONE) {
			scaler_stop();
			break;
		} else if (scaler_status & C_SCALER_STATUS_TIMEOUT) {
			DEBUG_MSG(DBG_PRINT("Scaler Timeout failed to finish its job\r\n"));
		} else if(scaler_status & C_SCALER_STATUS_INIT_ERR) {
			DEBUG_MSG(DBG_PRINT("Scaler INIT ERR failed to finish its job\r\n"));
		} else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) {
  			scaler_restart();
  		} else {
	  		DEBUG_MSG(DBG_PRINT("Un-handled Scaler status!\r\n"));
	  	}
 	}
 	
 	scaler_unlock();
	return scaler_status;
}

INT32S scaler_trigger(INT8U wait_done, ScalerFormat_t *pScale, INT32U boundary_color)
{
	INT8U mode = 0;
	INT32S ret;
	INT32U tempx, tempy;
	
	/* scaler lock */
	scaler_lock();	
	
	if(pScale->input_visible_width > pScale->input_width) {
		pScale->input_visible_width = 0;
	}
	
	if(pScale->input_visible_height > pScale->input_height) {
		pScale->input_visible_height = 0;
	}
	
	if(pScale->output_width > pScale->output_buf_width) {
		pScale->output_width = pScale->output_buf_width;
	}
	
	if(pScale->output_height > pScale->output_buf_height) {
		pScale->output_height = pScale->output_buf_height;
	}
	
	/* start scaling */
  	scaler_init();
	switch(pScale->scale_mode) 
	{
	case C_SCALER_FULL_SCREEN:	
		ret = scaler_image_pixels_set(pScale->input_width, pScale->input_height, pScale->output_buf_width, pScale->output_buf_height);	
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_image_pixels_set Fail!\r\n"));
			goto __exit;
		}
		break;
		
	case C_SCALER_BY_RATIO:	
		mode = 1;
	case C_SCALER_FULL_SCREEN_BY_RATIO:
		ret = scaler_input_pixels_set(pScale->input_width, pScale->input_height);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_pixels_set Fail!\r\n"));
			goto __exit;
		}
	
		ret = scaler_input_visible_pixels_set(pScale->input_visible_width, pScale->input_visible_height);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_visible_pixels_set Fail!\r\n"));
			goto __exit;
		}
		
		if(pScale->input_visible_width) {
			tempx = pScale->input_visible_width;
		} else {
			tempx = pScale->input_width;
		}
		
		if(pScale->input_visible_height) {
			tempy = pScale->input_visible_height;
	    } else {
			tempy = pScale->input_height;
	    }
		
		if(pScale->input_x_offset > tempx) {
			pScale->input_x_offset = 0;
		}
		
		if(pScale->input_y_offset > tempy) {
			pScale->input_y_offset = 0;
		}
		
		if(pScale->input_x_offset) {
			tempx -= pScale->input_x_offset;
		}
		
		if(pScale->input_y_offset) {
			tempy -= pScale->input_y_offset;
		}
		
		if(mode) {
			/* scale by ratio */
			tempx = (tempx << 16) / pScale->output_width;
			tempy = (tempy << 16) / pScale->output_height;
		} else {
			/* scale full screen by ratio */
			if (pScale->output_buf_height*tempx > pScale->output_buf_width*tempy) {
		      	tempx = tempy = (tempx << 16) / pScale->output_buf_width;
		      	pScale->output_height = (pScale->output_buf_height << 16) / tempx;
		    } else {
		      	tempx = tempy = (tempy << 16) / pScale->output_buf_height;
		      	pScale->output_width = (pScale->output_buf_width << 16) / tempy;
			}
		}
		
		ret = scaler_output_pixels_set(tempx, tempy, pScale->output_buf_width, pScale->output_buf_height);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_output_pixels_set Fail!\r\n"));
			goto __exit;
		}
	
		tempx = pScale->input_x_offset << 16;
		tempy = pScale->input_y_offset << 16;
	#if SCALE_INTP_EN == 1	
		tempx += 0x8000;
		tempy += 0x8000;
	#endif	
	
		ret = scaler_input_offset_set(tempx, tempy);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_offset_set Fail!\r\n"));
			goto __exit;
		}
		break;
		
	case C_SCALER_FULL_SCREEN_BY_DIGI_ZOOM:
		ret = scaler_input_pixels_set(pScale->input_width, pScale->input_height);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_pixels_set Fail!\r\n"));
			goto __exit;
		}
		
		ret = scaler_input_visible_pixels_set(pScale->input_visible_width, pScale->input_visible_height);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_visible_pixels_set Fail!\r\n"));
			goto __exit;
		}
		
		/* mutiple * 100 */
		if(pScale->digizoom_n == 0) {
			pScale->digizoom_n = 10;
		}

		if(pScale->digizoom_m == 0) {
			pScale->digizoom_m = 10;
		}
		tempx = 100 * (pScale->output_width * pScale->digizoom_m) / (pScale->input_width * pScale->digizoom_n);
		tempx = 65536 * 100 / tempx;
			
		tempx = pScale->output_width * (abs(pScale->digizoom_m - pScale->digizoom_n));
		tempx >>= 1;
		tempx =	(tempx << 16) / pScale->digizoom_n;
		tempy = pScale->output_height * (abs(pScale->digizoom_m - pScale->digizoom_n));
		tempy >>= 1;
		tempy = (tempy << 16) / pScale->digizoom_n;
		ret = scaler_input_offset_set(tempx, tempy);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_offset_set Fail!\r\n"));
			goto __exit;
		}
	default:
		while(1);
	}
	
	ret = scaler_output_offset_set(pScale->output_x_offset);
	if(ret < 0) {
		DEBUG_MSG(DBG_PRINT("scaler_output_offset_set Fail!\r\n"));
		goto __exit;
	}
	
	ret = scaler_input_addr_set(pScale->input_addr_y, pScale->input_addr_u, pScale->input_addr_v);
   	if(ret < 0) {
   		DEBUG_MSG(DBG_PRINT("scaler_input_addr_set Fail!\r\n"));
		goto __exit;
	}
   	
   	ret = scaler_output_addr_set(pScale->output_addr_y, pScale->output_addr_u, pScale->output_addr_v);
   	if(ret < 0) {
   		DEBUG_MSG(DBG_PRINT("scaler_output_addr_set Fail!\r\n"));
		goto __exit;
	}
   	
	ret = scaler_input_format_set(pScale->input_format);
	if(ret < 0) {
		DEBUG_MSG(DBG_PRINT("scaler_input_format_set Fail!\r\n"));
		goto __exit;
	}
	
	ret = scaler_output_format_set(pScale->output_format);
	if(ret < 0) {
		DEBUG_MSG(DBG_PRINT("scaler_output_format_set Fail!\r\n"));
		goto __exit;
	}
	
	switch(pScale->fifo_mode)
	{
	case C_SCALER_CTRL_FIFO_DISABLE:
		ret = scaler_input_fifo_line_set(C_SCALER_CTRL_IN_FIFO_DISABLE);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_fifo_line_set Fail!\r\n"));
			goto __exit;
		}

		ret = scaler_output_fifo_line_set(C_SCALER_CTRL_OUT_FIFO_DISABLE);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_output_fifo_line_set Fail!\r\n"));
			goto __exit;
		}
		break;	
		
	case C_SCALER_CTRL_IN_FIFO_16LINE:
	case C_SCALER_CTRL_IN_FIFO_32LINE:
	case C_SCALER_CTRL_IN_FIFO_64LINE:
	case C_SCALER_CTRL_IN_FIFO_128LINE:
	case C_SCALER_CTRL_IN_FIFO_256LINE:
		ret = scaler_input_fifo_line_set(pScale->fifo_mode);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_fifo_line_set Fail!\r\n"));
			goto __exit;
		}
		
		ret = scaler_output_fifo_line_set(C_SCALER_CTRL_OUT_FIFO_DISABLE);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_output_fifo_line_set Fail!\r\n"));
			goto __exit;
		}
		break;

	case C_SCALER_CTRL_OUT_FIFO_16LINE:
	case C_SCALER_CTRL_OUT_FIFO_32LINE:
	case C_SCALER_CTRL_OUT_FIFO_64LINE:
		ret = scaler_input_fifo_line_set(C_SCALER_CTRL_IN_FIFO_DISABLE);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_input_fifo_line_set Fail!\r\n"));
			goto __exit;
		}

		ret = scaler_output_fifo_line_set(pScale->fifo_mode);
		if(ret < 0) {
			DEBUG_MSG(DBG_PRINT("scaler_output_fifo_line_set Fail!\r\n"));
			goto __exit;
		}
		break;
		
	default:
		while(1);
	}
	
	ret = scaler_out_of_boundary_color_set(boundary_color);
__exit:	
	if(ret >= 0) {
		if(wait_done) {
			ret = scaler_wait_done();
		} else {
			ret = scaler_wait_idle();
			if(ret == C_SCALER_STATUS_STOP) { 
				scaler_start();
			} else {
				ret = -1;
				DEBUG_MSG(DBG_PRINT("ScalerStartFail!\r\n"));
			}
		}
	} 
	
	if(ret < 0) {
		scaler_unlock();	
	}

	return ret;
}

// jpeg once encode
INT32U jpeg_encode_once(JpegPara_t *pJpegEnc)
{
	INT32U 	encode_size;
	
	scaler_lock();
	
	jpeg_encode_init();
	gplib_jpeg_default_quantization_table_load(pJpegEnc->quality_value);	// Load default qunatization table(quality)
	gplib_jpeg_default_huffman_table_load();			// Load default huffman table 

	jpeg_encode_input_size_set(pJpegEnc->width, pJpegEnc->height);
	jpeg_encode_input_format_set(pJpegEnc->input_format);			// C_JPEG_FORMAT_YUYV / C_JPEG_FORMAT_YUV_SEPARATE
	jpeg_encode_yuv_sampling_mode_set(C_JPG_CTRL_YUV422);
	jpeg_encode_output_addr_set((INT32U) pJpegEnc->output_buffer);
	jpeg_encode_once_start(pJpegEnc->input_buffer_y, pJpegEnc->input_buffer_u, pJpegEnc->input_buffer_v);
	
	while(1) {
		pJpegEnc->jpeg_status = jpeg_encode_status_query(TRUE);
		if(pJpegEnc->jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
			encode_size = jpeg_encode_vlc_cnt_get();		// Get encode length
			cache_invalid_range(pJpegEnc->output_buffer, encode_size);
			break;
		} else if(pJpegEnc->jpeg_status & C_JPG_STATUS_ENCODING) {
			continue;
		} else {
			DEBUG_MSG(DBG_PRINT("JPEG encode error!\r\n"));
		}
	}
	jpeg_encode_stop();
	
	scaler_unlock();
	
	return encode_size;
}
// jpeg fifo encode
INT32S jpeg_encode_fifo_start(JpegPara_t *pJpegEnc)
{
	INT32S nRet;
	
	jpeg_encode_init();
	gplib_jpeg_default_quantization_table_load(pJpegEnc->quality_value);		// Load default qunatization table(quality=50)
	gplib_jpeg_default_huffman_table_load();			        // Load default huffman table 
	nRet = jpeg_encode_input_size_set(pJpegEnc->width, pJpegEnc->height);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = jpeg_encode_input_format_set(pJpegEnc->input_format);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = jpeg_encode_yuv_sampling_mode_set(C_JPG_CTRL_YUV422);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = jpeg_encode_output_addr_set((INT32U)pJpegEnc->output_buffer);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
										pJpegEnc->input_buffer_u, 
										pJpegEnc->input_buffer_v, 
										pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len);
#else
	nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
										pJpegEnc->input_buffer_u, 
										pJpegEnc->input_buffer_v, 
										(pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len) << 1);
#endif
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
	
	if(pJpegEnc->wait_done == 0) {
		RETURN(STATUS_OK);
	}
	
	nRet = jpeg_encode_status_query(TRUE);	//wait jpeg block encode done 
	if(nRet & (C_JPG_STATUS_TIMEOUT | C_JPG_STATUS_INIT_ERR |C_JPG_STATUS_RST_MARKER_ERR)) {
		nRet = STATUS_FAIL;
	}
Return:
	return nRet;
}

INT32S jpeg_encode_fifo_once(JpegPara_t *pJpegEnc)
{
	INT32S nRet;
	
	while(1) {
		if(pJpegEnc->wait_done == 0) {
			pJpegEnc->jpeg_status = jpeg_encode_status_query(TRUE);
		}
			 
	   	if(pJpegEnc->jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
	    	RETURN(C_JPG_STATUS_ENCODE_DONE);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
	    #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	    	nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
	    										pJpegEnc->input_buffer_u, 
	    										pJpegEnc->input_buffer_v, 
	    										pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len);
	    #else
   	    	nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
	    										pJpegEnc->input_buffer_u, 
	    										pJpegEnc->input_buffer_v, 
	    										(pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len) << 1);
	    #endif
	    	if(nRet < 0) {
	    		RETURN(nRet);
	    	}
	    	
	    	if(pJpegEnc->wait_done == 0) {
	    		RETURN(STATUS_OK);
	    	}
	    	
	    	pJpegEnc->jpeg_status = jpeg_encode_status_query(TRUE);	//wait jpeg block encode done 
	    	RETURN(pJpegEnc->jpeg_status);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_STOP) {
	        DEBUG_MSG(DBG_PRINT("\r\njpeg encode is not started!\r\n"));
	    	RETURN(C_JPG_STATUS_STOP);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_TIMEOUT) {
	        DEBUG_MSG(DBG_PRINT("\r\njpeg encode execution timeout!\r\n"));
	        RETURN(C_JPG_STATUS_TIMEOUT);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_INIT_ERR) {
	         DEBUG_MSG(DBG_PRINT("\r\njpeg encode init error!\r\n"));
	         RETURN(C_JPG_STATUS_INIT_ERR);
	    } else {
	    	DEBUG_MSG(DBG_PRINT("\r\nJPEG status error = 0x%x!\r\n", pJpegEnc->jpeg_status));
	        RETURN(STATUS_FAIL);
	    }
    }
	
Return:
	return nRet;
}

INT32S jpeg_encode_fifo_stop(JpegPara_t *pJpegEnc)
{
	INT32S nRet;
	
	if(pJpegEnc->wait_done == 0) {
		pJpegEnc->jpeg_status = jpeg_encode_status_query(TRUE);
	}
			
	while(1) {	
		if(pJpegEnc->jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
			nRet = jpeg_encode_vlc_cnt_get();	// get jpeg encode size
			RETURN(nRet);
		} else if(pJpegEnc->jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
		#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
			nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
												pJpegEnc->input_buffer_u, 
												pJpegEnc->input_buffer_v, 
												pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len);
		#else
			nRet = jpeg_encode_on_the_fly_start(pJpegEnc->input_buffer_y, 
												pJpegEnc->input_buffer_u, 
												pJpegEnc->input_buffer_v, 
												(pJpegEnc->input_y_len + pJpegEnc->input_uv_len + pJpegEnc->input_uv_len)<<1);
		#endif
			if(nRet < 0) {
				RETURN(STATUS_FAIL);
			}
			pJpegEnc->jpeg_status = jpeg_encode_status_query(TRUE);	//wait jpeg block encode done 
		} else if(pJpegEnc->jpeg_status & C_JPG_STATUS_STOP) {
	        DEBUG_MSG(DBG_PRINT("\r\njpeg encode is not started!\r\n"));
	    	RETURN(STATUS_FAIL);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_TIMEOUT) {
	        DEBUG_MSG(DBG_PRINT("\r\njpeg encode execution timeout!\r\n"));
	        RETURN(STATUS_FAIL);
	    } else if(pJpegEnc->jpeg_status & C_JPG_STATUS_INIT_ERR) {
	         DEBUG_MSG(DBG_PRINT("\r\njpeg encode init error!\r\n"));
	         RETURN(STATUS_FAIL);
	    } else {
	    	DEBUG_MSG(DBG_PRINT("\r\nJPEG status error = 0x%x!\r\n", pJpegEnc->jpeg_status));
	        RETURN(STATUS_FAIL);
	    }
	}
	
Return:
	jpeg_encode_stop();
	return nRet;
}

INT32S save_jpeg_file(INT16S fd, INT32U encode_frame, INT32U encode_size)
{
	INT32S nRet;
	
	nRet = write(fd, encode_frame, encode_size);
	if(nRet != encode_size) {
		RETURN(STATUS_FAIL);
	}
		
	nRet = close(fd);
	if(nRet < 0) {
		RETURN(STATUS_FAIL);
	}
	
	fd = -1;
	nRet = STATUS_OK;	
Return:
	return nRet;
}

//audio
INT32S avi_audio_memory_allocate(INT32U	cblen)
{
	INT16U *ptr;
	INT32S i, j, size, nRet;

	for(i=0; i<AVI_ENCODE_PCM_BUFFER_NO; i++) {
		pAviEncAudPara->pcm_input_addr[i] = (INT32U) gp_malloc_align(cblen, 4);
		if(!pAviEncAudPara->pcm_input_addr[i]) {
			RETURN(STATUS_FAIL);
		}
		
		ptr = (INT16U*)pAviEncAudPara->pcm_input_addr[i];
		for(j=0; j<(cblen/2); j++) {
			*ptr++ = 0x8000;
		}
	}
	
#if C_USB_AUDIO == CUSTOM_ON
	if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM)) {
		RETURN(STATUS_OK);
	}
#endif
	size = pAviEncAudPara->pack_size * C_WAVE_ENCODE_TIMES;
	pAviEncAudPara->pack_buffer_addr = (INT32U) gp_malloc_align(size, 4);
	if(!pAviEncAudPara->pack_buffer_addr) {
		RETURN(STATUS_FAIL);
	}
	nRet = STATUS_OK;
Return:	
	return nRet;
}

void avi_audio_memory_free(void)
{
	INT32U i;
	
	for(i=0; i<AVI_ENCODE_PCM_BUFFER_NO; i++) {
		gp_free((void *)pAviEncAudPara->pcm_input_addr[i]);
		pAviEncAudPara->pcm_input_addr[i] = 0;
	}
	
	gp_free((void *)pAviEncAudPara->pack_buffer_addr);
	pAviEncAudPara->pack_buffer_addr = 0;
}

INT32U avi_audio_get_next_buffer(void)
{
	INT32U addr;
	
	addr = pAviEncAudPara->pcm_input_addr[g_pcm_index++];
	if(g_pcm_index >= AVI_ENCODE_PCM_BUFFER_NO) {
		g_pcm_index = 0;
	}
	return addr;
}

#if MIC_INPUT_SRC == C_ADC_LINE_IN || MIC_INPUT_SRC == C_BUILDIN_MIC_IN 
INT32S avi_adc_double_buffer_put(INT16U *data,INT32U cwlen, OS_EVENT *os_q)
{
	INT32S status;

#if MIC_INPUT_SRC == C_ADC_LINE_IN
	g_avi_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	g_avi_adc_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
#endif
	g_avi_adc_dma_dbf.t_addr = (INT32U) data;
	g_avi_adc_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
	g_avi_adc_dma_dbf.count = (INT32U) cwlen;
	g_avi_adc_dma_dbf.notify = NULL;
	g_avi_adc_dma_dbf.timeout = 0;
	status = dma_transfer_with_double_buf(&g_avi_adc_dma_dbf, os_q);
	if(status < 0) {
		return status;
	}
	return STATUS_OK;
}

INT32U avi_adc_double_buffer_set(INT16U *data, INT32U cwlen)
{
	INT32S status;

#if MIC_INPUT_SRC == C_ADC_LINE_IN
	g_avi_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	g_avi_adc_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
#endif
	g_avi_adc_dma_dbf.t_addr = (INT32U) data;
	g_avi_adc_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
	g_avi_adc_dma_dbf.count = (INT32U) cwlen;
	g_avi_adc_dma_dbf.notify = NULL;
	g_avi_adc_dma_dbf.timeout = 0;
	status = dma_transfer_double_buf_set(&g_avi_adc_dma_dbf);
	if(status < 0) {
		return status;
	}
	return STATUS_OK;
}

INT32S avi_adc_dma_status_get(void)
{
	if(g_avi_adc_dma_dbf.channel == 0xff) {
		return -1;
	}
	
	return dma_status_get(g_avi_adc_dma_dbf.channel);	
}

void avi_adc_double_buffer_free(void)
{
	dma_transfer_double_buf_free(&g_avi_adc_dma_dbf);
	g_avi_adc_dma_dbf.channel = 0xff;
}

void avi_adc_hw_start(void)
{
#if MIC_INPUT_SRC == C_ADC_LINE_IN
	//set adc pin is input floating
#if AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_0
	gpio_init_io(IO_F6, GPIO_INPUT);
	gpio_set_port_attribute(IO_F6, ATTRIBUTE_HIGH);
#elif AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_1	
	gpio_init_io(IO_F7, GPIO_INPUT);
	gpio_set_port_attribute(IO_F7, ATTRIBUTE_HIGH);
#elif AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_2
	gpio_init_io(IO_F8, GPIO_INPUT);
	gpio_set_port_attribute(IO_F8, ATTRIBUTE_HIGH);
#elif AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_3
	gpio_init_io(IO_F9, GPIO_INPUT);
	gpio_set_port_attribute(IO_F9, ATTRIBUTE_HIGH);
#elif AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_4
	gpio_init_io(IO_H6, GPIO_INPUT);
	gpio_set_port_attribute(IO_H6, ATTRIBUTE_HIGH);
#elif AVI_AUDIO_RECORD_ADC_CH == ADC_LINE_5
	gpio_init_io(IO_H7, GPIO_INPUT);
	gpio_set_port_attribute(IO_H7, ATTRIBUTE_HIGH);	
#endif
	
	adc_fifo_clear();
	adc_fifo_level_set(4);
	adc_auto_ch_set(AVI_AUDIO_RECORD_ADC_CH);	
	adc_auto_sample_start();

#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	mic_fifo_clear();	
	mic_fifo_level_set(4);
	mic_agc_enable_set(1);
	mic_auto_sample_start();
#endif
}

void avi_adc_hw_stop(void)
{
#if MIC_INPUT_SRC == C_ADC_LINE_IN
	adc_auto_sample_stop();
#elif MIC_INPUT_SRC == C_BUILDIN_MIC_IN
	mic_auto_sample_stop();
#endif	
}
#endif

#if MIC_INPUT_SRC == C_GPY0050_IN
INT16U avi_adc_get_gpy0050(void)
{
	INT16U temp;
	INT32S i;
	
	R_SPI0_TX_STATUS |= 0x8020;
	R_SPI0_RX_STATUS |= 0x8020;
	
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low	
	R_SPI0_TX_DATA = C_CMD_DUMMY_COM;		//dummy command
	R_SPI0_TX_DATA = C_CMD_ADC_IN4;			//select IN4
	R_SPI0_TX_DATA = C_CMD_ZERO_COM;		// get low byte
	
	//wait busy
	for(i=0; i<5000; i++) {
		if((R_SPI0_MISC & 0x0010) == 0) {
			break;	
		}
	}
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high
		
	temp = R_SPI0_RX_DATA;// don't care
	temp = (INT16U)(R_SPI0_RX_DATA << 8);// high byte
	temp |= R_SPI0_RX_DATA & 0xFF;				 // low byte
	
	if(temp <= 0x00FF) {
		temp = g_gpy0050_pre_value;
	} else if(temp >= 0xFFC0) {
		temp = g_gpy0050_pre_value;
	} else {
		g_gpy0050_pre_value = temp;
	}
	return temp^0x8000;
}

void gpy0050_enable(void)
{
	INT32S i;
	
	gpio_init_io(C_GPY0050_SPI_CS_PIN, GPIO_OUTPUT);
	gpio_set_port_attribute(C_GPY0050_SPI_CS_PIN, ATTRIBUTE_HIGH);
	gpio_write_io(C_GPY0050_SPI_CS_PIN, DATA_HIGH);	//pull cs high
	
	//GPL32 SPI IF 1 initialization
	R_SPI0_CTRL = 0x0800;	//reset SPI0
	R_SPI0_MISC |= 0x0100;  //enable smart mode 
	R_SPI0_CTRL = 0x8035;	//Master mode 3, SPI_CLK = SYS_CLK/64
	
	//sw reset gpy0050
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low		
	R_SPI0_TX_DATA = C_CMD_RESET_IN4;		//reset IN4
	
	//wait busy
	for(i=0; i<5000; i++) {
		if((R_SPI0_MISC & 0x0010) == 0) {
			break;	
		}
	}
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high	
	
	//enable MIC AGC and ADC	
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);		//pull cs low	
	R_SPI0_TX_DATA = C_CMD_ENABLE_MIC_AGC_ADC;	//enable MIC & AGC;
	R_SPI0_TX_DATA = C_CMD_ADC_IN4;				//select IN4
	
	//wait busy
	for(i=0; i<5000; i++) {
		if((R_SPI0_MISC & 0x0010) == 0) {
			break;	
		}
	}
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high
	
	OSTimeDly(30);	//wait 300ms after AGC & MIC enable
	
	//dummy data
	avi_adc_get_gpy0050();
	avi_adc_get_gpy0050();
}

void gpy0050_disable()
{
	INT32S i;
	
	//power down 
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low		
	R_SPI0_TX_DATA = C_CMD_POWER_DOWN;		
	
	//wait busy
	for(i=0; i<5000; i++) {
		if(R_SPI0_MISC & 0x0010 == 0) {
			break;	
		}
	}
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high	
	
	//GPL32 SPI disable
	R_SPI0_CTRL = 0;
	R_SPI0_MISC = 0;
}

void gpy0050_timer_isr(void)
{
	INT16U *ptr;
	INT32U pcm_cwlen;
	
	ptr = (INT16U*)pAviEncAudPara->pcm_input_addr[g_mic_buffer];
	*(ptr + g_mic_cnt++) = avi_adc_get_gpy0050();
	
	pcm_cwlen = pAviEncAudPara->pcm_input_size * C_WAVE_ENCODE_TIMES;
	if(g_mic_cnt >= pcm_cwlen) {	
		if(g_mic_buffer) {
			g_mic_buffer = 0;
		} else {
			g_mic_buffer = 1;
		}
		g_mic_cnt = 0;
		OSQPost(avi_aud_q, (void*)AVI_AUDIO_DMA_DONE);
	}
}
#endif	// MIC_INPUT_SRC

#if	AVI_ENCODE_SHOW_TIME == 1
const INT8U *number[] = {	
	acFontHZArial01700030, 
	acFontHZArial01700031, 
	acFontHZArial01700032,
	acFontHZArial01700033, 
	acFontHZArial01700034,
	acFontHZArial01700035,
	acFontHZArial01700036,
	acFontHZArial01700037, 
	acFontHZArial01700038,
	acFontHZArial01700039 
};

//=======================================================================
//  Draw OSD function
//=======================================================================
void cpu_draw_osd(const INT8U *source_addr, INT32U target_addr, INT16U offset, INT16U res)
{	
	const INT8U* chptr;
	INT8U i,j,tmp;
	INT8U *ptr;
	
	ptr = (INT8U*) target_addr;
	ptr+= offset;
	chptr = source_addr;
	for(i=0; i<19; i++) {
		tmp = *chptr++;
		for(j=0; j<8; j++) {
			if(tmp & 0x80) {	
				*ptr++ =0x80;
				*ptr++ = 0xff;
			} else {
				ptr += 2;
			}	
			tmp = tmp<<1;
		}
 		ptr += (res-8)*2;
 	}
} 
//=======================================================================
//  Draw OSD function
//=======================================================================
void cpu_draw_time_osd(TIME_T current_time, INT32U target_buffer, INT16U resolution)
{
	INT8U  data;
	INT16U offset, space, wtemp;
	INT32U line;
	
	if(resolution == 320) {
		line = target_buffer + 220*resolution*2;//QVGA
		offset = 160*2;
	} else {
		line = target_buffer + 440*resolution*2;//VGA	
		offset = 480*2;
	}
	space = 16;
	//Arial 17
	//year
	if (current_time.tm_year > 2008) {
		wtemp = current_time.tm_year - 2000;
		cpu_draw_osd(number[2], line, offset, resolution);
		cpu_draw_osd(number[0],line,offset+space*1,resolution);
		data = wtemp/10;
		wtemp -= data*10;
		cpu_draw_osd(number[data],line,offset+space*2,resolution);
		data = wtemp;
		cpu_draw_osd(number[data],line,offset+space*3,resolution);
	} else {
		cpu_draw_osd(number[2], line, offset, resolution);
		cpu_draw_osd(number[0],line,offset+space*1,resolution);
		cpu_draw_osd(number[0],line,offset+space*2,resolution);
		cpu_draw_osd(number[8],line,offset+space*3,resolution);
	}
	
	// :
	cpu_draw_osd(acFontHZArial017Slash,line,offset+space*4,resolution);
	
	//month
	wtemp = current_time.tm_mon; 
	cpu_draw_osd(number[wtemp/10],line,offset+space*5,resolution);
	cpu_draw_osd(number[wtemp%10],line,offset+space*6,resolution);
	
	//:
	cpu_draw_osd(acFontHZArial017Slash,line,offset+space*7,resolution);
	
	//day
	wtemp = current_time.tm_mday;
	cpu_draw_osd(number[wtemp/10],line,offset+space*8,resolution);
	cpu_draw_osd(number[wtemp%10],line,offset+space*9,resolution);
	
	//hour
	wtemp = current_time.tm_hour;
	cpu_draw_osd(number[wtemp/10],line,offset+space*11,resolution);
	cpu_draw_osd(number[wtemp%10],line,offset+space*12,resolution);
	
	// :
	cpu_draw_osd(acFontHZArial017Comma, line,offset+space*13,resolution);
	
	//minute
	wtemp = current_time.tm_min;
	cpu_draw_osd(number[wtemp/10],line,offset+space*14,resolution);
	cpu_draw_osd(number[wtemp%10],line,offset+space*15,resolution);
	
	// :
	cpu_draw_osd(acFontHZArial017Comma,line,offset+space*16,resolution);
	
	//second
	wtemp = current_time.tm_sec;
	cpu_draw_osd(number[wtemp/10], line,offset+space*17,resolution);
	cpu_draw_osd(number[wtemp%10],line,offset+space*18,resolution);	
	
	//drain cache
	if(resolution == 320) {
		cache_drain_range((target_buffer + (resolution*440*2)), resolution*(480-440)*2);
	} else {
		cache_drain_range((target_buffer + (resolution*220*2)), resolution*(240-220)*2);
	}
}
#if 0
extern INT16U ad_value;
extern INT16U ad_18_value;
void cpu_draw_advalue_osd(INT16U value, INT32U target_buffer, INT16U resolution, INT16U aa)
{
	INT8U  data;
	INT16U offset, space, wtemp;
	INT32U line;
	if(resolution == 320){
		line = target_buffer + 220*resolution*2;//QVGA
		offset = 250*2;
	} else {
		line = target_buffer + aa*resolution*2;//VGA	
		offset = 480*2;
	}
	space = 16;
	//Arial 17
	//year
	wtemp = value; 
	data = wtemp/10000;
	wtemp -= data*10000;
	cpu_draw_osd(number[data], line, offset, resolution);
	data = wtemp/1000;
	wtemp -= data*1000;
	cpu_draw_osd(number[data],line,offset+space*1,resolution);
	data = wtemp/100;
	wtemp -= data*100;
	cpu_draw_osd(number[data],line,offset+space*2,resolution);
	data = wtemp/10;
	wtemp -= data*10;
	cpu_draw_osd(number[data],line,offset+space*3,resolution);
	data = wtemp;
	cpu_draw_osd(number[data],line,offset+space*4,resolution);
}
#endif
#endif
