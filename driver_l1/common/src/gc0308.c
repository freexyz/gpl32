/*
 ******************************************************************************
 * gc0308.c -
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
 *	2013.05.31	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "drv_l1_sensor.h"
#include "gplib.h"


#if (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1)
#ifdef __GC0308_DRV_C__

/*
 * for debug
 */
#define DEBUG_GC0308		1
#if DEBUG_GC0308
    #define _D(x)		(x)
#else
    #define _D(x)
#endif

/*
 * definitions
 */
#define GC0308_ID		0x42

/*
 * macros
 */
#define sccb_rd_r8d8		sccb_read_Reg8Data8
#define sccb_wr_r8d8		sccb_write_Reg8Data8

/*
 *****************************************************************************
 * Description:	GC0308 Initialization
 * Syntax: void gc0308_init(
 *			  signed short nWidthH,		// Active H Width
 *			  signed short nWidthV,		// Active V Width
 *			unsigned short uFlag		// Flag Type
 *			);
 * Return: None
 *****************************************************************************
 */
void gc0308_init(short nWidthH, short nWidthV,	unsigned short uFlag)
{
	INT16U	uCtrlReg1, uCtrlReg2;
	INT8U	tmp, inreso = 1;	//1:VGA, 0:QVGA

	_D(DBG_PRINT("\033[1;36mGC0308 - mount\r\n\033[0m"));

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
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;
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

	_D(sccb_rd_r8d8(GC0308_ID, 0x00, &tmp));
	_D(DBG_PRINT(" P0:0=0x%02x\r\n", (unsigned char) tmp));

	// 640x480 init registers code
	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x00);	// set page0

	sccb_wr_r8d8(GC0308_ID, 0xd2, 0x10);	// close AEC
	sccb_wr_r8d8(GC0308_ID, 0x22, 0x55);	// close AWB
	
	sccb_wr_r8d8(GC0308_ID, 0x5a, 0x56);
	sccb_wr_r8d8(GC0308_ID, 0x5b, 0x40);
	sccb_wr_r8d8(GC0308_ID, 0x5c, 0x4a);
	
	sccb_wr_r8d8(GC0308_ID, 0x22, 0x57);	// open AWB

#if 1
	sccb_wr_r8d8(GC0308_ID, 0x01, 0xce);	//hb[7:0]
	sccb_wr_r8d8(GC0308_ID, 0x02, 0x70);	//vb[7:0]
	sccb_wr_r8d8(GC0308_ID, 0x0f, 0x00);	//[7:4]VB_High_4bit, [3:0]HB_High_4bit
	sccb_wr_r8d8(GC0308_ID, 0xe2, 0x00);
	sccb_wr_r8d8(GC0308_ID, 0xe3, 0x96);	//anti-flicker step [7:0]
	sccb_wr_r8d8(GC0308_ID, 0xe4, 0x02);	//exp level 1
	sccb_wr_r8d8(GC0308_ID, 0xe5, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xe6, 0x02);	//exp level 2
	sccb_wr_r8d8(GC0308_ID, 0xe7, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xe8, 0x02);	//exp level 3
	sccb_wr_r8d8(GC0308_ID, 0xe9, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xea, 0x0c);	//exp level 4
	sccb_wr_r8d8(GC0308_ID, 0xeb, 0xbe);
	sccb_wr_r8d8(GC0308_ID, 0xec, 0x20);	//[5:4]exp_level  [3:0]minimum exposure high 4 bits
#else
	sccb_wr_r8d8(GC0308_ID, 0x01, 0x6a);	//hb[7:0]
	sccb_wr_r8d8(GC0308_ID, 0x02, 0x70);	//vb[7:0]
	sccb_wr_r8d8(GC0308_ID, 0x0f, 0x00);	//[7:4]VB_High_4bit, [3:0]HB_High_4bit
	sccb_wr_r8d8(GC0308_ID, 0xe2, 0x00);
	sccb_wr_r8d8(GC0308_ID, 0xe3, 0x96);	//anti-flicker step [7:0]
	sccb_wr_r8d8(GC0308_ID, 0xe4, 0x02);	//exp level 1
	sccb_wr_r8d8(GC0308_ID, 0xe5, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xe6, 0x02);	//exp level 2
	sccb_wr_r8d8(GC0308_ID, 0xe7, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xe8, 0x02);	//exp level 3
	sccb_wr_r8d8(GC0308_ID, 0xe9, 0x58);
	sccb_wr_r8d8(GC0308_ID, 0xea, 0x04);	//exp level 4
	sccb_wr_r8d8(GC0308_ID, 0xeb, 0xb0);
	sccb_wr_r8d8(GC0308_ID, 0xec, 0x20);	//[5:4]exp_level  [3:0]minimum exposure high 4 bits
