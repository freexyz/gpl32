/* 
* Purpose: TFT driver/interface
*
* Author: Allen Lin
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_tft.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_TFT) && (_DRV_L1_TFT == 1)                   //
//================================================================//

// Constant definitions used in this file only go here

// Type definitions used in this file only go here
INT32U  CSN_n;
INT32U  SCL_n;
INT32U  SDA_n;
INT32U  SDI_n;

#define SPI_CS 64

#ifndef CUSTOMER_DEFINE
    /* free the customer define */
    #define DPF_1024x768            0  
    #define DPF_800x600             0                                
    #define DPF_800x480             0                        
    #define DPF_640x480             0                               
    #define DPF_480x272             0                            
    #define DPF_480x234             0                                               
    #define DPF_320x240             0                           
    #define DPF_720x480             0
    #define DPF_400x240             0
    #define DPF_160x128				0                           
    #define DPF_H_V                 0 
#endif

// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file

// Functions defined in this file
void tft_CPT_CLAA070MA0ACW_init(void);

void tft_init(void)
{
    R_TFT_CTRL = 0;
    tft_slide_en_set(FALSE); 
}

void tft_start(INT32U model)
{
    switch(model) {   
#if (DPF_H_V==DPF_800x600)
        case AUO_A080SN01:
            tft_auo_a080sn01_init();
            break;
        case INNOLUX_AT080TN42:
            tft_innolux_at080tn42_init();
            break;
        case INNOLUX_AT080TN52:    
            tft_innolux_at080tn52_init();
            break;
        case CPT_CLAA070MA0ACW:
			tft_CPT_CLAA070MA0ACW_init();
            break;

        case CHIMEI_Q08009_701:
        	tft_chimei_q08009_701_init();
            break;
#endif                      
#if (DPF_H_V==DPF_1024x768)
        case AUO_M150XN07:
        	tft_auo_m150xn07_init();
            break;
#endif
#if (DPF_H_V==DPF_800x480)           
        case INNOLUX_AT080TN03:
            tft_innolux_at080tn03_init();
            break;
		case INNOLUX_AT070TN92:
            tft_innolux_at070tn92_init();
            break;
        case HSD070IDW1:
            tft_HSD070IDW1_init();
            break;
        case SAMSUNG_LTP700WVF01:
            tft_samsung_ltp700wvf01_init();
            break;
        case AUO_A102VW01:
            tft_auo_a102v201_init();
            break;
        case TPO_TD070WGCB2:
            tft_tpo_td070wgcb2_init();
            break; 
        case AUO_C080VW02:
            tft_auo_c080vw02_init();
            break;
        case AUO_C070VW02:
            tft_auo_c070vw02_init();
            break;
        case AUO_A070VW02:
        	tft_auo_a070vw02_init();
        	break;
        case CPT_CLAA070LD0ACW:
        	tft_cpt_claa070ld0acw_init();
        	break;
#endif          
#if (DPF_H_V==DPF_640x480)             
        case LGP_LB064V02:
            tft_lgp_lb064v02_init();
            break;
#endif                     
#if (DPF_H_V==DPF_480x272)
        case WINTEK_WDF4827Y:
            tft_wintek_wdf4827y_init();
            break;   
#endif                       
#if (DPF_H_V==DPF_480x234) 
        case ANALOG_PANEL_TYPE_1: 
            tft_gpy0200_analog_panel_init();
            break;
        case AUO_C065GW02:
            tft_gpy0200_auo_c065gw02_init();
            break;
#endif 
#if (DPF_H_V==DPF_320x240)     
		case GP_GPM758A0S:
			tft_gp_gpm758a0s_init();
			break;
        case TPO_TD035TTEA1:
            tft_tpo_td035ttea1_init();
            return;
        case AUO_A024CN02V9:
            tft_auo_a024cn02v9_init();
            break;
        case TPO_TD025THEA7:
            tft_tpo_td025thea7_init();
            break;
        case GPM879A0:
            tft_GPM879A0_init();
            break;
        case AUO_A035QN02: 
            tft_AUO_A035QN02_init();
            break;
        case CHILIN_LQ035NC111:
        	tft_chilin_lq035nc111_init();
        	break;
        case INNOLUX_AT056TN03:
            tft_gpy0200_innolux_at056tn03_init();
            break;
        case PMV_035BG:
            tft_gpy0200_pmv_035bg_init();
            break;
        case TIANMA_TM035KDH03: 
        	tft_tianma_tm035kdh03_init();
        	break;
        case AUO_A040CN01:
            tft_gpy0200_auo_a040cn01_init();
            break;
        case TPO_TD025THD1:
        	tft_tpo_td025thd1_init();
        	break;
#endif
#if (DPF_H_V==DPF_400x240)
		case FOX_FL320WQC11:
        	tft_fox_fl320wqc11_init();
        	break;
#endif
#if (DPF_H_V==DPF_160x128)
		case GPM_LM765A0:
        	tft_gpm_lm765a0_init();
        	break;
        case GPM_LM765H0:
        	tft_gpm_lm765h0_init();
        	break;	
#endif 
        default:
            return;
    }
    tft_tft_en_set(TRUE);
}

void tft_start_with_pwm(INT32U model,BOOLEAN pwm0_vghl_en,BOOLEAN pwm1_backlight_en)
{
    tft_start(model);
    
    R_PWM_CTRL |= 0x70; /* set backlight level */
   	
    if (pwm0_vghl_en == TRUE) {
        tft_pwm0_en_set(TRUE); 
        drv_msec_wait(200);   
    }
    
    if (pwm1_backlight_en == TRUE) {
        if (pwm0_vghl_en == FALSE) {
        	drv_msec_wait(100);
        }	 
        tft_backlight_en_set(TRUE);
    }
}

void cmd_delay(INT32U i) 
{
	unsigned int j, cnt;
	cnt = i*2;
	for (j=0;j<cnt;j++);
}

void cmd_longdelay(INT32U i) 
{
	unsigned long j, cnt;
	cnt = i*100;
	for (j=0;j<cnt;j++);
}	

void himax_serial_cmd(INT32U cmd) {

    INT32S i;
    
    gpio_write_io(CSN_n,0);//CS=0
    gpio_write_io(SCL_n,1);//SCL=1
    gpio_write_io(SDA_n,0);//SDA
    
    // set csn hi
    gpio_write_io(CSN_n,1);//CS=1
    cmd_delay(1);
    gpio_write_io(SCL_n,0);//SCL=1
    for (i=0;i<16;i++) {
        // shift data
        gpio_write_io(SDA_n, ((cmd&0x8000)>>15)); /* SDA */
        cmd = (cmd<<1);
        cmd_delay(1);
        // toggle clock
        gpio_write_io(SCL_n,1);//SCL=0
        cmd_delay(1);
        gpio_write_io(SCL_n,0);//SCL=0		
        cmd_delay(1);
    }
    gpio_write_io(SCL_n,1);//SCL=1
    cmd_delay(1);
    // set csn low
    gpio_write_io(CSN_n,0);//CS=0
    
    cmd_delay(1);

}

void serial_cmd_1(INT32U cmd) {

	INT32S i;
	
	gpio_write_io(CSN_n,1);//CS=1
	gpio_write_io(SCL_n,0);//SCL=0
	gpio_write_io(SDA_n,0);//SDA
	
	// set csn low
	gpio_write_io(CSN_n,0);//CS=0
	cmd_delay(1);
	for (i=0;i<16;i++) {
		// shift data
		gpio_write_io(SDA_n, ((cmd&0x8000)>>15)); /* SDA */
		cmd = (cmd<<1);
		cmd_delay(1);
		// toggle clock
		gpio_write_io(SCL_n,1);//SCL=0
		cmd_delay(1);
		gpio_write_io(SCL_n,0);//SCL=0		
		cmd_delay(1);
	}
	
	// set csn high
	gpio_write_io(CSN_n,1);//CS=1
	
	cmd_delay(1);
			
}

void serial_cmd_2(INT32U cmd) {

	INT32S i;
	
	gpio_write_io(CSN_n,1);//CS=1
	gpio_write_io(SCL_n,0);//SCL=0
	gpio_write_io(SDA_n,0);//SDA
	
	// set csn low
	gpio_write_io(CSN_n,0);//CS=0
	cmd_delay(1);
	for (i=0;i<24;i++) {
		// shift data
		gpio_write_io(SDA_n, ((cmd&0x800000)>>23)); /* SDA */
		cmd = (cmd<<1);
		cmd_delay(1);
		// toggle clock
		gpio_write_io(SCL_n,1);//SCL=0
		cmd_delay(1);
		gpio_write_io(SCL_n,0);//SCL=0		
		cmd_delay(1);
	}
	
	// set csn high
	gpio_write_io(CSN_n,1);//CS=1
	
	cmd_delay(1);
			
}
void serial_cmd_3(INT32U cmd) {

	INT32S i;
	
	gpio_write_io(CSN_n,1);//CS=1
	gpio_write_io(SCL_n,0);//SCL=0
	gpio_write_io(SDA_n,0);//SDA
	
	// set csn low
	gpio_write_io(CSN_n,0);//CS=0
	cmd_delay(1);
	for (i=0;i<32;i++) {
		// shift data
		gpio_write_io(SDA_n, ((cmd&0x80000000)>>31)); /* SDA */
		cmd = (cmd<<1);
		cmd_delay(1);
		// toggle clock
		gpio_write_io(SCL_n,1);//SCL=0
		cmd_delay(1);
		gpio_write_io(SCL_n,0);//SCL=0		
		cmd_delay(1);
	}
	
	// set csn high
	gpio_write_io(CSN_n,1);//CS=1
	
	cmd_delay(1);
}
#if (DPF_H_V==DPF_1024x768)
void tft_auo_m150xn07_init(void)
{
    /* 800*600 */
   	R_TFT_V_PERIOD = 790;
	R_TFT_V_START = 22;
	R_TFT_V_END	= 790;

	R_TFT_H_PERIOD = 1160;
	R_TFT_H_START = 136;
	R_TFT_H_END	= 1159;

	R_TFT_VS_END = 0;
	
	R_TFT_TS_MISC = 1;

	tft_rb_switch_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_180);

	tft_data_mode_set(TFT_DATA_MODE_888);

  #if BOARD_TYPE == BOARD_EMU_V2_0
  	R_IOB_DRV |= 0xF;
  	#if MCU_VERSION == GPL326XX
  	R_IOB_DRV_H |= 0xF;
  	#endif
  #endif

	R_TFT_CTRL = 0x8735;
}
#endif

void tft_GPM879A0_spi_write(INT8U reg, INT8U data)
{
#if 0
	INT32U ser_data = 0;
	INT16U temp = 0;
#endif
	
	INT8U ser_data[2] = {0};
	INT8U rcv[2];
	
#if 0
	temp |= reg << 8;
	temp |= data;
	ser_data = temp << 2;
	himax_serial_cmd(ser_data);
#else
	ser_data[0] = reg;
	ser_data[1] = data;
	spi_transceive(SPI_0,ser_data, 2, rcv, 2);
#endif
}

void tft_tpo_spi_write(INT32U data, INT32U len)
{
	INT8U send_data[5] = {0};
	INT8U rcv_data[5] = {0};
	INT32U send_len = 0;
	INT32U ser_data = 0;
	INT8S  i;
	
	tft_tpo_spi_data_build((INT8U *)&data,len, send_data, &send_len);
	for(i=0;i<send_len;i++) {
		ser_data |= (send_data[i] << 8*(send_len-1-i));
	}
#if 1
	gpio_write_io(SPI_CS,0);
	spi_transceive(SPI_0,send_data, send_len, rcv_data, send_len);
	gpio_write_io(SPI_CS,1);
#else
	if (send_len == 2) {
		serial_cmd_1(ser_data);
	}
	else if (send_len == 3) {
		serial_cmd_2(ser_data);
	}
	else if (send_len == 4) {
		serial_cmd_3(ser_data);
	}
#endif	
}

void tft_auo_spi_write(INT8U reg, INT8U data)
{
	INT8U ser_data[2] = {0};
	INT8U rcv[2];
	
	ser_data[0] = (reg&7)<<5;
	ser_data[1] = data;
	spi_transceive(SPI_0,ser_data, 2, rcv, 2);
}

void tft_tpo_spi_data_build(INT8U *in_data,INT8U mode, INT8U *out_data, INT32U *out_len)
{
	if (mode == 1) {
		out_data[0] = in_data[0] >> 1;
		out_data[0] &= 0x7F;
		out_data[1] = (in_data[0] & 1) << 7;
		*out_len = 2;
	}
	else if (mode == 2) {
		out_data[0] = in_data[0] >> 1;
		out_data[0] &= 0x7F;
		out_data[1] = (in_data[0] & 1) << 7;
		out_data[1] |= 0x40;
		out_data[1] |= ((in_data[1] >> 2) & 0x3f);
		out_data[2] = (in_data[1] & 3) << 6;
		*out_len = 3;
	}
	else if (mode == 3) {
		out_data[0] = in_data[0] >> 1;
		out_data[0] &= 0x7F;
		out_data[1] = (in_data[0] & 1) << 7;
		out_data[1] |= 0x40;
		out_data[1] |= ((in_data[1] >> 2) & 0x3f);
		out_data[2] = (in_data[1] & 3) << 6;
		out_data[2] |= 0x20;
		out_data[2] |= ((in_data[2] >> 3) & 0x1f);
		out_data[3] = (in_data[3] & 7) <<5;
		*out_len = 4;
	}	 
}

void tft_AUO_A035QN02_spi_write(INT8U reg, INT16U data)
{
	INT8U ser_reg[3] = {0};
	INT8U rcv[3];
	INT8U ser_data[3] = {0};
	

	ser_reg[0] = 0x70;
	ser_reg[1] = 0x00;
	ser_reg[2] = reg;	
	
	spi_transceive(SPI_0,ser_reg, 3, rcv, 3);
	ser_data[0] = 0x72;
	ser_data[1] = (data&0xff00)>>8; 
	ser_data[2] = (data&0x00ff);	
	spi_transceive(SPI_0,ser_data, 3, rcv, 3);	
}

