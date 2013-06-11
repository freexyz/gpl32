#ifndef __drv_l1_JPEG_H__
#define __drv_l1_JPEG_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

// JFIF releate register
#define C_JPG_NON_STANDARD_FORMAT		0x00000000
#define C_JPG_STANDARD_FORMAT			0x00000001

// JPEG image width and height registers
#define C_JPG_HSIZE_MAX					0x00001FFF		// Maximum 8191 pixels
#define C_JPG_VSIZE_MAX					0x00001FFF		// Maximum 8191 pixels

// JPEG read control register
#define C_JPG_READ_CTRL_EN				0x80000000
#define C_JPG_READ_ADDR_UPDATE			0x40000000
#define C_JPG_READ_ON_FLY_LEN_MAX		0x000FFFFF

// VLC control register
#define C_JPG_VLC_TOTAL_LEN_MAX			0x03FFFFFF

// Control register
#define C_JPG_CTRL_COMPRESS				0x00000001
#define C_JPG_CTRL_DECOMPRESS			0x00000002
#define C_JPG_CTRL_LEVEL8_SCALEDOWN_EN	0x00000004
#define C_JPG_CTRL_CLIPPING_EN			0x00000008
#define C_JPG_CTRL_YUV444				0x00000000
#define C_JPG_CTRL_YUV422				0x00000010
#define C_JPG_CTRL_YUV420				0x00000020
#define C_JPG_CTRL_YUV411				0x00000030
#define C_JPG_CTRL_GRAYSCALE			0x00000040
#define C_JPG_CTRL_YUV422V				0x00000050
#define C_JPG_CTRL_YUV411V				0x00000060
#define C_JPG_CTRL_YUV420H2				0x00000120
#define C_JPG_CTRL_YUV420V2				0x00000220
#define C_JPG_CTRL_YUV411H2				0x00000130
#define C_JPG_CTRL_YUV411V2				0x00000260
#define C_JPG_CTRL_MODE_MASK			0x000003F0
#define C_JPG_CTRL_YCBCR_FORMAT			0x00001000
#define C_JPG_CTRL_YUV_FORMAT			0x00000000
#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XXB)
	#define C_JPG_CTRL_USING_SCALER_EN	0x00002000
#elif (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
	#define C_JPG_CTRL_USING_SCALER_EN	0x44002000
#else //GPL327XX
	#define C_JPG_CTRL_USING_SCALER_EN	0x04002000
#endif

#define C_JPG_CTRL_YUYV_ENCODE_EN		0x00004000
#define C_JPG_CTRL_LEVEL2_SCALEDOWN_EN	0x00010000
#define C_JPG_CTRL_SCALE_H_EVEN			0x00020000
#define C_JPG_CTRL_SCALE_H_ODD			0x00000000
#define C_JPG_CTRL_SCALE_V_EVEN			0x00040000
#define C_JPG_CTRL_SCALE_V_ODD			0x00000000

// Reset register
#define C_JPG_RESET						0x00000001

// Ring FIFO control register
#define C_JPG_FIFO_DISABLE				0x00000000
#define C_JPG_FIFO_ENABLE				0x00000001
#define C_JPG_FIFO_16LINE				0x00000009
#define C_JPG_FIFO_32LINE				0x00000001
#define C_JPG_FIFO_64LINE				0x00000003
#define C_JPG_FIFO_128LINE				0x00000005
#define C_JPG_FIFO_256LINE				0x00000007
#define C_JPG_FIFO_LINE_MASK			0x0000000F

// Image size register
#define C_JPG_IMAGE_SIZE_MAX			0x0FFFFFFF

// Interrupt control register
#define C_JPG_INT_COMP_EN				0x00000001
#define C_JPG_INT_DECOMP_EN				0x00000002
#define C_JPG_INT_FIFO_EN				0x00000004
#define C_JPG_INT_READ_EMPTY_EN			0x00000008

