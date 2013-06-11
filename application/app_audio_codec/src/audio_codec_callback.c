#include "Audio_Codec_Callback.h"

void audio_decode_end(INT32U audio_decoder_num);

INT32U g_user_read_data_addr=0;
INT32U global_user_read_data_addr=0;//for speech repeat 090701
INT16S g_user_read_data_fd = -1;
INT8U g_user_read_data_mode = 0;
ALIGN8 INT8U G_user_read_buffer[512*2];//081115
// following are call out only when MEDIA_SOURCE.type = 0 (user-defined)
INT32S audio_encoded_data_read(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U data_offset);
INT16S G_Snd_UserFd;
INT32U audio_encode_data_write(INT8U bHeader, INT32U buffer_addr, INT32U cbLen);
INT32S (*audio_data_write_callback)(INT8U bHeader, INT32U buffer_addr, INT32U cbLen);

//=======================================================================
//  audio callback 
//=======================================================================
void audio_decode_end(INT32U audio_decoder_num)
{
	if(audio_decoder_num == 0)
		Audio_Decoder_Status_0 = AUDIO_CODEC_PROCESS_END;
	else if(audio_decoder_num == 1)
		Audio_Decoder_Status_1 = AUDIO_CODEC_PROCESS_END;
	else if(audio_decoder_num == 2)
		Audio_Decoder_Status_2 = AUDIO_CODEC_PROCESS_END;
	
	DBG_PRINT("Audio Decoder %d play-end callback...\r\n", audio_decoder_num);
}

void user_err(void)
{
	INT32U err_temp=1;
	while(err_temp)
		;
}

#if 0
INT32S audio_user_set_comb_startaddr_length(INT16S fd,INT32U audio_index,INT8U mode,INT32U audio_comb_start_addr)//081115
{
	INT8U *U8_ptr;
	INT32S ret,total_audio_index;
	INT8U char_temp3,char_temp2,char_temp1,char_temp0;

	g_user_read_data_fd = fd;
	g_user_read_data_mode = mode;
	g_user_read_data_addr = audio_comb_start_addr;
	ret = read_two_sector(g_user_read_data_fd, (g_user_read_data_addr&0xfffffe00), (INT32U)G_user_read_buffer, g_user_read_data_mode);
	if(ret<0)
	{
		user_err();
	}
	U8_ptr = (INT8U*)(G_user_read_buffer + ((INT32U)g_user_read_data_addr & 0x000001ff) );
	char_temp0 = *(U8_ptr++);
	char_temp1 = *(U8_ptr++);
	char_temp2 = *(U8_ptr++);
	char_temp3 = *(U8_ptr++);
	total_audio_index = (INT32S)((char_temp3<<24) | (char_temp2<<16) | (char_temp1<<8) | char_temp0);
	if((audio_index >= total_audio_index) || (total_audio_index <= 0))
	{
		user_err();
	}
	g_user_read_data_addr += 4;
	g_user_read_data_addr += audio_index*8;
	ret = (INT32S)read_two_sector(g_user_read_data_fd, (g_user_read_data_addr&0xfffffe00), (INT32U)G_user_read_buffer, g_user_read_data_mode);
	if(ret<0)
	{
		user_err();
	}
	U8_ptr = (INT8U*)(G_user_read_buffer + ((INT32U)g_user_read_data_addr & 0x000001ff) );
	char_temp0 = *(U8_ptr++);
	char_temp1 = *(U8_ptr++);
	char_temp2 = *(U8_ptr++);
	char_temp3 = *(U8_ptr++);
	g_user_read_data_addr = audio_comb_start_addr + (INT32S)((char_temp3<<24) | (char_temp2<<16) | (char_temp1<<8) | char_temp0);
	global_user_read_data_addr = g_user_read_data_addr;//090701
	char_temp0 = *(U8_ptr++);
	char_temp1 = *(U8_ptr++);
	char_temp2 = *(U8_ptr++);
	char_temp3 = *(U8_ptr++);
	g_audio_data_length = (INT32S)((char_temp3<<24) | (char_temp2<<16) | (char_temp1<<8) | char_temp0);
	return total_audio_index;
}
#endif

