/*
* Purpose: TV encoder driver/interface
*
* Author: simon/dominat
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/
//Include files
#include "drv_l1_tv.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_TV) && (_DRV_L1_TV == 1)                     //
//================================================================//


/*******************************************************************************
 * TV Constant Definitions - Begin
 ******************************************************************************/
const INT16U Tbl_TV_RegSet[][9] = {
      {0x815E,0x5A00,0x8200,0x0000,0x6D4E,0x1F10,0x7C00,0xF003,0x2100},  //0: NTSC-M
      {0x815E,0x5A00,0x7600,0x0000,0x6D4E,0xB800,0x1E00,0xF003,0x2100},  //1: NTSC-J
      {0x8161,0x5A00,0x8200,0x0000,0x6D4E,0xCB10,0x8A00,0x0903,0x2A00},  //2: NTSC-N
      {0x815E,0x5A00,0x8200,0x0000,0x6D4E,0x27F0,0xA500,0xF007,0x2100},  //3: PAL-M
      {0x8161,0x5A00,0x7600,0x0000,0x6D4E,0xCBE0,0x8A00,0x0905,0x2A00},  //4: PAL-B
      {0x8161,0x5A00,0x8200,0x0000,0x6D4E,0xCBF0,0x8A00,0x0905,0x2A00},  //5: PAL-N
      {0x8161,0x5A00,0x7600,0x0000,0x6D4E,0xCBE0,0x8A00,0x0905,0x2A00},  //6: PAL-Nc
      {0x815E,0x5A00,0x7600,0x0000,0x6D4E,0xB800,0x1E00,0xF003,0x2100},  //7: NTSC-J (Non-Interlaced)
      {0x8161,0x5A00,0x7600,0x0000,0x6D5F,0xCBE0,0x8A00,0x0905,0x2A00},  //8: PAL-B (Non-Interlaced)
};
/*******************************************************************************
 * TV Constant Definitions - End
 ******************************************************************************/

/*******************************************************************************
 *  TV Driver Functions - Begin
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
void tv_init (void)
{
    R_TV_CTRL    = 0x0;          //  Disable TV at first
    R_SYSTEM_CLK_CTRL    |= TV_DAPLLEN;  //  Enable 27MHz clock
}

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
 * #define  TV_D1                       2
 *  - INT32S nNonInterlace      :   TV display non-interlace set
 * #define  TV_INTERLACE                0
 * #define  TV_NON_INTERLACE            1
 * OUTPUT:
 *  - Initiate TV related registers
 * RETURN:
 *  - None
 * NOTE:
 *  - This function should be called after calling tv_init () in 1ms.
 ******************************************************************************/
void tv_start (INT32S nTvStd, INT32S nResolution, INT32S nNonInterlace)
{
    R_TV_CTRL    = 0x0;          //  Disable TV at first
    if (nNonInterlace) {
        if (nTvStd < TVSTD_PAL_M) {     //  NTSC
            nTvStd  = TVSTD_NTSC_J_NONINTL;
        } else {
            if(nTvStd==TVSTD_NTSC_J_NONINTL)
              nTvStd  = TVSTD_NTSC_J_NONINTL;
            else
              nTvStd  = TVSTD_PAL_B_NONINTL;
        }
    }
    R_TV_SATURATION = Tbl_TV_RegSet[nTvStd][0];
    R_TV_HUE        = Tbl_TV_RegSet[nTvStd][1];
    R_TV_BRIGHTNESS = Tbl_TV_RegSet[nTvStd][2];
    R_TV_SHARPNESS  = Tbl_TV_RegSet[nTvStd][3];
    R_TV_Y_GAIN     = Tbl_TV_RegSet[nTvStd][4];
    R_TV_Y_DELAY    = Tbl_TV_RegSet[nTvStd][5];
    R_TV_V_POSITION = Tbl_TV_RegSet[nTvStd][6];
    R_TV_H_POSITION = Tbl_TV_RegSet[nTvStd][7];
    R_TV_VIDEODAC   = Tbl_TV_RegSet[nTvStd][8];
    R_TV_CTRL    =   (   TV_BIT_VALUE_SET (B_TVSTD, 7) |                \
                            TV_BIT_VALUE_SET (B_NONINTL, nNonInterlace) |  \
                            TV_BIT_VALUE_SET (B_RESOLUTION, nResolution)|  \
                            TV_BIT_VALUE_SET (B_TVEN, 1)
                        );
    if(nResolution==TV_D1){
       if((nTvStd < TVSTD_PAL_M)||(nTvStd==TVSTD_NTSC_J_NONINTL)){
         #if _PROJ_TYPE == _PROJ_TURNKEY
          R_TV_H_POSITION=0xF0EF;
         #else
          R_TV_H_POSITION=0xF0F6;
         #endif
       }
       R_TV_CTRL2|=0x1;
    }
}

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
 *  - This function should be called after calling tv_init () in 1ms.
 ******************************************************************************/
