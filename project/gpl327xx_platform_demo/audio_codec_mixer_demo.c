#include "application.h"
#include "drv_l2_ad_key_scan.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1		

//=================================================================================================
//	Audio Decode demo code
//=================================================================================================
void Audio_Decode_Mixer_Demo(void)
{
	char path_mp3[24];
	char path_midi[24];
	char path_adpcm[24];
	INT32S fd_adpcm_comb,adpcm_index;

	AUDIO_ARGUMENT 	audio_argument_0;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	AUDIO_ARGUMENT 	midi_argument;
	MEDIA_SOURCE	media_source_midi;
	AUDIO_CODEC_STATUS 	midi_status;
	
	while(1)
	{	
		if(_devicemount(FS_SD))// mount disk
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
	#if	USE_DISK == FS_NAND1
			DBG_PRINT("NAND LOW LEVEL FORMAT = %d \r\n", DrvNand_lowlevelformat());	
			DBG_PRINT("Format NAND = %d\r\n", _format(FS_NAND1, FAT32_Type));	
			DrvNand_flush_allblk();	
	#else			
			_deviceunmount(USE_DISK);
	#endif			
		}
		else
		{
			DBG_PRINT("Mount DISK Success[%d]!\r\n", USE_DISK);
			break;
		}
	}
	
	switch(USE_DISK)
	{
		case FS_SD:	
			gp_memcpy((INT8S*)path_mp3, (INT8S*)"C:\\happy.mp3", sizeof(path_mp3));
			gp_memcpy((INT8S*)path_midi, (INT8S*)"C:\\happy.idi", sizeof(path_midi));
			gp_memcpy((INT8S*)path_adpcm, (INT8S*)"C:\\adpcm.bin", sizeof(path_adpcm));
			break;
		case FS_NAND1:	
			gp_memcpy((INT8S*)path_mp3, (INT8S*)"A:\\happy.mp3", sizeof(path_mp3));
			gp_memcpy((INT8S*)path_midi, (INT8S*)"A:\\happy.idi", sizeof(path_midi));
			gp_memcpy((INT8S*)path_adpcm, (INT8S*)"A:\\adpcm.bin", sizeof(path_adpcm));
			break;	
		case FS_USBH:	
			gp_memcpy((INT8S*)path_mp3, (INT8S*)"G:\\happy.mp3", sizeof(path_mp3));
			gp_memcpy((INT8S*)path_midi, (INT8S*)"G:\\happy.idi", sizeof(path_midi));
			gp_memcpy((INT8S*)path_adpcm, (INT8S*)"G:\\adpcm.bin", sizeof(path_adpcm));
			break;		
	}

	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	adc_key_scan_init();

	while(1)
	{
		adc_key_scan();

		if(ADKEY_IO1)			// Start to play MP3
		{	
			media_source_0.type_ID.FileHandle = open(path_mp3, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = MP3;
			
			audio_argument_0.Main_Channel = 1;		// Use DAC Channel A+B
			audio_argument_0.L_R_Channel = 3;		// Left + Right Channel
			audio_argument_0.mute = 0;
			audio_argument_0.volume = 15;			// volume level = 0~63

			G_snd_info.Speed = 12;					//for speed control(golbal var)
			G_snd_info.Pitch = 12;					//for pitch control(golbal var)
		
			audio_status_0 = audio_decode_start(audio_argument_0, media_source_0);
		}
		else if(ADKEY_IO2)		// Stop MP3
		{
			audio_decode_stop(audio_argument_0);
		}
		else if(ADKEY_IO3)		// Start to play MIDI
		{
			media_source_midi.type_ID.FileHandle = open(path_midi, O_RDONLY);
			media_source_midi.type = SOURCE_TYPE_FS;
			media_source_midi.Format.AudioFormat = MIDI;
			
			midi_argument.Main_Channel = 2;		// Use SPU Channels
			midi_argument.L_R_Channel = 3;		// Left + Right Channel
			midi_argument.mute = 0;
			midi_argument.volume = 127;			// volume level = 0~127
			midi_argument.midi_index = 0;
			
			midi_status = audio_decode_start(midi_argument, media_source_midi);
		}
		else if(ADKEY_IO4)		// Stop MIDI
		{
			audio_decode_stop(midi_argument);	
		}
		else if(ADKEY_IO5)		// Play DRM
		{		
			fd_adpcm_comb = open(path_adpcm, O_RDONLY);
			if(adpcm_index == 0)
				adpcm_index = 1;
			else
				adpcm_index = 0;
			SPU_Set_adpcm_comb_addr(0, 0);
			SPU_load_comb_adpcm_to_RAM(fd_adpcm_comb, adpcm_index, 3);//mode 0: from nand flash logic area; 2: nand flash app area, 3:from FS
			close(fd_adpcm_comb);
			SPU_play_comb_adpcm_FixCH(64, 100, 29); 
		}
		else if(ADKEY_IO6)
		{
		}
		else if(ADKEY_IO7)
		{
		}
		else if(ADKEY_IO8)		// Exit audio decoder demo
		{
			if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING)
				audio_decode_stop(audio_argument_0);
				
			break;
		}
	}
	
	SPU_Free_adpcm_data_temp_buffer();
	audio_decode_exit();	// Release the resources needed by audio decoder
}

