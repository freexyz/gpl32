//* Version : 1.00
//* History ;   1. 2008/4/17 shin modify Audio data alignment
//							2. 2008/5/8  shin modified
//               a. Add avi key word check
//               b. modify vedio data offset check

/*Include files */
#include "ap_mov_mjpeg.h"
#include "mov_kernel.h"

#define MOV_ERR_NOERR							0
#define MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT		0x80000001
#define MOV_ERR_NOT_ENOUGH_BSBUFFER				0x80000002
#define MOV_ERR_CMOV_NOT_SUPPORT				0x80000003
#define MOV_ERR_AUDIO_FORMAT_NOT_SUPPORT		0x80000004
#define C_MJPEG_READ_FILE_BUFFER_SIZE		4096

static INT32U output_format;
static INT8U output_ratio;             // 0=Fit to output_buffer_width and output_buffer_height, 1=Maintain ratio and fit to output_buffer_width or output_buffer_height, 2=Same as 1 but without scale up
static INT16U output_buffer_width;
static INT16U output_buffer_height;
static INT16U output_image_width;
static INT16U output_image_height;
static INT32U out_of_boundary_color;
static INT32U output_buffer_pointer;

// JPEG variables
static INT16U image_valid_width, image_valid_height, jpeg_image_yuv_mode;
static INT32U jpeg_width, jpeg_height;
static INT32U jpeg_fifo_register = C_JPG_FIFO_32LINE;
static INT32U fifo_line_num = 32;

// Buffer addresses
static INT32U jpeg_out_y, jpeg_out_cb, jpeg_out_cr;
static INT32U scaler_out_buffer;

// Scaler variables
static INT32U scaler_fifo_register = C_SCALER_CTRL_FIFO_32LINE;

mov_track		track;
mov_track		*MOV;

INT32S tk_mov_decode_file_id(INT32U FileNum);
INT32S tk_mov_get_audio_samplerate(void);
INT32S tk_mov_find_adpos(INT32U framenum);
INT32S tk_mov_find_vdpos(INT32U JumpFrameNum);
INT32S tk_mov_get_scale(void);
INT32S tk_mov_get_duration(void);
INT32S tk_mov_get_frame_number(void);
INT32S tk_mov_get_hassound(void);
INT32S tk_mov_get_width(void);
INT32S tk_mov_get_height(void);
INT32S tk_mov_wave_bitformat_get(void);
INT32S tk_mov_wave_channelnum_get(void);
INT32S mov_parsing(INT16U fin,INT16U fin2);
INT32U tk_mov_close_file_id(void);
INT32S tk_mov_read_audio_data(CHAR *buf, INT32U len);
void   tk_mov_release_memory(void);
FP64   tk_mov_get_fps(void);
INT8U *tk_mov_get_waveformatex(void);
INT32S mov_mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
INT32S mov_mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct);
INT32S mov_mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser);
INT32S mov_mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct);
#ifndef _DPF_PROJECT
INT32U tk_mov_decode_file_parsing(INT16S mov_movhandle, INT16S mov_audhandle);
INT32S mov_mjpeg_file_decode_nth_frame(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U nth);
#endif
/*== mov file read function part start==*/
//FILE == file handle (INT16U);
static INT32S ftell(INT16U fin)
{
	return (INT32S)lseek(fin,0,SEEK_CUR);
}

static void* calloc(INT32U num, INT32U unitnum)
{
	return gp_malloc(num*unitnum);
}

static void* realloc(void* ptr, INT32U num)
{
	gp_free(ptr);
	return gp_malloc(num);
}

static INT32S isalpha(INT8U date)
{
	//'A'=>0x41, 'Z'=>0x5A,	'a'=>0x61, 'z'=>0x7A.
	if((date >= 0x61)&&(date <= 0x7A))
	{
		return 0;
	}
	else if((date >= 0x41)&&(date <= 0x52))
	{
		return 0;
	}
	else
		return 1;
}

static INT32S reset(quicktime_atom_t *atom)
{
	atom->end = 0;
	atom->type[0] = atom->type[1] = atom->type[2] = atom->type[3] = 0;
	return 0;
}

static INT32S read_type(INT8U *data, INT8U *type)
{
	type[0] = data[4];
	type[1] = data[5];
	type[2] = data[6];
	type[3] = data[7];
/* need this for quicktime_check_sig */
	if((isalpha(type[0]) && isalpha(type[1])) && (isalpha(type[2]) && isalpha(type[3])))
		return 1;
	else
		return 0;
}

static INT32U read_size(INT8U *data)
{
	INT32U	result;
	INT32U	a, b, c, d;
	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];
	result = (a << 24) | (b << 16) | (c << 8) | d;
	return result;
}

void mov_atom_skip(INT16U	fin, quicktime_atom_t *atom)
{
    if(atom->start == atom->end)
    	atom->end++;
		lseek(fin,atom->end,SEEK_SET);
}

INT32S mov_match_32(void *_input, void *_output)
{
	INT8U *input = (INT8U *)_input;
  INT8U *output = (INT8U *)_output;
	if(input[0] == output[0] &&
		input[1] == output[1] &&
		input[2] == output[2] &&
		input[3] == output[3])
		return 1;
	else
		return 0;
}

static INT64S read_size64(INT8U *data)
{
	INT64U result, a, b, c, d, e, f, g, h;

	a = data[0];
	b = data[1];
	c = data[2];
	d = data[3];
	e = data[4];
	f = data[5];
	g = data[6];
	h = data[7];

	result = (a << 56) |
		(b << 48) |
		(c << 40) |
		(d << 32) |
		(e << 24) |
		(f << 16) |
		(g << 8) |
		h;

	if(result < HEADER_LENGTH)
		result = HEADER_LENGTH;
	return (INT64S)result;
}

INT32S mov_atom_is(quicktime_atom_t *atom, CHAR *type)
{
	if(atom->type[0] == (INT8U)type[0] &&
		 atom->type[1] == (INT8U)type[1] &&
		 atom->type[2] == (INT8U)type[2] &&
		 atom->type[3] == (INT8U)type[3])
		return 1;
	else
		return 0;
}

/*== mov file read function part end==*/

/*== mov global value getting start ==*/
FP64 tk_mov_get_fps(void)
{
	FP64 temp;
	temp = (FP64)MOV->scale / (FP64)MOV->duration;
	return temp * (FP64)MOV->video_stbl.stsz.total_entries;
}

INT32S tk_mov_get_width(void)
{
	stsd_table_t *temp;
	temp = MOV->video_stbl.stsd.table;
	return temp->width;
}

INT32S tk_mov_get_height(void)
{
	stsd_table_t *temp;
	temp = MOV->video_stbl.stsd.table;
	return temp->height;
}

INT32S tk_mov_get_hassound(void)
{
	return MOV->has_audio;
}

INT8U * tk_mov_get_waveformatex(void)
{
	return (INT8U*)&MOV->wave_format_info;
}

INT32S tk_mov_get_frame_number(void)
{
	return MOV->video_stbl.stsz.total_entries;
}

INT32S tk_mov_wave_channelnum_get(void)
{
	return MOV->wave_format_info.n_channels;
}

INT32S tk_mov_get_audio_samplerate(void)
{
	return MOV->wave_format_info.n_samples_per_sec;
}

INT32S tk_mov_wave_bitformat_get(void)
{
	return MOV->wave_format_info.w_bits_per_sample;
}

INT32S tk_mov_get_scale(void)
{
	return MOV->scale;
}

INT32S tk_mov_get_duration(void)
{
	return MOV->duration;
}
/*== mov global value getting end ==*/

INT32S mov_atom_read_header(INT16U fin, quicktime_atom_t *atom)
{
	INT32S	result = 0;
	INT8U	header[8];

	reset(atom);
	//ftell == get current file offset;
	atom->start = lseek(fin,0,SEEK_CUR);
	//if(!fread(header,8,1,fin))
	if(!read(fin,(INT32U)header,8))
		return 1;
	result = read_type(header, atom->type);
	atom->size = read_size(header);
	atom->end = atom->start + atom->size;

// Skip placeholder atom //
	if(mov_match_32(atom->type, "wide"))
	{
		atom->start = lseek(fin,0,SEEK_CUR);
		reset(atom);
		if(!read(fin,(INT32U)header,8))
			return 1;
		result = read_type(header, atom->type);
		atom->size -= 8;
		if(atom->size <= 0)
		{
			// Wrapper ended.  Get new atom size //
			atom->size = read_size(header);
		}
		atom->end = atom->start + atom->size;
	}
	else
// Get extended size //
	if(atom->size == 1)
	{
		if(!read(fin,(INT32U)header,8))
			return 1;
		atom->size = read_size64(header);
		atom->end = atom->start + atom->size;
	}
	return result;
}

