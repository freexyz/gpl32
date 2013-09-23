#ifndef __drv_l1_GPIO_H__
#define __drv_l1_GPIO_H__
 
#include "driver_l1.h"
#include "drv_l1_sfr.h"
#include "drv_l1_tools.h"

/*GPIO Register define*/
/* GPIO: 0xC0000000 */
#define GPIO_BASE_ADDR             	0xC0000000
#define EACH_GPIO_DATA_REG_OFFSET   0x00000020 
#define EACH_DIR_REG_OFFSET        	0x00000020
#define EACH_ATTRIB_REG_OFFSET     	0x00000020

#define IOA_DATA_ADDR              		(GPIO_BASE_ADDR+0x00)   /*0xC0000000*/ 
#define IOA_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x04)   /*0xC0000004*/ 
#define IOA_DIR_ADDR               		(GPIO_BASE_ADDR+0x08)   /*0xC0000008*/ 
#define IOA_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x0C)   /*0xC000000C*/ 
#define IOA_DRV	                        (GPIO_BASE_ADDR+0x10)   /*0xC0000010*/ 
                                                                               
#define IOB_DATA_ADDR              		(GPIO_BASE_ADDR+0x20)   /*0xC0000020*/ 
#define IOB_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x24)   /*0xC0000024*/ 
#define IOB_DIR_ADDR               		(GPIO_BASE_ADDR+0x28)   /*0xC0000028*/ 
#define IOB_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x2C)   /*0xC000002C*/ 
#define IOB_DRV	                        (GPIO_BASE_ADDR+0x30)   /*0xC0000030*/ 
                                                                               
#define IOC_DATA_ADDR              		(GPIO_BASE_ADDR+0x40)   /*0xC0000040*/ 
#define IOC_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x44)   /*0xC0000044*/ 
#define IOC_DIR_ADDR              	 	(GPIO_BASE_ADDR+0x48)   /*0xC0000048*/ 
#define IOC_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x4C)   /*0xC000004C*/ 
#define IOC_DRV	                        (GPIO_BASE_ADDR+0x50)   /*0xC0000050*/ 
                                                                               
#define IOD_DATA_ADDR              		(GPIO_BASE_ADDR+0x60)   /*0xC0000060*/ 
#define IOD_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x64)   /*0xC0000064*/ 
#define IOD_DIR_ADDR               		(GPIO_BASE_ADDR+0x68)   /*0xC0000068*/ 
#define IOD_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x6C)   /*0xC000006C*/ 
#define IOD_DRV	                        (GPIO_BASE_ADDR+0x70)   /*0xC0000070*/ 
                                                                               
#define IOE_DATA_ADDR              		(GPIO_BASE_ADDR+0x80)   /*0xC0000080*/ 
#define IOE_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x84)   /*0xC0000084*/ 
#define IOE_DIR_ADDR               		(GPIO_BASE_ADDR+0x88)   /*0xC0000088*/ 
#define IOE_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x8C)   /*0xC000008C*/ 
#define IOE_DRV	                        (GPIO_BASE_ADDR+0x90)   /*0xC0000090*/ 
                                                                               
#define IOF_DATA_ADDR              		(GPIO_BASE_ADDR+0xA0)   /*0xC00000A0*/ 
#define IOF_BUFFER_ADDR            		(GPIO_BASE_ADDR+0xA4)   /*0xC00000A4*/ 
#define IOF_DIR_ADDR               		(GPIO_BASE_ADDR+0xA8)   /*0xC00000A8*/ 
#define IOF_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0xAC)   /*0xC00000AC*/ 
#define IOF_DRV	                        (GPIO_BASE_ADDR+0xB0)   /*0xC00000B0*/ 
                                                                               
#define IOG_DATA_ADDR              		(GPIO_BASE_ADDR+0xC0)   /*0xC00000C0*/ 
#define IOG_BUFFER_ADDR            		(GPIO_BASE_ADDR+0xC4)   /*0xC00000C4*/ 
#define IOG_DIR_ADDR               		(GPIO_BASE_ADDR+0xC8)   /*0xC00000C8*/ 
#define IOG_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0xCC)   /*0xC00000CC*/ 
#define IOG_DRV	                        (GPIO_BASE_ADDR+0xD0)   /*0xC00000D0*/ 

