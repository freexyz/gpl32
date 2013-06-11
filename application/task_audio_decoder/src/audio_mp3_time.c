#include "ap_music.h"

#if (defined SUPPORT_AI_FM_MODE) && (SUPPORT_AI_FM_MODE == CUSTOM_ON)

#if 1  // defined in mp3 library
const short bitrate_tbl[2][3][16] = {//<< bitrate table under diff lsf/layer >>
	//LSF = 0
	{ {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
	{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
	{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },
	//LSF = 1
	{ {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};
const int mp3_samplerate[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };

const short DIV144_Fs[9] = {3343, 3072, 4608, 6687, 6144, 9216, 13374, 12288, 18432};
#endif

#define AVE_FRAME_CNT 100
INT16S f_mp3;

const short mp3_sample_per_frame_tbl[2][3] = 
{
// layer 1,		   2,	   3
	{    384,	1152,   1152},  // MPEG 1
	{	 384,	1152,	 576},	// MPEG 2 & 2.5
};

double sec_per_frame = 0.0;

INT32S get_1byte(INT8U *byte)
{
	if(read(f_mp3,(INT32U)byte, 1) != 1)
	{
		//unsigned __int64 pre_pos = 0;
		//fgetpos(f_mp3, &pre_pos);
		DBG_PRINT("\nFile End position\r\n");
		return 0;
	}

	return 1;
}


//  1:  get mp3 sync word
// -1:  file end & can't get mp3 sync word
INT32S mp3_file_parsing(void)
{
	INT8U byte0, byte1, byte2, byte3;
	//unsigned char buffer[1024];
	//INT32S  ret = -1;
	INT32U  pre_samplerate = 0;
	INT32U  pre_channel = 0;
	INT32U  pre_layer = 0;	
	INT32U  lsf, mpeg25;
	INT32U  index, bitrate_index;
	INT32U  sampling_frequency, padding, framesize;
	INT32U  pre_pos = 0;
	INT32U  layer, samplerate, channel;
    INT32U  lTemp;
    INT32U  err_cnt = 0;
	// initial mp3 file information
	layer = 0;
	samplerate = 0;
	channel = 0;

	if(get_1byte(&byte0) != 1)	return -1;
	if(get_1byte(&byte1) != 1)	return -1;

	while(1)
	{
		if(!((byte0 == 0xff) && ((byte1 & 0xe0) == 0xe0)))
		{
			byte0 = byte1;
			if(get_1byte(&byte1) != 1)	return -1;
			if (++err_cnt > 100000) {
			    return -1;
			}
			continue;
		}
		else
		{
		    err_cnt = 0;
			if(get_1byte(&byte2) != 1)	return -1;
			if(get_1byte(&byte3) != 1)	return -1;

			//mpegid
			if (byte1 & 0x10){
				lsf = (byte1 & 0x8) ? 0x0 : 0x1;
				mpeg25 = 0;
			}
			else{
				lsf = 1;
				mpeg25 = 1;				
				if(byte1 & 0x08) 
					goto label1;
			}

			//layer description
			index = (byte1 & 0x06) >> 1;
			if(index == 0x00){
				goto label1;
			}
			else {			
				layer = 4 - index;
			}

			if(layer == 1)
			{
				if(pre_channel == 0)
					goto label1;
				else
					goto label2;
			}
			
			//biterate
			index = (byte2 & 0xF0)>>4;			
			if(index == 0x0F){
				goto label1;
			}
			else{
				bitrate_index = index;
				//BitRate = bitrate_tbl[lsf][layer-1][index];
				//BitRate *= 1000;
			}
			
			//sample rate
			index = (byte2 & 0x0C)>>2;	
			if(index == 0x03){
			    goto label1;			    
			}
			else
			{
				if(mpeg25)
					sampling_frequency = 6 + index;				
				else
					sampling_frequency = index + (lsf*3);


				samplerate =  mp3_samplerate[index];
			
				index = byte1 & 0x18;
				if(index == 0x00)						//MPEG2.5
					samplerate = samplerate >> 2;
				else if(index == 0x10)					//MPEG2
					samplerate = samplerate >> 1;
					
				//SampleRate = samplerate;	
			}

			//channel mode
			// modified by Comi 20080314
			//index = 3 - ((byte3 & 0xC0) >> 6);  
			index = (byte3 >> 6) & 0x03;
			channel = (index == 0x03 ? 1 : 2);

			if(pre_samplerate == 0 || pre_channel == 0 || pre_layer == 0)
			{
				padding = (byte2 >> 1) & 0x01;

				if(bitrate_index != 0) 
				{ // Not free format
					if(layer == 3)
					{
						if(bitrate_index != 0) { // Not free format
							
							lTemp = bitrate_tbl[lsf][2][bitrate_index] * DIV144_Fs[sampling_frequency];
							framesize = (short)(lTemp>>10);
							framesize >>= lsf;
							framesize = framesize + padding;
						}
					}
					else if(layer == 2)
					{
						if(bitrate_index != 0) { // Not free format			
							lTemp = bitrate_tbl[lsf][1][bitrate_index] * DIV144_Fs[sampling_frequency];
							framesize = (short)(lTemp>>10);
							framesize = framesize + padding;
						}
					}
				}
				else
				{
					DBG_PRINT("Free format header.\r\n");
					return -1;
				}				

				// pass bitstream 
				if(lseek(f_mp3, (framesize - 4), SEEK_CUR) < 0)
					return -1;

				if(get_1byte(&byte0) != 1)	return -1;
				if(get_1byte(&byte1) != 1)	return -1;
				
				// check next header...
				if ((byte0 == 0xff) && ((byte1 & 0xe0) == 0xe0))
				{
					pre_samplerate = samplerate;
					pre_channel = channel;
					pre_layer = layer;
					pre_pos = lseek(f_mp3, 0, SEEK_CUR);
					//fgetpos(f_mp3, &pre_pos);
					pre_pos -= (framesize + 2);
				}
				else
				{
label2:
					if(pre_pos == 0)
					{
						continue;
					}					
					pre_samplerate = 0;
					pre_channel = 0;
					pre_layer = 0;
					lseek(f_mp3, pre_pos, SEEK_SET);					
				}
				continue;
			}
			else
			{
				if(pre_samplerate != samplerate || pre_channel != channel || pre_layer != layer)
				{
					pre_samplerate = 0;
					pre_channel = 0;
					pre_layer = 0;
					lseek(f_mp3, pre_pos, SEEK_SET);
					continue;
				}
			}

			sec_per_frame = (double)mp3_sample_per_frame_tbl[lsf][layer-1] / (double)samplerate;

			lseek(f_mp3, pre_pos, SEEK_SET);
			break;

label1:
			byte0 = byte1;
			byte1 = byte2;
			if(lseek(f_mp3, -1, SEEK_CUR) < 0)
				return -1;
		}
	}

	return 1;
}

INT32U mp3_get_time(INT32U len)
{
	unsigned char byte0, byte1, byte2, byte3;	
	INT32U time_in_sec;
    INT32U layer, lsf, mpeg25;
	INT32U sampling_frequency, padding, index, bitrate_index, framesize;
	INT32U lTemp;
	INT32U sum_framesize = 0;
	INT32U frame_cnt = 0;
	
    lTemp = (len/3) & ~0x3;
	lseek(f_mp3, lTemp, SEEK_CUR);
	
	time_in_sec = 0;
    
	if(get_1byte(&byte0) != 1)	return time_in_sec;
	if(get_1byte(&byte1) != 1)	return time_in_sec;
	
	while(1)
	{
		if(((byte0 == 0xff) && ((byte1 & 0xe0) == 0xe0)))
		{	// get sync word
			if(get_1byte(&byte2) != 1)	break;
			if(get_1byte(&byte3) != 1)	break;

			//mpegid
			if (byte1 & 0x10)
			{
				lsf = (byte1 & 0x8) ? 0x0 : 0x1;
				mpeg25 = 0;
			}	
			else
			{
				mpeg25 = 1;
				lsf = 1;
			}

			//layer description
			index = (byte1 & 0x06) >> 1;
			layer = 4 - index;

			//biterate
			index = (byte2 & 0xF0)>>4;			
			bitrate_index = index;

			index = (byte2 & 0x0C)>>2;
			if(mpeg25)
				sampling_frequency = 6 + index;				
			else
				sampling_frequency = index + (lsf*3);

			padding = (byte2 >> 1) & 0x01;
			
			if (lsf > 1 || bitrate_index > 15 ) {
			    framesize = 0;
			}
			else { 
			    if(layer == 3)
			    {
			    	lTemp = (long) bitrate_tbl[lsf][2][bitrate_index] * DIV144_Fs[sampling_frequency];
			    	framesize = (short)(lTemp>>10);
			    	framesize >>= lsf;
			    	framesize = framesize + padding;
			    }
			    else if(layer == 2)
			    {		
			    	lTemp = (long) bitrate_tbl[lsf][1][bitrate_index] * DIV144_Fs[sampling_frequency];
			    	framesize = (short)(lTemp>>10);
			    	framesize = framesize + padding;
			    }
			    else {
			        framesize = 0;
			    }
			}
			
            sum_framesize += framesize;
            
            if (framesize > 4) {
		        frame_cnt++;
		        if(lseek(f_mp3, (framesize - 4), SEEK_CUR) < 0)
			    { 
				    break;
			    }
			}
			
			if (frame_cnt >= AVE_FRAME_CNT) {
                break;
            }
            
			if(get_1byte(&byte0) != 1)	return time_in_sec;
			if(get_1byte(&byte1) != 1)	return time_in_sec;
		}
		else
		{	// sync word error
			byte0 = byte1;
			if(get_1byte(&byte1) != 1)	return time_in_sec;			
		}
	}
	
	if (!frame_cnt || !sum_framesize) {
	    return 0;
	}
	
	framesize = sum_framesize/frame_cnt;
	lTemp = len / framesize;
	
	time_in_sec = (INT32U)((double)lTemp* sec_per_frame);
	
	return time_in_sec;
}

INT32S mp3_get_duration(INT16S fd, INT32U len)
{	
	
	INT32S ret;
	INT32U time_in_sec;
	INT32U pos;
	
    f_mp3 = fd;
	
	pos = lseek(f_mp3, 0, SEEK_CUR);
	ret = mp3_file_parsing();
	if(ret != 1)
	{
		DBG_PRINT("can't get mp3 sync word!!!\r\n");
		lseek(f_mp3, pos, SEEK_SET);
		return 0;
	}
       
	time_in_sec = 0;
	time_in_sec = mp3_get_time(len);
	
	lseek(f_mp3, pos, SEEK_SET);
	
	//DBG_PRINT("Total Frame = %d\r\n", frame_cnt);
	DBG_PRINT("Total time = %d sec\r\n", time_in_sec);

	return time_in_sec;
}

#endif