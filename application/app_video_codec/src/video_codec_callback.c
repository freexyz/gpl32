#include "video_codec_callback.h"
#include "frdbm.h"
#include "fd.h"

static INT8U sensor_frame_end_action = SENSOR_FEA_VIDEO_ENCODE;
INT32U video_codec_show_buffer;
INT32U video_codec_display_flag;
#if VIDEO_DISPALY_WITH_PPU == 1
PPU_REGISTER_SETS turnkey_ppu_register_structure_1;
PPU_REGISTER_SETS *video_ppu_register_set;
#endif

#if FACE_DETECTION == 1
INT32U face_buffer_ck=0,old_frame,buffer_set=0;
INT32U sensor_buffer_post,sensor_buffer_post_flag;
#endif

extern void ISR_ServiceGpid(void);

//=======================================================================
//  Video decode end hook function
//=======================================================================
//=======================================================================================
//		Video decode stop hook function
//=======================================================================================
void video_decode_end(void)
{
	DBG_PRINT("AVI Decode play-end callback...\r\n");
}

//=======================================================================================
//		Video decode output buffer hook function
//=======================================================================================
void video_decode_FrameReady(INT8U *FrameBufPtr)
{
#if	VIDEO_DISPALY_WITH_PPU == 0
    video_codec_show_buffer = (INT32U)FrameBufPtr;
 	video_codec_display_flag = 1;
#else	//display with PPU
	INT16U width, height;
	video_decode_get_display_size(&width, &height);
	gplib_ppu_text_calculate_number_array(video_ppu_register_set, C_PPU_TEXT1, width, height, (INT32U)FrameBufPtr);
	gplib_ppu_go(video_ppu_register_set);
#endif
}


//=======================================================================
//  Video encode hook function
//=======================================================================
//=======================================================================
//		mipi isr function
//=======================================================================
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)
#include "drv_l1_mipi.h"

void mipi_sot_sync_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_hd_1bit_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_hd_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_crc_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_sof(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_isr_enable(INT32U enable)
{
	mipi_user_isr_register(SOT_SYNC_ERR0_IDX, mipi_sot_sync_err);
	mipi_set_irq_enable(enable, SOT_SYNC_ERR0_IDX);

	mipi_user_isr_register(HD_ERR_1BIT_IDX, mipi_hd_1bit_err);
	mipi_set_irq_enable(enable, HD_ERR_1BIT_IDX);

	mipi_user_isr_register(HD_ERR_IDX, mipi_hd_err);
	mipi_set_irq_enable(enable, HD_ERR_IDX);

	mipi_user_isr_register(CRC_ERR_IDX, mipi_crc_err);
	mipi_set_irq_enable(enable, CRC_ERR_IDX);

	mipi_user_isr_register(SOF_FLAG_IDX, mipi_sof);
	mipi_set_irq_enable(enable, SOF_FLAG_IDX);
}
#endif

//=======================================================================
//		sensor start
//=======================================================================
INT32U video_encode_sensor_start(INT16U width, INT16U height, INT32U csi_frame1, INT32U csi_frame2)
{
	OS_CPU_SR cpu_sr;

	// Setup CMOS sensor
	OS_ENTER_CRITICAL();
	CSI_Init(width, height, FT_CSI_YUVIN|FT_CSI_YUVOUT|FT_CSI_RGB1555, csi_frame1, csi_frame2);
	OS_EXIT_CRITICAL();
	user_defined_set_sensor_irq(1);
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)
	mipi_isr_enable(1);
#endif
  	return 0;
}

//=======================================================================
//		sensor stop
//=======================================================================
INT32U video_encode_sensor_stop(void)
{
  	//close sensor
	user_defined_set_sensor_irq(0);
	R_CSI_TG_CTRL0 = 0;
	R_CSI_TG_CTRL1 = 0;

#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)
	mipi_uninit();
	mipi_isr_enable(0);
#endif
    return 0;
}

