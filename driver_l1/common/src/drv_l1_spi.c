/* 
* Purpose: SPI driver/interface
*
* Author: Allen Lin
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_spi.h"
#include "drv_l1_tools.h"


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_SPI) && (_DRV_L1_SPI == 1)                   //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here
#define SPI_TRANS_POLLING 1
#define SPI_TRANS_INT 0

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file
INT8U           spi_usage[SPI_MAX_NUMS];
SPI_MASTER      spi_master[SPI_MAX_NUMS];

// Functions defined in this file
static INT32S spi_transceive_dma(INT8U spi_num);
static SPI_SFR * get_SPI_SFR_base(INT8U spi_num);
static void spi_reg_clear(INT8U spi_num);
static void spi_txrx_level_set(INT8U spi_num, INT8U tx_level, INT8U rx_level);

#if SPI_TRANS_INT == 1
static INT32S spi_transceive_int(INT8U spi_num);
static void   spi0_isr_master(void);
static void   spi1_isr_master(void);
#endif

#if SPI_TRANS_POLLING == 1
static INT32S spi_transceive_polling(INT8U spi_num);
#endif

/*******************************************************************************
* spi_init - Initialize SPI interface
* DESCRIPTION:
* 	This routine:
*		-Reset the SPI.
*       -Set the SPI mode.
*       -Set the clock phase and polarity.
*		-Enable the SPI.  
* INPUT: None.
* OUTPUT: None.
* RETURN: None.
*******************************************************************************/
void spi_init(INT8U spi_num)
{		
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	spi_usage[spi_num] = 0;
	
	spi_reg_clear(spi_num);
	
	/* reset the SPI */
	spi->CTRL |= SPI_CONTROL_RST;

	/*enable the SPI */
	spi->CTRL |= SPI_CONTROL_ENA;
	
	spi->MISC |= SPI_MISC_SMART;
	
	gp_memset((INT8S *)&spi_master[spi_num], 0, sizeof(SPI_MASTER));
	spi_master[spi_num].spi_txrx = spi_transceive_dma;
	
#if SPI_TRANS_INT == 1
	spi_txrx_level_set(spi_num, 4, 4); /* set interrupt trigger level */
	if (spi_num == SPI_0) {
		vic_irq_register(VIC_SPI0, spi0_isr_master);	/* register SPI isr */
		vic_irq_enable(VIC_SPI0);
	}
	else {
		vic_irq_register(VIC_SPI1, spi1_isr_master);	/* register SPI isr */
		vic_irq_enable(VIC_SPI1);
	}
#endif

}

INT32S spi_disable(INT8U spi_num)
{
	if (spi_num == SPI_0) {
		R_SPI0_CTRL &= ~SPI_CONTROL_ENA;
	}
	else if (spi_num == SPI_1) {
		R_SPI1_CTRL &= ~SPI_CONTROL_ENA;
	}
	else {
		return STATUS_FAIL;
	} 
	return STATUS_OK;
}

#if SPI_TRANS_INT == 1
static void spi_tx_int_set(INT8U spi_num, INT8S tx_int)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return;
	}
	
	if (tx_int == SPI_ENABLED) {
		spi->TX_STATUS |= SPI_TX_STATUS_IEN;
	}
	else {
		spi->TX_STATUS &= ~SPI_TX_STATUS_IEN;
	}
	return;
} 

static void spi_rx_int_set(INT8U spi_num, INT8S rx_int)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return;
	}
	
	if (rx_int == SPI_ENABLED) {
		spi->RX_STATUS |= SPI_RX_STATUS_IEN;
	}
	else {
		spi->RX_STATUS &= ~SPI_RX_STATUS_IEN;
	}
	return;
} 
#endif

static INT32S spi_usage_get(INT8U spi_num)
{
	if (spi_usage[spi_num] == 1) {
		return SPI_BUSY;
	}
	spi_usage[spi_num] = 1;
	
	return STATUS_OK;
}

static void spi_usage_free(INT8U spi_num)
{
	spi_usage[spi_num] = 0;	
}

static void spi_reg_clear(INT8U spi_num)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	spi->CTRL = 0;
	spi->TX_STATUS = 0;
	spi->RX_STATUS = 0;
	spi->MISC = 0;
		
	spi->TX_STATUS |= SPI_TX_STATUS_IF; /* clear interrupt flag */
	spi->RX_STATUS |= SPI_RX_STATUS_IF;
}

