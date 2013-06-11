#include "video_decoder.h"

//=======================================================================================
//		video decode task create
//=======================================================================================
void video_decode_entrance(void)
{ 
	if(vid_dec_entry() < 0) {
		while(1);
	}
}

//=======================================================================================
//		video decode task delect 
//=======================================================================================
void video_decode_exit(void)
{	
	vid_dec_stop();
	vid_dec_parser_stop();
	if(vid_dec_exit() < 0) {
		while(1);
	}
}

//=======================================================================================
//		audio dac ramp up 
//=======================================================================================
void avi_audio_decode_rampup(void)
{
	//check ramp up 
	if((R_DAC_CHA_FIFO & 0x3000) == 0x3000) {
		return;
	}
		
	dac_init();
	dac_vref_set(1);//enable DAC
	while(R_DAC_PGA&0x0100);
	//enable cha chb 
	R_DAC_CHA_CTRL |= 0x2000;
	R_DAC_CHB_CTRL |= 0x2000; 
}

//=======================================================================================
//		audio dac ramp down 
//=======================================================================================
void avi_audio_decode_rampdown(void)
{
	dac_disable();
}

//=======================================================================================
//		AVI File Info
//=======================================================================================
VIDEO_CODEC_STATUS video_decode_Info(VIDEO_INFO *info)
{
	INT16U width, height;
	
	// audio
	switch(p_wave_info->wFormatTag)
	{
	case WAVE_FORMAT_PCM:
		info->AudFormat = WAV;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"pcm");
		break;
	
	case WAVE_FORMAT_MULAW:
	case WAVE_FORMAT_ALAW:
		info->AudFormat = WAV;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"adpcm");
		break;
	
	case WAVE_FORMAT_ADPCM:
		info->AudFormat = MICROSOFT_ADPCM;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"adpcm");
		break;
	
	case WAVE_FORMAT_DVI_ADPCM:
		info->AudFormat = IMA_ADPCM;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"adpcm");
		break;
	
	case WAVE_FORMAT_MPEGLAYER3:	
		info->AudFormat = MP3;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"mp3");
		break;
	
	case WAVE_FORMAT_MPEG_ADTS_AAC:
		info->AudFormat = MP3;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"aac");
		break;
	
	default:
		info->AudFormat = AUD_AUTOFORMAT;
		gp_strcpy((INT8S*)info->AudSubFormat, (INT8S *)"none");		
	}
	
	if(p_wave_info) {	
		info->AudSampleRate = p_wave_info->nSamplesPerSec;
		info->AudBitRate = p_wave_info->nAvgBytesPerSec;
		info->AudChannel = p_wave_info->nChannels;
		info->AudFrameSize = p_wave_info->nBlockAlign;	
	} else {
		info->AudSampleRate = 0;
		info->AudBitRate = 0;
		info->AudChannel = 0;
		info->AudFrameSize = 0;	
	}	
	
	// video
	switch(p_bitmap_info->biCompression)
	{
	case C_XVID_FORMAT:
	case C_M4S2_FORMAT:
		info->VidFormat = MPEG4;
		gp_strcpy((INT8S*)info->VidSubFormat, (INT8S *)"mp4");
		break;
		
	case C_MJPG_FORMAT:
		info->VidFormat = MJPEG;
		gp_strcpy((INT8S*)info->VidSubFormat, (INT8S *)"jpg");
		break;
		
	default:
		info->VidFormat = VID_AUTOFORMAT;
		gp_strcpy((INT8S*)info->VidSubFormat, (INT8S *)"non");
	}
	
	if(p_bitmap_info) {
		if(vid_dec_get_total_time()) {
			info->VidFrameRate = vid_dec_get_total_samples()/vid_dec_get_total_time();
		} else {
			info->VidFrameRate = vid_dec_get_total_samples();
		}

		vid_dec_get_size(&width, &height);
		info->Width = width;
		info->Height = height;
		info->TotalDuration = vid_dec_get_total_time();	
	} else {
		info->VidFrameRate = 0;
		info->Width = 0;
		info->Height = 0;
		info->TotalDuration = 0;	
	}
	return VIDEO_CODEC_PROCESSING;
}

