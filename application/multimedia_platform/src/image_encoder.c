/*
* Description: This task encodes YCbCr or YUYV data into JPEG variable length coded data
*
* Author: Cater Chen
*
* Date: 2008/10/28
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "image_encoder.h"

#define quant_table_user_define      (((INT32U)1)<<27)
void image_encode_entrance(void);
void image_encode_exit(void);
INT32U image_encode_start(IMAGE_ENCODE_ARGUMENT arg);
void image_block_write_task_entry(void *p_arg);
CODEC_START_STATUS image_encode_block_write(ENCODE_BLOCK_WRITE_ARGUMENT *encode_info);
void image_encode_stop(void);
IMAGE_CODEC_STATUS image_encode_status(void);

static INT16U image_valid_width, image_valid_height;
static INT32U y_addr, u_addr, v_addr;
IMAGE_ENCODE_PTR block_encode_ptr;
INT8U header_ck,ck_count,block_wr_num;
INT16S encode_fd;
INT32U encode_status,process_status,use_disk;
INT32U fly_y_addr, fly_u_addr, fly_v_addr, y_len, uv_len;
INT32U image_encode_start(IMAGE_ENCODE_ARGUMENT arg);
INT32U BLOCK_READ=0,encode_count,task_create=0;
//TASK define for encode block write
MSG_Q_ID encode_msg_queue_id;
#define ENCODE_QUEUE_MAX_NUMS   		    16
#define ENCODE_MAXIMUM_MESSAGE_NUM			16
#define ENCODE_MAX_MSG_SIZE                0x20
#define BLOCK_WRITE_START                    1
#define BlockWriteStackSize                 512
#define TSK_ENCODE_BLOCK_WRITE              24
INT32U block_write_para[ENCODE_MAX_MSG_SIZE/4 + 1];
INT32U BlockWriteStack[BlockWriteStackSize];

void image_encode_entrance(void)
{
   if(task_create==0){
      //Create a Encode Block Write task
      OSTaskCreate(image_block_write_task_entry, (void *) 0, &BlockWriteStack[BlockWriteStackSize - 1], TSK_ENCODE_BLOCK_WRITE);
      task_create=1;
   }
   encode_status=0;
   process_status=0;
}						

void image_encode_exit(void)
{	
	OSTaskDel(TSK_ENCODE_BLOCK_WRITE);
	msgQDelete(encode_msg_queue_id);
    task_create=0;
}	

INT32U image_encode_start(IMAGE_ENCODE_ARGUMENT arg)
{
	INT8U source_format, input_mode, sampling_mode, MCU;
	INT32U fly_y_addr, fly_u_addr, fly_v_addr, y_len, uv_len,i,buf_ptr_ck,sum_encode_size,blocklength;
	INT32U encode_output_ptr,encode_size,malloc_error,msg_id,OutputBufPtr_1,OutputBufPtr_2,buffer_chage,quality;
	INT32S jpeg_status;
	IMAGE_ENCODE_ARGUMENT image_encode;
	ENCODE_BLOCK_WRITE_ARGUMENT image;

	if(process_status==0){
		//global value init
		process_status=1;
		encode_count=1;
		//local value init
		buf_ptr_ck=0;
		sum_encode_size=0;
		//encode struct ptr
		image_encode=arg;
		malloc_error=0;
		//encode information
		image_valid_width = image_encode.InputWidth;
		image_valid_height = image_encode.InputHeight;
		if(image_encode.InputFormat==IMAGE_ENCODE_INPUT_FORMAT_YUYV) { // C_JPEG_FORMAT_YUV_SEPARATE or C_JPEG_FORMAT_YUYV
		    source_format = C_JPEG_FORMAT_YUYV;
			MCU = GPL326XX;
		} else if(image_encode.InputFormat==IMAGE_ENCODE_INPUT_FORMAT_YUYV_SCALE) {
		#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
			DBG_PRINT("Not Support IMAGE_ENCODE_INPUT_FORMAT_YUYV_SCALE!!!\r\n");
			return -1;
		#else
			source_format = C_JPEG_FORMAT_YUYV;
			MCU = GPL327XX;
		#endif
		} else if(image_encode.InputFormat == IMAGE_ENCODE_INPUT_FORMAT_YUV_SEPARATE) {
		#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
			source_format = C_JPEG_FORMAT_YUV_SEPARATE; 
		#else
			DBG_PRINT("Not Support IMAGE_ENCODE_INPUT_FORMAT_YUV_SEPARATE!!!\r\n");
			return -1;
		#endif
		} else {
			DBG_PRINT("InputFormat Error!!!\r\n");
			return -1;
		}
		  	
		input_mode = image_encode.EncodeMode;				// 0=read once, 1=read on-the-fly
		if(image_encode.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422) {
		    sampling_mode = C_JPG_CTRL_YUV422;		        // C_JPG_CTRL_YUV422 or C_JPG_CTRL_YUV420
		} else if(image_encode.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV420) {
	    	sampling_mode = C_JPG_CTRL_YUV420;
		} else {
			DBG_PRINT("OutputFormat Error!!!\r\n");
			return -1;
		}
		   
		switch(image_encode.quantizationquality) 
		{
		case 100:
			quality=100;
			break;

		case 98:
			quality=98;
			break;	       

		case 95:
			quality=95;
			break;

		case 93:
			quality=93;
			break;	       

		case 90:
			quality=90;
			break;

		case 85:
			quality=85;
			break;	       

		case 80:
			quality=80;
			break;

		case 70:
			quality=70;
			break;	

		case 50:
			quality=50;
			break;	

		default:
			quality=(INT32U)&image_encode;
			quality |= quant_table_user_define;
			break;           
		}   	        
		    
		encode_fd=image_encode.FileHandle;
		use_disk=image_encode.UseDisk;
		 
		blocklength=image_encode.BlockLength;
		if(!blocklength){
		   blocklength=image_encode.scalerinfoptr->OutputHeight;
		}
		 
		if (source_format == C_JPEG_FORMAT_YUYV) {		// YUYV
			y_addr = (INT32U)image_encode.InputBufPtr.yaddr;
			u_addr = 0;
			v_addr = 0;
			if (!y_addr){
			    y_addr = (INT32U) gp_malloc_align((image_valid_width*image_valid_height)*2, 32); 
			    buf_ptr_ck=1;
			}    
			if (!y_addr) {
				gp_free((void *) y_addr);
				DBG_PRINT("JPEG FORMAT YUYV malloc buffer error!\r\n");
				malloc_error=1;
			}
		} else if (sampling_mode == C_JPG_CTRL_YUV422) {
            y_addr = (INT32U)image_encode.InputBufPtr.yaddr;
			u_addr = (INT32U)image_encode.InputBufPtr.uaddr;
			v_addr = (INT32U)image_encode.InputBufPtr.vaddr;	
            // Allocate memory for JPEG encode output ptr
            if (!y_addr || !u_addr || !v_addr) {
			   y_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength), 32);
			   u_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength)>>1, 32);
			   v_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength)>>1, 32);
			   buf_ptr_ck=1;
            }
			if (!y_addr || !u_addr || !v_addr) {
				gp_free((void *) y_addr);
				gp_free((void *) u_addr);
				gp_free((void *) v_addr);
				DBG_PRINT("YUV422 malloc buffer error!\r\n");
				malloc_error=1;
			}
		} else {		// C_JPG_CTRL_YUV420
			y_addr = (INT32U)image_encode.InputBufPtr.yaddr;
			u_addr = (INT32U)image_encode.InputBufPtr.uaddr;
			v_addr = (INT32U)image_encode.InputBufPtr.vaddr;
			// Allocate memory for JPEG encode output ptr
			if (!y_addr || !u_addr || !v_addr) {
			    y_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength), 32);
			    u_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength)>>2, 32);
			    v_addr = (INT32U) gp_malloc_align((image_valid_width*blocklength)>>2, 32);
			    buf_ptr_ck=1;
			}
			if (!y_addr || !u_addr || !v_addr) {
				gp_free((void *) y_addr);
				gp_free((void *) u_addr);
				gp_free((void *) v_addr);
				DBG_PRINT("YUV420 malloc buffer error!\r\n");
				malloc_error=1;
			}
		}
	
		// Allocate memory for JPEG encode output ptr
		encode_output_ptr = (INT32U)image_encode.OutputBufPtr;
		if(!encode_output_ptr){
		    if(input_mode == IMAGE_ENCODE_ONCE_READ)
	      	   encode_output_ptr=(INT32U) gp_malloc_align((image_valid_width*image_valid_height)*2, 32);
	      	else 
	      	   encode_output_ptr=(INT32U) gp_malloc_align((image_valid_width*blocklength), 32);
	    }  	   
		if (!encode_output_ptr) {
			gp_free((void *) encode_output_ptr);
			DBG_PRINT("encode_output_ptr malloc buffer error!\r\n");
			malloc_error=1;
		}
		   
		if(malloc_error==0) {
			if((input_mode==IMAGE_ENCODE_ONCE_READ) || (input_mode==IMAGE_ENCODE_BLOCK_READ)){
			     // Setup JPEG encode engine
			    jpeg_encode_init();
			    
			    gplib_jpeg_default_quantization_table_load(quality);		// Load default qunatization table(quality=50)
			    gplib_jpeg_default_huffman_table_load();			        // Load default huffman table 
			    jpeg_encode_input_size_set(image_valid_width, image_valid_height);
			    jpeg_encode_input_format_set(source_format);
			    jpeg_encode_yuv_sampling_mode_set(sampling_mode);
			    jpeg_encode_output_addr_set((INT32U) encode_output_ptr);	
		    }
			if (input_mode == IMAGE_ENCODE_ONCE_READ) {	// Enocde once
				jpeg_encode_once_start(y_addr, u_addr, v_addr);
				while(1){
					jpeg_status = jpeg_encode_status_query(1);
				  	if (jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
				    	encode_size = jpeg_encode_vlc_cnt_get();		// Get encode length
				      	cache_invalid_range(encode_output_ptr, encode_size);
				      	image_encode_end(encode_size);
				      	encode_status=1;
				      	break;
				  	}else if (jpeg_status & C_JPG_STATUS_STOP) {
				      	DBG_PRINT("JPEG is not started!\r\n");
				      	break;
				  	}else if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
				      	DBG_PRINT("JPEG execution timeout!\r\n");
				      	break;
				  	}else if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
				      	DBG_PRINT("JPEG init error!\r\n");
				      	break;
				  	}else {
				      	DBG_PRINT("jpeg_status = 0x%08x!\r\n", (INT32U)jpeg_status);
				      	DBG_PRINT("JPEG status error!\r\n");
				  	}
				  	OSTimeDly(1);
				}	
			} else if(input_mode == IMAGE_ENCODE_BLOCK_READ){		// Encode on the fly
		  		if(source_format==C_JPEG_FORMAT_YUV_SEPARATE) {
		            y_len = (image_valid_width*blocklength);		  		// Encode 16 lines each time
	  	            if (sampling_mode == C_JPG_CTRL_YUV422) {
	  		            uv_len = y_len >> 1;
	  	            } else {		// YUV420
	  		            uv_len = y_len >> 2;
		            }
	  	            block_encode_ptr.yaddr=(INT8U *)y_addr;
		            block_encode_ptr.uaddr=(INT8U *)u_addr;
		            block_encode_ptr.vaddr=(INT8U *)v_addr;
	  	            image_encoded_block_read(image_encode.scalerinfoptr,&block_encode_ptr);
	  	            fly_y_addr = (INT32U)block_encode_ptr.yaddr;
	  	            fly_u_addr = (INT32U)block_encode_ptr.uaddr;
	  	            fly_v_addr = (INT32U)block_encode_ptr.vaddr;
		            jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, y_len+uv_len+uv_len);
	  	            while (1) {
	  		               jpeg_status = jpeg_encode_status_query(1);
			                 if (jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
				                 encode_size = jpeg_encode_vlc_cnt_get();	// Get encode length
				                 cache_invalid_range(encode_output_ptr, encode_size);
				                 image_encode_end(encode_size);
				                 encode_status=1;
				                 BLOCK_READ=1;
				                 break;
			                 }
	  		                if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
				                // Prepare new data for JPEG encoding
				                image_encoded_block_read(image_encode.scalerinfoptr,&block_encode_ptr);
				                encode_count++;
				                DBG_PRINT("encode_count = 0x%08x!\r\n", (INT32U)encode_count);
				                // Now restart JPEG encoding on the fly
		  		                if (jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, y_len+uv_len+uv_len)) {
		  			                 DBG_PRINT("Failed to call jpeg_encode_on_the_fly_start()\r\n");
		  			                 break;
		  		                }
			                }		// if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY)
			                if (jpeg_status & C_JPG_STATUS_STOP) {
				                DBG_PRINT("JPEG is not started!\r\n");
				                break;
			                }
			                if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
				                DBG_PRINT("JPEG execution timeout!\r\n");
				                break;
			                }
			                if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
				                DBG_PRINT("JPEG init error!\r\n");
				                break;
			                }
		                    OSTimeDly(1);
		            }
				} else if(source_format==C_JPEG_FORMAT_YUYV && MCU == GPL326XX){
		            y_len = (image_valid_width*blocklength);		  		// Encode 16 lines each time
	  	            if (source_format == C_JPEG_FORMAT_YUYV) {
	  		            y_len <<= 1;
	  		            uv_len = 0;
	  	            } else if (sampling_mode == C_JPG_CTRL_YUV422) {
	  		            uv_len = y_len >> 1;
	  	            } else {		// YUV420
	  		            uv_len = y_len >> 2;
		            }
	  	            fly_y_addr = y_addr;
	  	            fly_u_addr = u_addr;
	  	            fly_v_addr = v_addr;
		       	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	    			jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, y_len+uv_len+uv_len);
	    		#else    
	        		jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, (y_len+uv_len+uv_len)<<1);
	    		#endif
	  	            while (1) {
	  		             jpeg_status = jpeg_encode_status_query(1);
			                if (jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
				                encode_size = jpeg_encode_vlc_cnt_get();	// Get encode length
				                cache_invalid_range(encode_output_ptr, encode_size);
				                image_encode_end(encode_size);
				                encode_status=1;
				                BLOCK_READ=1;
				                break;
			                }				
	  		                if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
				                // Prepare new data for JPEG encoding
				                fly_y_addr += y_len;
				                fly_u_addr += uv_len;
				                fly_v_addr += uv_len;
				                encode_count++;
				                DBG_PRINT("encode_count = 0x%08x!\r\n", (INT32U)encode_count);
				                // Now restart JPEG encoding on the fly
			                #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)    
		  		                if (jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr,  y_len+uv_len+uv_len)) {
		  		                  	DBG_PRINT("Failed to call jpeg_encode_on_the_fly_start()\r\n");
		  			                break;
		  		                }
		  		            #else
		  		                if (jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr,  (y_len+uv_len+uv_len)<<1)) {
		  		            		DBG_PRINT("Failed to call jpeg_encode_on_the_fly_start()\r\n");
		  			                break;
		  		                }
		  		            #endif      
			                }		// if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY)
			                if (jpeg_status & C_JPG_STATUS_STOP) {
				                DBG_PRINT("JPEG is not started!\r\n");
				                break;
			                }
			                if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
				                DBG_PRINT("JPEG execution timeout!\r\n");
				                break;
			                }
			                if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
			 	                DBG_PRINT("JPEG init error!\r\n");
				                break;
			                }
			                OSTimeDly(1);
		            }
			    } else if(source_format==C_JPEG_FORMAT_YUYV && MCU == GPL327XX){
		            y_len = (image_valid_width*blocklength);		  		// Encode 16 lines each time
	  	            if (source_format == C_JPEG_FORMAT_YUYV) {
	  		            y_len <<= 1;
	  		            uv_len = 0;
	  	            } else if (sampling_mode == C_JPG_CTRL_YUV422) {
	  		            uv_len = y_len >> 1;
	  	            } else {		// YUV420
	  		            uv_len = y_len >> 2;
		            }
	  	           	
	  	            block_encode_ptr.yaddr=(INT8U *)y_addr;
		            block_encode_ptr.uaddr=(INT8U *)u_addr;
		            block_encode_ptr.vaddr=(INT8U *)v_addr;
	  	            image_encoded_block_read(image_encode.scalerinfoptr,&block_encode_ptr);
	  	            fly_y_addr = (INT32U)block_encode_ptr.yaddr;
	  	            fly_u_addr = (INT32U)block_encode_ptr.uaddr;
	  	            fly_v_addr = (INT32U)block_encode_ptr.vaddr;
		            jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, (y_len+uv_len+uv_len) << 1);
	  	            while (1) {
						jpeg_status = jpeg_encode_status_query(1);
						if (jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
							encode_size = jpeg_encode_vlc_cnt_get();	// Get encode length
							cache_invalid_range(encode_output_ptr, encode_size);
							image_encode_end(encode_size);
							encode_status=1;
							BLOCK_READ=1;
							break;
						}
						if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
							// Prepare new data for JPEG encoding
							image_encoded_block_read(image_encode.scalerinfoptr,&block_encode_ptr);
							encode_count++;
							DBG_PRINT("encode_count = 0x%08x!\r\n", (INT32U)encode_count);
							// Now restart JPEG encoding on the fly
							if (jpeg_encode_on_the_fly_start(fly_y_addr, fly_u_addr, fly_v_addr, (y_len+uv_len+uv_len) << 1)) {
								DBG_PRINT("Failed to call jpeg_encode_on_the_fly_start()\r\n");
								break;
							}
						}		// if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY)
						if (jpeg_status & C_JPG_STATUS_STOP) {
							DBG_PRINT("JPEG is not started!\r\n");
							break;
						}
						if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
							DBG_PRINT("JPEG execution timeout!\r\n");
							break;
						}
						if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
							DBG_PRINT("JPEG init error!\r\n");
							break;
						}
						OSTimeDly(1);
		            }
		       	} 
  			} else if(input_mode == IMAGE_ENCODE_BLOCK_READ_WRITE) {
				block_wr_num=(image_valid_height/blocklength)+1;
				header_ck=0xD0;
				ck_count=0;
				buffer_chage=0;
				OutputBufPtr_1=encode_output_ptr;
				OutputBufPtr_2=(INT32U) gp_malloc_align((image_valid_width*blocklength), 32);
				if (!OutputBufPtr_2) {
					gp_free((void *) OutputBufPtr_2);
				 	malloc_error=1;
				}
				if(malloc_error==0) {
	        		for(i=0;i<block_wr_num;i++) { 	
	  		            // Setup JPEG encode engine 
		                jpeg_encode_init();
					    gplib_jpeg_default_quantization_table_load(quality);		// Load default qunatization table(quality=50)
		                gplib_jpeg_default_huffman_table_load();			        // Load default huffman table 
	                    jpeg_encode_input_size_set(image_valid_width, blocklength);
		                jpeg_encode_input_format_set(source_format);	
		                jpeg_encode_yuv_sampling_mode_set(sampling_mode);
		                jpeg_encode_output_addr_set((INT32U) encode_output_ptr);
	  		            block_encode_ptr.yaddr=(INT8U *)y_addr;
			            block_encode_ptr.uaddr=(INT8U *)u_addr;
			            block_encode_ptr.vaddr=(INT8U *)v_addr;
		  	            image_encoded_block_read(image_encode.scalerinfoptr,&block_encode_ptr);
	  		            jpeg_encode_once_start(y_addr, u_addr, v_addr);
			            while(1){
							jpeg_status = jpeg_encode_status_query(1);
							if (jpeg_status & C_JPG_STATUS_ENCODE_DONE) {
							   encode_size = jpeg_encode_vlc_cnt_get();		// Get encode length
							   cache_invalid_range(encode_output_ptr, encode_size);
							   jpeg_encode_stop();
							   encode_count++;
							   DBG_PRINT("encode_count = 0x%08x!\r\n", (INT32U)encode_count);
							   break;
							} else if (jpeg_status & C_JPG_STATUS_STOP) {
							   DBG_PRINT("JPEG is not started!\r\n");
							   break;
							} else if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
							   DBG_PRINT("JPEG execution timeout!\r\n");
							   break;
							} else if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
							   DBG_PRINT("JPEG init error!\r\n");
							   break;
							} else {
							   DBG_PRINT("jpeg_status = 0x%08x!\r\n", (INT32U)jpeg_status);
							   DBG_PRINT("JPEG status error!\r\n");
							}
							OSTimeDly(1);
		                }
		                image.encodebufferptr=encode_output_ptr;
		                image.encodesize=encode_size;
	  	                msgQSend(encode_msg_queue_id, BLOCK_WRITE_START, (void *)&image,  sizeof(ENCODE_BLOCK_WRITE_ARGUMENT), MSG_PRI_NORMAL);   
		                sum_encode_size+=encode_size;
		                msgQReceive(ApQ, &msg_id, (void *) &block_write_para, sizeof(ENCODE_BLOCK_WRITE_ARGUMENT));  
		                if(msg_id==START_OK) {
		                   if(buffer_chage==0) {
		                      encode_output_ptr=OutputBufPtr_2;
		                      buffer_chage=1;
		                   } else {
		                      encode_output_ptr=OutputBufPtr_1;
		                      buffer_chage=0;
		                   } 
		                } else {
		                   DBG_PRINT("encode block write status error!\r\n");
		                   break;
		                }      
		        	}
		  	    }else{
		  	    	DBG_PRINT("OutputBufPtr_2 malloc buffer error!\r\n");
		  	    }
			}
		}
		
		if((input_mode==IMAGE_ENCODE_ONCE_READ) || (input_mode==IMAGE_ENCODE_BLOCK_READ)) {
			jpeg_encode_stop();
		}
		if(input_mode==IMAGE_ENCODE_BLOCK_READ_WRITE) {
			encode_status=1;
		   	BLOCK_READ=1;
		   	gp_free((void *) OutputBufPtr_2);
		}
		if(buf_ptr_ck==1) {
			if(y_addr) gp_free((void *) y_addr);
		   	if(u_addr) gp_free((void *) u_addr);
		   	if(v_addr) gp_free((void *) v_addr);
		}
		process_status=0;
		if(malloc_error==1) {
		    return RESOURCE_NO_FOUND_ERROR; 
		} else {    
		    if(input_mode==IMAGE_ENCODE_BLOCK_READ_WRITE) {
		       return sum_encode_size;
		    } else {
		       return encode_size;
		    }   
		}  
	} else {
		return REENTRY_ERROR;
  	} 
}

void image_block_write_task_entry(void *p_arg)
{
    INT32U block_write_state,msg_id;
    
    encode_msg_queue_id = msgQCreate(ENCODE_QUEUE_MAX_NUMS, ENCODE_MAXIMUM_MESSAGE_NUM, ENCODE_MAX_MSG_SIZE);
    
    while(1){
         msgQReceive(encode_msg_queue_id, &msg_id, (void *) block_write_para, ENCODE_MAX_MSG_SIZE);
         if(msg_id==BLOCK_WRITE_START){
            block_write_state=image_encode_block_write((ENCODE_BLOCK_WRITE_ARGUMENT *)block_write_para);
            msgQSend(ApQ, block_write_state, (void *) block_write_para, sizeof(ENCODE_BLOCK_WRITE_ARGUMENT), MSG_PRI_NORMAL);
         }
         OSTimeDly(1);
    }


}

CODEC_START_STATUS image_encode_block_write(ENCODE_BLOCK_WRITE_ARGUMENT *encode_info)
{
	  INT8U *ptr_ck;
	  INT32U size_ck,encode_size_ck,i,write_state_error;
	  INT32S disk_size;            
	  	                   
	  ptr_ck=(INT8U *)encode_info->encodebufferptr;
	  size_ck=encode_info->encodesize%4;      	                                      	                                      	                                      	                             
	  ptr_ck+=(encode_info->encodesize-2);
	  write_state_error=0;
	  
	  if(ck_count++ < block_wr_num){
	          if(size_ck==1){
	              for(i=0;i<3;i++){
	                  *ptr_ck=0x0;
	                  ptr_ck++;
	               }
	              *ptr_ck=0xFF;
	              ptr_ck++;
	              *ptr_ck=header_ck;
	              encode_size_ck=encode_info->encodesize+3;
	          }else if(size_ck==2){
	              for(i=0;i<2;i++){
	                  *ptr_ck=0x0;
	                  ptr_ck++;
	              }
	              *ptr_ck=0xFF;
	              ptr_ck++;
	              *ptr_ck=header_ck;
	              encode_size_ck=encode_info->encodesize+2;
	          }else{ 
                  for(i=0;i<5;i++){
                      *ptr_ck=0x0;
                      ptr_ck++;
	              }         
	              *ptr_ck=0xFF;
	              ptr_ck++;
	              *ptr_ck=header_ck;
	              encode_size_ck=encode_info->encodesize+5;
              }
      }
        
	  if (encode_fd >= 0) {
		   disk_size=vfsFreeSpace(use_disk);
		   if(disk_size > encode_size_ck){
		     write(encode_fd, encode_info->encodebufferptr, encode_size_ck);
		   }else{
		     write_state_error=1;
		     DBG_PRINT("sd card free size error!\r\n");
		   }  
	  }else{
	       DBG_PRINT("sd card write error!\r\n");
	       write_state_error=1;
	  }
	  
	  if(header_ck++ > 0xD6)
		 header_ck=0xD0;

	  if(ck_count==block_wr_num)
	      close(encode_fd);
	     
	     
      if(write_state_error==0)
         return START_OK;
      else
         return RESOURCE_WRITE_ERROR; 	 
}


void image_encode_stop(void)
{
    jpeg_encode_stop();
}

IMAGE_CODEC_STATUS image_encode_status(void)
{
    if(encode_status==1)
       return IMAGE_CODEC_DECODE_END;
    else
       return IMAGE_CODEC_DECODE_FAIL;

    encode_status=0;
}

