#ifndef __MCU_SPU_MIDI_DRIVER_H__
#define __MCU_SPU_MIDI_DRIVER_H__

#include "gplib.h"

typedef struct{
	INT32U R_MIDI_CH_PAN;
	INT32U R_MIDI_CH_VOLUME;
	INT32U R_MIDI_CH_EXPRESSION;
	INT32U R_MIDI_CH_PitchBend;
	INT32U R_CH_SMFTrack;			// Record 16 MIDI channel's instrument
	INT32U R_ChannelInst;			// Instrument Index mapping of Logical Channel in MIDI file
	INT32U *R_PB_TABLE_Addr;			//PitchBend Table Address
	INT32U R_RPN_ReceiveFlag;		//RPN Receive Flag
	INT32U R_RPN_DATA;				//MIDI CH RPN Value
	
}MIDI_ChannelInfoStruct;

typedef struct{
	INT32U R_NOTE_PITCH;//081105
	INT32U R_NOTE_VELOCITY;
	INT32U R_MIDI_ToneLib_PAN;
	INT32U R_MIDI_ToneLib_Volume;
	INT32U R_MIDI_CH_MAP;
	INT32U R_NoteOnHist;			// Log the NoteOn mapping channel to a Circular Queue
	INT32U R_PB_PhaseRecord;		//Original Channel Phase Value
}SPU_ChannelInfoStruct;

typedef struct{
	INT32U ChannelNumber;
	INT32U Pitch;
	INT32U Velocity;
	INT32U Duration;
}NoteEventStruct;

typedef struct{
	INT32U BeatCountValue;
}BeatCountEventStruct;

typedef struct{
	INT32U ChannelNumber;
	INT32U PitchBendValue;
}PitchBendEventStruct;

typedef struct{
	INT32U ChannelNumber;
	INT32U ControlNumber;
	INT32U ControlValue;
}ControlEventStruct;

typedef struct{
	INT32U ChannelNumber;
	INT32U InstrumentIndex;
}ProgramChangeEventStruct;

typedef struct{
	INT32U TempoValue;
}TempoEventStruct;

typedef struct{
	INT32U ChannelNumber;
	INT32U LyricWordCount;
	INT32U Duration;
}LyricEventStruct;

typedef struct{
	INT32U TextType;
	INT32U LyricWordCount;
}TextEventStruct;

typedef union{
	NoteEventStruct NoteEvent;
	BeatCountEventStruct BeatCountEvent;
	PitchBendEventStruct PitchBendEvent;
	ControlEventStruct ControlEvent;
	ProgramChangeEventStruct ProgramChangeEvent;
	TempoEventStruct TempoEvent;
	LyricEventStruct LyricEvent;
	TextEventStruct TextEvent;
}MIDI_EventStruct;

typedef struct{
	INT32U ALP_ID[4];
	INT32U SampleRate;
	INT32U SampleLength;
	INT32U LoopStartAddr;
	INT32U EnvelopStartAddr;
	INT32U WaveType;
	INT32U BasePitch;
	INT32U MaxPitch;
	INT32U MinPitch;
	INT32U RampDownClock;
	INT32U RampDownStep;
	INT32U Pan;
	INT32U Velocity;
}ALP_Header_Struct;

#define C_SPU_PW_FIQ				1
#define C_SPU_BEAT_FIQ				2
#define C_SPU_ENV_FIQ				3
#define C_SPU_FIQ					4
#define C_MAX_FIQ					5				// Don't forget to modify this when new fast interrupt source is added

#define B_SPU_PW_FIQ				0x01
#define B_SPU_BEAT_FIQ				0x02
#define B_SPU_ENV_FIQ				0x04
#define B_SPU_FIQ					0x08

#define D_EventCH					40
#define D_ChOffset 					0x0020

#define	PB_8bit						0
#define PB_16bit					1
#define PB_floating					0

#define D_TotalCH					32	

#define	D_MidiCH					16
#define	D_NoNote					33				// Used to mask note add by porter 7A04
#define D_ServiceRate				281250			// G200/F32 Realchip SPU32 Service rate; 27MHz/6/32
#define D_BeatBaseCnt				352		
// Tonemaker system need 5 ms as BEAT COUNT interval 
// BeatCnt = 1/D_ServiceRate * BeatBaseCount * 4 , where BeatCnt == 5 ms then BeatBaseCnt = D_ServiceRate*BeatCnt/4
// BeatBaseCnt = 281.250*5/4 = 351.5625
// Beat Count is triggered every 4 hits of Beat base count

#define D_PLLCLK 					3579545

//#define  D_BASEPHASE   			0x3BA			// 2^(PhaseBits=19)*256/D_ServiceRate(140625 FPGA Mode)
#define D_BASEPHASE					0x1DD			// 2^(PhaseBits=19)*256/D_ServiceRate(281250 RealChip Mode)

#define D_DAC_Volume				0x7F			// Realchip : 0x7f, FPGA : 0x7F