#endif

	sccb_wr_r8d8(GC0308_ID, 0x05, 0x00);	// row_start_high
	sccb_wr_r8d8(GC0308_ID, 0x06, 0x00);	// row_start_low
	sccb_wr_r8d8(GC0308_ID, 0x07, 0x00);	// col_start_high
	sccb_wr_r8d8(GC0308_ID, 0x08, 0x00);	// col_start_low
	sccb_wr_r8d8(GC0308_ID, 0x09, 0x01);	//[8]cis_win_height  488
	sccb_wr_r8d8(GC0308_ID, 0x0a, 0xe8);	//[7:0]cis_win_height
	sccb_wr_r8d8(GC0308_ID, 0x0b, 0x02);	//[9:8]cis_win_width 648
	sccb_wr_r8d8(GC0308_ID, 0x0c, 0x88);	//[7:0]cis_win_width
	sccb_wr_r8d8(GC0308_ID, 0x0d, 0x02);	//vs_st
	sccb_wr_r8d8(GC0308_ID, 0x0e, 0x02);	//vs_et
	sccb_wr_r8d8(GC0308_ID, 0x10, 0x26);	////[7:4]restg_width, [3:0]sh_width
	sccb_wr_r8d8(GC0308_ID, 0x11, 0xfd);	//fd//[7:4]tx_width, [3:0]space width,*2
	sccb_wr_r8d8(GC0308_ID, 0x12, 0x2a);	//sh_delay
	sccb_wr_r8d8(GC0308_ID, 0x13, 0x00);	//[3:0] row_tail_width

	sccb_wr_r8d8(GC0308_ID, 0x14, 0x12);	//[7]hsync_always ,[6] NA,  [5:4] CFA sequence

	sccb_wr_r8d8(GC0308_ID, 0x15, 0x0a);	//[7:6]output_mode,,[5:4]restg_mode,[3:2]sdark_mode, [1]new exposure,[0]badframe_en
	sccb_wr_r8d8(GC0308_ID, 0x16, 0x05);	//[7:5]NA, [4]capture_ad_data_edge, [3:0]Number of A/D pipe stages
	sccb_wr_r8d8(GC0308_ID, 0x17, 0x01);	//[7:6]analog_opa_r,[5]coltest_en, [4]ad_test_enable,
	sccb_wr_r8d8(GC0308_ID, 0x18, 0x44);	//[7]NA,  [6:4]column gain ee, [3]NA, [2:0]column gain eo
	sccb_wr_r8d8(GC0308_ID, 0x19, 0x44);	//[7]NA,  [6:4]column gain oe, [3]NA, [2:0]column gain oo
	sccb_wr_r8d8(GC0308_ID, 0x1a, 0x1e);	//1e//[7]rsv1,[6]rsv0, [5:4]coln_r,
	sccb_wr_r8d8(GC0308_ID, 0x1b, 0x00);	//[7:2]NA, [1:0]BIN4 AND BIN2
	sccb_wr_r8d8(GC0308_ID, 0x1c, 0xc1);	//c1//[7]hrst_enbale, [6:4]da_rsg, [3]tx high enable, [2]NA, [1:0]da18_r
	sccb_wr_r8d8(GC0308_ID, 0x1d, 0x08);	//08//[7]vref_en, [6:4]da_vef, [3]da25_en, [2]NA, [1:0]da25_r,set da25 voltage
	sccb_wr_r8d8(GC0308_ID, 0x1e, 0x60);	//60//[7]LP_MTD,[6:5]opa_r,ADC's operating current,  [4:2]NA, [1:0]sref
	sccb_wr_r8d8(GC0308_ID, 0x1f, 0x16);	//[7:6]NA, [5:4]sync_drv, [3:2]data_drv, [1:0]pclk_drv

	sccb_wr_r8d8(GC0308_ID, 0x20, 0xff);	//[7]bks[6]gamma[5]cc[4]ee[3]intp[2]dn[1]dd[0]lsc
	sccb_wr_r8d8(GC0308_ID, 0x21, 0xfa);	//[7]na[6]na[5]skin_ee[4]cbcr_hue_en[3]y_as_en[2]auto_gray_en[1]y_gamma_en[0]na
	sccb_wr_r8d8(GC0308_ID, 0x22, 0x57);	//[7]na [6]auto_dndd [5]auto_ee [4]auto_sa [3]na [2]abs [1]awb  [0]na
	sccb_wr_r8d8(GC0308_ID, 0x24, 0xa2);	//
	sccb_wr_r8d8(GC0308_ID, 0x25, 0x0f);

	// output sync_mode
	sccb_wr_r8d8(GC0308_ID, 0x26, 0x02);	//
	sccb_wr_r8d8(GC0308_ID, 0x2f, 0x01);	//debug mode3
	sccb_wr_r8d8(GC0308_ID, 0x30, 0xf7);	//blk mode [7]dark current mode:1 use exp rated dark ,0 use ndark row calculated
	sccb_wr_r8d8(GC0308_ID, 0x31, 0x50);	//blk_value limit.64 low align to 11bits;8 for 256 range
	sccb_wr_r8d8(GC0308_ID, 0x32, 0x00);	//global offset
	sccb_wr_r8d8(GC0308_ID, 0x39, 0x04);	// exp_ate_darkc
	sccb_wr_r8d8(GC0308_ID, 0x3a, 0x18);	//{7:6}offset submode {5:0}offset ratio
	sccb_wr_r8d8(GC0308_ID, 0x3b, 0x20);	//{7:6}darkc submode {5:0}dark current ratio
	sccb_wr_r8d8(GC0308_ID, 0x3c, 0x00);	//manual g1 offset
	sccb_wr_r8d8(GC0308_ID, 0x3d, 0x00);	//manual r offset
	sccb_wr_r8d8(GC0308_ID, 0x3e, 0x00);	//manual b offset
	sccb_wr_r8d8(GC0308_ID, 0x3f, 0x00);	//manual g2 offset
	sccb_wr_r8d8(GC0308_ID, 0x50, 0x10);	//10  //global gain
	sccb_wr_r8d8(GC0308_ID, 0x53, 0x82);	//G
	sccb_wr_r8d8(GC0308_ID, 0x54, 0x80);	//R channel gain
	sccb_wr_r8d8(GC0308_ID, 0x55, 0x80);	//B channel gain
	sccb_wr_r8d8(GC0308_ID, 0x56, 0x82);

	sccb_wr_r8d8(GC0308_ID, 0x57, 0x80);	//R
	sccb_wr_r8d8(GC0308_ID, 0x58, 0x80);	//G
	sccb_wr_r8d8(GC0308_ID, 0x59, 0x80);	//B

	sccb_wr_r8d8(GC0308_ID, 0x8b, 0x40);	//r2
	sccb_wr_r8d8(GC0308_ID, 0x8c, 0x40);	//g2
	sccb_wr_r8d8(GC0308_ID, 0x8d, 0x40);	//b2
	sccb_wr_r8d8(GC0308_ID, 0x8e, 0x2e);	//r4
	sccb_wr_r8d8(GC0308_ID, 0x8f, 0x2e);	//g4
	sccb_wr_r8d8(GC0308_ID, 0x90, 0x2e);	//b4
	sccb_wr_r8d8(GC0308_ID, 0x91, 0x3c);	//[7]singed4 [6:0]row_cneter
	sccb_wr_r8d8(GC0308_ID, 0x92, 0x50);	//col_center
	sccb_wr_r8d8(GC0308_ID, 0x5d, 0x12);	//decrease 1
	sccb_wr_r8d8(GC0308_ID, 0x5e, 0x1a);	//decrease 2
	sccb_wr_r8d8(GC0308_ID, 0x5f, 0x24);	//decrease 3
	sccb_wr_r8d8(GC0308_ID, 0x60, 0x07);	//[4]zero weight mode
	sccb_wr_r8d8(GC0308_ID, 0x61, 0x15);	//[7:6]na
	sccb_wr_r8d8(GC0308_ID, 0x62, 0x08);	//b base
	sccb_wr_r8d8(GC0308_ID, 0x64, 0x03);	//[7:4]n base [3:0]c weight
	sccb_wr_r8d8(GC0308_ID, 0x66, 0xe8);	//dark_th ,bright_th
	sccb_wr_r8d8(GC0308_ID, 0x67, 0x86);	//flat high, flat low
	sccb_wr_r8d8(GC0308_ID, 0x68, 0xa2);	//[7:4]dd limit [1:0]dd ratio
	sccb_wr_r8d8(GC0308_ID, 0x69, 0x18);	//gain high th
	sccb_wr_r8d8(GC0308_ID, 0x6a, 0x0f);	//[7:4]dn_c slop          //[3]use post_gain [2]use pre_gain [1]use global gain [0]use col gain
	sccb_wr_r8d8(GC0308_ID, 0x6b, 0x00);	//[7:4]dn_b slop [3:0]dn_n slop
	sccb_wr_r8d8(GC0308_ID, 0x6c, 0x5f);	//[7:4]bright_th start [3:0]bright_th slop
	sccb_wr_r8d8(GC0308_ID, 0x6d, 0x8f);	//[7:4]dd_limit_start[3:0]dd_limit slop
	sccb_wr_r8d8(GC0308_ID, 0x6e, 0x55);	//[7:4]ee1 effect start [3:0]slope  broad
	sccb_wr_r8d8(GC0308_ID, 0x6f, 0x38);	//[7:4]ee2 effect start [3:0]slope  narrow
	sccb_wr_r8d8(GC0308_ID, 0x70, 0x15);	//saturation dec slope
	sccb_wr_r8d8(GC0308_ID, 0x71, 0x33);	//[7:4]low limit,[3:0]saturation slope
	sccb_wr_r8d8(GC0308_ID, 0x72, 0xdc);	//[7]edge_add_mode [6]new edge mode [5]edge2_mode [4]HP_mode
	sccb_wr_r8d8(GC0308_ID, 0x73, 0x80);	//[7]edge_add_mode2 [6]NA [5]only 2direction [4]fixed direction th
	sccb_wr_r8d8(GC0308_ID, 0x74, 0x02);	//direction th1
	sccb_wr_r8d8(GC0308_ID, 0x75, 0x3f);	//direction th2
	sccb_wr_r8d8(GC0308_ID, 0x76, 0x02);	//direction diff th      h>v+diff ; h>th1 ; v<th2
	sccb_wr_r8d8(GC0308_ID, 0x77, 0x36);	//[7:4]edge1_effect [3:0]edge2_effect
	sccb_wr_r8d8(GC0308_ID, 0x78, 0x88);	//[7:4]edge_pos_ratio  [3:0]edge neg ratio
	sccb_wr_r8d8(GC0308_ID, 0x79, 0x81);	//edge1_max,edge1_min
	sccb_wr_r8d8(GC0308_ID, 0x7a, 0x81);	//edge2_max,edge2_min
	sccb_wr_r8d8(GC0308_ID, 0x7b, 0x22);	//edge1_th,edge2_th
	sccb_wr_r8d8(GC0308_ID, 0x7c, 0xff);	//pos_edge_max,neg_edge_max
	sccb_wr_r8d8(GC0308_ID, 0x93, 0x48);	// <--40
	sccb_wr_r8d8(GC0308_ID, 0x94, 0x00);
	sccb_wr_r8d8(GC0308_ID, 0x95, 0x05);
	sccb_wr_r8d8(GC0308_ID, 0x96, 0xe8);
	sccb_wr_r8d8(GC0308_ID, 0x97, 0x40);	//
	sccb_wr_r8d8(GC0308_ID, 0x98, 0xf0);
	sccb_wr_r8d8(GC0308_ID, 0xb1, 0x38);	//manual cb
	sccb_wr_r8d8(GC0308_ID, 0xb2, 0x38);	//manual cr
	sccb_wr_r8d8(GC0308_ID, 0xbd, 0x38);
	sccb_wr_r8d8(GC0308_ID, 0xbe, 0x36);	// [5:4]gray mode 00:4&8  01:4&12 10:4&20  11:8$16   [3:0] auto_gray
	sccb_wr_r8d8(GC0308_ID, 0xd0, 0xc9);	// exp is gc mode
	sccb_wr_r8d8(GC0308_ID, 0xd1, 0x10);	//every N
