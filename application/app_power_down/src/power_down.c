#include "power_down.h"

#define	HALT_MODE_RESET_VECTOR		(*((volatile INT32U *) 0xC0000178))
#define	R_SPU_CH_EN					(*((volatile INT32U *) 0xD0400E00))
#define	R_SPU_CH_EN_HIGH			(*((volatile INT32U *) 0xD0400E80))
#define	R_SPU_CH_STATUS				(*((volatile INT32U *) 0xD0400E3C))
#define	R_SPU_CH_STATUS_HIGH		(*((volatile INT32U *) 0xD0400EBC))
#define	R_SPU_CH_RAMPDOWN			(*((volatile INT32U *) 0xD0400E28))
#define	R_SPU_CH_RAMPDOWN_HIGH		(*((volatile INT32U *) 0xD0400EA8))

void SetupGPIO(void)
{
	//IOA0~7 : TFT DATA
	//IOA8~15: CMOS DATA

	//DATA0~DATA15 out put low
	R_IOA_DIR |=  0xFFFF;
	R_IOA_ATT |=  0xFFFF;
  	R_IOA_O_DATA &=~0xFFFF;

  	//IOB0 : TFT DE
  	//IOB1 : TFT HS
  	//IOB2 : TFT VS
  	//IOB3 : TFT CLK
  	//IOB5 : CSI HS
  	//IOB6 : CSI VS

  	//Control Pin of TFT IOB0~4 PULL LOW
  	R_IOB_DIR |=  0x003F;
 	R_IOB_ATT |=  0x003F;
  	R_IOB_O_DATA &=~0x003F;

  	//IOC1 : SPI CLK
    //IOC2 : SPI TX
    //IOC3 : SPI RX
    //IOC4 : SDC_CMD
    //IOC5 : SDC_CLK
    //IOC6 : SDC_DATA3
    //IOC7 : SDC_DATA0
    //IOC8 : SDC_DATA1
    //IOC9 : SDC_DATA2
    //IOC10: CSI_CLKI 		(EXTB)
    //IOC11: CSI_CLKO
    R_IOC_DIR |=  0x080E;
 	R_IOC_ATT |=  0x080E;
  	R_IOC_O_DATA &=~0x080E;

    //IOF1 : BKCSB1		(NAND OR SPI_FLASH_CS)
    //IOF2 : BKCSB2 	(SDC)
    //IOF3 : BKCSB3 	(SDC)
    //IOF4 : SDRAM_CSB 	(SDRAM USER)
    //IOF5 : EXT0
    //IOF6 : ADC0
    //IOF7 : ADC1
    //IOF8 : ADC2
    //IOF9 : ADC3
    //IOF10: AOUT 		(TV OUT)
    //IOF11: RSET 		(TV OUT)
    //IOF12: VREF		(TV OUT)
    //IOF14: BM1

    //DATA0~DATA15 out put low
    R_IOF_DIR |=  0x5FEC;
	R_IOF_ATT |=  0x5FEC;
  	R_IOF_O_DATA &=~0x5FEC;

    //IOG5 : MEM_A20		(NF_ALE)
    //IOG6 : MEM_A19		(NF_CLE)
    //IOG7 : MEM_A14
    //IOG8 : MEM_A13
    //IOG9 : MEM_A12
    //IOG10: MEM_A22 		(NF_REB)
    //IOG11: MEM_A21		(NF_WEB) (PWM0)
    //IOG13: PWM1
    //IOG14: FB1
    //IOG15: VC1

    //DATA0~DATA15 out put low
  	R_IOG_DIR |=  0xE000;
	R_IOG_ATT |=  0xE000;
  	R_IOG_O_DATA &=~0xE000;

  	//IOH0 : MEM_OE_B
  	//IOH1 : SDRAM_CKE
    //IOH2 : JTAG_TDI
    //IOH3 : JTAG_TD0
    //IOH4 : JTAG_TCK
    //IOH5 : JTAG_TMS

    //JTAG pin used as GPIOs
    R_FUNPOS1 &=~ BIT0;
	R_IOH_DIR &=~ (0x0d<<2);
  	R_IOH_ATT |= (0x0d<<2);
  	R_IOH_O_DATA |= (0x0d<<2);
}