#define IOH_DATA_ADDR              		(GPIO_BASE_ADDR+0xE0)   /*0xC00000E0*/ 
#define IOH_BUFFER_ADDR            		(GPIO_BASE_ADDR+0xE4)   /*0xC00000E4*/ 
#define IOH_DIR_ADDR               		(GPIO_BASE_ADDR+0xE8)   /*0xC00000E8*/ 
#define IOH_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0xEC)   /*0xC00000EC*/ 
#define IOH_DRV	                        (GPIO_BASE_ADDR+0xF0)   /*0xC00000F0*/ 


#define IOI_DATA_ADDR              		(GPIO_BASE_ADDR+0x180)   /*0xC00000E0*/ 
#define IOI_BUFFER_ADDR            		(GPIO_BASE_ADDR+0x184)   /*0xC00000E4*/ 
#define IOI_DIR_ADDR               		(GPIO_BASE_ADDR+0x188)   /*0xC00000E8*/ 
#define IOI_ATTRIB_ADDR            		(GPIO_BASE_ADDR+0x18C)   /*0xC00000EC*/ 
#define IOI_DRV	                        (GPIO_BASE_ADDR+0x190)   /*0xC00000F0*/ 


/* Attribution Register High/Low definition */
#define INPUT_NO_RESISTOR       1
#define OUTPUT_UNINVERT_CONTENT 1 
#define INPUT_WITH_RESISTOR     0
#define OUTPUT_INVERT_CONTENT   0


#define GPIO_FAIL               0
#define GPIO_OK                 1

#define EACH_REGISTER_GPIO_NUMS 16

#define LOWEST_BIT_MASK         0x00000001


#define DRV_WriteReg32(addr,data)     ((*(volatile INT32U *)(addr)) = (INT32U)data)
#define DRV_Reg32(addr)               (*(volatile INT32U *)(addr))



/* Bearer type enum */
typedef enum 
{
	GPIO_SET_A=0,
	GPIO_SET_B,
	GPIO_SET_C,
	GPIO_SET_D,
    GPIO_SET_E,
    GPIO_SET_F,
    GPIO_SET_G,
    GPIO_SET_H,
    GPIO_SET_I,
    GPIO_SET_MAX
} GPIO_SET_ENUM;


#ifndef __GPIO_PAD_DEFINE__
#define __GPIO_PAD_DEFINE__
/* Share Pin Config*/
/*Position Config star*/
#define NAND_SHARE_MODE             NF_SHARE_MODE

#define NAND_CS_POS                 NF_CS_AS_BKCS2
#define NAND_CS_DRIVING             IO_DRIVING_4mA

#define NAND_PAGE_TYPE              NAND_LARGEBLK        
#define NAND_WP_IO                  NAND_WP_PIN_NONE  //IO_E9

#define NAND_CTRL_POS               NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9
#define NAND_CTRL_DRIVING           IO_DRIVING_4mA


 /* only effect in nand "non-shard with SDRAM" MODE */
#define NAND_DATA5_0_POS            NAND_DATA5_0_AS_IOD5_0
#define NAND_DATA7_6_POS            NAND_DATA7_6_AS_IOD7_6
#define NAND_DATA_DRIVING           IO_DRIVING_4mA


#define UART_TX_RX_POS              UART_TX_IOE2__RX_IOE3
#define UART_TX_RX_DRIVING              UART_TX_IOE2__RX_IOE3

#define TFT_HSYNC_VSYNC_CLK_POS     TFT_HSYNC_VSYNC_CLK_at_IOB1_IOE3_IOB3
#define TFT_HSYNC_VSYNC_CLK_DRIVING IO_DRIVING_4mA /* only 4mA/8mA can choice */
#define TFT_POL_STV_STH             TFT_POL_IOB4__STV_IOB5__STH_IOB6
#define TFT_POL_STV_STH_DRIVING     IO_DRIVING_4mA /* MAX 16mA (IOG9, other max is 8mA) */
#define TFT_DATA0_7_DRIVING         IO_DRIVING_4mA
#define TFT_DATA8_15_DRIVING        IO_DRIVING_4mA
#define TFT_R0G0B0_R1G1B1_DRIVING   IO_DRIVING_4mA
#define TFT_R2B2_DRIVING            IO_DRIVING_4mA

#define CMOS_POS                    CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOC11_IOB5_IOB6
#define CMOS_DRIVING                IO_DRIVING_4mA

#define CF_DATA0_15_POS             CF_DATA0_15_at_IOD0_15
#define CF_DATA0_15_DRIVING         IO_DRIVING_4mA /* IOE only 4mA/8mA can choice, IOD: 4mA/8mA/12mA/16mA can choice*/
#define CF_A0_A1_IORDB_DRVING       IO_DRIVING_4mA 
#define CF_CS0_CS2_IORDY_DRVING     IO_DRIVING_4mA 

