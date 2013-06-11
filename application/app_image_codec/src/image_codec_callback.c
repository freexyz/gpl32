#include "image_codec_callback.h"

//=======================================================================
//
//  image callback 
//
//=======================================================================

void image_decode_end(void);
void image_encode_end(INT32U encode_size);
CODEC_START_STATUS image_encoded_block_read(IMAGE_SCALE_ARGUMENT *scaler_info,IMAGE_ENCODE_PTR *encode_ptr);
void user_defined_image_decode_isr(void);
void user_defined_image_decode_entrance(INT32U TV_TFT);
void scaler_picture(INT32U input_ptr);
void image_sensor_start(void);
void auto_switch_csi_frame(void);
void image_scaler_entry(void *p_arg);
void auto_switch_tv_display_frame(void);
void auto_switch_tft_display_frame(void);

INT32U tv_display_flag,display_buf,scaler_task_state,buf_ck;
INT32U encode_first=0,start_ptr,sensor_buf_1,sensor_buf_2,ppu_buf,scaler_buf_1,scaler_buf_2;
INT32U output_y_ptr,output_u_ptr,output_v_ptr;
INT32U scaler_ptr,scaler_base,tem_buf,scaler_output_x,scaler_output_y,scale_factor_x,scale_factor_y,scale_inputoffsety;
INT32U scaler_output_y_1,BLOCK_ENCODE,scaler_input_x,scaler_input_y,encode_buf,save_file_ck,decode_mode_ck;

FP32 c_xxx_zoom=1, c_zoom_tmp;
void scaler_sensor_buf(INT32U output_ptr);
MSG_Q_ID scaler_msg_queue_id;
#define SCALER_QUEUE_MAX_NUMS   		    16
#define SCALER_MAXIMUM_MESSAGE_NUM			16
#define SCALER_MAX_MSG_SIZE                0x20
#define SCALER_START                         1
#define TSK_SENSOR_SCALER                   30
#define ScalerStackSize                     512
INT32U ImageScalerStack[ScalerStackSize];
INT32U scaler_para[SCALER_MAX_MSG_SIZE/4 + 1];
extern INT32U BLOCK_READ;
extern PPU_REGISTER_SETS *ppu_register_set;

//=======================================================================
// image encode end  hook function 
//=======================================================================
void image_encode_end(INT32U encode_size)
{
	DBG_PRINT("Image Encode play-end callback...\r\n");
}


//=======================================================================
// image decode end hook function 
//=======================================================================
void image_decode_end(void)
{
	DBG_PRINT("Image Decode play-end callback...\r\n");
}

