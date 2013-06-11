/*
* Purpose: system initial functions after reset
*
* Author: dominant
*
* Date: 2009/12/08
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
     1. Dominant modify new system initial style to switch PLL more stable
     2. New system initial support overclock foe GPL326XX
*/
//Include files
#include "driver_l1.h"
#include "drv_l1_nand_flash_ext.h"

void drv_l1_init(void);
void drv_l1_reinit(void);



void drv_l1_init(void)
{
	// First initiate those settings that affect system, like: SDRAM access, system clock, cache, exception handler, interrupt, watchdog
	// Then initiate those hardware modules that are bus masters, like: DMA, JPEG, scaler, PPU, SPU, TFT
	// Finally, initiate other hardware modules

  #if _DRV_L1_MEMC==1
	//memc_init();						// Initiate Memory controller
    // the memc initial move to system_init();                    
  #endif

  #if _DRV_L1_SYSTEM == 1  
    if (sdram_calibrationed_flag() == 0) {
    	if (mcu_version_get() == GPL32_B) {   //GPL325XX VerB
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SDRAM_SIZE, NEG_ON, SDRAM_DRV_8mA);
           #else
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SDRAM_SIZE, NEG_ON, SDRAM_DRV_4mA);
           #endif
        } else if (mcu_version_get() == GPL32_C) {  //GPL325XX VerC
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SDRAM_SIZE, NEG_ON, SDRAM_DRV_8mA);
           #else
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SDRAM_SIZE, NEG_ON, SDRAM_DRV_4mA);
           #endif
        } else if (mcu_version_get() >= GPL326XX) {  //GPL326XX 
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
           if (MHZ <= 96) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV1, SDRAM_SIZE, NEG_ON, SDRAM_DRV_16mA);
           } else if (MHZ == 102) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV2, SDRAM_SIZE, NEG_ON, SDRAM_DRV_16mA);
           } else if (MHZ == 108) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV3, SDRAM_SIZE, NEG_ON, SDRAM_DRV_16mA);
           } 
           #else
           if (MHZ <= 96) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV0, SDRAM_SIZE, NEG_ON, SDRAM_DRV_4mA);
           } else if (MHZ == 102) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV1, SDRAM_SIZE, NEG_ON, SDRAM_DRV_4mA);
           } else if (MHZ == 108) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV2, SDRAM_SIZE, NEG_ON, SDRAM_DRV_4mA);
           } 
           #endif
        }
    } else if(sdram_calibrationed_flag() != 0) { /*calibrationed by nvpacker*/
       #if BOARD_TYPE < BOARD_TURNKEY_BASE  // EMU Board
        sys_pll_set(MHZ, 0, 0, 0, 0, SDRAM_DRV_12mA);  /*setup PLL without sdram parameter*/
       #else  // Turnkey Board
        sys_pll_set(MHZ, 0, 0, 0, 0, SDRAM_DRV_4mA);  /*setup PLL without sdram parameter*/
       #endif 
    } 

    system_bus_arbiter_init();			// Initaile bus arbiter   
  #endif


  #if _DRV_L1_CACHE == 1
	cache_init();						// Initiate CPU Cache
  #endif

	exception_table_init();				// Initiate CPU exception handler
  #if _DRV_L1_INTERRUPT == 1
	vic_init();							// Initiate Interrupt controller
  #endif

  #if _DRV_L1_WATCHDOG==1
	watchdog_init();					// Initiate WatchDOg
  #endif

  #if _DRV_L1_DMA == 1
	dma_init();							// Initiate DMA controller
  #endif

  #if _DRV_L1_DEFLICKER == 1
	deflicker_init();					// Initiate deflicker engine
  #endif

  #if _DRV_L1_PPU == 1
	ppu_init();							// Initiate PPU engine
  #endif

  #if _DRV_L1_JPEG == 1
	jpeg_init();						// Initiate JPEG engine
  #endif

  #if _DRV_L1_SCALER == 1
	scaler_init();						// Initiate Scaler engine
  #endif

  #if _DRV_L1_GPIO==1
	gpio_init();						// Initiate GPIO and IO PADs
  #endif

  #if _DRV_L1_TFT==1
	tft_init();							// Initiate TFT controller
  #endif

  #if _DRV_L1_SPU==1
//	SPU_Initial();							// Initiate SPU
  #endif

  #if _DRV_L1_UART==1
	#if BOARD_TYPE < BOARD_TURNKEY_BASE
      uart0_init();						// Initiate UART
      #if (defined CONSOLE_DBG_MSG) && (CONSOLE_DBG_MSG==CUSTOM_ON)  /* jandy add , 2009/01/12 */
    	uart0_buad_rate_set(UART0_BAUD_RATE);
     	uart0_tx_enable();
     	uart0_rx_enable();
      #endif
    #endif
  #endif

  #if _DRV_L1_SPI==1
	spi_disable(0);						// Initiate SPI
  #endif

  #if _DRV_L1_KEYSCAN==1
	//matre_keyscaninit();
  #endif

  #if _DRV_L1_EXT_INT==1
    ext_int_init();
  #endif

  #if _DRV_L1_DAC==1
	dac_init();						// Initiate DAC
  #endif

  #if _DRV_L1_ADC==1
	adc_init();						// Initiate Analog to Digital Convertor
  #endif
	
  #if _DRV_L1_MIC==1
  	mic_init();						// Initiate Micphone
  #endif

  #if _DRV_L1_TIMER==1
	timer_init();						// Initiate Timer
    timerD_counter_init();              // Tiny Counter Initial (1 tiny count == 2.67 us)
  #endif




 #if _DRV_L1_RTC==1