void NandSleepSetting(void)
{
#if NAND_CTRL_POS != NF_ALE_SHARE_CLE_SHARE_REB_SHARE__WEB_SHARE
	int mask = 0;
#endif
 //SLEEP之前設置Nand RDY為輸入floating(高阻)
 R_IOC_DIR &= ~BIT11;
 R_IOC_ATT |= BIT11;
 R_IOC_O_DATA |= BIT11;

 #if NAND_CS_POS == NF_CS_AS_BKCS1
     R_MEM_IO_CTRL &=~ (1<<12);
     R_IOF_DIR |=  BIT1;
     R_IOF_ATT |=  BIT1;
     R_IOF_O_DATA |= BIT1;
 #elif NAND_CS_POS == NF_CS_AS_BKCS2
     R_MEM_IO_CTRL &=~ (1<<13);
     R_IOF_DIR |=  BIT2;
     R_IOF_ATT |=  BIT2;
     R_IOF_O_DATA |= BIT2;
 #elif NAND_CS_POS == NF_CS_AS_BKCS3
     R_MEM_IO_CTRL &=~ (1<<14);
     R_IOF_DIR |=  BIT3;
     R_IOF_ATT |=  BIT3;
     R_IOF_O_DATA |= BIT3;
 #endif

 (*((volatile INT32U *) 0xD0900140)) &= ~BIT12; //Disable the NAND Flash Controller
 R_SYSTEM_CLK_EN1 &=~ (1<<13);					//disable nand  control clock

 #if NAND_SHARE_MODE == NF_SHARE_MODE
     #if NAND_CTRL_POS == NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11
       R_MEM_IO_CTRL &=~ ((1<<1)|(1<<2)|(1<<3)|(1<<4));
       mask = BIT5 | BIT6;
       R_IOG_DIR |= mask;
       R_IOG_ATT |= mask;
       R_IOG_O_DATA |= mask;
       mask = BIT10 | BIT11;
       R_IOG_DIR |= mask;
       R_IOG_ATT |= mask;
       R_IOG_O_DATA &= ~mask;
     #endif
 #else
     #if NAND_CTRL_POS == NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
        mask = BIT12 | BIT13;
        R_IOC_DIR |= mask;
        R_IOC_ATT |= mask;
        R_IOC_O_DATA |= mask;
        mask = BIT14 | BIT15;
        R_IOC_DIR |= mask;
        R_IOC_ATT |= mask;
        R_IOC_O_DATA &= ~mask;
        #if NAND_DATA5_0_POS == NAND_DATA5_0_AS_IOD5_0
           mask = 0x00FF;//[BIT0-BIT7]
           R_IOD_DIR |= mask;
           R_IOD_ATT |= mask;
           R_IOD_O_DATA &= ~mask;
        #endif
        #if NAND_DATA5_0_POS == NAND_DATA5_0_AS_IOB13_8
           mask = 0xFF00;//[BIT8-BIT15]
           R_IOB_DIR |= mask;
           R_IOB_ATT |= mask;
           R_IOB_O_DATA &= ~mask;
        #endif

     #endif

     #if NAND_CTRL_POS == NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9
        mask = BIT6 | BIT7;
        R_IOC_DIR |= mask;
        R_IOC_ATT |= mask;
        R_IOC_O_DATA |= mask;
        mask = BIT8 | BIT9;
        R_IOC_DIR |= mask;
        R_IOC_ATT |= mask;
        R_IOC_O_DATA &= ~mask;
        #if NAND_DATA5_0_POS == NAND_DATA5_0_AS_IOA13_8
           mask = 0xFF00;//[BIT8-BIT15]
           R_IOA_DIR |= mask;
           R_IOA_ATT |= mask;
           R_IOA_O_DATA &= ~mask;
        #endif
     #endif

 #endif
}

#ifndef __CS_COMPILER__
	#pragma arm section rwdata="pwr_ctrl", code="pwr_ctrl"
	#pragma O2
	void DisableSDRAMAndGoToSleep(void);
	void system_power1_on_ctrl(void);
#else
	void DisableSDRAMAndGoToSleep(void) __attribute__ ((section(".pwr_ctrl")));
	void system_power1_on_ctrl(void) __attribute__ ((section(".pwr_ctrl")));
#endif