INT32S audio_encoded_data_read(INT32U buf_addr, INT32U buf_size, INT8U *data_start_addr, INT32U buf_offset)
{
#if 1
	INT8U* offset;

	offset = data_start_addr + buf_offset;
	gp_memcpy((INT8S*)buf_addr, (INT8S*)offset, buf_size);
	
	return buf_size;
#else//load comb data from nandflash or FS
	INT32S ret;
	INT32U temp;
	INT32U temp_addr,temp_size;
	INT8U *U8_ptr_source,*U8_ptr_target;
	
	if(g_user_read_data_mode == 6)////从SDRAM中播AUDIO数据
	{
		//U8_ptr_source = (INT8U*)g_user_read_data_addr;
		U8_ptr_source = (INT8U*)(global_user_read_data_addr + g_audio_data_offset);
		U8_ptr_target = (INT8U*)buf_addr;
		temp_size = buf_size;
		for(; temp_size; temp_size--)
		{
			*U8_ptr_target++ = *U8_ptr_source++;
		}
		//g_user_read_data_addr += buf_size;
		g_audio_data_offset += buf_size;
		return buf_size;
	}	
	temp_size = buf_size;
	U8_ptr_target = (INT8U*)buf_addr;
	if(g_user_read_data_addr & 0x000001ff)
	{
		temp = (g_user_read_data_addr&0xfffffe00) + 0x00000200;
		ret = read_a_sector(g_user_read_data_fd, (g_user_read_data_addr&0xfffffe00), (INT32U)G_user_read_buffer, g_user_read_data_mode);
		if(ret<0)
		{
			user_err();
		}
		U8_ptr_source = (INT8U*)(G_user_read_buffer + ((INT32U)g_user_read_data_addr & 0x000001ff) );
		while((g_user_read_data_addr < temp) && (temp_size))
		{
			*U8_ptr_target++ = *U8_ptr_source++;
			g_user_read_data_addr += 1;		
			temp_size -= 1;
		}
	}
	temp_addr = (INT32U)U8_ptr_target;
	while(temp_size >= 512)
	{
		ret = read_a_sector(g_user_read_data_fd, (g_user_read_data_addr&0xfffffe00), temp_addr, g_user_read_data_mode);
		if(ret < 0)
		{
			user_err();
		}
		g_user_read_data_addr += 512;
		temp_size -= 512;
		temp_addr += 512;		
	}
	if(temp_size)
	{
		ret = read_a_sector(g_user_read_data_fd, (g_user_read_data_addr&0xfffffe00), (INT32U)G_user_read_buffer, g_user_read_data_mode);
		if(ret < 0)
		{
			user_err();
		}
		U8_ptr_source = (INT8U*)(G_user_read_buffer + ((INT32U)g_user_read_data_addr & 0x000001ff) );
		U8_ptr_target = (INT8U*)temp_addr;
		while(temp_size)
		{
			*U8_ptr_target++ = *U8_ptr_source++;
			g_user_read_data_addr += 1;		
			temp_size -= 1;
		}
	}
	//  temp_addr1 = g_user_read_data_addr;
	return buf_size;
#endif
}
//=============================================================================
INT32S Snd_GetData(INT32U buf_adr,INT32U buf_size)
{
	//USER should fill this buf
	//Just for demo use
	return read(G_Snd_UserFd, buf_adr, buf_size);
}

INT32U Snd_GetLen(void)	//read file length
{
	struct stat_t state_buffer;

 	fstat(G_Snd_UserFd, &state_buffer);
	return state_buffer.st_size;
}

//=============================================================================
//========================================================
//Function Name:	Snd_Lseek
//Syntax:		INT32S Snd_Lseek(INT32S offset,INT16S fromwhere)
//Purpose:		change data point of file
//Note:			use lseek(fd, 0, SEEK_CUR) can get current offset of file.
//Parameters:   fd, offset, fromwhere
//Return:		data point
//=======================================================
//INT32S lseek(INT16S fd,INT32S offset,INT16S fromwhere);
INT32S Snd_Lseek(INT32S offset,INT16S fromwhere)
{
	INT32S R_Offset;
	//USER should move read pointer follow the paramater
	//Just for demo use
    lseek(G_Snd_UserFd, offset, SEEK_CUR);
	R_Offset = lseek(G_Snd_UserFd, 0, SEEK_CUR);
	return 0;
}

//=======================================================================
//  SPU midi decode use memory manage  
//=======================================================================
void* user_memory_malloc(INT32U size)
{
	return gp_malloc_align(size, 4);
}

void user_memory_free(void* buffer_addr)
{
	gp_free(buffer_addr);	
}

void audio_data_write_callback_register(INT32S (*callback_function)(INT8U bHeader, INT32U buffer_addr, INT32U cbLen))
{
	audio_data_write_callback = callback_function;
	return;
}

/*******************************************************************************
This function is for audio record to write data when use user-define mode.
INT32U audio_encode_data_write(INT8U bHeader, INT32U buffer_addr, INT32U cbLen)
parameter: 	bHeader -> is file header data or not.
			buffer_addr -> buffer address
			cbLen -> buffer size by byte
return:	return the success write data size 	
********************************************************************************/
INT32U audio_encode_data_write(INT8U bHeader, INT32U buffer_addr, INT32U cbLen)
{
	static INT8U  status = 0;
	static INT16S fd;
	INT32U nRet;
	
	if(audio_data_write_callback!=NULL)
	{
		audio_data_write_callback(bHeader, buffer_addr, cbLen);
		return cbLen;
	} 
	else
	{		
		if(status == 0)
		{
			fd = open("a:\\UserDef.wav", O_CREAT|O_WRONLY);
			status = 1;
		}
	
		if(bHeader == 0)
		{
			nRet = write(fd, (INT32U) buffer_addr, cbLen);
		}
		else
		{
			lseek(fd, 0, SEEK_SET);
			nRet = write(fd, (INT32U) buffer_addr, cbLen);
			close(fd);
		}
		return nRet;
	}	
}

