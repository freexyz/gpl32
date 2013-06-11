#ifndef _MOV_KERNEL_H_
#define _MOV_KERNEL_H_

#define HEADER_LENGTH 8
typedef struct alwaveformatex_tag
{
  INT16S  	w_format_tag;
  INT16S  	n_channels;
  INT32S	n_samples_per_sec;
  INT32S	n_avg_bytes_per_sec;
  INT16S  	n_block_align;
  INT16S  	w_bits_per_sample;
  INT16S	cb_size;
  INT16S	buf[16];	
} MOV_WAVEFORMATEX;

typedef struct
{
	INT64S	start;
	INT64S	end;        /* byte endpoint in file */
	INT64S	size;       /* byte size for writing */
	INT32S	use_64;     /* Use 64 bit header */
	BOOLEAN type[4];
} quicktime_atom_t;

typedef struct
{
	INT32S	seed;
	INT32S	flags;
	INT32S	size;
	INT16U	*alpha;
	INT16U	*red;
	INT16U	*green;
	INT16U	*blue;
} quicktime_ctab_t;


//===========================================================================//
typedef struct
{
	char		format[4];
	INT8U		reserved[6];
	INT32S	data_reference;
/* common to audio and video */
	INT32S	version;
	INT32S	revision;
	char		vendor[4];
/* video description */
	INT32S	temporal_quality;
	INT32S	spatial_quality;
	INT32S	width;
	INT32S	height;
	FP32		dpi_horizontal;
	FP32		dpi_vertical;
	INT32S	data_size;
	INT32S	frames_per_sample;
	char		compressor_name[32];
	INT32S	depth;
	INT32S	ctab_id;
	quicktime_ctab_t ctab;
	INT32S	fields;    
  INT32S	dominance;   
  INT32S	has_fiel;
/* audio description */
	INT32U	channels;
	INT32U	sample_size;
/* Audio extension for version == 2 */
  INT32U	formatSpecificFlags;
  INT32U	constBytesPerAudioPacket;
  INT32U	constLPCMFramesPerAudioPacket;
/* LQT: We have int16_t for the compression_id, because otherwise negative
   values don't show up correctly */
  INT16S	compression_id;
	INT32S	packet_size;
  FP64		samplerate;
/* Audio extension for version == 1 */
  INT32U	audio_samples_per_packet;
  INT32U	audio_bytes_per_packet;
  INT32U	audio_bytes_per_frame;
  INT32U	audio_bytes_per_sample;
} stsd_table_t;

/* time to sample */
typedef struct
{
	INT32S	sample_count;
	INT32S	sample_duration;
} stts_table_t;

typedef struct
{
	INT32S	total_entries;
	stsd_table_t *table;
} stsd_t;

typedef struct
{
	INT32S	total_entries;
  INT32S	entries_allocated;
  INT32S	default_duration;
	stts_table_t *table;
} stts_t;

typedef struct
{
	INT32S	total_entries;
	INT32S	entries_allocated;
	INT32S	*p_sample;
} stss_t;

/* sample to chunk */
typedef struct
{
	INT32S	chunk;
	INT32S	samples;
	INT32S	id;
} stsc_table_t;

typedef struct
{
	INT32S	total_entries;	
	INT32S	entries_allocated;
	stsc_table_t *table;
} stsc_t;

//not need use 64 bits value
typedef struct
{
	INT32S 	sample_size;		 //INT64S 	sample_size;
	INT32S	total_entries;
	INT32S	entries_allocated;   /* used by the library for allocating a table */
	INT32S	*p_size;			 //INT64S 	*p_size;
} stsz_t;

typedef struct
{
	INT32S	total_entries;
	INT32S	entries_allocated;    /* used by the library for allocating a table */
	INT32S 	*p_offset;			  //INT64S 	*p_offset;
} stco_t;

typedef struct
{
	stsd_t stsd;
	stts_t stts;
	stss_t stss;
	stsc_t stsc;
	stsz_t stsz;
	stco_t stco;
} stbl_t;


typedef struct
{	
	INT32S	has_video;
	INT32S	has_audio;
	stbl_t	video_stbl;
	stbl_t	audio_stbl;

	MOV_WAVEFORMATEX wave_format_info;
	INT32S	scale;
	INT32S	duration;
	INT32S	rate;

	INT32S	video_stsz_index;
	INT32S	video_stsc_index;
	INT32S	video_stco_index;
	INT32S	audio_stsz_index;
	INT32S	audio_stsc_index;
	INT32S	audio_stco_index;
	INT32S	frame_num_in_chunk;
	INT32S	frame_cnt_in_chunk;
	INT32S	frame_pos;
	INT32S	audio_pos;
	INT32S	audio_flag;
	INT32S	audio_leftlen;
	INT32S	audio_framelen;
	INT16U	fin;		//video
	INT16U	fin2;		//audio
	INT32S	err_num;
} mov_track;
#endif   //_MOV_KERNEL_H_