/*******************************************************************************
* spi_clk_set - Set the spi clock
* DESCRIPTION:
* 	This routine:
*       -Set the SPI clock.
* INPUT: spi_clk - SPI clock.
* OUTPUT: None.
* RETURN: STATUS_OK/STATUS_FAIL
*******************************************************************************/
INT32S spi_clk_set(INT8U spi_num, INT8S spi_clk)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return STATUS_FAIL;
	}

	/* check the spi clock */
	switch(spi_clk)
	{
		case SYSCLK_2:
		case SYSCLK_4:
		case SYSCLK_8:
		case SYSCLK_16:
		case SYSCLK_32:
		case SYSCLK_64: 
		case SYSCLK_128:
			break;
		default:
			return STATUS_FAIL;
	}
	
	/* set the SPI clock */
	spi->CTRL &= ~SPI_CONTROL_CLK; 
	spi->CTRL |= spi_clk;
	
	return STATUS_OK;
}

INT32S spi_clk_rate_set(INT8U spi_num, INT32U clk_rate)
{
	INT32U div;
	INT32U i;
	
	div = MCLK/clk_rate;
	
	for (i=0;i<7;i++) {
		div >>= 1;
		if (div == 1) {
			break;
		}
	}
	
	if (i == 7) {
		i--;
	}
	
	div = i;		
	spi_clk_set(spi_num, div);
	
	return STATUS_OK;
}
/*******************************************************************************
* spi_pha_pol_set - Set the spi clock phase and polarity
* DESCRIPTION:
* 	This routine:
*       -Set the SPI phase and polarity.
* INPUT: spi_num - SPI channel.
*        pha_pol - phase and polarity
* OUTPUT: None.
* RETURN: STATUS_OK/STATUS_FAIL
*******************************************************************************/
INT32S spi_pha_pol_set(INT8U spi_num, INT8U pha_pol)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return STATUS_FAIL;
	}
	
	/*pahse and polarity should not be changed after SPIEN is 1*/
	spi_disable(spi_num);
	
	switch(pha_pol) 
	{
		case PHA0_POL0:
			spi->CTRL &= ~SPI_CONTROL_CPHA;
			spi->CTRL &= ~SPI_CONTROL_CPOL;
			break;
		case PHA0_POL1:
			spi->CTRL &= ~SPI_CONTROL_CPHA;
			spi->CTRL |= SPI_CONTROL_CPOL;
			break; 
		case PHA1_POL0:
			spi->CTRL |= SPI_CONTROL_CPHA;
			spi->CTRL &= ~SPI_CONTROL_CPOL;
			break; 
		case PHA1_POL1:
			spi->CTRL |= SPI_CONTROL_CPHA;
			spi->CTRL |= SPI_CONTROL_CPOL;
			break; 
		default:
			break;
	}
	spi->CTRL |= SPI_CONTROL_ENA;
	return STATUS_OK;
}

void spi_cs_by_internal_set(INT8U spi_num,INT8U pin_num, INT8U active)
{
	gpio_init_io(pin_num,GPIO_OUTPUT);
	gpio_set_port_attribute(pin_num, 1);
	
	spi_master[spi_num].cs_internal = TRUE;
	spi_master[spi_num].cs_active = active;
	spi_master[spi_num].cs_gpio_pin = pin_num;
	gpio_write_io(spi_master[spi_num].cs_gpio_pin, (!spi_master[spi_num].cs_active)&0x1);
	return;
}

void spi_cs_by_external_set(INT8U spi_num)
{
	spi_master[spi_num].cs_internal = FALSE;
	return;
}

/*******************************************************************************
* spi_lbm_set - Set the SPI loop back mode
* DESCRIPTION:
* 	This routine:
*       -Set the SPI loop back mode
* INPUT: status
* OUTPUT: None.
* RETURN: None.
*******************************************************************************/
void spi_lbm_set(INT8U spi_num, INT8S status)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return ;
	}
	
	if (status == SPI_LBM_LOOP_BACK) {
		spi->CTRL |= SPI_CONTROL_LBM;
	}
	else {
		spi->CTRL &= ~SPI_CONTROL_LBM; 
	}
	
	return;
}


