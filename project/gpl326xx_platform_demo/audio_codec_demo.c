#include <stdio.h>
#include <string.h>

#include "application.h"
#include "drv_l2_ad_key_scan.h"
# include "gplib.h"


typedef struct FILELIST{
        char data[128];
        struct FILELIST* next;
        struct FILELIST* prev;
}filelist;
filelist *list_head = NULL;
//==========================================================
// macro
//==========================================================
#define USE_DISK	FS_SD
#define USE_ADKEY	1

#define Volume_value_max 127
#define Volume_value_min   0
#define Volume_value_interval 5

#define Speed_value_max 24
#define Speed_value_min 0
#define Speed_value_interval 1

#define Pitch_value_max 24
#define Pitch_value_min 0
#define Pitch_value_interval 1

#define seek_interval 10

#define Demo_Message	print_string 

#define DEBUG
#ifdef DEBUG
#define DEBUG_Message	print_string
#else
#define DEBUG_Message
#endif

//===================================================================
// struct
//====================================================================
enum Audio_demo_key
{
	key_change_type,
	key_play_next,
	key_play_previous,
	key_pause_resume,
	key_mute,
	key_mode_change,
	key_level_up,
	key_level_down,
	key_invalid
};


enum Music_Type
{
	MP3_Type,
	WMA_Type,
	AAC_Type,
	A64_Type,
	A18_Type,
	MIDI_Type
};
enum Effect_Mode
{
	Speed_mode,
	Voice_mode,
	Volume_mode,
	FF_FB_mode
};




extern INT8U g_user_read_data_mode;
extern INT32U global_user_read_data_addr;
struct file_info_list
{
	struct file_info_list *pre,*next;
	char *file_name;
};

typedef struct Audio_demo_info_s
{
	enum Music_Type music_type;
	int  device;
	enum Effect_Mode effect_mode;
	int Effect_Value[4];
	char *path;
	AUDIO_ARGUMENT audio_argument;
	MEDIA_SOURCE  media_source;
	int mute;
	AUDIO_CODEC_STATUS  Audio_Status;
	INT32U total_time;
	AUDIO_FORMAT audio_format;
	int file_id;
	int total_file;
	INT8U scan_finish;
	struct file_info_list *head;
	INT8U show_time_enable;
}Audio_demo_info_t;

OS_EVENT  *Mbox_WMA_Play_Seek_Flag;  
OS_EVENT  *Mbox_WMA_Play_Seek_Offset;

//======================================================
// the extern value
//======================================================
#if APP_VOICE_CHANGER_EN	
extern void *hVC ;		//for speed control
#endif




extern SACM_CTRL G_SACM_Ctrl;
extern MSG_Q_ID  KeyTaskQ;
extern INT16S oemuni_to_utf8(INT8S *Chart, INT8U *UTF8Chart);
extern void mm_dump(void);
//================================================================
// globle value
//==================================================================

INT8U audio_codec_seek_enable=1;
INT32U audio_codec_seek_time;
Audio_demo_info_t *g_audio_demo_info;
static INT8U  accord_key=1;
static INT32U old_time=0;




const char audio_codec_prompt[]="[Audio codec Message] ";
unsigned short audio_type_name[][6]={
	{0x002a,0x002e,0x006d,0x0070,0x0033,0x0000},//*.mp3 unicode
	{0x002a,0x002e,0x0077,0x006d,0x0061,0x0000},
	{0x002a,0x002e,0x0061,0x0061,0x0063,0x0000},//*.aac
	{0x002a,0x002e,0x0061,0x0036,0x0034,0x0000},
	{0x002a,0x002e,0x0061,0x0031,0x0038,0x0000},
	{0x002a,0x002e,0x0067,0x006d,0x0064,0x0000},
	};
unsigned short audio_codec_store[]={0x0063,0x003a,0x005c,0x005c,0x0000};
/*================================================
uninit adc_key
===================================================*/
INT32U adc_key_scan_uninit(void)
{
	//KeyTaskQ = msgQCreate(KEY_QUEUE_MAX, KEY_QUEUE_MAX, KEY_QUEUE_MAX_MSG_LEN);
	msgQDelete(KeyTaskQ);	
	ad_key_uninitial();
	return 0;
}

/*===============================================
UNICODE file name get
===================================================*/
static int  unicode_filename_strlen(char *filename)
{
	unsigned short *temp=(unsigned short *)filename;
	int count=0;
	while(*temp!=0x0000)
	{
		temp++;
		count +=2;
	}
	return count;
}
static void sent_utf8_consol(char *filename)
{
	INT8U *utf8;
	int utf8_len;
	int ret=0;
	utf8_len=unicode_filename_strlen(filename)*3/2;
	utf8=gp_malloc(utf8_len);
	ret=oemuni_to_utf8((INT8S *)filename,utf8);
	if(ret==0)
		Demo_Message("%s:filename:%s",audio_codec_prompt,utf8);
	else
		Demo_Message("%s:change to utf8 err",audio_codec_prompt);
	gp_free(utf8);
}

/*=================================================
same opertion for file info
==================================================*/
static void add_file_list(void *handle,char *filename)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	struct file_info_list *head=ptr_audio_demo_info->head,*temp,*temp1;

	temp=(struct file_info_list*)gp_malloc(sizeof(struct file_info_list)+unicode_filename_strlen(filename)+2);
	temp->file_name =(char *)(temp+2);
	gp_memcpy((INT8S *)temp->file_name,(INT8S *)filename,unicode_filename_strlen(filename)+2);
	
	if(head==NULL)
	{
		head = temp;
		temp->next = head;
		temp->pre = head;
		ptr_audio_demo_info->head =head;
	}
	else
	{	
		for(temp1=head;temp1->next!=head;temp1=temp1->next); //find the end list
		temp1->next = temp;
		temp->pre = temp1;
		temp->next =head;
		head->pre = temp;
	}
}