//	sccb_wr_r8d8(GC0308_ID, 0xd2, 0x90);	// 7 aec enable 5 clore y mode 4skin weight 3 weight drop mode
//	sccb_wr_r8d8(GC0308_ID, 0xd3, 0x80);	//Y_target and low pixel thd high X4 low X2
	sccb_wr_r8d8(GC0308_ID, 0xd3, 0xa0);
	sccb_wr_r8d8(GC0308_ID, 0xd5, 0xf2);	//lhig
	sccb_wr_r8d8(GC0308_ID, 0xd6, 0x16);	// ignore mode
	sccb_wr_r8d8(GC0308_ID, 0xdb, 0x92);
	sccb_wr_r8d8(GC0308_ID, 0xdc, 0xa5);	//fast_margin  fast_ratio
	sccb_wr_r8d8(GC0308_ID, 0xdf, 0x23);	// I_fram D_ratio
	sccb_wr_r8d8(GC0308_ID, 0xd9, 0x00);	// colore offset in CAL ,now is too dark so set zero
	sccb_wr_r8d8(GC0308_ID, 0xda, 0x00);	// GB offset
	sccb_wr_r8d8(GC0308_ID, 0xe0, 0x09);
	sccb_wr_r8d8(GC0308_ID, 0xec, 0x20);
	sccb_wr_r8d8(GC0308_ID, 0xed, 0x04);	//minimum exposure low 8  bits
	sccb_wr_r8d8(GC0308_ID, 0xee, 0xa0);	//max_post_dg_gain
	sccb_wr_r8d8(GC0308_ID, 0xef, 0x40);	//max_pre_dg_gain
	sccb_wr_r8d8(GC0308_ID, 0x80, 0x03);
	sccb_wr_r8d8(GC0308_ID, 0x80, 0x03);