void mov_read_stsd_audio(INT16U fin, stsd_table_t *table, quicktime_atom_t *parent_atom)
{
	quicktime_atom_t leaf_atom;
	table->version = quicktime_read_int16(fin);
	table->revision = quicktime_read_int16(fin);
	read(fin, (INT32U)&table->vendor, 4);

  if(table->version < 2)
  {
  	table->channels = quicktime_read_int16(fin);
    table->sample_size = quicktime_read_int16(fin);
    table->compression_id = quicktime_read_int16(fin);
    table->packet_size = quicktime_read_int16(fin);
    table->samplerate = quicktime_read_fixed32(fin);
    // Kluge for fixed32 limitation
    if(table->samplerate + 65536 == 96000 || table->samplerate + 65536 == 88200)
		table->samplerate += 65536;
    if(table->version == 1)
    {
		table->audio_samples_per_packet = quicktime_read_int32(fin);
		table->audio_bytes_per_packet = quicktime_read_int32(fin);
		table->audio_bytes_per_frame  = quicktime_read_int32(fin);
		table->audio_bytes_per_sample  = quicktime_read_int32(fin);
      	if(table->version == 2) // Skip another 20 bytes ???
		{
			//this program could run here ???  gaoxin 2007.12.14
		 	//quicktime_set_position(file, quicktime_position(file) + 20);
			lseek(fin,20,SEEK_CUR);
		}
     }
   }
   else /* SoundDescriptionV2 */
   {
   		/*
      *  SInt16     always3;
      *  SInt16     always16;
      *  SInt16     alwaysMinus2;
      *  SInt16     always0;
      *  UInt32     always65536;
      *  UInt32     sizeOfStructOnly;
      */
	  lseek(fin,16,SEEK_CUR);
      /*
      * Float64    audioSampleRate;
      */
      table->samplerate = quicktime_read_double64(fin);
	  /*
  	  * UInt32     numAudioChannels;
      */
	  table->channels = quicktime_read_int32(fin);
      /*
      * SInt32     always7F000000;
      */
	  lseek(fin,4,SEEK_CUR);
      table->sample_size = quicktime_read_int32(fin);
      table->formatSpecificFlags = quicktime_read_int32(fin);
      /* The following 2 are (hopefully) unused */
      table->constBytesPerAudioPacket = quicktime_read_int32(fin);
      table->constLPCMFramesPerAudioPacket = quicktime_read_int32(fin);
	  lseek(fin,8,SEEK_CUR);
    }
    MOV->wave_format_info.n_channels = table->channels;
	MOV->wave_format_info.n_samples_per_sec = (int)table->samplerate;

	if(MOV->wave_format_info.w_format_tag==WAVE_FORMAT_PCM)
		MOV->wave_format_info.w_bits_per_sample = (short)table->sample_size;
	else if(MOV->wave_format_info.w_format_tag==WAVE_FORMAT_ALAW || MOV->wave_format_info.w_format_tag==WAVE_FORMAT_MULAW)
		MOV->wave_format_info.w_bits_per_sample = 8;
	else
		MOV->wave_format_info.w_bits_per_sample = 4;

	if(	MOV->wave_format_info.w_format_tag==WAVE_FORMAT_PCM ||
	   	MOV->wave_format_info.w_format_tag==WAVE_FORMAT_ALAW ||
		MOV->wave_format_info.w_format_tag==WAVE_FORMAT_MULAW)
		MOV->wave_format_info.n_block_align = (MOV->wave_format_info.w_bits_per_sample*MOV->wave_format_info.n_channels)/8;//only for pcm,alaw and ulaw
	else
		MOV->wave_format_info.n_block_align = table->audio_bytes_per_frame;

		/* Read additional atoms */
    while(ftell(fin) < parent_atom->end)
    {
		mov_atom_read_header(fin, &leaf_atom);
	    mov_atom_skip(fin, &leaf_atom);
    }
}

void mov_default_ctab(quicktime_ctab_t * ctab, INT32S depth)
{
	switch(depth)
  {
    case 1:
      ctab->size = 2;
      break;
    case 2:
      ctab->size = 4;
      break;
    case 4:
      ctab->size = 16;
      break;
    case 8:
      ctab->size = 256;
      break;
    case 34:
      ctab->size = 4;
      break;
    case 36:
      ctab->size = 16;
      break;
    case 40:
      ctab->size = 256;
      break;
    default: /* No palette available */
			return;
  }
}

void mov_read_stsd_video(INT16U fin, stsd_table_t *table, quicktime_atom_t *parent_atom)
{
	quicktime_atom_t leaf_atom;
	INT32S len, bits_per_pixel;

	table->version = quicktime_read_int16(fin);
	table->revision = quicktime_read_int16(fin);
	read(fin, (INT32U)table->vendor, 4);
	table->temporal_quality = quicktime_read_int32(fin);
	table->spatial_quality = quicktime_read_int32(fin);
	table->width = quicktime_read_int16(fin);
	table->height = quicktime_read_int16(fin);
	table->dpi_horizontal = quicktime_read_fixed32(fin);
	table->dpi_vertical = quicktime_read_fixed32(fin);
	table->data_size = quicktime_read_int32(fin);
	table->frames_per_sample = quicktime_read_int16(fin);
	len = quicktime_read_char(fin);
	read(fin, (INT32U)table->compressor_name, 31);
	table->depth = quicktime_read_int16(fin);
	table->ctab_id = quicktime_read_int16(fin);

  /*  If ctab_id is zero, the colortable follows immediately
   *  after the ctab ID
  */
  bits_per_pixel = table->depth & 0x1f;
	if(!table->ctab_id &&
  	((bits_per_pixel == 1) ||
    (bits_per_pixel == 2) ||
    (bits_per_pixel == 4) ||
    (bits_per_pixel == 8)))
  {
		//quicktime_read_ctab(fin, &(table->ctab));
  }
	else
    mov_default_ctab(&(table->ctab), table->depth);

	while(ftell(fin) + 8 < parent_atom->end)
	{
		mov_atom_read_header(fin, &leaf_atom);
		if(mov_atom_is(&leaf_atom, "fiel"))
		{
			table->fields = quicktime_read_char(fin);
			table->dominance = quicktime_read_char(fin);
	        table->has_fiel = 1;
		}
		else
		{
		    mov_atom_skip(fin, &leaf_atom);
		}
	}
}

INT32S mov_read_stsd_table(INT16U fin, stsd_table_t *table,int mode)
{
	quicktime_atom_t leaf_atom;
	mov_atom_read_header(fin, &leaf_atom);

	table->format[0] = leaf_atom.type[0];
	table->format[1] = leaf_atom.type[1];
	table->format[2] = leaf_atom.type[2];
	table->format[3] = leaf_atom.type[3];

	read(fin, (INT32U)table->reserved, 6);
	table->data_reference = quicktime_read_int16(fin);

	if (mov_match_32(table->format, "pano"))
		return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
	else if (mov_match_32(table->format, "qtvr"))
		return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
	else if (mov_match_32(table->format, "\0\0\0\0") )
		return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
  else if (mov_match_32(table->format, "text"))
		return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
  else if (mov_match_32(table->format, "tx3g"))
		return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
	else
	{
	  if(mode==2)
		{//only support 8bit pcm now
			if(mov_match_32(table->format, "raw ")|| mov_match_32(table->format, "sowt"))
				MOV->wave_format_info.w_format_tag = WAVE_FORMAT_PCM;
			else if(mov_match_32(table->format, "alaw"))
				MOV->wave_format_info.w_format_tag = WAVE_FORMAT_ALAW;
			else if(mov_match_32(table->format, "ulaw"))
				MOV->wave_format_info.w_format_tag = WAVE_FORMAT_MULAW;
			else
				return MOV_ERR_AUDIO_FORMAT_NOT_SUPPORT;
			mov_read_stsd_audio(fin, table, &leaf_atom);
		}
    if(mode==1)
		{
			if(mov_match_32(table->format, "jpeg"))
				mov_read_stsd_video(fin, table, &leaf_atom);
			else if(mov_match_32(table->format, "mjpa"))
				mov_read_stsd_video(fin, table, &leaf_atom);
			else
				return MOV_ERR_VIDEO_FORMAT_NOT_SUPPORT;
		}
	}
	return 0;
}

INT32S mov_read_stsd(INT16U fin, stsd_t *stsd, INT32S mode)
{
	INT32S i;
	INT32S err;

	lseek(fin,4,SEEK_CUR);
	stsd->total_entries = quicktime_read_int32(fin);
	stsd->table = (stsd_table_t *)calloc(stsd->total_entries, sizeof(stsd_table_t));
	for(i = 0; i < stsd->total_entries; i++)
	{
		err = mov_read_stsd_table(fin,&(stsd->table[i]),mode);
		if(err!=MOV_ERR_NOERR)
			return err;
    }
	return 0;
}

