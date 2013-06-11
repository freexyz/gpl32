/*
* Purpose: ADC driver/interface
*
* Author: allenlin
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_adc.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_ADC) && (_DRV_L1_ADC == 1)                   //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file
ADC_CTRL adc_ctrl;
void (*adc_user_isr)(INT16U data);
void (*tp_user_isr)(void);
// Functions defined in this file
static void adc_auto_isr(void);
static void adc_manual_isr(void);
void tp_callback_set(void (*user_isr)(void));
void adc_tp_en(BOOLEAN status);
void adc_tp_int_en(BOOLEAN status);

void adc_init(void)
{
	R_ADC_SETUP  = 0;
	R_ADC_ASADC_CTRL = 0;
	R_ADC_MADC_CTRL  = 0;
	R_ADC_TP_CTRL    = 0;
	
	R_ADC_SETUP |= ADC_ADBEN; /* AD Bias Reference Voltage Enable */	
	
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	R_ADC_SETUP	 |= ADC_AUTO_ASIEN; /* enable ADC bias in GPL327XX series */	
	R_ADC_USELINEIN = 0x7;	/*set line1, line0 and mic are alanog pad */
#elif (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB)
	R_SYSTEM_CTRL_NEW |= 0x400;	/* AD Bias Reference Voltage Enable for GPL326XXB series */
	R_ADC_USELINEIN = 0x3C;	/* LINE0/1 (IOF 6/7) work as NAND REB/ WEB by default for GPL326XXB series */
#else
	R_ADC_USELINEIN = 0xF;	
#endif

	R_ADC_SH_WAIT = 0x0807;
	
	adc_user_isr = 0;
	gp_memset((INT8S *)&adc_ctrl, 0, sizeof(ADC_CTRL)); 
	
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
	vic_irq_register(VIC_ADCF, adc_auto_isr);	/* register auto smaple isr */
	vic_irq_enable(VIC_ADCF);
#else
	vic_irq_register(VIC_CF, adc_auto_isr);	/* register auto smaple isr */
	vic_irq_enable(VIC_CF);
#endif
	vic_irq_register(VIC_ADC, adc_manual_isr);	/* register manual smaple isr */
	vic_irq_enable(VIC_ADC);
}	

static void adc_auto_isr()
{
	while(((R_ADC_ASADC_CTRL & 0xF) != 0) && (adc_ctrl.count < adc_ctrl.data_len)) {
			adc_ctrl.adc_data[adc_ctrl.count++] = R_ADC_ASADC_DATA;
	}
	R_ADC_ASADC_CTRL |= ADC_ASIF; /* clear flag must after getting data */
	
	if (adc_ctrl.count >= adc_ctrl.data_len) {
		adc_auto_int_set(FALSE); /* disable fifo interrupt */
		*adc_ctrl.notify = 1;
	}	
}

static void adc_manual_isr()
{
	
	if (R_ADC_TP_CTRL & ADC_TP_IF) {
		R_ADC_TP_CTRL |= ADC_TP_IF;
        (*tp_user_isr)();
	}	
	
	if (R_ADC_MADC_CTRL & ADC_ADCRIF) {
		if(R_ADC_MADC_CTRL & (ADC_MIASE | ADC_ASIME)) {
			adc_manual_sample_start();
			return;
		}

		if((R_ADC_MADC_CTRL & ADC_CNVRDY) == 0) {
			adc_manual_sample_start();
			return;
		}
		
	    R_ADC_MADC_CTRL |= ADC_ADCRIF;
	    (*adc_user_isr)((INT16U)R_ADC_MADC_DATA);
	    R_ADC_MADC_CTRL &= ~ADC_RIEN; /* disable manual interrupt */		
	}
}

void adc_vref_enable_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_ADC_SETUP |= ADC_ADBEN; /* AD Bias Reference Voltage Enable */
	}
		
	else {
		R_ADC_SETUP &= ~ADC_ADBEN; /* AD Bias Reference Voltage Disable */
	}
}

void adc_fifo_level_set(INT8U level)
{
	R_ADC_ASADC_CTRL &= ~ADC_FIFO_LEVEL;
	R_ADC_ASADC_CTRL |= (level << 4); 
}

void adc_auto_ch_set(INT8U ch)
{
	R_ADC_SETUP &= ~ADC_AUTO_CH_SEL;
	R_ADC_SETUP |= (ch << 4);
}

void adc_manual_ch_set(INT8U ch)
{
	R_ADC_MADC_CTRL &= ~ADC_MANUAL_CH_SEL;
	R_ADC_MADC_CTRL |= ch;
}

void adc_auto_int_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_ADC_ASADC_CTRL |= ADC_AUTO_ASIEN;
	}
	else {
		R_ADC_ASADC_CTRL &= ~ADC_AUTO_ASIEN;
	}
}

void adc_manual_callback_set(void (*user_isr)(INT16U data))
{
	if (user_isr != 0) {
		adc_user_isr = user_isr;
	}
}

