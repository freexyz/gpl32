#include "avi_encoder_app.h"

#if MPEG4_ENCODE_ENABLE == 1
#include "drv_l1_mpeg4.h"
#endif

/* os task stack size */
#if FACE_DETECTION_MODE == 1
#define C_SCALER_STACK_SIZE			8192
#else
#define C_SCALER_STACK_SIZE			512
#endif
#define	C_JPEG_STACK_SIZE			512
#define C_SCALER_QUEUE_MAX			5
#define C_JPEG_QUEUE_MAX			5

/* os tsak stack */
INT32U	ScalerTaskStack[C_SCALER_STACK_SIZE];
INT32U	JpegTaskStack[C_JPEG_STACK_SIZE];

/* os task queue */
OS_EVENT *scaler_task_q;
OS_EVENT *scaler_task_ack_m;
OS_EVENT *scaler_frame_q;  
OS_EVENT *display_frame_q;
OS_EVENT *cmos_frame_q;
OS_EVENT *vid_enc_task_q;
OS_EVENT *vid_enc_frame_q;
OS_EVENT *video_frame_q;
OS_EVENT *vid_enc_task_ack_m;
OS_EVENT *scaler_hw_sem;
void *scaler_task_q_stack[C_SCALER_QUEUE_MAX];
void *scaler_frame_q_stack[AVI_ENCODE_SCALER_BUFFER_NO];
void *display_frame_q_stack[AVI_ENCODE_DISPALY_BUFFER_NO];
void *cmos_frame_q_stack[AVI_ENCODE_CSI_BUFFER_NO];
void *video_encode_task_q_stack[C_JPEG_QUEUE_MAX];
void *video_encode_frame_q_stack[AVI_ENCODE_VIDEO_BUFFER_NO];
void *video_frame_q_stack[AVI_ENCODE_VIDEO_BUFFER_NO];
#if C_UVC == CUSTOM_ON
OS_EVENT *usbwebcam_frame_q;
void *usbwebcam_frame_q_stack[AVI_ENCODE_VIDEO_BUFFER_NO];
extern volatile char PicsToggle;
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////
// scaler task
///////////////////////////////////////////////////////////////////////////////////////////////////////
INT32S scaler_task_create(INT8U pori)
{
	INT8U  err;
	INT32S nRet;
	
	scaler_task_q = OSQCreate(scaler_task_q_stack, C_SCALER_QUEUE_MAX);
	if(!scaler_task_q) RETURN(STATUS_FAIL);
	
	scaler_task_ack_m = OSMboxCreate(NULL);
	if(!scaler_task_ack_m) RETURN(STATUS_FAIL);
	
	scaler_frame_q = OSQCreate(scaler_frame_q_stack, AVI_ENCODE_SCALER_BUFFER_NO);
	if(!scaler_frame_q) RETURN(STATUS_FAIL);
	
	display_frame_q = OSQCreate(display_frame_q_stack, AVI_ENCODE_DISPALY_BUFFER_NO);
	if(!display_frame_q) RETURN(STATUS_FAIL);
	
	cmos_frame_q = OSQCreate(cmos_frame_q_stack, AVI_ENCODE_CSI_BUFFER_NO);
	if(!cmos_frame_q) RETURN(STATUS_FAIL);	
	
	scaler_hw_sem = OSSemCreate(1);
	if(!scaler_hw_sem) RETURN(STATUS_FAIL);
	
	err = OSTaskCreate(scaler_task_entry, (void *)NULL, &ScalerTaskStack[C_SCALER_STACK_SIZE - 1], pori);	
	if(err != OS_NO_ERR) RETURN(STATUS_FAIL);
	
	nRet = STATUS_OK;
Return:
	return nRet;
}

INT32S scaler_task_del(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	POST_MESSAGE(scaler_task_q, MSG_SCALER_TASK_EXIT, scaler_task_ack_m, 5000, msg, err);
Return:	
	OSQFlush(scaler_task_q);
   	OSQDel(scaler_task_q, OS_DEL_ALWAYS, &err);
	scaler_task_q = NULL;
	
   	OSQFlush(scaler_frame_q);
   	OSQDel(scaler_frame_q, OS_DEL_ALWAYS, &err);
   	scaler_frame_q = NULL;
   	
   	OSQFlush(display_frame_q);
   	OSQDel(display_frame_q, OS_DEL_ALWAYS, &err);
   	display_frame_q = NULL;
   	   	
   	OSQFlush(cmos_frame_q);
   	OSQDel(cmos_frame_q, OS_DEL_ALWAYS, &err);
   	cmos_frame_q = NULL;
   	
	OSMboxDel(scaler_task_ack_m, OS_DEL_ALWAYS, &err);
	OSSemDel(scaler_hw_sem, OS_DEL_ALWAYS, &err);
	return nRet;
}