void mov_read_stts(INT16U fin, stts_t *stts)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stts->total_entries = quicktime_read_int32(fin);

	stts->table = (stts_table_t *)gp_malloc(sizeof(*stts->table) * stts->total_entries);
	for(i = 0; i < stts->total_entries; i++)
  {
		stts->table[i].sample_count = quicktime_read_int32(fin);
		stts->table[i].sample_duration = quicktime_read_int32(fin);
  }
}

void mov_read_stss(INT16U fin, stss_t *stss)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stss->total_entries = quicktime_read_int32(fin);

	if(stss->entries_allocated < stss->total_entries)
	{
		stss->entries_allocated = stss->total_entries;
		stss->p_sample = (INT32S*)realloc((void *)stss->p_sample, sizeof(INT32S) * stss->entries_allocated);
	}
	for(i = 0; i < stss->total_entries; i++)
	{
		stss->p_sample[i] = quicktime_read_int32(fin);
	}
}

void mov_read_stsc(INT16U fin, stsc_t *stsc)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stsc->total_entries = quicktime_read_int32(fin);

	stsc->entries_allocated = stsc->total_entries;
	stsc->table = (stsc_table_t*)gp_malloc(sizeof(stsc_table_t) * stsc->total_entries);
	for(i = 0; i < stsc->total_entries; i++)
	{
		stsc->table[i].chunk = quicktime_read_int32(fin);
		stsc->table[i].samples = quicktime_read_int32(fin);
		stsc->table[i].id = quicktime_read_int32(fin);
	}
}

void mov_read_stsz(INT16U fin, stsz_t *stsz)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stsz->sample_size = quicktime_read_int32(fin);
	stsz->total_entries = quicktime_read_int32(fin);
	stsz->entries_allocated = stsz->total_entries;
	if(!stsz->sample_size)
	{
		//not need use 64 bits value
		stsz->p_size = (INT32S*)calloc(sizeof(INT32S), stsz->entries_allocated);
		for(i = 0; i < stsz->total_entries; i++)
		{
			stsz->p_size[i] = quicktime_read_int32(fin);
		}
	}
}

void mov_read_stco(INT16U fin, stco_t *stco)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stco->total_entries = quicktime_read_int32(fin);
	stco->entries_allocated = stco->total_entries;
	stco->p_offset = (INT32S*)calloc(1, sizeof(INT32S) * stco->entries_allocated);
	for(i = 0; i < stco->total_entries; i++)
	{
		stco->p_offset[i] = quicktime_read_uint32(fin);
	}
}

void mov_read_stco64(INT16U fin, stco_t *stco)
{
	INT32S i;
	lseek(fin,4,SEEK_CUR);
	stco->total_entries = quicktime_read_int32(fin);
	stco->entries_allocated = stco->total_entries;
	stco->p_offset = (INT32S *)calloc(1, sizeof(INT32S) * stco->entries_allocated);
	for(i = 0; i < stco->total_entries; i++)
	{
		stco->p_offset[i] = quicktime_read_int64(fin);
	}
}

INT32S mov_read_stbl(INT16U fin, stbl_t *stbl, quicktime_atom_t *parent_atom, INT32S mode)
{
	quicktime_atom_t leaf_atom;
	INT32S err;

	do
  {
		mov_atom_read_header(fin, &leaf_atom);

    /* mandatory */
		if(mov_atom_is(&leaf_atom, "stsd"))
		{
			err = mov_read_stsd(fin, &(stbl->stsd),mode);
			if((err!=MOV_ERR_NOERR)&&(err!=MOV_ERR_AUDIO_FORMAT_NOT_SUPPORT))
				return err;
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "stts"))
		{
			mov_read_stts(fin, &(stbl->stts));
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "ctts"))
		{
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "stss"))
		{
			mov_read_stss(fin, &(stbl->stss));
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "stsc"))
		{
			mov_read_stsc(fin, &(stbl->stsc));
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "stsz"))
		{
			mov_read_stsz(fin, &(stbl->stsz));
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "co64"))
		{
			mov_read_stco64(fin, &(stbl->stco));
			mov_atom_skip(fin, &leaf_atom);
		}
		else if(mov_atom_is(&leaf_atom, "stco"))
		{
			mov_read_stco(fin, &(stbl->stco));
			mov_atom_skip(fin, &leaf_atom);
		}
		else
			mov_atom_skip(fin, &leaf_atom);
    }while(ftell(fin) < parent_atom->end);

	return 0;
}

INT32S mov_read_minf(INT16U fin, mov_track *trak, quicktime_atom_t *parent_atom)
{
	quicktime_atom_t leaf_atom;
	INT32S err;
	INT32S stbl_mode = 0;	//1: video ; 2:audio
	do
  {
		mov_atom_read_header(fin, &leaf_atom);

    /* mandatory */
		if(mov_atom_is(&leaf_atom, "vmhd"))
		{
			if(trak->has_video ==0)
			{
				trak->has_video = 1;
				stbl_mode = 1;
				mov_atom_skip(fin, &leaf_atom);
			}
		}
		else if(mov_atom_is(&leaf_atom, "smhd"))
		{
			if(trak->has_audio == 0)
			{
				trak->has_audio = 1;
				stbl_mode = 2;
				mov_atom_skip(fin, &leaf_atom);
			}
		}
		else if(mov_atom_is(&leaf_atom, "stbl"))
		{
			if(stbl_mode==1)
			{
				err = mov_read_stbl(fin, &trak->video_stbl, &leaf_atom,stbl_mode);
				if(err!=MOV_ERR_NOERR)
					return err;
			}
			else if(stbl_mode==2)
			{
				err = mov_read_stbl(fin, &trak->audio_stbl, &leaf_atom,stbl_mode);
				if(err!=MOV_ERR_NOERR)
					continue;
			}
		}
		else
			mov_atom_skip(fin, &leaf_atom);
    }while(ftell(fin) < parent_atom->end);
	return 0;
}


INT32S mov_read_mdia(INT16U fin, mov_track *trak,quicktime_atom_t *trak_atom)
{
	quicktime_atom_t leaf_atom;
	INT32S scale;
	INT32S duration;
	INT32S err;

	do
	{
		mov_atom_read_header(fin, &leaf_atom);
		if(mov_atom_is(&leaf_atom, "mdhd"))
		{
			lseek(fin,12,SEEK_CUR);
			scale = quicktime_read_int32(fin);
			duration = quicktime_read_int32(fin);
			lseek(fin,(int)(leaf_atom.size-28),SEEK_CUR);
		}
		else
		if(mov_atom_is(&leaf_atom, "minf"))
		{
			err = mov_read_minf(fin, trak, &leaf_atom);
			if(err!=MOV_ERR_NOERR)
				return err;
		}
		else
			mov_atom_skip(fin, &leaf_atom);
	}while(ftell(fin) < trak_atom->end);

	if(trak->has_video==1 && trak->scale==0 && trak->duration==0)
	{
		trak->scale = scale;
		trak->duration = duration;
	}
	return 0;
}

INT32S mov_read_trak(INT16U fin, mov_track *trak, quicktime_atom_t *trak_atom)
{
	quicktime_atom_t leaf_atom;
	INT32S err;
	do
  {
		mov_atom_read_header(fin, &leaf_atom);
		if(mov_atom_is(&leaf_atom, "mdia"))
		{
			err = mov_read_mdia(fin, trak, &leaf_atom);
			if(err!=MOV_ERR_NOERR)
				return err;
		}
		else
			mov_atom_skip(fin, &leaf_atom);
  } while(ftell(fin) < trak_atom->end);
  return 0;
}

INT32S mov_read_track(INT16U fin, mov_track *trak, quicktime_atom_t *parent_atom)
{
	quicktime_atom_t leaf_atom;
	INT32S err;

  	do
  	{
		mov_atom_read_header(fin, &leaf_atom);

      	if(mov_atom_is(&leaf_atom, "cmov"))
      	{
			return MOV_ERR_CMOV_NOT_SUPPORT;
	    }
		else if(mov_atom_is(&leaf_atom, "trak"))
		{
			err = mov_read_trak(fin, trak, &leaf_atom);
			if(err!=MOV_ERR_NOERR)
				return err;
		}
		else
			mov_atom_skip(fin, &leaf_atom);
	}while(ftell(fin) < parent_atom->end);
	return 0;
}

INT32S mov_parsing(INT16U fin,INT16U fin2)
{
	INT32S	result;
	quicktime_atom_t leaf_atom;
	INT32S	err = MOV_ERR_NOERR;

	lseek(fin,0,SEEK_SET);
	lseek(fin2,0,SEEK_SET);

	gp_memset((INT8S *)&track,0,sizeof(mov_track));
	MOV = &track;
	do
  	{
		result = mov_atom_read_header(fin, &leaf_atom);
    	if(!result)
    	{
			if(mov_atom_is(&leaf_atom, "moov"))
    		{
        		err = mov_read_track(fin, MOV, &leaf_atom);
				if(err!=MOV_ERR_NOERR)
					return err;
     		}
		    else
		      	mov_atom_skip(fin, &leaf_atom);
    	}
   	}while(!result );

	MOV->frame_pos = MOV->video_stbl.stco.p_offset[0];
	MOV->frame_num_in_chunk =  MOV->video_stbl.stsc.table[0].samples;
	MOV->fin = fin;
	MOV->fin2 = fin2;
	return err;
}

