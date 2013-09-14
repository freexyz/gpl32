#include <stdio.h>
#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"



void Image_Codec_Demo(void);
void Save_jpeg_file(IMAGE_ENCODE_ARGUMENT encode_info,INT32U jpeg_encode_output_prt,INT32U jpeg_encode_size,INT32U DEVICE);
void image_filelist(INT32U file_head);
INT8S image_type_select_task_create(void);
void image_gif_select(void);
void image_bmp_select(void);
void image_jpg_select(void);
void image_type_select_task_entry(void * pro);
void mount_disk(void);
static void sent_utf8_consol(char *filename);
static int  unicode_filename_strlen(char *filename);
extern INT16S oemuni_to_utf8(INT8S *Chart, INT8U *UTF8Chart);
extern int memset(void*, int , int);


extern scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern tv_display_flag, display_buf;
extern INT16U pic_num;

#define	     DISKUSED		 FS_SD //FS_NAND1		    

#define ENABLE                  1
#define DISABLE                 0
#define DECODE_SDRAM_EN    	 	ENABLE
#define SCALE_EN            	ENABLE

#define USE_ADKEY			1


#define TypeSelectCmdBufferSize	5
#define TypeSelectTaskStackSize	1024

#define DISPLAY_DEV_HPIXEL      C_DISPLAY_DEV_HPIXEL
#define DISPLAY_DEV_VPIXEL      C_DISPLAY_DEV_VPIXEL
#define SCALER_DEV_HPIXEL       640//2048
#define SCALER_DEV_VPIXEL       480//1536

INT32U	scale_x_output,scale_y_output,count,flag;
FP32 xxx_zoom, zoom_tmp;
INT32U File_Save_error;
INT32U decode_output_ptr,encode_output_ptr,scale_output_ptr; 
INT8U encode_mode_select = 0;

typedef struct image_info_s{
	char image_name[128];
	struct image_info_s *next,*prev;
} image_info_t; 

image_info_t *image_head = NULL;

#define LEN sizeof(image_info_t)
	

OS_EVENT *TypeSelectCmd_q;        //Message queen index
void *TypeSelectCmdBuffer[TypeSelectCmdBufferSize];	    //Point message array index
INT32U TypeSelectTaskStack[TypeSelectTaskStackSize];

typedef enum DECODE_TYPE{
	DECODE_JPG = 0x01,
	DECODE_GIF,
	DECODE_BMP
}DECODE_IMAGE_TYPE;

typedef enum ENCODE_MODE{
	ENCODE_ONCE_READ = 0X01,
	ENCODE_BLOCK_READ,
	ENCODE_BLOCK_READ_WRITE
}JPEG_ENCODE_MODE;

const INT32U *IMAGE_HEADER_SOURCE[] = {
	(INT32U *)"*.jpg",
	(INT32U *)"*.bmp",
	(INT32U *)"*.gif"
};

const INT16U UNICODE_IMAGE_HEADER[][6]={
	{0x002a,0x002e,0x006a,0x0070,0x0067,0x0000},  //*.jpg
	{0x002A,0x002E,0x0067,0x0069,0x0066,0x0000},  //*.gif
	{0x002A,0x002E,0x0062,0x006D,0x0070,0x0000}  //*.bmp
	
};

INT16U sd_codec_store[]={0x0063,0x003a,0x005c,0x005c,0x0000};

static INT8U jpeg_422_q50_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x63, 0x6F, 0x64, 0x65, 0x72, 0xFF, 
#if 1		// Quality 50
	0xDB, 0x00, 0x43, 0x00, 0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A,	0x10, 0x0E, 0x0D, 0x0E, 0x12, 0x11, 
	0x10, 0x13, 0x18, 0x28, 0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25, 0x1D, 0x28, 0x3A, 0x33, 
	0x3D, 0x3C, 0x39, 0x33, 0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44, 0x57, 0x45, 0x37, 0x38, 
	0x50, 0x6D, 0x51, 0x57, 0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71, 0x79, 0x70, 0x64, 0x78, 
	0x5C, 0x65, 0x67, 0x63, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x11,	0x12, 0x12, 0x18, 0x15, 0x18, 0x2F, 
	0x1A, 0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