/*******************************************************************************
* spi_transceive - master mode transmit and receive data 
* DESCRIPTION:
* 	This routine:
*       -master mode transmit and receive data
* INPUT: spi_num,tx_data,tx_len,rx_len
* OUTPUT: rx_data
* RETURN: STATUS_OK/SPI_TIMEOUT/SPI_BUSY.
*******************************************************************************/
INT32S spi_transceive(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len)
{
	INT32S result;
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif	
	result = spi_usage_get(spi_num);
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif

	if (result == SPI_BUSY) {
		return SPI_BUSY;
	}	
	
	spi_master[spi_num].spi_num = spi_num;
	spi_master[spi_num].tx_buf = tx_data;
	spi_master[spi_num].rx_buf = rx_data;
	spi_master[spi_num].len = tx_len;
	spi_master[spi_num].rcv_len = rx_len;
	spi_master[spi_num].tx_count = 0;
	spi_master[spi_num].rx_count = 0;
	spi_master[spi_num].spi_sfr = get_SPI_SFR_base(spi_num);
	
	spi_master[spi_num].spi_sfr->CTRL |= SPI_CONTROL_RST;

	if (spi_master[spi_num].cs_internal == TRUE) { /* chip select by internal */
		gpio_write_io(spi_master[spi_num].cs_gpio_pin, spi_master[spi_num].cs_active);
	}

	if (spi_master[spi_num].spi_txrx != 0) {
		result = spi_master[spi_num].spi_txrx(spi_num);
	}

	if (spi_master[spi_num].cs_internal == TRUE) { /* chip select by internal */
		gpio_write_io(spi_master[spi_num].cs_gpio_pin, (!spi_master[spi_num].cs_active)&0x1);
	}

	//*rx_len = spi_master[spi_num].rx_count;
	
#if _OPERATING_SYSTEM == 1				// Soft Protect for critical section
	OSSchedLock();
#endif	
	spi_usage_free(spi_num);
#if _OPERATING_SYSTEM == 1	
	OSSchedUnlock();
#endif
	
	return result;
}

INT32S spi_transceive_cpu(INT8U spi_num, INT8U *tx_data, INT32U tx_len, INT8U *rx_data, INT32U rx_len)
{
	INT32S result;
	
	spi_master[spi_num].spi_txrx = spi_transceive_polling;
	result = spi_transceive(spi_num,tx_data,tx_len,rx_data,rx_len);
	spi_master[spi_num].spi_txrx = spi_transceive_dma;
	
	return result;
}

/*******************************************************************************
* spi_txrx_level_set - Set the SPI tx and rx FIFO level
* DESCRIPTION:
* 	This routine:
*       -Set the SPI tx and rx FIFO level
* INPUT: status
* OUTPUT: None.
* RETURN: None.
*******************************************************************************/
static void spi_txrx_level_set(INT8U spi_num, INT8U tx_level, INT8U rx_level)
{
	SPI_SFR *spi = (SPI_SFR *)get_SPI_SFR_base(spi_num);
	
	if (spi_num >= SPI_MAX_NUMS) {
		return;
	}
	
	if ((rx_level>7) || (tx_level>7)) {
		return;
	} 

	spi->TX_STATUS &= ~SPI_TX_STATUS_LEVEL;
	spi->TX_STATUS |= tx_level << 4;
	
	spi->RX_STATUS &= ~SPI_RX_STATUS_LEVEL;
	spi->RX_STATUS |= rx_level << 4;
	
	spi_master[spi_num].tx_int_level = tx_level;
	spi_master[spi_num].rx_int_level = rx_level;
}

#if SPI_TRANS_INT == 1

