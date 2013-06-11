#ifndef __GIF_DECODE_H__
#define __GIF_DECODE_H__

#include "gplib.h"

#define C_GIF_DEC_WORKING_MEMORY_SIZE  	68000
#define C_GIF_DEC_FS_BUFFER_SIZE 		2048

#define C_GIF_STATUS_OK					0x00000000
#define C_GIF_STATUS_LINE_DONE			0x00000001
#define C_GIF_STATUS_FRAME_HEADER		0x80000000
#define C_GIF_STATUS_END_OF_FRAME		0x80000001
#define C_GIF_STATUS_END_OF_IMAGE		0x80000002 
#define C_GIF_STATUS_NOT_DONE			0x80000080
#define C_GIF_STATUS_IMAGE_TOO_LARGE	0x80000082
#define C_GIF_STATUS_FILE_HAS_NO_FRAME	0x80000083
#define C_GIF_STATUS_NOT_A_GIF			0x80000085

extern INT32S gif_dec_init(CHAR* p_workmem, INT8U *fs_buf);
extern INT32S gif_dec_get_ri(CHAR *p_workmem);
extern INT32S gif_dec_parsing_file_header(CHAR *p_workmem, INT32S wi, INT32S flag);
extern INT32S gif_dec_parsing_frame_header(CHAR *p_workmem, INT32S wi);
extern INT32S gif_dec_get_still_or_motion(CHAR *p_workmem);
extern INT32S gif_dec_run(CHAR *p_workmem, INT16S *p_output, INT32S wi);
extern INT32S gif_dec_get_screen_width(CHAR *p_workmem);
extern INT32S gif_dec_get_screen_height(CHAR *p_workmem);
extern INT32S gif_dec_get_img_width(CHAR *p_workmem);
extern INT32S gif_dec_get_img_height(CHAR *p_workmem);
extern INT32S gif_dec_get_X_offset(CHAR *p_workmem);
extern INT32S gif_dec_get_Y_offset(CHAR *p_workmem);
extern INT32S gif_dec_get_next_ypos(CHAR *p_workmem);
extern INT32S gif_dec_get_cur_ypos(CHAR *p_workmem);
extern void	gif_dec_set_AtFileEnd(CHAR *p_workmem);
extern INT32S gif_dec_set_decoder_bmpbuffer(CHAR *p_workmem, INT16S *bmpbuffer);
extern INT8U* gif_dec_get_fill_rgb(CHAR *p_workmem);
extern INT32U gif_dec_get_delayTime(CHAR *p_workmem);
extern INT32S gif_dec_get_interlace(CHAR *p_workmem);
extern INT32S gif_dec_get_imageNum(CHAR *p_workmem);


#endif		// __GIF_DECODE_H__