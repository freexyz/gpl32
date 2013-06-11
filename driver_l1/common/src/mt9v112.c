/*
 ******************************************************************************
 * mt9v112.c -
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
 *	2013.05.24	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "drv_l1_sensor.h"
#include "gplib.h"


#if (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1)
#ifdef __MT9V112_DRV_C__

/*
 * for debug
 */
#define DEBUG_MT9V112			1
#if DEBUG_MT9V112
    #define _D(x)			(x)
#else
    #define _D(x)
#endif

/*
 * definitions
 */
#define MT9V112_ID		0xBA	// 0x90

/*
 * macros
 */
#define sccb_rd_r8d16			sccb_read_Reg8Data16
#define sccb_wr_r8d16			sccb_write_Reg8Data16

#define sccb_set_r8d16(id, reg, mask)	\
{					\
	INT16U	d;			\
	sccb_rd_r8d16(id, reg, &d);	\
	d |= ((INT16U)  (mask));	\
	sccb_wr_r8d16(id, reg, d);	\
}

#define sccb_clr_r8d16(id, reg, mask)	\
{					\
	INT16U	d;			\
	sccb_rd_r8d16(id, reg, &d);	\
	d &= ((INT16U) ~(mask));	\
	sccb_wr_r8d16(id, reg, d);	\
}

/*
 *****************************************************************************
 * Description:	MT9V112 Initialization
 * Syntax: void mt9v112_init(
 *			  signed short nWidthH,		// Active H Width
 *			  signed short nWidthV,		// Active V Width
 *			unsigned short uFlag		// Flag Type
 *			);
 * Return: None
 *****************************************************************************
 */
