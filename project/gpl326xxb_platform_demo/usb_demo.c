#include "application.h"
#include "state_cam.h"

//#define Nand_test
//#define SD_test

extern void state_usb_entry(void* para1);

void Usb_Device_Demo(void)
{	
#if 0
	INT16U *pBUF,i;
	INT32U u32StartSectorNum = 0;
	INT32U seccount = 1;
	NAND_APP_Initial();
	pBUF= (INT16U *) gp_malloc_align(1024* 1, 16);
	for(i = 0; i < 256; i++)
		pBUF[i] = i;
	NandBootWriteSector((INT32U)(u32StartSectorNum&0xfffffffc), seccount , (INT32U)pBUF);
	NandBootReadSector((INT32U)(u32StartSectorNum&0xfffffffc), seccount , (INT32U)pBUF);
#endif

#ifdef Nand_test
INT16S ret;
#endif

DBG_PRINT("Into Usb_Device_Demo(void)\r\n");

#ifdef Nand_test
	
	while(1)
	{
		if(_devicemount(FS_NAND1))					// Mount device nand
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", FS_NAND1);
			ret = _format(FS_NAND1, FAT32_Type); 
 			DrvNand_flush_allblk();
			_deviceunmount(FS_NAND1); 
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", FS_NAND1);
			break;
		}
	}
#endif

#ifdef SD_test
	while(1)
	{

		
	storage_sdms_detection();
	if(storage_detection(1))
	{
	DBG_PRINT("sd plug in\r\n");
		if(_devicemount(FS_SD))
			{
			DBG_PRINT("SD_MOUNT Fail\r\n");
			 _format(FS_SD,FAT32_Type);	
			}
		else
			{ 
			DBG_PRINT("SD_MOUNT Success\r\n");				  				
		   break;
			}
		}
		
	}
#endif

	
	storages_init(0x30);//Nand

	state_usb_entry((void *)1);
	
	//stop detect 
	storage_polling_stop();
	while(1);
}

void Usb_MSDC_task_entry(void *p_arg)
{
	Usb_Device_Demo();
}

void Usb_WebCam_Demo(void)
{
	VIDEO_ARGUMENT arg;
	
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
		 	
	arg.bScaler = 1;
	arg.TargetWidth = 640;
	arg.TargetHeight = 480;
	arg.SensorWidth	= 640;
	arg.SensorHeight = 480;
	arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;	//display width
	arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
	arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;//display buffer height
	
	arg.VidFrameRate = 30;
	arg.AudSampleRate = 8000;
	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; //for display
	video_encode_webcam_start(arg);
	
	storages_init(0x10);
	usb_cam_entrance(0, 640, 480, 8000);
	
	usb_webcam_state();
	
	DBG_PRINT("P-CAM Exit\r\n");
	video_encode_webcam_stop();
	
	OSTimeDly(30);
	usb_cam_exit();
	
	//stop detect
	storage_polling_stop();
	while(1);
}
