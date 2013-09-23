#ifndef __AVI_ENCODER_APP_H__
#define __AVI_ENCODER_APP_H__

#include "application.h"
#include "avi_encoder_scaler_jpeg.h"
#include "define.h"

typedef struct AviEncAudPara_s
{
	//audio encode
	INT32U  audio_format;			// audio encode format
	INT16U  audio_sample_rate;		// sample rate
	INT16U  channel_no;				// channel no, 1:mono, 2:stereo
	INT8U   *work_mem;				// wave encode work memory 
	INT32U  pack_size;				// wave encode package size in byte
	INT32U  pack_buffer_addr;
	INT32U  pcm_input_size;			// wave encode pcm input size in short
	INT32U  pcm_input_addr[AVI_ENCODE_PCM_BUFFER_NO];
}AviEncAudPara_t;

typedef struct AviEncVidPara_s
{
	//sensor input 
    INT32U  sensor_output_format;   // sensor output format
    INT16U  sensor_capture_width;   // sensor width
    INT16U  sensor_capture_height;  // sensor height
    INT32U  csi_frame_addr[AVI_ENCODE_CSI_BUFFER_NO];	// sensor input buffer addr
    INT32U  csi_fifo_addr[AVI_ENCODE_CSI_FIFO_NO];		// sensor fifo mode use
    
    //scaler output
    INT16U 	scaler_flag;
    INT16U 	dispaly_scaler_flag;  	// 0: not scaler, 1: scaler again for display 
    FP32    scaler_zoom_ratio;      // for zoom scaler   
    INT32U  scaler_output_addr[AVI_ENCODE_SCALER_BUFFER_NO];	// scaler output buffer addr
      								
    //display scaler
    INT32U  display_output_format;  // for display use
    INT16U  display_width;          // display width
    INT16U  display_height;         // display height
    INT16U  display_buffer_width;   // display width
    INT16U  display_buffer_height;  // display height 
    INT32U  display_output_addr[AVI_ENCODE_DISPALY_BUFFER_NO];	// display scaler buffer addr
    
    //video encode
    INT32U  video_format;			// video encode format
    INT8U   dwScale;				// dwScale
    INT8U   dwRate;					// dwRate 
    INT16U  quality_value;			// video encode quality
    INT16U  encode_width;           // video encode width
    INT16U  encode_height;          // videoe ncode height
    INT32U  video_encode_addr[AVI_ENCODE_VIDEO_BUFFER_NO]; //video encode buffer

#if MPEG4_ENCODE_ENABLE == 1	
	//mpeg4 encode use
	INT32U  isram_addr;
	INT32U  write_refer_addr;
	INT32U  reference_addr;
#endif
}AviEncVidPara_t;

typedef struct AviEncPacker_s
{
	void 	*avi_workmem;
	INT16S  file_handle;
    INT16S  index_handle;
    INT8U   index_path[16];
    
    //avi video info
    GP_AVI_AVISTREAMHEADER *p_avi_vid_stream_header;
    INT32U  bitmap_info_cblen;		// bitmap header length
    GP_AVI_BITMAPINFO *p_avi_bitmap_info;
    
    //avi audio info
    GP_AVI_AVISTREAMHEADER *p_avi_aud_stream_header;
    INT32U  wave_info_cblen;		// wave header length
    GP_AVI_PCMWAVEFORMAT *p_avi_wave_info;
    
    //avi packer 
	INT32U  task_prio;
	void    *file_write_buffer;
	INT32U  file_buffer_size;		// AviPacker file buffer size in byte
	void    *index_write_buffer;
	INT32U	index_buffer_size;		// AviPacker index buffer size in byte
}AviEncPacker_t;

typedef struct VidEncFrame_s
{
	INT32U ready_frame;
	INT32U encode_size;
	INT32U key_flag;
}VidEncFrame_t;

