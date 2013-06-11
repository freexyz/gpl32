#include "audio_decoder.h"

//============================== Audio Decoder ================================
//
// including WMA, MP3, WAV, A1800, S880, MIDI(SPU) .....
//
//=============================================================================
/* extern varaible */
extern INT32U mp3_ID3V2_length;
extern INT32S audio_total_time;
extern INT32S audio_decode_sample_number;
extern INT8U audio_rampdown_disable_flag;

/* function */
void audio_decode_entrance(void);						// peripheral setting, global variable initial, memory allocation
void audio_decode_exit(void);							// peripheral unset, global variable reset, memory free
void audio_decode_rampup(void);							// Ramp to Middle Level 0x80(Standby)
void audio_decode_rampdown(void);						// Ramp to Zero Level 0x0(Power Saving)
CODEC_START_STATUS audio_decode_start(AUDIO_ARGUMENT arg, MEDIA_SOURCE src);
void audio_decode_stop(AUDIO_ARGUMENT arg);				// resource should be completely released if card accendentially plug out
void audio_decode_pause(AUDIO_ARGUMENT arg);
void audio_decode_resume(AUDIO_ARGUMENT arg);	
void audio_decode_volume_set(AUDIO_ARGUMENT *arg, int volume);
void audio_decode_mute(AUDIO_ARGUMENT *arg);
void audio_decode_unmute(AUDIO_ARGUMENT *arg);
AUDIO_CODEC_STATUS audio_decode_status(AUDIO_ARGUMENT arg);
void audio_decode_get_info(AUDIO_ARGUMENT arg, MEDIA_SOURCE src, AUDIO_INFO *audio_info);

/* gloable varaible */
// For task control
STAudioTaskPara Aud_Para;
STAudioTaskPara Aud_BG_Para;
STAudioTaskPara MIDI_Para;

// For audio decoder control
INT32U Audio_Decoder_Status_0;
INT32U Audio_Decoder_Status_1;
INT32U Audio_Decoder_Status_2;

// For user-define data mode

//added by Dexter.Ming 081009
AUDIO_INFO		G_audio_info;
AUDIO_ARGUMENT 	G_audio_argument;
MEDIA_SOURCE	G_media_source;
AUDIO_CODEC_STATUS 	G_audio_status;
SACM_CTRL G_SACM_Ctrl;
SND_INFO  G_snd_info;

#if APP_VOICE_CHANGER_EN	
void *hVC = NULL;		//for speed control
#endif
#if APP_UP_SAMPLE_EN
void *hUpSample = NULL;	//for up-sample
#endif
#if APP_CONST_PITCH_EN
void *hConstPitch = NULL;
#endif
#if APP_ECHO_EN
void *hEcho = NULL;
#endif

//=============================================================================
//============================== Audio Decoder ================================
//
// including WMA, MP3, WAV, A1800, S880, MIDI(SPU) .....
//
//=============================================================================
void audio_decode_entrance(void)						// peripheral setting, global variable initial, memory allocation
{
	gp_memset((INT8S*)&Aud_Para, 0, sizeof(STAudioTaskPara));
	gp_memset((INT8S*)&Aud_BG_Para, 0, sizeof(STAudioTaskPara));
	gp_memset((INT8S*)&MIDI_Para, 0, sizeof(STAudioTaskPara));
	
	Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESS_END;
	Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESS_END;
	Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESS_END;
#if APP_CONST_PITCH_EN	
	if(hConstPitch == NULL)
		hConstPitch = ConstantPitch_Create(8192, 22050, 1, 0);
#endif
#if APP_ECHO_EN
	if(hEcho == NULL)
		hEcho = Echo_Create(8192, 48000, 1, 0);
#endif
#if APP_VOICE_CHANGER_EN
	if(hVC == NULL)	
		hVC = VoiceChanger_Create(8192, 48000, 2, 0);
#endif
#if APP_UP_SAMPLE_EN
	if(hUpSample == NULL)
		hUpSample = UpSample_Create(8192);
#endif
	gp_memset((INT8S*)&G_audio_info, 0, sizeof(AUDIO_INFO));
	gp_memset((INT8S*)&G_audio_argument, 0, sizeof(AUDIO_ARGUMENT));
	gp_memset((INT8S*)&G_media_source, 0, sizeof(MEDIA_SOURCE));
	gp_memset((INT8S*)&G_audio_status, 0, sizeof(AUDIO_CODEC_STATUS));
	gp_memset((INT8S*)&G_SACM_Ctrl, 0, sizeof(SACM_CTRL));
	gp_memset((INT8S*)&G_snd_info, 0, sizeof(SND_INFO));
	
	G_snd_info.ConstPitchEn = FALSE;
	G_snd_info.Pitch_idx = 7;
	
	G_snd_info.EchoEn = FALSE;
	G_snd_info.weight_idx = 1;
	G_snd_info.delay_len = 1024;		
	
	G_snd_info.VoiceChangerEn = FALSE;
	G_snd_info.Speed = 12;
	G_snd_info.Pitch = 12;
}

