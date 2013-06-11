#include "turnkey_image_task.h"

#define IMAGE_QUEUE_MAX_NUMS   				128
#define IMAGE_MAX_MSG_SIZE 					0x40
#define C_IMAGE_DECODE_FILE_QUEUE_SIZE	    8

MSG_Q_ID image_msg_queue_id = NULL;
static INT32U image_local_queue[IMAGE_QUEUE_MAX_NUMS][1 + (IMAGE_MAX_MSG_SIZE+3)/4];
static INT16U image_q_read, image_q_write;

static void *fs_fifo_buffer_a[C_IMAGE_DECODE_FILE_QUEUE_SIZE];
static void *fs_fifo_buffer_b[C_IMAGE_DECODE_FILE_QUEUE_SIZE];
OS_EVENT *image_task_fs_queue_a = NULL;
OS_EVENT *image_task_fs_queue_b = NULL;

static INT32U image_task_para[(IMAGE_MAX_MSG_SIZE+3)/4];

// For motion-JPEG only
static IMAGE_DECODE_STRUCT img_decode_struct;
INT32U OutputFlag;

//sunxw added for media play start//
static INT32S media_qflush_flag = 0;
static INT32U temp_state_id = 0;
//sunxw added for media play end//

void image_task_init(void);
INT32S remove_request_from_queue(INT32U cmd_id);


void image_task_init(void)
{
	INT32U i;

	// Create message queue to receive request from user task
	if (!image_msg_queue_id) {
    	image_msg_queue_id = msgQCreate(IMAGE_QUEUE_MAX_NUMS, IMAGE_QUEUE_MAX_NUMS, IMAGE_MAX_MSG_SIZE);
    }

    // Local queue for saving requests from user task so that we can handle stop request
    for (i=0; i<IMAGE_QUEUE_MAX_NUMS; i++) {
    	image_local_queue[i][0] = 0;
    }
    image_q_read = 0;
    image_q_write = 0;

	// Create message queue to receive reply from file server task
	if (!image_task_fs_queue_a) {
		image_task_fs_queue_a = OSQCreate(&fs_fifo_buffer_a[0], C_IMAGE_DECODE_FILE_QUEUE_SIZE);
	}
	if (!image_task_fs_queue_b) {
		image_task_fs_queue_b = OSQCreate(&fs_fifo_buffer_b[0], C_IMAGE_DECODE_FILE_QUEUE_SIZE);
	}
}

