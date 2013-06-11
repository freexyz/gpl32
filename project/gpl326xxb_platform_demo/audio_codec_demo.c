#include <stdio.h>
#include <string.h>

#include "application.h"
#include "drv_l2_ad_key_scan.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1
//#define USE_DISK	FS_USBH
#define USE_ADKEY	1

//add WMA_Seek
OS_EVENT  *Mbox_WMA_Play_Seek_Flag;
OS_EVENT  *Mbox_WMA_Play_Seek_Offset;
INT8U     WMA_Play_Seek_OK;
INT32U    seek_offset;


typedef struct FILELIST{
        char data[128];
        struct FILELIST* next;
        struct FILELIST* prev;
}filelist;

filelist *list_head = NULL;

extern INT8U g_user_read_data_mode;
extern INT32U global_user_read_data_addr;
//=================================================================================================
//	Audio Decode demo code
//=================================================================================================
void Audio_Decode_Demo(void)
{
	AUDIO_ARGUMENT 	audio_argument_0;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	AUDIO_ARGUMENT 	audio_argument_1;
	MEDIA_SOURCE	media_source_1;
	AUDIO_CODEC_STATUS 	audio_status_1;
	
	AUDIO_ARGUMENT 	audio_argument_2;
	MEDIA_SOURCE	media_source_2;
	AUDIO_CODEC_STATUS 	audio_status_2;
	
	char path[24];
	INT8U adpcm_index;
	INT16S fd_adpcm, volume;
	INT32U snd_buffer_addr[3];
	INT32S ret;
	int i;
	struct sfn_info file_info;

	while(1)
	{	
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
		#if USE_DISK == FS_NAND1
			ret = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();  
			_deviceunmount(FS_NAND1); 
		#endif
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}

	switch(USE_DISK)
	{
	case FS_SD:	
		gp_memcpy((INT8S*)path, (INT8S*)"C:\\happy.mp3", sizeof(path));
		break;
	case FS_NAND1:	
		gp_memcpy((INT8S*)path, (INT8S*)"A:\\happy.mp3", sizeof(path));
		break;	
	case FS_USBH:	
		gp_memcpy((INT8S*)path, (INT8S*)"G:\\happy.mp3", sizeof(path));
		break;		
	}
		
	//init varaible
	audio_argument_0.Main_Channel = 0;
	audio_argument_1.Main_Channel = 1;
	audio_argument_2.Main_Channel = 2;
	media_source_0.type = SOURCE_TYPE_FS;
	media_source_1.type = SOURCE_TYPE_FS;
	media_source_2.type = SOURCE_TYPE_FS;
	audio_status_0 = AUDIO_CODEC_PROCESS_END;
	audio_status_1 = AUDIO_CODEC_PROCESS_END;
	audio_status_2 = AUDIO_CODEC_PROCESS_END;
	
	adpcm_index = 0;
	volume = 0;
	fd_adpcm = -1;
	snd_buffer_addr[0] = 0;
	snd_buffer_addr[1] = 0;
	snd_buffer_addr[2] = 0;
	file_info.f_size = 0;
	ret = 0;
	
#if USE_ADKEY == 1	
	adc_key_scan_init();
#endif	
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	
	while(1)
	{
		//---------------------------------------------在指定的通道播放合并的adpcm文件
		#if 0	
			if(SPU_Get_SingleChannel_Status(15) == 0)
			{
				if(snd_buffer_addr[0] == 0)
				{
					snd_buffer_addr[0] = (INT32U)gp_malloc_align(0x40000, 4); //256kbyte
					while(snd_buffer_addr[0] == 0);
				}
				
				fd_adpcm = open("C:\\adpcm_comb1.bin", O_RDONLY);
				if(fd_adpcm <0)
					while(1);
					
				if(++adpcm_index > 3)
					adpcm_index = 0;
				
				SPU_Set_adpcm_comb_addr(0, snd_buffer_addr[0]);
				SPU_load_comb_adpcm_to_RAM(fd_adpcm, adpcm_index, 3);
				close(fd_adpcm);
				//SPU_play_comb_adpcm_FixCH(64, 100, 29);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)snd_buffer_addr[0], 64, 100, 15);
			}
			if(SPU_Get_SingleChannel_Status(14) == 0)
			{
				if(snd_buffer_addr[1] == 0)
				{
					snd_buffer_addr[1] = (INT32U)gp_malloc_align(0x40000, 4);//256kbyte
					while(snd_buffer_addr[1] == 0);
				}
				
				fd_adpcm = open("C:\\adpcm_comb2.bin", O_RDONLY);
				if(fd_adpcm <0)
					while(1);
				
				if(++adpcm_index > 3)
					adpcm_index = 0;
					
				SPU_Set_adpcm_comb_addr(0, snd_buffer_addr[1]);
				SPU_load_comb_adpcm_to_RAM(fd_adpcm, adpcm_index, 3);
				close(fd_adpcm);
				//SPU_play_comb_adpcm_FixCH(64, 100, 29);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)snd_buffer_addr[1], 64, 100, 14);
			}
		#endif
		//-----------------------------------------------------repeat play midi
		//---------------------------------------------在指定的通道循环播放drm
		#if 0	
			if(SPU_Get_SingleChannel_Status(13) == 0)
			{
				fd_adpcm = open("C:\\camera.drm", O_RDONLY);
				if(fd_adpcm<0)
					while(1);
					
				sfn_stat(fd_adpcm, &file_info);
				if(snd_buffer_addr[2])
					gp_free((void*)snd_buffer_addr[2]);	
				
				snd_buffer_addr[2] = (INT32U)gp_malloc_align(file_info.f_size, 4);//256kbyte
				while(snd_buffer_addr[2] == 0);
				
				ret = (INT32S)read(fd_adpcm, (INT32U)snd_buffer_addr[2], file_info.f_size);
				if(ret != file_info.f_size)
					while(1);
					
				close(fd_adpcm);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)snd_buffer_addr[2], 64, 100, 13);
			}
		#endif
		//-----------------------------------------------------repeat play midi
		//---------------------------------------------------------------------测试播放sdram中的WAV数据
		#if 0
			if(audio_decode_status(audio_argument_1) == AUDIO_CODEC_PROCESS_END)
			{
				media_source_1.Format.AudioFormat = MP3;
				audio_argument_1.Main_Channel = 1;// 1:Use DAC Channel A+B  2:for MIDI play only
				media_source_1.type_ID.FileHandle = -1;
				media_source_1.type = SOURCE_TYPE_USER_DEFINE;
				audio_argument_1.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_1.mute = 0;
				audio_argument_1.volume = 15;			// volume level = 0~63
				G_snd_info.Speed = 12;					//for speed control(golbal var)
				G_snd_info.Pitch = 12;					//for pitch control(golbal var)
				g_user_read_data_mode = 6;//从SDRAM中播AUDIO数据
				g_audio_data_length = 293600;//282440;//声音数据的长度
				global_user_read_data_addr = 0x800000;//声音数据在SDRAM中的起始地址
				audio_status_1 = audio_decode_start(audio_argument_1, media_source_1);
			}
		#endif
		//---------------------------------------------------------------------循环播放mp3等音频
		#if 0
			if(audio_decode_status(audio_argument_1) == AUDIO_CODEC_PROCESS_END)//play speech
			{
				media_source_1.type_ID.FileHandle = open("C://happy.a18", O_RDONLY);
				media_source_1.type = SOURCE_TYPE_FS;
				media_source_1.Format.AudioFormat = A1800;
				audio_argument_1.Main_Channel = 0;		// Use SPU Channel A+B
				audio_argument_1.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_1.mute = 0;
				audio_argument_1.volume = 30;			// volume level = 0~32
				G_snd_info.Speed = 12;					//for speed control(golbal var)
				G_snd_info.Pitch = 12;					//for pitch control(golbal var)
				audio_status_1 = audio_decode_start(audio_argument_1, media_source_1);
			}
		#elif 0
			if(audio_decode_status(audio_argument_1) == AUDIO_CODEC_PROCESS_END)
			{
				nvmemory_init();
				media_source_1.type_ID.FileHandle = (INT16S)nv_open((INT8U*)"HAPPY.A18");
				media_source_1.type = SOURCE_TYPE_NVRAM;
				media_source_1.Format.AudioFormat = A1800;
				audio_argument_1.Main_Channel = 0;		// Use SPU Channel A+B
				audio_argument_1.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_1.mute = 0;
				audio_argument_1.volume = 63;			// midi volume 0~127
				audio_status_1 = audio_decode_start(audio_argument_1, media_source_1);
			}
		#endif
		//-----------------------------------------------------repeat play midi
		#if 0
		if(audio_decode_status(audio_argument_2) == AUDIO_CODEC_PROCESS_END)
		{
			#if 1//play midi from FS
				media_source_2.type = SOURCE_TYPE_FS;
				media_source_2.type_ID.FileHandle = open("C://happy.gmd", O_RDONLY);
				media_source_2.type_ID.memptr = 0x0; 	
				media_source_2.Format.AudioFormat = MIDI;
				audio_argument_2.Main_Channel = 2;		// 2:for MIDI play only
				audio_argument_2.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_2.mute = 0;
				audio_argument_2.volume = 100;			// midi volume 0~127
				audio_argument_2.midi_index = 1;		// midi index in "happy.gmd"
				audio_status_2 = audio_decode_start(audio_argument_2, media_source_2);
				SPU_MIDI_Repeat(0);						// repeat disable
			#elif 0//play midi use nv_open/nv_read
				nvmemory_init();
				media_source_2.type = SOURCE_TYPE_NVRAM;
				media_source_2.type_ID.FileHandle = (INT16S)nv_open((INT8U*)"HAPPY.GMD");
				media_source_2.type_ID.memptr = 0x0;
				media_source_2.Format.AudioFormat = MIDI;
				audio_argument_2.Main_Channel = 2;		// 2:for MIDI play only
				audio_argument_2.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_2.mute = 0;
				audio_argument_2.volume = 63;			// midi volume 0~127
				audio_argument_2.midi_index = 2;		// midi index in "happy.gmd"
				audio_status_2 = audio_decode_start(audio_argument_2, media_source_2);
				SPU_MIDI_Repeat(0);						// repeat disable			
			#else //play midi in nand app area fix addr 
				media_source_2.type = SOURCE_TYPE_USER_DEFINE;
				media_source_2.type_ID.FileHandle = -1;
				media_source_2.type_ID.memptr = IDI_TO_NAND_ADDR; //"happy.gmd" in nand app address
				media_source_2.Format.AudioFormat = MIDI;
				audio_argument_2.Main_Channel = 2; 		// 2:for MIDI play only
				audio_argument_2.L_R_Channel = 3;		// Left + Right Channel
				audio_argument_2.mute = 0;				
				audio_argument_2.volume = 63;			// midi volume 0~127
				audio_argument_2.midi_index = 3;		// midi index in "happy.gmd"
				audio_status_2 = audio_decode_start(audio_argument_2, media_source_2);
				SPU_MIDI_Repeat(1);						// repeat enable	
			#endif
		}
		#endif
		//---------------------------------------------------------------------------------------
		OSTimeDly(3);
	#if USE_ADKEY == 1			
		adc_key_scan();
	#else	
		key_Scan();
	#endif
	#if USE_ADKEY == 1	
		if(ADKEY_IO1)	// Start to decode the specified file
	#else	
		if(IO1)
	#endif		
		{	
			#if 1
			media_source_0.type_ID.FileHandle = open(path, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = MP3;
			
			audio_argument_0.Main_Channel = 1;		// Use DAC Channel A+B
			audio_argument_0.L_R_Channel = 3;		// Left + Right Channel
			audio_argument_0.mute = 0;
			audio_argument_0.volume = 63;			// volume level = 0~63
			audio_argument_0.midi_index = 1;		// midi use

			G_snd_info.Speed = 12;					//for speed control(golbal var)
			G_snd_info.Pitch = 12;					//for pitch control(golbal var)

			audio_status_0 = audio_decode_start(audio_argument_0, media_source_0);
			#else
			gp_memcpy((INT8S*)path, (INT8S*)"C:\\happy.a64", sizeof(path));
			media_source_0.type_ID.FileHandle = open(path, O_RDONLY);
			sfn_stat(media_source_0.type_ID.FileHandle, &file_info);
			
			media_source_0.type = SOURCE_TYPE_USER_DEFINE;//SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = A6400;
			
			audio_argument_0.Main_Channel = 1;		// Use DAC Channel A+B
			audio_argument_0.L_R_Channel = 3;		// Left + Right Channel
			audio_argument_0.mute = 0;
			audio_argument_0.volume = 63;			// volume level = 0~63
			audio_argument_0.midi_index = 1;		// midi use
			audio_argument_0.data_start_addr = (INT8U*) gp_malloc_align(file_info.f_size, 32);
			read(media_source_0.type_ID.FileHandle, (INT32U)audio_argument_0.data_start_addr, file_info.f_size);
			audio_argument_0.data_offset = 0;
			audio_argument_0.data_len = file_info.f_size;

			G_snd_info.Speed = 12;					//for speed control(golbal var)
			G_snd_info.Pitch = 12;					//for pitch control(golbal var)

			audio_status_0 = audio_decode_start(audio_argument_0, media_source_0);
			#endif
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO2)		// Stop decoding the current file
	#else
		else if(IO2)
	#endif		
		{
			if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING) 
			{
				audio_decode_stop(audio_argument_0);
				close(media_source_0.type_ID.FileHandle);
			}
			else
			{
				struct f_info   file_info;
			
				if(USE_DISK == FS_SD)
					chdir("C:\\");
				else if(USE_DISK == FS_NAND1)
					chdir("A:\\");
				else if(USE_DISK == FS_USBH)
					chdir("G:\\");
							
				ret = _findnext(&file_info);
				if(ret < 0)
				{
					ret = _findfirst("*.wma*", &file_info, D_ALL);
					if(ret < 0)
					{
						disk_safe_exit(USE_DISK);
						_deviceunmount(USE_DISK);
						while(1)
						{
							if(_devicemount(USE_DISK))
							{
								DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);		
								_deviceunmount(USE_DISK);	
							}
							else
							{
								DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
								break;
							}
						}
					}
				}
                gp_strcpy((INT8S*)path, (INT8S*)file_info.f_name);
                DBG_PRINT("FileName = %s\r\n", file_info.f_name); 

                media_source_0.type_ID.FileHandle = open(path, O_RDONLY);
                media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
                media_source_0.Format.AudioFormat = WMA;
                G_SACM_Ctrl.AudioFormat= WMA;
                G_SACM_Ctrl.Offsettype = SND_OFFSET_TYPE_SIZE;
                G_SACM_Ctrl.Offset     = 0;

                audio_argument_0.Main_Channel = 1;  // Use DAC Channel A+B
                audio_argument_0.L_R_Channel = 3;  // Left + Right Channel
                audio_argument_0.mute = 0;
                audio_argument_0.volume = 1;   // volume level = 0~63

                G_snd_info.Speed = 12;     //for speed control(golbal var)
                G_snd_info.Pitch = 12;     //for pitch control(golbal var)
                WMA_Play_Seek_OK = 0;
                audio_status_0 = audio_decode_start(audio_argument_0, media_source_0);

                for(i=0; i<500; i++)
                  OSTimeDly(1);

                WMA_Play_Seek_OK = 1;
                seek_offset = 15*1000;                                   //seek_time = s*ms
                OSMboxPost(Mbox_WMA_Play_Seek_Offset , &seek_offset);
                OSMboxPost(Mbox_WMA_Play_Seek_Flag , &WMA_Play_Seek_OK);

			}
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO3)		// Pause/Resume the decoder
	#else
		else if(IO3)
	#endif	
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
	#if USE_ADKEY == 1		
		else if(ADKEY_IO4)		// Volume up
	#else
		else if(IO4)
	#endif
		{
			volume = audio_argument_0.volume;
			volume += 4;
			if(volume > 63)
				volume = 63;
			
			audio_argument_0.volume = volume;
			audio_decode_volume_set(&audio_argument_0, volume);	
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO5)		// Volume down
	#else
		else if(IO5)
	#endif
		{		
			volume = audio_argument_0.volume;
			if(volume == 63)
				volume -= 3;
			else
				volume -= 4;
				
			if(volume <= 0)
				volume = 0;
				
			audio_argument_0.volume = volume;
			audio_decode_volume_set(&audio_argument_0, volume);
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO6)		// Mute
	#else
		else if(IO6)
	#endif
		{
			audio_decode_mute(&audio_argument_0);
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO7)		// Un-Mute
	#else
		else if(IO7)
	#endif	
		{
			audio_decode_unmute(&audio_argument_0);
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO8)		// Exit audio decoder demo
	#else 
		else if(IO8)
	#endif
		{
			#if 1
			if(audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING)
				audio_decode_stop(audio_argument_0);
			#else
			if( audio_decode_status(audio_argument_0) == AUDIO_CODEC_PROCESSING) 
			{
				audio_decode_stop(audio_argument_0);
				gp_free(audio_argument_0.data_start_addr);
				close(media_source_0.type_ID.FileHandle);
			}
			#endif
		}
	}
				
	audio_decode_exit();	// Release the resources needed by audio decoder
}

