/*
* Purpose: UART driver/interface
*
* Author: Tristan Yang
*
* Date: 2007/09/21
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/

#include "drv_l1_uart.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_UART) && (_DRV_L1_UART == 1)                 //
//================================================================//

#define UART_RX			0
#define UART_TX			1
#define UART_DISABLE	0
#define UART_ENABLE		1

#define UART0_FIFO_SIZE 32
typedef struct fifo {
	INT8U *p_start;
	INT8U *p_end;
	INT8U *p_write;
	INT8U *p_read;
} UART_FIFO;

UART_FIFO 	sw_fifo_head;
INT8U 		sw_fifo_buf[UART0_FIFO_SIZE];
INT32U		uart0_tx_on;
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
fp_msg_qsend ur_msg_QSend=NULL;
void   *ur_msgQId;
INT32U ur_msgId;
#endif

void uart0_set_ctrl(INT32U val);
INT32U uart0_get_ctrl(void);
INT32U 	uart0_get_status(void);
void 	uart0_fifo_ctrl(INT8U, INT8U);
void 	uart0_rx_tx_en(INT32U dir, INT32U enable);
void 	uart0_fifo_en(INT32U enable);
void 	uart0_fifo_enable(void);
void 	uart0_fifo_disable(void);

INT32S uart0_sw_fifo_init(void)
{
	INT32U i;

	for (i=0; i<UART0_FIFO_SIZE; i++) {
		sw_fifo_buf[i] = 0;
	}

	sw_fifo_head.p_start = &sw_fifo_buf[0];
	sw_fifo_head.p_end = &sw_fifo_buf[UART0_FIFO_SIZE-1];
	sw_fifo_head.p_write = &sw_fifo_buf[0];
	sw_fifo_head.p_read = &sw_fifo_buf[0];

	return 0;
}

INT32S uart0_sw_fifo_get(INT8U *data)
{
	// Only read p_write once, and only write p_read once
	INT8U *read_ptr;

	if (!data) {
		return -1;
	}
  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
  #endif
	if (sw_fifo_head.p_write == sw_fifo_head.p_read) {		// FIFO is empty
	  #if _OPERATING_SYSTEM != _OS_NONE
		OSSchedUnlock();
	  #endif
		return -2;
	}

	read_ptr = sw_fifo_head.p_read;
	*data = *read_ptr++;
	if (read_ptr > sw_fifo_head.p_end) {
		read_ptr = sw_fifo_head.p_start;
	}
	sw_fifo_head.p_read = read_ptr;
  #if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
  #endif

	return 0;
}

INT32S uart0_sw_fifo_put(INT8U data)		// Self protect FIFO
{
	// Only read p_read once, and only write p_write once
	INT8U *read_ptr, *write_ptr;

	read_ptr = sw_fifo_head.p_read - 1;
	if (read_ptr < sw_fifo_head.p_start) {
		read_ptr = sw_fifo_head.p_end;
	}
	write_ptr = sw_fifo_head.p_write;
	if (write_ptr == read_ptr) {			// FIFO is full
		return -2;
	}
	*write_ptr++ = data;
	if (write_ptr > sw_fifo_head.p_end) {
		write_ptr = sw_fifo_head.p_start;
	}
	sw_fifo_head.p_write = write_ptr;
	return 0;
}

void uart0_set_to_int(INT8U status)
{
	INT32U val;
	val = uart0_get_ctrl();

	if (status == UART_ENABLE) {
		val |= C_UART_CTRL_RXTO_INT;
	} else {
		val &= ~C_UART_CTRL_RXTO_INT;
	}

	uart0_set_ctrl(val);

}
INT32S uart_device_protect(void)
{
	return vic_irq_disable(VIC_UART);
}

void uart_device_unprotect(INT32S mask)
{
	if (mask == 0) {						// Clear device interrupt mask
		vic_irq_enable(VIC_UART);
	} else if (mask == 1) {
		vic_irq_disable(VIC_UART);
	} else {								// Something is wrong, do nothing
		return;
	}
}

void uart0_isr(void)
{
	INT32U status = 0;

  #if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
    INT8U  get_data=0;
  #endif

	status = R_UART_STATUS;

	if (status & C_UART_CTRL_RX_INT) {
    	while ((R_UART_STATUS & C_UART_STATUS_RX_EMPTY) == 0) {
        	uart0_sw_fifo_put((INT8U) R_UART_DATA);
    	}
    }

    if (status & C_UART_CTRL_RXTO_INT) {
    	while ((R_UART_STATUS & C_UART_STATUS_RX_EMPTY) == 0) {
        	uart0_sw_fifo_put((INT8U) R_UART_DATA);
    	}
    }

  #if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
    uart0_sw_fifo_get(&get_data);
    if (ur_msg_QSend)
    ur_msg_QSend(ur_msgQId, ur_msgId, &get_data, sizeof(INT8U), 0);
  #endif
}

void uart0_set_ctrl(INT32U val)
{
	// Bit15: Receive interrupt enable
	// Bit14: Transmit interrupt enable
	// Bit13: Receive timeout interrupt enable
	// Bit12: UART enable
	// Bit11: Modem status interrupt enable
	// Bit10: Self loop test enable(used in IrDA mode only)
	// Bit9-7: Reservede
	// Bit6-5: Word length, 00=5 bits, 01=6 bits, 10=7 bits, 11=8 bits
	// Bit4: FIFO enable
	// Bit3: Stop bit size, 0=1 bit, 1=2 bits
	// Bit2: Parity, 0=odd parity, 1=even parity
	// Bit1: Parity enable, 0=disable, 1=enable
	// Bit0: Send break, 0=normal, 1=send break signal
	R_UART_CTRL = val & 0xFC7F;
}

INT32U uart0_get_ctrl(void)
{
	return R_UART_CTRL;
}

void uart0_buad_rate_set(INT32U bps)
{
	R_UART_BAUD_RATE = MCLK / bps;
}

INT32U uart0_get_status(void)
{
	return (R_UART_STATUS & 0xE0FF);
}

void uart0_fifo_ctrl(INT8U tx_level, INT8U rx_level)
{
	INT32U val;

	val = ((INT32U) tx_level & 0x7) << 12;
	val |= ((INT32U) rx_level & 0x7) << 4;
	R_UART_FIFO = val;
}

void uart0_init(void)
{
	// 8 bits, 1 stop bit, no parity check, RX disabled
	R_UART_CTRL = (C_UART_CTRL_UART_ENABLE | C_UART_CTRL_WORD_8BIT);

#ifdef UART_PIN_POS
    #if UART_PIN_POS == UART_TX_IOH2__RX_IOH3
        R_GPIOCTRL = 0; /*switch off ice*/
        R_FUNPOS0 |= 1;
    #else
        R_FUNPOS0 &= ~(1);
    #endif