static int del_file_list(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	struct file_info_list *head=ptr_audio_demo_info->head,*temp1,*temp2;

	if(head==NULL)
		return -1;
	else if(head->pre==head)
	{
		gp_free(head);
		ptr_audio_demo_info->head=NULL;
		return 0;
	}
	else
	{
		for(temp1=head;temp1->next!=head;temp1=temp1->next); //find the end list
		temp2 = temp1->pre;
		temp2->next = head;
		head->pre = temp2;
		gp_free(temp1);
	}
	return 0;
		
}
static void  change_file_list(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	struct file_info_list *head=ptr_audio_demo_info->head;
	struct f_info  info;
	int ret;
	if(head!=NULL)
	{
		while(del_file_list(handle)==0); //del all file list
		head=NULL;
	}
	ptr_audio_demo_info->file_id=0;
	ptr_audio_demo_info->total_file=0;
	ptr_audio_demo_info->scan_finish=0;
	
	ret=_findfirst((char *)audio_type_name[ptr_audio_demo_info->music_type],&info,D_FILE);
	if(ret==0)	
	{
		add_file_list(ptr_audio_demo_info,(char *)info.f_name);
		ptr_audio_demo_info->total_file++;
	}
	else
	{
		sent_utf8_consol((char*)audio_type_name[ptr_audio_demo_info->music_type]);
		Demo_Message("not find file\r\n");
		ptr_audio_demo_info->scan_finish=1;
	}	
}

/*=================================================
function
===================================================*/
static void audio_codec_show_menu()
{
	Demo_Message("**********************************************************************\r\n");
	Demo_Message("*                        This is Audio codec   demo                     *\r\n");
	Demo_Message("**********************************************************************\r\n");
	Demo_Message("KEY1    Type (MP3/ WMA/ AAC/ A64/ A18/ MIDI)\r\n");
	Demo_Message("KEY2    Play next\r\n");
	Demo_Message("KEY3    Play previous\r\n");
	Demo_Message("KEY4    Pause/ Resume\r\n");
	Demo_Message("KEY5    Mute/ Unmute\r\n");
	Demo_Message("KEY6    Speed control/ Voice chnager/ Volume control/ FF&FB\r\n");
	Demo_Message("KEY7    Level up\r\n");
	Demo_Message("KEY8    Level down\r\n");
	Demo_Message("\r\n");
	Demo_Message("\r\n");
}


