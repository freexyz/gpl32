#ifndef _AVI_KERNEL_H_
#define _AVI_KERNEL_H_

typedef struct
{
  INT32U  bi_size;
  INT32U  bi_width;
  INT32U  bi_height;
  INT16U  bi_planes;
  INT16U  bi_bit_count;
  INT32U  bi_compression;
  INT32U  bi_size_image;
  INT32U  bi_x_pels_per_meter;
  INT32U  bi_y_pels_per_meter;
  INT32U  bi_clr_used;
  INT32U  bi_clr_important;
} alBITMAPINFOHEADER;

typedef struct alwaveformatex_tag
{
  INT16U  w_format_tag;
  INT16U  n_channels;
  INT32U  n_samples_per_sec;
  INT32U  n_avg_bytes_per_sec;
  INT16U  n_block_align;
  INT16U  w_bits_per_sample;
  INT16U  cb_size;
  INT16U  buf[16];	
} AVI_WAVEFORMATEX;

typedef struct
{
  INT32S   	fdes;              /* File descriptor of AVI file */
  INT32S   	fdes2;              /* File descriptor of AVI file */
  INT32U	fsize;
  INT32S   	vids_strh_seen;		// 
  INT32S   	vids_strf_seen;
  INT32S   	auds_strh_seen;
  INT32S   	AVI_errno;

  INT32S	scale;				
  INT32S	rate;				
  INT32S    length;		
  FP64		fps;               /* Frames per second */
  INT8S   	compressor2[8];     /* Type of compressor, 4 bytes + padding for 0 byte */

  INT32S   	video_pos;         // video position in the bit streams.
  INT32S    video_tag;			// the first video track's tag.
  INT32S   	audio_pos;			// audio position in the bit streams.
  INT32S    audio_tag;			// the first audio track's tag.

  INT32S    audio_flag;	    // audio_flag = 0;  read from audio frame head, should check the audio tag. 
							// audio_flag = 1;  read among the audio frame, don not check the tag.
  INT32S    audio_leftlen;     // The length of data could be read from an audio frame. for ring buffer.
  INT32S    audio_framelen;    // The length of an audio frame.  for ring buffer.
  INT32S	anum;                // total number of audio tracks 
  
  INT32S  	movi_start;
  //sunxw added for FF & FB play
  INT32S	idx1_start;			//idx1_start == 0 means no idx1 table.
  INT32S	idx1_length;
  alBITMAPINFOHEADER bitmap_info_header;		//ww 2007.12.4
  AVI_WAVEFORMATEX wave_format_ex;				//ww 2007.12.4

} avi_t;

/////////////////////////////////////////////////////////////////////////////
//		Constant Definition
/////////////////////////////////////////////////////////////////////////////

#define AVI_fileno(a) (a->fdes)

#define AVI_MODE_WRITE  0
#define AVI_MODE_READ   1

#define AVI_WAVE_HEADER_SIZE	25			//sizeof(waveformatex),   ww, 2007.12.4

#define AVI_TAG_MASK			0xffff0000
#define	AVI_ASCII_00DB			0x62640000		//"00db"	
#define AVI_ASCII_00DC			0x63640000		//"00dc"
#define AVI_ASCII_00WB			0x62770000		//"00wb"
#define AVI_ASCII_IDX1			0x31786469		//"idx1"

#define AVI_IXTAG_MASK			0x0000ffff
#define AVI_ASCII_IX00			0x00007869		//"ix00"		//wenli add, 2008.4.9 debug, 
#define AVI_ASCII_JUNK			0x4b4e554a		//"JUNK"		//wenli add, 2008.4.9 debug, 
#endif   //_AVI_KERNEL_H_