CODEC_START_STATUS image_encoded_block_read(IMAGE_SCALE_ARGUMENT *scaler_info,IMAGE_ENCODE_PTR *encode_ptr)
{
	     
	INT32U scale_state;
    IMAGE_SCALE_ARGUMENT scaler_arg;
	IMAGE_ENCODE_PTR *ptr;
    INT32U y_len,uv_len;
   
	if(BLOCK_ENCODE == 1) {
		if(BLOCK_READ==1) {
		   encode_first=0;
		   BLOCK_READ=0;
		}
		if(encode_first==0) {
			if(decode_mode_ck==0) {
		    	scaler_base=scaler_ptr;
		  	} else {
		     	scaler_base=encode_buf;
		    }    
		  	encode_first=1;
		  	start_ptr=1;                         //0:chage image input_y_buffer_ptr, 1:chage scaler input y offset 
		  	scaler_input_x=scaler_info->InputWidth; 
      		scaler_input_y=scaler_info->InputHeight;
     		scaler_output_x=scaler_info->OutputWidth;
      		scaler_output_y=scaler_info->OutputHeight;
		#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)        
			scaler_output_y <<= 1;
		#endif 
      		scaler_output_y_1=scaler_output_y+2;
      		scale_factor_x=scaler_info->outputwidthfactor;
      		scale_factor_y=scaler_info->outputheightfactor;
      		scale_inputoffsety=scaler_info->inputoffsety;
		} else {
			//image start ptr
		  	if(start_ptr==0) {
		    	y_len = (scaler_input_x*scaler_input_y)<<1;
		    	scaler_ptr+=y_len;
		    	scaler_info->InputBufPtr=(INT8U *)scaler_ptr;
		  	} else {
		    	//scale start ptr
		    	y_len = (scale_factor_y*scaler_output_y);
		    #if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)  	  
		    	y_len >>= 1;
		    #endif
		    	scale_inputoffsety+=y_len;
		  	}
		}
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)   
		tem_buf=(INT32U) gp_malloc_align((scaler_info->OutputWidth*(scaler_info->OutputHeight+2))*2, 16); 
	#else
	  	tem_buf = (INT32U)encode_ptr->yaddr;
	#endif	
		DBG_PRINT("tem_buf = 0x%08x!\r\n", (INT32U)tem_buf);
		//image scaler output infomation
		if(start_ptr==0) {
			scaler_arg.InputBufPtr=scaler_info->InputBufPtr;
		} else {  
		    scaler_arg.InputBufPtr=(INT8U *)scaler_base;
		}	    
		scaler_arg.OutputBufPtr=(INT8U *)tem_buf;
		scaler_arg.inputvisiblewidth=scaler_input_x;
		scaler_arg.inputvisibleheight=scaler_input_y;
		scaler_arg.InputWidth=scaler_input_x;
		scaler_arg.InputHeight=scaler_input_y;
		scaler_arg.outputwidthfactor=scale_factor_x;
		scaler_arg.outputheightfactor=scale_factor_y; 
		scaler_arg.inputoffsetx=0;
		scaler_arg.inputoffsety=scale_inputoffsety;
		scaler_arg.OutputBufWidth=scaler_output_x;
		scaler_arg.OutputWidth=scaler_output_x;
		scaler_arg.OutputBufHeight=scaler_output_y_1;
		scaler_arg.OutputHeight=scaler_output_y_1;
		scaler_arg.OutBoundaryColor=scaler_info->OutBoundaryColor;
		scaler_arg.InputFormat=scaler_info->InputFormat;
		scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
		image_scale_start(scaler_arg);
		while (1) {
			scale_state=image_scale_status();
			if (scale_state==IMAGE_CODEC_DECODE_END) {
			#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
			    scaler_info->InputBufPtr=(INT8U *)tem_buf;
			    scaler_info->inputvisiblewidth=scaler_output_x;
			    scaler_info->inputvisibleheight=scaler_output_y_1;
			    scaler_info->InputWidth=scaler_output_x;
			    scaler_info->InputHeight=scaler_output_y_1;
			    scaler_info->outputwidthfactor=(1<<16);
			    scaler_info->outputheightfactor=(1<<16);
			    scaler_info->inputoffsetx=0;
			    scaler_info->inputoffsety=0;
			    scaler_info->OutputBufWidth=scaler_output_x;
			    scaler_info->OutputBufHeight=scaler_output_y;
			    scaler_info->OutputWidth=scaler_output_x;
			    scaler_info->OutputHeight=scaler_output_y;
			    scaler_info->OutBoundaryColor=(INT32U)0x008080;
			    scaler_info->InputFormat=SCALER_INPUT_FORMAT_YUYV;
			    scaler_info->OutputFormat=IMAGE_OUTPUT_FORMAT_YUV422;
			    scale_state=image_block_encode_scale(scaler_info,encode_ptr);
			    while (1){
			        if(scale_state==START_OK){
			           DBG_PRINT("image scaler ok\r\n");
			           break;
			        }else{
			           DBG_PRINT("image scaler failed\r\n");
			           break;
			        }
			    }
			#endif  
		   		break;
			} else {
			    DBG_PRINT("image scaler failed\r\n");
			    break;
			}	
		}
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
		gp_free((void *) tem_buf);       
	#endif
		return START_OK;  
	} else {
	    if(BLOCK_READ==1) {
	       encode_first=0;
	       BLOCK_READ=0;
	    }
	    if(encode_first==1) {
	       y_len = (320*16)<<1;		  		// Encode 16 lines each time
	    }
	    if(encode_first==0) {
	      ptr=encode_ptr;
	      output_y_ptr=(INT32U)ptr->yaddr;
	      output_u_ptr=(INT32U)ptr->uaddr;
	      output_v_ptr=(INT32U)ptr->vaddr;
	      y_len=0;
	      uv_len=0;
	      encode_first=1;
	    }
	    scaler_ptr+=y_len;
	    scaler_picture(scaler_ptr);
	    return START_OK;
	}
}