#define D_DefaultRampDnOffset		(0x20 << 9)		// Default value for Note Off Rampdown(Release) 0x0006<<9
#define D_DefaultRampDnClock		0x0002 			// 13*64*4*frame = 11.821ms; (1 frame = 1/281250 sec)

#define D_EnvClockWord				0x3333			// FPGA MODE 32/FrameRate == 32/140625 == 0.225ms; 
													// F32/G200 Realchip 64/281250 == 0.227ms
													// G100 series SPU16==>16/51270 == 0.312ms
								
#define D_AlpHeaderSize				20				// 20 words for ALP header

#define T_VarPhaseTable 			T_VarPhaseTableBottom + 107
#define D_PITCH_BEND_TABLE_TOTAL	0x000D

//High volume select
#define	D_VolSel_0					0				//single channel will be 1/32 of max volume
#define	D_VolSel_1					(0x1 << 6)		//1/8
#define	D_VolSel_2					(0x2 << 6)		//1/2
#define	D_VolSel_3					(0x3 << 6)		//1

//SPU_BIN
#define C_SPU_NO_ERR				0
#define C_INST_ERR					0xFFFFFFFF
#define C_LIB_ERR					0xFFFFFFFE
#define C_DRUM_ERR					0xFFFFFFFD
#define C_MEM_ERR					0xFFFFFFFC
#define	C_MIDI_ERR					0xFFFFFFFB

#define	T_InstrumentStartSection_size	128
#define T_InstrumentPitchTable_size 	128
#define	T_InstrumentSectionAddr_size	128
#define T_DrumAddr_size					128
#define T_Midi_size						1

#if MCU_VERSION <= GPL32_C
    #define SPU_ChannelNumber		32
#else
    #define SPU_ChannelNumber		16
#endif

#define MIDI_ChannelNumber			16
#define C_DefaultBeatBase			352
#define C_DefaultTempo				120

#define	STS_MIDI_PLAY				0x0001		// SPU Status
#define STS_MIDI_CALLBACK			0x0100		// MIDI stop call back routine has been initiated
#define STS_MIDI_REPEAT_ON			0x0200		// MIDI play back continuously
#define STS_MIDI_PAUSE_ON			0x0400		// MIDI Pause flag
#define STS_ADPCM_MODE_ON			0x0800		// SPU is at ADPCM mode
#define STS_NORMAL_MODE_ON			0x0000  	// SPU is at PCM mode
#define C_MIDI_Delay				0x00010000	// wait beat count equal to 0x0000

#define C_NoteEvent					0x0000
#define C_BeatCountEvent			0x0001
#define C_PitchBendEvent			0x0002
#define C_ControlEvent				0x0003
#define C_ProgramChangeEvent		0x0004
#define C_TempoEvent				0x0005
#define C_MIDI_EndEvent				0x0006
#define C_LyricEvent				0x0007
#define C_TextEvent					0x0008

// the following definition is for control event
#define C_DataEntryEvent			0x0006
#define C_VolumeEvent				0x0007
#define C_PanEvent					0x000A
#define C_ExpressionEvent			0x000B
#define C_RPN_LSB_Event				0x0064
#define C_RPN_MSB_Event				0x0065

extern const INT32U T_VarPhaseTableBottom[];
extern const INT32U T_PicthWheelTable[];
extern const INT32U T_PicthWheelTable_TWO[];
extern const INT32U T_BitEnable[];

extern INT32U T_InstrumentStartSection[];
extern INT32U T_InstrumentPitchTable[];
extern INT32U *T_InstrumentSectionAddr[];
extern INT32U T_InstrumentStartSection_1[];
extern INT32U T_InstrumentPitchTable_1[];
extern INT32U *T_InstrumentSectionAddr_1[];
extern INT32U *T_DrumAddr[];

