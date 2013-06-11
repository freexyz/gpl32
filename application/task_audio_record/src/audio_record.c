/**************************************************************************************************
audio record to storage:
The encode format can be
1. A1800 (GP)
2. WAVE  (PCM)
3. WAVE  (microsoft ADPCM)
4. WAVE  (IMA ADPCM)
5. MP3	 sample rate, 48k, 44.1K, 32k, 24K, 22.05k, 16K, 12K, 11.025K, 8k  
		 bit rate, 32,40,48,56,64,80,96,112,128,160,192,224,256,320 kbps
		  			 8,16,24,32,40,48,56, 64, 80, 96,112,128,144,160 kbps
***************************************************************************************************/
#include "audio_record.h"
#include <stdlib.h>

/*  define */
#define ADC_RECORD_STACK_SIZE			1024
#define AUD_ENC_QACCEPT_MAX				5

/* os stack */
INT32U  AdcRecordStack[ADC_RECORD_STACK_SIZE];

/* os global varaible */
void	 	*aud_enc_reqQ_area[AUD_ENC_QACCEPT_MAX];
OS_EVENT 	*aud_enc_reqQ;

#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
void	 	*aud_enc_reqQ_area2[AUD_ENC_QACCEPT_MAX];
OS_EVENT 	*aud_enc_reqQ2;
#endif

/* global varaible */
//static AUD_ENC_WAVE_HEADER	WaveHeadPara;
static Audio_Encode_Para	AudioEncodePara;
static Audio_Encode_Para	*pAudio_Encode_Para;
static INT8U g_ring_buffer;
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
static INT8U g_adc_bufidx;
static INT8U g_mic_bufidx;
static INT32U g_aud_dst_buffer;
#else
static INT8U g_buffer_index;
#endif
static INT32U g_aud_pcm_buffer[C_AUD_PCM_BUFFER_NO];

#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
static DMA_STRUCT g_aud_adc_dma_dbf; 
static DMA_STRUCT g_aud_mic_dma_dbf; 
#elif AUDIO_INPUT_SRC == GPY0050_IN
static INT16U g_pre_value;
static INT32U g_buffer_cnt;
static INT32U g_ready_buffer;
#endif

INT32S (*pfn_audio_encode_stop)(void);
int    (*pfn_audio_encode_once)(void *workmem, const short* buffer_addr, int cwlen);
void   *hProc;

/* function */
//dma
#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
static INT32S aud_adc_double_buffer_put(INT16U *data,INT32U cwlen, INT8U aud_in, OS_EVENT *os_q);
static INT32U aud_adc_double_buffer_set(INT16U *data, INT32U cwlen, INT8U aud_in);
static INT32S aud_adc_dma_status_get(INT8U aud_in);
static void aud_adc_double_buffer_free(INT8U aud_in);
static void adc_hardware_start(INT32U SampleRate);
#endif

//other
//static void aud_enc_RIFF_init(INT32U samplerate);
static INT32S write_audio_data(INT32U buffer_addr, INT32U cbLen);
static INT32S adc_memory_allocate(INT32U cbLen);
static INT32S adc_memory_free(void);
static void adc_work_mem_free(void);

//wave
#if 0
static INT32S wave_encode_start(void);
static INT32S wave_encode_stop(void);
static int wave_encode_once(void *workmem, const short* buffer_addr, int cwlen);
#endif
#if APP_A1800_ENCODE_EN	
static INT32S a1800_encode_start(void);
static INT32S a1800_encode_stop(void);
static int a1800_encode_once(void *workmem, const short* buffer_addr, int cwlen);
#endif
#if APP_WAV_CODEC_EN
static INT32S wave_encode_lib_start(INT32U AudioFormat);
static INT32S wave_encode_lib_stop(void);
static int wave_encode_lib_once(void *workmem, const short* buffer_addr, int cwlen);
#endif
#if APP_MP3_ENCODE_EN
static INT32S mp3_encode_start(void);
static INT32S mp3_encode_stop(void);
static int mp3_encode_once(void *workmem, const short* buffer_addr, int cwlen);
#endif

#if AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
extern INT32S mic_timer_stop(INT8U timer_id);
extern void mic_fifo_clear(void);
extern void mic_fifo_level_set(INT8U level);
extern INT32S mic_auto_sample_start(void);
extern INT32S mic_sample_rate_set(INT8U timer_id, INT32U hz);
#elif AUDIO_INPUT_SRC == GPY0050_IN
static INT16U gpy0050_get_value(void);
static void gpy0050_start(void);
static void gpy0050_stop(void);
static void gpy0050_isr(void);
#endif
//===============================================================================================
// 	audio task create
//	parameter:	priority.
//	return:		none.
//===============================================================================================
INT32S adc_record_task_create(INT8U priority)
{
	INT8U 	err;

	aud_enc_reqQ = OSQCreate(aud_enc_reqQ_area, AUD_ENC_QACCEPT_MAX);
	if (!aud_enc_reqQ)		
		return STATUS_FAIL;

#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
	aud_enc_reqQ2 = OSQCreate(aud_enc_reqQ_area2, AUD_ENC_QACCEPT_MAX);
	if (!aud_enc_reqQ2)		
		return STATUS_FAIL;
#endif
	
	pAudio_Encode_Para = &AudioEncodePara;
	gp_memset((INT8S *)pAudio_Encode_Para, 0, sizeof(AudioEncodePara));
	
	err = OSTaskCreate(adc_record_entry, 0, (void *)&AdcRecordStack[ADC_RECORD_STACK_SIZE - 1], priority);
	DEBUG_MSG(DBG_PRINT("AudioEncodeTaskCreate = %d\r\n", err));
	
    if(err != OS_NO_ERR)
    	return STATUS_FAIL;
    else
    	return STATUS_OK;
}

//===============================================================================================
// 	audio task detete
//	parameter:	priority.
//	return:		none.
//===============================================================================================
INT32S adc_record_task_del(INT8U priority)
{
	INT8U err;
	
	OSQFlush(aud_enc_reqQ);
	err = OSTaskDel(priority);
	DEBUG_MSG(DBG_PRINT("AudioEncodeTaskDel = %d\r\n", err));
	OSQDel(aud_enc_reqQ, OS_DEL_ALWAYS, &err);

#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
	OSQFlush(aud_enc_reqQ2);
	OSQDel(aud_enc_reqQ2, OS_DEL_ALWAYS, &err);
#endif
	
	return err;
}