/*== decode mov date start ==*/
INT32S mov_audio_seek(void)
{
	INT32S len=0;

	if(MOV->audio_stco_index<MOV->audio_stbl.stco.total_entries)
	{
		MOV->audio_pos = MOV->audio_stbl.stco.p_offset[MOV->audio_stco_index];
		MOV->audio_stco_index++;
	}
	else
		return -1;

	if(MOV->audio_stbl.stsc.total_entries==1)
		len = MOV->audio_stbl.stsc.table[0].samples;

	if(MOV->audio_stsc_index<MOV->audio_stbl.stsc.total_entries-1)
	{
		if(MOV->audio_stco_index>=MOV->audio_stbl.stsc.table[MOV->audio_stsc_index+1].chunk)
			MOV->audio_stsc_index++;
		len = MOV->audio_stbl.stsc.table[MOV->audio_stsc_index].samples;
	}

	if(MOV->wave_format_info.w_format_tag==WAVE_FORMAT_PCM)
	{
		len *= (MOV->wave_format_info.w_bits_per_sample>>3)*MOV->wave_format_info.n_channels;
	}
	else if(MOV->wave_format_info.w_format_tag==WAVE_FORMAT_ALAW
		|| MOV->wave_format_info.w_format_tag==WAVE_FORMAT_MULAW)//modify by gaoxin 2008.04.14
	{
		len /= MOV->audio_stbl.stsd.table[0].audio_samples_per_packet;
		len *= MOV->wave_format_info.n_block_align;
	}

	return len;
}
/*== decode mov date end ==*/

/*== mov control api start==*/
#ifndef	_DPF_PROJECT
INT32U tk_mov_decode_file_parsing(INT16S mov_movhandle, INT16S mov_audhandle)//Roy ADD
{
	MJPEG_HEADER_PARSE_STRUCT header_ptr;

	header_ptr.vedhandle = mov_movhandle;
	header_ptr.audhandle = mov_audhandle;
	header_ptr.status = 1;
	msgQSend(image_msg_queue_id, MSG_MJPEG_PARSE_HEADER_REQ, &header_ptr, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
	return 0;
}
#endif
INT32S tk_mov_decode_file_id(INT32U FileNum)
{
#ifdef _DPF_PROJECT
	INT32S ret = 0;
	STORAGE_FINFO finfo;
 	INT16S mov_movhandle;
 	INT16S mov_audhandle;

	MJPEG_HEADER_PARSE_STRUCT header_ptr;


	#if FS_SCAN_STYLE == FS_SCAN_COMBINATION
 	ret = storage_fopen(curr_storage_id, MEDIA_PHOTO_AVI, FileNum, &finfo);
	#else
	ret = storage_fopen(curr_storage_id, MEDIA_VIDEO_ONLY, FileNum, &finfo);
	#endif
	if(ret == STATUS_OK)
  	{
    	mov_movhandle =  finfo.fd;
		if(gp_strcmp((INT8S*)finfo.f_extname , (INT8S*)"MOV")!=0)
		{
			close(mov_movhandle);
			return 1;
		}
	}


	#if FS_SCAN_STYLE == FS_SCAN_COMBINATION
 	ret = storage_fopen(curr_storage_id, MEDIA_PHOTO_AVI, FileNum, &finfo);
	#else
	ret = storage_fopen(curr_storage_id, MEDIA_VIDEO_ONLY, FileNum, &finfo);
	#endif
	if(ret != STATUS_OK)
  	{
		close(mov_movhandle);
		return 1;
	}
	else
		mov_audhandle = finfo.fd;


	header_ptr.vedhandle = mov_movhandle;
	header_ptr.audhandle = mov_audhandle;
	header_ptr.status = 1;
	msgQSend(image_msg_queue_id, MSG_MJPEG_PARSE_HEADER_REQ, &header_ptr, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
	return ret;
#else
	return 0;
#endif
}

INT32S tk_mov_find_vdpos(INT32U JumpFrameNum)
{
	INT32S	BSlen = 0;
	INT32S	i;
	INT16U	fin;

	fin = MOV->fin;
	if(JumpFrameNum ==0)
		return -1;
  	JumpFrameNum--;

	if(JumpFrameNum==0)
	{
		if(MOV->video_stbl.stsz.total_entries==1)
			BSlen = MOV->video_stbl.stsz.sample_size;
		else if((MOV->video_stsz_index < MOV->video_stbl.stsz.total_entries)&&(MOV->video_stsz_index>=0))
			BSlen = MOV->video_stbl.stsz.p_size[MOV->video_stsz_index];
		else
			return -1;

		//find JPEG data offset position.
		lseek(fin,MOV->frame_pos,SEEK_SET);
		MOV->frame_pos += BSlen;

		MOV->video_stsz_index++;
		MOV->frame_cnt_in_chunk++;
		if(MOV->frame_cnt_in_chunk >= MOV->frame_num_in_chunk)
		{
			MOV->frame_cnt_in_chunk = 0;
			MOV->video_stco_index++;
			if(MOV->video_stco_index < MOV->video_stbl.stco.total_entries)
			{
				MOV->frame_pos = MOV->video_stbl.stco.p_offset[MOV->video_stco_index];
				if(MOV->video_stsc_index < (MOV->video_stbl.stsc.total_entries-1))
				{
					if(MOV->video_stco_index >= MOV->video_stbl.stsc.table[MOV->video_stsc_index+1].chunk-1)
					{
						MOV->video_stsc_index++;
						MOV->frame_num_in_chunk =  MOV->video_stbl.stsc.table[MOV->video_stsc_index].samples;
					}
				}
			}
		}
	}
	else
	{
		for(i=0;i<JumpFrameNum;i++)
		{
			if(MOV->video_stbl.stsz.total_entries==1)
				BSlen = MOV->video_stbl.stsz.sample_size;
			else if((MOV->video_stsz_index < MOV->video_stbl.stsz.total_entries)&&(MOV->video_stsz_index>=0))
				BSlen = MOV->video_stbl.stsz.p_size[MOV->video_stsz_index];
			else
				return -1;

			MOV->frame_pos += BSlen;
			MOV->video_stsz_index++;
			MOV->frame_cnt_in_chunk++;
			if(MOV->frame_cnt_in_chunk >= MOV->frame_num_in_chunk)
			{
				MOV->frame_cnt_in_chunk = 0;
				MOV->video_stco_index++;
				if(MOV->video_stco_index < MOV->video_stbl.stco.total_entries)
				{
					MOV->frame_pos = MOV->video_stbl.stco.p_offset[MOV->video_stco_index];
					if(MOV->video_stsc_index < (MOV->video_stbl.stsc.total_entries-1))
					{
						if(MOV->video_stco_index >= MOV->video_stbl.stsc.table[MOV->video_stsc_index+1].chunk-1)
						{
							MOV->video_stsc_index++;
							MOV->frame_num_in_chunk =  MOV->video_stbl.stsc.table[MOV->video_stsc_index].samples;
						}
					}
				}
			}
		}

		if(MOV->video_stbl.stsz.total_entries==1)
			BSlen = MOV->video_stbl.stsz.sample_size;
		else if((MOV->video_stsz_index < MOV->video_stbl.stsz.total_entries)&&(MOV->video_stsz_index>=0))
			BSlen = MOV->video_stbl.stsz.p_size[MOV->video_stsz_index];
		else
			return -1;

		lseek(fin,MOV->frame_pos,SEEK_SET);
		MOV->frame_pos += BSlen;

		MOV->video_stsz_index++;
		MOV->frame_cnt_in_chunk++;
		if(MOV->frame_cnt_in_chunk >= MOV->frame_num_in_chunk)
		{
			MOV->frame_cnt_in_chunk = 0;
			MOV->video_stco_index++;
			if(MOV->video_stco_index < MOV->video_stbl.stco.total_entries)
			{
				MOV->frame_pos = MOV->video_stbl.stco.p_offset[MOV->video_stco_index];
				if(MOV->video_stsc_index < (MOV->video_stbl.stsc.total_entries-1))
				{
					if(MOV->video_stco_index >= MOV->video_stbl.stsc.table[MOV->video_stsc_index+1].chunk-1)
					{
						MOV->video_stsc_index++;
						MOV->frame_num_in_chunk =  MOV->video_stbl.stsc.table[MOV->video_stsc_index].samples;
					}
				}
			}
		}
	}
	return BSlen;
}

INT32S tk_mov_find_adpos(INT32U framenum)
{
	INT16U fin;

	fin = MOV->fin2;
	MOV->audio_framelen=mov_audio_seek();
	if(MOV->audio_framelen==-1)
		return -1;   //read end
	return 	MOV->audio_framelen;
}

void tk_mov_release_memory(void)
{
	if(MOV->has_audio)
	{
		if(MOV->audio_stbl.stco.p_offset!=NULL)
		{
			gp_free(MOV->audio_stbl.stco.p_offset);
			MOV->audio_stbl.stco.p_offset = NULL;
		}
		if(MOV->audio_stbl.stsc.table!=NULL)
		{
			gp_free(MOV->audio_stbl.stsc.table);
			MOV->audio_stbl.stsc.table = NULL;
		}
		if(MOV->audio_stbl.stsd.table!=NULL)
		{
			gp_free(MOV->audio_stbl.stsd.table);
			MOV->audio_stbl.stsd.table = NULL;
		}
		if(MOV->audio_stbl.stss.p_sample!=NULL)
		{
			gp_free(MOV->audio_stbl.stss.p_sample);
			MOV->audio_stbl.stss.p_sample = NULL;
		}
		if(MOV->audio_stbl.stsz.p_size!=NULL)
		{
			gp_free(MOV->audio_stbl.stsz.p_size);
			MOV->audio_stbl.stsz.p_size = NULL;
		}
		if(MOV->audio_stbl.stts.table!=NULL)
		{
			gp_free(MOV->audio_stbl.stts.table);
			MOV->audio_stbl.stts.table = NULL;
		}
	}

	if(MOV->has_video)
	{
		if(MOV->video_stbl.stco.p_offset!=NULL)
		{
			gp_free(MOV->video_stbl.stco.p_offset);
			MOV->video_stbl.stco.p_offset = NULL;
		}
		if(MOV->video_stbl.stsc.table!=NULL)
		{
			gp_free(MOV->video_stbl.stsc.table);
			MOV->video_stbl.stsc.table = NULL;
		}
		if(MOV->video_stbl.stsd.table!=NULL)
		{
			gp_free(MOV->video_stbl.stsd.table);
			MOV->video_stbl.stsd.table = NULL;
		}
		if(MOV->video_stbl.stss.p_sample!=NULL)
		{
			gp_free(MOV->video_stbl.stss.p_sample);
			MOV->video_stbl.stss.p_sample = NULL;
		}
		if(MOV->video_stbl.stsz.p_size!=NULL)
		{
			gp_free(MOV->video_stbl.stsz.p_size);
			MOV->video_stbl.stsz.p_size = NULL;
		}
		if(MOV->video_stbl.stts.table!=NULL)
		{
			gp_free(MOV->video_stbl.stts.table);
			MOV->video_stbl.stts.table = NULL;
		}
	}
	return;
}

INT32U tk_mov_close_file_id(void)
{
	close((INT16S)MOV->fin);
	MOV->fin = 0xffff;
	close((INT16S)MOV->fin2);
	MOV->fin2 = 0xffff;
	return 0;
}

void mov_mjpeg_memory_allocate(INT32U fifo)
{
	INT32U jpeg_output_y_size;
	INT32U jpeg_output_cb_cr_size;
	INT16U cbcr_shift;

	if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420) {
		cbcr_shift = 2;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411) {
		cbcr_shift = 2;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV422) {
		cbcr_shift = 1;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV422V) {
		cbcr_shift = 1;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV444) {
		cbcr_shift = 0;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_GRAYSCALE) {
		cbcr_shift = 20;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411V) {
		cbcr_shift = 2;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420H2) {
		cbcr_shift = 1;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420V2) {
		cbcr_shift = 1;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411H2) {
		cbcr_shift = 1;
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411V2) {
		cbcr_shift = 1;
	} else {
		return;
	}

	if (fifo) {
		jpeg_output_y_size = jpeg_width*fifo*2;
	} else {
		jpeg_output_y_size = jpeg_width*jpeg_height;
	}
	jpeg_output_cb_cr_size = jpeg_output_y_size >> cbcr_shift;

  	jpeg_out_y = (INT32U) gp_malloc_align(jpeg_output_y_size, 8);
  	if (!jpeg_out_y) {
  		return;
  	}
  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
  		jpeg_out_cb = (INT32U) gp_malloc_align(jpeg_output_cb_cr_size, 8);
  		if (!jpeg_out_cb) {
  			gp_free((void *) jpeg_out_y);
  			jpeg_out_y = NULL;
  			return;
  		}
  		jpeg_out_cr = (INT32U) gp_malloc_align(jpeg_output_cb_cr_size, 8);
  		if (!jpeg_out_cr) {
  			gp_free((void *) jpeg_out_cb);
  			jpeg_out_cb = NULL;
  			gp_free((void *) jpeg_out_y);
  			jpeg_out_y = NULL;
  			return;
  		}

  		cache_invalid_range(jpeg_out_y, jpeg_output_y_size);
  		cache_invalid_range(jpeg_out_cb, jpeg_output_cb_cr_size);
		cache_invalid_range(jpeg_out_cr, jpeg_output_cb_cr_size);
  	} else {
  		jpeg_out_cb = 0;
  		jpeg_out_cr = 0;

  		cache_invalid_range(jpeg_out_y, jpeg_output_y_size);
  	}
}

