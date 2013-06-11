#ifndef __AUDIO_DECODER_TASK_H__
#define __AUDIO_DECODER_TASK_H__

#include "application.h"

#define RING_BUF_SIZE 	      4096
#define BG_RING_BUF_SIZE 	  WMA_DEC_BITSTREAM_BUFFER_SIZE
#define MP3_RING_BUF_SIZE 	  8192
#define MP3_PCM_BUF_SIZE  MP3_DEC_FRAMESIZE*2 /* 1152*2,two channel*/
#define WMA_PCM_BUF_SIZE  WMA_DEC_FRAMESIZE*2 /* 2048*2 */
#define WAV_PCM_BUF_SIZE  WAV_DEC_FRAMESIZE*2
#define DEC_MEMORY_SIZE   WMA_DEC_MEMORY_SIZE
#define A1800_PCM_BUF_SIZE	A18_DEC_FRAMESIZE*2
#define MP3_FRAME_ERROR_CNT (5000/26)
#define AAC_PCM_BUF_SIZE  AAC_DEC_FRAMESIZE*2

#define AUDIO_SEND_FAIL   -1
#define AUDIO_SEND_OK     0

#define AUDIO_IDLE        0
#define AUDIO_PLAYING     1
#define AUDIO_PLAY_PAUSE  2
#define AUDIO_PLAY_STOP   3

#define AUDIO_READ_FAIL   -2
#define AUDIO_READ_PEND   -3
#define AUDIO_READ_WAIT   -4

#define ID3_TAG_NONE      0
#define ID3_TAG_V1        1
#define ID3_TAG_V2        2
#define ID3_TAG_V2_FOOTER 3
#define ID3_TAG_FLAG_FOOTER 0x10

#define AUDIO_PARSE_FAIL 		-1
#define AUDIO_PARSE_SUCCS 		0
#define AUDIO_MAX_TYPE			4
#define AUDIO_PASER_BUFFER_SIZE  (	2048*10)

typedef struct 
{
	AUDIO_TYPE audio_file_real_type;
	INT8S (*parse_audio_file_head)(INT8S *p_audio_file_head);
}AUDIO_FILE_PARSE;

typedef struct 
{	
	INT8S  mpeg_version;
	INT8S  layer;
	INT8S  sample_rate;
}MP3_FILE_HEAD;

typedef struct
{
	INT8U    state;
	INT8U    source_type;
	INT8U    curr_vol;
	INT32U   audio_format;
	INT32S	(*fp_deocde_init)();
	INT32S	(*fp_deocde)();
}AUDIO_CONTEXT,*AUDIO_CONTEXT_P;

typedef struct
{
	INT8U *ptr;
	INT32U cnt;
}RES_HD;

/*
typedef struct
{
	INT8U	*ring_buf;
	INT8S   *work_mem;
	INT16S  file_handle;
	INT32U  ring_size;
	INT32U  ri;
	INT32U  wi;
	INT32U   f_last;
	INT32U  file_len;
	INT32U  file_cnt;
	INT32U  try_cnt;
	INT32U  read_secs;
}AUDIO_CTRL;
*/

extern void    audio_send_result(INT32S res_type,INT32S result);
#if (defined _PROJ_TYPE) && (_PROJ_TYPE != _PROJ_TURNKEY)
extern void    audio_BG_send_result(INT32S res_type,INT32S result);
extern void    MIDI_send_result(INT32S res_type,INT32S result);
#endif
extern INT32S  audio_play_file_set(INT16S fd, AUDIO_CONTEXT *aud_context, AUDIO_CTRL *aud_ctrl, OS_EVENT *ack_fsq);
extern INT32S  audio_mem_alloc(INT32U audio_format,AUDIO_CTRL *aud_ctrl, INT16S *pcm[]);

extern void audio_task_entry(void *p_arg);
extern INT32U g_MIDI_index;		// added by Bruce, 2008/10/01
extern INT16U A1800_Bitrate;	// added by Bruce, 2008/10/03
//extern void	(*decode_end)();	// added by Bruce, 2008/10/03
extern void (*decode_end)(INT32U audio_decoder_num);	// modified by Bruce, 2008/11/20

extern INT32S mp3_get_duration(INT16S fd,INT32U len);
extern int wma_dec_seek(char *wmadec_workmem, int msTime);

#endif 		/* __AUDIO_DECODER_TASK_H__ */