//=======================================================================
//		preview frame buffer ready
//=======================================================================
INT32S video_encode_display_frame_ready(INT32U frame_buffer)
{
#if FACE_DETECTION == 1
	if (Face_Detect_Demo_En) {
		sensor_buffer_post_flag = 0;
		if (train_counter == 1) {
			if (ftrain_end()) {
				ftrain_start(frame_buffer);
			}

		} else {
			if (face_buffer_ck) {
				old_frame = face_detect_return();
				if (old_frame) {
					face_frame_set(frame_buffer);
					sensor_buffer_post	= old_frame;
					sensor_buffer_post_flag = 1;
				}
			} else {
				face_frame_set(frame_buffer);
				face_buffer_ck          = 1;
				sensor_buffer_post_flag = 1;
				buffer_set              = 1;
			}
		}
	}

	if (!sensor_buffer_post_flag)
		pic_face(frame_buffer);
#endif

#if VIDEO_DISPALY_WITH_PPU == 0
	video_codec_display_flag = 1;
  	video_codec_show_buffer = frame_buffer;
#else	//display with ppu
	INT16U width, height;
	video_encode_get_display_size(&width, &height);
	gplib_ppu_text_calculate_number_array(video_ppu_register_set, C_PPU_TEXT1, width, height, frame_buffer);	// Calculate Number array
	gplib_ppu_go(video_ppu_register_set);
	//gplib_ppu_go_and_wait_done(video_ppu_register_set);
	//gplib_ppu_go_without_wait(video_ppu_register_set);
#endif

#if FACE_DETECTION == 1
	if (Face_Detect_Demo_En) {
		if (buffer_set == 1) {
			buffer_set = 2;
			return 1;
		} else {
			if (sensor_buffer_post_flag)
				return sensor_buffer_post;
			else
				return frame_buffer;
		}
	} else {
		return frame_buffer;
	}
#else
	return 0;
#endif
}

//=======================================================================
//		encode frame buffer ready when use user define mode
//=======================================================================
INT32S  video_encode_frame_ready(void *workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag)
{
	if(fourcc == 0x63643030)
	{	//"00dc", video frame ready

	}
	else if(fourcc == 0x62773130)
	{	//"01wb", audio frame ready

	}
	else
		return STATUS_FAIL;

	return STATUS_OK;
}

//=======================================================================
//		avi encode end,
// ISRC: source; IART: Artist; ICOP: copyright; IARL: achival location
// ICMS: commissioned; ICMT: comments;  ICRD: create date; ICRP: cropped
// IDIM: dimensions; IDPI: dots per inch; IENG: engineer; IGNR: genre
// IKEY: keyword; ILGT: lightness; IMED: medium; INAM: name;
// IPLT: palette setting; IPRD: product; ISFT: software; ISHP: sharpness
// ISRC: source; ISRF: source form; ITCH: technician
//=======================================================================
void video_encode_end(void *workmem)
{
	//add description to avi file
	//int AviPackerV3_AddInfoStr(const char *fourcc, const char *info_string);
	AviPackerV3_AddInfoStr(workmem, "ISRC", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "IART", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "ICOP", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "ICRD", "2010-06-29");
}

//--------------------------------------------------------------------
//	user can add special effect in audio or video when use video encode
//
//---------------------------------------------------------------------
#if AUDIO_SFX_HANDLE
extern INT16S   *pcm_bg_out[];
extern OS_EVENT    *aud_bg_send_q;

INT32U video_encode_audio_sfx(INT16U *PCM_Buf, INT32U cbLen)
{
	INT32S i, index;
	INT32U Temp;
	OS_Q_DATA OSQData;
	AUDIO_ARGUMENT aud_arg;

	aud_arg.Main_Channel = 0;

	if(audio_decode_status(aud_arg) == AUDIO_CODEC_PROCESSING)
	{
		OSQQuery(aud_bg_send_q, &OSQData);
		index = OSQData.OSNMsgs-1;
		if(index<0)
			index = 0;

		for(i = 0; i<(cbLen/2) ; i++)
		{
			Temp = (INT16U) pcm_bg_out[index][i];
			Temp +=  PCM_Buf[i];
			PCM_Buf[i] = Temp >> 1;
		}
	}

	return (INT32U)PCM_Buf;
}
#endif

#if VIDEO_SFX_HANDLE
INT32U video_encode_video_sfx(INT32U buf_addr, INT32U cbLen)
{
	INT32U i;
	INT8U *pdata = (INT8U *)buf_addr;

	for(i=0; i<cbLen ;i++)
	{
		if(i % 4 == 1)
		 *(pdata + i) <<= 1;
	}

	return (INT32U)pdata;
}
#endif