#define SD_POS                      SDC_IOC4_IOC5_IOC6_IOC7_IOC8_IOC9
#define SD_DRIVING                  IO_DRIVING_4mA /* IOC only 4mA/8mA can choice, IOD: 4mA/8mA/12mA/16mA can choice*/

#define SPI1_POS                    SPI1_RX_IOE3__CLK_IOE1__TX_IOE2
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


  #define KEYCHANGE_POS               KEYCHANGE_at_IOE3

/* Don't modify start*/
// GPIO PAD
#define NF_ALE_IOC12__CLE_IOC13__REB_IOC14__WEB_IOC15    0x00000000
#define NF_ALE_IOC6__CLE_IOC7__REB_IOC8__WEB_IOC9        0x00000008
#define NF_ALE_IOG5__CLE_IOG6__REB_IOG10__WEB_IOG11      0x10000008
#define NF_ALE_SHARE_CLE_SHARE_REB_SHARE__WEB_SHARE      0x80000000
#define NF_CS_AS_BKCS1                  0x00000000
#define NF_CS_AS_BKCS2                  0x00000010
#define NF_CS_AS_BKCS3                  0x00000020
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
// GPIO PAD_SETS DRIVING
#define IO_DRIVING_CANNOT_SET  0  /* fix in 4mA */
#define IO_DRIVING_4mA         0
#define IO_DRIVING_8mA         1
#define IO_DRIVING_12mA        2
#define IO_DRIVING_16mA        3

//SPI1
#define SPI1_RX_IOE3__CLK_IOE1__TX_IOE2 ~(1<<12) /* GPL325XX */
#define SPI1_RX_IOC4__CLK_IOC5__TX_IOC7  (1<<12) /* GPL325XX */
#define SPI1_RX_IOI3__CLK_IOI1__TX_IOI2  (1<<1)  /* GPL326XXB */
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
#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOH0_IOB5_IOB6  0x10008000 /* code-inter define MLB 0x1 will be marked*/
#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOA8_15_IOC10_IOC11_IOB5_IOB6  0x10004000 /* code-inter define MLB 0x1 will be marked*/
#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC2_IOH0   0x1000A000   /* GPL327XX */	
#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOD8_13_IOF2_IOF5_IOC10_IOF14_IOC11_IOH0  0x1000B000   /* GPL327XX */
#define CMOS_DATA0_7_CLK1_CLK0_HSYNC_VSTNC__IOB8_15_IOF3_IOH0_IOB4_IOB7  0x1000C000   /* GPL326XXB */
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
#define KEYCHANGE_at_IOE1                           0x00000000
#define KEYCHANGE_at_IOF3                           0x00001000
#define KEYCHANGE_POSFUN_MASK                       0xFFFFEFFF
#define KEYCHANGE_POS_SET_VALUE                     KEYCHANGE_POS
#define KEY_CHANGE_B_AT_IOF3              0x00000000
#define KEY_CHANGE_B_AT_IOE1              (1<<12)


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

#endif // GPIO_PAD_DEFINE


#ifndef __GPIO_TYPEDEF__
#define __GPIO_TYPEDEF__