#if (DPF_H_V==DPF_800x600)

void tft_auo_a080sn01_init(void)
{   /*800*600*/
	CSN_n = IO_F14;
	SCL_n = IO_F7;
	SDA_n = IO_F8;
	
    gpio_init_io (CSN_n, GPIO_OUTPUT);
	gpio_init_io (SCL_n, GPIO_OUTPUT);
	gpio_init_io (SDA_n, GPIO_OUTPUT);
	
	gpio_set_port_attribute(CSN_n, 1);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_set_port_attribute(SDA_n, 1);
	gpio_write_io(CSN_n,1);
	
	R_TFT_V_PERIOD = 628-25;
	R_TFT_V_START = 27-25;
	R_TFT_V_END	= 627-25;
	
	R_TFT_H_PERIOD = 1030-172;/* typ.=1056 */
	R_TFT_H_START = 216-172;
	R_TFT_H_END	= 1015-172;
	
	R_TFT_VS_START = 26;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 216;
	R_TFT_HS_END = 0;
	
	R_TFT_VS_WIDTH = 3;
	R_TFT_HS_WIDTH = 128;
	
	R_TFT_TS_MISC = 1;
	
	tft_rb_switch_set(TRUE);

#if (BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_8D_V2_0 || BOARD_TYPE == BOARD_TK_32600_V1_0)
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif

	//tft_gamma_table_init(); /* init 256*3 gamma table */
	tft_gamma_en_set(TRUE);
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
	tft_clk_set(TFT_CLK_DIVIDE_3);
	
	serial_cmd_1(0x04D1); /* reset */
	serial_cmd_1(0x04D3);
	drv_msec_wait(1);
	//serial_cmd_1(0x0493); /* disable dithering */
	//serial_cmd_1(0x116F); /* VCOM generated by internal */ 
	serial_cmd_1(0x419f); /* set bit 8 to 1 */
}

void tft_innolux_at080tn42_init(void)
{
    /* 800*600 */
    R_TFT_V_PERIOD = 625;
    R_TFT_V_START = 20;
    R_TFT_V_END	= 620;
    
    R_TFT_H_PERIOD = 865;
    R_TFT_H_START = 60;
    R_TFT_H_END	= 859;
    R_TFT_VS_START = 20;
    R_TFT_VS_END = 0;
    R_TFT_HS_START = 60;
    R_TFT_HS_END = 0;
    
    R_TFT_TS_CKV = 42;
    R_TFT_TW_CKV = 40;
    R_TFT_TS_MISC = 1;
    R_TFT_TS_STV = 22;
    R_TFT_TW_STV = 500;
    R_TFT_TS_STH = 60;
    R_TFT_TW_STH = 1;
    R_TFT_TS_OEV = 32;
    R_TFT_TW_OEV = 50;
    R_TFT_TS_LD = 52;
    R_TFT_TW_LD = 8;
    R_TFT_TS_POL = 60;
    
    tft_rb_switch_set(TRUE);
    tft_data_mode_set(TFT_DATA_MODE_666);
    tft_dith_en_set(TRUE);
    tft_gamma_en_set(TRUE);
    tft_dclk_sel_set(TFT_DCLK_SEL_180);
    tft_new_pol_en_set(TRUE);
    
    tft_mode_set(TFT_MODE_TCON);
    tft_clk_set(TFT_CLK_DIVIDE_3); /* frame rate = 60@96MHz */
    
}

void tft_innolux_at080tn52_init(void)
{
    /* 800*600 */
    R_TFT_V_PERIOD = 610;
	R_TFT_V_START = 7;
	R_TFT_V_END	= 607;
	
	R_TFT_H_PERIOD = 870;
	R_TFT_H_START = 56;
	R_TFT_H_END	= 855;
	
	R_TFT_VS_START = 26;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 216;
	R_TFT_HS_END = 0;
	
	R_TFT_VS_WIDTH = 3;
	R_TFT_HS_WIDTH = 128;
	
	R_TFT_TS_MISC = 1;
    
    tft_rb_switch_set(TRUE);
    
#if (BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_8D_V1_0|| BOARD_TYPE == BOARD_TK_8D_V2_0 || BOARD_TYPE == BOARD_TK_32600_V1_0)
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif
    
    tft_gamma_en_set(TRUE);
    tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
	tft_clk_set(TFT_CLK_DIVIDE_3);
}

void tft_CPT_CLAA070MA0ACW_init(void)
{
	/*800*600*/
		INT32U i;
		
        CSN_n = IO_F14; 
        SCL_n = IO_F7; 
        SDA_n = IO_F8; 
        
    gpio_init_io (CSN_n, GPIO_OUTPUT); 
        gpio_init_io (SCL_n, GPIO_OUTPUT); 
        gpio_init_io (SDA_n, GPIO_OUTPUT); 
        
        gpio_set_port_attribute(CSN_n, 1); 
        gpio_set_port_attribute(SCL_n, 1); 
        gpio_set_port_attribute(SDA_n, 1); 
        gpio_write_io(CSN_n,1); 

#if 0 
        R_TFT_V_PERIOD = 628-25; 
        R_TFT_V_START = 27-25; 
        R_TFT_V_END        = 627-25; 
        
        R_TFT_H_PERIOD = 1030-172;/* typ.=1056 */ 
        R_TFT_H_START = 216-172; 
        R_TFT_H_END        = 1015-172; 
#endif 
        
        R_TFT_V_PERIOD = 660; 
        R_TFT_V_START = 60; 
        R_TFT_V_END        = 60+600; 
        
        R_TFT_H_PERIOD = 1000;/* typ.=1056 */ 
        R_TFT_H_START = 200; 
        R_TFT_H_END        = 999; 
        
        R_TFT_VS_START = 26; 
        R_TFT_VS_END = 0; 
        R_TFT_HS_START = 216; 
        R_TFT_HS_END = 0; 
        
        R_TFT_VS_WIDTH = 3; 
        R_TFT_HS_WIDTH = 128; 
        
        R_TFT_TS_MISC = 1; 
        
        tft_rb_switch_set(TRUE); 

#if (BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_8D_V2_0 || BOARD_TYPE == BOARD_TK_32600_COMBO || BOARD_TYPE == BOARD_TK_32600_7A_V1_0) 
        tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */ 
        tft_dith_en_set(TRUE); /* enable dithing */ 
          /* R2 B2 at IOB5 and IOB6 */ 
          R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4 
        tft_data_mode_set(TFT_DATA_MODE_888); 
          /* R2 B2 at IOB5 and IOB6 */ 
          R_TFT_TS_MISC |= 0x10000; 
#else 
        tft_data_mode_set(TFT_DATA_MODE_888); 
#endif 

        //tft_gamma_table_init(); /* init 256*3 gamma table */ 
        tft_gamma_en_set(TRUE); 
        
        tft_vsync_unit_set(TRUE); 
        tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV)); 
        tft_mode_set(TFT_MODE_PARALLEL); 
        tft_clk_set(TFT_CLK_DIVIDE_3); 
        
        serial_cmd_1(0x04D1); /* reset */ 
        serial_cmd_1(0x04D3); 
        drv_msec_wait(1); 
        //serial_cmd_1(0x0493); /* disable dithering */ 
        //serial_cmd_1(0x116F); /* VCOM generated by internal */ 
        serial_cmd_1(0x419f); /* set bit 8 to 1 */ 
        
        tft_dclk_delay_set(TFT_DCLK_DELAY_5);
/*	gpio_drving_init_io(IO_B1,(IO_DRV_LEVEL) IO_DRIVING_8mA); //TFT_HSYNC_VSYNC_CLK_DRIVING 
	gpio_drving_init_io(IO_B2,(IO_DRV_LEVEL) IO_DRIVING_8mA);  
	gpio_drving_init_io(IO_B3,(IO_DRV_LEVEL) IO_DRIVING_8mA);
*/	
	

    for (i=IO_A0 ; i<=IO_A15; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) IO_DRIVING_16mA);//IO_DRIVING_16mA
    }

    for (i=IO_B0 ; i<=IO_B3; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) IO_DRIVING_16mA);//IO_DRIVING_8mA
    }

    for (i=IO_B5 ; i<=IO_B6; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) IO_DRIVING_16mA);////IO_DRIVING_16mA
    }


}
void tft_chimei_q08009_701_init(void)
{
	R_TFT_V_PERIOD = 635;
	R_TFT_V_START = 23;
	R_TFT_V_END	= 623;

	R_TFT_H_PERIOD = 865;/* typ.=1056 */
	R_TFT_H_START = 40;
	R_TFT_H_END	= 839;

	R_TFT_VS_START = 26;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 216;
	R_TFT_HS_END = 0;

	R_TFT_VS_WIDTH = 3;
	R_TFT_HS_WIDTH = 128;

	R_TFT_TS_MISC = 1;

	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE); /* enable dithing */
	tft_gamma_en_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_180);

  #if BOARD_TYPE == BOARD_EMU_V2_0
  	R_IOB_DRV |= 0xF;
  	#if MCU_VERSION == GPL326XX
  	R_IOB_DRV_H |= 0xF;
  	#endif
  #endif

	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
	tft_clk_set(TFT_CLK_DIVIDE_3);
}

#endif //DPF_H_V==DPF_800x600

#if (DPF_H_V==DPF_800x480)
void tft_innolux_at080tn03_init(void)
{
    /* 800*480 */
    R_TFT_V_PERIOD = 525;
    R_TFT_V_START = 20;
    R_TFT_V_END	= 500;
    
    R_TFT_H_PERIOD = 1015;
    R_TFT_H_START = 59;
    R_TFT_H_END	= 858;
    R_TFT_VS_START = 20;
    R_TFT_VS_END = 0;
    R_TFT_HS_START = 59;
    R_TFT_HS_END = 0;
    
    R_TFT_TS_CKV = 42;
    R_TFT_TW_CKV = 40;
    R_TFT_TS_MISC = 1;
    R_TFT_TS_STV = 22;
    R_TFT_TW_STV = 500;
    R_TFT_TS_STH = 59;
    R_TFT_TW_STH = 1;
    R_TFT_TS_OEV = 32;
    R_TFT_TW_OEV = 50;
    R_TFT_TS_LD = 52;
    R_TFT_TW_LD = 8;
    R_TFT_TS_POL = 60;
    
    tft_rb_switch_set(TRUE);
    tft_data_mode_set(TFT_DATA_MODE_666);
    tft_dith_en_set(TRUE);
    tft_gamma_en_set(TRUE);
    tft_dclk_sel_set(TFT_DCLK_SEL_90);
    tft_new_pol_en_set(TRUE); /* new 1 line inversion */
    tft_mode_set(TFT_MODE_TCON);

    if (MHZ == 96)
    {
        tft_clk_set(TFT_CLK_DIVIDE_3); /* frame rate = 60 @96MHZ */
    }
    else 
    {
        tft_clk_set(TFT_CLK_DIVIDE_2);
    }

#if 0
    _PWM_CTRL |= 0x3f70;
    rv_msec_wait(2);
    _PWM_CTRL |= 0x3; /* enable PWM0 VGH and VGL*/
    rv_msec_wait(500);
    _PWM_CTRL |= 0x4; /* enable PWM1 LED backlight*/
#endif
}

void tft_HSD070IDW1_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 525;
	R_TFT_V_START = 8;
	R_TFT_V_END	= 488;
	
	R_TFT_H_PERIOD = 928;
	R_TFT_H_START = 100;
	R_TFT_H_END	= 899;
	/*
	R_TFT_CLIP_V_START = 8;
	R_TFT_CLIP_V_END = 488;  	
	R_TFT_CLIP_H_START = 100;   	
	R_TFT_CLIP_H_END = 899;
	*/
	R_TFT_VS_START = 7;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 99;
	R_TFT_HS_END = 0;				
	
	R_TFT_TS_MISC = 1;
	  
	tft_rb_switch_set(TRUE);
	
	//tft_gamma_table_init(); /* init 256*3 gamma table */
	tft_gamma_en_set(TRUE);
	
#if BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_32600_V1_0
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif
	
	tft_vsync_unit_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_90);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_4); /* frame rate = 60@96MHz */
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_3);
    }
}

void tft_auo_a070vw02_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 525;
	R_TFT_V_START = 8;
	R_TFT_V_END	= 488;
	
	R_TFT_H_PERIOD = 1056;
	R_TFT_H_START = 100;
	R_TFT_H_END	= 899;
	
	R_TFT_VS_START = 7;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 99;
	R_TFT_HS_END = 0;				
	
	R_TFT_TS_MISC = 1;
	  
	tft_rb_switch_set(TRUE);	
	tft_gamma_en_set(TRUE);
	tft_dith_en_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_3); /* frame rate = 60@96MHz */
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_2);
    }
}

void tft_cpt_claa070ld0acw_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 535;
	R_TFT_V_START = 32;
	R_TFT_V_END	= 512;
	
	R_TFT_H_PERIOD = 948;
	R_TFT_H_START = 88;
	R_TFT_H_END	= 887;
	
	R_TFT_VS_START = 7;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 99;
	R_TFT_HS_END = 0;				
	
	R_TFT_TS_MISC = 1;
	tft_rb_switch_set(TRUE);
	
	//tft_gamma_table_init(); /* init 256*3 gamma table */
	tft_gamma_en_set(TRUE);

#if BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_32600_V1_0
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif
	
	tft_vsync_unit_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_90);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    
    tft_clk_set(TFT_CLK_DIVIDE_3); /* frame rate = 60@96MHz */
}

void tft_samsung_ltp700wvf01_init(void)
{
    /*800*480*/	
	R_TFT_V_PERIOD = 493;
	R_TFT_V_START = 8;
	R_TFT_V_END	= 488;
	
	R_TFT_H_PERIOD = 824;
	R_TFT_H_START = 16;
	R_TFT_H_END	= 815;
	
	R_TFT_VS_START = 7;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 16;
	R_TFT_HS_END = 0;				
	
	R_TFT_TS_MISC = 1;
	tft_rb_switch_set(TRUE);
	tft_gamma_en_set(TRUE);
	
#if BOARD_TYPE == BOARD_TK_V4_1
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif

	tft_vsync_unit_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_180);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_4); /* frame rate = 60@96MHz */
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_3);
    }
}

