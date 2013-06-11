#include "avi_encoder_state.h"
 
#define C_AVI_ENCODE_STATE_STACK_SIZE   512
#define AVI_ENCODE_QUEUE_MAX_LEN    	5

/* OS stack */
INT32U AVIEncodeStateStack[C_AVI_ENCODE_STATE_STACK_SIZE];

/* global varaible */
OS_EVENT *AVIEncodeApQ;
OS_EVENT *avi_encode_ack_m;
void *AVIEncodeApQ_Stack[AVI_ENCODE_QUEUE_MAX_LEN];

/* function point */
INT32S (*pfn_avi_encode_put_data)(void *workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag);

//================================================================================================================== 
INT32U avi_enc_packer_start(AviEncPacker_t *pAviEncPacker)
{
	INT32S nRet; 
	INT32U bflag;
	
	if(pAviEncPacker == pAviEncPacker0) {
		bflag = C_AVI_ENCODE_PACKER0;
		pAviEncPacker->task_prio = AVI_PACKER0_PRIORITY;
	} else if(pAviEncPacker == pAviEncPacker1) {
		bflag = C_AVI_ENCODE_PACKER1;
		pAviEncPacker->task_prio = AVI_PACKER1_PRIORITY;
	} else {
		RETURN(STATUS_FAIL);
	}
	
	nRet = STATUS_OK;
	if((avi_encode_get_status() & bflag) == 0) {
		switch(pAviEncPara->source_type)
		{
		case SOURCE_TYPE_FS:
			avi_encode_set_avi_header(pAviEncPacker);
			nRet = AviPackerV3_Open(pAviEncPacker->avi_workmem,
									pAviEncPacker->file_handle, 
									pAviEncPacker->index_handle,
									pAviEncPacker->p_avi_vid_stream_header,
									pAviEncPacker->bitmap_info_cblen,
									pAviEncPacker->p_avi_bitmap_info,
									pAviEncPacker->p_avi_aud_stream_header,
									pAviEncPacker->wave_info_cblen,
									pAviEncPacker->p_avi_wave_info, 
									pAviEncPacker->task_prio,
									pAviEncPacker->file_write_buffer, 
									pAviEncPacker->file_buffer_size, 
									pAviEncPacker->index_write_buffer, 
									pAviEncPacker->index_buffer_size);
			AviPackerV3_SetErrHandler(pAviEncPacker->avi_workmem, avi_packer_err_handle);
			pfn_avi_encode_put_data = AviPackerV3_PutData;
			break;
		case SOURCE_TYPE_USER_DEFINE:
			pAviEncPacker->p_avi_wave_info = pAviEncPacker->avi_workmem;
			pfn_avi_encode_put_data = video_encode_frame_ready;
			break;
		}		
		avi_encode_set_status(bflag);
		DEBUG_MSG(DBG_PRINT("a.AviPackerOpen[0x%x] = 0x%x\r\n", bflag, nRet));
	} else {
		RETURN(STATUS_FAIL);
	}
Return:	
	return nRet;
}