static void spi0_isr_master(void)
{
    INT8U  dummy;
    INT32U tx_status = 0;
    INT32U rx_status = 0;
    INT32U i = 0;
    
    tx_status = spi_master[SPI_0].spi_sfr->TX_STATUS;
    rx_status = spi_master[SPI_0].spi_sfr->RX_STATUS;
    
	if (rx_status & SPI_RX_STATUS_IF) {
		spi_master[SPI_0].spi_sfr->RX_STATUS |= SPI_RX_STATUS_IF; /* clear interrupt flag */
		while(spi_master[SPI_0].spi_sfr->MISC & SPI_MISC_RNE){
			/* receive */
			if (spi_master[SPI_0].rx_count < spi_master[SPI_0].len) {
				if (spi_master[SPI_0].rx_count < spi_master[SPI_0].rcv_len) {
				spi_master[SPI_0].rx_buf[spi_master[SPI_0].rx_count++] = (INT8U)spi_master[SPI_0].spi_sfr->RX_DATA;
			}
			else {
				dummy = (INT8U)spi_master[SPI_0].spi_sfr->RX_DATA;
					spi_master[SPI_0].rx_count++;
				}
			}
			else {
				dummy = (INT8U)spi_master[SPI_0].spi_sfr->RX_DATA;
			} 
		}
	}	

    if (tx_status & SPI_TX_STATUS_IF) {	
    	spi_master[SPI_0].spi_sfr->TX_STATUS |= SPI_TX_STATUS_IF; /* clear interrupt flag */
    	
    	/* start transmition */
    	for (i=0;(i<8-spi_master[SPI_0].tx_int_level)&&(spi_master[SPI_0].len>spi_master[SPI_0].tx_count);i++) {	
			if ((spi_master[SPI_0].spi_sfr->MISC & SPI_MISC_TNF)) {		
				spi_master[SPI_0].spi_sfr->TX_DATA = (INT32U)spi_master[SPI_0].tx_buf[spi_master[SPI_0].tx_count++];
			}
		}
		if (spi_master[SPI_0].tx_count >= spi_master[SPI_0].len) {
			spi_tx_int_set(spi_master[SPI_0].spi_num, SPI_DISABLED);
		}	
	}
}

static void spi1_isr_master(void)
{
    INT8U  dummy;
    INT32U tx_status = 0;
    INT32U rx_status = 0;
    INT32U i = 0;
    
    tx_status = spi_master[SPI_1].spi_sfr->TX_STATUS;
    rx_status = spi_master[SPI_1].spi_sfr->RX_STATUS;;
    
	if (rx_status & SPI_RX_STATUS_IF) {
		spi_master[SPI_1].spi_sfr->RX_STATUS |= SPI_RX_STATUS_IF; /* clear interrupt flag */
		while(spi_master[SPI_1].spi_sfr->MISC & SPI_MISC_RNE){
			/* receive */
			if (spi_master[SPI_1].rx_count < spi_master[SPI_1].len) {
				if (spi_master[SPI_1].rx_count < spi_master[SPI_1].rcv_len) {
				spi_master[SPI_1].rx_buf[spi_master[SPI_1].rx_count++] = (INT8U)spi_master[SPI_1].spi_sfr->RX_DATA;
			}
			else {
				dummy = (INT8U)spi_master[SPI_1].spi_sfr->RX_DATA;
					spi_master[SPI_1].rx_count++;
				}
			}
			else {
				dummy = (INT8U)spi_master[SPI_1].spi_sfr->RX_DATA;
			} 
		}
	}	

    if (tx_status & SPI_TX_STATUS_IF) {	
    	spi_master[SPI_1].spi_sfr->TX_STATUS |= SPI_TX_STATUS_IF; /* clear interrupt flag */
    	
    	/* start transmition */
    	for (i=0;(i<8-spi_master[SPI_1].tx_int_level)&&(spi_master[SPI_1].len>spi_master[SPI_1].tx_count);i++) {
    		
			if ((spi_master[SPI_1].spi_sfr->MISC & SPI_MISC_TNF)) {		
				spi_master[SPI_1].spi_sfr->TX_DATA = (INT32U)spi_master[SPI_1].tx_buf[spi_master[SPI_1].tx_count++];
			}
		}
		if (spi_master[SPI_1].tx_count >= spi_master[SPI_1].len) {
			spi_tx_int_set(spi_master[SPI_1].spi_num, SPI_DISABLED);
		}	
	}
}
#endif

