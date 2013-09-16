#include <stdio.h>
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "video_codec_callback.h"

extern int memset(void*, int , int);
extern int oemuni_to_utf8(CHAR*, CHAR*);

static void sent_utf8_consol(char *filename);
void video_list_create(char* video_name);
void video_list_insert(char* video_name);
static int  unicode_filename_strlen(char *filename);
void video_filelist(INT32U file_head);
void video_mjpeg_select(void);
void video_mpeg4_select(void);
void video_format_select_task_entry(void * pro);
INT8S video_format_select_task_create(void);
static void video_get_file_total(INT32U file_header, INT32U *total);

#define USE_DISK	FS_SD //FS_NAND1//FS_USBH

#define USE_ADKEY			1
#define USER_DEFINE_BUFFER	1

#define VideoFormatSelectCmdBufferSize	5
#define VideoFormatSelectTaskStackSize	1024

OS_EVENT *VideoFormatSelectCmd_q;        //Message queen index
void *VideoFormatSelectCmdBuffer[VideoFormatSelectCmdBufferSize];	    //Point message array index
INT32U VideoFormatSelectTaskStack[VideoFormatSelectTaskStackSize];

INT8S control_flag = 0;

const CHAR* VIDEO_SOURCE_TYPE[]={
	(CHAR*)"*.avi",   
	(CHAR*)"*.mp4",   
	(CHAR*)"*.jpg",   
};

const INT16U VIDEO_TYPE_UNICDOE[][6]={
	{0x002A,0x002E,0x0061,0x0076,0x0069,0x0000},//*.avi   
	{0x002A,0x002E,0x006D,0x0070,0x0034,0x0000},//*.mp4  
	{0x002A,0x002E,0x006A,0x0070,0x006A,0x0000},//*.jpg unicode
	{0x002A,0x002E,0x0061,0x0076,0x0069,0x0000} //*.avi   
};

INT16U sd_codec_store_v[]={0x0063,0x003a,0x005c,0x005c,0x0000};

typedef struct video_list_s{
	char video_name[128];
	struct video_list_s *next,*prev;
} video_list_t;

video_list_t *video_head;

#define FILE_LEN  sizeof(video_list_t)


typedef enum VIDEO_DECODE_TYPE{
	DECODE_MJPEG = 0X01,   
	DECODE_MPEG4,   
	ENCODE_JPEG,
	ENCODE_MJPEG	   
}VIDEO_FORMAT_ARR;

typedef enum CONTROL_SELECT{
	SPEED_CONTROL = 0x00,
	VOLUME_CONTROL,
	FF_FB_CONTROL
}SELECT_CONTROL;



typedef struct video_info_s{
	INT8U  mode;   
	INT32U type;   
	INT8U* path;   
	INT8U* avi_dec_buffer[2];   
	INT32S index;   
	INT32U total;   
}video_info_t;

video_info_t *data;

#define VIDEO_PLAY_PREV		0x01
#define VIDEO_PLAY_NEXT		0x02
#define VIDEO_PLAY_LOOP		0x03


#define VIDEO_ENCODE_MODE	0x01  
#define VIDEO_DECODE_MODE	0x02

#define MAX_PATH_LENGGTH	64  
	