INT32U avi_enc_packer_stop(AviEncPacker_t *pAviEncPacker)
{
	INT32S nRet;
	INT32U bflag;
	
	if(pAviEncPacker == pAviEncPacker0) {
		bflag = C_AVI_ENCODE_PACKER0;
	} else if(pAviEncPacker == pAviEncPacker1) {
		bflag = C_AVI_ENCODE_PACKER1;
	} else {
		RETURN(STATUS_FAIL);
	}
	
	if(avi_encode_get_status() & bflag) {
		 avi_encode_clear_status(bflag);
		if(avi_encode_get_status() & C_AVI_ENCODE_ERR) {
			avi_encode_clear_status(C_AVI_ENCODE_ERR);
			switch(pAviEncPara->source_type) 
			{
			case SOURCE_TYPE_FS:
				nRet = AviPackerV3_Close(pAviEncPacker->avi_workmem);
				avi_encode_fail_handle_close_file(pAviEncPacker);
				break;
			case SOURCE_TYPE_USER_DEFINE:
	        	nRet = STATUS_OK;
	        	break;
	        } 
        } else {
       		switch(pAviEncPara->source_type)
	        {
			case SOURCE_TYPE_FS:
				video_encode_end(pAviEncPacker->avi_workmem);
	           	nRet = AviPackerV3_Close(pAviEncPacker->avi_workmem); 
	           	avi_encode_close_file(pAviEncPacker);
	        	break;		
	        case SOURCE_TYPE_USER_DEFINE:
	        	nRet = STATUS_OK;
	        	break;
	        } 
       	}
       	
		if(nRet < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("c.AviPackerClose[0x%x] = 0x%x\r\n", bflag, nRet)); 
	}
	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S vid_enc_preview_start(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	//alloc memory
	if(avi_encode_memory_alloc() < 0) {
		avi_encode_memory_free();
		DEBUG_MSG(DBG_PRINT("avi memory alloc fail!!!\r\n"));
		RETURN(STATUS_FAIL);				
	}
	
	if(avi_packer_memory_alloc() < 0) {
		avi_packer_memory_free();
		DEBUG_MSG(DBG_PRINT("avi packer memory alloc fail!!!\r\n"));
		RETURN(STATUS_FAIL);				
	}
	
	// start scaler
	if((avi_encode_get_status()&C_AVI_ENCODE_SCALER) == 0) {
		if(scaler_task_start() < 0) RETURN(STATUS_FAIL);
		avi_encode_set_status(C_AVI_ENCODE_SCALER);
		DEBUG_MSG(DBG_PRINT("a.scaler start\r\n")); 
	}
	// start video
#if AVI_ENCODE_VIDEO_ENCODE_EN == 1 
	if((avi_encode_get_status()&C_AVI_ENCODE_VIDEO) == 0) {	
		if(video_encode_task_start() < 0) RETURN(STATUS_FAIL);
		avi_encode_set_status(C_AVI_ENCODE_VIDEO);
		DEBUG_MSG(DBG_PRINT("b.video start\r\n"));
	}
#endif
#if AVI_ENCODE_PRE_ENCODE_EN == 1
	if((avi_encode_get_status() & C_AVI_ENCODE_PRE_START) == 0) {
		video_encode_task_post_q(0);	
		avi_encode_set_status(C_AVI_ENCODE_PRE_START); 
	}
#endif
	// start sensor
	if((avi_encode_get_status()&C_AVI_ENCODE_SENSOR) == 0) {
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_START_SENSOR, avi_encode_ack_m, 5000, msg, err);	
		avi_encode_set_status(C_AVI_ENCODE_SENSOR);
		DEBUG_MSG(DBG_PRINT("c.sensor start\r\n")); 
	}
	
#if AVI_ENCODE_PRE_ENCODE_EN == 1
	// start audio 
	if(pAviEncAudPara->audio_format && ((avi_encode_get_status()&C_AVI_ENCODE_AUDIO) == 0)) {
		if(avi_audio_record_start() < 0) RETURN(STATUS_FAIL);
		avi_encode_set_status(C_AVI_ENCODE_AUDIO);
		avi_encode_audio_timer_start();
		DEBUG_MSG(DBG_PRINT("d.audio start\r\n"));
	}
#endif	
Return:	
	return nRet;
}

INT32S vid_enc_preview_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	// stop audio	
	if(avi_encode_get_status()&C_AVI_ENCODE_AUDIO)
	{
		avi_encode_clear_status(C_AVI_ENCODE_AUDIO);
		nRet = avi_audio_record_stop();
		avi_encode_audio_timer_stop();
		if(nRet < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("a.audio stop\r\n"));
	}
	// stop sensor
	if(avi_encode_get_status()&C_AVI_ENCODE_SENSOR)
	{
		avi_encode_clear_status(C_AVI_ENCODE_SENSOR);
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_STOP_SENSOR, avi_encode_ack_m, 5000, msg, err);	
		DEBUG_MSG(DBG_PRINT("b.sensor stop\r\n"));
	}	
	// stop video
	if(avi_encode_get_status()&C_AVI_ENCODE_VIDEO)
	{	
		avi_encode_clear_status(C_AVI_ENCODE_VIDEO);
		if(video_encode_task_stop() < 0) {
			RETURN(STATUS_FAIL);
		}	
		DEBUG_MSG(DBG_PRINT("c.video stop\r\n"));
	}
	// stop scaler
	if(avi_encode_get_status()&C_AVI_ENCODE_SCALER)
	{
		avi_encode_clear_status(C_AVI_ENCODE_SCALER);
		if(scaler_task_stop() < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("d.scaler stop\r\n"));  
	}