void tv_enable (void)
{
    R_TV_CTRL |= TV_ENABLE;
}

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
void tv_disable (void)
{
    R_TV_CTRL &= ~TV_ENABLE;
}

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
INT32S tv_adjustment_set (INT32S nAdjustType, INT32S nAdjustment)
{
    BOOLEAN bInc;
    INT32U  *pPtr;
    INT32S  nBit;
    INT32U  uMaskBits, uRegValue;

    if ((nAdjustType < FT_TV_SATURATION) && (nAdjustType > FT_TV_CBAR)) {
        return STATUS_FAIL;
    }
    bInc = TRUE;                                //  Default increment mode
    nBit = B_BIT0;                              //  Default from BIT 0
    uMaskBits = MASK_8_BITS;                    //  Default mask 8 bits low
    switch (nAdjustType) {
    case    FT_TV_SATURATION:
    case    FT_TV_HUE:
    case    FT_TV_BRIGHTNESS:
    case    FT_TV_Y_GAIN:
    case    FT_TV_V_POS:
    case    FT_TV_H_POS:
    case    FT_TV_VIDEO_DAC:
        pPtr = ((INT32U *)&R_TV_SATURATION) + nAdjustType;
        break;
    case    FT_TV_CBAR:
        pPtr = ((INT32U *)&R_TV_SATURATION) + FT_TV_SHARPNESS;
        bInc = FALSE;
        nBit = B_BIT6;
        uMaskBits = MASK_2_BITS;
        break;
    case    FT_TV_LPF:
        pPtr = ((INT32U *)&R_TV_SATURATION) + FT_TV_SHARPNESS;
        bInc = FALSE;
        nBit = B_BIT4;
        uMaskBits = MASK_2_BITS;
        break;
    case    FT_TV_SHARPNESS_SEL:
        pPtr = ((INT32U *)&R_TV_SATURATION) + FT_TV_SHARPNESS;
        bInc = FALSE;
        nBit = B_BIT2;
        uMaskBits = MASK_2_BITS;
        break;
    case    FT_TV_SHARPNESS:
        pPtr = ((INT32U *)&R_TV_SATURATION) + FT_TV_SHARPNESS;
        bInc = FALSE;
        uMaskBits = MASK_2_BITS;
        break;
    case    FT_TV_Y_DELAY:
        bInc = FALSE;
        uMaskBits = MASK_2_BITS;
        break;
    }
    uRegValue = *pPtr;
    uRegValue &= TV_BIT_VALUE_SET (nBit, uMaskBits);
    if (bInc) {
        nAdjustment += (INT32S)(uRegValue >> nBit);
        nAdjustment &= uMaskBits;
        nAdjustment = (INT32S)TV_BIT_VALUE_SET (nBit, nAdjustment);
    } else {
        nAdjustment &= uMaskBits;
        nAdjustment = (INT32S)TV_BIT_VALUE_SET (nBit, nAdjustment);
    }
    *pPtr &= ~TV_BIT_VALUE_SET (nBit, uMaskBits);
    *pPtr |= (INT32U)nAdjustment;
    return STATUS_OK;
}
/*******************************************************************************
 *  TV Driver Functions - End
 ******************************************************************************/


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_TV) && (_DRV_L1_TV == 1)                     //
//================================================================//