typedef struct AviEncPara_s
{   
	INT8U  source_type;				// SOURCE_TYPE_FS or SOURCE_TYPE_USER_DEFINE
    INT8U  reserved;
    INT8U  fifo_enc_err_flag;
    INT8U  fifo_irq_cnt;
   	
    //avi file info
    INT32U  avi_encode_status;      // 0:IDLE
    AviEncPacker_t *AviPackerCur;
    
	//allow record size
	INT64U  disk_free_size;			// disk free size
	INT32U  record_total_size;		// AviPacker storage to disk total size
    
	//aud & vid sync
	INT64S  delta_ta, tick;
    INT64S  delta_Tv;
    INT32S  freq_div;
    INT64S  ta, tv, Tv;
   	INT32U  pend_cnt, post_cnt;
	INT32U  vid_pend_cnt, vid_post_cnt;
	VidEncFrame_t video[AVI_ENCODE_VIDEO_BUFFER_NO]; 
}AviEncPara_t;

typedef struct ScalerFormat_s
{
	/* img input para */
	INT32U input_format;			/* input format*/
	INT16U input_width;				/* 1~0x1FFF, input image x size */
	INT16U input_height;			/* 1~0x1FFF, input image y size */
	INT16U input_visible_width;		/* 0~0x1FFF, 0 is disable, clip x size*/
	INT16U input_visible_height;	/* 0~0x1FFF, 0 is disable, clip y size */
	INT16U input_x_offset;			/* 0~0x1FFF, x start offset in effective area */
	INT16U input_y_offset;			/* 0~0x1FFF, y start offset in effective area */
	
	INT32U input_addr_y;			/* input y addr, must be 4-align */
	INT32U input_addr_u;			/* input u addr, must be 4-align */
	INT32U input_addr_v;			/* input v addr, must be 4-align */
	
	INT32U output_format; 			/* output format*/
	INT16U output_width; 			/* 1~0x1FFF, must be 8-align, but YUV444/YUV422/YUV420 is 16-align, YUV411 is 32-align */
	INT16U output_height;			/* 1~0x1FFF */
	INT16U output_buf_width;		/* 1~0x1FFF, must be 8-align, but YUV444/YUV422/YUV420 is 16-align, YUV411 is 32-align */
	INT16U output_buf_height;		/* 1~0x1FFF */
	
	INT16U output_x_offset;			/* 0~0x1FFF, must be 8-align, skip x size after every line output */
	INT16U reserved0;
	
	INT32U output_addr_y;			/* output y addr, must be 4-align */
	INT32U output_addr_u;			/* output u addr, must be 4-align */
	INT32U output_addr_v;			/* output v addr, must be 4-align */
	
	/* scale para */
	INT32U fifo_mode;				/* FIFO in or FIFO out mode */
	INT8U  scale_mode;				/* C_SCALER_FULL_SCREEN / C_SCALER_FIT_RATIO.... */
	INT8U  digizoom_m;				/* digital zoom, ratio =  m/n */
	INT8U  digizoom_n;
	INT8U  reserved1;
}ScalerFormat_t;

typedef struct JpegPara_s
{
	INT32U jpeg_status;
	INT32U wait_done;
	INT32U quality_value;
	
	INT32U input_format;
	INT32U width;
	INT32U height;
	INT32U input_buffer_y;
	INT32U input_buffer_u;
	INT32U input_buffer_v;
	INT32U input_y_len;
	INT32U input_uv_len;
	INT32U output_buffer;
}JpegPara_t;

//extern os-event
extern OS_EVENT *AVIEncodeApQ;
extern OS_EVENT *scaler_task_q;
extern OS_EVENT *scaler_frame_q;  
extern OS_EVENT *display_frame_q;
extern OS_EVENT *cmos_frame_q;
extern OS_EVENT *vid_enc_task_q;
extern OS_EVENT *vid_enc_frame_q;
extern OS_EVENT *video_frame_q;
extern OS_EVENT *avi_aud_q;
#if C_UVC == CUSTOM_ON
extern OS_EVENT *usbwebcam_frame_q;
#endif

extern AviEncPara_t *pAviEncPara;
extern AviEncAudPara_t *pAviEncAudPara;
extern AviEncVidPara_t *pAviEncVidPara;
extern AviEncPacker_t *pAviEncPacker0, *pAviEncPacker1;

