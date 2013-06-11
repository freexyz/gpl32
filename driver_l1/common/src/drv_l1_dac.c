/* 
* Purpose: DAC driver/interface
*
* Author: Allen Lin
*
* Date: 2007/10/15
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_dac.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_DAC) && (_DRV_L1_DAC == 1)                   //
//================================================================//


// Constant definitions used in this file only go here

// Type definitions used in this file only go here

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file
DAC_CTRL   dac_ctrl;
INT8U      cha_int_level;
INT8U      chb_int_level;
DMA_STRUCT dma_struct_dbf;

// Functions defined in this file
static void dac_cha_fifo_isr(void);
static void dac_chb_fifo_isr(void); 

void dac_init(void)
{
	R_DAC_CHA_CTRL = 0;
	R_DAC_CHB_CTRL = 0;  
	
	R_DAC_CHA_CTRL |= DAC_FEM_IF; /* clear FIFO empty flag */
	R_DAC_CHB_CTRL |= DAC_FEM_IF;
	
	R_DAC_CHA_CTRL &= ~DAC_FEM_IEN; /* disabe FIFO empty interrupt */
	R_DAC_CHB_CTRL &= ~DAC_FEM_IEN; /* disabe FIFO empty interrupt */ 
	
	R_DAC_CHA_CTRL |= DAC_VREFEN;
	while((R_DAC_PGA & DAC_MUTE) != 0); /* wait vref ready */
	
	R_DAC_CHA_CTRL |= (DAC_CH_EN|DAC_SIGNED); /* enable CHA and use signed data type */
	R_DAC_CHB_CTRL |= (DAC_CH_EN|DAC_CHB_SSF);
	
	R_DAC_CHA_FIFO |= DAC_FIFO_RST; /* reset FIFO */
	R_DAC_CHB_FIFO |= DAC_FIFO_RST; 
	
	//R_DAC_IISEN |= DAC_IIS_EN; /* enable IIS */
	
	vic_irq_register(VIC_AUDA, dac_cha_fifo_isr);	/* register DAC isr */
	vic_irq_enable(VIC_AUDA);
	
	vic_irq_register(VIC_AUDB, dac_chb_fifo_isr);	/* register DAC isr */
	vic_irq_enable(VIC_AUDB);
	
	dac_fifo_level_set(0x8,0x8);
	dma_struct_dbf.channel = 0xff;
}

void dac_enable_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_DAC_CHA_CTRL |= DAC_CH_EN;
		R_DAC_CHB_CTRL |= DAC_CH_EN;
	}
	else {
		R_DAC_CHA_CTRL &= ~DAC_CH_EN;
		R_DAC_CHB_CTRL &= ~DAC_CH_EN;
	}
}

void dac_disable(void)
{	
	R_DAC_CHA_CTRL &= ~DAC_VREFEN;
	
	/* at least delay 300 ms */
#if _OPERATING_SYSTEM == 1
	//OSTimeDly(31);
	drv_msec_wait(305);
#else
	drv_msec_wait(305);
#endif
	R_ANALOG_CTRL = 0; /* clear ENCI */
	R_DAC_CHA_CTRL &= ~DAC_CH_EN;
	R_DAC_CHB_CTRL &= ~DAC_CH_EN;
}

void dac_fifo_level_set(INT8U cha_level,INT8U chb_level)
{
	R_DAC_CHA_FIFO &= ~DAC_FIFO_LEVEL;
	R_DAC_CHB_FIFO &= ~DAC_FIFO_LEVEL;
	
	R_DAC_CHA_FIFO |= (cha_level << 4); 
	R_DAC_CHB_FIFO |= (cha_level << 4); 
	cha_int_level = cha_level;
	chb_int_level = chb_level;		
}

void dac_data_signed_set(BOOLEAN flag)
{
	if (flag == TRUE) {
		R_DAC_CHA_CTRL |= DAC_SIGNED;
	}
	else {
		R_DAC_CHA_CTRL &= ~DAC_SIGNED;
	}
}

