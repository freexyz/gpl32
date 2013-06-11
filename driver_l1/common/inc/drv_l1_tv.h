#ifndef __drv_l1_TV_H__
#define __drv_l1_TV_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


#define	B_BIT0							0
#define	B_BIT1							1
#define	B_BIT2							2
#define	B_BIT3							3
#define	B_BIT4							4
#define	B_BIT5							5
#define	B_BIT6							6
#define	B_BIT7							7
#define	B_BIT8							8
#define	B_BIT9							9
#define	B_BIT10							10
#define	B_BIT11							11
#define	B_BIT12							12
#define	B_BIT13							13
#define	B_BIT14							14
#define	B_BIT15							15
#define	B_BIT16							16
#define	B_BIT17							17
#define	B_BIT18							18
#define	B_BIT19							19
#define	B_BIT20							20
#define	B_BIT21							21
#define	B_BIT22							22
#define	B_BIT23							23
#define	B_BIT24							24
#define	B_BIT25							25
#define	B_BIT26							26
#define	B_BIT27							27
#define	B_BIT28							28
#define	B_BIT29							29
#define	B_BIT30							30
#define	B_BIT31							31
#define	BIT0							(((INT32U)1)<<B_BIT0)
#define	BIT1							(((INT32U)1)<<B_BIT1)
#define	BIT2							(((INT32U)1)<<B_BIT2)
#define	BIT3							(((INT32U)1)<<B_BIT3)
#define	BIT4							(((INT32U)1)<<B_BIT4)
#define	BIT5							(((INT32U)1)<<B_BIT5)
#define	BIT6							(((INT32U)1)<<B_BIT6)
#define	BIT7							(((INT32U)1)<<B_BIT7)
#define	BIT8							(((INT32U)1)<<B_BIT8)
#define	BIT9							(((INT32U)1)<<B_BIT9)
#define	BIT10							(((INT32U)1)<<B_BIT10)
#define	BIT11							(((INT32U)1)<<B_BIT11)
#define	BIT12							(((INT32U)1)<<B_BIT12)
#define	BIT13							(((INT32U)1)<<B_BIT13)
#define	BIT14							(((INT32U)1)<<B_BIT14)
#define	BIT15							(((INT32U)1)<<B_BIT15)
#define	BIT16							(((INT32U)1)<<B_BIT16)
#define	BIT17							(((INT32U)1)<<B_BIT17)
#define	BIT18							(((INT32U)1)<<B_BIT18)
#define	BIT19							(((INT32U)1)<<B_BIT19)
#define	BIT20							(((INT32U)1)<<B_BIT20)
#define	BIT21							(((INT32U)1)<<B_BIT21)
#define	BIT22							(((INT32U)1)<<B_BIT22)
#define	BIT23							(((INT32U)1)<<B_BIT23)
#define	BIT24							(((INT32U)1)<<B_BIT24)
#define	BIT25							(((INT32U)1)<<B_BIT25)
#define	BIT26							(((INT32U)1)<<B_BIT26)
#define	BIT27							(((INT32U)1)<<B_BIT27)
#define	BIT28							(((INT32U)1)<<B_BIT28)
#define	BIT29							(((INT32U)1)<<B_BIT29)
#define	BIT30							(((INT32U)1)<<B_BIT30)
#define	BIT31							(((INT32U)1)<<B_BIT31)
#define	MASK_1_BITS						(((INT32U)BIT1)-1)
#define	MASK_2_BITS						(((INT32U)BIT2)-1)
#define	MASK_3_BITS						(((INT32U)BIT3)-1)
#define	MASK_4_BITS						(((INT32U)BIT4)-1)
#define	MASK_5_BITS						(((INT32U)BIT5)-1)
#define	MASK_6_BITS						(((INT32U)BIT6)-1)
#define	MASK_7_BITS						(((INT32U)BIT7)-1)
#define	MASK_8_BITS						(((INT32U)BIT8)-1)
#define	MASK_9_BITS						(((INT32U)BIT9)-1)
#define	MASK_10_BITS					(((INT32U)BIT10)-1)
#define	MASK_11_BITS					(((INT32U)BIT11)-1)
#define	MASK_12_BITS					(((INT32U)BIT12)-1)
#define	MASK_13_BITS					(((INT32U)BIT13)-1)
#define	MASK_14_BITS					(((INT32U)BIT14)-1)
#define	MASK_15_BITS					(((INT32U)BIT15)-1)
#define	MASK_16_BITS					(((INT32U)BIT16)-1)
#define	MASK_17_BITS					(((INT32U)BIT17)-1)
#define	MASK_18_BITS					(((INT32U)BIT18)-1)
#define	MASK_19_BITS					(((INT32U)BIT19)-1)
#define	MASK_20_BITS					(((INT32U)BIT20)-1)
#define	MASK_21_BITS					(((INT32U)BIT21)-1)
#define	MASK_22_BITS					(((INT32U)BIT22)-1)
#define	MASK_23_BITS					(((INT32U)BIT23)-1)
#define	MASK_24_BITS					(((INT32U)BIT24)-1)
#define	MASK_25_BITS					(((INT32U)BIT25)-1)
#define	MASK_26_BITS					(((INT32U)BIT26)-1)
#define	MASK_27_BITS					(((INT32U)BIT27)-1)
#define	MASK_28_BITS					(((INT32U)BIT28)-1)
#define	MASK_29_BITS					(((INT32U)BIT29)-1)
#define	MASK_30_BITS					(((INT32U)BIT30)-1)
#define	MASK_31_BITS					(((INT32U)BIT31)-1)
#define	MASK_32_BITS					((INT32U)0xFFFFFFFF)
/*******************************************************************************
 *  TV MACRO FUNCTION DEFINITIONS - BEGIN
 ******************************************************************************/
