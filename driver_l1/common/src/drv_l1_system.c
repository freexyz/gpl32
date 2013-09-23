/*
* Purpose: System option setting
*
* Author: wschung, dominantyang
*
* Date: 2009/12/17
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :

     1 .Add system_bus_arbiter_init  2008/4/21 wschung
        setting all masters as 0
       .Add system_sdram_driving      2008/4/21 wschung
        setting SDRAM driving as 8 ma
     2 .Modify sdram config API 2009/09/02 Dominantyang
*/
#include "drv_l1_system.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_SYSTEM) && (_DRV_L1_SYSTEM == 1)             //
//================================================================//


#define INT_EXTAB             0x04000000
#define INT_RTC               0x80000000

/* use IRAM for storing memory setting, a */
#define R_ISRAM_CS0			        (*((volatile INT32U *) 0xF8000000))
#define R_ISRAM_CS1			        (*((volatile INT32U *) 0xF8000004))
#define R_ISRAM_CS2			        (*((volatile INT32U *) 0xF8000008))
#define R_ISRAM_CS3			        (*((volatile INT32U *) 0xF800000C))

INT32U  MCLK = INIT_MCLK;
INT32U  MHZ  = INIT_MHZ;

/* Prototypes */
void system_32768(INT32U Enable);
void system_half_no_reset(void);
void system_half_wakeup(void);
void system_set_pll(INT32U PLL_CLK);
void system_clks_enable0(INT32U CLK_MASK);
void system_clks_enable1(INT32U CLK_MASK);
void system_clks_disable0(INT32U CLK_MASK);
void system_clks_disable1(INT32U CLK_MASK);
void system_bus_arbiter_init(void);
void system_sdram_driving(void);
void system_enter_halt_mode(void);
void system_power_on_ctrl(void);
INT32U system_rtc_counter_get(void);
void system_rtc_counter_set(INT32U rtc_cnt);
INT32S sys_sdram_self_refresh_thread_cycle(INT8U sref_cycle);
INT32S sys_sdram_CAS_latency_cycle(SDRAM_CL cl_cycle);
INT32S sys_sdram_wakeup_cycle(INT8U RC_cycle);
INT32S sys_sdram_refresh_period_ns(INT32U refresh_period_ns);
INT32S sys_sdram_MHZ_set(SDRAM_CLK_MHZ sd_clk);
INT32S sys_sdram_read_latch_at_CLK_Neg(INT8U ebable_disable);
INT8U mcu_version_get(void);
void sys_sdram_input_clock_dly_enable(INT8U ebable_disable);
static tREFI_ENUM refresh_period=tREFI_15o6u;
INT32S sys_sdram_refresh_period_set(tREFI_ENUM tREFI);
INT32S sdram_calibrationed_flag(void);

// Provide your System driver and ISR here
INT32S sdram_calibrationed_flag(void)
{

#if 0
    INT32U check_sum;
    check_sum = (R_MEM_SDRAM_CTRL0<<16|R_MEM_SDRAM_CTRL1) +
               (R_MEM_SDRAM_TIMING<<16 | R_MEM_SDRAM_CBRCYC) + R_MEM_SDRAM_MISC;

    if (check_sum==0x8F9D3106) {
        return 0;
    }
    return 1;
#else
    if ((R_MEM_SDRAM_CTRL0==0x13)   &&
        (R_MEM_SDRAM_CTRL1==0x3000) &&
        (R_MEM_SDRAM_TIMING==0xF8A) &&
        (R_MEM_SDRAM_CBRCYC==0x100) &&
        (R_MEM_SDRAM_MISC==0x6)) {
        return 0;
    }
    return 1;
#endif
}

INT8U mcu_version_get(void)
{
	INT32U value;

	value = *((INT32U *) 0xF0001E00);
	if (value == 0x00000012) {			// GPL322xx Version A
		return 1;
	} else if (value == 0xE2811002) {	// GPL322xx Version B
		return 2;
	} else if (value == 0xE2811003) {	// GPL322xx Version C
		return 3;
	} else if (value == 0x00826959) { 	// GPL326xx Version A
		return 0x10;
	} else if (value == 0x68D13A20) {	// GPL326xx Version B
        return 0x11;  					
    } else if (value == 0x1C211C22) {	// GPL326xx Version C
        return 0x12;  					    
	} else if (value == 0xE55EC001) {	// GPL327xx Version A
        return 0x20;  					
	} else if (value == 0x31A01000) {	// GPL327xx Version B
        return 0x21;  					
	} else if (value == 0xDC68429A) {	// GPL326xxB Version A
        return 0x31;  					
	}

	return 255;
}