Return:	
	//free memory
	avi_encode_memory_free();
	avi_packer_memory_free();
	DEBUG_MSG(DBG_PRINT("e.free memory\r\n"));  
	return nRet;
}

INT32S avi_enc_start(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	// start audio 
	if(pAviEncAudPara->audio_format && ((avi_encode_get_status()&C_AVI_ENCODE_AUDIO) == 0)) {
		if(avi_audio_record_start() < 0) {
			RETURN(STATUS_FAIL);
		}
		avi_encode_set_status(C_AVI_ENCODE_AUDIO);
		DEBUG_MSG(DBG_PRINT("b.audio start\r\n"));
	}
	// restart audio 
#if AVI_ENCODE_PRE_ENCODE_EN == 1
	if(pAviEncAudPara->audio_format && (avi_encode_get_status()&C_AVI_ENCODE_AUDIO)) {
		if(avi_audio_record_restart() < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("b.audio restart\r\n"));
	}
#endif
	// start avi encode
	if((avi_encode_get_status()&C_AVI_ENCODE_START) == 0) {
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_START_ENCODE, avi_encode_ack_m, 5000, msg, err);	
		avi_encode_set_status(C_AVI_ENCODE_START);
		avi_encode_set_status(C_AVI_ENCODE_PRE_START);
		DEBUG_MSG(DBG_PRINT("c.encode start\r\n")); 
	}
Return:	
	return nRet;
}

INT32S avi_enc_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
#if AVI_ENCODE_PRE_ENCODE_EN == 0	
	// stop audio
	if(avi_encode_get_status()&C_AVI_ENCODE_AUDIO) {
		avi_encode_clear_status(C_AVI_ENCODE_AUDIO);
		if(avi_audio_record_stop() < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("a.audio stop\r\n"));
	}
#endif
	// stop avi encode
	if(avi_encode_get_status()&C_AVI_ENCODE_START) {
		avi_encode_clear_status(C_AVI_ENCODE_START);	
	#if AVI_ENCODE_PRE_ENCODE_EN == 0			
		avi_encode_clear_status(C_AVI_ENCODE_PRE_START); 
	#endif	
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_STOP_ENCODE, avi_encode_ack_m, 5000, msg, err);	
		DEBUG_MSG(DBG_PRINT("b.encode stop\r\n")); 
	}
	//empty vid_enc_frame_q and post to video_frame_q.
	video_encode_task_empty_q();
Return:	
	return nRet;
}

INT32S avi_enc_pause(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	if(avi_encode_get_status()&C_AVI_ENCODE_START) {
    	POST_MESSAGE(AVIEncodeApQ, MSG_AVI_PAUSE_ENCODE, avi_encode_ack_m, 5000, msg, err);	
    	avi_encode_set_status(C_AVI_ENCODE_PAUSE);
		DEBUG_MSG(DBG_PRINT("encode pause\r\n")); 
    }
Return:    
	return nRet;
}

INT32S avi_enc_resume(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	if(avi_encode_get_status()&(C_AVI_ENCODE_START|C_AVI_ENCODE_PAUSE)) {
    	POST_MESSAGE(AVIEncodeApQ, MSG_AVI_RESUME_ENCODE, avi_encode_ack_m, 5000, msg, err);	
    	avi_encode_clear_status(C_AVI_ENCODE_PAUSE);
		DEBUG_MSG(DBG_PRINT("encode resume\r\n"));  
    }
Return:    
	return nRet;
}

INT32S avi_enc_save_jpeg(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	if((avi_encode_get_status() & C_AVI_ENCODE_PRE_START) == 0) {
		video_encode_task_post_q(0);
		avi_encode_set_status(C_AVI_ENCODE_PRE_START);
	}  
	avi_encode_set_status(C_AVI_ENCODE_JPEG);
	POST_MESSAGE(AVIEncodeApQ, MSG_AVI_CAPTURE_PICTURE, avi_encode_ack_m, 5000, msg, err);		
Return: 
	//empty vid_enc_frame_q and post to video_frame_q.
	video_encode_task_empty_q();
#if AVI_ENCODE_PRE_ENCODE_EN == 0   
	avi_encode_clear_status(C_AVI_ENCODE_PRE_START);
	avi_encode_clear_status(C_AVI_ENCODE_JPEG);  
#endif	
	return nRet;
}