//=============================================================================
void audio_decode_exit(void)							// peripheral unset, global variable reset, memory free
{
#if APP_CONST_PITCH_EN
	ConstantPitch_Del(hConstPitch);
	hConstPitch = NULL;
#endif
#if APP_ECHO_EN
	Echo_Del(hEcho);
	hEcho = NULL;
#endif
#if APP_VOICE_CHANGER_EN
	VoiceChanger_Del(hVC);
	hVC = NULL;
#endif
#if APP_UP_SAMPLE_EN
	UpSample_Del(hUpSample);
	hUpSample = NULL;
#endif
}

//=============================================================================
void audio_decode_rampup(void)							// Ramp to Middle Level 0x80(Standby)
{
	dac_vref_set(TRUE);
	dac_init();
}

//=============================================================================
void audio_decode_rampdown(void)						// Ramp to Zero Level 0x0(Power Saving)
{
	dac_disable();
}

//=============================================================================
CODEC_START_STATUS audio_decode_start(AUDIO_ARGUMENT arg, MEDIA_SOURCE src)
{	
	INT32U			msg_id;
	STAudioConfirm  test_para;
	struct sfn_info temp_sfn_file;

	switch(arg.Main_Channel) 
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 0:
		// use spu channel to play audio
		// Check whether audio decoder 0 is idle or not
		if(Audio_Decoder_Status_0 != AUDIO_CODEC_PROCESS_END)
			return Audio_Decoder_Status_0;
		
		Aud_BG_Para.mute = arg.mute;
		Aud_BG_Para.volume = arg.volume;
		Aud_BG_Para.fd = src.type_ID.FileHandle;
		Aud_BG_Para.src_id = arg.midi_index;

		// [New Add]
		audio_bg_ctrl.data_offset = arg.data_offset;

		if(src.type == SOURCE_TYPE_FS)
		{
			Aud_BG_Para.src_type = AUDIO_SRC_TYPE_FS;
			sfn_stat(src.type_ID.FileHandle, &temp_sfn_file);
			Aud_BG_Para.file_len = temp_sfn_file.f_size;
			audio_bg_move_data = NULL;
		}
		else if(src.type == SOURCE_TYPE_NVRAM)
		{
			Aud_BG_Para.src_type = AUDIO_SRC_TYPE_APP_RS;  
			Aud_BG_Para.file_len = nv_rs_size_get(Aud_BG_Para.fd);
			audio_bg_move_data = NULL;
		}
		else if(src.type == SOURCE_TYPE_USER_DEFINE)
		{
			Aud_BG_Para.src_type = AUDIO_SRC_TYPE_USER_DEFINE;
			Aud_BG_Para.file_len = arg.data_len;
			audio_bg_move_data = audio_encoded_data_read;
			audio_bg_ctrl.data_start_addr = arg.data_start_addr;
		}
		else if(src.type == SOURCE_TYPE_FS_RESOURCE_IN_FILE)	// added by Bruce, 2010/01/22
		{
			Aud_BG_Para.src_type = AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE;
			Aud_BG_Para.file_len = arg.data_len;
			audio_bg_move_data = NULL;
		}
		
		switch(src.Format.AudioFormat)
		{
			case MIDI:
				Aud_BG_Para.audio_format = AUDIO_TYPE_MIDI;
				break;
			case WMA:
				#if APP_WMA_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_WMA;
				break;
			case MP3:
				#if APP_MP3_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_MP3;
				break;
			case WAV:
				#if APP_WAV_CODEC_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_WAV;
				break;
			case A1800:
				#if APP_A1800_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_A1800;
				break;
			case A1600:
				#if APP_A1600_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_A1600;
				break;
			case S880:
				#if APP_S880_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_S880;
				break;
			case A6400:
				#if APP_A6400_DECODE_EN == 0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_BG_Para.audio_format = AUDIO_TYPE_A6400;
				break;
			case AAC:
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			default:
				break;
		}
		
		msgQFlush(Audio_BG_status_Q);
		Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESSING;//091027
		decode_end = audio_decode_end;
        if(Aud_BG_Para.volume >= 63) {
			Aud_BG_Para.volume = 127;
		} else {
			Aud_BG_Para.volume <<= 1;
		}
		
        msgQSend(AudioBGTaskQ, MSG_AUD_PLAY, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
		while(1)
		{
			msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
			if(msg_id == EVENT_APQ_ERR_MSG)
			{
				if(test_para.result_type == MSG_AUD_BG_PLAY_RES)
				{
					if(test_para.result == AUDIO_ERR_NONE)
					{
						break;
					}
					else
					{
						Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESS_END;
						return RESOURCE_NO_FOUND_ERROR;
					}
					break;
				}
			}
			OSTimeDly(2);
		}
		break;
#endif	

	case 1:
		// use dac to play audio 
		// Check whether audio decoder 1 is idle or not
		if(Audio_Decoder_Status_1 != AUDIO_CODEC_PROCESS_END)
			return Audio_Decoder_Status_1;
	
		Aud_Para.mute = arg.mute;
		Aud_Para.volume = arg.volume;
		Aud_Para.fd = src.type_ID.FileHandle;
		Aud_Para.src_id = 0;  

		// [New Add]
		audio_ctrl.data_offset = arg.data_offset;

		if(src.type == SOURCE_TYPE_FS)
		{
			Aud_Para.src_type = AUDIO_SRC_TYPE_FS;
			sfn_stat(src.type_ID.FileHandle, &temp_sfn_file);
			Aud_Para.file_len = temp_sfn_file.f_size;
			audio_move_data = NULL;
		}
		else if(src.type == SOURCE_TYPE_NVRAM)
		{
			Aud_Para.src_type = AUDIO_SRC_TYPE_APP_RS; 
			Aud_Para.file_len = nv_rs_size_get(Aud_Para.fd);
			audio_move_data = NULL; 
		}
		else if(src.type == SOURCE_TYPE_USER_DEFINE)
		{
			Aud_Para.src_type = AUDIO_SRC_TYPE_USER_DEFINE;
			
			if(G_SACM_Ctrl.Offsettype != SND_OFFSET_TYPE_NONE)
				arg.data_len = Snd_GetLen();

			Aud_Para.file_len = arg.data_len;
			audio_move_data = audio_encoded_data_read;
			audio_ctrl.data_start_addr = arg.data_start_addr;
		}
		else if(src.type == SOURCE_TYPE_FS_RESOURCE_IN_FILE)	// added by Bruce, 2010/01/22
		{
			Aud_Para.src_type = AUDIO_SRC_TYPE_FS_RESOURCE_IN_FILE;
			Aud_Para.file_len = arg.data_len;
			audio_move_data = NULL;
		}
			
		switch(src.Format.AudioFormat)
		{
			case MIDI:
				return CHANNEL_ASSIGN_ERROR;
				//break;
			case WMA:
			#if APP_WMA_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_WMA;
				break;
			case MP3:
			#if APP_MP3_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_MP3;
				break;
			case WAV:
			#if APP_WAV_CODEC_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_WAV;
				break;
			case A1800:
			#if APP_A1800_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_A1800;
				break;
			case A1600:
			#if APP_A1600_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_A1600;
				break;
			case S880:
			#if APP_S880_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_S880;
				break;
			case A6400:
			#if APP_A6400_DECODE_EN == 0
				return AUDIO_ALGORITHM_NO_FOUND_ERROR;
			#endif
				Aud_Para.audio_format = AUDIO_TYPE_A6400;
				break;
			case AAC:
				#if APP_ACC_DECODE_EN==0
					return AUDIO_ALGORITHM_NO_FOUND_ERROR;
				#endif
				Aud_Para.audio_format  = AUDIO_TYPE_AAC;
			default:
				break;
		}
			
		msgQFlush(Audio_FG_status_Q);	
		Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESSING;
		decode_end = audio_decode_end;
		msgQSend(AudioTaskQ, MSG_AUD_PLAY, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
		while(1)
		{
			msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
			if(msg_id == EVENT_APQ_ERR_MSG)
			{
				if(test_para.result_type == MSG_AUD_PLAY_RES)
				{
					if(test_para.result == AUDIO_ERR_NONE)
					{
						if (Aud_Para.volume < 64) 
						{
							dac_pga_set(Aud_Para.volume);		
						}
					}
					else
					{
						Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESS_END;
						return RESOURCE_NO_FOUND_ERROR;
					}
					break;
				}
			}
			OSTimeDly(2);
		}
		break;
		
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 2:
		// midi paly with spu 
		// Check whether audio decoder 2 is idle or not
		if(Audio_Decoder_Status_2 != AUDIO_CODEC_PROCESS_END)
			return Audio_Decoder_Status_2;
	
	#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
		SPU_MIDI_Set_SPU_Channel_Mask(0xFFFFFFFC);		// Return SPU channel 0 & 1 for MIDI
	#else	//GPL326XX
		SPU_MIDI_Set_SPU_Channel_Mask(0xFFFC);			// Return SPU channel 0 & 1 for MIDI
	#endif
		
		MIDI_Para.mute = arg.mute;
		MIDI_Para.volume = arg.volume;
		MIDI_Para.fd = src.type_ID.FileHandle;
		MIDI_Para.src_id = arg.midi_index;
		
		if(src.type == SOURCE_TYPE_FS)
		{	//use gp file system
			SPU_Set_idi_addr(0x0);
			MIDI_Para.src_type = AUDIO_SRC_TYPE_FS;
			sfn_stat(src.type_ID.FileHandle, &temp_sfn_file);
		}
		else if(src.type == SOURCE_TYPE_NVRAM)
		{	//nv_read
			SPU_Set_idi_addr(0x0);
			MIDI_Para.src_type = AUDIO_SRC_TYPE_APP_RS; 
		}
		else if(src.type == SOURCE_TYPE_USER_DEFINE)
		{	//read nand app fix addr, set app addr by SPU_Set_idi_addr()
			SPU_Set_idi_addr((INT32U)src.type_ID.memptr);
			MIDI_Para.src_type = AUDIO_SRC_TYPE_USER_DEFINE; 
		}

		switch(src.Format.AudioFormat)
		{
			case MIDI:
				MIDI_Para.audio_format = AUDIO_TYPE_MIDI;
				break;
			default:
				return CHANNEL_ASSIGN_ERROR;
		}	
			
		msgQFlush(MIDI_status_Q);	
		Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESSING;
		decode_end = audio_decode_end;
		msgQSend(MIDITaskQ, MSG_AUD_PLAY, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
		while(1)
		{
			msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
			if(msg_id == EVENT_APQ_ERR_MSG)
			{
				if(test_para.result_type == MSG_AUD_BG_PLAY_RES)
				{		
					if(test_para.result == 	AUDIO_ERR_NONE)
					{
						if (MIDI_Para.volume < 128) 
						{
							SPU_MIDI_Set_MIDI_Volume(MIDI_Para.volume);		
						}
					}
					else
					{
						Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESS_END;		
						return RESOURCE_NO_FOUND_ERROR;
					}
					return START_OK;
				}
			}
			OSTimeDly(2);
		}
		break;
#endif

	default:
		// not channel 0, 1 or 2
		return CHANNEL_ASSIGN_ERROR;
	}	
				
	return START_OK;
}

//=============================================================================
void audio_decode_stop(AUDIO_ARGUMENT arg)				// resource should be completely released if card accendentially plug out
{
	INT32U			msg_id;
	STAudioConfirm	test_para;
	
	switch(arg.Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 0:
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(AudioBGTaskQ, MSG_AUD_STOP, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_STOP_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESS_END;
		break;
#endif

	case 1:
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(AudioTaskQ, MSG_AUD_STOP, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_STOP_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESS_END;
		break;
		
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	case 2:	
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(MIDITaskQ, MSG_AUD_STOP, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_STOP_RES)
						break;
				}
				OSTimeDly(2);
			}
		#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
			SPU_MIDI_Set_SPU_Channel_Mask(0xFFFFFFFC);		// Return SPU channel 0 & 1 for MIDI
		#else	//GPL326XX
			SPU_MIDI_Set_SPU_Channel_Mask(0xFFFC);			// Return SPU channel 0 & 1 for MIDI
		#endif
		}
		Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESS_END;
		break;
#endif	
	}
}


//=============================================================================
void audio_decode_pause(AUDIO_ARGUMENT arg)
{
	INT32U			msg_id;
	STAudioConfirm	test_para;

	switch(arg.Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	case 0:	
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESSING)
		{
			msgQSend(AudioBGTaskQ, MSG_AUD_PAUSE, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_PAUSE_RES)
						break;
				}
				OSTimeDly(2);
			}
				Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESS_PAUSED;
		}
		break;