void system_32768(INT32U Enable)
{
 if (Enable) R_SYSTEM_CLK_CTRL =0x4400;
  while (R_SYSTEM_POWEP_STATE == 0); //waiting stable
}
void system_half_no_reset(void)
{
    INT32U data;
    //TFT Close

    //DAC/AD close

    /*SDRAM Latch*/
    R_MEM_SDRAM_MISC =0x4;
     /* Halt mode config */
    R_SYSTEM_CTRL |= 0x02;   //without reset
    R_SYSTEM_HALT      = 0x500A; //enter HALF mode
    data =(*((volatile INT32U *) 0xFF000000));//DUMMY OP
}

void system_half_wakeup(void)
{
   //waiting PLL stable

   //TFT initial and enable ?

   //DAC/AD initial and enable ?
}

void system_set_pll(INT32U PLL_CLK)
{
    sys_pll_set(PLL_CLK,0,0,0,0,R_MEM_DRV);
}

void system_clks_enable0(INT32U CLK_MASK)
{
 R_SYSTEM_CLK_EN0 |=CLK_MASK ;
}
void system_clks_enable1(INT32U CLK_MASK)
{
 R_SYSTEM_CLK_EN1 |=CLK_MASK ;
}

void system_clks_disable0(INT32U CLK_MASK)
{
 R_SYSTEM_CLK_EN0 &=~CLK_MASK ;
}

void system_clks_disable1(INT32U CLK_MASK)
{
 R_SYSTEM_CLK_EN1 &=~CLK_MASK ;
}


void system_bus_arbiter_init(void)
{
#if MCU_VERSION >= GPL326XX
  R_MEM_M2_BUS_PRIORITY =0;	/* usb20 */
  R_MEM_M3_BUS_PRIORITY =0;	/* ppu */
  R_MEM_M4_BUS_PRIORITY =0;	/* dma */
  R_MEM_M5_BUS_PRIORITY =0; /* jpeg */
  R_MEM_M6_BUS_PRIORITY =0;	/* scale */
  R_MEM_M7_BUS_PRIORITY =5; /* spu */
  R_MEM_M8_BUS_PRIORITY =0;	/* nfc */
  R_MEM_M9_BUS_PRIORITY =0;	/* cpu */
  R_MEM_M10_BUS_PRIORITY =0;/* mp3 */
  R_MEM_M11_BUS_PRIORITY =1;/* mp4 */
#else  // GPL325XX
  R_MEM_M2_BUS_PRIORITY =0;
  R_MEM_M3_BUS_PRIORITY =0;
  R_MEM_M4_BUS_PRIORITY =0;
  R_MEM_M5_BUS_PRIORITY =0;
  R_MEM_M6_BUS_PRIORITY =0;
  R_MEM_M7_BUS_PRIORITY =0;
  R_MEM_M8_BUS_PRIORITY =0;
  R_MEM_M9_BUS_PRIORITY =0;
  R_MEM_M10_BUS_PRIORITY =0;
  R_MEM_M11_BUS_PRIORITY =0;
#endif
}

void system_sdram_driving(void)
{
 //A version default drivering are 12 ma
 //Set all pin as 8ma
#if BOARD_TYPE < BOARD_TURNKEY_BASE
	R_MEM_DRV =0xAAAA;
#else
	R_MEM_DRV = 0; /* 4mA */
#endif
}

#if 0
void system_usbh_cheat(void)
{
    //R_SYSTEM_CLK_CTRL &= ~0x8;
}

void system_usbh_uncheat(void)
{
    R_SYSTEM_CLK_CTRL |= 0x8;
}
#endif

extern INT32U    day_count;
INT32U system_rtc_counter_get(void)
{
    return (INT32U) day_count;
}

void system_rtc_counter_set(INT32U rtc_cnt)
{
    day_count = rtc_cnt;
}

void system_da_ad_pll_en_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_SYSTEM_CLK_CTRL |= 0x10; /* DA/AD PLL Enable */
	}

	else {
		R_SYSTEM_CLK_CTRL &= ~0x10; /* DA/AD PLL FALSE */
	}
}

/* if the sref_cycle less, more easy(more slow) to entry self refresh latch mode */
INT32S sys_sdram_self_refresh_thread_cycle(INT8U sref_cycle)
{
    INT32U sdram_ctrl1_reg_val;
    sdram_ctrl1_reg_val = R_MEM_SDRAM_CTRL1;
    sdram_ctrl1_reg_val &= ~(0x00FF); /*clear [7:0]*/
    sdram_ctrl1_reg_val |= sref_cycle;
    if (sref_cycle!=0 && sref_cycle<64) {return STATUS_FAIL;}
    R_MEM_SDRAM_CTRL1 = sdram_ctrl1_reg_val;

    return STATUS_OK;
}

