//* Version : 1.00
//* History ;   1. 2008/4/17 shin modify Audio data alignment
//							2. 2008/5/8  shin modified
//               a. Add avi key word check
//               b. modify vedio data offset check

/*Include files */
#include "ap_motion_jpeg.h"
#include "avi_kernel.h"

/* define */
#define AVI_OK			 		0
#define AVI_ID_FIND_FAIL 		1
//Error Code//
#define AVI_ERR_MASK		 	0x8000		/*used for judge the err bit.  */
#define AVI_ERR_READ         	0x8003     /* Error reading from AVI File */
#define AVI_ERR_NO_MEM       	0x8008     /* malloc failed */
#define AVI_ERR_NO_AVI       	0x8009     /* Not an AVI file */
#define AVI_ERR_NO_HDRL      	0x8010     /* AVI file has no ha no header list,
                                           corrupted ??? */
#define AVI_ERR_NO_MOVI      	0x8011     /* AVI file has no has no MOVI list,
                                           corrupted ??? */
#define AVI_ERR_NO_VIDS      	0x8012     /* AVI file contains no video data */
#define AVI_ERR_NO_IDX       	0x8013     /* The file has been opened with
                                           getIndex==0, but an operation has been
                                           performed that needs an index */
#define AVI_ERR_NOT_MJPG	 	0x8014
#define AVI_ERR_NOT_SUPPORT	 	0x9000     // some function not support now.

#define ERR_EXIT(x) \
{ \
	AVI->AVI_errno = x; \
	return AVI->AVI_errno; \
}

#define PAD_EVEN(x)		( ((x)+1) & ~1 )

#define C_MJPEG_READ_FILE_BUFFER_SIZE		4096

#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
INT8U 	avi_workbuffer[512];
INT32S 	fin_pos = 0;
INT32S	file_type_flag = 0;
#endif
/* Variables defined in this file */
static avi_t avi_Info;
#ifdef	_DPF_PROJECT
static avi_t *AVI;
#else
avi_t *AVI;
#endif
typedef struct
{
	INT32S frame_num;
	INT32S aud_frame_num;
	INT32S aud_data_size;
}AVI_IDX1PAGE_INFO;

typedef struct
{
	INT32S totalframe;
	INT32S 	total_idx1_page;
	AVI_IDX1PAGE_INFO *ptr;
}AVI_IDX1_INFO;

AVI_IDX1_INFO avi_idx1_info;
static CHAR ap_mjpeg_temp_buffer[256];

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

/* Functions defined in this file */
static INT32S avi_seek(INT32S fin, INT32S n);
static INT32S avi_read(INT32S fin,CHAR *buf, INT32U len);
static INT32S avi_parsing_hdrl(avi_t *AVI, INT32S header_offset, INT32S hdrl_len);

void avi_idx1_parse(void);
INT32U avi_parsing(INT16S fileno, INT16S fileno1);
INT16S tk_avi_audiofilehandle_get(void);
INT32S tk_avi_get_audio_data(INT32U framenum);
INT32S tk_avi_read_audio_data(CHAR *buf, INT32U len);
INT32U tk_avi_decode_file_name(CHAR* pstring);
INT32U tk_avi_decode_file_id(INT32U FileNum);
INT32U tk_avi_get_audio_samplerate(void);
INT32S tk_avi_get_width(void);
INT32S tk_avi_get_height(void);
INT32S tk_avi_find_vdpos(INT32U JumpFrameNum);
INT32U tk_avi_close_file_id(INT32U FileNum);
INT32S tk_avi_get_hassound(void);
INT32S tk_avi_get_scale(void);
INT32S tk_avi_get_rate(void);
INT8U* tk_avi_get_waveformatex(void);
INT16U tk_avi_wave_bitformat_get(void);
INT16U tk_avi_wave_channelnum_get(void);
INT16U tk_avi_wave_format_type_get(void);
FP64   tk_avi_get_fps(void);
//sunxw added for FF FB play
void   avi_idx1_tabel_free(void);
INT32S avi_settime_frame_pos_get(INT32S time);
void   avi_idx1_tabel_free(void);
INT32S avi_idx1_totalframe_get(void);
INT32S avi_mp3_settime_frame_pos_get(INT32S time);

void mjpeg_memory_allocate(INT32U fifo);
void mjpeg_scaler_set_parameters(INT32U fifo);

#ifndef	_DPF_PROJECT
INT32U tk_avi_decode_file_parsing(INT16S avi_movhandle, INT16S avi_audhandle);
INT32S mjpeg_file_decode_nth_frame(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U nth);
#endif