//=================================================================================================
//	Audio Encode demo code
//=================================================================================================

void list_create(char* data)
{
	filelist *temp;

    list_head = (filelist*)gp_malloc(sizeof(filelist));
    list_head->next = list_head;
    list_head->prev = list_head;
	gp_memcpy((INT8S*)&list_head->data, (INT8S*)"head", sizeof(list_head->data));

    temp = list_head;

    temp->next = (filelist*)gp_malloc(sizeof(filelist));
    temp = temp->next;
    temp->next = list_head;
    temp->prev = list_head;
    list_head->prev = temp;
	gp_memcpy((INT8S*)&temp->data, (INT8S*)data, sizeof(temp->data));
	
  	DBG_PRINT("p->data=%s, data=%s\r\n", temp->data, data);
}

void list_insert(char* data)
{
	filelist *p;
	filelist *temp;
	
	p = list_head->prev;

    temp = (filelist*)gp_malloc(sizeof(filelist));
    gp_memcpy((INT8S*)&temp->data, (INT8S*)data, sizeof(temp->data));

	DBG_PRINT("p->data=%s, data=%s\r\n", temp->data, data);

    temp->next = p->next;
    p->next = temp;
    (temp->next)->prev = temp;
    temp->prev = p;

}

void list_disp()
{
	filelist *temp;

    temp = list_head;
    DBG_PRINT("p->data=%s, p=0x%08x, p->next=0x%08x, p->prev=0x%08x\r\n", 
    		temp,
    		temp->data,
    		temp->next,
    		temp->prev);
   
    temp = temp->next;
    while(temp != list_head)
    {
        DBG_PRINT("p->data=%s, p=0x%08x, p->next=0x%08x, p->prev=0x%08x\r\n", 
	        temp,
    		temp->data,
    		temp->next,
    		temp->prev);

        temp = temp->next;
    }
}