#if 1	//smallest gamma curve
	sccb_wr_r8d8(GC0308_ID, 0x9F, 0x0B);
	sccb_wr_r8d8(GC0308_ID, 0xA0, 0x16);
	sccb_wr_r8d8(GC0308_ID, 0xA1, 0x29);
	sccb_wr_r8d8(GC0308_ID, 0xA2, 0x3C);
	sccb_wr_r8d8(GC0308_ID, 0xA3, 0x4F);
	sccb_wr_r8d8(GC0308_ID, 0xA4, 0x5F);
	sccb_wr_r8d8(GC0308_ID, 0xA5, 0x6F);
	sccb_wr_r8d8(GC0308_ID, 0xA6, 0x8A);
	sccb_wr_r8d8(GC0308_ID, 0xA7, 0x9F);
	sccb_wr_r8d8(GC0308_ID, 0xA8, 0xB4);
	sccb_wr_r8d8(GC0308_ID, 0xA9, 0xC6);
	sccb_wr_r8d8(GC0308_ID, 0xAA, 0xD3);
	sccb_wr_r8d8(GC0308_ID, 0xAB, 0xDD);
	sccb_wr_r8d8(GC0308_ID, 0xAC, 0xE5);
	sccb_wr_r8d8(GC0308_ID, 0xAD, 0xF1);
	sccb_wr_r8d8(GC0308_ID, 0xAE, 0xFA);
	sccb_wr_r8d8(GC0308_ID, 0xAF, 0xFF);