void dac_mono_set(void)
{
	R_DAC_CHB_CTRL |= DAC_MONO;
	R_DAC_CHB_CTRL |= DAC_CHB_CHACFG;
}

/*left right left..data in buffer */
void dac_stereo_set(void)
{	
	R_DAC_CHB_CTRL &= ~DAC_MONO;
	R_DAC_CHB_CTRL |= DAC_CHB_CHACFG;
}

/*place left in one buffer and right in one buffer
  you have to send data to channel A and channel B individually */
void dac_stereo_indi_buffer_set(void)
{
	R_DAC_CHB_CTRL &= ~DAC_MONO;
	R_DAC_CHB_CTRL &= ~DAC_CHB_CHACFG;
}

void dac_sample_rate_set(INT32U hz)
{
	dac_timer_freq_setup(hz*2);	
}

void dac_timer_stop(void)
{
	timer_stop(TIMER_E);	
}

void dac_pga_set(INT8U gain)
{
	R_DAC_PGA &= ~DAC_PGA;
	R_DAC_PGA |= gain;		
}

INT8U dac_pga_get(void)
{
	INT8U pga;
	
	pga = R_DAC_PGA;
	pga &= DAC_PGA;
	
	return pga;
}

void dac_vref_set(BOOLEAN status)
{
	if (status == TRUE) { /* enable */
		R_DAC_CHA_CTRL |= DAC_VREFEN;
		while((R_DAC_PGA & DAC_MUTE) != 0); /* wait vref ready */
	}
	else {/* disable */
		R_DAC_CHA_CTRL &= ~DAC_VREFEN;
	}
}

void dac_int_set(INT8U ch,INT8U status)
{
	if (ch == DAC_CHA) {
		if (status == DAC_ENABLED) {
			R_DAC_CHA_CTRL |= DAC_FEM_IEN; /* enable FIFO empty interrupt */	
		}
		else {
			R_DAC_CHA_CTRL &= ~DAC_FEM_IEN; /* disable FIFO empty interrupt */
		}
	}
	else {
		if (status == DAC_ENABLED) {
			R_DAC_CHB_CTRL |= DAC_FEM_IEN; /* enable FIFO empty interrupt */	
		}
		else {
			R_DAC_CHB_CTRL &= ~DAC_FEM_IEN; /* disable FIFO empty interrupt */
		}
	}
}

static void dac_cha_fifo_isr(void)
{
	INT8U 	i = 0;
	R_DAC_CHA_CTRL |= DAC_FEM_IF; /* clear interrupt flag */
	
	/* start transmition */ 
    for (i=0;(i<(16-cha_int_level)) && (dac_ctrl.cha_len > dac_ctrl.cha_count);i++) {
		if((!(R_DAC_CHA_FIFO & DAC_FIFO_FULL))) {	
			R_DAC_CHA_DATA = dac_ctrl.cha_data[dac_ctrl.cha_count++];
		}
	}
	if (dac_ctrl.cha_count >= dac_ctrl.cha_len) {
		dac_int_set(DAC_CHA, DAC_DISABLED);
		*dac_ctrl.cha_notify = 1;
	}	
}

static void dac_chb_fifo_isr(void)
{
	INT8U 	i = 0;
	R_DAC_CHB_CTRL |= DAC_FEM_IF; /* clear interrupt flag */
	
	/* start transmition */ 
    for (i=0;(i<(16-chb_int_level)) && (dac_ctrl.chb_len > dac_ctrl.chb_count);i++) {
		if((!(R_DAC_CHB_FIFO & DAC_FIFO_FULL))) {	
			R_DAC_CHB_DATA = dac_ctrl.chb_data[dac_ctrl.chb_count++];
		}
	}
	if (dac_ctrl.chb_count >= dac_ctrl.chb_len) {
		dac_int_set(DAC_CHB, DAC_DISABLED);
		*dac_ctrl.chb_notify = 1;
	}	
}

