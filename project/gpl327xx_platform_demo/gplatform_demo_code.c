#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "video_codec_callback.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1	
//#define USE_DISK	FS_USBH

//=================================================================================================
//	Platform demo code
//=================================================================================================
void Platform_Demo_Code(void)
{
	char 	video_path[32] ="C:\\Demo0091NBA.avi"; 
	char    audio_path[32] = "C:\\happy.mp3";
	char    image_path1[32] = "C:\\chen52.jpg";
	char    image_path2[32] = "C:\\chen53.jpg";
	INT8S	volume;
	INT32S  nRet;
	INT32U  decode_output_ptr;
	AUDIO_ARGUMENT audio_arg;
	IMAGE_ARGUMENT image_arg;
	VIDEO_ARGUMENT video_arg;
	VIDEO_INFO	information;
	MEDIA_SOURCE   src;
	AUDIO_CODEC_STATUS 	audio_status;
	IMAGE_CODEC_STATUS  image_status;
	VIDEO_CODEC_STATUS  video_status;
		
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
			break;
		}
	}

	switch(USE_DISK)
	{
	case FS_SD:	
		gp_memcpy((INT8S*)video_path, (INT8S*)"C:\\Demo0091NBA.avi", sizeof(video_path));
		gp_memcpy((INT8S*)audio_path, (INT8S*)"C:\\happy.mp3", sizeof(audio_path));
		gp_memcpy((INT8S*)image_path1, (INT8S*)"C:\\chen52.jpg", sizeof(image_path1));
		gp_memcpy((INT8S*)image_path2, (INT8S*)"C:\\chen53.jpg", sizeof(image_path2));
		break;
	case FS_NAND1:	
		gp_memcpy((INT8S*)video_path, (INT8S*)"A:\\Demo0091NBA.avi", sizeof(video_path));
		gp_memcpy((INT8S*)audio_path, (INT8S*)"A:\\happy.mp3", sizeof(audio_path));
		gp_memcpy((INT8S*)image_path1, (INT8S*)"A:\\chen52.jpg", sizeof(image_path1));
		gp_memcpy((INT8S*)image_path2, (INT8S*)"A:\\chen53.jpg", sizeof(image_path2));
		break;	
	case FS_USBH:	
		gp_memcpy((INT8S*)video_path, (INT8S*)"G:\\Demo0091NBA.avi", sizeof(video_path));
		gp_memcpy((INT8S*)audio_path, (INT8S*)"G:\\happy.mp3", sizeof(audio_path));
		gp_memcpy((INT8S*)image_path1, (INT8S*)"G:\\chen52.jpg", sizeof(image_path1));
		gp_memcpy((INT8S*)image_path2, (INT8S*)"G:\\chen53.jpg", sizeof(image_path2));
		break;		
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
	audio_decode_entrance();
	image_decode_entrance();
	video_decode_entrance();     

	adc_key_scan_init(); //init key scan	
	volume = 0x3F;
	nRet = C_DISPLAY_DEV_HPIXEL*C_DISPLAY_DEV_VPIXEL*2;	
	decode_output_ptr = (INT32U) gp_malloc_align(nRet, 64);//malloc decode frame buffer
	while(!decode_output_ptr);
	
	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)	
		{	//play audio
			if(video_decode_status() == VIDEO_CODEC_PROCESS_END)
			{
				src.type = SOURCE_TYPE_FS;
				src.type_ID.FileHandle = open(audio_path, O_RDONLY);
				src.Format.AudioFormat = MP3;
				
				audio_arg.Main_Channel = 1;		// Use DAC Channel A+B
				audio_arg.L_R_Channel = 3;		// Left + Right Channel
				audio_arg.mute = 0;
				audio_arg.volume = volume;		// volume level = 0~63
				
				G_snd_info.Speed = 12;					//for speed control(golbal var)
			    G_snd_info.Pitch = 12;					//for pitch control(golbal var)
				
				audio_status = audio_decode_start(audio_arg, src);
			}
		}
		else if(ADKEY_IO2)		
		{
			if(audio_decode_status(audio_arg) == AUDIO_CODEC_PROCESSING)
				audio_decode_stop(audio_arg);
		}
		else if(ADKEY_IO3)    
		{	//play image
			if(video_decode_status() == VIDEO_CODEC_PROCESS_END)
			{
				src.type = SOURCE_TYPE_FS;
				if((nRet++ % 2) == 0)
					src.type_ID.FileHandle = (INT32U)open(image_path1, O_RDONLY);
				else
					src.type_ID.FileHandle = (INT32U)open(image_path2, O_RDONLY);
				
				image_arg.OutputBufPtr=(INT8U *)decode_output_ptr;            //decode output buffer
				image_arg.OutputBufWidth = C_DISPLAY_DEV_HPIXEL;                  //width of output buffer 
				image_arg.OutputBufHeight = C_DISPLAY_DEV_VPIXEL;                 //Heigh of output buffer
				image_arg.OutputWidth = C_DISPLAY_DEV_HPIXEL;                     //scaler width of output image
				image_arg.OutputHeight = C_DISPLAY_DEV_VPIXEL;                    //scaler Heigh of output image
				image_arg.OutBoundaryColor = 0x008080;
				image_arg.ScalerOutputRatio = FIT_OUTPUT_SIZE;
				image_arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV;  
				
				image_decode_start(image_arg, src);
				while (1) 
				{
			 		image_status=image_decode_status();
	         		if (image_status == IMAGE_CODEC_DECODE_END) 
	         		{
						video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)decode_output_ptr, C_DISPLAY_DEVICE,IMAGE_OUTPUT_FORMAT_YUYV);
						break;
			  		}
			  		else if(image_status==IMAGE_CODEC_DECODE_FAIL) 
			  		{
						DBG_PRINT("image decode failed\r\n");
						break;
			  		}
			  	}
			  	image_encode_stop();
			  	close(src.type_ID.FileHandle);
		  	}		
		}
		else if(ADKEY_IO4)     
		{
			audio_arg.Main_Channel = 1;
			if(audio_decode_status(audio_arg) == AUDIO_CODEC_PROCESS_END) 
			{
				DBG_PRINT("video_decode_start\r\n");	
				video_arg.bScaler = TRUE;
				video_arg.bUseDefBuf = FALSE;
				video_arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;		
				video_arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	
				video_arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	
				video_arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	
				video_arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV;	
						
				src.type = SOURCE_TYPE_FS;
				src.Format.VideoFormat = MJPEG;	
				src.type_ID.FileHandle = open(video_path, O_RDONLY);
					
				if(src.type_ID.FileHandle < 0)
				{
					DBG_PRINT("file open fail\r\n");
					close(src.type_ID.FileHandle);
					continue;
				}
					
				video_status = video_decode_paser_header(&information, video_arg, src);
				if(video_status != VIDEO_CODEC_STATUS_OK)
				{
					DBG_PRINT("paser header fail !!!\r\n");
					continue;
				}
				video_decode_start(video_arg, src);
				audio_decode_volume(volume);
			}
		}
		else if(ADKEY_IO5)    
		{		
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
			{
				video_decode_stop();
				DBG_PRINT("video_decode_stop\r\n");
			}
		}
		else if(ADKEY_IO6)     
		{
			
		}
		else if(ADKEY_IO7)    
		{
	
		}
		else if(ADKEY_IO8)
		{
		
		}
	}
}