void tft_auo_a102v201_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 530;
	R_TFT_V_START = 20;
	R_TFT_V_END	= 500;
	
	R_TFT_H_PERIOD = 1005;
	R_TFT_H_START = 60;
	R_TFT_H_END	= 859;
	R_TFT_VS_START = 20;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 60;
	R_TFT_HS_END = 0;
	
	R_TFT_TS_CKV = 42;
	R_TFT_TW_CKV = 40;
	R_TFT_TS_MISC = 1;/* RB switch,data mode= 666 */
	R_TFT_TS_STV = 22;
	R_TFT_TW_STV = 500;
	R_TFT_TS_STH = 60;
	R_TFT_TW_STH = 1;
	R_TFT_TS_OEV = 32;
	R_TFT_TW_OEV = 50;
	R_TFT_TS_LD = 52;
	R_TFT_TW_LD = 8;
	R_TFT_TS_POL = 60;
	
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_180);
	
	tft_signal_inv_set(TFT_DCLK_INV, (TFT_ENABLE & TFT_DCLK_INV));
	tft_mode_set(TFT_MODE_TCON);
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS = 60 */
}

void tft_tpo_td070wgcb2_init(void)
{
	/* 854*480 */
	/*
	R_TFT_V_PERIOD = 800;
	R_TFT_V_START = 23;
	R_TFT_V_END	= 791;
	
	R_TFT_H_PERIOD = 1056;
	R_TFT_H_START = 30;
	R_TFT_H_END	= 1053;
	
	R_TFT_VS_START = 22;
	R_TFT_VS_END = 502;
	R_TFT_HS_START = 30;
	R_TFT_HS_END = 885;
	*/
#if 1 /* ppu resolution 800*480 */
	R_TFT_V_PERIOD = 525;
	R_TFT_V_START = 23;
	R_TFT_V_END	= 503;
	
	R_TFT_H_PERIOD = 1015;
	R_TFT_H_START = 58;
	R_TFT_H_END	= 857;
	
#else /* ppu resolution 1024*768 */
	R_TFT_V_PERIOD = 800;
	R_TFT_V_START = 23;
	R_TFT_V_END	= 791;
	
	R_TFT_H_PERIOD = 1056;
	R_TFT_H_START = 30;
	R_TFT_H_END	= 1053;
#endif

	R_TFT_VS_START = 22;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 30;
	R_TFT_HS_END = 0;

	R_TFT_CLIP_V_START = 23;
	R_TFT_CLIP_V_END =  503;
	R_TFT_CLIP_H_START = 30;
	R_TFT_CLIP_H_END = 885;
	
	R_TFT_TS_MISC = 1;
	
	tft_slide_en_set(TRUE); /* clip enable */
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_888);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
#if (defined BOARD_TYPE) && (BOARD_TYPE==BOARD_EMU_V2_0)   // jandy add, 2008/11/17  
	tft_dclk_sel_set(TFT_DCLK_SEL_180);
#elif (defined BOARD_TYPE) && (BOARD_TYPE==BOARD_EMU_V1_0)  
    tft_dclk_sel_set(TFT_DCLK_SEL_90);
#else
    tft_dclk_sel_set(TFT_DCLK_SEL_90);
#endif
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_3); /* frame rate = 60 @ 96M*/
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_3);
    }
}

void tft_auo_c080vw02_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 530;
	R_TFT_V_START = 20;
	R_TFT_V_END	= 500;
	
	R_TFT_H_PERIOD = 1005;
	R_TFT_H_START = 60;
	R_TFT_H_END	= 859;
	R_TFT_VS_START = 20;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 60;
	R_TFT_HS_END = 0;
	
	R_TFT_TS_CKV = 42;
	R_TFT_TW_CKV = 40;
	R_TFT_TS_MISC = 1;/* RB switch,data mode= 666 */
	R_TFT_TS_STV = 22;
	R_TFT_TW_STV = 500;
	R_TFT_TS_STH = 60;
	R_TFT_TW_STH = 1;
	R_TFT_TS_OEV = 32;
	R_TFT_TW_OEV = 50;
	R_TFT_TS_LD = 52;
	R_TFT_TW_LD = 8;
	R_TFT_TS_POL = 60;
	
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_180);
	
	tft_mode_set(TFT_MODE_TCON);
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS = 60 */
}

void tft_auo_c070vw02_init(void)
{
	/* 800*480 */
	R_TFT_V_PERIOD = 540;
	R_TFT_V_START = 20;
	R_TFT_V_END	= 500;
	
	R_TFT_H_PERIOD = 1000;
	R_TFT_H_START = 60;
	R_TFT_H_END	= 859;
	R_TFT_VS_START = 20;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 60;
	R_TFT_HS_END = 0;
	
	R_TFT_TS_CKV = 42;
	R_TFT_TW_CKV = 40;
	R_TFT_TS_MISC = 1;
	R_TFT_TS_STV = 22;
	R_TFT_TW_STV = 500;
	R_TFT_TS_STH = 60;
	R_TFT_TW_STH = 1;
	R_TFT_TS_OEV = 32;
	R_TFT_TW_OEV = 50;
	R_TFT_TS_LD = 52;
	R_TFT_TW_LD = 8;
	R_TFT_TS_POL = 60;
	
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	tft_dclk_sel_set(TFT_DCLK_SEL_90);
	tft_new_pol_en_set(TRUE); /* new 1 line inversion */
	//tft_signal_inv_set(TFT_DCLK_INV, (TFT_ENABLE & TFT_DCLK_INV));
	tft_mode_set(TFT_MODE_TCON);
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS = 60@96MHz */
	//R_TFT_CTRL = 0x0345;	
}

void tft_innolux_at070tn92_init(void)
{
	R_TFT_V_PERIOD = 525;
    R_TFT_V_START = 20;
    R_TFT_V_END	= 500;
	
    R_TFT_H_PERIOD = 870;
	R_TFT_H_START = 56;
	R_TFT_H_END	= 855;
	
	R_TFT_VS_START = 26;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 216;
	R_TFT_HS_END = 0;


	R_TFT_VS_WIDTH = 3;
	R_TFT_HS_WIDTH = 128;
	
	R_TFT_TS_MISC = 1;
    
    tft_rb_switch_set(TRUE);
    
#if BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_32600_V1_0
	tft_data_mode_set(TFT_DATA_MODE_666); /* connect to parallel 666 by HW */
	tft_dith_en_set(TRUE); /* enable dithing */
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000; 
#elif BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4
	tft_data_mode_set(TFT_DATA_MODE_888);
  	/* R2 B2 at IOB5 and IOB6 */
  	R_TFT_TS_MISC |= 0x10000;
#else
	tft_data_mode_set(TFT_DATA_MODE_888);
#endif
    
    tft_gamma_en_set(TRUE);
    tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
	tft_clk_set(TFT_CLK_DIVIDE_3);
}

#endif //DPF_H_V==DPF_800x480

#if (DPF_H_V==DPF_640x480) 
void tft_lgp_lb064v02_init(void)
{
	R_TFT_V_PERIOD = 525;
	R_TFT_V_START = 30;
	R_TFT_V_END	= 510;
	
	R_TFT_H_PERIOD = 770;
	R_TFT_H_START = 40;
	R_TFT_H_END	= 679;
	
	R_TFT_VS_START = 29;
	R_TFT_VS_END = 0;/*511*/
	R_TFT_HS_START = 39;
	R_TFT_HS_END = 0;/*681*/
	
	R_TFT_TS_MISC = 1;

	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);

    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_4); /* FS=60 @ 96MHz */	
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_2);
    }
}
#endif // (DPF_H_V==DPF_640x480) 

#if (DPF_H_V==DPF_480x272)
void tft_wintek_wdf4827y_init(void)
{	/* 480RGB*272 */
	R_TFT_V_PERIOD = 350;
	R_TFT_V_START = 13;
	R_TFT_V_END	= 285;
	
	R_TFT_H_PERIOD = 580;
	R_TFT_H_START = 43;
	R_TFT_H_END	= 522;
	
	R_TFT_VS_START = 12;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 43;
	R_TFT_HS_END = 0;
	
	R_TFT_VS_WIDTH = 10;
	R_TFT_HS_WIDTH = 41;
	
	R_TFT_TS_MISC = 1;
	
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_888);
	tft_gamma_en_set(TRUE);
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);

    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_8);/* FS = 60 */
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_4);
    }
}
#endif  //(DPF_H_V==DPF_480x272)

#if (DPF_H_V==DPF_480x234)
void tft_gpy0200_analog_panel_init(void)
{
	/* 480 *234 */
#if MCU_VERSION == GPL326XX

	R_ANALOG_TFT_CTRL_L = 0xf;
	R_ANALOG_TFT_CTRL_H = 0x0;
	R_ANALOG_TFT_CTRL2_L = 0x61; 
	R_ANALOG_TFT_CTRL2_H = 0x0;
	R_ANALOG_TFT_TS_STH_L = 0x57;
	R_ANALOG_TFT_TS_STH_H = 0x01;
	R_ANALOG_TFT_H_START_L = 0x52;
	R_ANALOG_TFT_H_START_H = 0x01;
	R_ANALOG_TFT_V_START = 10;
	R_ANALOG_TFT_TW_STH = 3;
	R_ANALOG_TFT_T_HS2OEH = 127;
	R_ANALOG_TFT_TW_OEH = 35;
	R_ANALOG_TFT_T_HS2CKV = 134;
	R_ANALOG_TFT_TW_CKV = 107;
	R_ANALOG_TFT_T_HS2OEV = 18;
	R_ANALOG_TFT_TW_OEV = 144;
	R_ANALOG_TFT_TS_STV = 8;  
	R_ANALOG_TFT_CLK_DLY = 0; 
	R_ANALOG_TFT_AG = 0;
	R_ANALOG_TFT_TS_POL = 162;

#else
 #if _DRV_L1_GPY0200 == 1
	INT8U data = 0;
	
	gpy0200_aging_pattern_en_set(FALSE,0);
	
	data = gpy0200_i2c_read(0xc); /* enable VDAC */
	data |= 0x1;
	gpy0200_i2c_write(0xc,data);
	
	//data = gpy0200_i2c_read(0x0c);
	//data &= ~0x3e;
	//data |= 0x40;
	//gpy0200_i2c_write(0x0c,data);

	//data = gpy0200_i2c_read(0x20); /* enable gpy0200 tcon */
	//data |= 0x1;
	//gpy0200_i2c_write(0x20,data);
	
	gpy0200_i2c_write(0x20,0x0f);	//TFT_CTRL_L,enable tcon
	
	gpy0200_i2c_write(0x21,0x0);	//TFT_CTRL_H
	gpy0200_i2c_write(0x22,0x61);	//TFT_CTRL2_L
	gpy0200_i2c_write(0x23,0x0);	//TFT_CTRL2_H
	gpy0200_i2c_write(0x24,0x57);	//Ts_STH_L = GPL32 H_START
	gpy0200_i2c_write(0x25,0x01);	//Ts_STH_H = GPL32 H_START
	gpy0200_i2c_write(0x26,0x52);	//H_Start_L= GPL32 H_START - 4
	gpy0200_i2c_write(0x27,0x01);	//H_Start_H= GPL32 H_START - 4
	gpy0200_i2c_write(0x28,10);		//V_Start = GPL32 V_Start
	gpy0200_i2c_write(0x29,3);		//Tw_STH
	gpy0200_i2c_write(0x2a,127);	//T_HS2OEH
	gpy0200_i2c_write(0x2b,35);		//Tw_OEH
	gpy0200_i2c_write(0x2c,134);	//T_HS2CKV
	gpy0200_i2c_write(0x2d,107);	//Tw_CKV
	gpy0200_i2c_write(0x2e,18);		//T_HS2OEV
	gpy0200_i2c_write(0x2f,144);	//Tw_OEV
	gpy0200_i2c_write(0x30,8);		//Ts_STV
	gpy0200_i2c_write(0x31,0);		//CLK_DLY
	gpy0200_i2c_write(0x32,0);		//AG
	gpy0200_i2c_write(0x33,162);	//Ts_POL = GP8 T_HS2OEV + Tw_OEV
 #endif
#endif
	
	R_TFT_V_PERIOD = 260;    // V_PERIOD = 260 
    R_TFT_V_START  = 10;     // V_START  = 10
    R_TFT_V_END    = 10 + 234;    // V_END    = 250 = 10 + 240

    R_TFT_H_PERIOD = 2000;   // H_PERIOD = 1100
    R_TFT_H_START  = 340;    // H_START  = 100
    R_TFT_H_END    = 340+1440-1;   // H_END    = 1060= 100 + 960
    
    R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;/*254*/
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;/*1780*/
	
	//tft_gamma_table_init(); /* init 256*3 gamma table */
	tft_gamma_en_set(TRUE);
	
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_clk_set(TFT_CLK_DIVIDE_3);

}