//===============================================================================================
// 	audio task main entry
//===============================================================================================
void adc_record_entry(void *param)
{
	INT8U   err;
	INT16U  mask, t;
	INT32U  msg_id;
	INT32S  i, nRet;
#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC
	INT16U  *ptr;
	INT32U  ready_addr, free_addr, pcm_addr;
#elif AUDIO_INPUT_SRC == DOUBLE_LINEIN
	INT16U  t2;
	INT16U  *ptr1, *ptr2, *temp;
	INT32U  adc_ready_addr, adc_free_addr, adc_pcm_addr;
	INT32U  mic_ready_addr, mic_free_addr, mic_pcm_addr;
#endif

#if 0
	INT32U L_pcm_ptr, R_pcm_ptr;
#endif

	while(1) 
	{
		msg_id = (INT32U) OSQPend(aud_enc_reqQ, 0, &err);
		if(err != OS_NO_ERR)
			continue;
		
		switch(msg_id)
		{
		case MSG_ADC_DMA_DONE:
		#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC
			pcm_addr = ready_addr;
			ready_addr = free_addr;
			g_buffer_index++;
			if(g_buffer_index >= C_AUD_PCM_BUFFER_NO)
				g_buffer_index = 0;
			free_addr = g_aud_pcm_buffer[g_buffer_index];
			
			if(pAudio_Encode_Para->Status == C_STOP_RECORDING)
			{
				//check dma is done and stop
				if(aud_adc_dma_status_get(AUDIO_INPUT_SRC) == 0)
					OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_STOP);	
			}
			else
			{	
				//set dma buffer
				aud_adc_double_buffer_set((INT16U *)free_addr, pAudio_Encode_Para->PCMInFrameSize, AUDIO_INPUT_SRC);
			}
		#elif AUDIO_INPUT_SRC == DOUBLE_LINEIN

			msg_id = (INT32U) OSQPend(aud_enc_reqQ2, 0, &err);
			if (msg_id != MSG_ADC_DMA_DONE)
			{
				DBG_PRINT("MIC DMA error !\r\n");
				continue;
			}
		
			adc_pcm_addr = adc_ready_addr;
			adc_ready_addr = adc_free_addr;
			mic_pcm_addr = mic_ready_addr;
			mic_ready_addr = mic_free_addr;
			g_adc_bufidx++;
			g_mic_bufidx++;
			if(g_adc_bufidx >= (C_AUD_PCM_BUFFER_NO>>1)) 
				g_adc_bufidx = 0;
			if(g_mic_bufidx >= C_AUD_PCM_BUFFER_NO) 
				g_mic_bufidx = (C_AUD_PCM_BUFFER_NO>>1);
			
			adc_free_addr = g_aud_pcm_buffer[g_adc_bufidx];
			mic_free_addr = g_aud_pcm_buffer[g_mic_bufidx];
			
			if(pAudio_Encode_Para->Status == C_STOP_RECORDING)
			{	
				//check dma is done and stop
				if ((aud_adc_dma_status_get(ADC_LINE_IN) == 0) &&
				     (aud_adc_dma_status_get(BUILD_IN_MIC) == 0))	
					OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_STOP);	
			}
			else
			{
				//set dma buffer
				aud_adc_double_buffer_set((INT16U *)adc_free_addr, pAudio_Encode_Para->PCMInFrameSize>>1, ADC_LINE_IN);
				aud_adc_double_buffer_set((INT16U *)mic_free_addr, pAudio_Encode_Para->PCMInFrameSize>>1, BUILD_IN_MIC);
			}
		#elif AUDIO_INPUT_SRC == GPY0050_IN
			pcm_addr = g_ready_buffer;
			if(pAudio_Encode_Para->Status == C_STOP_RECORDING)
				OSQPost(aud_enc_reqQ, (void *)MSG_AUDIO_ENCODE_STOP);
		#endif
			
			DEBUG_MSG(DBG_PRINT("."));
			//unsigned to signed
		#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
			cache_invalid_range(adc_pcm_addr, pAudio_Encode_Para->PCMInFrameSize );
			cache_invalid_range(mic_pcm_addr, pAudio_Encode_Para->PCMInFrameSize );
		#else
			cache_invalid_range(pcm_addr, pAudio_Encode_Para->PCMInFrameSize << 1);
		#endif

		#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
			ptr1 = (INT16U*)adc_pcm_addr;
			ptr2 = (INT16U*)mic_pcm_addr;
			temp = (INT16U*)g_aud_dst_buffer;
		#else
			ptr = (INT16U*)pcm_addr;
		#endif

		#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
			for (i=0; i<(pAudio_Encode_Para->PCMInFrameSize>>1); i++)
			{
					t = *ptr1;
					t2 = *ptr2;
				#if ((defined MCU_VERSION) && (MCU_VERSION < GPL327XX))	
					t ^= mask;
					t2 ^= mask;
				#endif
			
				#if APP_LPF_ENABLE == 1
					t = (INT16U)LPF_process(t);		
					t2 = (INT16U)LPF_process(t2);		
				#endif
					*ptr1 = t;	
					*ptr2 = t2;	

					*temp++ = *ptr1++;
					*temp++ = *ptr2++;	
			}

		#else
			for(i=0; i<pAudio_Encode_Para->PCMInFrameSize; i++)
			{
				
					t = *ptr;
				#if (AUDIO_INPUT_SRC == ADC_LINE_IN)  || ((defined MCU_VERSION) && (MCU_VERSION < GPL327XX))	
					t ^= mask;
				#endif
			
				#if APP_LPF_ENABLE == 1
					t = (INT16U)LPF_process(t);		
				#endif
					*ptr++ = t;	
			}
		#endif
			


#if 0
		write(L_pcm_ptr, (INT32U)adc_pcm_addr, pAudio_Encode_Para->PCMInFrameSize);
		write(R_pcm_ptr, (INT32U)mic_pcm_addr, pAudio_Encode_Para->PCMInFrameSize);
#endif
			
			//energy detect
			//pcm_energy_detect((INT16S *)pcm_addr, pAudio_Encode_Para->PCMInFrameSize);
			
			//encode pcm wave 
		#if AUDIO_INPUT_SRC == DOUBLE_LINEIN	
			nRet = pfn_audio_encode_once(hProc, (const short*)g_aud_dst_buffer, pAudio_Encode_Para->PCMInFrameSize); 	
		#else
			nRet = pfn_audio_encode_once(hProc, (const short*)pcm_addr, pAudio_Encode_Para->PCMInFrameSize); 	
		#endif	
		
			if(nRet < 0)
			{
				OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_ERR);	
				continue;
			}
			
			//check storage is full
			if(pAudio_Encode_Para->SourceType == C_GP_FS)
			{
				if(pAudio_Encode_Para->FileLenth >= pAudio_Encode_Para->disk_free_size)
					OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_STOPING);
			}
			break;
		
		case MSG_AUDIO_ENCODE_START:
			hProc = 0;
			g_ring_buffer = 0;
			mask = 0x8000;
			
#if 0			
			L_pcm_ptr = open("L_pcm.pcm", O_WRONLY|O_CREAT|O_TRUNC);
			R_pcm_ptr = open("R_pcm.pcm", O_WRONLY|O_CREAT|O_TRUNC);