#elif 0
	sccb_wr_r8d8(GC0308_ID, 0x9F, 0x0E);
	sccb_wr_r8d8(GC0308_ID, 0xA0, 0x1C);
	sccb_wr_r8d8(GC0308_ID, 0xA1, 0x34);
	sccb_wr_r8d8(GC0308_ID, 0xA2, 0x48);
	sccb_wr_r8d8(GC0308_ID, 0xA3, 0x5A);
	sccb_wr_r8d8(GC0308_ID, 0xA4, 0x6B);
	sccb_wr_r8d8(GC0308_ID, 0xA5, 0x7B);
	sccb_wr_r8d8(GC0308_ID, 0xA6, 0x95);
	sccb_wr_r8d8(GC0308_ID, 0xA7, 0xAB);
	sccb_wr_r8d8(GC0308_ID, 0xA8, 0xBF);
	sccb_wr_r8d8(GC0308_ID, 0xA9, 0xCE);
	sccb_wr_r8d8(GC0308_ID, 0xAA, 0xD9);
	sccb_wr_r8d8(GC0308_ID, 0xAB, 0xE4);
	sccb_wr_r8d8(GC0308_ID, 0xAC, 0xEC);
	sccb_wr_r8d8(GC0308_ID, 0xAD, 0xF7);
	sccb_wr_r8d8(GC0308_ID, 0xAE, 0xFD);
	sccb_wr_r8d8(GC0308_ID, 0xAF, 0xFF);
#elif 0
	sccb_wr_r8d8(GC0308_ID, 0x9F, 0x10);
	sccb_wr_r8d8(GC0308_ID, 0xA0, 0x20);
	sccb_wr_r8d8(GC0308_ID, 0xA1, 0x38);
	sccb_wr_r8d8(GC0308_ID, 0xA2, 0x4E);
	sccb_wr_r8d8(GC0308_ID, 0xA3, 0x63);
	sccb_wr_r8d8(GC0308_ID, 0xA4, 0x76);
	sccb_wr_r8d8(GC0308_ID, 0xA5, 0x87);
	sccb_wr_r8d8(GC0308_ID, 0xA6, 0xA2);
	sccb_wr_r8d8(GC0308_ID, 0xA7, 0xB8);
	sccb_wr_r8d8(GC0308_ID, 0xA8, 0xCA);
	sccb_wr_r8d8(GC0308_ID, 0xA9, 0xD8);
	sccb_wr_r8d8(GC0308_ID, 0xAA, 0xE3);
	sccb_wr_r8d8(GC0308_ID, 0xAB, 0xEB);
	sccb_wr_r8d8(GC0308_ID, 0xAC, 0xF0);
	sccb_wr_r8d8(GC0308_ID, 0xAD, 0xF8);
	sccb_wr_r8d8(GC0308_ID, 0xAE, 0xFD);
	sccb_wr_r8d8(GC0308_ID, 0xAF, 0xFF);