void Video_Decode_Demo()
{
	INT8U   *avi_dec_buffer[2];  

	INT32S  nRet, i;
	INT8S   play_volume = 63;
	FP32    play_speed;
	VIDEO_CODEC_STATUS status;
	VIDEO_INFO	information;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	INT8U err;

	video_list_t *decode_video;
	
	INT8U VideoFormat_select = 0;
	
	//init 
	avi_dec_buffer[0] = avi_dec_buffer[1] = 0;
	play_speed = 1;
	
	DBG_PRINT("\r\n***************************************************\r\n");
	DBG_PRINT("              This is  VIDEO DECODE DEMO         **\r\n");
	DBG_PRINT("KEY_1 select decode type                         **\r\n");
	DBG_PRINT("KEY_2 play next file                             **\r\n");
	DBG_PRINT("KEY_3 play previous                              **\r\n");
	DBG_PRINT("KEY_4 Decode Stop                                **\r\n");
	DBG_PRINT("KEY_5 Select speed control/volume control/FF&FB  **\r\n");
	DBG_PRINT("KEY_6 Level up                                   **\r\n");
	DBG_PRINT("KEY_7 Level down                                 **\r\n");
	DBG_PRINT("KEY_8 snap a frame while video decoding          **\r\n");
	DBG_PRINT("***************************************************\r\n");	
	
		
	// Mount device
	while(1)
	{
		if(_devicemount(USE_DISK))					
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

	ChangeCodePage(UNI_UNICODE);

#if USER_DEFINE_BUFFER == 1	
	nRet = C_DISPLAY_DEV_HPIXEL*C_DISPLAY_DEV_VPIXEL*2;	
	for(i=0; i<2; i++)
	{
		avi_dec_buffer[i] = (INT8U *)gp_malloc_align(nRet, 64);
		while(!avi_dec_buffer[i]);
	}
#endif

    video_format_select_task_create();
	
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
		#if (MCU_VERSION >= GPL326XX && MCU_VERSION < GPL327XX)
			if(VideoFormat_select == 0){			
				src.Format.VideoFormat = MJPEG;
				video_mjpeg_select();
				DBG_PRINT("Video format is MJPEG\r\n");
				VideoFormat_select = 1;
			}
			else{
				src.Format.VideoFormat = MPEG4;
				video_mpeg4_select();
				VideoFormat_select = 0;
				DBG_PRINT("Video format is MPEG4\r\n");
			}	
		#else
			src.Format.VideoFormat = MJPEG;
			video_mjpeg_select();
			DBG_PRINT("Video format is MJPEG\r\n");
		#endif

			if(video_head != NULL){
				decode_video = video_head;
			//	decode_video = decode_video->next;			
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO2)         // Stop decoding the current file and play next
	#else
		else if(IO2)
	#endif	
		{
			decode_video = decode_video->next;
			if(decode_video == NULL){
				decode_video = video_head;
				decode_video = decode_video->next;
			}

			sent_utf8_consol(decode_video->video_name);	
						
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
			arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
			arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
			arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height
			
			arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format
			
			if(USE_DISK == FS_SD)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY);	//open file handle
			else if(USE_DISK == FS_NAND1)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY); //open file handle
			else if(USE_DISK == FS_USBH)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY);	//open file handle
			
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
			audio_decode_volume(play_volume);
			
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO3)     //Stop decoding the current file and play back
	#else
		else if(IO3)
	#endif
		{				
			if(video_decode_status() != VIDEO_CODEC_PROCESS_END)
				video_decode_stop();

			
			if(decode_video == video_head->next){				
				decode_video = video_head->next;
			}
			else			
				decode_video = decode_video->prev;
			
			sent_utf8_consol(decode_video->video_name);	

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
			arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
			arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
			arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height
			
			arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format
			
			if(USE_DISK == FS_SD)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY);	//open file handle
			else if(USE_DISK == FS_NAND1)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY); //open file handle
			else if(USE_DISK == FS_USBH)
				src.type_ID.FileHandle = open(decode_video->video_name, O_RDONLY);	//open file handle
			
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
			audio_decode_volume(play_volume);
					
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO4)
	#else
		else if(IO4)    //Decode Stop
	#endif
		{

			if(video_decode_status() != VIDEO_CODEC_PROCESS_END)
			{
				DBG_PRINT("video_decode_stop\r\n");	
				video_decode_stop();
			}
			
			err = OSTaskDel(31);
			if(err == OS_NO_ERR)	
				DBG_PRINT("OSTaskDel Success \r\n");
			else
				DBG_PRINT("OSTaskDel Fail\r\n");											
			
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO5)
	#else
		else if(IO5)
	#endif
		{	
			control_flag ++;
			if(control_flag >= 3)
				control_flag = 0;
			if(control_flag == 0){
				DBG_PRINT("SPEED_CONTROL select \r\n");
			}
			else if(control_flag == 1){
				DBG_PRINT("VOLUME_CONTROL select \r\n");
			}
			else{
				DBG_PRINT("FF_FB_CONTROL select \r\n");
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO6)   //level up
	#else
		else if(IO6)
	#endif
		{	

			switch(control_flag)
			{
				case SPEED_CONTROL:
					play_speed += 0.5;
					if(play_speed > 4)	
						play_speed = 3.5;
					nRet = play_speed*10;
					DBG_PRINT("speed = %d.%d\r\n", nRet/10, nRet%10);	
					video_decode_set_play_speed(play_speed);
					break;
				case VOLUME_CONTROL:
					play_speed += 0.5;
					if(play_speed > 4)	
						play_speed = 3.5;
					nRet = play_speed*10;
					DBG_PRINT("speed = %d.%d\r\n", nRet/10, nRet%10);	
					video_decode_set_play_speed(play_speed);
					break;
				case FF_FB_CONTROL:
					if(video_decode_status() == VIDEO_CODEC_PROCESSING)
					{
						nRet = video_decode_get_current_time();
						nRet += 5;
						video_decode_set_play_time(nRet);
						DBG_PRINT("Set Play Time = %dsec\r\n", nRet);
					}
					break;
				default :
					break;
			}								
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO7)   //level down
	#else
		else if(IO7)
	#endif
		{	
			switch(control_flag)
			{
				case SPEED_CONTROL:
					play_speed -= 0.5;
					if(play_speed < 0.5)	
						play_speed = 0.5;
					nRet = play_speed*10;
					DBG_PRINT("speed = %d.%d\r\n", nRet/10, nRet%10);	
					video_decode_set_play_speed(play_speed);
					break;
				case VOLUME_CONTROL:
					play_volume --;
					if(play_volume < 0)
						play_volume = 0;
					DBG_PRINT("volume = %d \r\n",play_volume);
					audio_decode_volume(play_volume);
					break;
				case FF_FB_CONTROL:
					if(video_decode_status() == VIDEO_CODEC_PROCESSING)
					{
						nRet = video_decode_get_current_time();
						nRet -= 5;
						video_decode_set_play_time(nRet);
						DBG_PRINT("Set Play Time = %dsec\r\n", nRet);
					}
					break;	
				default :
					break;						
			}
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO8)
	#else 
		else if(IO8)
	#endif
		{	
			DBG_PRINT("KEY_8 pressed \r\n");
	
		 if(video_decode_status() == VIDEO_CODEC_PROCESSING){	
					video_decode_pause();
					video_decode_FrameReady((INT8U*)avi_dec_buffer[0]);
		}
		else if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE){
					video_decode_resume();
		}
		else
			continue;
		}	
	}
}