typedef enum {
    IO_A0=0,
    IO_A1 ,
    IO_A2 ,
    IO_A3 ,
    IO_A4 ,
    IO_A5 ,
    IO_A6 ,
    IO_A7 ,
    IO_A8 ,
    IO_A9 ,
    IO_A10,
    IO_A11,
    IO_A12,
    IO_A13,
    IO_A14,
    IO_A15,
    IO_B0 ,
    IO_B1 ,
    IO_B2 ,
    IO_B3 ,
    IO_B4 ,
    IO_B5 ,
    IO_B6 ,
    IO_B7 ,
    IO_B8 ,
    IO_B9 ,
    IO_B10,
    IO_B11,
    IO_B12,
    IO_B13,
    IO_B14,
    IO_B15,
    IO_C0 ,
    IO_C1 ,
    IO_C2 ,
    IO_C3 ,
    IO_C4 ,
    IO_C5 ,
    IO_C6 ,
    IO_C7 ,
    IO_C8 ,
    IO_C9 ,
    IO_C10,
    IO_C11,
    IO_C12,
    IO_C13,
    IO_C14,
    IO_C15,
    IO_D0 ,
    IO_D1 ,
    IO_D2 ,
    IO_D3 ,
    IO_D4 ,
    IO_D5 ,
    IO_D6 ,
    IO_D7 ,
    IO_D8 ,
    IO_D9 ,
    IO_D10,
    IO_D11,
    IO_D12,
    IO_D13,
    IO_D14,
    IO_D15,
    IO_E0 ,
    IO_E1 ,
    IO_E2 ,
    IO_E3 ,
    IO_E4 ,
    IO_E5 ,
    IO_E6 ,
    IO_E7 ,
    IO_E8 ,
    IO_E9 ,
    IO_E10,
    IO_E11,
    IO_E12,
    IO_E13,
    IO_E14,
    IO_E15,
    IO_F0 ,
    IO_F1 ,
    IO_F2 ,
    IO_F3 ,
    IO_F4 ,
    IO_F5 ,
    IO_F6 ,
    IO_F7 ,
    IO_F8 ,
    IO_F9 ,
    IO_F10,
    IO_F11,
    IO_F12,
    IO_F13,
    IO_F14,
    IO_F15,
    IO_G0 ,
    IO_G1 ,
    IO_G2 ,
    IO_G3 ,
    IO_G4 ,
    IO_G5 ,
    IO_G6 ,
    IO_G7 ,
    IO_G8 ,
    IO_G9 ,
    IO_G10,
    IO_G11,
    IO_G12,
    IO_G13,
    IO_G14,
    IO_G15,
    IO_H0 ,
    IO_H1 ,
    IO_H2 ,
    IO_H3 ,
    IO_H4 ,
    IO_H5 ,
    IO_H6 ,
    IO_H7 ,
    IO_H8 ,
    IO_H9 ,
    IO_H10,
    IO_H11,
    IO_H12,
    IO_H13,
    IO_H14,
    IO_H15,
    IO_I0 ,
    IO_I1 ,
    IO_I2 ,
    IO_I3 ,
    IO_I4 ,
    IO_I5 ,
    IO_I6 ,
    IO_I7 ,
    IO_I8 ,
    IO_I9 ,
    IO_I10,
    IO_I11,
    IO_I12,
    IO_I13,
    IO_I14,
    IO_I15        
} GPIO_ENUM;

#endif  //__GPIO_TYPEDEF__


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


#ifndef _GPIO_DRVING_DEF_
#define _GPIO_DRVING_DEF_

typedef enum {
    IOA_DRV_4mA=0x0,
    IOA_DRV_8mA=0x1,
    IOA_DRV_12mA=0x2,
    IOA_DRV_16mA=0x3,
/* IOB Driving Options */
    IOB_DRV_4mA=0x0,
    IOB_DRV_8mA=0x1,
    IOB_DRV_12mA=0x2,
    IOB_DRV_16mA=0x3,    
/* IOC Driving Options */    
    IOC_DRV_4mA=0x0,
    IOC_DRV_8mA=0x1,
    IOC_DRV_12mA=0x2,
    IOCB_DRV_16mA=0x3,     
/* IOD Driving Options */
    IOD_DRV_4mA=0x0,
    IOD_DRV_8mA=0x1,
    IOD_DRV_12mA=0x2,
    IOD_DRV_16mA=0x3,
/* IOE Driving Options */    
    IOE_DRV_4mA=0x0,
    IOE_DRV_8mA=0x1,    
    IOE_DRV_12mA=0x2,
    IOE_DRV_16mA=0x3, 
/* IOF Driving Options */
    IOF_DRV_4mA=0x0,
    IOF_DRV_8mA=0x1,
    IOF_DRV_12mA=0x2,
    IOF_DRV_16mA=0x3, 
    IOF0_DRV_8mA=0x1,
    IOF1_DRV_8mA=0x1,
    IOF2_DRV_8mA=0x1,
    IOF3_DRV_8mA=0x1,
    IOF4_DRV_8mA=0x1,
    IOF5_DRV_8mA=0x1,
    IOF13_DRV_8mA=0x1,
    IOF14_DRV_8mA=0x1,
    IOF15_DRV_8mA=0x1,
    IOF4_DRV_12mA=0x2,
    IOF4_DRV_16mA=0x3,
/* IOG Driving Options */
    IOG_DRV_4mA=0x0,
    IOG_DRV_8mA=0x1,
    IOG_DRV_12mA=0x2,
    IOG_DRV_16mA=0x3, 
    IOG0_DRV_8mA=0x1,
    IOG1_DRV_8mA=0x1,
    IOG2_DRV_8mA=0x1,
    IOG3_DRV_8mA=0x1,
    IOG4_DRV_8mA=0x1,

    IOG5_DRV_8mA=0x1,
    IOG5_DRV_12mA=0x2,
    IOG5_DRV_16mA=0x3,

    IOG6_DRV_8mA=0x1,
    IOG6_DRV_12mA=0x2,
    IOG6_DRV_16mA=0x3,

    IOG7_DRV_8mA=0x1,
    IOG7_DRV_12mA=0x2,
    IOG7_DRV_16mA=0x3,

    IOG8_DRV_8mA=0x1,
    IOG8_DRV_12mA=0x2,
    IOG8_DRV_16mA=0x3,
    
    IOG9_DRV_8mA=0x1,
    IOG9_DRV_12mA=0x2,
    IOG9_DRV_16mA=0x3,    
/* IOH Driving Options */
    IOH_DRV_4mA=0x0,
    IOH_DRV_8mA=0x1,
    IOH_DRV_12mA=0x2,
    IOH_DRV_16mA=0x3, 
    IOH0_DRV_8mA=0x1,
    IOH1_DRV_8mA=0x1,
    IOH1_DRV_12mA=0x2,
    IOH1_DRV_16mA=0x3,
    IOH2_DRV_8mA=0x1,
    IOH3_DRV_8mA=0x1,
    IOH4_DRV_8mA=0x1,
    IOH5_DRV_8mA=0x1,
/* IOI Driving Options */
    IOI_DRV_4mA=0x0,
    IOI_DRV_8mA=0x1,
    IOI_DRV_12mA=0x2,
    IOI_DRV_16mA=0x3
} IO_DRV_LEVEL;


