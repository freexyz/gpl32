#ifndef __DRIVER_L1_CFG_H__
#define __DRIVER_L1_CFG_H__

//=========================Engineer Mode ("Not" defined CUSTOMER_DEFINE)=====================//
#ifndef CUSTOMER_DEFINE  //For Engineer Development and test                                 //
//===========================================================================================//
    
    #define INIT_MCLK                   96000000
    #define INIT_MHZ                    (INIT_MCLK/1000000)
    
	#define PLL_24MHz					24000000
	#define PLL_27MHz					27000000
	#define PLL_SRC_CLK					PLL_24MHz 
	
	#define SDRAM_SIZE 					SD_256Mb
    #define SDRAM_START_ADDR            0x00000000
    #define SDRAM_END_ADDR              0x01FFFFFF

    #define ISRAM_START_ADDR            0xF8000000
    #define ISRAM_END_ADDR              0xF8000000  
    #define IRAM_SWAITCH_FLAG_START     0xF8000000
    #define IRAM_SWAITCH_FLAG_END       0xF8000000
	
    // MCU configuration
    #define _DRV_L1_ADC                 1
    #define _DRV_L1_CACHE               1
    #define _DRV_L1_DAC                 1
    #define _DRV_L1_DMA                 1
    #define _DRV_L1_EXT_INT             1
    #define _DRV_L1_GPIO                1
    #define _DRV_L1_GTE                 1
    #define _DRV_L1_INTERRUPT           1
    #define _DRV_L1_JPEG                1
    #define _DRV_L1_MEMC                1
    #define _DRV_L1_RTC                 1
    #define _DRV_L1_SCALER              1
    #define _DRV_L1_SDC                 1
    #define _DRV_L1_SENSOR              1
    #define _DRV_L1_SPI                 1
    #define _DRV_L1_SW_TIMER            1
    #if _DRV_L1_SW_TIMER==1
    	#define DRV_L1_SW_TIMER_SOURCE  	TIMER_RTC
        #define DRV_L1_SW_TIMER_Hz      	1024
    #endif
    #define _DRV_L1_SYSTEM              1
    #define _DRV_L1_TFT                 1
    #define _DRV_L1_TIMER               1
    #define _DRV_L1_UART                1
    #define _DRV_L1_USBH                1
    #define _DRV_L1_USBD                1
    #define _DRV_L1_WATCHDOG            1
    #define _DRV_L1_XDC                 0
    #if SUPPORT_GPY02XX != GPY02XX_NONE
        #define _DRV_L1_GPY0200             1
    #else
        #define _DRV_L1_GPY0200             0
    #endif

	#define _DRV_L1_SPU                 0
	#define _DRV_L1_PPU                 0
	#define _DRV_L1_DEFLICKER			0
	#define _DRV_L1_NOR                 0
	#define _DRV_L1_NAND                0
	#define _DRV_L1_CFC                 0
	#define _DRV_L1_MSC                 0
	#define _DRV_L1_KEYSCAN             0
	#define _DRV_L1_TV                  0
	#define _DRV_L1_MIC					1
	#define _DRV_L1_MPEG4				0
	#define _DRV_L1_MIPI				0
	
    // UART interface config
    #define UART0_BAUD_RATE             115200

    /* free customer define */
    #define DPF_1024x768            0
    #define DPF_800x600             0
    #define DPF_800x480             0
    #define DPF_640x480             0
    #define DPF_480x272             0
    #define DPF_480x234             0
    #define DPF_320x240             0
    #define DPF_720x480             0
    #define DPF_400x240             0
    #define DPF_160x128             0
    #define DPF_H_V                 0

    // Display device definition
    #define AUO_M150XN07                0x0000  //1024x768
    #define AUO_A080SN01            	0x1000  // 800x600                              //
    #define INNOLUX_AT080TN42       	0x1001  // 800x600                               //
    #define INNOLUX_AT080TN52       	0x1002  // 800x600                              //
    #define CPT_CLAA070MA0ACW			0x1003  // 800x600
    #define CHIMEI_Q08009_701           0x1004  // 800x600
    #define INNOLUX_AT080TN03       	0x2000  // 800x480                               //
    #define SAMSUNG_LTP700WVF01     	0x2001  // 800x480                               //
    #define AUO_A102VW01            	0x2002  // 800x480                               //
    #define TPO_TD070WGCB2          	0x2003  // 800x480                               //
    #define AUO_C080VW02            	0x2004  // 800x480                               //
    #define AUO_C070VW02            	0x2005  // 800x480                               //
    #define HSD070IDW1              	0x2006  // 800x480                               //
    #define AUO_A070VW02            	0x2007  // 800x480                               //
    #define CPT_CLAA070LD0ACW       	0x2008  // 800x480                               //
    #define INNOLUX_AT070TN92			0x2009	// 800x480
    #define LGP_LB064V02            	0x3000  // 640x480
    #define WINTEK_WDF4827Y         	0x4000  // 480x272
    #define ANALOG_PANEL_TYPE_1     	0x5000  // 480x234                               //
    #define AUO_C065GW02            	0x5001  // 480x234
   	#define TPO_TD035TTEA1          	0x6000  // 320x240                               //
    #define INNOLUX_AT056TN03       	0x6001  // 320x240                                //
    #define PMV_035BG               	0x6002  // 320x240                                 //
    #define GPM879A0                	0x6003  // 320x240                                 //
    #define AUO_A024CN02V9          	0x6004  // 320x240                                 //
    #define TPO_TD025THEA7          	0x6005  // 320x240                                 //
    #define AUO_A035QN02            	0x6006  // 320x240                                 //
    #define GP_GPM758A0S            	0x6007  // 320x240                                 //
    #define CHILIN_LQ035NC111       	0x6008  // 320x240                                 //
    #define TIANMA_TM035KDH03       	0x6009  // 320x240                                 //
    #define AUO_A040CN01            	0x600A  // 320x240
    #define TPO_TD025THD1	          	0x600B  // 320x240                                 //
    #define FOX_FL320WQC11	          	0x7000  // 400x240 
    #define GPM_LM765A0		          	0x8000  // 160x128
    #define GPM_LM765H0		          	0x8001  // 160x128                                  //
	#define C_TV_QVGA                   0xE250	// 320x240
    #define C_TV_VGA                    0xE251	// 640x480
    #define C_TV_D1                     0xE252  // 720x480

    #define C_DISPLAY_DEVICE            TPO_TD025THD1          	// Select one of the display device from above list
    #define C_DISPLAY_DEV_HPIXEL        320                     // Set display device horizontal pixel number
    #define C_DISPLAY_DEV_VPIXEL        240                     // Set display device vertical pixel number

    // TFT Back Light Configuration
    #define LCD_BACKLIGHT_LED           0
    #define LCD_BACKLIGHT_CCFL          1
    #define LCD_BACKLIGHT_BUILTIN       2
    #define LCD_BACKLIGHT_MODE          LCD_BACKLIGHT_BUILTIN
    #define PWM0_VGHL_EN                FALSE
    #if LCD_BACKLIGHT_MODE == LCD_BACKLIGHT_LED
    #define PWM1_BACKLIGHT_EN           TRUE
    #else
    #define PWM1_BACKLIGHT_EN           FALSE
    #endif
   
   	// Select CMOS Sensor Type
	//#define		__TV_VGA__
	#define	__TV_QVGA__
	
	//#define	__HSB0357_DRV_C__
	//#define	__OV6680_DRV_C__
	//#define	__OV7680_DRV_C__
	#define	__OV7670_DRV_C__
	//#define	__OV9655_DRV_C__
	//#define	__OV9660_DRV_C__
	//#define	__OV2655_DRV_C__ 
	//#define	__OV3640_DRV_C__ 
	//#define	__OV5642_DRV_C__ 
	//#define	__OV7675_DRV_C__ 
	//#define	__OV2643_DRV_C__ 
	//#define	__OID_SENSOR_DRV_C__ 
	//#define	__BF3710_DRV_C__ 
	//#define	__OV3640_MIPI_DRV_C__	
   
   	#if (defined MCU_VERSION) && (MCU_VERSION < GPL326XX)
		#define CSI_IRQ_MODE			CSI_IRQ_PPU_IRQ 
	#elif (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)   
		#define CSI_IRQ_MODE			CSI_IRQ_PPU_IRQ
	#elif (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)
		#define CSI_IRQ_MODE			CSI_IRQ_TG_IRQ
	#endif
    /*--- Engineer Mode Pad Define ---*/
    #ifndef __GPIO_PAD_DEFINE__
    #define __GPIO_PAD_DEFINE__
         /* Share Pin Config*/
         /*Position Config star*/
         #define NAND_SHARE_MODE             NF_SHARE_MODE

         #define NAND_CS_POS                 NF_CS_AS_BKCS1
         #define NAND_CS_DRIVING             IO_DRIVING_4mA

         #define NAND_PAGE_TYPE              NAND_LARGEBLK
         #define NAND_WP_IO                  NAND_WP_PIN	//IO_E9

         #define NAND_CTRL_POS               NF_ALE_SHARE_CLE_SHARE_REB_SHARE__WEB_SHARE
         #define NAND_CTRL_DRIVING           IO_DRIVING_4mA

          /* only effect in nand "non-shard with SDRAM" MODE */
         #define NAND_DATA5_0_POS            NAND_DATA5_0_AS_IOD5_0
         #define NAND_DATA7_6_POS            NAND_DATA7_6_AS_IOD7_6
         #define NAND_DATA_DRIVING           IO_DRIVING_4mA


         #define UART_TX_RX_POS              UART_TX_IOF7__RX_NONE//UART_TX_IOF7__RX_IOF8
         #define UART_TX_RX_DRIVING          IO_DRIVING_4mA

         #define TFT_HSYNC_VSYNC_CLK_POS     TFT_HSYNC_VSYNC_CLK_at_IOB1_IOB2_IOB3
         #define TFT_HSYNC_VSYNC_CLK_DRIVING IO_DRIVING_8mA /* only 4mA/8mA can choice */
         #define TFT_POL_STV_STH             TFT_POL_IOB4__STV_IOB5__STH_IOB6
         #define TFT_POL_STV_STH_DRIVING     IO_DRIVING_4mA /* MAX 16mA (IOG9, other max is 8mA) */
         #define TFT_DATA0_7_DRIVING         IO_DRIVING_4mA
         #define TFT_DATA8_15_DRIVING        IO_DRIVING_4mA
         #define TFT_R0G0B0_R1G1B1_DRIVING   IO_DRIVING_4mA
         #define TFT_R2B2_DRIVING            IO_DRIVING_4mA

         #define CMOS_POS                    CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC11_IOH0
         #define CMOS_DRIVING                IO_DRIVING_4mA

         #define CF_DATA0_15_POS             CF_DATA0_15_at_IOD0_15
         #define CF_DATA0_15_DRIVING         IO_DRIVING_4mA /* IOE only 4mA/8mA can choice, IOD: 4mA/8mA/12mA/16mA can choice*/
         #define CF_A0_A1_IORDB_DRVING       IO_DRIVING_4mA
         #define CF_CS0_CS2_IORDY_DRVING     IO_DRIVING_4mA
		
		 #define SD_DUAL_SUPPORT			 1
         #define SD_POS                      SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9
         #define SD_DRIVING                  IO_DRIVING_8mA /* IOC only 4mA/8mA can choice, IOD: 4mA/8mA/12mA/16mA can choice*/

         #define SPI1_POS                    SPI1_RX_NONE__CLK_NONE__TX_NONE
         #define SPI1_DRIVING                IO_DRIVING_4mA  /* 4mA/8mA can choice */
         #define SPI0_POS                    SPI0_RX_IOC3__CLK_IOC1__TX_IOC2
         #define SPI0_DRIVING                IO_DRIVING_4mA  /* 4mA/8mA can choice */

         #define IOG5_UOT_PCLK_DIV6          0   /* 0: disable  1: enable*/
         #define IOG5_UOT_PCLK_DIV6_DRIVING  IO_DRIVING_4mA  /* 4mA/8mA/12mA/16mA can choice */
         #define KEY_CHANGE_B_POS            KEY_CHANGE_B_AT_IOF3

         #define XA22_XA21_POS               XA22_IOG3__XA21_IOG4

         #define BKCS_0_EN                   FALSE
         #define BKCS_0_DRIVING              IO_DRIVING_4mA

         #define BKCS_1_EN                   TRUE
         #define BKCS_1_DRIVING              IO_DRIVING_4mA

         #define BKCS_2_EN                   TRUE
         #define BKCS_2_DRIVING              IO_DRIVING_4mA

         #define BKCS_3_EN                   TRUE
         #define BKCS_3_DRIVING              IO_DRIVING_4mA

         #define TIMER_A_PWM_EN              FALSE
         #define TIMER_A_PWM_DRIVING         IO_DRIVING_4mA /*4mA/8mA/12mA/16mA can choice*/

         #define TIMER_B_PWM_EN              FALSE
         #define TIMER_B_PWM_DRIVING         IO_DRIVING_4mA /*4mA/8mA/12mA/16mA can choice*/

         #define TIMER_C_PWM_EN              FALSE
         #define TIMER_C_PWM_DRIVING         IO_DRIVING_CANNOT_SET
    #endif  //__GPIO_PAD_DEFINE__