#elif 0
	sccb_wr_r8d8(GC0308_ID, 0x9F, 0x14);
	sccb_wr_r8d8(GC0308_ID, 0xA0, 0x28);
	sccb_wr_r8d8(GC0308_ID, 0xA1, 0x44);
	sccb_wr_r8d8(GC0308_ID, 0xA2, 0x5D);
	sccb_wr_r8d8(GC0308_ID, 0xA3, 0x72);
	sccb_wr_r8d8(GC0308_ID, 0xA4, 0x86);
	sccb_wr_r8d8(GC0308_ID, 0xA5, 0x95);
	sccb_wr_r8d8(GC0308_ID, 0xA6, 0xB1);
	sccb_wr_r8d8(GC0308_ID, 0xA7, 0xC6);
	sccb_wr_r8d8(GC0308_ID, 0xA8, 0xD5);
	sccb_wr_r8d8(GC0308_ID, 0xA9, 0xE1);
	sccb_wr_r8d8(GC0308_ID, 0xAA, 0xEA);
	sccb_wr_r8d8(GC0308_ID, 0xAB, 0xF1);
	sccb_wr_r8d8(GC0308_ID, 0xAC, 0xF5);
	sccb_wr_r8d8(GC0308_ID, 0xAD, 0xFB);
	sccb_wr_r8d8(GC0308_ID, 0xAE, 0xFE);
	sccb_wr_r8d8(GC0308_ID, 0xAF, 0xFF);
#else	// largest gamma curve
	sccb_wr_r8d8(GC0308_ID, 0x9F, 0x15);
	sccb_wr_r8d8(GC0308_ID, 0xA0, 0x2A);
	sccb_wr_r8d8(GC0308_ID, 0xA1, 0x4A);
	sccb_wr_r8d8(GC0308_ID, 0xA2, 0x67);
	sccb_wr_r8d8(GC0308_ID, 0xA3, 0x79);
	sccb_wr_r8d8(GC0308_ID, 0xA4, 0x8C);
	sccb_wr_r8d8(GC0308_ID, 0xA5, 0x9A);
	sccb_wr_r8d8(GC0308_ID, 0xA6, 0xB3);
	sccb_wr_r8d8(GC0308_ID, 0xA7, 0xC5);
	sccb_wr_r8d8(GC0308_ID, 0xA8, 0xD5);
	sccb_wr_r8d8(GC0308_ID, 0xA9, 0xDF);
	sccb_wr_r8d8(GC0308_ID, 0xAA, 0xE8);
	sccb_wr_r8d8(GC0308_ID, 0xAB, 0xEE);
	sccb_wr_r8d8(GC0308_ID, 0xAC, 0xF3);
	sccb_wr_r8d8(GC0308_ID, 0xAD, 0xFA);
	sccb_wr_r8d8(GC0308_ID, 0xAE, 0xFD);
	sccb_wr_r8d8(GC0308_ID, 0xAF, 0xFF);