#if C_UVC == CUSTOM_ON
INT32S usb_webcam_start(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
	//alloc memory
	if(avi_encode_memory_alloc() < 0) {
		avi_encode_memory_free();
		DEBUG_MSG(DBG_PRINT("avi memory alloc fail!!!\r\n"));
		RETURN(STATUS_FAIL);				
	}
	
	// start scaler
	if((avi_encode_get_status()&C_AVI_ENCODE_SCALER) == 0) {
		if(scaler_task_start() < 0) {
			RETURN(STATUS_FAIL);
		}
		avi_encode_set_status(C_AVI_ENCODE_SCALER);
		DEBUG_MSG(DBG_PRINT("a.scaler start\r\n")); 
	}
		
	// start video
	if((avi_encode_get_status()&C_AVI_ENCODE_VIDEO) == 0) {	
		if(video_encode_task_start() < 0) RETURN(STATUS_FAIL);
		avi_encode_set_status(C_AVI_ENCODE_VIDEO);
		DEBUG_MSG(DBG_PRINT("b.video start\r\n"));
	}
	
	// start sensor
	if((avi_encode_get_status()&C_AVI_ENCODE_SENSOR) == 0) {
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_START_SENSOR, avi_encode_ack_m, 5000, msg, err);	
		avi_encode_set_status(C_AVI_ENCODE_SENSOR);
		DEBUG_MSG(DBG_PRINT("c.sensor start\r\n")); 
	}

	//usb web cam
	if((avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM) == 0) {
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_START_USB_WEBCAM, avi_encode_ack_m, 5000, msg, err);
		avi_encode_set_status(C_AVI_ENCODE_USB_WEBCAM);
		DEBUG_MSG(DBG_PRINT("d.usb web cam\r\n"));
	}
	
#if C_USB_AUDIO == CUSTOM_ON	
	// start audio 
	if(pAviEncAudPara->audio_format && ((avi_encode_get_status()&C_AVI_ENCODE_AUDIO) == 0)) {
		if(avi_audio_record_start() < 0) {
			RETURN(STATUS_FAIL);	
		}
		avi_encode_set_status(C_AVI_ENCODE_AUDIO);
		avi_encode_audio_timer_start();
		DEBUG_MSG(DBG_PRINT("e.audio start\r\n"));
	}
#endif
	
Return:    
	return nRet;
}

INT32S usb_webcam_stop(void)
{
	INT8U  err;
	INT32S nRet, msg;
	
	nRet = STATUS_OK;
#if C_USB_AUDIO == CUSTOM_ON	
	// stop audio	
	if(avi_encode_get_status()&C_AVI_ENCODE_AUDIO) {
		avi_encode_clear_status(C_AVI_ENCODE_AUDIO);
		nRet = avi_audio_record_stop();
		avi_encode_audio_timer_stop();
		if(nRet < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("a.audio stop\r\n"));
	}
#endif
	// stop sensor
	if(avi_encode_get_status()&C_AVI_ENCODE_SENSOR) {
		avi_encode_clear_status(C_AVI_ENCODE_SENSOR);
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_STOP_SENSOR, avi_encode_ack_m, 5000, msg, err);	
		DEBUG_MSG(DBG_PRINT("b.sensor stop\r\n"));
	}	
	
	// stop video
	if(avi_encode_get_status()&C_AVI_ENCODE_VIDEO) {	
		avi_encode_clear_status(C_AVI_ENCODE_VIDEO);
		if(video_encode_task_stop() < 0) {
			RETURN(STATUS_FAIL);	
		}
		DEBUG_MSG(DBG_PRINT("c.video stop\r\n"));
	}
	
	// stop scaler
	if(avi_encode_get_status()&C_AVI_ENCODE_SCALER) {
		avi_encode_clear_status(C_AVI_ENCODE_SCALER);
		if(scaler_task_stop() < 0) {
			RETURN(STATUS_FAIL);
		}
		DEBUG_MSG(DBG_PRINT("d.scaler stop\r\n"));  
	}
	
	//usb web cam
	if(avi_encode_get_status()&C_AVI_ENCODE_USB_WEBCAM) {
		avi_encode_clear_status(C_AVI_ENCODE_USB_WEBCAM);
		POST_MESSAGE(AVIEncodeApQ, MSG_AVI_STOP_USB_WEBCAM, avi_encode_ack_m, 5000, msg, err);
		DEBUG_MSG(DBG_PRINT("e.usb webcam stop\r\n"));  
	}
	
