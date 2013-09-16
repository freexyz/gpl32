/******************************************************************************
 * Purpose: GPIO driver/interface  
 * Author: Dominant Yang
 * Date: 2008/01/21
 * Copyright Generalplus Corp. ALL RIGHTS RESERVED.
 ******************************************************************************/
 
/******************************************************************************
 * Paresent Header Include
 ******************************************************************************/
#include "drv_l1_gpio.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_GPIO) && (_DRV_L1_GPIO == 1)                 //
//================================================================//

/******************************************************************************
 * Static Variables
 ******************************************************************************/ 
/*以下參數最好置放於 internal ram*/
static INT16U bit_mask_array[16];
static INT16U bit_mask_inv_array[16];
static INT32U gpio_data_out_addr[9];
static INT32U gpio_data_in_addr[9];
static INT8U  gpio_set_array[144];
static INT8U  gpio_setbit_array[144];
/*以上參數最好置放於 internal ram*/

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void gpio_init(void);
//void gpio_pad_init(void);
void gpio_pad_init2(void);
BOOLEAN gpio_init_io(INT32U port, BOOLEAN direction);
BOOLEAN gpio_read_io(INT32U port);
BOOLEAN gpio_write_io(INT32U port, BOOLEAN data);
BOOLEAN gpio_set_port_attribute(INT32U port, BOOLEAN attribute);
BOOLEAN gpio_write_all_by_set(INT32U gpio_set_num, INT32U write_data);
BOOLEAN gpio_get_dir(INT32U port);
INT32U gpio_read_all_by_set(INT8U gpio_set_num);
BOOLEAN gpio_drving_init_io(GPIO_ENUM port, IO_DRV_LEVEL gpio_driving_level);
void gpio_monitor(GPIO_MONITOR *gpio_monitor);
void gpio_drving_init(void);
void gpio_drving_uninit(void);
void gpio_xdc_nand_pad_mode_set(XD_PAD_MODE Mode);
XD_PAD_MODE gpio_xdc_nand_mode_get(void);
void gpio_basic_init(void);
static INT8U gpio_mcu_ver_get(void);

/******************************************************************************
 * Function Body
 ******************************************************************************/
void gpio_sdram_swith(INT32U port, BOOLEAN data)
{
  INT16U i;
  i =0x00001;
  i= i << port ;
  if (data)
  {
	 R_FUNPOS1 |= i;
  } else
  {
  	 R_FUNPOS1 &= ~i;
  }	 
}
void gpio_init(void)
{
    /* initial gpio pad */
    gpio_pad_init2();
    /* initial all gpio */
	gpio_basic_init();
    /* initial gpio pad driving */
    gpio_drving_init();
}

void gpio_basic_init(void)
{
    INT16U i;
    for (i=0; i<8;i++)
    {
      //   DRV_Reg32(IOA_ATTRIB_ADDR+(0x20*i)) = 0xffff;  /*Set all gpio attribute to 1 */
         gpio_data_out_addr[i] = (0xC0000004 + (0x20*i));
         gpio_data_in_addr[i] = (0xC0000000 + (0x20*i));
    }
        
    for (i=0; i<16; i++)
    {
        bit_mask_array[i]=1<<i;
        bit_mask_inv_array[i]=~(bit_mask_array[i]);
    }

    for (i=0; i<128; i++)
    {
        gpio_set_array[i]=i/16;
        gpio_setbit_array[i]=i%16;
    };

	
#if (MCU_VERSION >= GPL326XXB) && (MCU_VERSION < GPL327XX)
	gpio_data_out_addr[8] = (0xC0000184);
	gpio_data_in_addr[8] = (0xC0000180);
		
    for (i=128; i<144; i++)
    {
        gpio_set_array[i]=i/16;
        gpio_setbit_array[i]=i%16;
    };	
		
#endif

}


#if 0
void gpio_pad_init(void)
{
#if NAND_CS_POS == NF_CS_AS_BKCS1
   R_MEM_IO_CTRL = 0xEFFF;   //MEMC1 Disable
#elif NAND_CS_POS == NF_CS_AS_BKCS2
   R_MEM_IO_CTRL = 0xDFFF;   //MEMC2
#elif NAND_CS_POS == NF_CS_AS_BKCS3
   R_MEM_IO_CTRL = 0xBFFF;   //MEMC1   
#endif

#if UART_POS_SET_VALUE == UART_TX_IOH2__RX_IOH3
   R_GPIOCTRL &= ~0x00000001; /*switch off ice*/
#elif UART_POS_SET_VALUE == UART_TX_IOF3__RX_IOF8
    R_MEM_IO_CTRL &= 0xBFFF;   //Release BKCS3(IOF3) to UART TX
#else

#endif


    R_FUNPOS0 = ((R_FUNPOS0 & 0)| UART_TX_RX_POS | KEYCHANGE_POS_SET_VALUE |NAND_POS_SET_VALUE | UART_POS_SET_VALUE | TFT_POS_SET_VALUE | CMOS_POS_SET_VALUE |CF_POS_SET_VALUE ) ;                     

#if NAND_CTRL_POS==NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
    R_IOSRSEL |= IOSRSEL_NF_ALE_CLE_REB_WEB_IOG5_IOG6_IOG10_IOG11;
#endif

#if NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOA13_8  /* force mode to modify NF_DATA0~7 to IOA8~15*/
    R_SYSMONICTRL |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
#endif    
    
#if (UART_TX_RX_POS == UART_TX_IOB2__RX_IOB1) ||  (UART_TX_RX_POS == UART_TX_IOF3__RX_IOF8)
    R_IOSRSEL |= IOSRSEL_UART_TXRX_0_IOH3H2_IOE2E3__1_IOB2B1_IOF3F8;
#endif

#if (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOH0_IOB5_IOB6)
    R_GPIOCTRL |= IOCTRL_CMOS_CLK0_AT_IOH0;
#endif

}
#endif

void gpio_pad_init2(void)
{
    /*initial temp register*/
    INT32U mem_io_ctrl=R_MEM_IO_CTRL;
    INT32U funpos0=R_FUNPOS0;
    INT32U funpos1=R_FUNPOS1;
    INT32U iosrsel=R_IOSRSEL;
    INT32U sysmonictrl=R_SYSMONICTRL;
    INT32U pwm_ctrl=R_PWM_CTRL;
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
	INT32U funpos2=R_FUNPOS2;
#endif
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB) && (MCU_VERSION < GPL327XX) 
	INT32U funpos3=R_FUNPOS3;
#endif

/* BKCS Pad Initial START */
#if (defined BKCS_0_EN) && (BKCS_0_EN == TRUE)
   mem_io_ctrl &= ~(1<<11);   //MEMC0 Disable and become gpio
#endif

#if (defined BKCS_1_EN) && (BKCS_1_EN == TRUE)
   mem_io_ctrl &= ~(1<<12);   //MEMC1 Disable and become gpio
#endif

#if (defined BKCS_2_EN) && (BKCS_2_EN == TRUE)
   mem_io_ctrl &= ~(1<<13);   //MEMC2 Disable and become gpio
#endif

#if (defined BKCS_3_EN) && (BKCS_3_EN == TRUE)
   mem_io_ctrl &= ~(1<<14);   //MEMC3 Disable and become gpio 
#endif
/* BKCS Pad Initial END */