//======================== Engineer Mode END ================================================//
#else                              //Engineer Difne area END                                 //
//===========================================================================================//

//========================= CUSTOMER DEFINE END =============================================//
#endif  // not CUSTOMER_DEFINE                                                               //
//========================= CUSTOMER DEFINE END =============================================//


//=== This is for code configuration DON'T REMOVE or MODIFY it =============================//
#ifdef __GPIO_PAD_DEFINE__          //Don't modify start                                    //
//==========================================================================================//

    #if    MCU_VERSION == GPL32_B
    #define KEYCHANGE_POS               KEYCHANGE_at_IOE3
    #elif  MCU_VERSION == GPL32_A
    #define KEYCHANGE_POS               KEYCHANGE_at_IOE1
    #endif

    // GPIO PAD
    #define NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15    0x00000000
    #define NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9        0x00000008
    #define NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11      0x10000008
    #define NF_ALE_SHARE_CLE_SHARE_REB_SHARE__WEB_SHARE		 0x80000000
    #define NF_CS_AS_BKCS1                  0x00000000
    #define NF_CS_AS_BKCS2                  0x00000010
    #define NF_CS_AS_BKCS3                  0x00000020

    #define NAND_LARGEBLK	 				0x00000000
    #define NAND_SMALLBLK  					0x00000001
    #define NAND_WP_PIN_NONE                0xFF
	#define NAND_WP_PIN						35


    #define NAND_DATA7_6_AS_IOB15_14        0x00000000
    #define NAND_DATA7_6_AS_IOD7_6          0x00000040
    #define NAND_DATA7_6_AS_IOE7_6          0x00000080
    #define NAND_DATA7_6_AS_IOC5_4          0x000000C0
    #define NAND_DATA5_0_AS_IOB13_8         0x00000000
    #define NAND_DATA5_0_AS_IOD5_0          0x00000100
    #define NAND_DATA5_0_AS_IOE5_0          0x00000200
    #define NAND_DATA7_0_AS_IOA15_8         0x000003C0               /* code-inter define */
    #define NAND_DATA7_6_AS_IOA15_14        NAND_DATA7_0_AS_IOA15_8  /* code-inter define */
    #define NAND_DATA5_0_AS_IOA13_8         NAND_DATA7_0_AS_IOA15_8  /* code-inter define */
    #define NAND_POSFUN_MASK                0x000003F8
    #define NF_SHARE_MODE  0
    #define NF_NON_SHARE   1
    #define NAND_POS_SET_VALUE  ((NAND_CS_POS|NAND_CTRL_POS|NAND_DATA5_0_POS|NAND_DATA7_6_POS)&NAND_POSFUN_MASK)

    // XD
    #define XD_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15    0x00000000
    #define XD_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9        0x00000008
    #define XD_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11      0x10000008
    //#define XD_CS_AS_BKCS1                  0x00000000
    //#define XD_CS_AS_BKCS2                  0x00000010
    //#define XD_CS_AS_BKCS3                  0x00000020

    #define XD_LARGEBLK	 				  0x00000000
    #define XD_SMALLBLK  				  0x00000001
    #define XD_WP_PIN_NONE                0xFF

    #define XD_DATA7_6_AS_IOB15_14        0x00000000
    #define XD_DATA7_6_AS_IOD7_6          0x00000040
    #define XD_DATA7_6_AS_IOE7_6          0x00000080
    #define XD_DATA7_6_AS_IOC5_4          0x000000C0
    #define XD_DATA5_0_AS_IOB13_8         0x00000000
    #define XD_DATA5_0_AS_IOD5_0          0x00000100
    #define XD_DATA5_0_AS_IOE5_0          0x00000200
    #define XD_DATA7_0_AS_IOA15_8         0x000003C0               /* code-inter define */
    #define XD_DATA7_6_AS_IOA15_14        XD_DATA7_0_AS_IOA15_8  /* code-inter define */
    #define XD_DATA5_0_AS_IOA13_8         XD_DATA7_0_AS_IOA15_8  /* code-inter define */

    // GPIO PAD_SETS DRIVING
    #define IO_DRIVING_CANNOT_SET  0  /* fix in 4mA */
    #define IO_DRIVING_4mA         0
    #define IO_DRIVING_8mA         1
    #define IO_DRIVING_12mA        2
    #define IO_DRIVING_16mA        3

    //SPI1
    #define SPI1_RX_IOE3__CLK_IOE1__TX_IOE2 ~(1<<12) /* GPL325XX */
    #define SPI1_RX_IOC4__CLK_IOC5__TX_IOC7  (1<<12) /* GPL325XX */
    #define SPI1_RX_NONE__CLK_NONE__TX_NONE	 0x80000000
	
	//SPI0
	#define SPI0_RX_IOC3__CLK_IOC1__TX_IOC2	 (0<<8)
	#define SPI0_RX_IOC7__CLK_IOF5__TX_IOC4	 (1<<8)	/* GPL327XX */
	#define SPI0_RX_IOC3__CLK_IOF5__TX_IOF6	 (2<<8)	/* GPL327XX */

    //UART    
	#define UART_TX_IOE2__RX_IOE3                       0x00000000
   	#define UART_TX_IOH2__RX_IOH3                       0x00000001
	#define UART_TX_IOB2__RX_IOB1                       0x10000000  /* code-inter define MLB 0x1 will be marked*/
	#define UART_TX_IOF3__RX_IOF8                       0x10000001  /* code-inter define MLB 0x1 will be marked*/
	#define UART_TX_IOF7__RX_IOF8						0x00000002  /* GPL327XX */
	#define UART_TX_IOF7__RX_NONE						0x00000003  /* GPL327XX */
	#define UART_TX_NONE__RX_NONE						0x80000000  
	#define UART_POSFUN_MASK                            0xFFFFFFFE
    #define UART_POS_SET_VALUE                          UART_TX_RX_POS

    /*TFT*/
    #define TFT_HSYNC_VSYNC_CLK_at_IOB1_IOB2_IOB3       0x00000000
    #define TFT_HSYNC_VSYNC_CLK_at_IOF10_IOF11_IO12     0x00000002
    #define TFT_POSFUN_MASK                             0xFFFFFFFD
    #define TFT_POS_SET_VALUE                           TFT_HSYNC_VSYNC_CLK_POS

    #define TFT_POL_IOB4__STV_IOB5__STH_IOB6            ~(1<<13)
    #define TFT_POL_IOG9__STV_IOG3__STH_IOG4            (1<<13)
    /*CMOS Pin Position Configuration*/ 
    #define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOE8_15_IOE4_IOE5_IOE6_IOE7    0x00004000
    #define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA0_7_IOB0_IOB1_IOB2_IOB3     0x00008000
    #define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOH0_IOB5_IOB6   0x10008000 /* code-inter define MLB 0x1 will be marked*/
    #define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOC11_IOB5_IOB6  0x10004000 /* code-inter define MLB 0x1 will be marked*/
	#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC2_IOH0   0x1000A000   /* GPL327XX */	
    #define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC11_IOH0  0x1000B000   /* GPL327XX */
    //SD Card
    /*===========================================
    SDC: SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9
        CMD:IOC4
        CLK:IOC5
        DATA0:IOC7
        DATA3:IOC6
        DATA1:IOC8
        DATA2:IOC9

    SDC: SDC_IOD8_IOD9_IOD10_IOD11_IOD12_IOD13
        CMD:IOD8
        CLK:IOD9
        DATA0:IOD11
        DATA3:IOD10
        DATA1:IOD12
        DATA2:IOD13
    SDC: SDC_IOB8_IOB9_IOB10_IOB11_IOB12_IOB13
        CMD:IOB8
        CLK:IOB9
        DATA0:IOB11
        DATA3:IOB10
        DATA1:IOB12
        DATA2:IOB13
    ==============================================*/
    #define SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9         0x00000001
    #define SDC_IOD8_IOD9_IOD10_IOD11_IOD12_IOD13     0x00000002
    #define SDC_IOB8_IOB9_IOB10_IOB11_IOB12_IOB13     0x00000003 /* GPL326XX */
    

    /*CF Pin Position Configuration*/
    #define CF_DATA0_15_at_IOD0_15                      0x00000000
    #define CF_DATA0_15_at_IOE0_15                      0x00000800


    /*Key Change Pin Position Configuration */
    #if MCU_VERSION == GPL32_A
     #define KEYCHANGE_at_IOE1                           0x00000000
     #define KEYCHANGE_at_IOF3                           0x00001000
    #elif  MCU_VERSION == GPL32_B
     #define KEYCHANGE_at_IOE3                           0x00000000
     #define KEYCHANGE_at_IOF1                           0x00001000
    #else
     #define KEYCHANGE_at_IOE1                           0x00000000
     #define KEYCHANGE_at_IOF3                           0x00001000
    #endif
    #define KEYCHANGE_POSFUN_MASK                       0xFFFFEFFF
    #define KEYCHANGE_POS_SET_VALUE                     KEYCHANGE_POS
    #define KEY_CHANGE_B_AT_IOF3              			0x00000000
    #define KEY_CHANGE_B_AT_IOE1              			(1<<12)


    /* GPDFXXXX VerB New Position Define element */
    #define IOPOS_KEYC_0_IOF3__1_IOE1                           0x1000   /* Keychange 0:IOF3 ; 1:IOE1 */
    #define IOPOS_TFT_POL_STV_STH_0_IOB456__1_IOG934            0x2000   /* 0: POL-STV-STH=IOB4-IOB5-IOB6 ; 1: POL-STV-STH=IOG9-IOG3-IOG4 */
    #define IOPOS_MEM_A21_A22_0_IOG3_4__1_IOG10_11              0x4000   /* 0: MEMA21-MEMA22=IOG3-IOG4 ; 1: MEMA21-MEMA22=IOG10-IOG11 */
    #define IOCTRL_CMOS_CLK0_AT_IOH0                            0x0002
    #define IOCTRL_SDRAM_CKE_AT_IOH1                            0x0020
    #define IOCTRL_TFT_DATA0_AT_IOA0                            0x0010
    #define IOCTRL_SPI0_CS_AT_IOC0                              0x0008
    #define IOPOS_CSI_DATA0_7_CLK1_CLK0_HSYNC_VSYNC             0x8000   /* ??? */
    #define IOCTRL_BKOEB_AT_IOH0                                0x0004
    #define IOSRSEL_UART_TXRX_0_IOH3H2_IOE2E3__1_IOB2B1_IOF3F8  0x8000   /* IOSESEL_15 0: TXRX=H3H2 or TXRX=E2E3; 1: TXRX=B2B1 or TXRX=F3F8 */
    #define IOSRSEL_NF_ALE_CLE_REB_WEB_IOG5_IOG6_IOG10_IOG11    0x4000
    #define IOSYSMONICTRL_MEM_XDPD                              0x0040
    #define IOSRSEL_CLK_DIV6_EN_XA20                            0x2000
    #define IOSYSMONICTRL_NF_DATA0_7_FORCE_TO_IOA8_15           0x0080  /* NF DATA0~7 force to IOA8~15 */

    /* XA22/21 POS Define*/
    #define XA22_IOG3__XA21_IOG4                  0x00000000
    #define XA22_IOG10__XA21_IOG11                0x00000001

    /* Error handle */
    #if NAND_CTRL_POS == NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11
    #define IOG5_UOT_PCLK_DIV6          0   /* 0: disable  1: enable*/
    #endif
	
	/* define csi irq mode */
	#define CSI_IRQ_PPU_IRQ					0x00000000	
	#define CSI_IRQ_TG_IRQ					0x00000100	
	#define CSI_IRQ_TG_FIFO8_IRQ			0x00000108
	#define CSI_IRQ_TG_FIFO16_IRQ			0x00000110
	#define CSI_IRQ_TG_FIFO32_IRQ			0x00000120
//=== This is for code configuration DON'T REMOVE or MODIFY it =============================//
#endif                              // Don't modify END                                     //
//==========================================================================================//



#endif      // __DRIVER_L1_CFG_H__
