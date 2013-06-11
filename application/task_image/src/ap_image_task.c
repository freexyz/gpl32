/*
* Description: This file parse/decode/scale images
*
* Author: Tristan Yang
*
* Date: 2008/07/19
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "ap_image_task.h"

// Control variables
#define C_IMAGE_READ_FILE_BUFFER_SIZE		512

static TK_FILE_SERVICE_STRUCT fs_cmd;

INT32S image_decode_with_clip_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size);

TK_IMAGE_TYPE_ENUM image_buffer_judge_type(INT8U *buffer_ptr)
{
	if (*buffer_ptr==0xFF && *(buffer_ptr+1)==0xD8) {	    // JPEG
		return 	TK_IMAGE_TYPE_JPEG;
	} else if (*buffer_ptr=='R' && *(buffer_ptr+1)=='I' && *(buffer_ptr+2)=='F' && *(buffer_ptr+3)=='F') {		// Motion-JPEG
		return TK_IMAGE_TYPE_MOTION_JPEG;
	} else if (*buffer_ptr=='G' && *(buffer_ptr+1)=='P' && *(buffer_ptr+2)=='Z' && *(buffer_ptr+3)=='P') {		// GPZP
		return TK_IMAGE_TYPE_GPZP;
	} else if (*buffer_ptr=='B' && *(buffer_ptr+1)=='M') {		// BMP
		return TK_IMAGE_TYPE_BMP;
	} else if (*buffer_ptr=='G' && *(buffer_ptr+1)=='I' && *(buffer_ptr+2)=='F' ) {		// GIF
		return TK_IMAGE_TYPE_GIF;
//	} else if (*buffer_ptr==0x89 && *(buffer_ptr+1)=='P' && *(buffer_ptr+2)=='N' && *(buffer_ptr+3)=='G') {		// PNG
//		return TK_IMAGE_TYPE_PNG;
	} else if ((*(buffer_ptr+4)=='p' && *(buffer_ptr+5)=='n' && *(buffer_ptr+6)=='o' && *(buffer_ptr+7)=='t') ||
	           (*(buffer_ptr+4)=='s' && *(buffer_ptr+5)=='k' && *(buffer_ptr+6)=='i' && *(buffer_ptr+7)=='p') ||
	           (*(buffer_ptr+4)=='w' && *(buffer_ptr+5)=='i' && *(buffer_ptr+6)=='d' && *(buffer_ptr+7)=='e') ||
	           (*(buffer_ptr+4)=='f' && *(buffer_ptr+5)=='r' && *(buffer_ptr+6)=='e' && *(buffer_ptr+7)=='e') ||
	           (*(buffer_ptr+4)=='m' && *(buffer_ptr+5)=='o' && *(buffer_ptr+6)=='o' && *(buffer_ptr+7)=='v') ||
	           (*(buffer_ptr+4)=='m' && *(buffer_ptr+5)=='d' && *(buffer_ptr+6)=='a' && *(buffer_ptr+7)=='t')) {		//MOV
		return TK_IMAGE_TYPE_MOV_JPEG;

  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
	} else {
		INT8U tmp[12];
		INT8U i;

		for (i=0; i<12; i++) {
			tmp[i] = (*(buffer_ptr+i)) ^ 0xFF;
		}

		if (tmp[0]==0xFF && tmp[1]==0xD8) {	    // JPEG
			return 	TK_IMAGE_TYPE_JPEG;
		} else if (tmp[0]=='R' && tmp[1]=='I' && tmp[2]=='F' && tmp[3]=='F') {		// Motion-JPEG
			return TK_IMAGE_TYPE_MOTION_JPEG;
		} else if (tmp[0]=='G' && tmp[1]=='P' && tmp[2]=='Z' && tmp[3]=='P') {		// GPZP
			return TK_IMAGE_TYPE_GPZP;
		} else if (tmp[0]=='B' && tmp[1]=='M') {		// BMP
			return TK_IMAGE_TYPE_BMP;
		} else if (tmp[0]=='G' && tmp[1]=='I' && tmp[2]=='F' ) {		// GIF
			return TK_IMAGE_TYPE_GIF;
		} else if ((tmp[4]=='p' && tmp[5]=='n' && tmp[6]=='o' && tmp[7]=='t') ||
		           (tmp[4]=='s' && tmp[5]=='k' && tmp[6]=='i' && tmp[7]=='p') ||
		           (tmp[4]=='w' && tmp[5]=='i' && tmp[6]=='d' && tmp[7]=='e') ||
		           (tmp[4]=='f' && tmp[5]=='r' && tmp[6]=='e' && tmp[7]=='e') ||
		           (tmp[4]=='m' && tmp[5]=='o' && tmp[6]=='o' && tmp[7]=='v') ||
		           (tmp[4]=='m' && tmp[5]=='d' && tmp[6]=='a' && tmp[7]=='t')) {		//MOV
			return TK_IMAGE_TYPE_MOV_JPEG;
		}
  #endif
	}

	return TK_IMAGE_TYPE_MAX;
}

TK_IMAGE_TYPE_ENUM image_file_judge_type(INT16S fd)
{
	INT8U *buffer_ptr;
	INT32S len;
	TK_IMAGE_TYPE_ENUM result;
	INT8U  err;

	buffer_ptr = (INT8U *) gp_iram_malloc_align(C_IMAGE_READ_FILE_BUFFER_SIZE, 16);
	if (!buffer_ptr) {
		buffer_ptr = (INT8U *) gp_malloc_align(C_IMAGE_READ_FILE_BUFFER_SIZE, 16);
		if (!buffer_ptr) {
			DBG_PRINT("image_file_judge_type() failed to allocate buffer_ptr\r\n");

			return TK_IMAGE_TYPE_MAX;
		}
	}

	lseek((INT16S)fd, 0, SEEK_SET);
	fs_cmd.fd = fd;
	fs_cmd.result_queue = image_task_fs_queue_a;
	fs_cmd.buf_addr = (INT32U) buffer_ptr;
	fs_cmd.buf_size = C_IMAGE_READ_FILE_BUFFER_SIZE;
	while (OSQAccept(image_task_fs_queue_a, &err)) ;
	msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);

	len = (INT32S) OSQPend(image_task_fs_queue_a, 200, &err);
	if (err!=OS_NO_ERR || len<4) {
		DBG_PRINT("image_file_judge_type() failed to read data from file\r\n");
		gp_free((void *) buffer_ptr);

		return TK_IMAGE_TYPE_MAX;
	}

	result = image_buffer_judge_type(buffer_ptr);
	gp_free((void *) buffer_ptr);

	return result;
}

INT32S image_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser)
{
	if (!parser) {
		return -1;
	}

	parser->orientation = 1;
	parser->date_time_ptr[0] = 0x0;
	switch (parser->source_type) {
	case TK_IMAGE_SOURCE_TYPE_FILE:				// File system
		parser->image_type = (INT8U) image_file_judge_type((INT16S) parser->image_source);
		switch (parser->image_type) {
		case TK_IMAGE_TYPE_JPEG:
			jpeg_file_parse_header(parser);
			break;

		case TK_IMAGE_TYPE_MOTION_JPEG:
			mjpeg_file_parse_header(parser);
			break;

		case TK_IMAGE_TYPE_MOV_JPEG:
			mov_mjpeg_file_parse_header(parser);
			break;

	  #if GPLIB_BMP_EN == 1
		case TK_IMAGE_TYPE_BMP:
			bmp_file_parse_header(parser);
			break;
	  #endif

	  #if GPLIB_GIF_EN == 1
		case TK_IMAGE_TYPE_GIF:
			gif_parse_header(parser);
			break;
	  #endif

		default:
            parser->parse_status = STATUS_FAIL;

			return STATUS_FAIL;
		}
	    break;

	case TK_IMAGE_SOURCE_TYPE_BUFFER:			// SDRAM buffer
		parser->image_type = (INT8U) image_buffer_judge_type((INT8U *) parser->image_source);
		switch (parser->image_type) {
		case TK_IMAGE_TYPE_JPEG:
			jpeg_buffer_parse_header(parser);
			break;

		case TK_IMAGE_TYPE_MOTION_JPEG:
			break;

	  #if GPLIB_BMP_EN == 1
		case TK_IMAGE_TYPE_BMP:
			bmp_buffer_parse_header(parser);
			break;
	  #endif

	  #if GPLIB_GIF_EN == 1
		case TK_IMAGE_TYPE_GIF:
			gif_parse_header(parser);
			break;
	  #endif

		default:
            parser->parse_status = STATUS_FAIL;

			return STATUS_FAIL;
		}
	    break;

	default:
	    DBG_PRINT("Image decode task get an unknow image source type!\r\n");
	    parser->parse_status = STATUS_FAIL;

	    return STATUS_FAIL;
	}

	return STATUS_OK;
}

INT32S image_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
{
	return image_decode_with_clip_and_scale(img_decode_struct, 0, 0);
}

INT32S image_decode_ext_and_scale(IMAGE_DECODE_EXT_STRUCT *img_decode_ext_struct)
{
	INT32U clip_xy, clip_size;

	clip_xy = ((INT32U) img_decode_ext_struct->clipping_start_x<<16) | img_decode_ext_struct->clipping_start_y;
	clip_size = ((INT32U) img_decode_ext_struct->clipping_width<<16) | img_decode_ext_struct->clipping_height;

	return image_decode_with_clip_and_scale((IMAGE_DECODE_STRUCT *) img_decode_ext_struct, clip_xy, clip_size);
}

INT32S image_decode_with_clip_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U clip_xy, INT32U clip_size)
{
	TK_FILE_SERVICE_STRUCT flush_fs_cmd;
	INT8U error;

	if (!img_decode_struct || !img_decode_struct->output_buffer_width || !img_decode_struct->output_buffer_height) {
		return -1;
	}
	switch (img_decode_struct->source_type) {
	case TK_IMAGE_SOURCE_TYPE_FILE:         // File system
		while (OSQAccept(image_task_fs_queue_a, &error)) ;
		while (OSQAccept(image_task_fs_queue_b, &error)) ;
		switch (image_file_judge_type((INT16S) img_decode_struct->image_source)) {
		case TK_IMAGE_TYPE_JPEG:
			jpeg_file_decode_and_scale(img_decode_struct, clip_xy, clip_size);

			// Flush file reading FIFO A
			if (image_task_fs_queue_a) {
				flush_fs_cmd.result_queue = image_task_fs_queue_a;
				msgQSend(fs_msg_q_id, MSG_FILESRV_FLUSH, (void *) &flush_fs_cmd, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				while (((INT32U) OSQPend(image_task_fs_queue_a, 200, &error)) != C_FILE_SERVICE_FLUSH_DONE) {
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
				while (((INT32U) OSQPend(image_task_fs_queue_b, 200, &error)) != C_FILE_SERVICE_FLUSH_DONE) {
					if (error != OS_NO_ERR) {
						DBG_PRINT("image_decode_and_scale() failed to flush image_task_fs_queue_b");
						break;
					}
				}
			}
			break;

		case TK_IMAGE_TYPE_MOTION_JPEG:
			mjpeg_file_decode_one_frame(img_decode_struct);
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
			break;
		case TK_IMAGE_TYPE_MOV_JPEG:
			mov_mjpeg_file_decode_one_frame(img_decode_struct);
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
			break;
	  #if GPLIB_BMP_EN == 1
		case TK_IMAGE_TYPE_BMP:
			bmp_file_decode_and_scale(img_decode_struct);
			break;
	  #endif

	  #if GPLIB_GIF_EN == 1
		case TK_IMAGE_TYPE_GIF:
			gif_decode_and_scale(img_decode_struct);
			break;
	  #endif

		default:
			img_decode_struct->decode_status = STATUS_FAIL;

			return STATUS_FAIL;
		}
	    break;

	case TK_IMAGE_SOURCE_TYPE_BUFFER:         // SDRAM buffer
		switch (image_buffer_judge_type((INT8U *) img_decode_struct->image_source)) {
		case TK_IMAGE_TYPE_JPEG:
			jpeg_buffer_decode_and_scale(img_decode_struct, clip_xy, clip_size);
			break;

		case TK_IMAGE_TYPE_MOTION_JPEG:
			break;

	  #if GPLIB_BMP_EN == 1
		case TK_IMAGE_TYPE_BMP:
//			bmp_buffer_decode_and_scale(img_decode_struct);
			break;
	  #endif

	  #if GPLIB_GIF_EN == 1
		case TK_IMAGE_TYPE_GIF:
			gif_decode_and_scale(img_decode_struct);
			break;
	  #endif

		default:
			img_decode_struct->decode_status = STATUS_FAIL;

			return STATUS_FAIL;
		}
	    break;

	default:
	    DBG_PRINT("Image decode task get an unknow image source type!\r\n");
	    img_decode_struct->decode_status = STATUS_FAIL;

	    return STATUS_FAIL;
	}

	return STATUS_OK;
}

INT32S image_encode(IMAGE_ENCODE_STRUCT *img_encode_struct)
{
  #if GPLIB_JPEG_ENCODE_EN == 1
	if (!img_encode_struct ||
		!img_encode_struct->OutputBuf ||
		!img_encode_struct->InputBuf_Y ||
		!img_encode_struct->InputWidth ||
		!img_encode_struct->InputHeight) {
		img_encode_struct->EncodeState = -1;

		return STATUS_FAIL;
	}

	switch (img_encode_struct->EncodeType) {
	case TK_IMAGE_TYPE_JPEG:
		return jpeg_buffer_encode(img_encode_struct);
	}
  #endif

	img_encode_struct->EncodeState = -1;

	return STATUS_FAIL;
}

INT32S image_scale(IMAGE_SCALE_STRUCT *img_scale_struct)
{
	INT32S	scaler_status;
	INT32U  external_line_addr = 0;

	if (!img_scale_struct || !img_scale_struct->output_buffer_width || !img_scale_struct->output_buffer_height) {
		return -1;
	}
	scaler_init();

	scaler_input_pixels_set(img_scale_struct->input_width, img_scale_struct->input_height);
	scaler_input_visible_pixels_set(img_scale_struct->input_visible_width, img_scale_struct->input_visible_height);
	scaler_input_addr_set((INT32U) img_scale_struct->input_buf1, NULL, NULL);
	scaler_input_offset_set(img_scale_struct->input_offset_x, img_scale_struct->input_offset_y);
	scaler_input_format_set(img_scale_struct->scaler_input_format);

	scaler_output_pixels_set(img_scale_struct->output_width_factor, img_scale_struct->output_height_factor, img_scale_struct->output_buffer_width, img_scale_struct->output_buffer_height);
	scaler_output_addr_set((INT32U) img_scale_struct->output_buf1, NULL, NULL);
	scaler_output_format_set(img_scale_struct->scaler_output_format);

	scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	scaler_out_of_boundary_mode_set(1);			// Use Use color defined in scaler_out_of_boundary_color_set()
	scaler_out_of_boundary_color_set(img_scale_struct->out_of_boundary_color);
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
	if(img_scale_struct->output_buffer_width > 1024) {
		external_line_addr = (INT32U)gp_malloc_align((img_scale_struct->output_buffer_width - 1024) << 1, 16);
		if(external_line_addr) {
			scaler_external_line_buffer_set(external_line_addr);
			scaler_line_buffer_mode_set(C_SCALER_HYBRID_LINE_BUFFER);
		} else {
			scaler_external_line_buffer_set(external_line_addr);
			scaler_line_buffer_mode_set(C_SCALER_INTERNAL_LINE_BUFFER);
		}
	}
#endif	
	while (1) {
  		scaler_status = scaler_wait_idle();
  		if (scaler_status == C_SCALER_STATUS_STOP) {
			scaler_start();
		} else if (scaler_status & C_SCALER_STATUS_DONE) {
			img_scale_struct->status = STATUS_OK;
			break;
		} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
			DBG_PRINT("Scaler failed to finish its job\r\n");
			img_scale_struct->status = STATUS_FAIL;
			break;
  		} else {
	  		DBG_PRINT("Un-handled Scaler status!\r\n");
	  		img_scale_struct->status = STATUS_FAIL;
	  		break;
	  	}
  	}
  	scaler_stop();
	if(external_line_addr) {
		gp_free((void *)external_line_addr);
	}
  	if (scaler_status & C_SCALER_STATUS_DONE) {
  		cache_invalid_range((INT32U) img_scale_struct->output_buf1, (img_scale_struct->output_buffer_width*img_scale_struct->output_buffer_height)<<1);

  		return STATUS_OK;
  	}
  	return STATUS_FAIL;
}