Return:	
	//free memory
	avi_encode_memory_free();
	DEBUG_MSG(DBG_PRINT("f.free memory\r\n"));  
	return nRet;
}
#endif
//======================================================================================================
INT32S avi_encode_state_task_create(INT8U pori)
{
	INT8U err;
	INT32S nRet;
	
	AVIEncodeApQ = OSQCreate(AVIEncodeApQ_Stack, AVI_ENCODE_QUEUE_MAX_LEN);
    if(!AVIEncodeApQ) {
    	RETURN(STATUS_FAIL);
    }
    
    avi_encode_ack_m = OSMboxCreate(NULL);
	if(!avi_encode_ack_m) {
		RETURN(STATUS_FAIL);
	}
	
	err = OSTaskCreate(avi_encode_state_task_entry, (void*) NULL, &AVIEncodeStateStack[C_AVI_ENCODE_STATE_STACK_SIZE - 1], pori);
	if(err != OS_NO_ERR) {
		RETURN(STATUS_FAIL);
	}
    nRet = STATUS_OK;
Return:
    return nRet;
}

INT32S avi_encode_state_task_del(void)
{
    INT8U   err;
    INT32S  nRet, msg;
   
    nRet = STATUS_OK; 
 	POST_MESSAGE(AVIEncodeApQ, MSG_AVI_ENCODE_STATE_EXIT, avi_encode_ack_m, 5000, msg, err);	
 Return:	
   	OSQFlush(AVIEncodeApQ);
   	OSQDel(AVIEncodeApQ, OS_DEL_ALWAYS, &err);
	OSMboxDel(avi_encode_ack_m, OS_DEL_ALWAYS, &err);
	AVIEncodeApQ = NULL;
    return nRet;
}

INT32S avi_enc_storage_full(void)
{
	DEBUG_MSG(DBG_PRINT("avi encode storage full!!!\r\n"));
	if(OSQPost(AVIEncodeApQ, (void*)MSG_AVI_STORAGE_FULL) != OS_NO_ERR) {
		return STATUS_FAIL;
	}
	return STATUS_OK;
} 

INT32S avi_packer_err_handle(INT32S ErrCode)
{
	DEBUG_MSG(DBG_PRINT("AviPacker-ErrID = 0x%x!!!\r\n", ErrCode));
	avi_encode_set_status(C_AVI_ENCODE_ERR);	
#if 1
	/* close avi packer task automatic */
	OSQPost(AVIEncodeApQ, (void*)MSG_AVI_STORAGE_ERR);
	return 1;
#else
	/* cloase avipack task by user */
	return 0;
#endif			
}

void avi_encode_state_task_entry(void *para)
{
	INT8U   err, success_flag, key_flag;
    INT32S  nRet;
    INT32U  msg_id, video_frame, video_stream, encode_size;
    INT64S  dwtemp;	
    VidEncFrame_t *pVideo;	
	OS_CPU_SR cpu_sr;
	
    while(1)
    {      
        msg_id = (INT32U) OSQPend(AVIEncodeApQ, 0, &err);
        if((!msg_id) || (err != OS_NO_ERR))
        	continue;
        	
        switch(msg_id)
        {
        case MSG_AVI_START_SENSOR:	//sensor
          	OSQFlush(cmos_frame_q);
        #if VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FRAME_MODE
        	video_frame = pAviEncVidPara->csi_frame_addr[0];
			video_stream = 0;
       		for(nRet = 1; nRet<AVI_ENCODE_CSI_BUFFER_NO; nRet++) {
	            OSQPost(cmos_frame_q, (void *) pAviEncVidPara->csi_frame_addr[nRet]);
	        }    
		#elif VIDEO_ENCODE_MODE == C_VIDEO_ENCODE_FIFO_MODE
			video_frame = avi_encode_get_csi_frame();
			video_stream = avi_encode_get_csi_frame();
			for(nRet = 0; nRet<pAviEncPara->vid_post_cnt; nRet++) {
				pAviEncVidPara->csi_fifo_addr[nRet] = avi_encode_get_csi_frame();		
			}		
		#endif
	        nRet = video_encode_sensor_start(pAviEncVidPara->sensor_capture_width, 
	        								pAviEncVidPara->sensor_capture_height,
	        								video_frame, 
	        								video_stream);
	        if(nRet >= 0)
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
        	else
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_FAIL);
            break;
        
        case MSG_AVI_STOP_SENSOR: 
            nRet = video_encode_sensor_stop();
            OSQFlush(cmos_frame_q);
            if(nRet >= 0)
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
        	else
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_FAIL);
            break;   
#if C_UVC == CUSTOM_ON            
        case MSG_AVI_START_USB_WEBCAM:	//sensor  
        	nRet = video_encode_task_post_q(1);	    
	        if(nRet >= 0)
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
        	else
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_FAIL);
            break;
              
        case MSG_AVI_STOP_USB_WEBCAM: 
            OSQFlush(usbwebcam_frame_q);
            if(nRet >= 0)
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
        	else
            	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_FAIL);
            break;