void tft_gpy0200_auo_c065gw02_init(void)
{
	/* 400RGB*234 ,PPU use 480*234 */
#if _DRV_L1_GPY0200 == 1
	
	gpy0200_i2c_write(0x5f,0xa5);
	
	gpy0200_i2c_write(0x20,0x0d);	//TFT_CTRL_L
	gpy0200_i2c_write(0x21,0x0);	//TFT_CTRL_H
	gpy0200_i2c_write(0x22,0x41);		// TFT_CTRL2_L
	gpy0200_i2c_write(0x23,0);		// TFT_CTRL2_H
	gpy0200_i2c_write(0x24,0x55);	//Ts_STH_L = GPL32 H_START
	gpy0200_i2c_write(0x25,0x01);	//Ts_STH_H = GPL32 H_START
	gpy0200_i2c_write(0x26,0x52);	//H_Start_L= GPL32 H_START - 4
	gpy0200_i2c_write(0x27,0x01);	//H_Start_H= GPL32 H_START - 4
	
	gpy0200_i2c_write(0x28,10);		//V_Start = GPL32 V_Start
	gpy0200_i2c_write(0x29,3);		//Tw_STH
	gpy0200_i2c_write(0x2a,162);	//T_HS2OEH
	gpy0200_i2c_write(0x2b,36);		//Tw_OEH
	
	//gp8_spi_write(0x11,177);	//T_DIS1
	gpy0200_i2c_write(0x2c,72);		//T_HS2CKV
	gpy0200_i2c_write(0x2d,108);	//Tw_CKV
	gpy0200_i2c_write(0x2e,18);		//T_HS2OEV
	gpy0200_i2c_write(0x2f,144);	//Tw_OEV
	gpy0200_i2c_write(0x30,8);		//Ts_STV
	gpy0200_i2c_write(0x31,0);		//CLK_DLY
	gpy0200_i2c_write(0x32,0);		//AG
	gpy0200_i2c_write(0x33,162);	//Ts_POL = GP8 T_HS2OEV + Tw_OEV
	
	
	R_TFT_V_PERIOD = 260;    // V_PERIOD = 260 
    R_TFT_V_START  = 10;     // V_START  = 10
    R_TFT_V_END    = 244;    // V_END    = 250 = 10 + 240

    R_TFT_H_PERIOD = 2010;   // H_PERIOD = 1100
    R_TFT_H_START  = 340;    // H_START  = 100
    R_TFT_H_END    = 340+1440-1;   // H_END    = 1060= 100 + 960
    
    tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 10;
	R_TFT_CLIP_V_END =  244;
	R_TFT_CLIP_H_START = 340;
	R_TFT_CLIP_H_END = 340+1200-1;
    
    R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;/*254*/
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;/*1780*/	
	
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS=60 */
#endif
}
#endif //DPF_H_V==DPF_480x234

#if (DPF_H_V==DPF_320x240)
void tft_tpo_td035ttea1_init(void)
{
	/*320*240*/
	R_TFT_V_PERIOD = 280;
	R_TFT_V_START = 17;
	R_TFT_V_END	= 257;
	
	R_TFT_H_PERIOD = 440;
	R_TFT_H_START = 78;
	R_TFT_H_END	= 397;
	
	R_TFT_VS_START = 16;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 78;
	R_TFT_HS_END = 0;
	
	R_TFT_VS_WIDTH = 7;
	R_TFT_HS_WIDTH = 37;
	
	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
    if (MHZ == 96)
    {
    	tft_clk_set(TFT_CLK_DIVIDE_8);
    }
    else
    {
    	tft_clk_set(TFT_CLK_DIVIDE_4);
    }
	
	tft_tft_en_set(TRUE); /* RGB signal have to send before initial step */
	
	drv_msec_wait(10);
#if 1
	/*use spi transmission */
	gpio_init_io(SPI_CS,GPIO_OUTPUT);
	gpio_set_port_attribute(SPI_CS, 1);
  	gpio_write_io(SPI_CS,1);
  	spi_clk_set(SPI_0,SYSCLK_8); /* 10MHz require in spec */
  	spi_cs_by_external_set(SPI_0);
#else /* use gpio pin */
  	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
#endif

  	/* spi write ,Lo byte is command and Hi byte is parameter in writeing value */
  	tft_tpo_spi_write(0x12,1);       // partial mode on     
       
	tft_tpo_spi_write(0x02b0,2);     // Select internal or external DE mode
	tft_tpo_spi_write(0x05b4,2);     // Display mode setup
	tft_tpo_spi_write(0x0836,2);     // Memory access control, fixed setting
	tft_tpo_spi_write(0x03b7,2);     // For IC booster clock timing setting
	tft_tpo_spi_write(0x39be,2);     // For panel timing setting
	tft_tpo_spi_write(0x19b5,2);     // VCS adjustment for eliminate flicker
	tft_tpo_spi_write(0x0808c0,3);   // For panel timing setting
	tft_tpo_spi_write(0x1818c2,3);   // For panel timing setting
	tft_tpo_spi_write(0x3012c4,3);   // For panel timing setting
	tft_tpo_spi_write(0x0cc5,2);     // For panel timing setting
	tft_tpo_spi_write(0x04ed,2);     // For panel timing setting
/*
	tft_tpo_spi_write(0x45ba,2);     // For gamma setting
	tft_tpo_spi_write(0x3577d6,3);   // For gamma setting
	tft_tpo_spi_write(0x01d7,2);     // For gamma setting
	tft_tpo_spi_write(0x00d8,2);     // For gamma setting
	tft_tpo_spi_write(0x00d9,2);     // For gamma setting
	tft_tpo_spi_write(0x8001ee,3);   // For gamma setting
*/

	drv_msec_wait(200); /*delay 200ms */
	
	tft_tpo_spi_write(0x13,1);               // Normal mode on
	tft_tpo_spi_write(0x11,1);               // Sleep out
	
	/*delay 6 frames */
	drv_msec_wait(700);
	tft_tpo_spi_write(0x29,1);               // Display on
	
    tft_tft_en_set(TRUE);
}

void tft_AUO_A035QN02_init(void)
{	/* 320RGB*240 Parallel RGB(666) */
	R_TFT_V_PERIOD = 244;
	R_TFT_V_START = 2;
	R_TFT_V_END	= 2+240;
	
	R_TFT_H_PERIOD = 336;
	R_TFT_H_START = 8;
	R_TFT_H_END	= 8+319;
	
	R_TFT_VS_START = 2;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 88;
	R_TFT_HS_END = 0;
	
	R_TFT_VS_WIDTH = 7;
	R_TFT_HS_WIDTH = 37;

	tft_rb_switch_set(TRUE);
	tft_data_mode_set(TFT_DATA_MODE_666);
	tft_dith_en_set(TRUE);
	tft_gamma_en_set(TRUE);
	
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_PARALLEL);
	tft_clk_set(TFT_CLK_DIVIDE_8);
	tft_tft_en_set(TRUE); /* RGB signal have to send before initial step */
	
	drv_msec_wait(10);

#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	gpio_write_io(SCL_n,1);//SCL=1
#endif


	spi_cs_by_internal_set(SPI_0,SPI_CS, 0);
	spi_clk_set(SPI_0,SYSCLK_128); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);		


//  Recommand Register Settings. Not setting these registers can also work well.
	tft_AUO_A035QN02_spi_write(0x01,0x2AEF); //RGB order, UDC, LRC control
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x03,0x7872);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x0C,0x0004);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x0D,0x000C);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x0E,0x3100);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x1E,0x00A8);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x2E,0xB945);
	drv_msec_wait(5);			
	//  y control 1
	tft_AUO_A035QN02_spi_write(0x30,0x0304);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x31,0x0507);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x32,0x0405);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x33,0x0007);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x34,0x0507);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x35,0x0004);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x36,0x0605);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x37,0x0103);
	drv_msec_wait(5);			
	//  y control 2
	tft_AUO_A035QN02_spi_write(0x3A,0x000F);
	drv_msec_wait(5);			
	tft_AUO_A035QN02_spi_write(0x3B,0x000F);
	drv_msec_wait(5);				
	
#if 0
	R_PWM_CTRL |= 0x3f70;
	drv_mese_wait(500);
	R_PWM_CTRL |= 0x5; /* enable PWM1 LED backlight*/
#endif	
}

void tft_auo_a024cn02v9_init(void)
{
	spi_cs_by_internal_set(SPI_0,SPI_CS, 0);
	spi_clk_set(SPI_0,SYSCLK_8); 
	spi_pha_pol_set(SPI_0,PHA0_POL0);
	
	tft_auo_spi_write(0,0x07);
	tft_auo_spi_write(3,0x00);
	tft_auo_spi_write(0,0x0F);
	
	/* 480dot*234 */
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 19;
	R_TFT_V_END	= 259;
	
	R_TFT_H_PERIOD = 1100;
	R_TFT_H_START = 80;
	R_TFT_H_END	= 1040;/* 320*240 ppu resolution */
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 19;
	R_TFT_CLIP_V_END =  253;
	R_TFT_CLIP_H_START = 80;
	R_TFT_CLIP_H_END = 560; 
	
	R_TFT_HS_WIDTH = 19;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 80;
	R_TFT_HS_END = 0;
	
	R_TFT_LINE_RGB_ORDER = 0x04;
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
}

void tft_tpo_td025thea7_init(void)
{
	/* 320*240 */
#if 0
	INT32U spi_cs = IO_E0;
	INT8U send[2];
	
	spi_cs_by_internal_set(SPI_0,spi_cs, 0);
	spi_clk_set(SPI_0,SYSCLK_8); 
	spi_pha_pol_set(SPI_0,PHA0_POL0);
	
	send[0] = 0x08;
	send[1] = 0;
	spi_transceive(SPI_0,send, 2, NULL, 0);
#else
	CSN_n = IO_B10;
	SDA_n = IO_B9;
    SCL_n = IO_B8;
	
    gpio_init_io (CSN_n, GPIO_OUTPUT);
	gpio_init_io (SCL_n, GPIO_OUTPUT);
	gpio_init_io (SDA_n, GPIO_OUTPUT);
	
	gpio_set_port_attribute(CSN_n, 1);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_set_port_attribute(SDA_n, 1);
	
    serial_cmd_1(0x0800);
#endif
	/*
	R_TFT_LINE_RGB_ORDER = 0x00;
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 22-1;
	R_TFT_V_END	= 22+240-1;
	
	R_TFT_H_PERIOD = 1560;
	R_TFT_H_START = 240;
	R_TFT_H_END	= 240+1280;
	*/
	R_TFT_HS_WIDTH			= 0;				//	1		=HPW
	R_TFT_H_START			= 1+239;			//	240		=HPW+HBP
	R_TFT_H_END				= 1+239+1280;	//	1520	=HPW+HBP+HDE
	R_TFT_H_PERIOD			= 1+239+1280+40;	//	1560	=HPW+HBP+HDE+HFP
	R_TFT_VS_WIDTH			= 0;				//	1		=VPW				(DCLK)
	R_TFT_V_START			= 21;			//	21		=VPW+VBP			(LINE)
	R_TFT_V_END				= 21+240;		//	261		=VPW+VBP+VDE		(LINE)
	R_TFT_V_PERIOD			= 21+240+1;		//	262		=VPW+VBP+VDE+VFP	(LINE)
	R_TFT_LINE_RGB_ORDER    = 0x00;
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS052);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
#if 0
	spi_cs_by_external_set(SPI_0);
	spi_clk_set(SPI_0,SYSCLK_8); 
#endif
}

void tft_tpo_td025thd1_init(void)
{
	/* 320*240 */
#if BOARD_TYPE == BOARD_EMU_V2_0
	CSN_n = IO_B10;
	SDA_n = IO_B9;
    SCL_n = IO_B8;
#elif BOARD_TYPE == BOARD_DEMO_GPL32XXX 
	CSN_n = IO_C3;
	SDA_n = IO_C2;
    SCL_n = IO_C1;
#elif BOARD_TYPE == BOARD_DEMO_GPL326XX
	CSN_n = IO_G15;
	SDA_n = IO_G14;
    SCL_n = IO_G13;
#elif (BOARD_TYPE == BOARD_DEMO_GPL327XX) || (BOARD_TYPE == BOARD_DEMO_GP327XXXA)
	CSN_n = IO_C3;
	SDA_n = IO_C2;
    SCL_n = IO_C1;
#elif (BOARD_TYPE == BOARD_DEMO_GPL326XXB) || (BOARD_TYPE == BOARD_DEMO_GP326XXXA)
	CSN_n = IO_G15;
	SDA_n = IO_G13;
    SCL_n = IO_C10;    
#else
	CSN_n = IO_C3;
	SDA_n = IO_C2;
    SCL_n = IO_C1;
#endif	
    gpio_init_io (CSN_n, GPIO_OUTPUT);
	gpio_init_io (SCL_n, GPIO_OUTPUT);
	gpio_init_io (SDA_n, GPIO_OUTPUT);
	
	gpio_set_port_attribute(CSN_n, 1);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_set_port_attribute(SDA_n, 1);
	
    serial_cmd_1(0x0800);

	R_TFT_HS_WIDTH			= 0;				//	1		=HPW
	R_TFT_H_START			= 1+239;			//	240		=HPW+HBP
	R_TFT_H_END				= 1+239+1280;	//	1520	=HPW+HBP+HDE
	R_TFT_H_PERIOD			= 1+239+1280+40;	//	1560	=HPW+HBP+HDE+HFP
	R_TFT_VS_WIDTH			= 0;				//	1		=VPW				(DCLK)
	R_TFT_V_START			= 21;			//	21		=VPW+VBP			(LINE)
	R_TFT_V_END				= 21+240;		//	261		=VPW+VBP+VDE		(LINE)
	R_TFT_V_PERIOD			= 21+240+1;		//	262		=VPW+VBP+VDE+VFP	(LINE)
	R_TFT_LINE_RGB_ORDER    = 0x00;
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS052);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
}

#if 0
void chilin_spi_write(INT32U cmd) 
{
	INT32S i;
	
	gpio_write_io(CSN_n,1);
	gpio_write_io(SCL_n,1);
	gpio_write_io(SDA_n,1);
	
	// set csn low
	gpio_write_io(CSN_n,0);//CS=0
	cmd_delay(1);
	
	for (i=0;i<24;i++) {
		gpio_write_io(SCL_n,0);
		cmd_delay(1);
		// shift data
		gpio_write_io(SDA_n, ((cmd&0x800000)>>23));
		cmd = (cmd<<1);
		cmd_delay(1);
		gpio_write_io(SCL_n,1);
		cmd_delay(1);
	}
	
	// set csn high
	gpio_write_io(CSN_n,1);//CS=1
	
	cmd_delay(1);

}
#endif

