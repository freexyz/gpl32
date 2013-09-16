#ifndef __TASK_VIDEO_DECODER_H__
#define __TASK_VIDEO_DECODER_H__

#include "application.h"
#include "task_video_decoder_cfg.h"
#include "MultiMediaParser.h"

typedef struct
{   
	INT32U  status;
    INT32S  file_handle;	//file handle
    INT32S  FileType;
    INT64U  FileSize;
    void  	*media_handle;
   
    // video & audio sync 
    INT64S tick_time;
    INT64S tick_time2;
    INT64S time_range;
    INT32S n;
    INT64S ta, tv, Tv;
   	INT32U fail_cnt;
   	INT32U pend_cnt, post_cnt;
    
    // video 
    INT16U video_flag;
    INT16U user_define_flag;
    INT32U video_format;
    INT32U VidTickRate;
    INT32U video_total_sample;
    INT32U video_total_time;
    INT32U video_cur_time;
	INT32U video_seek_time;
    INT32U video_decode_addr[VIDEO_FRAME_NO];
    
    // deblock 
    INT32U deblock_flag;
    INT32U deblock_addr[DEBLOCK_FRAME_NO];
    
    // speed control
    INT32U play_speed;
    INT8U  reverse_play_set;
    INT8U  reverse_play_flag;
    INT8U  ff_audio_flag;
    
    // audio
    INT8U  audio_flag;
    INT32U aud_frame_size;
    INT32U audio_decode_addr[AUDIO_FRAME_NO];
	
	// audio decoder
	INT8U  *work_mem_addr;
	INT32U work_mem_size;
	INT8U  *ring_buffer_addr;
	INT32U ring_buffer_size;
	INT32U WI;
	INT32U RI;
	
	// audio special effect
#if VOICE_CHANGER_EN == 1
	void   *vc_handle;
#endif
#if UP_SAMPLE_EN == 1
	void   *us_handle;
#endif

	// audio up sample
	INT8U  upsample_flag;
	INT8U  aud_dec_ch_no;
	INT16U aud_dec_sr;
	
	// scaler
	INT16U scaler_flag;
	INT16U user_scaler_flag;
	INT32U scaler_output_addr[SCALER_FRAME_NO];
	INT16U image_output_width, image_output_height;
	INT16U buffer_output_width, buffer_output_height;
	INT32U mpeg4_decode_out_format;
    INT32U video_decode_in_format;
    INT32U video_decode_out_format;
} video_decode_para_struct;

typedef struct
{
	INT16U current_index;
	INT16U total_number; 
}vid_dec_buf_struct;

typedef struct
{
	INT32U scaler_mode; 
	INT32U input_addr;
	INT32U input_format;
	INT32U input_x;
	INT32U input_y;
	INT32U input_visible_x;
	INT32U input_visible_y;
	
	INT32U output_addr;
	INT32U output_format; 
	INT32U output_x; 
	INT32U output_y;
	INT32U output_buffer_x;
	INT32U output_buffer_y;
	INT32U boundary_color;
}scaler_info;

typedef struct
{
	INT32U scaler_mode;
	INT32U raw_data_addr;
	INT32U raw_data_size; 
	
	INT32U jpeg_fifo;
	INT32U scaler_fifo;
	INT32U fifo_line;
	INT32U jpeg_yuv_mode;
	INT32U jpeg_valid_w;
	INT32U jpeg_valid_h;
	INT32U jpeg_extend_w;
	INT32U jpeg_extend_h; 
	
	INT32U output_addr;	
	INT32U output_format; 
	INT32U jpeg_output_w; 
	INT32U jpeg_output_h;
	INT32U jpeg_output_buffer_w;
	INT32U jpeg_output_buffer_h;
	INT32U boundary_color;
}mjpeg_info;

extern OS_EVENT *vid_dec_state_q;
extern OS_EVENT *aud_dec_q;
extern OS_EVENT *vid_dec_q;

extern video_decode_para_struct *p_vid_dec_para;
extern const GP_BITMAPINFOHEADER *p_bitmap_info;
extern const GP_WAVEFORMATEX *p_wave_info;
extern long  g_bitmap_info_len, g_wave_info_len;

// library
extern const void *AviParser_GetFcnTab(void);
extern const void *QtffParser_GetFcnTab(void);

// mpeg4 decoder
extern void mpeg4_decode_init(void);
extern INT32S mpeg4_decode_config(INT8U codec_type, INT32U output_format, INT16U width, INT16U height, INT8U vop_time_inc_reso);
extern INT32S mpeg4_decode_start(INT32U vlc_addr, INT32U decode_addr, INT32U refer_addr);
extern void mpeg4_decode_stop(void);
extern INT32S mpeg4_wait_idle(INT8U wait_done);
extern INT32S mpeg4_status_polling(void);

extern INT32S drvl1_mp4_decode_match_code_set(INT8U count, INT32U value);
extern void drvl1_mp4_deblock_set(INT32U quant_value, INT8U I_frame);
extern INT32S drvl1_mp4_deblock_iram_set(INT32U iram_addr);
extern INT32S drvl1_mp4_deblock_start(INT32U deb_ouput_addr);
extern void drvl1_mp4_deblock_stop(void);

// timer api
extern void vid_dec_timer_isr(void);
extern void vid_dec_stop_timer(void);
extern void vid_dec_start_timer(void);

