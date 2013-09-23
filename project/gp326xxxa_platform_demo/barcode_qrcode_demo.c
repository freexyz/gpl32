#include <stdio.h>
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "video_codec_callback.h"
#include "gpbarcode_api.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1
//#define USE_DISK	FS_USBH
#define BARCODE	1
#define	QRCODE	1

#define USE_ADKEY	1


#if (QRCODE == 1)||(BARCODE == 1)
INT32U qrcode_frame;
INT8U resultstream[1024];	//direct allocate fix memory for decoderesult
#endif

#if BARCODE == 1
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
char barcode_decoder_name[][16] = 
{
	"ID Err",
	"CODE128"
};

static void qrcide_barcode_show_menu(void)
{
	DBG_PRINT("**********************************************************************\r\n");
	DBG_PRINT("*                  QRCODE/ BARCODE demo				               *\r\n");
	DBG_PRINT("**********************************************************************\r\n");
	DBG_PRINT("DEMO Requirement:	\r\n");
	DBG_PRINT("OV7670 VGA Sensor:	\r\n");
	DBG_PRINT("SD Card with camera_ok.drm and camera_ng.drm	\r\n");
	DBG_PRINT("KEY1    QRCODE DECODE\r\n");
	DBG_PRINT("KEY2    1D BARD CODE DECODE\r\n");
	DBG_PRINT("\r\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
char *barcode_get_decoder_name(GPBARCODE_DEC_ID id)
{
	switch (id)
	{
	case GPBARCODE_NONE:
		return (char *)(barcode_decoder_name[0]);
	case GPBARCODE_CODE128:
		return (char *)(barcode_decoder_name[1]);
	default:
		return (char *)(barcode_decoder_name[0]);
	}

	return (char *)(barcode_decoder_name[0]);
}

#endif

void BarCode_QRCode_Demo(void)
{	
	char 	path[64];
	INT32S  nRet;
	INT64U  disk_free;
	FP32    zoom_ratio;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;
#ifdef QRCODE
	INT8U* QRCode_Working_Memory;
	INT8U image_type;
	INT16S ret;
	INT8U *res;
	INT32U error_num;
	
	INT16S fd_adpcm,fd_adpcm1;
	INT32U snd_buffer_addr[3];
//	INT32S ret;
	struct sfn_info file_info;
	INT32U t1=0;		
#endif

#ifdef BARCODE
	INT8U* BarCode_Working_Memory;
	int n;
	gpImage img;
	int i;
#endif

	
#if 1 	
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
			disk_free = vfsFreeSpace(USE_DISK);
			DBG_PRINT("Disk Free Size = %dMbyte\r\n", disk_free/1024/1024);
			break;
		}
	}
#endif
	qrcide_barcode_show_menu();	
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
//	while(1);

//load audio
	if(SPU_Get_SingleChannel_Status(13) == 0)
	{
		fd_adpcm = open("C:\\camera_ok.drm", O_RDONLY);
		if(fd_adpcm<0)
			while(1);
		fd_adpcm1 = open("C:\\camera_ng.drm", O_RDONLY);
		if(fd_adpcm1<0)
			while(1);

			
		sfn_stat(fd_adpcm, &file_info);
		if(snd_buffer_addr[0])
			gp_free((void*)snd_buffer_addr[0]);	
				
		snd_buffer_addr[0] = (INT32U)gp_malloc_align(file_info.f_size, 4);//256kbyte
		while(snd_buffer_addr[0] == 0);
			
		ret = (INT32S)read(fd_adpcm, (INT32U)snd_buffer_addr[0], file_info.f_size);
		

		sfn_stat(fd_adpcm1, &file_info);
		if(snd_buffer_addr[1])
			gp_free((void*)snd_buffer_addr[1]);	
				
		snd_buffer_addr[1] = (INT32U)gp_malloc_align(file_info.f_size, 4);//256kbyte
		while(snd_buffer_addr[1] == 0);
			
		ret = (INT32S)read(fd_adpcm1, (INT32U)snd_buffer_addr[1], file_info.f_size);
		
		close(fd_adpcm);			
		close(fd_adpcm1);

	}



	while(1)
	{
	#if USE_ADKEY == 1
		adc_key_scan();
	#else
		key_Scan();
	#endif
		OSTimeDly(3);
	

	#if USE_ADKEY == 1	
		if(ADKEY_IO1)
	#else
		if(IO1)
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
//				DBG_PRINT("file name = %s\r\n", path);	
				video_encode_set_jpeg_quality(90);
				video_encode_capture_picture(src);

		//#ifdef QRCODE
		#if QRCODE == 1

				QRCode_Working_Memory = (INT8U *)gp_malloc_align((INT32U)QR_GetWorkingMemorySize(), 64);
				image_type = 0;
				gp_memset((INT8S *)resultstream,0,1024);	//direct set resultmem as 0, length=1024	
				ret = QR_Init(QRCode_Working_Memory,640,480,(INT8U *)qrcode_frame, image_type, resultstream);

				//DBG_PRINT("QRCode Start Time= %d \r\n",OSTimeGet());
				t1 = OSTimeGet();
				res =(INT8U*) QR_Decode(QRCode_Working_Memory);
				DBG_PRINT("QRCode Decoding Time: %d ms \r\n",(OSTimeGet()-t1)*10);
		
					if(res==NULL)
					{
						DBG_PRINT("QRCode Decode NG! \r\n");
			//			if(SPU_Get_SingleChannel_Status(13) == 0)
						{
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)snd_buffer_addr[1], 64, 100, 13);
						}
						error_num = QR_GetErrorNum(QRCode_Working_Memory);
						switch(error_num)
						{
						default: break;
						}
					}
					else
					{
			//			if(SPU_Get_SingleChannel_Status(13) == 0)
						{
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)snd_buffer_addr[0], 64, 100, 13);
						}
						DBG_PRINT("QRCode Decode OK! \r\n");
						DBG_PRINT("QRCode Result: %s\r\n", resultstream);
					}
				gp_free(QRCode_Working_Memory);	
		#endif
				
			}
		}
	#if USE_ADKEY == 1	
		if(ADKEY_IO2)
	#else
		if(IO2)
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
//				DBG_PRINT("file name = %s\r\n", path);	
				video_encode_set_jpeg_quality(90);
				video_encode_capture_picture(src);		
		#ifdef BARCODE
				
				img.width = 640;
				img.height = 480;
				img.widthStep = 640*2;
				img.ch = 2;
				img.format = IMG_FMT_UYVY;//IMG_FMT_YUYV;
				img.ptr = (unsigned char *)qrcode_frame;
				
				BarCode_Working_Memory = (INT8U *)gp_malloc_align((INT32U)gpbarcode_dec_get_mem_size(), 64);
				gpbarcode_dec_init(BarCode_Working_Memory);
				gpbarcode_dec_set_step(BarCode_Working_Memory, 1, 2);
				t1 = OSTimeGet();
				n = gpbarcode_dec(BarCode_Working_Memory, &img);
				DBG_PRINT("BARCode Decoding Time: %d ms \r\n",(OSTimeGet()-t1)*10);

				for(i = 0 ; i < n ; i++)
				{
					GPBARCODE_DEC_ID id = gpbarcode_get_decoder_id(BarCode_Working_Memory, i);

					DBG_PRINT("[%s]: \" %s \" \r\n", barcode_get_decoder_name(id), gpbarcode_get_data(BarCode_Working_Memory, i));			
				}
				gp_free(BarCode_Working_Memory);	
		#endif
		
				
			}		
		}
	}
	
	video_encode_preview_stop();
	video_encode_exit();        // Release the resources needed by Video encoder
}