/* under 96MHz, SDRAM can support CL2 always. */
INT32S sys_sdram_CAS_latency_cycle(SDRAM_CL cl_cycle)
{
    INT32U sdram_ctrl1_reg_val;
    INT8U  cl_val;

    cl_val=(INT8U) cl_cycle;
    sdram_ctrl1_reg_val = R_MEM_SDRAM_CTRL1;
    sdram_ctrl1_reg_val &= ~(0x3000); /*clear [13:12]*/
    sdram_ctrl1_reg_val |= ((cl_val&0x3)<<12);
    if (cl_val<2) {return STATUS_FAIL;}

    R_MEM_SDRAM_CTRL1 = sdram_ctrl1_reg_val;
    return STATUS_OK;
}

/* when entry the latch mode, need RC cycle times to wake up */
INT32S sys_sdram_wakeup_cycle(INT8U RC_cycle)
{
    INT32U sdram_timing_reg_val;

    sdram_timing_reg_val = R_MEM_SDRAM_TIMING;
    RC_cycle &= 0xF;  /* fore bits effect */
    sdram_timing_reg_val &= ~(0xF);
    sdram_timing_reg_val |= RC_cycle;
    R_MEM_SDRAM_TIMING = sdram_timing_reg_val;

    return STATUS_OK;
}

/* large period time get more effect,default is 15600 ns (15.6us), don't less then it */
INT32S sys_sdram_refresh_period_ns(INT32U refresh_period_ns)
{
    INT32U cbrcyc_reg_val;

    //cbrcyc_reg_val = R_MEM_SDRAM_CBRCYC;
    
    if (refresh_period_ns < 15600) {return STATUS_FAIL;}

    cbrcyc_reg_val=(refresh_period_ns*MHZ/1000)&0xFFFF;
    R_MEM_SDRAM_CBRCYC = cbrcyc_reg_val;

    return STATUS_OK;
}


#ifndef __CS_COMPILER__
#pragma arm section rwdata="sdram_ctrl", code="sdram_ctrl"
#else
INT32S sys_sdram_init(SD_CLK_OUT_DELAY_CELL clk_out_dly_cell, SD_CLK_IN_DELAY_CELL clk_in_dly_cell,
                    SD_CLK_PHASE clk_phase, SD_BUS_WIDTH bus_mode, SD_BANK_BOUND bank_bound,
                    SD_SIZE sd_size, SD_SWITCH disable_enable) __attribute__ ((section(".sdram_ctrl")));
INT32S sys_sdram_read_latch_at_CLK_Neg(INT8U ebable_disable) __attribute__ ((section(".sdram_ctrl")));
#endif

INT32S sys_sdram_init(SD_CLK_OUT_DELAY_CELL clk_out_dly_cell, SD_CLK_IN_DELAY_CELL clk_in_dly_cell,
                    SD_CLK_PHASE clk_phase, SD_BUS_WIDTH bus_mode, SD_BANK_BOUND bank_bound, 
                    SD_SIZE sd_size, SD_SWITCH disable_enable)
{
    INT32U sdram_ctrl0_val;

    bank_bound = SD_BANK_1M;

    if(clk_in_dly_cell!=SDRAM_IN_DLY_DISABLE)
    {
        sys_sdram_input_clock_dly_enable(1);
    } else {
        sys_sdram_input_clock_dly_enable(0);
        clk_in_dly_cell=SDRAM_IN_DLY_LV0;
    }

    sdram_ctrl0_val = R_MEM_SDRAM_CTRL0;
    sdram_ctrl0_val &= ~(0xFFEF);  /* keep sdram enable and disable status */

    sdram_ctrl0_val |= (((clk_out_dly_cell & 0xF))<<12 | ((clk_phase & 1)<<11) | ((clk_in_dly_cell&0x7)<<8)| ((bank_bound&0x1)<<6) | ((bus_mode&0x1)<<5) |((disable_enable&0x1)<<4) | ((sd_size & 0x7)<<0));
    R_MEM_SDRAM_CTRL0 = sdram_ctrl0_val;

    return STATUS_OK;
}


INT32S sys_sdram_read_latch_at_CLK_Neg(INT8U ebable_disable)
{
    INT32U sdram_misc_val;

    sdram_misc_val = R_MEM_SDRAM_MISC;

    if (ebable_disable==0) {
        sdram_misc_val &= ~(0x2);
    } else {
        sdram_misc_val |= 0x2;
    }
    sdram_misc_val =0x6;
    R_MEM_SDRAM_MISC = sdram_misc_val;
    return STATUS_OK;
}

