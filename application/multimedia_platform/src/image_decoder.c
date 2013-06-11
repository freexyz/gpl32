#include "image_Decoder.h"

//Task Priority
typedef enum
{
    TSK_PRI_AUDIO_DAC=0,
    TSK_PRI_AUDIO,
    TSK_PRI_BG_AUDIO,
    TSK_PRI_FILE_SRV,
    TSK_PRI_IMAGE,
    TSK_PRI_PPU,
    TSK_PRI_SYSTEM,
    TSK_PRI_UMI,
    TSK_PRI_FILE_SCAN,
    TSK_PRI_IDLE,
    TSK_PRI_MAX
} TASK_PRI_ENUM;

#define SCALER_SIZE_2          2
#define SCALER_SIZE_4          4
#define SCALER_SIZE_8          8
#define SCALER_8_MULTIPLE     0xF
#define SCALER_2_MULTIPLE     0x1

extern MSG_Q_ID image_msg_queue_id;
extern OS_EVENT *image_task_fs_queue_a;
extern OS_EVENT *image_task_fs_queue_b;
INT32U image_decode_task_para[17];
extern TK_IMAGE_TYPE_ENUM image_file_judge_type(INT16S fd);

IMAGE_DECODE_STRUCT image_decode_struct;
IMAGE_SCALE_STRUCT  image_scale_struct;
IMAGE_HEADER_PARSE_STRUCT header_parse_struct;
INT32U image_decode_ck,image_scaler_ck,image_block_encode_scaler_ck,image_decode_info_ck;
INT32U malloc_buffer_error,scaler_format_error,encode_out_buffer_base,encode_size_sum;

void image_decode_entrance(void);
void image_decode_exit(void);
CODEC_START_STATUS image_decode_Info(IMAGE_INFO *info,MEDIA_SOURCE src);
CODEC_START_STATUS image_decode_start(IMAGE_ARGUMENT arg,MEDIA_SOURCE src);
CODEC_START_STATUS image_block_encode_scale(IMAGE_SCALE_ARGUMENT *scaler_arg,IMAGE_ENCODE_PTR *output_ptr);
void image_decode_stop(void);
IMAGE_CODEC_STATUS image_decode_status(void);
CODEC_START_STATUS image_scale_start(IMAGE_SCALE_ARGUMENT arg);
IMAGE_CODEC_STATUS image_scale_status(void);
//=============================================================================
//============================== Image Decoder ================================
//
// including JPEG, JPEG Progressive,Single Picture from Motion JPEG, BMP, GIF .....
//
//=============================================================================
void image_decode_entrance(void)
{        
    INT8S *pimage;
    
    image_decode_ck=0;
    image_scaler_ck=0;
    image_block_encode_scaler_ck=0;
    image_decode_info_ck=0;
    pimage=(INT8S *)&image_decode_struct;
    gp_memset(pimage,0,sizeof(IMAGE_DECODE_STRUCT));
     
}


void image_decode_exit(void)
{
/*	
	INT8U err;
	
	OSTaskDel(TSK_PRI_IMAGE);
	msgQDelete(image_msg_queue_id);
	OSQDel(image_task_fs_queue_a,OS_DEL_ALWAYS, &err);
	OSQDel(image_task_fs_queue_b,OS_DEL_ALWAYS, &err);
*/	
}


