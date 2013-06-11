#ifndef __drv_l1_MPEG4_H__
#define __drv_l1_MPEG4_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

//codec type
#define C_MPEG4_CODEC			0x01
#define C_H263_CODEC			0x02 
#define C_SORENSON_H263_CODEC	0x04

//mpeg4 status flag
#define C_MP4_STATUS_STOP				0x00000001
#define C_MP4_STATUS_DECODEING			0x00000002
#define C_MP4_STATUS_ENDECODEING		0x00000002
#define C_MP4_STATUS_END_OF_FRAME		0x00000004
#define C_MP4_STATUS_START_OF_FRAME		0x00000008
#define C_MP4_STATUS_DEC_ERR			0x00000010
#define C_MP4_STATUS_VLD_RESTART		0x00000020
#define C_MP4_STATUS_BUSY				0x00000040

#define C_MP4_STATUS_INIT_ERR			0x80001000
#define C_MP4_STATUS_TIMEOUT			0x80002000
#define C_MP4_STATUS_UNDEF				0x80004000

//R_MP4_CTRL
#define C_MP4_JPEG_ENCODE_MODE			0x0000
#define C_MP4_JPEG_DECODE_MODE			0x0001
#define C_MP4_MPEG1_ENCODE_MODE			0x0002
#define C_MP4_MPEG1_DECODE_MODE			0x0003
#define C_MP4_MPEG4_ENCODE_MODE			0x0006
#define C_MP4_MPEG4_DECODE_MODE			0x0007
#define C_MP4_JPEG_YUV_MODE_EN			0x0008
#define C_MP4_JPEG_MPEG_BW_MODE_EN		0x0010
#define C_MP4_JPEG_BW_WITH_UV_DATA		0x0000
#define C_MP4_JPEG_BW_ONLY_Y_DATA		0x0020
#define C_MP4_Y_FULL_RANGE				0x0040
#define C_MP4_UV_FULL_RANGE				0x0080

//R_MP4_RESET
#define C_MP4_SW_RESET					0x0001 

//R_MP4_BUF_CTRL
#define C_MP4_DOUBLE_BUF_CTRL_EN		0x0001
#define C_MP4_DOUBLE_BUF_UPDATE_EN		0x0002
#define C_MP4_ZERO_MV_EN				0x0010
#define C_MP4_JPG_PASS_EN				0x0020

//R_MP4_SET_EN
#define C_MP4_GOP_RESET					0x0001
#define C_MP4_HALT_PIXEL_EN				0x0002
#define C_MP4_HW_CTRL_BITRATE_EN		0x0004
#define C_MP4_RATE_INIT_SET				0x0008
#define C_MP4_SOF_TRI_BY_CPU			0x0010
#define C_MP4_UNRESTRICTED_EN			0x0020
#define C_MP4_GOP_SET_BY_REG_EN			0x0040
#define C_MP4_MPG1_SKIP_EN				0x0080										

//R_MP4_IP_FRAME
#define C_MPEG_I_FRAME					0x0001
#define C_MPEG_P_FRAME					0x0000	

//R_MP4_SOFCTRL
#define C_MP4_START_COMPRESS			0x0001
#define C_MP4_COMPRESS_BUSY				0x0001
#define C_MP4_ENCODE_EOF_STATUS			0x0002
#define C_MP4_SKIP_FRAME_FLAG			0x0004
#define C_MP4_CPU_SOF_INFO_FLAG			0x0008
#define C_MP4_START_DECOMPRESS			0x0010
#define C_MP4_DECOMPRESS_BUSY			0x0010

//R_MP4_INT_ENABLE & R_MP4_INT_STATUS
#define C_MP4_INT_MC_EN					0x0001
#define C_MP4_INT_JPEG_EN				0x0002
#define C_MP4_INT_EOF_EN				0x0004
#define C_MP4_INT_SOF_EN				0x0008
#define C_MP4_INT_DEC_ERR_EN			0x0010
#define C_MP4_INT_VLD_RESTART_EN		0x0020
#define C_MP4_INT_MASK					0x003F

//R_MP4_YUV_MODE
#define C_MP4_YUV420_TO_YUV422_EN		0x0030

//h263 format
#define C_H263_SUB_QCIF_FORMAT			0x0002
#define C_H263_QCIF_FORMAT				0x0004
#define C_H263_CIF_FORMAT				0x0006
#define C_H263_4CIF_FORMAT				0x0008