#endif			
			if(pAudio_Encode_Para->AudioFormat == WAVE_FORMAT_PCM)
			{
				//nRet = wave_encode_start();
				//pfn_audio_encode_once = wave_encode_once;
				//pfn_audio_encode_stop = wave_encode_stop;
				nRet = wave_encode_lib_start(pAudio_Encode_Para->AudioFormat);
				pfn_audio_encode_once = wave_encode_lib_once;
				pfn_audio_encode_stop = wave_encode_lib_stop;
			}
		#if APP_A1800_ENCODE_EN	
			else if(pAudio_Encode_Para->AudioFormat == WAVE_FORMAT_A1800)
			{
				nRet = a1800_encode_start();
				pfn_audio_encode_once = a1800_encode_once;
				pfn_audio_encode_stop = a1800_encode_stop;
			}
		#endif				
		#if	APP_WAV_CODEC_EN		
			else if((pAudio_Encode_Para->AudioFormat == WAVE_FORMAT_IMA_ADPCM) ||
					(pAudio_Encode_Para->AudioFormat == WAVE_FORMAT_ADPCM))
			{
				nRet = wave_encode_lib_start(pAudio_Encode_Para->AudioFormat);
				pfn_audio_encode_once = wave_encode_lib_once;
				pfn_audio_encode_stop = wave_encode_lib_stop;
			}
		#endif
		#if APP_MP3_ENCODE_EN
			else if(pAudio_Encode_Para->AudioFormat == WAVE_FORMAT_MP3)
			{
				nRet = mp3_encode_start();	
				pfn_audio_encode_once = mp3_encode_once;
				pfn_audio_encode_stop = mp3_encode_stop;
			}
		#endif
			if(nRet < 0)
			{
				DEBUG_MSG(DBG_PRINT("AudioEncodeStartFail!!!\r\n"));
				adc_work_mem_free();
				pAudio_Encode_Para->Status = C_START_FAIL;
				continue;
			}
						
		#if	APP_DOWN_SAMPLE_EN
			if(pAudio_Encode_Para->bEnableDownSample)
			{	
				if(pAudio_Encode_Para->DownsampleFactor * pAudio_Encode_Para->SampleRate > 48000)
					pAudio_Encode_Para->DownsampleFactor = 48000 / pAudio_Encode_Para->SampleRate;
					
				if(pAudio_Encode_Para->DownsampleFactor >= 2 )
				{	
					pAudio_Encode_Para->DownSampleWorkMem = DownSample_Create(pAudio_Encode_Para->PCMInFrameSize, pAudio_Encode_Para->DownsampleFactor);	
					
					DownSample_Link(pAudio_Encode_Para->DownSampleWorkMem,	NULL, pfn_audio_encode_once, pAudio_Encode_Para->SampleRate,
										pAudio_Encode_Para->Channel, pAudio_Encode_Para->DownsampleFactor);
														
					hProc = pAudio_Encode_Para->DownSampleWorkMem;
					pfn_audio_encode_once = DownSample_PutData;
					
					pAudio_Encode_Para->SampleRate = DownSample_GetSampleRate(hProc);
					pAudio_Encode_Para->Channel = DownSample_GetChannel(hProc);
					pAudio_Encode_Para->PCMInFrameSize *=  pAudio_Encode_Para->DownsampleFactor; 	
				}
			}
		#endif
		#if	APP_LPF_ENABLE == 1			
			LPF_init(pAudio_Encode_Para->SampleRate, 3);
		#endif			

			DBG_PRINT("pAudio_Encode_Para->PCMInFrameSize=%d\r\n", pAudio_Encode_Para->PCMInFrameSize);
		#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC
			nRet = adc_memory_allocate(pAudio_Encode_Para->PCMInFrameSize<<1);
		#elif AUDIO_INPUT_SRC == DOUBLE_LINEIN
			nRet = adc_memory_allocate(pAudio_Encode_Para->PCMInFrameSize);
			g_aud_dst_buffer = (INT32U)gp_malloc_align(pAudio_Encode_Para->PCMInFrameSize<<1, 32);
		#endif
		
			if(nRet < 0)
			{
				DEBUG_MSG(DBG_PRINT("AudioEncodeMemoryFail!!!\r\n"));
				adc_work_mem_free();
				pAudio_Encode_Para->Status = C_START_FAIL;
				continue;
			}
		#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC
			ready_addr = g_aud_pcm_buffer[0];
			free_addr = g_aud_pcm_buffer[1];
			g_buffer_index = 1;
			aud_adc_double_buffer_put((INT16U*)ready_addr, pAudio_Encode_Para->PCMInFrameSize, AUDIO_INPUT_SRC, aud_enc_reqQ);
			aud_adc_double_buffer_set((INT16U*)free_addr, pAudio_Encode_Para->PCMInFrameSize, AUDIO_INPUT_SRC);
			adc_hardware_start(pAudio_Encode_Para->SampleRate);
			#if MCU_VERSION >= GPL326XXB
			mic_dagc_enable(1);
			#endif 
			
		#elif AUDIO_INPUT_SRC == DOUBLE_LINEIN
			adc_ready_addr = g_aud_pcm_buffer[0];
			adc_free_addr = g_aud_pcm_buffer[1];
			mic_ready_addr = g_aud_pcm_buffer[3];
			mic_free_addr = g_aud_pcm_buffer[4];
			g_adc_bufidx = 1;
			g_mic_bufidx = 4;
			aud_adc_double_buffer_put((INT16U*)adc_ready_addr, pAudio_Encode_Para->PCMInFrameSize>>1, ADC_LINE_IN, aud_enc_reqQ);
			aud_adc_double_buffer_set((INT16U*)adc_free_addr, pAudio_Encode_Para->PCMInFrameSize>>1, ADC_LINE_IN);
			aud_adc_double_buffer_put((INT16U*)mic_ready_addr, pAudio_Encode_Para->PCMInFrameSize>>1, BUILD_IN_MIC, aud_enc_reqQ2);
			aud_adc_double_buffer_set((INT16U*)mic_free_addr, pAudio_Encode_Para->PCMInFrameSize>>1, BUILD_IN_MIC);
			adc_hardware_start(pAudio_Encode_Para->SampleRate);
			#if MCU_VERSION >= GPL326XXB
			mic_dagc_enable(1);
			#endif 
		#elif AUDIO_INPUT_SRC == GPY0050_IN
			g_buffer_index = 0;
			gpy0050_start();
			timer_freq_setup(C_AUDIO_RECORD_TIMER, pAudio_Encode_Para->SampleRate, 0, gpy0050_isr);
		#endif
			pAudio_Encode_Para->Status = C_START_RECORD;
			break;
		
		case MSG_AUDIO_ENCODE_STOPING:	
			pAudio_Encode_Para->Status = C_STOP_RECORDING;
			break;
			
		case MSG_AUDIO_ENCODE_STOP:
			DEBUG_MSG(DBG_PRINT("AudioEncodeStop\r\n"));
		#if AUDIO_INPUT_SRC == ADC_LINE_IN	
			adc_timer_stop(C_ADC_USE_TIMER);
		#elif AUDIO_INPUT_SRC == BUILD_IN_MIC
			mic_timer_stop(C_ADC_USE_TIMER);
		#elif  AUDIO_INPUT_SRC == DOUBLE_LINEIN
			adc_timer_stop(C_ADC_USE_TIMER);
			mic_timer_stop(C_ADC_USE_TIMER);
		#elif AUDIO_INPUT_SRC == GPY0050_IN
			timer_stop(C_AUDIO_RECORD_TIMER);	
			gpy0050_stop();
		#endif		
			pfn_audio_encode_stop();
			OSQFlush(aud_enc_reqQ);
		#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
			OSQFlush(aud_enc_reqQ2);
		#endif
			adc_memory_free();
		#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC
			aud_adc_double_buffer_free(AUDIO_INPUT_SRC);
		#elif AUDIO_INPUT_SRC == DOUBLE_LINEIN
			aud_adc_double_buffer_free(ADC_LINE_IN);
			aud_adc_double_buffer_free(BUILD_IN_MIC);
			if (g_aud_dst_buffer)
				gp_free((void *)g_aud_dst_buffer);
				
		#endif
		
#if 0
		close(L_pcm_ptr);
		close(R_pcm_ptr);
#endif		
			
		#if APP_DOWN_SAMPLE_EN
			if(pAudio_Encode_Para->bEnableDownSample)
			{
				DownSample_Del(pAudio_Encode_Para->DownSampleWorkMem);
				pAudio_Encode_Para->bEnableDownSample = FALSE;
				pAudio_Encode_Para->DownsampleFactor = 0;
			}
		#endif			
			pAudio_Encode_Para->Status = C_STOP_RECORD;		
			break;
		
		case MSG_AUDIO_ENCODE_ERR:
			DEBUG_MSG(DBG_PRINT("AudioEncodeERROR!!!\r\n"));
			OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_STOPING);
			break;
		}
	}
}

//===============================================================================================
// 	aud_adc_double_buffer_put 
//	parameter:	data = buffer_addr.
//				len  = size in word
//				os_q = OS _EVENT
//	return:		status.
//===============================================================================================
#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
static INT32S aud_adc_double_buffer_put(INT16U *data,INT32U cwlen, INT8U aud_in, OS_EVENT *os_q)
{
	INT32S status;

//#if AUDIO_INPUT_SRC	== ADC_LINE_IN	
//	g_aud_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
//#elif AUDIO_INPUT_SRC == BUILD_IN_MIC
//	g_aud_adc_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
//#endif	

	if (aud_in == ADC_LINE_IN)
	{
		g_aud_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
		g_aud_adc_dma_dbf.t_addr = (INT32U) data;
		g_aud_adc_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
		g_aud_adc_dma_dbf.count = (INT32U) cwlen;
		g_aud_adc_dma_dbf.notify = NULL;
		g_aud_adc_dma_dbf.timeout = 0;
		status = dma_transfer_with_double_buf(&g_aud_adc_dma_dbf, os_q);
	}
	else if (aud_in == BUILD_IN_MIC)
	{
		g_aud_mic_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
		g_aud_mic_dma_dbf.t_addr = (INT32U) data;
		g_aud_mic_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
		g_aud_mic_dma_dbf.count = (INT32U) cwlen;
		g_aud_mic_dma_dbf.notify = NULL;
		g_aud_mic_dma_dbf.timeout = 0;
		status = dma_transfer_with_double_buf(&g_aud_mic_dma_dbf, os_q);
	}

	if (status != 0)
		return status;
		
	return STATUS_OK;
}

