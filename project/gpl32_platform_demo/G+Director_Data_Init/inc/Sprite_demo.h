/*
* Description: This task is the G+PPUImageConverter Sprite data verification.
*
* Author: Cater Chen
*
* Date: 2009/10/26
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "application.h"

// Sprite Function
#define  Pos_Noscroll        0                   // Set Sprite NO Scroll
#define  Pos_scroll          1                   // Set Sprite Scroll
#define  SpriteNoGroupID     0                   // Set Sprite Group Function Disable
#define  SpriteGroupID1      1                   // Set Sprite GroupID 1
#define  SpriteGroupID2      2                   // Set Sprite GroupID 2
#define  SpriteGroupID3      3                   // Set Sprite GroupID 3

//Display screen selection for sprite coordinate
typedef enum
{
		Sprite_Coordinate_320X240=0,             // Display screen is the 320x240.
		Sprite_Coordinate_640X480,               // Display screen is the 640x480.
		Sprite_Coordinate_480X234,               // Display screen is the 480x234.
		Sprite_Coordinate_480X272,               // Display screen is the 480x272.
		Sprite_Coordinate_720X480,               // Display screen is the 720x480.
	    Sprite_Coordinate_800X480,               // Display screen is the 800x480.
		Sprite_Coordinate_800X600,               // Display screen is the 800x600.
		Sprite_Coordinate_1024X768               // Display screen is the 1024x768.
} Sprite_Coordinate_Mode;

//Sprite coordinate transformation. 
typedef enum
{
		PPU_hardware_coordinate=0,              // User defines Position.
		Center2LeftTop_coordinate,              // User defines Center Position change for LeftTop Position.
		LeftTop2Center_coordinate               // User defines LeftTop Position change for Center Position.
} Coordinate_Change_Mode;

// Start address of sprite ram and total of sprite number for sprite image.
typedef struct {
	INT16U	nSP_CharNum;                        //  0. Character number of sprite image
	INT16U	nSP_Hsize;                          //  1. Character number of sprite image
	INT16U	nSP_Vsize;                          //  2. Character number of sprite image
	INT32U	nSPNum_ptr;                         //  3. Sprite image pointer of sprite ram		
	INT32U	nEXSPNum_ptr;                       //  4. Sprite image pointer of sprite exram	
} SpN_ptr, *PSpN_ptr;

typedef struct {
  INT8U group_id;                               // Sprite Group ID.
  POS_STRUCT_GP32XXX V3D_POS1;                  // Sprite V3D Position Type 1. 
  POS_STRUCT V3D_POS2;                          // Sprite V3D Position Type 2.  
} V3D_POS_STRUCT, *V3D_POS_STRUCT_PTR;

//Sprite RAM For Program;
extern SpN_RAM	SpriteRAM[];                            //G+ API sprite ram address
#if ((MCU_VERSION == GPL326XX))                         
extern SpN_EX_RAM SpriteExRAM[];                        //G+ API sprite externd ram address
#endif

// Sprite Function API
//----------------------------------------------------------------------------------------------------
//	Description:	G+Director c format output attribute initiation for sprite buffer.
//	Syntax:			void set_sprite_init(INT32U sprite_number,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct start pointer(G+Director xxxxCellIdx.c).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_sprite_init(INT32U sprite_number,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Display sprite initiation.
//	Syntax:			void set_sprite_init(INT32U sprite_number,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct->SpCell for image start pointer(G+Director xxxxCellIdx.c).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     -const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//                  INT16S sprite_pos_x     :   Sprite x position of display screen.
//                  INT16S sprite_pos_y     :   Sprite y position of display screen.
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_sprite_display_init(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Display sprite image transformation.
//	Syntax:			void set_sprite_image_number(INT32U sprite_number,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct->SpCell for image start pointer(G+Director).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     -const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_sprite_image_number(INT32U sprite_number,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Disable display sprite.
//	Syntax:			void set_sprite_disable(INT32U sprite_number)
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//	Return: 		None
//  Note  :         The set_sprite_disable function must be combined with paint_ppu_spriteram function.
//----------------------------------------------------------------------------------------------------
extern void set_sprite_disable(INT32U sprite_number);

//----------------------------------------------------------------------------------------------------
//	Description:	Calculate sprite attribute for sprite ram.
//	Syntax:			void paint_ppu_spriteram(PPU_REGISTER_SETS *p_register_set,Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number);
//	Input Paramter(s):
//					PPU_REGISTER_SETS *p_register_set	    :	ppu application information.
//					-typedef struct 
//                   {
//						INT32U update_register_flag;			
//					 #define C_UPDATE_REG_SET_PALETTE0			   0x01000000
//					 #define C_UPDATE_REG_SET_PALETTE1			   0x02000000
//					 #define C_UPDATE_REG_SET_PALETTE2			   0x04000000
//					 #define C_UPDATE_REG_SET_PALETTE3			   0x08000000
//					 #define C_UPDATE_REG_SET_HORIZONTAL_MOVE	   0x10000000
//					 #define C_UPDATE_REG_SET_TEXT1_HCOMPRESS	   0x20000000
//					 #define C_UPDATE_REG_SET_TEXT3_25D			   0x40000000
//					 #define C_UPDATE_REG_SET_SPRITE_ATTRIBUTE	   0x80000000
//					 #define C_UPDATE_REG_SET_SPRITE_EX_ATTRIBUTE  0x00100000
//					 #define C_UPDATE_REG_SET_DMA_MASK			   0xFFF00000
//					 #define C_UPDATE_REG_SET_PPU				   0x00000001
//					 #define C_UPDATE_REG_SET_TEXT1				   0x00000002
//					 #define C_UPDATE_REG_SET_TEXT2				   0x00000004
//					 #define C_UPDATE_REG_SET_TEXT3				   0x00000008
//					 #define C_UPDATE_REG_SET_TEXT4				   0x00000010
//					 #define C_UPDATE_REG_SET_SPRITE		       0x00000020
//					 #define C_UPDATE_REG_SET_COLOR				   0x00000040
//						INT32U ppu_palette0_ptr;				
//						INT32U ppu_palette1_ptr;				
//						INT32U ppu_palette2_ptr;				
//						INT32U ppu_palette3_ptr;				
//						INT32U ppu_horizontal_move_ptr;			
//						INT32U ppu_text1_hcompress_ptr;			
//						INT32U ppu_text3_25d_ptr;				
//						INT32U ppu_sprite_attribute_ptr;		
//					    INT32U ppu_sprite_ex_attribute_ptr;          
//						INT16U ppu_blending_level;				
//						INT16U ppu_fade_control;				
//						INT32U ppu_palette_control;				
//						INT32U ppu_rgb565_transparent_color;	
//						INT32U ppu_window1_x;					
//						INT32U ppu_window1_y;					
//						INT32U ppu_window2_x;					
//						INT32U ppu_window2_y;					
//						INT32U ppu_window3_x;					
//						INT32U ppu_window3_y;					
//						INT32U ppu_window4_x;					
//						INT32U ppu_window4_y;					
//						INT32U ppu_enable;						
//						INT32U ppu_misc;                                               
//						INT32U ppu_free_mode;                                       
//						INT32U ppu_frame_buffer_fifo;                           
//						INT32U ppu_special_effect_rgb;			
//						struct ppu_text_register_sets 
//                      {
//							INT16U position_x;					
//							INT16U position_y;					
//							INT16U offset_x;					
//							INT16U offset_y;					
//							INT32U attribute;					
//							INT32U control;						
//							INT32U n_ptr;						
//							INT32U a_ptr;						
//							INT16U sine;						
//							INT16U cosine;						
//							INT32U segment;						
//						} text[4];
//						INT16U ppu_vcompress_value;				
//						INT16U ppu_vcompress_offset;			
//						INT16U ppu_vcompress_step;				
//						INT16U text3_25d_y_compress;			
//						INT32U sprite_control;					
//						INT32U sprite_segment;					
//						INT32U extend_sprite_control;			
//						INT32U extend_sprite_addr;				
//						INT32U color_mapping_ptr;
//					} PPU_REGISTER_SETS, *PPU_REGISTER_SETS_PTR;
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  Coordinate_Change_Mode coordinate_mode  :   Sprite coordinate transformation selection.
//                  -typedef enum
//                   {
//		                PPU_hardware_coordinate=0,            
//		                Center2LeftTop_coordinate,             
//		                LeftTop2Center_coordinate              
//                   } Coordinate_Change_Mode;
//
//                  INT16U sprite_number                    :   Maximum of Display sprite number.
//	Return: 		None
//  Note  :         Maximum of Display sprite number must be combined with hardware.
//----------------------------------------------------------------------------------------------------                                                                   
extern void paint_ppu_spriteram(PPU_REGISTER_SETS *p_register_set,Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number);

//----------------------------------------------------------------------------------------------------
//	Description:	Obtainment of sprite ram start address and sprite number.
//	Syntax:			void Get_sprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//                  SpN_ptr *sprite_ptr     :   Sprite information note of sprite ram start address and sprite number.
//					-typedef struct 
//                   {
//						INT16U	nSP_CharNum;                        
//						INT16U	nSP_Hsize;                         
//						INT16U	nSP_Vsize;                         
//						INT32U	nSPNum_ptr;                        
//						INT32U	nEXSPNum_ptr;          
//					 } SpN_ptr, *PSpN_ptr;  
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void Get_sprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr);                                                

//----------------------------------------------------------------------------------------------------
//	Description:	Sprite V3D coordinate transformation.
//	Syntax:			void sprite25D_Pos_Coordinate_Chang(Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,POS_STRUCT_PTR in);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  Coordinate_Change_Mode coordinate_mode  :   Sprite coordinate transformation selection.
//                  -typedef enum
//                   {
//		                PPU_hardware_coordinate=0,            
//		                Center2LeftTop_coordinate,             
//		                LeftTop2Center_coordinate              
//                   } Coordinate_Change_Mode;
//                  POS_STRUCT_PTR in                       :   Position value.
//					-typedef struct 
//                   {
//						INT16S x0;
//						INT16S y0;
//						INT16S x1;
//						INT16S y1;
//						INT16S x2;
//						INT16S y2;
//						INT16S x3;
//						INT16S y3;
//					 } POS_STRUCT, *POS_STRUCT_PTR
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void spriteV3D_Pos_Coordinate_Change(Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,POS_STRUCT_PTR in);    

//----------------------------------------------------------------------------------------------------
//	Description:	Sprite 2D position transformation.
//	Syntax:			void SetSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  INT16S x                                :   move range of x direction.
//                  INT16S y                                :   move range of y direction.
//                  INT16U scroll                           :   Display screen boundary decision.
//                  -#define  Pos_Noscroll       0    
//                  -#define  Pos_scroll         1                      
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void SetSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll); 

//----------------------------------------------------------------------------------------------------
//	Description:	Sprite 3D position transformation.
//	Syntax:			void SpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,POS_STRUCT_PTR in);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  Coordinate_Change_Mode coordinate_mode  :   Sprite coordinate transformation selection.
//                  -typedef enum
//                   {
//		                PPU_hardware_coordinate=0,            
//		                Center2LeftTop_coordinate,             
//		                LeftTop2Center_coordinate              
//                   } Coordinate_Change_Mode;
//                  SpN_ptr *sprite_ptr     :   Sprite information note of sprite ram start address and sprite number.
//					-typedef struct 
//                   {
//						INT16U	nSP_CharNum;                        
//						INT16U	nSP_Hsize;                         
//						INT16U	nSP_Vsize;                         
//						INT32U	nSPNum_ptr;                        
//						INT32U	nEXSPNum_ptr;          
//					 } SpN_ptr, *PSpN_ptr; 
//                  V3D_POS_STRUCT_PTR in                       :   Position value and group id.
//                  -typedef struct 
//                   {
//                      INT8U group_id;
//                      POS_STRUCT_GP32XXX V3D_POS1;
//                      POS_STRUCT V3D_POS2;  
//                   } V3D_POS_STRUCT, *V3D_POS_STRUCT_PTR;                    
//	Return: 		None
//  Note  :         Sprite V3D position type and group id must be combined with hardware.  
//----------------------------------------------------------------------------------------------------
extern void SpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,V3D_POS_STRUCT_PTR in);
                                                  
//Sprite Bin API
//----------------------------------------------------------------------------------------------------
//	Description:	Load Sprite Cell Index Date.
//	Syntax:			void load_sprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN);
//	Input Paramter(s):
//					INT16U display_sp_num		:	Display sprite number.
//                  INT16U sp_num               :   Sprite image number.
//                  INT16S sp_pos_x             :   Sprite x position of display screen.
//                  INT16S sp_pos_y             :   Sprite y position of display screen.
//                  INT32U* SPRITE_ATX_BIN      :   Bin start address(G+Director xxxxCellIdx.bin).                 
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void load_sprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN);

//----------------------------------------------------------------------------------------------------
//	Description:	Load Sprite Cell Index Date.
//	Syntax:			void load_sprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN);
//	Input Paramter(s):
//					INT16U display_sp_num		:	Display sprite number.
//                  INT16U sp_num               :   Sprite image number.
//                  INT16U sp_image_num         :   Sprite cell index image number.
//                  INT16S sp_pos_x             :   Sprite x position of display screen.
//                  INT16S sp_pos_y             :   Sprite y position of display screen.
//                  INT32U* SPRITE_ATX_BIN      :   Bin start address(G+Director xxxxCellIdx.bin).                 
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_sprite_image_number_bin(INT16U display_sp_num,INT16U sp_num,INT16U sp_image_num,INT32U* SPRITE_ATX_BIN);

//Exsprite Bin API
//----------------------------------------------------------------------------------------------------
//	Description:	Load Sprite Cell Index Date.
//	Syntax:			void load_exsprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN);
//	Input Paramter(s):
//					INT16U display_sp_num		:	Display sprite number.
//                  INT16U sp_num               :   Sprite image number.
//                  INT16S sp_pos_x             :   Sprite x position of display screen.
//                  INT16S sp_pos_y             :   Sprite y position of display screen.
//                  INT32U* SPRITE_ATX_BIN      :   Bin start address(G+Director xxxxCellIdx.bin).                 
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void load_exsprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN);

//----------------------------------------------------------------------------------------------------
//	Description:	Load Sprite Cell Index Date.
//	Syntax:			void set_exsprite_image_number_bin(INT16U display_sp_num,INT16U sp_num,INT16U sp_image_num,INT32U* SPRITE_ATX_BIN);
//	Input Paramter(s):
//					INT16U display_sp_num		:	Display sprite number.
//                  INT16U sp_num               :   Sprite image number.
//                  INT16U sp_image_num         :   Sprite cell index image number.
//                  INT16S sp_pos_x             :   Sprite x position of display screen.
//                  INT16S sp_pos_y             :   Sprite y position of display screen.
//                  INT32U* SPRITE_ATX_BIN      :   Bin start address(G+Director xxxxCellIdx.bin).                 
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_exsprite_image_number_bin(INT16U display_sp_num,INT16U sp_num,INT16U sp_image_num,INT32U* SPRITE_ATX_BIN);

// Load FILE API
//----------------------------------------------------------------------------------------------------
//	Description:	Load Bin file.
//	Syntax:			INT32U* LoadFILE_BIN(INT8U* pFileName);;
//	Input Paramter(s):
//					INT8U* pFileName		:	Bin file name and source device.                 
//	Return: 		Bin file start address on sdram.
//----------------------------------------------------------------------------------------------------
extern INT32U* LoadFILE_BIN(INT8U* pFileName);

//Extended Sprite Function API
//----------------------------------------------------------------------------------------------------
//	Description:	G+Director c format output attribute initiation for Extend sprite buffer.
//	Syntax:			void set_exsprite_init(INT32U sprite_number,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct start pointer(G+Director xxxxCellIdx.c).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_exsprite_init(INT32U sprite_number,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Display Extend sprite initiation.
//	Syntax:			void set_exsprite_display_init(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct->SpCell for image start pointer(G+Director xxxxCellIdx.c).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     -const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//                  INT16S sprite_pos_x     :   Sprite x position of display screen.
//                  INT16S sprite_pos_y     :   Sprite y position of display screen.
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_exsprite_display_init(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Display sprite image transformation.
//	Syntax:			void set_exsprite_image_number(INT32U sprite_number,INT32U sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//					INT32U sprite_ptr		:	SPRITE struct->SpCell for image start pointer(G+Director).
//                  -typedef struct 
//                   {
//	                     INT16S	nCharNum;                           
//	                     INT16S	nPosX;                             
//	                     INT16S	nPosY;                              
//	                     INT16S	nSizeX;                            
//	                     INT16S	nSizeY;                             
//	                     INT16U	uAttr0;                            
//	                     INT16U	uAttr1;                            
//	                     INT16S	nPosX1;                             
//	                     INT16S	nPosX2;                            
//	                     INT16S	nPosX3;                            
//	                     INT16S	nPosY1;                             
//	                     INT16S	nPosY2;                             
//	                     INT16S	nPosY3;                            
//	                     -const INT32U	*SpCell;                    
//                   } SPRITE, *PSPRITE;
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void set_exsprite_image_number(INT32U sprite_number,INT32U sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Disable display Extend sprite.
//	Syntax:			void set_exsprite_disable(INT32U sprite_number);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//	Return: 		None
//  Note  :         The set_sprite_disable function must be combined with paint_ppu_exspriteram function.
//----------------------------------------------------------------------------------------------------
extern void set_exsprite_disable(INT32U sprite_number);

//----------------------------------------------------------------------------------------------------
//	Description:	Calculate Extend sprite attribute for Extend sprite ram.
//	Syntax:			void paint_ppu_exspriteram(Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  Coordinate_Change_Mode coordinate_mode  :   Sprite coordinate transformation selection.
//                  -typedef enum
//                   {
//		                PPU_hardware_coordinate=0,            
//		                Center2LeftTop_coordinate,             
//		                LeftTop2Center_coordinate              
//                   } Coordinate_Change_Mode;
//
//                  INT16U sprite_number                    :   Maximum of Display sprite number.
//	Return: 		None
//  Note  :         The Display sprite number must be combined with hardware.
//---------------------------------------------------------------------------------------------------- 
extern void paint_ppu_exspriteram(Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number);

//----------------------------------------------------------------------------------------------------
//	Description:	Extend sprite ram initiation.
//	Syntax:			INT32U extend_sprite_malloc_memory(INT16U exsp_num,INT8U sp_cdm_en);
//	Input Paramter(s):
//                  INT16U exsp_num                    :   Maximum of Display sprite number.
//	Return: 		Extend sprite ram address.
//  Note  :         The Display sprite number must be combined with hardware.
//---------------------------------------------------------------------------------------------------- 
extern INT32U extend_sprite_malloc_memory(INT16U exsp_num);

//----------------------------------------------------------------------------------------------------
//	Description:	Obtainment of Extend sprite ram start address and sprite number.
//	Syntax:			void Get_exsprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr);
//	Input Paramter(s):
//					INT32U sprite_number	:	Sprite image number.
//                  SpN_ptr *sprite_ptr     :   Sprite information note of sprite ram start address and sprite number.
//					-typedef struct 
//                   {
//						INT16U	nSP_CharNum;                        
//						INT16U	nSP_Hsize;                         
//						INT16U	nSP_Vsize;                         
//						INT32U	nSPNum_ptr;                        
//						INT32U	nEXSPNum_ptr;          
//					 } SpN_ptr, *PSpN_ptr;  
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void Get_exsprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr);

//----------------------------------------------------------------------------------------------------
//	Description:	Extend Sprite 2D position transformation.
//	Syntax:			void SetEXSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  INT16S x                                :   move range of x direction.
//                  INT16S y                                :   move range of y direction.
//                  INT16U scroll                           :   Display screen boundary decision.
//                  -#define  Pos_Noscroll       0    
//                  -#define  Pos_scroll         1                      
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void SetEXSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll);
 
//----------------------------------------------------------------------------------------------------
//	Description:	Sprite 3D position transformation.
//	Syntax:			void SpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,POS_STRUCT_PTR in);
//	Input Paramter(s):
//					Sprite_Coordinate_Mode display_mode		:	Display screen size selection.
//                  -typedef enum
//                   {
//		                Sprite_Coordinate_320X240=0,
//		                Sprite_Coordinate_640X480,
//		                Sprite_Coordinate_480X234,
//		                Sprite_Coordinate_480X272,
//		                Sprite_Coordinate_720X480,
//	                    Sprite_Coordinate_800X480,
//		                Sprite_Coordinate_800X600,
//		                Sprite_Coordinate_1024X768
//                   } Sprite_Coordinate_Mode;
//                  Coordinate_Change_Mode coordinate_mode  :   Sprite coordinate transformation selection.
//                  -typedef enum
//                   {
//		                PPU_hardware_coordinate=0,            
//		                Center2LeftTop_coordinate,             
//		                LeftTop2Center_coordinate              
//                   } Coordinate_Change_Mode;
//                  SpN_ptr *sprite_ptr     :   Sprite information note of sprite ram start address and sprite number.
//					-typedef struct 
//                   {
//						INT16U	nSP_CharNum;                        
//						INT16U	nSP_Hsize;                         
//						INT16U	nSP_Vsize;                         
//						INT32U	nSPNum_ptr;                        
//						INT32U	nEXSPNum_ptr;          
//					 } SpN_ptr, *PSpN_ptr; 
//                  V3D_POS_STRUCT_PTR in                       :   Position value and group id.
//                  -typedef struct 
//                   {
//                      INT8U group_id;
//                      POS_STRUCT_GP32XXX V3D_POS1;
//                      POS_STRUCT V3D_POS2;  
//                   } V3D_POS_STRUCT, *V3D_POS_STRUCT_PTR;                    
//	Return: 		None
//  Note  :         Sprite V3D position type and group id must be combined with hardware.  
//----------------------------------------------------------------------------------------------------
extern void EXSpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,V3D_POS_STRUCT_PTR in);

#endif