#ifndef	_DPF_PROJECT
INT32U tk_avi_decode_file_parsing(INT16S avi_movhandle, INT16S avi_audhandle)//Roy ADD
{
	MJPEG_HEADER_PARSE_STRUCT	header_ptr;
	header_ptr.vedhandle = avi_movhandle;
	header_ptr.audhandle = avi_audhandle;
	header_ptr.status = 0;
	msgQSend(image_msg_queue_id, MSG_MJPEG_PARSE_HEADER_REQ, &header_ptr, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
	return 0;

	//return avi_parsing(avi_movhandle,avi_audhandle);
}
#endif

INT32U tk_avi_decode_file_name(CHAR* pstring)
{
	//INT32S ret;
 	INT16S avi_movhandle;
 	INT16S avi_audhandle;
	MJPEG_HEADER_PARSE_STRUCT	header_ptr;

	avi_movhandle = open(pstring,O_RDONLY);
	if(avi_movhandle < 0)
		return -1;

   	avi_audhandle = open(pstring,O_RDONLY);
	if(avi_audhandle < 0)
		return -1;

	header_ptr.vedhandle = avi_movhandle;
	header_ptr.audhandle = avi_audhandle;
	header_ptr.status = 0;
	msgQSend(image_msg_queue_id, MSG_MJPEG_PARSE_HEADER_REQ, &header_ptr, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);

	return 0;
	/*
	ret = avi_parsing(avi_movhandle,avi_audhandle);
	if(ret ==0)
		return AVI_OK;
	else
		return -1;
	*/
}

INT32U tk_avi_decode_file_id(INT32U FileNum)
{
  #ifdef _DPF_PROJECT
	INT32S ret = 0;
	STORAGE_FINFO finfo;
 	INT16S avi_movhandle;
 	INT16S avi_audhandle;
	MJPEG_HEADER_PARSE_STRUCT header_ptr;

	#if FS_SCAN_STYLE == FS_SCAN_COMBINATION
 	ret = storage_fopen(curr_storage_id, MEDIA_PHOTO_AVI, FileNum, &finfo);
	#else
	ret = storage_fopen(curr_storage_id, MEDIA_VIDEO_ONLY, FileNum, &finfo);
	#endif
    if(ret == STATUS_OK)
    	avi_movhandle =  finfo.fd;

	if(gp_strcmp((INT8S*)finfo.f_extname , (INT8S*)"AVI")!=0)
	//if((finfo.f_extname[0]!='A')||(finfo.f_extname[1]!='V')&&(finfo.f_extname[2]!='I'))
	{
		#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
		if(gp_strcmp((INT8S*)finfo.f_extname , "GV")!=0)
		//if((finfo.f_extname[0]=='G')&&(finfo.f_extname[1]=='V'))
			file_type_flag = 1;
		else
		#endif
		{
			close(avi_movhandle);
			return 1;
		}
	}
	#if FS_SCAN_STYLE == FS_SCAN_COMBINATION
 	ret = storage_fopen(curr_storage_id, MEDIA_PHOTO_AVI, FileNum, &finfo);
	#else
	ret = storage_fopen(curr_storage_id, MEDIA_VIDEO_ONLY, FileNum, &finfo);
	#endif
    if(ret == STATUS_OK)
    	avi_audhandle =  finfo.fd;


	header_ptr.vedhandle = avi_movhandle;
	header_ptr.audhandle = avi_audhandle;
	header_ptr.status = 0;
	msgQSend(image_msg_queue_id, MSG_MJPEG_PARSE_HEADER_REQ, &header_ptr, sizeof(MJPEG_HEADER_PARSE_STRUCT), MSG_PRI_NORMAL);
/*
	ret = avi_parsing(avi_movhandle,avi_audhandle);
	if(ret ==0)
		return AVI_OK;
	else
		return -1;
*/
	return ret;
  #else
	return 0;
  #endif
}

INT32U tk_avi_close_file_id(INT32U FileNum)
{
	#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
	file_type_flag = 0;
	#endif
	close((INT16S)AVI->fdes);
	AVI->fdes = -1;
	close((INT16S)AVI->fdes2);
	AVI->fdes2 = -1;
	return 0;
}

INT16S tk_avi_audiofilehandle_get(void)
{
	return (INT16S)AVI->fdes2;
}

INT32U tk_avi_get_audio_samplerate(void)
{
	return AVI->wave_format_ex.n_samples_per_sec;
}

FP64 tk_avi_get_fps(void)
{
	return AVI->fps;
}

INT32S tk_avi_get_hassound(void)
{
	if(AVI->anum)
		return 1;
	else
		return 0;
}

INT32S tk_avi_get_scale(void)
{
	return AVI->scale;
}

INT32S tk_avi_get_rate(void)
{
	return AVI->rate;
}

INT32S tk_avi_get_width(void)
{
	return AVI->bitmap_info_header.bi_width;
}

INT32S tk_avi_get_height(void)
{
	return AVI->bitmap_info_header.bi_height;
}

INT8U* tk_avi_get_waveformatex(void)		//this function is wav decode lib use only.
{
	return (INT8U*)&AVI->wave_format_ex;
}

INT16U tk_avi_wave_bitformat_get(void)
{
	return AVI->wave_format_ex.w_bits_per_sample;
}

INT16U tk_avi_wave_channelnum_get(void)
{
	return AVI->wave_format_ex.n_channels;
}

INT16U tk_avi_wave_format_type_get(void)
{
	return AVI->wave_format_ex.w_format_tag;
}
INT32S tk_avi_find_vdpos(INT32U JumpFrameNum)
{
	INT32S pos;
	CHAR data[8];
	INT32S tag,len;
	INT32S num = 0;

	if (JumpFrameNum ==0) return -1;
    JumpFrameNum--;
	pos = AVI->video_pos;
	while(1)
	{
		lseek(AVI->fdes,pos,SEEK_SET);
		if(avi_read(AVI->fdes,data,8)!=8)
		{
			AVI->AVI_errno = AVI_ERR_READ;
			return -1;
		}
		tag = *(INT32S *)(data);
		len=*(INT32S *)(data+4);
		if(tag == AVI->video_tag)   //"xxdcb"
		{
			//check length validity // sunxw added 09/02/23
			if(len >= AVI->fsize)
				return -1;

			if(JumpFrameNum==0)
			{
				lseek(AVI->fdes, pos+8,SEEK_SET);  //2008.12.28
				AVI->video_pos = pos+len+8;
				return AVI->video_pos;
			}
			else{
				JumpFrameNum--;
				pos += len+8;
			}
		}
		else
		{
			if(((tag & AVI_TAG_MASK) == AVI_ASCII_00DC)  || ((tag & AVI_TAG_MASK) == AVI_ASCII_00WB) || tag == AVI_ASCII_IDX1)
			{
				//check length validity // sunxw added 09/02/23
				if(len >= AVI->fsize)
					return -1;
				pos += len+8;
			}
			else
			{
				pos += 1;  //to read every byte;
				num++;
				if(num>90)
					return -1;
			}
			//check length validity // sunxw added 09/02/23
			if(pos >= AVI->fsize)
				return -1;
		}
		//DBG_PRINT ("P 0x%x\n\r", pos);
	}
}

INT32S tk_avi_get_audio_data(INT32U framenum)
{
	INT32S pos, pos1;
	CHAR   data[8];
	INT32S tag;
 	INT32U len = 0;
	INT32S num = 0;

	pos = pos1 = AVI->audio_pos;
	while(framenum)
	{
		lseek(AVI->fdes2,pos,SEEK_SET);
		if(avi_read(AVI->fdes2,data,8)!=8)
		{
			AVI->AVI_errno = AVI_ERR_READ;
			return -1;
		}
		tag = *(INT32S*)(data);
		len=*(INT32U*)(data+4);
		if(tag == AVI->audio_tag)   //"xxwb"
		{
			//check length validity // sunxw added 09/02/23
			if(len >= AVI->fsize)
				return -1;

			framenum--;
			AVI->audio_pos = pos1 + len + 8;
		}
		else
		{
			if(((tag & AVI_TAG_MASK) == AVI_ASCII_00DC)  || ((tag & AVI_TAG_MASK) == AVI_ASCII_00WB) || tag == AVI_ASCII_IDX1)
			{
				//check length validity // sunxw added 09/02/23
				if(len >= AVI->fsize)
					return -1;

				pos += len+8;
				pos1 = pos;
			}
			else
			{
				num++;
				pos += 1;  //to read every byte;
				pos1 = pos;
				if(num>90)
					return -1;
			}
			//check length validity // sunxw added 09/02/23
			if(pos >= AVI->fsize)
				return -1;
		}
	}
	return len;
}

INT32S tk_avi_read_audio_data(CHAR *buf, INT32U len)
{
	INT32S pos;
	INT32U flag = 2;
	INT32S BSlen = 0;
	INT32S ret;

	pos = lseek(AVI->fdes2,0,SEEK_CUR);
	while(flag)
	{
		if((BSlen = avi_read(AVI->fdes2,buf,len))!=len)
		{
			AVI->AVI_errno = AVI_ERR_READ;
			ret = -1;
			flag --;
			lseek(AVI->fdes2,pos,SEEK_SET);
		}
		else
		{
		 	ret = BSlen;
		 	flag = 0;
		}
	}

	return ret;
}

#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
void avi_date_decode(INT8U * ptr)
{
	INT32S count;
	INT8U tmp[12];
	if(file_type_flag == 1)
	{
		for (count=0;count<512;count++) {
	        avi_workbuffer[count] ^= 0xFF;
	    }
    }
    else
    {
		for (count=0; count<12; count++) {
			tmp[count] = avi_workbuffer[count] ^ 0xFF;
		}
		if (tmp[0]=='R' && tmp[1]=='I' && tmp[2]=='F' && tmp[3]=='F')
		{
			file_type_flag = 1;
			for (count=0;count<512;count++) {
		        avi_workbuffer[count] ^= 0xFF;
		    }
		}
    }
}
#endif

static INT32S avi_read(INT32S fin, CHAR *buf, INT32U len)
{
	INT32S n;
	#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
	INT32U templen;
	fin_pos = lseek(fin,0,SEEK_CUR);
	if(fin_pos >= 512)
	{
		// out 512 range
		n=read(fin,(INT32U)buf,len);
	}
	else
	{
		// in 512 range
		if((fin_pos+len)<= 512)
		{
			gp_memcpy((INT8S *)buf, (INT8S *)(avi_workbuffer + fin_pos),len);
			fin_pos += len;
			lseek(fin,fin_pos,SEEK_SET);
			n = len;
		}
		else
		{
			templen = 512 - fin_pos;
			gp_memcpy((INT8S *)buf, (INT8S *)(avi_workbuffer + fin_pos),templen);
			fin_pos += templen;
			lseek(fin,fin_pos,SEEK_SET);

			read(fin,(INT32U)(buf + templen),(len - templen));
			n = len;
		}
	}
	#else
		n=read(fin,(INT32U)buf,len);
	#endif

	return n;
}

static INT32S avi_seek(INT32S fin, INT32S n)
{
	INT32S k;

	k=lseek(fin, n, SEEK_CUR);
	return k;
}

INT32U avi_parsing(INT16S fileno, INT16S fileno1)
{
	INT32S oldpos,newpos;
	INT32S n;
	INT32S fin;
	INT32S header_offset,hdrl_len;
	INT32S err=0;
	INT32S flag;
	INT32S pos;
	CHAR   data1[8];
	INT32S tag,len;
	INT32S extract_flag;

	#if (defined GP_FILE_FORMAT_SUPPORT) && (GP_FILE_FORMAT_SUPPORT == GP_FILE_FORMAT_SET_1)
	fin_pos = 0;
	lseek(fileno,0,SEEK_SET);
	read(fileno, (INT32U)avi_workbuffer, 512);
	avi_date_decode(avi_workbuffer);
	#endif

	avi_idx1_info.total_idx1_page = 0;
	avi_idx1_info.ptr = NULL;
	oldpos = -1;
	newpos = -1;
	header_offset=0;
	hdrl_len=0;
	flag=0x8000;
	extract_flag=0;

	gp_memset((INT8S *)&avi_Info,0,sizeof(avi_t));
	AVI = &avi_Info;
	fin = AVI->fdes = fileno;
	AVI->fdes2 = fileno1;
	AVI->auds_strh_seen = AVI->vids_strf_seen = AVI->vids_strh_seen=0;

	lseek(fin,0,SEEK_SET);
	AVI->fsize = lseek(fin,0,SEEK_END);
	lseek(fin,0,SEEK_SET);
	/* Read first 12 bytes and check that this is an AVI file */
	if( avi_read(fin, ap_mjpeg_temp_buffer, 12) != 12 ) ERR_EXIT(AVI_ERR_READ)
	if( gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *)"RIFF", 4) !=0 || gp_strncmp((INT8S *)ap_mjpeg_temp_buffer+8,(INT8S *)"AVI ",4) !=0 )
		ERR_EXIT(AVI_ERR_NO_AVI)


	/* Go through the AVI file and extract the header list,
	the start position of the 'movi' list and an optionally
	present idx1 tag */
	while(1)
	{
		if( avi_read(fin, ap_mjpeg_temp_buffer, 8) != 8 ) break; /* We assume it's EOF */
		newpos = avi_seek(fin,0);
		if(oldpos==newpos) {	/* This is a broken AVI stream... */
			ERR_EXIT(AVI_ERR_READ)
			//return -1;
		}
		oldpos=newpos;

		n = PAD_EVEN(*((INT32S *) ap_mjpeg_temp_buffer+1));
		if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *)"LIST", 4) == 0)
		{
			if( avi_read(fin, ap_mjpeg_temp_buffer, 4) != 4 ) ERR_EXIT(AVI_ERR_READ)
			n -= 4;
			if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *)"hdrl", 4) == 0)
			{
				hdrl_len = n;
				// offset of header
				header_offset = avi_seek(fin,0);
				//if(avi_read(fin,(char *)hdrl_data,n) != n ) ERR_EXIT(AVI_ERR_READ)
				if (avi_seek(fin,n)==-1) ERR_EXIT(AVI_ERR_READ)//break;
			}
			else if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "movi", 4) == 0)
			{
				AVI->movi_start = avi_seek(fin,0);
				extract_flag |= 0x1;
				if (avi_seek(fin,n)==-1) ERR_EXIT(AVI_ERR_READ)//break;
			}
			else
				if (avi_seek(fin,n)==-1) ERR_EXIT(AVI_ERR_READ) // break;
		}
		//sunxw added for find idx1
		else if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *)"idx1", 4) == 0)
		{
			AVI->idx1_start = avi_seek(fin,0);
			AVI->idx1_length = n;
			extract_flag |= 0x2;
			avi_seek(fin,n);
		}
		else
		{
			avi_seek(fin,n);
		}
		if(extract_flag > 2)
			break;
	}

	//=============== parsing the hdrl ==============//
	if(!header_offset) ERR_EXIT(AVI_ERR_NO_HDRL)
	if(!AVI->movi_start) ERR_EXIT(AVI_ERR_NO_MOVI)
	err = avi_parsing_hdrl(AVI,header_offset,hdrl_len);
	if(err & AVI_ERR_MASK)  //if(err & 0x8000)
		return err;

   	if(!AVI->vids_strh_seen || !AVI->vids_strf_seen) ERR_EXIT(AVI_ERR_NO_VIDS)

	/* Reposition the file */
	lseek(AVI->fdes,AVI->movi_start,SEEK_SET);
	AVI->video_pos = 0;

    //==========================================
	if(gp_strncmp((INT8S*)AVI->compressor2,(INT8S*)"MJPG",4))
		ERR_EXIT(AVI_ERR_NOT_MJPG);

   	pos = AVI->video_pos = AVI->audio_pos=AVI->movi_start;
   	lseek(AVI->fdes,pos,SEEK_SET);
   	if(!AVI->anum)
		flag |= 1;

	while(flag)
	{
		if(avi_read(AVI->fdes, data1, 8)!=8)
		{
			//AVI->AVI_errno = AVI_ERR_READ;
			//return -1;
			if ((flag & 0x2)==2 && (flag & 0x1)==0)    //wenli 2008.4.9;  Donot find the audio data, but find the video data.
			{
				AVI->anum =0;
				return err;
			}
			else {
				AVI->AVI_errno = AVI_ERR_READ;
				return -1;
			}
		}
		tag = *(INT32S *)(data1);
		len=*(INT32S *)(data1+4);
		if((tag & AVI_TAG_MASK) == AVI_ASCII_00WB && (flag & 0x1)!=1)   //"xxwb"
		{
		   flag |=1;
		   AVI->audio_tag = tag;
		   AVI->audio_pos = pos;
		   pos=lseek(AVI->fdes,len,SEEK_CUR);
		}
		else if((tag & AVI_TAG_MASK) == AVI_ASCII_00DC && (flag & 0x2)!=2)   //"xxdc"
		{
		   flag |= 2;
		   AVI->video_tag = tag;
		   AVI->video_pos = pos;
		   pos=lseek(AVI->fdes,len,SEEK_CUR);
	    }
	    //sunxw added
	    else if((tag & AVI_TAG_MASK) == AVI_ASCII_00DB)   //"xxdc"
	    {
			return -1;
	    }
		else if((tag & AVI_TAG_MASK)== AVI_ASCII_00DC || (tag & AVI_TAG_MASK)==AVI_ASCII_00WB
				|| (tag & AVI_IXTAG_MASK)  == AVI_ASCII_IX00   //wenli 2008.4.9
			    || tag == AVI_ASCII_JUNK    //wenli 2008.4.9
		)
		{
		   pos=lseek(AVI->fdes,len,SEEK_CUR);
		}
		else
		{
		   pos += 1;  //to read every byte;
		   lseek(AVI->fdes,pos,SEEK_SET);
		}
		if((flag&0x3)==3 )
			flag = 0;
	}
	#ifndef _DPF_PROJECT
	avi_idx1_parse();
	#endif
	return err;
}