/* Nand Pad Dispacher START */
#if (_DRV_L1_NAND==1)
   #if (defined NAND_PAGE_TYPE) && (NAND_PAGE_TYPE==NAND_SMALLBLK)  // different page type will decision nand cs out
       #if NAND_CS_POS == NF_CS_AS_BKCS1
          nand_small_page_cs_pin_reg(NAND_CS1);          
       #elif NAND_CS_POS == NF_CS_AS_BKCS2
          nand_small_page_cs_pin_reg(NAND_CS2);
       #elif NAND_CS_POS == NF_CS_AS_BKCS3
          nand_small_page_cs_pin_reg(NAND_CS3);
       #endif
       funpos0 |= (1<<5); funpos0 |= (1<<4);  /* Hardware CS assign to NULL CS */
   #else
       #if NAND_CS_POS == NF_CS_AS_BKCS1
          mem_io_ctrl &= ~(1<<12);   //MEMC1 Disable and become gpio
          funpos0 &= ~(1<<5|1<<4); 
          nand_large_page_cs_reg(NAND_CS1);
       #elif NAND_CS_POS == NF_CS_AS_BKCS2
          mem_io_ctrl &= ~(1<<13);   //MEMC2 Disable and become gpio
          funpos0 &= ~(1<<5); funpos0 |= (1<<4);
          nand_large_page_cs_reg(NAND_CS2);
       #elif NAND_CS_POS == NF_CS_AS_BKCS3
          mem_io_ctrl &= ~(1<<14);   //MEMC3 Disable and become gpio   
          funpos0 |= (1<<5); funpos0 &= ~(1<<4);
          nand_large_page_cs_reg(NAND_CS3);
       #endif
   #endif
   
   #if (defined NAND_WP_IO) && (NAND_WP_IO!=NAND_WP_PIN_NONE)
       nand_wp_pin_reg(NAND_WP_IO);
   #endif
   
   #if NAND_SHARE_MODE == NF_NON_SHARE  /* only effect in nand "non-shard with SDRAM" MODE */
       #if NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOB13_8
           funpos0 &= ~(1<<9|1<<8);                  /*(Fun_POS[9:8] == 2'h0)*/
       #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOD5_0
           funpos0 &= ~(1<<9);  funpos0 |= (1<<8); /*(Fun_POS[9:8] == 2'h1)*/
       #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOE5_0
           funpos0 |= (1<<9);  funpos0 &= ~(1<<8); /*(Fun_POS[9:8] == 2'h2)*/
       #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOA13_8  /* force mode to modify NF_DATA0~7 to IOA8~15*/
           sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
       #endif
       
       #if NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOB15_14
           funpos0 &= ~(1<<7|1<<6);                  /*(Fun_POS[7:6] == 2'h0) */
       #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOD7_6
           funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h1)*/
       #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOE7_6
           funpos0 |= (1<<7);  funpos0 &= ~(1<<6); /*((Fun_POS[7:6] == 2'h2)*/
       #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOC5_4
           funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h3)*/
       #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOA15_14  /* force mode to modify NF_DATA0~7 to IOA8~15*/
           sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
           #if NAND_DATA5_0_POS != NAND_DATA5_0_AS_IOA13_8
               #error  
           #endif
       #endif
   #endif //NAND_SHARE_MODE == NF_NON_SHARE
#endif //(_DRV_L1_NAND==1)

#if (_DRV_L1_NAND == 1) && (_DRV_L1_XDC == 0) 
     #if (NAND_SHARE_MODE == NF_NON_SHARE)  /* in non-share mode have 2 choice */ 
         #if NAND_CTRL_POS==NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
             funpos0 &= ~(1<<3);
             iosrsel &= ~(1<<14);
         #elif NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9 
         	 #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
             funpos0 |= (1<<3);
             iosrsel &= ~(1<<14);
             #else
             iosrsel |= 1<<14;
             #endif
         #elif NAND_CTRL_POS==NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
             /* Release MEMA20 & MEMA19 for nand CTRL ALE/CLE */
             mem_io_ctrl &= ~(1<<1 | 1<<2);
             iosrsel |= IOSRSEL_NF_ALE_CLE_REB_WEB_IOG5_IOG6_IOG10_IOG11;       
         #endif
     #elif (NAND_SHARE_MODE == NF_SHARE_MODE) /* in share mode only 2 choices (and sd ram share set)*/
         #if NAND_CTRL_POS==NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
             /* Release MEMA20 & MEMA19 for nand CTRL ALE/CLE */
             mem_io_ctrl &= ~(1<<1 | 1<<2);
             iosrsel |= IOSRSEL_NF_ALE_CLE_REB_WEB_IOG5_IOG6_IOG10_IOG11;
         #elif NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9
         	 #if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
         	 iosrsel |= 1 << 14;
         	 #endif
         #else
             iosrsel =0;
         #endif
     #endif
#elif (_DRV_L1_NAND == 0) && (_DRV_L1_XDC == 1)
     #if XD_CTRL_POS==XD_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
         funpos0 &= ~(1<<3);
         iosrsel &= ~(1<<14);
     #elif XD_CTRL_POS==XD_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9 
         funpos0 |= (1<<3);
         iosrsel &= ~(1<<14);
     #endif
#elif (_DRV_L1_NAND == 1) && (_DRV_L1_XDC == 1) 
   #if (NAND_SHARE_MODE == NF_NON_SHARE)  /* in non-share mode have 2 choice */ 
     #if NAND_CTRL_POS==NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
         funpos0 &= ~(1<<3);
         iosrsel &= ~(1<<14);
     #elif NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9 
     	 #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)  
         funpos0 |= (1<<3);
         iosrsel &= ~(1<<14);
         #else  
         iosrsel |= 1<<14;
         #endif
     #endif
   #elif (NAND_SHARE_MODE == NF_SHARE_MODE) /* in share mode only 2 choices (and sd ram share set)*/
     #if NAND_CTRL_POS==NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
     /* Release MEMA20 & MEMA19 for nand CTRL ALE/CLE */
     mem_io_ctrl &= ~(1<<1 | 1<<2);
     iosrsel |= IOSRSEL_NF_ALE_CLE_REB_WEB_IOG5_IOG6_IOG10_IOG11;

     #elif NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9
     	 #if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
         iosrsel |= 1 << 14;
     	 #endif
     #endif
   #endif
#endif  //(_DRV_L1_NAND==1)

 
#if (_DRV_L1_XDC == 1) 
    #if (defined XD_CS_POS) && (XD_CS_POS == NF_CS_AS_BKCS1)
        xd_cs_pin_reg(XD_CS1);
    #elif (defined XD_CS_POS) && (XD_CS_POS == NF_CS_AS_BKCS2)
        xd_cs_pin_reg(XD_CS2);
    #elif (defined XD_CS_POS) && (XD_CS_POS == NF_CS_AS_BKCS3)
        xd_cs_pin_reg(XD_CS3);
    #endif
#endif

#if (_DRV_L1_NAND == 1) && (_DRV_L1_XDC == 0)   /* Only Nand1 */
    #if (NAND_SHARE_MODE == NF_SHARE_MODE)  
        R_MEM_SDRAM_MISC|=0x10;  /* Disable SDRAM Preload process to avoid system crash */
        //R_NF_CTRL = 0x2000;  /* Enable Nand Share Pad (with SDRAM) */  
        R_MEM_M11_BUS_PRIORITY = 0x0004 ;//shin add for adding others SDRAM bandwidth beside NF
        nand_share_mode_reg(NF_SHARE_PIN_MODE);        
    #elif (NAND_SHARE_MODE == NF_NON_SHARE)
        nand_share_mode_reg(NF_NON_SHARE_MODE);
    #endif

#elif (_DRV_L1_NAND == 1) && (_DRV_L1_XDC == 1)  
    #if (NAND_SHARE_MODE == NF_SHARE_MODE)   /* Nand-Share + XD-NonShare*/
        R_MEM_SDRAM_MISC|=0x10;  /* Disable SDRAM Preload process to avoid system crash */
        //R_NF_CTRL = 0x2000;  /* Enable Nand Share Pad (with SDRAM) */  
        R_MEM_M11_BUS_PRIORITY = 0x0004 ;//shin add for adding others SDRAM bandwidth beside NF
        nand_share_mode_reg(NF_SHARE_PIN_MODE); 
    #elif (NAND_SHARE_MODE == NF_NON_SHARE)  /* Nand-NonShare + XD-Share*/
        nand_share_mode_reg(NF_NON_SHARE_MODE);
    #endif
        xdc_nand_pad_mode_set(XD_NAND_MODE);  /*jandy add, 2009/03/31 */
#elif (_DRV_L1_NAND == 0) && (_DRV_L1_XDC == 1) /* XD only, must be non-share mode */ 
    #if XD_DATA5_0_POS==XD_DATA5_0_AS_IOB13_8
        funpos0 &= ~(1<<9|1<<8);                  /*(Fun_POS[9:8] == 2'h0)*/
    #elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOD5_0
        funpos0 &= ~(1<<9);  funpos0 |= (1<<8); /*(Fun_POS[9:8] == 2'h1)*/
    #elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOE5_0
        funpos0 |= (1<<9);  funpos0 &= ~(1<<8); /*(Fun_POS[9:8] == 2'h2)*/
    #elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOA13_8  /* force mode to modify NF_DATA0~7 to IOA8~15*/
        sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
    #endif
    
    #if XD_DATA7_6_POS==XD_DATA7_6_AS_IOB15_14
        funpos0 &= ~(1<<7|1<<6);                  /*(Fun_POS[7:6] == 2'h0) */
    #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOD7_6
        funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h1)*/
    #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOE7_6
        funpos0 |= (1<<7);  funpos0 &= ~(1<<6); /*((Fun_POS[7:6] == 2'h2)*/
    #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOC5_4
        funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h3)*/
    #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOA15_14  /* force mode to modify NF_DATA0~7 to IOA8~15*/
        sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
        #if XD_DATA5_0_POS != XD_DATA5_0_AS_IOA13_8
            #error  
        #endif
    #endif
    xdc_nand_pad_mode_set(XD_ONLY_MODE);  /*jandy add, 2009/03/31 */
