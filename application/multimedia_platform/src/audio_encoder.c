#include "audio_encoder.h"

extern OS_EVENT 	*aud_enc_reqQ;
//===================================================================================
// audio encode entry
//===================================================================================
void audio_encode_entrance(void)
{
	adc_record_task_create(AUD_ENC_PRIORITY);
}

void audio_encode_exit(void)
{
	 adc_record_task_del(AUD_ENC_PRIORITY);
}

//===================================================================================
// start record
//===================================================================================
CODEC_START_STATUS audio_encode_start(MEDIA_SOURCE src, INT16U SampleRate, INT32U BitRate)
{	
	INT32S i, status;
	INT64U temp;
	
	if(audio_record_get_status() == C_START_RECORD)
		return REENTRY_ERROR;
		
	if((src.type != SOURCE_TYPE_FS)&&(src.type != SOURCE_TYPE_USER_DEFINE))
		return RESOURCE_NO_FOUND_ERROR;
	
	if(src.type == SOURCE_TYPE_FS)
	{
		if(src.type_ID.FileHandle < 0)	
			return RESOURCE_NO_FOUND_ERROR;
		
		audio_record_set_status(C_STOP_RECORD);
		audio_record_set_source_type(C_GP_FS);
		temp = audio_record_set_file_handle_free_size(src.type_ID.FileHandle);
		if(temp <= 10240) //reserve 10*1024byte
			return 	RESOURCE_WRITE_ERROR;
	}
	else
	{
		audio_record_set_status(C_STOP_RECORD);
		audio_record_set_source_type(C_USER_DEFINE);
		audio_record_set_file_handle_free_size(-1);
	}
		
	if(src.Format.AudioFormat == WAV)
	{
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
		audio_record_set_info(WAVE_FORMAT_PCM, SampleRate, BitRate, C_STEREO_RECORD);
#else
		audio_record_set_info(WAVE_FORMAT_PCM, SampleRate, BitRate, C_MONO_RECORD);
#endif		
		
	}
	else if(src.Format.AudioFormat == A1800)
	{
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
		DBG_PRINT("A1800 does not support stereo encoding !!!\r\n");
		return AUDIO_ALGORITHM_NO_FOUND_ERROR;
#else	
		audio_record_set_info(WAVE_FORMAT_A1800, 16000, BitRate, C_MONO_RECORD);
		audio_record_set_down_sample(TRUE, 2);
#endif		
	}
	else if(src.Format.AudioFormat == IMA_ADPCM)
	{
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
		audio_record_set_info(WAVE_FORMAT_IMA_ADPCM, SampleRate, BitRate, C_STEREO_RECORD);
#else	
		audio_record_set_info(WAVE_FORMAT_IMA_ADPCM, SampleRate, BitRate, C_MONO_RECORD);
#endif		
	}
	else if(src.Format.AudioFormat == MICROSOFT_ADPCM)
	{
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
		audio_record_set_info(WAVE_FORMAT_ADPCM, SampleRate, BitRate, C_STEREO_RECORD);
#else	
		audio_record_set_info(WAVE_FORMAT_ADPCM, SampleRate, BitRate, C_MONO_RECORD);
#endif		
	}
	else if(src.Format.AudioFormat == MP3)
	{
#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
		audio_record_set_info(WAVE_FORMAT_MP3, SampleRate, BitRate, C_STEREO_RECORD);		
#else	
		audio_record_set_info(WAVE_FORMAT_MP3, SampleRate, BitRate, C_MONO_RECORD);		
#endif		
	}
	else 
		return AUDIO_ALGORITHM_NO_FOUND_ERROR;
	
	OSQPost(aud_enc_reqQ, (void *) MSG_AUDIO_ENCODE_START);
	//wait 10 second
	for(i=0; i<1000; i++)
	{
		OSTimeDly(1);
		status = audio_record_get_status();
		if(status == C_START_RECORD)
			return START_OK;
			
		if(status == C_START_FAIL)
			return RESOURCE_NO_FOUND_ERROR;
	}
	
	return CODEC_START_STATUS_ERROR_MAX;
}

//===================================================================================
// stop record
//===================================================================================
void audio_encode_stop(void)
{
	INT32S i;
	
	if(audio_record_get_status() == C_START_RECORD)
	{
		OSQPost(aud_enc_reqQ, (void *)MSG_AUDIO_ENCODE_STOPING);
		//wait 10 second
		for(i=0; i<1000; i++)
		{
			OSTimeDly(1);
			if(audio_record_get_status() == C_STOP_RECORD)
				return;
		}
	}
}

//====================================================================================
//	status
//====================================================================================
AUDIO_CODEC_STATUS audio_encode_status(void)
{
	if(audio_record_get_status() == C_START_RECORD)
		return AUDIO_CODEC_PROCESSING;
	else if(audio_record_get_status() == C_STOP_RECORD)
		return AUDIO_CODEC_PROCESS_END;
	else if(audio_record_get_status() == C_STOP_RECORDING)
		return AUDIO_CODEC_PROCESS_END;
	else if(audio_record_get_status() == C_START_FAIL)
		return AUDIO_CODEC_BREAK_OFF;
	else
		return AUDIO_CODEC_STATUS_MAX;
}

//====================================================================================
//	Set DownSample when audio record
//  bEnable: enable or disable
//  DownSampleFactor: 2, 3 or 4
//====================================================================================
CODEC_START_STATUS audio_encode_set_downsample(INT8U bEnable, INT8U DownSampleFactor)
{
	INT32S nRet;
	
	if(DownSampleFactor > 4)
		DownSampleFactor = 4;
	
	if(DownSampleFactor < 2)
		DownSampleFactor = 2;
	
	nRet = audio_record_set_down_sample(bEnable, DownSampleFactor);
	if(nRet >= 0)
		return START_OK;
	else
		return  RESOURCE_NO_FOUND_ERROR;
}	
