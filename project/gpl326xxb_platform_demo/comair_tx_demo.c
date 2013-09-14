#include "application.h"
#include "drv_l2_ad_key_scan.h"


#define USE_DISK	FS_SD

#define C_FILE_NODE_SIZE			500

INT16U	file_node_buf_1[C_FILE_NODE_SIZE];
struct sfn_info file_fs_info;
struct STFileNodeInfo musicFNodeInfo;

#define FILE_NAME_TEST	1

#if FILE_NAME_TEST
f_ppos sFile;
struct f_info file_info_test;
#endif

static void comair_tx_show_menu(void)
{
	DBG_PRINT("**********************************************************************\r\n");
	DBG_PRINT("*                  		COMAIR TX demo		  		                *\r\n");
	DBG_PRINT("**********************************************************************\r\n");
	DBG_PRINT("DEMO Requirement:	\r\n");
	DBG_PRINT("OV7670 VGA Sensor:	\r\n");
	DBG_PRINT("SD Card with camera_ok.drm and camera_ng.drm	\r\n");
	DBG_PRINT("KEY1    Play a wav file\r\n");
	DBG_PRINT("KEY2    Send a single COMAIR command\r\n");
	DBG_PRINT("KEY3    Stop or restart COMAIR timers\r\n");
	DBG_PRINT("KEY4    Stop decoding the wav file\r\n");
	DBG_PRINT("KEY5    Volume Up\r\n");
	DBG_PRINT("KEY6    Volume Down\r\n");
	DBG_PRINT("KEY7    Pause/ Resume Audio Playback\r\n");
	DBG_PRINT("KEY8    Send a Multiple COMAIR command\r\n");

	DBG_PRINT("\r\n");
}
//=================================================================================================
//	COMAIR Send Command demo code
//=================================================================================================
void COMAIR_SendCmd_Demo(void)
{
	
	INT32S temp_volume;
	AUDIO_ARGUMENT 	audio_argument_0;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;

	INT8U mp3_play_index=0;
	INT32U folder_nums, file_nums;
	INT8U cmd8=0xC1;
	INT16U cmd16=0xC0C1;

	while(1)
	{	
		if(_devicemount(USE_DISK))// mount disk
		{
			DBG_PRINT("Mount DISK Fail[%d]!\r\n", USE_DISK);
			_deviceunmount(USE_DISK);
		}
		else
		{
			DBG_PRINT("Mount DISK Success[%d]!\r\n", USE_DISK);
			break;
		}
	}
	comair_tx_show_menu();
	
	musicFNodeInfo.disk = USE_DISK;
	gp_strcpy((INT8S *) musicFNodeInfo.extname, (INT8S *) "wav");
	musicFNodeInfo.FileBuffer = file_node_buf_1;
	musicFNodeInfo.FileBufferSize = C_FILE_NODE_SIZE;
	musicFNodeInfo.FolderBuffer = NULL;
	musicFNodeInfo.FolderBufferSize = 0;
	musicFNodeInfo.filter			=NULL;
	GetFileNumEx(&musicFNodeInfo, &folder_nums, &file_nums);

	adc_key_scan_init();
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	comair_tx_init();			// Initialize COMAIR settings

	set_comair_pincode_cmd_gen(0x0000);		/* Setup COMAIR pin code with 0x0000 */


	audio_argument_0.Main_Channel = 1;		// Use DAC Channel A+B
	audio_argument_0.L_R_Channel = 3;		// Left + Right Channel
	audio_argument_0.mute = 0;
	audio_argument_0.volume = 63;			// volume level = 0~63
	audio_argument_0.midi_index = 1;		// midi use

	G_snd_info.Speed = 12;					//for speed control(golbal var)
	G_snd_info.Pitch = 12;					//for pitch control(golbal var)

	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)	// Start to decode the specified file, wav files
		{
//			audio_decode_mute(&audio_argument_0);

			if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING)
				audio_decode_stop(audio_argument_0);

			DBG_PRINT("audio_decode_start :: wav files\r\n");

#if FILE_NAME_TEST
			sFile = GetFileNodeInfo(&musicFNodeInfo, mp3_play_index, &file_fs_info);
			GetFileInfo(sFile, &file_info_test);
			DBG_PRINT("FULL_FILE = %s\r\n", (char *)file_info_test.f_name);
#else
			GetFileNodeInfo(&musicFNodeInfo, mp3_play_index, &file_fs_info); 
			DBG_PRINT("FILE = %s\r\n", (char *)file_fs_info.f_name);
#endif
			mp3_play_index++;
			if(mp3_play_index >= file_nums)
				mp3_play_index = 0;

			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = WAV;//MP3;
			media_source_0.type_ID.FileHandle = sfn_open(&file_fs_info.f_pos);
			if ( media_source_0.type_ID.FileHandle < 0 )
			{
				DBG_PRINT("file open fail\r\n");
				close(media_source_0.type_ID.FileHandle);
				continue;
			}
			audio_status_0 = audio_decode_start(audio_argument_0, media_source_0);


		}
		else if(ADKEY_IO4)		// Stop decoding the current file
		{
			audio_decode_stop(audio_argument_0);
			close(media_source_0.type_ID.FileHandle);
		}
		else if(ADKEY_IO7)		// Pause/Resume the decoder
		{
			if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING)
			{
				audio_decode_pause(audio_argument_0);
			}
			else if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESS_PAUSED)
			{
				audio_decode_resume(audio_argument_0);	
			}
		}
		else if(ADKEY_IO5)		// Volume up
		{
			temp_volume = audio_argument_0.volume;
			temp_volume += 4;
			if(temp_volume > 63)
				temp_volume = 63;
			
			audio_decode_volume_set(&audio_argument_0, temp_volume);
		}
		else if(ADKEY_IO6)		// Volume down
		{		
			temp_volume = audio_argument_0.volume;
			if(temp_volume == 63)
				temp_volume -= 3;
			else
				temp_volume -= 4;
			if(temp_volume < 0)
				temp_volume = 0;
				
			audio_decode_volume_set(&audio_argument_0, temp_volume);
		}
		else if(ADKEY_IO2)		// Send single COMAIR command
		{
			if ( (cmd8 > 0xFF) || (cmd8 < 0xC0) )
				cmd8 = 0xC1;

			if (send_comair_cmd_single_cmd(cmd8) == COMAIR_SEND_ERROR_CMD_START_TO_SEND)
			{
				DBG_PRINT("===COMAIR=== sent single cmd: 0x%X\r\n", cmd8);
				cmd8++;
			}
			else
			{
				DBG_PRINT("===COMAIR=== single-cmd:: failed to send command\r\n");
			}
		}
		else if(ADKEY_IO8)		// Send multiple COMAIR commands
		{
			if ( (cmd16 > 0xFFFF) || (cmd16 < 0xC0C1))
				cmd16 = 0xC0C1;

			if (send_comair_cmd_multi_cmd(cmd16) != COMAIR_SEND_ERROR_CMD_START_TO_SEND)
			{
				DBG_PRINT("===COMAIR=== multi-cmd:: failed to send command\r\n");
			}
			else
			{
				DBG_PRINT("===COMAIR=== sent multiple cmds: 0x%X\r\n", cmd16);
				cmd16 += 0x0202;
			}
		}
		else if(ADKEY_IO3)		// stop or restart COMAIR timers
		{
			if ( get_comair_timer_status() )
				stop_comair_timer();
			else
				start_comair_timer();
		}
	}
	
	audio_decode_exit();	// Release the resources needed by audio decoder
}