INT32S scaler_task_start(void)
{
	INT8U  err;
	INT32S i, nRet, msg;
	
	OSQFlush(scaler_frame_q);
	for(i=0; i<AVI_ENCODE_SCALER_BUFFER_NO; i++) {
		avi_encode_post_empty(scaler_frame_q, pAviEncVidPara->scaler_output_addr[i]);
	}
	
	OSQFlush(display_frame_q);
	for(i=0; i<AVI_ENCODE_DISPALY_BUFFER_NO; i++) {
		avi_encode_post_empty(display_frame_q, pAviEncVidPara->display_output_addr[i]);
	}
	
	nRet = STATUS_OK;
	POST_MESSAGE(scaler_task_q, MSG_SCALER_TASK_INIT, scaler_task_ack_m, 5000, msg, err);
Return:		
	return nRet;	
}

INT32S scaler_task_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	POST_MESSAGE(scaler_task_q, MSG_SCALER_TASK_STOP, scaler_task_ack_m, 5000, msg, err);
Return:		
	return nRet;
}

void scaler_task_entry(void *parm)
{
	INT8U err;
	INT32U msg_id;
	INT32U sensor_frame, scaler_frame, display_frame;
	ScalerPara_t scale;
#if	AVI_ENCODE_SHOW_TIME == 1
	TIME_T osd_time;
#endif
	while(1)
	{
		msg_id = (INT32U) OSQPend(scaler_task_q, 0, &err);
		if((err != OS_NO_ERR)||	!msg_id)
			continue;
			
		switch(msg_id)
		{
		case MSG_SCALER_TASK_INIT:
			display_frame = 0;
			pAviEncPara->fifo_enc_err_flag = 0;
			pAviEncPara->fifo_irq_cnt = 0; 
			pAviEncPara->vid_pend_cnt = 0;
			pAviEncPara->vid_post_cnt = 0;
			OSMboxPost(scaler_task_ack_m, (void*)C_ACK_SUCCESS);
			break;
		
		case MSG_SCALER_TASK_STOP:
			OSQFlush(scaler_task_q);
			OSMboxPost(scaler_task_ack_m, (void*)C_ACK_SUCCESS);
			break;
				
		case MSG_SCALER_TASK_EXIT:
			OSMboxPost(scaler_task_ack_m, (void*)C_ACK_SUCCESS);
			OSTaskDel(OS_PRIO_SELF);
			break;
		
		default:
			sensor_frame = msg_id;
		 	if(AVI_ENCODE_DIGITAL_ZOOM_EN == 1 || pAviEncVidPara->scaler_flag > 0) {
		 		scaler_frame = avi_encode_get_empty(scaler_frame_q);
    			if(scaler_frame == 0) {
    				DEBUG_MSG(DBG_PRINT("scaler_frame Fail!!!\r\n"));
					goto DEFAULT_END;
				}
			
			#if AVI_ENCODE_DIGITAL_ZOOM_EN == 1
				scale.scaler_mode = C_SCALER_ZOOM_FIT_BUFFER;
				scale.scaler_factor = pAviEncVidPara->scaler_zoom_ratio;
			#else		
				scale.scaler_mode = C_SCALER_FULL_SCREEN;
				scale.scaler_factor = 1;
			#endif	
				
				scale.input_format = pAviEncVidPara->sensor_output_format;
				scale.input_x = pAviEncVidPara->sensor_capture_width;
				scale.input_y = pAviEncVidPara->sensor_capture_height;
				scale.input_visible_x = 0;
				scale.input_visible_y = 0;
				scale.input_addr_y = sensor_frame;
				scale.input_addr_u = 0;
				scale.input_addr_v = 0;
				
				scale.output_format = C_SCALER_CTRL_OUT_YUYV;
				scale.output_x = pAviEncVidPara->encode_width;
				scale.output_y = pAviEncVidPara->encode_height;
				scale.output_buffer_x = pAviEncVidPara->encode_width;
				scale.output_buffer_y = pAviEncVidPara->encode_height;
				scale.output_addr_y = scaler_frame;
				scale.output_addr_u = 0;
				scale.output_addr_v = 0;
				scale.boundary_color = 0x008080;
    			scaler_zoom_once(&scale);				
    			
				avi_encode_post_empty(cmos_frame_q, sensor_frame);
			} else {
				scaler_frame = sensor_frame;
			}
			
			#if FACE_DETECTION_MODE == 0
				//post ready frame to video encode task
				OSQPost(vid_enc_task_q, (void *)scaler_frame);
			#endif
			
		#if AVI_ENCODE_PREVIEW_DISPLAY_EN == 1	
			if(pAviEncVidPara->dispaly_scaler_flag == 0) {
			#if	AVI_ENCODE_SHOW_TIME == 1
				cal_time_get(&osd_time);
				cpu_draw_time_osd(osd_time, scaler_frame, pAviEncVidPara->display_width);
			#endif
			    #if FACE_DETECTION_MODE == 1
			        INT32U temp;
					
					temp=video_encode_display_frame_ready(scaler_frame);
					if(temp!=1 && temp!=0)
					{
						//post ready frame to video encode task
						OSQPost(vid_enc_task_q, (void *)temp);					
				    }		    	
			    #else
					video_encode_display_frame_ready(scaler_frame);
				#endif
			} else {	
				display_frame = avi_encode_get_empty(display_frame_q);
				if(display_frame == 0) {
					DEBUG_MSG(DBG_PRINT("video_frame = 0\r\n"));
					goto DEFAULT_END;
				}
				
				scale.scaler_mode = C_SCALER_FULL_SCREEN;
				scale.scaler_factor = 1;
				scale.input_format = C_SCALER_CTRL_IN_YUYV;
				scale.input_x = pAviEncVidPara->encode_width;
				scale.input_y = pAviEncVidPara->encode_height;
				scale.input_visible_x = 0;
				scale.input_visible_y = 0;
				scale.input_addr_y = scaler_frame;
				scale.input_addr_u = 0;
				scale.input_addr_v = 0;
				
				scale.output_format = pAviEncVidPara->display_output_format;
				scale.output_x = pAviEncVidPara->display_width;
				scale.output_y = pAviEncVidPara->display_height;
				scale.output_buffer_x = pAviEncVidPara->display_buffer_width;
				scale.output_buffer_y = pAviEncVidPara->display_buffer_height;
				scale.output_addr_y = display_frame;
				scale.output_addr_u = 0;
				scale.output_addr_v = 0;
				scale.boundary_color = 0x008080;
    			scaler_zoom_once(&scale);
				
			#if	AVI_ENCODE_SHOW_TIME == 1
				cal_time_get(&osd_time);
				cpu_draw_time_osd(osd_time, display_frame, pAviEncVidPara->display_width);
			#endif
				video_encode_display_frame_ready(display_frame);
				avi_encode_post_empty(display_frame_q, display_frame);				
        	}
		#endif
			
		DEFAULT_END:
			break;	
		}
	}		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	video encode task
///////////////////////////////////////////////////////////////////////////////////////////////////////
INT32S video_encode_task_create(INT8U pori)
{
	INT8U  err;
	INT32S nRet;
	
	vid_enc_task_q = OSQCreate(video_encode_task_q_stack, C_JPEG_QUEUE_MAX);
	if(!scaler_task_q) RETURN(STATUS_FAIL);
	
	vid_enc_task_ack_m = OSMboxCreate(NULL);
	if(!scaler_task_ack_m) RETURN(STATUS_FAIL);
	
	vid_enc_frame_q = OSQCreate(video_encode_frame_q_stack, AVI_ENCODE_VIDEO_BUFFER_NO);
	if(!vid_enc_frame_q) RETURN(STATUS_FAIL);
	
	video_frame_q = OSQCreate(video_frame_q_stack, AVI_ENCODE_VIDEO_BUFFER_NO);
	if(!video_frame_q) RETURN(STATUS_FAIL);
	
#if C_UVC == CUSTOM_ON
    usbwebcam_frame_q = OSQCreate(usbwebcam_frame_q_stack, AVI_ENCODE_VIDEO_BUFFER_NO);
    if(!usbwebcam_frame_q)  RETURN(STATUS_FAIL);
#endif
	
	err = OSTaskCreate(video_encode_task_entry, (void *)NULL, &JpegTaskStack[C_JPEG_STACK_SIZE-1], pori); 	
	if(err != OS_NO_ERR) RETURN(STATUS_FAIL);
		
	nRet = STATUS_OK;
Return:
	return nRet;
}

INT32S video_encode_task_del(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	POST_MESSAGE(vid_enc_task_q, MSG_VIDEO_ENCODE_TASK_EXIT, vid_enc_task_ack_m, 5000, msg, err);
Return:	
	OSQFlush(vid_enc_task_q);
   	OSQDel(vid_enc_task_q, OS_DEL_ALWAYS, &err);
   	vid_enc_task_q = NULL;
   	
   	OSQFlush(vid_enc_frame_q);
   	OSQDel(vid_enc_frame_q, OS_DEL_ALWAYS, &err);
   	vid_enc_frame_q = NULL;
   	
   	OSQFlush(video_frame_q);
   	OSQDel(video_frame_q, OS_DEL_ALWAYS, &err);
   	video_frame_q = NULL;
   	
#if C_UVC == CUSTOM_ON   	
   	OSQFlush(usbwebcam_frame_q);
   	OSQDel(usbwebcam_frame_q, OS_DEL_ALWAYS, &err);
   	usbwebcam_frame_q = NULL;
#endif   	
	OSMboxDel(vid_enc_task_ack_m, OS_DEL_ALWAYS, &err);
	return nRet;
}

INT32S video_encode_task_start(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	POST_MESSAGE(vid_enc_task_q, MSG_VIDEO_ENCODE_TASK_MJPEG_INIT, vid_enc_task_ack_m, 5000, msg, err);
Return:
	return nRet;	
}

INT32S video_encode_task_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	POST_MESSAGE(vid_enc_task_q, MSG_VIDEO_ENCODE_TASK_STOP, vid_enc_task_ack_m, 5000, msg, err);
Return:
    return nRet;	
}

