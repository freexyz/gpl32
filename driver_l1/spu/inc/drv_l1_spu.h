#ifndef __drv_l1_SPU_H__
#define __drv_l1_SPU_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

// Vector Interrupt Controller
#define VIC_ALM_SCH_HMS		1
#define VIC_TMB_ABC			2
#define VIC_3				3
#define VIC_4				4
#define VIC_SPI1			5
#define VIC_EXT_AB			6
#define VIC_KEY_CHANGE		7
#define VIC_KEY_SCAN		8
#define VIC_TIMER3		    9
#define VIC_TIMER2			10
#define VIC_11				11
#define VIC_DFR				12
#define VIC_SPI0		    13
#define VIC_UART			14
#define VIC_CF				15
#define VIC_MS				16
#define VIC_TIMER1			17
#define VIC_18				18
#define VIC_SD				19
#define VIC_NFC	    		20
#define VIC_USB 			21
#define VIC_SCALER			22
#define VIC_JPEG			23
#define VIC_DMA				24
#define VIC_TIMER0			25
#define VIC_26				26
#define VIC_PPU				27
#define VIC_ADCF			28
#define VIC_ADC				29
#define VIC_AUDB			30
#define VIC_AUDA			31
#define VIC_UNEXPECT		32
#define VIC_MAX_IRQ			33				// Don't forget to modify this when new interrupt source is added

//timer define
#define C_Timer_A		0
#define C_Timer_B		1
#define C_Timer_C		2
#define C_Timer_D		3
#define C_Timer_E		4
#define C_Timer_F		5

//define SPU interrupt 
#define VIC_SPU_PW			1
#define VIC_SPU_BEAT		2
#define VIC_SPU_ENV			3
#define VIC_SPU_FIQ			4
#define VIC_MAX_FIQ			5				// Don't forget to modify this when new fast interrupt source is added


// P_SPU_CH_EN (0xD0400E00)
// bit[15:0] : channel[15:0] enable

// P_SPU_CH_EN_HIGH (0xD0400E80)
// bit[15:0] : channel[31:16] enable

// P_SPU_MAIN_VOLUME (0xD0400E04)
// bit[6:0] : spu main volume[6:0]
// bit[15:7] : reserved

// P_SPU_CH_FIQ_EN (0xD0400E08)
// bit[15:0] : channel[15:0] FIQ enable

// P_SPU_CH_FIQ_EN_HIGH(0xD0400E88)
// bit[15:0] : channel[31:16] FIQ enable

// P_SPU_CH_FIQ_STATUS (0xD0400E0C)
// bit[15:0] : channel[15:0] FIQ status

// P_SPU_CH_FIQ_STATUS_HIGH (0xD0400E8C)
// bit[15:0] : channel[31:16] FIQ status

// P_SPU_BEAT_BASE_COUNTER (0xD0400E10)
// bit[10:0] : beat base counter[10:0]
// bit[15:11] : reserved

// P_SPU_BEAT_COUNTER (0xD0400E14)
// bit[13:0] : beat count
// bit[14] : current beat IRQ event status
#define C_BEAT_IRQ_STATUS			0x4000
// bit[15] : beat IRQ enable
#define C_BEAT_IRQ_EN				0x8000

// P_SPU_ENV_CLK_CH0_3 (0xD0400E18)
// bit[3:0] : channel[0] envelope clock selection
// bit[7:4] : channel[1] envelope clock selection
// bit[11:8] : channel[2] envelope clock selection
// bit[15:12] : channel[3] envelope clock selection

// P_SPU_ENV_CLK_CH4_7 (0xD0400E1C)
// bit[3:0] : channel[4] envelope clock selection
// bit[7:4] : channel[5] envelope clock selection
// bit[11:8] : channel[6] envelope clock selection
// bit[15:12] : channel[7] envelope clock selection

