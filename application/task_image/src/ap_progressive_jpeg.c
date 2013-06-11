/*
* Description: This file parse/decode/scale progressive jpeg images
*
* Author: Tristan Yang
*
* Date: 2008/09/30
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "ap_progressive_jpeg.h"

static TK_FILE_SERVICE_STRUCT pjpeg_fs_cmd_a, pjpeg_fs_cmd_b;
static INT32U pjpeg_cmd_id;
static INT8U pjpeg_source_type;
#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
static INT8U pjpeg_need_invert;
#endif
static INT32U pjpeg_source_buffer_start, pjpeg_source_buffer_end;
static INT8U *pjpeg_working_memory;
static INT8U *pjpeg_input_buf;
static INT32S pjpeg_write_index;
static INT8U read_file_end;


INT32S progressive_jpeg_read_data(void);

INT32S progressive_jpeg_read_data(void)
{
	INT32S read_count;
	INT32S read_index;
	INT8U error;

	// Check whether we have to break decoding this image
	if (image_task_handle_remove_request(pjpeg_cmd_id) > 0) {
		return -1;
	}

	read_index = progressive_jpeg_dec_get_ri(pjpeg_working_memory);
	if (read_file_end) {
		if (read_index != pjpeg_write_index) {
			return 0;
		}
		return -1;
	}

    if (read_index < pjpeg_write_index) {
    	if (C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE+read_index-pjpeg_write_index <= C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2) {
    		// No need to fill buffer now
	    	return 0;
	    }
    } else if (read_index > pjpeg_write_index) {
    	if (read_index-pjpeg_write_index <= C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2) {
	    	// No need to fill buffer now
	    	return 0;
	    }
    }

	if (pjpeg_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {				// Read and decode JPEG from file system
		if (pjpeg_write_index == 0) {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &pjpeg_fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			read_count = (INT32S) OSQPend(pjpeg_fs_cmd_a.result_queue, 200, &error);
		} else {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &pjpeg_fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			read_count = (INT32S) OSQPend(pjpeg_fs_cmd_b.result_queue, 200, &error);
		}
		if ((error!=OS_NO_ERR) || (read_count<0)) {
			DBG_PRINT("Failed to read progressive file\r\n");

			return -1;
		}

	  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
		if (pjpeg_need_invert) {
			INT8U *ptr8;

			if (pjpeg_write_index == 0) {
				ptr8 = (INT8U *) pjpeg_fs_cmd_a.buf_addr;
			} else {
				ptr8 = (INT8U *) pjpeg_fs_cmd_b.buf_addr;
			}
			if (*ptr8==0x00 && *(ptr8+1)==0x27) {
				INT32U *ptr32;
				INT8U i;

				if (pjpeg_write_index == 0) {
					ptr32 = (INT32U *) pjpeg_fs_cmd_a.buf_addr;
				} else {
					ptr32 = (INT32U *) pjpeg_fs_cmd_b.buf_addr;
				}
				for (i=0; i<128; i++) {
					*ptr32++ ^= 0xFFFFFFFF;
				}
			}
			pjpeg_need_invert = 0;
		}
	  #endif

	} else {		// Decode a JPEG buffer
  		if (pjpeg_source_buffer_start >= pjpeg_source_buffer_end) {
  			read_count = 4;
  		} else {
	  		read_count = pjpeg_source_buffer_end - pjpeg_source_buffer_start;
	  		if (read_count > C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2) {
	  			read_count = C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2;
	  		}
	  	}
	  	if (pjpeg_write_index == 0) {
  			gp_memcpy((INT8S *) pjpeg_input_buf, (INT8S *) pjpeg_source_buffer_start, read_count);
  		} else {
  			gp_memcpy((INT8S *) pjpeg_input_buf + (C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2), (INT8S *) pjpeg_source_buffer_start, read_count);
  		}

	  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
		if (pjpeg_need_invert) {
			INT8U *ptr8;

			ptr8 = (INT8U *) pjpeg_source_buffer_start;
			if (*ptr8==0x00 && *(ptr8+1)==0x27) {
				INT32U *ptr32;
				INT8U i;

				ptr32 = (INT32U *) pjpeg_source_buffer_start;
				for (i=0; i<128; i++) {
					*ptr32++ ^= 0xFFFFFFFF;
				}
			}
			pjpeg_need_invert = 0;
		}
	  #endif

  		pjpeg_source_buffer_start += read_count;

	}

	if (read_count != C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2) {
		read_file_end = 1;
		progressive_jpeg_set_file_end(pjpeg_working_memory);
	}

	pjpeg_write_index += C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2;
	if (pjpeg_write_index == C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE) {
		pjpeg_write_index = 0;
	}

	return 0;
}

INT32S jpeg_decode_progressive(IMAGE_DECODE_STRUCT *img_decode_struct)
{
  	INT32S jpeg_status;
  	INT32S scaler_status;
	INT8U scaler_done;

	INT32S i, component;
	INT32S get_header;
	INT8U *coef_mem;
	INT32S scan_end, file_end, MCUrow_cnt;
	INT8U *YCbCr[3], *YCbCr_Img[3];
	INT32S width_in_block[3], height_in_block[3], YCbCr_Img_Size[3];
	INT32S YCbCr_Size[3];
	INT32U hv[3], v[3];
	INT16U hsize, vsize;
	INT32U scaler_mode, coef_size, mode, pjpg_mode;
	INT32U cnt;
	INT16U *p_quant_tbl;
	INT32U padding_blocks_h[3], padding_blocks_v[3];

	pjpeg_input_buf = (INT8U *) gp_malloc_align(C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE, 32);
	if (!pjpeg_input_buf) {
		DBG_PRINT("pjpeg_input_buf is NULL\r\n");

		return -2;
	}

	pjpeg_working_memory = gp_malloc(C_PROGRESSIVE_JPEG_WORKING_MEMORY_SIZE);
	if (pjpeg_working_memory == NULL) {
		DBG_PRINT("wording memory error!\r\n");
		gp_free((void *) pjpeg_input_buf);

		return -2;
	}

	pjpeg_cmd_id = img_decode_struct->cmd_id;
	pjpeg_source_type = img_decode_struct->source_type;
	if (pjpeg_source_type == TK_IMAGE_SOURCE_TYPE_FILE) {
		pjpeg_fs_cmd_a.fd = (INT16S) img_decode_struct->image_source;
		pjpeg_fs_cmd_a.buf_addr = (INT32U) pjpeg_input_buf;
		pjpeg_fs_cmd_a.buf_size = C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE / 2;
		pjpeg_fs_cmd_a.result_queue = image_task_fs_queue_a;

		pjpeg_fs_cmd_b.fd = (INT16S) img_decode_struct->image_source;
		pjpeg_fs_cmd_b.buf_addr = ((INT32U) pjpeg_input_buf) + (C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE/2);
		pjpeg_fs_cmd_b.buf_size = C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE / 2;
		pjpeg_fs_cmd_b.result_queue = image_task_fs_queue_b;

		lseek((INT16S) img_decode_struct->image_source, 0, SEEK_SET);

	} else {
		pjpeg_source_buffer_start = img_decode_struct->image_source;
		pjpeg_source_buffer_end = img_decode_struct->image_source + img_decode_struct->source_size;
	}

	// Read file header, set default decompression parameters
	progressive_jpeg_dec_init(pjpeg_working_memory, pjpeg_input_buf);
	read_file_end = 0;
	pjpeg_write_index = 0;
  #if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT==GP_FILE_FORMAT_SET_1)
	pjpeg_need_invert = 1;
  #endif
	if (progressive_jpeg_read_data()) {
		gp_free((void *) pjpeg_input_buf);
		gp_free((void *) pjpeg_working_memory);

		return -1;
	}
	get_header = 0;
	do {
		INT32S ret;

		ret = progressive_jpeg_parsing(pjpeg_working_memory, pjpeg_write_index);
		switch (ret) {
		case JPEG_HEADER_OK:
			get_header = 1;
			break;

		case JPEG_BS_NOT_ENOUGH:
			if (progressive_jpeg_read_data()) {
				gp_free((void *) pjpeg_input_buf);
				gp_free((void *) pjpeg_working_memory);

				return -1;
			}
			break;

		case JPEG_SUSPENDED:
		case JPEG_REACHED_EOI:
			DBG_PRINT("file end!!!\r\n");
			gp_free((void *) pjpeg_input_buf);
			gp_free((void *) pjpeg_working_memory);

			return -1;
		}
	} while (!get_header);

	hsize = progressive_jpeg_get_width(pjpeg_working_memory);
	vsize = progressive_jpeg_get_height(pjpeg_working_memory);
	DBG_PRINT("W=%d H=%d Progressive ", hsize, vsize);

	component = progressive_jpeg_get_component(pjpeg_working_memory);
	if (component!=1 && component!=3) {
		DBG_PRINT("Can't not handle Component = %d!\r\n", component);
		gp_free((void *) pjpeg_input_buf);
		gp_free((void *) pjpeg_working_memory);

		return -1; // CMYK
	}

	YCbCr[0] = ((void *) 0);
	YCbCr[1] = ((void *) 0);
	YCbCr[2] = ((void *) 0);
	YCbCr_Img[0] = ((void *) 0);
	YCbCr_Img[1] = ((void *) 0);
	YCbCr_Img[2] = ((void *) 0);

	for (i=0; i<component; i++) {
		INT32U h, size;

		hv[i] = progressive_jpeg_get_hv(pjpeg_working_memory, i);
		width_in_block[i] = progressive_jpeg_get_width_in_blocks(pjpeg_working_memory, i);
		height_in_block[i] = progressive_jpeg_get_height_in_blocks(pjpeg_working_memory, i);
		h = hv[i] >> 4;
		v[i] = hv[i] & 0x0F;

		padding_blocks_h[i] = width_in_block[i] % h;
		padding_blocks_v[i] = height_in_block[i] % v[i];

		size = ((width_in_block[i] + padding_blocks_h[i]) * v[i]) << 6;
		YCbCr_Size[i] = size;
		YCbCr[i] = gp_malloc_align(size, 8);
		if (YCbCr[i] == NULL) {
			DBG_PRINT("Memory allocate for YCbCr[%d] fail!\r\n", i);
			gp_free((void *) pjpeg_input_buf);
			gp_free((void *) pjpeg_working_memory);
			if (YCbCr[0] != NULL)
				gp_free(YCbCr[0]);
			if (YCbCr[1] != NULL)
				gp_free(YCbCr[1]);
			if (YCbCr[2] != NULL)
				gp_free(YCbCr[2]);

			if (YCbCr_Img[0] != NULL)
				gp_free(YCbCr_Img[0]);
			if (YCbCr_Img[1] != NULL)
				gp_free(YCbCr_Img[1]);
			if (YCbCr_Img[2] != NULL)
				gp_free(YCbCr_Img[2]);

			return -2;
		}

		size = (width_in_block[i] + padding_blocks_h[i]) * (height_in_block[i] + padding_blocks_v[i]) << 6;
		YCbCr_Img_Size[i] = size;
		YCbCr_Img[i] = gp_malloc(size);
		if (YCbCr_Img[i] == NULL) {
			DBG_PRINT("Memory allocate for YCbCr_Img[%d] fail!\r\n", i);
			gp_free((void *) pjpeg_input_buf);
			gp_free((void *) pjpeg_working_memory);
			if (YCbCr[0] != NULL)
				gp_free(YCbCr[0]);
			if (YCbCr[1] != NULL)
				gp_free(YCbCr[1]);
			if (YCbCr[2] != NULL)
				gp_free(YCbCr[2]);

			if (YCbCr_Img[0] != NULL)
				gp_free(YCbCr_Img[0]);
			if (YCbCr_Img[1] != NULL)
				gp_free(YCbCr_Img[1]);
			if (YCbCr_Img[2] != NULL)
				gp_free(YCbCr_Img[2]);

			return -2;
		}

		gp_memset((void *) YCbCr_Img[i], 0x80, size);
	}

	mode = 0xFFFFFFFF;

	if (component == 1) {
		pjpg_mode = C_JPG_CTRL_GRAYSCALE;
		scaler_mode = C_SCALER_CTRL_IN_Y_ONLY;
		DBG_PRINT("GrayScale\r\n");
	} else {		 // component == 3
		if (hv[0]==0x21 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV422;
			scaler_mode = C_SCALER_CTRL_IN_YUV422;
			DBG_PRINT("YUV422\r\n");
		} else if (hv[0]==0x11 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV444;
			scaler_mode = C_SCALER_CTRL_IN_YUV444;
			DBG_PRINT("YUV444\r\n");
		} else if (hv[0]==0x22 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV420;
			scaler_mode = C_SCALER_CTRL_IN_YUV420;
			DBG_PRINT("YUV420\r\n");
		} else if (hv[0]==0x41 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV411;
			scaler_mode = C_SCALER_CTRL_IN_YUV411;
			DBG_PRINT("YUV411\r\n");
		} else if (hv[0]==0x12 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV422V;
			scaler_mode = C_SCALER_CTRL_IN_YUV422V;
			DBG_PRINT("YUV422V\r\n");
		} else if (hv[0]==0x14 && hv[1]==0x11 && hv[2]==0x11) {
			pjpg_mode = C_JPG_CTRL_YUV411V;
			scaler_mode = C_SCALER_CTRL_IN_YUV411V;
			DBG_PRINT("YUV411V\r\n");
		} else {
			DBG_PRINT("Can't not handle YUV format hv[0]=0x%x hv[1]=0x%x hv[2]=0x%x\r\n", hv[0], hv[1], hv[2]);
			gp_free((void *) pjpeg_input_buf);
			gp_free((void *) pjpeg_working_memory);
			for (i=0; i<component; i++) {
				gp_free(YCbCr[i]);
				gp_free(YCbCr_Img[i]);
			}

			return -1;
		}
	}

	// Fill quantization table
	p_quant_tbl = progressive_jpeg_get_quant_tbl(pjpeg_working_memory, 0);
	jpeg_quant_luminance_set(0, 64, p_quant_tbl);
	p_quant_tbl = progressive_jpeg_get_quant_tbl(pjpeg_working_memory, 1);
	jpeg_quant_chrominance_set(0, 64, p_quant_tbl);

	coef_size = progressive_jpeg_get_coefmemsize(pjpeg_working_memory);
	coef_mem = gp_malloc_align(coef_size, 32);
	if (coef_mem == NULL) {
		DBG_PRINT("Memory allocate for coef_mem fail!\r\n");
		gp_free((void *) pjpeg_input_buf);
		gp_free((void *) pjpeg_working_memory);
		for (i=0; i<component; i++) {
			gp_free(YCbCr[i]);
			gp_free(YCbCr_Img[i]);
		}

		return -2;
	}
	progressive_jpeg_set_coefmem(pjpeg_working_memory, coef_mem);

	file_end = 0;
	scan_end = 0;
	MCUrow_cnt = 0;
	cnt = 0;
	while (1) {
		INT32S hufdec_ok = 0;
		INT32U comp_in_scan;

		do {
			INT32S ret;

			if (progressive_jpeg_read_data()) {
				gp_free((void *) pjpeg_input_buf);
				gp_free((void *) pjpeg_working_memory);
				for (i=0; i<component; i++) {
					gp_free(YCbCr[i]);
					gp_free(YCbCr_Img[i]);
				}
				gp_free(coef_mem);

				return -1;
			}

			// Start decompressor
			ret = progressive_jpeg_hf_dec(pjpeg_working_memory, pjpeg_write_index);
			switch(ret) {
			case JPEG_SCAN_COMPLETED:
				hufdec_ok = 1;
				scan_end = 1;
				break;

			case JPEG_REACHED_EOI:
				hufdec_ok = 1;
				scan_end = 1;
				file_end = 1;
				break;

			case JPEG_ROW_COMPLETED:
				hufdec_ok = 1;
				break;

			case JPEG_PASS_AC_REFINE:
			case JPEG_BS_NOT_ENOUGH:
			case JPEG_MCU_COMPLETED:
			default:
				break;
			}
		} while (!hufdec_ok);

#if 0
		// software IDCT
		progressive_jpeg_dec_idct(pjpeg_working_memory, YCbCr);
		jpeg_status = C_JPG_STATUS_DECODE_DONE;
#else
		{
			INT32U p_Y, p_Cb, p_Cr;
			INT32U comp_id, img_w, img_h, dct_size;

			cnt++;
			comp_in_scan = progressive_jpeg_get_comp_in_scan(pjpeg_working_memory);
			if (comp_in_scan != 1) {
				mode = pjpg_mode;
				p_Y = (INT32U) YCbCr[0];
				p_Cb = (INT32U) YCbCr[1];
				p_Cr = (INT32U) YCbCr[2];

				img_w = (width_in_block[0] + padding_blocks_h[0]) << 3;
				img_h = v[0] << 3;

				dct_size = coef_size;
			} else {
				mode = C_JPG_CTRL_GRAYSCALE;
				comp_id = progressive_jpeg_get_comp_idx(pjpeg_working_memory, 0);
				p_Y = (INT32U) YCbCr[comp_id];
				p_Cb = 0;
				p_Cr = 0;
				img_w = width_in_block[comp_id] << 3;
				img_h = v[comp_id] << 3;

				dct_size = (img_w * img_h) << 1; // 2 BYTE
			}

			// Hardware IDCT
			jpeg_init();
			jpeg_fifo_line_set(C_JPG_FIFO_DISABLE);

			// Set hw for progressive jpeg
			jpeg_progressive_mode_enable();  // Progressive mode enable & DCT coefficients are 2-byte accuracy

			// Set output addr: Y, Cb, Cr
			jpeg_yuv_addr_set(p_Y, p_Cb, p_Cr);

			// Set image size & YUV mode
			jpeg_image_size_set(img_w, img_h);
			jpeg_yuv_sampling_mode_set(mode);

			// Set input addr & size
			jpeg_vlc_addr_set((INT32U)coef_mem);
			jpeg_vlc_maximum_length_set(dct_size);

			jpeg_decompression_start();
			jpeg_status = jpeg_status_polling(1);
			jpeg_stop();

			if (jpeg_status != C_JPG_STATUS_DECODE_DONE) {
				DBG_PRINT("jpeg_status_polling() returns fail! result=0x%x\r\n", jpeg_status);
				gp_free((void *) pjpeg_input_buf);
				gp_free((void *) pjpeg_working_memory);
				for (i=0; i<component; i++) {
					gp_free(YCbCr[i]);
					gp_free(YCbCr_Img[i]);
				}
				gp_free(coef_mem);

				return -1;
			}

			for (i=0; i<comp_in_scan; i++) {
				comp_id = progressive_jpeg_get_comp_idx(pjpeg_working_memory, i);
				cache_invalid_range((INT32U)YCbCr[comp_id], YCbCr_Size[comp_id]);
			}
		}
#endif
		// Reset memory after IDCT
		progressive_jpeg_set_coefmem(pjpeg_working_memory, coef_mem);

		// Fill YCbCr in YCbCr_Img
		comp_in_scan = progressive_jpeg_get_comp_in_scan(pjpeg_working_memory);
		for (i=0; i<comp_in_scan; i++) {
			INT16U id;
			INT8U *p_YCbCr, *p_YCbCrImg;
			INT32S width, height, padding_width, padding_width2;
			INT32S j;
			INT32S v_sample;

			id = progressive_jpeg_get_comp_idx(pjpeg_working_memory, i);
			p_YCbCrImg = YCbCr_Img[id];
			p_YCbCr = YCbCr[id];
			width = width_in_block[id] << 3;
			height = MCUrow_cnt * v[id] << 3;
			padding_width = padding_blocks_h[id] << 3;
			p_YCbCrImg += ((width + padding_width) * height);

			if (comp_in_scan == 1) {
				padding_width2 = 0;
			} else {
				padding_width2 = padding_width;
			}

			// check the last row is v_sample or not
			v_sample = v[id];
			if (scan_end == 1) {
				INT32S tmp;

				tmp = height_in_block[id] % v_sample;
				if (tmp != 0) {
					v_sample = tmp;
				}
			}

			for (j=0; j<v_sample; j++) {
				INT32S x, y;

				for (y=0; y<8; y++) {
					for (x=0; x<width; x++) {
						short tmp = *p_YCbCrImg;
						short tmp2 = *p_YCbCr++;

						tmp -= 128;
						tmp2 -= 128;
						tmp += tmp2;
						if (tmp > 127) {
							tmp = 127;
						} else if (tmp < -128) {
							tmp = -128;
						}
						*p_YCbCrImg++ = (INT8U) (tmp+128);
					}
					p_YCbCrImg += padding_width;
					p_YCbCr += padding_width2;
				}
			}
		}
		MCUrow_cnt++;

		if (scan_end == 1) {
			progressive_jpeg_reset(pjpeg_working_memory);
			scan_end = 0;
			MCUrow_cnt = 0;
		}

		if (file_end) {
			break;
		}
	}

	scaler_init();
	scaler_done = 0;

  #ifdef _DPF_PROJECT
	umi_scaler_color_matrix_load();
  #endif

	scaler_input_pixels_set((width_in_block[0]+padding_blocks_h[0])<<3, (height_in_block[0]+padding_blocks_v[0])<<3);
	scaler_input_visible_pixels_set(hsize, vsize);
	scaler_input_addr_set((INT32U) YCbCr_Img[0], (INT32U) YCbCr_Img[1], (INT32U) YCbCr_Img[2]);
	scaler_input_format_set(scaler_mode);

	scaler_output_pixels_set((hsize<<16)/img_decode_struct->output_image_width, (vsize<<16)/img_decode_struct->output_image_height, img_decode_struct->output_buffer_width, img_decode_struct->output_buffer_height);
	scaler_output_addr_set(img_decode_struct->output_buffer_pointer, NULL, NULL);
	scaler_output_format_set(img_decode_struct->output_format);

   	scaler_fifo_line_set(C_SCALER_CTRL_FIFO_DISABLE);
	scaler_out_of_boundary_mode_set(1);			// Use Use color defined in scaler_out_of_boundary_color_set()
	scaler_out_of_boundary_color_set(img_decode_struct->out_of_boundary_color);

	while (!scaler_done) {
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

	gp_free((void *) pjpeg_input_buf);
  	gp_free((void *) pjpeg_working_memory);
	for (i=0; i<component; i++) {
		gp_free(YCbCr[i]);
		gp_free(YCbCr_Img[i]);
	}
	gp_free(coef_mem);

	return 0;
}