#else		// Quality 100
	0xDB, 0x00, 0x43, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
#endif

	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

void Image_Codec_Demo(void)
{

	char SD_ID[24] = "c:\\bg2.jpg";// Fetch file on SD Card
	char NAND_ID[24] = "a:\\chen52.jpg";// Fetch file on NAND
	char UDB_ID[24] = "g:\\chen52.jpg";// Fetch file on USB
	
 
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT32U decode_state,encode_state,encode_size;
	IMAGE_ENCODE_ARGUMENT encode_info;
	IMAGE_SCALE_ARGUMENT  scaler_arg; 
	INT32U fifo_a,fifo_b,mcu_size;	
	INT8U image_type_select = 0;
	INT16S fd;
	INT16S path_flag;
	INT8U err;
	INT32S disk_free;
	char path[24];

	image_info_t *decode_image = NULL;
	
	mount_disk();
	
	if (DISKUSED == FS_SD){	
		path_flag = chdir("C:\\JPEG_ENCODE\\");
		if(path_flag < 0)
			mkdir("JPEG_ENCODE");
	}
	else if(DISKUSED == FS_NAND1){
		path_flag = chdir("A:\\JPEG_ENCODE\\");
		if(path_flag < 0)
			mkdir("JPEG_ENCODE");
	}
	else{
		path_flag = chdir("G:\\JPEG_ENCODE\\");
		if(path_flag < 0)
			mkdir("JPEG_ENCODE");	
	}

    
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

#if USE_ADKEY == 1
	adc_key_scan_init(); 	//init key scan
#endif	


	DBG_PRINT("\r\n*****************************************************************\r\n");
	DBG_PRINT("                 This is the image demo code                   **\r\n");
	DBG_PRINT("KEY_1   Select decode type:jpeg/bmp/gif                          **\r\n");
	DBG_PRINT("KEY_2   Select jpeg encode mode:encode_once/block_read/block_rw  **\r\n");
	DBG_PRINT("KEY_3   Image decode start or decode next image                  **\r\n");
	DBG_PRINT("KEY_5   Jpeg encode start                                        **\r\n");
	DBG_PRINT("KEY_6   Stop and exit !                                          **\r\n");
	DBG_PRINT("*****************************************************************\r\n");


	//image decoder to used image callback function
    BLOCK_ENCODE = 1;
    decode_mode_ck = 0;


	//malloc frame buffer
	decode_output_ptr = (INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2, 64);//malloc decode frame buffer
	if(decode_output_ptr == 0)
		DBG_PRINT("decode_output_ptr malloc fail\r\n");
	
	encode_output_ptr = (INT32U) gp_malloc_align((SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL), 32);//malloc decode frame buffer
	if(encode_output_ptr == 0)
		DBG_PRINT("encode_output_ptr malloc fail\r\n");
	
	scale_output_ptr = (INT32U) gp_malloc_align((SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL)*2, 32);//malloc decode frame buffer
	if(scale_output_ptr == 0)
		DBG_PRINT("scale_output_ptr malloc fail\r\n");

	gp_memset((INT8S *)decode_output_ptr, 0x00, (DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2);
	gp_memset((INT8S *)encode_output_ptr, 0x00,(SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL));
    gp_memset((INT8S *)scale_output_ptr, 0x00, (SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL)*2);

	//image source infomation
	image_source.type = SOURCE_TYPE_FS;                                    //image file infomation form file system

	image_type_select_task_create();

	while(1)
	{
		OSTimeDly(3);
	#if USE_ADKEY == 1	
		adc_key_scan();
	#else	
		key_Scan();
	#endif	
	#if USE_ADKEY == 1	
		if(ADKEY_IO1)                // select image decode type
	#else	
		if(IO1)
	#endif	
		{
			ChangeCodePage(UNI_UNICODE);
			image_type_select ++;
			if(image_type_select >= 4)
				image_type_select = 1;
			
			if(image_type_select == DECODE_JPG){
				DBG_PRINT("decode jpg type!\r\n");
				image_jpg_select();				
			}
			else if(image_type_select == DECODE_BMP){
				DBG_PRINT("decode bmp type!\r\n");
				image_bmp_select();			
			}
			else{
				DBG_PRINT("decode gif type!\r\n");
				image_gif_select();
			}

			if (image_head != NULL){
				decode_image = image_head;
				decode_image = decode_image->next;
			}
		
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO2)         // select image encode mode
	#else
		else if(IO2)
	#endif	
		{
			ChangeCodePage(UNI_ENGLISH);
		
			gp_memset((INT8S *)decode_output_ptr, 0x00, (DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2);
			gp_memset((INT8S *)encode_output_ptr, 0x00,(SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL));
 			gp_memset((INT8S *)scale_output_ptr, 0x00, (SCALER_DEV_HPIXEL*SCALER_DEV_VPIXEL)*2);
		
			if (DISKUSED == FS_SD){
				DBG_PRINT("Media : SDCard \r\n");
				image_source.type_ID.FileHandle=(INT32U)open(SD_ID, O_RDONLY);    //open jpg file			   
			}
			else if (DISKUSED == FS_NAND1){
				DBG_PRINT("Media : NAND \r\n");
				image_source.type_ID.FileHandle=(INT32U)open(NAND_ID, O_RDONLY);  //open jpg file
			}
			else{
				DBG_PRINT("Media : UDB_ID \r\n");
				image_source.type_ID.FileHandle=(INT32U)open(UDB_ID, O_RDONLY);	 //open jpg file
			}

			if(image_source.type_ID.FileHandle < 0)
			    DBG_PRINT("Open file fail !\r\n");

				
			image_decode.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; 				 //scaler out format
				
			//image decode output infomation
			image_decode.OutputBufPtr=(INT8U *)decode_output_ptr;			 //decode output buffer
			image_decode.OutputBufWidth=DISPLAY_DEV_HPIXEL; 				 //width of output buffer 
			image_decode.OutputBufHeight=DISPLAY_DEV_VPIXEL;				 //Heigh of output buffer
			image_decode.OutputWidth=DISPLAY_DEV_HPIXEL;					 //scaler width of output image
			image_decode.OutputHeight=DISPLAY_DEV_VPIXEL;					 //scaler Heigh of output image
			image_decode.ScalerOutputRatio=FIT_OUTPUT_SIZE; 				 //Fit to output_buffer_width and output_buffer_height for image output size
			image_decode.OutBoundaryColor=(INT32U)0x008080; 				 //set the black for out of boundary color 
				
			//image decode function
			image_decode_entrance();//global variable initial for image decode 
			image_decode_start(image_decode,image_source);//image decode start
				
			while (1) {
				decode_state = image_decode_status();
				if (decode_state == IMAGE_CODEC_DECODE_END) {
					DBG_PRINT("image decode ok\r\n");
					//callback used
					scaler_ptr = decode_output_ptr;
					break;
				}
				else if(decode_state == IMAGE_CODEC_DECODE_FAIL) {
					DBG_PRINT("image decode failed\r\n");
					break;
				} 
			}		

			image_decode_stop();//image decode stop
			
			scaler_arg.inputvisiblewidth=DISPLAY_DEV_HPIXEL;
			scaler_arg.inputvisibleheight=DISPLAY_DEV_VPIXEL;
			scaler_arg.InputWidth=DISPLAY_DEV_HPIXEL;
			scaler_arg.InputHeight=DISPLAY_DEV_VPIXEL;
			
			scaler_arg.inputoffsetx=0;
			scaler_arg.inputoffsety=0;

			scaler_arg.InputBufPtr=(INT8U *)decode_output_ptr;
			scaler_arg.OutBoundaryColor=(INT32U)0x008080;

	
			encode_mode_select ++;
			if(encode_mode_select >= 4)
				encode_mode_select = 1;
			
			if(encode_mode_select == ENCODE_ONCE_READ){
				DBG_PRINT("ENCODE_ONCE_READ MODE \r\n");
			
				//image scaler output infomation

				scaler_arg.outputwidthfactor=0;
				scaler_arg.outputheightfactor=0;

				scaler_arg.OutputBufWidth=SCALER_DEV_HPIXEL;
				scaler_arg.OutputBufHeight=SCALER_DEV_VPIXEL;
				scaler_arg.OutputWidth=SCALER_DEV_HPIXEL;
				scaler_arg.OutputHeight=SCALER_DEV_VPIXEL;

				scaler_arg.OutputBufPtr=(INT8U *)scale_output_ptr;

				scaler_arg.InputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
				scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
				
				image_scale_start(scaler_arg);
				
				decode_state = image_scale_status();

				encode_info.OutputBufPtr=(INT8U *)encode_output_ptr;			  //encode output buffer
				encode_info.BlockLength=0; 									  //encode block read length
				encode_info.EncodeMode=IMAGE_ENCODE_ONCE_READ; 				  //encode mode setting
				encode_info.InputWidth=SCALER_DEV_HPIXEL;						  //width of input image
				encode_info.InputHeight=SCALER_DEV_VPIXEL; 					  //Heigh of input image
				encode_info.quantizationquality=50;							  //encode quality
			 	encode_info.InputBufPtr.yaddr=(INT8U *)scale_output_ptr;		  //encode input buffer
				encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUYV;				 //encode input format
				encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;			 //encode input format
						
			}
			else if(encode_mode_select == ENCODE_BLOCK_READ){
				DBG_PRINT("ENCODE_BLOCK_READ MODE \r\n");

			    //image scaler output infomation
			    scaler_arg.outputwidthfactor=(DISPLAY_DEV_HPIXEL<<16)/SCALER_DEV_HPIXEL;   //scale x factor
			    scaler_arg.outputheightfactor=(7<<16)/15;                                 //scale y factor, 240/32 = 7, 480/32 = 15
			    scaler_arg.OutputWidth=SCALER_DEV_HPIXEL;
			    scaler_arg.OutputHeight=48;                                                //scale output y size length 	   
			    scaler_arg.InputFormat=SCALER_INPUT_FORMAT_YUYV;
		        scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUV422;

			   //image encode infomation
			    encode_info.OutputBufPtr=(INT8U *)encode_output_ptr;              //encode output buffer
			    encode_info.BlockLength=48;                                       //encode block read length
		        encode_info.EncodeMode=IMAGE_ENCODE_BLOCK_READ;                   //encode mode setting
		        encode_info.InputWidth=SCALER_DEV_HPIXEL;                         //width of input image
		        encode_info.InputHeight=SCALER_DEV_VPIXEL;                        //Heigh of input image
		        encode_info.quantizationquality=50;                               //encode quality
		        encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUYV_SCALE;   	//encode input format
		        encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;       //encode input format
		        encode_info.scalerinfoptr=&scaler_arg;
	 								
			}
			else
			{
				DBG_PRINT("ENCODE_BLOCK_READ_WRITE MODE \r\n");

			    //image scaler output infomation
				scaler_arg.outputwidthfactor=(DISPLAY_DEV_HPIXEL<<16)/SCALER_DEV_HPIXEL;   //scale x factor
				scaler_arg.outputheightfactor=(7<<16)/15;                                 //scale y factor, 240/32 = 7, 480/32 = 15
				scaler_arg.OutputWidth=SCALER_DEV_HPIXEL;
				scaler_arg.OutputHeight=32;                                                //scale output y size length 
				scaler_arg.InputFormat=SCALER_INPUT_FORMAT_YUYV;
			    scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUV422;

				//image encode infomation
				encode_info.BlockLength=32;                                       //encode block read length
			    encode_info.EncodeMode=IMAGE_ENCODE_BLOCK_READ_WRITE;             //encode mode setting
			    encode_info.InputWidth=SCALER_DEV_HPIXEL;                         //width of input image
			    encode_info.InputHeight=SCALER_DEV_VPIXEL;                        //Heigh of input image
			    encode_info.quantizationquality=50;                               //encode quality 
			    encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUYV_SCALE;     //encode input format
			    encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;       //encode input format
			    encode_info.scalerinfoptr=&scaler_arg;
			    encode_output_ptr = (INT32U) gp_malloc_align(SCALER_DEV_HPIXEL*encode_info.BlockLength, 32);//malloc decode frame buffer 
			    encode_info.OutputBufPtr=(INT8U *)encode_output_ptr;              //encode output buffer 

			}
					   
		}

	#if USE_ADKEY == 1	
		else if(ADKEY_IO3)     //start image decode
	#else
		else if(IO3)
	#endif
		{	
			DBG_PRINT("\r\nDecode ");

			sent_utf8_consol(decode_image->image_name);	
			image_source.type_ID.FileHandle=(INT32U)open(decode_image->image_name,O_RDONLY);   //open jpg file		
			if(image_source.type_ID.FileHandle < 0)
				DBG_PRINT("Open file fail !\r\n");

			decode_image = decode_image->next;
			if(decode_image == NULL){
				decode_image = image_head;
				decode_image = decode_image->next;
			}
				
			image_decode.ScalerOutputRatio=FIT_OUTPUT_SIZE;   
			image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;                  //scaler out format
			//image output infomation
			image_decode.OutputBufPtr=(INT8U *)decode_output_ptr;            //decode output buffer
			image_decode.OutputBufWidth=C_DISPLAY_DEV_HPIXEL;                  //width of output buffer 
			image_decode.OutputBufHeight=C_DISPLAY_DEV_VPIXEL;                 //Heigh of output buffer
			image_decode.OutputWidth=C_DISPLAY_DEV_HPIXEL;                     //scaler width of output image
			image_decode.OutputHeight=C_DISPLAY_DEV_VPIXEL;                    //scaler Heigh of output image
			
			//image decode function
			image_decode_entrance();     //global variable initial for image decode 
			image_decode_start(image_decode,image_source);    //image decode start
			while (1) {
				 decode_state = image_decode_status();
		         if (decode_state == IMAGE_CODEC_DECODE_END) {
					 video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)decode_output_ptr, C_DISPLAY_DEVICE, IMAGE_OUTPUT_FORMAT_YUYV);
					 close(image_source.type_ID.FileHandle);
					 break;
				  }else if(decode_state == IMAGE_CODEC_DECODE_FAIL) {
				  	 DBG_PRINT("image decode failed\r\n");
					 break;
				  }	
			}	

			image_decode_stop();
		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO5)    //start image encode
	#else
		else if(IO4)
	#endif
		{
			if(encode_mode_select == ENCODE_ONCE_READ || encode_mode_select == ENCODE_BLOCK_READ){
				//image encode function
				image_encode_entrance();     //global variable initial for image encoder
				encode_size=image_encode_start(encode_info);   //image encode start
				
				while (1) {
					 encode_state=image_encode_status();
					 if (encode_state==IMAGE_CODEC_DECODE_END) {
						 Save_jpeg_file(encode_info,encode_output_ptr,encode_size,DISKUSED);
						 DBG_PRINT("image encode success\r\n");
						 break;
					 }else if(encode_state==IMAGE_CODEC_DECODE_FAIL) {
						 DBG_PRINT("image decode failed\r\n");
						 break;
					 }	
				}  
			}		
			else{
				
				if (DISKUSED == FS_SD)
					sprintf((char *)path, (const char *)"C:\\JPEG_ENCODE\\blockrw%03d.jpeg", pic_num);
				else if(DISKUSED == FS_NAND1)
					sprintf((char *)path, (const char *)"a:\\JPEG_ENCODE\\blockrw%03d.jpeg", pic_num);
			 	else
					sprintf((char *)path, (const char *)"g:\\JPEG_ENCODE\\blockrw%03d.jpeg", pic_num);

				 // Save encoded data to file
			 	fd = open((CHAR *)path, O_CREAT|O_RDWR);
			 	if (fd >= 0) {
				 jpeg_422_q50_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
				 jpeg_422_q50_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
				 jpeg_422_q50_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
				 jpeg_422_q50_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);

				 if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
					  jpeg_422_q50_header_block_rw[0xA4] =	0x21;
					  fifo_a=SCALER_DEV_HPIXEL/16;
					  fifo_b=(encode_info.BlockLength/8);
					  mcu_size=fifo_a*fifo_b;
				 }else{
					  jpeg_422_q50_header_block_rw[0xA4] =	0x22;
					  fifo_a=SCALER_DEV_HPIXEL/16;
					  fifo_b=(encode_info.BlockLength/16);
					  mcu_size=fifo_a*fifo_b;
				 }
				 
				 jpeg_422_q50_header_block_rw[0x260] = (mcu_size >> 8);
				 jpeg_422_q50_header_block_rw[0x261] = (mcu_size & 0xFF);
				 disk_free=vfsFreeSpace(DISKUSED);

				 if(disk_free > sizeof(jpeg_422_q50_header_block_rw)){
				 	write(fd, (INT32U) &jpeg_422_q50_header_block_rw[0], sizeof(jpeg_422_q50_header_block_rw));
					File_Save_error=0;
				 }else{
				 	DBG_PRINT("Save file failed\r\n");
					File_Save_error=1;
				 }					
				}
				if(File_Save_error==0){
			   		encode_info.FileHandle=fd;
			   		encode_info.UseDisk=DISKUSED;
			  		//image encode function
			 		image_encode_entrance();//global variable initial for image encoder
			   		encode_size=image_encode_start(encode_info);
			   		while (1) {
				  		encode_state=image_encode_status();
				  		if (encode_state==IMAGE_CODEC_DECODE_END) {
	  				#if DISKUSED == FS_NAND1
							DrvNand_flush_allblk();		
	  				#endif
					  		DBG_PRINT("image encode ok\r\n");
					 		 break;
				  		}else if(encode_state==IMAGE_CODEC_DECODE_FAIL) {
					  		DBG_PRINT("image encode failed\r\n");
					 		 break;
				  		}  
			   		}
			   		DBG_PRINT("image encode success\r\n");
				}
				else{
			   		DBG_PRINT("image encode failed\r\n");
				}			
			}

		}
	#if USE_ADKEY == 1	
		else if(ADKEY_IO8)    //stop and exit
	#else
		else if(IO5)
	#endif
		{
			
			image_decode_stop();

			image_encode_stop();//image encode stop 
			image_encode_exit();

			err = OSTaskDel(31);
			if(err == OS_NO_ERR) {	
				DBG_PRINT("OSTaskDel Success\r\n");
			} else {
				DBG_PRINT("OSTaskDel Fail\r\n");
			}			
			DBG_PRINT("image demo exit!\r\n");
		}
	}
}
void image_type_select_task_entry(void * pro)
{
	INT8U err;
	INT32U msg_id;

	DBG_PRINT("\r\nimage_type_select_task_entry success!\r\n");
	
	while(1)
	{
		
		msg_id = (INT32U)OSQPend(TypeSelectCmd_q,0,&err);
		if(!msg_id || err != OS_NO_ERR)
			continue;
		switch(msg_id){
			case DECODE_JPG:
				image_filelist(DECODE_JPG -1);
				break;
			case DECODE_BMP:
				image_filelist(DECODE_BMP -1);
				break;
			case DECODE_GIF:
				image_filelist(DECODE_GIF -1);
				break;
			default :

				break;
	
		}
		OSTimeDly(1);	
	}
}