void tft_chilin_lq035nc111_init(void)
{	
	/* 320*240 */
	INT8U sdata[3];
	INT32U spi_ctrl = R_SPI0_CTRL; /* TFT spi pin share with spi flash */
	
	gpio_init_io(IO_A9,GPIO_OUTPUT);
	gpio_set_port_attribute(IO_A9,1);
	
#if 0
	R_SPI0_CTRL = 0;
	
	CSN_n = IO_A8;
	SCL_n = IO_C1;
	SDA_n = IO_C2;
    
    gpio_init_io (CSN_n, GPIO_OUTPUT);
	gpio_init_io (SCL_n, GPIO_OUTPUT);
	gpio_init_io (SDA_n, GPIO_OUTPUT);
	
	gpio_set_port_attribute(CSN_n, 1);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_set_port_attribute(SDA_n, 1);
#else
	spi_cs_by_internal_set(SPI_0,IO_A8, 0); /* 3.5" turnkey board V1.0 */ 
	spi_clk_set(SPI_0,SYSCLK_128); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
#endif

	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 18;
	R_TFT_V_END	= 18+240;
	
	R_TFT_H_PERIOD = 1224;
	R_TFT_H_START = 204;
	R_TFT_H_END	= 204+960;
	
	R_TFT_HS_WIDTH = 90;
	R_TFT_VS_WIDTH = 3;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 204;
	R_TFT_HS_END = 0;	
	
	gpio_write_io(IO_A9,0);
	drv_msec_wait(5);
	gpio_write_io(IO_A9,1);
	drv_msec_wait(5);

#if 0
	chilin_spi_write(0x700003);
	chilin_spi_write(0x726164);
	chilin_spi_write(0x700005);
	chilin_spi_write(0x72bcd4); /* DE mode */
#else
	gp_memcpy((INT8S*)sdata,(INT8S*)"\x70\x00\x03",3);
	spi_transceive_cpu(SPI_0, sdata, 3, sdata, 3);
	gp_memcpy((INT8S*)sdata,(INT8S*)"\x72\x61\x64",3);
	spi_transceive_cpu(SPI_0, sdata, 3, sdata, 3);
	
	gp_memcpy((INT8S*)sdata,(INT8S*)"\x70\x00\x05",3);
	spi_transceive_cpu(SPI_0, sdata, 3, sdata, 3);
	gp_memcpy((INT8S*)sdata,(INT8S*)"\x72\xbc\xd4",3);
	spi_transceive_cpu(SPI_0, sdata, 3, sdata, 3);
	
	spi_cs_by_external_set(SPI_0);
#endif

	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_gamma_en_set(TRUE);
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS = 60 */
			
	R_SPI0_CTRL = spi_ctrl;
}

void tianma_spi_write(INT32U cmd) 
{
	INT32S i;
	
	gpio_write_io(CSN_n,1);
	gpio_write_io(SCL_n,1);
	gpio_write_io(SDA_n,1);
	
	// set csn low
	gpio_write_io(CSN_n,0);//CS=0
	cmd_delay(1);
	
	for (i=0;i<16;i++) {
		gpio_write_io(SCL_n,0);
		cmd_delay(1);
		// shift data
		gpio_write_io(SDA_n, ((cmd&0x8000)>>15));
		cmd = (cmd<<1);
		cmd_delay(1);
		gpio_write_io(SCL_n,1);
		cmd_delay(1);
	}
	
	// set csn high
	gpio_write_io(CSN_n,1);//CS=1
	
	cmd_delay(1);

}

void tft_tianma_tm035kdh03_init(void)
{	
	/* 320*240 */
	INT32U spi_ctrl = R_SPI0_CTRL;
	
	gpio_init_io(IO_A9,GPIO_OUTPUT);
	gpio_set_port_attribute(IO_A9,1);

	R_SPI0_CTRL = 0; /* disable spi */
	
	CSN_n = IO_A8;
	SCL_n = IO_C1; /* shared with spi pin */
	SDA_n = IO_C2;
	
    gpio_init_io (CSN_n, GPIO_OUTPUT);
	gpio_init_io (SCL_n, GPIO_OUTPUT);
	gpio_init_io (SDA_n, GPIO_OUTPUT);
	
	gpio_set_port_attribute(CSN_n, 1);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_set_port_attribute(SDA_n, 1);
	gpio_write_io(CSN_n,1);

	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 13;
	R_TFT_V_END	= 13+240;
	
	R_TFT_H_PERIOD = 1716;
	R_TFT_H_START = 70;
	R_TFT_H_END	= 70+960;
	
	R_TFT_HS_WIDTH = 90;
	R_TFT_VS_WIDTH = 3;
	
	R_TFT_VS_START = 13;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 70;
	R_TFT_HS_END = 0;	
	
	gpio_write_io(IO_A9,0);
	drv_msec_wait(5);
	gpio_write_io(IO_A9,1);
	drv_msec_wait(5);

	tianma_spi_write(0x0207);
	drv_msec_wait(1);
	tianma_spi_write(0x0ec9); /* 8 bit DE mode */
	
	//tianma_spi_write(0x0602); /* dith off */
 	drv_msec_wait(1);           
 	tianma_spi_write(0x3AEA); /* VCOMDC */   
 	drv_msec_wait(1);     
 	//tianma_spi_write(0x4217); /* gamma */
 	drv_msec_wait(1);    
 	//tianma_spi_write(0x463f); /* gamma */ 
 	drv_msec_wait(1);    
 	//tianma_spi_write(0x4a2F); /* gamma */
 	drv_msec_wait(1);    
 	//tianma_spi_write(0x3E90); /* AC = 4.6V,VGH=13,VGL=-10 */
 	tianma_spi_write(0x3EA0); /* AC = 4.6V,VGH=15,VGL=-10 */
 	drv_msec_wait(1);

	R_FUNPOS1 |= 0x40; /* disable hsync and vsync signal */
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_gamma_en_set(TRUE);
	tft_vsync_unit_set(TRUE);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_clk_set(TFT_CLK_DIVIDE_4);
			
	R_SPI0_CTRL = spi_ctrl; /* recovery spi setting */
}

void tft_GPM879A0_init(void)
{	/* 320RGB*240 */
#if 1
	INT32S temp = 241;
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 18;
	R_TFT_V_END	= 18+240;
	
	R_TFT_H_PERIOD = 1560;
	R_TFT_H_START = temp;
	R_TFT_H_END	= temp+1280;
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 18; 
	R_TFT_CLIP_V_END =  18+240;
	R_TFT_CLIP_H_START = temp;
	R_TFT_CLIP_H_END = temp+1280;
	
	R_TFT_HS_WIDTH = 3;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = temp;
	R_TFT_HS_END = 0;		
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS052);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_4);		
#else
	int temp = 241;
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 18;
	R_TFT_V_END	= 18+240;
	
	R_TFT_H_PERIOD = 1560;
	R_TFT_H_START = temp;
	R_TFT_H_END	= temp+960;
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 18; 
	R_TFT_CLIP_V_END =  18+240;
	R_TFT_CLIP_H_START = temp;
	R_TFT_CLIP_H_END = temp+960;
	
	R_TFT_HS_WIDTH = 3;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = temp;
	R_TFT_HS_END = 0;		
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_4);	
#endif	

#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	gpio_write_io(SCL_n,1);//SCL=1
#endif

	spi_cs_by_internal_set(SPI_0,SPI_CS, 0);
	spi_clk_set(SPI_0,SYSCLK_128); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);		
/*
	tft_GPM879A0_spi_write(0x0B,0x00); // Hsync+Vsync Mode ; line invertion
	drv_mese_wait(100);
	tft_GPM879A0_spi_write(0x02,0x36); // Set Vcom DC voltage
	drv_mese_wait(100);	
	tft_GPM879A0_spi_write(0x0C,0x22); // Set DC-DC clk; DC1:DCLK/32; DC2:DCLK/32
	drv_mese_wait(100);
	tft_GPM879A0_spi_write(0x01,0x16); // Set Vcom AC voltage
	drv_mese_wait(100);
	tft_GPM879A0_spi_write(0x02,0x36); // 
	drv_mese_wait(100);	
	tft_GPM879A0_spi_write(0x03,0x09); // 
	drv_mese_wait(100);		
	tft_GPM879A0_spi_write(0x04,0x01); // 
	drv_mese_wait(100);		
	tft_GPM879A0_spi_write(0x05,0x22); // 
	drv_mese_wait(100);		

	tft_GPM879A0_spi_write(0x06,0x0A); // 
	drv_mese_wait(100);			

	tft_GPM879A0_spi_write(0x08,0x11); // 
	drv_mese_wait(100);			
	tft_GPM879A0_spi_write(0x09,0x80); // 
	drv_mese_wait(100);			
*/
	tft_GPM879A0_spi_write(0x06,0x2A); // 
	drv_msec_wait(100);			


	tft_GPM879A0_spi_write(0x0A,0x4F); // 
	drv_msec_wait(100);			
/*	tft_GPM879A0_spi_write(0x0B,0x05); // 
	drv_mese_wait(100);			
	tft_GPM879A0_spi_write(0x0C,0x44); // 
	drv_mese_wait(100);			
	tft_GPM879A0_spi_write(0x0D,0x0A); // 
	drv_mese_wait(100);			
	tft_GPM879A0_spi_write(0x0E,0x08); // 
	drv_mese_wait(100);			
	tft_GPM879A0_spi_write(0x0F,0x80); // 
	drv_mese_wait(100);			

	tft_GPM879A0_spi_write(0x07,0xEF); // 
	drv_mese_wait(200);				
	tft_GPM879A0_spi_write(0x30,0x05); // 
	drv_mese_wait(100);				
*/	
	
#if 0
	R_PWM_CTRL = 0x3f10;
	drv_msec_wait(500);
	R_PWM_CTRL |= 0x4; /* enable PWM1 LED backlight*/
#endif	
}

void tft_gp_gpm758a0s_init(void) 
{
	/* 320*240 */
	
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	
	serial_cmd_1 (0x0B01);
	
	serial_cmd_1 (0x06BF);//add by yc				
	serial_cmd_1 (0x0A59);//add by yc
	cmd_delay (100);

	serial_cmd_1 (0x0236);
	
	serial_cmd_1 (0x0C22);
	
	serial_cmd_1 (0x0116);
	
	serial_cmd_1 (0x10a7);
	serial_cmd_1 (0x1157);
	serial_cmd_1 (0x1273);
	serial_cmd_1 (0x1372);
	serial_cmd_1 (0x1473);
	serial_cmd_1 (0x1555);
	serial_cmd_1 (0x1617);
	serial_cmd_1 (0x1762);

	R_TFT_HS_WIDTH			= 0;				//			=HPW
	R_TFT_H_START			= 273; //241; //252;				//273		=HPW+HBP
	R_TFT_H_END				= 273+1280; //241+960; //252+1280;	    	//			=HPW+HBP+HDE
	R_TFT_H_PERIOD			= 273+1280+2;				//			=HPW+HBP+HDE+HFP
	R_TFT_VS_WIDTH			= 0;				//			=VPW				(DCLK)
	R_TFT_V_START			= 10;//999 21;//ggg 1+17;			    //			=VPW+VBP			(LINE)
	R_TFT_V_END				= 253;//ggg 17+240+1;		    //			=VPW+VBP+VDE		(LINE)
	R_TFT_V_PERIOD			= 263;//ggg 1+17+240+4;		    	//			=VPW+VBP+VDE+VFP	(LINE)
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV|TFT_DCLK_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV)|(TFT_ENABLE & TFT_DCLK_INV));
	tft_mode_set(TFT_MODE_CCIR656);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_4);
	
}

void tft_gpy0200_innolux_at056tn03_init(void)
{
#if _DRV_L1_GPY0200 == 1
	/* lcd resolution 320*234, ppu use 320*240 */
	gpy0200_i2c_write(0x5f,0xa5);
	
	gpy0200_i2c_write(0x20,0x0b);	//TFT_CTRL_L
	gpy0200_i2c_write(0x21,0x0);	//TFT_CTRL_H
	gpy0200_i2c_write(0x22,0x41);		//{Norm_W} TFT_CTRL2_L
	gpy0200_i2c_write(0x23,0);		//TFT_CTRL2_H
	gpy0200_i2c_write(0x24,0x4F);	//Ts_STH_L = GPL32 H_START
	gpy0200_i2c_write(0x25,0x1);	//Ts_STH_H = GPL32 H_START
	gpy0200_i2c_write(0x26,0x4C);	//H_Start_L= GPL32 H_START - 4
	gpy0200_i2c_write(0x27,0x1);	//H_Start_H= GPL32 H_START - 4
	gpy0200_i2c_write(0x28,10);		//V_Start = GPL32 V_Start
	gpy0200_i2c_write(0x29,3);		//Tw_STH
	gpy0200_i2c_write(0x2a,162);	//T_HS2OEH
	gpy0200_i2c_write(0x2b,36);		//Tw_OEH
	//gp8_spi_write(0x11,177);	//T_DIS1
	gpy0200_i2c_write(0x2c,72);		//T_HS2CKV
	gpy0200_i2c_write(0x2d,108);	//Tw_CKV
	gpy0200_i2c_write(0x2e,18);		//T_HS2OEV
	gpy0200_i2c_write(0x2f,144);	//Tw_OEV
	gpy0200_i2c_write(0x30,8);		//Ts_STV
	gpy0200_i2c_write(0x31,0);		//CLK_DLY
	gpy0200_i2c_write(0x32,0);		//AG
	gpy0200_i2c_write(0x33,162);	//Ts_POL = GP8 T_HS2OEV + Tw_OEV
	
	R_TFT_V_PERIOD = 260;
    R_TFT_V_START  = 10;
    R_TFT_V_END    = 250;

    R_TFT_H_PERIOD = 1500; 
    R_TFT_H_START  = 334;
    R_TFT_H_END    = 334+960-1;
    
    tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 10;
	R_TFT_CLIP_V_END =  244;
	R_TFT_CLIP_H_START = 334;
	R_TFT_CLIP_H_END = 334+960-1;
    
    R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;/*254*/
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;/*1780*/	
	
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_clk_set(TFT_CLK_DIVIDE_4); /* FS=60 */	
#endif
}