//=======================================================================================
//		avi decode paser header
//=======================================================================================
VIDEO_CODEC_STATUS	video_decode_paser_header(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src)
{
	INT32S nRet, video_type;
	struct sfn_info fd_info;
	
	if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	
	if(video_decode_status() == VIDEO_CODEC_PROCESSING) {
		return VIDEO_CODEC_STATUS_ERR;
	}
					
	if((src.type != SOURCE_TYPE_FS) && (src.type != SOURCE_TYPE_FS_RESOURCE_IN_FILE)) {	
		DEBUG_MSG(DBG_PRINT("Only Support GP File System\r\n"));		
		return 	VIDEO_CODEC_RESOURCE_NO_FOUND;
	}
	
	if(src.Format.VideoFormat != MJPEG && src.Format.VideoFormat != MPEG4) {
		DEBUG_MSG(DBG_PRINT("Only Support MJPEG and MPEG4\r\n"));		
		return	CHANNEL_ASSIGN_ERROR;
	}
	
	if(src.type_ID.FileHandle < 0) {	
		DEBUG_MSG(DBG_PRINT("File handle Error\r\n"));
		return RESOURCE_READ_ERROR;
	}
	
	if(src.type == SOURCE_TYPE_FS_RESOURCE_IN_FILE) {
		video_type = FILE_TYPE_AVI;
		fd_info.f_size = src.type_ID.temp;
	} else {
		sfn_stat(src.type_ID.FileHandle, &fd_info);
		video_type = vid_dec_get_file_format((INT8S*)fd_info.f_extname);
	}
	
	if(video_type < 0) {
		close(src.type_ID.FileHandle);
		return VIDEO_CODEC_STATUS_ERR;
	}
		
	nRet = vid_dec_parser_start(src.type_ID.FileHandle, video_type, fd_info.f_size);
	if(nRet < 0) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	
	video_decode_Info(video_info);		
	return VIDEO_CODEC_STATUS_OK;
}