#endif

  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
  #endif
	uart0_tx_on = 0;						// TX is disabled by software
  #if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
  #endif

	uart0_sw_fifo_init();
    vic_irq_register(VIC_UART, uart0_isr);	// Non vector interrupt register
    vic_irq_enable(VIC_UART);
    uart0_set_to_int(UART_ENABLE); /* enable rx timeout interrupt */
}

void uart0_rx_tx_en(INT32U dir, INT32U enable)
{
  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
  #endif
	if (dir == UART_RX) {					// RX
		if (enable == UART_ENABLE) {
			R_UART_CTRL |= C_UART_CTRL_RX_INT;
		} else {
			R_UART_CTRL &= ~C_UART_CTRL_RX_INT;
		}
	} else {								// TX
		if (enable == UART_ENABLE) {
			uart0_tx_on = 1;
		} else {
			uart0_tx_on = 0;
		}
	}
  #if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
  #endif
}

void uart0_rx_enable(void)
{
	uart0_rx_tx_en(UART_RX, UART_ENABLE);
}

void uart0_rx_disable(void)
{
	uart0_rx_tx_en(UART_RX, UART_DISABLE);
}

void uart0_tx_enable(void)
{
	uart0_rx_tx_en(UART_TX, UART_ENABLE);
}

void uart0_tx_disable(void)
{
	uart0_rx_tx_en(UART_TX, UART_DISABLE);
}

void uart0_fifo_en(INT32U enable)
{
  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
  #endif
	if (enable == UART_ENABLE) {
		R_UART_CTRL |= C_UART_CTRL_FIFO_ENABLE;
	} else {
		R_UART_CTRL &= ~C_UART_CTRL_FIFO_ENABLE;
	}
  #if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
  #endif
}

void uart0_fifo_enable(void)
{
	uart0_fifo_en(UART_ENABLE);
}

void uart0_fifo_disable(void)
{
	uart0_fifo_en(UART_DISABLE);
}

void uart0_data_send(INT8U data, INT8U wait)
{
  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
	OSSchedLock();
  #endif
	if (uart0_tx_on == 0) {
	  #if _OPERATING_SYSTEM != _OS_NONE
		OSSchedUnlock();
	  #endif
	  	return;
	}
  #if _OPERATING_SYSTEM != _OS_NONE
	OSSchedUnlock();
  #endif

	if (wait) {
		while (R_UART_STATUS & C_UART_STATUS_TX_FULL) ;		// Wait until FIFO is not full
	}

	R_UART_DATA = data;						// Send our data now
}

INT32S uart0_data_get(INT8U *data, INT8U wait)
{
    while (uart0_sw_fifo_get(data)) {
    	if (!wait) {						// Queue is empty and the caller doesn't want to wait
    		return STATUS_FAIL;
    	}
    }
    return STATUS_OK;
}

INT32S uart0_word_len_set(INT8U word_len)
{
	R_UART_CTRL &= ~(3<<5);

	switch(word_len) {
		case WORD_LEN_5:
			R_UART_CTRL |= C_UART_CTRL_WORD_5BIT;
			break;
		case WORD_LEN_6:
			R_UART_CTRL |= C_UART_CTRL_WORD_6BIT;
			break;
		case WORD_LEN_7:
			R_UART_CTRL |= C_UART_CTRL_WORD_7BIT;
			break;
		case WORD_LEN_8:
			R_UART_CTRL |= C_UART_CTRL_WORD_8BIT;
			break;
		default :
			break;
	}

	return STATUS_OK;
}

INT32S uart0_stop_bit_size_set(INT8U stop_size)
{
	R_UART_CTRL &= ~(1<<3);

	if (stop_size == STOP_SIZE_2) {
		R_UART_CTRL |= C_UART_CTRL_2STOP_BIT;
	}

	return STATUS_OK;
}

INT32S uart0_parity_chk_set(INT8U status, INT8U parity)
{
	if (status == UART_DISABLE) {
		R_UART_CTRL &= ~(1<<1);
	}

	else {
		R_UART_CTRL |= C_UART_CTRL_PARITY_EN;
		R_UART_CTRL &= ~(1<<2);
		if (parity == PARITY_EVEN) {
			R_UART_CTRL |= C_UART_CTRL_EVEN_PARITY;
		}
	}

	return STATUS_OK;
}

/* project about resource register funtion */
#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
void turnkey_uart_resource_register(void *msg_qsend, void *msgq_id, INT32U msg_id)
{
	ur_msg_QSend = (fp_msg_qsend) msg_qsend;
	ur_msgQId = msgq_id;
	ur_msgId = msg_id;
}
#endif


#endif