void mov_mjpeg_scaler_set_parameters(INT32U fifo)
{
	INT32U factor;

  #ifdef _DPF_PROJECT
	umi_scaler_color_matrix_load();
  #endif

	scaler_input_pixels_set(jpeg_width, jpeg_height);
	scaler_input_visible_pixels_set(image_valid_width, image_valid_height);
	if (output_image_width && output_image_height) {
	    scaler_output_pixels_set((image_valid_width<<16)/output_image_width, (image_valid_height<<16)/output_image_height, output_buffer_width, output_buffer_height);
	} else {
	    if (!output_image_width) {
	        output_image_width = output_buffer_width;
	    }
	    if (!output_image_height) {
	        output_image_height = output_buffer_height;
	    }
    	if (output_ratio == 0x0) {      // Fit to output buffer width and height
      		scaler_output_pixels_set((image_valid_width<<16)/output_image_width, (image_valid_height<<16)/output_image_height, output_buffer_width, output_buffer_height);
    	} else if (output_ratio==2 && image_valid_width<=output_image_width && image_valid_height<=output_image_height) {
    		scaler_output_pixels_set(1<<16, 1<<16, output_buffer_width, output_buffer_height);
    		output_image_width = image_valid_width;
    		output_image_height = output_buffer_height;
    	} else {						// Fit to output buffer width or height
      		if (output_image_height*image_valid_width > output_image_width*image_valid_height) {
      			factor = (image_valid_width<<16)/output_image_width;
      			output_image_height = (image_valid_height<<16)/factor;
      		} else {
      			factor = (image_valid_height<<16)/output_image_height;
      			output_image_width = (image_valid_width<<16)/factor;
      		}
      		scaler_output_pixels_set(factor, factor, output_buffer_width, output_buffer_height);
      	}
    }

	scaler_input_addr_set(jpeg_out_y, jpeg_out_cb, jpeg_out_cr);

	if (output_buffer_pointer) {
	    scaler_out_buffer = output_buffer_pointer;
	} else {
	    scaler_out_buffer = (INT32U) gp_malloc((output_buffer_width*output_buffer_height)<<1);
	    if (!scaler_out_buffer) {
		    return;
		}
	}

   	scaler_output_addr_set(scaler_out_buffer, NULL, NULL);
   	scaler_fifo_line_set(fifo);
	scaler_YUV_type_set(C_SCALER_CTRL_TYPE_YCBCR);
	if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV444) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV444);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV422) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV422V) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV420);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_GRAYSCALE) {
		scaler_input_format_set(C_SCALER_CTRL_IN_Y_ONLY);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411V) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV411V);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420H2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV420V2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411H2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422);
	} else if (jpeg_image_yuv_mode == C_JPG_CTRL_YUV411V2) {
		scaler_input_format_set(C_SCALER_CTRL_IN_YUV422V);
	}
	scaler_output_format_set(output_format);
	scaler_out_of_boundary_mode_set(1);			// Use Use color defined in scaler_out_of_boundary_color_set()
	scaler_out_of_boundary_color_set(out_of_boundary_color);
}