// P_SPU_ENV_CLK_CH8_11 (0xD0400E20)
// bit[3:0] : channel[8] envelope clock selection
// bit[7:4] : channel[9] envelope clock selection
// bit[11:8] : channel[10] envelope clock selection
// bit[15:12] : channel[11] envelope clock selection

// P_SPU_ENV_CLK_CH12_15 (0xD0400E24)
// bit[3:0] : channel[12] envelope clock selection
// bit[7:4] : channel[13] envelope clock selection
// bit[11:8] : channel[14] envelope clock selection
// bit[15:12] : channel[15] envelope clock selection

// P_SPU_ENV_CLK_CH16_19 (0xD0400E98)
// bit[3:0] : channel[16] envelope clock selection
// bit[7:4] : channel[17] envelope clock selection
// bit[11:8] : channel[18] envelope clock selection
// bit[15:12] : channel[19] envelope clock selection

// P_SPU_ENV_CLK_CH20_23 (0xD0400E9C)
// bit[3:0] : channel[20] envelope clock selection
// bit[7:4] : channel[21] envelope clock selection
// bit[11:8] : channel[22] envelope clock selection
// bit[15:12] : channel[23] envelope clock selection

// P_SPU_ENV_CLK_CH24_27 (0xD0400EA0)
// bit[3:0] : channel[24] envelope clock selection
// bit[7:4] : channel[25] envelope clock selection
// bit[11:8] : channel[26] envelope clock selection
// bit[15:12] : channel[27] envelope clock selection

// P_SPU_ENV_CLK_CH28_31 (0xD0400EA4)
// bit[3:0] : channel[28] envelope clock selection
// bit[7:4] : channel[29] envelope clock selection
// bit[11:8] : channel[30] envelope clock selection
// bit[15:12] : channel[31] envelope clock selection

// P_SPU_ENV_RAMP_DOWN (0xD0400E28)
// bit[15:0] : channel[15:0] ramp down start

// P_SPU_ENV_RAMP_DOWN_HIGH (0xD0400EA8)
// bit[15:0] : channel[31:16] ramp down start

// P_SPU_CH_STOP_STATUS (0xD0400E2C)
// bit[15:0] : channel[15:0] stop status

// P_SPU_CH_STOP_STATUS_HIGH (0xD0400EAC)
// bit[15:0] : channel[31:16] stop status

// P_SPU_CH_ZC_ENABLE (0xD0400E30)
// bit[15:0] : channel[15:0] zero crossing enable

// P_SPU_CH_ZC_ENABLE_HIGH (0xD0400EB0)
// bit[15:0] : channel[31:16] zero crossing enable

// P_SPU_CONTROL_FLAG (0xD0400E34)
// bit[2:0] : reserved
// bit[3] : Initial channel's accumulator
#define C_INIT_ACC					0x0008
// bit[4] : reserved
// bit[5] : sample rate of tone color too high flag
#define C_FOF_FLAG					0x0020
// bit[7:6] : volume of single channel
#define C_CH_VOL_SEL				0x00C0
#define C_CH_VOL_1_DIV_32			0x0000		// 1/32
#define C_CH_VOL_1_DIV_8			0x0040		// 1/8
#define C_CH_VOL_1_DIV_2			0x0080		// 1/4
#define C_CH_VOL_1_DIV_1			0x00C0		// 1
// bit[8] : reserved
// bit[9] : 1: interpolation off, 0: interpolation on
#define C_NO_INTER					0x0200		
// bit[10] : 1: high quality interpolation off, 0: high quality interpolation on
#define C_NO_HIGH_INTER				0x0400
// bit[11] : 1: compressor on, 0: compressor off
#define C_COMP_EN					0x0800
// bit[14:12] : reserved
// bit[15] : signal saturate flag
#define C_SATURATE					0x8000