void video_type_select(INT32U type)
{
	OSQPost(VideoFormatSelectCmd_q,(void *)type);   
	OSTimeDly(2);   
}

void video_format_select_task_entry(void * pro)
{
	INT8U err;
	INT32U msg_id;

	DBG_PRINT("image_type_select_task_entry  success !\r\n");
	
	while(1)
	{	
		msg_id = (INT32U)OSQPend(VideoFormatSelectCmd_q,0,&err);
		if(!msg_id || err != OS_NO_ERR)
			continue;
		switch(msg_id){
			case DECODE_MJPEG:
				video_filelist(DECODE_MJPEG - 1);
				break;
			case DECODE_MPEG4:
				video_filelist(DECODE_MPEG4 - 1);
				break;
			case ENCODE_JPEG:         
				video_get_file_total(ENCODE_JPEG, &(data->total));            
				break;            
			case ENCODE_MJPEG:         
				video_get_file_total(ENCODE_MJPEG, &(data->total));            
				break;    

			default :

				break;
	
		}
		OSTimeDly(1);	
	}
}


INT8S video_format_select_task_create(void)
{
	INT32S ret;

	VideoFormatSelectCmd_q = OSQCreate(VideoFormatSelectCmdBuffer,VideoFormatSelectCmdBufferSize);

	if(!VideoFormatSelectCmd_q){
		DBG_PRINT("VideoFormatSelectCmd_q create fail !\r\n");
		return STATUS_FAIL;
	}

	ret = OSTaskCreate(video_format_select_task_entry,(void *)0,&VideoFormatSelectTaskStack[VideoFormatSelectTaskStackSize -1],31);

	if(ret < 0 ){
		DBG_PRINT("image_type_select_task_entry create fail !\r\n");
		return STATUS_FAIL;
	}

	return STATUS_OK;
}

void video_mpeg4_select(void)
{
	OSQPost(VideoFormatSelectCmd_q,(void *)DECODE_MPEG4);
	OSTimeDly(2);
}

