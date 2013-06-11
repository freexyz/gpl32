#include <stdio.h>
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "video_codec_callback.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1
//#define USE_DISK	FS_USBH
//=================================================================================================
//	AVI Decode demo code
//=================================================================================================
void Video_Decode_Demo()
{
	#define USE_ADKEY			1
	#define USER_DEFINE_BUFFER	1
	//char 	sd_path[64] = "C:\\video.bin";
	//char 	sd_path[64] = "C:\\fx07.mov";
	char 	sd_path[64] = "C:\\Demo0091NBA.avi"; 
	char	nand_path[64] = "A:\\Demo0091NBA.avi";
	char 	usb_path[64] = "G:\\Demo0091NBA.avi";
	INT8U   *avi_dec_buffer[2];  
	INT8S	reverse_play_flag;
	INT32S  nRet, i;
	FP32    play_speed;
	VIDEO_CODEC_STATUS status;
	VIDEO_INFO	information;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	struct f_info   file_info;
	
	//init 
	avi_dec_buffer[0] = avi_dec_buffer[1] = 0;
	reverse_play_flag = 0;
	play_speed = 1;
		
	while(1)
	{
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);		
		#if USE_DISK == FS_NAND1
			nRet = _format(FS_NAND1, FAT32_Type); 
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
	
	// Initialize display device
#if C_DISPLAY_DEVICE >= C_TV_QVGA
	tv_init();	
#if C_DISPLAY_DEVICE == C_TV_QVGA
	tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_NON_INTERLACE);	