void tft_gpy0200_auo_a040cn01_init(void)
{   /* 480dot*234 ,ppu use 320*240 */
#if _DRV_L1_GPY0200 == 1
#if 1
	gpy0200_i2c_write(0x5f,0xa5);
#else /* io as spi */
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_write_io(CSN_n,1);
	
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	//gpio_write_io(SCL_n,1);
	
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	gpio_write_io(SDA_n,0);
	
	gpio_init_io(SDI_n,GPIO_INPUT);
	gpio_set_port_attribute(SDI_n, 0);	
#endif

	gpy0200_i2c_write(0x20,0x01);	//TFT_CTRL_L
	gpy0200_i2c_write(0x21,0x08);	//TFT_CTRL_H
	gpy0200_i2c_write(0x22,0x20);	// TFT_CTRL2_L
	gpy0200_i2c_write(0x23,0x0);	// TFT_CTRL2_H

	gpy0200_i2c_write(0x24,0x83);	//Ts_STH_L = GPL32 H_START - 3
	gpy0200_i2c_write(0x25,0);		//Ts_STH_H = GPL32 H_START - 3
	gpy0200_i2c_write(0x26,0x82);	//H_Start_L= GPL32 H_START - 5
	gpy0200_i2c_write(0x27,0x0);	//H_Start_H= GPL32 H_START - 5
	gpy0200_i2c_write(0x28,10);		//V_Start = GPL32 V_Start
	gpy0200_i2c_write(0x29,3);		//Tw_STH
	gpy0200_i2c_write(0x2a,42);		//T_HS2OEH
	gpy0200_i2c_write(0x2b,9);		//Tw_OEH
	//gp8_spi_write(0x11,51);		//T_DIS1
	gpy0200_i2c_write(0x2c,26);		//T_HS2CKV
	gpy0200_i2c_write(0x2d,84);		//Tw_CKV
	gpy0200_i2c_write(0x2e,6);		//T_HS2OEV
	gpy0200_i2c_write(0x2f,48);		//Tw_OEV
	gpy0200_i2c_write(0x30,8);		//Ts_STV
	gpy0200_i2c_write(0x31,0x4);	//CLK_DLY
	gpy0200_i2c_write(0x32,0x00);	//AG
	gpy0200_i2c_write(0x33,54);	    //Ts_POL = GP8 T_HS2OEV + Tw_OEV
	
	R_TFT_V_PERIOD = 260;
	R_TFT_V_START = 10;
	R_TFT_V_END	= 10+240;
	
	R_TFT_H_PERIOD = 1200;
	R_TFT_H_START = 132;
	R_TFT_H_END	= 132+960-1;/* 320*240 ppu resolution */
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 10;
	R_TFT_CLIP_V_END =  10+234;
	R_TFT_CLIP_H_START = 132;
	R_TFT_CLIP_H_END = 132+480-1; 
	
	R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
#endif
}

void tft_gpy0200_pmv_035bg_init(void)
{   /* 960dot*234,ppu use 320*240 */
#if _DRV_L1_GPY0200 == 1
#if 1
	gpy0200_i2c_write(0x5f,0xa5);
#else /* io as spi */
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_write_io(CSN_n,1);
	
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	//gpio_write_io(SCL_n,1);
	
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	gpio_write_io(SDA_n,0);
	
	gpio_init_io(SDI_n,GPIO_INPUT);
	gpio_set_port_attribute(SDI_n, 0);	
#endif

	gpy0200_i2c_write(0x20,0x03);	//TFT_CTRL_L
	gpy0200_i2c_write(0x21,0x08);	//TFT_CTRL_H
	gpy0200_i2c_write(0x22,0x20);	// TFT_CTRL2_L
	gpy0200_i2c_write(0x23,0x0);	// TFT_CTRL2_H

	gpy0200_i2c_write(0x24,0x83);	//Ts_STH_L = GPL32 H_START - 3
	gpy0200_i2c_write(0x25,0);		//Ts_STH_H = GPL32 H_START - 3
	gpy0200_i2c_write(0x26,0x82);	//H_Start_L= GPL32 H_START - 5
	gpy0200_i2c_write(0x27,0x0);	//H_Start_H= GPL32 H_START - 5
	gpy0200_i2c_write(0x28,10);		//V_Start = GPL32 V_Start
	gpy0200_i2c_write(0x29,3);		//Tw_STH
	gpy0200_i2c_write(0x2a,42);		//T_HS2OEH
	gpy0200_i2c_write(0x2b,9);		//Tw_OEH
	//gp8_spi_write(0x11,51);		//T_DIS1
	gpy0200_i2c_write(0x2c,26);		//T_HS2CKV
	gpy0200_i2c_write(0x2d,84);		//Tw_CKV
	gpy0200_i2c_write(0x2e,6);		//T_HS2OEV
	gpy0200_i2c_write(0x2f,48);		//Tw_OEV
	gpy0200_i2c_write(0x30,8);		//Ts_STV
	gpy0200_i2c_write(0x31,0x4);	//CLK_DLY
	gpy0200_i2c_write(0x32,0x00);	//AG
	gpy0200_i2c_write(0x33,54);	    //Ts_POL = GP8 T_HS2OEV + Tw_OEV
	
	R_TFT_V_PERIOD = 260;
	R_TFT_V_START = 10;
	R_TFT_V_END	= 10+240;
	
	R_TFT_H_PERIOD = 1200;
	R_TFT_H_START = 132;
	R_TFT_H_END	= 132+960-1;/* 320*240 ppu resolution */
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 10;
	R_TFT_CLIP_V_END =  10+234;
	R_TFT_CLIP_H_START = 132;
	R_TFT_CLIP_H_END = 132+960-1; 
	
	R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
#endif
}
#endif //DPF_H_V==DPF_320x240

#if (DPF_H_V==DPF_400x240)
static void i80_wr_cmd(INT8U command)
{
	INT32U reg;
	
	R_TFT_MEM_BUFF_WR = command;
	reg = R_TFT_CTRL;
	reg &= ~(0xF << 4);
	reg |= (0x8 << 4) | 0x01;
	R_TFT_CTRL = reg;
}

static void i80_wr_data(INT8U data)
{
	INT32U reg;
	
	R_TFT_MEM_BUFF_WR = data;
	reg = R_TFT_CTRL;
	reg &= ~(0xF << 4);
	reg |= (0xA << 4) | 0x01;
	R_TFT_CTRL = reg;
}

static INT32U i80_read_data(INT8U command)
{
	INT32U reg, data;
	
	i80_wr_cmd(command);
	
	drv_msec_wait(1);
	
	reg = R_TFT_CTRL;
	reg &= ~(0xF << 4);
	reg |= (0xB << 4) | 0x01;
	R_TFT_CTRL = reg;						
	drv_msec_wait(1);
		
	data = R_TFT_MEM_BUFF_RD;
	return data;
}

static void DISPCMD1(INT8U command, INT8U data)
{	
	i80_wr_cmd(command);
	drv_msec_wait(1);	//delay
	i80_wr_data(data);	
}

void tft_fl320wqc11_OnSeq(INT32U enable)
{
	if(enable) {
		// Display ON Setting
		DISPCMD1(0x28,0x20);
		drv_msec_wait(40);
		DISPCMD1(0x28,0x38);
		drv_msec_wait(40);
		DISPCMD1(0x28,0x3C);
	} else {
		// Display Off
		DISPCMD1(0x28, 0x38);
		drv_msec_wait(40);
		DISPCMD1(0x28, 0x28);
		drv_msec_wait(40);
		DISPCMD1(0x28, 0x20);
		// Power supply halt setting bits
		DISPCMD1(0x1F, 0x94);
		drv_msec_wait(10);
		DISPCMD1(0x1F, 0x8C);
		DISPCMD1(0x1C, 0x00);
		DISPCMD1(0x1F, 0x8D);
	}
}

void tft_fox_fl320wqc11_init(void)
{
	INT32U reg;
	
	R_TFT_CTRL = 0;
	R_TFT_CTRL &= ~(1 << 13);  	//word mode 
	R_TFT_CTRL |= 1 << 1;		//set clock	
	
	R_TFT_TS_MISC &= ~(3 << 10);		
	R_TFT_TS_MISC |= 1 << 10;		//565 mode 
	
	R_TFT_LINE_RGB_ORDER |= 1 << 7; //memory csb mode
#if 0 //match frame size
    R_TFT_H_PERIOD = 480-1;	//480
	R_TFT_V_PERIOD = 272-1;	//272  
#else //free size
	R_TFT_H_PERIOD = 400-1;	//400
	R_TFT_V_PERIOD = 240-1;	//240  
#endif
    R_TFT_HS_START = 0;		//x+
    R_TFT_HS_END = 	400-1;	//480-1+x

	R_TFT_VS_START = 0;		//0+y
    R_TFT_VS_END = 240;		//240-1+y
	    
    R_TFT_V_START = 1;		//Setup time 
    R_TFT_VS_WIDTH = 20;	//Blanking time
    R_TFT_H_START = 1;		//R/W pulse "high" period = (TFT_H_Start + 1)
    R_TFT_HS_WIDTH = 1;		//R/W pulse "low" period =  (TFT_HS_Width + 1)

	//init display
	i80_read_data(0x00);
			
	DISPCMD1(0xE5,0x10);
	DISPCMD1(0xE7,0x10);
	DISPCMD1(0xE8,0x48);
	DISPCMD1(0xEC,0x09);
	DISPCMD1(0xED,0x6C);

	// Power on Setting
	DISPCMD1(0x23,0x6B);
	DISPCMD1(0x24,0x57);
	DISPCMD1(0x25,0x71);
	DISPCMD1(0xE2,0x18);
	DISPCMD1(0x1B,0x15);
	DISPCMD1(0x01,0x00);
	DISPCMD1(0x1C,0x03);

	// Power on sequence
	DISPCMD1(0x19,0x01);
	drv_msec_wait(10);
	DISPCMD1(0x1F,0x8C);
	DISPCMD1(0x1F,0x84);
	drv_msec_wait(20);
	DISPCMD1(0x1F,0x94);
	drv_msec_wait(20);
	DISPCMD1(0x1F,0xD4);
	drv_msec_wait(10);

	// Gamma Setting
	DISPCMD1(0x40,0x00);
	DISPCMD1(0x41,0x2E);
	DISPCMD1(0x42,0x2C);
	DISPCMD1(0x43,0x3E);
	DISPCMD1(0x44,0x3D);
	DISPCMD1(0x45,0x3F);
	DISPCMD1(0x46,0x2A);
	DISPCMD1(0x47,0x7E);
	DISPCMD1(0x48,0x08);
	DISPCMD1(0x49,0x05);
	DISPCMD1(0x4A,0x06);
	DISPCMD1(0x4B,0x0B);
	DISPCMD1(0x4C,0x17);
	DISPCMD1(0x50,0x00);
	DISPCMD1(0x51,0x02);
	DISPCMD1(0x52,0x01);
	DISPCMD1(0x53,0x13);
	DISPCMD1(0x54,0x11);
	DISPCMD1(0x55,0x3F);
	DISPCMD1(0x56,0x01);
	DISPCMD1(0x57,0x55);
	DISPCMD1(0x58,0x08);
	DISPCMD1(0x59,0x14);
	DISPCMD1(0x5A,0x19);
	DISPCMD1(0x5B,0x1A);
	DISPCMD1(0x5C,0x17);
	DISPCMD1(0x5D,0xFF);

#define R16_DATA 	0x28
#define SCR_X		400	
#define SCR_Y		240	

	DISPCMD1(0x16,R16_DATA | 0x00);		// R16_DATA=0x28
	DISPCMD1(0x17, 0x05);
	DISPCMD1(0x60, 0x08);				//TE pin enable

	DISPCMD1(0x02, 0x00);
	DISPCMD1(0x03, 0x00);
	DISPCMD1(0x04, (SCR_X - 1) >> 8);	// SCR_X=400
	DISPCMD1(0x05, (SCR_X - 1) & 0xff);

	DISPCMD1(0x06, 0x00);
	DISPCMD1(0x07, 0x00);
	DISPCMD1(0x08, (SCR_Y - 1) >> 8);	// SCR_Y=240
	DISPCMD1(0x09, (SCR_Y - 1) & 0xff);

	DISPCMD1(0x80, (0x00) >> 8);
	DISPCMD1(0x81, (0x00) & 0xff);
	DISPCMD1(0x82, (0x00) >> 8);
	DISPCMD1(0x83, (0x00) & 0xff);
	
	tft_fl320wqc11_OnSeq(1);
	drv_msec_wait(200);
	
	//write to GRAM
	i80_wr_cmd(0x22);	
	
	reg = R_TFT_CTRL;
	reg &= ~(0x0F << 4);
	reg |= (0xC << 4) | 0x01;
	R_TFT_CTRL = reg;
}
#endif //DPF_H_V==DPF_400x240

#if (DPF_H_V==DPF_160x128)
static void i80_write_cmd(INT8U cmd)
{
	R_TFT_MEM_BUFF_WR = cmd;
	R_TFT_CTRL = 0x2087;
}

static void i80_write_data(INT8U data)
{
	R_TFT_MEM_BUFF_WR = data;
	R_TFT_CTRL = 0x20A7;		
}

