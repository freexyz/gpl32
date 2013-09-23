#include "project.h"
#include "gplib_cfg.h"
#include "driver_l1_cfg.h"

typedef struct {
	INT16S	nCharNum;                           //  0.  Character Number Set
	INT16S	nPosX;                              //  1.  Sprite 2D X Position or Sprite Virtual 3D X0 Position
	INT16S	nPosY;                              //  2.  Sprite 2D Y Position or Sprite Virtual 3D Y0 Position
	INT16S	nSizeX;                             //  3.  Width of Sprite Image
	INT16S	nSizeY;                             //  4.  Heigh of Sprite Image
	INT16U	uAttr0;                             //  5.  Sprite Attribute 0
	INT16U	uAttr1;                             //  6.  Sprite Attribute 1
	INT16S	nPosX1;                             //  7.  Sprite Virtual 3D X1 Position
	INT16S	nPosX2;                             //  8.  Sprite Virtual 3D X1 Position
	INT16S	nPosX3;                             //  9.  Sprite Virtual 3D X1 Position
	INT16S	nPosY1;                             //  10. Sprite Virtual 3D Y1 Position
	INT16S	nPosY2;                             //  11. Sprite Virtual 3D Y2 Position
	INT16S	nPosY3;                             //  12. Sprite Virtual 3D Y3 Position
	const INT32U	*SpCell;                    //  13. Sprite Attribute Start Pointer
} SPRITE, *PSPRITE;

typedef struct {
    INT16U	nCharNum;                           //  0.  Character Number Sum
} SPRITE_BIN_HEADER_H, *PSPRITE_BIN_HEADER_H;

typedef struct {
	INT16U	nImageNum;                          //  1.  Image Number Sum
	INT16U	noffset;                            //  2.  Sprite Data Offset Of Bin
	INT16U	nbank;                              //  3.  Sprite Data Bank Of Bin
} SPRITE_BIN_HEADER_L, *PSPRITE_BIN_HEADER_L;

typedef struct {
	INT16U	nSP_CharNum;                        //  0.  Character Number of sprite image
	INT16U	nSP_Hsize;                          //  1.  Character Number of sprite image
	INT16U	nSP_Vsize;                          //  2.  Character Number of sprite image
	INT32U	nSPNum_ptr;                         //  3.  sprite image pointer of sprite ram  		
	INT32U	nEXSPNum_ptr;                       //  4.  sprite image pointer of sprite exram		
} SpN_ptr, *PSpN_ptr;

typedef enum
{
		Sprite_Coordinate_320X240=0,
		Sprite_Coordinate_640X480,
		Sprite_Coordinate_480X234,
		Sprite_Coordinate_480X272,
		Sprite_Coordinate_720X480,
	    Sprite_Coordinate_800X480,
		Sprite_Coordinate_800X600,
		Sprite_Coordinate_1024X768
} Sprite_Coordinate_Mode;

typedef enum
{
		PPU_hardware_coordinate=0,
		Center2LeftTop_coordinate,
		LeftTop2Center_coordinate
} Coordinate_Change_Mode;

typedef struct {
	INT16S	ex_attr0;                           //  0.  Sprite Extend Attribute 0
	INT16U	ex_attr1;                           //  1.  Sprite Extend Attribute 1
} SpN_EX_RAM, *PSpN_EX_RAM;

typedef struct {
	INT16U	nCharNumLo_16;                      //  0.  CharNum low half-WORD set
	INT16S	uPosX_16;                           //  1.  Sprite 2D X Position or Sprite Virtual 3D X0 Position
	INT16S	uPosY_16;                           //  2.  Sprite 2D Y Position or Sprite Virtual 3D Y0 Position
	INT16U	attr0;                              //  3.  Sprite Attribute 0
	INT16U	attr1;                              //  4.  Sprite Attribute 1
	INT16U	uX1_16;                             //  5.  Sprite Virtual 3D Position 1 
	INT16U	uX2_16;                             //  6.  Sprite Virtual 3D Position 2 
	INT16U	uX3_16;                             //  7.  Sprite Virtual 3D Position 3
} SpN_RAM, *PSpN_RAM;

typedef struct {
	INT16S x0;
	INT16S y0;
	INT16S x1;
	INT16S y1;
	INT16S x2;
	INT16S y2;
	INT16S x3;
	INT16S y3;
} POS_STRUCT, *POS_STRUCT_PTR;