//	rtc_init();							// Initiate Real Time Clock
  #endif

  #if _DRV_L1_SW_TIMER== 1
   #if _DRV_L1_RTC==1
    sw_timer_init(TIMER_RTC, 1024);
   #endif
   #ifdef DRV_L1_SW_ISR_PHY 
    sw_timer_with_isr_phy_init(DRV_L1_SW_ISR_PHY, DRV_L1_SW_ISR_PHY_Hz);
   #endif
  #endif
  
  #if _DRV_L1_NAND==1
  	#ifdef GPL326XXB_BCH60
  		nand_iopad_sel(NAND_IO_AUTO_SCAN);
  	#endif
  	
    DrvL1_Nand_Init();				// Initiate NAND
  #endif
    
  #if (defined SUPPORT_JTAG) && (SUPPORT_JTAG == CUSTOM_OFF)
     gpio_set_ice_en(FALSE);  
  #endif

  #if BOARD_TYPE == BOARD_TK_V4_0
  	/* iog0 for gpy0030 */
  	gpio_set_bkueb(FALSE);
  	gpio_init_io(IO_G0, GPIO_OUTPUT);
  	gpio_set_port_attribute(IO_G0, 1);
  	gpio_write_io(IO_G0, DATA_HIGH);
  	/* enable TFT panel */
  	gpio_set_port_attribute(IO_B4,1);
  	gpio_init_io(IO_B4, GPIO_OUTPUT);
  	gpio_write_io(IO_B4, DATA_HIGH);
  #elif BOARD_TYPE == BOARD_TK_V4_1
  	/* enable TFT panel */
  	gpio_set_port_attribute(IO_F9,1);
  	gpio_init_io(IO_F9, GPIO_OUTPUT);
  	gpio_write_io(IO_F9, DATA_HIGH);
  #elif (BOARD_TYPE == BOARD_TK_V4_3 || BOARD_TYPE == BOARD_TK_V4_4 || BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_8D_V2_0)
  	/* ioh0 for gpy0030 */
  	gpio_set_memcs(0,0);
	gpio_set_memcs(1,0);
	gpio_set_memcs(2,0);
	gpio_set_memcs(3,0);
	drv_msec_wait(10);
  	gpio_init_io(IO_H0, GPIO_OUTPUT);
  	gpio_set_port_attribute(IO_H0, 1);
  	gpio_write_io(IO_H0, DATA_HIGH);
  	/* enable TFT panel */
  	gpio_set_port_attribute(IO_F14,1);
  	gpio_init_io(IO_F14, GPIO_OUTPUT);
  	gpio_write_io(IO_F14, DATA_HIGH);
  	/* turn off ICE */
  	gpio_set_ice_en(FALSE);
  #elif BOARD_TYPE == BOARD_TK_A_V1_0 ||  BOARD_TYPE == BOARD_TK_A_V2_0 || BOARD_TYPE == BOARD_TK_A_V2_1_5KEY
  	/* turn off ICE */
  	gpio_set_ice_en(FALSE);
  #elif BOARD_TYPE == BOARD_TK_32600_V1_0
  	/* ioh1 for gpy0031 and TFT enable */
  	gpio_sdram_swith(5,1);
	drv_msec_wait(10);
  	gpio_init_io(IO_H1, GPIO_OUTPUT);
  	gpio_set_port_attribute(IO_H1, 1);
  	gpio_write_io(IO_H1, DATA_HIGH);
  	/* turn off ICE */
  	gpio_set_ice_en(FALSE);
  #endif

    


  #if _DRV_L1_GPY0200 == 1
	gpy0200_init(); /*gpy0200 init */
  #endif
	

}