INT8S image_type_select_task_create(void)
{
	INT32S ret;

	TypeSelectCmd_q = OSQCreate(TypeSelectCmdBuffer,TypeSelectCmdBufferSize);

	if(!TypeSelectCmd_q){
		DBG_PRINT("TypeSelectCmd_q create fail !\r\n");
		return STATUS_FAIL;
	}

	ret = OSTaskCreate(image_type_select_task_entry,(void *)0,&TypeSelectTaskStack[TypeSelectTaskStackSize -1],31);

	if(ret < 0 ){
		DBG_PRINT("image_type_select_task_entry create fail !\r\n");
		return STATUS_FAIL;
	}

	return STATUS_OK;
}

void image_jpg_select(void)
{
	OSQPost(TypeSelectCmd_q,(void *)DECODE_JPG);
	OSTimeDly(2);
}

void image_bmp_select(void)
{
	OSQPost(TypeSelectCmd_q,(void *)DECODE_BMP);
	OSTimeDly(2);
}

void image_gif_select(void)
{
	OSQPost(TypeSelectCmd_q,(void *)DECODE_GIF);
	OSTimeDly(2);
}

void image_list_create(char* image_name)
{
	image_info_t *temp;

    image_head = (image_info_t *)gp_malloc(LEN + unicode_filename_strlen((char *)image_name)+2);
    image_head->next = image_head;
    image_head->prev = image_head;
	gp_memcpy((INT8S*)&image_head->image_name, (INT8S*)"head", sizeof(image_head->image_name));

    temp = image_head;

    temp->next = (image_info_t *)gp_malloc(LEN + unicode_filename_strlen((char *)image_name)+2);
    temp = temp->next;
    temp->next = image_head;
    temp->prev = image_head;
    image_head->prev = temp;
	gp_memcpy((INT8S*)&temp->image_name, (INT8S*)image_name, LEN + unicode_filename_strlen((char *)image_name)+2);
	sent_utf8_consol(image_name);
	
}

