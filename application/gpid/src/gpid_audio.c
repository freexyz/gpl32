#include "application.h"
#include "gpid_audio.h"
#include "res_bin_file_mgr.h"

extern BOOLEAN FirstPlaySoundFlag;

// Static variables
static AUDIO_ARGUMENT gAudioArg;
static MEDIA_SOURCE	gMediaSource;
static INT32U gCurrResIdx;	// resource index inside a res bin file

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_init(INT16S fd)
{
	gAudioArg.Main_Channel = 1;		// Use DAC Channel A+B
	gAudioArg.L_R_Channel = 3;		// Left + Right Channel
	gAudioArg.mute = 0;
	gAudioArg.volume = 63;			// volume level = 0~63
	gAudioArg.midi_index = 0;		// midi use

	gMediaSource.type = SOURCE_TYPE_FS_RESOURCE_IN_FILE;	// audio in a .bin file
	gMediaSource.type_ID.FileHandle = fd;
	gMediaSource.Format.AudioFormat = A1800;
	
	gCurrResIdx = 0;
}

//=============================================================================
// Note:
//=============================================================================
INT8U
gpid_audio_get_status(void)
{
	AUDIO_CODEC_STATUS status;
	INT8U ret;

	status = audio_decode_status(gAudioArg);
	switch(status)
	{
	case AUDIO_CODEC_PROCESSING:
		ret = C_GPID_AUDIO_STATUS_PLAY;
		break;

	case AUDIO_CODEC_PROCESS_PAUSED:
		ret = C_GPID_AUDIO_STATUS_PAUSE;
		break;

	default:
		ret = C_GPID_AUDIO_STATUS_STOP;
		break;
	}

	return ret;
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_play(void)
{
	INT16U buffer[4];
	RBF_Res_T res;

	if (FirstPlaySoundFlag==FALSE)				//When player play sound first, this flag is TRUE
		FirstPlaySoundFlag = TRUE;

	switch(gpid_audio_get_status())
	{
	case C_GPID_AUDIO_STATUS_PAUSE:
		gpid_audio_resume();
		break;

	case C_GPID_AUDIO_STATUS_STOP:
		if (RBF_GetRes(RBF_GetCurrRcIdx(), 0, &res) != C_RBF_RC_SUCCESS)
			break;

		if ( gpid_audio_get_audio_format() == A1800 )
		{
			// Check if the beginning of the resource (buf[0:3]) is "GENERA".
			// If yes, the resource contains a 48-byte header and must be skipped.
			RBF_Read(res.offset+4, buffer, 8);
			if ((buffer[0] == 0x4547) && (buffer[1] == 0x454E) &&
				(buffer[2] == 0x4152) && (buffer[3] == 0x504C))
				lseek(RBF_GetFd(), res.offset + 48, SEEK_SET);
			else
				lseek(RBF_GetFd(), res.offset, SEEK_SET);
		}
		else if ( gpid_audio_get_audio_format() == MP3 )
		{
			gAudioArg.data_offset = res.offset;
			lseek(RBF_GetFd(), res.offset, SEEK_SET);
		}

		gAudioArg.data_len = res.len;
		audio_decode_start(gAudioArg, gMediaSource);
		break;

	default:
		break;
	}
}

#if C_RBF_SUPPORT_RBF_V1
//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_ext_res_play(unsigned long index)
{
	INT16U buffer[4];
	RBF_Res_T res;

	switch(gpid_audio_get_status())
	{
	case C_GPID_AUDIO_STATUS_STOP:
		if (RBF_GetExtRes(index, &res) != C_RBF_RC_SUCCESS)
			break;

		if ( gpid_audio_get_audio_format() == A1800 )
		{
			// Check if the beginning of the resource (buf[0:3]) is "GENERA".
			// If yes, the resource contains a 48-byte header and must be skipped.
			RBF_Read(res.offset+4, buffer, 8);
			if ((buffer[0] == 0x4547) && (buffer[1] == 0x454E) &&
				(buffer[2] == 0x4152) && (buffer[3] == 0x504C))
				lseek(RBF_GetFd(), res.offset + 48, SEEK_SET);
			else
				lseek(RBF_GetFd(), res.offset, SEEK_SET);
		}
		else if ( gpid_audio_get_audio_format() == MP3 )
		{
			gAudioArg.data_offset = res.offset;
			lseek(RBF_GetFd(), res.offset, SEEK_SET);
		}

		gAudioArg.data_len = res.len;
		audio_decode_start(gAudioArg, gMediaSource);
		break;

	default:
		break;
	}
}
#endif

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_stop(void)
{
	audio_decode_stop(gAudioArg);
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_pause(void)
{
	audio_decode_pause(gAudioArg);
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_resume(void)
{
	audio_decode_resume(gAudioArg);
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_set_audio_format(AUDIO_FORMAT format)
{
	// Only A1800 and MP3 are supported now
	if (format == A1800 || format == MP3)
		gMediaSource.Format.AudioFormat = format;
}

//=============================================================================
// Note:
//=============================================================================
AUDIO_FORMAT
gpid_audio_get_audio_format(void)
{
	return gMediaSource.Format.AudioFormat;
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_set_volume(INT32U volume)
{
	if (volume > 63)
		volume = 63;
	
	gAudioArg.volume = volume;
	audio_decode_volume_set(&gAudioArg, volume);	
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_volume_up(void)
{
	INT32U volume;

	volume = gAudioArg.volume + 4;
	if(volume > 63)
		volume = 63;
	
	gAudioArg.volume = volume;
	audio_decode_volume_set(&gAudioArg, volume);	
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_volume_down(void)
{
	INT32S volume;

	volume = gAudioArg.volume;
	if(volume == 63)
		volume -= 3;
	else
		volume -= 4;
		
	if(volume <= 0)
		volume = 0;
	
	gAudioArg.volume = (INT32U)volume;
	audio_decode_volume_set(&gAudioArg, (INT32U)volume);	
}

//=============================================================================
// Note:
//=============================================================================
BOOLEAN
gpid_audio_is_mute(void)
{
	return gAudioArg.mute;
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_mute(void)
{
	if (!gAudioArg.mute)
		audio_decode_mute(&gAudioArg);
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_unmute(void)
{
	if (gAudioArg.mute)
		audio_decode_unmute(&gAudioArg);
}

//=============================================================================
// Note:
//=============================================================================
void
gpid_audio_to_n(INT32U idx)
{
	//gCurrResIdx = idx;
	RBF_SetCurrRcIdx(idx);
}