void drv_l1_reinit(void)
{
	// First initiate those settings that affect system, like: SDRAM access, system clock, cache, exception handler, interrupt, watchdog
	// Then initiate those hardware modules that are bus masters, like: DMA, JPEG, scaler, PPU, SPU, TFT
	// Finally, initiate other hardware modules


  #if _DRV_L1_SYSTEM == 1  
    if (sdram_calibrationed_flag() == 0) {
    	if (mcu_version_get() == GPL32_B) {
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SD_64Mb, NEG_ON, SDRAM_DRV_8mA);
           #else
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SD_64Mb, NEG_ON, SDRAM_DRV_4mA);
           #endif
        } else if (mcu_version_get() == GPL32_C) {
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SD_64Mb, NEG_ON, SDRAM_DRV_8mA);
           #else
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV1, SDRAM_IN_DLY_LV0, SD_64Mb, NEG_ON, SDRAM_DRV_4mA);
           #endif
        } else if (mcu_version_get() == GPL326XX) {
           #if BOARD_TYPE < BOARD_TURNKEY_BASE
           if (MHZ <= 96) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV1, SD_64Mb, NEG_ON, SDRAM_DRV_16mA);
           } else if (MHZ == 102) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV2, SD_64Mb, NEG_ON, SDRAM_DRV_16mA);
           } else if (MHZ == 108) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV3, SDRAM_IN_DLY_LV3, SD_64Mb, NEG_ON, SDRAM_DRV_16mA);
           } 
           #else
           if (MHZ <= 96) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV0, SD_64Mb, NEG_ON, SDRAM_DRV_4mA);
           } else if (MHZ == 102) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV1, SD_64Mb, NEG_ON, SDRAM_DRV_4mA);
           } else if (MHZ == 108) {
            sys_pll_set(MHZ, SDRAM_OUT_DLY_LV4, SDRAM_IN_DLY_LV2, SD_64Mb, NEG_ON, SDRAM_DRV_4mA);
           } 
           #endif
        }

    } else if(sdram_calibrationed_flag() != 0) { /*calibrationed by nvpacker*/
        sys_pll_set(MHZ, 0, 0, 0, 0, 0);  /*setup PLL without sdram parameter*/
    } 
    system_bus_arbiter_init();			// Initaile bus arbiter   
  #endif

  #if _DRV_L1_CACHE == 1
	cache_init();						// Initiate CPU Cache
  #endif

	exception_table_init();				// Initiate CPU exception handler
  #if _DRV_L1_INTERRUPT == 1
	vic_init();							// Initiate Interrupt controller
  #endif

  #if _DRV_L1_WATCHDOG==1
	watchdog_init();					// Initiate WatchDOg
  #endif

  #if _DRV_L1_DMA == 1
	dma_init();							// Initiate DMA controller
  #endif

  #if _DRV_L1_PPU == 1
	ppu_init();							// Initiate PPU engine
  #endif

  #if _DRV_L1_JPEG == 1
	jpeg_init();						// Initiate JPEG engine
  #endif

  #if _DRV_L1_SCALER == 1
	scaler_init();						// Initiate Scaler engine
  #endif

  #if _DRV_L1_GPIO==1
	gpio_init();						// Initiate GPIO and IO PADs
  #endif


  #if _DRV_L1_UART==1
	#if BOARD_TYPE < BOARD_TURNKEY_BASE
    uart0_init();						// Initiate UART
      #if (defined CONSOLE_DBG_MSG) && (CONSOLE_DBG_MSG==CUSTOM_ON)  /* jandy add , 2009/01/12 */
     	uart0_buad_rate_set(UART0_BAUD_RATE);
     	uart0_tx_enable();
     	uart0_rx_enable();
      #endif
    #endif
  #endif

  #if _DRV_L1_SPI==1
	spi_disable(0);						// Initiate SPI
  #endif

  #if _DRV_L1_KEYSCAN==1
	//matre_keyscaninit();
  #endif

  #if _DRV_L1_EXT_INT==1
    ext_int_init();
  #endif

  #if _DRV_L1_TIMER==1
	timer_init();						// Initiate Timer
    timerD_counter_init();              // Tiny Counter Initial (1 tiny count == 2.67 us)
  #endif

 #if _DRV_L1_RTC==1
	rtc_init();							// Initiate Real Time Clock
  #endif

  #if _DRV_L1_SW_TIMER== 1
   #if _DRV_L1_RTC==1
    sw_timer_init(TIMER_RTC, 1024);
   #endif
  #endif
  
  #if _DRV_L1_NAND==1
     DrvL1_Nand_Init();				// Initiate NAND
  #endif
  
  #if BOARD_TYPE == BOARD_TK_V4_0
  	/* iog0 for spy0030 */
  	gpio_set_bkueb(FALSE);
  	gpio_init_io(IO_G0, GPIO_OUTPUT);
  	gpio_set_port_attribute(IO_G0, 1);
  	gpio_write_io(IO_G0, DATA_HIGH);
  	/* enable TFT panel */
  	gpio_set_port_attribute(IO_B4,1);
  	gpio_init_io(IO_B4, GPIO_OUTPUT);
  	gpio_write_io(IO_B4, DATA_HIGH);
  #elif BOARD_TYPE == BOARD_TK_V4_1
  	/* enable TFT panel */
  	gpio_set_port_attribute(IO_F9,1);
  	gpio_init_io(IO_F9, GPIO_OUTPUT);
  	gpio_write_io(IO_F9, DATA_HIGH);
  #endif

  #if _DRV_L1_GPY0200 == 1
	gpy0200_init(); /*gpy0200 init */
  #endif
  
}