#ifndef __CS_COMPILER__
#pragma arm section rwdata, code
#endif

INT32S sys_sdram_enable(SD_SWITCH sd_disable_enable)
{
    INT32U sdram_ctrl0_val;
    sdram_ctrl0_val = R_MEM_SDRAM_CTRL0;
    sdram_ctrl0_val &= ~(0x0010); /* clear bit4 */
    sdram_ctrl0_val |= (sd_disable_enable&0x1)<<4;
    R_MEM_SDRAM_CTRL0 = sdram_ctrl0_val;

    return STATUS_OK;
}


INT32S sys_sdram_refresh_period_set(tREFI_ENUM tREFI) {
    refresh_period=tREFI;
    return STATUS_OK;
}

INT32S sys_sdram_MHZ_set(SDRAM_CLK_MHZ sdram_clk)
{

    INT32U cbrcyc_reg_val;
    INT32U oc=1; /* Internal test for overing clock */

    //cbrcyc_reg_val = R_MEM_SDRAM_CBRCYC;

    cbrcyc_reg_val=(refresh_period*sdram_clk/10)&0xFFFF * oc;

    R_MEM_SDRAM_CBRCYC = cbrcyc_reg_val;

    return STATUS_OK;
}

void sys_weak_6M_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_SYSTEM_CTRL |= 0x20; /* weak 6M mode */
	}
	else {
		R_SYSTEM_CTRL &= ~0x20; /* strong 6M mode */
	}
}

void sys_reg_sleep_mode_set(BOOLEAN status)
{
	if (status == TRUE) {
		R_SYSTEM_CTRL |= 0x80; /* enable reg sleep mode */
	}
	else {
		R_SYSTEM_CTRL &= ~0x80; /* disable reg sleep mode */
	}
}

void sys_sdram_auto_refresh_set(INT8U cycle)
{
	R_MEM_SDRAM_CTRL1 &= ~0xFF;
	R_MEM_SDRAM_CTRL1 |= cycle;
}

void sys_sdram_input_clock_dly_enable(INT8U ebable_disable)
{
    INT32U sdram_misc_reg_val;

    sdram_misc_reg_val = R_MEM_SDRAM_MISC;

    if (ebable_disable==0) {
        sdram_misc_reg_val &= ~(0x4);
    } else {
        sdram_misc_reg_val |= (0x4);
    }
    R_MEM_SDRAM_MISC = sdram_misc_reg_val;
}