void tft_gpm_lm765a0_init(void)
{
#define USE_160_128		1
#define USE_162_132		2
#define TFT_RESOLUTION	USE_160_128
	
	tft_data_mode_set(TFT_DATA_MODE_8);
	R_TFT_LINE_RGB_ORDER |= 0x0080; //memory mode
                        
#if 0	//hw reset 
	gpio_init_io(IO_C3, GPIO_OUTPUT);	
	gpio_set_port_attribute(IO_C3, ATTRIBUTE_HIGH);
    gpio_write_io(IO_C3, DATA_LOW);
    drv_msec_wait(200);
    drv_msec_wait(200);
    gpio_write_io(IO_C3, DATA_HIGH);
    drv_msec_wait(200);	
#endif

#if 0 //match frame size 
	R_TFT_V_PERIOD = 240-1;	//240  
    R_TFT_H_PERIOD = 320-1;	//320
#else //free size	
	#if TFT_RESOLUTION == USE_162_132    
	R_TFT_V_PERIOD = 132-1;	
    R_TFT_H_PERIOD = 162-1;	
	#elif TFT_RESOLUTION == USE_160_128    
	R_TFT_V_PERIOD = 128-1;	 
    R_TFT_H_PERIOD = 160-1;	
	#endif    
#endif    
    R_TFT_V_START = 1;		//Setup time 
    R_TFT_VS_WIDTH = 20;	//Blanking time
    R_TFT_H_START = 1;		//R/W pulse "high" period = (TFT_H_Start + 1)
    R_TFT_HS_WIDTH = 1;		//R/W pulse "low" period =  (TFT_HS_Width + 1)

#if TFT_RESOLUTION == USE_162_132    
    R_TFT_VS_START = 0;		//0+y
    R_TFT_VS_END = 132;		//132
    R_TFT_HS_START = 0;		//0+x
    R_TFT_HS_END = 	162-1;	//162
#elif TFT_RESOLUTION == USE_160_128
	R_TFT_VS_START = 0;		//0+y
    R_TFT_VS_END = 128;		//128
    R_TFT_HS_START = 0;		//x+0
    R_TFT_HS_END = 	160-1;	//160-1+x
#endif    
	//Start initial sequence 		    
	i80_write_cmd(0x11);	//00h
	drv_msec_wait(200);

	i80_write_cmd(0xC0); 	//GVDD
	i80_write_data(0x02);
	i80_write_data(0x00);	//VCI 1
	drv_msec_wait(200);
	drv_msec_wait(200);
		
	i80_write_cmd(0xc1); //AVDD VGL VGL VCL
	i80_write_data(0x05);

	i80_write_cmd(0xc5);  //SET VCOMH
	i80_write_data(0xBA); //08	

	i80_write_cmd(0xc6);  //VCOMAC
	i80_write_data(0x0A);	//0D

	i80_write_cmd(0x3A);
	i80_write_data(0x05);	//RGB-565
	cmd_longdelay(200);

	i80_write_cmd(0x36);	//show h or v
	i80_write_data(0x60);	//by zhangxh 0xC0	

	i80_write_cmd(0xE0);  //0Eh gamma set
	i80_write_data(0x00); //V0
	i80_write_data(0x01); //V1  01
	i80_write_data(0x0E); //V2  0E
	i80_write_data(0x35); //V61  35
	i80_write_data(0x2B); //V62
	i80_write_data(0x0B); //V63
	i80_write_data(0x1C); //V13  1F 
	i80_write_data(0x01); //V50      
	i80_write_data(0x00); //V4
	i80_write_data(0x04); //V8   04
	i80_write_data(0x0C); //V20  0C
	i80_write_data(0x0F); //V27  0A
	i80_write_data(0x00); //V36  00
	i80_write_data(0x08); //V43  08 06
	i80_write_data(0x03); //V55  02
	i80_write_data(0x06); //V59  06
	 	
	i80_write_cmd(0xE1);  //E1h gamma set
	i80_write_data(0x06); //V0
	i80_write_data(0x23); //V1
	i80_write_data(0x20); //V2
	i80_write_data(0x0E); //V61   OE
	i80_write_data(0x0A); //V62
	i80_write_data(0x04); //V63
	i80_write_data(0x04); //V13
	i80_write_data(0x19); //V50  1B
	i80_write_data(0x06); //V4   06
	i80_write_data(0x04); //V8   03
	i80_write_data(0x04); //V20  04 02
	i80_write_data(0x04); //V27  04
	i80_write_data(0x0A); //V36  05
	i80_write_data(0x0C); //V43  0C
	i80_write_data(0x04); //V55  04
	i80_write_data(0x05); //V59 

	//column address set 
	i80_write_cmd(0x2A);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x00);
#if TFT_RESOLUTION == USE_162_132    
	i80_write_data(0xa1); 	 //162
#elif TFT_RESOLUTION == USE_160_128    
	i80_write_data(0x9F);	//160
#endif

	//row address set
	i80_write_cmd(0x2B);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x00);
#if TFT_RESOLUTION == USE_162_132    
	i80_write_data(0x83);
#elif TFT_RESOLUTION == USE_160_128    
	i80_write_data(0x7F);
#endif 

	//start show
	i80_write_cmd(0x29);
	i80_write_cmd(0x2C);
	      
	R_TFT_CTRL = 0x20C3; //system clock/2 
}

void tft_gpm_lm765h0_init(void)
{
#define PANEL_128_160	1
#define PANEL_160_128	2
#define PANEL_USEMODE	PANEL_160_128

	tft_data_mode_set(TFT_DATA_MODE_8);
	R_TFT_LINE_RGB_ORDER |= 0x0080; //memory mode
                        
#if 0	//hw reset 
	gpio_init_io(IO_C3, GPIO_OUTPUT);	
	gpio_set_port_attribute(IO_C3, ATTRIBUTE_HIGH);
    gpio_write_io(IO_C3, DATA_LOW);
    drv_msec_wait(200);
    drv_msec_wait(200);
    gpio_write_io(IO_C3, DATA_HIGH);
    drv_msec_wait(200);	
#endif 
#if 0 //match frame size 
	R_TFT_V_PERIOD = 240-1;	//240  
    R_TFT_H_PERIOD = 320-1;	//320
#else //free size
	#if PANEL_USEMODE == PANEL_128_160
	R_TFT_V_PERIOD = 160-1;	
    R_TFT_H_PERIOD = 128-1;	
    #elif PANEL_USEMODE	== PANEL_160_128
    R_TFT_V_PERIOD = 128-1;	
    R_TFT_H_PERIOD = 160-1;	
    #endif
#endif
    
    R_TFT_V_START = 1;		//Setup time 
    R_TFT_VS_WIDTH = 20;	//Blanking time
    R_TFT_H_START = 1;		//R/W pulse "high" period = (TFT_H_Start + 1)
    R_TFT_HS_WIDTH = 1;		//R/W pulse "low" period =  (TFT_HS_Width + 1)

#if PANEL_USEMODE == PANEL_128_160	     
	R_TFT_VS_START = 0;		//0+y
    R_TFT_VS_END = 160;		//128-1+y
    
    R_TFT_HS_START = 0;		//x+
    R_TFT_HS_END = 	128-1;	//160-1+x
#elif PANEL_USEMODE	== PANEL_160_128	
    R_TFT_VS_START = 0;		//0+y
    R_TFT_VS_END = 128;		//128-1+y
    
    R_TFT_HS_START = 0;		//x+
    R_TFT_HS_END = 	160-1;	//160-1+x
#endif    
	//Start initial sequence 		    
    i80_write_cmd(0x11);  //sleep out
    drv_msec_wait(200);
    drv_msec_wait(200);
    
	i80_write_cmd(0x26);
    i80_write_data(0x04); 
    
    i80_write_cmd(0xCD);
    i80_write_data(0x04);
	
    i80_write_cmd(0xB1);  //Frame Rate control
    i80_write_data(0x0e);
    i80_write_data(0x14);
    
    i80_write_cmd(0xc0);  //GVDD step = 0.05V
    i80_write_data(0x08);
    i80_write_data(0x00);
	
	i80_write_cmd(0xc1);  //AVDD VGL VGL VCL
	i80_write_data(0x05); //VGH = 16.5V  VGL=-8.25V 
	
	i80_write_cmd(0xc5);  //SET VCOMH & VCOML	
	i80_write_data(0x3a);
	i80_write_data(0x2d);
		
	i80_write_cmd(0x35); //TE on
	i80_write_data(0x00);
			
	i80_write_cmd(0x3A); //interface format
	//i80_write_data(0x06);	//18bit
	i80_write_data(0x05);	//16bit	
	
	i80_write_cmd(0x002A);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x7f);
	
    i80_write_cmd(0x002B);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x00);
	i80_write_data(0x9f);
	
    i80_write_cmd(0x00b4);
	i80_write_data(0x00);	 
	
	i80_write_cmd(0xF2);  //Gamma set E0.E1 enable control
	i80_write_data(0x01); //01 = enable    00=disable
    
  	i80_write_cmd(0xE0);  //0Eh gamma set
	i80_write_data(0x3F); //V0     -> V63
	i80_write_data(0x2b); //V1     -> V62
	i80_write_data(0x28); //V2     -> V61
	i80_write_data(0x30); //V4     -> V59
	i80_write_data(0x27); //V6     -> V57
	i80_write_data(0x0d); //V13    -> V50
	i80_write_data(0x53); //V20    -> V43
	i80_write_data(0xfa); //V36 27 -> V27 36
	i80_write_data(0x3c); //V43    -> V20
	i80_write_data(0x19); //V50    -> V13
	i80_write_data(0x22); //V57    -> V6
	i80_write_data(0x1e); //V59    -> V4
	i80_write_data(0x02); //V61    -> V2
	i80_write_data(0x01); //V62    -> V1
	i80_write_data(0x00); //V63    ->	V0
	 	
	i80_write_cmd(0xE1);  //E1h gamma set
	i80_write_data(0x00); //V0	->V63
	i80_write_data(0x1b); //V1	->V62
	i80_write_data(0x1f); //V2	->V61
	i80_write_data(0x0f); //V4    ->V59
	i80_write_data(0x16); //V6 	->V57
	i80_write_data(0x13); //V13	->V50
	i80_write_data(0x30); //V20	->V43
	i80_write_data(0x84); //V36 V27  ->V27 36
	i80_write_data(0x43); //V43   ->V20
	i80_write_data(0x06); //V50   ->V13
	i80_write_data(0x1d); //V57   ->V6
	i80_write_data(0x21); //V59   ->V4
	i80_write_data(0x3d); //V61   ->V2
	i80_write_data(0x3e); //V62   ->V1
	i80_write_data(0x3f); //V63   ->V0
	
	//memory access control
	i80_write_cmd(0x36);
#if PANEL_USEMODE == PANEL_128_160	
	i80_write_data(0x08);
#elif  PANEL_USEMODE == PANEL_160_128	
	i80_write_data(0xA8);
#endif

	//column address set 
   	i80_write_cmd(0x2A);
   	i80_write_data(0x00);
   	i80_write_data(0x00);
   	i80_write_data(0x00);
#if PANEL_USEMODE == PANEL_128_160	
   	i80_write_data(0x7F);	//160
#elif  PANEL_USEMODE == PANEL_160_128		
   	i80_write_data(0x9F);  
#endif

   	//row address set
   	i80_write_cmd(0x2B);
   	i80_write_data(0x00);
   	i80_write_data(0x00);
   	i80_write_data(0x00);   
#if PANEL_USEMODE == PANEL_128_160	
	i80_write_data(0x9F);
#elif  PANEL_USEMODE == PANEL_160_128		
	i80_write_data(0x7F);
#endif	
 	//start show
   	i80_write_cmd(0x29);
    i80_write_cmd(0x2C);
   
	R_TFT_CTRL = 0x20C3;
}
#endif //DPF_H_V==DPF_160x128

#if 0 /* diaplay by himax */
void tft_innolux_at070tn07_init(void)
{   /* 480RGB*234 */
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 19;
	R_TFT_V_END	= 253;
	
	R_TFT_H_PERIOD = 2010;/*1800*/
	R_TFT_H_START = 277;
	R_TFT_H_END	= 1717;
	
	R_TFT_HS_WIDTH = 5;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;/*254*/
	R_TFT_HS_START = 177;
	R_TFT_HS_END = 0;/*1780*/
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_3);   /*FS=60 @96MHz*/
	
	drv_msec_wait(100); /*delay 100ms */

#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	gpio_write_io(SCL_n,1);//SCL=1
#endif
	
	spi_cs_by_internal_set(SPI_0,SPI_CS, 1);
	spi_clk_set(SPI_0,SYSCLK_64); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
	
	tft_himax_spi_write(0x0,0x67);
	tft_himax_spi_write(0x06,0x44);
	//tft_himax_spi_write(0x01,0x57);
}

void tft_auo_c065gw02_init(void)
{   /* 400RGB*234 */
	R_TFT_V_PERIOD = 262; /* ppu resolution 480*234 */
	R_TFT_V_START = 19;
	R_TFT_V_END	= 253;
	
	R_TFT_H_PERIOD = 2010;
	R_TFT_H_START = 210;
	R_TFT_H_END	= 1650;
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 19;
	R_TFT_CLIP_V_END =  253;
	R_TFT_CLIP_H_START = 210;
	R_TFT_CLIP_H_END = 1410;
	
	R_TFT_HS_WIDTH = 5;
	
	R_TFT_VS_START = 18;
	//R_TFT_VS_END = 254;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 177;
	R_TFT_HS_END = 1780;
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS = 60 */
	
	drv_msec_wait(100); /*delay 100ms */
#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	gpio_write_io(SCL_n,1);//SCL=1
#endif

	spi_cs_by_internal_set(SPI_0,SPI_CS, 1);
	spi_clk_set(SPI_0,SYSCLK_64); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
	
	tft_himax_spi_write(0x0,0x66);
	tft_himax_spi_write(0x06,0x44);
	//tft_himax_spi_write(0x01,0x57);
}

void tft_innolux_at056tn03_init(void)
{	/* 320RGB*234 */
	R_TFT_V_PERIOD = 262; /* ppu resloution 320*240 */
	R_TFT_V_START = 19;
	R_TFT_V_END	= 253;
	
	R_TFT_H_PERIOD = 1210;
	R_TFT_H_START = 177;
	R_TFT_H_END	= 1137;
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 19; 
	R_TFT_CLIP_V_END =  259;
	R_TFT_CLIP_H_START = 177;
	R_TFT_CLIP_H_END = 1137;
	
	R_TFT_HS_WIDTH = 5;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 177;
	R_TFT_HS_END = 0;
	
	tft_signal_inv_set(TFT_DCLK_INV|TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_DCLK_INV)|(TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_4);
	
	drv_msec_wait(100); /*delay 100ms */
#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	gpio_write_io(SCL_n,1);//SCL=1
#endif
	spi_cs_by_internal_set(SPI_0,SPI_CS, 1);
	spi_clk_set(SPI_0,SYSCLK_64); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
	
	tft_himax_spi_write(0x0,0x65);
	tft_himax_spi_write(0x06,0x44);
	
	//tft_himax_spi_write(0x06,0x04);
	//tft_himax_spi_write(0x06,0x44);
	//tft_himax_spi_write(0x01,0x57);
}