//===============================================================================================
// 	aud_adc_double_buffer_set 
//	parameter:	data = buffer_addr.
//				len  = size in word
//	return:		status.
//===============================================================================================
static INT32U aud_adc_double_buffer_set(INT16U *data, INT32U cwlen, INT8U aud_in)
{
	INT32S status;
	
//#if AUDIO_INPUT_SRC	== ADC_LINE_IN	
//	g_aud_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
//#elif AUDIO_INPUT_SRC == BUILD_IN_MIC
//	g_aud_adc_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
//#endif	

	if (aud_in == ADC_LINE_IN)
	{
		g_aud_adc_dma_dbf.s_addr = (INT32U) P_ADC_ASADC_DATA;
		g_aud_adc_dma_dbf.t_addr = (INT32U) data;
		g_aud_adc_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
		g_aud_adc_dma_dbf.count = (INT32U) cwlen;
		g_aud_adc_dma_dbf.notify = NULL;
		g_aud_adc_dma_dbf.timeout = 0;
		status = dma_transfer_double_buf_set(&g_aud_adc_dma_dbf);
	}
	else if (aud_in == BUILD_IN_MIC)
	{
		g_aud_mic_dma_dbf.s_addr = (INT32U) P_MIC_ASADC_DATA;
		g_aud_mic_dma_dbf.t_addr = (INT32U) data;
		g_aud_mic_dma_dbf.width = DMA_DATA_WIDTH_2BYTE;		
		g_aud_mic_dma_dbf.count = (INT32U) cwlen;
		g_aud_mic_dma_dbf.notify = NULL;
		g_aud_mic_dma_dbf.timeout = 0;
		status = dma_transfer_double_buf_set(&g_aud_mic_dma_dbf);
	}
	
	if(status != 0)
		return status;

	return STATUS_OK;
}

//===============================================================================================
// 	aud_adc_dma_status_get 
//	parameter:	none
//	return:		status.
//===============================================================================================
static INT32S aud_adc_dma_status_get(INT8U aud_in)
{
	if (aud_in == ADC_LINE_IN)
	{
		if (g_aud_adc_dma_dbf.channel == 0xff) 
			return -1;
		
		return dma_status_get(g_aud_adc_dma_dbf.channel);	
	}
	else if (aud_in == BUILD_IN_MIC)
	{
		if (g_aud_mic_dma_dbf.channel == 0xff) 
			return -1;
		
		return dma_status_get(g_aud_mic_dma_dbf.channel);
	}
	
	return 0;
}

//===============================================================================================
// 	aud_adc_double_buffer_free 
//	parameter:	none
//	return:		free double buffer dma channel.
//===============================================================================================
static void aud_adc_double_buffer_free(INT8U aud_in)
{
	if (aud_in == ADC_LINE_IN)
	{
		dma_transfer_double_buf_free(&g_aud_adc_dma_dbf);
		g_aud_adc_dma_dbf.channel = 0xff;
	}
	else if (aud_in == BUILD_IN_MIC)
	{
		dma_transfer_double_buf_free(&g_aud_mic_dma_dbf);
		g_aud_mic_dma_dbf.channel = 0xff;
	}
}

//===============================================================================================
// 	adc_hardware_start 
//	parameter:	SampleRate = adc sample rate set.
//	return:		none.
//===============================================================================================
static void adc_hardware_start(INT32U SampleRate)
{	
#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == DOUBLE_LINEIN
	INT32U input_pin;
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	//set adc channel is input float
	#if C_ADC_USE_CHANNEL == ADC_LINE_0
	input_pin = IO_F6; 
	#elif C_ADC_USE_CHANNEL == ADC_LINE_1
	input_pin = IO_F7; 
	#elif C_ADC_USE_CHANNEL == ADC_LINE_2
	input_pin = IO_F8; 
	#elif C_ADC_USE_CHANNEL == ADC_LINE_3
	input_pin = IO_F9; 
	#endif
#else
	#if C_ADC_USE_CHANNEL == ADC_LINE_0
	input_pin = IO_F7; 	
	#elif C_ADC_USE_CHANNEL == ADC_LINE_1
	input_pin = IO_F8; 
	#endif
#endif 
	gpio_init_io(input_pin, GPIO_INPUT);
	gpio_set_port_attribute(input_pin, ATTRIBUTE_HIGH);

	adc_fifo_clear();
	adc_auto_ch_set(C_ADC_USE_CHANNEL);
	adc_fifo_level_set(4);
	adc_auto_sample_start();
	//OSTimeDly(50); //wait bais stable
	adc_sample_rate_set(C_ADC_USE_TIMER, SampleRate);	
#endif 
	
#if AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
	mic_fifo_clear();	
	mic_fifo_level_set(4);
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN	
	mic_agc_enable_set(0);
	R_MIC_SETUP |= (0x1<<11);
#else
	mic_agc_enable_set(1);
#endif
	mic_auto_sample_start();
	OSTimeDly(50); //wait bais stable
	mic_sample_rate_set(C_ADC_USE_TIMER, SampleRate);
#endif
	DEBUG_MSG(DBG_PRINT("AudioSampleRate = %d\r\n", SampleRate));
}
#endif

//===============================================================================================
// 	gpy0050 record  
//	parameter:	none
//	return:		free double buffer dma channel.
//===============================================================================================
#if AUDIO_INPUT_SRC == GPY0050_IN
#define SPI0_TXRX_BY_CPU(Txdata, RxData)\
{\
	R_SPI0_TX_DATA = Txdata; \
	while((R_SPI0_RX_STATUS & 0x07) == 0);\
	RxData = R_SPI0_RX_DATA;\
}\

static INT16U gpy0050_get_value(void)
{
	INT8U dummy;
	INT16U temp;
	
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low	
	SPI0_TXRX_BY_CPU(C_CMD_DUMMY_COM, dummy);
	SPI0_TXRX_BY_CPU(C_CMD_ADC_IN4, dummy);
	temp = dummy;
	SPI0_TXRX_BY_CPU(C_CMD_ZERO_COM, dummy);
	temp <<= 8;
	temp |= dummy;
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high
		
	if(temp <= 0x00FF)
		temp = g_pre_value;
	else if(temp >= 0xFFC0)
		temp = g_pre_value;
	else
		g_pre_value = temp;
	
	return temp;
}

static void gpy0050_start(void)
{
	INT8U dummy;
	
	gpio_init_io(C_GPY0050_SPI_CS_PIN, GPIO_OUTPUT);
	gpio_set_port_attribute(C_GPY0050_SPI_CS_PIN, ATTRIBUTE_HIGH);
	gpio_write_io(C_GPY0050_SPI_CS_PIN, DATA_HIGH);	//pull cs high
	
	//GPL32 SPI IF 1 initialization
	R_SPI0_TX_STATUS = 0x8020;
	R_SPI0_RX_STATUS = 0x8020;
	R_SPI0_CTRL = 0x0800;	//reset SPI0
	R_SPI0_MISC = 0x0100;  //enable smart mode 
	R_SPI0_CTRL = 0x8035;	//Master mode 3, SPI_CLK = SYS_CLK/64
	
	//sw reset gpy0050
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low		
	SPI0_TXRX_BY_CPU(C_CMD_RESET_IN4, dummy);
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high	
	
	//enable MIC AGC and ADC	
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);		//pull cs low
	SPI0_TXRX_BY_CPU(C_CMD_ENABLE_MIC_AGC_ADC, dummy);
	SPI0_TXRX_BY_CPU(C_CMD_ADC_IN4, dummy);
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high
	
	OSTimeDly(30);	//wait 300ms after AGC & MIC enable
	
	//dummy data
	gpy0050_get_value();
	gpy0050_get_value();
}

static void gpy0050_stop(void)
{
	INT8U dummy;
	
	//power down 
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 0);	//pull cs low		
	SPI0_TXRX_BY_CPU(C_CMD_POWER_DOWN, dummy);
	gpio_write_io(C_GPY0050_SPI_CS_PIN, 1);	//pull cs high	
	
	//GPL32 SPI disable
	R_SPI0_CTRL = 0;
	R_SPI0_MISC = 0;
}

static void gpy0050_isr(void)
{
	INT16U *ptr;
	
	ptr = (INT16U*)g_aud_pcm_buffer[g_buffer_index];
	*(ptr + g_buffer_cnt++) = gpy0050_get_value();	
	if(g_buffer_cnt >= pAudio_Encode_Para->PCMInFrameSize)
	{	
		g_buffer_cnt = 0;
		g_ready_buffer = g_aud_pcm_buffer[g_buffer_index];
		g_buffer_index++;
		if(g_buffer_index >= C_AUD_PCM_BUFFER_NO) g_buffer_index = 0;	
		OSQPost(aud_enc_reqQ, (void*)MSG_ADC_DMA_DONE);
	}
}
#endif

