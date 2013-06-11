
/*
* Description: This task reads data from specified file into specified buffer
*
* Author: sunxw
*
* Date: 2008/07/2
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "ap_avifilesrv_task.h"
#define  AUDIO_AVI_RINGBUF_LEN 4000

INT32U avi_audio_flag;
INT32S audiolen;
	
void FileSrv_AVIAudio_Read(STAVIAUDIOBufConfig *para);
void FileSrv_AVIVideo_Read(INT32U JumpFrameNum);

void FileSrv_AVIAudio_Read(STAVIAUDIOBufConfig *para)
{
	INT32S read_cnt;
	INT32S temp_len;
	STAVIAUDIOBufConfig *newptr;
	INT8U *ptr = NULL;

	newptr = avi_audio_buf_wi->next;
	if(avi_audio_buf_wi->status!=AUDIO_BUF_OVER)
	{
		switch(newptr->status)
		{
			case AUDIO_BUF_FIRST_FILL:
				avi_audio_flag = 1;
				if(avi_mov_switchflag == C_MOV_MODE_PLAY)
					audiolen = tk_mov_find_adpos(1);		
				else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
				audiolen = tk_avi_get_audio_data(1);		
			case AUDIO_BUF_FILL_ALL_AGAIN:
				avi_audio_flag = 0;
				while(1)
				{
					if(avi_audio_flag)
					{
						if(avi_mov_switchflag == C_MOV_MODE_PLAY)
							audiolen = tk_mov_find_adpos(1);		
						else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
						audiolen = tk_avi_get_audio_data(1);		
						avi_audio_flag = 0;
					}
					
					//added for the avi file size is small.
					if(audiolen < 0)
					{
						avi_audio_buf_wi->status = AUDIO_BUF_OVER;
						avi_audio_flag = 1;	
						break;			
					}
					if(audiolen <= avi_audio_buf_wi->length)
					{
						if(avi_audio_buf_wi->length>=(avi_audio_buf_wi->wi + (INT32U)audiolen))
						{
							if(avi_mov_switchflag == C_MOV_MODE_PLAY)
								read_cnt = tk_mov_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
							read_cnt = tk_avi_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							avi_audio_buf_wi->wi += (INT32U)audiolen;
							avi_audio_flag = 1;
						}
						else
						{
							avi_audio_buf_wi->status = AUDIO_BUF_FILLING;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi = avi_audio_buf_wi->next;
							if(avi_audio_buf_wi->status != AUDIO_BUF_FIRST_FILL)
								break;
						}
					}
					//sunxw modify code for avi audio length
					else if(audiolen > avi_audio_buf_wi->length*2)
					{
						avi_audio_buf_wi->status = AUDIO_BUF_OVER;
						avi_audio_buf_wi->wi = 0;
						break;
					}
					else
					{
						if(ptr == NULL)
							ptr = (INT8U*)gp_malloc(audiolen);
							
						if(avi_mov_switchflag == C_MOV_MODE_PLAY)
							read_cnt = tk_mov_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
						else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
							read_cnt = tk_avi_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
						if(read_cnt != -1)
						{
							gp_memcpy((INT8S*)avi_audio_buf_wi->audiobufptr,(INT8S*)ptr,avi_audio_buf_wi->length);
							avi_audio_buf_wi->wi = avi_audio_buf_wi->length;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi->status = AUDIO_BUF_FILLING;
							avi_audio_buf_wi = avi_audio_buf_wi->next;
							
							avi_audio_buf_wi->wi = (INT32U)audiolen - avi_audio_buf_wi->length;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi->status = AUDIO_BUF_GETING;
							gp_memcpy((INT8S*)avi_audio_buf_wi->audiobufptr,(INT8S*)((INT32U)ptr+avi_audio_buf_wi->length),avi_audio_buf_wi->wi);
							avi_audio_flag = 1;
							
							avi_audio_buf_wi = avi_audio_buf_wi->next;							
							if(avi_audio_buf_wi->status != AUDIO_BUF_FIRST_FILL)
							{
								gp_free((void*)ptr);
								ptr = NULL;
								break;												
							}	
						}
					}			
				}
				if(audio_fs_result_q)
					OSQPost(audio_fs_result_q , (void *) read_cnt);					
				break;

			case AUDIO_BUF_GETING:
				// if audio len < 1000,or audio len > 1000;		
				temp_len = 0;
				while(1)
				{
					if(avi_audio_flag)
					{
						if(avi_mov_switchflag == C_MOV_MODE_PLAY)
							audiolen = tk_mov_find_adpos(1);		
						else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
						audiolen = tk_avi_get_audio_data(1);		
						if(audiolen == -1)
						{
							avi_audio_buf_wi->status = AUDIO_BUF_OVER;
							avi_audio_flag = 1;
							avi_audio_buf_wi->ri = 0;
							break;
						}
						else
							avi_audio_flag = 0;
					}
					
					if(audiolen <= avi_audio_buf_wi->length)
					{	
						if(avi_audio_buf_wi->length>=(avi_audio_buf_wi->wi + (INT32U)audiolen))
						{
							if(avi_mov_switchflag == C_MOV_MODE_PLAY)
								read_cnt = tk_mov_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
							read_cnt = tk_avi_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							if(read_cnt != -1)
							{
								avi_audio_buf_wi->wi += (INT32U)audiolen;
								avi_audio_flag = 1;
								temp_len += audiolen;
							}
							else
							{
								avi_audio_buf_wi->status = AUDIO_BUF_OVER;
								avi_audio_flag = 1;
								avi_audio_buf_wi->ri = 0;
								break;
							}
						}
						else
						{
							avi_audio_buf_wi->status = AUDIO_BUF_FILLING;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi = avi_audio_buf_wi->next;
							break;
						}

						if(temp_len>AUDIO_AVI_RINGBUF_LEN)
							break;	
					}	
					else
						break;					
				}
					
				break;
				
			
			case AUDIO_BUF_EMPTY:
				//data len smaller than ring buffer len.		
				// if audio len < 1000,or audio len > 1000;		
				temp_len = 0;
				while(1)
				{
					if(avi_audio_flag)
					{
						if(avi_mov_switchflag == C_MOV_MODE_PLAY)
							audiolen = tk_mov_find_adpos(1);		
						else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
						audiolen = tk_avi_get_audio_data(1);		
						if(audiolen == -1)
						{
							avi_audio_buf_wi->status = AUDIO_BUF_OVER;
							avi_audio_flag = 1;
							avi_audio_buf_wi->ri = 0;
							break;
						}
						else
							avi_audio_flag = 0;
					}
						
					if(audiolen <= avi_audio_buf_wi->length)	
					{	
						if(avi_audio_buf_wi->length>=(avi_audio_buf_wi->wi + (INT32U)audiolen))
						{
							if(avi_mov_switchflag == C_MOV_MODE_PLAY)
								read_cnt = tk_mov_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
							read_cnt = tk_avi_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
							if(read_cnt != -1)
							{
								avi_audio_buf_wi->wi += (INT32U)audiolen;
								avi_audio_flag = 1;
								temp_len += audiolen;
							}
							else
							{
								avi_audio_buf_wi->status = AUDIO_BUF_OVER;
								avi_audio_flag = 1;
								avi_audio_buf_wi->ri = 0;
								break;
							}
						}
						else
						{
							avi_audio_buf_wi->status = AUDIO_BUF_FILLING;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi = avi_audio_buf_wi->next;
							break;
						}
					}
					//sunxw modify code for avi audio length
					else if(audiolen > avi_audio_buf_wi->length*2)
					{
						avi_audio_buf_wi->status = AUDIO_BUF_OVER;
						avi_audio_buf_wi->wi = 0;
						break;
					}
					else
					{
						if(ptr == NULL)
							ptr = (INT8U*)gp_malloc(audiolen);
							
						if(avi_mov_switchflag == C_MOV_MODE_PLAY)
							read_cnt = tk_mov_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
						else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
							read_cnt = tk_avi_read_audio_data((CHAR *)((INT32U)avi_audio_buf_wi->audiobufptr+avi_audio_buf_wi->wi),(INT32U)audiolen);
						if(read_cnt != -1)
						{
							gp_memcpy((INT8S*)avi_audio_buf_wi->audiobufptr,(INT8S*)ptr,avi_audio_buf_wi->length);
							avi_audio_buf_wi->wi = avi_audio_buf_wi->length;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi->status = AUDIO_BUF_FILLING;
							avi_audio_buf_wi = avi_audio_buf_wi->next;
							
							avi_audio_buf_wi->wi = (INT32U)audiolen - avi_audio_buf_wi->length;
							avi_audio_buf_wi->ri = 0;
							avi_audio_buf_wi->status = AUDIO_BUF_GETING;
							gp_memcpy((INT8S*)avi_audio_buf_wi->audiobufptr,(INT8S*)((INT32U)ptr+avi_audio_buf_wi->length),avi_audio_buf_wi->wi);
							avi_audio_flag = 1;
							
							avi_audio_buf_wi = avi_audio_buf_wi->next;							
							gp_free((void*)ptr);
							ptr = NULL;
								break;												
						}	
						else
						{
							avi_audio_buf_wi->status = AUDIO_BUF_OVER;
							avi_audio_flag = 1;
							avi_audio_buf_wi->ri = 0;
							gp_free((void*)ptr);
							ptr = NULL;							
							break;						
						}			
					}
					
					if(audio_fs_result_q)
						OSQPost(audio_fs_result_q , (void *) read_cnt);
				}							
				break;
			
			case AUDIO_BUF_OVER:
				break;
			
			case AUDIO_BUF_FILLING:
				
				break;
			default:
				break;	
		}
	}
}

void FileSrv_AVIVideo_Read(INT32U JumpFrameNum)
{
	INT32S filepos;
	
	if(avi_mov_switchflag == C_MOV_MODE_PLAY)
		filepos = tk_mov_find_vdpos(JumpFrameNum);
	else if(avi_mov_switchflag == C_AVI_MODE_PLAY)
	
	filepos = tk_avi_find_vdpos(JumpFrameNum);
	OSQPost(image_task_fs_queue_a, (void *) filepos);	/* send return status */
}