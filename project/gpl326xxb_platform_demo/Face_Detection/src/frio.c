/*
 ******************************************************************************
 * frio.c
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
 *	2013.06.03	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "application.h"
#include "frio.h"

#define STKSZ_FRIO		4096
static INT32U			task_stack[STKSZ_FRIO];
static OS_EVENT			*frio;
static unsigned long		iobuf;

/*
 * for debug
 */
#define TRACE_MSG		0

#if (TRACE_MSG)
    #define _M(x)		(x)
#else
    #define _M(x)
#endif

// Status IO assignment.
#define GPIO0			IO_A8
#define GPIO1			IO_A9
#define GPIO2			IO_A10
#define GPIO3			IO_A11
#define GPIO4			IO_A12
#define GPIO5			IO_A13
#define GPIO6			IO_A14
#define GPIO7			IO_A15


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
static void _frio_set(unsigned long n)
{
	/* indicates the current mode.
		00 : boot up
		01 : training mode
		10 : verification mode
		11 : tracking mode
	*/
	gpio_write_io(GPIO0, ((n & FRIO_GPIO0) ? 1 : 0));
	gpio_write_io(GPIO1, ((n & FRIO_GPIO1) ? 1 : 0));

	/* face is detected */
	gpio_write_io(GPIO2, ((n & FRIO_GPIO2) ? 1 : 0));

	/* a face is recognized or image of a face is taken in training mode */
	gpio_write_io(GPIO3, ((n & FRIO_GPIO3) ? 1 : 0));

	/* face data base is full */
	gpio_write_io(GPIO4, ((n & FRIO_GPIO4) ? 1 : 0));

	_M(DBG_PRINT("GPIO = 0x%08x\r\n", n));
}

void frio_hi(unsigned long n)
{
	iobuf |= n;

	_frio_set(iobuf);
}

void frio_lo(unsigned long n)
{
	iobuf &= ~(n);

	_frio_set(iobuf);
}

void frio_toggle(unsigned long n)
{
	iobuf ^= n;

	_frio_set(iobuf);
}

void frio_set(unsigned long n, unsigned long mask)
{
	iobuf &= ~(mask);
	iobuf |= n;

	_frio_set(iobuf);
}

void frio_hi_latency(unsigned long n, unsigned long time)
{
	iobuf |= n;

	OSMboxPost(frio, &time);
}


/*
 *****************************************************************************
 * bitmap
 *	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	|3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1|0|0|0|0|0|0|0|0|0|0|
 *	|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 *	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *	| | | | | | | | | | | | | | | | | | | | | | | | |G|G|G|G|G|G|G|G|
 *	| | | | | | | | | | | | | | | | | | | | | | | | |P|P|P|P|P|P|P|P|
 *	| | | | | | | | | | | | | | | | | | | | | | | | |I|I|I|I|I|I|I|I|
 *	| | | | | | | | | | | | | | | | | | | | | | | | |O|O|O|O|O|O|O|O|
 *	| | | | | | | | | | | | | | | | | | | | | | | | |7|6|5|4|3|2|1|O|
 *	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *****************************************************************************
 */
static void task_frio(void *para)
{
	unsigned char	err;
	unsigned long	*tmp;

	gpio_init_io(GPIO0, GPIO_OUTPUT);
	gpio_init_io(GPIO1, GPIO_OUTPUT);
	gpio_init_io(GPIO2, GPIO_OUTPUT);
	gpio_init_io(GPIO3, GPIO_OUTPUT);
	gpio_init_io(GPIO4, GPIO_OUTPUT);

	gpio_drving_init_io(GPIO0, (IO_DRV_LEVEL) IO_DRIVING_16mA);	// IO_DRIVING_16mA
	gpio_drving_init_io(GPIO1, (IO_DRV_LEVEL) IO_DRIVING_16mA);	// IO_DRIVING_16mA
	gpio_drving_init_io(GPIO2, (IO_DRV_LEVEL) IO_DRIVING_16mA);	// IO_DRIVING_16mA
	gpio_drving_init_io(GPIO3, (IO_DRV_LEVEL) IO_DRIVING_16mA);	// IO_DRIVING_16mA
	gpio_drving_init_io(GPIO4, (IO_DRV_LEVEL) IO_DRIVING_16mA);	// IO_DRIVING_16mA

	frio = OSMboxCreate(NULL);
	if (!frio) {
		_M(DBG_PRINT("frio - create fail\r\n"));
		return;
	}

	iobuf = 0;
	while (1) {
		tmp = (unsigned long *) OSMboxPend(frio, 0, &err);

		if (iobuf & FRIO_GPIO3) {
			OSTimeDlyHMSM(0, 0, 0, 300);
			_M(DBG_PRINT("*tmp = %03d\r\n", *tmp));

			iobuf &= ~(FRIO_GPIO3);
		}

		_frio_set(iobuf);
	}
}

void frio_init(unsigned char prio)
{
	/* create a user-defined task */
	OSTaskCreate(task_frio, (void *) 0, &task_stack[STKSZ_FRIO - 1], prio);
}

