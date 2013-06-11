#include "project.h"
#include "gplib_cfg.h"

typedef struct {
	INT16U	nTx_mode;                                //  0.  Text mode¡G0: Character mode; 1: Bitmap mode
	INT16U	nAddr_mode;                              //  1.  Addressing mode¡G0: Relative; 1: Direct
	INT16U	nColor_mode;                             //  2.  Color mode¡G0:4,1:16,2:64,3:256,4:32768,5:65536,6:RGBG0,7: RGBG1,8: RGBG2,9: RGBG3,10:YUYV0, 11:YUYV1, 12:YUYV2, 13:YUYV3.
	INT16U	nP1024_mode;                             //  3.  P1024 share mode¡G0: disable; 1:enable
	INT16U	nPal_type;                               //  4.  Palette type¡G0: 16-bit ; 1: 25-bit
	INT16U	nBank_sel;                               //  5.  Bank selection¡G0 - 15
	INT16U	nPal_sel;                                //  6.  Palette selection¡G0 - 3
	INT16U	nDepth_num;                              //  7.  Depth number¡G0 - 3
	INT16U	nChar_width;                             //  8.  Character width¡G8, 16, 32, 64, 128, 256
	INT16U	nChar_height;                            //  9.  Character height¡G8, 16, 32, 64, 128, 256
	INT16U	nFlip_mode;                              //  10. Flip mode¡G0:No Flip, 1:H Flip, 2:V Flip, 3:H/V Flip
	INT16U	nBlend_level;                            //  11. Blend level¡G0 - 63
	INT16U	nV3D_Mode;                               //  12. Virtual 3D Mode or not¡G0:No, 1:Yes
	INT16U	nTx_image;                               //  14. The number of images in the TEXT
	INT16U	nImage_size_offset;                      //  15. Image size  offset
	INT16U	index_offset;                            //  16. Image cell index table offset at____CellIndex.bin N¡GThe number of images
	INT16U	data_offset;                             //  17. Image cell data table offset at ____CellData.bin  N¡GThe number of images
} Pic_Header, *PPic_Header;
 
typedef struct {
	INT16U	nTx_mode;                                //  0.  Text mode¡G0: Character mode; 1: Bitmap mode
	INT16U	nAddr_mode;                              //  1.  Addressing mode¡G0: Relative; 1: Direct
	INT16U	nColor_mode;                             //  2.  Color mode¡G0:4,1:16,2:64,3:256,4:32768,5:65536,6:RGBG0,7: RGBG1,8: RGBG2,9: RGBG3,10:YUYV0, 11:YUYV1, 12:YUYV2, 13:YUYV3.
	INT16U	nP1024_mode;                             //  3.  P1024 share mode¡G0: disable; 1:enable
	INT16U	nPal_type;                               //  4.  Palette type¡G0: 16-bit ; 1: 25-bit
	INT16U	nBank_sel;                               //  5.  Bank selection¡G0 - 15
	INT16U	nPal_sel;                                //  6.  Palette selection¡G0 - 3
	INT16U	nDepth_num;                              //  7.  Depth number¡G0 - 3
	INT16U	nChar_width;                             //  8.  Character width¡G8, 16, 32, 64, 128, 256
	INT16U	nChar_height;                            //  9.  Character height¡G8, 16, 32, 64, 128, 256
	INT16U	nFlip_mode;                              //  10. Flip mode¡G0:No Flip, 1:H Flip, 2:V Flip, 3:H/V Flip
	INT16U	nBlend_level;                            //  11. Blend level¡G0 - 63
	INT16U	nV3D_Mode;                               //  12. Virtual 3D Mode or not¡G0:No, 1:Yes	
	INT16U	nTx_image;                               //  14. The number of images in the TEXT
	const INT32U *nImage_size;                       //  15.  Image size  offset
	const INT32U *index_data;                        //  16. Image cell index table offset at____CellIndex.bin N¡GThe number of images
	const INT32U *cell_data;                         //  17. Image cell data table offset at ____CellData.bin  N¡GThe number of images
} Text_Header, *PText_Header;
/*******************************************************************************
 *  PPU CONSTANT DEFINITIONS - BEGIN (FOR G+ DIRECTOR USAGE)
 ******************************************************************************/