// P_SPU_COMPRESSOR_CTRL (0xD0400E38)
// bit[2:0] : compress ratio
#define C_COMPRESS_RATIO			0x0007
#define C_COMP_RATIO_1_DIV_2		0x0000
#define C_COMP_RATIO_1_DIV_3		0x0001
#define C_COMP_RATIO_1_DIV_4		0x0002
#define C_COMP_RATIO_1_DIV_5		0x0003
#define C_COMP_RATIO_1_DIV_6		0x0004
#define C_COMP_RATIO_1_DIV_7		0x0005
#define C_COMP_RATIO_1_DIV_8		0x0006
#define C_COMP_RATIO_1_DIV_INIF		0x0007
// bit[3] : 1: disable zero cross, 0: enable zero cross
#define C_DISABLE_ZC				0x0008
// bit[5:4] : release time scale
#define C_RELEASE_SCALE				0x0030
#define C_RELEASE_TIME_MUL_1		0x0000
#define C_RELEASE_TIME_MUL_4		0x0010
#define C_RELEASE_TIME_MUL_16		0x0020
#define C_RELEASE_TIME_MUL_64		0x0030
// bit[7:6] : attack time scale
#define C_ATTACK_SCALE				0x00C0
#define C_ATTACK_TIME_MUL_1			0x0000
#define C_ATTACK_TIME_MUL_4			0x0040
#define C_ATTACK_TIME_MUL_16		0x0080
#define C_ATTACK_TIME_MUL_64		0x00C0
// bit[14:8] : compress threshold, 0x01~0x7F
#define C_COMPRESS_THRESHOLD		0x7F00
// bit[15] : peak mode
#define C_COMPRESS_PEAK_MODE		0x8000

// P_SPU_CH_STATUS (0xD0400E3C)
// bit[15:0] : channel[15:0] status

// P_SPU_CH_STATUS_HIGH (0xD0400EBC)
// bit[15:0] : channel[31:16] status

// P_SPU_WAVE_IN_LEFT (0xD0400E40)
// bit[15:0] : the data is mixed with the left output of SPU

// P_SPU_WAVE_IN_RIGHT (0xD0400E44)
// bit[15:0] : the data is mixed with the right output of SPU

// P_SPU_WAVE_OUT_LEFT (0xD0400E48)
// bit[15:0] : the data is the final left output outcome of 32-channel + software channel

// P_SPU_POST_WAVE_OUT_LEFT (0xD0400EC8)
// bit[15:0] : the data is the left output port of 32-channel

// P_SPU_WAVE_OUT_RIGHT (0xD0400E4C)
// bit[15:0] : the data is the final right output outcome of 32-channel + software channel

// P_SPU_POST_WAVE_OUT_RIGHT (0xD0400ECC)
// bit[15:0] : the data is the right output port of 32-channel

// P_SPU_CH_REPEAT_EN (0xD0400E50)
// bit[15:0] : channel[15:0] envelope repeat enable

// P_SPU_CH_REPEAT_EN_HIGH (0xD0400ED0)
// bit[15:0] : channel[31:16] envelope repeat enable
 
// P_SPU_CH_ENV_MODE (0xD0400E54)
// bit[15:0] : channel[15:0] envelope mode, 1: manual mode, 0: auto mode

// P_SPU_CH_ENV_MODE_HIGH (0xD0400ED4)
// bit[15:0] : channel[31:16] envelope mode, 1: manual mode, 0: auto mode

// P_SPU_CH_TONE_RELEASE (0xD0400E58)
// bit[15:0] : channel[15:0] tone release

// P_SPU_CH_TONE_RELEASE_HIGH (0xD0400ED8)
// bit[15:0] : channel[31:16] tone release

// P_SPU_CH_IRQ_STATUS (0xD0400E5C)
// bit[15:0] : channel[15:0] envelope IRQ status

// P_SPU_CH_IRQ_STATUS_HIGH (0xD0400EDC)
// bit[15:0] : channel[31:16] envelope IRQ status

// P_SPU_CH_PITCH_BEND_EN (0xD0400E60)
// bit[15:0] : channel[15:0] pitch bend enable