#if 0
//===============================================================================================
// 	aud_enc_RIFF_init
//	parameter:	samplerate:  wave file sample rate.
//	return:		none.
//===============================================================================================
static void aud_enc_RIFF_init(INT32U samplerate)
{
	WaveHeadPara.RIFF_ID[0] ='R';
	WaveHeadPara.RIFF_ID[1] ='I';
	WaveHeadPara.RIFF_ID[2] ='F';
	WaveHeadPara.RIFF_ID[3] ='F';
	WaveHeadPara.RIFF_len = 0;
	
	WaveHeadPara.type_ID[0] = 'W';
	WaveHeadPara.type_ID[1] = 'A';
	WaveHeadPara.type_ID[2] = 'V';
	WaveHeadPara.type_ID[3] = 'E';
	
	WaveHeadPara.fmt_ID[0] = 'f';
	WaveHeadPara.fmt_ID[1] = 'm';
	WaveHeadPara.fmt_ID[2] = 't';
	WaveHeadPara.fmt_ID[3] = ' ';
	
	WaveHeadPara.fmt_len = 16;
	WaveHeadPara.format = 1;
	WaveHeadPara.channel = 1;
	WaveHeadPara.sample_rate = samplerate;
	
	//8, 16, 24 or 32
	WaveHeadPara.Sign_bit_per_sample = 16;
	//BlockAlign = SignificantBitsPerSample / 8 * NumChannels 
	WaveHeadPara.Block_align = 16/8*1;
	//AvgBytesPerSec = SampleRate * BlockAlign 
	WaveHeadPara.avg_byte_per_sec = samplerate*2;
	
	WaveHeadPara.data_ID[0] = 'd';
	WaveHeadPara.data_ID[1] = 'a'; 
	WaveHeadPara.data_ID[2] = 't';
	WaveHeadPara.data_ID[3] = 'a';
	
	WaveHeadPara.data_len = 0;
}
#endif

//===============================================================================================
// 	write_audio_data 
//	parameter:	buffer_addr = buffer_addr.
//				cblen = size in byte
//	return:		success write length in byte.
//===============================================================================================
static INT32S write_audio_data(INT32U buffer_addr, INT32U cbLen)
{
	INT32U cb_write_len;
	
	if(pAudio_Encode_Para->SourceType == C_GP_FS)
		cb_write_len = write(pAudio_Encode_Para->FileHandle, (INT32U)buffer_addr, cbLen);
	else
		cb_write_len = audio_encode_data_write(0, (INT32U)buffer_addr, cbLen);
	
	if(cb_write_len != cbLen)
	{
		DEBUG_MSG(DBG_PRINT("AudioWriteFileFail!!!\r\n"));
		return AUD_RECORD_FILE_WRITE_ERR;	
	}
	return cb_write_len;	//byte size
}

//===============================================================================================
// 	save_buffer_to_storage 
//	parameter:	
//	return:		status
//===============================================================================================
static INT32S save_buffer_to_storage(void)
{
	INT8U  *addr;
	INT32S size, write_cblen;
	
	if(!g_ring_buffer)
	{
		if(pAudio_Encode_Para->read_index > C_BS_BUFFER_SIZE/2)
		{
			g_ring_buffer = ~g_ring_buffer;
			addr = pAudio_Encode_Para->Bit_Stream_Buffer;
			size = C_BS_BUFFER_SIZE/2;
			if(pAudio_Encode_Para->SourceType == C_GP_FS)
				write_cblen = write_audio_data((INT32U)addr, size);
			else
				write_cblen = audio_encode_data_write(0, (INT32U)addr, size);
				
			if(write_cblen != size)
				return 	AUD_RECORD_FILE_WRITE_ERR;	
		}
	}
	else
	{
		if(pAudio_Encode_Para->read_index < C_BS_BUFFER_SIZE/2)
		{
			g_ring_buffer = ~g_ring_buffer;
			addr = pAudio_Encode_Para->Bit_Stream_Buffer + C_BS_BUFFER_SIZE/2;
			size = C_BS_BUFFER_SIZE/2;
			if(pAudio_Encode_Para->SourceType == C_GP_FS)
				write_cblen = write_audio_data((INT32U)addr, size);
			else
				write_cblen = audio_encode_data_write(0, (INT32U)addr, size);
				
			if(write_cblen != size)
				return 	AUD_RECORD_FILE_WRITE_ERR;	
		}
	}
	
	return AUD_RECORD_STATUS_OK;	
}