#endif

	sccb_wr_r8d8(GC0308_ID, 0xc0, 0x00);	//Y_gamma_0
	sccb_wr_r8d8(GC0308_ID, 0xc1, 0x10);	//Y_gamma_1
	sccb_wr_r8d8(GC0308_ID, 0xc2, 0x1c);	//Y_gamma_2
	sccb_wr_r8d8(GC0308_ID, 0xc3, 0x30);	//Y_gamma_3
	sccb_wr_r8d8(GC0308_ID, 0xc4, 0x43);	//Y_gamma_4
	sccb_wr_r8d8(GC0308_ID, 0xc5, 0x54);	//Y_gamma_5
	sccb_wr_r8d8(GC0308_ID, 0xc6, 0x65);	//Y_gamma_6
	sccb_wr_r8d8(GC0308_ID, 0xc7, 0x75);	//Y_gamma_7
	sccb_wr_r8d8(GC0308_ID, 0xc8, 0x93);	//Y_gamma_8
	sccb_wr_r8d8(GC0308_ID, 0xc9, 0xb0);	//Y_gamma_9
	sccb_wr_r8d8(GC0308_ID, 0xca, 0xcb);	//Y_gamma_10
	sccb_wr_r8d8(GC0308_ID, 0xcb, 0xe6);	//Y_gamma_11
	sccb_wr_r8d8(GC0308_ID, 0xcc, 0xff);	//Y_gamma_12
	sccb_wr_r8d8(GC0308_ID, 0xf0, 0x02);
	sccb_wr_r8d8(GC0308_ID, 0xf1, 0x01);
	sccb_wr_r8d8(GC0308_ID, 0xf2, 0x01);	//manual stretch K
	sccb_wr_r8d8(GC0308_ID, 0xf3, 0x30);	//the limit of Y_stretch
	sccb_wr_r8d8(GC0308_ID, 0xf9, 0x9f);
	sccb_wr_r8d8(GC0308_ID, 0xfa, 0x78);

	//---------------------------------------------------------------
	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x01);	//set page 1

	sccb_wr_r8d8(GC0308_ID, 0x00, 0xf5);	//high_low limit
	sccb_wr_r8d8(GC0308_ID, 0x02, 0x1a);	//y2c
	sccb_wr_r8d8(GC0308_ID, 0x0a, 0xa0);	// number limit
	sccb_wr_r8d8(GC0308_ID, 0x0b, 0x60);	// skip_mode
	sccb_wr_r8d8(GC0308_ID, 0x0c, 0x08);
	sccb_wr_r8d8(GC0308_ID, 0x0e, 0x4c);	 // width step
	sccb_wr_r8d8(GC0308_ID, 0x0f, 0x39);	 // height step

	sccb_wr_r8d8(GC0308_ID, 0x11, 0x3f);	  // 0x37

	sccb_wr_r8d8(GC0308_ID, 0x12, 0x72);
	sccb_wr_r8d8(GC0308_ID, 0x13, 0x13);	//13//smooth 2
	sccb_wr_r8d8(GC0308_ID, 0x14, 0x42);	//R_5k_gain_base
	sccb_wr_r8d8(GC0308_ID, 0x15, 0x43);	//B_5k_gain_base
	sccb_wr_r8d8(GC0308_ID, 0x16, 0xc2);	//sinT
	sccb_wr_r8d8(GC0308_ID, 0x17, 0xa8);	//ac//a8//a8//a8//cosT
	sccb_wr_r8d8(GC0308_ID, 0x18, 0x18);	//X1 thd
	sccb_wr_r8d8(GC0308_ID, 0x19, 0x40);	//X2 thd
	sccb_wr_r8d8(GC0308_ID, 0x1a, 0xd0);	//e4//d0//Y1 thd
	sccb_wr_r8d8(GC0308_ID, 0x1b, 0xf5);	//Y2 thd
	sccb_wr_r8d8(GC0308_ID, 0x70, 0x40);	 // A R2G low
	sccb_wr_r8d8(GC0308_ID, 0x71, 0x58);	 // A R2G high
	sccb_wr_r8d8(GC0308_ID, 0x72, 0x30);	 // A B2G low
	sccb_wr_r8d8(GC0308_ID, 0x73, 0x48);	 // A B2G high
	sccb_wr_r8d8(GC0308_ID, 0x74, 0x20);	 // A G low
	sccb_wr_r8d8(GC0308_ID, 0x75, 0x60);	 // A G high
	sccb_wr_r8d8(GC0308_ID, 0x77, 0x20);
	sccb_wr_r8d8(GC0308_ID, 0x78, 0x32);
	sccb_wr_r8d8(GC0308_ID, 0x30, 0x03);	//[1]HSP_en [0]sa_curve_en
	sccb_wr_r8d8(GC0308_ID, 0x31, 0x40);
	sccb_wr_r8d8(GC0308_ID, 0x32, 0xe0);
	sccb_wr_r8d8(GC0308_ID, 0x33, 0xe0);
	sccb_wr_r8d8(GC0308_ID, 0x34, 0xe0);
	sccb_wr_r8d8(GC0308_ID, 0x35, 0xb0);
	sccb_wr_r8d8(GC0308_ID, 0x36, 0xc0);
	sccb_wr_r8d8(GC0308_ID, 0x37, 0xc0);
	sccb_wr_r8d8(GC0308_ID, 0x38, 0x04);	//sat1, at8
	sccb_wr_r8d8(GC0308_ID, 0x39, 0x09);
	sccb_wr_r8d8(GC0308_ID, 0x3a, 0x12);
	sccb_wr_r8d8(GC0308_ID, 0x3b, 0x1c);
	sccb_wr_r8d8(GC0308_ID, 0x3c, 0x28);
	sccb_wr_r8d8(GC0308_ID, 0x3d, 0x31);
	sccb_wr_r8d8(GC0308_ID, 0x3e, 0x44);
	sccb_wr_r8d8(GC0308_ID, 0x3f, 0x57);
	sccb_wr_r8d8(GC0308_ID, 0x40, 0x6c);
	sccb_wr_r8d8(GC0308_ID, 0x41, 0x81);
	sccb_wr_r8d8(GC0308_ID, 0x42, 0x94);
	sccb_wr_r8d8(GC0308_ID, 0x43, 0xa7);
	sccb_wr_r8d8(GC0308_ID, 0x44, 0xb8);
	sccb_wr_r8d8(GC0308_ID, 0x45, 0xd6);
	sccb_wr_r8d8(GC0308_ID, 0x46, 0xee);	//sat15,at224
	sccb_wr_r8d8(GC0308_ID, 0x47, 0x0d);	//blue_edge_dec_ratio

	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x00);

	sccb_wr_r8d8(GC0308_ID, 0xd2, 0x90); // Open AEC at last.  

	/////////////////////////////////////////////////////////
	//-----------Update the registers -------------//
	///////////////////////////////////////////////////////////

	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x00);	//set Page0

	sccb_wr_r8d8(GC0308_ID, 0x10, 0x26);                               
	sccb_wr_r8d8(GC0308_ID, 0x11, 0x0d);	// fd,modified by mormo 2010/07/06                               
	sccb_wr_r8d8(GC0308_ID, 0x1a, 0x2a);	// 1e,modified by mormo 2010/07/06                                  

	sccb_wr_r8d8(GC0308_ID, 0x1c, 0x49);	// c1,modified by mormo 2010/07/06                                 
	sccb_wr_r8d8(GC0308_ID, 0x1d, 0x9a);	// 08,modified by mormo 2010/07/06                                 
	sccb_wr_r8d8(GC0308_ID, 0x1e, 0x61);	// 60,modified by mormo 2010/07/06                                 