// P_SPU_CH_PITCH_BEND_EN_HIGH (0xD0400EE0)
// bit[15:0] : channel[31:16] pitch bend enable

// P_SPU_ATTACK_RELEASE_TIME (0xD0400E68)
// bit[7:0] : release time control
// bit[15:8] : attack time control

// P_SPU_BENK_ADDR (0xD0400E7C)
// bit[8:0] : wave table¡¦s bank address
// bitp15:9] : reserved

// P_SPU_POST_WAVE_CTRL (0xD0400E94)
// bit[0] : internal post wave output queue to DMA overflow
#define C_PW_OVERFLOW				0x0001
// bit[4:1] : reserved
// bit[5] : post wave output right channel enable  0: LLLLLL... 1: LRLRLR....
#define C_PW_LR						0x0020
// bit[6] : 1: post wave output to DAC, 0: post wave do not output to DAC
#define C_PW_TO_DAC					0x0040
// bit[7] : 1: post wave is signed, 0: post wave is unsigned
#define C_PW_SIGNED					0x0080
// bit[9:8] : reserved
// bit[10] : 1: post wave IRQ active, 0: post wave IRQ inactive
#define C_PW_IRQ_ACTIVE				0x0400
// bit[11] : 1: post wave IRQ enable, 0: post wave IRQ disable
#define C_PW_IRQ_ENABLE				0x0800
// bit[12] : 1: post wave processing in 288KHz, 0: post wave processing is not valid
#define C_PW_CLOCK_SET				0x1000
// bit[13] : 1: enable post wave low pass filter, 0: disable post wave low pass filter
#define C_PW_LPF_ENABLE				0x2000
// bit[14] : 1: post wave down sample by 6, 0: post wave not down sample
#define C_PW_DOWN_SAMPLE			0x4000
// bit[15] : 1: post wave output enable, 0: post wave output disable
#define C_PW_DMA					0x8000

// P_SPU_WAVE_ADDR (0xD0401000 + 0x40*X)
// bit[15:0] : wave address[15:0]

// P_SPU_MODE (0xD0401004 + 0x40*X)
// bit[5:0] : wave address[21:16]
// bit[11:6] : loop address[21:6]
// bit[13:12] : tone color control mode
#define C_TONE_COLOR_MODE			0x3000
#define C_SW_MODE					0x0000
#define C_HW_AUTO_END_MODE			0x1000
#define C_HW_AUTO_REPEAT_MODE		0x2000
#define C_HW_AUTO_REPEAT_MODE1		0x3000
// bit[14] : 1: 16-bit data mode, 0: 8-bit data mode
#define C_16BIT_DATA_MODE			0x4000
// bit[15] : 1: ADPCM mode, 0: PCM mode
#define C_ADPCM_MODE				0x8000

// P_SPU_LOOP_ADDR (0xD0401008 + 0x40*X)
// bit[15:0] : loop address[15:0]

// P_SPU_PAN_VELOCITY (0xD040100C + 0x40*X)
// bit[6:0] :  channel volumn[6:0]
// bit[7] : reserved
// bit[14:8] : pan value[6:0]
// bit[15] : reserved

// P_SPU_ENVELOPE_0 (0xD0401010 + 0x40*X)
// bit[6:0] : envelope increment[6:0]
// bit[7] : 1: envelope direction is negative, 0: envelope direction is positive
#define C_ENVELOPE_SIGN				0x0080
// bit[14:8] : envelope target[6:0]
// bit[15] : reserved

// P_SPU_ENVELOPE_DATA (0xD0401014 + 0x40*X)
// bit[6:0] : envelope data, EDD[6:0]
// bit[7] : reserved
// [15:8] : envelope counter[7:0]

// P_SPU_ENVELOPE_1 (0xD0401018 + 0x40*X)
// bit[7:0] : envelope counter re-load data[7:0]
// bit[8] : 1: envelope repeat mode, 0: envelope normal mode
#define C_ENVELOPE_REPEAT			0x0100
// bit[15:9] : envelope repeat counter[6:0]