void image_list_insert(char* image_name)
{
	image_info_t *p;
	image_info_t *temp;
	
	p = image_head->prev;

    temp = (image_info_t*)gp_malloc(LEN + unicode_filename_strlen((char *)image_name)+2);
    gp_memcpy((INT8S*)&temp->image_name, (INT8S*)image_name, LEN + unicode_filename_strlen((char *)image_name)+2);

	sent_utf8_consol(image_name);

    temp->next = p->next;
    p->next = temp;
    (temp->next)->prev = temp;
    temp->prev = p;

}

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

static void sent_utf8_consol(char *filename)
{        
	char *utf8;       
	int utf8_len,utf8_s;        
	int ret=0; 
	
	utf8_s = unicode_filename_strlen(filename);	
	utf8_len = unicode_filename_strlen(filename)*3/2;        
	utf8 = gp_malloc(utf8_len);   
	ret = oemuni_to_utf8((INT8S *)filename,(INT8U *)utf8);     
	if(ret == 0)        
		DBG_PRINT("filename:%s\r\n",utf8);       
	else        
		DBG_PRINT("change to utf8 err\r\n"); 
	gp_free(utf8);
}

void image_filelist(INT32U file_head)
{
	struct f_info finfo;
	int ret = 0;
	
	if(DISKUSED == FS_SD){
			DBG_PRINT("Media : SDCard \r\n");
			//chdir("C:\\");
			chdir((char *)sd_codec_store);
		}
		else if(DISKUSED == FS_NAND1){
			DBG_PRINT("Media : FS_NAND1 \r\n");
			chdir("A:\\");
		}
		else if(DISKUSED == FS_USBH){
			DBG_PRINT("Media : FS_USBH\r\n");
			chdir("G:\\");
		}
	
	memset(&finfo,0,sizeof(finfo));

	if (_findfirst ((char *)UNICODE_IMAGE_HEADER[file_head], &finfo, D_FILE))	{
		DBG_PRINT("Error:no image files !!!\r\n");
	}
	else{
		do	{
			if (ret == 0)
				image_list_create((char*)&finfo.f_name);
			else
				image_list_insert((char*)&finfo.f_name);
					
			ret ++;
		}
		while (_findnext (&finfo) == 0);

		DBG_PRINT("Total image member : %d \r\n",ret);
	}
}


void mount_disk(void)
{
	
	INT32S disk_free;
	while(1)
	{
		if(_devicemount(DISKUSED))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", DISKUSED);						
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", DISKUSED);
			DBG_PRINT("StartTime = %d\r\n", OSTimeGet());
			disk_free = vfsFreeSpace(DISKUSED);
			DBG_PRINT("Disk Free Size = %dMbyte\r\n", disk_free/1024/1024);
			DBG_PRINT("EndTime = %d\r\n", OSTimeGet());			
			break;
		}
	}
}

