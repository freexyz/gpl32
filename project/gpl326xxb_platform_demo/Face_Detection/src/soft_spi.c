/*
 ******************************************************************************
 * soft_spi.c
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
 *	2013.10.03	T.C. Chiu	first edition
 *
 *
 ******************************************************************************
 */
#include "application.h"

/*
 * for debug
 */
#define TRACE_MSG		0
#if (TRACE_MSG)
    #define _M(x)		(x)
#else
    #define _M(x)
#endif


/*
 * i/o assignment
 */
#define SSPI_CS			IO_A8
#define SSPI_CLK		IO_A9
#define SSPI_MOSI		IO_A10
#define SSPI_MISO		IO_A11

#define FPGA_INT		IO_A15

#define sspi_dly()		{ sspi_cpu_dly(1); }
//#define sspi_dly()		{ drv_msec_wait(1); }

#define sspi_cs_lo()		{ gpio_write_io(SSPI_CS,   0); }
#define sspi_cs_hi()		{ gpio_write_io(SSPI_CS,   1); }

#define sspi_clk_lo()		{ gpio_write_io(SSPI_CLK,  0); }
#define sspi_clk_hi()		{ gpio_write_io(SSPI_CLK,  1); }

#define sspi_mosi_lo()		{ gpio_write_io(SSPI_MOSI, 0); }
#define sspi_mosi_hi()		{ gpio_write_io(SSPI_MOSI, 1); }

#define sspi_miso()		gpio_read_io(SSPI_MISO)

#define fpga_int()		gpio_read_io(FPGA_INT)

/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void sspi_cpu_dly(unsigned long i) 
{
	unsigned long j;

	for (j=0; j<(i<<8); j++) {
		i = i;
	}
}


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
unsigned char sspi_txrx(unsigned char value)
{
	int		i;
	unsigned char	tmp;


	for (i=0, tmp=0; i<8; i++) {
		/* assign mosi */
		if (value & 0x80) {
			sspi_mosi_hi();
		} else {
			sspi_mosi_lo();
		}
		value <<= 1;

		/* read miso */
		tmp <<= 1;
		tmp |= sspi_miso();

		/* assign clock */
		sspi_clk_hi();
		sspi_dly();
		sspi_clk_lo();
		sspi_dly();
	}
	return tmp;
}

void sspi_write(unsigned char add, unsigned short value)
{
	if (add >= 0x80) {
		_M(DBG_PRINT("sspi - address value over range\r\n"));
		return;
	}
	add &= 0x7F;

	/* assign cs = low */
	sspi_cs_lo();

	/* transceive 8 bits */
	sspi_txrx(add);

	/* transceive 8 bits */
	sspi_txrx((unsigned char) ((value & 0xFF00) >> 8));

	/* transceive 8 bits */
	sspi_txrx((unsigned char) (value & 0x00FF));

	/* assign cs = high */
	sspi_cs_hi();
}

unsigned short sspi_read(unsigned char add)
{
	unsigned short	tmp;

	if (add >= 0x80) {
		_M(DBG_PRINT("sspi - address value over range\r\n"));
		return 0;
	}
	add |= 0x80;

	/* assign cs = low */
	sspi_cs_lo();

	/* transceive 8 bits */
	sspi_txrx(add);

	/* transceive 8 bits */
	tmp  = ((unsigned short) sspi_txrx(0)) << 8;

	/* transceive 8 bits */
	tmp |= (unsigned short) sspi_txrx(0);

	/* assign cs = high */
	sspi_cs_hi();

	return tmp;
}


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void sspi_test(void)
{
	int		i;
	unsigned short	x, y;
	unsigned short	tbl[] = { 0x0000, 0x0100, 0x5502, 0x3055, 0xAA04, 0x50AA,
				  0x5A06, 0x075A, 0x0008, 0x09FF, 0xEE0A };
	unsigned short	buf[11];

	DBG_PRINT("\033[1;36m(sspi)\033[0m test start...\r\n");
	sspi_cs_hi();
	sspi_clk_lo();

	gpio_init_io(SSPI_CS,   GPIO_OUTPUT);
	gpio_init_io(SSPI_CLK,  GPIO_OUTPUT);
	gpio_init_io(SSPI_MOSI, GPIO_OUTPUT);
	gpio_init_io(SSPI_MISO, GPIO_INPUT);

	gpio_drving_init_io(SSPI_CS,   (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(SSPI_CLK,  (IO_DRV_LEVEL) IO_DRIVING_8mA);
	gpio_drving_init_io(SSPI_MOSI, (IO_DRV_LEVEL) IO_DRIVING_8mA);

	for (i=2; i<11; i++) {
		buf[i] = sspi_read(i);
		DBG_PRINT("  (sspi) read, add=0x%02x value=0x%04x\r\n", i, buf[i]);
	}

	DBG_PRINT("(sspi) write test\r\n");
	for (i=2; i<11; i++) {
		sspi_write(i, tbl[i]);
		DBG_PRINT("  (sspi) write, add=0x%02x value=0x%04x\r\n", i, tbl[i]);
	}

	DBG_PRINT("(sspi) read test\r\n");
	buf[0] = 0x0000;
	buf[1] = 0x0100;
	for (i=2; i<11; i++) {
		buf[i] = sspi_read(i);
		DBG_PRINT("  (sspi) read, add=0x%02x value=0x%04x, %s\r\n", i, buf[i], (tbl[i] == buf[i]) ? "pass" : "fail");
	}

	DBG_PRINT("\033[1;36m(sspi)\033[0m test end...\r\n");

	DBG_PRINT("\033[1;36m(int)\033[0m INT start...\r\n");
	i = 0;
	while (1) {
		if (fpga_int()) {
			if (i<64) {
				x = sspi_read(0);
				y = sspi_read(1);
				DBG_PRINT("  (int) %02d, x=0x%04x y=0x%04x\r\n", i, x, y);
				i++;
			} else {
				break;
			}
		}
	}
	DBG_PRINT("\033[1;36m(int)\033[0m INT end...\r\n");
}