void video_mjpeg_select(void)
{
	OSQPost(VideoFormatSelectCmd_q,(void *)DECODE_MJPEG);
	OSTimeDly(2);
}

/***********************************************************************************************
UNICODE file name get
************************************************************************************************/
static int  unicode_filename_strlen(char *filename)
{
	unsigned short *temp=(unsigned short *)filename;
	int count=0;
	while(*temp!=0x0000)
	{
		temp++;
		count +=2;
	}
	return count;
}


/***********************************************************************************************
convert to utf8
************************************************************************************************/
static void sent_utf8_consol(char *filename)
{        
	char *utf8;       
	int utf8_len;        
	int ret=0;      
	utf8_len=unicode_filename_strlen(filename)*3/2;        
	utf8=gp_malloc(utf8_len);   
	ret=oemuni_to_utf8(filename,utf8);     
	if(ret==0)        
		DBG_PRINT("filename:%s\r\n",utf8);       
	else        
		DBG_PRINT("change to utf8 err\r\n"); 
	gp_free(utf8);
}	

void video_filelist(INT32U file_head)
{
	struct f_info finfo;
	int ret = 0;
	
	if(USE_DISK == FS_SD){
			DBG_PRINT("Media : SDCard \r\n");
			//chdir("C:\\");
			chdir((char *)sd_codec_store_v);
		}
		else if(USE_DISK == FS_NAND1){
			DBG_PRINT("Media : FS_NAND1 \r\n");
			chdir("A:\\");
		}
		else if(USE_DISK == FS_USBH){
			DBG_PRINT("Media : FS_USBH\r\n");
			chdir("G:\\");
		}
	
	memset(&finfo,0,sizeof(finfo));

	if (_findfirst ((char *)VIDEO_TYPE_UNICDOE[file_head], &finfo, D_FILE))	{
		DBG_PRINT("Error:no image files !!!\r\n");
	}
	
	do	{
		if (ret == 0)
			video_list_create((char*)&finfo.f_name);
		else
			video_list_insert((char*)&finfo.f_name);
				
		ret ++;
	}
	while (_findnext (&finfo) == 0);

	DBG_PRINT("Total image member : %d \r\n",ret);

}

void video_list_create(char* video_name)
{
	video_list_t *temp;

    video_head = (video_list_t *)gp_malloc(FILE_LEN + unicode_filename_strlen((char *)video_name)+2);
    video_head->next = video_head;
    video_head->prev = video_head;
	gp_memcpy((INT8S*)&video_head->video_name, (INT8S*)"head", sizeof(video_head->video_name));

    temp = video_head;

    temp->next = (video_list_t *)gp_malloc(FILE_LEN + unicode_filename_strlen((char *)video_name)+2);
    temp = temp->next;
    temp->next = video_head;
    temp->prev = video_head;
    video_head->prev = temp;
	gp_memcpy((INT8S*)&temp->video_name, (INT8S*)video_name, FILE_LEN + unicode_filename_strlen((char *)video_name)+2);
	sent_utf8_consol(video_name);
	
}

void video_list_insert(char* video_name)
{
	video_list_t *p;
	video_list_t *temp;
	
	p = video_head->prev;

    temp = (video_list_t *)gp_malloc(FILE_LEN + unicode_filename_strlen((char *)video_name)+2);
    gp_memcpy((INT8S*)&temp->video_name, (INT8S*)video_name, FILE_LEN + unicode_filename_strlen((char *)video_name)+2);

	sent_utf8_consol(video_name);

    temp->next = p->next;
    p->next = temp;
    (temp->next)->prev = temp;
    temp->prev = p;
}