static INT32S avi_parsing_hdrl(avi_t *AVI, INT32S header_offset, INT32S hdrl_len)
{
	INT32S fin;
	INT32S num_stream;
	INT32S i,n,lasttag;
	INT32S wavlen;
	alBITMAPINFOHEADER *bih;
	AVI_WAVEFORMATEX *wfe;

	num_stream = 0;
	fin = AVI->fdes;

	lseek(fin,header_offset,SEEK_SET);
	/* Interpret the header list */
	for(i=0;i<hdrl_len;)
	{
		/* List tags are completly ignored */
		if(avi_read(fin, ap_mjpeg_temp_buffer, 8)!=8) ERR_EXIT(AVI_ERR_READ)
		if(gp_strncmp((INT8S*) ap_mjpeg_temp_buffer, (INT8S *) "LIST", 4)==0) { i+= 12; avi_seek(fin,4); continue; }
		n = PAD_EVEN(*(INT32S*)(ap_mjpeg_temp_buffer+4));

		/* Interpret the tag and its args */
		//---- strh --------------------------------//
		if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "strh", 4)==0)
		{
			i += 8;

			if(avi_read(fin, ap_mjpeg_temp_buffer, 8)!=8) ERR_EXIT(AVI_ERR_READ)
			if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *)"vids", 4) == 0 && !AVI->vids_strh_seen)
			{
				//memcpy(AVI->compressor, ap_mjpeg_temp_buffer+4, 4);
				//AVI->compressor[4] = 0;

				avi_seek(fin,12);
				//if(avi_read(fin, ap_mjpeg_temp_buffer, 16)!=16) ERR_EXIT(AVI_ERR_READ)
				if(avi_read(fin, ap_mjpeg_temp_buffer, 8)!=8) ERR_EXIT(AVI_ERR_READ)  //2007.12.4
				AVI->scale = *(INT32S*)(ap_mjpeg_temp_buffer);
				AVI->rate  = *(INT32S*)(ap_mjpeg_temp_buffer+4);
				if(AVI->scale!=0) AVI->fps = (double)AVI->rate/(double)AVI->scale;
				//sunxw added for get total time
				if(avi_read(fin, ap_mjpeg_temp_buffer, 8)!=8) ERR_EXIT(AVI_ERR_READ)  //2008.12.2
				AVI->length = *(INT32S*)(ap_mjpeg_temp_buffer+4);
				AVI->length = AVI->length/(INT32S)AVI->fps;
				AVI->vids_strh_seen = 1;
				lasttag = 1; /* vids */
			}
			else if (gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "auds", 4) ==0 && ! AVI->auds_strh_seen)
			{	//inc audio tracks
				++AVI->anum;

				//avi_seek(fin,16);
				//if(avi_read(fin, ap_mjpeg_temp_buffer, 24)!=24) ERR_EXIT(AVI_ERR_READ)

				lasttag = 2; /* auds */
			}
			else if (gp_strncmp ((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "iavs", 4) ==0 && ! AVI->auds_strh_seen) {
				//fprintf(stderr, "AVILIB: error - DV AVI Type 1 no supported\n");
				//return (-1);
				ERR_EXIT(AVI_ERR_NOT_SUPPORT)  //2007.12.4
			}
			else
				lasttag = 0;
				num_stream++;
		}
		//---- dmlh --------------------------------//
		/*else if(gp_strncmp(ap_mjpeg_temp_buffer, "dmlh", 4) == 0)
		{
			//if(avi_read(fin, ap_mjpeg_temp_buffer, 4)!=4) ERR_EXIT(AVI_ERR_READ)
			//AVI->total_frames = *(INT32S *)(ap_mjpeg_temp_buffer);
			i += 8;
		}*/
		//---- strf --------------------------------//
		else if(gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "strf", 4)==0)
		{
			INT32S temp;
			i += 8;
 			temp = avi_seek(fin,0);

			if(lasttag == 1)
			{  //ww 2007.12.4
				bih = &AVI->bitmap_info_header;
				avi_read(fin,(CHAR*)bih,sizeof(alBITMAPINFOHEADER));
				if(bih->bi_size != 0x28)
				{	// how to do?????
					ERR_EXIT(AVI_ERR_NOT_SUPPORT);
				}
				AVI->vids_strf_seen = 1;
				//AVI->v_codecf_off = header_offset + i+16;
				gp_memcpy((INT8S *)AVI->compressor2,(INT8S *) &bih->bi_compression, 4);
				AVI->compressor2[4] = 0;
			}
			else if(lasttag == 2)
			{
				wavlen=n;			//0430 added
				//ww 2007.12.4
 				if(n>AVI_WAVE_HEADER_SIZE)
					wavlen = AVI_WAVE_HEADER_SIZE;
				wfe = &AVI->wave_format_ex;
				if(avi_read(fin,(CHAR*) wfe,wavlen) != wavlen) ERR_EXIT(AVI_ERR_READ);  //2007.12.5
				lseek(fin,n-wavlen,SEEK_CUR);	//0430 added
			}
		}
		//---- JUNK --------------------------------//
      else if((gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "JUNK", 4) == 0) ||
		  (gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "strn", 4) == 0) ||
		  (gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "dmlh", 4) == 0) ||   //2007.12.4
		  (gp_strncmp((INT8S *) ap_mjpeg_temp_buffer, (INT8S *) "vprp", 4) == 0)){
		  i += 8;
		  // do not reset lasttag
      } else
      {
		  i += 8;
		  lasttag = 0;
      }

      i += n;
	  lseek(fin,header_offset+i,SEEK_SET);
	}

	#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
	if(AVI->wave_format_ex.w_format_tag == 0x0055)	//check it mp3 flag.
		ERR_EXIT(AVI_ERR_NOT_SUPPORT)
	#endif

	return 0;
}

