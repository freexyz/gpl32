/*
 ******************************************************************************
 * console.c
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
 *	2013.05.15	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include "application.h"

#define TASK_PRIO	41
#define STKSZ_SPIFLASH	4096
static INT32U		task_stack[STKSZ_SPIFLASH];

extern void		serial_init(void);
extern int		getc(void);


void task_console(void *para)
{
	int	c;

	DBG_PRINT("=== console task =======================================\r\n");
	serial_init();


	while (1) {
		c = getc();
		DBG_PRINT("type in = %c\r\n", (char) c);
	}
}

void console(void)
{
	/* create a user-defined task */
	OSTaskCreate(task_console, (void *) 0, &task_stack[STKSZ_SPIFLASH - 1], TASK_PRIO);
}

