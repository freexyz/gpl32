#ifndef __AUDIO_RECORD_H__
#define __AUDIO_RECORD_H__

#include "application.h"
//=============================================================
// audio encode configure set 
//=============================================================
#if	(defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
#define	ADC_LINE_IN						0
#define	GPY0050_IN						1
#define	BUILD_IN_MIC					2  
#define	DOUBLE_LINEIN					3  
#define AUDIO_INPUT_SRC					BUILD_IN_MIC
#else
#define	BUILD_IN_MIC					0xFF					//no built-in MIC for GPL32_A/B/C  	
#define	DOUBLE_LINEIN					0xFE  
#define ADC_LINE_IN						0
#define	GPY0050_IN						1
#define AUDIO_INPUT_SRC					ADC_LINE_IN
#endif

#if AUDIO_INPUT_SRC == ADC_LINE_IN || AUDIO_INPUT_SRC == BUILD_IN_MIC || AUDIO_INPUT_SRC == DOUBLE_LINEIN
	#define C_ADC_USE_TIMER				ADC_AS_TIMER_C			//adc:ADC_AS_TIMER_C ~ F, mic: ADC_AS_TIMER_C ~ F
	#define C_ADC_USE_CHANNEL			ADC_LINE_1				//adc channel: 0 ~ 3 
#elif AUDIO_INPUT_SRC == GPY0050_IN
	#define C_AUDIO_RECORD_TIMER		TIMER_B		 			//timer, A,B,C
	#define C_GPY0050_SPI_CS_PIN		IO_F5					//gpy0500 spi interface cs pin
#endif

#if AUDIO_INPUT_SRC == DOUBLE_LINEIN
	#define C_AUD_PCM_BUFFER_NO				6						//pcm buffer number
#else
	#define C_AUD_PCM_BUFFER_NO				3						//pcm buffer number
#endif	

#define C_A1800_RECORD_BUFFER_SIZE		A18_ENC_FRAMESIZE		//PCM buffer size, fix 320
#define C_WAVE_RECORD_BUFFER_SIZE		1024*16					//PCM buffer size, depend on SR=16KHz 	
#define C_BS_BUFFER_SIZE				1024*64					//file buffer size, fix 64Kbyte 	

#define A1800_TIMES						30						//a1800 encode times, 320*30, <80*30, depend on SR=16KHz
#define ADPCM_TIMES						16						//adpcm encode times, 500*16, 256*16, depend on SR=16KHz
#define MP3_TIME						15						//MP3 encode times, 1152*15

// pcm energy detect threshold 
#define PCM_GLOBAL_THR 					0x800					//Frame Energy Threshold to judge as active							
#define PCM_LOCAL_THR  					0x800 					//Local Energy Threshold of each sample  

//=============================================================
// audio encode status 
//=============================================================
#define C_GP_FS				0
#define C_USER_DEFINE		1

#define C_MONO_RECORD		1
#define C_STEREO_RECORD 	2

#if DBG_MESSAGE == 1
	#define DEBUG_MSG(x)	{x;}
#else
	#define DEBUG_MSG(x)	{}
#endif

#define C_STOP_RECORD		0x00000000
#define C_STOP_RECORDING	0x00000001
#define C_START_RECORD		0x00000002
#define C_START_FAIL		0x80000001

#define AUD_RECORD_STATUS_OK			0x00000000
#define AUD_RECORD_STATUS_ERR			0x80000001
#define AUD_RECORD_INIT_ERR				0x80000002
#define AUD_RECORD_DMA_ERR				0x80000003
#define AUD_RECORD_RUN_ERR				0x80000004
#define AUD_RECORD_FILE_WRITE_ERR		0x80000005
#define AUD_RECORD_MEMORY_ALLOC_ERR		0x80000006