void scaler_picture(INT32U input_ptr)
{

    INT32S scaler_status;

	scaler_init();
	
	scaler_input_pixels_set(320, 16);
	scaler_input_visible_pixels_set(320, 16);
	scaler_input_addr_set(input_ptr, NULL, NULL);
	scaler_input_format_set(C_SCALER_CTRL_IN_YUYV);

	scaler_output_pixels_set((1<<16), (1<<16), 320, 16);
	scaler_output_addr_set(output_y_ptr, output_u_ptr, output_v_ptr);
	scaler_output_format_set(C_SCALER_CTRL_OUT_YUV422);

	scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	scaler_out_of_boundary_color_set(0x008080);

	while (1) {
  		scaler_status = scaler_wait_idle();
  		if (scaler_status == C_SCALER_STATUS_STOP) {
			scaler_start();
		} else if (scaler_status & C_SCALER_STATUS_DONE) {
			break;
		} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
			DBG_PRINT("Scaler failed to finish its job\r\n");
			break;
  		} else {
	  		DBG_PRINT("Un-handled Scaler status!\r\n");
	  		break;
	  	}
  	}
  	scaler_stop();
 	
}

void scaler_sensor_buf(INT32U output_ptr)
{

    INT32S scaler_status;

	scaler_init();
	
    scaler_image_pixels_set(640, 480, 320, 240);
	scaler_input_addr_set(ppu_buf, NULL, NULL);
	scaler_input_format_set(C_SCALER_CTRL_IN_YUYV);
	scaler_output_addr_set(output_ptr, 0, 0);
	scaler_output_format_set(C_SCALER_CTRL_OUT_YUYV);

	scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	scaler_out_of_boundary_color_set(0x008080);

	while (1) {
  		scaler_status = scaler_wait_idle();
  		if (scaler_status == C_SCALER_STATUS_STOP) {
			scaler_start();
		} else if (scaler_status & C_SCALER_STATUS_DONE) {
			break;
		} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
			DBG_PRINT("Scaler failed to finish its job\r\n");
			break;
  		} else {
	  		DBG_PRINT("Un-handled Scaler status!\r\n");
	  		break;
	  	}
  	}
  	scaler_stop();
 	
}
//=======================================================================================
//		Following are the user-defined code for display on TV or TFT LCM
//=======================================================================================
//=======================================================================
//	tv isr handle
//=======================================================================
void user_defined_image_decode_isr(void)
{
	// tft Vertical-Blanking interrupt
	if ((R_PPU_IRQ_STATUS & 0x00002000) && (R_PPU_IRQ_EN & 0x00002000))
	{
		R_PPU_IRQ_STATUS = 0x00002000;
        auto_switch_tft_display_frame();
	}
	if((R_PPU_IRQ_STATUS & 0x000000800) && (R_PPU_IRQ_EN & 0x00000800)){
		R_PPU_IRQ_STATUS = 0x000000800;
		auto_switch_tv_display_frame();
	}
	//sensor
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x40){
	   R_PPU_IRQ_STATUS = 0x40;
	   auto_switch_csi_frame();
	}
}

void user_defined_image_decode_entrance(INT32U TV_TFT)
{
	tv_display_flag=0;
	scaler_task_state=1;
	save_file_ck=0;
	
	// Register PPU ISR
	vic_irq_register(VIC_PPU, user_defined_image_decode_isr);
	R_PPU_IRQ_STATUS = 0x7FFF;	// Clear all PPU pending interrupts
	if(TV_TFT >= C_TV_QVGA)
	   R_PPU_IRQ_EN=0x0800;     // TV Vertical-Blanking IRQ enable register
	else
       R_PPU_IRQ_EN=0x2000;     // TFT Vertical-Blanking IRQ enable register
	vic_irq_enable(VIC_PPU);
    
    //Create a scaler task
    OSTaskCreate(image_scaler_entry, (void *) 0, &ImageScalerStack[ScalerStackSize - 1], TSK_SENSOR_SCALER);
}
//=======================================================================================
//		Following are the user-defined code
//=======================================================================================
void image_sensor_start(void)
{
    DBG_PRINT("Enable Sensor\r\n");

    sensor_buf_1=(INT32U) gp_malloc_align(640*480*2, 64);
	sensor_buf_2=(INT32U) gp_malloc_align(640*480*2, 64);
	scaler_buf_1=(INT32U) gp_malloc_align(320*240*2, 64);
	scaler_buf_2=(INT32U) gp_malloc_align(320*240*2, 64);
	
	// Setup CMOS sensor
	CSI_Init(640, 480, FT_CSI_YUVIN|FT_CSI_RGB1555, sensor_buf_1, NULL);
	(*((volatile INT32U *) 0xD0500240)) = 0x2C67;
	(*((volatile INT32U *) 0xD0500244)) = 0xC090;
	R_PPU_IRQ_EN |= 0x40;	//enable CMOS SENSOR
    buf_ck=0;	
}