// P_SPU_ENV_ADDR_HIGH (0xD040101C + 0x40*X)
// bit[5:0] : envelope address[21:16]
// bit[6] : 1: envelope IRQ enable, 0: envelope IRQ disable
#define C_ENVELOPE_IRQ_ENABLE		0x0040
// bit[15:7] : envelope IRQ fire address[8:0]

// P_SPU_ENV_ADDR_LOW (0xD0401020 + 0x40*X)
// bit[15:0] : envelope address[15:0]

// P_SPU_WAVE_DATA_0 (0xD0401024 + 0x40*X)
// bit[15:0] : previous tone-color data value

// P_SPU_LOOP_CTRL (0xD0401028 + 0x40*X)
// bit[8:0] : envelope repeat address offset[8:0]
// bit[15:9] : envelope ramp down offset[6:0]

// P_SPU_WAVE_DATA (0xD040102C + 0x40*X)
// bit[15:0] : current tone-color data value

// P_SPU_ADPCM_SEL (0xD0401034 + 0x40*X)
// bit[8:0] : reserved
// bit[13:9] : point number of first frame in ADPCM36 mode
// bit[14] : reserved
// bit[15] : 1: ADPCM36 mode, 0: ADPCM mode
#define C_ADPCM36_MODE				0x8000

// P_SPU_PHASE_HIGH (0xD0401800 + 0x40*X)
// bit[2:0] : tone color pitch phase[18:16]
// bit[15:3] : reserved

// P_SPU_PHASE_ACC_HIGH (0xD0401804 + 0x40*X)
// bit[2:0] : phase accumulator[18:16]
// bit[15:3] : reserved

// P_SPU_TARGET_PHASE_HIGH (0xD0401808 + 0x40*X)
// bit[2:0] : pitch bend target phase[18:16]
// bit[15:3] : reserved

// P_SPU_RAMP_DOWN_CLK (0xD040180C + 0x40*X)
// bit[2:0] : envelope ramp down clock selection
#define C_RAMP_DOWN_CLK_0738us		0x0000
#define C_RAMP_DOWN_CLK_2955us		0x0001
#define C_RAMP_DOWN_CLK_11821us		0x0002
#define C_RAMP_DOWN_CLK_47284us		0x0003
#define C_RAMP_DOWN_CLK_189137us	0x0004
#define C_RAMP_DOWN_CLK_756548us	0x0005
#define C_RAMP_DOWN_CLK_1513ms		0x0006
#define C_RAMP_DOWN_CLK_1513ms_2	0x0007
// bit[15:3] : reserved

// P_SPU_PHASE (0xD0401810 + 0x40*X)
// bit[15:0] : tone color pitch phase[15:0]

// P_SPU_PHASE_ACC (0xD0401814 + 0x40*X)
// bit[15:0] : tone color accumulator[15:0]

// P_SPU_TARGET_PHASE (0xD0401818 + 0x40*X)
// bit[15:0] : pitch bend target phase

// P_SPU_PHASE_CTRL (0xD040181C + 0x40*X)
// bit[11:0] : pitch bend phase offset[11:0]
// bit[12] : 1: pahse decrease, 0: phase increase
#define C_PHASE_SIGN				0x1000
// bit[15:13] : pitch bend phase time step
#define C_PHASE_TIME_STEP_0114us	0x0000
#define C_PHASE_TIME_STEP_0227us	0x2000
#define C_PHASE_TIME_STEP_0455us	0x4000
#define C_PHASE_TIME_STEP_0909us	0x6000
#define C_PHASE_TIME_STEP_1819us	0x8000
#define C_PHASE_TIME_STEP_3637us	0xA000
#define C_PHASE_TIME_STEP_7274us	0xC000
#define C_PHASE_TIME_STEP_14549us	0xE000