/**************************************************

**************************************************/
static enum Audio_demo_key audio_codec_scan_key(void *handle)
{
	enum Audio_demo_key key=key_invalid;
	adc_key_scan();
	if(ADKEY_IO1)
		key=key_change_type;
	else if(ADKEY_IO2)
		key=key_play_next;
	else if(ADKEY_IO3)
		key=key_play_previous;
	else if(ADKEY_IO4)
		key=key_pause_resume;
	else if(ADKEY_IO5)
		key = key_mute;
	else if(ADKEY_IO6)
		key=key_mode_change;
	else if(ADKEY_IO7)
		key=key_level_up;
	else if(ADKEY_IO8)
		key=key_level_down;
	return key;
		
}
//==========================================================================
//brief: This is callback function ,to show current time
//when G_SACM_Ctrl.Offsettype ==SND_OFFSET_TYPE_NONE: show now  seek. do not show time.
//==========================================================================
static void  audio_codec_show_time()
{
	static int count;
	INT8U  KeyPara[2];
	
	INT32U time;
	if((count++>64)&&(G_SACM_Ctrl.Offsettype ==SND_OFFSET_TYPE_NONE)&&(g_audio_demo_info->Audio_Status!=AUDIO_CODEC_STATUS_MAX))
	{
		if(g_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESSING)
		{
			if(g_audio_demo_info->show_time_enable==1)//only those type can get current time
			{
				time = audio_decode_GetCurTime(g_audio_demo_info->media_source.type_ID.FileHandle ,g_audio_demo_info->audio_format)/10; //return value is 0.1s);
				if(time!=old_time)
				{
					Demo_Message(" %s:current time:%d\r\n",audio_codec_prompt, time);
					old_time=time;
				}
			}
			accord_key=0;
		}	
		if(accord_key==0) //if key has sent, do not sent again.
		{
			g_audio_demo_info->Audio_Status =audio_decode_status(g_audio_demo_info->audio_argument);
			if(g_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESS_END)
			{
		
				KeyPara[0]=AD_KEY_2;
				KeyPara[1]=RB_KEY_DOWN;
				msgQSend(KeyTaskQ,1,KeyPara,2,0); //simulate next key
				accord_key =1;
			}
		}
		count=0;
		
	}
}
//=============================================================================
//pause & resume
//=============================================================================
static int audio_codec_pause_resume_music(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	AUDIO_CODEC_STATUS status=ptr_audio_demo_info->Audio_Status;
	
	if(status==AUDIO_CODEC_PROCESSING)
	{
		audio_decode_pause(ptr_audio_demo_info->audio_argument);
	}
	else if(status==AUDIO_CODEC_PROCESS_PAUSED)
	{
		audio_decode_resume(ptr_audio_demo_info->audio_argument);
	}
	ptr_audio_demo_info->Audio_Status=audio_decode_status(ptr_audio_demo_info->audio_argument);
	return 0;
}
//====================================================================
// mute music
//====================================================================
static int  audio_codec_mute_music(void * handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	if(ptr_audio_demo_info->mute)
	{
		ptr_audio_demo_info->mute =0;
		audio_decode_unmute(&ptr_audio_demo_info->audio_argument);
		Demo_Message("%s: audio codec unmute\r\n",audio_codec_prompt);
	}
	else
	{
		ptr_audio_demo_info->mute =1;
		audio_decode_mute(&ptr_audio_demo_info->audio_argument);
		Demo_Message("%s: audio codec mute\r\n",audio_codec_prompt);
	}
	return 0;
	
}
static int audio_codec_stop_music(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	audio_decode_stop(ptr_audio_demo_info->audio_argument);
	return 0;
}
/*===============================================
play music
=================================================*/
static int audio_codec_play_music(void* handle, char *filename)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	AUDIO_ARGUMENT *ptr_audio_arg = &ptr_audio_demo_info->audio_argument;
	MEDIA_SOURCE *ptr_media_src = &ptr_audio_demo_info->media_source;
	AUDIO_FORMAT audio_format;
	

	ptr_audio_demo_info->Audio_Status=audio_decode_status(ptr_audio_demo_info->audio_argument);
	
	if((ptr_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESSING)||(ptr_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESS_PAUSED))
	{
		audio_codec_stop_music(handle);
		//ptr_audio_demo_info->Audio_Status=audio_decode_status(ptr_audio_demo_info->audio_argument);
	}
	gp_memset((INT8S *)ptr_audio_arg,0,sizeof(AUDIO_ARGUMENT));
	gp_memset((INT8S *)ptr_media_src,0,sizeof(MEDIA_SOURCE));
	if(ptr_audio_demo_info->path)
	{
		gp_free(ptr_audio_demo_info->path);
		ptr_audio_demo_info->path =NULL;
	}
	ptr_audio_demo_info->path=gp_malloc(unicode_filename_strlen(filename)+2);
	gp_memcpy((INT8S*)ptr_audio_demo_info->path,(INT8S*)filename,unicode_filename_strlen(filename)+2);
	
	ptr_audio_arg->mute =ptr_audio_demo_info->mute;
	ptr_audio_arg->volume=ptr_audio_demo_info->Effect_Value[Volume_mode];
	ptr_audio_arg->midi_index =0;
	ptr_audio_arg->L_R_Channel =3;


	if(ptr_media_src->type_ID.FileHandle>=0)
	{
			close(ptr_media_src->type_ID.FileHandle);
			ptr_media_src->type_ID.FileHandle =-1;
	}
	ptr_media_src->type_ID.FileHandle =open(filename,O_RDONLY); 
	if(ptr_media_src->type_ID.FileHandle<0)
	{
		DEBUG_Message("open file :%s fail\r\n",filename);
		gp_free(ptr_audio_demo_info->path);
		return -1;
	}
	ptr_media_src->type =SOURCE_TYPE_FS;
	
	switch(ptr_audio_demo_info->music_type)
	{
		case MP3_Type:
			audio_format = MP3;
			ptr_audio_arg->Main_Channel=1;
			ptr_media_src->Format.AudioFormat =MP3;
			g_audio_demo_info->show_time_enable =1;
			break;
		case WMA_Type:
			audio_format = WMA;
			ptr_audio_arg->Main_Channel=1;
			ptr_media_src->Format.AudioFormat =WMA;
			g_audio_demo_info->show_time_enable =0;
			break;
		case AAC_Type:
			audio_format=AAC;
			ptr_audio_arg->Main_Channel=1;
			ptr_media_src->Format.AudioFormat = AAC;
			g_audio_demo_info->show_time_enable =0;
			break;
		case A64_Type:
			audio_format = A6400;
			ptr_audio_arg->Main_Channel=1;
			ptr_media_src->Format.AudioFormat =A6400;
			g_audio_demo_info->show_time_enable =0;
			break;
		case A18_Type:
			audio_format = A1800;
			ptr_audio_arg->Main_Channel=1;
			ptr_media_src->Format.AudioFormat =A1800;
			g_audio_demo_info->show_time_enable =0;
			break;
		case MIDI_Type:
			audio_format =MIDI;
			ptr_audio_arg->Main_Channel=2;
			ptr_media_src->Format.AudioFormat =MIDI;
			ptr_media_src->type_ID.memptr =0;
			g_audio_demo_info->show_time_enable =0;
			break;
		default:
			break;
	}
	ptr_audio_demo_info->audio_format = audio_format;
	ptr_audio_demo_info->Effect_Value[Speed_mode]=12;
	ptr_audio_demo_info->Effect_Value[Voice_mode]=12; //set speed voice default vaule

	if(audio_decode_start(ptr_audio_demo_info->audio_argument,ptr_audio_demo_info->media_source)!=START_OK)
	{
		Demo_Message("play file: ");
		sent_utf8_consol(ptr_audio_demo_info->path);
		Demo_Message("fail\r\n");
		return -1;
	}
	ptr_audio_demo_info->Audio_Status = audio_decode_status(ptr_audio_demo_info->audio_argument);
	if(ptr_audio_demo_info->music_type==MIDI_Type)
		SPU_MIDI_Repeat(0);
	
	ptr_audio_demo_info->total_time = audio_decode_GetTotalTime(ptr_media_src->type_ID.FileHandle ,audio_format);
	sent_utf8_consol(ptr_audio_demo_info->path);
	Demo_Message("\r\n");
	return 0;
	
}