#if 1
void video_select_mode(VIDEO_ARGUMENT arg)
{
	switch(data->mode){   
		case VIDEO_ENCODE_MODE:
			DBG_PRINT("Enter Encode Mode\n");  
			if(video_decode_status() != VIDEO_CODEC_PROCESS_END) {        
				video_decode_stop();
			}
			video_decode_exit();         
			video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder 
			video_encode_preview_start(arg);   
			break;         
		case VIDEO_DECODE_MODE: 
			data->index = data->total;
			DBG_PRINT("Enter Decode Mode\n");         
			video_encode_preview_stop();         
			video_encode_exit();         // Release the resources needed by Video encoder         
		    video_decode_entrance();     // Initialize and allocate the resources needed by Video decoder      
			break;         
		default:
			break;      
	}   
}
#endif
void video_decode_play(	VIDEO_ARGUMENT *arg, MEDIA_SOURCE *src, INT32U mode)
{
	int status;   
	VIDEO_INFO	information;    

	if(video_decode_status() != VIDEO_CODEC_PROCESS_END)   
		video_decode_stop();      

	(*arg).bScaler = 0x01;	//scaler output size or not    
	   
#if	USER_DEFINE_BUFFER == 1
	(*arg).bUseDefBuf = TRUE;	//use user define buffer      
	(*arg).AviDecodeBuf1 = data->avi_dec_buffer[0];   
	(*arg).AviDecodeBuf2 = data->avi_dec_buffer[1];   
#else	  
	(*arg).bUseDefBuf = FALSE;	//auto alloc buffer size      
	(*arg).AviDecodeBuf1 = NULL;   
	(*arg).AviDecodeBuf2 = NULL;   
#endif
		      
	(*arg).DisplayWidth = C_DISPLAY_DEV_HPIXEL;		//display width      
	(*arg).DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height      
	(*arg).DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width     
	(*arg).DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height    
			         
	(*arg).OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format 


	if(mode == VIDEO_PLAY_PREV){
		data->index = (--data->index <= 0) ? data->total :data->index;
	}
	else if(mode == VIDEO_PLAY_NEXT){
		data->index = (++data->index > data->total) ?	1 : data->index;
	}
	
	if(USE_DISK == FS_SD)         
		sprintf((char *)(data->path), (const char *)"C:\\VIDEO_ENCODE\\avi_rec%03d.avi", data->index);            
	else if(USE_DISK == FS_NAND1)         
		sprintf((char *)(data->path), (const char *)"A:\\VIDEO_ENCODE\\avi_rec%03d.avi", data->index);
	DBG_PRINT("Video Decode Play[%s]\n", data->path);
	(*src).type_ID.FileHandle = open((char*)(data->path), O_RDONLY);	//open file handl 	

	if((*src).type_ID.FileHandle < 0)   
	{   
		DBG_PRINT("file open fail\r\n");      
		close((*src).type_ID.FileHandle);      
	}	     
	   
	(*src).type = SOURCE_TYPE_FS;	//play file by file system     
	   
	DBG_PRINT("video_decode_start\r\n");	      
	status = video_decode_paser_header(&information, *arg, *src);   
	if(status != VIDEO_CODEC_STATUS_OK)   
	{   
		DBG_PRINT("paser header fail !!!\r\n");      
	}   
	DBG_PRINT("Aud SampleRate = %d\r\n", information.AudSampleRate);   
	DBG_PRINT("Vid FrameRate = %d\r\n", information.VidFrameRate);   
	DBG_PRINT("resolution = %d x %d\r\n", information.Width, information.Height);   
	DBG_PRINT("Total Time = %d seconds\r\n", information.TotalDuration);   
	   
	status = video_decode_start(*arg, *src);   
	if(status != START_OK)   
	{   
		DBG_PRINT("video start fail!!!\r\n");      
	}   
	audio_decode_volume(63);   

}

void video_encode_init(void)
{
	int i, size;   
	/* init global variables */   
	data = gp_malloc(sizeof(video_info_t));   
	if(data == NULL){   
		DBG_PRINT("Alloc global data error\n");      
	}   
	memset(data, 0, sizeof(video_info_t));   

	data->mode = VIDEO_ENCODE_MODE;
	data->path = gp_malloc(MAX_PATH_LENGGTH);   

#if USER_DEFINE_BUFFER == 1	
	size = C_DISPLAY_DEV_HPIXEL*C_DISPLAY_DEV_VPIXEL*2;	   
	for(i=0; i<2; i++)   
	{   
		data->avi_dec_buffer[i] = (INT8U *)gp_malloc_align(size, 64);      
		while(!data->avi_dec_buffer[i]);      
	}   
#endif

	video_format_select_task_create();   
	user_defined_video_codec_entrance();     
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder 

	data->type = ENCODE_MJPEG;            
	video_type_select(ENCODE_MJPEG);
}