//memory api
extern INT32S vid_dec_memory_alloc(void);
extern void vid_dec_memory_free(void);
extern INT32S vid_dec_memory_realloc(void);
extern void vid_dec_buffer_lock(INT32U addr);
extern void vid_dec_buffer_unlock(void);
extern INT32U vid_dec_get_next_vid_buffer(void);
extern INT32U vid_dec_get_next_deblock_buffer(void);
extern INT32U vid_dec_get_next_aud_buffer(void);
extern INT32U vid_dec_get_next_scaler_buffer(void);

//info api
extern void vid_dec_set_status(INT32S flag);
extern void vid_dec_clear_status(INT32S flag);
extern INT32S vid_dec_get_status(void);

//video api
extern void vid_dec_set_video_flag(INT8S video_flag);
extern INT8S vid_dec_get_video_flag(void);
extern void vid_dec_get_size(INT16U *width, INT16U *height);
extern INT32S vid_dec_get_video_format(INT32U biCompression);
extern INT32S vid_dec_get_file_format(INT8S *pdata);

//deblock flag
extern void vid_dec_set_deblock_flag(INT8S deblock_flag);
extern INT8S vid_dec_get_deblock_flag(void);

//audio api
extern void vid_dec_set_audio_flag(INT8S audio_flag);
extern INT8S vid_dec_get_audio_flag(void);

//audio decoder
extern INT32S vid_dec_set_aud_dec_work_mem(INT32U work_mem_size);
extern INT32S vid_dec_set_aud_dec_ring_buffer(void); 
extern void vid_dec_aud_dec_memory_free(void);

//audio dac
extern INT32S aud_dec_double_buffer_put(INT16U *pdata,INT32U cwlen, OS_EVENT *os_q);
extern INT32U aud_dec_double_buffer_set(INT16U *pdata, INT32U cwlen);
extern INT32S aud_dec_dma_status_get(void);
extern INT32S aud_dec_dma_dbf_status_get(void);
extern void aud_dec_double_buffer_free(void);
extern void aud_dec_dac_start(INT8U channel, INT32U sample_rate);
extern void aud_dec_dac_stop(void);
extern void aud_dec_ramp_down_handle(INT8U channel_no);

//scaler
extern INT32S vid_dec_set_scaler(INT32U scaler_mode, INT32U video_output_format, 
								INT16U image_output_width, INT16U image_output_height, 
								INT16U buffer_output_width, INT16U buffer_output_height);
extern void vid_dec_set_user_define_buffer(INT8U user_flag, INT32U addr0, INT32U addr1);

//mjpeg decode
extern INT32S mjpeg_seek_to_jpeg_header(INT32U raw_data_addr);
extern INT32S mjpeg_decode_get_size(INT32U raw_data_addr, INT32U size, INT16U *width, INT16U *height, INT16U *img_yuv_mode);
extern INT32S mjpeg_memory_alloc(mjpeg_info *pMjpeg);
extern void mjpeg_memory_free(void);
extern void mjpeg_decode_set_scaler(mjpeg_info *pMjpeg);
extern INT32S mjpeg_decode_and_scaler(mjpeg_info *pMjpeg);
extern INT32S mjpeg_decode_without_scaler(mjpeg_info *pMjpeg);
extern INT32S mjpeg_decode_stop_all(INT32U flag);

//bit stream api
extern void vid_dec_parser_bit_stream_init(void);
extern INT32S vid_dec_paser_bit_stream(INT8U *pdata, INT32U bs_size, INT16U *width, INT16U *height, INT8U *time_inc_bits, INT8U *quant);
extern INT32S vid_dec_paser_h263_bit_stream(INT8U *pdata, INT16U *width, INT16U *height, INT32U *key_word);
extern INT32S vid_dec_paser_sorenson263_bit_stream(INT8U *pdata, INT16U *width, INT16U *height, INT32U *key_word);

//scaler
extern INT32S scaler_size_start(scaler_info *pScaler);
extern INT32S scaler_size_wait_done(void);
					
//video_decode_state
extern INT32S vid_dec_entry(void);
extern INT32S vid_dec_exit(void);
extern INT32S vid_dec_parser_start(INT16S fd, INT32S FileType, INT64U FileSize);
extern INT32S vid_dec_parser_stop(void);
extern INT32S vid_dec_start(void);
extern INT32S vid_dec_stop(void);
extern INT32S vid_dec_pause(void);
extern INT32S vid_dec_resume(void);
extern INT32S vid_dec_get_total_samples(void);
extern INT32S vid_dec_get_total_time(void);
extern INT32S vid_dec_get_current_time(void);
extern INT32S vid_dec_set_play_time(INT32U second);
extern INT32S vid_dec_set_play_speed(INT32U speed);
extern INT32S vid_dec_set_reverse_play(INT32U Enable);
extern INT32S vid_dec_nth_frame(INT32U nth);
extern INT32S vid_dec_end_callback(INT32U bflag);

extern INT32S video_decode_state_task_create(INT8U pori);
extern INT32S video_decode_state_task_del(INT8U pori);
extern void video_decode_state_task_entry(void *para);

//video decoder
extern INT32S video_decode_task_start(void);
extern INT32S video_decode_task_restart(void);
extern INT32S video_decode_task_stop(void);
extern INT32S video_decode_task_nth_frame(void);
extern INT32S video_decode_task_create(INT8U proi);
extern INT32S video_decode_task_del(INT8U proi);
extern void video_decode_task_entry(void *parm);

//audio decoder
extern INT32S audio_decode_task_start(void);
extern INT32S audio_decode_task_stop(void);
extern INT32S audio_decode_task_pause(void);
extern INT32S audio_decode_task_resume(void);
extern INT32S audio_decode_task_create(INT8U prio);
extern INT32S audio_decode_task_del(INT8U prio);
extern void audio_decode_task_entry(void *param);

#endif
