/*
* Description: This file parse/decode/scale gif images
*
* Author: Tristan Yang
*
* Date: 2008/08/13
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "ap_gif.h"

// Task relative definitions and variables
#define C_GIF_DECODE_QUEUE_SIZE		4
#define C_GIF_DECODE_BUFF_LINE		32

INT8U *gif_input_buf;
static CHAR *gif_working_memory;
static TK_FILE_SERVICE_STRUCT gif_fs_cmd_a, gif_fs_cmd_b;
static INT32S gif_read_index, gif_write_index;
static INT32U gif_source_buffer_start, gif_source_buffer_end;

static INT32U gif_cmd_id;
static INT8U gif_source_type;
#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
static INT8U gif_need_invert;
#endif

static INT32U gif_output_format;
static INT8U  gif_output_ratio;             // 0=Fit to output_buffer_width and output_buffer_height, 1=Maintain ratio and fit to output_buffer_width or output_buffer_height, 2=Same as 1 but without scale up
static INT16U gif_output_buffer_width;
static INT16U gif_output_buffer_height;
static INT16U gif_output_image_width;
static INT16U gif_output_image_height;
static INT32U gif_out_of_boundary_color;
static INT32U gif_output_buffer_pointer;
static INT32U gif_scaler_out_buffer;


INT32S	gif_input_data_read(void);
void	gif_scaler_set_parameters(INT32U width, INT32U height, INT32U buff, INT8S *status_ptr, INT32U gif_scaler_mode);

INT32S gif_input_data_read(void)
{
	INT32U len;
	INT8U error;

	// Check whether we have to break decoding this image
	if (image_task_handle_remove_request(gif_cmd_id) > 0) {
		return -1;
	}

	gif_read_index = gif_dec_get_ri(gif_working_memory);

	if ((!gif_read_index || gif_read_index>(C_GIF_DEC_FS_BUFFER_SIZE>>1)) && gif_write_index==0) {
		if (gif_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &gif_fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			gif_write_index = (INT32S) OSQPend(gif_fs_cmd_a.result_queue, 200, &error);
			if ((error!=OS_NO_ERR) || (gif_write_index<0)) {
				DBG_PRINT("Failed to read data from gif file\r\n");

	  			return -1;
	  		}
		  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
			if (gif_need_invert) {
				INT8U *ptr8;

				ptr8 = (INT8U *) gif_fs_cmd_a.buf_addr;
				if (*ptr8==0xB8 && *(ptr8+1)==0xB6 && *(ptr8+2)==0xB9) {
					INT32U *ptr32;
					INT8U i;

					ptr32 = (INT32U *) gif_fs_cmd_a.buf_addr;
					for (i=0; i<128; i++) {
						*ptr32++ ^= 0xFFFFFFFF;
					}
				}
				gif_need_invert = 0;
			}
		  #endif

		} else {
	  		if (gif_source_buffer_start >= gif_source_buffer_end) {
	  			len = 4;
	  		} else {
		  		len = gif_source_buffer_end - gif_source_buffer_start;
		  		if (len > (C_GIF_DEC_FS_BUFFER_SIZE>>1)) {
		  			len = C_GIF_DEC_FS_BUFFER_SIZE >> 1;
		  		}
		  	}
	  		gp_memcpy((INT8S *) gif_input_buf, (INT8S *) gif_source_buffer_start, len);

		  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
			if (gif_need_invert) {
				INT8U *ptr8;

				ptr8 = (INT8U *) gif_source_buffer_start;
				if (*ptr8==0xB8 && *(ptr8+1)==0xB6 && *(ptr8+2)==0xB9) {
					INT32U *ptr32;
					INT8U i;

					ptr32 = (INT32U *) gif_source_buffer_start;
					for (i=0; i<128; i++) {
						*ptr32++ ^= 0xFFFFFFFF;
					}
				}
				gif_need_invert = 0;
			}
		  #endif

	  		gif_source_buffer_start += len;
	  		gif_write_index = (INT32S) len;
	  	}

  		if (gif_write_index < (C_GIF_DEC_FS_BUFFER_SIZE>>1)) {
  			gif_dec_set_AtFileEnd(gif_working_memory);
  		} else {
			gif_write_index = C_GIF_DEC_FS_BUFFER_SIZE >> 1;
		}
	}
	if (gif_read_index && gif_read_index<(C_GIF_DEC_FS_BUFFER_SIZE>>1) && gif_write_index==(C_GIF_DEC_FS_BUFFER_SIZE>>1)) {
		if (gif_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &gif_fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			gif_write_index = (INT32S) OSQPend(gif_fs_cmd_b.result_queue, 200, &error);
			if ((error!=OS_NO_ERR) || (gif_write_index<0)) {
				DBG_PRINT("Failed to read data from gif file\r\n");

				return -1;
	  		}

  		} else {
	  		if (gif_source_buffer_start >= gif_source_buffer_end) {
	  			len = 4;
	  		} else {
		  		len = gif_source_buffer_end - gif_source_buffer_start;
		  		if (len > (C_GIF_DEC_FS_BUFFER_SIZE>>1)) {
		  			len = C_GIF_DEC_FS_BUFFER_SIZE >> 1;
		  		}
		  	}
	  		gp_memcpy((INT8S *) gif_input_buf + (C_GIF_DEC_FS_BUFFER_SIZE>>1), (INT8S *) gif_source_buffer_start, len);
	  		gif_source_buffer_start += len;
	  		gif_write_index = (INT32S) len;
  		}

  		if (gif_write_index < (C_GIF_DEC_FS_BUFFER_SIZE>>1)) {
  			gif_dec_set_AtFileEnd(gif_working_memory);
  			gif_write_index += (C_GIF_DEC_FS_BUFFER_SIZE>>1);
  		} else {
		  	gif_write_index = 0;
		}
	}

	return 0;
}

void gif_scaler_set_parameters(INT32U width, INT32U height, INT32U buff, INT8S *status_ptr, INT32U gif_scaler_mode)
{
  #ifdef _DPF_PROJECT
	umi_scaler_color_matrix_load();
  #endif

	scaler_input_pixels_set(width, height);
	scaler_input_visible_pixels_set(width, height);

	if (gif_output_image_width && gif_output_image_height) {
	    scaler_output_pixels_set((width<<16)/gif_output_image_width, (height<<16)/gif_output_image_height, gif_output_buffer_width, gif_output_buffer_height);
	} else {
	    if (!gif_output_image_width) {
	        gif_output_image_width = gif_output_buffer_width;
	    }
	    if (!gif_output_image_height) {
	        gif_output_image_height = gif_output_buffer_height;
	    }
    	if (gif_output_ratio == 0x0) {      // Fit to output buffer width and height
      		scaler_output_pixels_set((width<<16)/gif_output_image_width, (height<<16)/gif_output_image_height, gif_output_buffer_width, gif_output_buffer_height);
    	} else if (gif_output_ratio==2 && width<=gif_output_image_width && height<=gif_output_image_height) {
    		scaler_output_pixels_set(1<<16, 1<<16, gif_output_buffer_width, gif_output_buffer_height);
    		gif_output_image_width = width;
    		gif_output_image_height = gif_output_buffer_height;
    	} else {						// Fit to output buffer width or height
    		INT32U factor;

      		if (gif_output_image_height*width > gif_output_image_width*height) {
      			factor = (width<<16)/gif_output_image_width;
      			gif_output_image_height = (height<<16)/factor;
      		} else {
      			factor = (height<<16)/gif_output_image_height;
      			gif_output_image_width = (width<<16)/factor;
      		}
      		scaler_output_pixels_set(factor, factor, gif_output_buffer_width, gif_output_buffer_height);
      	}
    }
	scaler_input_addr_set(buff, 0, 0);

	if (gif_output_buffer_pointer) {
	    gif_scaler_out_buffer = gif_output_buffer_pointer;
	} else {
	    gif_scaler_out_buffer = (INT32U) gp_malloc((gif_output_buffer_width*gif_output_buffer_height)<<1);
	    if (!gif_scaler_out_buffer) {
		    *status_ptr = -2;
			gp_free((void *) buff);

		    return;
		}
	}
   	scaler_output_addr_set(gif_scaler_out_buffer, NULL, NULL);
   	scaler_fifo_line_set(gif_scaler_mode);
	scaler_YUV_type_set(C_SCALER_CTRL_TYPE_YCBCR);
	scaler_input_format_set(C_SCALER_CTRL_IN_RGB565);
	scaler_output_format_set(gif_output_format);
	scaler_out_of_boundary_mode_set(1);			// Use Use color defined in scaler_out_of_boundary_color_set()
	scaler_out_of_boundary_color_set(gif_out_of_boundary_color);
}



INT32S gif_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser)
{
	INT32S status;

	if (!parser) {
		return -1;
	}

	gif_cmd_id = parser->cmd_id;
	gif_source_type = parser->source_type;

	// Prepare working memory
	gif_working_memory = (CHAR *) gp_malloc_align(C_GIF_DEC_WORKING_MEMORY_SIZE, 16);
	if (!gif_working_memory) {
		DBG_PRINT("Failed to allocate gif_working_memory\r\n");
		parser->parse_status = -2;

		return -2;
	}

	gif_input_buf = (INT8U *) gp_iram_malloc_align(C_GIF_DEC_FS_BUFFER_SIZE, 32);
	if (!gif_input_buf) {
		gif_input_buf = (INT8U *) gp_malloc_align(C_GIF_DEC_FS_BUFFER_SIZE, 32);
		if (!gif_input_buf) {
			DBG_PRINT("Failed to allocate gif_input_buf\r\n");
			parser->parse_status = -2;
			gp_free(gif_working_memory);

			return -2;
		}
	}

	if (gif_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
		// Setup a command that will be used to read data into fifo A
		gif_fs_cmd_a.fd = (INT16S) parser->image_source;
		gif_fs_cmd_a.buf_addr = (INT32U) gif_input_buf;
		gif_fs_cmd_a.buf_size = C_GIF_DEC_FS_BUFFER_SIZE>>1;
		gif_fs_cmd_a.result_queue = image_task_fs_queue_a;

		// Setup a command that will be used to read data into fifo B
		gif_fs_cmd_b.fd = (INT16S) parser->image_source;
		gif_fs_cmd_b.buf_addr = ((INT32U) gif_input_buf) + (C_GIF_DEC_FS_BUFFER_SIZE>>1);
		gif_fs_cmd_b.buf_size = C_GIF_DEC_FS_BUFFER_SIZE>>1;
		gif_fs_cmd_b.result_queue = image_task_fs_queue_b;

		// Seek file pointer to begin
		lseek((INT16S) parser->image_source, 0, SEEK_SET);
	} else {
		gif_source_buffer_start = parser->image_source;
		gif_source_buffer_end = parser->image_source + parser->source_size;
	}

	// Initiate gif decode engine
	gif_dec_init(gif_working_memory, gif_input_buf);
	gif_write_index = 0;
  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
	gif_need_invert = 1;
  #endif
	// Get GIF file and parse header
   	do {
   		if (gif_input_data_read()) {
   			break;
   		}

   		status = gif_dec_parsing_file_header(gif_working_memory, gif_write_index, 1);

	} while (status == C_GIF_STATUS_NOT_DONE) ;
	if (status != C_GIF_STATUS_OK) {
		DBG_PRINT("Parse header failed. Skip this image\r\n");
		parser->parse_status = STATUS_FAIL;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);

	  	return -1;
	}

	parser->parse_status = STATUS_OK;
	parser->width = (INT16U) gif_dec_get_screen_width(gif_working_memory);;
	parser->height = (INT16U) gif_dec_get_screen_height(gif_working_memory);;

	gp_free((void *) gif_input_buf);
	gp_free((void *) gif_working_memory);

	return 0;
}

INT32S gif_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
{
	INT32S still_or_motion;				// 0 = still gif, 1 = motion gif
	INT32S interlace_flag;				// 0 = non-interlace, 1 = interlace
	INT32U gif_width, gif_height;
	INT32S status;
	INT32U gif_working_line_buffer;
	INT32U gif_photo_buffer;
	INT16U pos_x, pos_y;
	INT8U  scaler_done;
	INT32U gif_buff_size, i;
	INT32U decode_line, scaler_status, scaler_mode;

	if (!img_decode_struct) {
		return STATUS_FAIL;
	}

	gif_cmd_id					= img_decode_struct->cmd_id;
	gif_source_type				= img_decode_struct->source_type;
	gif_output_format			= img_decode_struct->output_format;
	gif_output_ratio			= img_decode_struct->output_ratio;
	gif_output_buffer_width		= img_decode_struct->output_buffer_width;
	gif_output_buffer_height	= img_decode_struct->output_buffer_height;
	gif_output_image_width		= img_decode_struct->output_image_width;
	gif_output_image_height		= img_decode_struct->output_image_height;
	gif_out_of_boundary_color	= img_decode_struct->out_of_boundary_color;
	gif_output_buffer_pointer	= img_decode_struct->output_buffer_pointer;

	// Prepare working memory
	gif_working_memory = (CHAR *) gp_malloc_align(C_GIF_DEC_WORKING_MEMORY_SIZE, 16);
	if (!gif_working_memory) {
		DBG_PRINT("Failed to allocate gif_working_memory\r\n");
		img_decode_struct->decode_status = -2;

		return -2;
	}

	gif_input_buf = (INT8U *) gp_iram_malloc_align(C_GIF_DEC_FS_BUFFER_SIZE, 32);
	if (!gif_input_buf) {
		gif_input_buf = (INT8U *) gp_malloc_align(C_GIF_DEC_FS_BUFFER_SIZE, 32);
		if (!gif_input_buf) {
			DBG_PRINT("Failed to allocate gif_input_buf\r\n");
			img_decode_struct->decode_status = -2;
			gp_free(gif_working_memory);

			return -2;
		}
	}

	if (gif_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
		// Setup a command that will be used to read data into fifo A
		gif_fs_cmd_a.fd = (INT16S) img_decode_struct->image_source;
		gif_fs_cmd_a.buf_addr = (INT32U) gif_input_buf;
		gif_fs_cmd_a.buf_size = C_GIF_DEC_FS_BUFFER_SIZE>>1;
		gif_fs_cmd_a.result_queue = image_task_fs_queue_a;

		// Setup a command that will be used to read data into fifo B
		gif_fs_cmd_b.fd = (INT16S) img_decode_struct->image_source;
		gif_fs_cmd_b.buf_addr = ((INT32U) gif_input_buf) + (C_GIF_DEC_FS_BUFFER_SIZE>>1);
		gif_fs_cmd_b.buf_size = C_GIF_DEC_FS_BUFFER_SIZE>>1;
		gif_fs_cmd_b.result_queue = image_task_fs_queue_b;

		// Seek file pointer to begin
		lseek((INT16S) img_decode_struct->image_source, 0, SEEK_SET);
	} else {
		gif_source_buffer_start = img_decode_struct->image_source;
		gif_source_buffer_end = img_decode_struct->image_source + img_decode_struct->source_size;
	}

	// Initiate gif decode engine
	gif_dec_init(gif_working_memory, gif_input_buf);
	gif_write_index = 0;
  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
	gif_need_invert = 1;
  #endif
	// Get GIF file and parse header
   	do {
   		if (gif_input_data_read()) {
   			break;
   		}

   		status = gif_dec_parsing_file_header(gif_working_memory, gif_write_index, 1);

	} while (status == C_GIF_STATUS_NOT_DONE) ;
	if (status != C_GIF_STATUS_OK) {
		DBG_PRINT("Parse header failed. Skip this file\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);

	  	return STATUS_FAIL;
	}

	still_or_motion = gif_dec_get_still_or_motion(gif_working_memory);
	gif_width = (INT32U) gif_dec_get_screen_width(gif_working_memory);
	gif_height = (INT32U) gif_dec_get_screen_height(gif_working_memory);
	if (!gif_width || !gif_height) {
		DBG_PRINT("Gif width or height is 0. Skip this file\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);

	  	return STATUS_FAIL;
	}

#if (defined LIMIT_IMAGE_WIDTH_HEIGHT) && (LIMIT_IMAGE_WIDTH_HEIGHT == CUSTOM_ON)
	if(gif_width * gif_height > IMAGE_WIDTH_HEIGHT_LIMITATION)
	{
		DBG_PRINT("Do not support this gif!\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);

	  	return STATUS_FAIL;
	}
#endif

	if (gif_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
		// Seek file pointer to begin
		lseek((INT16S) img_decode_struct->image_source, 0, SEEK_SET);
	} else {
		gif_source_buffer_start = img_decode_struct->image_source;
	}

	// Initiate gif decode engine
	gif_dec_init(gif_working_memory, gif_input_buf);
	gif_write_index = 0;
  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
	gif_need_invert = 1;
  #endif
	// Get GIF file and parse header again. What a chore!
   	do {
		if (gif_input_data_read()) {
   			break;
   		}

   		status = gif_dec_parsing_file_header(gif_working_memory, gif_write_index, 0);

	} while (status == C_GIF_STATUS_NOT_DONE) ;


	do {
		status = gif_dec_parsing_frame_header(gif_working_memory, gif_write_index);
   		if (gif_input_data_read()) {
   			break;
   		}

	} while (status == C_GIF_STATUS_NOT_DONE) ;


	interlace_flag = gif_dec_get_interlace(gif_working_memory);

	//DBG_PRINT(", W=%d, H=%d %s\r\n", gif_width, gif_height, still_or_motion? "Motion":"Still");

	gif_working_line_buffer = (INT32U) gp_malloc_align(gif_width<<1, 16);
	if (!gif_working_line_buffer) {
		DBG_PRINT("Failed to allocate gif_working_line_buffer\r\n");
		img_decode_struct->decode_status = -2;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);

		return -2;
	}
	gif_dec_set_decoder_bmpbuffer(gif_working_memory, (INT16S *) gif_working_line_buffer);

	if (!still_or_motion && !interlace_flag) {
		scaler_mode = C_SCALER_CTRL_FIFO_32LINE;
		gif_photo_buffer = (INT32U) gp_malloc_align((gif_width*(C_GIF_DECODE_BUFF_LINE*2))<<1, 16);
	} else {
		scaler_mode = C_SCALER_CTRL_FIFO_DISABLE;
		gif_photo_buffer = (INT32U) gp_malloc_align((gif_width*gif_height)<<1, 16);
	}
	if (!gif_photo_buffer) {
		DBG_PRINT("Failed to allocate gif_photo_buffer\r\n");
		img_decode_struct->decode_status = -2;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);
		gp_free((void *) gif_working_line_buffer);

		return -2;
	}

	if (status != C_GIF_STATUS_OK) {
		DBG_PRINT("Parse header failed. Skip this file\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) gif_input_buf);
		gp_free((void *) gif_working_memory);
		gp_free((void *) gif_working_line_buffer);
		gp_free((void *) gif_photo_buffer);

	  	return STATUS_FAIL;
	}

	// Initiate Scaler
  	scaler_init();
  	scaler_done = 0;

	// Setup
	gif_scaler_set_parameters(gif_width, gif_height, gif_photo_buffer, &(img_decode_struct->decode_status), scaler_mode);
	if (img_decode_struct->decode_status == STATUS_FAIL) {
		return STATUS_FAIL;
	}

   	decode_line = 0;
   	i = 0;
   	gif_buff_size = C_GIF_DECODE_BUFF_LINE*gif_width*2;

   	// Now start GIF decoding on the fly
  	while (1) {
		if (gif_input_data_read()) {
   			break;
   		}

  		if (!still_or_motion) {
			pos_x = gif_dec_get_X_offset(gif_working_memory);
			if (pos_x > gif_width-1) {
				pos_x = 0;
			}
			pos_y = gif_dec_get_next_ypos(gif_working_memory);
			if (pos_y > gif_height) {
				pos_y = gif_height;
			}
  			if (!interlace_flag) {
  				status = gif_dec_run(gif_working_memory, (INT16S *) (gif_photo_buffer + (((pos_y & 0x1F) * gif_width) << 1) + pos_x + ((i & 0x1) * gif_buff_size)), gif_write_index);
  			} else {
  				status = gif_dec_run(gif_working_memory, (INT16S *) (gif_photo_buffer+(pos_y*gif_width<<1)+pos_x), gif_write_index);
  			}
  		} else {
  			status = gif_dec_run(gif_working_memory, (INT16S *) gif_photo_buffer, gif_write_index);
  		}

		if (status == C_GIF_STATUS_FRAME_HEADER) {
			//DBG_PRINT("C_GIF_STATUS_FRAME_HEADER\r\n");
		} else if (status > 0) {
			// A line is ended
			//DBG_PRINT("A line is done\r\n");
			decode_line++;

		} else if (status == C_GIF_STATUS_END_OF_FRAME) {
			if (still_or_motion || interlace_flag) {
				//if (!gif_dec_get_X_offset(gif_working_memory) && !gif_dec_get_Y_offset(gif_working_memory)) {
					break;
				//}
			}
		} else if (status == C_GIF_STATUS_OK || status == C_GIF_STATUS_ALLOC_SRAM) {
			//DBG_PRINT("C_GIF_STATUS_OK\r\n");
		} else if (status == C_GIF_STATUS_END_OF_IMAGE) {
			if (still_or_motion || interlace_flag) {
				break;
			}
		} else {
			DBG_PRINT("gif_dec_run error code=0x%X\r\n", status);
			DBG_PRINT("i=0x%X ,  pos_y=0x%X \r\n", i, pos_y);
			DBG_PRINT("still_or_motion=0x%X \r\n", still_or_motion);
			break;
		}

		// Now start scaler
		if ((!still_or_motion && !interlace_flag) && (decode_line == C_GIF_DECODE_BUFF_LINE || status == C_GIF_STATUS_END_OF_FRAME || status == C_GIF_STATUS_END_OF_IMAGE || status == C_GIF_STATUS_ALLOC_SRAM)) {
	  		scaler_status = scaler_wait_idle();
			if (scaler_status == C_SCALER_STATUS_STOP) {
				scaler_start();
			} else if (scaler_status & C_SCALER_STATUS_DONE) {
				break;
			} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
				DBG_PRINT("Scaler failed to finish its job\r\n");
				break;
			} else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) {
			  	if (scaler_restart()) {
					DBG_PRINT("Failed to call scaler_restart\r\n");
					break;
				}
			} else {
		  		DBG_PRINT("Un-handled Scaler status!\r\n");
		  		break;
			}
	  		decode_line = 0;
	  		i++;
		}
  	}

	if (still_or_motion || interlace_flag) {
		while (!scaler_done) {
	  		scaler_status = scaler_wait_idle();
			if (scaler_status == C_SCALER_STATUS_STOP) {
				scaler_start();
			} else if (scaler_status & C_SCALER_STATUS_DONE) {
				break;
			} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
				DBG_PRINT("Scaler failed to finish its job\r\n");
				break;
			} else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) {
		  		if (scaler_restart()) {
					DBG_PRINT("Failed to call scaler_restart\r\n");
					break;
				}
			} else {
		  		DBG_PRINT("Un-handled Scaler status!\r\n");
		 		break;
		  	}
	  	}
	}

  	scaler_stop();

  	// Free working memory
	gp_free((void *) gif_input_buf);
	gp_free((void *) gif_working_memory);
	gp_free((void *) gif_working_line_buffer);
	gp_free((void *) gif_photo_buffer);

	if (scaler_status & C_SCALER_STATUS_DONE) {
		img_decode_struct->decode_status = STATUS_OK;
		img_decode_struct->output_image_width = gif_output_image_width;
		img_decode_struct->output_image_height = gif_output_image_height;
	    if (!gif_output_buffer_pointer) {
	        img_decode_struct->output_buffer_pointer = gif_scaler_out_buffer;
	    }
	    cache_invalid_range(gif_scaler_out_buffer, (gif_output_buffer_width*gif_output_buffer_height)<<1);

	} else {
    	img_decode_struct->decode_status = STATUS_FAIL;
    	if (!gif_output_buffer_pointer) {
    	    gp_free((void *) gif_scaler_out_buffer);
    	}
        return STATUS_FAIL;
	}

	return STATUS_OK;
}
