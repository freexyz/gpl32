/*
 ******************************************************************************
 * zt3150.c -
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
 *	2013.11.27	T.C. Chiu	frist edition
 *
 ******************************************************************************
 */
#include "drv_l1_sensor.h"
#include "gplib.h"


#if (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1)
#ifdef __ZT3150_DRV_C__

/*
 * for debug
 */
#define DEBUG_ZT3150		1
#if DEBUG_ZT3150
    #define _D(x)		(x)
#else
    #define _D(x)
#endif

/*
 * definitions
 */
#define ZT3150_ID		0x04

#define ZT3150_VGA		1
#define ZT3150_1280x480		0

#define ZT3150_RESET		IO_A15

/*
 * macros
 */
#define sccb_rd_r16d8		sccb_read_Reg16Data8
#define sccb_wr_r16d8		sccb_write_Reg16Data8


/*
 *****************************************************************************
 * Description:	ZT3150 Initialization
 * Syntax: void zt3150_init(
 *			  signed short nWidthH,		// Active H Width
 *			  signed short nWidthV,		// Active V Width
 *			unsigned short uFlag		// Flag Type
 *			);
 * Return: None
 *****************************************************************************
 */
void zt3150_init(short nWidthH, short nWidthV,	unsigned short uFlag)
{
	INT16U	uCtrlReg1, uCtrlReg2;
	INT8U	inreso = 1;		//1:VGA, 0:QVGA
	INT8U	tmp;
	INT32S	nack;


	_D(DBG_PRINT("\033[1;36mZT3150 - mount\r\n\033[0m"));

	// Enable CSI clock to let sensor initialize at first
//	uCtrlReg2      = CLKOEN | CSI_RGB565 | CSI_HIGHPRI | CSI_NOSTOP | CLK_SEL48M;
	uCtrlReg2      = CLKOEN | CSI_RGB565 | CSI_HIGHPRI | CSI_NOSTOP | CLK_SEL27M;
//	R_SYSTEM_CTRL &= ~0x4000;	// (freq/1)
	R_SYSTEM_CTRL |= 0x4000;	// (freq/2)

	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;	// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555) {		// RGB1555
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656) {		// CCIR656
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;
		uCtrlReg2 |= D_TYPE1;
	} else {				// NOT CCIR656
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE;
		uCtrlReg2 |= D_TYPE0;
	}
	if (uFlag & FT_CSI_YUVIN) { 		// YUVIN?
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT) {		// YUVOUT?
		uCtrlReg1 |= YUVOUT;
	}

	// Whether compression or not?
	if ((nWidthH == 320) && (nWidthV == 240)) {
		if (inreso == 1) {
			// VGA in, QVGA out
			R_CSI_TG_HRATIO  = 0x0102;	// Scale to 1/2
			R_CSI_TG_VRATIO  = 0x0102;	// Scale to 1/2
			R_CSI_TG_HWIDTH  = nWidthH*2;	// Horizontal frame width
			R_CSI_TG_VHEIGHT = nWidthV*2;	// Vertical frame width
		}
	} else {
		R_CSI_TG_HRATIO  = 0;
		R_CSI_TG_VRATIO  = 0;
	}

	R_CSI_TG_VL0START = 0x0000;		// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;		//*P_Sensor_TG_V_L1Start = 0x0000;
	R_CSI_TG_HSTART   = 0x0000;		// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;			//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP | CLKOEN;	//enable CSI CLKO
	drv_msec_wait(100);

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);

	// reset
	_D(DBG_PRINT(" hard reset\r\n"));
	gpio_write_io(ZT3150_RESET, 1);
	gpio_init_io(ZT3150_RESET, GPIO_OUTPUT);
	gpio_drving_init_io(ZT3150_RESET, (IO_DRV_LEVEL) IO_DRIVING_4mA);

	gpio_write_io(ZT3150_RESET, 0);
	drv_msec_wait(400);
	gpio_write_io(ZT3150_RESET, 1);

	// 
	_D(DBG_PRINT(" wait ready\r\n"));
	for (;;) {
		nack = sccb_rd_r16d8(ZT3150_ID, 0x0080, &tmp);
		if (tmp == 0x80) {
			break;
		}
		drv_msec_wait(100);
	}

	_D(DBG_PRINT(" write command\r\n"));
#if (ZT3150_VGA)
	sccb_wr_r16d8(ZT3150_ID, 0x0081, 0x08);
	sccb_wr_r16d8(ZT3150_ID, 0x0082, 0x82);
#endif
#if (ZT3150_1280x480)
	sccb_wr_r16d8(ZT3150_ID, 0x0081, 0x08);
	sccb_wr_r16d8(ZT3150_ID, 0x0082, 0x00);
#endif
	sccb_wr_r16d8(ZT3150_ID, 0x0083, 0x00);

	sccb_wr_r16d8(ZT3150_ID, 0x0080, 0x01);
	drv_msec_wait(50);


	R_CSI_TG_CTRL1 = uCtrlReg2;			//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1;			//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
	_D(DBG_PRINT(" nWidthH=%04d, nWidthV=%04d, uFlag=0x%04x\r\n", nWidthH, nWidthV, uFlag));
	_D(DBG_PRINT(" R_CSI_TG_HRATIO  = 0x%04x, R_CSI_TG_HWIDTH  = %04d\r\n", (INT16U) R_CSI_TG_HRATIO, (INT16U) R_CSI_TG_HWIDTH));
	_D(DBG_PRINT(" R_CSI_TG_VRATIO  = 0x%04x, R_CSI_TG_VHEIGHT = %04d\r\n", (INT16U) R_CSI_TG_VRATIO, (INT16U) R_CSI_TG_VHEIGHT));
	_D(DBG_PRINT(" R_CSI_TG_CTRL0   = 0x%04x\r\n", (INT16U) uCtrlReg1));
	_D(DBG_PRINT(" R_CSI_TG_CTRL1   = 0x%04x\r\n", (INT16U) uCtrlReg2));
	_D(DBG_PRINT("\033[1;36mZT3150 - done\r\n\033[0m"));
}

/*
 *****************************************************************************
 * Eod Of File
 *****************************************************************************
 */
#endif	/* __ZT3150_DRV_C__ */
#endif	/* (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1) */