extern void SPU_Init(void);
extern INT32U SPU_Get_DrvVersion(void);
extern void   SPU_Clear_SRAM(void);
extern void   SPU_Clear_Register(void);
//enable channel
extern void SPU_Enable_Channel(INT8U ChannelIndex);
extern void SPU_Disable_Channel(INT8U ChannelIndex);
extern void SPU_Enable_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_MultiChannel(INT32U ChannelBit);
extern INT32U SPU_GetChannelEnableStatus(void);
//main volume
extern void  SPU_Set_MainVolume(INT8U VolumeData);
extern INT8U SPU_Get_MainVolume(void);
//channel fiq
extern void SPU_Enable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Disable_FIQ_Channel(INT8U ChannelIndex);
extern void SPU_Enable_FIQ_MultiChannel(INT32U ChannelBit);
extern void SPU_Disable_FIQ_MultiChannel(INT32U ChannelBit);
//fiq status
extern void   SPU_Clear_FIQ_Status(INT32U ChannelBit);
extern INT32U SPU_Get_FIQ_Status(void);
//beat count
extern void   SPU_Set_BeatBaseCounter(INT16U BeatBaseCounter);
extern INT16U SPU_Get_BeatBaseCounter(void);
extern void   SPU_Set_BeatCounter(INT16U BeatCounter);
extern INT16U SPU_Get_BeatCounter(void);
INT8U SPU_Get_BeatIRQ_Enable_Flag(void);	
//beat irq
extern void SPU_Enable_BeatIRQ(void);
extern void SPU_Disable_BeatIRQ(void);
extern void SPU_Clear_BeatIRQ_Flag(void);
extern INT8U SPU_Get_BeatIRQ_Flag(void);
//envelope ramp down
extern void SPU_Set_EnvelopeClock(INT8U EnvClock, INT8U ChannelIndex);
extern void SPU_Set_EnvRampDown(INT8U RampDownChannel);
extern void SPU_Set_EnvRampDownMultiChannel(INT32U ChannelBit);
extern INT32U SPU_Get_EnvRampDown(void);
//channel stop flag
extern void SPU_Clear_Ch_StopFlag(INT8U ChannelIndex);
extern void SPU_Clear_MultiCh_StopFlag(INT32U ChannelBit);
extern INT32U SPU_Get_Ch_StopStatus(void);
//compressor control
extern void SPU_EnableChannelZC(INT8U ChannelIndex);
extern void SPU_DisableChannelZC(INT8U ChannelIndex);
extern void SPU_AccumulatorInit(void);
extern INT8U SPU_Read_FOF_Status(void);
extern void SPU_Clear_FOF(void);
//channel volume
extern void SPU_SingleChVolumeSelect(INT8U VolumeSelect);
extern INT8U SPU_GetSingleChVolumeSetting(void);
//control flag 
extern void SPU_InterpolationON(void);
extern void SPU_InterpolationOFF(void);
extern void SPU_HQ_InterpolationON(void);
extern void SPU_HQ_InterpolationOFF(void);
extern void SPU_CompressorON(void);
extern void SPU_CompressorOFF(void);
extern void SPU_ClearSaturateFlag(void);
extern INT8U SPU_ReadSaturateFlag(void);
extern void SPU_SetCompressorRatio(INT8U ComRatio);
extern INT8U SPU_GetCompressorRatio(void);
extern void SPU_EnableCompZeroCrossing(void);
extern void SPU_DisableCompZeroCrossing(void);
extern void SPU_SetReleaseTimeScale(INT8U ReleaseTimeScale);
extern INT8U SPU_ReadReleaseTimeScale(void);
extern void SPU_SetAttackTimeScale(INT8U AttackTimeScale);
extern INT8U SPU_ReadAttackTimeScale(void);
extern void SPU_SetCompressThreshold(INT8U CompThreshold);
extern INT8U SPU_ReadCompressThreshold(void);
extern void SPU_SelectRMS_Mode(void);
extern void SPU_SelectPeakMode(void);
//channel status
extern INT32U SPU_GetChannelStatus(void);
//software channel
extern void SPU_SendToSoftChannel_Left(INT16U PCM_Data);
extern void SPU_SendToSoftChannel_Right(INT16U PCM_Data);
extern INT16U SPU_GetSPU_PlusSoftOutLeft(void);
extern INT16U SPU_GetSPU_PlusSoftOutRight(void);
//wave out 
extern INT16U SPU_GetSPU_OutLeft(void);
extern INT16U SPU_GetSPU_OutRight(void);
//channel repeat
extern void SPU_EnableChannelRepeat(INT8U ChannelIndex);
extern void SPU_DisableChannelRepeat(INT8U ChannelIndex);
extern void SPU_EnableMultiChannelRepeat(INT32U ChannelBit);
extern void SPU_DisableMultiChannelRepeat(INT32U ChannelBit);
//envelop auto mode
extern void SPU_EnvelopeAutoMode(INT8U ChannelIndex);
extern void SPU_EnvelopeManualMode(INT8U ChannelIndex);
//channel release
extern void SPU_SetChannelRelease(INT8U ChannelIndex);
//envelop IRQ
extern INT32U SPU_GetEnvelopeIRQ_Status(void);
extern void SPU_ClearEnvelopeIRQ_Status(INT32U ChannelBit);
//pitchbend control
extern void SPU_EnablePitchBend(INT8U ChannelIndex);
extern void SPU_DisablePitchBend(INT8U ChannelIndex);
//release attack time
extern void SPU_SetReleaseTime(INT8U ReleaseTime);
extern INT8U SPU_ReadReleaseTime(void);
extern void SPU_SetAttackTime(INT8U AttackTime);
extern INT8U SPU_ReadAttackTime(void);
//bank 
extern void SPU_SetBenkAddr(INT16U BenkAddr);
//post process
extern void SPU_ClearPWFOV_Flag(void);
extern void SPU_EnablePW_RightChannel(void);
extern void SPU_DisablePW_RightChannel(void);
extern INT8U SPU_GetPWFOV_Flag(void);
extern void SPU_EnablePostWaveOutputSilence(void);
extern void SPU_DisablePostWaveOutputSilence(void);
extern void SPU_SetPostWave_Signed(void);
extern void SPU_SetPostWave_Unsigned(void);
extern void SPU_ClearPostWaveIRQ_Status(void);
extern INT8U SPU_GetPostWaveIRQ_Status(void);
extern void SPU_EnablePostWaveIRQ(void);
extern void SPU_DisablePostWaveIRQ(void);
extern void SPU_SetPostWaveClock_288K(void);
extern void SPU_SetPostWaveClock_281K(void);
extern void SPU_EnablePostWaveLPF(void);
extern void SPU_DisablePostWaveLPF(void);
extern void SPU_EnablePostWaveDownSample(void);
extern void SPU_DisablePostWaveDownSample(void);
extern void SPU_EnablePostWaveToDMA(void);
extern void SPU_DisablePostWaveToDMA(void);
//wave start/loop address
extern void SPU_SetStartAddress(INT32U StartAddr, INT8U ChannelIndex);
extern void SPU_SetLoopAddress(INT32U LoopAddr, INT8U ChannelIndex);
extern void SPU_GetStartAddress(INT32U *StartAddr, INT8U ChannelIndex);
extern void SPU_GetLoopAddress(INT32U *LoopAddr, INT8U ChannelIndex);
//tone color mode
extern void SPU_SetToneColorMode(INT8U ToneColorMode, INT8U ChannelIndex);
extern void SPU_Set_16bit_Mode(INT8U ChannelIndex);
extern void SPU_Set_8bit_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SetPCM_Mode(INT8U ChannelIndex);
//volicety/pan 
extern void SPU_SetVelocity(INT8U VelocityValue, INT8U ChannelIndex);
extern void SPU_SetPan(INT8U PanValue, INT8U ChannelIndex);
// envelop set
extern void SPU_SetEnvelope_0(INT16U Envelope_0, INT8U ChannelIndex);
extern void SPU_SetEnvelopeIncrement(INT8U EnvInc, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeIncrement(INT8U ChannelIndex);
extern void SPU_SetEnvelopePostive(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNegative(INT8U ChannelIndex);
extern void SPU_SetEnvelopeTarget(INT8U EnvTarget, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeTarget(INT8U ChannelIndex);
extern void SPU_SetEnvelopeData(INT8U EnvData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeCounter(INT8U EnvCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelope_1(INT16U Envelope_1, INT8U ChannelIndex);
extern void SPU_SetEnvelopeReloadData(INT8U EnvReloadData, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeReloadData(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeNormalMode(INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatCounter(INT8U RepeatCounter, INT8U ChannelIndex);
extern INT8U SPU_ReadEnvelopeRepeatCounter(INT8U ChannelIndex);
extern void SPU_SetEnvelopeAddress(INT32U EnvelopeAddr, INT8U ChannelIndex);
//envelop irq
extern void SPU_SetEnvelopeIRQ_Enable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_Disable(INT8U ChannelIndex);
extern void SPU_SetEnvelopeIRQ_FireAddress(INT16U FireAddr, INT8U ChannelIndex);
extern void SPU_SetWaveData_0(INT16U WDD_0, INT8U ChannelIndex);
extern void SPU_SetWaveData(INT16U WDD, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRepeatAddrOffset(INT16U EAOffset, INT8U ChannelIndex);
extern void SPU_SetEnvelopeRampDownOffset(INT8U RampDownOffset, INT8U ChannelIndex);
//ADPCM36 mode
extern void SPU_SelectADPCM_Mode(INT8U ChannelIndex);
extern void SPU_SelectADPCM36_Mode(INT8U ChannelIndex);
extern void SPU_SetADPCM_PointNumber(INT8U PointNumber, INT8U ChannelIndex);
extern void SPU_ClearADCPM36_Mode(INT8U ChannelIndex);	
//set phase
extern void SPU_SetPhase(INT32U Phase, INT8U ChannelIndex);
extern INT32U SPU_ReadPhase(INT8U ChannelIndex);
extern void SPU_SetPhaseAccumulator(INT32U PhaseAcc, INT8U ChannelIndex);
extern INT32U SPU_ReadPhaseAccumulator(INT8U ChannelIndex);
extern void SPU_SetTargetPhase(INT32U TargetPhase, INT8U ChannelIndex);
extern INT32U SPU_ReadTargetPhase(INT8U ChannelIndex);
extern void SPU_SetRampDownClock(INT8U RampDownClock, INT8U ChannelIndex);
extern void SPU_SetPhaseOffset(INT16U PhaseOffset, INT8U ChannelIndex);
extern void SPU_SetPhaseIncrease(INT8U ChannelIndex);
extern void SPU_SetPhaseDecrease(INT8U ChannelIndex);
extern void SPU_SetPhaseTimeStep(INT8U PhaseTimeStep, INT8U ChannelIndex);
extern void SPU_SetPhaseControl(INT16U PhaseControl, INT8U ChannelIndex);
extern INT16U SPU_ReadPhaseControl(INT8U ChannelIndex);
//fiq set
extern void SPU_FIQ_Register(INT8U FIQ_Index, void (*F_ISR)(void));
extern void SPU_FIQ_Enable(INT8U FIQ_Index);
extern void SPU_FIQ_Disable(INT8U FIQ_Index);
extern void SPU_Test_IRQ_Register(INT8U IRQ_Index, void (*F_ISR)(void));
extern void SPU_Test_IRQ_Enable(INT8U IRQ_Index);
extern void SPU_Test_IRQ_Disable(INT8U IRQ_Index);
extern void SPU_Test_Timer_IRQ_Setup(INT32U timer_id, INT32U freq_hz, void (*TimerIsrFunction)(void));
#endif		// __drv_l1_SPU_H__
