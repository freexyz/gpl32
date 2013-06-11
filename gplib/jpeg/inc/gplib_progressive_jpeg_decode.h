#ifndef __GPLIB_PROGRESSIVE_JPEG_DECODE_H__
#define __GPLIB_PROGRESSIVE_JPEG_DECODE_H__


#include "gplib.h"

#define JPEG_SUSPENDED								0x80000001	// Suspended due to lack of input data

#define JPEG_BS_NOT_ENOUGH							0	// Data in the bitstream buffer are not enough
#define JPEG_REACHED_SOS							1	// Reached start of new scan
#define JPEG_REACHED_EOI							2	// Reached end of image
#define JPEG_ROW_COMPLETED							3	// Completed one iMCU row
#define JPEG_SCAN_COMPLETED							4	// Completed last iMCU row of a scan
#define JPEG_FILE_END								5	// File Read End
#define JPEG_MCU_COMPLETED							6
#define JPEG_PASS_AC_REFINE							7	// To pass ac refine data segment
#define JPEG_HEADER_OK								8	// Found valid image datastream

#define C_PROGRESSIVE_JPEG_WORKING_MEMORY_SIZE    	6240
#define C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE	4096

void progressive_jpeg_dec_init(INT8U *working_mem, INT8U *bs_buf);
INT32S progressive_jpeg_parsing(INT8U *working_mem, INT32U wi);				// Header parsing
void progressive_jpeg_reset(INT8U *working_mem);							// Make output row index be zero
INT32S progressive_jpeg_hf_dec(INT8U *working_mem, INT32U wi);				// Huffman decoder
INT32U progressive_jpeg_dec_get_ri(INT8U *working_mem);						// Get read index
void progressive_jpeg_set_file_end(INT8U *working_mem);						// Tell library that reading file is ending
const INT8U *progressive_jpeg_get_version(void);							// Get library version
INT32U progressive_jpeg_get_width(INT8U *working_mem);						// Get image width
INT32U progressive_jpeg_get_height(INT8U *working_mem);						// Get image height
INT32U progressive_jpeg_get_component(INT8U *working_mem);					// Get the numbers of component
INT32U progressive_jpeg_get_hv(INT8U *working_mem, INT32S component_idx);	// Get the horizontal & vertical sampling in the component_idx of this component
INT32U progressive_jpeg_get_max_h(INT8U *working_mem);						// Get the max horizontal sampling in this image
INT32U progressive_jpeg_get_max_v(INT8U *working_mem);						// Get the max vertical sampling in this image
INT32U progressive_jpeg_get_MCUsize(INT8U *working_mem, INT32S component_idx);			// Get the MCU size in the component_idx of this component
INT32U progressive_jpeg_get_width_in_blocks(INT8U *working_mem, INT32S component_idx);	// Get the numbers of block in horizontal
INT32U progressive_jpeg_get_height_in_blocks(INT8U *working_mem, INT32S component_idx);	// Get the numbers of block in vertical
INT32U progressive_jpeg_get_coefmemsize(INT8U *working_mem);							// Get the size of dct coefficients needed
INT32U progressive_jpeg_get_blocksperrow(INT8U *working_mem, INT32S component_idx);		// Get the blocks of one row in the component_idx of this component
void progressive_jpeg_set_coefmem(INT8U *working_mem, INT8U *coef_mem);					// Set the dct coefficients memory
INT32U progressive_jpeg_get_comp_in_scan(INT8U *working_mem);							// Get the numbers of component in this scan
INT32U progressive_jpeg_get_comp_idx(INT8U *working_mem, INT32S id);					// Get the index of component
INT16U * progressive_jpeg_get_quant_tbl(INT8U *working_mem, INT32S component_idx);		// Get quantization table pointer


#endif		// __GPLIB_PROGRESSIVE_JPEG_DECODE_H__