void auto_switch_csi_frame(void)
{
	 if(scaler_task_state==1){
	   if(buf_ck == 0){
 	      ppu_buf=sensor_buf_1;
 	      *P_CSI_TG_FBSADDR = sensor_buf_2;
 	      buf_ck=1;	            
       }else{
		  ppu_buf=sensor_buf_2;
		  *P_CSI_TG_FBSADDR = sensor_buf_1;
		  buf_ck=0;
       }
       if(save_file_ck==0){
          msgQSend(scaler_msg_queue_id, SCALER_START,(void *)0, 0, MSG_PRI_NORMAL); 	
          scaler_task_state=0;
       }
     }
}

void auto_switch_tv_display_frame(void)
{ 
       if(tv_display_flag==1){
          encode_buf=display_buf;
          video_codec_show_image(C_DISPLAY_DEVICE,display_buf, C_DISPLAY_DEVICE,IMAGE_OUTPUT_FORMAT_YUYV);
          tv_display_flag=0;
       }	
}

void auto_switch_tft_display_frame(void)
{ 
       if(tv_display_flag==1){
          encode_buf=display_buf;
          video_codec_show_image(C_DISPLAY_DEVICE,display_buf, C_DISPLAY_DEVICE,IMAGE_OUTPUT_FORMAT_YUYV);
          tv_display_flag=0;
       }	
}

void image_scaler_entry(void *p_arg)
{
   INT32U scale_state,msg_id;
   IMAGE_SCALE_ARGUMENT scaler_arg;
   
   scaler_msg_queue_id = msgQCreate(SCALER_QUEUE_MAX_NUMS, SCALER_MAXIMUM_MESSAGE_NUM, SCALER_MAX_MSG_SIZE);
  
   while(1){
      msgQReceive(scaler_msg_queue_id, &msg_id, (void *) scaler_para, SCALER_MAX_MSG_SIZE);
      if(msg_id==SCALER_START){	
	     if(buf_ck == 0){
		    display_buf=scaler_buf_1;
            scaler_arg.OutputBufPtr=(INT8U *)scaler_buf_1;
         }else{
		    display_buf=scaler_buf_2;
            scaler_arg.OutputBufPtr=(INT8U *)scaler_buf_2;
         }  
         scaler_arg.InputBufPtr=(INT8U *)ppu_buf;
         scaler_arg.inputvisiblewidth=640;
	     scaler_arg.inputvisibleheight=480;
	     scaler_arg.InputWidth=640;
	     scaler_arg.InputHeight=480;
	     c_zoom_tmp = 65536/(0.5*c_xxx_zoom);
	     scaler_arg.outputwidthfactor=(int)c_zoom_tmp;
	     scaler_arg.outputheightfactor=(int)c_zoom_tmp; 
	     c_zoom_tmp = 1-(1/c_xxx_zoom);
	     scaler_arg.inputoffsetx=(int)((float)320*c_zoom_tmp)<<16;
	     scaler_arg.inputoffsety=(int)((float)240*c_zoom_tmp)<<16;
	     scaler_arg.OutputBufWidth=320;
	     scaler_arg.OutputBufHeight=240;
	     scaler_arg.OutputWidth=320;
	     scaler_arg.OutputHeight=240;
	     scaler_arg.OutBoundaryColor=(INT32U)0x008080;
	     scaler_arg.InputFormat=SCALER_INPUT_FORMAT_YUYV;
         scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUYV;
	     image_scale_start(scaler_arg);
	     while (1){
		    scale_state=image_scale_status();
		    if (scale_state==IMAGE_CODEC_DECODE_END) {
               tv_display_flag=1;
               scaler_task_state=1;
               break;
		    }else{
		       DBG_PRINT("auto_switch_csi_frame failed\r\n");
               break;
		    } 
	     }	           
      }	
    OSTimeDly(1);
   } 
}
