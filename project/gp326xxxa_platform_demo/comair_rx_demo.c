#include <stdio.h>
#include <string.h>

#include "application.h"
#include "drv_l2_ad_key_scan.h"
#include "GP17lib_ComAir_RxDecProc.h"


#define USE_DISK	FS_SD
#define USE_ADKEY	1

extern void audio_data_write_callback_register(INT32S (*callback_function)(INT8U bHeader, INT32U buffer_addr, INT32U cbLen));

//=================================================================================================
//	Audio Encode demo code
//=================================================================================================
INT32S audio_data_process(INT8U bHeader, INT32U buffer_addr, INT32U cbLen)
{
	INT32U ChkCmd;
	INT32U option_GainCtrl;
	INT32U FrameSize;
	INT16U *micdata;
	INT32U i;
	
	FrameSize = 64;
	option_GainCtrl = 300;

	micdata = (INT16U *)buffer_addr;
	
	for(i=0; i<cbLen/128; i++) {
		ChkCmd = ComAir_DecFrameProc((short *)(micdata+i*FrameSize),option_GainCtrl);
		if(ChkCmd!=-1)
			DBG_PRINT("0x%x \r\n", ChkCmd);
	}
	return 0;
}

void Comair_RX_Demo(void)
{
	MEDIA_SOURCE	media_source;
	int ret;
	INT32S option_mode,option_thr,option_Decision,ComAir_Mode;
	INT16U PinCode;

	option_mode = 0x13;
	option_thr = 64;
	option_Decision = 0;
	PinCode = 0x0000;
	ComAir_Mode = option_mode;
	
	ComAir_DecFrameInit(17500,562,ComAir_Mode,option_thr,PinCode,option_Decision);
	
	audio_data_write_callback_register(audio_data_process);

	audio_encode_entrance();
	audio_decode_entrance();

	while(1)
	{
		OSTimeDly(3);
		
		if(audio_encode_status() == AUDIO_CODEC_PROCESS_END)
		{
			media_source.type = SOURCE_TYPE_USER_DEFINE;
			media_source.Format.AudioFormat = WAV;
			ret = audio_encode_start(media_source, 48000, 0);
		}
	}
}