#endif

/* NAND/XD pad Dispacher END*/

#if XA22_XA21_POS == XA22_IOG10__XA21_IOG11
    funpos0 |= (1<<14);
#elif XA22_XA21_POS == XA22_IOG3__XA21_IOG4 
    funpos0 &= ~(1<<14);
#endif



/* CFC Pad Dispacher START */
#if CF_DATA0_15_POS == CF_DATA0_15_at_IOD0_15
    funpos0 &= ~(1<<11);
#elif CF_DATA0_15_POS == CF_DATA0_15_at_IOE0_15
    funpos0 |= (1<<11);
#endif
/* CFC Pad Dispacher END */

/* SD Pad Dispacher START */
#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XX)
	#if (SD_POS == SDC_IOD8_IOD9_IOD10_IOD11_IOD12_IOD13)
	    iosrsel |= (1<<11);
	#elif (SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)
	    iosrsel &= ~(1<<11);    
	#endif
#elif (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)
	#if (SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)
	    funpos2 &= ~0x200;
	#else
	    funpos2 |= 0x200;    
	#endif
#elif (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
	#if (SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)
	    iosrsel &= ~(1<<11);
	#else
		iosrsel |= (1<<11);
	#endif	
#endif

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB) && (MCU_VERSION < GPL327XX) 
	#if (SD1_POS == SDC1_IOI4_IOI5_IOI6_IOI7_IOI8_IOI9)
		funpos3 |= 0x4;    
	#else
		funpos3 &= ~0x4;
	#endif
#endif

/* SD Pad Dispacher END */

/* UART Pad Dispacher START */  
#if (UART_TX_RX_POS == UART_TX_IOE2__RX_IOE3)
    funpos0 &= ~(1<<0);
    iosrsel &= ~(1<<15);
#elif (UART_TX_RX_POS == UART_TX_IOH2__RX_IOH3)
    funpos1 &= ~0x00000001; /*switch off ice*/
    funpos0 |= (1<<0);
    iosrsel &= ~(1<<15);
#elif (UART_TX_RX_POS == UART_TX_IOB2__RX_IOB1)
    funpos0 &= ~(1<<0);
    iosrsel |= (1<<15);
#elif (UART_TX_RX_POS == UART_TX_IOF3__RX_IOF8)
    mem_io_ctrl &= 0xBFFF;   //Release BKCS3(IOF3) to UART TX
    funpos0 |= (1<<0);
    iosrsel |= (1<<15);
#elif (UART_TX_RX_POS == UART_TX_IOF7__RX_IOF8)
	funpos0 |= (1<<0);
	iosrsel |= (1<<15);
 	funpos2 |= (1<<0);
#elif (UART_TX_RX_POS == UART_TX_IOF7__RX_NONE)
	funpos0 &= ~(1<<0);
	iosrsel &= ~(1<<15);
 	funpos2 |= (1<<0);
#endif
/* UART Pad Dispacher END */

/* KEY Change B Dispacher START */ 
#if KEY_CHANGE_B_POS==KEY_CHANGE_B_AT_IOE1
    funpos0 |= (1<<12);
#endif
/* KEY Change B Dispacher END */ 

/* CSI Pad Dispacher START */
#if (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOH0_IOB5_IOB6)
    funpos1 |= (1<<1);
    funpos0 |= (1<<15);
    funpos1 |= (1<<2);  /* Disable NOR Flash BKOEB */
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOC11_IOB5_IOB6)
    funpos1 &= ~(1<<1);
    funpos0 |= (1<<15);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOE8_15_IOE4_IOE5_IOE6_IOE7)
    funpos0 |= (1<<10);
    funpos0 &= ~(1<<15);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA0_7_IOB0_IOB1_IOB2_IOB3)
    funpos0 &= ~(1<<10);
    funpos0 &= ~(1<<15);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC2_IOH0)
    funpos1 |= (1<<1);
    funpos0 |= (1<<10);
    funpos0 &= ~(1<<15); 
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC11_IOH0)
	funpos1 &= ~(1<<1);
    funpos0 |= (1<<10);
    funpos0 &= ~(1<<15);        
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOB8_15_IOF3_IOH0_IOB4_IOB7)
	funpos1 &= ~(1<<1);
	funpos1 &= ~(1<<2);
	funpos0 &= ~(1<<15);
	funpos0 |= (1<<10);
	funpos2 |= (1<<4);
#endif
/* CSI Pad Dispacher END */

/* SPI (second) Pad Dispacher START */
#if SPI1_POS == SPI1_RX_IOE3__CLK_IOE1__TX_IOE2
    iosrsel &= ~(1<<12);
#elif SPI1_POS == SPI1_RX_IOC4__CLK_IOC5__TX_IOC7
    iosrsel |= (1<<12);
#elif SPI1_POS == SPI1_RX_IOI3__CLK_IOI1__TX_IOI2
	funpos3 |= (1<<1);
#endif

#if SPI0_POS == SPI0_RX_IOC3__CLK_IOC1__TX_IOC2
    funpos1 &= ~(3<<8);
#elif SPI0_POS == SPI0_RX_IOC7__CLK_IOF5__TX_IOC4
    funpos1 |= 1<<8;
#elif SPI_POS == SPI0_RX_IOC3__CLK_IOF5__TX_IOF6
    funpos1 |= 2<<8;
#endif

/* SPI Pad Dispacher END */

/* IOG5 Output PCLK/6 Pad dispacher START */
#if NAND_CTRL_POS != NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11
    #if IOG5_UOT_PCLK_DIV6 == 1   /* 0: disable  1: enable*/
        iosrsel |= (1<<13);
    #endif
#endif
/* IOG5 Output PCLK/6 Pad dispacher END */


/* TFT Pad Dispacher START */
#if TFT_POL_STV_STH == TFT_POL_IOG9__STV_IOG3__STH_IOG4
     funpos0 |= (1<<13);
#elif TFT_POL_STV_STH == TFT_POL_IOB4__STV_IOB5__STH_IOB6
    funpos0 &= ~(1<<13);
#endif

#if TFT_HSYNC_VSYNC_CLK_POS == TFT_HSYNC_VSYNC_CLK_at_IOB1_IOB2_IOB3
    funpos0 &= ~(1<<1);
#elif TFT_HSYNC_VSYNC_CLK_POS == TFT_HSYNC_VSYNC_CLK_at_IOF10_IOF11_IO12
    funpos0 |= (1<<1);
#endif
/* TFT Pad Dispacher END */

/* Timer PWM Pad START */
#if TIMER_A_PWM_EN == TRUE
    mem_io_ctrl &= ~(1<<0);
#endif

#if TIMER_B_PWM_EN == TRUE
    mem_io_ctrl &= ~(1<<6);
#endif

#if TIMER_C_PWM_EN == TRUE
    pwm_ctrl &= ~(1<<11);  /* PWM1 Pin No use */
//  pwm_ctrl &= ~(1<<12);  /* FB1 Pin No use */
//  pwm_ctrl &= ~(1<<13);  /* VC1 Pin No use */
//  pwm_ctrl &= ~(1<<2);   /* PWM1 Function Disable */
#endif
/* Timer PWM Pad END */



	/* fill register */
	R_MEM_IO_CTRL = mem_io_ctrl;
	R_FUNPOS0 = funpos0;
	R_FUNPOS1 = funpos1;
	R_IOSRSEL = iosrsel;
	R_SYSMONICTRL = sysmonictrl;
	R_PWM_CTRL = pwm_ctrl;
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
	R_FUNPOS2 = funpos2;
#endif

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB) && (MCU_VERSION < GPL327XX) 
	R_FUNPOS3 = funpos3;
#endif

}