//=======================================================================================
//		avi decode start 
//=======================================================================================
CODEC_START_STATUS video_decode_start(VIDEO_ARGUMENT arg, MEDIA_SOURCE src)
{
	INT16U width, height;
	INT32U video_output_format;
	
	//display size
	if(arg.DisplayWidth == 0 || arg.DisplayHeight == 0) {
		arg.DisplayWidth = arg.TargetWidth;
		arg.DisplayHeight = arg.TargetHeight;
	}
	
	if(arg.DisplayBufferWidth == 0 || arg.DisplayBufferHeight == 0) {
		arg.DisplayBufferWidth = arg.TargetWidth;
		arg.DisplayBufferHeight = arg.TargetHeight;
	}
	
	//format
	switch(arg.OutputFormat)
	{
	case IMAGE_OUTPUT_FORMAT_RGB565: 	video_output_format = C_SCALER_CTRL_OUT_RGB565; break;
	case IMAGE_OUTPUT_FORMAT_YUYV: 		video_output_format = C_SCALER_CTRL_OUT_YUYV; break;
	case IMAGE_OUTPUT_FORMAT_UYVY: 		video_output_format = C_SCALER_CTRL_OUT_UYVY; break;
#if MCU_VERSION < GPL327XX	
	case IMAGE_OUTPUT_FORMAT_RGB1555: 	video_output_format = C_SCALER_CTRL_OUT_RGB1555; break;
	case IMAGE_OUTPUT_FORMAT_RGBG: 		video_output_format = C_SCALER_CTRL_OUT_RGBG; break;
	case IMAGE_OUTPUT_FORMAT_GRGB: 		video_output_format = C_SCALER_CTRL_OUT_GRGB; break;
	case IMAGE_OUTPUT_FORMAT_YUYV8X32: 	video_output_format = C_SCALER_CTRL_OUT_YUYV8X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV8X64:	video_output_format = C_SCALER_CTRL_OUT_YUYV8X64; break;
	case IMAGE_OUTPUT_FORMAT_YUYV16X32: video_output_format = C_SCALER_CTRL_OUT_YUYV16X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV16X64: video_output_format = C_SCALER_CTRL_OUT_YUYV16X64; break;
	case IMAGE_OUTPUT_FORMAT_YUYV32X32: video_output_format = C_SCALER_CTRL_OUT_YUYV32X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV64X64: video_output_format = C_SCALER_CTRL_OUT_YUYV64X64; break;
#endif	
	default: return CODEC_START_STATUS_ERROR_MAX;
	}
	
	//scaler
	vid_dec_get_size(&width, &height);
	if(arg.bScaler & 0x01) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_SCALER_FULL_SCREEN;
		vid_dec_set_scaler(C_SCALER_FULL_SCREEN, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);
	} else if(arg.bScaler & 0x02) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_NO_SCALER_FIT_BUFFER;
		vid_dec_set_scaler(C_NO_SCALER_FIT_BUFFER, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);
	} else if(arg.bScaler & 0x04) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_SCALER_FIT_BUFFER;
		vid_dec_set_scaler(C_SCALER_FIT_BUFFER, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);	
	} else {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_NO_SCALER;	
		vid_dec_set_scaler(C_NO_SCALER, video_output_format, width, height, width, height);
	} 
	
	//deblock
	if((p_bitmap_info->biCompression != C_MJPG_FORMAT) && (arg.bScaler & 0x80)) {
		vid_dec_set_deblock_flag(TRUE);
	} else {
		vid_dec_set_deblock_flag(FALSE);
	} 
	
	//user define buffer	
	vid_dec_set_user_define_buffer(arg.bUseDefBuf, (INT32U)arg.AviDecodeBuf1, (INT32U)arg.AviDecodeBuf2);
	if(vid_dec_start() < 0) {
		video_decode_stop();
		return CODEC_START_STATUS_ERROR_MAX;
	}
	return START_OK;
}

//=======================================================================================
//		video decode stop
//=======================================================================================
VIDEO_CODEC_STATUS video_decode_stop(void)
{
	INT32S nRet, nTemp;
	
	nRet = vid_dec_stop();
	nTemp = vid_dec_parser_stop();
	if((nRet < 0) || (nTemp < 0)) {
		return VIDEO_CODEC_STATUS_ERR;
	}	
	return VIDEO_CODEC_STATUS_OK;
}

//=======================================================================================
//		video decode pause
//=======================================================================================
VIDEO_CODEC_STATUS video_decode_pause(void)
{
	if(video_decode_status() != VIDEO_CODEC_PROCESSING) {
		return VIDEO_CODEC_STATUS_ERR;
	}
		
	if(vid_dec_pause() < 0) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	return VIDEO_CODEC_STATUS_OK;
}

//=======================================================================================
//		video decode resume
//=======================================================================================
VIDEO_CODEC_STATUS video_decode_resume(void)
{
	if(video_decode_status() != VIDEO_CODEC_PROCESS_PAUSE) {
		return VIDEO_CODEC_STATUS_ERR;
	}	
	
	if(vid_dec_resume() < 0) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	return VIDEO_CODEC_STATUS_OK;
}

//=======================================================================================
//		video decode volume
//=======================================================================================
void audio_decode_volume(INT8U volume)
{
	if(volume > 0x3F) {
		volume = 0x3F;
	}
	R_DAC_PGA = volume;	
}