//=========================================================================
//brief: seek function . 
//=========================================================================
static void audio_codec_seek(void *handle,INT32U sec,INT8S direct)
{
	
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	INT32U current_time;
	INT32U total_time;
	MEDIA_SOURCE *ptr_media_src = &ptr_audio_demo_info->media_source;
	if((ptr_audio_demo_info->music_type==WMA_Type)||(ptr_audio_demo_info->music_type==MP3_Type))
	{
		if((ptr_audio_demo_info->Effect_Value[Speed_mode]!=12)||(ptr_audio_demo_info->Effect_Value[Voice_mode]!=12))
		{
			Demo_Message("%s: speed value is %d,voice value is%d,please set them to 12\r\n",audio_codec_prompt,ptr_audio_demo_info->Effect_Value[Speed_mode],ptr_audio_demo_info->Effect_Value[Voice_mode]);
			return ;
		}
		if(G_SACM_Ctrl.Offsettype ==SND_OFFSET_TYPE_TIME)
			current_time = audio_codec_seek_time; //now is seek. I think seek is OK. I will seek base seek_time
		else
			current_time = audio_decode_GetCurTime(ptr_media_src->type_ID.FileHandle ,ptr_audio_demo_info->audio_format); //return value is 0.1s
		current_time =current_time*100; //change to ms
		total_time=ptr_audio_demo_info->total_time*1000; //s ->ms
		DEBUG_Message("current_time: %dms\n",current_time);
		if(direct==1)
		{
			audio_codec_seek_time = current_time+sec*1000;
			if(audio_codec_seek_time>total_time)
				audio_codec_seek_time=ptr_audio_demo_info->total_time*1000;
		}
		else
		{
			if(current_time>sec*1000)
				audio_codec_seek_time=current_time-sec*1000;
			else
				audio_codec_seek_time=0;
		}	
		G_SACM_Ctrl.Offsettype =SND_OFFSET_TYPE_TIME;
		 OSMboxPost(Mbox_WMA_Play_Seek_Flag , &audio_codec_seek_enable);
		 OSMboxPost(Mbox_WMA_Play_Seek_Offset , &audio_codec_seek_time);
               
	}
	else 
		Demo_Message("%s:  %s not suppoert seek\n",audio_codec_prompt,audio_type_name[ptr_audio_demo_info->music_type]);
}

//===========================================================================
// level up
//==========================================================================
static void audio_codec_level_up(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	switch(ptr_audio_demo_info->effect_mode)
	{
		case Speed_mode:
		 #if APP_VOICE_CHANGER_EN	
			ptr_audio_demo_info->Effect_Value[Speed_mode]+=Speed_value_interval;
			if(ptr_audio_demo_info->Effect_Value[Speed_mode]>Speed_value_max)
				ptr_audio_demo_info->Effect_Value[Speed_mode]=Speed_value_max;
			VoiceChanger_SetParam(hVC,ptr_audio_demo_info->Effect_Value[Speed_mode],ptr_audio_demo_info->Effect_Value[Voice_mode]);
			Demo_Message("%s:current speed:%d\r\n",audio_codec_prompt,ptr_audio_demo_info->Effect_Value[Speed_mode]);
		#else
			Demo_Message("%s:change speed not support\n\r",audio_codec_prompt);
		#endif
			 break;
		case Voice_mode:
		#if APP_VOICE_CHANGER_EN
			ptr_audio_demo_info->Effect_Value[Voice_mode]+=Pitch_value_interval;
			if(ptr_audio_demo_info->Effect_Value[Voice_mode]>Pitch_value_max)
				ptr_audio_demo_info->Effect_Value[Voice_mode]=Pitch_value_max;
			VoiceChanger_SetParam(hVC,ptr_audio_demo_info->Effect_Value[Speed_mode],ptr_audio_demo_info->Effect_Value[Voice_mode]);
			Demo_Message("%s:current pitch:%d\r\n",audio_codec_prompt,ptr_audio_demo_info->Effect_Value[Voice_mode]);
		#else
			Demo_Message("%s:change pitch  not support\n\r",audio_codec_prompt);
		#endif
			 break;
		case Volume_mode:
			 ptr_audio_demo_info->Effect_Value[Volume_mode]+=Volume_value_interval;
			 if( ptr_audio_demo_info->Effect_Value[Volume_mode]>Volume_value_max)
			 	 ptr_audio_demo_info->Effect_Value[Volume_mode]=Volume_value_max;
			 audio_decode_volume_set( &ptr_audio_demo_info->audio_argument,ptr_audio_demo_info->Effect_Value[Volume_mode]);
			 Demo_Message("%s:current volume:%d\n\r",audio_codec_prompt, ptr_audio_demo_info->Effect_Value[Volume_mode]);
			 break;
		case FF_FB_mode:
			audio_codec_seek(handle,seek_interval,1);
			break;
		default:
			DBG_PRINT("not support\r\n");
	}
		
}
//===========================================================================
//level down
//===========================================================================
static void audio_codec_level_down(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	switch(ptr_audio_demo_info->effect_mode)
	{
		case Speed_mode:
		 #if APP_VOICE_CHANGER_EN	
			ptr_audio_demo_info->Effect_Value[Speed_mode]-=Speed_value_interval;
			if(ptr_audio_demo_info->Effect_Value[Speed_mode]<Speed_value_min)
				ptr_audio_demo_info->Effect_Value[Speed_mode]=Speed_value_min;
			VoiceChanger_SetParam(hVC,ptr_audio_demo_info->Effect_Value[Speed_mode],ptr_audio_demo_info->Effect_Value[Voice_mode]);
			Demo_Message("%s:current speed:%d\r\n",audio_codec_prompt,ptr_audio_demo_info->Effect_Value[Speed_mode]);
		#else
			Demo_Message("%s:change speed not support\n\r",audio_codec_prompt);
		#endif
			 break;
		case Voice_mode:
		#if APP_VOICE_CHANGER_EN
			ptr_audio_demo_info->Effect_Value[Voice_mode]-=Pitch_value_interval;
			if(ptr_audio_demo_info->Effect_Value[Voice_mode]<Pitch_value_min)
				ptr_audio_demo_info->Effect_Value[Voice_mode]=Pitch_value_min;
			VoiceChanger_SetParam(hVC,ptr_audio_demo_info->Effect_Value[Speed_mode],ptr_audio_demo_info->Effect_Value[Voice_mode]);
			Demo_Message("%s:current pitch:%d\r\n",audio_codec_prompt,ptr_audio_demo_info->Effect_Value[Voice_mode]);
		#else
			Demo_Message("%s:change picth not support\n\r",audio_codec_prompt);
		#endif
			 break;
		case Volume_mode:
			 ptr_audio_demo_info->Effect_Value[Volume_mode]-=Volume_value_interval;
			 if( ptr_audio_demo_info->Effect_Value[Volume_mode]<Volume_value_min)
			 	 ptr_audio_demo_info->Effect_Value[Volume_mode]=Volume_value_min;
			 audio_decode_volume_set( &ptr_audio_demo_info->audio_argument,ptr_audio_demo_info->Effect_Value[Volume_mode]);
			 Demo_Message("%s:current volume:%d\n\r",audio_codec_prompt, ptr_audio_demo_info->Effect_Value[Volume_mode]);
			 break;
		case FF_FB_mode:
			audio_codec_seek(handle,seek_interval,0);
			break;
		default:
			DBG_PRINT("not support\r\n");
	}
}
//=============================================================================
//change mode
//=============================================================================
static void audio_codec_change_mode(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	switch (ptr_audio_demo_info->effect_mode)
	{
		case Speed_mode:
			 ptr_audio_demo_info->effect_mode =Voice_mode;
			 Demo_Message("%s:current mode: Voice_mode \r\n",audio_codec_prompt);
			 break;
		case Voice_mode:
		 	ptr_audio_demo_info->effect_mode = Volume_mode;
			Demo_Message("%s:current mode:Volume_mode\r\n",audio_codec_prompt);
			break;
		case Volume_mode:
			ptr_audio_demo_info->effect_mode=FF_FB_mode;
			Demo_Message("%s:current mode :FF_mode\r\n ",audio_codec_prompt);
			break;
		case FF_FB_mode:
			ptr_audio_demo_info->effect_mode=Speed_mode;
			Demo_Message("%s:current mode :Speed_mode\r\n",audio_codec_prompt);
			break;
		default:
			Demo_Message("%s:ERR: not this mode\n\r",audio_codec_prompt);
	}
	
}
static void scan_next_filename(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	struct f_info  info;
	int ret=0;
	ret=_findnext(&info);
	if(ret==0)
	{
		add_file_list(ptr_audio_demo_info,(char *)info.f_name);
		ptr_audio_demo_info->total_file++;
	}
	else
		ptr_audio_demo_info->scan_finish=1;
}
static char * audio_codec_get_file_name(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	struct file_info_list *temp1=ptr_audio_demo_info->head;
	int index,i=1;
	if(temp1==NULL)
	{
		Demo_Message("%s: %s is not find\n",audio_codec_prompt,audio_type_name[ptr_audio_demo_info->music_type]);
		return NULL;
	}
	
	index = ptr_audio_demo_info->file_id;
	while(i<index)
	{
		temp1=temp1->next;
		i++;
	}
	return temp1->file_name;
}
static void audio_codec_play_next(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	int index = ptr_audio_demo_info->file_id;
	char *filename;
	
	index++;
	if(index>ptr_audio_demo_info->total_file)
		index=ptr_audio_demo_info->total_file;
	
	 ptr_audio_demo_info->file_id=index;

	filename=audio_codec_get_file_name(handle);
	if(filename!=NULL)
	{
		audio_codec_play_music(handle,filename);
		
		if(ptr_audio_demo_info->scan_finish==0)
			scan_next_filename(handle);
	}
}
static void  audio_codec_play_pre(void *handle)
{
	
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	int index = ptr_audio_demo_info->file_id;
	char *filename;
	
	index--;
	if(index<1)
		index =1;

	 ptr_audio_demo_info->file_id=index;

	filename=audio_codec_get_file_name(handle);
	if(filename!=NULL)
		audio_codec_play_music(handle,filename);
	

}
/*==================================================

==================================================*/
void	audio_codec_change_type(void *handle)
{
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	enum Music_Type  music_type;

	music_type=ptr_audio_demo_info->music_type;
	music_type++;
	if(music_type>MIDI_Type)
		music_type = MP3_Type;
	ptr_audio_demo_info->music_type=music_type;
	switch(ptr_audio_demo_info->music_type)
	{
		case MP3_Type:
			Demo_Message("%s:Current play type is MP3\r\n",audio_codec_prompt);
			break;
		case WMA_Type:
			Demo_Message("%s:Current play type is WMA\r\n",audio_codec_prompt);
			break;
		case AAC_Type:;
			Demo_Message("%s:Current play type is AAC\r\n",audio_codec_prompt);	
			break;
		case A64_Type:
			Demo_Message("%s:Current play type is A64\r\n",audio_codec_prompt);	
			break;
		case A18_Type:
			Demo_Message("%s:Current play type is A18\r\n",audio_codec_prompt);	
			break;
		case MIDI_Type:
			Demo_Message("%s:Current play type is GMIDI\r\n",audio_codec_prompt);	
			break;
		default:
			break;
	}
	
	
	 change_file_list(handle);
}