void gpio_drving_init(void)
{
    INT8U i;
    gpio_drving_uninit();
    


/* IOG5 Output PCLK/6 Pad dispacher START */

#if IOG5_UOT_PCLK_DIV6 == 1   /* 0: disable  1: enable*/
    gpio_drving_init_io(IO_G5,(IO_DRV_LEVEL) IOG5_UOT_PCLK_DIV6_DRIVING);
#endif

/* IOG5 Output PCLK/6 Pad dispacher END */


/* Timer PWM Driving START */
#if TIMER_A_PWM_EN == TRUE
    gpio_drving_init_io(IO_G0,(IO_DRV_LEVEL) TIMER_A_PWM_DRIVING);
#endif

#if TIMER_B_PWM_EN == TRUE
    gpio_drving_init_io(IO_G1,(IO_DRV_LEVEL) TIMER_A_PWM_DRIVING);
#endif

#if TIMER_C_PWM_EN == TRUE
    gpio_drving_init_io(IO_G13,(IO_DRV_LEVEL) TIMER_A_PWM_DRIVING);
#endif
/* Timer PWM Driving END */

/* TFT Pad Driving START */
#if TFT_POL_STV_STH == TFT_POL_IOG9__STV_IOG3__STH_IOG4
    gpio_drving_init_io(IO_G9,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING);  
    gpio_drving_init_io(IO_G3,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING);  
    gpio_drving_init_io(IO_G4,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING); 
#elif TFT_POL_STV_STH == TFT_POL_IOB4__STV_IOB5__STH_IOB6
    gpio_drving_init_io(IO_B4,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING);  
    gpio_drving_init_io(IO_B5,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING);  
    gpio_drving_init_io(IO_B6,(IO_DRV_LEVEL) TFT_POL_STV_STH_DRIVING); 
#endif

#if TFT_HSYNC_VSYNC_CLK_POS == TFT_HSYNC_VSYNC_CLK_at_IOB1_IOB2_IOB3
    gpio_drving_init_io(IO_B1,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);  
    gpio_drving_init_io(IO_B2,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);  
    gpio_drving_init_io(IO_B3,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);
#elif TFT_HSYNC_VSYNC_CLK_POS == TFT_HSYNC_VSYNC_CLK_at_IOF10_IOF11_IO12
    gpio_drving_init_io(IO_F10,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);  
    gpio_drving_init_io(IO_F11,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);  
    gpio_drving_init_io(IO_F12,(IO_DRV_LEVEL) TFT_HSYNC_VSYNC_CLK_DRIVING);
#endif

#ifdef TFT_DATA0_7_DRIVING
    for (i=IO_A0 ; i<=IO_A7; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) TFT_DATA0_7_DRIVING);
    }
#endif

#ifdef TFT_DATA8_15_DRIVING
    for (i=IO_A0 ; i<=IO_A7; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) TFT_DATA8_15_DRIVING);
    }
#endif

#ifdef TFT_R0G0B0_R1G1B1_DRIVING
    for (i=IO_B8 ; i<=IO_B13; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) TFT_R0G0B0_R1G1B1_DRIVING);
    }
#endif

#ifdef TFT_R2B2_DRIVING
    for (i=IO_B14 ; i<=IO_B15; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) TFT_R2B2_DRIVING);
    }
#endif


/* TFT Pad Driving END */

/* SPI (second) Pad Dispacher START */
#if SPI0_POS == SPI0_RX_IOC3__CLK_IOC1__TX_IOC2
    gpio_drving_init_io(IO_C0,(IO_DRV_LEVEL) SPI0_DRIVING);  //CS
    gpio_drving_init_io(IO_C3,(IO_DRV_LEVEL) SPI0_DRIVING);
    gpio_drving_init_io(IO_C1,(IO_DRV_LEVEL) SPI0_DRIVING);  //CLK
    gpio_drving_init_io(IO_C2,(IO_DRV_LEVEL) SPI0_DRIVING);  //TX
#elif SPI0_POS == SPI0_RX_IOC7__CLK_IOF5__TX_IOC4
	gpio_drving_init_io(IO_C7,(IO_DRV_LEVEL) SPI0_DRIVING);
    gpio_drving_init_io(IO_F5,(IO_DRV_LEVEL) SPI0_DRIVING);  //CLK
    gpio_drving_init_io(IO_C4,(IO_DRV_LEVEL) SPI0_DRIVING);  //TX
#elif SPI0_POS == SPI0_RX_IOC3__CLK_IOF5__TX_IOF6
	gpio_drving_init_io(IO_C3,(IO_DRV_LEVEL) SPI0_DRIVING);
    gpio_drving_init_io(IO_F5,(IO_DRV_LEVEL) SPI0_DRIVING);  //CLK
    gpio_drving_init_io(IO_C6,(IO_DRV_LEVEL) SPI0_DRIVING);  //TX    
#endif

#if SPI1_POS == SPI1_RX_IOE3__CLK_IOE1__TX_IOE2
    gpio_drving_init_io(IO_E1,(IO_DRV_LEVEL) SPI1_DRIVING);  //CLK
    gpio_drving_init_io(IO_E2,(IO_DRV_LEVEL) SPI1_DRIVING);  //TX
#elif SPI1_POS == SPI1_RX_IOC4__CLK_IOC5__TX_IOC7
    gpio_drving_init_io(IO_C5,(IO_DRV_LEVEL) SPI1_DRIVING);  //CLK
    gpio_drving_init_io(IO_C7,(IO_DRV_LEVEL) SPI1_DRIVING);  //TX
#endif
/* SPI Pad Dispacher END */


/* CSI Pad Dispacher START */
#if (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOH0_IOB5_IOB6)
    gpio_drving_init_io(IO_C10,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_H0,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B5,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B6,(IO_DRV_LEVEL) CMOS_DRIVING);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOC11_IOB5_IOB6)
    gpio_drving_init_io(IO_C10,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_C11,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B5,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B6,(IO_DRV_LEVEL) CMOS_DRIVING);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOE8_15_IOE4_IOE5_IOE6_IOE7)
    gpio_drving_init_io(IO_E4,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_E5,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_E6,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_E7,(IO_DRV_LEVEL) CMOS_DRIVING);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA0_7_IOB0_IOB1_IOB2_IOB3)
    gpio_drving_init_io(IO_B0,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B1,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B2,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_B3,(IO_DRV_LEVEL) CMOS_DRIVING);
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC2_IOH0)
    gpio_drving_init_io(IO_C10,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_F14,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_C2,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_H0,(IO_DRV_LEVEL) CMOS_DRIVING);    
