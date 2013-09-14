#include "application.h"
#include "drv_l2_ad_key_scan.h"

#define USE_DISK	FS_SD 
//#define USE_DISK	FS_NAND1 

#define MP3_DEMO	  	0
#define A6400_DEMO		0
#define A1800_DEMO		1
#define A1600_DEMO		0
#define S880_DEMO	  	0

void Sacm_Decode_Demo(void)
{
	char path_mp3[24];	
	char path_a64[24];	
	char path_a18[24];	
	char path_a16[24];		
	char path_s88[24];		
	INT32U temp;
	SND_INFO  sndinfo_0;
	AUDIO_ARGUMENT audio_argument_0;
#if USE_DISK == FS_NAND1	
	INT32S ret;
#endif
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
	case FS_NAND1:
		gp_strcpy((INT8S*)path_mp3, (INT8S*)"A:\\happy.mp3");
		gp_strcpy((INT8S*)path_a64, (INT8S*)"A:\\happy.a64");	
		gp_strcpy((INT8S*)path_a18, (INT8S*)"A:\\happy.a18");	
		gp_strcpy((INT8S*)path_a16, (INT8S*)"A:\\demo.a16");	
		gp_strcpy((INT8S*)path_s88, (INT8S*)"A:\\demo.s88");			
		break;
	case FS_SD:
		gp_strcpy((INT8S*)path_mp3, (INT8S*)"C:\\happy.mp3");
		gp_strcpy((INT8S*)path_a64, (INT8S*)"C:\\happy.a64");	
		gp_strcpy((INT8S*)path_a18, (INT8S*)"C:\\happy.a18");	
		gp_strcpy((INT8S*)path_a16, (INT8S*)"C:\\happy.a16");	
		gp_strcpy((INT8S*)path_s88, (INT8S*)"C:\\happy.s88");		
		break;
	case FS_USBH:
		gp_strcpy((INT8S*)path_mp3, (INT8S*)"G:\\happy.mp3");
		gp_strcpy((INT8S*)path_a64, (INT8S*)"G:\\happy.a64");	
		gp_strcpy((INT8S*)path_a18, (INT8S*)"G:\\happy.a18");	
		gp_strcpy((INT8S*)path_a16, (INT8S*)"G:\\happy.a16");	
		gp_strcpy((INT8S*)path_s88, (INT8S*)"G:\\happy.s88");		
		break;
	}

	adc_key_scan_init();
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	
	while(1)
	{
		adc_key_scan();
//============================================================================
#if MP3_DEMO
		if(ADKEY_IO1)	// Start to decode the specified file
		{	
			ADKEY_IO1 = 0;
			Snd_Stop(MP3);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_mp3, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = MP3;

			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_NONE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO2)	// Start to decode the specified file
		{	
			ADKEY_IO2 = 0;
			Snd_Stop(MP3);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			
			G_Snd_UserFd = open(path_mp3, O_RDONLY);	//User define
	
			sndinfo_0.AudioFormat = MP3;
			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 16000;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO3)
		{
			ADKEY_IO3 = 0;
			Snd_Stop(MP3);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_mp3, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = MP3;

			sndinfo_0.Speed = 12+10;
			sndinfo_0.Pitch = 12+10;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_NONE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
#endif
//============================================================================
#if A6400_DEMO
		if(ADKEY_IO1)	// Start to decode the specified file
		{	
			ADKEY_IO1 = 0;
			Snd_Stop(A6400);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_a64, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = A6400;

			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO2)	// Start to decode the specified file
		{	
			ADKEY_IO2 = 0;
			Snd_Stop(A6400);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			
			G_Snd_UserFd = open(path_a64, O_RDONLY);	//User define
			
			sndinfo_0.AudioFormat = A6400;
			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 16000;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO3)	// Start to decode the specified file
		{	
			ADKEY_IO3 = 0;
			Snd_Stop(A6400);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_a64, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = A6400;

			sndinfo_0.Speed = 12+10;
			sndinfo_0.Pitch = 12+10;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
#endif
//============================================================================
#if A1800_DEMO
		if(ADKEY_IO1)	// Start to decode the specified file
		{	
			ADKEY_IO1 = 0;
			Snd_Stop(A1800);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_a18, O_RDONLY);	//User define
			
			sndinfo_0.AudioFormat = A1800;
			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_NONE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO2)	// Start to decode the specified file
		{	
			Snd_Stop(A1800);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			
			G_Snd_UserFd = open(path_a18, O_RDONLY);	//User define
			
			sndinfo_0.AudioFormat = A1800;
			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 16000;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO3)	// Start to decode the specified file
		{	
			Snd_Stop(A1800);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;

			G_Snd_UserFd = open(path_a18, O_RDONLY);	//User define
			
			sndinfo_0.AudioFormat = A1800;
			sndinfo_0.Speed = 12+10;
			sndinfo_0.Pitch = 12+10;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO4)	// Stop decoding the current file
		{
			Snd_Stop(A1800);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
		}
		if(ADKEY_IO5)     // Pause the decoder
		{
			Snd_Pause( A1800 );
		}

		if(ADKEY_IO6)     // Resum the decoder
		{
			Snd_Resume( A1800 );
		}
		if(ADKEY_IO7)     // Volume SET
		{
			temp = audio_argument_0.volume;
			temp += 4;
			if(temp > 63)
				temp = 63;
			
			Snd_SetVolume(A1800,temp);
		}
		if(ADKEY_IO8)     // Volume GET
		{		
			audio_argument_0.volume = Snd_GetVolume(A1800);
		}

#endif
//============================================================================
#if A1600_DEMO
		if(ADKEY_IO1)	// Start to decode the specified file
		{	
			ADKEY_IO1 = 0;
			Snd_Stop(A1600);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_a16, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = A1600;

			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		else if(ADKEY_IO2)	// Start to decode the specified file
		{	
			ADKEY_IO2 = 0;
			Snd_Stop(A1600);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_a16, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = A1600;

			sndinfo_0.Speed = 12+10;
			sndinfo_0.Pitch = 12+10;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO3)	// Stop decoding the current file
		{
			Snd_Stop(A1600);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
		}
		if(ADKEY_IO4)     // Pause the decoder
		{
			Snd_Pause( A1600 );
		}

		if(ADKEY_IO5)     // Resum the decoder
		{
			Snd_Resume( A1600 );
		}
		if(ADKEY_IO6)     // Volume SET
		{
			temp = audio_argument_0.volume;
			temp += 4;
			if(temp > 63)
				temp = 63;
			
			Snd_SetVolume(A1600, temp);
		}
		if(ADKEY_IO7)     // Volume GET
		{		
			audio_argument_0.volume = Snd_GetVolume(A1600);
		}
#endif
//============================================================================
#if S880_DEMO
		if(ADKEY_IO1)	// Start to decode the specified file
		{	
			ADKEY_IO1 = 0;
			Snd_Stop(S880);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_s88, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = S880;

			sndinfo_0.Speed = 12;
			sndinfo_0.Pitch = 12;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		else if(ADKEY_IO2)	// Start to decode the specified file
		{	
			ADKEY_IO2 = 0;
			Snd_Stop(S880);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
			G_Snd_UserFd = open(path_s88, O_RDONLY);	//User define
			sndinfo_0.AudioFormat = S880;

			sndinfo_0.Speed = 12+10;
			sndinfo_0.Pitch = 12+10;
			sndinfo_0.Offset = 0;
			sndinfo_0.Offsettype = SND_OFFSET_TYPE_SIZE;
			sndinfo_0.Srctype = SOURCE_TYPE_USER_DEFINE;
			Snd_Play(&sndinfo_0);
		}
		if(ADKEY_IO3)	// Stop decoding the current file
		{
			Snd_Stop(S880);
			close(G_Snd_UserFd);
			G_Snd_UserFd = -1;
		}
		if(ADKEY_IO4)     // Pause the decoder
		{
			Snd_Pause( S880 );
		}

		if(ADKEY_IO5)     // Resum the decoder
		{
			Snd_Resume( S880 );
		}
		if(ADKEY_IO6)     // Volume SET
		{
			temp = audio_argument_0.volume;
			temp += 4;
			if(temp > 63)
				temp = 63;
			
			Snd_SetVolume(S880,temp);
		}
		if(ADKEY_IO7)     // Volume GET
		{		
			audio_argument_0.volume = Snd_GetVolume(S880);
		}
#endif
	}
	audio_decode_exit();	// Release the resources needed by audio decoder
}


//============================================================================================
//	record demo 
//============================================================================================

void Sacm_Encode_Demo(void)
{
	char 	path_wav[24] = "C:\\WaveRec.wav";	// Fetch file on SD Card
	char 	path_a18[24] = "C:\\A1800.a18";
	INT8U	bRecording = 0;
	INT16S 	ret;
	INT64U  disk_free;
	MEDIA_SOURCE	media_source;
	
	while(1)
	{	
		ret = _devicemount(USE_DISK);
		if(ret!=0)
		{
			DBG_PRINT("Mount Disk Error!\r\n");
		}
		else
		{
			DBG_PRINT("Mount Disk Success!\r\n");
			disk_free = vfsFreeSpace(USE_DISK);
			DBG_PRINT("Disk Free Size = %d\r\n", disk_free);
			break;
		}
	}
	
	adc_key_scan_init();
	audio_encode_entrance();
	
	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)
		{
			if(bRecording)
			{
				DBG_PRINT("audio_encode_stop...");
				audio_encode_stop();
			#if USE_DISK == FS_NAND1
				DBG_PRINT("Nand flush\r\n");
				DrvNand_flush_allblk();
			#endif
				bRecording = 0;
				DBG_PRINT("finish!!!\r\n");
			}				
		}
		else if(ADKEY_IO2)
		{
			if(bRecording == 0)
			{
				DBG_PRINT("File = %s\r\n",path_a18);
				media_source.type = SOURCE_TYPE_FS; 
				//media_source.type = SOURCE_TYPE_USER_DEFINE;
				media_source.type_ID.FileHandle = open(path_a18, O_WRONLY|O_CREAT|O_TRUNC);
				if(media_source.type_ID.FileHandle < 0)
				{
					DBG_PRINT("OPEN FILE FAIL\r\n");
					continue;
				}
				media_source.Format.AudioFormat = A1800;
				//audio_encode_set_downsample(TRUE, 2);
				audio_encode_start(media_source, 16000, 32000);
				bRecording = 1;
			}
		}
		else if(ADKEY_IO3)
		{
			if(bRecording == 0)
			{
				DBG_PRINT("File = %s\r\n",path_wav);
				media_source.type = SOURCE_TYPE_FS; 
				//media_source.type = SOURCE_TYPE_USER_DEFINE;
				media_source.type_ID.FileHandle = open(path_wav, O_WRONLY|O_CREAT|O_TRUNC);
				if(media_source.type_ID.FileHandle < 0)
				{
					DBG_PRINT("OPEN FILE FAIL\r\n");
					continue;
				}
				media_source.Format.AudioFormat = WAV;
				//audio_encode_set_downsample(TRUE, 2);
				audio_encode_start(media_source, 16000, 0);
				bRecording = 1;	
			}
		}
		else if(ADKEY_IO4)
		{
		
		}
	}
	audio_encode_exit();
}