/*================================================
key init 
==================================================*/
static void audio_codec_key_init()
{
	adc_key_scan_init();
}

static int audio_codec_info_init(Audio_demo_info_t  **audio_demo_info)
{
	int mount_cnt=4;
	Audio_demo_info_t *ptr_audio_demo_info;

	ptr_audio_demo_info =(Audio_demo_info_t *)gp_malloc(sizeof(Audio_demo_info_t));
	
	//step1 clear struct
	gp_memset((INT8S*)ptr_audio_demo_info,0,sizeof(struct Audio_demo_info_s));
	//step2 mount device
	while(mount_cnt--)
	{
		if(_devicemount(USE_DISK))					// Mount device
		{
			DEBUG_Message("%s:Mount Disk Fail[%d]\r\n", audio_codec_prompt,USE_DISK);
		#if USE_DISK == FS_NAND1
			ret = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();  
			_deviceunmount(FS_NAND1); 
		#endif
			OSTimeDly(5);
		}
		else
		{
			chdir((char *)audio_codec_store);
			DEBUG_Message("%s:Mount Disk success[%d]\r\n", audio_codec_prompt,USE_DISK);
			break;
		}
	}
	if(mount_cnt==0)
		return -1;
       //step3 init struct
       ptr_audio_demo_info->device=USE_DISK;
	ptr_audio_demo_info->music_type=MP3_Type;
	ptr_audio_demo_info->effect_mode=Speed_mode;
	ptr_audio_demo_info->Effect_Value[Speed_mode]=12;
	ptr_audio_demo_info->Effect_Value[Voice_mode]=12;
	ptr_audio_demo_info->Effect_Value[Volume_mode]=63;
	ptr_audio_demo_info->Effect_Value[FF_FB_mode]=0;
	ptr_audio_demo_info->mute = 0;
	ptr_audio_demo_info->Audio_Status = AUDIO_CODEC_STATUS_MAX;

	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	 #if APP_VOICE_CHANGER_EN	
	 	 G_snd_info.VoiceChangerEn=TRUE;//this maybe add on Audio_decode_entrance function better.
	 #endif
	 g_audio_demo_info =ptr_audio_demo_info;

	Demo_Message("%s:Current play type is MP3\r\n",audio_codec_prompt);
	change_file_list(ptr_audio_demo_info);
	*audio_demo_info= ptr_audio_demo_info;
	return 0;
	
}

