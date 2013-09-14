/*
* Purpose: MIC driver/interface
*
* Author: 
*
* Date: 
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 
* History :
*/

//Include files
#include "drv_l1_mic.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_MIC) && (_DRV_L1_MIC == 1)                   //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file
MIC_CTRL mic_ctrl;

void mic_init(void)
{
	R_MIC_SETUP  = 0;
	R_MIC_ASADC_CTRL = 0;
	
	R_MIC_SETUP |= MIC_ADBEN; /* AD Bias Reference Voltage Enable */	
	
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)	
	R_MIC_SETUP	|= MIC_AUTO_ASIEN;	/* to enable MIC/ADC bias in GPL327XX series */
	R_MIC_PGA_GAIN |= 0x300;
#elif (defined MCU_VERSION) && ((MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA))
	
	R_SYSTEM_CTRL_NEW |= 0x400;	/* to enable MIC/ADC bias in GPL326xxB series */
	R_MIC_PGA_GAIN &= ~ 0x300;
#endif
	R_MIC_SH_WAIT = 0x0807;
		
	gp_memset((INT8S *)&mic_ctrl, 0, sizeof(MIC_CTRL)); 
}	

void mic_auto_int_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_MIC_ASADC_CTRL |= MIC_AUTO_ASIEN;
	}
	else {
		R_MIC_ASADC_CTRL &= ~MIC_AUTO_ASIEN;
	}
}

void mic_fifo_level_set(INT8U level)
{
	R_MIC_ASADC_CTRL &= ~MIC_FIFO_LEVEL;
	R_MIC_ASADC_CTRL |= (level << 4); 
}

void mic_fifo_clear(void)
{
	INT16U dummy;
	while((R_MIC_ASADC_CTRL & 0xF) != 0) {
		dummy = R_MIC_ASADC_DATA;	
	}	
}

INT32S mic_auto_sample_start(void)
{
	R_MIC_SETUP |= MIC_ADBEN; /* AD Bias Reference Voltage Enable */
	R_MIC_SETUP |= MIC_EN; // MIC enable 
	R_MIC_ASADC_CTRL |= MIC_ASIF; /* clear fifo interrupt flag */
	R_MIC_ASADC_CTRL |= MIC_ASADC_DMA; /* DMA mode enable */
	R_MIC_SETUP |= MIC_ASEN;
	R_MIC_SETUP |= MIC_ASMEN; /* start auto sample */
#if (defined MCU_VERSION) && ((MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA))
	R_MIC_SETUP |= 0x0800;	//select MICP as input channel for MIC which is thru PGA module
#endif 

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	R_MIC_SETUP |= MIC_BITTRANS;
#endif	
	
	return STATUS_OK;
}

void mic_auto_sample_stop(void)
{
	R_MIC_SETUP &= ~MIC_ASMEN;	/* stop auto sample */
}

INT32S mic_sample_rate_set(INT8U timer_id, INT32U hz)
{
	INT32U as_timer;
	
	switch(timer_id) {
		case ADC_AS_TIMER_C:
			as_timer = TIMER_C;
			break;
		case ADC_AS_TIMER_D:
			as_timer = TIMER_D;
			break;
		case ADC_AS_TIMER_E:
			as_timer = TIMER_E;
			break;
		case ADC_AS_TIMER_F:
			as_timer = TIMER_F;
			break;
		default:
			return STATUS_FAIL;
	}
	
	R_MIC_SETUP &= ~MIC_ASTMS;
	R_MIC_SETUP |= as_timer;
	adc_timer_freq_setup(as_timer, hz);
	
	return STATUS_OK;
}

INT32S mic_timer_stop(INT8U timer_id)
{
	INT32U as_timer;
	
	switch(timer_id) {
		case ADC_AS_TIMER_C:
			as_timer = TIMER_C;
			break;
		case ADC_AS_TIMER_D:
			as_timer = TIMER_D;
			break;
		case ADC_AS_TIMER_E:
			as_timer = TIMER_E;
			break;
		case ADC_AS_TIMER_F:
			as_timer = TIMER_F;
			break;
		default:
			return STATUS_FAIL;
	}
	timer_stop(as_timer);	
	return STATUS_OK;
}

void mic_vref_enable_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_MIC_SETUP |= MIC_ADBEN; /* AD Bias Reference Voltage Enable */
	}
		
	else {
		R_MIC_SETUP &= ~MIC_ADBEN; /* AD Bias Reference Voltage Disable */
	}
}

void mic_agc_enable_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_MIC_SETUP |= MIC_AGCEN; /* AGC Enable */
	}
		
	else {
		R_MIC_SETUP &= ~MIC_AGCEN; /* AGC Disable */
	}
}