void dac_cha_data_put(INT16S *data,INT32U len, INT8S *notify)
{
	dac_ctrl.cha_data = data;
	dac_ctrl.cha_len = len;
	dac_ctrl.cha_count = 0;
	dac_ctrl.cha_notify = notify;
	
	if (len != 0) {
		dac_int_set(DAC_CHA, DAC_ENABLED);
	}	
}

void dac_chb_data_put(INT16S *data,INT32U len, INT8S *notify)
{
	dac_ctrl.chb_data = data;
	dac_ctrl.chb_len = len;
	dac_ctrl.chb_count = 0;
	dac_ctrl.chb_notify = notify;
	
	if (len != 0) {
		dac_int_set(DAC_CHB, DAC_ENABLED);
	}		
}

INT32S dac_cha_data_dma_put(INT16S *data,INT32U len, INT8S *notify)
{
	DMA_STRUCT dma_struct;
	INT32S status;
	
	*notify = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) data;
	dma_struct.t_addr = (INT32U) P_DAC_CHA_DATA;
	dma_struct.width = DMA_DATA_WIDTH_2BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	dma_struct.count = (INT32U) len;
	dma_struct.notify = notify;
	dma_struct.timeout = 0;
	status = dma_transfer(&dma_struct);
	if ( status != 0)
		return status;
		
	return STATUS_OK;
}

INT32S dac_chb_data_dma_put(INT16S *data,INT32U len, INT8S *notify)
{
	DMA_STRUCT dma_struct;
	INT32S status;
	
	*notify = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) data;
	dma_struct.t_addr = (INT32U) P_DAC_CHB_DATA;
	dma_struct.width = DMA_DATA_WIDTH_2BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	dma_struct.count = (INT32U) len;
	dma_struct.notify = notify;
	dma_struct.timeout = 0;
	status = dma_transfer(&dma_struct);
	if ( status != 0)
		return status;

	return STATUS_OK;
}

#if _OPERATING_SYSTEM == 1
/* use DMA double buffer */
INT32S dac_cha_dbf_put(INT16S *data,INT32U len, OS_EVENT *os_q)
{
	
	INT32S status;
	
	dma_struct_dbf.s_addr = (INT32U) data;
	dma_struct_dbf.t_addr = (INT32U) P_DAC_CHA_DATA;
	dma_struct_dbf.width = DMA_DATA_WIDTH_2BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	dma_struct_dbf.count = (INT32U) len;
	dma_struct_dbf.notify = NULL;
	dma_struct_dbf.timeout = 0;
	
	status = dma_transfer_with_double_buf(&dma_struct_dbf, os_q);
	if ( status != 0)
		return status;
		
	return STATUS_OK;
}
/* set second buffer for DMA double buffer */
INT32S dac_cha_dbf_set(INT16S *data,INT32U len)
{
	INT32S status;
	
	dma_struct_dbf.s_addr = (INT32U) data;
	dma_struct_dbf.count = (INT32U) len;
	dma_struct_dbf.notify = NULL;
	dma_struct_dbf.timeout = 0;
	
	status = dma_transfer_double_buf_set(&dma_struct_dbf);
	if ( status != 0)
		return status;
		
	return STATUS_OK;
}

INT32S dac_dbf_status_get(void)
{
	if (dma_struct_dbf.channel == 0xff) {
		return -1;
	}
	return dma_dbf_status_get(dma_struct_dbf.channel);	
}

INT32S dac_dma_status_get(void)
{
	if (dma_struct_dbf.channel == 0xff) {
		return -1;
	}
	return dma_status_get(dma_struct_dbf.channel);	
}

void dac_dbf_free(void)
{
	dma_transfer_double_buf_free(&dma_struct_dbf);
	dma_struct_dbf.channel = 0xff;
}
#endif


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_DAC) && (_DRV_L1_DAC == 1)              //
//================================================================//