#elif C_DISPLAY_DEVICE == C_TV_VGA
	tv_start (TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
#elif C_DISPLAY_DEVICE == C_TV_D1
	tv_start (TVSTD_NTSC_J, TV_D1, TV_NON_INTERLACE);
#else
	while(1);	
#endif
#else
	tft_init();
	tft_start(C_DISPLAY_DEVICE);
#endif

	user_defined_video_codec_entrance();
	video_decode_entrance();     // Initialize and allocate the resources needed by Video decoder
#if USE_ADKEY == 1
	adc_key_scan_init(); 	//init key scan
#endif	
#if USER_DEFINE_BUFFER == 1	
	nRet = C_DISPLAY_DEV_HPIXEL*C_DISPLAY_DEV_VPIXEL*2;	
	for(i=0; i<2; i++)
	{
		avi_dec_buffer[i] = (INT8U *)gp_malloc_align(nRet, 64);
		while(!avi_dec_buffer[i]);
	}
#endif	
	while(1)
	{	
		OSTimeDly(3);
	#if USE_ADKEY == 1	
		adc_key_scan();
	#else	
		key_Scan();
	#endif	
	#if USE_ADKEY == 1	
		if(ADKEY_IO1)                // Start to decode the specified file 
	#else	
		if(IO1)
	#endif	
		{
			if(video_decode_status() != VIDEO_CODEC_PROCESS_END)
				video_decode_stop();

			arg.bScaler = 0x01;	//scaler output size or not
		#if	USER_DEFINE_BUFFER == 1
			arg.bUseDefBuf = TRUE;	//use user define buffer 
			arg.AviDecodeBuf1 = avi_dec_buffer[0];
			arg.AviDecodeBuf2 = avi_dec_buffer[1];
		#else	
			arg.bUseDefBuf = FALSE;	//auto alloc buffer size  
			arg.AviDecodeBuf1 = NULL;
			arg.AviDecodeBuf2 = NULL;
		#endif
		
			arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;		//display width
			arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;		//display height
			arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
			arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height
			
			arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format
			if(USE_DISK == FS_SD)
				src.type_ID.FileHandle = open(sd_path, O_RDONLY);	//open file handle
			else if(USE_DISK == FS_NAND1)
				src.type_ID.FileHandle = open(nand_path, O_RDONLY); //open file handle
			else if(USE_DISK == FS_USBH)
				src.type_ID.FileHandle = open(usb_path, O_RDONLY);	//open file handle
			
			if(src.type_ID.FileHandle < 0)
			{
				DBG_PRINT("file open fail\r\n");
				close(src.type_ID.FileHandle);
				continue;
			}	
			
		#if 1	
			src.type = SOURCE_TYPE_FS;	//play file by file system
		#else
			// add this for play a video in a file special position.	
			src.type = SOURCE_TYPE_FS_RESOURCE_IN_FILE;
			nRet = 0x80;
			lseek(src.type_ID.FileHandle, nRet, SEEK_SET);
			src.type_ID.temp = 0x1AF2154;	//current avi file size
		#endif
			src.Format.VideoFormat = MJPEG;
			
			DBG_PRINT("video_decode_start\r\n");	
			status = video_decode_paser_header(&information, arg, src);
			if(status != VIDEO_CODEC_STATUS_OK)
			{
				DBG_PRINT("paser header fail !!!\r\n");
				continue;
			}
			DBG_PRINT("Aud SampleRate = %d\r\n", information.AudSampleRate);
			DBG_PRINT("Vid FrameRate = %d\r\n", information.VidFrameRate);
			DBG_PRINT("resolution = %d x %d\r\n", information.Width, information.Height);
			DBG_PRINT("Total Time = %d seconds\r\n", information.TotalDuration);
			
			status = video_decode_start(arg, src);
			if(status != START_OK)
			{
				DBG_PRINT("video start fail!!!\r\n");
				continue;
			}
			audio_decode_volume(0x3F);
			reverse_play_flag = 0;
			play_speed = 1;
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO2)         // Stop decoding the current file
	#else
		else if(IO2)
	#endif	
		{
			if(video_decode_status() != VIDEO_CODEC_PROCESS_END)
			{
				DBG_PRINT("video_decode_stop\r\n");	
				video_decode_stop();
			}
			else
			{
		#if 1
				if(USE_DISK == FS_SD)
					chdir("C:\\");
				else if(USE_DISK == FS_NAND1)
					chdir("A:\\");
				else if(USE_DISK == FS_USBH)
					chdir("G:\\");
				
				nRet = _findnext(&file_info);
				if(nRet < 0)
				{
					nRet = _findfirst("*.*", &file_info, D_ALL);
					if(nRet < 0)
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
				gp_strcpy((INT8S*)sd_path, (INT8S*)file_info.f_name);
				DBG_PRINT("FileName = %s\r\n", file_info.f_name);		
		#endif
			}	
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO3)
	#else
		else if(IO3)
	#endif
		{
			if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE)
			{
				DBG_PRINT("video_decode_resume\r\n");
				video_decode_resume();
			}
			else if(video_decode_status() == VIDEO_CODEC_PROCESSING)
 			{
				DBG_PRINT("video_decode_pause\r\n");
				video_decode_pause();
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO4)
	#else
		else if(IO4)
	#endif
		{
			if(reverse_play_flag)
			{
				reverse_play_flag = 0;
				video_decode_set_reserve_play(0);
			}
			else
			{
				reverse_play_flag = 1;
				video_decode_set_reserve_play(1);
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO5)
	#else
		else if(IO5)
	#endif
		{	//ff play
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
			{
				nRet = video_decode_get_current_time();
				nRet += 5;
				video_decode_set_play_time(nRet);
				DBG_PRINT("Set Play Time = %dsec\r\n", nRet);
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO6)
	#else
		else if(IO6)
	#endif
		{	//fb play
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
			{			
				nRet = video_decode_get_current_time();
				nRet -= 5;
				video_decode_set_play_time(nRet);
				DBG_PRINT("Set Play Time = %dsec\r\n", nRet);
			}	
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO7)
	#else
		else if(IO7)
	#endif
		{	
			play_speed += 0.5;
			if(play_speed > 4)	play_speed = 0.5;
			nRet = play_speed*10;
			DBG_PRINT("speed = %d.%d\r\n", nRet/10, nRet%10);	
			video_decode_set_play_speed(play_speed);
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO8)
	#else 
		else if(IO8)
	#endif
		{	
			if(video_decode_status() != VIDEO_CODEC_PROCESS_END)
				continue;
			
			arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;		//display width
			arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;		//display height
			arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
			arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height
			
			arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	
			src.type = SOURCE_TYPE_FS;
			src.Format.VideoFormat = MJPEG;
			if(USE_DISK == FS_SD)
				src.type_ID.FileHandle = open(sd_path, O_RDONLY);
			else if(USE_DISK == FS_NAND1)
				src.type_ID.FileHandle = open(nand_path, O_RDONLY); 
				
			if(src.type_ID.FileHandle < 0)
			{
				DBG_PRINT("file open fail\r\n");
				close(src.type_ID.FileHandle);
				continue;
			}	
			
			if(video_decode_get_nth_video_frame(&information, arg, src, 1) == VIDEO_CODEC_STATUS_OK)
				video_decode_FrameReady((INT8U*)avi_dec_buffer[0]);
		}	
	}
}

//=================================================================================================
//	AVI encode demo code
//=================================================================================================
void Video_Encode_Demo()
{	
	char 	path[64];
	INT32S  nRet;
	INT64U  disk_free;
	FP32    zoom_ratio;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;
	
	while(1)
	{
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
		#if	USE_DISK == FS_NAND1
            nRet = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();   
			_deviceunmount(FS_NAND1);
		#endif								
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			DBG_PRINT("StartTime = %d\r\n", OSTimeGet());
			disk_free = vfsFreeSpace(USE_DISK);
			DBG_PRINT("Disk Free Size = %dMbyte\r\n", disk_free/1024/1024);
			DBG_PRINT("EndTime = %d\r\n", OSTimeGet());			
			break;
		}
	}
	
	// Initialize display device
#if C_DISPLAY_DEVICE >= C_TV_QVGA
	tv_init();	
#if C_DISPLAY_DEVICE == C_TV_QVGA
	tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_NON_INTERLACE);	
#elif C_DISPLAY_DEVICE == C_TV_VGA
	tv_start (TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
#elif C_DISPLAY_DEVICE == C_TV_D1
	tv_start (TVSTD_NTSC_J, TV_D1, TV_NON_INTERLACE);
#else
	while(1);
#endif
#else
	tft_init();
	tft_start(C_DISPLAY_DEVICE);
#endif
	
	user_defined_video_codec_entrance();
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder
#if USE_ADKEY == 1	
	//ADC init key scan
	adc_key_scan_init();
#endif	
	nRet = 0;
	zoom_ratio = 1;
	
	arg.bScaler = 1;	// must be 1
	arg.TargetWidth = 640; 	//encode width
	arg.TargetHeight = 480;	//encode height
	arg.SensorWidth	= 640;	//sensor input width
	arg.SensorHeight = 480;	//sensor input height
	arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;	//display width
	arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
	arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;//display buffer height
	arg.VidFrameRate = 30;	//video encode frame rate
	arg.AudSampleRate = 8000;//audio record sample rate
	arg.OutputFormat = DISPLAY_OUTPUT_FORMAT; //display output format 
	video_encode_preview_start(arg);
	while(1)
	{
	#if USE_ADKEY == 1
		adc_key_scan();
	#else
		key_Scan();
	#endif
		OSTimeDly(3);
	
	#if USE_ADKEY == 1
		if(ADKEY_IO1)                // Start to decode the specified file 
	#else
		if(IO1)
	#endif	
		{
			if(video_encode_status() == VIDEO_CODEC_PROCESSING)
				continue;
				
			if(USE_DISK == FS_SD)
				sprintf((char *)path, (const char *)"C:\\avi_rec%d.avi", nRet++);
			else if(USE_DISK == FS_NAND1)
				sprintf((char *)path, (const char *)"A:\\avi_rec%d.avi", nRet++);
			
			src.type = SOURCE_TYPE_FS;
			src.Format.VideoFormat = MJPEG;	
			src.type_ID.FileHandle = open(path, O_WRONLY|O_CREAT|O_TRUNC);
			if(src.type_ID.FileHandle < 0)
			{
				DBG_PRINT("file open fail\r\n");
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
						src.type_ID.FileHandle = open(path, O_WRONLY|O_CREAT|O_TRUNC);
						break;
					}
				}
			}
			
			DBG_PRINT("file name = %s\r\n", path);		
			DBG_PRINT("video_encode_start\r\n");
			video_encode_start(src);
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO2)         // Stop decoding the current file
	#else
		else if(IO2)
	#endif	
		{
			if(video_encode_status() == VIDEO_CODEC_PROCESSING)
			{
				DBG_PRINT("video_encode_stop\r\n");
				video_encode_stop();
			#if	USE_DISK == FS_NAND1
				DBG_PRINT("DrvNand_flush_allblk\r\n");
				DrvNand_flush_allblk();
			#endif
			}	
		}
	#if USE_ADKEY == 1
		else if(ADKEY_IO3)
	#else
		else if(IO3)
	#endif
		{
			if(video_encode_status() == VIDEO_CODEC_PROCESS_PAUSE)
			{
				DBG_PRINT("video encode resume\r\n");
				video_encode_resume();
			}
			else if(video_encode_status() == VIDEO_CODEC_PROCESSING)
			{
				DBG_PRINT("video encode pause\r\n");
				video_encode_pause();
			}	
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO4)
	#else
		else if(IO4)
	#endif
		{
			if(USE_DISK == FS_SD)
				sprintf((char *)path, (const char *)"C:\\image%d.jpg", nRet++);
			else if(USE_DISK == FS_NAND1)
				sprintf((char *)path, (const char *)"A:\\image%d.jpg", nRet++);
			
			src.type = SOURCE_TYPE_FS;
			src.Format.VideoFormat = MJPEG;	
			src.type_ID.FileHandle = open(path, O_WRONLY|O_CREAT|O_TRUNC);
			if(src.type_ID.FileHandle >= 0)
			{
				DBG_PRINT("file name = %s\r\n", path);	
				video_encode_set_jpeg_quality(90);
				video_encode_capture_picture(src);
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO5)
	#else
		else if(IO5)
	#endif
		{
			if(video_encode_status() == VIDEO_CODEC_PROCESS_END)
			{
				DBG_PRINT("video_encode_preview_start\r\n");
				video_encode_preview_start(arg);
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO6)
	#else
		else if(IO6)
	#endif
		{
			if(video_encode_status() == VIDEO_CODEC_PROCESS_END)
			{
				DBG_PRINT("video_encode_preview_stop\r\n");
				video_encode_preview_stop();
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO7)
	#else
		else if(IO7)
	#endif
		{
			DBG_PRINT("video_encode_exit\r\n");
			video_encode_exit();
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO8)
	#else
		else if(IO8)
	#endif
		{

		}	
	}
	
	video_encode_preview_stop();
	video_encode_exit();        // Release the resources needed by Video encoder
}