void video_encode_uninit(void)
{
	int i = 0;
	for(i=0; i< 2; i++){
		if(data->avi_dec_buffer[i] != NULL){
			DBG_PRINT("free avi decode buffer\n");
			gp_free(data->avi_dec_buffer[i]);
			data->avi_dec_buffer[i] = NULL;
		}

	}
	gp_free(data->path);
	DBG_PRINT("free file path buffer\n");
	gp_free(data);  
	DBG_PRINT("free global data\n");
	data = NULL;   
}

//=================================================================================================
//	AVI encode demo code 
//=================================================================================================
void Video_Encode_Demo()
{	  
	char 	path[64];     
	INT32S  nRet;   
	INT32S  ret;   
	INT64U  disk_free;   
	FP32    zoom_ratio;   
	VIDEO_ARGUMENT arg;   
	MEDIA_SOURCE   src;   

	DBG_PRINT("\r\n***************************************************\r\n");   
	DBG_PRINT("              This is  VIDEO ENCODE DEMO         **\r\n");   
	DBG_PRINT("KEY_1                                            **\r\n");   
	DBG_PRINT("KEY_2 encode a avi file in MJPEG format          **\r\n");   
	DBG_PRINT("KEY_3 stop video encoding                        **\r\n");   
	DBG_PRINT("KEY_4 pause/resume video encoding                **\r\n");   
	DBG_PRINT("KEY_5 capture a jpg format file                  **\r\n");   
	DBG_PRINT("KEY_6 play next                                  **\r\n");   
	DBG_PRINT("KEY_7 play previous                              **\r\n");   
	DBG_PRINT("KEY_8 exit video encoding demo                   **\r\n");   
	DBG_PRINT("***************************************************\r\n");   

	// Mount device   
	while(1)   
	{   
		if(_devicemount(USE_DISK))					                   
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
	video_encode_init();   
#if USE_ADKEY == 1	 
	//ADC init key scan   
	adc_key_scan_init();   
#endif

	if (USE_DISK == FS_SD){	   
		ret = chdir("C:\\VIDEO_ENCODE\\");      
		if(ret < 0)      
			mkdir("VIDEO_ENCODE");         
	}   
	else if(USE_DISK == FS_NAND1){   
		ret = chdir("A:\\VIDEO_ENCODE\\");      
		if(ret < 0)      
			mkdir("VIDEO_ENCODE");         
	}   
	else{   
		ret = chdir("G:\\VIDEO_ENCODE\\");      
		if(ret < 0)      
			mkdir("VIDEO_ENCODE");	            
	}   
	   
	   
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
#if 0
		/* Loop  */
		if((data->mode == VIDEO_DECODE_MODE) && (video_decode_status() == VIDEO_CODEC_PROCESS_END)){
			DBG_PRINT("loop play current file\n");
			video_decode_play(&arg, &src, VIDEO_PLAY_LOOP);
		}
#endif
	#if USE_ADKEY == 1	    
		if(ADKEY_IO1)      
	#else   
		else if(IO1)      
	#endif   
		{      
			         
		}	        
	#if USE_ADKEY == 1   
		else if(ADKEY_IO2)                // Start to decode the specified file       
	#else   
		if(IO2)      
	#endif	    
		{ 
			if(data->mode != VIDEO_ENCODE_MODE){
				data->mode = VIDEO_ENCODE_MODE;
				video_select_mode(arg);
			}

			if(data->type != ENCODE_MJPEG){         
				data->type = ENCODE_MJPEG;            
				video_type_select(ENCODE_MJPEG);            
			}         
				            
			if(video_encode_status() == VIDEO_CODEC_PROCESSING)         
				continue;            
						                  
			OSTimeDly(3);         
			DBG_PRINT("%d", data->total); 

			data->index = data->total;
						
			if(USE_DISK == FS_SD)         
				sprintf((char *)(data->path), (const char *)"C:\\VIDEO_ENCODE\\avi_rec%03d.avi", ++(data->total));            
			else if(USE_DISK == FS_NAND1)         
				sprintf((char *)(data->path), (const char *)"A:\\VIDEO_ENCODE\\avi_rec%03d.avi", ++(data->total)); 

			DBG_PRINT("create record avi file[%s]\n", data->path);
			  
			src.type = SOURCE_TYPE_FS;         
			src.Format.VideoFormat = MJPEG;	         
			src.type_ID.FileHandle = open((char *)(data->path), O_WRONLY|O_CREAT|O_TRUNC);         
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
		else if(ADKEY_IO3)         // Stop decoding the current file      
	#else   
		else if(IO3)      
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
		else if(ADKEY_IO4)      
	#else   
		else if(IO4)      
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
		else if(ADKEY_IO5)      
	#else   
		else if(IO5)      
	#endif   
		{
			if(data->mode != VIDEO_ENCODE_MODE){
				data->mode = VIDEO_ENCODE_MODE;
				video_select_mode(arg);
			}  
		  
			if(data->type != ENCODE_JPEG){         
				data->type = ENCODE_JPEG;            
				video_type_select(ENCODE_JPEG);            
				OSTimeDly(3);            
			}	           
			DBG_PRINT("%d", data->total);         

			if(USE_DISK == FS_SD)         
				sprintf((char *)path, (const char *)"C:\\VIDEO_ENCODE\\image%03d.jpg", (data->total)++);            
			else if(USE_DISK == FS_NAND1)         
				sprintf((char *)path, (const char *)"A:\\VIDEO_ENCODE\\image%03d.jpg", (data->total)++);            
			         
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
		else if(ADKEY_IO6)      
	#else   
		else if(IO6)      
	#endif   
		{ 
			if(data->total > 0){
				/*total not equal zero,*/
				if(data->mode != VIDEO_DECODE_MODE){
					data->mode = VIDEO_DECODE_MODE; 
					video_select_mode(arg);
				}
				video_decode_play(&arg, &src, VIDEO_PLAY_NEXT);
			}
		}      
	#if USE_ADKEY == 1	    
		else if(ADKEY_IO7)      
	#else   
		else if(IO7)      
	#endif   
		{ 
			if(data->total > 0){
				/*total not equal zero,*/
				if(data->mode != VIDEO_DECODE_MODE){
					data->mode = VIDEO_DECODE_MODE; 
					video_select_mode(arg);
				}
				video_decode_play(&arg, &src, VIDEO_PLAY_PREV);
			}
			         
		}      
	#if USE_ADKEY == 1	    
		else if(ADKEY_IO8)      
	#else   
		else if(IO8)      
	#endif   
		{      
			DBG_PRINT("video_encode_exit\r\n");         
			video_encode_exit();         
		}      
	}   
	   
	video_encode_preview_stop();   
	video_encode_uninit();   
	video_encode_exit();        // Release the resources needed by Video encoder   
}

static void video_get_file_total(INT32U file_header, INT32U *total)
{
	INT32S ret = 0;   
	INT32U index = 0;   
	struct f_info  file_info;   

	if(USE_DISK == FS_SD){   
		DBG_PRINT("Media : SDCard \r\n");      
		chdir("C:\\VIDEO_ENCODE");      
	}   
	else if(USE_DISK == FS_NAND1){   
		DBG_PRINT("Media : FS_NAND1 \r\n");      
		chdir("A:\\VIDEO_ENCODE");      
	}   
	else if(USE_DISK == FS_USBH){   
		DBG_PRINT("Media : FS_USBH\r\n");      
		chdir("G:\\VIDEO_ENCODE");      
	}   

	memset(&file_info, 0, sizeof(file_info));   

	ChangeCodePage(UNI_UNICODE);   

	ret = _findfirst((char *)VIDEO_TYPE_UNICDOE[file_header], &file_info, D_FILE);   
	if(ret < 0){   
		*total = 0;      
		ChangeCodePage(UNI_ENGLISH);      
		return;      
	}   

	do{   
		index ++;      
		sent_utf8_consol((char*)file_info.f_name);      
	}while(_findnext(&file_info) == 0);   
	   
	ChangeCodePage(UNI_ENGLISH);   
	*total = index;   
}

//=================================================================================================
//	video decode simple demo code 
//=================================================================================================
void Video_Decode_Simple_Demo(void *para)
{
	VIDEO_CODEC_STATUS status;
	VIDEO_INFO	information;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	

	// Mount device   
	while(1) {   
		if(_devicemount(USE_DISK)) {      
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);         
		#if	USE_DISK == FS_NAND1      
            nRet = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();            
			_deviceunmount(FS_NAND1);         
		#endif								                            
		} else {      
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);         	               
			break;         
		}      
	}  

	user_defined_video_codec_entrance();
	video_decode_entrance();
	
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
	
	arg.bScaler = 0x01;		//scaler output size or not
	arg.bUseDefBuf = FALSE;	//auto alloc buffer size  
	arg.AviDecodeBuf1 = NULL;
	arg.AviDecodeBuf2 = NULL;	
	arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;	//display width
	arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
	arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;	//display buffer height	
	arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format  
	
	src.Format.VideoFormat = MJPEG;	
	src.type = SOURCE_TYPE_FS;					//play file by file system

#if USE_DISK == FS_SD	
	chdir("C:\\");
#elif USE_DISK == FS_NAND1 
	chdir("A:\\");
#elif USE_DISK == FS_USBH
	chdir("G:\\");
#endif			
	src.type_ID.FileHandle = open("Demo0091NBA.avi", O_RDONLY);	//open file handle
	if(src.type_ID.FileHandle < 0) {
		DBG_PRINT("file open fail\r\n");
		while(1);
	}
			
	DBG_PRINT("video_decode_start\r\n");	
	status = video_decode_paser_header(&information, arg, src);
	if(status != VIDEO_CODEC_STATUS_OK) {
		DBG_PRINT("paser header fail !!!\r\n");
		while(1);
	}
	
	DBG_PRINT("Aud SampleRate = %d\r\n", information.AudSampleRate);
	DBG_PRINT("Vid FrameRate = %d\r\n", information.VidFrameRate);
	DBG_PRINT("resolution = %d x %d\r\n", information.Width, information.Height);
	DBG_PRINT("Total Time = %d seconds\r\n", information.TotalDuration);
			
	status = video_decode_start(arg, src);
	if(status != START_OK) {
		DBG_PRINT("video start fail!!!\r\n");
		while(1);
	}
	
	audio_decode_volume(0x3F);		
		
	while(1) {
		OSTimeDly(10);
		if(video_decode_status() == VIDEO_CODEC_PROCESS_END) {
			break;
		}
	}
	
	video_decode_stop();
	video_decode_exit();
	
	//unmount disk
	_deviceunmount(USE_DISK);
	disk_safe_exit(USE_DISK);	
}