extern char* SPU_get_driver_version(void);
extern void Load_ALP_Header(INT32U *pAddr);
extern void SPU_PlayNote(INT8U uiPitch, INT32U *uiAddr, INT8U uiPen, INT8U uiVelocity, INT8U uiSPUChannel);
extern void SPU_PlayDrum(INT8U uiDrumIndex, INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
extern INT8U SPU_PlayPCM_NoEnv(INT32U *pAddr, INT8U uiPan, INT8U uiVelocity);//return play channel
extern void SPU_PlayPCM_NoEnv_FixCH(INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
extern void SPU_PlayTone(INT8U uiPitch, INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
extern void SPU_PlayTone1(INT8U uiPitch, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
extern void SPU_MIDI_Play(INT8U repeat_en);//repeat_en: 1 repeat;   0 no repeat
extern void SPU_MIDI_Repeat(INT8U repeat_en);//repeat_en: 1 repeat;   0 no repeat
extern void SPU_MIDI_Stop(void);
extern void SPU_MIDI_Service(void);
extern void F_CheckDuration(void);

extern void SPU_EnableChannelFIQ(void);
extern void SPU_DisableChannelFIQ(void);
extern void SPU_EnableEnvelopeFIQ(void);
extern void SPU_DisableEnvelopeFIQ(void);
extern void SPU_EnableBeatCountFIQ(void);
extern void SPU_DisableBeatCountFIQ(void);
extern void SPU_EnablePostWaveFIQ(void);
extern void SPU_DisablePostWaveFIQ(void);

extern void SPU_MIDI_Pause(void);
extern void SPU_MIDI_Resume(void);
extern void SPU_pause_channel(INT8U spu_channel);
extern void SPU_resume_channel(INT8U spu_channel);
extern void SPU_Initial(void);
extern void SPU_MIDI_Set_MIDI_Volume(INT32U MIDI_Volume);//0~127
extern INT32U SPU_MIDI_Get_Status(void);
extern INT8U SPU_Get_SingleChannel_Status(INT8U SPU_Channel);

extern void F_StopExpiredCH(void);
extern void F_GetSeqCmd(void);
extern void F_CheckDuration(void);
extern void ProcessNoteEvent(void);
extern void ProcessControlEvent(void);
extern void ProcessPitchBendEvent(void);
extern void ProcessProgramChangeEvent(void);
extern void ProcessBeatCountEvent(void);
extern void ProcessEndEvent(void);

extern void SPU_StopChannel(INT32U StopChannel);
extern void SPU_MIDI_SetStopCallBackFunction(void (*StopCallBack)(void));
extern void SPU_MIDI_SetDataEntryEventCallback(void (*DataEntryEvent_callback)(void));	// 20081027 Roy
extern void SPU_MIDI_GetControlEventInfo(INT8U *para);									// 20081027 Roy
extern INT32U SPU_MIDI_GetCurDt(void);													// 20081028 Roy
extern void SPU_MIDI_PlayDt(INT32U MidiStartDt, INT32U MidiStopDt);						// 20081028 Roy
extern void SPU_MIDI_SetPlayDtEndCallback(void (*PlayDtStopCallBack)(void));			// 20081028 Roy
extern void SPU_AttachISR(INT8U FIQ_ID, void (*ISR)(void));
extern void SPU_ReleaseISR(INT8U FIQ_ID);
extern void SPU_MIDI_Set_SPU_Channel_Mask(INT32U SPU_CH_MASK);
extern void SPU_MIDI_SetMidiMask(INT32U MIDI_Ch);

extern INT32S SPU_Free_Midi(void);
extern INT32S SPU_Free_ToneColor_1(void);//free 第二组音色
extern void	SPU_MIDI_Set_MIDI_Pan(INT32U MIDI_Pan);
extern void	SPU_Set_midi_ring_buffer_addr(INT32U ring_buffer_addr);
extern INT32S SPU_load_tonecolor(INT16S fd_idi, INT32U MIDI_Index, INT8U mode);//mode:  0:NAND flash logic area, 2:NAND flash app area, 3:from file system
extern INT32S SPU_load_tonecolor_1(INT16S fd_idi, INT32U MIDI_Index, INT8U mode);//load第二组音色
extern INT32S SPU_check_fill_midi_ring_buffer(void);
extern void SPU_MIDI_SetMidiCH_ChangeColor_Mask(INT32U MIDI_Ch_Mask);
extern void SPU_MIDI_SetMidiCH_ChangeColor(INT32U MIDI_CH, INT32U color_index);
extern void TM_PlayTone(INT32U uiMidiCh, INT8U uiPitch, INT8U uiPan, INT8U uiVelocity);
extern void TM_StopTone(INT32U uiMidiCh, INT8U uiPitch);
extern void SPU_MIDI_Set_Tempo(INT8U tempo);
extern void SPU_Set_idi_addr(INT32U idi_addr);//设置idi数据在Nand flash中的起始地址或是在FS文件中的偏移地址
extern void SPU_Set_adpcm_comb_addr(INT32U adpcm_comb_addr, INT32U ram_buffer_addr);//设置adpcm_comb数据在Nand flash中的起始地址或是在FS文件中的偏移地址
			//以及存放ADPCM的临时RAM地址；ram_buffer_addr=0:调用user_memory_malloc()分配地址，其它，需要提供足够大的RAM区域的地址
extern INT32S SPU_load_comb_adpcm_to_RAM(INT16S fd_adpcm_comb, INT32U adpcm_index, INT8U mode);//mode:  0:NAND flash logic area, 2:NAND flash app area, 3:from file system
extern INT32S SPU_Free_adpcm_data_temp_buffer(void);//释放用于存放ADPCM数据的内存
extern INT32S SPU_play_comb_adpcm_FixCH(INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);

extern void* user_memory_malloc(INT32U size);
extern void user_memory_free(void* buffer_addr);

extern INT32S read_a_sector(INT16S fd, INT32U data_offset_addr, INT32U buffer_addr,INT8U mode);
extern INT32S read_two_sector(INT16S fd, INT32U data_offset_addr, INT32U buffer_addr,INT8U mode);
//mode:  0:NAND flash logic area 
//       2:NAND flash app area
//       3:from file system
//		 4:nv_read
//		 5:nvp_read
//       6:from sdram without fs

#endif		// __MCU_SPU_MIDI_DRIVER_H__