#if MCU_VERSION < GPL326XX   // GPL325XX POWER ON Functions

    /* place to internal ram (432byte)*/    
    #ifndef __CS_COMPILER__
    #pragma arm section rwdata="pwr_ctrl", code="pwr_ctrl"
    #pragma O2
    void system_power_on_ctrl(void);
    void system_enter_halt_mode(void);
    void sys_ir_delay(INT32U t);
    #else
    void system_power_on_ctrl(void) __attribute__ ((section(".pwr_ctrl")));
    void system_enter_halt_mode(void) __attribute__ ((section(".pwr_ctrl")));
    void sys_ir_delay(INT32U t) __attribute__ ((section(".pwr_ctrl")));
    #endif    
    INT32U    day_count = 0xFFFFFFFF;//2454829; /* julian date count */
    INT32U    halt_data = 1;
    INT32U    ir_opcode = 1;
    INT32U    ir_cnt = 1;
    INT32U	  Alarm_Trriger_By_GPL32 = 1;	
    INT32U	  Day_Trriger_By_GPL32 = 1;	
    #if SUPPORT_IR_WAKEUP == 1
    INT32U    ir_data = 1;
    INT32U    irkey_pwr = 0x10;
    #endif
    
    void system_power_on_ctrl(void)
    {
    #if SUPPORT_GPY02XX == GPY02XX_NONE
    	/* wakeup by RTC_DAY int */
    	#if _DRV_L1_RTC == 1
    	if (R_RTC_INT_STATUS & RTC_DAY_IEN) {
    		R_RTC_INT_STATUS = 0x1f;
    		day_count++; /* add 1 to julian date conut */
    		if (ir_opcode != 0xFFFF0000) {
    			system_enter_halt_mode();
    		}
    	}
    	#endif
    #endif
    	
    	if (ir_opcode != 0xFFFF0000) {	
    		/* wakeup by extab */
    		if (R_INT_IRQFLAG & INT_EXTAB) {
    			R_TIMERB_CTRL = 0x8061;
    			R_INT_KECON |= 0x40; /* clear exta int */
    		 
    			/* default IOF5 is input with pull low */
    		  	sys_ir_delay(7519); /* 40 ms in 48MHz*/
    		  	for (ir_cnt=0;ir_cnt<22;ir_cnt++) {
    				if ((R_IOF_I_DATA & 0x20) == 0) {
    					system_enter_halt_mode();
    				}
    				sys_ir_delay(752); /* 4 ms in 48MHz*/
    			}
    		}
        }
    
    	R_SYSTEM_CTRL &= ~0x20; /* strong 6M mode */
    #if (BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_V4_4 || BOARD_TYPE == BOARD_TK_V4_1 || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_8D_V2_0)
    	R_IOH_O_DATA |= 0x20; /* set IOH5 to high for rom code */
    	sys_ir_delay(1879); /* 10 ms in 48MHz*/
    #endif
    }
    
    void sys_ir_delay(INT32U t)
    {
    	R_TIMERB_PRELOAD = (0x10000-t);
    
    	R_TIMERB_CTRL |= 0x2000;
    	while((R_TIMERB_CTRL & 0x8000) == 0);
      	R_TIMERB_CTRL &= ~0x2000;
    }
    
    void system_enter_halt_mode(void)
    {
    	R_SYSTEM_PLL_WAIT_CLK = 0x100; /* set pll wait clock to 8 ms when wakeup*/
    	R_SYSTEM_CTRL &= ~0x2; /* CPU reset when wakeup */
    
        R_SYSTEM_HALT = 0x500A;
        halt_data = R_CACHE_CTRL;
    
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
    }
    
    #if SUPPORT_IR_WAKEUP == 1
    void sys_ir_power_handler(void)
    {
    	/* 96MHz -> 2.66us */
    	/* 48MHz -> 5.33us */
    	/* 6MHz  -> 42.66us */
    	R_TIMERB_CTRL = 0x8061;
    	ir_opcode = 0xFFFF;
    
    	/* disable unuse peripheral clock to save power */
    #if SUPPORT_AD_KEY != KEY_AD_5_MODE
    #if BOARD_TYPE == BOARD_TK_A_V1_0 || BOARD_TYPE == BOARD_TK_A_V2_0 || BOARD_TYPE == BOARD_TK_A_V2_1_5KEY
    	R_SYSTEM_CLK_EN0 = 0x201F; /* keep TFT clk */
    #else
    	R_SYSTEM_CLK_EN0 = 0x001F;
    #endif
    #else
    #if BOARD_TYPE == BOARD_TK_A_V1_0 || BOARD_TYPE == BOARD_TK_A_V2_0 || BOARD_TYPE == BOARD_TK_A_V2_1_5KEY
    	R_SYSTEM_CLK_EN0 = 0x241F; /* keep TFT and ADC clk */
    #else
    	R_SYSTEM_CLK_EN0 = 0x041F;
    #endif
    	ir_cnt = 0;
    #endif
    	R_SYSTEM_CLK_EN1 = 0xC080;
    
    	/* ir receiver */
    	while(1) {
    #if SUPPORT_GPY02XX == GPY02XX_NONE
    		if (R_RTC_INT_STATUS & RTC_DAY_IEN) {
    			R_RTC_INT_STATUS = 0x1f;
    			day_count++; /* add 1 to julian date conut */
    			
    		#if (_ALARM_SETTING == CUSTOM_ON)&&(_POWERTIME_SETTING != CUSTOM_ON)
    			Day_Trriger_By_GPL32 = TRIGGER_DAY;
    			break;
    		#endif
    		
    		}
    #endif
    		if (ir_opcode == irkey_pwr) { /* IR power key */
    			break;
    		}
    #if SUPPORT_GPY02XX == GPY02XX_NONE
    		if (R_INT_KECON & 0x40) {
    			R_INT_KECON |= 0x40;
    			sys_ir_delay(234); /* 10ms */
    			for (ir_cnt=0;ir_cnt<22;ir_cnt++) {
    				if ((R_IOF_I_DATA & 0x20) == 0) {
    					break;
    				}
    				sys_ir_delay(23); /* 1 ms */
    			}
    			if (ir_cnt == 22) {
    				break;
    			}
    		}
    #if SUPPORT_EXT_ADP_DETECTION == CUSTOM_ON
    		if ((R_IOF_I_DATA & 0x4) == 0) {
    		//if ((R_IOH_I_DATA & 0x20) == 0) {
    			 ir_opcode = 0xF0000000;/* adpater plug out */
    			 break;	
    		}
    #endif
    
    #if (_ALARM_SETTING == CUSTOM_ON)||(_POWERTIME_SETTING == CUSTOM_ON)
     		if (R_RTC_INT_STATUS & RTC_ALM_IEN) {
    			R_RTC_INT_STATUS = RTC_ALM_IEN;
    			Alarm_Trriger_By_GPL32 = TRIGGER_ALARM;
    			break;
    		}
    #endif
    
    #else
    #if SUPPORT_AD_KEY != KEY_AD_5_MODE
    		if (R_INT_KECON & 0x40) {
    			R_INT_KECON |= 0x40;
    			break;
    		}
    #else
    		if (R_INT_KECON & 0x40) {
    			R_INT_KECON |= 0x40;
    			break;
    		}
    		
    		sys_ir_delay(4);
    		R_ADC_MADC_CTRL |= 0x40;
    		while(!(R_ADC_MADC_CTRL & 0x8000));
    		R_ADC_MADC_CTRL |= 0x8000;
    		
    		if ((R_ADC_MADC_DATA > 0xB240) && (R_ADC_MADC_DATA < 0xE880)) {
    			ir_cnt++;
    		}
    		else {
    			ir_cnt = 0;
    		}
    		if (ir_cnt > 500) {
    			break;
    		}
    #endif
    #endif
    
    		if (R_IOC_I_DATA & 0x400) { /* wait low */
    			continue;
    		}
    		for (ir_cnt=0;ir_cnt<10;ir_cnt++) {
    			sys_ir_delay(20); /* 882us */
    			if (R_IOC_I_DATA & 0x400) { /* if hi */
    				break;
    			}
    		}
    		if (ir_cnt != 10) {
    			continue;
    		}
    
    		while((R_IOC_I_DATA & 0x400) == 0); /* wait until hi */
    		sys_ir_delay(57); /* 2.45 ms */
    		if ((R_IOC_I_DATA & 0x400) == 0) {
    			continue; /* repeat command */
    		}
    		sys_ir_delay(54); /* 2.3 ms */
    		ir_data = 0;
    
    		for (ir_cnt=0;ir_cnt<32;ir_cnt++) {
    			while((R_IOC_I_DATA & 0x400) == 0);
    			sys_ir_delay(20); /* 882us */
    			if (R_IOC_I_DATA & 0x400) {
    				ir_data |= (1<<ir_cnt);
    				sys_ir_delay(23); /* 1000us */
    			}
    		}
    
    		ir_opcode = (ir_data & 0xFF0000) >> 16;
    	}
    
    	R_SYSTEM_CLK_EN0 = 0xFFFF; /* enable all clock */
    	R_SYSTEM_CLK_EN1 = 0xFFFF;
    	R_MEM_SDRAM_CTRL1 &= ~0xFF; /* cancel auto enter self refresh mode */
    
    	/* for MCU version C(the rom code of version C doesn't set sys clk */
    #if MCU_VERSION == GPL32_C
    	R_SYSTEM_PLLEN = 0x10;   /* sys clk = 48MHz */
    	R_SYSTEM_CLK_CTRL = 0x8400;
    #endif
    
    #if BOARD_TYPE == BOARD_TK_A_V1_0 || BOARD_TYPE == BOARD_TK_A_V2_0 || BOARD_TYPE == BOARD_TK_A_V2_1_5KEY
    	R_TFT_CTRL = 0;
    #endif
    
    	if (ir_opcode != 0xF0000000) {
    		ir_opcode = 0xFFFF0000;
    		R_RTC_INT_CTRL |= 0x1;
    	}
    	system_enter_halt_mode();
    }
    #endif
    #ifndef __CS_COMPILER__
    #pragma arm section rwdata, code
    #endif