#define TV_BIT_VALUE_SET(__BIT_N__,__VALUE_SET__)  \
(((INT32U)(__VALUE_SET__))<<(__BIT_N__))
/*******************************************************************************
 *  TV MACRO FUNCTION DEFINITIONS - END
 ******************************************************************************/
/*******************************************************************************
 *  TV REGISTER STRUCTURE DEFINITIONS - BEGIN (INTERNAL USAGE)
 ******************************************************************************/
typedef union {
    INT32U  wordData;
    struct {
        INT32S  BYTEDATA        :   8;              //  BIT[7:0]
    } byteData;
} Tv_Adjust_Reg, *PTv_Adjust_Reg;
/*******************************************************************************
 *  TV REGISTER STRUCTURE DEFINITIONS - END (INTERNAL USAGE)
 ******************************************************************************/
/*******************************************************************************
 *  TV SYSTEM CLOCK REGISTER CONSTANT DEFINITIONS - BEGIN
 ******************************************************************************/
//  R_SYSTEM_CLK_CTRL    -   System Clock Control Register
#define B_DAPLLEN                       4
#define TV_DAPLLEN                      (((INT32U)1)<<B_DAPLLEN)
/*******************************************************************************
 *  TV SYSTEM CLOCK REGISTER CONSTANT DEFINITIONS - END
 ******************************************************************************/
/*******************************************************************************
 *  TV CONTROL REGISTER CONSTANT DEFINITIONS - BEGIN (INTERNAL USAGE)
 ******************************************************************************/
//  R_TV_CTRL        -   BIT[6:5]        -   Resolution
#define B_RESOLUTION                    5
//  R_TV_CTRL        -   BIT[4]          -   NONINTL
#define B_NONINTL                       4
//  R_TV_CTRL        -   BIT[3:1]        -   TVSTD
#define B_TVSTD                         1
//  R_TV_CTRL        -   BIT[0]          -   TVEN
#define B_TVEN                          0
#define TV_DISABLE                      (((INT32U)0)<<B_TVEN)
#define TV_ENABLE                       (((INT32U)1)<<B_TVEN)
/*******************************************************************************
 *  TV CONTROL REGISTER CONSTANT DEFINITIONS - END (INTERNAL USAGE)
 ******************************************************************************/
/*******************************************************************************
 *  TV DRIVER FUNCTIONS - BEGIN
 ******************************************************************************/
/*******************************************************************************
 * FUNCTION NAME: tv_init
 * DESCRIPTION: Initiate TV
 * INPUT:
 *  - None
 * OUTPUT:
 *  - Enable TV clock
 *  - Reset TV control register
 * RETURN:
 *  - None
 * NOTE:
 *  - All other functions should be called after 1ms.
 ******************************************************************************/