/*========================================================
init function for audio Decode_demo
=========================================================*/
static int Audio_codec_init(Audio_demo_info_t  **audio_demo_info)
{
	int ret=0;
	ChangeCodePage(UNI_UNICODE);
	ret = audio_codec_info_init(audio_demo_info);
	if(ret!=0)
	{
		DEBUG_Message("%s:ERR, init audio_demo_info_fai\r\n",audio_codec_prompt);;
		return -1;
	}
	audio_codec_key_init();
	sys_registe_timer_isr(audio_codec_show_time);

	//init the globe value
	audio_codec_seek_enable=1;
	audio_codec_seek_time=0;
	accord_key=1;
	old_time=0;
	return 0;
}
static void Audio_codec_uninit(void *handle)
{
	
	Audio_demo_info_t *ptr_audio_demo_info=(Audio_demo_info_t *)handle;
	MEDIA_SOURCE *ptr_media_src = &ptr_audio_demo_info->media_source;
	
	sys_release_timer_isr(audio_codec_show_time);
	if((ptr_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESSING)||(ptr_audio_demo_info->Audio_Status==AUDIO_CODEC_PROCESS_PAUSED))
	{
		audio_codec_stop_music(handle);
		//ptr_audio_demo_info->Audio_Status=audio_decode_status(ptr_audio_demo_info->audio_argument);
	}
	if(ptr_audio_demo_info->path)
	{
		gp_free(ptr_audio_demo_info->path);
		ptr_audio_demo_info->path =NULL;
	}
	if(ptr_media_src->type_ID.FileHandle>=0)
	{
			close(ptr_media_src->type_ID.FileHandle);
			ptr_media_src->type_ID.FileHandle =-1;
	}
	audio_decode_exit();
	while(del_file_list(handle)==0); //del file cach
	disk_safe_exit(USE_DISK);
	_deviceunmount(USE_DISK);
	gp_free(handle);
	//uninit the globe value
	audio_codec_seek_enable=1;
	audio_codec_seek_time=0;
	g_audio_demo_info=NULL;
	accord_key=1;
	old_time=0;
	adc_key_scan_uninit();
	
}
//=================================================================================================
//	Audio Decode demo code
//=================================================================================================
void Audio_Decode_Demo(void)
{

	Audio_demo_info_t  *audio_demo_info;
	enum Audio_demo_key audio_codec_key;
	INT8U loop_flag =1;

	mm_dump();
	audio_codec_show_menu();
	if(Audio_codec_init(&audio_demo_info)!=0)
	{
		DBG_PRINT("audio decode init fail\r\n");
		return ;
	}
	while(loop_flag)
	{
		audio_codec_key=audio_codec_scan_key(audio_demo_info);
		switch (audio_codec_key)
		{
			case key_change_type:
				audio_codec_change_type(audio_demo_info);
				DBG_PRINT("key_change_type\r\n");
				break;
			case key_play_next:
				DBG_PRINT("key_play_next\r\n");
				audio_codec_play_next(audio_demo_info);
				break;
			case key_play_previous:
				DBG_PRINT("key_play_previous\r\n");
				audio_codec_play_pre(audio_demo_info);
				break;
			case key_pause_resume:
				DBG_PRINT("key pause & resume\r\n");
				audio_codec_pause_resume_music(audio_demo_info);
				break;
			case key_mute:
				DBG_PRINT("key mute\r\n");
				audio_codec_mute_music(audio_demo_info);
				break;
			case key_mode_change:
				DBG_PRINT("key mode change\r\n");
				audio_codec_change_mode(audio_demo_info);
				break;
			case key_level_up:
				DBG_PRINT("key level up\r\n");
				audio_codec_level_up(audio_demo_info);
				break;
			case key_level_down:
				DBG_PRINT("key level down\r\n");
				audio_codec_level_down(audio_demo_info);
				break;
			default:
				break;
		}
	}
	Audio_codec_uninit(audio_demo_info);
	Demo_Message(":%s : exit demo",audio_codec_prompt);
	mm_dump();
}






//=================================================================================================
//	Audio Encode demo code
//=================================================================================================

void list_create(char* data)
{
	filelist *temp;

    list_head = (filelist*)gp_malloc(sizeof(filelist));
    list_head->next = list_head;
    list_head->prev = list_head;
	gp_memcpy((INT8S*)&list_head->data, (INT8S*)"head", sizeof(list_head->data));

    temp = list_head;

    temp->next = (filelist*)gp_malloc(sizeof(filelist));
    temp = temp->next;
    temp->next = list_head;
    temp->prev = list_head;
    list_head->prev = temp;
	gp_memcpy((INT8S*)&temp->data, (INT8S*)data, sizeof(temp->data));
	
  	DBG_PRINT("p->data=%s, data=%s\r\n", temp->data, data);
}

void list_insert(char* data)
{
	filelist *p;
	filelist *temp;
	
	p = list_head->prev;

    temp = (filelist*)gp_malloc(sizeof(filelist));
    gp_memcpy((INT8S*)&temp->data, (INT8S*)data, sizeof(temp->data));

	DBG_PRINT("p->data=%s, data=%s\r\n", temp->data, data);

    temp->next = p->next;
    p->next = temp;
    (temp->next)->prev = temp;
    temp->prev = p;

}