#endif
	
	case 1:
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESSING)
		{
			msgQSend(AudioTaskQ, MSG_AUD_PAUSE, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_PAUSE_RES)
						break;
				}
				OSTimeDly(2);
			}
				Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESS_PAUSED;
		}
		break;

#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 2:
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESSING)
		{
			msgQSend(MIDITaskQ, MSG_AUD_PAUSE, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_PAUSE_RES)
						break;
				}
				OSTimeDly(2);
			}
				Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESS_PAUSED;
		}
		break;
#endif
	}
}


//=============================================================================
void audio_decode_resume(AUDIO_ARGUMENT arg)
{
	INT32U			msg_id;
	STAudioConfirm	test_para;

	switch(arg.Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	case 0:	
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(AudioBGTaskQ, MSG_AUD_RESUME, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_RESUME_RES)
						break;
				}
				OSTimeDly(2);
			}
				Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESSING;
		}
		break;
#endif

	case 1:
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(AudioTaskQ, MSG_AUD_RESUME, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_RESUME_RES)
						break;
				}
				OSTimeDly(2);
			}
				Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESSING;
		}
		break;
		
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 2:
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			msgQSend(MIDITaskQ, MSG_AUD_RESUME, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_BG_RESUME_RES)
						break;
				}
				OSTimeDly(2);
			}
			Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESSING;
		}
		break;
