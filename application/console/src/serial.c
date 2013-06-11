/*
 ******************************************************************************
 * serial.c
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.05.16	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "application.h"
#include "drv_l1_uart.h"


extern void		uart0_fifo_disable(void);

OS_EVENT		*serial_rx;
INT8U			msg;

/*
 *
 *
 *
 */
void serial_isr(void)
{
	INT32U	status = 0;

	status = R_UART_STATUS;
	if ((status & C_UART_CTRL_RX_INT) || (status & C_UART_CTRL_RXTO_INT)) {
		msg = (INT8U) R_UART_DATA;
		OSMboxPost(serial_rx, &msg);
	}
}

void serial_init(void)
{
	serial_rx = OSMboxCreate(NULL);
	if (!serial_rx) {
		return;
	}

	vic_irq_disable(VIC_UART);

	uart0_fifo_disable();
	vic_irq_register(VIC_UART, serial_isr);

	vic_irq_enable(VIC_UART);
}


/*
 *
 *
 *
 */
int getc(void)
{
	INT8U	*tmp;
	INT8U	err;

	tmp = (INT8U *) OSMboxPend(serial_rx, 0, &err);
	return (int) *tmp;
}

void putc(const char c)
{
	uart0_data_send(c, 1);
}

int tstc(void)
{
	int	tmp;

	tmp = 0;
	return tmp;
}

void puts(const char *s)
{
	while (*s)
		uart0_data_send(*s, 1);
}