INT32S mic_auto_data_dma_get(INT16U *data, INT32U len, INT8S *notify)
{
	DMA_STRUCT  dma_struct;
	INT32S      status;
	
	if (len > MIC_BLOCK_LEN) {
		return MIC_ERR_WRONG_LEN;
	}
	
	*notify = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) P_MIC_ASADC_DATA;
	dma_struct.t_addr = (INT32U) data;
	dma_struct.width = DMA_DATA_WIDTH_2BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	dma_struct.count = (INT32U) len;
	dma_struct.notify = notify;
	dma_struct.timeout = 0;
	
	status = dma_transfer(&dma_struct);
	if ( status != 0)
		return status;
		
	return STATUS_OK;	
}

INT32S mic_auto_data_get(INT16U *data, INT32U len, INT8S *notify)
{
	if (len > MIC_BLOCK_LEN) {
		return MIC_ERR_WRONG_LEN;
	}
	R_MIC_ASADC_CTRL &= ~MIC_ASADC_DMA; /* DMA mode disable */
	
	mic_ctrl.mic_data = data;
	mic_ctrl.data_len = len;
	mic_ctrl.count = 0;
	mic_ctrl.notify = notify;
	*mic_ctrl.notify = 0;
	
	if (len != 0) {
		mic_auto_int_set(TRUE); /* enable fifo interrupt */
	}
	return STATUS_OK;	
}


#if (defined MCU_VERSION) && ((MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA))
void mic_set_pga_gain(INT16U booster_gain, INT16U pga_gain)
{
	//booster gain: 00: 0dB; 01: 6dB; 10: 12dB; 11: 18dB
	//pga gain : 00000: 37.5dB ~ 11111: - infinite ( mute ) 
	//please refer to programming guide for detail
	R_MIC_PGA_GAIN = ((booster_gain <<8) & 0x0300) | (pga_gain & 0x001F);
}

INT16U mic_get_pga_gain(void)
{
	return (R_MIC_PGA_GAIN & 0x1F);
}

INT16U mic_get_booster_gain(void)
{
	return ((R_MIC_PGA_GAIN & 0x1F) >>8);
}

void mic_dagc_enable(INT16U enable)
{
	if(enable == 1)
		R_DAGC_ENABLE |= 0x1;
	else
		R_DAGC_ENABLE &= ~0x1;	
}

INT16U mic_dagc_get_status(void)
{
	return (R_DAGC_STATUS & 0x1F);
}

void mic_dagc_set_update_freq(INT16U update_freq)
{
	R_DAGC_ENABLE &= ~0xFFF0;
	R_DAGC_ENABLE |=  (update_freq << 4);
}

INT16U mic_dagc_get_update_freq(void)
{
	return ((R_DAGC_ENABLE & 0xFFF0) >> 4);	
}

void mic_dagc_setup(INT16U mode, INT16U threshold, INT16U att_scale, INT16U rel_scale, INT16U dis_zc)
{
	INT16U val;
	
	if(mode == 1)
		val |= 0x8000;		// Peak mode
	else	
		val &= ~0x8000;	//RMS mode	
	
	//threshold is from 0x1 ~ 0x7F; 0 is not allowed! (default is 0x70)
	//att_scale: 00: AttackTime *1 ; 01: AttackTime* 4; 10: AttackTime*16; 11: AttackTime*64
	//rel_scale: 00: ReleaseTime *1 ; 01: ReleaseTime* 4; 10: ReleaseTime*16; 11: ReleaseTime*64
	val = (threshold <<8) & 0x7F00 | (att_scale<<6) & 0x00C0 | (rel_scale<<4) & 0x0030;
	
	if(dis_zc == 1)
		val |= 0x08;		//disable zero-crossing
	else
		val &= ~0x08;		//enable zero-crossing
	
	R_DAGC_CTRL = (val | 0x07);		
}

INT16U mic_dagc_get_threshold(void)
{
	return ((R_DAGC_CTRL & 0x7F00) >> 8);	
}

void mic_dagc_set_att_rel_time(INT16U att_time, INT16U rel_time)
{
	//att_time (AttackTime) is from 0x00 to 0xFF and default is 0x80
	//rel_time (ReleaseTime) is from 0x00 to 0xFF and default is 0x80
	R_DAGC_TIME = (att_time << 8) | rel_time; 
}

INT16U mic_dagc_get_att_time(void)
{
	return (R_DAGC_TIME >> 8);
}

INT16U mic_dagc_get_rel_time(void)
{
	return (R_DAGC_TIME & 0xFF);
}
#endif
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_MIC) && (_DRV_L1_MIC == 1)              //
//================================================================//