INT32S mov_mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
{
   	INT32S parse_status;
	INT8U *p_vlc;
	INT32U header_len;
  	INT32U fly_len;
  	INT32S jpeg_status;
  	INT32S scaler_status;
	INT8U scaler_done;
	TK_FILE_SERVICE_STRUCT fs_cmd_a, fs_cmd_b;
	INT32S read_cnt;
	INT8U fifo_a;
	INT8U error;

	if (!img_decode_struct) {
		return -1;
	}
	if (!image_task_fs_queue_a || !image_task_fs_queue_b) {
		img_decode_struct->decode_status = STATUS_FAIL;

		return -1;
	}

    output_format = img_decode_struct->output_format;
    output_ratio = img_decode_struct->output_ratio;
    output_buffer_width = img_decode_struct->output_buffer_width;
    output_buffer_height = img_decode_struct->output_buffer_height;
    output_image_width = img_decode_struct->output_image_width;
    output_image_height = img_decode_struct->output_image_height;
    out_of_boundary_color = img_decode_struct->out_of_boundary_color;
    output_buffer_pointer = img_decode_struct->output_buffer_pointer;

	// Setup a command that will be used to read data into fifo A
	fs_cmd_a.fd = (INT32S) MOV->fin;
	fs_cmd_a.buf_addr = (INT32U) gp_iram_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);
	if (!fs_cmd_a.buf_addr) {
	  	fs_cmd_a.buf_addr = (INT32U) gp_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);

	  	if (!fs_cmd_a.buf_addr) {
	        DBG_PRINT("jpeg_file_decode_and_scale() failed to allocate fs_cmd_a.buf_addr");
	        img_decode_struct->decode_status = STATUS_FAIL;

	  		return -1;
	  	}
	}
	fs_cmd_a.buf_size = C_MJPEG_READ_FILE_BUFFER_SIZE;
	fs_cmd_a.result_queue = image_task_fs_queue_a;

	// Setup a command that will be used to read data into fifo B
	fs_cmd_b.fd = (INT32S) MOV->fin;
	fs_cmd_b.buf_addr = (INT32U) gp_iram_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);
	if (!fs_cmd_b.buf_addr) {
	  	fs_cmd_b.buf_addr = (INT32U) gp_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);

	  	if (!fs_cmd_b.buf_addr) {
	  		DBG_PRINT("jpeg_file_decode_and_scale() failed to allocate fs_cmd_b.buf_addr");
	  		img_decode_struct->decode_status = STATUS_FAIL;
	  		gp_free((void *) fs_cmd_a.buf_addr);

	  		return -1;
	  	}
	}
	fs_cmd_b.buf_size = C_MJPEG_READ_FILE_BUFFER_SIZE;
	fs_cmd_b.result_queue = image_task_fs_queue_b;

	fifo_a = 1;		// First, read data to FIFO A

	header_len = 0;
	// Initiate software header parser and hardware engine
	jpeg_decode_init();
   	do {
		// Send this command to file server task
		msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
		read_cnt = (INT32S) OSQPend(image_task_fs_queue_a, 200, &error);
		if ((error!=OS_NO_ERR) || (read_cnt<=0)) {
			img_decode_struct->decode_status = STATUS_FAIL;
			gp_free((void *) fs_cmd_b.buf_addr);
			gp_free((void *) fs_cmd_a.buf_addr);

	  		return -1;
	  	}
	  	parse_status = jpeg_decode_parse_header((INT8U *) fs_cmd_a.buf_addr, read_cnt);

   		if (parse_status == JPEG_PARSE_NOT_DONE) {
   			header_len += read_cnt;
   		}
	} while (parse_status == JPEG_PARSE_NOT_DONE) ;
	if (parse_status != JPEG_PARSE_OK) {
		DBG_PRINT("Parse header failed. Skip this file\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) fs_cmd_a.buf_addr);
		gp_free((void *) fs_cmd_b.buf_addr);

	  	return -1;
	}

	image_valid_width = jpeg_decode_image_width_get();
	image_valid_height = jpeg_decode_image_height_get();
	jpeg_image_yuv_mode = jpeg_decode_image_yuv_mode_get();
	jpeg_width = jpeg_decode_image_extended_width_get();
	jpeg_height = jpeg_decode_image_extended_height_get();

	if (img_decode_struct->output_ratio == 3) {		// Special case for thumbnail show
		if (image_valid_width > image_valid_height) {
			output_buffer_height = (output_buffer_width*3) >> 2;
			output_buffer_height += 0x10;
			output_buffer_height &= ~0x1F;
			img_decode_struct->output_buffer_height = output_buffer_height;
		} else if (image_valid_width < image_valid_height){
			output_buffer_width = (output_buffer_height*3) >> 2;
			output_buffer_width += 0x10;
			output_buffer_width &= ~0x1F;
			img_decode_struct->output_buffer_width = output_buffer_width;
        }
        img_decode_struct->output_image_width = output_buffer_width;
        img_decode_struct->output_image_height = output_buffer_height;
        output_image_width = output_buffer_width;
	    output_image_height = output_buffer_height;
	}

	msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
	fifo_a ^= 0x1;

	mov_mjpeg_memory_allocate(fifo_line_num);
	if (!jpeg_out_y) {
		DBG_PRINT("Failed to allocate memory in mov_mjpeg_memory_allocate()\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
		if (jpeg_out_cb) {
			gp_free((void *) jpeg_out_cb);
		}
		if (jpeg_out_cr) {
			gp_free((void *) jpeg_out_cr);
		}

		return -1;
	}

	if (jpeg_decode_output_set(jpeg_out_y, jpeg_out_cb, jpeg_out_cr, jpeg_fifo_register)) {
		DBG_PRINT("Failed to call jpeg_decode_output_set()\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
	  	gp_free((void *) fs_cmd_a.buf_addr);
		gp_free((void *) fs_cmd_b.buf_addr);
	  	gp_free((void *) jpeg_out_y);
	  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
	  		gp_free((void *) jpeg_out_cb);
	  		gp_free((void *) jpeg_out_cr);
	  	}

		return -1;
	}

	p_vlc = jpeg_decode_image_vlc_addr_get();

	if (fifo_a) {
		if ((INT32U) p_vlc == (fs_cmd_b.buf_addr + read_cnt)) {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			fifo_a ^= 0x1;
			read_cnt = (INT32S) OSQPend(image_task_fs_queue_a, 200, &error);
   			if ((error!=OS_NO_ERR) || (read_cnt<=0)) {
   			    DBG_PRINT("Failed to read JPEG file\r\n");
   			    img_decode_struct->decode_status = STATUS_FAIL;
                gp_free((void *) fs_cmd_a.buf_addr);
        		gp_free((void *) fs_cmd_b.buf_addr);
        	  	gp_free((void *) jpeg_out_y);
        	  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
        	  		gp_free((void *) jpeg_out_cb);
        	  		gp_free((void *) jpeg_out_cr);
        	  	}

		  		return -1;
		  	}
			p_vlc = (INT8U *) fs_cmd_a.buf_addr;
			fly_len = (fs_cmd_a.buf_addr+read_cnt) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - (C_MJPEG_READ_FILE_BUFFER_SIZE));	// Set maximum VLC length to prevent JPEG from hangging too long
		} else {
			fly_len = (fs_cmd_b.buf_addr+read_cnt) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - ((INT32U) p_vlc - (INT32U) fs_cmd_b.buf_addr));	// Set maximum VLC length to prevent JPEG from hangging too long
		}
	} else {
		if ((INT32U) p_vlc == (fs_cmd_a.buf_addr + read_cnt)) {
			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			fifo_a ^= 0x1;
   			read_cnt = (INT32S) OSQPend(image_task_fs_queue_b, 200, &error);
   			if ((error!=OS_NO_ERR) || (read_cnt<=0)) {
   				DBG_PRINT("Failed to read JPEG file\r\n");
   				img_decode_struct->decode_status = STATUS_FAIL;
			  	gp_free((void *) fs_cmd_a.buf_addr);
        		gp_free((void *) fs_cmd_b.buf_addr);
        	  	gp_free((void *) jpeg_out_y);
        	  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
        	  		gp_free((void *) jpeg_out_cb);
        	  		gp_free((void *) jpeg_out_cr);
        	  	}

		  		return -1;
		  	}
			p_vlc = (INT8U *) fs_cmd_b.buf_addr;
			fly_len = (fs_cmd_b.buf_addr+read_cnt) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - (C_MJPEG_READ_FILE_BUFFER_SIZE));	// Set maximum VLC length to prevent JPEG from hangging too long
		} else {
			fly_len = (fs_cmd_a.buf_addr+read_cnt) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - ((INT32U) p_vlc - (INT32U) fs_cmd_a.buf_addr));	// Set maximum VLC length to prevent JPEG from hangging too long
		}
	}

   	// Now start JPEG decoding on the fly
	if (jpeg_decode_on_the_fly_start(p_vlc, fly_len)) {
		DBG_PRINT("Failed to call jpeg_decode_on_the_fly_start()\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
	  	gp_free((void *) fs_cmd_a.buf_addr);
		gp_free((void *) fs_cmd_b.buf_addr);
	  	gp_free((void *) jpeg_out_y);
	  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
	  		gp_free((void *) jpeg_out_cb);
	  		gp_free((void *) jpeg_out_cr);
	  	}

		return -1;
	}

  	// Initiate Scaler
  	scaler_init();
  	scaler_done = 0;

	// Setup Scaler
	mov_mjpeg_scaler_set_parameters(scaler_fifo_register);
  	if (!scaler_out_buffer) {
		jpeg_decode_stop();
		DBG_PRINT("Failed to allocate scaler_out_buffer\r\n");
		img_decode_struct->decode_status = STATUS_FAIL;
	  	gp_free((void *) fs_cmd_a.buf_addr);
		gp_free((void *) fs_cmd_b.buf_addr);
	  	gp_free((void *) jpeg_out_y);
	  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
	  		gp_free((void *) jpeg_out_cb);
	  		gp_free((void *) jpeg_out_cr);
	  	}

		return -1;
  	}

  	while (1) {
  		jpeg_status = jpeg_decode_status_query(1);

		if (jpeg_status & C_JPG_STATUS_DECODE_DONE) {
		  	// Wait until scaler finish its job
		  	while (!scaler_done) {
		  		scaler_status = scaler_wait_idle();
		  		if (scaler_status == C_SCALER_STATUS_STOP) {
					scaler_start();
				} else if (scaler_status & C_SCALER_STATUS_DONE) {
					break;
				} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
					DBG_PRINT("Scaler failed to finish its job\r\n");
					break;
				} else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) {
		  			scaler_restart();
		  		} else {
			  		DBG_PRINT("Un-handled Scaler status!\r\n");
			  		break;
			  	}
		  	}
			break;
		}		// if (jpeg_status & C_JPG_STATUS_DECODE_DONE)

  		if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
			if (fifo_a) {
				msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				fifo_a ^= 0x1;
	   			read_cnt = (INT32S) OSQPend(image_task_fs_queue_a, 200, &error);
   				if ((error!=OS_NO_ERR) || (read_cnt<0)) {
	   				DBG_PRINT("Failed to read more data from JPEG file\r\n");
		  			break;
			  	}

				// Now restart JPEG decoding on the fly
		  		if (jpeg_decode_on_the_fly_start((INT8U *) fs_cmd_a.buf_addr, fs_cmd_a.buf_size)) {
		  			DBG_PRINT("Failed to call jpeg_decode_on_the_fly_start()\r\n");
		  			break;
		  		}
			} else {
				msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				fifo_a ^= 0x1;
	   			read_cnt = (INT32S) OSQPend(image_task_fs_queue_b, 200, &error);
   				if ((error!=OS_NO_ERR) || (read_cnt<0)) {
	   				DBG_PRINT("Failed to read more data from JPEG file\r\n");
		  			break;
			  	}

				// Now restart JPEG decoding on the fly
		  		if (jpeg_decode_on_the_fly_start((INT8U *) fs_cmd_b.buf_addr, fs_cmd_b.buf_size)) {
		  			DBG_PRINT("Failed to call jpeg_decode_on_the_fly_start()\r\n");
		  			break;
		  		}
			}
		}		// if (jpeg_status & C_JPG_STATUS_INPUT_EMPTY)

		if (jpeg_status & C_JPG_STATUS_OUTPUT_FULL) {
		  	// Start scaler to handle the full output FIFO now
		  	if (!scaler_done) {
			  	scaler_status = scaler_wait_idle();
			  	if (scaler_status == C_SCALER_STATUS_STOP) {
					scaler_start();
			  	} else if (scaler_status & C_SCALER_STATUS_DONE) {
			  		// Scaler might finish its job before JPEG does when image is zoomed in.
			  		scaler_done = 1;
			  	} else if (scaler_status & (C_SCALER_STATUS_TIMEOUT|C_SCALER_STATUS_INIT_ERR)) {
					DBG_PRINT("Scaler failed to finish its job\r\n");
					break;
				} else if (scaler_status & C_SCALER_STATUS_INPUT_EMPTY) {
			  		scaler_restart();
			  	} else {
			  		DBG_PRINT("Un-handled Scaler status!\r\n");
			  		break;
			  	}
			}

	  		// Now restart JPEG to output to next FIFO
	  		if (jpeg_decode_output_restart()) {
	  			DBG_PRINT("Failed to call jpeg_decode_output_restart()\r\n");
	  			break;
	  		}
		}		// if (jpeg_status & C_JPG_STATUS_OUTPUT_FULL)

		if (jpeg_status & C_JPG_STATUS_STOP) {
			DBG_PRINT("JPEG is not started!\r\n");
			break;
		}
		if (jpeg_status & C_JPG_STATUS_TIMEOUT) {
			DBG_PRINT("JPEG execution timeout!\r\n");
			break;
		}
		if (jpeg_status & C_JPG_STATUS_INIT_ERR) {
			DBG_PRINT("JPEG init error!\r\n");
			break;
		}
		if (jpeg_status & C_JPG_STATUS_RST_VLC_DONE) {
			DBG_PRINT("JPEG Restart marker number is incorrect!\r\n");
			break;
		}
		if (jpeg_status & C_JPG_STATUS_RST_MARKER_ERR) {
			DBG_PRINT("JPEG Restart marker sequence error!\r\n");
			break;
		}
  	}

	jpeg_decode_stop();

	scaler_stop();

  	gp_free((void *) jpeg_out_y);
  	if (jpeg_image_yuv_mode != C_JPG_CTRL_GRAYSCALE) {
  		gp_free((void *) jpeg_out_cb);
  		gp_free((void *) jpeg_out_cr);
  	}
  	gp_free((void *) fs_cmd_a.buf_addr);
	gp_free((void *) fs_cmd_b.buf_addr);

	if ((jpeg_status & C_JPG_STATUS_DECODE_DONE) && (scaler_status & C_SCALER_STATUS_DONE)) {
	    if (!output_buffer_pointer) {
	        output_buffer_pointer = scaler_out_buffer;
	    }
	} else {
		img_decode_struct->decode_status = STATUS_FAIL;
    	if (!output_buffer_pointer) {
    	    gp_free((void *) scaler_out_buffer);
    	}

        return -1;
	}

    img_decode_struct->decode_status = STATUS_OK;
    if (!img_decode_struct->output_buffer_pointer && output_buffer_pointer) {
        img_decode_struct->output_buffer_pointer = output_buffer_pointer;
    }
    img_decode_struct->output_image_width = output_image_width;
    img_decode_struct->output_image_height = output_image_height;

	return STATUS_OK;
}