// Interrupt pending register
#define C_JPG_INT_COMP_PEND				0x00000001
#define C_JPG_INT_DECOMP_PEND			0x00000002
#define C_JPG_INT_FIFO_PEND				0x00000004
#define C_JPG_INT_READ_EMPTY_PEND		0x00000008
#define C_JPG_INT_RST_VLC_DONE_PEND		0x00000010
#define C_JPG_INT_RST_ERR_PEND			0x00000020
#define C_JPG_INT_MASK					0x0000003F

// Define JPEG engine status
#define C_JPG_STATUS_DECODING			0x00000001
#define C_JPG_STATUS_ENCODING			0x00000002
#define C_JPG_STATUS_INPUT_EMPTY		0x00000004
#define C_JPG_STATUS_OUTPUT_FULL		0x00000008
#define C_JPG_STATUS_DECODE_DONE		0x00000010
#define C_JPG_STATUS_ENCODE_DONE		0x00000020
#define C_JPG_STATUS_STOP				0x00000040
#define C_JPG_STATUS_TIMEOUT			0x00000080
#define C_JPG_STATUS_INIT_ERR			0x00000100
#define C_JPG_STATUS_RST_VLC_DONE		0x00000200
#define C_JPG_STATUS_RST_MARKER_ERR		0x00000400
#define C_JPG_STATUS_SCALER_DONE		0x00008000

// JPEG init API
extern void jpeg_init(void);

// JPEG APIs for setting Quantization table
extern INT32S jpeg_quant_luminance_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_quant_chrominance_set(INT32U offset, INT32U len, INT16U *val);

// JPEG APIs for setting Huffman table
extern INT32S jpeg_huffman_dc_lum_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_dc_lum_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_lum_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_chrom_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_dc_chrom_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_dc_chrom_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_lum_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_ac_lum_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_lum_huffval_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_chrom_mincode_set(INT32U offset, INT32U len, INT16U *val);
extern INT32S jpeg_huffman_ac_chrom_valptr_set(INT32U offset, INT32U len, INT8U *val);
extern INT32S jpeg_huffman_ac_chrom_huffval_set(INT32U offset, INT32U len, INT8U *val);

// JPEG APIs for setting image relative parameters
extern INT32S jpeg_restart_interval_set(INT16U interval);
extern INT32S jpeg_image_size_set(INT16U hsize, INT16U vsize);		// hsize<=8000, vsize<=8000, Decompression: hsize must be multiple of 16, vsize must be at least multiple of 8
extern INT32S jpeg_yuv_sampling_mode_set(INT32U mode);				// mode=C_JPG_CTRL_YUV422/C_JPG_CTRL_YUV420/C_JPG_CTRL_YUV444/C_JPG_CTRL_YUV411/C_JPG_CTRL_GRAYSCALE/C_JPG_CTRL_YUV422V/C_JPG_CTRL_YUV411V/C_JPG_CTRL_YUV420H2/C_JPG_CTRL_YUV420V2/C_JPG_CTRL_YUV411H2/C_JPG_CTRL_YUV411V2
extern INT32S jpeg_clipping_mode_enable(void);						// Decompression: When clipping mode is enabled, JPEG will output image data according to jpeg_image_clipping_range_set()
extern INT32S jpeg_clipping_mode_disable(void);
extern INT32S jpeg_image_clipping_range_set(INT16U start_x, INT16U start_y, INT16U width, INT16U height);	// x, y, width, height must be at lease 8-byte alignment
extern INT32S jpeg_level2_scaledown_mode_enable(void);				// Decompression: When level2 scale-down mode is enabled, output size will be 1/2 in width and height
extern INT32S jpeg_level2_scaledown_mode_disable(void);
extern INT32S jpeg_level8_scaledown_mode_enable(void);				// Decompression: When level8 scale-down mode is enabled, output size will be 1/8 in width and height
extern INT32S jpeg_level8_scaledown_mode_disable(void);
extern INT32S jpeg_level2_scaledown_union_mode_enable(INT32U enable);
extern INT32S jpeg_level4_scaledown_union_mode_enable(INT32U enable);
extern INT32S jpeg_encode_bitstream_offset_set(INT32U length, INT32U offset);
extern void jpeg_decode_use_scaler_sram(INT32U enable);
extern void jpeg_decode_use_mjpeg_output(INT32U enable);
extern INT32S jpeg_using_scaler_mode_enable(void);					// Decompression: When this mode is enabled, JPEG and Scaler engines work together without zoom function. Only YUV420 and YUV422 are supported in this mode.
extern INT32S jpeg_using_scaler_mode_disable(void);
extern INT32S jpeg_progressive_mode_enable(void);					// Decompression: When this mode is enabled, JPEG will do de-quantization and IDCT only. VLD huffman decoding is bypassed. 
extern INT32S jpeg_progressive_mode_disable(void);
extern INT32S jpeg_yuyv_encode_mode_enable(void);					// Compression: When this mode is enabled, JPEG will enocde YUYV data instead of Y/U/V separating data. Only YUV420 and YUV422 are supported in this mode. Scaler should not be used under this mode.
extern INT32S jpeg_yuyv_encode_mode_disable(void);