int audio_filelist(char* keyword)
{
	struct f_info finfo;
	int idx = 0;

	if (_findfirst (keyword, &finfo, D_ALL))	{
		DBG_PRINT("error:no audio files !!!\r\n");
		return -1;
	}
	
	do	{
		if (idx == 0)
			list_create((char*)&finfo.f_name);
		else
			list_insert((char*)&finfo.f_name);
				
		idx ++;
	}
	while (_findnext (&finfo) == 0);

	list_disp();

	return idx;

}


void audio_encode_playback(char* filename, AUDIO_ARGUMENT* dec_arg)
{
	AUDIO_ARGUMENT 	temp;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	INT32U  dec_type = 0;
	
	INT32U  filename_len = strlen(filename);
		
	//init varaible
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));
	media_source_0.type = SOURCE_TYPE_FS;
	audio_status_0 = AUDIO_CODEC_PROCESS_END;
	
	
	if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'a') && 
		(filename[filename_len-2] == '1') && 
		(filename[filename_len-1] == '8'))
	{
		dec_type = 0;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'w') && 
		(filename[filename_len-2] == 'a') && 
		(filename[filename_len-1] == 'v'))
	{
		dec_type = 1;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'm') && 
		(filename[filename_len-2] == 'p') && 
		(filename[filename_len-1] == '3'))
	{
		dec_type = 2;
	}
		
	switch(dec_type)
	{
		case 0:	// a18
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = A1800;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)

			break;
		case 1:	// pcm, ima_adpcm, ms_adpcm
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = WAV;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)

			break;
			
		case 2:	// mp3
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = MP3;
			
			temp.midi_index = 1;		// midi use

			G_snd_info.Speed = 12;					// for speed control(golbal var)
			G_snd_info.Pitch = 12;					// for pitch control(golbal var)

			break;
		default:
			break;
	}
	
	DBG_PRINT("now decoding %s ... \r\n", filename);
	audio_status_0 = audio_decode_start(temp, media_source_0);	
	if (audio_status_0 != 0)
		DBG_PRINT("audio_decode_error!!!\r\n");
			
	
}