#if SPI_TRANS_POLLING == 1
static INT32S spi_transceive_polling(INT8U spi_num)
{
	INT32U timeout;
	INT8U  dummy;
	
	while (spi_master[spi_num].len > spi_master[spi_num].tx_count) { 
		spi_master[spi_num].spi_sfr->TX_DATA = (INT32U)spi_master[spi_num].tx_buf[spi_master[spi_num].tx_count];
		timeout = 0;
		while(!(spi_master[spi_num].spi_sfr->RX_STATUS & SPI_RX_STATUS_RXFLAG)) {
			if (++timeout == SPI_TIMEOUT_VALUE) {
	        	return SPI_TIMEOUT;
	    	}	
	    }
	    if (spi_master[spi_num].tx_count < spi_master[spi_num].rcv_len) { 
		spi_master[spi_num].rx_buf[spi_master[spi_num].tx_count] = (INT8U)spi_master[spi_num].spi_sfr->RX_DATA;
		}
		else {
			dummy = (INT8U)spi_master[spi_num].spi_sfr->RX_DATA;
		}
		spi_master[spi_num].tx_count++;
	}
	//spi_master[spi_num].rx_count = spi_master[spi_num].tx_count;
	return STATUS_OK;
}
#endif

#if SPI_TRANS_INT == 1
static INT32S spi_transceive_int(INT8U spi_num)
{
	INT32S i;

	spi_rx_int_set(spi_master[spi_num].spi_num, SPI_ENABLED); /* Enable SPI receive interrupt */
	spi_tx_int_set(spi_master[spi_num].spi_num, SPI_ENABLED); /* Enable SPI transmit interrupt */
	
	while(spi_master[spi_num].tx_count != spi_master[spi_num].len);
	for (i=0;i<7;i++) { /* maybe some datas still in fifo */
		while(!(spi_master[spi_num].spi_sfr->MISC & SPI_MISC_TNF));
		spi_master[spi_num].spi_sfr->TX_DATA = 0xff;
	}
	
	while (spi_master[spi_num].rx_count<spi_master[spi_num].len); /* wait for completion */

	spi_rx_int_set(spi_master[spi_num].spi_num, SPI_DISABLED); /* Disable SPI receive interrupt */
	
	return STATUS_OK;
}
#endif

static INT32S spi_transceive_dma(INT8U spi_num)
{
	DMA_STRUCT dma_struct;
	INT8S      done1,done2;
	INT32S     status;
	//INT8U      dummy;
	
	spi_txrx_level_set(spi_master[spi_num].spi_num,0,0);
	
	done1 = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) &spi_master[spi_num].spi_sfr->RX_DATA;
	dma_struct.width = DMA_DATA_WIDTH_1BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	if (spi_master[spi_num].rcv_len == 0) {
		dma_struct.t_addr = (INT32U) 0x60000000; /* reserved memory */
		dma_struct.count = (INT32U) (spi_master[spi_num].len);
	}
	else {
		dma_struct.t_addr = (INT32U) spi_master[spi_num].rx_buf;
		//dma_struct.count = (INT32U) (spi_master[spi_num].rcv_len);
		dma_struct.count = (INT32U) (spi_master[spi_num].len);
	}
	dma_struct.notify = &done1;
	dma_struct.timeout = 128;	
	status = dma_transfer(&dma_struct);
	if (status != 0) {
		return status;
	}
	
	done2 = C_DMA_STATUS_WAITING;
	dma_struct.s_addr = (INT32U) spi_master[spi_num].tx_buf;
	dma_struct.t_addr = (INT32U) &spi_master[spi_num].spi_sfr->TX_DATA;
	dma_struct.width = DMA_DATA_WIDTH_1BYTE;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
	dma_struct.count = (INT32U) (spi_master[spi_num].len);
	dma_struct.notify = &done2;
	dma_struct.timeout = 128;	
	
	status = dma_transfer(&dma_struct);
	
	if (status != 0) {
		return status;
	}
	
	while (done1 == C_DMA_STATUS_WAITING);	
	while (done2 == C_DMA_STATUS_WAITING);
		
	if (done1 == C_DMA_STATUS_TIMEOUT || done2 == C_DMA_STATUS_TIMEOUT) {
		return SPI_TIMEOUT;
	}  
		
	//spi_master[spi_num].rx_count = spi_master[spi_num].len;
	return STATUS_OK;
}

static SPI_SFR * get_SPI_SFR_base(INT8U spi_num)
{
	if (spi_num == 0){
		return (SPI_SFR *)P_SPI0_CTRL;
	}
	else {
		return (SPI_SFR *)P_SPI1_CTRL;
	}
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_SPI) && (_DRV_L1_SPI == 1)                   //
//================================================================//