//=======================================================================================
//		video decode status
//=======================================================================================
VIDEO_CODEC_STATUS video_decode_status(void)
{
	if(vid_dec_get_status() & C_VIDEO_DECODE_ERR) {
		return VIDEO_CODEC_PASER_HEADER_FAIL;
	} else if(vid_dec_get_status() & C_VIDEO_DECODE_PAUSE) {
		return VIDEO_CODEC_PROCESS_PAUSE;
	} else if(vid_dec_get_status() & C_VIDEO_DECODE_PLAY) {
		return VIDEO_CODEC_PROCESSING;
	} else if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER) {
		return VIDEO_CODEC_PROCESSING;
	} else if(vid_dec_get_status() & C_VIDEO_DECODE_PARSER_NTH) {
		return VIDEO_CODEC_PROCESSING;	
	} else {
		return VIDEO_CODEC_PROCESS_END;
	}
}

//========================================================================================
//	video decode set play time, jump somewhere and play 
//========================================================================================
VIDEO_CODEC_STATUS video_decode_set_play_time(INT32S SecTime)
{
	if(video_decode_status() == VIDEO_CODEC_PASER_HEADER_FAIL){
		return VIDEO_CODEC_STATUS_ERR;
	}
	
	if(video_decode_status() == VIDEO_CODEC_PROCESS_END) {
		return  VIDEO_CODEC_PROCESS_END;
	}
			
	if(SecTime < 0) {
		SecTime = 0;
	}
	
	if(SecTime >= vid_dec_get_total_time()) {
		return VIDEO_CODEC_STATUS_ERR;
	}	
	
	if(vid_dec_set_play_time(SecTime) < 0) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	return VIDEO_CODEC_STATUS_OK;
}

//========================================================================================
//	video decode set play speed 
//========================================================================================
VIDEO_CODEC_STATUS video_decode_set_play_speed(FP32 speed)
{
	if(video_decode_status() == VIDEO_CODEC_PROCESSING) {
		if(vid_dec_set_play_speed(speed * 0x10000) < 0) {
			return VIDEO_CODEC_STATUS_ERR;
		}
	}
	return VIDEO_CODEC_STATUS_OK;
}

//========================================================================================
//	video decode set play speed 
//========================================================================================
VIDEO_CODEC_STATUS video_decode_set_reserve_play(INT32U enable)
{
	if(video_decode_status() == VIDEO_CODEC_PROCESSING) {
		if(vid_dec_set_reverse_play(enable) < 0) {
			return VIDEO_CODEC_STATUS_ERR;
		}
	}
	return VIDEO_CODEC_STATUS_OK;
}


//========================================================================================
//	video decode get current playing time
//========================================================================================
INT32U	video_decode_get_current_time(void)
{
	return vid_dec_get_current_time();
}