INT32S mov_mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
{
	INT32U header_len;
 	INT32S parse_status;
	INT8U *p_vlc;
	INT8U  err;
	INT32S len;
	INT32U fly_len;
	INT32S mjpeg_status;
	INT8U fifo_a;
	TK_FILE_SERVICE_STRUCT fs_cmd_a, fs_cmd_b;

	if (!image_task_fs_queue_a || !image_task_fs_queue_b) {
		img_decode_struct->decode_status = STATUS_FAIL;

		return -1;
	}

	// Setup a command that will be used to read data into fifo A
	fs_cmd_a.fd = MOV->fin;
	fs_cmd_a.buf_addr = (INT32U) gp_iram_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);
	if (!fs_cmd_a.buf_addr) {
	  	fs_cmd_a.buf_addr = (INT32U) gp_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);

  		if (!fs_cmd_a.buf_addr) {
        	DBG_PRINT("mjpeg_file_decode_without_scale() failed to allocate fs_cmd_a.buf_addr");
			img_decode_struct->decode_status = STATUS_FAIL;

			return STATUS_FAIL;
		}
  	}
	fs_cmd_a.buf_size = C_MJPEG_READ_FILE_BUFFER_SIZE;
	fs_cmd_a.result_queue = image_task_fs_queue_a;

	// Setup a command that will be used to read data into fifo B
	fs_cmd_b.fd = MOV->fin;
	fs_cmd_b.buf_addr = (INT32U) gp_iram_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);
	if (!fs_cmd_b.buf_addr) {
	  	fs_cmd_b.buf_addr = (INT32U) gp_malloc_align(C_MJPEG_READ_FILE_BUFFER_SIZE, 16);
	  	if (!fs_cmd_b.buf_addr) {
	  		DBG_PRINT("mjpeg_file_decode_without_scale() failed to allocate fs_cmd_b.buf_addr");
			img_decode_struct->decode_status = STATUS_FAIL;
	  		gp_free((void *) fs_cmd_a.buf_addr);

			return STATUS_FAIL;
	  	}
	}
	fs_cmd_b.buf_size = C_MJPEG_READ_FILE_BUFFER_SIZE;
	fs_cmd_b.result_queue = image_task_fs_queue_b;

	fifo_a = 1;
	header_len = 0;
	// Initiate software header parser and hardware engine
	mjpeg_decode_init();
 	do  {
 		msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
     	len = (INT32S) OSQPend(image_task_fs_queue_a, 200, &err);
      	if ((err!=OS_NO_ERR) || (len<=0)) {
			img_decode_struct->decode_status = STATUS_FAIL;
			gp_free((void *) fs_cmd_a.buf_addr);
			gp_free((void *) fs_cmd_b.buf_addr);

        	return -1;
      	}
 	  	parse_status = jpeg_decode_parse_header((INT8U *) fs_cmd_a.buf_addr, C_MJPEG_READ_FILE_BUFFER_SIZE);

 	  	if (parse_status == JPEG_PARSE_NOT_DONE) {
   			header_len += len;
   		}
	} while (parse_status == JPEG_PARSE_NOT_DONE) ;
	if (parse_status != JPEG_PARSE_OK) {
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) fs_cmd_a.buf_addr);
		gp_free((void *) fs_cmd_b.buf_addr);

	 	return -1;
	}

	// Send a command to read fifo_b
	msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
	fifo_a ^= 0x1;

	if (mjpeg_decode_output_format_set(SCALER_CTRL_OUT_YUYV)) {
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) fs_cmd_b.buf_addr);
		gp_free((void *) fs_cmd_a.buf_addr);

		return -1;
	}

	if (mjpeg_decode_output_addr_set((INT32U) img_decode_struct->output_buffer_pointer, 0, 0)) {
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) fs_cmd_b.buf_addr);
		gp_free((void *) fs_cmd_a.buf_addr);

		return -1;
	}

	p_vlc = jpeg_decode_image_vlc_addr_get();

	if (fifo_a) {
		if((INT32U) p_vlc == (fs_cmd_b.buf_addr + C_MJPEG_READ_FILE_BUFFER_SIZE)) {
   			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
      		fifo_a ^= 0x1;
      		len = (INT32S) OSQPend(image_task_fs_queue_a, 200, &err);
      		if ((err != OS_NO_ERR) || (len <= 0)) {
				img_decode_struct->decode_status = STATUS_FAIL;
				gp_free((void *) fs_cmd_b.buf_addr);
				gp_free((void *) fs_cmd_a.buf_addr);

        		return -1;
      		}
 	  		p_vlc = (INT8U *) fs_cmd_a.buf_addr;
 	  		fly_len = C_MJPEG_READ_FILE_BUFFER_SIZE;

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - C_MJPEG_READ_FILE_BUFFER_SIZE);	// Set maximum VLC length to prevent JPEG from hangging too long
		} else {
			fly_len = (fs_cmd_b.buf_addr+C_MJPEG_READ_FILE_BUFFER_SIZE) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - ((INT32U) p_vlc - (INT32U) fs_cmd_b.buf_addr));	// Set maximum VLC length to prevent JPEG from hangging too long
		}
	} else {
		if((INT32U) p_vlc == (fs_cmd_a.buf_addr + C_MJPEG_READ_FILE_BUFFER_SIZE)) {
   			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
      		fifo_a ^= 0x1;
      		len = (INT32S) OSQPend(image_task_fs_queue_b, 200, &err);
      		if ((err != OS_NO_ERR) || (len <= 0)) {
				img_decode_struct->decode_status = STATUS_FAIL;
				gp_free((void *) fs_cmd_b.buf_addr);
				gp_free((void *) fs_cmd_a.buf_addr);

        		return -1;
      		}
 	  		p_vlc = (INT8U *) fs_cmd_b.buf_addr;
 	  		fly_len = C_MJPEG_READ_FILE_BUFFER_SIZE;

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - C_MJPEG_READ_FILE_BUFFER_SIZE);	// Set maximum VLC length to prevent JPEG from hangging too long
		} else {
			fly_len = (fs_cmd_a.buf_addr+C_MJPEG_READ_FILE_BUFFER_SIZE) - ((INT32U) p_vlc);

			jpeg_decode_vlc_maximum_length_set(img_decode_struct->source_size - header_len - ((INT32U) p_vlc - (INT32U) fs_cmd_a.buf_addr));	// Set maximum VLC length to prevent JPEG from hangging too long
		}
	}

 	// Now start JPEG decoding on the fly
	if (mjpeg_decode_vlc_on_the_fly(p_vlc, fly_len)) {
		img_decode_struct->decode_status = STATUS_FAIL;
		gp_free((void *) fs_cmd_b.buf_addr);
		gp_free((void *) fs_cmd_a.buf_addr);

		return -1;
	}

 	while (1) {
  		mjpeg_status = mjpeg_decode_status_query(1);
		if (mjpeg_status & C_JPG_STATUS_SCALER_DONE) {
			break;
		}		// if (mjpeg_status & C_JPG_STATUS_DECODE_DONE)
 		if (mjpeg_status & C_JPG_STATUS_INPUT_EMPTY) {
			if (fifo_a) {
				msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_b, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
	      		fifo_a ^= 0x1;
	      		len = (INT32S) OSQPend(image_task_fs_queue_a, 200, &err);
	      		if ((err != OS_NO_ERR) || (len <= 0)) {
	        		break;
	      		}
	 	  		// Now restart JPEG decoding on the fly
	  			if (mjpeg_decode_vlc_on_the_fly((INT8U *) fs_cmd_a.buf_addr, C_MJPEG_READ_FILE_BUFFER_SIZE)) {
	  				break;
	  			}
			} else {
	   			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &fs_cmd_a, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
	      		fifo_a ^= 0x1;
	      		len = (INT32S) OSQPend(image_task_fs_queue_b, 200, &err);
	      		if ((err != OS_NO_ERR) || (len <= 0)) {
					break;
	      		}
	 	  		// Now restart JPEG decoding on the fly
	  			if (mjpeg_decode_vlc_on_the_fly((INT8U *) fs_cmd_b.buf_addr, C_MJPEG_READ_FILE_BUFFER_SIZE)) {
	  				break;
	  			}
			}
		}		// if (mjpeg_status & C_JPG_STATUS_INPUT_EMPTY)

		if (mjpeg_status & (C_JPG_STATUS_STOP|C_JPG_STATUS_TIMEOUT|C_JPG_STATUS_INIT_ERR)) {
			break;
		}
	}
	mjpeg_decode_stop();
	gp_free((void *) fs_cmd_b.buf_addr);
	gp_free((void *) fs_cmd_a.buf_addr);

	if (mjpeg_status & C_JPG_STATUS_SCALER_DONE) {
		img_decode_struct->decode_status = STATUS_OK;

		return 0;
	} else {
		img_decode_struct->decode_status = STATUS_FAIL;

		return -1;
	}
}