//	sccb_wr_r8d8(GC0308_ID, 0x1f, 0x16);	// io driver current 
	
	sccb_wr_r8d8(GC0308_ID, 0x3a, 0x20);

	sccb_wr_r8d8(GC0308_ID, 0x50, 0x14);	// 10,modified by mormo 2010/07/06                               
	sccb_wr_r8d8(GC0308_ID, 0x53, 0x80);                                
	sccb_wr_r8d8(GC0308_ID, 0x56, 0x80);

	sccb_wr_r8d8(GC0308_ID, 0x8b, 0x20);	//LSC                                 
	sccb_wr_r8d8(GC0308_ID, 0x8c, 0x20);                                
	sccb_wr_r8d8(GC0308_ID, 0x8d, 0x20);                                
	sccb_wr_r8d8(GC0308_ID, 0x8e, 0x14);                                
	sccb_wr_r8d8(GC0308_ID, 0x8f, 0x10);                                
	sccb_wr_r8d8(GC0308_ID, 0x90, 0x14);                                

	sccb_wr_r8d8(GC0308_ID, 0x94, 0x02);                                
	sccb_wr_r8d8(GC0308_ID, 0x95, 0x07);                                
	sccb_wr_r8d8(GC0308_ID, 0x96, 0xe0);                                

	sccb_wr_r8d8(GC0308_ID, 0xb1, 0x40);	// YCPT                                 
	sccb_wr_r8d8(GC0308_ID, 0xb2, 0x40);                                
	sccb_wr_r8d8(GC0308_ID, 0xb3, 0x40);
	sccb_wr_r8d8(GC0308_ID, 0xb6, 0xe0);

	sccb_wr_r8d8(GC0308_ID, 0xd0, 0xcb);	// AECT  c9,modifed by mormo 2010/07/06                                
	sccb_wr_r8d8(GC0308_ID, 0xd3, 0x48);	// 80,modified by mormor 2010/07/06                           

	sccb_wr_r8d8(GC0308_ID, 0xf2, 0x02);                                
	sccb_wr_r8d8(GC0308_ID, 0xf7, 0x12);
	sccb_wr_r8d8(GC0308_ID, 0xf8, 0x0a);

	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x01);	//set  Page1

	sccb_wr_r8d8(GC0308_ID, 0x02, 0x20);
	sccb_wr_r8d8(GC0308_ID, 0x04, 0x10);
	sccb_wr_r8d8(GC0308_ID, 0x05, 0x08);
	sccb_wr_r8d8(GC0308_ID, 0x06, 0x20);
	sccb_wr_r8d8(GC0308_ID, 0x08, 0x0a);

	sccb_wr_r8d8(GC0308_ID, 0x0e, 0x44);                                
	sccb_wr_r8d8(GC0308_ID, 0x0f, 0x32);
	sccb_wr_r8d8(GC0308_ID, 0x10, 0x41);                                
	sccb_wr_r8d8(GC0308_ID, 0x11, 0x37);                                
	sccb_wr_r8d8(GC0308_ID, 0x12, 0x22);                                
	sccb_wr_r8d8(GC0308_ID, 0x13, 0x19);                                
	sccb_wr_r8d8(GC0308_ID, 0x14, 0x44);                                
	sccb_wr_r8d8(GC0308_ID, 0x15, 0x44);

	sccb_wr_r8d8(GC0308_ID, 0x19, 0x50);                                
	sccb_wr_r8d8(GC0308_ID, 0x1a, 0xd8); 

	sccb_wr_r8d8(GC0308_ID, 0x32, 0x10); 

	sccb_wr_r8d8(GC0308_ID, 0x35, 0x00);                                
	sccb_wr_r8d8(GC0308_ID, 0x36, 0x80);                                
	sccb_wr_r8d8(GC0308_ID, 0x37, 0x00); 
	//-----------Update the registers end---------//

	sccb_wr_r8d8(GC0308_ID, 0xfe, 0x00);	// set back for page1

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
	_D(DBG_PRINT("\033[1;36mGC0308 - done\r\n\033[0m"));
}

/*
 *****************************************************************************
 * Eod Of File
 *****************************************************************************
 */
#endif	/* __GC0308_DRV_C__ */
#endif	/* (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1) */