//========================================================================================
// get n-th video frame data
//========================================================================================
VIDEO_CODEC_STATUS video_decode_get_nth_video_frame(VIDEO_INFO *video_info, VIDEO_ARGUMENT arg, MEDIA_SOURCE src, INT32U nth_frame)
{
	INT16U width, height;
	INT32S nRet, nTemp;
	INT32U video_output_format;
	
	//check playing
	if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	
	if(video_decode_status() == VIDEO_CODEC_PROCESSING) {
		return VIDEO_CODEC_STATUS_ERR;
	}	
		
	//format
	switch(arg.OutputFormat)
	{
	case IMAGE_OUTPUT_FORMAT_RGB565: 	video_output_format = C_SCALER_CTRL_OUT_RGB565; break;
	case IMAGE_OUTPUT_FORMAT_YUYV: 		video_output_format = C_SCALER_CTRL_OUT_YUYV; break;
	case IMAGE_OUTPUT_FORMAT_UYVY: 		video_output_format = C_SCALER_CTRL_OUT_UYVY; break;
#if MCU_VERSION < GPL327XX
	case IMAGE_OUTPUT_FORMAT_RGB1555: 	video_output_format = C_SCALER_CTRL_OUT_RGB1555; break;	
	case IMAGE_OUTPUT_FORMAT_RGBG: 		video_output_format = C_SCALER_CTRL_OUT_RGBG; break;
	case IMAGE_OUTPUT_FORMAT_GRGB: 		video_output_format = C_SCALER_CTRL_OUT_GRGB; break;
	case IMAGE_OUTPUT_FORMAT_YUYV8X32: 	video_output_format = C_SCALER_CTRL_OUT_YUYV8X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV8X64:	video_output_format = C_SCALER_CTRL_OUT_YUYV8X64; break;
	case IMAGE_OUTPUT_FORMAT_YUYV16X32: video_output_format = C_SCALER_CTRL_OUT_YUYV16X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV16X64: video_output_format = C_SCALER_CTRL_OUT_YUYV16X64; break;
	case IMAGE_OUTPUT_FORMAT_YUYV32X32: video_output_format = C_SCALER_CTRL_OUT_YUYV32X32; break;
	case IMAGE_OUTPUT_FORMAT_YUYV64X64: video_output_format = C_SCALER_CTRL_OUT_YUYV64X64; break;
	case IMAGE_OUTPUT_FORMAT_YUV422: 	video_output_format = C_SCALER_CTRL_OUT_YUV422; break;
	case IMAGE_OUTPUT_FORMAT_YUV420: 	video_output_format = C_SCALER_CTRL_OUT_YUV420; break;
	case IMAGE_OUTPUT_FORMAT_YUV411: 	video_output_format = C_SCALER_CTRL_OUT_YUV411; break;
	case IMAGE_OUTPUT_FORMAT_YUV444: 	video_output_format = C_SCALER_CTRL_OUT_YUV444; break;
	case IMAGE_OUTPUT_FORMAT_Y_ONLY: 	video_output_format = C_SCALER_CTRL_OUT_Y_ONLY; break;
#endif
	default: return CODEC_START_STATUS_ERROR_MAX;
	}
	
	if(video_decode_paser_header(video_info, arg, src) != VIDEO_CODEC_STATUS_OK) {
		return VIDEO_CODEC_STATUS_ERR;
	}
		
	//scaler
	vid_dec_get_size(&width, &height);
	if(arg.bScaler & 0x01) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_SCALER_FULL_SCREEN;
		vid_dec_set_scaler(C_SCALER_FULL_SCREEN, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);
	} else if(arg.bScaler & 0x02) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_NO_SCALER_FIT_BUFFER;
		vid_dec_set_scaler(C_NO_SCALER_FIT_BUFFER, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);
	} else if(arg.bScaler & 0x04) {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_SCALER_FIT_BUFFER;
		vid_dec_set_scaler(C_SCALER_FIT_BUFFER, video_output_format, arg.DisplayWidth, arg.DisplayHeight, arg.DisplayBufferWidth, arg.DisplayBufferHeight);	
	} else {
		p_vid_dec_para->scaler_flag = p_vid_dec_para->user_scaler_flag = C_NO_SCALER;
		vid_dec_set_scaler(C_NO_SCALER, video_output_format, width, height, width, height);
	} 
	
	//deblock
	vid_dec_set_deblock_flag(FALSE);
	//user define buffer
	vid_dec_set_user_define_buffer(arg.bUseDefBuf, (INT32U)arg.AviDecodeBuf1, (INT32U)arg.AviDecodeBuf2);
	nRet = vid_dec_nth_frame(1);
	nTemp = vid_dec_parser_stop();
	if(nRet < 0 || nTemp < 0) {
		return VIDEO_CODEC_STATUS_ERR;
	}
	return VIDEO_CODEC_STATUS_OK;
} 

//=======================================================================================
//		video decode get output display buffer size
//=======================================================================================
void video_decode_get_display_size(INT16U *pwidth, INT16U *pheight)
{
	*pwidth = p_vid_dec_para->buffer_output_width;
	*pheight = p_vid_dec_para->buffer_output_width;
}