// Color Mode Definitions
#define TXN_COLOR_4                     0
#define TXN_COLOR_16                    1
#define TXN_COLOR_64                    2
#define TXN_COLOR_256                   3
#define TXN_COLOR_32768                 4
#define TXN_COLOR_65536                 5
#define TXN_COLOR_RGBG0                 6
#define TXN_COLOR_RGBG1                 7
#define TXN_COLOR_RGBG2                 8
#define TXN_COLOR_RGBG3                 9
#define TXN_COLOR_YUYV0                 10
#define TXN_COLOR_YUYV1                 11
#define TXN_COLOR_YUYV2                 12
#define TXN_COLOR_YUYV3                 13
#define TXN_COLOR_ALPHA_CHANNEL_BLEND   14
#define TXN_COLOR_ARGB4444              15
#define TXN_COLOR_RGBA                  16
//  Palette Index Definitions
#define TXN_PALETTE0                    0
#define TXN_PALETTE1                    1
#define TXN_PALETTE2                    2
#define TXN_PALETTE3                    3
#define TXN_PALETTE4                    4
#define TXN_PALETTE5                    5
#define TXN_PALETTE6                    6
#define TXN_PALETTE7                    7
#define TXN_PALETTE8                    8
#define TXN_PALETTE9                    9
#define TXN_PALETTE10                   10
#define TXN_PALETTE11                   11
#define TXN_PALETTE12                   12
#define TXN_PALETTE13                   13
#define TXN_PALETTE14                   14
#define TXN_PALETTE15                   15
//  PalRAM Definitions
#define PAL_RAM0                        0
#define PAL_RAM1                        1
#define PAL_RAM2                        2
#define PAL_RAM3                        3
//  CellY Definitions
#define TXN_VS0                         0
#define TXN_VS8                         8
#define TXN_VS16                        16
#define TXN_VS32                        32
#define TXN_VS64                        64
#define TXN_VS128                       128
#define TXN_VS256                       256
//  CellX Definitions
#define TXN_HS0                         0
#define TXN_HS8                         8
#define TXN_HS16                        16
#define TXN_HS32                        32
#define TXN_HS64                        64
#define TXN_HS128                       128
#define TXN_HS256                       256
//  TxNFlip Definitions
#define NO_FLIP                         0
#define H_FLIP                          1
#define V_FLIP                          2
#define HV_FLIP                         3
//  TxNSize Definitions
#define TXN_512X256                     0
#define TXN_512X512                     1
#define TXN_1024X512                    2
#define TXN_1024X1024                   3
#define TXN_2048X1024                   4
#define TXN_2048X2048                   5
#define TXN_4096X2048                   6
#define TXN_4096X4096                   7
//  nDepth Definitions
#define TXN_DEPTH0                      0
#define TXN_DEPTH2                      1
#define TXN_DEPTH4                      2
#define TXN_DEPTH6                      3
//  nPalType
#define PAL16_SEPARATE                  0
#define PAL25_SEPARATE                  1
#define PAL1024_DISABLE                 0
#define PAL1024_ENABLE                  1
//  Address mode Definitions
#define TEXT_RELATIVE                   0       
#define TEXT_DIRECT                     1      
//  Text mode Definitions
#define TEXT_CHARACTER                  0
#define TEXT_BITMAP                     1
//  Text Virtual 3D mode Definitions
#define TEXTV3D_DISABLE                 0
#define TEXTV3D_ENABLE                  1
//  Text Bi-Interpolation mode Definitions
#define TEXTINIT_DISABLE                 0
#define TEXTINIT_ENABLE                  1
//  Text Blend Level Definitions
#define	TXBLEND_LEVEL0			            0 
#define	TXBLEND_LEVEL1			            1
#define	TXBLEND_LEVEL2			            2
#define	TXBLEND_LEVEL3			            3
#define	TXBLEND_LEVEL4			            4
#define	TXBLEND_LEVEL5			            5
#define	TXBLEND_LEVEL6			            6
#define	TXBLEND_LEVEL7			            7
#define	TXBLEND_LEVEL8			            8 
#define	TXBLEND_LEVEL9			            9
#define	TXBLEND_LEVEL10			            10
#define	TXBLEND_LEVEL11			            11
#define	TXBLEND_LEVEL12			            12
#define	TXBLEND_LEVEL13			            13
#define	TXBLEND_LEVEL14			            14
#define	TXBLEND_LEVEL15			            15
#define	TXBLEND_LEVEL16			            16
#define	TXBLEND_LEVEL17			            17
#define	TXBLEND_LEVEL18			            18
#define	TXBLEND_LEVEL19			            19
#define	TXBLEND_LEVEL20			            20
#define	TXBLEND_LEVEL21			            21
#define	TXBLEND_LEVEL22			            22
#define	TXBLEND_LEVEL23			            23
#define	TXBLEND_LEVEL24			            24
#define	TXBLEND_LEVEL25			            25
#define	TXBLEND_LEVEL26			            26
#define	TXBLEND_LEVEL27			            27
#define	TXBLEND_LEVEL28			            28
#define	TXBLEND_LEVEL29			            29
#define	TXBLEND_LEVEL30			            30
#define	TXBLEND_LEVEL31			            31
#define	TXBLEND_LEVEL32			            32
#define	TXBLEND_LEVEL33			            33
#define	TXBLEND_LEVEL34			            34
#define	TXBLEND_LEVEL35			            35
#define	TXBLEND_LEVEL36			            36
#define	TXBLEND_LEVEL37			            37
#define	TXBLEND_LEVEL38			            38
#define	TXBLEND_LEVEL39			            39
#define	TXBLEND_LEVEL40			            40
#define	TXBLEND_LEVEL41			            41
#define	TXBLEND_LEVEL42			            42
#define	TXBLEND_LEVEL43			            43
#define	TXBLEND_LEVEL44			            44
#define	TXBLEND_LEVEL45			            45
#define	TXBLEND_LEVEL46			            46
#define	TXBLEND_LEVEL47			            47
#define	TXBLEND_LEVEL48			            48
#define	TXBLEND_LEVEL49			            49
#define	TXBLEND_LEVEL50			            50
#define	TXBLEND_LEVEL51			            51
#define	TXBLEND_LEVEL52			            52
#define	TXBLEND_LEVEL53			            53
#define	TXBLEND_LEVEL54 		            54
#define	TXBLEND_LEVEL55			            55
#define	TXBLEND_LEVEL56			            56
#define	TXBLEND_LEVEL57			            57
#define	TXBLEND_LEVEL58			            58
#define	TXBLEND_LEVEL59			            59
#define	TXBLEND_LEVEL60			            60
#define	TXBLEND_LEVEL61			            61
#define	TXBLEND_LEVEL62			            62
#define	TXBLEND_LEVEL63			            63
