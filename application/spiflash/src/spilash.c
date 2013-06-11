/*
 ******************************************************************************
 * spiflash.c
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

#include <ctype.h>
#include "spiflash.h"

#define TASK_PRIO	40
#define STKSZ_SPIFLASH	4096
static INT32U		task_stack[STKSZ_SPIFLASH];
static char		*spistr = { "Copyright (c) by ZealTek Electronic Co., Ltd." };
static INT8U		buf[8192];

extern int		print_buffer(unsigned long addr, void *data, unsigned int width, unsigned int count, unsigned int linelen);
extern void		SPI_FLASH_TEST(void);

void task_spiflash(void *para)
{

	INT8U		id;
	INT32S		ret;

//	DBG_PRINT("=== spiflash test ======================================\r\n");
//	SPI_FLASH_TEST();

	DBG_PRINT("=== spiflash task ======================================\r\n");
	gpio_drving_init_io(IO_C1, 3);
	gpio_drving_init_io(IO_C2, 3);
	gpio_drving_init_io(IO_C3, 3);

	SPI_Flash_init();

	while (1) {
		/* read ID */
		DBG_PRINT(" -read ID, ");
		ret = SPI_Flash_readID(&id);
		switch (ret) {
		case STATUS_OK:	DBG_PRINT("pass (0x%02X)\r\n", id);	break;
		case (-1):	DBG_PRINT("busy\r\n");			break;
		case (-2):	DBG_PRINT("timeout\r\n");		break;
		default:	DBG_PRINT("unknown (%d)\r\n", ret);	break;
		}

		/* block erase */
		DBG_PRINT(" -block erase, ");
		ret = SPI_Flash_erase_block((1 << 16));
		switch (ret) {
		case STATUS_OK:	DBG_PRINT("pass\r\n");			break;
		case (-1):	DBG_PRINT("busy\r\n");			break;
		case (-2):	DBG_PRINT("timeout\r\n");		break;
		default:	DBG_PRINT("unknown (%d)\r\n", ret);	break;
		}

		/* sector erase */
		DBG_PRINT(" -sector erase, ");
		ret = SPI_Flash_erase_sector((1 << 8));
		switch (ret) {
		case STATUS_OK:	DBG_PRINT("pass\r\n");			break;
		case (-1):	DBG_PRINT("busy\r\n");			break;
		case (-2):	DBG_PRINT("timeout\r\n");		break;
		default:	DBG_PRINT("unknown (%d)\r\n", ret);	break;
		}

		/* write page */
		DBG_PRINT(" -buffer content\r\n");
		gp_memset((INT8S *) buf, 0x55, sizeof(buf));
		print_buffer((unsigned long) buf, (void *) buf, 1, 256, 16);

		DBG_PRINT(" -write page, ");
		ret = SPI_Flash_write_page((1 << 8), buf);
		switch (ret) {
		case STATUS_OK:	DBG_PRINT("pass\r\n");			break;
		case (-1):	DBG_PRINT("busy\r\n");			break;
		case (-2):	DBG_PRINT("timeout\r\n");		break;
		default:	DBG_PRINT("unknown (%d)\r\n", ret);	break;
		}

		/* read page */
		DBG_PRINT(" -buffer content\r\n");
		gp_memset((INT8S *) buf, 0, sizeof(buf));
		print_buffer((unsigned long) buf, (void *) buf, 1, 256, 16);

		DBG_PRINT(" -read page, ");
		ret = SPI_Flash_read_page((1 << 8), buf);
		switch (ret) {
		case STATUS_OK:	DBG_PRINT("pass\r\n");			break;
		case (-1):	DBG_PRINT("busy\r\n");			break;
		case (-2):	DBG_PRINT("timeout\r\n");		break;
		default:	DBG_PRINT("unknown (%d)\r\n", ret);	break;
		}
		DBG_PRINT(" -buffer content\r\n");
		print_buffer((unsigned long) buf, (void *) buf, 1, 256, 16);

		/* suspend task */
		DBG_PRINT(" -spiflash task suspend !\r\n");
		OSTaskSuspend(TASK_PRIO);
	}
}

void spiflash(void)
{
	/* create a user-defined task */
	OSTaskCreate(task_spiflash, (void *) 0, &task_stack[STKSZ_SPIFLASH - 1], TASK_PRIO);


}