//=======================================================================
//	Set Sensor irq enable/disable.
//=======================================================================
void user_defined_set_sensor_irq(INT32U enable)
{
	if(enable > 0) {
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
		R_PPU_IRQ_STATUS = 0x40;
		R_PPU_IRQ_EN |= 0x40;	//enable csi frame end irq
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_TGR_IRQ_STATUS = 0x1;
		R_TGR_IRQ_EN = 0x1;	//enable csi frame end irq
	#elif (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ)
		R_TGR_IRQ_STATUS = 0x21;
		R_TGR_IRQ_EN = 0x21;	//enable csi fifo irq, fifo under run and frame end irq
	#endif
	} else {
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
	 	R_PPU_IRQ_EN &= ~0x40;
	 	R_PPU_IRQ_STATUS = 0x40;
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_TGR_IRQ_EN &= ~0x1;
		R_TGR_IRQ_STATUS = 0x1;
	#elif (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ)
		R_TGR_IRQ_EN &= ~0x21;
		R_TGR_IRQ_STATUS = 0x31;
	#endif
	}
}

//=======================================================================
//	Set the action to take inside sensor frame end interrupt
//=======================================================================
void user_defined_set_sensor_fea(INT8U action)
{
	sensor_frame_end_action = action;
}

//=======================================================================
//	tft and sensor isr handle
//=======================================================================
void user_defined_video_codec_isr(void)
{
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x2000) {
		// tft Vertical-Blanking interrupt
		R_PPU_IRQ_STATUS |= 0x2000;
        if(video_codec_display_flag == 1) {
          video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)video_codec_show_buffer, C_DISPLAY_DEVICE, DISPLAY_OUTPUT_FORMAT);
           video_codec_display_flag = 0;
        }
	}

	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0800) {
		// tv Vertical-Blanking interrupt
		R_PPU_IRQ_STATUS |= 0x0800;
        if(video_codec_display_flag == 1) {
          video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)video_codec_show_buffer, C_DISPLAY_DEVICE, DISPLAY_OUTPUT_FORMAT);
          video_codec_display_flag = 0;
        }
	}

	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0040) {
		// csi frame end irq
		R_PPU_IRQ_STATUS |= 0x0040;
		if (sensor_frame_end_action == SENSOR_FEA_OID) {
			ISR_ServiceGpid();
		} else {
			video_encode_auto_switch_csi_frame();
		}
	}

	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x1000) {
		//tft under run
		R_PPU_IRQ_STATUS |= 0x1000;
		while(1);
	}

	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0400) {
		//tv under run
		R_PPU_IRQ_STATUS |= 0x0400;
		while(1);
	}

	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x8000) {
		//sensor under run
		R_PPU_IRQ_STATUS |= 0x8000;
		while(1);
	}

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)
	csi_tg_irq_handle();
#endif
}

void csi_tg_irq_handle(void)
{
#if (CSI_IRQ_MODE & CSI_IRQ_TG_IRQ)
	INT32U status;
	status = R_TGR_IRQ_STATUS;

	if(status & R_TGR_IRQ_EN) {
		if(status & R_TGR_IRQ_EN & 0x10) {
			// fifo under run
			R_TGR_IRQ_STATUS |= 0x10;
			while(1);
		} else if(status & R_TGR_IRQ_EN & 0x20) {
			// fifo end
			R_TGR_IRQ_STATUS |= 0x20;
	   		video_encode_auto_switch_csi_fifo_end();
		}

		if(status & R_TGR_IRQ_EN & 0x01) {
			// frame end
			R_TGR_IRQ_STATUS |= 0x01;
		#if CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
			if (sensor_frame_end_action == SENSOR_FEA_OID) {
				ISR_ServiceGpid();
			} else {
				video_encode_auto_switch_csi_frame();
			}
		#else	//CSI_IRQ_TG_FIFO_IRQ
			video_encode_auto_switch_csi_fifo_frame_end();
		#endif
		}
	}
#endif
}

void user_defined_video_codec_entrance(void)
{
#if VIDEO_DISPALY_WITH_PPU == 0
	video_codec_display_flag = 0;
	R_PPU_IRQ_EN = 0;			// disable all ppu interrupt
	R_PPU_IRQ_STATUS = 0x7FFF;	// Clear all PPU interrupts
	vic_irq_register(VIC_PPU, user_defined_video_codec_isr);

	if(C_DISPLAY_DEVICE >= C_TV_QVGA) {
		R_PPU_IRQ_EN = 0x0800; // TV Vertical-Blanking IRQ enable register
	} else {
		R_PPU_IRQ_EN = 0x2000; // TFT Vertical-Blanking IRQ enable register
	}
	vic_irq_enable(VIC_PPU);
#else
	video_codec_ppu_configure();
#endif

#if (CSI_IRQ_MODE & CSI_IRQ_TG_IRQ)
	R_TGR_IRQ_EN = 0;			// disable all csi interrupt
	R_TGR_IRQ_STATUS = 0xFF;	// Clear all csi interrupts
#endif

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	vic_irq_register(VIC_26, csi_tg_irq_handle);
	vic_irq_enable(VIC_26);
#endif
}