typedef struct {
	FP32 x0;
	FP32 y0;
	FP32 x1;
	FP32 y1;
	FP32 x2;
	FP32 y2;
	FP32 x3;
	FP32 y3;
} POS_STRUCT_GP32XXX, *POS_STRUCT_PTR_GP32XXX;

typedef struct {
  INT8U group_id;
  POS_STRUCT_GP32XXX V3D_POS1;
  POS_STRUCT V3D_POS2;  
} V3D_POS_STRUCT, *V3D_POS_STRUCT_PTR;

typedef struct {
	INT32U cdm0;
	INT32U cdm1;
	INT32U cdm2;
	INT32U cdm3;
} CDM_STRUCT, *CDM_STRUCT_PTR;

#if (MCU_VERSION == GPL326XX)
// PPU new Function
typedef struct {
    INT16U  nCharNumLo;                         //  0.  CharNum low half-WORD set
    INT16U  nRotate;                            //  1.  Rotate set
    INT16U  nPosX;                              //  2.  Position X set
    INT16U  nZoom;                              //  3.  Zoom set
    INT16U  nPosY;                              //  4.  Position Y set
    INT16U  nPaletteBank;                       //  5.  Palette bank set
    INT16U  nBlend;                             //  6.  Blend64 enable set
    INT16U  nDepth;                             //  7.  Depth set
    INT16U  nPalette;                           //  8.  Palette index set
    INT16U  nVS;                                //  9.  Cell veritcal size set
    INT16U  nHS;                                //  10. Cell horizontal size set
    INT16U  nFlipV;                             //  11. Veritical flip set
    INT16U  nFlipH;                             //  12. Horizontal flip set
    INT16U  nColor;                             //  13. Color set
    INT16U  nMosaic;                            //  14. Mosiaic set
    INT16U  nBldLvl;                            //  15. Blend64 level set
    INT16U  nCharNumHi;                         //  16. CharNum high half-WORD set
    INT16U  nSPGroup;                           //  17. Sprite Group Function 
    INT16U  nSPLargeSize;                       //  18. Sprite Large Size Set
    INT16U  nSPInterpolation;                   //  19. Sprite bi-liner interpolation  
} SpN16_CellIdx, *PSpN16_CellIdx;
#else
typedef struct {
    INT16U  nCharNumLo;                         //  0.  CharNum low half-WORD set
    INT16U  nRotate;                            //  1.  Rotate set
    INT16U  nPosX;                              //  2.  Position X set
    INT16U  nZoom;                              //  3.  Zoom set
    INT16U  nPosY;                              //  4.  Position Y set
    INT16U  nPaletteBank;                       //  5.  Palette bank set
    INT16U  nBlend;                             //  6.  Blend64 enable set
    INT16U  nDepth;                             //  7.  Depth set
    INT16U  nPalette;                           //  8.  Palette index set
    INT16U  nVS;                                //  9.  Cell veritcal size set
    INT16U  nHS;                                //  10. Cell horizontal size set
    INT16U  nFlipV;                             //  11. Veritical flip set
    INT16U  nFlipH;                             //  12. Horizontal flip set
    INT16U  nColor;                             //  13. Color set
    INT16U  nMosaic;                            //  14. Mosiaic set
    INT16U  nBldLvl;                            //  15. Blend64 level set
    INT16U  nCharNumHi;                         //  16. CharNum high half-WORD set
    INT16U  nResvd1;                            //  17. Reserved for WORD alignment
} SpN16_CellIdx, *PSpN16_CellIdx;
#endif