void DisableSDRAMAndGoToSleep(void)
{
	INT32S i;

	//disable the various system clocks. This does not appear to affect the power draw during Sleep.
	R_SYSTEM_CLK_EN0 = 0x000F;		//enable system bus, memory, GPIO, interrup  clock
	R_SYSTEM_CLK_EN1 = 0xE080;		//enable internal sram, nand flash, system control register, system control clock

	// SLEEP之前設置 SDRAM CS(IOF4),為GPIO 並設置其為輸出低
	R_MEM_IO_CTRL &= ~0x0400;		//release SDRAM CS to GPIO
    R_IOF_ATT |= 0x0010;
    R_IOF_DIR |= 0x0010;
    R_IOF_O_DATA &= ~0x0010;

    //SLEEP之前設置SDRAM WEB為GPIO 並且設置其為輸出低
    (*((volatile INT32U *) 0xD02000A4)) |= 0x0001;

    //disable SDRAM power source
    //R_IOA_O_DATA |= GPIO_3V3_SDRAM_CTRL;	//Control SDRAM VCC

#if (EXTA_WAKEUP == 1)
	//make sure the button is really up, IOF5
	while(R_IOF_I_DATA & BIT5) {
		R_INT_KECON |= BIT6;		//clear EXTA int flag
	}
#endif

#if (EXTB_WAKEUP == 1)
	//wait for the user to release the power button.
	while(R_IOC_I_DATA & BIT10) {
		R_INT_KECON |= BIT7;		//clear EXTB int flag
	}
#endif

	//1. Write control register (entering Wait/Halt/Sleep mode)
	R_SYSTEM_SLEEP = 0xA00A;

	//2. Read one of Cache Controller registers
	i = R_CACHE_CTRL;

	//3. 16 nops
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

void system_power1_on_ctrl(void)
{
	R_IOA_DIR =  0x00FF;
	R_IOA_ATT =  0x00FF;
  	R_IOA_O_DATA =0x0008;

  	/* strong 6M mode */
	R_SYSTEM_CTRL &= ~0x20;
}
#ifndef __CS_COMPILER__
	#pragma arm section rwdata, code
#endif

/********************************************************
// mode
// 0: sleep mode
// 1: halt next instruction after weekup
// 4: halt reset after weekup
// other: wait mode
*********************************************************/
void PowerDown_Mode(INT8U mode)
{
	volatile INT32S i;
	INT32U tmp;

	DBG_PRINT("%s = %d\r\n", __func__, mode);
	//========== Disable IRQ and FIQ
	R_INT_GMASK = 0x00000001;

	//========== WAKE UP KEY IOF5
#if (EXTA_WAKEUP == 1)
	extab_int_clr(EXTA);  		//if EXTB happen ,clear the interrupt flag
	extab_edge_set(EXTA, 1); 	//rising edge
	extab_enable_set(EXTA,TRUE);//re-enable interrupt
#endif
	//========== WAKE UP KEY IOC10
#if (EXTB_WAKEUP == 1)
	extab_int_clr(EXTB);  		//if EXTB happen ,clear the interrupt flag
	extab_edge_set(EXTB, 1); 	//rising edge
	extab_enable_set(EXTB,TRUE);//re-enable interrupt
#endif
	tmp = R_KEYCH;

	//========== NAND SLEEP
	DBG_PRINT("%d\r\n", __LINE__);
	if(mode == 0) {
		NandSleepSetting();
    }

	//=========== PPU
	DBG_PRINT("%d\r\n", __LINE__);
	if(R_PPU_ENABLE & BIT0) {	//judge if enable PPU
		if((R_PPU_IRQ_STATUS & BIT0) == 0) {
			R_PPU_FB_GO = 1;
		}

		while((R_PPU_IRQ_STATUS & BIT0) == 0);
		while((R_DEFLICKER_CTRL & BIT8) == BIT8);
	}
	R_PPU_ENABLE &= ~BIT0;    //disable PPU

	//========== CMOS
	DBG_PRINT("%d\r\n", __LINE__);
	R_CSI_TG_CTRL1 = 0;
	R_CSI_TG_CTRL0 = 0;

	//========= SD CAR
	DBG_PRINT("%d\r\n", __LINE__);
	drvl2_sd_card_remove();

	R_IOC_DIR &=~  0x03F0;
 	R_IOC_ATT |=  0x03F0;
  	R_IOC_O_DATA |= 0x03F0;

    //======== SCALER
    DBG_PRINT("%d\r\n", __LINE__);
    scaler_stop();

    //======== JPEG
    DBG_PRINT("%d\r\n", __LINE__);
	jpeg_init();

	//======== TV
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	DBG_PRINT("%d\r\n", __LINE__);
	if(R_TV_CTRL & BIT0) {
		while((R_PPU_IRQ_STATUS & BIT11) == 0);
	}

	R_TV_CTRL &= ~BIT0;		//disable TV module
	R_TV_VIDEODAC = 0;
#endif

    //====== DISABLE DAC
    DBG_PRINT("%d\r\n", __LINE__);
    *P_DAC_CHA_DATA = 0;
    *P_DAC_CHB_DATA = 0;
	R_DAC_PGA &= ~0x003F;
	R_DAC_CHA_CTRL &= ~0x3000;
	R_DAC_CHB_CTRL &= ~0x2000;

	// Delay for 300ms
	for(i=0;i<10*1000000;i++);
	R_ANALOG_CTRL &= ~0x01;

	//============ DISABLE USB
	DBG_PRINT("%d\r\n", __LINE__);
	usb_uninitial();
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
	*P_USBD_CONFIG |= 0x800;        //Switch to USB20PHY
	*P_USBD_CONFIG1 |= 0x100;		//[8],SW Suspend For PHY
#endif

	//======== DISABLE SPI
	spi_disable(SPI_0);
    spi_disable(SPI_1);

	//======= DISABLE ADC
	R_ADC_SETUP = 0x0000;

	//======= DISABLE MIC
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
	R_MIC_SETUP = 0x0000;
#endif

	//======= DISABLE RTC
	DBG_PRINT("%d\r\n", __LINE__);
#if (RTC_WAKEUP == 0)
	R_RTC_CTRL = 0;
	R_RTC_INT_CTRL = 0x0000;
	R_RTC_INT_STATUS = 0x51F;
#else
#if 1 // rtc MINIF wake up
	R_RTC_CTRL &= ~(0x07 << 8);
	R_RTC_CTRL |= 0x8200;		//enable DHMSEN
	R_RTC_INT_STATUS |= 0x51F;
	R_RTC_INT_CTRL = (1 << 2);	//wake up source MINIF
#elif 0	//rtc ALMEN wake up
	{
		t_rtc time;

		R_RTC_CTRL &= ~(0x07 << 8);
		R_RTC_CTRL |= 0x8400;		//enable ALMEN
		R_RTC_INT_STATUS |= 0x51F;

		time.rtc_sec = R_RTC_SEC;
		time.rtc_min = R_RTC_MIN;
		time.rtc_hour = R_RTC_HOUR;

		time.rtc_sec += 10;
		if(time.rtc_sec >= 60) {
			time.rtc_min++;
			time.rtc_sec = 0;
		}

		if(time.rtc_min >= 60) {
			time.rtc_hour++;
			time.rtc_min = 0;
		}

		R_RTC_ALARM_SEC = time.rtc_sec;
		R_RTC_ALARM_MIN = time.rtc_min;
		R_RTC_ALARM_HOUR = time.rtc_hour;

		rtc_int_clear(RTC_ALM_IEN);
		rtc_int_set(RTC_ALM_IEN, RTC_ALM_IEN);
	}
#else //idp rtc ALM pin wake up
	{
		t_rtc time;

		idp_rtc_alm_pin_wakeup_set(1);
		rtc_time_get(&time);

		time.rtc_sec += 10;
		if(time.rtc_sec >= 60) {
			time.rtc_min++;
			time.rtc_sec = 0;
		}

		if(time.rtc_min >= 60) {
			time.rtc_hour++;
			time.rtc_min = 0;
		}
		rtc_alarm_set(&time);
		idp_rtc_int_clear(GPX_RTC_ALM_IEN);
		idp_rtc_int_set(GPX_RTC_ALM_IEN, GPX_RTC_ALM_IEN);
	}
#endif
#endif	//RTC_WAKEUP

	//===== DISABLE TIME AND TIME BASE
	R_TIMERA_CTRL = 0;
	R_TIMERB_CTRL = 0;
	R_TIMERC_CTRL = 0;
	R_TIMERD_CTRL = 0;
	R_TIMERE_CTRL = 0;
	R_TIMERF_CTRL = 0;

	R_TIMEBASEA_CTRL = 0;
	R_TIMEBASEB_CTRL = 0;
	R_TIMEBASEC_CTRL = 0;

	//======= DISABLE UART
	R_UART_CTRL = 0;

	//======= DISABLE SPU
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	if(R_SPU_CH_STATUS != 0) {
		R_SPU_CH_RAMPDOWN = 0xFFFF;
		while(R_SPU_CH_STATUS != 0);
	}

	if(R_SPU_CH_STATUS_HIGH != 0) {
		R_SPU_CH_RAMPDOWN_HIGH = 0xFFFF;
		while(R_SPU_CH_STATUS_HIGH != 0);
	}

	SPU_Clear_SRAM();
	SPU_Clear_Register();

	*P_SPU_CH_EN = 0;
	*P_SPU_CH_EN_HIGH = 0;
	*P_SPU_CONTROL_FLAG &= ~BIT11;
#endif

	//====== DISABLE DMA
	while((R_DMA0_CTRL&BIT1) == BIT1);	//wait untill DMA0 is idle
	R_DMA0_CTRL &= ~BIT0;				//disable DMA0

	while((R_DMA1_CTRL&BIT1) == BIT1);	//wait untill DMA1 is idle
	R_DMA1_CTRL &= ~BIT0;				//disable DMA1

	while((R_DMA2_CTRL&BIT1) == BIT1);	//wait untill DMA2 is idle
	R_DMA2_CTRL &= ~BIT0;				//disable DMA2

	while((R_DMA3_CTRL&BIT1) == BIT1);	//wait untill DMA3 is idle
	R_DMA3_CTRL &= ~BIT0;				//disable DMA3

	R_DMA0_CTRL = 0;
	R_DMA1_CTRL = 0;
	R_DMA2_CTRL = 0;
	R_DMA3_CTRL = 0;

	//======== DISABLE ALL PORT
	SetupGPIO();

	//======= SYSTEM REGULATOR SET
	R_SYSTEM_CTRL |= 0x80;				//the regulator will enter the sleep mode if system is in halt or standby mode

	//======= SYSTEM CLK
	R_SYSTEM_CTRL |= 0x20;				//Set the 6M to weak mode(strong mode in order to ensure the 32K clock start correctly)
	R_SYSTEM_CTRL &= ~((1<<12) | (1<<8));	//disable RTC schedule funtion  ,disable CSI Clock
	R_SYSTEM_CLK_CTRL &= ~((1<<8) | (1<<4));//turn off Time Base 32K Clock, Disable DA/AD PLL

	if(mode==0) {
		//sleep mode and wake up reset
		void (*pFunc)(void);

		R_SYSTEM_CTRL &= ~(1 << 0);		//Disable 6MHz XTAL PAD.
		R_SYSTEM_PLL_WAIT_CLK = 0x100;	//set pll wait clock to 8 ms when wakeup/
		pFunc = DisableSDRAMAndGoToSleep;
		pFunc();						//Disable SDRAM
	} else if(mode==1) {
		//halt mode and wake up next instruction
		R_SYSTEM_CTRL |= (1 << 0);		//let the 6MHz XTAL PAD enter sleep mode when system is in Halt mode.
		sys_sdram_auto_refresh_set(64); //auto enter self refresh mode
		R_SYSTEM_PLL_WAIT_CLK = 0x100; 	//set pll wait clock to 8 ms when wakeup
		R_SYSTEM_CTRL |= 0x2; 			//next instruction after weekup
		R_SYSTEM_HALT = 0x500A;
	} else if(mode==4) {
		//halt mode and wake up reset
	#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
		//set halt mode reset address
		HALT_MODE_RESET_VECTOR = 0xF80003A0;
	#endif
		R_SYSTEM_CTRL |= (1 << 0);		//let the 6MHz XTAL PAD enter sleep mode when system is in Halt mode.
		R_SYSTEM_PLL_WAIT_CLK = 0x100; 	//set pll wait clock to 8 ms when wakeup/
		R_SYSTEM_CTRL &= ~0x2; 			//reset after wake up

	#if (EXTA_WAKEUP == 1)
	 	while(R_IOF_I_DATA & BIT5) { 	//IOF5
			R_INT_KECON |= BIT6;		//clear EXTA int flag
		}
	#endif
	#if (EXTB_WAKEUP == 1)
		while(R_IOC_I_DATA & BIT10)	{	//IOC10 wait for the user to release the power button.
			R_INT_KECON |= BIT7;		//clear EXTB int flag
		}
	#endif

		R_SYSTEM_HALT = 0x500A;
	} else {
		//wait mode
		R_SYSTEM_PLL_WAIT_CLK = 0x100; 	//set pll wait clock to 8 ms when wakeup
		R_SYSTEM_WAIT = 0x5005;
	}

    tmp = R_CACHE_CTRL;
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