void video_codec_show_image(INT32U TV_TFT, INT32U BUF, INT32U DISPLAY_MODE ,INT32U SHOW_TYPE)
{
	INT32U reg = 0;

	switch(SHOW_TYPE)
	{
   	case IMAGE_OUTPUT_FORMAT_RGB565:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else if(DISPLAY_MODE == C_TV_D1) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
   			R_PPU_ENABLE= PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE;
   		}
   		break;

   	case IMAGE_OUTPUT_FORMAT_RGBG:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else if(DISPLAY_MODE == C_TV_D1) {
        	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
        	R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        }
   		break;

   	case IMAGE_OUTPUT_FORMAT_GRGB:
   		if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else if(DISPLAY_MODE == C_TV_D1) {
        	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        }
   		break;

   	case IMAGE_OUTPUT_FORMAT_UYVY:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
   		}
   		break;

   	case IMAGE_OUTPUT_FORMAT_YUYV:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
   		}
   		break;
	}

	if (TV_TFT >= C_TV_QVGA) {
		R_TV_FBI_ADDR = BUF;
	} else {
		R_TFT_FBI_ADDR = BUF;
	}
}

#if VIDEO_DISPALY_WITH_PPU == 1
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "sprite_dataSP_HDR.h"

void video_codec_ppu_configure(void)
{
	INT16U ppu_frame_width, ppu_frame_height;
	INT32S i, j;
	INT32U frame_size, buffer_ptr;

	/* initial ppu register parameter set structure */
   	video_ppu_register_set = &turnkey_ppu_register_structure_1;
	gplib_ppu_init(video_ppu_register_set);
	gplib_ppu_enable_set(video_ppu_register_set, 1);							// Enable PPU
	gplib_ppu_char0_transparent_set(video_ppu_register_set, 1);					// Enable character 0 transparent mode

#if 1	// TV/tft frame mode
  	gplib_ppu_non_interlace_set(video_ppu_register_set, 0);			            // Set interlace mode
  	gplib_ppu_frame_buffer_mode_set(video_ppu_register_set, 1, 0);		        // Enable TV/TFT frame buffer mode
#else	// TFT line mode
	gplib_ppu_non_interlace_set(video_ppu_register_set, 0);          		 	// Set non-interlace mode
	gplib_ppu_frame_buffer_mode_set(video_ppu_register_set, 0, 1);    			// Enable TFT line mode
#endif
	gplib_ppu_fb_format_set(video_ppu_register_set, 1, 1);			            // Set PPU output frame buffer format to YUYV

   	if(DISPLAY_DEVICE == DISPLAY_TV)	//TV
   	{
   		if(TV_TFT_MODE == QVGA_MODE)
    	{
    		ppu_frame_width = 320;
   			ppu_frame_height = 240;
    		gplib_ppu_vga_mode_set(video_ppu_register_set, 0);						// Disable VGA mode
    		gplib_ppu_resolution_set(video_ppu_register_set, C_TV_RESOLUTION_320X240);// Set display resolution to 320X240
    	}
   		else if(TV_TFT_MODE == VGA_MODE)
   		{
   			ppu_frame_width = 640;
   			ppu_frame_height = 480;
    		gplib_ppu_vga_mode_set(video_ppu_register_set, 1);						// Enable VGA mode
    		gplib_ppu_resolution_set(video_ppu_register_set, C_TV_RESOLUTION_640X480);// Set display resolution to 640X480
    	}
    	else
		{
			while(1);
		}
    }
    else if(DISPLAY_DEVICE == DISPLAY_TFT)//tft
    {
    	gplib_ppu_vga_mode_set(video_ppu_register_set, 0);							// Disable VGA mode
    	if(TV_TFT_MODE == TFT_320x240_MODE)
    	{
    		ppu_frame_width = 320;
   			ppu_frame_height = 240;
    		gplib_ppu_resolution_set(video_ppu_register_set, C_TFT_RESOLUTION_320X240);
    	}
    	else if(TV_TFT_MODE == TFT_640x480_MODE)
    	{
    		ppu_frame_width = 640;
   			ppu_frame_height = 480;
    		gplib_ppu_resolution_set(video_ppu_register_set, C_TFT_RESOLUTION_640X480);
    	}
    	else if(TV_TFT_MODE == TFT_800x480_MODE)
    	{
    		ppu_frame_width = 800;
   			ppu_frame_height = 480;
	    	gplib_ppu_resolution_set(video_ppu_register_set, C_TFT_RESOLUTION_800X480);	// Set display resolution to 800X480
		}
		else
		{
			while(1);
		}
    }

    gplib_ppu_bottom_up_mode_set(video_ppu_register_set, 0);                    // Disable bottom to top
    gplib_ppu_palette_type_set(video_ppu_register_set, 0, 1);                   // text and sprite palette mode
    gplib_ppu_text_direct_mode_set(video_ppu_register_set, 0);			        // Enable TEXT direct address mode

    gplib_ppu_long_burst_set(video_ppu_register_set, 0);						// Disable PPU long burst
    if(DISPLAY_DEVICE == DISPLAY_TFT)//tft
    	gplib_ppu_tft_long_burst_set(video_ppu_register_set, 1);    			// Enable TFT long burst

    gplib_ppu_deflicker_mode_set(0);			            					// Disable De-flicker is only valid under VGA + Interlace + Frame mode
    gplib_ppu_yuv_type_set(video_ppu_register_set, 3);							// Set 32-bit color format to Y1UY0V

    //palette ram init
    gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 1, (INT32U)_sprite_dataSP_Palette0);
    gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 3, (INT32U)_sprite_dataSP_Palette1);

    frame_size = ppu_frame_width * ppu_frame_height * 2;
    for (i=0; i<PPU_FRAME_NO; i++)
    {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		DBG_PRINT("ppu_frame = 0x%x\r\n", buffer_ptr);
		if (buffer_ptr)
		{
	    	INT32U *ptr;
	    	ptr = (INT32U *) buffer_ptr;
	    	for (j=0; j<frame_size; j++)
	    	{
	    	    *ptr++ = 0x00800080;              // Black for YUYV color mode
	    	}
			gplib_ppu_frame_buffer_add(video_ppu_register_set, buffer_ptr);
		}
		else
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
	}

	/* display at text1 or video decode at text1*/
	buffer_ptr = (INT32U) gp_malloc(1024*4);	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	if (!buffer_ptr)
	{
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
		while(1);
	}

	gplib_ppu_text_enable_set(video_ppu_register_set, C_PPU_TEXT1, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(video_ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
 	gplib_ppu_text_direct_mode_set(video_ppu_register_set, 0);			                // Enable TEXT direct address mode
	gplib_ppu_text_attribute_source_select(video_ppu_register_set, C_PPU_TEXT1, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(video_ppu_register_set, C_PPU_TEXT1, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(video_ppu_register_set, C_PPU_TEXT1, 0);				    // Set TEXT segment address

	gplib_ppu_text_number_array_ptr_set(video_ppu_register_set, C_PPU_TEXT1, buffer_ptr); // Set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(video_ppu_register_set, C_PPU_TEXT1, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(video_ppu_register_set, C_PPU_TEXT1, 1, 3);				     // Set TEXT color to YUYV

	if(DISPLAY_DEVICE == DISPLAY_TV)	//TV
	{
		gplib_ppu_text_mode_set(video_ppu_register_set, C_PPU_TEXT1, 0);                    // TEXT 2D
	}
	else if(DISPLAY_DEVICE == DISPLAY_TFT)//tft
	{
		gplib_ppu_text_mode_set(video_ppu_register_set, C_PPU_TEXT1, 0);					// TEXT 2D
		//gplib_ppu_text_mode_set(video_ppu_register_set, C_PPU_TEXT1, 1);                  // TEXT rotate
		//gplib_ppu_text_rotate_zoom_set(video_ppu_register_set, C_PPU_TEXT1, 0, 1.25);
	}

	gplib_ppu_text_position_set(video_ppu_register_set, C_PPU_TEXT1, 0, 0);
	gplib_ppu_text_offset_set(video_ppu_register_set, C_PPU_TEXT1, 0, 0);

	gplib_ppu_sprite_init(video_ppu_register_set);
	gplib_ppu_sprite_enable_set(video_ppu_register_set, 0);			                     	// Disable Sprite
}
#endif