//===============================================================================================
// 	save_final_data_to_storage 
//	parameter:	
//	return:		status
//===============================================================================================
static INT32S save_final_data_to_storage(void)
{
	INT8U *addr;
	INT32S size, write_cblen;
	
	if(pAudio_Encode_Para->read_index > C_BS_BUFFER_SIZE/2)
	{
		addr = pAudio_Encode_Para->Bit_Stream_Buffer + C_BS_BUFFER_SIZE/2;
		size = pAudio_Encode_Para->read_index - C_BS_BUFFER_SIZE/2;
		if(pAudio_Encode_Para->SourceType == C_GP_FS)
			write_cblen = write_audio_data((INT32U)addr, size);
		else
			write_cblen = audio_encode_data_write(0, (INT32U)addr, size);
		
		if(write_cblen != size)
			return 	AUD_RECORD_FILE_WRITE_ERR;
	}
	else
	{
		addr = pAudio_Encode_Para->Bit_Stream_Buffer;
		size = pAudio_Encode_Para->read_index;
		if(pAudio_Encode_Para->SourceType == C_GP_FS)
			write_cblen = write_audio_data((INT32U)addr, size);
		else
			write_cblen = audio_encode_data_write(0, (INT32U)addr, size);
		
		if(write_cblen != size)
			return 	AUD_RECORD_FILE_WRITE_ERR;
	}
	
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	adc_memory_allocate 
//	parameter:	cbLen = adc buffer size in byte.
//	return:		status.
//===============================================================================================
static INT32S adc_memory_allocate(INT32U cbLen)
{
	INT16U *ptr;
	INT32S i, j;
	
	for(i=0; i<C_AUD_PCM_BUFFER_NO; i++)
	{
		g_aud_pcm_buffer[i] =(INT32U) gp_malloc_align(cbLen, 32);
		if(!g_aud_pcm_buffer[i])
		{
			adc_memory_free();
			return AUD_RECORD_MEMORY_ALLOC_ERR;
		}
		
		ptr = (INT16U*)g_aud_pcm_buffer[i];
		for(j=0; j<cbLen/2; j++)
			*ptr++ = 0x8000; 
	}
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	adc_memory_free 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S adc_memory_free(void)
{
	INT32S i;
	
	for(i=0; i<C_AUD_PCM_BUFFER_NO; i++)
	{
		if(g_aud_pcm_buffer[i])
		{
			gp_free((void *)g_aud_pcm_buffer[i]);
			g_aud_pcm_buffer[i] = 0;
		}
	}
	
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	adc_work_mem_free 
//	parameter:	
//	return:		
//===============================================================================================
static void adc_work_mem_free(void)
{
	if(pAudio_Encode_Para->EncodeWorkMem)
		gp_free((void *)pAudio_Encode_Para->EncodeWorkMem); 
	if(pAudio_Encode_Para->Bit_Stream_Buffer)
		gp_free((void *)pAudio_Encode_Para->Bit_Stream_Buffer);
	if(pAudio_Encode_Para->pack_buffer)
		gp_free((void *)pAudio_Encode_Para->pack_buffer); 
}

//===============================================================================================
// 	audio_record_set_status 
//	parameter:	status
//	return:		
//===============================================================================================
void audio_record_set_status(INT32U status)
{
	pAudio_Encode_Para->Status = status;
}

//===============================================================================================
// 	audio_record_get_status 
//	parameter:	
//	return:		status.
//===============================================================================================
INT32U audio_record_get_status(void)
{
	return pAudio_Encode_Para->Status;
}

//===============================================================================================
// 	audio_record_set_source_type 
//	parameter:	type = GP_FS/user_define
//	return:		
//===============================================================================================
void audio_record_set_source_type(INT32U type)
{
	pAudio_Encode_Para->SourceType = type;
}

//===============================================================================================
// 	audio_record_get_source_type 
//	parameter:	
//	return:		type.
//===============================================================================================
INT32U audio_record_get_source_type(void)
{
	return pAudio_Encode_Para->SourceType;
}

//===============================================================================================
// 	audio_record_set_file_handle_free_size 
//	parameter:	file_handle = wave file handle
//	return:		status.
//===============================================================================================
INT64U audio_record_set_file_handle_free_size(INT16S file_handle)
{
	INT8U  fs_disk;
	INT64U disk_free = 0;
	
	if(file_handle >= 0)
	{
		fs_disk = GetDiskOfFile(file_handle);
		disk_free = vfsFreeSpace(fs_disk);
		pAudio_Encode_Para->FileHandle = file_handle;
		pAudio_Encode_Para->disk_free_size = disk_free - 10*1024; //reserved 10K
	}
	else
	{
		pAudio_Encode_Para->FileHandle = -1;
		pAudio_Encode_Para->disk_free_size = 0;
	}
	return disk_free;
}

//===============================================================================================
// 	audio_record_set_info 
//	parameter:	audio_format = format
//				sample_rate =
//				bit_rate = 
//				channel =
//	return:		status.
//===============================================================================================
void audio_record_set_info(INT32U audio_format, INT32U sample_rate, INT32U bit_rate, INT16U channel)
{
	pAudio_Encode_Para->AudioFormat = audio_format;
	pAudio_Encode_Para->SampleRate = sample_rate;
	pAudio_Encode_Para->BitRate = bit_rate; 
	pAudio_Encode_Para->Channel = channel; //1, mono, 2, stereo
}

//===============================================================================================
// 	audio_record_set_down_sample 
//	parameter:	b_down_sample = enable/disable
//				ratio = 2~4
//	return:		status.
//===============================================================================================
INT32S audio_record_set_down_sample(BOOLEAN b_down_sample, INT8U ratio)
{
#if	APP_DOWN_SAMPLE_EN
	pAudio_Encode_Para->bEnableDownSample = b_down_sample;
	pAudio_Encode_Para->DownsampleFactor = ratio;
	return AUD_RECORD_STATUS_OK;
#else
	return AUD_RECORD_STATUS_ERR;
#endif
}

//===============================================================================================
// 	pcm_energy_detect
//	parameter:	none.
//	return:		status.
//===============================================================================================
INT32S pcm_energy_detect(INT16S* buffer_addr, INT32U pcm_size)
{
	INT16S temp;
	INT16U avg_value;
	INT64U temp_total;
	INT32S i, cnt, local_cnt;
	
	temp_total = 0;
	cnt = 0;
	for (i = 0; i < pcm_size; i++) {
		temp = abs(*(buffer_addr + i));
		temp_total += (INT64U)temp;
		if (temp > PCM_LOCAL_THR) {
			cnt += 1; 
		}
	}
	
	// average
	avg_value = (INT64U)temp_total / pcm_size;
	DBG_PRINT("temp_total = 0x%x\r\n", temp_total);
	DBG_PRINT("avg_value = 0x%x, cnt = %d\r\n", avg_value, cnt);
	
	//samples above Local_Thr to judge as active
	local_cnt = pcm_size / 6;
	
	if((avg_value > PCM_GLOBAL_THR)||(cnt>local_cnt)) {
		return 0;	//active
	} else {
		return 1; 	//not active
	}
}

#if 0
//===============================================================================================
// 	wave_encode_start 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S wave_encode_start(void)
{
	INT32S cbLen;
	AUD_ENC_WAVE_HEADER *pWaveHeadPara;
	
	pAudio_Encode_Para->EncodeWorkMem = NULL;
	pAudio_Encode_Para->Bit_Stream_Buffer = (INT8U *)gp_malloc_align(C_BS_BUFFER_SIZE, 4);
	if(!pAudio_Encode_Para->Bit_Stream_Buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	//copy header 	
	pWaveHeadPara = &WaveHeadPara;
	cbLen = sizeof(AUD_ENC_WAVE_HEADER);
	aud_enc_RIFF_init(pAudio_Encode_Para->SampleRate);
	gp_memcpy((INT8S*)pAudio_Encode_Para->Bit_Stream_Buffer, (INT8S*)pWaveHeadPara, cbLen);
	
	pAudio_Encode_Para->read_index = cbLen;
	pAudio_Encode_Para->FileLenth = cbLen;
	pAudio_Encode_Para->PackSize = C_WAVE_RECORD_BUFFER_SIZE<<1;
	pAudio_Encode_Para->pack_buffer = NULL;
	pAudio_Encode_Para->PCMInFrameSize = C_WAVE_RECORD_BUFFER_SIZE;
	pAudio_Encode_Para->OnePCMFrameSize = C_WAVE_RECORD_BUFFER_SIZE;
	
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	wave_encode_stop 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S wave_encode_stop(void)
{
	INT32S nRet;
	AUD_ENC_WAVE_HEADER *pWaveHeadPara;
	
	nRet = save_final_data_to_storage();
	if(nRet < 0)
	{
		adc_work_mem_free();
		return AUD_RECORD_FILE_WRITE_ERR;
	}
	
	//write header
	pWaveHeadPara = &WaveHeadPara;
	pAudio_Encode_Para->FileLenth -= sizeof(AUD_ENC_WAVE_HEADER);
	pWaveHeadPara->RIFF_len = sizeof(AUD_ENC_WAVE_HEADER) + pAudio_Encode_Para->FileLenth - 8;//file size -8
	pWaveHeadPara->data_len = pAudio_Encode_Para->FileLenth;	
	if(pAudio_Encode_Para->SourceType == C_GP_FS)
	{
		lseek(pAudio_Encode_Para->FileHandle, 0, SEEK_SET);
		write(pAudio_Encode_Para->FileHandle, (INT32U)pWaveHeadPara, sizeof(AUD_ENC_WAVE_HEADER));
		close(pAudio_Encode_Para->FileHandle);
	}
	else if(pAudio_Encode_Para->SourceType == C_USER_DEFINE)
	{
		audio_encode_data_write(1, (INT32U)pWaveHeadPara, sizeof(AUD_ENC_WAVE_HEADER));
	}
	
	//free memory
	adc_work_mem_free();
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	wave_encode_once 
//	parameter:	workmem = work memory
//				buffer_addr = buffer address
//				cwlen = buffer size in word
//	return:		length in word
//===============================================================================================
static int wave_encode_once(void *workmem, const short* buffer_addr, int cwlen)
{
	INT8U  *dest_addr;
	INT32S nRet, temp;
	INT32U cblen, size;
	
	cblen = cwlen<<1;
	temp = pAudio_Encode_Para->read_index + cblen;
	if(temp > C_BS_BUFFER_SIZE)
	{
		size = C_BS_BUFFER_SIZE - pAudio_Encode_Para->read_index;
		dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
		gp_memcpy((INT8S*)dest_addr, (INT8S*)buffer_addr, size);
		
		temp = cblen - size;		//remain size
		dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer;
		buffer_addr += (size>>1); 	//word address
		gp_memcpy((INT8S*)dest_addr, (INT8S*)buffer_addr, temp);
		pAudio_Encode_Para->read_index = temp;
	}
	else
	{
		dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
		gp_memcpy((INT8S*)dest_addr, (INT8S*)buffer_addr, cblen);
		pAudio_Encode_Para->read_index += cblen;
	}
	
	pAudio_Encode_Para->FileLenth += cblen;
	nRet = save_buffer_to_storage();
	if(nRet < 0)
		return AUD_RECORD_FILE_WRITE_ERR;

	return cwlen;  
}
#endif

//===============================================================================================
// 	a1800_encode_start 
//	parameter:	none.
//	return:		status.
//===============================================================================================
#if APP_A1800_ENCODE_EN
static INT32S a1800_encode_start(void)
{
	INT32S nRet;
	
	pAudio_Encode_Para->EncodeWorkMem = (INT8U *)gp_malloc(A18_ENC_MEMORY_SIZE);
	if(pAudio_Encode_Para->EncodeWorkMem == NULL)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	gp_memset((INT8S*)pAudio_Encode_Para->EncodeWorkMem, 0, A18_ENC_MEMORY_SIZE);
	pAudio_Encode_Para->Bit_Stream_Buffer = (INT8U *)gp_malloc_align(C_BS_BUFFER_SIZE, 4);
	if(!pAudio_Encode_Para->Bit_Stream_Buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	gp_memset((INT8S*)pAudio_Encode_Para->Bit_Stream_Buffer, 0, 6);
	pAudio_Encode_Para->read_index = 6;
	pAudio_Encode_Para->FileLenth = 6;

	A18_enc_set_BitRate((unsigned char *)pAudio_Encode_Para->EncodeWorkMem, pAudio_Encode_Para->BitRate);
	nRet = A18_enc_get_BitRate((unsigned char *)pAudio_Encode_Para->EncodeWorkMem);
	if(nRet != pAudio_Encode_Para->BitRate)		
		return AUD_RECORD_RUN_ERR;
	
	nRet = A18_enc_init((unsigned char *)pAudio_Encode_Para->EncodeWorkMem);
	if(nRet != A18_ENC_NO_ERROR)	
		return AUD_RECORD_RUN_ERR;
	
	pAudio_Encode_Para->PackSize = A18_enc_get_PackageSize((unsigned char *)pAudio_Encode_Para->EncodeWorkMem);	
	pAudio_Encode_Para->pack_buffer = (INT8U *)gp_malloc_align(pAudio_Encode_Para->PackSize, 4);
	if(!pAudio_Encode_Para->pack_buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
		
	pAudio_Encode_Para->PCMInFrameSize = C_A1800_RECORD_BUFFER_SIZE * A1800_TIMES;
	pAudio_Encode_Para->OnePCMFrameSize = C_A1800_RECORD_BUFFER_SIZE;	
	
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	a1800_encode_stop 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S a1800_encode_stop(void)
{
	INT8U  A1800Header[6];
	INT32S nRet;

	nRet = save_final_data_to_storage();
	if(nRet < 0)
	{
		adc_work_mem_free();
		return AUD_RECORD_FILE_WRITE_ERR;
	}
	
	//write header
	pAudio_Encode_Para->FileLenth -= 4;		//header length not include file size(4 byte)
	A1800Header[0] = pAudio_Encode_Para->FileLenth & 0xFF; 		  //file length 
	A1800Header[1] = (pAudio_Encode_Para->FileLenth >> 8) & 0xFF; //file length
	A1800Header[2] = (pAudio_Encode_Para->FileLenth >> 16) & 0xFF; //file length 
	A1800Header[3] = (pAudio_Encode_Para->FileLenth >> 24) & 0xFF; //file length
	A1800Header[4] =  pAudio_Encode_Para->BitRate & 0xFF; 		//bit rate
	A1800Header[5] = (pAudio_Encode_Para->BitRate >> 8) & 0xFF; //bit rate
	if(pAudio_Encode_Para->SourceType == C_GP_FS)
	{
		lseek(pAudio_Encode_Para->FileHandle, 0, SEEK_SET);
		write(pAudio_Encode_Para->FileHandle, (INT32U)A1800Header, 6);
		close(pAudio_Encode_Para->FileHandle);
	}
	else if(pAudio_Encode_Para->SourceType == C_USER_DEFINE)
		audio_encode_data_write(1, (INT32U)A1800Header, 4);
	
	//free memory
	adc_work_mem_free();
	return  AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	a1800_encode_once 
//	parameter:	workmem = work memory
//				buffer_addr = buffer address
//				cwlen = buffer size in word
//	return:		length in word
//===============================================================================================
static int a1800_encode_once(void *workmem, const short* buffer_addr, int cwlen)
{
	INT8U  *dest_addr;
	INT32U N;
	INT32S nRet, cblen, temp, size;
	
	cblen = 0;
	N = cwlen;
	
	while(N >= pAudio_Encode_Para->OnePCMFrameSize)
	{
		nRet = A18_enc_run((unsigned char *)pAudio_Encode_Para->EncodeWorkMem, (short *)buffer_addr, (unsigned char *)pAudio_Encode_Para->pack_buffer);
		if(nRet != A18_ENC_NO_ERROR)	
			return AUD_RECORD_RUN_ERR;
			
		buffer_addr += pAudio_Encode_Para->OnePCMFrameSize;
		cblen += pAudio_Encode_Para->PackSize;
		N -= pAudio_Encode_Para->OnePCMFrameSize;
		
		//copy to bit stream buffer
		temp = pAudio_Encode_Para->read_index + pAudio_Encode_Para->PackSize;
		if(temp > C_BS_BUFFER_SIZE)
		{
			size = C_BS_BUFFER_SIZE - pAudio_Encode_Para->read_index;
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)pAudio_Encode_Para->pack_buffer, size);
		
			temp = pAudio_Encode_Para->PackSize - size;		//remain size
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)(pAudio_Encode_Para->pack_buffer + size), temp);
			pAudio_Encode_Para->read_index = temp;
		}
		else
		{
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)pAudio_Encode_Para->pack_buffer, pAudio_Encode_Para->PackSize);
			pAudio_Encode_Para->read_index += pAudio_Encode_Para->PackSize;
		}
	}
	
	pAudio_Encode_Para->FileLenth += cblen;
	nRet = save_buffer_to_storage();
	if(nRet < 0)
		return AUD_RECORD_FILE_WRITE_ERR;
			
	return cwlen;
}
#endif //APP_A1800_ENCODE_EN

//===============================================================================================
// 	wave_encode_lib_start 
//	parameter:	none.
//	return:		status.
//===============================================================================================
#if APP_WAV_CODEC_EN
static INT32S wave_encode_lib_start(INT32U AudioFormat)
{
	INT32S nRet, size;
	
	size = wav_enc_get_mem_block_size();
	pAudio_Encode_Para->EncodeWorkMem = (INT8U *)gp_malloc(size);
	if(pAudio_Encode_Para->EncodeWorkMem == NULL)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	gp_memset((INT8S*)pAudio_Encode_Para->EncodeWorkMem, 0, size);
	pAudio_Encode_Para->Bit_Stream_Buffer = (INT8U *)gp_malloc_align(C_BS_BUFFER_SIZE, 4);
	if(!pAudio_Encode_Para->Bit_Stream_Buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
		
	nRet = wav_enc_Set_Parameter( pAudio_Encode_Para->EncodeWorkMem, 
								  pAudio_Encode_Para->Channel, 
								  pAudio_Encode_Para->SampleRate, 
								  AudioFormat );
	if(nRet < 0)	
		return AUD_RECORD_RUN_ERR;
	
	nRet = wav_enc_init(pAudio_Encode_Para->EncodeWorkMem);
	if(nRet < 0)	
		return AUD_RECORD_RUN_ERR;
		
	//copy header
	size = wav_enc_get_HeaderLength(pAudio_Encode_Para->EncodeWorkMem);	
	gp_memset((INT8S*)pAudio_Encode_Para->Bit_Stream_Buffer, 0, size);
	pAudio_Encode_Para->read_index = size;
	pAudio_Encode_Para->FileLenth = size;
	pAudio_Encode_Para->NumSamples = 0;
		
	//pAudio_Encode_Para->PackSize = wav_enc_get_BytePerPackage(pAudio_Encode_Para->EncodeWorkMem);
	//pAudio_Encode_Para->pack_buffer = (INT8U *)gp_malloc_align(pAudio_Encode_Para->PackSize, 4);
	//if(!pAudio_Encode_Para->pack_buffer)
	//	return AUD_RECORD_MEMORY_ALLOC_ERR;
		
	size = wav_enc_get_SamplePerFrame(pAudio_Encode_Para->EncodeWorkMem);
	pAudio_Encode_Para->PCMInFrameSize = size * ADPCM_TIMES;
	pAudio_Encode_Para->OnePCMFrameSize = size;


	if(AudioFormat == WAVE_FORMAT_PCM) {
		pAudio_Encode_Para->PackSize = size * 2;
	} else {
		pAudio_Encode_Para->PackSize = wav_enc_get_BytePerPackage(pAudio_Encode_Para->EncodeWorkMem);
	}
	pAudio_Encode_Para->pack_buffer = (INT8U *)gp_malloc_align(pAudio_Encode_Para->PackSize, 4);
	if(!pAudio_Encode_Para->pack_buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	wave_encode_lib_stop 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S wave_encode_lib_stop(void)
{
	INT8U *pHeader;
	INT32S cbLen, nRet;
	
	nRet = save_final_data_to_storage();
	if(nRet < 0)
	{
		adc_work_mem_free();
		return AUD_RECORD_FILE_WRITE_ERR;
	}
	
	//write header
	cbLen = wav_enc_get_HeaderLength(pAudio_Encode_Para->EncodeWorkMem);
	pHeader = (INT8U *)gp_malloc(cbLen);
	if(!pHeader)
	{
		adc_work_mem_free();
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	}
	
	nRet = wav_enc_get_header(pAudio_Encode_Para->EncodeWorkMem, pHeader, pAudio_Encode_Para->FileLenth, pAudio_Encode_Para->NumSamples);
	if(pAudio_Encode_Para->SourceType == C_GP_FS)
	{
		lseek(pAudio_Encode_Para->FileHandle, 0, SEEK_SET);
		write(pAudio_Encode_Para->FileHandle, (INT32U)pHeader, cbLen);
		close(pAudio_Encode_Para->FileHandle);
	}
	else if(pAudio_Encode_Para->SourceType == C_USER_DEFINE)
	{
		audio_encode_data_write(1, (INT32U)pHeader, cbLen);
	}
	
	//free memory
	gp_free((void *)pHeader);
	adc_work_mem_free();
	
	return	AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	wave_encode_lib_once 
//	parameter:	workmem = work memory
//				buffer_addr = buffer address
//				cwlen = buffer size in word
//	return:		length in word
//===============================================================================================
static int wave_encode_lib_once(void *workmem, const short* buffer_addr, int cwlen)
{
	INT8U  *dest_addr;
	INT32U N;
	INT32S nRet, cblen, temp, size;
	
	cblen = 0;
	N = cwlen;
	
	while(N >= pAudio_Encode_Para->OnePCMFrameSize)
	{
		nRet = wav_enc_run(pAudio_Encode_Para->EncodeWorkMem, (short *)buffer_addr, pAudio_Encode_Para->pack_buffer);
		if(nRet < 0)		
			return  AUD_RECORD_RUN_ERR;
		
		pAudio_Encode_Para->NumSamples += wav_enc_get_SamplePerFrame(pAudio_Encode_Para->EncodeWorkMem);
		buffer_addr += pAudio_Encode_Para->OnePCMFrameSize;
		cblen += pAudio_Encode_Para->PackSize;
		N -= pAudio_Encode_Para->OnePCMFrameSize;
		
		//copy to bit stream buffer
		temp = pAudio_Encode_Para->read_index + pAudio_Encode_Para->PackSize;
		if(temp > C_BS_BUFFER_SIZE)
		{
			size = C_BS_BUFFER_SIZE - pAudio_Encode_Para->read_index;
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)pAudio_Encode_Para->pack_buffer, size);
		
			temp = pAudio_Encode_Para->PackSize - size;		//remain size
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)(pAudio_Encode_Para->pack_buffer + size), temp);
			pAudio_Encode_Para->read_index = temp;
		}
		else
		{
			dest_addr = pAudio_Encode_Para->Bit_Stream_Buffer + pAudio_Encode_Para->read_index;
			gp_memcpy((INT8S*)dest_addr, (INT8S*)pAudio_Encode_Para->pack_buffer, pAudio_Encode_Para->PackSize);
			pAudio_Encode_Para->read_index += pAudio_Encode_Para->PackSize;
		}
	}
	
	pAudio_Encode_Para->FileLenth += cblen;
	nRet = save_buffer_to_storage();
	if(nRet < 0)
		return AUD_RECORD_FILE_WRITE_ERR;
			
	return cwlen;
}
#endif	//APP_WAV_CODEC_EN

//===============================================================================================
// 	mp3_encode_start 
//	parameter:	none.
//	return:		status.
//===============================================================================================
#if APP_MP3_ENCODE_EN
static INT32S mp3_encode_start(void)
{
	INT32S nRet, size;
	
	size = mp3enc_GetWorkMemSize();
	pAudio_Encode_Para->EncodeWorkMem = (INT8U *)gp_malloc(size);
	if(!pAudio_Encode_Para->EncodeWorkMem)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
	
	gp_memset((INT8S*)pAudio_Encode_Para->EncodeWorkMem, 0, size);	
	pAudio_Encode_Para->Bit_Stream_Buffer = (INT8U *)gp_malloc_align(C_BS_BUFFER_SIZE, 4);	//ring buffer 
	if(!pAudio_Encode_Para->Bit_Stream_Buffer)
		return AUD_RECORD_MEMORY_ALLOC_ERR;
		
	nRet = mp3enc_init(	pAudio_Encode_Para->EncodeWorkMem, 
						pAudio_Encode_Para->Channel, 
						pAudio_Encode_Para->SampleRate,
						(pAudio_Encode_Para->BitRate/1000), 
						0, //copyright
						(CHAR *)pAudio_Encode_Para->Bit_Stream_Buffer,
						C_BS_BUFFER_SIZE, 
						0); //RingWI, for ID3 header skip. 
	if(nRet<0)
		return AUD_RECORD_INIT_ERR;
	
	pAudio_Encode_Para->read_index = 0;						
	pAudio_Encode_Para->PackSize = 0;
	pAudio_Encode_Para->pack_buffer = NULL;
	//size = nRet * MP3_TIME * pAudio_Encode_Para->Channel;
	size = nRet * MP3_TIME;
	pAudio_Encode_Para->PCMInFrameSize = size;
	//size = nRet*pAudio_Encode_Para->Channel;
	size = nRet;
	pAudio_Encode_Para->OnePCMFrameSize = size; 
			
	return AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	MP3_encode_stop 
//	parameter:	none.
//	return:		status.
//===============================================================================================
static INT32S mp3_encode_stop(void)
{
	INT32S nRet;
	INT32U old_index;
	
	old_index = pAudio_Encode_Para->read_index;
	nRet = mp3enc_end((void *)pAudio_Encode_Para->EncodeWorkMem);
	if(nRet> 0)
	{
		pAudio_Encode_Para->read_index = nRet;
		nRet = pAudio_Encode_Para->read_index - old_index;
		if(nRet <0)
			nRet += C_BS_BUFFER_SIZE;
		
		pAudio_Encode_Para->FileLenth += nRet;	
		nRet = save_buffer_to_storage();
		if(nRet < 0)
		{
			adc_work_mem_free();
			return AUD_RECORD_FILE_WRITE_ERR;
		}
	}
	
	nRet = save_final_data_to_storage();
	if(nRet < 0)
	{
		adc_work_mem_free();
		return AUD_RECORD_FILE_WRITE_ERR;
	}
	
	if(pAudio_Encode_Para->SourceType == C_GP_FS)
		close(pAudio_Encode_Para->FileHandle);
	else
		audio_encode_data_write(1, NULL, 0);
	
	//free memory
	adc_work_mem_free();
	return  AUD_RECORD_STATUS_OK;
}

//===============================================================================================
// 	mp3_encode_once 
//	parameter:	workmem = work memory
//				buffer_addr = buffer address
//				cwlen = buffer size in word
//	return:		length in word
//===============================================================================================
static int mp3_encode_once(void *workmem, const short* buffer_addr, int cwlen)
{
	INT32U N, old_index;
	INT32S nRet, cblen;
	
	cblen = 0;
	old_index = pAudio_Encode_Para->read_index;
	N = cwlen;
	
	while(N >= pAudio_Encode_Para->OnePCMFrameSize)
	{
		nRet =  mp3enc_encframe((void *)pAudio_Encode_Para->EncodeWorkMem, (short *)buffer_addr);
		if(nRet<0)	
			return AUD_RECORD_RUN_ERR;
		
		pAudio_Encode_Para->read_index = nRet;
		buffer_addr += pAudio_Encode_Para->OnePCMFrameSize;
		N -= pAudio_Encode_Para->OnePCMFrameSize;
	}
		
	cblen = pAudio_Encode_Para->read_index - old_index;
	if(cblen <0)
		cblen += C_BS_BUFFER_SIZE;
		
	pAudio_Encode_Para->FileLenth += cblen;	
	nRet = save_buffer_to_storage();
	if(nRet < 0)
		return AUD_RECORD_FILE_WRITE_ERR;
		
	return cwlen;
}
#endif //APP_MP3_ENCODE_EN