INT32S video_encode_task_post_q(INT32U mode)
{
	INT32U i;
	
	if(mode == 0) {
		//avi queue
		OSQFlush(vid_enc_frame_q);
    	OSQFlush(video_frame_q);
    	for(i=0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {
			OSQPost(video_frame_q, (void *) pAviEncVidPara->video_encode_addr[i]);	
		}
	} else {
		//usb webcam queue
		OSQFlush(usbwebcam_frame_q);
        for(i=0; i<AVI_ENCODE_VIDEO_BUFFER_NO; i++) {	
			OSQPost(usbwebcam_frame_q, (void *) pAviEncVidPara->video_encode_addr[i]);	
		}		
	}
	return STATUS_OK;
}

INT32S video_encode_task_empty_q(void)
{
	INT32U video_frame;
	VidEncFrame_t * pVideo;
	
	do {
		video_frame = avi_encode_get_empty(vid_enc_frame_q);
		if(video_frame) {
			pVideo = (VidEncFrame_t *)video_frame;
			avi_encode_post_empty(video_frame_q, pVideo->ready_frame);
		}
	} while(video_frame);
	return STATUS_OK;
}

void video_encode_task_entry(void *parm)
{
	INT8U   err, rCnt;
	INT32S  nRet, header_size, encode_size;
	INT32U  msg_id;
	INT32U	video_frame, scaler_frame;
	JpegPara_t jpeg;
#if MPEG4_ENCODE_ENABLE == 1
	#define MAX_P_FRAME		10
	INT8U	time_inc_bit, p_cnt;
	INT32U  temp, write_refer_addr, reference_addr; 
#endif
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
	INT8U  jpeg_start_flag;
	INT16U scaler_height; 
	INT32U input_y_len, input_uv_len, uv_length;
	INT32U y_frame, u_frame, v_frame; 
	INT32S status;
	ScalerPara_t scale;
#endif
#if	AVI_ENCODE_SHOW_TIME == 1
	TIME_T osd_time;
#endif
#if C_UVC == CUSTOM_ON
	ISOTaskMsg isosend;
#endif
	//R_IOC_DIR |= 0x0C; R_IOC_ATT |= 0x0C; R_IOC_O_DATA = 0x0;
	while(1)
	{
		msg_id = (INT32U) OSQPend(vid_enc_task_q, 0, &err);
		if(err != OS_NO_ERR) {
		    continue;
		}
		    
		switch(msg_id)
		{
		case MSG_VIDEO_ENCODE_TASK_MJPEG_INIT:
			rCnt = 0;
		#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
			pAviEncPara->vid_post_cnt = pAviEncVidPara->sensor_capture_height / SENSOR_FIFO_LINE;
			if(pAviEncVidPara->sensor_capture_height % SENSOR_FIFO_LINE)
				while(1);//this must be no remainder

			if(pAviEncPara->vid_post_cnt > AVI_ENCODE_CSI_FIFO_NO)
				while(1);//fifo buffer is not enough

			jpeg_start_flag = 0;
			scaler_height = pAviEncVidPara->encode_height / pAviEncPara->vid_post_cnt;
			uv_length = pAviEncVidPara->encode_width * (scaler_height + 2);
			input_y_len = pAviEncVidPara->encode_width * scaler_height;
			input_uv_len = input_y_len >> 1; //YUV422
			//input_uv_len = input_y_len >> 2; //YUV420	
		#endif
		#if AVI_ENCODE_VIDEO_ENCODE_EN == 1
			header_size = avi_encode_set_jpeg_quality(pAviEncVidPara->quality_value);
		#endif
			OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_SUCCESS);
			break;
		
		case MSG_VIDEO_ENCODE_TASK_MPEG4_INIT:
			nRet = STATUS_FAIL;
		#if MPEG4_ENCODE_ENABLE == 1
			encode_width = pAviVidPara->encode_width;
		 	encode_height = pAviVidPara->encode_height;
		 	yuv_sampling_mode = MP4_ENC_INPUT_FORMAT;
		 	q_value = pAviVidPara->quality_value & 0x1F;
		 	if(q_value == 0) q_value = pAviVidPara->quality_value = 0x05;
		 	nRet = avi_encode_mpeg4_malloc(encode_width, encode_height);
		 	if(nRet < 0)
		 	{
		 		DEBUG_MSG(DBG_PRINT("mpeg4 memory alloc fail!!!\r\n"));	
		 		avi_encode_mpeg4_free();
		 		OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_FAIL);
		 		continue;
		 	}
		 	
			header_size = avi_encode_set_mp4_resolution(encode_width, encode_height);
			if(header_size < 0)
			{
				avi_encode_mpeg4_free();
		 		OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_FAIL);
		 		continue;
			}
		 	
		 	write_refer_addr = pAviVidPara->write_refer_addr; 
		 	reference_addr = pAviVidPara->reference_addr;
		 	time_inc_bit = 0x0F;
		 	p_cnt = 0;
		 	mpeg4_encode_init();
		 	mpeg4_encode_isram_set(pAviEncPara->isram_addr);
		 	mpeg4_encode_config(yuv_sampling_mode, encode_width, encode_height, time_inc_bit - 1);
		 	mpeg4_encode_ip_set(FALSE, MAX_P_FRAME);
		#endif 	
			if(nRet >= 0)
		 		OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_SUCCESS);
			else
				OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_FAIL);
			break;	
				 		
		case MSG_VIDEO_ENCODE_TASK_STOP:
		#if MPEG4_ENCODE_ENABLE == 1	
			if(pAviVidPara->video_format == C_XVID_FORMAT)
				avi_encode_mpeg4_free();
		#endif
			OSQFlush(vid_enc_task_q);
			OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_SUCCESS);
			break;
			
		case MSG_VIDEO_ENCODE_TASK_EXIT:
			OSMboxPost(vid_enc_task_ack_m, (void*)C_ACK_SUCCESS);	
			OSTaskDel(OS_PRIO_SELF);
			break;
		
		default:
#if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE
			scaler_frame = msg_id;
			if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM)) {
			#if C_UVC == CUSTOM_ON
				#if MCU_VERSION <= GPL32_C
				if(PicsToggle==0) {
					video_frame = avi_encode_get_empty(usbwebcam_frame_q);
				} else {
					video_frame = 0;
				}
				#else
				video_frame = avi_encode_get_empty(usbwebcam_frame_q);
				#endif
			#else
				video_frame = 0;
			#endif //C_UVC	  	
			} else if(avi_encode_get_status()&C_AVI_ENCODE_PRE_START) {
				video_frame = avi_encode_get_empty(video_frame_q);
			} else {
				goto VIDEO_ENCODE_FRAME_MODE_END;
			}
			
			if(video_frame == 0) {
				//DEBUG_MSG(DBG_PRINT("video_frame = 0x%x\r\n", video_frame));
				goto VIDEO_ENCODE_FRAME_MODE_END;
			}
			
			if(pAviEncVidPara->video_format == C_MJPG_FORMAT) {
			#if	AVI_ENCODE_SHOW_TIME == 1
				if(pAviEncVidPara->dispaly_scaler_flag == 1) {
					cal_time_get(&osd_time);
					cpu_draw_time_osd(osd_time, scaler_frame, pAviEncVidPara->encode_width);
				}
			#endif
				jpeg.quality_value = pAviEncVidPara->quality_value;
				jpeg.input_format = C_JPEG_FORMAT_YUYV;
				jpeg.width = pAviEncVidPara->encode_width;
				jpeg.height = pAviEncVidPara->encode_height;
				jpeg.input_buffer_y = scaler_frame;
				jpeg.input_buffer_u = 0;
				jpeg.input_buffer_v = 0;
				jpeg.output_buffer = video_frame + header_size;
				encode_size = jpeg_encode_once(&jpeg);
				if(encode_size < 0) {
					DEBUG_MSG(DBG_PRINT("encode_size = 0\r\n"));
					goto VIDEO_ENCODE_FRAME_MODE_END;
				}
				
				if(pAviEncPara->avi_encode_status & C_AVI_ENCODE_START && 
					pAviEncPara->video[rCnt].key_flag == 0) {
					pAviEncPara->video[rCnt].ready_frame = video_frame;
					pAviEncPara->video[rCnt].encode_size = encode_size + header_size;
					pAviEncPara->video[rCnt].key_flag = AVIIF_KEYFRAME;
					avi_encode_post_empty(vid_enc_frame_q, (INT32U)&pAviEncPara->video[rCnt]);
					rCnt++;
					if(rCnt >= AVI_ENCODE_VIDEO_BUFFER_NO) {
						rCnt = 0;
					}
				} else if(pAviEncPara->avi_encode_status & C_AVI_ENCODE_JPEG) {
					pAviEncPara->video[rCnt].ready_frame = video_frame;
					pAviEncPara->video[rCnt].encode_size = encode_size + header_size;
					pAviEncPara->video[rCnt].key_flag = AVIIF_KEYFRAME;
					avi_encode_post_empty(vid_enc_frame_q, (INT32U)&pAviEncPara->video[rCnt]);
					//clear status
					pAviEncPara->avi_encode_status &= ~C_AVI_ENCODE_JPEG;
				} else if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM)) {
				#if C_UVC == CUSTOM_ON
					if(USBCamApQ) {
						isosend.FrameSize = encode_size + header_size;
						isosend.AddrFrame = video_frame;
						err = OSQPost(USBCamApQ, (void*)(&isosend));//usb start send data
					#if MCU_VERSION <= GPL32_C
						PicsToggle = 1;
					#endif
					} else {
						//post back to queue
						avi_encode_post_empty(usbwebcam_frame_q, video_frame);
					}
				#endif	
				} else {
					//post back to queue
					avi_encode_post_empty(video_frame_q, video_frame);
				}
				
			VIDEO_ENCODE_FRAME_MODE_END:
				//post empty buffer to scale or sensor
				if((AVI_ENCODE_DIGITAL_ZOOM_EN == 0) && (pAviEncVidPara->scaler_flag == 0)) {
					avi_encode_post_empty(cmos_frame_q, scaler_frame);
				} else {
					avi_encode_post_empty(scaler_frame_q, scaler_frame);
				}
			}
		#if MPEG4_ENCODE_ENABLE == 1
			else
			{
				temp = reference_addr;
				reference_addr = write_refer_addr;
				write_refer_addr = temp;
				mpeg4_encode_start(C_MPEG_I_FRAME, pAviEncPara->quality_value, output_frame, (INT32U)scaler_frame, 
								write_refer_addr, reference_addr);
				nRet = mpeg4_wait_idle(TRUE);
				if(nRet != 0x04)
					while(1);
			
				mpeg4_encode_stop();
				encode_size = mpeg4_encode_get_vlc_size();
				if(p_cnt == 0)
				{	//i frame
					pAviEncPara->ready_frame = video_frame;
					pAviEncPara->ready_size = encode_size + header_size;
					pAviEncPara->key_flag = AVIIF_KEYFRAME;
				}	
				else
				{	//p frame
					pAviEncPara->ready_frame = output_frame;
					pAviEncPara->ready_size = encode_size;
					pAviEncPara->key_flag = 0x00;
				}
					
				p_cnt++;
				if(p_cnt > MAX_P_FRAME)	
					p_cnt = 0;
			}
		#endif
			break;
			