//gpy0500 command
#define C_CMD_RESET_IN1				0x83
#define C_CMD_RESET_IN4				0x89		
#define C_CMD_ENABLE_ADC			0x98
#define C_CMD_ENABLE_MIC_AGC_ADC	0x9B
#define C_CMD_ENABLE_MIC_ADC		0x9B
#define C_CMD_ENABLE_MIC_AGC		0x93
#define C_CMD_DUMMY_COM				0xC0
#define C_CMD_ADC_IN1				0x82
#define C_CMD_ADC_IN4				0x88
#define C_CMD_ZERO_COM				0x00
#define C_CMD_POWER_DOWN			0x90
#define C_CMD_TEST_MODE				0xF0

typedef enum
{
	MSG_ADC_DMA_DONE = C_DMA_STATUS_DONE,
	MSG_AUDIO_ENCODE_START = 0x10000000,
	MSG_AUDIO_ENCODE_STOPING,
	MSG_AUDIO_ENCODE_STOP,
	MSG_AUDIO_ENCODE_ERR
}AUDIO_RECORD_ENUM;

typedef struct 
{
	INT8U	RIFF_ID[4];	//= {'R','I','F','F'};
	INT32U 	RIFF_len;	//file size -8
	INT8U 	type_ID[4];	// = {'W','A','V','E'};
	INT8U	fmt_ID[4];	// = {'f', 'm','t',' '};
	INT32U  fmt_len;	//16 + extern format byte
	INT16U  format;		// = 1; 	//pcm
	INT16U	channel;	// = 1;	// mono
	INT32U  sample_rate;// = 8000;
	INT32U  avg_byte_per_sec;// = 8000*2;	//AvgBytesPerSec = SampleRate * BlockAlign 
	INT16U	Block_align;// = (16 / 8*1) ;				//BlockAlign = SignificantBitsPerSample / 8 * NumChannels 
	INT16U	Sign_bit_per_sample;// = 16;		//8, 16, 24 or 32
	INT8U	data_ID[4];// = {'d','a','t','a'};
	INT32U	data_len; //extern format byte
}AUD_ENC_WAVE_HEADER;

typedef struct
{
	INT32U  Status;				
	INT32U  SourceType;
	INT16S  FileHandle;
	INT16U	Channel;			//1,mono or 2,stereo
	INT32U  AudioFormat;
	INT32U  SampleRate;			//sample rate
	INT32U  BitRate;            //bite rate
	INT32U  FileLenth;			//byte 
	INT32U  NumSamples;			//sample

#if	APP_DOWN_SAMPLE_EN	
	INT8U   bEnableDownSample;
	INT8U 	*DownSampleWorkMem;
	INT8U   DownsampleFactor;
#endif
	
	INT8U 	*EncodeWorkMem;		//wrok memory
	INT8U   *Bit_Stream_Buffer;	//encode bit stream buffer
	INT32U  read_index;			//bit stream buffer index
	INT32U	PackSize;			//for file write size, byte
	INT8U   *pack_buffer;		//a1800 and wav lib use
	INT32U	PCMInFrameSize;		//pcm input buffer, short
	INT32U  OnePCMFrameSize; 	//short
	
	//allow record size
	INT64U  disk_free_size;
}Audio_Encode_Para;

//task api
INT32S adc_record_task_create(INT8U priority);
INT32S adc_record_task_del(INT8U priority);
void adc_record_entry(void *param);

//api
void audio_record_set_status(INT32U status);
INT32U audio_record_get_status(void);
void audio_record_set_source_type(INT32U type);
INT32U audio_record_get_source_type(void);
INT64U audio_record_set_file_handle_free_size(INT16S file_handle);
void audio_record_set_info(INT32U audio_format, INT32U sample_rate, INT32U bit_rate, INT16U channel);
INT32S audio_record_set_down_sample(BOOLEAN b_down_sample, INT8U ratio);
INT32S pcm_energy_detect(INT16S* buffer_addr, INT32U pcm_size);

#endif