#else  // GPL326XX System Power Functions

    /* place to internal ram (432byte)*/
    
    #ifndef __CS_COMPILER__
    #pragma arm section rwdata="pwr_ctrl", code="pwr_ctrl"
    #pragma O2
    void system_power_on_ctrl(void);
    void system_enter_halt_mode(void);
    void sys_ir_delay(INT32U t);
    #else
    void system_power_on_ctrl(void) __attribute__ ((section(".pwr_ctrl")));
    void system_enter_halt_mode(void) __attribute__ ((section(".pwr_ctrl")));
    void sys_ir_delay(INT32U t) __attribute__ ((section(".pwr_ctrl")));
    #endif
    INT32U    day_count = 0xFFFFFFFF;//2454829; /* julian date count */
    INT32U    halt_data = 1;
    INT32U    ir_opcode = 1;
    INT32U    ir_cnt = 1;
    INT32U	  Alarm_Trriger_By_Gp6 = 1;
    INT32U	  Day_Trriger_By_Gp6 = 1;
    #if SUPPORT_IR_WAKEUP == 1
    INT32U    ir_data = 1;
    INT32U    irkey_pwr = 0x10;
    #endif
    
    void system_power_on_ctrl(void)
    {
    #if MCU_VERSION < GPL326XX
    	/* wakeup by RTC_DAY int */
    	#if _DRV_L1_RTC == 1
    	if (R_RTC_INT_STATUS & RTC_DAY_IEN) {
    		R_RTC_INT_STATUS = 0x1f;
    		day_count++; /* add 1 to julian date conut */
    		if (ir_opcode != 0xFFFF0000) {
    			system_enter_halt_mode();
    		}
    	}
    	#endif
    #endif
    
    #if _PROJ_TYPE == _PROJ_TURNKEY
    	R_TIMERB_CTRL = 0x8061;
      #if SUPPORT_HARDWARE_IR == CUSTOM_ON
        if (R_IR_RX_CTRL & 0x20) {
        	R_IR_RX_CTRL |= 0x20;		//clear PWR Interrupt
        }
      #endif
      #if SUPPORT_HARDWARE_IR != CUSTOM_ON
    	if (ir_opcode != 0xFFFF0000) {
      #endif
    		/* wakeup by extab */
    		if (R_INT_IRQFLAG & INT_EXTAB) {
    			//R_TIMERB_CTRL = 0x8061;
    			R_INT_KECON |= 0x40; /* clear exta int */
    
    			/* default IOF5 is input with pull low */
    		  	sys_ir_delay(7519); /* 40 ms in 48MHz*/
    		  	for (ir_cnt=0;ir_cnt<22;ir_cnt++) {
    				if ((R_IOG_I_DATA & 0x200) == 0) {
    					system_enter_halt_mode();
    				}
    				sys_ir_delay(752); /* 4 ms in 48MHz*/
    			}
    		}
      #if SUPPORT_HARDWARE_IR != CUSTOM_ON
        }
      #endif
    
      #if SUPPORT_HARDWARE_IR == CUSTOM_ON
    
    	R_MEM_SDRAM_CTRL0 = 0x13;
    	R_MEM_SDRAM_CTRL1 = 0x3000;
    	R_MEM_SDRAM_TIMING = 0xF8A;
    	R_MEM_SDRAM_CBRCYC = 0x100;
    	R_MEM_SDRAM_MISC = 0x6;
    
    	R_MEM_DRV = 0xFFFF;
      #endif
    #endif
    	R_SYSTEM_CTRL &= ~0x20; /* strong 6M mode */
    	*P_USBD_CONFIG &= ~0x800;	//Switch to full speed
    	*P_USBD_CONFIG1 &= ~0x100; /* enable usb phy */
    }
    
    void sys_ir_delay(INT32U t)
    {
    	R_TIMERB_PRELOAD = (0x10000-t);
    
    	R_TIMERB_CTRL |= 0x2000;
    	while((R_TIMERB_CTRL & 0x8000) == 0);
      	R_TIMERB_CTRL &= ~0x2000;
    }
    
    void system_enter_halt_mode(void)
    {
    	R_SYSTEM_PLL_WAIT_CLK = 0x100; /* set pll wait clock to 8 ms when wakeup*/
    	R_SYSTEM_CTRL &= ~0x2; /* CPU reset when wakeup */
    
        R_SYSTEM_HALT = 0x500A;
        halt_data = R_CACHE_CTRL;
    
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
        ASM(NOP);
    }
    
    #if SUPPORT_IR_WAKEUP == 1
    void sys_ir_power_handler(void)
    {
    	/* 96MHz -> 2.66us */
    	/* 48MHz -> 5.33us */
    	/* 6MHz  -> 42.66us */
    	*P_USBD_CONFIG |= 0x800;	//Switch to USB20PHY
    	*P_USBD_CONFIG1 |= 0x100;//[8],SW Suspend For PHY
      #if SUPPORT_HARDWARE_IR == CUSTOM_ON
    	R_IR_RX_CTRL = 0x01;
    	R_IR_RX_CTRL |= 0x90;
    	R_IR_RX_PWR = 0xef10ff00;
      #endif
    
      #if SUPPORT_HARDWARE_IR != CUSTOM_ON
    	R_TIMERB_CTRL = 0x8061;
    	ir_opcode = 0xFFFF;
    
    	/* disable unuse peripheral clock to save power */
    #if SUPPORT_AD_KEY != KEY_AD_5_MODE
    	R_SYSTEM_CLK_EN0 = 0x001F;
    #else
    	R_SYSTEM_CLK_EN0 = 0x041F;
    	ir_cnt = 0;
    #endif
    	R_SYSTEM_CLK_EN1 = 0xC080;
    
    	/* ir receiver */
    	while(1) {
    		if (R_RTC_INT_STATUS & RTC_DAY_IEN) {
    			R_RTC_INT_STATUS = 0x1f;
    			day_count++; /* add 1 to julian date conut */
    
    		#if (_ALARM_SETTING == CUSTOM_ON)&&(_POWERTIME_SETTING != CUSTOM_ON)
    			Day_Trriger_By_Gp6 = TRIGGER_DAY;
    			break;
    		#endif
    
    		}
    
    		if (ir_opcode == irkey_pwr) { /* IR power key */
    			break;
    		}
    
    		if (R_INT_KECON & 0x40) {
    			R_INT_KECON |= 0x40;
    			sys_ir_delay(234); /* 10ms */
    			for (ir_cnt=0;ir_cnt<22;ir_cnt++) {
    				if ((R_IOG_I_DATA & 0x200) == 0) {
    					break;
    				}
    				sys_ir_delay(23); /* 1 ms */
    			}
    			if (ir_cnt == 22) {
    				break;
    			}
    		}
    #if SUPPORT_EXT_ADP_DETECTION == CUSTOM_ON
    		if ((R_IOF_I_DATA & 0x4) == 0) {
    		//if ((R_IOH_I_DATA & 0x20) == 0) {
    			 ir_opcode = 0xF0000000;/* adpater plug out */
    			 break;
    		}
    #endif
    
    #if (_ALARM_SETTING == CUSTOM_ON)||(_POWERTIME_SETTING == CUSTOM_ON)
     		if (R_RTC_INT_STATUS & RTC_ALM_IEN) {
    			R_RTC_INT_STATUS = RTC_ALM_IEN;
    			Alarm_Trriger_By_Gp6 = TRIGGER_ALARM;
    			break;
    		}
    #endif
    
    		if (R_IOC_I_DATA & 0x400) { /* wait low */
    			continue;
    		}
    		for (ir_cnt=0;ir_cnt<10;ir_cnt++) {
    			sys_ir_delay(20); /* 882us */
    			if (R_IOC_I_DATA & 0x400) { /* if hi */
    				break;
    			}
    		}
    		if (ir_cnt != 10) {
    			continue;
    		}
    
    		while((R_IOC_I_DATA & 0x400) == 0); /* wait until hi */
    		sys_ir_delay(57); /* 2.45 ms */
    		if ((R_IOC_I_DATA & 0x400) == 0) {
    			continue; /* repeat command */
    		}
    		sys_ir_delay(54); /* 2.3 ms */
    		ir_data = 0;
    
    		for (ir_cnt=0;ir_cnt<32;ir_cnt++) {
    			while((R_IOC_I_DATA & 0x400) == 0);
    			sys_ir_delay(20); /* 882us */
    			if (R_IOC_I_DATA & 0x400) {
    				ir_data |= (1<<ir_cnt);
    				sys_ir_delay(23); /* 1000us */
    			}
    		}
    
    		ir_opcode = (ir_data & 0xFF0000) >> 16;
    	}
    
    	R_SYSTEM_CLK_EN0 = 0xFFFF; /* enable all clock */
    	R_SYSTEM_CLK_EN1 = 0xFFFF;
    	R_MEM_SDRAM_CTRL1 &= ~0xFF; /* cancel auto enter self refresh mode */
    
    	/* for MCU version C(the rom code of version C doesn't set sys clk */
    	R_SYSTEM_PLLEN = 0x10;   /* sys clk = 48MHz */
    	R_SYSTEM_CLK_CTRL = 0x8400;
    
    	if (ir_opcode != 0xF0000000) {
    		ir_opcode = 0xFFFF0000;
    		R_RTC_INT_CTRL |= 0x1;
    		R_FUNPOS2 |= 0x21780000;
    	}
      #endif
    	system_enter_halt_mode();
    
    }
    #endif
    
    #ifndef __CS_COMPILER__
    #pragma arm section rwdata, code
    #endif

#endif    // End POWER ON Functions 

#if SUPPORT_IR_WAKEUP == 1
void sys_ir_power_set_pwrkey(INT8U code)
{
	irkey_pwr = code;
}


INT32U sys_ir_power_get_pwrkey(void)
{
	return irkey_pwr;
}
#endif
void sys_ir_opcode_clear(void)
{
	ir_opcode = 0;	
}

INT32U sys_ir_opcode_get(void)
{
	return ir_opcode;	
}

#pragma O0
void system_enter_wait_mode(void)
{
	R_SYSTEM_WAIT = 0x5005;
    halt_data = R_CACHE_CTRL;

    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
    ASM(NOP);
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_SYSTEM) && (_DRV_L1_SYSTEM == 1)        //
//================================================================//