void list_disp()
{
	filelist *temp;

    temp = list_head;
    DBG_PRINT("p->data=%s, p=0x%08x, p->next=0x%08x, p->prev=0x%08x\r\n", 
    		temp,
    		temp->data,
    		temp->next,
    		temp->prev);
   
    temp = temp->next;
    while(temp != list_head)
    {
        DBG_PRINT("p->data=%s, p=0x%08x, p->next=0x%08x, p->prev=0x%08x\r\n", 
	        temp,
    		temp->data,
    		temp->next,
    		temp->prev);

        temp = temp->next;
    }
}

int audio_filelist(char* keyword)
{
	struct f_info finfo;
	int idx = 0;

	if (_findfirst (keyword, &finfo, D_ALL))	{
		DBG_PRINT("error:no audio files !!!\r\n");
		return -1;
	}
	
	do	{
		if (idx == 0)
			list_create((char*)&finfo.f_name);
		else
			list_insert((char*)&finfo.f_name);
				
		idx ++;
	}
	while (_findnext (&finfo) == 0);

	list_disp();

	return idx;

}


void audio_encode_playback(char* filename, AUDIO_ARGUMENT* dec_arg)
{
	AUDIO_ARGUMENT 	temp;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	INT32U  dec_type = 0;
	
	INT32U  filename_len = strlen(filename);
		
	//init varaible
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));
	media_source_0.type = SOURCE_TYPE_FS;
	audio_status_0 = AUDIO_CODEC_PROCESS_END;
	
	
	if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'a') && 
		(filename[filename_len-2] == '1') && 
		(filename[filename_len-1] == '8'))
	{
		dec_type = 0;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'w') && 
		(filename[filename_len-2] == 'a') && 
		(filename[filename_len-1] == 'v'))
	{
		dec_type = 1;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'm') && 
		(filename[filename_len-2] == 'p') && 
		(filename[filename_len-1] == '3'))
	{
		dec_type = 2;
	}
		
	switch(dec_type)
	{
		case 0:	// a18
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = A1800;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)

			break;
		case 1:	// pcm, ima_adpcm, ms_adpcm
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = WAV;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)

			break;
			
		case 2:	// mp3
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = MP3;
			
			temp.midi_index = 1;		// midi use

			G_snd_info.Speed = 12;					// for speed control(golbal var)
			G_snd_info.Pitch = 12;					// for pitch control(golbal var)

			break;
		default:
			break;
	}
	
	DBG_PRINT("now decoding %s ... \r\n", filename);
	audio_status_0 = audio_decode_start(temp, media_source_0);	
	if (audio_status_0 != 0)
		DBG_PRINT("audio_decode_error!!!\r\n");
			
	
}

void audio_stop_processing(AUDIO_ARGUMENT* dec_arg)
{

	AUDIO_ARGUMENT temp;
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));

	if (dec_arg->Main_Channel != 255)
	{
		if(audio_decode_status(temp) == AUDIO_CODEC_PROCESSING)
		{
			DBG_PRINT("audio_decode_stop..");
			audio_decode_stop(temp);
			DBG_PRINT("stop decoding!!!\r\n");
		}
		
		dec_arg->Main_Channel = 255;
	}

	if(audio_encode_status() == AUDIO_CODEC_PROCESSING)
	{
		DBG_PRINT("audio_encode_stop..");
		audio_encode_stop();
#if USE_DISK == FS_NAND1	
		DBG_PRINT("Nand flush\r\n");
		DrvNand_flush_allblk();
#endif
		DBG_PRINT("stop encoding!!!\r\n");
	}
}

void Audio_Encode_Demo(void)
{
	char    enc_dir[24];
	char    enc_file[24];
	char    enc_path[64];
	
	INT32S  enc_num = 0;
	INT32S  file_nums = 0;
	INT32U  enc_type = 0;	
	INT32U  cur_state = 0; // 0:encoding 1:decoding
	INT64U  disk_free;	
	
	AUDIO_ARGUMENT 	audio_argument;
	MEDIA_SOURCE	media_source;

	INT32S ret;
	
	filelist *temp = NULL;
	
	audio_argument.Main_Channel = 255;

	while(1)
	{	
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
		#if	USE_DISK == FS_NAND1
			ret = _format(FS_NAND1, FAT32_Type); 
			DrvNand_flush_allblk();
			_deviceunmount(FS_NAND1);
		#endif			
		}
		else
		{
			DBG_PRINT("Mount DISK Success[%d]!!!\r\n", USE_DISK);
			disk_free = vfsFreeSpace(USE_DISK);
			DBG_PRINT("Disk Free Size = %d\r\n", disk_free);
			break;
		}
	}
	
	switch(USE_DISK)
	{
		case FS_SD:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"C:\\AUDIO_ENCODE\\", sizeof(enc_dir));
			break;
		case FS_NAND1:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"A:\\AUDIO_ENCODE\\", sizeof(enc_dir));
			break;	
		case FS_USBH:	
			gp_memcpy((INT8S*)enc_dir, (INT8S*)"G:\\AUDIO_ENCODE\\", sizeof(enc_dir));
			break;		
	}
	
	
#if USE_ADKEY == 1		
	adc_key_scan_init();