extern void tv_init (void);
/*******************************************************************************
 * FUNCTION NAME: tv_start
 * DESCRIPTION: Initiate TV display registers
 * INPUT:
 *  - INT32S nTvStd             :   TV display type set
 * #define  TVSTD_NTSC_M                0
 * #define  TVSTD_NTSC_J                1
 * #define  TVSTD_NTSC_N                2
 * #define  TVSTD_PAL_M                 3
 * #define  TVSTD_PAL_B                 4
 * #define  TVSTD_PAL_N                 5
 * #define  TVSTD_PAL_NC                6
 * #define  TVSTD_NTSC_J_NONINTL        7
 * #define  TVSTD_PAL_B_NONINTL         8
 *  - INT32S nResolution        :   TV display resolution set
 * #define  TV_QVGA                     0
 * #define  TV_HVGA                     1
 * #define  TV_NTSC                     2
 *  - INT32S nNonInterlace      :   TV display non-interlace set
 * #define  TV_INTERLACE                0
 * #define  TV_NON_INTERLACE            1
 * OUTPUT:
 *  - Initiate TV related registers
 * RETURN:
 *  - None
 * NOTE:
 *  - TV enable bit of control register SHOULD NOT BE SET
 ******************************************************************************/
extern void tv_start (INT32S nTvStd, INT32S nResolution, INT32S nNonInterlace);
//  nTvStd
#define TVSTD_NTSC_M                    0
#define TVSTD_NTSC_J                    1
#define TVSTD_NTSC_N                    2
#define TVSTD_PAL_M                     3
#define TVSTD_PAL_B                     4
#define TVSTD_PAL_N                     5
#define TVSTD_PAL_NC                    6
#define TVSTD_NTSC_J_NONINTL            7
#define TVSTD_PAL_B_NONINTL             8
//  nResolution
#define TV_QVGA                         0
#define TV_HVGA                         1
#define TV_D1                           2
//  nNonInterlace
#define TV_INTERLACE                    0
#define TV_NON_INTERLACE                1
/*******************************************************************************
 * FUNCTION NAME: tv_enable
 * DESCRIPTION: Enable TV display
 * INPUT:
 *  - None
 * OUTPUT:
 *  - Set TV control register enable bit
 * RETURN:
 *  - None
 * NOTE:
 *  - This function should be called after calling tv_start () in 1ms.
 ******************************************************************************/
extern void tv_enable (void);
/*******************************************************************************
 * FUNCTION NAME: tv_disable
 * DESCRIPTION: Disable TV display
 * INPUT:
 *  - None
 * OUTPUT:
 *  - Clear TV control register enable bit
 * RETURN:
 *  - None
 ******************************************************************************/
extern void tv_disable (void);
/*******************************************************************************
 * FUNCTION NAME: tv_adjustment_set
 * DESCRIPTION: Set TV adjusting register value
 * INPUT:
 *  - INT32S nAdjustType        : Adjusting register set
 *#define   FT_TV_SATURATION            0
 *#define   FT_TV_HUE                   1
 *#define   FT_TV_BRIGHTNESS            2
 *#define   FT_TV_SHARPNESS             3
 *#define   FT_TV_Y_GAIN                4
 *#define   FT_TV_Y_DELAY               5
 *#define   FT_TV_V_POS                 6
 *#define   FT_TV_H_POS                 7
 *#define   FT_TV_VIDEO_DAC             8
 *#define   FT_TV_SHARPNESS_SEL         9
 *#define   FT_TV_LPF                   10
 *#define   FT_TV_CBAR                  11
 *  - INT32S nAdjustment        : Adjustment set
 * OUTPUT:
 *  - Set TV related adjusting register
 * RETURN:
 *  - STATUS_OK                 : Complete this function
 *  - STATUS_FAIL               : Invalid input parameters
 * NOTE:
 *  - +/- mode: FT_TV_SATURATION/FT_TV_HUE/FT_TV_BRIGHTNESS/FT_TV_Y_GAIN/
 *              FT_TV_V_POS/FT_TV_H_POS/FT_TV_VIDEO_DAC
 *  - Set mode: FT_TV_CBAR/FT_TV_LPF/FT_TV_SHARPNESS_SEL/
 *              FT_TV_SHARPNESS/FT_TV_Y_DELAY
 ******************************************************************************/
extern INT32S tv_adjustment_set (INT32S nAdjustType, INT32S nAdjustment);
//  nAdjustType
#define FT_TV_SATURATION                0
#define FT_TV_HUE                       1
#define FT_TV_BRIGHTNESS                2
#define FT_TV_SHARPNESS                 3
#define FT_TV_Y_GAIN                    4
#define FT_TV_Y_DELAY                   5
#define FT_TV_V_POS                     6
#define FT_TV_H_POS                     7
#define FT_TV_VIDEO_DAC                 8
#define FT_TV_SHARPNESS_SEL             9
#define FT_TV_LPF                       10
#define FT_TV_CBAR                      11
/*******************************************************************************
 *  TV DRIVER FUNCTIONS - END
 ******************************************************************************/

#endif      // __drv_l1_TV_H__