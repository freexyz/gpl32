#include <stdio.h>
#include "application.h"

void Image_Codec_Demo(void);
void Save_jpeg_file(IMAGE_ENCODE_ARGUMENT encode_info,INT32U jpeg_encode_output_prt,INT32U jpeg_encode_size,INT32U DEVICE);


INT32U	scale_x_output,scale_y_output,count,flag;
FP32 xxx_zoom, zoom_tmp;
extern scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern tv_display_flag, display_buf;

#define	DISKUSED		    FS_SD
//#define	DISKUSED		    FS_NAND1

#define ENABLE                  1
#define DISABLE                 0
#define DECODE_SDRAM_EN     ENABLE
#define SCALE_EN            ENABLE

#define DISPLAY_DEV_HPIXEL      C_DISPLAY_DEV_HPIXEL
#define DISPLAY_DEV_VPIXEL      C_DISPLAY_DEV_VPIXEL

#define DECODE_JPG              0
#define DECODE_GIF              1
#define DECODE_BMP              2
#define	DECODETYPE	        DECODE_JPG

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
	char SD_ID_JPG[24] = "C:\\chen52.jpg";// Fetch file on SD Card
	char SD_ID_GIF[24] = "C:\\chen52.gif";// Fetch file on SD Card
	char SD_ID_BMP[24] = "C:\\chen52.bmp";// Fetch file on SD Card
	char NAND_ID[24] = "A:\\chen52.jpg";// Fetch file on NAND
	char UDB_ID[24] = "G:\\chen52.jpg";// Fetch file on USB

	INT32U decode_output_ptr,encode_output_ptr,sdram_input_ptr,sdram_output_ptr,scale_output_ptr;  
	IMAGE_INFO image_file_info;
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT32U decode_state,encode_state,encode_size,i;
	IMAGE_ENCODE_ARGUMENT encode_info;
	IMAGE_SCALE_ARGUMENT  scaler_arg;
	INT8U *t_addr,*s_addr; 
	
	while(1)
    {
	    if( _devicemount(DISKUSED))          // Mount device
	    {
		    DBG_PRINT("Mount DISK Fail....\r\n");
	    }else{
		    DBG_PRINT("Mount DISK success....\r\n");
	    	break;
	    }
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

	//malloc frame buffer
	decode_output_ptr = (INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2, 64);//malloc decode frame buffer
	if(decode_output_ptr == 0)	
		while(1);
	
	encode_output_ptr = (INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL), 32);//malloc decode frame buffer
    if(encode_output_ptr == 0)
    	while(1);
    
    gp_memset((INT8S *)decode_output_ptr, 0x00, (DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2);
	gp_memset((INT8S *)encode_output_ptr, 0x00, DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL);
    scaler_ptr=decode_output_ptr;
    
    //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
    //image header infomation
    if(DECODETYPE==DECODE_JPG)
       image_source.type_ID.FileHandle=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
    else if(DECODETYPE==DECODE_GIF)
       image_source.type_ID.FileHandle=(INT32U)open(SD_ID_GIF, O_RDONLY);       //open gif file
    else
       image_source.type_ID.FileHandle=(INT32U)open(SD_ID_BMP, O_RDONLY);       //open bmp file      
    
    if(image_source.type_ID.FileHandle < 0)
    	while(1);
    	
    image_decode_Info(&image_file_info,image_source);
    close(image_source.type_ID.FileHandle);
	
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system
	
	if (DISKUSED==FS_SD)
	{
       if(DECODETYPE==DECODE_JPG)
          image_source.type_ID.FileHandle=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
       else if(DECODETYPE==DECODE_GIF)
          image_source.type_ID.FileHandle=(INT32U)open(SD_ID_GIF, O_RDONLY);       //open gif file
       else
          image_source.type_ID.FileHandle=(INT32U)open(SD_ID_BMP, O_RDONLY);       //open bmp file  
	}
	else if (DISKUSED==FS_NAND1)
	   image_source.type_ID.FileHandle=(INT32U)open(NAND_ID, O_RDONLY);  //open jpg file
	else 
	   image_source.type_ID.FileHandle=(INT32U)open(UDB_ID, O_RDONLY);   //open jpg file
   
   	if(image_source.type_ID.FileHandle < 0)
    	while(1);

	image_decode.ScalerOutputRatio=FIT_OUTPUT_SIZE;   
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;                  //scaler out format
	//image output infomation
	image_decode.OutputBufPtr=(INT8U *)decode_output_ptr;            //decode output buffer
	image_decode.OutputBufWidth=DISPLAY_DEV_HPIXEL;                  //width of output buffer 
	image_decode.OutputBufHeight=DISPLAY_DEV_VPIXEL;                 //Heigh of output buffer
	image_decode.OutputWidth=DISPLAY_DEV_HPIXEL;                     //scaler width of output image
	image_decode.OutputHeight=DISPLAY_DEV_VPIXEL;                    //scaler Heigh of output image
	
	//image decode function
	image_decode_entrance();//global variable initial for image decode 
	image_decode_start(image_decode,image_source);//image decode start
	
	while (1) {
		  decode_state=image_decode_status();
          if (decode_state==IMAGE_CODEC_DECODE_END) {
			 video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)decode_output_ptr, C_DISPLAY_DEVICE, IMAGE_OUTPUT_FORMAT_YUYV);
			 close(image_source.type_ID.FileHandle);
			 break;
		  }else if(decode_state==IMAGE_CODEC_DECODE_FAIL) {
			 DBG_PRINT("image decode failed\r\n");
			 break;
		  }	
	}		
	image_decode_stop();//image decode stop

	//image encode infomation
	encode_info.OutputBufPtr=(INT8U *)encode_output_ptr;           //encode output buffer
	encode_info.BlockLength=16;                                    //encode block read length
	encode_info.EncodeMode=IMAGE_ENCODE_BLOCK_READ;                //encode mode setting
	encode_info.InputWidth=DISPLAY_DEV_HPIXEL;                     //width of input image
	encode_info.InputHeight=DISPLAY_DEV_VPIXEL;                    //Heigh of input image
	encode_info.quantizationquality=50;                            //encode quality
	encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUYV;       //encode input format
	encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;           //encode input format
	encode_info.InputBufPtr.yaddr=(INT8U *)decode_output_ptr;       //encode input buffer
	
	//image encode function
	image_encode_entrance();//global variable initial for image encoder
	encode_size=image_encode_start(encode_info);//image encode start
	
	while (1) {
		  encode_state=image_encode_status();
          if (encode_state==IMAGE_CODEC_DECODE_END) {
			 Save_jpeg_file(encode_info,encode_output_ptr,encode_size,DISKUSED);
			 break;
		  }else if(encode_state==IMAGE_CODEC_DECODE_FAIL) {
			 DBG_PRINT("image decode failed\r\n");
			 break;
		  }	
	}
	image_encode_stop();//image encode stop
	
    if(DECODE_SDRAM_EN==ENABLE){
	   //image source infomation
	   image_source.type=SOURCE_TYPE_SDRAM;                             //image file infomation form sdram
	   image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;              //scaler out format
	   
	   //malloc frame buffer
	   sdram_output_ptr=(INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2, 64);//malloc decode frame buffer
	   sdram_input_ptr=(INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2, 32);//malloc sdram frame buffer
	   //jpeg header+encode data	  
	   jpeg_422_q50_header_block_rw[0x9E] = encode_info.InputHeight >> 8;
	   jpeg_422_q50_header_block_rw[0x9F] = encode_info.InputHeight & 0xFF;
	   jpeg_422_q50_header_block_rw[0xA0] = encode_info.InputWidth >> 8;
	   jpeg_422_q50_header_block_rw[0xA1] = encode_info.InputWidth & 0xFF;	  
		  s_addr=(INT8U *)&jpeg_422_q50_header_block_rw[0];
	      t_addr=(INT8U *)sdram_input_ptr;
	      for(i=0;i<sizeof(jpeg_422_q50_header_block_rw);i++)
	      {    
	            *t_addr=*s_addr;
	            s_addr++;
                t_addr++;
	      }
	      
	      s_addr=(INT8U *)encode_output_ptr;
	      for(i=0;i<encode_size;i++)
	      {    
	            *t_addr=*s_addr;
	            s_addr++;
                t_addr++;
	      }
	   
	   //image decode function   
	   image_source.type_ID.memptr=(INT8U *)sdram_input_ptr;            //open jpg file
	   image_decode.OutputBufPtr=(INT8U *)sdram_output_ptr;             //decode output buffer
	   image_decode.OutputBufWidth=DISPLAY_DEV_HPIXEL;                  //width of output buffer 
	   image_decode.OutputBufHeight=DISPLAY_DEV_VPIXEL;                 //Heigh of output buffer
	   image_decode.OutputWidth=DISPLAY_DEV_HPIXEL;                     //scaler width of output image
	   image_decode.OutputHeight=DISPLAY_DEV_VPIXEL;                    //scaler Heigh of output image
	   
	   //image decode function
       image_decode_entrance();//global variable initial for image decode 
	   image_decode_start(image_decode,image_source);//image decode start
	
	   while (1) {
		  decode_state=image_decode_status();
          if (decode_state==IMAGE_CODEC_DECODE_END) {
			   video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)sdram_output_ptr, C_DISPLAY_DEVICE,IMAGE_OUTPUT_FORMAT_YUYV);
			 break;
		  }else if(decode_state==IMAGE_CODEC_DECODE_FAIL) {
			 DBG_PRINT("image decode resource form SDRAM buffer failed\r\n");
			 break;
		  }	
	   }		
	   image_decode_stop();//image decode stop
	}
	
	if(SCALE_EN==ENABLE){
	   scale_output_ptr=(INT32U) gp_malloc_align((DISPLAY_DEV_HPIXEL*DISPLAY_DEV_VPIXEL)*2, 64);//malloc decode frame buffer
	   scale_x_output=(DISPLAY_DEV_HPIXEL>>1);
	   scale_y_output=(DISPLAY_DEV_VPIXEL>>1);
	   tv_display_flag=0;
	   user_defined_image_decode_entrance(C_DISPLAY_DEVICE);
	}
	
	
	while(1){
		if(SCALE_EN==ENABLE){
		   if (count % 2 == 0){
	          if(flag==0){
	            scale_x_output+=2;
	            scale_y_output+=2;
	            if (scale_x_output>(DISPLAY_DEV_HPIXEL+DISPLAY_DEV_HPIXEL>>1) || scale_y_output>(DISPLAY_DEV_VPIXEL+DISPLAY_DEV_VPIXEL>>1)){
			        scale_x_output=DISPLAY_DEV_HPIXEL+DISPLAY_DEV_HPIXEL>>1;
			        scale_y_output=DISPLAY_DEV_VPIXEL+DISPLAY_DEV_VPIXEL>>1;
			        flag=1;
	            }
	          }else{
	            scale_x_output-=2;
	            scale_y_output-=2;
	            if(scale_x_output<(DISPLAY_DEV_HPIXEL>>1) || scale_y_output<(DISPLAY_DEV_VPIXEL>>1) ){
			        scale_x_output=DISPLAY_DEV_HPIXEL>>1;
			        scale_y_output=DISPLAY_DEV_VPIXEL>>1;
			        flag=0;
	            }
	          }      
	       }
	       scaler_arg.inputvisiblewidth=DISPLAY_DEV_HPIXEL;
	       scaler_arg.inputvisibleheight=DISPLAY_DEV_VPIXEL;
	       scaler_arg.InputWidth=DISPLAY_DEV_HPIXEL;
	       scaler_arg.InputHeight=DISPLAY_DEV_VPIXEL;
	       scaler_arg.outputwidthfactor=0;
	       scaler_arg.outputheightfactor=0;
	       scaler_arg.inputoffsetx=0;
	       scaler_arg.inputoffsety=0;
	       scaler_arg.OutputBufWidth=DISPLAY_DEV_HPIXEL;
	       scaler_arg.OutputBufHeight=DISPLAY_DEV_VPIXEL;
	       scaler_arg.OutputWidth=scale_x_output;
	       scaler_arg.OutputHeight=scale_y_output;
	       scaler_arg.InputBufPtr=(INT8U *)decode_output_ptr;
	       scaler_arg.OutputBufPtr=(INT8U *)scale_output_ptr;
	       scaler_arg.InputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
	       scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
	       image_scale_start(scaler_arg);
	      while (1){
		     decode_state=image_scale_status();
             if (decode_state==IMAGE_CODEC_DECODE_END) {
               tv_display_flag=1;
               display_buf=scale_output_ptr;
               while(tv_display_flag!=1)
    	          OSTimeDly(1);
    	       video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)display_buf, C_DISPLAY_DEVICE,IMAGE_OUTPUT_FORMAT_YUYV);   
			   break;
		     }else if(decode_state==IMAGE_CODEC_DECODE_FAIL) {
			   DBG_PRINT("image decode resource form SDRAM buffer failed\r\n");
			   break;
		     }	
	      }
	     count++;
	    }	    
		OSTimeDly(5);
	}
}