// JPEG APIs for setting Y/Cb/Cr or YUYV data
extern INT32S jpeg_yuv_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Compression: input addresses(8-byte alignment for normal compression, 16-bytes alignment for YUYV data compression); Decompression: output addresses(8-byte alignment)
extern INT32S jpeg_multi_yuv_input_init(INT32U len);				// Compression: When YUYV mode is used, it should be the length of first YUYV data that will be compressed. It should be sum of Y,U and V data when YUV separate mode is used

// JPEG APIs for setting entropy encoded data address and length
extern INT32S jpeg_vlc_addr_set(INT32U addr);						// Compression: output VLC address, addr must be 16-byte alignment. Decompression: input VLC address
extern INT32S jpeg_vlc_maximum_length_set(INT32U len);				// Decompression: JPEG engine stops when maximum VLC bytes are read(Maximum=0x03FFFFFF)
extern INT32S jpeg_multi_vlc_input_init(INT32U len);				// Decompression: length(Maximum=0x000FFFFF) of first VLC buffer

// JPEG APIs for setting output FIFO line
extern INT32S jpeg_fifo_line_set(INT32U line);						// Decompression: FIFO line number(C_JPG_FIFO_DISABLE/C_JPG_FIFO_ENABLE/C_JPG_FIFO_16LINE/C_JPG_FIFO_32LINE/C_JPG_FIFO_64LINE/C_JPG_FIFO_128LINE/C_JPG_FIFO_256LINE)

// JPEG start, restart and stop function APIs
extern INT32S jpeg_compression_start(void);
extern INT32S jpeg_multi_yuv_input_restart(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U len);	// Compression: Second, third, ... YUV addresses(8-byte alignment for normal compression, 16-bytes alignment for YUYV data compression) and lengths(Maximum=0x000FFFFF)
extern INT32S jpeg_decompression_start(void);
extern INT32S jpeg_multi_vlc_input_restart(INT32U addr, INT32U len);// Decompression: Second, third, ... VLC addresses(16-byte alignment) and lengths(Maximum=0x000FFFFF)
extern INT32S jpeg_yuv_output_restart(void);						// Decompression: Restart JPEG engine when FIFO is full
extern void jpeg_stop(void);

// JPEG API for scaler ISR to send Done message
extern void jpeg_using_scaler_decode_done(void);

// JPEG status polling API
extern INT32S jpeg_status_polling(INT32U wait);						// If wait is 0, this function returns immediately. Return value:C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_OUTPUT_FULL/C_JPG_STATUS_DECODE_DONE/C_JPG_STATUS_ENCODE_DONE/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR
extern INT32U jpeg_compress_vlc_cnt_get(void);						// This API returns the total bytes of VLC data stream that JPEG compression has been generated

#endif		// __drv_l1_JPEG_H__