#endif             
        case MSG_AVI_START_ENCODE: //avi encode start
        	if((avi_encode_get_status()&C_AVI_ENCODE_PRE_START) == 0) {
				nRet = video_encode_task_post_q(0);	
        	}
        	
			pAviEncPara->ta = 0;
			pAviEncPara->tv = 0;
			pAviEncPara->Tv = 0;
			pAviEncPara->pend_cnt = 0;
			pAviEncPara->post_cnt = 0;
			for(nRet=0; nRet<AVI_ENCODE_VIDEO_BUFFER_NO; nRet++) {
				pAviEncPara->video[nRet].ready_frame = 0;
				pAviEncPara->video[nRet].encode_size = 0;
				pAviEncPara->video[nRet].key_flag = 0;
			}
			
			if(pAviEncPara->AviPackerCur->p_avi_wave_info)
			{
				pAviEncPara->freq_div = pAviEncAudPara->audio_sample_rate/AVI_ENCODE_TIME_BASE;
				pAviEncPara->tick = (INT64S)pAviEncVidPara->dwRate * pAviEncPara->freq_div;
				pAviEncPara->delta_Tv = pAviEncVidPara->dwScale * pAviEncAudPara->audio_sample_rate;
			}
			else
			{
				pAviEncPara->freq_div = 1;
				pAviEncPara->tick = (INT64S)pAviEncVidPara->dwRate * pAviEncPara->freq_div;
				pAviEncPara->delta_Tv = pAviEncVidPara->dwScale * AVI_ENCODE_TIME_BASE;
			}
		case MSG_AVI_RESUME_ENCODE:
		#if AVI_ENCODE_PRE_ENCODE_EN == 0
			if(pAviEncPara->AviPackerCur->p_avi_wave_info) 
				avi_encode_audio_timer_start();
        #endif	
        	avi_encode_video_timer_start();
            OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
            break;
            
        case MSG_AVI_STOP_ENCODE:  
        case MSG_AVI_PAUSE_ENCODE:
        #if AVI_ENCODE_PRE_ENCODE_EN == 0
        	if(pAviEncPara->AviPackerCur->p_avi_wave_info) 
        		avi_encode_audio_timer_stop();
        #endif	
        	avi_encode_video_timer_stop();
            OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
            break;   
        
        case MSG_AVI_CAPTURE_PICTURE:
        	do {
        		video_frame = avi_encode_get_empty(vid_enc_frame_q);
				pVideo = (VidEncFrame_t *)video_frame;
			} while(video_frame == 0);
			
			nRet = save_jpeg_file(pAviEncPara->AviPackerCur->file_handle, 
								pVideo->ready_frame, 
								pVideo->encode_size);					
			avi_encode_post_empty(video_frame_q, pVideo->ready_frame);
			if(nRet < 0) {
				goto AVI_CAPTURE_PICTURE_FAIL;
            }
            OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS);
       		break;
