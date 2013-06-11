#include <stdio.h>
#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"

#define USE_DISK	FS_SD
//#define USE_DISK    FS_NAND1

#define DV_PLAY			0
#define DV_RECORD		1

#define TAR_WIDTH		640
#define TAR_HEIGHT		480
#define SNR_WIDTH		640
#define SNR_HEIGHT		480
#define DIP_WIDTH		640/2
#define DIP_HEIGHT		480/2
#define FPS				19
#define SPR				8000 
 
#define C_FILE_NODE_SIZE			500

INT16U	file_node_buf[C_FILE_NODE_SIZE];
struct sfn_info file_fs_info;
struct STFileNodeInfo musicFNodeInfo;

extern void TFT_TD025THED1_Init(void);
extern INT8U  Display_Device;

//=================================================================================================
//	DV demo code
//=================================================================================================
void Digital_Video_Demo(void)
{	
	char  path[64];
	INT8U OperationMode; 
	INT8S volume, play_index; 
	FP64  zoom_ratio;
	INT32S nRet;
	INT32U folder_nums, file_nums;
	VIDEO_CODEC_STATUS status;
	VIDEO_INFO	information;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;
		
	while(1)
	{
		if( _devicemount(USE_DISK))             
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
	#if	USE_DISK == FS_NAND1
			nRet = DrvNand_lowlevelformat();
			DBG_PRINT("NAND LOW LEVEL FORMAT = %d \r\n", nRet);	
			nRet = _format(FS_NAND1, FAT32_Type);
			DBG_PRINT("Format NAND = %d\r\n", nRet);
			DrvNand_flush_allblk();		
	#endif			
			_deviceunmount(USE_DISK);					
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}
	
	tft_init();
	tv_init();
	
	adc_key_scan_init(); //init key scan
	
	DBG_PRINT("S8 -> Display in TFT\r\n");
	DBG_PRINT("S1~S7 -> Display in TV\r\n");
	// Initialize display device
	adc_key_scan();
	if(ADKEY_IO8)
	{    
		Display_Device = 1;	
		TFT_TD025THED1_Init();	
	}
	else
	{
		Display_Device = 0;
		tv_start (TVSTD_NTSC_J, TV_QVGA, TV_NON_INTERLACE);
	}
	user_defined_video_codec_entrance();
	
	//init as dv play mode
	volume = 0x3F;
	play_index = 0;
	zoom_ratio = 1;
	nRet = 0;
	OperationMode = DV_RECORD;
	video_encode_entrance();
	arg.bScaler = 1;
	arg.TargetWidth = TAR_WIDTH;
	arg.TargetHeight = TAR_HEIGHT;
	arg.SensorWidth	= SNR_WIDTH;
	arg.SensorHeight = SNR_HEIGHT;
	arg.DisplayWidth = DIP_WIDTH;
	arg.DisplayHeight = DIP_HEIGHT;
	arg.VidFrameRate = FPS;
	arg.AudSampleRate = SPR;
	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; //for display
	video_encode_preview_start(arg);
	
	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)     
		{
			switch(OperationMode)
			{//exit the old mode
			case DV_PLAY:
				if(video_decode_status() == VIDEO_CODEC_PROCESSING)
					video_decode_stop();
				video_decode_exit();
				break;
			case DV_RECORD:
				zoom_ratio = 1;
				video_encode_preview_stop();
				OSTimeDly(10);// wait csi stop and scaler task stop
				video_encode_exit();
				video_codec_show_image(1, 0, 0,IMAGE_OUTPUT_FORMAT_YUYV);
				break;
			}
			
			if(OperationMode == DV_PLAY)
				OperationMode = DV_RECORD;
			else if(OperationMode == DV_RECORD)
				OperationMode = DV_PLAY;
			
			switch(OperationMode)
			{//enter new mode
			case DV_PLAY:
				video_decode_entrance();
				musicFNodeInfo.disk = USE_DISK;
				gp_strcpy((INT8S *) musicFNodeInfo.extname, (INT8S *) "avi");
				musicFNodeInfo.FileBuffer = file_node_buf;
				musicFNodeInfo.FileBufferSize = C_FILE_NODE_SIZE;
				musicFNodeInfo.FolderBuffer = NULL;
				musicFNodeInfo.FolderBufferSize = 0;
				GetFileNumEx(&musicFNodeInfo, &folder_nums, &file_nums);
				break;
				
			case DV_RECORD:
				video_encode_entrance();
				arg.bScaler = 1;
				arg.TargetWidth = TAR_WIDTH;
				arg.TargetHeight = TAR_HEIGHT;
				arg.SensorWidth	= SNR_WIDTH;
				arg.SensorHeight = SNR_HEIGHT;
				arg.DisplayWidth = DIP_WIDTH;
				arg.DisplayHeight = DIP_HEIGHT;
				arg.VidFrameRate = FPS;
				arg.AudSampleRate = SPR;
				arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; //for display
				video_encode_preview_start(arg);
				break;	
			}
		}	
		else if(ADKEY_IO2)
		{//start play 
			switch(OperationMode)
			{
			case DV_PLAY:
				if(video_decode_status() == VIDEO_CODEC_PROCESS_END)
				{
					DBG_PRINT("video_decode_start\r\n");
					GetFileNodeInfo(&musicFNodeInfo, play_index, &file_fs_info); 
					DBG_PRINT("FILE = %s\r\n", (char *)file_fs_info.f_name);
					play_index++;
					if(play_index >= file_nums)
						play_index = 0;
										
					arg.bScaler = TRUE;
					arg.bUseDefBuf = FALSE;
					arg.TargetWidth = DIP_WIDTH;
					arg.TargetHeight = DIP_HEIGHT;
					arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV;	
					
					src.type = SOURCE_TYPE_FS;
					src.Format.VideoFormat = MJPEG;//only MJPEG
					src.type_ID.FileHandle = sfn_open(&file_fs_info.f_pos);  	
					src.type_ID.temp = sfn_open(&file_fs_info.f_pos);  	
				
					if((src.type_ID.FileHandle < 0)||(src.type_ID.temp < 0))
					{
						DBG_PRINT("file open fail\r\n");
						close(src.type_ID.FileHandle);
						close(src.type_ID.temp);
						continue;
					}
					
					status = video_decode_paser_header(&information, arg, src);
					if(status != VIDEO_CODEC_STATUS_OK)
					{
						DBG_PRINT("paser header fail !!!\r\n");
						continue;
					}	
					video_decode_start(arg, src);
					audio_decode_volume(volume);	
				}
				break;
			case DV_RECORD:
				if(video_encode_status() == VIDEO_CODEC_PROCESS_END)
				{
					DBG_PRINT("video_encode_start\r\n");
					if(USE_DISK == FS_SD)
						sprintf((char *)path, (const char *)"C:\\avi_rec%d.avi", nRet++);
					else if(USE_DISK == FS_NAND1)
						sprintf((char *)path, (const char *)"A:\\avi_rec%d.avi", nRet++);
					
					src.type = SOURCE_TYPE_FS;
					src.type_ID.FileHandle = open(path, O_WRONLY|O_CREAT|O_TRUNC);
					if(src.type_ID.FileHandle < 0)
					{
						DBG_PRINT("file open fail\r\n");
						continue;
					}
					else
						DBG_PRINT("file name = %s\r\n", path);
					src.Format.VideoFormat = MJPEG;	//only MJPEG
					video_encode_start(src);
				}
				break;
			}
		}
		else if(ADKEY_IO3)
		{//stop 
			switch(OperationMode)
			{
			case DV_PLAY:
				if(video_decode_status() == VIDEO_CODEC_PROCESSING)
				{
					DBG_PRINT("video_decode_stop\r\n");
					video_decode_stop();
				}
				break;
				
			case DV_RECORD:
				if(video_encode_status() == VIDEO_CODEC_PROCESSING)
				{
					DBG_PRINT("video_encode_stop\r\n");
					video_encode_stop();
			#if USE_DISK == FS_NAND1
					DBG_PRINT("DrvNand_flush_allblk\r\n");
					DrvNand_flush_allblk();
			#endif
				}
				break;	
			}
		}
		else if(ADKEY_IO4)
		{	//pause 
			switch(OperationMode)
			{
			case DV_PLAY:
				if(video_decode_status() == VIDEO_CODEC_PROCESSING)
				{
					DBG_PRINT("video_decode_pause\r\n");
					video_decode_pause();
				}
				break;
				
			case DV_RECORD:
				if(video_encode_status() == VIDEO_CODEC_PROCESSING)
				{
					DBG_PRINT("video encode pause\r\n");
					video_encode_pause();
				}
				break;
			}
		}
		else if(ADKEY_IO5)
		{//resume 
			switch(OperationMode)
			{
			case DV_PLAY:
				if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE)
				{
					DBG_PRINT("video_decode_resume\r\n");
					video_decode_resume();
				}
				break;
				
			case DV_RECORD:
				if(video_encode_status() == VIDEO_CODEC_PROCESSING)
				{
					DBG_PRINT("video encode resume\r\n");
					video_encode_resume();
				}
				break;
			}
		}
		else if(ADKEY_IO6)
		{//volume up / zoom in 
			switch(OperationMode)
			{
			case DV_PLAY:
				volume++;
				if(volume > 0x3F)
				{
					volume = 0x3F;
					continue;
				}
				DBG_PRINT("volume +\r\n");
				audio_decode_volume(volume);
				break;
				
			case DV_RECORD:
				zoom_ratio += 0.1;
		    	if(zoom_ratio > 4)
		    	{	
		    		zoom_ratio = 4;
    				continue;
        		}
        		DBG_PRINT("Zoom in\r\n");
        		video_encode_set_zoom_scaler(zoom_ratio);
				break;
			}
		}
		else if(ADKEY_IO7)
		{// volume down / zoom out
			switch(OperationMode)
			{
			case DV_PLAY:
				volume --;
				if(volume < 0)
				{
					volume = 0;
					continue;
				}
				DBG_PRINT("volume -\r\n");
				audio_decode_volume(volume);
				break;
				
			case DV_RECORD:	
				zoom_ratio -= 0.1;
		    	if(zoom_ratio < 1)
		    	{	
		    		zoom_ratio = 1;
		    		continue;
            	}
            	DBG_PRINT("Zoom out\r\n");
            	video_encode_set_zoom_scaler(zoom_ratio);
            	break;
			}
		}
		else if(ADKEY_IO8)
		{
		
		}
	}
}
