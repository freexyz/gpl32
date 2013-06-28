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
#include "drv_l2_spi_flash.h"

#include "fd.h"
#include "frdbm.h"
#include "key.h"
#include "serial.h"


extern void		uart0_fifo_disable(void);

#define STKSZ_SERIAL	4096
static INT32U		task_stack[STKSZ_SERIAL];
static OS_EVENT		*serial_rx;
static unsigned char	msg;

/*
 * for debug
 */
#define TRACE_MSG		0

#if (TRACE_MSG)
    #define _MSG(x)		(x)
    #define _NORMAL(x)
#else
    #define _MSG(x)
    #define _NORMAL(x)		(x)
#endif


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void serial_isr(void)
{
	unsigned long	status = 0;

	status = R_UART_STATUS;
	if ((status & C_UART_CTRL_RX_INT) || (status & C_UART_CTRL_RXTO_INT)) {
		msg = (INT8U) R_UART_DATA;
		OSMboxPost(serial_rx, &msg);
	}
}

void serial_initial(void)
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
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
int getc(void)
{
	unsigned char	*tmp;
	unsigned char	err;

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


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
static void task_serial(void *para)
{
	int		c;
	unsigned char	s;

	serial_initial();
	while (1) {
		c = getc();

		switch (c & CMD_MASK) {
		case CMD_TRACK:		/* tracking mode */
			fd_chg_mode(MODE_STANDBY);
			break;

		case CMD_TRAIN: 	/* training mode */
			_MSG(c = 0xFF);
			frdb_set_c_train((c & ~CMD_MASK) >> 3);
			fd_chg_mode(MODE_TRAIN);
			break;

		case CMD_IDENT:		/* identify mode */
			_MSG(c = 0xFF);
			frdb_set_c_ident((c & ~CMD_MASK) >> 3);
			fd_chg_mode(MODE_IDENT);
			break;

		case CMD_ERASE:		/* erase */
			_MSG(DBG_PRINT("(frdb erase) Start\r\n"));
			_MSG(c = 0xFF);
			s = (unsigned char) (c & ~CMD_MASK);
			for (c=0; c<FRDB_NUM; c++) {
				if (s & 0x08) {
					frdb_erase(c);
				}
				s >>= 1;
			}
			_MSG(DBG_PRINT("(frdb erase) Done\r\n"));
			break;

		case CMD_STATE:		/* request database status */
			serial_send(STA_STATE, frdb_state());
			break;

		case 0:			/* security inc. */
			adjustSecurity_set(1);
			break;
		case 2:			/* security dec. */
			adjustSecurity_set(0);
			break;

		default:
			_MSG(DBG_PRINT("unknown command (0x%02x)\r\n", (unsigned char) c));
			break;
		}
	}
}

void serial_send(unsigned char status, unsigned long c)
{
	/* check nRTR pin, 0 = ready, 1 = busy */


	/* pack status */
	c = (c << 3) | (status & CMD_MASK);
	_NORMAL(putc((unsigned char) c));
	_MSG(DBG_PRINT("status = "));
	_MSG(DBG_PRINT("%0d", (c & 0x80) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x40) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x20) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x10) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x08) ? 1 : 0));
	_MSG(DBG_PRINT(" "));
	_MSG(DBG_PRINT("%0d", (c & 0x04) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x02) ? 1 : 0));
	_MSG(DBG_PRINT("%0d", (c & 0x01) ? 1 : 0));
	_MSG(DBG_PRINT("\r\n"));
}

void serial_init(unsigned char prio)
{
	/* create a user-defined task */
	OSTaskCreate(task_serial, (void *) 0, &task_stack[STKSZ_SERIAL - 1], prio);
}