#if MIC_INPUT_SRC == C_GPY0050_IN
	extern INT8U  g_mic_buffer;
	extern INT32U g_mic_cnt;
#endif
#if C_UVC == CUSTOM_ON
//USB Webcam 
extern OS_EVENT *usbwebcam_frame_q;
extern OS_EVENT  *USBCamApQ;
extern OS_EVENT *USBAudioApQ;
#endif

//mpeg4 encoder
#if MPEG4_ENCODE_ENABLE == 1
	extern void mpeg4_encode_init(void);
	extern INT32S mpeg4_encode_config(INT8U input_format, INT16U width, INT16U height, INT8U vop_time_inc_reso);
	extern void mpeg4_encode_ip_set(INT8U byReg, INT32U IPXXX_No);
	extern INT32S mpeg4_encode_isram_set(INT32U addr);
	extern INT32S mpeg4_encode_start(INT8U ip_frame, INT8U quant_value, INT32U vlc_out_addr, 
									INT32U raw_data_in_addr, INT32U encode_out_addr, INT32U refer_addr);
	extern INT32S mpeg4_encode_get_vlc_size(void);
	extern INT32S mpeg4_wait_idle(INT8U wait_done);
	extern void mpeg4_encode_stop(void);
#endif

//avi encode state
extern INT32U avi_enc_packer_start(AviEncPacker_t *pAviEncPacker);
extern INT32U avi_enc_packer_stop(AviEncPacker_t *pAviEncPacker); 
extern INT32S vid_enc_preview_start(void);
extern INT32S vid_enc_preview_stop(void);
extern INT32S avi_enc_start(void);
extern INT32S avi_enc_stop(void);
extern INT32S avi_enc_pause(void);
extern INT32S avi_enc_resume(void);
extern INT32S avi_enc_save_jpeg(void);
#if C_UVC == CUSTOM_ON
extern INT32S usb_webcam_start(void);
extern INT32S usb_webcam_stop(void);
#endif
extern INT32S avi_enc_storage_full(void);
extern INT32S avi_packer_err_handle(INT32S ErrCode);
extern INT32S avi_encode_state_task_create(INT8U pori);
extern INT32S avi_encode_state_task_del(void);
extern void   avi_encode_state_task_entry(void *para);
extern INT32S  (*pfn_avi_encode_put_data)(void* workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag);

//scaler task
extern INT32S scaler_task_create(INT8U pori);
extern INT32S scaler_task_del(void);
extern INT32S scaler_task_start(void);
extern INT32S scaler_task_stop(void);
extern void   scaler_task_entry(void *parm);

//video_encode task
extern INT32S video_encode_task_create(INT8U pori);
extern INT32S video_encode_task_del(void);
extern INT32S video_encode_task_start(void);
extern INT32S video_encode_task_stop(void);
extern INT32S video_encode_task_post_q(INT32U mode);
extern INT32S video_encode_task_empty_q(void);
extern void   video_encode_task_entry(void *parm);

//audio task
extern INT32S avi_adc_record_task_create(INT8U pori);
extern INT32S avi_adc_record_task_del(void);
extern INT32S avi_audio_record_start(void);
extern INT32S avi_audio_record_restart(void);
extern INT32S avi_audio_record_stop(void);
extern void avi_audio_record_entry(void *parm);

//avi encode api
extern void   avi_encode_init(void);

extern void avi_encode_video_timer_start(void);
extern void avi_encode_video_timer_stop(void);
extern void avi_encode_audio_timer_start(void);
extern void avi_encode_audio_timer_stop(void);