void audio_stop_processing(AUDIO_ARGUMENT* dec_arg)
{

	AUDIO_ARGUMENT temp;
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));

	if (dec_arg->Main_Channel != 255)
	{
		if(audio_decode_status(temp) == AUDIO_CODEC_PROCESSING)
		{
			DBG_PRINT("audio_decode_stop..");
			audio_decode_stop(temp);
			DBG_PRINT("stop decoding!!!\r\n");
		}
		
		dec_arg->Main_Channel = 255;
	}

	if(audio_encode_status() == AUDIO_CODEC_PROCESSING)
	{
		DBG_PRINT("audio_encode_stop..");
		audio_encode_stop();
#if USE_DISK == FS_NAND1	
		DBG_PRINT("Nand flush\r\n");
		DrvNand_flush_allblk();
#endif
		DBG_PRINT("stop encoding!!!\r\n");
	}
}

void Audio_Encode_Demo(void)
{
	char    enc_dir[24];
	char    enc_file[24];
	char    enc_path[64];
	
	INT32S  enc_num = 0;
	INT32S  file_nums = 0;
	INT32U  enc_type = 0;	
	INT32U	cur_state = 0; // 0:encoding 1:decoding
	INT64U  disk_free;	
	
	AUDIO_ARGUMENT 	audio_argument;
	MEDIA_SOURCE	media_source;

	INT32S ret;
	
	filelist *temp = NULL;
	
	audio_argument.Main_Channel = 255;

	while(1)
	{	
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
		#if	USE_DISK == FS_NAND1
			ret = _format(FS_NAND1, FAT32_Type); 
			DrvNand_flush_allblk();
			_deviceunmount(FS_NAND1);
		#endif			
		}
		else
		{
			DBG_PRINT("Mount DISK Success[%d]!!!\r\n", USE_DISK);
			disk_free = vfsFreeSpace(USE_DISK);
			DBG_PRINT("Disk Free Size = %d\r\n", disk_free);
			break;
		}
	}
	
	switch(USE_DISK)
	{
	case FS_SD:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"C:\\AUDIO_ENCODE\\", sizeof(enc_dir));
		break;
	case FS_NAND1:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"A:\\AUDIO_ENCODE\\", sizeof(enc_dir));
		break;	
	case FS_USBH:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"G:\\AUDIO_ENCODE\\", sizeof(enc_dir));
		break;		
	}
	
	