INT32S mjpeg_file_parse_header(IMAGE_HEADER_PARSE_STRUCT *parser)
{
	INT32S result;

	result = avi_parsing((INT16S) parser->image_source, (INT16S) parser->image_source);
	if (result != 0) {
		parser->parse_status = STATUS_FAIL;

		return STATUS_FAIL;
	}

	result = tk_avi_find_vdpos(1);				// Parse first frame header
	if (result < 0) {
		parser->parse_status = STATUS_FAIL;

		return STATUS_FAIL;
	}

	parser->width = (INT32U) tk_avi_get_width();
	parser->height = (INT32U) tk_avi_get_height();
	parser->parse_status = STATUS_OK;

	return 0;
}

// Decode and scale a frame from AVI file
INT32S mjpeg_file_decode_one_frame(IMAGE_DECODE_STRUCT *img_decode_struct)
{
	INT32S result;

	result = avi_parsing((INT16S) img_decode_struct->image_source, (INT16S) img_decode_struct->image_source);
	if (result != 0) {
		avi_idx1_tabel_free();
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}
	tk_avi_find_vdpos(1);				// Get first frame

	gplib_jpeg_default_huffman_table_load();
	result = mjpeg_file_decode_and_scale(img_decode_struct);

	avi_idx1_tabel_free();
	return result;
}