extern INT32S avi_encode_set_file_handle_and_caculate_free_size(AviEncPacker_t *pAviEncPacker, INT16S FileHandle);
extern INT16S avi_encode_close_file(AviEncPacker_t *pAviEncPacker);
extern void avi_encode_fail_handle_close_file(AviEncPacker_t *pAviEncPacker);
extern INT32S avi_encode_set_avi_header(AviEncPacker_t *pAviEncPacker);
extern void avi_encode_set_curworkmem(void *pAviEncPacker);
//status
extern void   avi_encode_set_status(INT32U bit);
extern void   avi_encode_clear_status(INT32U bit);
extern INT32S avi_encode_get_status(void);
//memory
extern INT32U avi_encode_post_empty(OS_EVENT *event, INT32U frame_addr);
extern INT32U avi_encode_get_empty(OS_EVENT *event);
extern INT32U avi_encode_get_csi_frame(void);
extern INT32S avi_encode_memory_alloc(void);
extern INT32S avi_packer_memory_alloc(void);
extern void avi_encode_memory_free(void);
extern void avi_packer_memory_free(void);
#if MPEG4_ENCODE_ENABLE == 1
extern INT32S avi_encode_mpeg4_malloc(INT16U width, INT16U height);
extern void avi_encode_mpeg4_free(void);
#endif

//format
extern void avi_encode_set_sensor_format(INT32U format);
extern void avi_encode_set_display_format(INT32U format);
//other
extern void avi_encode_set_display_scaler(void);
extern INT32S jpeg_header_generate(INT8U q, INT16U w, INT16U h);
extern INT32S avi_encode_set_jpeg_quality(INT8U quality_value);
extern INT32S avi_encode_set_mp4_resolution(INT16U encode_width, INT16U encode_height);
extern BOOLEAN avi_encode_is_pause(void);
extern BOOLEAN avi_encode_disk_size_is_enough(INT32S cb_write_size);

extern void avi_encode_switch_csi_frame_buffer(void);
extern void vid_enc_csi_fifo_end(void);
extern void vid_enc_csi_frame_end(void);

extern INT32S scaler_clip(INT8U wait_done, gpImage *src, gpImage *dst, gpRect *clip);
extern INT32S scaler_once(INT8U wait_done, gpImage *src, gpImage *dst);
extern INT32S scaler_wait_done(void);
extern INT32S scaler_trigger(INT8U wait_done, ScalerFormat_t *pScale, INT32U boundary_color);
						
extern INT32U jpeg_encode_once(JpegPara_t *pJpegEnc);
// jpeg fifo encode
extern INT32S jpeg_encode_fifo_start(JpegPara_t *pJpegEnc);
extern INT32S jpeg_encode_fifo_once(JpegPara_t *pJpegEnc);
extern INT32S jpeg_encode_fifo_stop(JpegPara_t *pJpegEnc);
							
extern INT32S save_jpeg_file(INT16S fd, INT32U encode_frame, INT32U encode_size);
extern INT32S avi_audio_memory_allocate(INT32U	cblen);
extern void avi_audio_memory_free(void);
extern INT32U avi_audio_get_next_buffer(void);

#if MIC_INPUT_SRC == C_ADC_LINE_IN || MIC_INPUT_SRC == C_BUILDIN_MIC_IN
extern INT32S avi_adc_double_buffer_put(INT16U *data,INT32U cwlen, OS_EVENT *os_q);
extern INT32U avi_adc_double_buffer_set(INT16U *data, INT32U cwlen);
extern INT32S avi_adc_dma_status_get(void);
extern void avi_adc_double_buffer_free(void);
extern void avi_adc_hw_start(void);
extern void avi_adc_hw_stop(void);
#endif

#if MIC_INPUT_SRC == C_BUILDIN_MIC_IN
extern void mic_fifo_clear(void);
extern void mic_fifo_level_set(INT8U level);
extern INT32S mic_auto_sample_start(void);
extern void mic_auto_sample_stop(void);
extern INT32S mic_sample_rate_set(INT8U timer_id, INT32U hz);
extern INT32S mic_timer_stop(INT8U timer_id);
#endif

#if MIC_INPUT_SRC == C_GPY0050_IN
extern INT16U avi_adc_get_gpy0050(void);
extern void gpy0050_enable(void);
extern void gpy0050_disable(void);
extern void gpy0050_timer_isr(void);
#endif

#if	AVI_ENCODE_SHOW_TIME == 1
void cpu_draw_osd(const INT8U *source_addr, INT32U target_addr, INT16U offset, INT16U res);	
void cpu_draw_time_osd(TIME_T current_time, INT32U target_buffer, INT16U resolution);
#endif
#endif // __AVI_ENCODER_APP_H__