//------------------------------------------------------------------------------------------------------
// Define Sprite Rotate Level 0 - 63
//------------------------------------------------------------------------------------------------------
#define					SP_ROTATE0			(0 << 10)
#define					SP_ROTATE1			(1 << 10)
#define					SP_ROTATE2			(2 << 10)
#define					SP_ROTATE3			(3 << 10)
#define					SP_ROTATE4			(4 << 10)
#define					SP_ROTATE5			(5 << 10)
#define					SP_ROTATE6			(6 << 10)
#define					SP_ROTATE7			(7 << 10)
#define					SP_ROTATE8			(8 << 10)
#define					SP_ROTATE9			(9 << 10)
#define					SP_ROTATE10			(10 << 10)
#define					SP_ROTATE11			(11 << 10)
#define					SP_ROTATE12			(12 << 10)
#define					SP_ROTATE13			(13 << 10)
#define					SP_ROTATE14			(14 << 10)
#define					SP_ROTATE15			(15 << 10)
#define					SP_ROTATE16			(16 << 10)
#define					SP_ROTATE17			(17 << 10)
#define					SP_ROTATE18			(18 << 10)
#define					SP_ROTATE19			(19 << 10)
#define					SP_ROTATE20			(20 << 10)
#define					SP_ROTATE21			(21 << 10)
#define					SP_ROTATE22			(22 << 10)
#define					SP_ROTATE23			(23 << 10)
#define					SP_ROTATE24			(24 << 10)
#define					SP_ROTATE25			(25 << 10)
#define					SP_ROTATE26			(26 << 10)
#define					SP_ROTATE27			(27 << 10)
#define					SP_ROTATE28			(28 << 10)
#define					SP_ROTATE29			(29 << 10)
#define					SP_ROTATE30			(30 << 10)
#define					SP_ROTATE31			(31 << 10)
#define					SP_ROTATE32			(32 << 10)
#define					SP_ROTATE33			(33 << 10)
#define					SP_ROTATE34			(34 << 10)
#define					SP_ROTATE35			(35 << 10)
#define					SP_ROTATE36			(36 << 10)
#define					SP_ROTATE37			(37 << 10)
#define					SP_ROTATE38			(38 << 10)
#define					SP_ROTATE39			(39 << 10)
#define					SP_ROTATE40			(40 << 10)
#define					SP_ROTATE41			(41 << 10)
#define					SP_ROTATE42			(42 << 10)
#define					SP_ROTATE43			(43 << 10)
#define					SP_ROTATE44			(44 << 10)
#define					SP_ROTATE45			(45 << 10)
#define					SP_ROTATE46			(46 << 10)
#define					SP_ROTATE47			(47 << 10)
#define					SP_ROTATE48			(48 << 10)
#define					SP_ROTATE49			(49 << 10)
#define					SP_ROTATE50			(50 << 10)
#define					SP_ROTATE51			(51 << 10)
#define					SP_ROTATE52			(52 << 10)
#define					SP_ROTATE53			(53 << 10)
#define					SP_ROTATE54			(54 << 10)
#define					SP_ROTATE55			(55 << 10)
#define					SP_ROTATE56			(56 << 10)
#define					SP_ROTATE57			(57 << 10)
#define					SP_ROTATE58			(58 << 10)
#define					SP_ROTATE59			(59 << 10)
#define					SP_ROTATE60			(60 << 10)
#define					SP_ROTATE61			(61 << 10)
#define					SP_ROTATE62			(62 << 10)
#define					SP_ROTATE63			(63 << 10)
//------------------------------------------------------------------------------------------------------
// Define Sprite Zoom Level 0 - 63
//------------------------------------------------------------------------------------------------------
#define					SP_ZOOM0			(0 << 10)
#define					SP_ZOOM1			(1 << 10)
#define					SP_ZOOM2			(2 << 10)
#define					SP_ZOOM3			(3 << 10)
#define					SP_ZOOM4			(4 << 10)
#define					SP_ZOOM5			(5 << 10)
#define					SP_ZOOM6			(6 << 10)
#define					SP_ZOOM7			(7 << 10)
#define					SP_ZOOM8			(8 << 10)
#define					SP_ZOOM9			(9 << 10)
#define					SP_ZOOM10			(10 << 10)
#define					SP_ZOOM11			(11 << 10)
#define					SP_ZOOM12			(12 << 10)
#define					SP_ZOOM13			(13 << 10)
#define					SP_ZOOM14			(14 << 10)
#define					SP_ZOOM15			(15 << 10)
#define					SP_ZOOM16			(16 << 10)
#define					SP_ZOOM17			(17 << 10)
#define					SP_ZOOM18			(18 << 10)
#define					SP_ZOOM19			(19 << 10)
#define					SP_ZOOM20			(20 << 10)
#define					SP_ZOOM21			(21 << 10)
#define					SP_ZOOM22			(22 << 10)
#define					SP_ZOOM23			(23 << 10)
#define					SP_ZOOM24			(24 << 10)
#define					SP_ZOOM25			(25 << 10)
#define					SP_ZOOM26			(26 << 10)
#define					SP_ZOOM27			(27 << 10)
#define					SP_ZOOM28			(28 << 10)
#define					SP_ZOOM29			(29 << 10)
#define					SP_ZOOM30			(30 << 10)
#define					SP_ZOOM31			(31 << 10)
#define					SP_ZOOM32			(32 << 10)
#define					SP_ZOOM33			(33 << 10)
#define					SP_ZOOM34			(34 << 10)
#define					SP_ZOOM35			(35 << 10)
#define					SP_ZOOM36			(36 << 10)
#define					SP_ZOOM37			(37 << 10)
#define					SP_ZOOM38			(38 << 10)
#define					SP_ZOOM39			(39 << 10)
#define					SP_ZOOM40			(40 << 10)
#define					SP_ZOOM41			(41 << 10)
#define					SP_ZOOM42			(42 << 10)
#define					SP_ZOOM43			(43 << 10)
#define					SP_ZOOM44			(44 << 10)
#define					SP_ZOOM45			(45 << 10)
#define					SP_ZOOM46			(46 << 10)
#define					SP_ZOOM47			(47 << 10)
#define					SP_ZOOM48			(48 << 10)
#define					SP_ZOOM49			(49 << 10)
#define					SP_ZOOM50			(50 << 10)
#define					SP_ZOOM51			(51 << 10)
#define					SP_ZOOM52			(52 << 10)
#define					SP_ZOOM53			(53 << 10)
#define					SP_ZOOM54			(54 << 10)
#define					SP_ZOOM55			(55 << 10)
#define					SP_ZOOM56			(56 << 10)
#define					SP_ZOOM57			(57 << 10)
#define					SP_ZOOM58			(58 << 10)
#define					SP_ZOOM59			(59 << 10)
#define					SP_ZOOM60			(60 << 10)
#define					SP_ZOOM61			(61 << 10)
#define					SP_ZOOM62			(62 << 10)
#define					SP_ZOOM63			(63 << 10)
//------------------------------------------------------------------------------------------------------
// Define Sprite Blend Level 0 - 63
//------------------------------------------------------------------------------------------------------
#define					SPBLEND_LEVEL0			(0 << 8)
#define					SPBLEND_LEVEL1			(1 << 8)
#define					SPBLEND_LEVEL2			(2 << 8)
#define					SPBLEND_LEVEL3			(3 << 8)
#define					SPBLEND_LEVEL4			(4 << 8)
#define					SPBLEND_LEVEL5			(5 << 8)
#define					SPBLEND_LEVEL6			(6 << 8)
#define					SPBLEND_LEVEL7			(7 << 8)
#define					SPBLEND_LEVEL8			(8 << 8)
#define					SPBLEND_LEVEL9			(9 << 8)
#define					SPBLEND_LEVEL10			(10 << 8)
#define					SPBLEND_LEVEL11			(11 << 8)
#define					SPBLEND_LEVEL12			(12 << 8)
#define					SPBLEND_LEVEL13			(13 << 8)
#define					SPBLEND_LEVEL14			(14 << 8)
#define					SPBLEND_LEVEL15			(15 << 8)
#define					SPBLEND_LEVEL16			(16 << 8)
#define					SPBLEND_LEVEL17			(17 << 8)
#define					SPBLEND_LEVEL18			(18 << 8)
#define					SPBLEND_LEVEL19			(19 << 8)
#define					SPBLEND_LEVEL20			(20 << 8)
#define					SPBLEND_LEVEL21			(21 << 8)
#define					SPBLEND_LEVEL22			(22 << 8)
#define					SPBLEND_LEVEL23			(23 << 8)
#define					SPBLEND_LEVEL24			(24 << 8)
#define					SPBLEND_LEVEL25			(25 << 8)
#define					SPBLEND_LEVEL26			(26 << 8)
#define					SPBLEND_LEVEL27			(27 << 8)
#define					SPBLEND_LEVEL28			(28 << 8)
#define					SPBLEND_LEVEL29			(29 << 8)
#define					SPBLEND_LEVEL30			(30 << 8)
#define					SPBLEND_LEVEL31			(31 << 8)
#define					SPBLEND_LEVEL32			(32 << 8)
#define					SPBLEND_LEVEL33			(33 << 8)
#define					SPBLEND_LEVEL34			(34 << 8)
#define					SPBLEND_LEVEL35			(35 << 8)
#define					SPBLEND_LEVEL36			(36 << 8)
#define					SPBLEND_LEVEL37			(37 << 8)
#define					SPBLEND_LEVEL38			(38 << 8)
#define					SPBLEND_LEVEL39			(39 << 8)
#define					SPBLEND_LEVEL40			(40 << 8)
#define					SPBLEND_LEVEL41			(41 << 8)
#define					SPBLEND_LEVEL42			(42 << 8)
#define					SPBLEND_LEVEL43			(43 << 8)
#define					SPBLEND_LEVEL44			(44 << 8)
#define					SPBLEND_LEVEL45			(45 << 8)
#define					SPBLEND_LEVEL46			(46 << 8)
#define					SPBLEND_LEVEL47			(47 << 8)
#define					SPBLEND_LEVEL48			(48 << 8)
#define					SPBLEND_LEVEL49			(49 << 8)
#define					SPBLEND_LEVEL50			(50 << 8)
#define					SPBLEND_LEVEL51			(51 << 8)
#define					SPBLEND_LEVEL52			(52 << 8)
#define					SPBLEND_LEVEL53			(53 << 8)
#define					SPBLEND_LEVEL54			(54 << 8)
#define					SPBLEND_LEVEL55			(55 << 8)
#define					SPBLEND_LEVEL56			(56 << 8)
#define					SPBLEND_LEVEL57			(57 << 8)
#define					SPBLEND_LEVEL58			(58 << 8)
#define					SPBLEND_LEVEL59			(59 << 8)
#define					SPBLEND_LEVEL60			(60 << 8)
#define					SPBLEND_LEVEL61			(61 << 8)
#define					SPBLEND_LEVEL62			(62 << 8)
#define					SPBLEND_LEVEL63			(63 << 8)
//------------------------------------------------------------------------------------------------------
// Define Sprite Mosaic Level 0 - 3
//------------------------------------------------------------------------------------------------------
#define					SP_MOSAIC0			(0 << 14)
#define					SP_MOSAIC1			(1 << 14)
#define					SP_MOSAIC2			(2 << 14)
#define					SP_MOSAIC3			(3 << 14)
//------------------------------------------------------------------------------------------------------
// Define Sprite 16-bit Palette Select
//------------------------------------------------------------------------------------------------------
#define					SP_PBANK0			 0 	
#define					SP_PBANK1			 1
#define					SP_PBANK2			 2	
#define					SP_PBANK3			 3   
//------------------------------------------------------------------------------------------------------
// Define Sprite Blending Effect Disable/Enable
//------------------------------------------------------------------------------------------------------
#define					SPBLEND_DISABLE		(0 << 14)	// Set Sprite Blending effect disable
#define					SPBLEND_ENABLE		(1 << 14)   // Set Sprite Blending effect enable
//------------------------------------------------------------------------------------------------------
// Define Sprite Depth 0 - 3
//------------------------------------------------------------------------------------------------------
#define					SP_DEPTH1 			(0 << 12)	
#define					SP_DEPTH3 			(1 << 12)	
#define					SP_DEPTH5 			(2 << 12)	
#define					SP_DEPTH7 			(3 << 12)	
//------------------------------------------------------------------------------------------------------
// Define Sprite Palette bank 0 - 15
//------------------------------------------------------------------------------------------------------
#define					SP_PALETTE0 		(0 << 8)	
#define					SP_PALETTE1 		(1 << 8)
#define					SP_PALETTE2 		(2 << 8)
#define					SP_PALETTE3 		(3 << 8)
#define					SP_PALETTE4 		(4 << 8)
#define					SP_PALETTE5 		(5 << 8)
#define					SP_PALETTE6 		(6 << 8)
#define					SP_PALETTE7 		(7 << 8)
#define					SP_PALETTE8 		(8 << 8)
#define					SP_PALETTE9 		(9 << 8)
#define					SP_PALETTE10 		(10 << 8)
#define					SP_PALETTE11 		(11 << 8)
#define					SP_PALETTE12 		(12 << 8)
#define					SP_PALETTE13 		(13 << 8)
#define					SP_PALETTE14 		(14 << 8)
#define					SP_PALETTE15 		(15 << 8)
//------------------------------------------------------------------------------------------------------
// Define Sprite Character size for Y
//------------------------------------------------------------------------------------------------------
#define					SP_VSIZE8 			(0 << 6)	
#define					SP_VSIZE16 			(1 << 6)	
#define					SP_VSIZE32 			(2 << 6)	
#define					SP_VSIZE64 			(3 << 6)	
//------------------------------------------------------------------------------------------------------
// Define Sprite Character size for X
//------------------------------------------------------------------------------------------------------
#define					SP_HSIZE8 			(0 << 4)	
#define					SP_HSIZE16 			(1 << 4)
#define					SP_HSIZE32 			(2 << 4)
#define					SP_HSIZE64 			(3 << 4)
//------------------------------------------------------------------------------------------------------
// Define Sprite Large Character size for Y
//------------------------------------------------------------------------------------------------------
#define					SP_VSIZE_L32 		(0 << 6)	
#define					SP_VSIZE_L64 		(1 << 6)	
#define					SP_VSIZE_L128 		(2 << 6)	
#define					SP_VSIZE_L256 		(3 << 6)	
//------------------------------------------------------------------------------------------------------
// Define Sprite Large Character size for X
//------------------------------------------------------------------------------------------------------
#define					SP_HSIZE_L32 		(0 << 4)	
#define					SP_HSIZE_L64 		(1 << 4)
#define					SP_HSIZE_L128 		(2 << 4)
#define					SP_HSIZE_L256 		(3 << 4)
//------------------------------------------------------------------------------------------------------
// Define Sprite H/V-Flip Disable/Enable 
//------------------------------------------------------------------------------------------------------
#define					SPVFLIP_DISABLE 	0x0000	
#define					SPVFLIP_ENABLE 		0x0008	
#define					SPHFLIP_DISABLE 	0x0000	
#define					SPHFLIP_ENABLE 		0x0004	
//------------------------------------------------------------------------------------------------------
// Define Sprite Color Mode
//------------------------------------------------------------------------------------------------------
#define					SP_COLOR4 			0x0000
#define					SP_COLOR16 			0x0001
#define					SP_COLOR64 			0x0002
#define					SP_COLOR256 		0x0003
//------------------------------------------------------------------------------------------------------
// Sprite Palette page 0 to 15
//------------------------------------------------------------------------------------------------------
#define					SpPalettePage0    	0x0
#define					SpPalettePage1    	0x1
#define 				SpPalettePage2    	0x2
#define 				SpPalettePage3    	0x3
#define 				SpPalettePage4    	0x4
#define 				SpPalettePage5    	0x5
#define 				SpPalettePage6    	0x6
#define 				SpPalettePage7    	0x7
#define 				SpPalettePage8    	0x8
#define 				SpPalettePage9    	0x9
#define 				SpPalettePage10   	0xa
#define 				SpPalettePage11   	0xb
#define 				SpPalettePage12   	0xc
#define 				SpPalettePage13   	0xd
#define 				SpPalettePage14   	0xe
#define 				SpPalettePage15   	0xf
//------------------------------------------------------------------------------------------------------
// Sprite Depth
//------------------------------------------------------------------------------------------------------
#define 				SpriteDepth1      	0x0
#define 				SpriteDepth3        0x1
#define 				SpriteDepth5        0x2
#define 				SpriteDepth7       	0x3
//------------------------------------------------------------------------------------------------------
// Define Sprite Group Function ID
//------------------------------------------------------------------------------------------------------
#define 				SpriteNoGroupID     0x0                   // Set Sprite Group Function Disable
#define 				SpriteGroupID1      0x1                   // Set Sprite GroupID 1
#define 				SpriteGroupID2      0x2                   // Set Sprite GroupID 2
#define 				SpriteGroupID3      0x3                   // Set Sprite GroupID 3
//------------------------------------------------------------------------------------------------------
// Define Sprite Large Size Disable/Enable
//------------------------------------------------------------------------------------------------------
#define					SPLarge_DISABLE		        (0 << 2)	  // Set Sprite Large size effect disable
#define					SPLarge_ENABLE		        (1 << 2)      // Set Sprite Large size effect enable
//------------------------------------------------------------------------------------------------------
// Define Sprite Bi-liner Interpolation Disable/Enable
//------------------------------------------------------------------------------------------------------
#define					SPInterpolation_DISABLE		(0 << 3)	  // Set Sprite interpolation effect disable
#define					SPInterpolation_ENABLE		(1 << 3)      // Set Sprite interpolation effect enable