void mt9v112_init(short nWidthH, short nWidthV,	unsigned short uFlag)
{
	INT16U	uCtrlReg1, uCtrlReg2, tmp;
	INT8U	inreso = 1;		//1:VGA, 0:QVGA

	_D(DBG_PRINT(" MT9V112 - mount\r\n"));

	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2      = CLKOEN | CSI_RGB565 | CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000;
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2      = CLKOEN | CSI_RGB565 | CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000;
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2      = CLKOEN | CSI_RGB565 | CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000;
#elif CSI_CLOCK == CSI_CLOCK_13_5MHz
	uCtrlReg2      = CLKOEN | CSI_RGB565 | CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000;
#endif

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
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0000);
	sccb_set_r8d16(MT9V112_ID, 0x0D, 0x0001);	// reset sensor
	sccb_set_r8d16(MT9V112_ID, 0x0D, 0x0020);	// reset soc
	sccb_clr_r8d16(MT9V112_ID, 0x0D, 0x0021);	// resume both
	drv_msec_wait(100);

	_D(sccb_rd_r8d16(MT9V112_ID, 0x00, &tmp));
	_D(DBG_PRINT("  R0:0=0x%04x\r\n", (INT16U) tmp));

	// Timing Settings 27MHz
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0000);
	sccb_wr_r8d16(MT9V112_ID, 0x05, 0x014C);	// Context B (full-res) Horizontal Blank
	sccb_wr_r8d16(MT9V112_ID, 0x06, 0x000D);	// Context B (full-res) Vertical Blank
	sccb_wr_r8d16(MT9V112_ID, 0x07, 0x014C);	// Context A (preview) Horizontal Blank
	sccb_wr_r8d16(MT9V112_ID, 0x08, 0x000D);	// Context A (preview) Vertical Blank
	sccb_wr_r8d16(MT9V112_ID, 0x20, 0x0700);	// Read Mode Context B
	sccb_wr_r8d16(MT9V112_ID, 0x21, 0x0400);	// Read Mode Context A
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x39, 0x03D4);	// AE Line size Context A
	sccb_wr_r8d16(MT9V112_ID, 0x3A, 0x03D4);	// AE Line size Context B
	sccb_wr_r8d16(MT9V112_ID, 0x3B, 0x03D4);	// AE shutter delay limit Context A
	sccb_wr_r8d16(MT9V112_ID, 0x3C, 0x03D4);	// AE shutter delay limit Context B
	sccb_wr_r8d16(MT9V112_ID, 0x57, 0x01CB);	// Context A Flicker full frame time (60Hz)
	sccb_wr_r8d16(MT9V112_ID, 0x58, 0x0227);	// Context A Flicker full frame time (50Hz)
	sccb_wr_r8d16(MT9V112_ID, 0x59, 0x01CB);	// Context B Flicker full frame time (60Hz)
	sccb_wr_r8d16(MT9V112_ID, 0x5A, 0x0227);	// Context B Flicker full frame time (50Hz)
	sccb_wr_r8d16(MT9V112_ID, 0x5C, 0x100B);	// 60Hz Flicker Search Range
	sccb_wr_r8d16(MT9V112_ID, 0x5D, 0x140F);	// 50Hz Flicker Search Range
	sccb_wr_r8d16(MT9V112_ID, 0x64, 0x5E1C);	// Flicker parameter

	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0000);
	sccb_wr_r8d16(MT9V112_ID,   53, 0x2029);
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_wr_r8d16(MT9V112_ID,    6, 0x600E);

	sccb_wr_r8d16(MT9V112_ID, 0x3A, 0x0201);	// Output Format Control (A)
	sccb_wr_r8d16(MT9V112_ID, 0x9B, 0x0201);	// Output Format Control (B)

	// AWB Defaults
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID,   31, 0x0090);
	sccb_wr_r8d16(MT9V112_ID,   34, 0x9080);
	sccb_wr_r8d16(MT9V112_ID,   35, 0x8878);
	sccb_wr_r8d16(MT9V112_ID,   40, 0xEF02);
	sccb_wr_r8d16(MT9V112_ID,   41, 0x867A);

	// Enable Auto Sharpening
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_set_r8d16(MT9V112_ID,    5, 0x0008);
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x2E, 0x0C44);

	// gamma 0.45 BC 8 JPEG
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_wr_r8d16(MT9V112_ID, 0x34, 0x0000);	// LUMA_OFFSET
	sccb_wr_r8d16(MT9V112_ID, 0x35, 0xFF00);	// (CLIPPING_LIM_OUT_LUMA
	sccb_wr_r8d16(MT9V112_ID, 0x53, 0x1A08);	// GAMMA_A_Y1_Y2
	sccb_wr_r8d16(MT9V112_ID, 0x54, 0x603D);	// GAMMA_A_Y3_Y4
	sccb_wr_r8d16(MT9V112_ID, 0x55, 0xAB8C);	// GAMMA_A_Y5_Y6
	sccb_wr_r8d16(MT9V112_ID, 0x56, 0xDAC4);	// GAMMA_A_Y7_Y8
	sccb_wr_r8d16(MT9V112_ID, 0x57, 0xFFED);	// GAMMA_A_Y9_Y10
	sccb_wr_r8d16(MT9V112_ID, 0xDC, 0x1A08);	// GAMMA_B_Y1_Y2
	sccb_wr_r8d16(MT9V112_ID, 0xDD, 0x603D);	// GAMMA_B_Y3_Y4
	sccb_wr_r8d16(MT9V112_ID, 0xDE, 0xAB8C);	// GAMMA_B_Y5_Y6
	sccb_wr_r8d16(MT9V112_ID, 0xDF, 0xDAC4);	// GAMMA_B_Y7_Y8
	sccb_wr_r8d16(MT9V112_ID, 0xE0, 0xFFED);	// GAMMA_B_Y9_Y10

	// Fast CCM
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x24, 0x4000);	// MATRIX_ADJ_LIMITS
	sccb_wr_r8d16(MT9V112_ID, 0xF5, 0x0020);	// MWB POSITION
	sccb_wr_r8d16(MT9V112_ID, 0x5E, 0x4962);	// RATIO_BASE_REG
	sccb_wr_r8d16(MT9V112_ID, 0x5F, 0x7A58);	// RATIO_DELTA_REG
	sccb_wr_r8d16(MT9V112_ID, 0x60, 0x0002);	// SIGNS_DELTA_REG
	sccb_wr_r8d16(MT9V112_ID, 0x02, 0x00EA);	// BASE_MATRIX_SIGNS
	sccb_wr_r8d16(MT9V112_ID, 0x03, 0x291A);	// BASE_MATRIX_SCALE_K1_K5
	sccb_wr_r8d16(MT9V112_ID, 0x04, 0x04A4);	// BASE_MATRIX_SCALE_K6_K9
	sccb_wr_r8d16(MT9V112_ID, 0x09, 0x0097);	// BASE_MATRIX_COEF_K1
	sccb_wr_r8d16(MT9V112_ID, 0x0A, 0x0072);	// BASE_MATRIX_COEF_K2
	sccb_wr_r8d16(MT9V112_ID, 0x0B, 0x001E);	// BASE_MATRIX_COEF_K3
	sccb_wr_r8d16(MT9V112_ID, 0x0C, 0x001D);	// BASE_MATRIX_COEF_K4
	sccb_wr_r8d16(MT9V112_ID, 0x0D, 0x007E);	// BASE_MATRIX_COEF_K5
	sccb_wr_r8d16(MT9V112_ID, 0x0E, 0x0072);	// BASE_MATRIX_COEF_K6
	sccb_wr_r8d16(MT9V112_ID, 0x0F, 0x0011);	// BASE_MATRIX_COEF_K7
	sccb_wr_r8d16(MT9V112_ID, 0x10, 0x0034);	// BASE_MATRIX_COEF_K8
	sccb_wr_r8d16(MT9V112_ID, 0x11, 0x0082);	// BASE_MATRIX_COEF_K9
	sccb_wr_r8d16(MT9V112_ID, 0x15, 0x0111);	// DELTA_COEFS_SIGNS
	sccb_wr_r8d16(MT9V112_ID, 0x16, 0x003A);	// DELTA_MATRIX_COEF_D1
	sccb_wr_r8d16(MT9V112_ID, 0x17, 0x003B);	// DELTA_MATRIX_COEF_D2
	sccb_wr_r8d16(MT9V112_ID, 0x18, 0x0022);	// DELTA_MATRIX_COEF_D3
	sccb_wr_r8d16(MT9V112_ID, 0x19, 0x0051);	// DELTA_MATRIX_COEF_D4
	sccb_wr_r8d16(MT9V112_ID, 0x1A, 0x002B);	// DELTA_MATRIX_COEF_D5
	sccb_wr_r8d16(MT9V112_ID, 0x1B, 0x0032);	// DELTA_MATRIX_COEF_D6
	sccb_wr_r8d16(MT9V112_ID, 0x1C, 0x0071);	// DELTA_MATRIX_COEF_D7
	sccb_wr_r8d16(MT9V112_ID, 0x1D, 0x00BB);	// DELTA_MATRIX_COEF_D8
	sccb_wr_r8d16(MT9V112_ID, 0x1E, 0x00CB);	// DELTA_MATRIX_COEF_D9
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_set_r8d16(MT9V112_ID,    6, 0x8000);
	drv_msec_wait(100);
	sccb_clr_r8d16(MT9V112_ID,    6, 0x8000);

	// Vivid Mode
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_wr_r8d16(MT9V112_ID, 0x25, 0x002D);	// AWB_SPEED_SATURATION

	// Pixel Noise Reduction
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0000);
	sccb_wr_r8d16(MT9V112_ID, 0x34, 0xC019);
	sccb_wr_r8d16(MT9V112_ID, 0x40, 0x1800);
	sccb_wr_r8d16(MT9V112_ID, 0x76, 0x7358);
	sccb_wr_r8d16(MT9V112_ID, 0x04, 642);		// Column Width
	sccb_wr_r8d16(MT9V112_ID, 0x03, 482);		// Row Width
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_wr_r8d16(MT9V112_ID, 0xA0, 642);
	sccb_wr_r8d16(MT9V112_ID, 0xA3, 482);
	sccb_wr_r8d16(MT9V112_ID, 0xA6, 642);		// Reducer Horizontal Zoom Resize
	sccb_wr_r8d16(MT9V112_ID, 0xA9, 482);		// Reducer Vertical Zoom Resize

	// Row Noise Reduction
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0000);
	sccb_wr_r8d16(MT9V112_ID, 0x5F, 0x3630);
	sccb_wr_r8d16(MT9V112_ID, 0x30, 0x043E);
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0001);
	sccb_wr_r8d16(MT9V112_ID, 0x3B, 0x043E);
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x2E, 0x0000);
	drv_msec_wait(100);
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x3D, 0x17DD);
	sccb_wr_r8d16(MT9V112_ID, 0x2E, 0x0C44);

	// Set Day Mode
	sccb_wr_r8d16(MT9V112_ID, 0xF0, 0x0002);
	sccb_wr_r8d16(MT9V112_ID, 0x37, 0x0080);
	sccb_wr_r8d16(MT9V112_ID, 0x2E, 0x0000);
	drv_msec_wait(200);
	sccb_wr_r8d16(MT9V112_ID, 0x2E, 0x0C44);

	// Global Context Control
	if (inreso == 1) {
		sccb_set_r8d16(MT9V112_ID, 0xC8, 0x870B);
	}

	// Resync FPS reporting
	drv_msec_wait(100);

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
	_D(DBG_PRINT("  nWidthH=%04d, nWidthV=%04d, uFlag=0x%04x\r\n", nWidthH, nWidthV, uFlag));
	_D(DBG_PRINT("  R_CSI_TG_HRATIO  = 0x%04x, R_CSI_TG_HWIDTH  = 0x%04x\r\n", (INT16U) R_CSI_TG_HRATIO, (INT16U) R_CSI_TG_HWIDTH));
	_D(DBG_PRINT("  R_CSI_TG_VRATIO  = 0x%04x, R_CSI_TG_VHEIGHT = 0x%04x\r\n", (INT16U) R_CSI_TG_VRATIO, (INT16U) R_CSI_TG_VHEIGHT));
	_D(DBG_PRINT("  R_CSI_TG_CTRL0   = 0x%04x\r\n", (INT16U) uCtrlReg1));
	_D(DBG_PRINT("  R_CSI_TG_CTRL1   = 0x%04x\r\n", (INT16U) uCtrlReg2));
	_D(DBG_PRINT(" MT9V112 - done\r\n"));
}

/*
 *****************************************************************************
 * Eod Of File
 *****************************************************************************
 */
#endif	/* __MT9V112_DRV_C__ */
#endif	/* (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1) */