INT32S adc_manual_sample_start(void)
{	
	R_ADC_MADC_CTRL |= ADC_RIEN;  /* enable manual interrupt */
	R_ADC_MADC_CTRL |= ADC_MIASE; /* clear error flag */
	R_ADC_MADC_CTRL |= ADC_ASIME; /* clear error flag */
	
	R_ADC_MADC_CTRL |= ADC_STRCNV; /* start manual sample */
#if 0
	while(1) {
		R_ADC_MADC_CTRL |= ADC_STRCNV; /* start manual sample */
		while(!(R_ADC_MADC_CTRL & ADC_ADCRIF)); /* wait ready */
		R_ADC_MADC_CTRL |= ADC_ADCRIF;
		*data = R_ADC_MADC_DATA;
		if ((!(R_ADC_MADC_CTRL & ADC_MIASE)) && (!(R_ADC_MADC_CTRL & ADC_ASIME))) {
			break;
		}
		R_ADC_MADC_CTRL |= ADC_MIASE; /* clear error flag */
		R_ADC_MADC_CTRL |= ADC_ASIME; /* clear error flag */
	}
	//R_ADC_MADC_CTRL &= ~ADC_RIEN; /* disable manual interrupt */
#endif	
	return STATUS_OK;
}

INT32S adc_auto_sample_start(void)
{
	R_ADC_SETUP |= ADC_ADBEN; /* AD Bias Reference Voltage Enable */
	R_ADC_ASADC_CTRL |= ADC_ASIF; /* clear fifo interrupt flag */
	R_ADC_ASADC_CTRL |= ADC_ASADC_DMA; /* DMA mode enable */
	R_ADC_SETUP |= ADC_ASEN;
	R_ADC_SETUP |= ADC_ASMEN; /* start auto sample */
	return STATUS_OK;
}

INT32S adc_auto_data_dma_get(INT16U *data, INT32U len, INT8S *notify)
{
	DMA_STRUCT  dma_struct;
	INT32S      status;
	
	if (len > ADC_BLOCK_LEN) {
		return ADC_ERR_WRONG_LEN;
	}
	
	*notify = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) P_ADC_ASADC_DATA;
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

INT32S adc_auto_data_get(INT16U *data, INT32U len, INT8S *notify)
{
	if (len > ADC_BLOCK_LEN) {
		return ADC_ERR_WRONG_LEN;
	}
	R_ADC_ASADC_CTRL &= ~ADC_ASADC_DMA; /* DMA mode disable */
	
	adc_ctrl.adc_data = data;
	adc_ctrl.data_len = len;
	adc_ctrl.count = 0;
	adc_ctrl.notify = notify;
	*adc_ctrl.notify = 0;
	
	if (len != 0) {
		adc_auto_int_set(TRUE); /* enable fifo interrupt */
	}
	return STATUS_OK;	
}

void adc_auto_sample_stop(void)
{
	R_ADC_SETUP &= ~ADC_ASMEN;	
}

INT32S adc_sample_rate_set(INT8U timer_id, INT32U hz)
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
	
	R_ADC_SETUP &= ~ADC_ASTMS;
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
	R_ADC_SETUP |= timer_id;
#else
	R_ADC_SETUP |= as_timer;
#endif
	adc_timer_freq_setup(as_timer, hz);
	
	return STATUS_OK;
}

INT32S adc_timer_stop(INT8U timer_id)
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

void adc_fifo_clear(void)
{
	INT16U dummy;
	while((R_ADC_ASADC_CTRL & 0xF) != 0) {
		dummy = R_ADC_ASADC_DATA;	
	}	
}

void adc_tp_en(BOOLEAN status)
{
	if (status == TRUE) {
		R_ADC_TP_CTRL |= ADC_TP_EN;
		//R_ADC_TP_CTRL |= ADC_TP_MODE;
	}
	else {
		R_ADC_TP_CTRL &= ~ADC_TP_EN;
		//R_ADC_TP_CTRL &= ~ADC_TP_MODE;
	}
}

void adc_tp_int_en(BOOLEAN status)
{
	if (status == TRUE) {
		R_ADC_TP_CTRL |= ADC_TP_INT_EN;
	}
	else {
		R_ADC_TP_CTRL &= ~ADC_TP_INT_EN;
	}
}

INT32S adc_user_line_in_en(INT8U line_id,BOOLEAN status)
{
	switch(line_id) {
		case ADC_LINE_0:
		case ADC_LINE_1:
		case ADC_LINE_2:
		case ADC_LINE_3:
			break;
		default:
			return STATUS_FAIL;
	}
	
	if (status == TRUE) {
		R_ADC_USELINEIN |= line_id; 
	}
	else {
		R_ADC_USELINEIN &= ~line_id; 
	}
	
	return STATUS_OK;
}


void tp_callback_set(void (*user_isr)(void))
{
	if (user_isr != 0) {
		tp_user_isr = user_isr;
	}
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_ADC) && (_DRV_L1_ADC == 1)              //
//================================================================//