CODEC_START_STATUS image_decode_Info(IMAGE_INFO *info,MEDIA_SOURCE src)
{
    INT32U msg_id,image_decode_Info_state;
    IMAGE_INFO *image_info;
    MEDIA_SOURCE SOURCE;
    struct sfn_info file_info;
    IMAGE_HEADER_PARSE_STRUCT *image_header_parse;
    IMAGE_HEADER_PARSE_STRUCT image_header_Info;
    
    image_info=info;
    SOURCE=src;
    image_header_Info=header_parse_struct;
    
    if(image_decode_info_ck==0){
       image_decode_info_ck=1;
       switch (image_file_judge_type((INT16S)SOURCE.type_ID.FileHandle))
       {
          case TK_IMAGE_TYPE_JPEG:
               image_info->Format=JPEG;
               break; 
          
          case TK_IMAGE_TYPE_MOTION_JPEG: 
               image_info->Format=MJPEG_S;
               break;
          
          case TK_IMAGE_TYPE_BMP: 
               image_info->Format=BMP;
               break;
          
          case TK_IMAGE_TYPE_GIF: 
               image_info->Format=GIF;
               break;
       }
   
       image_decode_Info_state=sfn_stat((INT16S)SOURCE.type_ID.FileHandle, &file_info);
       if(image_decode_Info_state==0){
          image_info->FileSize=file_info.f_size;
          lseek(SOURCE.type_ID.FileHandle,0,0);
          image_header_Info.image_source=(INT32S)SOURCE.type_ID.FileHandle;
          image_header_Info.source_type=SOURCE.type;
          image_header_Info.cmd_id =1;
          image_header_Info.source_size=image_info->FileSize;
          msgQSend(image_msg_queue_id, MSG_IMAGE_PARSE_HEADER_REQ, (void *)&image_header_Info,  sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
          msgQReceive(ApQ, &msg_id, (void *)image_decode_task_para, sizeof(IMAGE_HEADER_PARSE_STRUCT));
          image_header_parse=(IMAGE_HEADER_PARSE_STRUCT *)image_decode_task_para;
          if(image_header_parse->parse_status==0){
             image_info->Width=(INT32U)image_header_parse->width;
             image_info->Height=(INT32U)image_header_parse->height;
             lseek(SOURCE.type_ID.FileHandle,0,0);
             image_decode_info_ck=0;
             return START_OK;
          }else{
             image_decode_info_ck=0;
             return RESOURCE_READ_ERROR;
          }
       }else{
          image_decode_info_ck=0;
          return RESOURCE_READ_ERROR; 
       }      
    }else{
       return REENTRY_ERROR;
    }    
}

CODEC_START_STATUS image_decode_start(IMAGE_ARGUMENT arg,MEDIA_SOURCE src)
{
    INT32U fs_state;
    IMAGE_ARGUMENT image_arg;
    IMAGE_DECODE_STRUCT image;
    MEDIA_SOURCE SOURCE;
    struct stat_t jpg_info;
    
    if(image_decode_ck==0){
       image_decode_ck=1;
       image=image_decode_struct;
       image_arg=arg;
       SOURCE=src;
       
       image.source_type=SOURCE.type;
       
       if(image.source_type==SOURCE_TYPE_SDRAM){
	     image.image_source=(INT32S)SOURCE.type_ID.memptr;
	     image.source_size=(image_arg.OutputWidth*image_arg.OutputHeight)*2;//jpg file size
       }else{
         image.image_source=(INT32S)SOURCE.type_ID.FileHandle;
         fs_state=fstat((INT16S)SOURCE.type_ID.FileHandle,&jpg_info);       //get image file infomation
	     if(fs_state==0)
	        image.source_size=jpg_info.st_size;                             //jpg file size
	     else
	        image.source_size=0x3fffff;                                     //jpg file size  
	   }
       image.output_buffer_pointer=(INT32U)image_arg.OutputBufPtr;
       image.output_buffer_width=(INT16U)image_arg.OutputBufWidth;
       image.output_buffer_height=(INT16U)image_arg.OutputBufHeight;
       image.output_image_width=(INT16U)image_arg.OutputWidth;
       image.output_image_height=(INT16U)image_arg.OutputHeight;
	   
	   switch(image_arg.OutputFormat)
       {          
             case IMAGE_OUTPUT_FORMAT_RGB565:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_RGB565;
                  break;           
             case IMAGE_OUTPUT_FORMAT_YUYV:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV;
                  break;           
             case IMAGE_OUTPUT_FORMAT_UYVY:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_UYVY;
                  break;          
       #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
       		 case IMAGE_OUTPUT_FORMAT_RGB1555:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_RGB1555;
                  break;        
             case IMAGE_OUTPUT_FORMAT_RGBG:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_RGBG;
                  break;                  
             case IMAGE_OUTPUT_FORMAT_GRGB:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_GRGB;
                  break;                      
             case IMAGE_OUTPUT_FORMAT_YUYV8X32:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X32;
                  break;            
             case IMAGE_OUTPUT_FORMAT_YUYV8X64:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X64;
                  break;           
             case IMAGE_OUTPUT_FORMAT_YUYV16X32:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X32;
                  break;                  
             case IMAGE_OUTPUT_FORMAT_YUYV16X64:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X64;
                  break;                
             case IMAGE_OUTPUT_FORMAT_YUYV32X32:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV32X32;
                  break;            
             case IMAGE_OUTPUT_FORMAT_YUYV64X64:
                  image.output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV64X64;
                  break;               
       #endif
       		default:
       			 return RESOURCE_NO_FOUND_ERROR;               
       }
   #if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
   		if(image_arg.ScalerOutputRatio != FIT_OUTPUT_SIZE) {
   			DBG_PRINT("GPL32700 Only Support FIT_OUTPUT_SIZE!!!\r\n");
   			return CODEC_START_STATUS_ERROR_MAX; 
   		}
   #endif     
       image.output_ratio=image_arg.ScalerOutputRatio;
       image.out_of_boundary_color=image_arg.OutBoundaryColor;
       image.cmd_id =1;
       //SEND MSG
       msgQSend(image_msg_queue_id, MSG_IMAGE_DECODE_REQ, (void *)&image,  sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
       return START_OK;        
    }else{   
       return RESOURCE_NO_FOUND_ERROR;  
    }
}

void image_decode_stop(void)
{
     IMAGE_DECODE_STRUCT image;
     INT32U msg_id;
     
     image=image_decode_struct;
     image.cmd_id =1;
     msgQSend(image_msg_queue_id, MSG_IMAGE_STOP_REQ, (void *)&image,  sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
     msgQReceive(ApQ, &msg_id, (void *)image_decode_task_para, sizeof(IMAGE_DECODE_STRUCT));
}
																
IMAGE_CODEC_STATUS image_decode_status(void)
{   
   INT32U msg_id,photo_status;
   IMAGE_DECODE_STRUCT *image;

   //RECEIVE MSG
   msgQReceive(ApQ, &msg_id, (void *) &image_decode_task_para, sizeof(IMAGE_DECODE_STRUCT));
   if(msg_id==MSG_IMAGE_DECODE_REPLY){  
      image_decode_ck=0;
      image=(IMAGE_DECODE_STRUCT *)image_decode_task_para;
      if((image->decode_status)!=0){
        photo_status=IMAGE_CODEC_DECODE_FAIL; 
        return photo_status;
      }else{
        photo_status=IMAGE_CODEC_DECODE_END; 
        return photo_status;
      }
   }else{      
        photo_status=IMAGE_CODEC_STATUS_MAX; 
        return photo_status;
   }
   
}

CODEC_START_STATUS image_scale_start(IMAGE_SCALE_ARGUMENT arg)
{     
	IMAGE_SCALE_STRUCT image_scaler;
	IMAGE_SCALE_ARGUMENT image_scaler_arg;
#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XX)
	DMA_STRUCT dma_struct;
	INT8S done1;
	INT16U in_scaler_x,out_scaler_x,SCALER_NUM,i,j;
	INT32U SCALER_STATE,scaler_buffer_1,scaler_buffer_2,scaler_buffer_3,scaler_buffer_4,scaler_buffer_5,scaler_buffer_6,scaler_buffer_7,scaler_buffer_8;
	INT32U S_Buf_1,S_Buf_2,S_Buf_3,S_Buf_4,S_Buf_5,S_Buf_6,S_Buf_7,S_Buf_8,T_Buf,result,line_offset,dma_move_size;
#endif
       
    if(image_scaler_ck==0){
    	image_scaler_ck=1;
        malloc_buffer_error=0;
        scaler_format_error=0;
        image_scaler=image_scale_struct;
        image_scaler_arg=arg;
    #if (defined MCU_VERSION) && (MCU_VERSION < GPL326XX)
        //Software scaler function Supported the scaler x size to exceed 1024 pixel. 
        if((image_scaler_arg.OutputBufWidth > 1024) || (image_scaler_arg.OutputWidth > 1024)){
           if((image_scaler_arg.OutputWidth > 1024)&&(image_scaler_arg.OutputWidth <= 2048)){
               SCALER_NUM=SCALER_SIZE_2;
               if (image_scaler_arg.InputWidth & SCALER_2_MULTIPLE) {
                   image_scaler_arg.InputWidth &= ~SCALER_2_MULTIPLE;
			       DBG_PRINT(" scaler input Width Size error!\r\n");
               }
               in_scaler_x=(image_scaler_arg.InputWidth / 2);
               if (image_scaler_arg.OutputWidth & SCALER_8_MULTIPLE) {
                   image_scaler_arg.OutputWidth &= ~SCALER_8_MULTIPLE;
			       DBG_PRINT(" scaler Output Width Size error!\r\n");
		       }
               out_scaler_x=(image_scaler_arg.OutputWidth / 2);// Make sure output width is at least multiple of 8 pixels
               scaler_buffer_1=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_2=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               S_Buf_1=scaler_buffer_1;
               S_Buf_2=scaler_buffer_2;
               if(!scaler_buffer_1 || !scaler_buffer_2){
				 gp_free((void *) scaler_buffer_1);
				 gp_free((void *) scaler_buffer_2);
				 DBG_PRINT(" 1024 < scaler size <=2048 malloc buffer error!\r\n");
				 malloc_buffer_error=1;
			   }
           }else if((image_scaler_arg.OutputWidth > 2048)&&(image_scaler_arg.OutputWidth <= 4096)){
               SCALER_NUM=SCALER_SIZE_4;
               if (image_scaler_arg.InputWidth & SCALER_2_MULTIPLE) {
                   image_scaler_arg.InputWidth &= ~SCALER_2_MULTIPLE;
			       DBG_PRINT(" scaler input Width Size error!\r\n");
               }               
               in_scaler_x=(image_scaler_arg.InputWidth / 4);
               if (image_scaler_arg.OutputWidth & SCALER_8_MULTIPLE) {
                   image_scaler_arg.OutputWidth &= ~SCALER_8_MULTIPLE;
			       DBG_PRINT(" scaler Output Width Size error!\r\n");
		       }               
               out_scaler_x=(image_scaler_arg.OutputWidth / 4);// Make sure output width is at least multiple of 8 pixels
               scaler_buffer_1=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_2=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_3=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_4=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               S_Buf_1=scaler_buffer_1;
               S_Buf_2=scaler_buffer_2;
               S_Buf_3=scaler_buffer_3;
               S_Buf_4=scaler_buffer_4;
               if(!scaler_buffer_1 || !scaler_buffer_2 || !scaler_buffer_3 || !scaler_buffer_4){
				 gp_free((void *) scaler_buffer_1);
				 gp_free((void *) scaler_buffer_2);
				 gp_free((void *) scaler_buffer_3);
				 gp_free((void *) scaler_buffer_4);
				 DBG_PRINT(" 2048 < scaler size <=4096 malloc buffer error!\r\n");
				 malloc_buffer_error=1;
			   }
           }else{
               SCALER_NUM=SCALER_SIZE_8;
               if (image_scaler_arg.InputWidth & SCALER_2_MULTIPLE) {
                   image_scaler_arg.InputWidth &= ~SCALER_2_MULTIPLE;
			       DBG_PRINT(" scaler input Width Size error!\r\n");
               }
               in_scaler_x=(image_scaler_arg.InputWidth / 8);
               if (image_scaler_arg.OutputWidth & SCALER_8_MULTIPLE) {
                   image_scaler_arg.OutputWidth &= ~SCALER_8_MULTIPLE;
			       DBG_PRINT(" scaler Output Width Size error!\r\n");
		       }               
               out_scaler_x=(image_scaler_arg.OutputWidth / 8);// Make sure output width is at least multiple of 8 pixels               
               scaler_buffer_1=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_2=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_3=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_4=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_5=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_6=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_7=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               scaler_buffer_8=(INT32U) gp_malloc_align((out_scaler_x*image_scaler_arg.OutputHeight)*2, 16);
               S_Buf_1=scaler_buffer_1;
               S_Buf_2=scaler_buffer_2;
               S_Buf_3=scaler_buffer_3;
               S_Buf_4=scaler_buffer_4;
               S_Buf_5=scaler_buffer_5;
               S_Buf_6=scaler_buffer_6;
               S_Buf_7=scaler_buffer_7;
               S_Buf_8=scaler_buffer_8;  
               if(!scaler_buffer_1 || !scaler_buffer_2 || !scaler_buffer_3 || !scaler_buffer_4 || !scaler_buffer_5 || !scaler_buffer_6 || !scaler_buffer_7 || !scaler_buffer_8){
				 gp_free((void *) scaler_buffer_1);
				 gp_free((void *) scaler_buffer_2);
				 gp_free((void *) scaler_buffer_3);
				 gp_free((void *) scaler_buffer_4);
				 gp_free((void *) scaler_buffer_5);
				 gp_free((void *) scaler_buffer_6);
				 gp_free((void *) scaler_buffer_7);
				 gp_free((void *) scaler_buffer_8);
				 DBG_PRINT(" 4096 < scaler size <=8192 malloc buffer error!\r\n");
				 malloc_buffer_error=1;
			   }                            
           }    
         if(malloc_buffer_error==0){     
           for(i=0;i<SCALER_NUM;i++)
           {
               image_scaler.input_width=image_scaler_arg.InputWidth;
               image_scaler.input_height=image_scaler_arg.InputHeight;
               image_scaler.input_visible_width=image_scaler_arg.inputvisiblewidth;
               image_scaler.input_visible_height=image_scaler_arg.inputvisibleheight;
               image_scaler.input_offset_x=((image_scaler_arg.inputoffsetx+(i*in_scaler_x))<<16);
               image_scaler.input_offset_y=image_scaler_arg.inputoffsety;
               if(image_scaler_arg.outputwidthfactor!=0 || image_scaler_arg.outputheightfactor!=0){
                  image_scaler.output_width_factor=image_scaler_arg.outputwidthfactor;
                  image_scaler.output_height_factor=image_scaler_arg.outputheightfactor;
               }else{
                  image_scaler.output_width_factor=(image_scaler_arg.InputWidth<<16)/image_scaler_arg.OutputWidth;
                  image_scaler.output_height_factor=(image_scaler_arg.InputHeight<<16)/image_scaler_arg.OutputHeight;
               } 
               image_scaler.output_buffer_width=out_scaler_x;
               image_scaler.output_buffer_height=image_scaler_arg.OutputHeight;
               image_scaler.input_buf1=(void *)image_scaler_arg.InputBufPtr;
               if(i==0)
                 image_scaler.output_buf1=(void *)scaler_buffer_1; 
               else if(i==1)
                 image_scaler.output_buf1=(void *)scaler_buffer_2;
               else if(i==2)
                 image_scaler.output_buf1=(void *)scaler_buffer_3;
               else if(i==3)
                 image_scaler.output_buf1=(void *)scaler_buffer_4;
               else if(i==4)
                 image_scaler.output_buf1=(void *)scaler_buffer_5;
               else if(i==5)
                 image_scaler.output_buf1=(void *)scaler_buffer_6;
               else if(i==6)
                 image_scaler.output_buf1=(void *)scaler_buffer_7;
               else
                 image_scaler.output_buf1=(void *)scaler_buffer_8;
   
               switch(image_scaler_arg.InputFormat)
               { 
                      case SCALER_INPUT_FORMAT_RGB565:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGB565;
                           break;                     
                      case SCALER_INPUT_FORMAT_YUYV:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_YUYV;
                           break;           
                      case SCALER_INPUT_FORMAT_UYVY:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_UYVY;
                           break; 
               #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
                      case SCALER_INPUT_FORMAT_RGB1555:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGB1555;
                           break;        
                      case SCALER_INPUT_FORMAT_RGBG:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGBG;
                           break;                  
                      case SCALER_INPUT_FORMAT_GRGB:
                           image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_GRGB;
                           break;               
               #endif                
                      default:
                           DBG_PRINT("scaler input format error!\r\n");
                           scaler_format_error=1;
                           break;         
               }
               switch(image_scaler_arg.OutputFormat)
               {                  
                      case IMAGE_OUTPUT_FORMAT_RGB565:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGB565;
                           break;                      
                      case IMAGE_OUTPUT_FORMAT_YUYV:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV;
                           break;           
                      case IMAGE_OUTPUT_FORMAT_UYVY:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_UYVY;
                           break;          
               #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
               		  case IMAGE_OUTPUT_FORMAT_RGB1555:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGB1555;
                           break;        
                      case IMAGE_OUTPUT_FORMAT_RGBG:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGBG;
                           break;                  
                      case IMAGE_OUTPUT_FORMAT_GRGB:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_GRGB;
                           break;               
                      case IMAGE_OUTPUT_FORMAT_YUYV8X32:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X32;
                           break;            
                      case IMAGE_OUTPUT_FORMAT_YUYV8X64:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X64;
                           break;           
                      case IMAGE_OUTPUT_FORMAT_YUYV16X32:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X32;
                           break;                  
                      case IMAGE_OUTPUT_FORMAT_YUYV16X64:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X64;
                           break;                
                      case IMAGE_OUTPUT_FORMAT_YUYV32X32:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV32X32;
                           break;            
                      case IMAGE_OUTPUT_FORMAT_YUYV64X64:
                           image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV64X64;
                           break;
               #endif
                      default:
                           DBG_PRINT("scaler output format error!\r\n");
                           scaler_format_error=1;
                           break;          
               } 
               image_scaler.out_of_boundary_color=image_scaler_arg.OutBoundaryColor;
               image_scaler.cmd_id =1;
               //SEND MSG
               if(scaler_format_error==0){ 
                  msgQSend(image_msg_queue_id, MSG_IMAGE_SCALE_REQ, (void *)&image_scaler,  sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
                  if(i < (SCALER_NUM-1))
                     SCALER_STATE=image_scale_status(); 
               }else{
                  DBG_PRINT("scaler format error !\r\n");
                  break;
               }      
           }
           if(scaler_format_error==0){ 
             T_Buf=(INT32U)image_scaler_arg.OutputBufPtr;
             line_offset=(out_scaler_x*2);
             dma_move_size=(out_scaler_x/2);                
             //Use the DMA to combine image
             for(i=0;i<image_scaler_arg.OutputBufHeight;i++)
             {                          
               for(j=0;j<SCALER_NUM;j++){
                  done1 = C_DMA_STATUS_WAITING;
	              if(j==0)
	                 dma_struct.s_addr = (INT32U) S_Buf_1;
	              else if(j==1)
	                 dma_struct.s_addr = (INT32U) S_Buf_2;
	              else if(j==2)
	                 dma_struct.s_addr = (INT32U) S_Buf_3;
	              else if(j==3)
	                 dma_struct.s_addr = (INT32U) S_Buf_4;
	              else if(j==4)
	                 dma_struct.s_addr = (INT32U) S_Buf_5;
	              else if(j==5)
	                 dma_struct.s_addr = (INT32U) S_Buf_6;
	              else if(j==6)
	                 dma_struct.s_addr = (INT32U) S_Buf_7;
	              else
	                 dma_struct.s_addr = (INT32U) S_Buf_8;   	                        
	              dma_struct.t_addr = (INT32U) T_Buf;
	              dma_struct.width = DMA_DATA_WIDTH_4BYTE;
	              dma_struct.count = dma_move_size;
	              dma_struct.notify = &done1;
	              dma_struct.timeout = 0;
	              result=dma_transfer_wait_ready(&dma_struct);
	              T_Buf+=line_offset;
	           }
	           //T_Buf+=line_offset;
	           if(SCALER_NUM==SCALER_SIZE_2){
                  S_Buf_1+=line_offset;
                  S_Buf_2+=line_offset;
               }else if(SCALER_NUM==SCALER_SIZE_4){
                  S_Buf_1+=line_offset;
                  S_Buf_2+=line_offset;
                  S_Buf_3+=line_offset;
                  S_Buf_4+=line_offset; 
               }else{
                  S_Buf_1+=line_offset;
                  S_Buf_2+=line_offset;
                  S_Buf_3+=line_offset;
                  S_Buf_4+=line_offset; 
                  S_Buf_5+=line_offset;
                  S_Buf_6+=line_offset;
                  S_Buf_7+=line_offset;
                  S_Buf_8+=line_offset;                               
               }	
	         }
             if(SCALER_NUM==SCALER_SIZE_2){
           	    gp_free((void *) scaler_buffer_1);
	            gp_free((void *) scaler_buffer_2);
             }else if(SCALER_NUM==SCALER_SIZE_4){
                gp_free((void *) scaler_buffer_1);
	            gp_free((void *) scaler_buffer_2);
	            gp_free((void *) scaler_buffer_3);
	            gp_free((void *) scaler_buffer_4);
             }else{
                gp_free((void *) scaler_buffer_1);
	            gp_free((void *) scaler_buffer_2);
	            gp_free((void *) scaler_buffer_3);
	            gp_free((void *) scaler_buffer_4);	                     
                gp_free((void *) scaler_buffer_5);
	            gp_free((void *) scaler_buffer_6);
	            gp_free((void *) scaler_buffer_7);
	            gp_free((void *) scaler_buffer_8);
             }
             image_scaler_ck=0;          
             return START_OK;
           }else{
             image_scaler_ck=0;
             return RESOURCE_READ_ERROR; 
           }    
         }
          image_scaler_ck=0;
          return RESOURCE_NO_FOUND_ERROR; 
        } else 
        #endif
        {
           image_scaler.input_width=image_scaler_arg.InputWidth;
           image_scaler.input_height=image_scaler_arg.InputHeight;
           image_scaler.input_visible_width=image_scaler_arg.inputvisiblewidth;
           image_scaler.input_visible_height=image_scaler_arg.inputvisibleheight;
           image_scaler.input_offset_x=image_scaler_arg.inputoffsetx;
           image_scaler.input_offset_y=image_scaler_arg.inputoffsety;
           if(image_scaler_arg.outputwidthfactor!=0 || image_scaler_arg.outputheightfactor!=0){
             image_scaler.output_width_factor=image_scaler_arg.outputwidthfactor;
             image_scaler.output_height_factor=image_scaler_arg.outputheightfactor;
           }else{
             image_scaler.output_width_factor=(image_scaler_arg.InputWidth<<16)/image_scaler_arg.OutputWidth;
             image_scaler.output_height_factor=(image_scaler_arg.InputHeight<<16)/image_scaler_arg.OutputHeight;
           } 
           image_scaler.output_buffer_width=image_scaler_arg.OutputBufWidth;// Make sure output width is at least multiple of 8 pixels
           if (image_scaler.output_buffer_width & SCALER_8_MULTIPLE) {
                 image_scaler.output_buffer_width &= ~SCALER_8_MULTIPLE;
			     DBG_PRINT(" scaler Output Width Size error!\r\n");
		   }
           image_scaler.output_buffer_height=image_scaler_arg.OutputBufHeight;
           image_scaler.input_buf1=(void *)image_scaler_arg.InputBufPtr;
           image_scaler.output_buf1=(void *)image_scaler_arg.OutputBufPtr;
           switch(image_scaler_arg.InputFormat)
           { 
                 case SCALER_INPUT_FORMAT_RGB565:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGB565;
                      break;                     
                 case SCALER_INPUT_FORMAT_YUYV:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_YUYV;
                      break;           
                 case SCALER_INPUT_FORMAT_UYVY:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_UYVY;
                      break; 
           #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
                 case SCALER_INPUT_FORMAT_RGB1555:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGB1555;
                      break;        
                 case SCALER_INPUT_FORMAT_RGBG:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_RGBG;
                      break;                  
                 case SCALER_INPUT_FORMAT_GRGB:
                      image_scaler.scaler_input_format=(INT8U)C_SCALER_CTRL_IN_GRGB;
                      break;               
           #endif            
                 default:
                      DBG_PRINT("scaler input format error!\r\n");
                      scaler_format_error=1;
                      break;         
           }
           switch(image_scaler_arg.OutputFormat)
           { 
                 case IMAGE_OUTPUT_FORMAT_RGB565:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGB565;
                      break;           
                 case IMAGE_OUTPUT_FORMAT_YUYV:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV;
                      break;           
                 case IMAGE_OUTPUT_FORMAT_UYVY:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_UYVY;
                      break;          
			#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)                      
                 case IMAGE_OUTPUT_FORMAT_RGB1555:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGB1555;
                      break;        
                 case IMAGE_OUTPUT_FORMAT_RGBG:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_RGBG;
                      break;                  
                 case IMAGE_OUTPUT_FORMAT_GRGB:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_GRGB;
                      break;               
                 case IMAGE_OUTPUT_FORMAT_YUYV8X32:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X32;
                      break;            
                 case IMAGE_OUTPUT_FORMAT_YUYV8X64:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV8X64;
                      break;           
                 case IMAGE_OUTPUT_FORMAT_YUYV16X32:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X32;
                      break;                  
                 case IMAGE_OUTPUT_FORMAT_YUYV16X64:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV16X64;
                      break;                
                 case IMAGE_OUTPUT_FORMAT_YUYV32X32:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV32X32;
                      break;            
                 case IMAGE_OUTPUT_FORMAT_YUYV64X64:
                      image_scaler.scaler_output_format=(INT8U)C_SCALER_CTRL_OUT_YUYV64X64;
                      break;
          	#endif
                 default:
                      DBG_PRINT("scaler output format error!\r\n");
                      scaler_format_error=1;
                      break;                 
           } 
           image_scaler.out_of_boundary_color=image_scaler_arg.OutBoundaryColor;
           image_scaler.cmd_id =1;
           if(scaler_format_error==0){ 
              //SEND MSG
              msgQSend(image_msg_queue_id, MSG_IMAGE_SCALE_REQ, (void *)&image_scaler,  sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
              image_scaler_ck=0;
              return START_OK;
           }else{
              image_scaler_ck=0;
              DBG_PRINT("scaler format error !\r\n");
              return RESOURCE_READ_ERROR;
           } 
        }
    }else{       
        return REENTRY_ERROR;       
    }
       
}

IMAGE_CODEC_STATUS image_scale_status(void)
{
        INT32U msg_id,scaler_status;
        IMAGE_SCALE_STRUCT *image_scaler;
        
        msgQReceive(ApQ, &msg_id, (void *) &image_decode_task_para, sizeof(IMAGE_SCALE_STRUCT));
        if(msg_id==MSG_IMAGE_SCALE_REPLY){  
           image_scaler_ck=0;
           image_scaler=(IMAGE_SCALE_STRUCT *)image_decode_task_para;
           if((image_scaler->status)!=0){
               scaler_status=IMAGE_CODEC_DECODE_FAIL;
               scaler_stop();
               return scaler_status;
           }else{
               scaler_status=IMAGE_CODEC_DECODE_END; 
               scaler_stop();
               return scaler_status;
           }
        }else{      
           scaler_status=IMAGE_CODEC_STATUS_MAX; 
           return scaler_status;
        }
}


CODEC_START_STATUS image_block_encode_scale(IMAGE_SCALE_ARGUMENT *scaler_arg,IMAGE_ENCODE_PTR *output_ptr)
{
   INT32S scaler_status;
   INT32U scaler_input_format,scaler_output_format,scaler_y_addr,scaler_u_addr,scaler_v_addr,scaler_factor_x,scaler_factor_y;
   IMAGE_SCALE_ARGUMENT *block_scaler_arg;
   IMAGE_ENCODE_PTR *scaler_output_ptr;

   if(image_scaler_ck==0){
     block_scaler_arg=scaler_arg;
     scaler_output_ptr=output_ptr;
     image_scaler_ck=1;
     scaler_format_error=0;
     malloc_buffer_error=0;
     switch(block_scaler_arg->InputFormat)
     { 
          case SCALER_INPUT_FORMAT_RGB565:
               scaler_input_format=C_SCALER_CTRL_IN_RGB565;
               break;                                   
          case SCALER_INPUT_FORMAT_YUYV:
               scaler_input_format=C_SCALER_CTRL_IN_YUYV;
               break;           
          case SCALER_INPUT_FORMAT_UYVY:
               scaler_input_format=C_SCALER_CTRL_IN_UYVY;
               break;
     #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
          case SCALER_INPUT_FORMAT_RGB1555:
               scaler_input_format=C_SCALER_CTRL_IN_RGB1555;
               break;        
          case SCALER_INPUT_FORMAT_RGBG:
               scaler_input_format=C_SCALER_CTRL_IN_RGBG;
               break;                  
          case SCALER_INPUT_FORMAT_GRGB:
               scaler_input_format=C_SCALER_CTRL_IN_GRGB;
               break;               
     #endif        
          default:
               scaler_format_error=1;
               break;         
     }
     switch(block_scaler_arg->OutputFormat)
     {       
           case IMAGE_OUTPUT_FORMAT_RGB565:
                scaler_output_format=C_SCALER_CTRL_OUT_RGB565;
                break;                      
           case IMAGE_OUTPUT_FORMAT_YUYV:
                scaler_output_format=C_SCALER_CTRL_OUT_YUYV;
                break;           
           case IMAGE_OUTPUT_FORMAT_UYVY:
                scaler_output_format=C_SCALER_CTRL_OUT_UYVY;
                break;          
     #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)                      
           case IMAGE_OUTPUT_FORMAT_RGB1555:
                scaler_output_format=C_SCALER_CTRL_OUT_RGB1555;
                break;        
           case IMAGE_OUTPUT_FORMAT_RGBG:
                scaler_output_format=C_SCALER_CTRL_OUT_RGBG;
                break;                  
           case IMAGE_OUTPUT_FORMAT_GRGB:
                scaler_output_format=C_SCALER_CTRL_OUT_GRGB;
                break;               
           case IMAGE_OUTPUT_FORMAT_YUV422:
                scaler_output_format=C_SCALER_CTRL_OUT_YUV422;
                break;                  
           case IMAGE_OUTPUT_FORMAT_YUV420:
                scaler_output_format=C_SCALER_CTRL_OUT_YUV420;
                break;                
           case IMAGE_OUTPUT_FORMAT_YUV411:
                scaler_output_format=C_SCALER_CTRL_OUT_YUV411;
                break;            
           case IMAGE_OUTPUT_FORMAT_YUV444:
                scaler_output_format=C_SCALER_CTRL_OUT_YUV444;
                break;
     #endif
           default:
                scaler_format_error=1;
                break;                  
     }
     if(scaler_format_error==0){
	    scaler_y_addr = (INT32U)scaler_output_ptr->yaddr;
	    scaler_u_addr = (INT32U)scaler_output_ptr->uaddr;
	    scaler_v_addr = (INT32U)scaler_output_ptr->vaddr;	
        if (!scaler_y_addr || !scaler_u_addr || !scaler_v_addr)
              malloc_buffer_error=1;
	    if(malloc_buffer_error==0){
	       scaler_init();
	       scaler_input_pixels_set(block_scaler_arg->InputWidth, block_scaler_arg->InputHeight);
	       scaler_input_visible_pixels_set(block_scaler_arg->InputWidth, block_scaler_arg->InputHeight);
	       scaler_input_addr_set((INT32U)block_scaler_arg->InputBufPtr, NULL, NULL);
	       scaler_input_format_set(scaler_input_format);
	       scaler_input_offset_set(block_scaler_arg->inputoffsetx,block_scaler_arg->inputoffsety);
	       scaler_factor_x=block_scaler_arg->outputwidthfactor;
	       scaler_factor_y=block_scaler_arg->outputheightfactor;
	       if(scaler_factor_x==0 || scaler_factor_y==0){
               scaler_factor_x=(block_scaler_arg->InputWidth<<16)/block_scaler_arg->OutputWidth;
               scaler_factor_y=(block_scaler_arg->InputHeight<<16)/block_scaler_arg->OutputHeight;
           }
	       scaler_output_pixels_set(scaler_factor_x,scaler_factor_y,block_scaler_arg->OutputWidth,block_scaler_arg->OutputHeight);
	       scaler_output_addr_set(scaler_y_addr,scaler_u_addr,scaler_v_addr);
	       scaler_output_format_set(scaler_output_format);
	       scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	       scaler_out_of_boundary_color_set(block_scaler_arg->OutBoundaryColor);
	       while (1){
  		        scaler_status = scaler_wait_idle();
  		        if(scaler_status == C_SCALER_STATUS_STOP){
			       scaler_start();
		        }else if(scaler_status & C_SCALER_STATUS_DONE){
			       break;
		        }else if(scaler_status & C_SCALER_STATUS_TIMEOUT){
			       DBG_PRINT("Scaler status timeout to finish its job\r\n");
			       break;
		        }else if(scaler_status & C_SCALER_STATUS_INIT_ERR){
			       DBG_PRINT("Scaler status initiation error to finish its job\r\n");
			       break;
		        }else if(scaler_status & C_SCALER_STATUS_INPUT_EMPTY){
			       DBG_PRINT("Scaler status input empty to finish its job\r\n");
			       break;		
  		        }else{
	  		       DBG_PRINT("Un-handled Scaler status!\r\n");
	  		       break;
	  	        }
	  	        OSTimeDly(1);
  	       }
  	       scaler_stop();
  	       image_scaler_ck=0;
  	       return START_OK;
  	    }else{
  	       image_scaler_ck=0;
  	       return RESOURCE_NO_FOUND_ERROR;
  	    }
  	 }else{
  	   image_scaler_ck=0;
  	   return RESOURCE_READ_ERROR;
  	 }
  	return REENTRY_ERROR; 
  }
  return REENTRY_ERROR; 
}