#elif (CMOS_POS==CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC11_IOH0)
    gpio_drving_init_io(IO_C10,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_F14,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_C11,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_drving_init_io(IO_H0,(IO_DRV_LEVEL) CMOS_DRIVING);    
#endif
/* CSI Pad Dispacher END */



/* UART Pad Dispacher START */  
#if (UART_TX_RX_POS == UART_TX_IOE2__RX_IOE3)
    gpio_drving_init_io(IO_E2,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
    gpio_drving_init_io(IO_E3,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
#elif (UART_TX_RX_POS == UART_TX_IOH2__RX_IOH3)
    gpio_drving_init_io(IO_H2,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
    gpio_drving_init_io(IO_H3,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
#elif (UART_TX_RX_POS == UART_TX_IOB2__RX_IOB1)
    gpio_drving_init_io(IO_B1,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
    gpio_drving_init_io(IO_B2,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
#elif (UART_TX_RX_POS == UART_TX_IOF3__RX_IOF8)
    gpio_drving_init_io(IO_F3,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
    gpio_drving_init_io(IO_F8,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
#elif (UART_TX_RX_POS == UART_TX_IOF7__RX_IOF8)
    gpio_drving_init_io(IO_F7,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
    gpio_drving_init_io(IO_F8,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);    
#elif (UART_TX_RX_POS == UART_TX_IOF7__RX_NONE)
    gpio_drving_init_io(IO_F7,(IO_DRV_LEVEL) UART_TX_RX_DRIVING);
#endif
/* UART Pad Dispacher END */


/* SD Pad Dispacher START */
#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XX)
	#if (SD_POS == SDC_IOD8_IOD9_IOD10_IOD11_IOD12_IOD13)
	    for (i=IO_D8 ; i<=IO_D13; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	    }
	#elif (SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)
	    for (i=IO_C4 ; i<=IO_C9; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	    }
	#endif
#else
	#if ((SD_POS == SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)||(SD_DUAL_SUPPORT==1))
	    for (i=IO_C4 ; i<=IO_C9; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	        gpio_init_io(i, GPIO_INPUT);
	        gpio_write_io(i, 1);
	    }
	#endif
	
	#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)
	#if((SD_POS == SDC_IOB8_IOB9_IOB10_IOB11_IOB12_IOB13)||(SD_DUAL_SUPPORT==1))
		for (i=IO_B8 ; i<=IO_B13; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	        gpio_init_io(i, GPIO_INPUT);
	        gpio_write_io(i, 1);
	    }
	#endif
	#if(MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
	#if((SD_POS == SDC1_IOI4_IOI5_IOI6_IOI7_IOI8_IOI9)||(SD_DUAL_SUPPORT==1))
		for (i=IO_I4 ; i<=IO_I9; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	        gpio_init_io(i, GPIO_INPUT);
	        gpio_write_io(i, 1);
	    }
	#endif
	#endif
	
	
	#else
	#if((SD_POS == SDC_IOD8_IOD9_IOD10_IOD11_IOD12_IOD13)||(SD_DUAL_SUPPORT==1))
		for (i=IO_D8 ; i<=IO_D13; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) SD_DRIVING);
	        gpio_init_io(i, GPIO_INPUT);
	        gpio_write_io(i, 1);
	    }
	#endif
	#endif
#endif
/* SD Pad Dispacher END */


/* CFC Pad Dispacher START */
#if CF_DATA0_15_POS == CF_DATA0_15_at_IOD0_15
    for (i=IO_D0 ; i<=IO_D15; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) CF_DATA0_15_DRIVING);
    }
#elif CF_DATA0_15_POS == CF_DATA0_15_at_IOE0_15
    for (i=IO_E0 ; i<=IO_E15; i++)
    {
        gpio_drving_init_io(i,(IO_DRV_LEVEL) CF_DATA0_15_DRIVING);
    }
#endif
/* CFC Pad Dispacher END */

/* Nand Pad Dispacher START */
#if NAND_CS_POS == NF_CS_AS_BKCS1
   gpio_drving_init_io(IO_F1,(IO_DRV_LEVEL) NAND_CS_DRIVING);
#elif NAND_CS_POS == NF_CS_AS_BKCS2
   gpio_drving_init_io(IO_F2,(IO_DRV_LEVEL) NAND_CS_DRIVING);
#elif NAND_CS_POS == NF_CS_AS_BKCS3
   gpio_drving_init_io(IO_F3,(IO_DRV_LEVEL) NAND_CS_DRIVING);
#endif

#if NAND_SHARE_MODE == NF_NON_SHARE  /* only effect in nand "non-shard with SDRAM" MODE */
    #if NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOB13_8
        for (i=IO_B8 ; i<=IO_B13; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOD5_0
        for (i=IO_D0 ; i<=IO_D5; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOE5_0
        for (i=IO_E0 ; i<=IO_E5; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOA13_8  /* force mode to modify NF_DATA0~7 to IOA8~15*/
        for (i=IO_A8 ; i<=IO_A13; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #endif
    
    #if NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOB15_14
        for (i=IO_B14 ; i<=IO_B15; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOD7_6
        for (i=IO_D6 ; i<=IO_D7; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOE7_6
        for (i=IO_E6 ; i<=IO_E7; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOC5_4
        for (i=IO_C4 ; i<=IO_C5; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOA15_14  /* force mode to modify NF_DATA0~7 to IOA8~15*/
        for (i=IO_A14 ; i<=IO_A15; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_DATA_DRIVING);
        }
    #endif
#endif //NAND_SHARE_MODE == NF_NON_SHARE

#if NAND_CTRL_POS==NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
        for (i=IO_C12 ; i<=IO_C15; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
        }
#elif (NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9)  && (SD_POS != SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9)
        for (i=IO_C6 ; i<=IO_C9; i++)
        {
            gpio_drving_init_io(i,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
        }
#elif NAND_CTRL_POS==NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
        gpio_drving_init_io(IO_G5,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
        gpio_drving_init_io(IO_G6,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
        gpio_drving_init_io(IO_G10,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
        gpio_drving_init_io(IO_G11,(IO_DRV_LEVEL) NAND_CTRL_DRIVING);
#endif
/* Nand Pad Dispacher END */  


/* XD Pad Dispacher START */
#if (defined _DRV_L1_XDC) && (_DRV_L1_XDC == 1)
	#if XD_CS_POS == NF_CS_AS_BKCS1
	   gpio_drving_init_io(IO_F1,(IO_DRV_LEVEL) XD_CS_DRIVING);
	#elif XD_CS_POS == NF_CS_AS_BKCS2
	   gpio_drving_init_io(IO_F2,(IO_DRV_LEVEL) XD_CS_DRIVING);
	#elif XD_CS_POS == NF_CS_AS_BKCS3
	   gpio_drving_init_io(IO_F3,(IO_DRV_LEVEL) XD_CS_DRIVING);
	#endif


	#if XD_DATA5_0_POS==XD_DATA5_0_AS_IOB13_8
	    for (i=IO_B8 ; i<=IO_B13; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOD5_0
	    for (i=IO_D0 ; i<=IO_D5; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOE5_0
	    for (i=IO_E0 ; i<=IO_E5; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOA13_8  /* force mode to modify NF_DATA0~7 to IOA8~15*/
	    for (i=IO_A8 ; i<=IO_A13; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#endif

	#if XD_DATA7_6_POS==XD_DATA7_6_AS_IOB15_14
	    for (i=IO_B14 ; i<=IO_B15; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOD7_6
	    for (i=IO_D6 ; i<=IO_D7; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOE7_6
	    for (i=IO_E6 ; i<=IO_E7; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOC5_4
	    for (i=IO_C4 ; i<=IO_C5; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOA15_14  /* force mode to modify NF_DATA0~7 to IOA8~15*/
	    for (i=IO_A14 ; i<=IO_A15; i++)
	    {
	        gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_DATA_DRIVING);
	    }
	#endif


	#if XD_CTRL_POS==XD_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
	        for (i=IO_C12 ; i<=IO_C15; i++)
	        {
	            gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	        }
	#elif XD_CTRL_POS==XD_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9 
	        for (i=IO_C6 ; i<=IO_C9; i++)
	        {
	            gpio_drving_init_io(i,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	        }
	#elif XD_CTRL_POS==XD_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11  
	        gpio_drving_init_io(IO_G5,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	        gpio_drving_init_io(IO_G6,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	        gpio_drving_init_io(IO_G10,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	        gpio_drving_init_io(IO_G11,(IO_DRV_LEVEL) XD_CTRL_DRIVING);
	#endif
#endif //(defined _DRV_L1_XDC) && (_DRV_L1_XDC == 1)	
/* XD Pad Dispacher END */ 


}




/* This interface is for the application layer to initail the GPIO direction*/
/* init_io will not only modyfy the direction but also control the attribute value */
BOOLEAN gpio_init_io(INT32U port, BOOLEAN direction)
{
    INT16U gpio_set;
    INT16U gpio_set_num;
    //INT32U trace;
    
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    direction &= LOWEST_BIT_MASK;
    if (direction == GPIO_OUTPUT) {
        DRV_Reg32(IOA_ATTRIB_ADDR+(EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);  /*Set attribute to 0 for input */
        DRV_Reg32((IOA_DIR_ADDR+EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);
    }
    else if (direction == GPIO_INPUT) {
        DRV_Reg32(IOA_ATTRIB_ADDR+(EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);  /*Set attribute to 1 for output */
        DRV_Reg32((IOA_DIR_ADDR+EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);
    }
    else { return GPIO_FAIL; }
    
    return GPIO_OK;
}


BOOLEAN gpio_read_io(INT32U port)
{
#if 1
    if (DRV_Reg32(gpio_data_in_addr[gpio_set_array[port]])&(bit_mask_array[gpio_setbit_array[port]]))
    {return 1;}
    else 
    {return 0;}
#else
    INT16U gpio_set; 
    INT16U gpio_set_num;
    /*debug k*/
    //INT32U k;
    
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    //k = DRV_Reg32(IOA_DATA_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set) ;
    return ((DRV_Reg32(IOA_DATA_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set) >> gpio_set_num) & LOWEST_BIT_MASK);  
#endif

}

BOOLEAN gpio_write_io(INT32U port, BOOLEAN data)
{
#if 1
    if ((data&LOWEST_BIT_MASK))
    {
        DRV_Reg32(gpio_data_out_addr[gpio_set_array[port]]) |= bit_mask_array[gpio_setbit_array[port]];
    }
    else
    {
        DRV_Reg32(gpio_data_out_addr[gpio_set_array[port]]) &= bit_mask_inv_array[gpio_setbit_array[port]];
    }
    return GPIO_OK;
#else   

    INT16U gpio_set; 
    INT16U gpio_set_num;
    INT32U trace;
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;  // gpio_set_array
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS; // gpio_setbit_array
    
    data &= LOWEST_BIT_MASK;
    if (data == DATA_HIGH){    
        DRV_Reg32((IOA_BUFFER_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);
    }
    else if (data == DATA_LOW){
        DRV_Reg32((IOA_BUFFER_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);
    }
    else  {return GPIO_FAIL; }

    return GPIO_OK;

#endif
    return GPIO_OK;
}



BOOLEAN gpio_drving_init_io(GPIO_ENUM port, IO_DRV_LEVEL gpio_driving_level)
{

    INT16U gpio_set;
    INT16U gpio_set_num;
    INT32U drv_level;

    //INT32U trace;

    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    drv_level = (INT32U) gpio_driving_level;

    if ( (gpio_mcu_ver_get() >= 10) && (port < IO_D0) || (port > IO_D15 && port < IO_F0))  // GPL326XX ALL IO or GPL32_A/B/C IOA/B/C and E
    {
        if (drv_level == 0)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)+4) &= ~(1 << gpio_set_num);
        }
        else if (drv_level == 1)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)+4) &= ~(1 << gpio_set_num);
        }
        else if (drv_level == 2)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)+4) |= (1 << gpio_set_num);
        }
        else if (drv_level == 3)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)+4) |= (1 << gpio_set_num);
        }
    }
    else if ((port < IO_E0) || (port > IO_E15) && (port < IO_I0))  /* GPL32_A/B/C PortSet IOD/F/G/H */
    {
        if ((drv_level&0x1) == 0)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num*2);
        }
        else if ((drv_level&0x1) == 1)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num*2);
        }

        if ((drv_level&0x10) == 0x00)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) &= ~(1 << (gpio_set_num*2+1));
        }
        else if ((drv_level&0x10) == 0x10)
        {
            DRV_Reg32(IOA_DRV+(EACH_DIR_REG_OFFSET*gpio_set)) |= (1 << (gpio_set_num*2+1));
        }
    }
    else if((port >= IO_I0) && (port <= IO_I9))
    {
        if (drv_level == 0)
        {
            DRV_Reg32(IOI_DRV) &= ~(1 << gpio_set_num);
            DRV_Reg32(IOI_DRV+4) &= ~(1 << gpio_set_num);
        }
        else if (drv_level == 1)
        {
            DRV_Reg32(IOI_DRV) |= (1 << gpio_set_num);
            DRV_Reg32(IOI_DRV+4) &= ~(1 << gpio_set_num);
        }
        else if (drv_level == 2)
        {
            DRV_Reg32(IOI_DRV) &= ~(1 << gpio_set_num);
            DRV_Reg32(IOI_DRV+4) |= (1 << gpio_set_num);
        }
        else if (drv_level == 3)
        {
            DRV_Reg32(IOI_DRV) |= (1 << gpio_set_num);
            DRV_Reg32(IOI_DRV+4) |= (1 << gpio_set_num);
        }  	
    }
    else
    {
        return GPIO_FAIL;
    }
	return GPIO_OK;
}


void gpio_drving_uninit(void)
{
    R_IOA_DRV = 0;
    R_IOB_DRV = 0;
    R_IOC_DRV = 0;
    R_IOD_DRV = 0;
    R_IOE_DRV = 0;
    R_IOF_DRV = 0;
    R_IOG_DRV = 0;
    R_IOH_DRV = 0;
#if (MCU_VERSION >= GPL326XXB) && (MCU_VERSION < GPL327XX)
    R_IOI_DRV = 0;
#endif    
}



BOOLEAN gpio_set_port_attribute(INT32U port, BOOLEAN attribute)
{
    INT16U gpio_set;
    INT16U gpio_set_num;
    
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    attribute &= LOWEST_BIT_MASK;
    
    if((port >= IO_I0) && (port <= IO_I9))
    {
	    if (attribute == ATTRIBUTE_HIGH) {
	        DRV_Reg32(IOI_ATTRIB_ADDR) |= (1 << gpio_set_num);
	    }
	    else if (attribute == ATTRIBUTE_LOW) {
	        DRV_Reg32(IOI_ATTRIB_ADDR) &= ~(1 << gpio_set_num);
	    }
    }	
	else
	{    
	    if (attribute == ATTRIBUTE_HIGH) {
	        DRV_Reg32((IOA_ATTRIB_ADDR+EACH_ATTRIB_REG_OFFSET*gpio_set)) |= (1 << gpio_set_num);
	    }
	    else if (attribute == ATTRIBUTE_LOW) {
	        DRV_Reg32((IOA_ATTRIB_ADDR+EACH_ATTRIB_REG_OFFSET*gpio_set)) &= ~(1 << gpio_set_num);
	    }
	    else { return GPIO_FAIL; }
	}
    return GPIO_OK; 
}


/*INPUT : 
         gpio_set_num: GPIO_SET_A, GPIO_SET_B, GPIO_SET_C, GPIO_SET_D
         write_data: 0x00001234h 
*/

BOOLEAN gpio_write_all_by_set(INT32U gpio_set_num, INT32U write_data)
{
    if (gpio_set_num < GPIO_SET_MAX) {
        if((gpio_set_num >= IO_I0) && (gpio_set_num <= IO_I9))
        	DRV_WriteReg32(IOI_BUFFER_ADDR,(write_data&LOWEST_BIT_MASK));
        else
        	DRV_WriteReg32(IOA_BUFFER_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set_num,(write_data&LOWEST_BIT_MASK));
        return GPIO_OK;   
    }
    else {
        return GPIO_FAIL; 
    }
}

INT32U gpio_read_all_by_set(INT8U gpio_set_num)
{
    if (gpio_set_num < GPIO_SET_MAX) {
        if((gpio_set_num >= IO_I0) && (gpio_set_num <= IO_I9))
	        return DRV_Reg32(IOI_DATA_ADDR);   
        else
	        return DRV_Reg32(IOA_DATA_ADDR+EACH_GPIO_DATA_REG_OFFSET*gpio_set_num);   
    }
        return 0xFFFFFFFF;
      
}

BOOLEAN gpio_get_dir(INT32U port)
{
    INT16U gpio_set; 
    INT16U gpio_set_num;
    
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    
    if((port >= IO_I0) && (port <= IO_I9))
    	return ((DRV_Reg32(IOA_DIR_ADDR) >> gpio_set_num) & LOWEST_BIT_MASK);  
    else
    	return ((DRV_Reg32(IOA_DIR_ADDR+EACH_DIR_REG_OFFSET*gpio_set) >> gpio_set_num) & LOWEST_BIT_MASK);  
}


BOOLEAN gpio_set_memcs(INT8U cs_port,INT8U cs_io)
{
    INT32U memset;
    memset = 1 << (cs_port +11);  
    if (cs_io) //cs
    {
      R_MEM_IO_CTRL |= memset;
    }
    else       //io
    {
      R_MEM_IO_CTRL &= ~memset;
    }
    return 0;
}

void gpio_set_bkueb(BOOLEAN status)
{
	if (status) {
	  R_MEM_IO_CTRL |= 1; /* enable */
	}
	else  {
	  R_MEM_IO_CTRL &= ~1; /* disable */
	}	
}

BOOLEAN gpio_set_sdramio(void)
{
#if ((BOARD_TYPE == BOARD_TK_V4_1) || (BOARD_TYPE == BOARD_GPY0200_EMU_V2_0) || (BOARD_TYPE == BOARD_TK35_V1_0) || (BOARD_TYPE == BOARD_TK_A_V2_0) || (BOARD_TYPE == BOARD_TK_A_V2_1_5KEY) || (BOARD_TYPE == BOARD_TK_7D_V1_0_7KEY) || BOARD_TYPE == BOARD_TK_8D_V1_0 || BOARD_TYPE == BOARD_TK_8D_V2_0)
	R_MEM_IO_CTRL &= ~0x6; /* XA19(IOG6) XA20(IOG5) */
#elif BOARD_TYPE == BOARD_TK_V4_0
  	R_MEM_IO_CTRL &= ~0x78; /* XA24(IOG1) XA23(IOG2) XA22(IOG3) XA21(IOG4) */
#elif (BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE == BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_V4_4)
 	R_MEM_IO_CTRL &= ~0x2E; /* XA23(IOG2) XA21(IOG4) XA20(IOG5) XA19(IOG6) */	 
#elif BOARD_TYPE < BOARD_TURNKEY_BASE
	R_MEM_IO_CTRL &= ~0x66; /* XA24(IOG1) XA23(IOG2) XA20(IOG5) XA19(IOG6) */
#endif

  return 0;
}

void gpio_set_ice_en(BOOLEAN status)
{
	if (status == TRUE) {
	  R_GPIOCTRL |= 1; /* enable */
	}
	else  {
	  R_GPIOCTRL &= ~1; /* disable */
	}	
}


#if 0
static BOOLEAN gpio_get_attrib(INT32U port)
{
    INT16U gpio_set; 
    INT16U gpio_set_num;
    
    gpio_set = port / EACH_REGISTER_GPIO_NUMS;
    gpio_set_num = port % EACH_REGISTER_GPIO_NUMS;
    
    return ((DRV_Reg32(IOA_ATTRIB_ADDR+EACH_ATTRIB_REG_OFFSET*gpio_set) >> gpio_set_num) & LOWEST_BIT_MASK);  
}
#endif

#ifndef _GPIO_MONITOR_DEF_
#define _GPIO_MONITOR_DEF_
typedef struct {
    INT8U KS_OUT74_EN   ;
    INT8U KS_OUT31_EN   ;
    INT8U KS_OUT0_EN    ;
    INT8U KS_EN         ;
    INT8U TS_all_en     ;
    INT8U SEN_EN        ;
    INT8U KEYN_IN2_EN   ;
    INT8U KSH_EN        ;
    INT8U ND_SHARE_EN   ;
    INT8U NAND_EN       ;
    INT8U UART_EN       ;
    INT8U TFTEN         ;
    INT8U TFT_MODE1     ;
    INT8U TFT_MODE0     ;
    INT8U EN_CF         ;
    INT8U EN_MS         ;
    INT8U EN_SD123      ;
    INT8U EN_SD         ;
    INT8U USEEXTB       ;
    INT8U USEEXTA       ;
    INT8U SPI1_EN       ;
    INT8U SPI0_EN       ;
    INT8U CCP_EN2       ;
    INT8U CCP_EN1       ;
    INT8U CCP_EN0       ;
    INT8U KEYC_EN       ;
    INT8U CS3_0_EN_i3   ;
    INT8U CS3_0_EN_i2   ;
    INT8U CS3_0_EN_i1   ;
    INT8U CS3_0_EN_i0   ;
    INT8U XD31_16_EN    ;
    INT8U BKOEB_EN      ;
    INT8U BKUBEB_EN     ;
    INT8U XA24_19EN3    ;
    INT8U XA24_19EN2    ;
    INT8U XA24_19EN1    ;
    INT8U XA24_19EN0    ;
    INT8U SDRAM_CKE_EN  ;
    INT8U SDRAM_CSB_EN  ;
    INT8U XA24_19EN5    ;
    INT8U XA24_19EN4    ;
    INT8U XA14_12_EN2   ;
    INT8U XA14_12_EN1   ;
    INT8U XA14_12_EN0   ;
    INT8U EPPEN         ;
    INT8U MONI_EN       ;
} GPIO_MONITOR;

#endif  //_GPIO_MONITOR_DEF_

void gpio_monitor(GPIO_MONITOR *gpio_monitor)
{
    INT8U smoni0=R_SMONI0;
    INT8U smoni1=R_SMONI1;
    
    gpio_monitor->KS_OUT74_EN   = (smoni0>>3  ) & 0x1 ;        
    gpio_monitor->KS_OUT31_EN   = (smoni0>>2  ) & 0x1 ;        
    gpio_monitor->KS_OUT0_EN    = (smoni0>>1  ) & 0x1 ;        
    gpio_monitor->KS_EN         = (smoni0>>0  ) & 0x1 ;        
    gpio_monitor->TS_all_en     = (smoni0>>7  ) & 0x1 ;        
    gpio_monitor->SEN_EN        = (smoni0>>6  ) & 0x1 ;        
    gpio_monitor->KEYN_IN2_EN   = (smoni0>>5  ) & 0x1 ;        
    gpio_monitor->KSH_EN        = (smoni0>>4  ) & 0x1 ;        
    gpio_monitor->ND_SHARE_EN   = (smoni0>>10  ) & 0x1 ;        
    gpio_monitor->NAND_EN       = (smoni0>>9  ) & 0x1 ;        
    gpio_monitor->UART_EN       = (smoni0>>8  ) & 0x1 ;        
    gpio_monitor->TFTEN         = (smoni0>>14  ) & 0x1 ;        
    gpio_monitor->TFT_MODE1     = (smoni0>>13  ) & 0x1 ;        
    gpio_monitor->TFT_MODE0     = (smoni0>>12  ) & 0x1 ;        
    gpio_monitor->EN_CF         = (smoni0>>19  ) & 0x1 ;        
    gpio_monitor->EN_MS         = (smoni0>>18  ) & 0x1 ;        
    gpio_monitor->EN_SD123      = (smoni0>>17  ) & 0x1 ;        
    gpio_monitor->EN_SD         = (smoni0>>16  ) & 0x1 ;        
    gpio_monitor->USEEXTB       = (smoni0>>23  ) & 0x1 ;        
    gpio_monitor->USEEXTA       = (smoni0>>22  ) & 0x1 ;        
    gpio_monitor->SPI1_EN       = (smoni0>>21  ) & 0x1 ;        
    gpio_monitor->SPI0_EN       = (smoni0>>20  ) & 0x1 ;        
    gpio_monitor->CCP_EN2       = (smoni0>>27  ) & 0x1 ;        
    gpio_monitor->CCP_EN1       = (smoni0>>26  ) & 0x1 ;        
    gpio_monitor->CCP_EN0       = (smoni0>>25  ) & 0x1 ;        
    gpio_monitor->KEYC_EN       = (smoni0>>24  ) & 0x1 ;        
    
    gpio_monitor->CS3_0_EN_i3   = (smoni1>>3  ) & 0x1 ;
    gpio_monitor->CS3_0_EN_i2   = (smoni1>>2  ) & 0x1 ;
    gpio_monitor->CS3_0_EN_i1   = (smoni1>>1  ) & 0x1 ;
    gpio_monitor->CS3_0_EN_i0   = (smoni1>>0  ) & 0x1 ; 
    gpio_monitor->XD31_16_EN    = (smoni1>>6  ) & 0x1 ; 
    gpio_monitor->BKOEB_EN      = (smoni1>>5  ) & 0x1 ; 
    gpio_monitor->BKUBEB_EN     = (smoni1>>4  ) & 0x1 ; 
    gpio_monitor->XA24_19EN3    = (smoni1>>11  ) & 0x1 ;  
    gpio_monitor->XA24_19EN2    = (smoni1>>10  ) & 0x1 ;  
    gpio_monitor->XA24_19EN1    = (smoni1>>9  ) & 0x1 ;  
    gpio_monitor->XA24_19EN0    = (smoni1>>8  ) & 0x1 ; 
    gpio_monitor->SDRAM_CKE_EN  = (smoni1>>15  ) & 0x1 ;  
    gpio_monitor->SDRAM_CSB_EN  = (smoni1>>14  ) & 0x1 ;  
    gpio_monitor->XA24_19EN5    = (smoni1>>13  ) & 0x1 ;  
    gpio_monitor->XA24_19EN4    = (smoni1>>12  ) & 0x1 ; 
    gpio_monitor->XA14_12_EN2   = (smoni1>>18  ) & 0x1 ;
    gpio_monitor->XA14_12_EN1   = (smoni1>>17  ) & 0x1 ;
    gpio_monitor->XA14_12_EN0   = (smoni1>>16  ) & 0x1 ; 
    gpio_monitor->EPPEN         = (smoni1>>20  ) & 0x1 ; 
    gpio_monitor->MONI_EN       = (smoni1>>24  ) & 0x1 ;     
}


void nand_xd_swap_to(NAND_XD_ENUM swap_to_id)
{
#if (_DRV_L1_NAND==1) && (_DRV_L1_XDC==1)
    INT32U funpos0=R_FUNPOS0;
    INT32U iosrsel=R_IOSRSEL;
    INT32U sysmonictrl=R_SYSMONICTRL;

    if (swap_to_id == ID_XD) 
    {
        /* Step1. 將 IOF1(CS1) IOF2(CS2) 以 gpio output 形式 pull high */
      #if (NAND_CS_POS == NF_CS_AS_BKCS1) || (XD_CS_POS == NF_CS_AS_BKCS1)
        R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS1;
      #endif
      #if (NAND_CS_POS == NF_CS_AS_BKCS2) || (XD_CS_POS == NF_CS_AS_BKCS2)
        R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS2;
      #endif
      #if (NAND_CS_POS == NF_CS_AS_BKCS3) || (XD_CS_POS == NF_CS_AS_BKCS3)
        R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS3;
      #endif
        /* Step2. Nand CTRL 設為 0 */
        R_NF_CTRL = 0;
        /* Step3. 將 Nand CS1 移到 NULL CS */
        funpos0 |= (1<<5); funpos0 |= (1<<4);  /* 設到 NULL CS */
        /* Step4. 將 ICNand 移到 XD 所指定之 nand pad */  // 此一步驟在 share 與 non-share 情況下是不用下的
      #if XD_DATA5_0_POS==XD_DATA5_0_AS_IOD5_0
        funpos0 &= ~(1<<9);  funpos0 |= (1<<8);   /*Nand Data Pin  設到 IOD0~5 */
      #elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOE5_0
        funpos0 |= (1<<9);  funpos0 &= ~(1<<8);   /*Nand Data Pin  設到 IOE0~5 */
      #elif XD_DATA5_0_POS==XD_DATA5_0_AS_IOB13_8
        funpos0 &= ~(1<<9|1<<8);                  /*Nand Data Pin  設到 IOB8~13 */
      #elif  XD_DATA5_0_POS==XD_DATA5_0_AS_IOA13_8   /*Nand Data Pin  設到 IOA8~13 */
        sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
      #endif

      #if XD_DATA7_6_POS==XD_DATA7_6_AS_IOD7_6
        funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h1)*/
      #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOE7_6
        funpos0 |= (1<<7);  funpos0 &= ~(1<<6);
      #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOB15_14
        funpos0 &= ~(1<<7|1<<6);  
      #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOC5_4
        funpos0 &= ~(1<<7);  funpos0 |= (1<<6); 
      #elif XD_DATA7_6_POS==XD_DATA7_6_AS_IOA15_14
        sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
        #if XD_DATA5_0_POS != XD_DATA5_0_AS_IOA13_8
            #error  XD DATA0~7 must ALL in IO8~15  
        #endif
      #endif


      
      #if XD_CTRL_POS==XD_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9 
        funpos0 |= (1<<3);  iosrsel &= ~(1<<14);  /*Nand CTRL Pin 設到 IOC6~9 */
      #elif XD_CTRL_POS==XD_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
        funpos0 &= ~(1<<3); iosrsel &= ~(1<<14);  /*Nand CTRL Pin 設到 IOC12~15 */
      #endif
        /*Nand CTRL Pin設到 IOC6~9 */
        /* Step5. IO PAD 生效*/
        R_FUNPOS0 = funpos0;
        R_IOSRSEL = iosrsel;
        /* Step5.1 若又 bug 再設一次 gpio 以防被拉 low */  
        /* Step6. 將 XD Ctrl 設回來準備運作 */
        R_NF_CTRL = 0x1157;   /* 若有 bug 就不要一次設回, 將 pad bit[13] 設對就好*/		//04/10
        //DBG_PRINT ("==>Swap to XD OK<==\r\n");
    } 
    else if (swap_to_id == ID_NAND) 
    {
        if (xdc_nand_mode_get()==XD_ONLY_MODE)
        {
            R_NF_CTRL = 0x0;  /*關掉Nand*/ 
        }
        else 
        {
            /* Step1. 將 IOF1(CS1), IOF2(CS2) 以 gpio output 形式 pull high */
            #if (NAND_CS_POS == NF_CS_AS_BKCS1) || (XD_CS_POS == NF_CS_AS_BKCS1)
              R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS1;
            #endif
            #if (NAND_CS_POS == NF_CS_AS_BKCS2) || (XD_CS_POS == NF_CS_AS_BKCS2)
              R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS2;
            #endif
            #if (NAND_CS_POS == NF_CS_AS_BKCS3) || (XD_CS_POS == NF_CS_AS_BKCS3)
              R_IOF_O_DATA |= (NAND_CS_ENUM) NAND_CS3;
            #endif
            /* Step2. Nand CTRL 設為 0 */
            R_NF_CTRL = 0;
            /* Step3. 將 Nand CS 移到 CS1 */
            #if NAND_CS_POS == NF_CS_AS_BKCS1
              funpos0 &= ~(1<<5); funpos0 &= ~(1<<4); /* Nand-CS 設到 CS1 */
            #elif NAND_CS_POS == NF_CS_AS_BKCS2
              funpos0 &= ~(1<<5); funpos0 |= (1<<4);  /* Nand-CS 設到 CS1 */
            #elif NAND_CS_POS == NF_CS_AS_BKCS3
              funpos0 |= (1<<5); funpos0 &= ~(1<<4);  /* Nand-CS 設到 CS1 */
            #endif
    
            /* Step4. 將 ICNand 移到 NAND 所指定之 nand pad */
            #if NAND_SHARE_MODE == NF_NON_SHARE  /* 只有 share mode 有需要調回pad的需求 */
                #if NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOD5_0
                  funpos0 &= ~(1<<9);  funpos0 |= (1<<8);   /*Nand Data Pin  設到 IOD0~5 */
                #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOE5_0
                  funpos0 |= (1<<9);  funpos0 &= ~(1<<8);   /*Nand Data Pin  設到 IOE0~5 */
                #elif NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOB13_8
                  funpos0 &= ~(1<<9|1<<8);                  /*Nand Data Pin  設到 IOB8~13 */
                #elif  NAND_DATA5_0_POS==NAND_DATA5_0_AS_IOA13_8   /*Nand Data Pin  設到 IOA8~13 */
                  sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
                #endif
          
                #if NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOD7_6
                  funpos0 &= ~(1<<7);  funpos0 |= (1<<6); /*((Fun_POS[7:6] == 2'h1)*/
                #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOE7_6
                  funpos0 |= (1<<7);  funpos0 &= ~(1<<6);
                #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOB15_14
                  funpos0 &= ~(1<<7|1<<6);  
                #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOC5_4
                  funpos0 &= ~(1<<7);  funpos0 |= (1<<6); 
                #elif NAND_DATA7_6_POS==NAND_DATA7_6_AS_IOA15_14
                  sysmonictrl |= IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15;
                  #if NAND_DATA5_0_POS != NAND_DATA5_0_AS_IOA13_8
                      #error  XD DATA0~7 must ALL in IO8~15  
                  #endif
                #endif
          
                #if NAND_CTRL_POS==NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9
                  #if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX) 
                  funpos0 |= (1<<3);  iosrsel &= ~(1<<14);  /*Nand CTRL Pin 設到 IOC6~9 */
                  #else
                  iosrsel |= 1<<14;
                  #endif
                #elif NAND_CTRL_POS==NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15
                  funpos0 &= ~(1<<3); iosrsel &= ~(1<<14);  /*Nand CTRL Pin 設到 IOC12~15 */
                #endif
            #endif
            /* Step5. IO PAD 生效*/
            R_FUNPOS0 = funpos0;
            /* Step6. 將 Nand CTRL 設回來準備運作 */
            R_NF_CTRL = 0x3131;   /* 若有 bug 就不要一次設回, 將 pad bit[13] 設對就好*/		//Daniel 02/24
            //DBG_PRINT ("==>Swap to NAND OK<==\r\n");
            R_SYSMONICTRL = sysmonictrl;
        }
    }  
#endif //(_DRV_L1_NAND==1) && (_DRV_L1_XDC==1) 
        //R_NF_CTRL = 0;   
}

void sd_io_swap(INT32U idx)
{
#if((SD_DUAL_SUPPORT==1)&&(MCU_VERSION>=GPL326XX_C)&&(MCU_VERSION<GPL327XX))
	/* if idx is 0, set SD pin to IOC 4~9, else set SD pin to IOB 8~13 */
	if(idx)	{
		R_FUNPOS2 |= 0x200;
	} else {
		R_FUNPOS2 &= ~0x200;
	}
#elif ((SD_DUAL_SUPPORT==1)&&(MCU_VERSION>=GPL327XX))
	/* if idx is 0, set SD pin to IOC 4~9, else set SD pin to IOD 8~13 */
	if(idx)	{
		R_IOSRSEL |= (1<<11);
	} else {
		R_IOSRSEL &= ~(1<<11);
	}
#endif
}

static INT8U gpio_mcu_ver_get(void)
{
	INT32U value;

	value = *((INT32U *) 0xF0001E00);
	if (value == 0x00000012) {			// GPL322xx Version A
		return 1;
	} else if (value == 0xE2811002) {	// GPL322xx Version B
		return 2;
	} else if (value == 0xE2811003) {	// GPL322xx Version C
		return 3;
	} else if (value == 0x826959) { 	// GPL326xx Version A
		return 0x10;
	} else if (value == 0x68D13A20) {
        return 0x11;  					// GPL326xx Version B
	}else if (value == 0x1C211C22) {
        return 0x12;  					// GPL326xx Version C
	}

	return 255;
}





//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_GPIO) && (_DRV_L1_GPIO == 1)            //
//================================================================//