//R_MP4_QTBLSET
#define C_MP4_QUANTIZER_ORIGNAL			0x0000
#define C_MP4_QUANTIZER_SET_1			0x0001
#define C_MP4_HW_QUANTIZE_EN			0x0002
#define C_MP4_SKIP_ALL_ZERO_BLOCK_EN	0x0004
#define C_MP4_SKIP_EXTRA_HEADER_EN		0x0008
#define C_MP4_FILL_PIXEL_MOTION_EN		0x0000
#define C_MP4_HALF_PIXEL_MOTION_EN		0x0010

//R_MP4_TRUNTRES
#define C_MP4_QUANTIZATION_ROUNDED		0x0000
#define C_MP4_QUANTIZATION_TRUNCATE		0x0001
#define C_MP4_DCT_RESO_9_8				0x0000
#define C_MP4_DCT_RESO_2_1				0x0002

//R_MP4_Busy
#define C_MP4_BUSY						0x0001

//R_MP4_YUV_MODE_SET
#define C_MP4_INPUT_VY1UY0				0x00000
#define C_MP4_INPUT_Y1UY0V				0x00001
#define C_MP4_INPUT_Y1VY0U				0x00002
#define C_MP4_INPUT_UY1VY0				0x00003
#define C_MP4_OUTPUT_VY1UY0				0x00000
#define C_MP4_OUTPUT_Y1UY0V				0x00001
#define C_MP4_OUTPUT_Y1VY0U				0x00002
#define C_MP4_OUTPUT_UY1VY0				0x00003

//R_MP4_DEB_CTRL
#define C_MP4_DEB_EN					0x0001

//R_MP4_QUANT_CTRL
#define C_MP4_QUANT_MASK				0x0001F
#define C_MP4_ADVANCE_FILTER_EN			0x00100
#define C_MP4_DEB_I_FRAME				0x10000			

// drv_l1 api
extern INT32S drvl1_mp4_decode_size_set(INT16U width, INT16U height);
extern INT32S drvl1_mp4_encode_size_set(INT16U width, INT16U height);
extern INT32S drvl1_mp4_decode_bypass_vlc_header_set(INT32U vlc_offset_addr, INT32U size);
extern INT32S drvl1_mp4_decode_match_code_set(INT8U count, INT32U value);
extern void drvl1_mp4_YUV_format_set(INT32U yuv_type);
extern void drvl1_mp4_deblock_set(INT32U quant_value, INT8U I_frame);
extern INT32S drvl1_mp4_deblock_iram_set(INT32U iram_addr);
extern INT32S drvl1_mp4_deblock_start(INT32U deb_ouput_addr);
extern void drvl1_mp4_deblock_stop(void);
extern INT32S drvl1_h263_decode_set(INT8U h263_en, INT8U sorensoen_h263_en, INT8U version, INT16U width, INT16U height);

//decode api
extern void mpeg4_decode_init(void);
extern void mpeg4_decode_uninit(void);
extern INT32S mpeg4_decode_config(INT8U codec_type, INT32U output_format, INT16U width, INT16U height, INT8U vop_time_inc_reso);
extern INT32S mpeg4_decode_start(INT32U vlc_addr, INT32U decode_addr, INT32U refer_addr);
extern void mpeg4_decode_stop(void);
extern INT32S mpeg4_wait_idle(INT8U wait_done);
extern INT32S mpeg4_status_polling(void);
extern INT32S mpeg4_device_protect(void);
extern void mpeg4_device_unprotect(INT32S mask);
extern void mpeg4_isr(void);

//encode api
extern void mpeg4_encode_init(void);
extern void mpeg4_encode_uninit(void);
extern INT32S mpeg4_encode_isram_set(INT32U addr);
extern void mpeg4_encode_ip_set(INT8U byReg, INT32U IPXXX_No);
extern INT32S mpeg4_encode_config(INT8U input_format, INT16U width, INT16U height, INT8U vop_time_inc_reso);
extern INT32S mpeg4_encode_start(INT8U ip_frame, INT8U quant_value, INT32U vlc_out_addr, 
								INT32U raw_data_in_addr, INT32U encode_out_addr, INT32U refer_addr);
extern INT32S mpeg4_encode_get_vlc_size(void);
extern void mpeg4_encode_stop(void);

#endif		// __drv_l1_MPEG4_H__