void tft_auo_a040cn01_init(void)
{   /* 480dot*234 */
	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 19;
	R_TFT_V_END	= 259;
	
	R_TFT_H_PERIOD = 1100;
	R_TFT_H_START = 80;
	R_TFT_H_END	= 1040;/* 320*240 ppu resolution */
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 19;
	R_TFT_CLIP_V_END =  253;
	R_TFT_CLIP_H_START = 80;
	R_TFT_CLIP_H_END = 560; 
	
	R_TFT_HS_WIDTH = 5;
	
	R_TFT_VS_START = 18;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 80;
	R_TFT_HS_END = 0;
	
	R_TFT_LINE_RGB_ORDER = 0x03;
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_5); /* FS=66 */
	
	drv_msec_wait(100); /*delay 100ms */
#if 0
	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
	
	gpio_write_io(SCL_n,1);//SCL=1
#endif
	
	spi_cs_by_internal_set(SPI_0,SPI_CS, 1);
	spi_clk_set(SPI_0,SYSCLK_128); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
	
	tft_himax_spi_write(0x0,0x60);
	tft_himax_spi_write(0x06,0x44);
	//tft_himax_spi_write(0x01,0x57);
}

void tft_pmv_035bg_init(void)
{   /* 960dot*234 */

	R_TFT_V_PERIOD = 262;
	R_TFT_V_START = 19;
	R_TFT_V_END	= 19+240;
	
	R_TFT_H_PERIOD = 1210;
	R_TFT_H_START = 132;
	R_TFT_H_END	= 132+960;/* 320*240 ppu resolution */
	
	tft_slide_en_set(TRUE);
	
	R_TFT_CLIP_V_START = 19;
	R_TFT_CLIP_V_END =  19+234;
	R_TFT_CLIP_H_START = 132;
	R_TFT_CLIP_H_END = 132+960; 

	R_TFT_VS_START = 1;
	R_TFT_VS_END = 0;
	R_TFT_HS_START = 1;
	R_TFT_HS_END = 0;
	
	R_TFT_LINE_RGB_ORDER = 0x40;
	
	tft_signal_inv_set(TFT_VSYNC_INV|TFT_HSYNC_INV, (TFT_ENABLE & TFT_VSYNC_INV)|(TFT_ENABLE & TFT_HSYNC_INV));
	tft_mode_set(TFT_MODE_UPS051);
	tft_data_mode_set(TFT_DATA_MODE_8);
	tft_clk_set(TFT_CLK_DIVIDE_3); /* FS=66 */
	
	spi_cs_by_internal_set(SPI_0,SPI_CS, 1);
	spi_clk_set(SPI_0,SYSCLK_128); 
	spi_pha_pol_set(SPI_0,PHA1_POL1);
	
	tft_himax_spi_write(0x0,0x61);
	tft_himax_spi_write(0x06,0x44);
}

void tft_himax_spi_write(INT8U reg, INT8U data)
{
#if 0
	INT32U ser_data = 0;
	INT16U temp = 0;
#endif
	
	INT8U ser_data[2] = {0};
	INT8U rcv[2];
	
#if 0
	temp |= reg << 8;
	temp |= data;
	ser_data = temp << 2;
	himax_serial_cmd(ser_data);
#else
	ser_data[0] = (reg<<2) | ((data&0xC0) >> 6);
	ser_data[1] = data << 2;
	spi_transceive(SPI_0,ser_data, 2, rcv, 2);
#endif
}

#endif /* display by himax */

void tft_tft_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_CTRL |= TFT_EN;
	}		
	else {
		R_TFT_CTRL &= ~TFT_EN;
	}
}

void tft_clk_set(INT32U clk)
{
#if MCU_VERSION != GPL326XX
	R_TFT_CTRL &= ~TFT_CLK_SEL;
	R_TFT_CTRL |= clk;	
	
#else	
	R_TFT_CTRL &= ~TFT_CLK_SEL;
	R_TFT_TS_MISC &= ~0xC0;


	if (clk < TFT_CLK_DIVIDE_9) {
		R_TFT_CTRL |= clk;
	}
	else {
		R_TFT_CTRL |= clk & 0xF;
		R_TFT_TS_MISC |= (clk & 0x20) << 1;
		R_TFT_TS_MISC |= (clk & 0x10) << 3;
	}
	
#endif
}

void tft_mode_set(INT32U mode)
{
	R_TFT_CTRL &= ~TFT_MODE;
	R_TFT_CTRL |= mode;		
}

void tft_mem_mode_ctrl_set(INT32U mem_ctrl)
{
	R_TFT_CTRL &= ~TFT_MODE;
	R_TFT_CTRL |= (mem_ctrl|TFT_EN);		
}

void tft_signal_inv_set(INT32U mask, INT32U value)
{
	/*set vsync,hsync,dclk and DE inv */
	R_TFT_CTRL &= ~mask;
	R_TFT_CTRL |= (mask & value);		
}

void tft_h_compress_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_CTRL |= TFT_H_COMPRESS;
	}
	else {
		R_TFT_CTRL &= ~TFT_H_COMPRESS;
	}
}

void tft_mem_unit_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_CTRL |= TFT_MEM_BYTE_EN;
	}
	else {
		R_TFT_CTRL &= ~TFT_MEM_BYTE_EN; /* word */
	}
}

void tft_interlace_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_CTRL |= TFT_INTERLACE_MOD;
	}
	else {
		R_TFT_CTRL &= ~TFT_INTERLACE_MOD;
	}
}

void tft_vsync_unit_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_CTRL |= TFT_VSYNC_UNIT;
	}
	else {
		R_TFT_CTRL &= ~TFT_VSYNC_UNIT;
	}
}

void tft_dith_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_TS_MISC |= TFT_DITH_EN;
	}	
	else {
		R_TFT_TS_MISC &= ~TFT_DITH_EN;
	}
}

void tft_data_mode_set(INT32U mode)
{
	R_TFT_TS_MISC &= ~TFT_DATA_MODE;
	R_TFT_TS_MISC |= mode;	
}

void tft_rb_switch_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_TS_MISC |= TFT_SWITCH_EN;
	}	
	else {
		R_TFT_TS_MISC &= ~TFT_SWITCH_EN;
	}
}

void tft_gamma_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_TS_MISC |= TFT_GAMMA_EN;
	}	
	else {
		R_TFT_TS_MISC &= ~TFT_GAMMA_EN;
	}
}

void tft_slide_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_TS_MISC |= TFT_SLIDE_EN;
	}	
	else {
		R_TFT_TS_MISC &= ~TFT_SLIDE_EN;
	}
}

void tft_dclk_sel_set(INT32U sel)
{
	R_TFT_TS_MISC &= ~TFT_DCLK_SEL;
	R_TFT_TS_MISC |= sel;		
}

void tft_dclk_delay_set(INT32U delay)
{
	R_TFT_TS_MISC &= ~TFT_DCLK_DELAY;
	R_TFT_TS_MISC |= delay;		
}

void tft_reg_pol_set(INT8U pol)
{
	if (pol == TFT_REG_POL_1LINE) {
		R_TFT_TS_MISC &= ~TFT_REG_POL;	
	}
	else {
		R_TFT_TS_MISC |= TFT_REG_POL;	
	}
}

void tft_new_pol_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_TFT_LINE_RGB_ORDER |= TFT_NEW_POL_EN;	
	}
	else {
		R_TFT_LINE_RGB_ORDER &= ~TFT_NEW_POL_EN;	
	}
}

INT32S tft_led_backlight_level_set(INT8U level)
{
	INT32U value;
	
	if (level > BACKLIGHT_LEVEL_7) {
		level = BACKLIGHT_LEVEL_7;
	}
	
	switch(level) {
		case BACKLIGHT_LEVEL_0:
		case BACKLIGHT_LEVEL_1:
		case BACKLIGHT_LEVEL_2:
		case BACKLIGHT_LEVEL_3:
		case BACKLIGHT_LEVEL_4:
		case BACKLIGHT_LEVEL_5:
		case BACKLIGHT_LEVEL_6:
		case BACKLIGHT_LEVEL_7:
			break;
		default:
			return STATUS_FAIL;
	}
	
	value = R_PWM_CTRL; 
	value &= ~PWM_VSET;
	value |= (level << PWM_VSET_BIT);
	
	R_PWM_CTRL = value;
	return STATUS_OK;
}

INT32S tft_ccfl_backlight_level_set(INT8U level)
{
	return STATUS_OK;
}

INT32S tft_pwm0_en_set(BOOLEAN status)
{
	if (status == TRUE) { 
	    /* should be in PWM pad */
		R_PWM_CTRL |= 0x703; /* enable PWM0(for PAVDD,VGH and VGL) and CLK_6M */
	}
	else {
		gpio_init_io(IO_G10,1);
		gpio_set_port_attribute(IO_G10,1);
		gpio_write_io(IO_G10,0);
		R_PWM_CTRL &= ~0x702; /* pwm off and switch to gpio pad */
		if ((R_PWM_CTRL & 0x4) == 0) { /* check pwm1 whether on */
			R_PWM_CTRL &= ~0x1; /* turn off CLK_6M */
			R_PWM_CTRL &= ~0x3F;
		}	
	}

	return STATUS_OK;
}

INT32S tft_backlight_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		/* should be in PWM pad */
		R_PWM_CTRL |= 0x3805; /* enable PWM1(for TFT backlight) and CLK_6M */
	}
	else {
		gpio_init_io(IO_G13,1);
		gpio_set_port_attribute(IO_G13,1);
		gpio_write_io(IO_G13,0);
		R_PWM_CTRL &= ~0x3804;  /* pwm off and switch to gpio pad */
		if ((R_PWM_CTRL & 0x2) == 0) { /* check pwm0 whether on */
			R_PWM_CTRL &= ~0x1; /* turn off CLK_6M */
			R_PWM_CTRL &= ~0x3F;
		}	
	}
	
	return STATUS_OK;
}

void tft_i2c_write(INT32U adr, INT32U value)
{
#if 1
	/*use spi transmission */
	gpio_init_io(SPI_CS,GPIO_OUTPUT);
	gpio_set_port_attribute(SPI_CS, 1);
  	gpio_write_io(SPI_CS,1);
  	spi_clk_set(SPI_0,SYSCLK_8); /* 10MHz require in spec */
  	spi_cs_by_external_set(SPI_0);
#else /* use gpio pin */
  	gpio_init_io(CSN_n,GPIO_OUTPUT);
	gpio_set_port_attribute(CSN_n, 1);
	gpio_init_io(SCL_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SCL_n, 1);
	gpio_init_io(SDA_n,GPIO_OUTPUT);
	gpio_set_port_attribute(SDA_n, 1);
#endif
   tft_tpo_spi_write(adr,value);

}

void tft_display_buffer_set(INT32U value)
{
	if (value) {
		R_TFT_FBI_ADDR = value;
	}
}

static INT8U clip_enable;
static INT32U clip_vstart;
static INT32U clip_vend;
static INT32U clip_hstart;
static INT32U clip_hend;
static INT32U period_vstart;
static INT32U period_vend;
static INT32U period_hstart;
static INT32U period_hend;
void tft_display_range_save(void)
{
	if (R_TFT_TS_MISC & 0x200000) {
		clip_enable = 1;
	} else {
		clip_enable = 0;
	}
	clip_vstart = R_TFT_CLIP_V_START;
	clip_vend = R_TFT_CLIP_V_END;
	clip_hstart = R_TFT_CLIP_H_START;
	clip_hend = R_TFT_CLIP_H_END;

	period_vstart = R_TFT_V_START;		// V_Start
	period_vend = R_TFT_V_END;			// V_End
	period_hstart = R_TFT_H_START;		// H_Start
	period_hend = R_TFT_H_END;			// H_End
}

void tft_restricted_display(INT32U w, INT32U h)
{
	INT32U temp;

	if (clip_enable) {
		temp = clip_hend - clip_hstart + 1;
		if (temp < w) {
			return;
		}
		R_TFT_H_START = clip_hstart + (temp-w >> 1);
		R_TFT_H_END = R_TFT_H_START + w - 1;

		temp = clip_vend - clip_vstart;
		if (temp < h) {
			h = temp;
		}
		R_TFT_V_START = clip_vstart + (temp-h >> 1);
		R_TFT_V_END = R_TFT_V_START + h;
	} else {
		temp = period_hend - period_hstart + 1;
		if (temp < w) {
			return;
		}
		R_TFT_H_START = period_hstart + (temp-w >> 1);
		R_TFT_H_END = R_TFT_H_START + w - 1;

		temp = period_vend - period_vstart;
		if (temp < h) {
			h = temp;
		}
		R_TFT_V_START = period_vstart + (temp-h >> 1);
		R_TFT_V_END = R_TFT_V_START + h;

		R_TFT_TS_MISC |= 0x200000;
		R_TFT_CLIP_V_START = period_vstart;
		R_TFT_CLIP_V_END = period_vend;
		R_TFT_CLIP_H_START = period_hstart;
		R_TFT_CLIP_H_END = period_hend;
	}
}

void tft_display_range_restore(void)
{
	if (clip_enable) {
		R_TFT_TS_MISC |= 0x200000;
	} else {
		R_TFT_TS_MISC &= ~0x200000;
	}
	R_TFT_CLIP_V_START = clip_vstart;
	R_TFT_CLIP_V_END = clip_vend;
	R_TFT_CLIP_H_START = clip_hstart;
	R_TFT_CLIP_H_END = clip_hend;

	R_TFT_V_START = period_vstart;		// V_Start
	R_TFT_V_END = period_vend;			// V_End
	R_TFT_H_START = period_hstart;		// H_Start
	R_TFT_H_END = period_hend;			// H_End
}
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_TFT) && (_DRV_L1_TFT == 1)                   //
//================================================================//