#endif //_GPIO_DRVING_DEF_

#ifndef XD_PIN_ENUM_DEF
#define XD_PIN_ENUM_DEF
    typedef enum
    {
      XD_CS1=0x2,   /* IOF1 */
      XD_CS2=0x4,   /* IOF2 */
      XD_CS3=0x8    /* IOF3 */
    } XD_CS_ENUM;

    typedef enum
    {
      XD_IOC12_15,   
      XD_IOC6_9,   
      XD_IOG5_6_10_11
    } XD_CTRL_PIN_ENUM;

    typedef enum
    {
      XD_DATA_IOB13_8,   
      XD_DATA_IOD5_0,   
      XD_DATA_IOE5_0,
      XD_DATA_IOA13_8
    } XD_DATA0_5_ENUM;

    typedef enum
    {
      XD_DATA_IOB15_14,   
      XD_DATA_IOD7_6,   
      XD_DATA_IOE7_6,
      XD_DATA_IOA15_14 
    } XD_DATA6_7_ENUM;
    
    typedef enum
    {
        ID_NAND,
        ID_NAND1=ID_NAND,
        ID_XD,
        ID_NAND2=ID_XD,
        ID_NULL
    } NAND_XD_ENUM;

    typedef enum {
        XD_ONLY_MODE=0,
        XD_NAND_MODE=1,
        XD_NAND1_NAND2_MODE=2,
        XD_PAD_MODE_MAX,
        XD_PAD_MODE_NULL=0xFF
    } XD_PAD_MODE;    
#endif  //XD_PIN_ENUM_DEF

extern void gpio_init(void);
extern BOOLEAN gpio_read_io(INT32U port);
extern BOOLEAN gpio_write_io(INT32U port, BOOLEAN data);
extern BOOLEAN gpio_init_io(INT32U port, BOOLEAN direction);
extern BOOLEAN gpio_set_port_attribute(INT32U port, BOOLEAN attribute);
extern BOOLEAN gpio_write_all_by_set(INT32U gpio_set_num, INT32U write_data);
extern INT32U gpio_read_all_by_set(INT8U gpio_set_num);
extern BOOLEAN gpio_get_dir(INT32U port);
extern BOOLEAN gpio_drving_init_io(GPIO_ENUM port, IO_DRV_LEVEL gpio_driving_level);
extern void gpio_monitor(GPIO_MONITOR *gpio_monitor);
extern void gpio_drving_init(void);
extern void gpio_drving_uninit(void);
extern void gpio_set_bkueb(BOOLEAN status);
extern void gpio_set_ice_en(BOOLEAN status);
extern void sd_io_swap(INT32U idx);

#endif /* __drv_l1_GPIO_H__ */