#endif
	}
}


//=============================================================================
void audio_decode_volume_set(AUDIO_ARGUMENT *arg, int volume)
{
	INT32U			msg_id;
	STAudioConfirm	test_para;
	
	switch(arg->Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 0:
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			arg->volume = volume;
			Aud_BG_Para.volume = volume;
			msgQSend(AudioBGTaskQ, MSG_AUD_VOLUME_SET, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_VOLUME_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
#endif
	
	case 1:	
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			arg->volume = volume;
			Aud_Para.volume = volume;
			msgQSend(AudioTaskQ, MSG_AUD_VOLUME_SET, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_VOLUME_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;

#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 2:
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESSING || Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESS_PAUSED)
		{
			arg->volume = volume;
			MIDI_Para.volume = volume;
			msgQSend(MIDITaskQ, MSG_AUD_VOLUME_SET, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_VOLUME_SET_RES)
						break;
				}
				OSTimeDly(2);
			}	
		}
		break;
#endif
	}
}


//=============================================================================
void audio_decode_mute(AUDIO_ARGUMENT *arg)
{
	INT32U			msg_id;
	STAudioConfirm	test_para;
	
	switch(arg->Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 0:
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = TRUE;
			Aud_BG_Para.mute = TRUE;
			msgQSend(AudioBGTaskQ, MSG_AUD_SET_MUTE, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
#endif
	
	case 1:
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = TRUE;
			Aud_Para.mute = TRUE;
			msgQSend(AudioTaskQ, MSG_AUD_SET_MUTE, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
	
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 2:	
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = TRUE;
			MIDI_Para.mute = TRUE;
			msgQSend(MIDITaskQ, MSG_AUD_SET_MUTE, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
#endif
	}
}

//=============================================================================
void audio_decode_unmute(AUDIO_ARGUMENT *arg)
{
	INT32U			msg_id;
	STAudioConfirm	test_para;
	
	switch(arg->Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 0:
		if(Audio_Decoder_Status_0 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = FALSE;
			Aud_BG_Para.mute = FALSE;
			msgQSend(AudioBGTaskQ, MSG_AUD_SET_MUTE, (void *)&Aud_BG_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_BG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
#endif

	case 1:
		if(Audio_Decoder_Status_1 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = FALSE;
			Aud_Para.mute = FALSE;
			msgQSend(AudioTaskQ, MSG_AUD_SET_MUTE, (void *)&Aud_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(Audio_FG_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
	
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	case 2:	
		if(Audio_Decoder_Status_2 == AUDIO_CODEC_PROCESSING)
		{
			arg->mute = FALSE;
			MIDI_Para.mute = FALSE;
			msgQSend(MIDITaskQ, MSG_AUD_SET_MUTE, (void *)&MIDI_Para, sizeof(STAudioTaskPara), MSG_PRI_NORMAL);
			while(1)
			{
				msgQReceive(MIDI_status_Q, &msg_id, (void *)&test_para, sizeof(STAudioConfirm));
				if(msg_id == EVENT_APQ_ERR_MSG)
				{
					if(test_para.result_type == MSG_AUD_MUTE_SET_RES)
						break;
				}
				OSTimeDly(2);
			}
		}
		break;
#endif
	}
}


//=============================================================================
AUDIO_CODEC_STATUS audio_decode_status(AUDIO_ARGUMENT arg)
{

	switch(arg.Main_Channel)
	{
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	case 0:
		return Audio_Decoder_Status_0;
		break;
#endif	
	case 1:
		return Audio_Decoder_Status_1;
		break;
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)		
	case 2:
		return Audio_Decoder_Status_2;
		break;	
#endif
	}
	
	return AUDIO_CODEC_PROCESS_END;
}


//=============================================================================
void audio_decode_get_info(AUDIO_ARGUMENT arg, MEDIA_SOURCE src, AUDIO_INFO *audio_info)
{
	INT8U temp_volume;
	struct sfn_info temp_sfn_file;
	INT8U flag = 0;

	sfn_stat(src.type_ID.FileHandle, &temp_sfn_file);
	audio_info->FileSize = temp_sfn_file.f_size;

	temp_volume = arg.volume;
	arg.volume = 0;
	arg.Main_Channel = 1;
	if(audio_decode_status(arg) == AUDIO_CODEC_PROCESS_END)
	{		
		audio_decode_start(arg, src);
		flag = 1;
	}
	
	switch(src.Format.AudioFormat)
	{
		case MIDI:
			//
			break;			
	#if APP_WMA_DECODE_EN == 1
		case WMA:
			audio_info->DataRate = wma_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info->Channel = wma_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info->SampleRate = wma_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info->FrameSize = WMA_DEC_FRAMESIZE;
			break;
	#endif	
	#if APP_MP3_DECODE_EN == 1
		case MP3:
			audio_info->DataRate = mp3_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info->Channel = mp3_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info->SampleRate = mp3_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info->FrameSize = MP3_DEC_FRAMESIZE;
			break;
	#endif	
	#if APP_WAV_CODEC_EN == 1
		case WAV:
			audio_info->DataRate = wav_dec_get_nAvgBytesPerSec((INT8U*)audio_ctrl.work_mem) * 8;	//???
			audio_info->Channel = wav_dec_get_nChannels((INT8U*)audio_ctrl.work_mem);
			audio_info->SampleRate = wav_dec_get_SampleRate((INT8U*)audio_ctrl.work_mem);
			//audio_info->FrameSize = WAV_DEC_FRAMESIZE;
			break;
	#endif	
	#if APP_A1800_DECODE_EN == 1
		case A1800:
			audio_info->DataRate = A18_dec_get_bitrate((void *)audio_ctrl.work_mem);
			audio_info->Channel = 1;
			audio_info->SampleRate = 16000;
			//audio_info->FrameSize = A1800_DEC_FRAMESIZE;
			audio_info->FrameSize = audio_info->DataRate / 8/ (audio_info->SampleRate / A18_DEC_FRAMESIZE);
			break;
	#endif	
	#if APP_A1600_DECODE_EN == 1
		case A1600:
			audio_info->DataRate = A16_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info->Channel = A16_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info->SampleRate = A16_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info->FrameSize = A16_DEC_FRAMESIZE;
			audio_info->FrameSize = audio_info->DataRate / 8/ (audio_info->SampleRate / A16_DEC_FRAMESIZE);
			break;
	#endif	
	#if APP_S880_DECODE_EN == 1
		case S880:
			audio_info->DataRate = S880_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info->Channel = S880_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info->SampleRate = S880_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info->FrameSize = S880_DEC_FRAMESIZE;
			audio_info->FrameSize = audio_info->DataRate / 8/ (audio_info->SampleRate / S880_DEC_FRAMESIZE);
			break;
	#endif	
	#if APP_A6400_DECODE_EN == 1
		case A6400:
			audio_info->DataRate = a6400_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info->Channel = a6400_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info->SampleRate = a6400_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info->FrameSize = A6400_DEC_FRAMESIZE;
			break;
	#endif	
		default:
			break;
	}
	
	//decode_end = NULL;//091127
	if(flag)
	{
		audio_decode_stop(arg);
	}
	arg.volume = temp_volume;
}
//=============================================================================
INT32U audio_decode_GetTotalTime(INT16S filehandle, AUDIO_FORMAT audioformat)
{

	AUDIO_INFO		audio_info_tmp;
	AUDIO_ARGUMENT 	audio_argument_tmp;
	MEDIA_SOURCE	media_source_tmp;
	struct sfn_info tmp_sfn_file;
	INT32U TotalTime = 0;
	INT16S ret;

	switch(audioformat)
	{
#if APP_MP3_DECODE_EN == 1
		case MP3:
			TotalTime = audio_total_time/10;
			break;
#endif
#if APP_WMA_DECODE_EN
		case WMA:
			TotalTime=audio_total_time/10;
#endif
		default:
			ret = sfn_stat(filehandle, &tmp_sfn_file);	
			if(ret < 0)
				return 0;
			
			media_source_tmp.type_ID.FileHandle = filehandle;
			media_source_tmp.type = SOURCE_TYPE_FS;
			media_source_tmp.Format.AudioFormat = audioformat;
			audio_rampdown_disable_flag = 1;
			audio_decode_get_info(audio_argument_tmp, media_source_tmp, &audio_info_tmp);
			audio_rampdown_disable_flag = 0;
		
			TotalTime = ((tmp_sfn_file.f_size)*8)/audio_info_tmp.DataRate;
			break;
	}

	return TotalTime;
}

//=============================================================================
INT32U audio_decode_GetCurTime(INT16S filehandle, AUDIO_FORMAT audioformat)
{

	AUDIO_INFO		audio_info_tmp;
	struct sfn_info tmp_sfn_file;
	INT32U CurTime = 0;
	INT32U CurLength;
	INT16S ret;

	ret = sfn_stat(filehandle, &tmp_sfn_file);	
	if(ret < 0)
		return 0;
	CurLength = lseek(filehandle, 0, SEEK_CUR);
	
	switch(audioformat)
	{
		case MIDI:
			//
			break;
	#if APP_WMA_DECODE_EN == 1
		case WMA:
			audio_info_tmp.DataRate = wma_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.Channel = wma_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.SampleRate = wma_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info_tmp.FrameSize = WMA_DEC_FRAMESIZE;
			break;
	#endif
	#if APP_MP3_DECODE_EN == 1
		case MP3:
//			audio_info_tmp.DataRate = mp3_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
//			audio_info_tmp.Channel = mp3_dec_get_channel((CHAR*)audio_ctrl.work_mem);
//			audio_info_tmp.SampleRate = mp3_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info.FrameSize = MP3_DEC_FRAMESIZE;
			CurTime = (INT32U)((unsigned long long int)audio_total_time*(CurLength - mp3_ID3V2_length)/(tmp_sfn_file.f_size - mp3_ID3V2_length));
			//return CurTime;
			break;
	#endif
	#if APP_WAV_CODEC_EN == 1
		case WAV:
			#if 0
			audio_info_tmp.DataRate = wav_dec_get_nAvgBytesPerSec((INT8U*)audio_ctrl.work_mem) * 8;	//???
			audio_info_tmp.Channel = wav_dec_get_nChannels((INT8U*)audio_ctrl.work_mem);
			audio_info_tmp.SampleRate = wav_dec_get_SampleRate((INT8U*)audio_ctrl.work_mem);
			//audio_info.FrameSize = WAV_DEC_FRAMESIZE;
			#else
			audio_info_tmp.SampleRate = wav_dec_get_SampleRate((INT8U*)audio_ctrl.work_mem);
			CurTime = audio_decode_sample_number/audio_info_tmp.SampleRate;//second
			#endif
			break;
	#endif
	#if APP_A1800_DECODE_EN == 1
		case A1800:
			#if 0
			audio_info_tmp.DataRate = A18_dec_get_bitrate((void *)audio_ctrl.work_mem);
			audio_info_tmp.Channel = 1;
			audio_info_tmp.SampleRate = 16000;
			//audio_info_tmp.FrameSize = A1800_DEC_FRAMESIZE;
			audio_info_tmp.FrameSize = audio_info_tmp.DataRate / 8/ (audio_info_tmp.SampleRate / A18_DEC_FRAMESIZE);
			#else
			audio_info_tmp.SampleRate = A18_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			CurTime = audio_decode_sample_number/audio_info_tmp.SampleRate;//second
			#endif
			break;
	#endif
	#if APP_A1600_DECODE_EN == 1
		case A1600:
			audio_info_tmp.DataRate = A16_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.Channel = A16_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.SampleRate = A16_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info_tmp.FrameSize = A16_DEC_FRAMESIZE;
			audio_info_tmp.FrameSize = audio_info_tmp.DataRate / 8/ (audio_info_tmp.SampleRate / A16_DEC_FRAMESIZE);
			break;
	#endif
	#if APP_S880_DECODE_EN == 1
		case S880:
			audio_info_tmp.DataRate = S880_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.Channel = S880_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.SampleRate = S880_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info_tmp.FrameSize = S880_DEC_FRAMESIZE;
			audio_info_tmp.FrameSize = audio_info_tmp.DataRate / 8/ (audio_info_tmp.SampleRate / S880_DEC_FRAMESIZE);
			break;
	#endif
	#if APP_A6400_DECODE_EN == 1
		case A6400:
			#if 0
			audio_info_tmp.DataRate = a6400_dec_get_bitrate((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.Channel = a6400_dec_get_channel((CHAR*)audio_ctrl.work_mem);
			audio_info_tmp.SampleRate = a6400_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			//audio_info_tmp.FrameSize = A6400_DEC_FRAMESIZE;
			#else
			audio_info_tmp.SampleRate = a6400_dec_get_samplerate((CHAR*)audio_ctrl.work_mem);
			CurTime = audio_decode_sample_number/audio_info_tmp.SampleRate;//second
			#endif
			break;
	#endif
		default:
			break;
	}
	//CurTime = ((CurLength)*8)/audio_info_tmp.DataRate;
	return CurTime;
}

//=============================================================================
//GPL16X interface
//added by Dexter.Ming
//
//=============================================================================
//=============================================================================
INT16U GetSoundLibVersion(void)
{
	INT16U version = 0x0127;
	DBG_PRINT("SoundLibVersion = %d\r\n",version);
	return version;
}

//=============================================================================
void Snd_SetVolume(INT8U CodecType, INT8U vol)
{
	if (vol <= 63) {
		//aud_status.volume = vol;
		G_audio_argument.volume = vol;
		//music_para.volume = aud_status.volume*VOLUME_STEP;
		DBG_PRINT("volume = %d\r\n",G_audio_argument.volume);
		audio_decode_volume_set(&G_audio_argument, vol);
	}
}

//=============================================================================
INT8U Snd_GetVolume(INT8U CodecType)
{
	return G_audio_argument.volume;
}

//=============================================================================
INT16S Snd_Stop(INT8U CodecType)
{
	if(G_media_source.Format.AudioFormat==CodecType)
	{
		audio_decode_stop(G_audio_argument);
	}
	else
	{
		return -1;
	}
	return 0;
}

//=============================================================================
INT16S Snd_Pause(INT8U CodecType)
{
	if(G_media_source.Format.AudioFormat==CodecType)
	{
		audio_decode_pause(G_audio_argument);
	}
	else
	{
		return -1;
	}
	return 0;
}

//=============================================================================
INT16S Snd_Resume(INT8U CodecType)
{
	if(G_media_source.Format.AudioFormat==CodecType)
	{
		audio_decode_resume(G_audio_argument);
	}
	else
	{
		return -1;
	}
	return 0;
}


//=============================================================================
INT16U Snd_GetStatus(INT8U CodecType)
{
	INT16U R_Return;
	R_Return = 1;
	return R_Return;
}

//=============================================================================
INT16S Snd_Init(void)
{
 	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	return 0;
}

//=============================================================================
INT16S Snd_Play(SND_INFO *sndinfo)
{
	G_media_source.type = sndinfo->Srctype;//SOURCE_TYPE_USER_DEFINE;//SOURCE_TYPE_FS;
	G_media_source.Format.AudioFormat = sndinfo->AudioFormat;	//A1800
	
	G_audio_argument.Main_Channel = 1;		// Use DAC Channel A+B
	G_audio_argument.L_R_Channel = 3;		// Left + Right Channel
	G_audio_argument.mute = 0;
	G_audio_argument.volume = 15;			// volume level = 0~63

	if(sndinfo->Srctype == SOURCE_TYPE_FS)
	{	// use FS can not use jump play function
		G_SACM_Ctrl.Offset = 0;
		G_SACM_Ctrl.Offsettype = SND_OFFSET_TYPE_NONE;
	}
	else if(sndinfo->Srctype == SOURCE_TYPE_USER_DEFINE)
	{	// support jmp to play
		if((G_media_source.Format.AudioFormat == A1600)||(G_media_source.Format.AudioFormat == S880))
		{
			G_SACM_Ctrl.Offset = 0;
			if(sndinfo->Offsettype != SND_OFFSET_TYPE_NONE)
				sndinfo->Offsettype = SND_OFFSET_TYPE_HEAD;
		}
		
		if((sndinfo->Offsettype == SND_OFFSET_TYPE_NONE)||(sndinfo->Offsettype == SND_OFFSET_TYPE_HEAD))
			G_SACM_Ctrl.Offset = 0;
		else 
			G_SACM_Ctrl.Offset = sndinfo->Offset;
	}
	else
		return -1;
		
	G_SACM_Ctrl.Offsettype = sndinfo->Offsettype;
	G_SACM_Ctrl.AudioFormat = sndinfo->AudioFormat;
	G_snd_info.Speed = sndinfo->Speed;
	G_snd_info.Pitch = sndinfo->Pitch;
	
	G_audio_status = audio_decode_start(G_audio_argument, G_media_source);
	return 0;
}

//=============================================================================
INT16S Snd_GetNodeInfo(INT8U CodecType)
{
	return G_audio_argument.volume;
}


//=============================================================================
INT32U Snd_GetTotalTime(INT16S filehandle, AUDIO_FORMAT audioformat)
{
	return audio_decode_GetTotalTime(filehandle, audioformat);
}
//-----------------------------------------------------------------------------
INT32U Snd_GetCurTime(INT16S filehandle, AUDIO_FORMAT audioformat)
{
	return audio_decode_GetCurTime(filehandle, audioformat);
}