AVI_CAPTURE_PICTURE_FAIL: 
        	OSMboxPost(avi_encode_ack_m, (void*)C_ACK_FAIL);
        	break;

       	case MSG_AVI_STORAGE_ERR:
       	case MSG_AVI_STORAGE_FULL:
       	#if AVI_ENCODE_PRE_ENCODE_EN == 0
       		// stop audio	
			if(avi_encode_get_status()&C_AVI_ENCODE_AUDIO)
			{	
				avi_encode_clear_status(C_AVI_ENCODE_AUDIO);
				avi_audio_record_stop();
			}
		#endif
			//stop avi encode timer
			if(avi_encode_get_status()&C_AVI_ENCODE_START)
			{
				avi_encode_clear_status(C_AVI_ENCODE_START);	
			#if AVI_ENCODE_PRE_ENCODE_EN == 0			
				avi_encode_clear_status(C_AVI_ENCODE_PRE_START); 
			#endif	
			#if AVI_ENCODE_PRE_ENCODE_EN == 0
	        	if(pAviEncPara->AviPackerCur->p_avi_wave_info) 
	        		avi_encode_audio_timer_stop();
	        #endif	
	        	avi_encode_video_timer_stop();
			}
       		//close avi packer
       		avi_enc_packer_stop(pAviEncPara->AviPackerCur);
            OSQFlush(AVIEncodeApQ);
            OSMboxAccept(avi_encode_ack_m);
            break;
            
       	case MSG_AVI_ENCODE_STATE_EXIT:
       		OSMboxPost(avi_encode_ack_m, (void*)C_ACK_SUCCESS); 
   			OSTaskDel(OS_PRIO_SELF);
       		break;
       	
       	case MSG_AVI_ONE_FRAME_ENCODE:
        	success_flag = 0;
        	OS_ENTER_CRITICAL();
			dwtemp = (INT64S)(pAviEncPara->tv - pAviEncPara->Tv);
			OS_EXIT_CRITICAL();
			if(dwtemp > (pAviEncPara->delta_Tv << 2)) {
				goto EncodeNullFrame;
			}
							
			video_frame = avi_encode_get_empty(vid_enc_frame_q);
			if(video_frame == 0) {
				//DEBUG_MSG(DBG_PRINT("one frame = 0x%x\r\n", video_frame));
				goto EndofEncodeOneFrame;
			}
			
			pVideo = (VidEncFrame_t *)video_frame;
			if(pVideo->encode_size == 0) {
				DEBUG_MSG(DBG_PRINT("encode_size = 0x%x\r\n", pVideo->encode_size));
				avi_encode_post_empty(video_frame_q, pVideo->ready_frame);
				goto EndofEncodeOneFrame;
			}
			
			video_stream = pVideo->encode_size + 8 + 2*16;
			if(!avi_encode_disk_size_is_enough(video_stream)) {
				avi_encode_post_empty(video_frame_q, pVideo->ready_frame);
				avi_enc_storage_full();
				goto EndofEncodeOneFrame;
			}
			video_frame = pVideo->ready_frame;
			encode_size = pVideo->encode_size;
			key_flag = pVideo->key_flag;
			nRet = pfn_avi_encode_put_data(	pAviEncPara->AviPackerCur->avi_workmem,
											*(long*)"00dc", 
											encode_size, 
											(void *)video_frame, 
											1, 
											key_flag);
			pVideo->encode_size = 0;
			pVideo->key_flag = 0;
			avi_encode_post_empty(video_frame_q, pVideo->ready_frame);
			if(nRet >= 0) {	
				DEBUG_MSG(DBG_PRINT("."));
				success_flag = 1;
				goto EndofEncodeOneFrame;
			} else { 	
				avi_encode_disk_size_is_enough(-video_stream);
				DEBUG_MSG(DBG_PRINT("VidPutData = %x, size = %d !!!\r\n", nRet-0x80000000, encode_size));
			}
EncodeNullFrame:
			avi_encode_disk_size_is_enough(8+2*16);
			nRet = pfn_avi_encode_put_data(	pAviEncPara->AviPackerCur->avi_workmem,
											*(long*)"00dc", 
											0, 
											(void *)NULL, 
											1, 
											0x00);
			if(nRet >= 0) {
				DEBUG_MSG(DBG_PRINT("N"));
				success_flag = 1;
			} else {
				avi_encode_disk_size_is_enough(-(8+2*16));
				DEBUG_MSG(DBG_PRINT("VidPutDataNULL = %x!!!\r\n", nRet-0x80000000));
			}
			
EndofEncodeOneFrame:
			if(success_flag) {
				OS_ENTER_CRITICAL();
				pAviEncPara->Tv += pAviEncPara->delta_Tv;
				OS_EXIT_CRITICAL();
			}
			pAviEncPara->pend_cnt++;
        	break;	
        } 
    }
}