#if USE_ADKEY == 1		
	adc_key_scan_init();
#endif

	audio_encode_entrance();
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	
	
	
	if (chdir (enc_dir) == -1)	{	// return value -1 means error occurred
		DBG_PRINT("change current directory failed\r\n");
		if (mkdir("AUDIO_ENCODE") == -1)	{
			DBG_PRINT("can not create directory AUDIO_ENCODE\r\n");
			goto FUNC_END;
		}	
		else	{
			DBG_PRINT("directory AUDIO created\r\n");
			chdir (enc_dir);
		}
	}	
	else	{
		DBG_PRINT ("change current directory successful\r\n");
	}
	
	// --------------- Description of Keys' Function --------------- //
	DBG_PRINT("GPL32XXX AUDIO EECODE DEMO\r\n");
	DBG_PRINT("Media : SDC/NAND/USBH (default: SDC)\r\n");
	DBG_PRINT("Key 1 : encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)\r\n");
	DBG_PRINT("Key 2 : start encoding (file_num ++)\r\n");
	DBG_PRINT("Key 3 : stop encoding/decoding\r\n");
	DBG_PRINT("Key 4 : reserved\r\n");
	DBG_PRINT("Key 5 : reserved\r\n");
	DBG_PRINT("Key 6 : play encoded file\r\n");
	DBG_PRINT("Key 7 : play previous encoded file\r\n");
	DBG_PRINT("Key 8 : stop encoding/decoding and exit audio encode/decode tasks\r\n");
	// ------------------------------------------------------------- //

	sprintf((char *)enc_file, "%s", "aud");
	file_nums = audio_filelist("aud*");

	if (file_nums > 0)
		enc_num = file_nums;
	
	if (list_head != NULL)
		temp = list_head;	
	
	while(1)
	{
		OSTimeDly(3);
	#if USE_ADKEY == 1	
		adc_key_scan();
	#else
		key_Scan();
	#endif
		

	#if USE_ADKEY == 1	
		if(ADKEY_IO1)
	#else
		if(IO1)
	#endif
		{
			// encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)
			enc_type++;
			if(enc_type > 4)
				enc_type = 0;
			
			switch(enc_type)
			{
				case 0:
					DBG_PRINT("encode type:A18\r\n");
					break;
				case 1:
					DBG_PRINT("encode type:WAV\r\n");
					break;
				case 2:
					DBG_PRINT("encode type:MS_ADPCM\r\n");
					break;
				case 3:
					DBG_PRINT("encode type:IMA_ADPCM\r\n");
					break;
				case 4:
					DBG_PRINT("encode type:MP3\r\n");
					break;
			}				
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO2)
	#else
		else if(IO2)
	#endif
		{			
			// start encoding (file_num ++)		
			if(audio_encode_status() == AUDIO_CODEC_PROCESS_END)
			{
				media_source.type = SOURCE_TYPE_FS; 
				//media_source.type = SOURCE_TYPE_USER_DEFINE;
				if(media_source.type_ID.FileHandle < 0)
				{
					DBG_PRINT("OPEN FILE FAIL\r\n");
					continue;
				}
							
				switch(enc_type)
				{
					case 0: // a18
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "a18");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
				media_source.Format.AudioFormat = A1800;
				//audio_encode_set_downsample(TRUE, 2);
						//audio_encode_start(media_source, 16000, 32000);
						ret = audio_encode_start(media_source, 16000, 0); // a18 only one channel
						break;	
					case 1:	// wav
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = WAV;
						//audio_encode_set_downsample(TRUE, 2);
						ret = audio_encode_start(media_source, 44100, 0);
						
						break;	
					case 2:	// ms_adpcm
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = MICROSOFT_ADPCM;
						//audio_encode_set_downsample(TRUE, 4);
						ret = audio_encode_start(media_source, 16000, 0);
						break;
					case 3:	// ima_adpcm
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = IMA_ADPCM;
						//audio_encode_start(media_source, 16000, 0);
						ret = audio_encode_start(media_source, 44100, 0);
						break;
					case 4:	// mp3
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "mp3");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = MP3;
						//audio_encode_set_downsample(TRUE, 2);
						//ret = audio_encode_start(media_source, 44100, 128000);
						ret = audio_encode_start(media_source, 22050, 128000);
						break;
						
						
			}
				
				if (ret != 0)
				{
					DBG_PRINT("encode %s start error... \r\n", enc_path);
					continue;
				}
				
				cur_state = 0;
				DBG_PRINT("now encoding %s ... \r\n", enc_path);
	
				if (list_head == NULL)
					list_create(enc_path);
				else	
					list_insert(enc_path);
	
				if (list_head != NULL)
					temp = list_head;
				
			}			
			
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO3)
	#else
		else if(IO3)
	#endif
		{
			// stop encoding/decoding
			audio_stop_processing(&audio_argument);
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO4)
	#else 
		else if(IO4)
	#endif
		{
			// reserved									
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO5)
	#else
		else if(IO5)
	#endif
		{
			// reserved			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO6)
	#else 
		else if(IO6)
	#endif
		{
			// play encoded file
		
			audio_stop_processing(&audio_argument);			
			
			if (list_head == NULL)
			{
				DBG_PRINT("error:no audio files !!!\r\n");
				continue;
			}
			
			if (cur_state == 0)
				{
				temp = temp->prev;
				}
			else
			{
				temp = temp->next;
				if(temp == list_head)
					temp = temp->next;
			}
			
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63

			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", temp->data);
			audio_encode_playback((char*)(temp->data), &audio_argument);
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO7)
	#else
		else if(IO7)
	#endif
		{
			// play previous encoded file
			audio_stop_processing(&audio_argument);
		
			if (list_head == NULL)
			{
				DBG_PRINT("error:no audio files !!!\r\n");
					continue;
				}
			
			temp = temp->prev;
			if(temp == list_head)
				temp = temp->prev;
						
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63
			
			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", temp->data);
			audio_encode_playback((char*)(temp->data), &audio_argument);
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO8)
	#else
		else if(IO8)
	#endif
		{
			// stop encoding/decoding and exit audio encode/decode tasks			
			audio_stop_processing(&audio_argument);
			break;
		}
					
		}
	
	
FUNC_END:	
			
	audio_encode_exit();
	audio_decode_exit();	// Release the resources needed by audio decoder	
	
	DBG_PRINT("exit audio encode demo !!!\r\n");
}

