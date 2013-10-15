/*
 ******************************************************************************
 * fdmm.c
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
 *	2013.10.11	T.C. Chiu
 *
 ******************************************************************************
 */
#include "application.h"

#include "fd.h"
#include "fdosd.h"
#include "frdbm.h"
#include "serial.h"
#include "fdmm.h"


#define STKSZ_TASK		4096
static INT32U			task_stack[STKSZ_TASK];
static OS_EVENT			*fdmbox_wait;
static OS_EVENT			*fdmbox_ready;

static INT32U			fdmm_time;
static unsigned long		iobuf;



/*
 * for debug
 */
#define TRACE_MSG		1

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
 * I/O output function
 *
 *****************************************************************************
 */
static void _fdio_set(unsigned long n)
{
	/* indicates the current mode.
		00 : boot up
		01 : training mode
		10 : verification mode
		11 : tracking mode
	*/
	gpio_write_io(GPIO0, ((n & FDIO_GPIO0) ? 1 : 0));
	gpio_write_io(GPIO1, ((n & FDIO_GPIO1) ? 1 : 0));

	/* face is detected */
	gpio_write_io(GPIO2, ((n & FDIO_GPIO2) ? 1 : 0));

	/* a face is recognized or image of a face is taken in training mode */
	gpio_write_io(GPIO3, ((n & FDIO_GPIO3) ? 1 : 0));

	/* face data base is full */
	gpio_write_io(GPIO4, ((n & FDIO_GPIO4) ? 1 : 0));
}

void fdio_hi(unsigned long n)
{
	iobuf |= n;

	_fdio_set(iobuf);
}

void fdio_lo(unsigned long n)
{
	iobuf &= ~(n);

	_fdio_set(iobuf);
}

void fdio_toggle(unsigned long n)
{
	iobuf ^= n;

	_fdio_set(iobuf);
}

void fdio_set(unsigned long mask, unsigned long n)
{
	iobuf &= ~(mask);
	iobuf |= n;

	_fdio_set(iobuf);
}

/*
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
 */
static void fdio_init(unsigned long n)
{
	gpio_init_io(GPIO0, GPIO_OUTPUT);
	gpio_init_io(GPIO1, GPIO_OUTPUT);
	gpio_init_io(GPIO2, GPIO_OUTPUT);
	gpio_init_io(GPIO3, GPIO_OUTPUT);
	gpio_init_io(GPIO4, GPIO_OUTPUT);

	// set GPIO pad driving
	gpio_drving_init_io(GPIO0, (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(GPIO1, (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(GPIO2, (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(GPIO3, (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(GPIO4, (IO_DRV_LEVEL) IO_DRIVING_8mA);

	iobuf = n;
	_fdio_set(iobuf);
}


/*
 *****************************************************************************
 *
 * face detection operation mode manager
 *
 *****************************************************************************
 */
void fdmm_track(void)
{
	int	i, n;

	fdmode	      = FDMODE_TRACK;
	face_color[0] = 0x80;
	face_color[1] = 0x80;

	train_count   = 0;
	ident_count   = 0;

	/* check database is full/empty */
	for (i=0, n=0; i<FRDB_NUM; i++) {
		if (frdb_is_valid(i)) {
			n++;
		}
	}
	if (n) {
		fdio_hi(FDIO_GPIO4);
	} else {
		fdio_lo(FDIO_GPIO4);
	}

	fdosd_disp_track();
	fdio_set(FDIO_GPIO0|FDIO_GPIO1, FDIO_GPIO0 | FDIO_GPIO1);
}

void fdmm_train(unsigned long c)
{
	fdmode	      = FDMODE_TRAIN;
	face_color[0] = 0x00;
	face_color[1] = 0xFF;

	train_count   = 0;

	frdb_set_c_train(c);
	ownerULBP     = (INT32U *) frdb_get_c_train();
//	gp_memset((INT8S *) ownerULBP, 0, (LBP_SZ*LBP_NUM));

	fdosd_disp_train();
	fdio_set(FDIO_GPIO0|FDIO_GPIO1, FDIO_GPIO0);
}

void fdmm_recog(unsigned long c)
{
	fdmode	      = FDMODE_RECOG;
	face_color[0] = 0x00;
	face_color[1] = 0x00;

	ident_count   = 0;

	frdb_set_c_ident(c);
	ownerULBP     = (INT32U *) frdb_get_c_ident();

	fdosd_disp_recog();
	fdio_set(FDIO_GPIO0|FDIO_GPIO1, FDIO_GPIO1);
}

void fdmm_dummy(INT32U millisecond)
{
	fdmode	  = FDMODE_DUMMY;
	fdmm_time = millisecond;

	fdosd_disp_slp();			// for test

	// post wait signal
	OSMboxPost(fdmbox_wait, (void *) &fdmm_time);
}

INT32U fdmm_wait(void)
{
	INT32U	msg;

	// post ready to track mode
	msg = (INT32U) OSMboxAccept(fdmbox_ready);
	if (!msg) {
		return 0;
	}
	return msg;
}

void fdmm_err(void)
{
	face_color[0] = 0xFF;
	face_color[1] = 0x00;
}

void fdmm_task(void *para)
{
	INT8U	err;
	INT32U	*msg;


	/* initial global variable */
	fdmm_time = 0;

	/* create mail box */
	fdmbox_wait = OSMboxCreate(NULL);
	if (!fdmm) {
		DBG_PRINT("fdmm - create fdmbox_wait fail\r\n");
		return;
	}
	fdmbox_ready = OSMboxCreate(NULL);
	if (!fdmm) {
		DBG_PRINT("fdmm - create fdmbox_ready fail\r\n");
		return;
	}

	/* task main loop */
	while (1) {
		msg = (INT32U *) OSMboxPend(fdmbox_wait, 0, &err);
		if ((err != OS_NO_ERR) || !msg) {
			continue;
		}

		if (*msg == 0) {
			*msg = 100;
		}
		if (*msg > 60*1000-1) {
			*msg = 1*1000;
		}
		OSTimeDlyHMSM(0, 0, *msg / 1000, *msg % 1000);

		OSMboxPost(fdmbox_ready, (void *) msg);
	}
}


void fdmm(unsigned char prio, unsigned long gpio)
{
	fdio_init(gpio);

	/* create a user-defined task */
	OSTaskCreate(fdmm_task, (void *) 0, &task_stack[STKSZ_TASK - 1], prio);
}