#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE	
			if(msg_id & C_AVI_ENCODE_FIFO_ERR)
			{
				DEBUG_MSG(DBG_PRINT("F1"));
				goto VIDEO_ENCODE_FIFO_MODE_FAIL;
			}
			
			if(pAviEncVidPara->scaler_flag)
			{
				//R_IOC_O_DATA ^= 0x08;
				scaler_frame = msg_id & (~C_AVI_ENCODE_FRAME_END);
				y_frame = avi_encode_get_empty(scaler_frame_q);
				if(y_frame == 0) {
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				u_frame = y_frame + uv_length;   		
				v_frame = u_frame + (uv_length >> 1);
				
				scale.scaler_mode = C_SCALER_FULL_SCREEN;
				scale.scaler_factor = 1;
				scale.input_format = pAviEncVidPara->sensor_output_format;
				scale.input_x = pAviEncVidPara->sensor_capture_width;
				scale.input_y = SENSOR_FIFO_LINE;
				scale.input_visible_x = 0;
				scale.input_visible_y = 0;
				scale.input_addr_y = scaler_frame;
				scale.input_addr_u = 0;
				scale.input_addr_v = 0;
				
				scale.output_format = C_SCALER_CTRL_OUT_YUV422;
				scale.output_x = pAviEncVidPara->encode_width;
				scale.output_y = scaler_height + 2; //+2 is to fix block line
				scale.output_buffer_x = pAviEncVidPara->encode_width;
				scale.output_buffer_y = scaler_height + 2; //+2 is to fix block line
				scale.output_addr_y = y_frame;
				scale.output_addr_u = u_frame;
				scale.output_addr_v = v_frame;
				scale.boundary_color = 0x008080;
    			scaler_zoom_once(&scale);								
				//R_IOC_O_DATA ^= 0x08;
			}
			else
			{
				y_frame = msg_id & (~C_AVI_ENCODE_FRAME_END);
				u_frame = v_frame = 0;
			}

			pAviEncPara->vid_pend_cnt++;
			if(msg_id & C_AVI_ENCODE_FRAME_END)				
			{
				if(pAviEncPara->vid_pend_cnt != pAviEncPara->vid_post_cnt)
				{
					DEBUG_MSG(DBG_PRINT("F2"));
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				if(jpeg_start_flag == 0) 
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
									
				nRet = 3; //jpeg encode end
				pAviEncPara->vid_pend_cnt = 0; 
			}
			else if(pAviEncPara->vid_pend_cnt == 1)
			{
				if(jpeg_start_flag == 1)
				{
					jpeg_start_flag = 0;
					jpeg_encode_stop();
				}
				nRet = 1; //jpeg encode start
			}
			else if(pAviEncPara->vid_pend_cnt < pAviEncPara->vid_post_cnt)
			{
				if(jpeg_start_flag == 0)
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
					
				nRet = 2; //jpeg encode once		
			}
			else 
			{
				// error happen
				goto VIDEO_ENCODE_FIFO_MODE_FAIL;//while(1);
			}
			
			switch(nRet)
			{
			case 1:
				//DEBUG_MSG(DBG_PRINT("J"));
				if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM)) {
				#if C_UVC == CUSTOM_ON		
					#if MCU_VERSION <= GPL32_C
					if(PicsToggle==0){
						video_frame = avi_encode_get_empty(usbwebcam_frame_q);
					} else {
						video_frame = 0;
					}
					#else
					video_frame = avi_encode_get_empty(usbwebcam_frame_q);
					#endif
				#else
					video_frame = 0;	
				#endif	  	
				} else if(avi_encode_get_status()&C_AVI_ENCODE_PRE_START) {
					video_frame = avi_encode_get_empty(video_frame_q);
				} else {
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				if(video_frame == 0) {
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				jpeg.jpeg_status = 0;
				jpeg.wait_done = 0;
				jpeg.quality_value = pAviEncVidPara->quality_value;
				if(pAviEncVidPara->scaler_flag) {
					jpeg.input_format = C_JPEG_FORMAT_YUV_SEPARATE;
				} else {
					jpeg.input_format = C_JPEG_FORMAT_YUYV;
				}
				
				jpeg.width = pAviEncVidPara->encode_width;
				jpeg.height = pAviEncVidPara->encode_height;
				jpeg.input_buffer_y = y_frame;
				jpeg.input_buffer_u = u_frame;
				jpeg.input_buffer_v = v_frame;
				jpeg.input_y_len = input_y_len;
				jpeg.input_uv_len = input_uv_len;
				jpeg.output_buffer = video_frame + header_size;	
				status = jpeg_encode_fifo_start(&jpeg);	
				if(status < 0) {
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				jpeg_start_flag = 1;
				break;
				
			case 2:
				//DEBUG_MSG(DBG_PRINT("*"));
				jpeg.jpeg_status = status;
				jpeg.wait_done = 0;
				jpeg.input_buffer_y = y_frame;
				jpeg.input_buffer_u = u_frame;
				jpeg.input_buffer_v = v_frame;
				jpeg.input_y_len = input_y_len;
				jpeg.input_uv_len = input_uv_len;
				status = jpeg_encode_fifo_once(&jpeg);
				if(status < 0) { 
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				break;
				
			case 3:
				//DEBUG_MSG(DBG_PRINT("G\r\n"));
				jpeg.jpeg_status = status;
				jpeg.wait_done = 0;
				jpeg.input_buffer_y = y_frame;
				jpeg.input_buffer_u = u_frame;
				jpeg.input_buffer_v = v_frame;
				jpeg.input_y_len = input_y_len;
				jpeg.input_uv_len = input_uv_len;
				encode_size = jpeg_encode_fifo_stop(&jpeg);
				if(encode_size < 0) {
					goto VIDEO_ENCODE_FIFO_MODE_FAIL;
				}
				
				jpeg_start_flag = 0;
				if(pAviEncPara->avi_encode_status & C_AVI_ENCODE_START && 
					pAviEncPara->video[rCnt].key_flag == 0) {
					pAviEncPara->video[rCnt].ready_frame = video_frame;
					pAviEncPara->video[rCnt].encode_size = encode_size + header_size;
					pAviEncPara->video[rCnt].key_flag = AVIIF_KEYFRAME;
					avi_encode_post_empty(vid_enc_frame_q, (INT32U)&pAviEncPara->video[rCnt]);
					rCnt++;
					if(rCnt >= AVI_ENCODE_VIDEO_BUFFER_NO) {
						rCnt = 0;
					}
				} else if(pAviEncPara->avi_encode_status & C_AVI_ENCODE_JPEG) {
					pAviEncPara->video[rCnt].ready_frame = video_frame;
					pAviEncPara->video[rCnt].encode_size = encode_size + header_size;
					pAviEncPara->video[rCnt].key_flag = AVIIF_KEYFRAME;
					avi_encode_post_empty(vid_enc_frame_q, (INT32U)&pAviEncPara->video[rCnt]);
					//clear status
					pAviEncPara->avi_encode_status &= ~C_AVI_ENCODE_JPEG;
				} else if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM)) {
				#if C_UVC == CUSTOM_ON
					if(USBCamApQ) {
						isosend.FrameSize = encode_size + header_size;
						isosend.AddrFrame = video_frame;
						err = OSQPost(USBCamApQ, (void*)(&isosend));//usb start send data
					#if MCU_VERSION <= GPL32_C
						PicsToggle = 1;
					#endif
					} else {
						//post back to queue
						avi_encode_post_empty(usbwebcam_frame_q, video_frame);
					}
				#endif	
				} else {
					//post back to queue
					avi_encode_post_empty(video_frame_q, video_frame);
				}
				break;
			}
			
			if(pAviEncVidPara->scaler_flag) {
				avi_encode_post_empty(scaler_frame_q, y_frame);
			}
			break;

VIDEO_ENCODE_FIFO_MODE_FAIL:
			pAviEncPara->vid_pend_cnt = 0;
			if(jpeg_start_flag)
			{
				jpeg_start_flag = 0;
				jpeg_encode_stop();
			}
			//post back to queue
			avi_encode_post_empty(video_frame_q, video_frame);
#endif
		break;
		}
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