#ifndef	_DPF_PROJECT
// Decode and scale a frame from AVI file
INT32S mjpeg_file_decode_nth_frame(IMAGE_DECODE_STRUCT *img_decode_struct, INT32U nth)
{
	INT32S result;

	result = avi_parsing((INT16S) img_decode_struct->image_source, (INT16S) img_decode_struct->image_source);
	if (result != 0)
	{
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}

	result = tk_avi_find_vdpos(nth);				// Get first frame
	if(result < 0)
	{
		img_decode_struct->decode_status = STATUS_FAIL;
		return STATUS_FAIL;
	}

	gplib_jpeg_default_huffman_table_load();
	result = mjpeg_file_decode_and_scale(img_decode_struct);

	return result;
}
#endif

// Shortcut for Motion-JPEG: decode without scaling
INT32S mjpeg_file_decode_without_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
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
	fs_cmd_a.fd = AVI->fdes;
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
	fs_cmd_b.fd = AVI->fdes;
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

void mjpeg_memory_allocate(INT32U fifo)
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

void mjpeg_scaler_set_parameters(INT32U fifo)
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

INT32S mjpeg_file_decode_and_scale(IMAGE_DECODE_STRUCT *img_decode_struct)
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
	fs_cmd_a.fd = (INT32S) AVI->fdes;
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
	fs_cmd_b.fd = (INT32S) AVI->fdes;
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

	mjpeg_memory_allocate(fifo_line_num);
	if (!jpeg_out_y) {
		DBG_PRINT("Failed to allocate memory in mjpeg_memory_allocate()\r\n");
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
	mjpeg_scaler_set_parameters(scaler_fifo_register);
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

void avi_idx1_tabel_free(void)
{
	if(avi_idx1_info.ptr!=NULL)
		gp_free((void*)avi_idx1_info.ptr);
	avi_idx1_info.ptr = NULL;
}

INT32S avi_idx1_totalframe_get(void)
{
	return avi_idx1_info.totalframe;
}

extern INT32S g_avi_audio_datalength;
INT32S avi_mp3_settime_frame_pos_get(INT32S time)
{
	//AVI->fdes		means video image file handle.
	//AVI->fdes2	means audio data	file handle.
	//use the AVI->fdes2 to find the idx1.
	//alloate max 4K buffer to find the frame position.
	//first find the video frame, next find the audio frame.
	INT32S	search_frame_num;
	INT32S	search_audiodata_size;
	INT32S  i,j,k;
	INT32S  time1;
	INT32S	ret = -1;
	INT32S	search_flag;
	INT32S	video_temp_save_pos;
	INT32S	audio_temp_save_pos;
	INT32S	video_temp_pos = 0;
	INT32S	audio_temp_pos = 0;
	INT32S	filepos;
	AVI_IDX1PAGE_INFO	*idx1_ptr;
	INT8U	*buf;
	INT8U	err;
	TK_FILE_SERVICE_STRUCT avi_idx1;

	//

	DBG_PRINT("JT=%d \r\n", time);
	//first: allocate memory buf for search frame.
	buf = (INT8U *)gp_malloc_align(4096,16);
	if(buf == NULL)
		return ret;
	//second: save video and audio file position
	video_temp_save_pos = lseek(AVI->fdes,0,SEEK_CUR);
	audio_temp_save_pos = lseek(AVI->fdes2,0,SEEK_CUR);
	//third: time transform video frame count.
	//		 time transform audio date size.

	time1 = time >>1;
	if(time & 0x1)
		time -= 1;
	if(AVI->anum)
	{
		search_audiodata_size = time1*77;	//71 frame mp3 = 2s

	}
	search_frame_num = (INT32S)(AVI->fps*time);

	ret = (INT32S)(AVI->fps*time1);
	ret = ret/500;
	search_frame_num = ret+search_frame_num;				//compensate 0.002 error.
	ret = search_frame_num;

	search_flag = 0;
	k = -1;
	for(i=0;i<avi_idx1_info.total_idx1_page;i++)
	{
		idx1_ptr = (AVI_IDX1PAGE_INFO*)(avi_idx1_info.ptr + i);
		if(AVI->anum)
		{
			if(search_audiodata_size > idx1_ptr->aud_frame_num)
			{
				search_audiodata_size = search_audiodata_size - idx1_ptr->aud_frame_num;
			}
			else
			{
				if(!(search_flag & 0x2))
				{
					if(i != k)
					{
						lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
						avi_idx1.fd = AVI->fdes2;
						avi_idx1.buf_addr = (INT32S)buf;
						avi_idx1.buf_size = 4096;
						avi_idx1.result_queue = image_task_fs_queue_a;

						msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
						filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
					   	if ((err!=OS_NO_ERR) || (filepos<=0))
							//while(1);
						k = i;
					}
					for(j=0;j<filepos;j=j+4)
					{
						if(*((INT32S *) buf+j) == AVI->audio_tag)
						{
							if(search_audiodata_size > 0)
							{
								search_audiodata_size--;
							}
							if(search_audiodata_size == 0)
							{
								audio_temp_pos = *((INT32S *) buf+j+2);
								AVI->audio_pos = AVI->movi_start + audio_temp_pos + 4;//offset 4 byte "movi" and offset "00dc" and offset length.
								break;
							}
						}
					}
					search_flag |= 0x2;
					if(search_flag == 0x3)
						break;
				}
			}
		}
		if(search_frame_num > idx1_ptr->frame_num)
		{
			//find the search frame in which idx1 bank.
			search_frame_num = search_frame_num - idx1_ptr->frame_num;
		}
		else
		{
			//find the idx1 bank.
			if(!(search_flag & 0x1))
			{
				if(i != k)
				{
					lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
					avi_idx1.fd = AVI->fdes2;
					avi_idx1.buf_addr = (INT32S)buf;
					avi_idx1.buf_size = 4096;
					avi_idx1.result_queue = image_task_fs_queue_a;

					msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
					filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
				   	if ((err!=OS_NO_ERR) || (filepos<=0))
						//while(1);
					k = i;
				}
				for(j=0;j<filepos;j=j+4)
				{
					if(*((INT32S *) buf+j) == AVI->video_tag)
					{
						if(search_frame_num > 0)
						{
							search_frame_num--;
						}
						if(search_frame_num == 0)
						{
							video_temp_pos = *((INT32S *) buf+j+2);
							AVI->video_pos = AVI->movi_start + video_temp_pos - 4;//offset 4 byte "movi" and offset "00dc" and offset length.
							lseek(AVI->fdes,(AVI->video_pos + 8),SEEK_SET);
							break;
						}
					}
				}

				search_flag |= 0x1;
				if((search_flag == 0x3)||(AVI->anum == 0))
					break;
			}
		}
	}

	search_flag = 0;
	k = 0;
	if(search_frame_num!=0)
	{
		search_frame_num += ret;
		for(i=0;i<avi_idx1_info.total_idx1_page;i++)
		{
			idx1_ptr = (AVI_IDX1PAGE_INFO*)(avi_idx1_info.ptr + i);
			if(search_frame_num > idx1_ptr->frame_num)
			{
				//find the search frame in which idx1 bank.
				search_frame_num = search_frame_num - idx1_ptr->frame_num;
			}
			else
			{
				//find the idx1 bank.
				lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
				avi_idx1.fd = AVI->fdes2;
				avi_idx1.buf_addr = (INT32S)buf;
				avi_idx1.buf_size = 4096;
				avi_idx1.result_queue = image_task_fs_queue_a;

				msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
			   	if ((err!=OS_NO_ERR) || (filepos<=0))
					//while(1);

				for(j=0;j<filepos;j=j+4)
				{
					if(*((INT32S *) buf+j) == AVI->video_tag)
					{
						if(search_frame_num > 0)
						{
							search_frame_num--;
						}
						if(search_frame_num == 0)
						{
							video_temp_pos = *((INT32S *) buf+j+2);
							AVI->video_pos = AVI->movi_start + video_temp_pos - 4;//offset 4 byte "movi" and offset "00dc" and offset length.
							lseek(AVI->fdes,(AVI->video_pos + 8),SEEK_SET);
							break;
						}
					}
				}
			break;
			}
		}
	}

	if(AVI->anum)
	{
		if((video_temp_pos != 0)&&(audio_temp_pos != 0))
		{
			lseek(AVI->fdes2,(AVI->audio_pos),SEEK_SET);
			AVI->audio_pos -= 8;

			g_avi_synchroframe = ret;
			g_avi_datalength = 0;
		}
	}

	if((video_temp_pos == 0)||(audio_temp_pos == 0))
	{
		lseek(AVI->fdes,video_temp_save_pos,SEEK_SET);
		lseek(AVI->fdes2,audio_temp_save_pos,SEEK_SET);
		ret = -1;
	}
	gp_free((void*)buf);
	return ret;
}
INT32S avi_settime_frame_pos_get(INT32S time)
{
	//AVI->fdes		means video image file handle.
	//AVI->fdes2	means audio data	file handle.
	//use the AVI->fdes2 to find the idx1.
	//alloate max 4K buffer to find the frame position.
	//first find the video frame, next find the audio frame.
	INT32S	search_frame_num;
	INT32S	search_audiodata_size;
	FP64	temp = 0;
	INT32S	time1;
	INT32S  i,j,k;
	INT32S	ret = -1;
	INT32S	search_flag;
	INT32S	video_temp_save_pos;
	INT32S	audio_temp_save_pos;
	INT32S	video_temp_pos = 0;
	INT32S	audio_temp_pos = 0;
	INT32S	audio_data_size;
	INT32S	filepos;
	AVI_IDX1PAGE_INFO	*idx1_ptr;
	INT8U	*buf;
	INT8U	err;
	TK_FILE_SERVICE_STRUCT avi_idx1;
	//first: allocate memory buf for search frame.
	buf = (INT8U *)gp_malloc_align(4096,16);
	if(buf == NULL)
		return ret;
	//second: save video and audio file position
	video_temp_save_pos = lseek(AVI->fdes,0,SEEK_CUR);
	audio_temp_save_pos = lseek(AVI->fdes2,0,SEEK_CUR);
	time1 = time;
	//third: time transform video frame count.
	//		 time transform audio date size.
	ret = search_frame_num = (INT32S)(AVI->fps*time);

	if(AVI->anum)
	{
		if(AVI->wave_format_ex.w_bits_per_sample == 0x10)//16 bit
			time = time << 1;
		if(AVI->wave_format_ex.n_channels == 0x2)//stereo
			time = time << 1;
		search_audiodata_size = (INT32S)AVI->wave_format_ex.n_samples_per_sec*time;

		if(AVI->wave_format_ex.w_bits_per_sample == 0x4)//16 bit
			search_audiodata_size = search_audiodata_size >> 1;
	}
	search_flag = 0;
	k = -1;
	for(i=0;i<avi_idx1_info.total_idx1_page;i++)
	{
		idx1_ptr = (AVI_IDX1PAGE_INFO*)(avi_idx1_info.ptr + i);
		if(AVI->anum)
		{
			if(search_audiodata_size > idx1_ptr->aud_data_size)
			{
				search_audiodata_size = search_audiodata_size - idx1_ptr->aud_data_size;
			}
			else
			{
				if(!(search_flag & 0x2))
				{
					if(i != k)
					{
						lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
						avi_idx1.fd = AVI->fdes2;
						avi_idx1.buf_addr = (INT32S)buf;
						avi_idx1.buf_size = 4096;
						avi_idx1.result_queue = image_task_fs_queue_a;

						msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
						filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
					   	if ((err!=OS_NO_ERR) || (filepos<=0))
							//while(1);
						k = i;
					}
					for(j=0;j<filepos;j=j+4)
					{
						if(*((INT32S *) buf+j) == AVI->audio_tag)
						{
							audio_data_size = *((INT32S *) buf+j+3);

							if(search_audiodata_size >= audio_data_size)
							{
								search_audiodata_size = search_audiodata_size - audio_data_size;
							}
							else if((search_audiodata_size < audio_data_size)&&(search_audiodata_size > 0))
							{
								search_audiodata_size = audio_data_size - search_audiodata_size;
								//data rate;
								if(AVI->wave_format_ex.w_bits_per_sample == 0x10)//16 bit
									search_audiodata_size = search_audiodata_size >> 1;
								if(AVI->wave_format_ex.n_channels == 0x2)//stereo
									search_audiodata_size = search_audiodata_size >> 1;
								temp = AVI->fps*search_audiodata_size/AVI->wave_format_ex.n_samples_per_sec;
								//g_avi_synchrocnt/4000*search_audiodata_size;
								search_frame_num += (INT32S)temp;
								search_audiodata_size = 0;
							}
							else if(search_audiodata_size == 0)
							{
								audio_temp_pos = *((INT32S *) buf+j+2);
								AVI->audio_pos = AVI->movi_start + audio_temp_pos + 4;//offset 4 byte "movi" and offset "01wb" and offset length.
								break;
							}
						}
					}
					search_flag |= 0x2;
					if(search_flag == 0x3)
						break;
				}
			}
		}
		if(search_frame_num > idx1_ptr->frame_num)
		{
			//find the search frame in which idx1 bank.
			search_frame_num = search_frame_num - idx1_ptr->frame_num;
		}
		else
		{
			//find the idx1 bank.
			if(!(search_flag & 0x1))
			{
				if(i != k)
				{
					lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
					avi_idx1.fd = AVI->fdes2;
					avi_idx1.buf_addr = (INT32S)buf;
					avi_idx1.buf_size = 4096;
					avi_idx1.result_queue = image_task_fs_queue_a;

					msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
					filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
				   	if ((err!=OS_NO_ERR) || (filepos<=0))
						//while(1);
					k = i;
				}
				for(j=0;j<filepos;j=j+4)
				{
					if(*((INT32S *) buf+j) == AVI->video_tag)
					{
						if(search_frame_num > 0)
						{
							search_frame_num--;
						}
						if(search_frame_num == 0)
						{
							video_temp_pos = *((INT32S *) buf+j+2);
							AVI->video_pos = AVI->movi_start + video_temp_pos - 4;//offset 4 byte "movi" and offset "00dc" and offset length.
							lseek(AVI->fdes,(AVI->video_pos + 8),SEEK_SET);
							break;
						}
					}
				}

				search_flag |= 0x1;
				if((search_flag == 0x3)||(AVI->anum == 0))
					break;
			}
		}
	}

	search_flag = 0;
	k = 0;
	if(search_frame_num!=0)
	{
		search_frame_num += ret;
		for(i=0;i<avi_idx1_info.total_idx1_page;i++)
		{
			idx1_ptr = (AVI_IDX1PAGE_INFO*)(avi_idx1_info.ptr + i);
			if(search_frame_num > idx1_ptr->frame_num)
			{
				//find the search frame in which idx1 bank.
				search_frame_num = search_frame_num - idx1_ptr->frame_num;
			}
			else
			{
				//find the idx1 bank.
				lseek(AVI->fdes2,(AVI->idx1_start+i*4096),SEEK_SET);
				avi_idx1.fd = AVI->fdes2;
				avi_idx1.buf_addr = (INT32S)buf;
				avi_idx1.buf_size = 4096;
				avi_idx1.result_queue = image_task_fs_queue_a;

				msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
				filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
			   	if ((err!=OS_NO_ERR) || (filepos<=0))
					//while(1);

				for(j=0;j<filepos;j=j+4)
				{
					if(*((INT32S *) buf+j) == AVI->video_tag)
					{
						if(search_frame_num > 0)
						{
							search_frame_num--;
						}
						if(search_frame_num == 0)
						{
							video_temp_pos = *((INT32S *) buf+j+2);
							AVI->video_pos = AVI->movi_start + video_temp_pos - 4;//offset 4 byte "movi" and offset "00dc" and offset length.
							lseek(AVI->fdes,(AVI->video_pos + 8),SEEK_SET);
							break;
						}
					}
				}
			break;
			}
		}
	}

	if(AVI->anum)
	{
		if((video_temp_pos != 0)&&(audio_temp_pos != 0))
		{
			lseek(AVI->fdes2,(AVI->audio_pos),SEEK_SET);
			AVI->audio_pos -= 8;

			g_avi_synchroframe = (FP64)AVI->fps*time1 + temp;
			g_avi_datalength = 0;
		}
	}
	if((video_temp_pos == 0)||(audio_temp_pos == 0))
	{
		lseek(AVI->fdes,video_temp_save_pos,SEEK_SET);
		lseek(AVI->fdes2,audio_temp_save_pos,SEEK_SET);
		ret = -1;
	}
	gp_free((void*)buf);
	return ret;
}
//SEEK_SET	header
//SEEK_CUR	current
//SEEK_END	end
void avi_idx1_parse(void)
{
	INT32S pos;
	INT8U *buf;
	INT8U  err;
	INT32S idx1_page;
	INT32S Residue;
	INT32S buf_length;
	INT32S i,j;
	INT32S frame_num,aud_frame_num,audio_data_size;
	INT32S filepos;
	TK_FILE_SERVICE_STRUCT avi_idx1;
	AVI_IDX1PAGE_INFO* idx1_ptr;

	pos = lseek(AVI->fdes2,0,SEEK_CUR);
	if(AVI->idx1_start != 0)
	{
		lseek(AVI->fdes2,AVI->idx1_start,SEEK_SET);
		if(AVI->idx1_length > 4096)
		{
		  #if _DRV_L1_GTE == 1
			INT32S_DIVISION(AVI->idx1_length,4096,idx1_page,Residue);
		  #else
		  	idx1_page = (AVI->idx1_length) >> 12;
		  	Residue = (AVI->idx1_length) & (4096-1);
		  #endif
			if(Residue != 0)
			{
				buf_length = Residue;
				idx1_page++;
			}
			buf = (INT8U *)gp_malloc_align(4096,16);
		}
		else
		{
			buf = (INT8U *)gp_malloc_align(AVI->idx1_length,16);
			idx1_page = 1;
			buf_length = AVI->idx1_length;
		}
		avi_idx1_info.totalframe = 0;
		avi_idx1_info.total_idx1_page = idx1_page;
		avi_idx1_info.ptr = (AVI_IDX1PAGE_INFO*)gp_malloc(idx1_page*sizeof(AVI_IDX1PAGE_INFO));

		for(i=0; i<idx1_page; i++)
		{
			avi_idx1.fd = AVI->fdes2;
			avi_idx1.buf_addr = (INT32S)buf;
			if(i!=(idx1_page-1))
				avi_idx1.buf_size = 4096;
			else
				avi_idx1.buf_size = buf_length;
			avi_idx1.result_queue = image_task_fs_queue_a;

			msgQSend(fs_msg_q_id, MSG_FILESRV_FS_READ, (void *) &avi_idx1, sizeof(TK_FILE_SERVICE_STRUCT), MSG_PRI_NORMAL);
			filepos = (INT32S) OSQPend(image_task_fs_queue_a, 0, &err);
		   	if ((err!=OS_NO_ERR) || (filepos<=0))
			{
				gp_free((void *)buf);
				avi_idx1_info.totalframe = 0;
				avi_idx1_info.total_idx1_page = 0;
				return;
			}
			frame_num = 0;
			aud_frame_num = 0;
			audio_data_size = 0;
			//start to parse
			//*((INT32S *) buf) == ckid
			//*((INT32S *) buf + 1) == Flags
			//*((INT32S *) buf + 2) == ChunkOffset
			//*((INT32S *) buf + 3) == ChunkLength
			for(j=0;j<filepos;j=j+4)
			{
				if(*((INT32S *) buf+j) == AVI->video_tag)
				{
					frame_num++;
					avi_idx1_info.totalframe++;
				}
				else if(*((INT32S *) buf+j) == AVI->audio_tag)
				{
					aud_frame_num++;
					audio_data_size += *((INT32S *) buf+j+3);
				}
			}
			idx1_ptr = (AVI_IDX1PAGE_INFO*)(avi_idx1_info.ptr + i);
			idx1_ptr->frame_num = frame_num;
			idx1_ptr->aud_frame_num = aud_frame_num;
			idx1_ptr->aud_data_size = audio_data_size;
		}
		gp_free((void *)buf);
	}
}