void image_task_entry(void *p_arg)
{
    INT32U msg_id;
	INT32S result;
	TK_FILE_SERVICE_STRUCT flush_fs_cmd;
	STAVIBufConfig *p_avi_decode_control;
	MJPEG_HEADER_PARSE_STRUCT *p_mjpeg_para;
	INT8U read_local_queue, error;

    image_task_init();

	while (1) {
        if (image_q_read != image_q_write) {
        	msg_id = image_local_queue[image_q_read][0];
        	read_local_queue = 1;
        } else {
        	msgQReceive(image_msg_queue_id, &msg_id, (void *) image_task_para, IMAGE_MAX_MSG_SIZE);
        	read_local_queue = 0;
        }
        switch (msg_id) {
        case MSG_IMAGE_PARSE_HEADER_REQ:
        	if (read_local_queue) {
        		image_parse_header((IMAGE_HEADER_PARSE_STRUCT *) &image_local_queue[image_q_read][1]);
				msgQSend(ApQ, MSG_IMAGE_PARSE_HEADER_REPLY, (void *) &image_local_queue[image_q_read][1], sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
        	} else {
				image_parse_header((IMAGE_HEADER_PARSE_STRUCT *) image_task_para);
				msgQSend(ApQ, MSG_IMAGE_PARSE_HEADER_REPLY, (void *) image_task_para, sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
			}
			break;

        case MSG_IMAGE_DECODE_REQ:
        	if (read_local_queue) {
        		image_decode_and_scale((IMAGE_DECODE_STRUCT *) &image_local_queue[image_q_read][1]);
        		msgQSend(ApQ, MSG_IMAGE_DECODE_REPLY, (void *) &image_local_queue[image_q_read][1], sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
        	} else {
        		image_decode_and_scale((IMAGE_DECODE_STRUCT *) image_task_para);
        		msgQSend(ApQ, MSG_IMAGE_DECODE_REPLY, (void *) image_task_para, sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
        	  #ifndef	_DPF_PROJECT
        		image_decode_end();
        	  #endif
        	}
        	break;

		case MSG_IMAGE_ENCODE_REQ:
        	if (read_local_queue) {
        		image_encode((IMAGE_ENCODE_STRUCT *) &image_local_queue[image_q_read][1]);
        		msgQSend(ApQ, MSG_IMAGE_ENCODE_REPLY, (void *) &image_local_queue[image_q_read][1], sizeof(IMAGE_ENCODE_STRUCT), MSG_PRI_NORMAL);
        	} else {
        		image_encode((IMAGE_ENCODE_STRUCT *) image_task_para);
        		msgQSend(ApQ, MSG_IMAGE_ENCODE_REPLY, (void *) image_task_para, sizeof(IMAGE_ENCODE_STRUCT), MSG_PRI_NORMAL);
        	}
        	break;

        case MSG_IMAGE_DECODE_EXT_REQ:
        	if (read_local_queue) {
        		image_decode_ext_and_scale((IMAGE_DECODE_EXT_STRUCT *) &image_local_queue[image_q_read][1]);
        		msgQSend(ApQ, MSG_IMAGE_DECODE_EXT_REPLY, (void *) &image_local_queue[image_q_read][1], sizeof(IMAGE_DECODE_EXT_STRUCT), MSG_PRI_NORMAL);
        	} else {
        		image_decode_ext_and_scale((IMAGE_DECODE_EXT_STRUCT *) image_task_para);
        		msgQSend(ApQ, MSG_IMAGE_DECODE_EXT_REPLY, (void *) image_task_para, sizeof(IMAGE_DECODE_EXT_STRUCT), MSG_PRI_NORMAL);
        	  #ifndef	_DPF_PROJECT
        		image_decode_end();
        	  #endif
        	}
        	break;

        case MSG_IMAGE_SCALE_REQ:
			if (read_local_queue) {
        		image_scale((IMAGE_SCALE_STRUCT *) &image_local_queue[image_q_read][1]);
        		msgQSend(ApQ, MSG_IMAGE_SCALE_REPLY, (void *) &image_local_queue[image_q_read][1], sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
        	} else {
				image_scale((IMAGE_SCALE_STRUCT *) image_task_para);
				msgQSend(ApQ, MSG_IMAGE_SCALE_REPLY, (void *) image_task_para, sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
			}
			break;

		case MSG_MJPEG_PARSE_HEADER_REQ:
			if (read_local_queue) {
        		p_mjpeg_para = (MJPEG_HEADER_PARSE_STRUCT *) &image_local_queue[image_q_read][1];
        	} else {
        		p_mjpeg_para = (MJPEG_HEADER_PARSE_STRUCT *) image_task_para;
        	}
			if (p_mjpeg_para->status == 0) {
				if (avi_parsing(p_mjpeg_para->vedhandle,p_mjpeg_para->audhandle)==0) {
					p_mjpeg_para->status = 0;
				} else {
					p_mjpeg_para->status = -1;
				}
			} else if (p_mjpeg_para->status == 1) {
				if (mov_parsing(p_mjpeg_para->vedhandle , p_mjpeg_para->audhandle)==0) {
					p_mjpeg_para->status = 0;
				} else {
					p_mjpeg_para->status = -1;
				}
			}
			msgQSend(ApQ, MSG_MJPEG_PARSE_HEADER_REPLY, (void *) p_mjpeg_para, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
			break;

        case MSG_MJPEG_DECODE:
        	if (read_local_queue) {
        		p_avi_decode_control = (STAVIBufConfig *) &image_local_queue[image_q_read][1];
        	} else {
        		p_avi_decode_control = (STAVIBufConfig *) image_task_para;
        	}
        	if (avi_mov_switchflag == C_AVI_MODE_PLAY) {
				result = tk_avi_vediodata_find(p_avi_decode_control->avi_framenum_silp);
			} else if (avi_mov_switchflag == C_MOV_MODE_PLAY) {
				result = tk_mov_vediodata_find(p_avi_decode_control->avi_framenum_silp);
			} else {
				break;
			}
			if (result != 0) {
				timer_stop(AVI_TIMER);
				msgQSend(ApQ, MSG_AVI_MJPEG_END, NULL, 0, MSG_PRI_URGENT);
	        	msgQFlush(image_msg_queue_id);
	        	//sunxw added for media play start//
	        	if(media_qflush_flag == 1)        	
					msgQSend(image_msg_queue_id, MSG_IMAGE_STOP_REQ, (void *) &temp_state_id, sizeof(INT32U), MSG_PRI_URGENT);	        	
				//sunxw added for media play end//	        	
	        	break;
			}

		case MSG_MJPEG_DECODE_1:
        	msgQFlush(image_msg_queue_id);
        	#ifdef _DPF_PROJECT
        	if(OutputFlag != 0)
        		OutputFlag=0;
        	else
        		OutputFlag = 1;	
        	#else
        		OutputFlag = 1;
        	#endif
        	
        	
        	//sunxw added for media play start//
        	if(media_qflush_flag == 1)        	
				msgQSend(image_msg_queue_id, MSG_IMAGE_STOP_REQ, (void *) &temp_state_id, sizeof(INT32U), MSG_PRI_URGENT);	        	
			//sunxw added for media play end//        	
			
        	if (read_local_queue) {
        		p_avi_decode_control = (STAVIBufConfig *) &image_local_queue[image_q_read][1];
        	} else {
        		p_avi_decode_control = (STAVIBufConfig *) image_task_para;
        	}
			img_decode_struct.source_size = 0x03FFFFFF;
			img_decode_struct.source_type = TK_IMAGE_SOURCE_TYPE_FILE;		// 0=File System, 1=SDRAM, 2=NVRAM
			img_decode_struct.output_format = SCALER_CTRL_OUT_YUYV;
			img_decode_struct.output_ratio = 0;             	// 0=Fit to output_buffer_width and output_buffer_height, 1=Maintain ratio and fit to output_buffer_width or output_buffer_height, 2=Same as 1 but without scale up
			img_decode_struct.output_buffer_width = p_avi_decode_control->avi_image_width;
			img_decode_struct.output_buffer_height = p_avi_decode_control->avi_image_height;
			img_decode_struct.output_image_width = p_avi_decode_control->avi_image_width;
			img_decode_struct.output_image_height = p_avi_decode_control->avi_image_height;
			img_decode_struct.out_of_boundary_color = 0x008080;		// Black
			img_decode_struct.output_buffer_pointer = (INT32U) p_avi_decode_control->avibufptr;
			
			if (p_avi_decode_control->display_mode & 0xA) {
            	// Regular decode and scale path
				if (avi_mov_switchflag == C_MOV_MODE_PLAY)
					result = mov_mjpeg_file_decode_and_scale(&img_decode_struct);
    			else if (avi_mov_switchflag == C_AVI_MODE_PLAY)
				result = mjpeg_file_decode_and_scale(&img_decode_struct);
            } else {
        		// Shortcut for Motion-JPEG: decode without scaling
				if (avi_mov_switchflag == C_MOV_MODE_PLAY)
					result = mov_mjpeg_file_decode_without_scale(&img_decode_struct);
				else if (avi_mov_switchflag == C_AVI_MODE_PLAY)
				result = mjpeg_file_decode_without_scale(&img_decode_struct);
	        }

			// Flush file reading FIFO A
			if (image_task_fs_queue_a) {
				flush_fs_cmd.result_queue = image_task_fs_queue_a;
				msgQSend(fs_msg_q_id, MSG_FILESRV_FLUSH, (void *) &flush_fs_cmd, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				while (((INT32U) OSQPend(image_task_fs_queue_a, 100, &error)) != C_FILE_SERVICE_FLUSH_DONE) {
					if (error != OS_NO_ERR) {
						DBG_PRINT("image_decode_and_scale() failed to flush image_task_fs_queue_a");
						break;
					}
				}
			}
			// Flush file reading FIFO B
			if (image_task_fs_queue_b) {
				flush_fs_cmd.result_queue = image_task_fs_queue_b;
				msgQSend(fs_msg_q_id, MSG_FILESRV_FLUSH, (void *) &flush_fs_cmd, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				while (((INT32U) OSQPend(image_task_fs_queue_b, 100, &error)) != C_FILE_SERVICE_FLUSH_DONE) {
					if (error != OS_NO_ERR) {
						DBG_PRINT("image_decode_and_scale() failed to flush image_task_fs_queue_b");
						break;
					}
				}
			}
			//msgQFlush(image_msg_queue_id);
   	 		if (result==0) {
			  #ifdef _DPF_PROJECT
				PPU_OS_LCOK();
				avi_ppu_set(p_avi_decode_control);
				//gplib_ppu_go_and_wait_done(turnkey_ppu_register_set);
			    PPU_OS_UNLCOK();
			  #else
	           	video_decode_FrameReady(p_avi_decode_control->avibufptr);	//Roy ADD
			  #endif
        	}
        	//msgQFlush(image_msg_queue_id);
        	break;

		case MSG_IMAGE_STOP_REQ:
			if (read_local_queue) {
        		remove_request_from_queue(image_local_queue[image_q_read][1]);
        	} else {
				remove_request_from_queue(image_task_para[0]);
			}
			break;

        default:
            break;
        }
        if (read_local_queue) {
			if (image_q_read == IMAGE_QUEUE_MAX_NUMS-1) {
				image_q_read = 0;
			} else {
				image_q_read++;
			}
        }
	}
}

INT32S remove_request_from_queue(INT32U cmd_id)
{
	INT32S result;
	INT16U index;

	index = image_q_read;

	// Remove all request of the same state from the local queue
	while (index != image_q_write) {
		if ((image_local_queue[index][1] & IMAGE_CMD_STATE_MASK) == (cmd_id & IMAGE_CMD_STATE_MASK)) {
			if (MSG_IMAGE_PARSE_HEADER_REQ == image_local_queue[index][0]) {
				((IMAGE_HEADER_PARSE_STRUCT *) &image_local_queue[index][1])->parse_status = STATUS_FAIL;
				msgQSend(ApQ, MSG_IMAGE_PARSE_HEADER_REPLY, (void *) &image_local_queue[index][1], sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
				image_local_queue[index][0] = MSG_IMAGE_REQUEST_MAX;

			} else if (MSG_IMAGE_DECODE_REQ == image_local_queue[index][0]) {
				((IMAGE_DECODE_STRUCT *) &image_local_queue[index][1])->decode_status = STATUS_FAIL;
				msgQSend(ApQ, MSG_IMAGE_DECODE_REPLY, (void *) &image_local_queue[index][1], sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
				image_local_queue[index][0] = MSG_IMAGE_REQUEST_MAX;

			} else if (MSG_IMAGE_DECODE_EXT_REQ == image_local_queue[index][0]) {
				((IMAGE_DECODE_EXT_STRUCT *) &image_local_queue[index][1])->basic.decode_status = STATUS_FAIL;
				msgQSend(ApQ, MSG_IMAGE_DECODE_EXT_REPLY, (void *) &image_local_queue[index][1], sizeof(IMAGE_DECODE_EXT_STRUCT), MSG_PRI_NORMAL);
				image_local_queue[index][0] = MSG_IMAGE_REQUEST_MAX;

			} else if (MSG_IMAGE_SCALE_REQ == image_local_queue[index][0]) {
				((IMAGE_SCALE_STRUCT *) &image_local_queue[index][1])->status = STATUS_FAIL;
				msgQSend(ApQ, MSG_IMAGE_SCALE_REPLY, (void *) &image_local_queue[index][1], sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
				image_local_queue[index][0] = MSG_IMAGE_REQUEST_MAX;
			}
		}

		if (index == IMAGE_QUEUE_MAX_NUMS-1) {
			index = 0;
		} else {
			index++;
		}
	}

	// Check whether we should remove request from image queue
	do {
		result = msgQAccept(image_msg_queue_id, &image_local_queue[image_q_write][0], (void *) &image_local_queue[image_q_write][1], IMAGE_MAX_MSG_SIZE);
		if (result >= 0) {
			if ((cmd_id & IMAGE_CMD_STATE_MASK) == (image_local_queue[image_q_write][1] & IMAGE_CMD_STATE_MASK)) {
				if (MSG_IMAGE_PARSE_HEADER_REQ == image_local_queue[image_q_write][0]) {
					((IMAGE_HEADER_PARSE_STRUCT *) &image_local_queue[image_q_write][1])->parse_status = STATUS_FAIL;
    				msgQSend(ApQ, MSG_IMAGE_PARSE_HEADER_REPLY, (void *) &image_local_queue[image_q_write][1], sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
					continue;

        		} else if (MSG_IMAGE_DECODE_REQ == image_local_queue[image_q_write][0]) {
        			((IMAGE_DECODE_STRUCT *) &image_local_queue[image_q_write][1])->decode_status = STATUS_FAIL;
					msgQSend(ApQ, MSG_IMAGE_DECODE_REPLY, (void *) &image_local_queue[image_q_write][1], sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);
					continue;

        		} else if (MSG_IMAGE_DECODE_EXT_REQ == image_local_queue[image_q_write][0]) {
        			((IMAGE_DECODE_EXT_STRUCT *) &image_local_queue[image_q_write][1])->basic.decode_status = STATUS_FAIL;
					msgQSend(ApQ, MSG_IMAGE_DECODE_EXT_REPLY, (void *) &image_local_queue[image_q_write][1], sizeof(IMAGE_DECODE_EXT_STRUCT), MSG_PRI_NORMAL);
					continue;

        		} else if (MSG_IMAGE_SCALE_REQ == image_local_queue[image_q_write][0]) {
        			((IMAGE_SCALE_STRUCT *) &image_local_queue[image_q_write][1])->status = STATUS_FAIL;
					msgQSend(ApQ, MSG_IMAGE_SCALE_REPLY, (void *) &image_local_queue[image_q_write][1], sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
					continue;
    			}
        	}
			if (image_q_write == IMAGE_QUEUE_MAX_NUMS-1) {
				image_q_write = 0;
			} else {
				image_q_write++;
			}
		}
	} while (result >= 0) ;

	// Send remove done message
	msgQSend(ApQ, MSG_IMAGE_STOP_REPLY, NULL, 0, MSG_PRI_NORMAL);
	//sunxw added for media play start//
	media_qflush_flag = 0;
	//sunxw added for media play end//
	return 0;
}

INT32S image_task_handle_remove_request(INT32U current_image_id)
{
	INT32S result;
	INT32S stop_current_decoding;

	stop_current_decoding = 0;

	while (1) {
		if ((image_q_read==image_q_write+1) || (image_q_read==0 && image_q_write==IMAGE_QUEUE_MAX_NUMS-1)) {
			DBG_PRINT("Local queue of image task is full\r\n");

			return -1;
		}

		result = msgQAccept(image_msg_queue_id, &image_local_queue[image_q_write][0], (void *) &image_local_queue[image_q_write][1], IMAGE_MAX_MSG_SIZE);
		if (result >= 0) {
			if (image_local_queue[image_q_write][0] == MSG_IMAGE_STOP_REQ) {
				// Check whether we should break decoding of current image
				if ((image_local_queue[image_q_write][1] & IMAGE_CMD_STATE_MASK) == (current_image_id & IMAGE_CMD_STATE_MASK)) {
					stop_current_decoding = 1;
				}
			}
			// Store this request into local queue
			if (image_q_write == IMAGE_QUEUE_MAX_NUMS-1) {
				image_q_write = 0;
			} else {
				image_q_write++;
			}
		} else {
			break;
		}
	}

	return stop_current_decoding;
}

// APIs that will be externed to others
INT32S image_task_parse_image(IMAGE_HEADER_PARSE_STRUCT *para)
{
	return msgQSend(image_msg_queue_id, MSG_IMAGE_PARSE_HEADER_REQ, (void *) para, sizeof(IMAGE_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
}

INT32S image_task_scale_image(IMAGE_SCALE_STRUCT *para)
{
	return msgQSend(image_msg_queue_id, MSG_IMAGE_SCALE_REQ, (void *) para, sizeof(IMAGE_SCALE_STRUCT), MSG_PRI_NORMAL);
}

INT32S image_task_remove_request(INT32U state_id)
{
	state_id = ((state_id - (INT32U) STATE_BASE) << IMAGE_CMD_STATE_SHIFT_BITS) & IMAGE_CMD_STATE_MASK;
	//sunxw added for media play start//
	media_qflush_flag = 1;
	temp_state_id = state_id;
	//sunxw added for media play end//
	return msgQSend(image_msg_queue_id, MSG_IMAGE_STOP_REQ, (void *) &state_id, sizeof(INT32U), MSG_PRI_URGENT);
}