INT32S mov_mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser)
{
	INT32S result;

	result = mov_parsing((INT16S) parser->image_source, (INT16S) parser->image_source);
	if (result != 0) {
		parser->parse_status = STATUS_FAIL;
		tk_mov_release_memory();
		return STATUS_FAIL;
	}

	parser->width = (INT32U) tk_mov_get_width();
	parser->height = (INT32U) tk_mov_get_height();
	parser->parse_status = STATUS_OK;
	tk_mov_release_memory();
	return 0;
}

// Decode and scale a frame from AVI file
INT32S mov_mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct)
{
	INT32S result;

	result = mov_parsing((INT16S) img_decode_struct->image_source, (INT16S) img_decode_struct->image_source);
	if (result != 0) {
		tk_mov_release_memory();
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}
	tk_mov_find_vdpos(1);				// Get first frame
	avi_mov_switchflag = C_AVI_MOV_DEFAULT;

	gplib_jpeg_default_huffman_table_load();
	result = mov_mjpeg_file_decode_and_scale(img_decode_struct);

	tk_mov_release_memory();
	return result;
}

#ifndef _DPF_PROJECT
// Decode and scale a frame from AVI file
INT32S mov_mjpeg_file_decode_nth_frame(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U nth)
{
	INT32S result;

	result = mov_parsing((INT16S) img_decode_struct->image_source, (INT16S) img_decode_struct->image_source);
	if (result != 0)
	{
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}

	result = tk_mov_find_vdpos(nth);				// Get first frame
	if(result < 0)
	{
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}
	avi_mov_switchflag = C_AVI_MOV_DEFAULT;

	gplib_jpeg_default_huffman_table_load();
	result = mov_mjpeg_file_decode_and_scale(img_decode_struct);

	tk_mov_release_memory();
	return result;
}
#endif
INT32S tk_mov_read_audio_data(CHAR *buf, INT32U len)
{
	INT32U flag = 2;
	INT32S BSlen = 0;
	INT32S ret;

	lseek(MOV->fin2,MOV->audio_pos,SEEK_SET);
	while(flag)
	{
		if((BSlen = read(MOV->fin2,(INT32U)buf,len))!=len)
		{
			ret = -1;
			flag --;
			lseek(MOV->fin2,MOV->audio_pos,SEEK_SET);
		}
		else
		{
		 	ret = BSlen;
		 	flag = 0;
		}
	}

	return ret;
}
/*== mov control api end==*/