//=================================================================================================
//	video encode simple demo code 
//=================================================================================================
void Video_Encode_Simple_Demo(void *para)
{
	INT32U i;
	INT64U disk_free;   
	VIDEO_ARGUMENT arg;   
	MEDIA_SOURCE   src;   
	
	// Mount device   
	while(1) {   
		if(_devicemount(USE_DISK)) { 
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);         
		#if	USE_DISK == FS_NAND1      
            nRet = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();            
			_deviceunmount(FS_NAND1);         
		#endif								                            
		} else {      
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);         
			DBG_PRINT("StartTime = %d\r\n", OSTimeGet());         
			disk_free = vfsFreeSpace(USE_DISK);         
			DBG_PRINT("Disk Free Size = %dMbyte\r\n", disk_free/1024/1024);         
			DBG_PRINT("EndTime = %d\r\n", OSTimeGet());			               
			break;         
		}      
	}  

	user_defined_video_codec_entrance();
	video_encode_entrance();
	
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

	for(i=0; i<10; i++) {
		OSTimeDly(100);
	}
	
	src.type = SOURCE_TYPE_FS;         
	src.Format.VideoFormat = MJPEG;	         
	
#if USE_DISK == FS_SD	
	chdir("C:\\");
#elif USE_DISK == FS_NAND1 
	chdir("A:\\");
#elif USE_DISK == FS_USBH
	chdir("G:\\");
#endif	
	src.type_ID.FileHandle = open("avi_rec.avi", O_WRONLY|O_CREAT|O_TRUNC);         
	if(src.type_ID.FileHandle < 0) {         
		DBG_PRINT("file open fail\r\n");            
		while(1);
	}         
			         	             
	DBG_PRINT("video_encode_start\r\n");         
	video_encode_start(src);         

	for(i=0; i<60; i++) {
		OSTimeDly(100);
	}
		
	video_encode_stop();
	video_encode_preview_stop();   
	video_encode_exit();
	
	//unmount disk
	_deviceunmount(USE_DISK);
	disk_safe_exit(USE_DISK);
}