#endif

	audio_encode_entrance();
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	
	
	
	if (chdir (enc_dir) == -1)	{	// return value -1 means error occurred
		DBG_PRINT("change current directory failed\r\n");
		if (mkdir("AUDIO_ENCODE") == -1)	{
			DBG_PRINT("can not create directory AUDIO_ENCODE\r\n");
			goto FUNC_END;
		}	
		else	{
			DBG_PRINT("directory AUDIO created\r\n");
			chdir (enc_dir);
		}
	}	
	else	{
		DBG_PRINT ("change current directory successful\r\n");
	}
	
	// --------------- Description of Keys' Function --------------- //
	DBG_PRINT("GPL32XXX AUDIO EECODE DEMO\r\n");
	DBG_PRINT("Media : SDC/NAND/USBH (default: SDC)\r\n");
	DBG_PRINT("Key 1 : encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)\r\n");
	DBG_PRINT("Key 2 : start encoding (file_num ++)\r\n");
	DBG_PRINT("Key 3 : stop encoding/decoding\r\n");
	DBG_PRINT("Key 4 : reserved\r\n");
	DBG_PRINT("Key 5 : reserved\r\n");
	DBG_PRINT("Key 6 : play encoded file\r\n");
	DBG_PRINT("Key 7 : play previous encoded file\r\n");
	DBG_PRINT("Key 8 : stop encoding/decoding and exit audio encode/decode tasks\r\n");
	// ------------------------------------------------------------- //

	sprintf((char *)enc_file, "%s", "aud");
	file_nums = audio_filelist("aud*");

	if (file_nums > 0)
		enc_num = file_nums;
	
	if (list_head != NULL)
		temp = list_head;	
	
	while(1)
	{
		OSTimeDly(3);
	#if USE_ADKEY == 1	
		adc_key_scan();
	#else
		key_Scan();
	#endif
		

	#if USE_ADKEY == 1	
		if(ADKEY_IO1)
	#else
		if(IO1)
	#endif
		{
			// encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)
			enc_type++;
			if(enc_type > 4)
				enc_type = 0;
			
			switch(enc_type)
			{
				case 0:
					DBG_PRINT("encode type:A18\r\n");
					break;
				case 1:
					DBG_PRINT("encode type:WAV\r\n");
					break;
				case 2:
					DBG_PRINT("encode type:MS_ADPCM\r\n");
					break;
				case 3:
					DBG_PRINT("encode type:IMA_ADPCM\r\n");
					break;
				case 4:
					DBG_PRINT("encode type:MP3\r\n");
					break;
			}
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO2)
	#else
		else if(IO2)
	#endif
		{
			// start encoding (file_num ++)		
			if(audio_encode_status() == AUDIO_CODEC_PROCESS_END)
			{
				media_source.type = SOURCE_TYPE_FS; 
				//media_source.type = SOURCE_TYPE_USER_DEFINE;
				if(media_source.type_ID.FileHandle < 0)
				{
					DBG_PRINT("OPEN FILE FAIL\r\n");
					continue;
				}
							
				switch(enc_type)
				{
					case 0: // a18
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "a18");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = A1800;
						//audio_encode_set_downsample(TRUE, 2);
						//audio_encode_start(media_source, 16000, 32000);
						ret = audio_encode_start(media_source, 16000, 0); // a18 only one channel
						break;	
					case 1:	// wav
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = WAV;
						//audio_encode_set_downsample(TRUE, 2);
						ret = audio_encode_start(media_source, 44100, 0);
						
						break;	
					case 2:	// ms_adpcm
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = MICROSOFT_ADPCM;
						//audio_encode_set_downsample(TRUE, 4);
						ret = audio_encode_start(media_source, 16000, 0);
						break;
					case 3:	// ima_adpcm
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "wav");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = IMA_ADPCM;
						//audio_encode_start(media_source, 16000, 0);
						ret = audio_encode_start(media_source, 44100, 0);
						break;
					case 4:	// mp3
						sprintf((char *)enc_path, "%s%03d.%s", enc_file, enc_num++, "mp3");
						media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
						media_source.Format.AudioFormat = MP3;
						//audio_encode_set_downsample(TRUE, 2);
						//ret = audio_encode_start(media_source, 44100, 128000);
						ret = audio_encode_start(media_source, 22050, 128000);
						break;
						
						
				}
				
				if (ret != 0)
				{
					DBG_PRINT("encode %s start error... \r\n", enc_path);
					continue;
				}
				
				cur_state = 0;
				DBG_PRINT("now encoding %s ... \r\n", enc_path);
	
				if (list_head == NULL)
					list_create(enc_path);
				else	
					list_insert(enc_path);
	
				if (list_head != NULL)
					temp = list_head;
				
			}			
			
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO3)
	#else
		else if(IO3)
	#endif
		{	
			// stop encoding/decoding
			audio_stop_processing(&audio_argument);
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO4)
	#else
		else if(IO4)
	#endif
		{
			// reserved									
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO5)
	#else
		else if(IO5)
	#endif
		{
			// reserved			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO6)
	#else
		else if(IO6)
	#endif
		{
			// play encoded file
		
			audio_stop_processing(&audio_argument);			
			
			if (list_head == NULL)
			{
				DBG_PRINT("error:no audio files !!!\r\n");
				continue;
			}
			
			if (cur_state == 0)
			{
				temp = temp->prev;
			}
			else
			{
				temp = temp->next;
				if(temp == list_head)
					temp = temp->next;
			}
			
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63

			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", temp->data);
			audio_encode_playback((char*)(temp->data), &audio_argument);
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO7)
	#else
		else if(IO7)
	#endif
		{
			// play previous encoded file
			audio_stop_processing(&audio_argument);
		
			if (list_head == NULL)
			{
				DBG_PRINT("error:no audio files !!!\r\n");
				continue;
			}
			
			temp = temp->prev;
			if(temp == list_head)
				temp = temp->prev;
						
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63
			
			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", temp->data);
			audio_encode_playback((char*)(temp->data), &audio_argument);
			
		}
	#if USE_ADKEY == 1		
		else if(ADKEY_IO8)
	#else
		else if(IO8)
	#endif
		{
			// stop encoding/decoding and exit audio encode/decode tasks			
			audio_stop_processing(&audio_argument);
			break;
		}
			
	}

	
FUNC_END:	
	
	audio_encode_exit();
	audio_decode_exit();	// Release the resources needed by audio decoder	
	
	DBG_PRINT("exit audio encode demo !!!\r\n");
}