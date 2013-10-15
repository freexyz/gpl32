/*
* Description: This task is the G+PPUImageConverter text data verification.
*
* Author: Cater Chen
*
* Date: 2008/11/26
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "application.h"
#include "text.h"

#ifndef __TEXT_DEMO_H__
#define __TEXT_DEMO_H__

typedef struct {
	INT16U	nTxAddr_mode;                            //  1.  Character width¡G8, 16, 32, 64, 128, 256
	INT16U	nChar_width;                             //  2.  Character width¡G8, 16, 32, 64, 128, 256
	INT16U	nChar_height;                            //  3.  Character height¡G8, 16, 32, 64, 128, 256
	INT16U	ntext_width;                             //  4.  Width of Text size
	INT16U	ntext_height;                            //  5.  Height of Text size
	INT16U	nImage_width;                            //  6.  Image width  N¡GThe number of images
	INT16U	nImage_height;                           //  7.  Image height N¡GThe number of images
	INT32U  Num_arr_ptr;                             //  8.  Text Number Array pointer 
	INT32U	index_ptr;                               //  9.  Image cell index table pointer
	INT32U	data_ptr;                                //  10. Image cell data table pointer
} nTX_Num_Arr, *PnTX_Num_Arr;

typedef enum
{
		TEXT_DATA_C_CODE=0,
		TEXT_DATA_BIN
} TEXT_DATA_FORMAT;

typedef struct {
	INT16U  image_number;                            //  1. Display of image number
	INT16U  position_x;					             //  2. Display of TEXT X position
	INT16U  position_y;					             //  3. Display of TEXT Y position
	INT16U  position_x_offset;					     //  4. Display of TEXT X position
	INT16U  position_y_offset;					     //  5. Display of TEXT Y position	
	INT16U  pal_bank;                                //  6. Display of TEXT palette bank
	INT32U  nNum_arr_ptr;                            //  7. Text Number Array pointer 
	INT32U	ntext_data_ptr;                          //  8. Image data bin pointer or Image header data
	INT32U	npal_ptr;                                //  9. Image palette bin pointer
	TEXT_DATA_FORMAT  image_type;                    //  10. image date type is the .bin or c code.
} nTX_image_info, *PnTX_image_info;

//----------------------------------------------------------------------------------------------------
//	Description:	Calculate text attribute for ppu.
//	Syntax:			void load_text_data(PPU_REGISTER_SETS *p_register_set,INT32U text_index,nTX_image_info *text_info);
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
//					INT32U text_index                       :   Display text number.
//                  nTX_image_info *text_info		        :	Display text information.
//					-typedef struct {
//						INT16U  image_number;                            
//						INT16U  position_x;					            
//						INT16U  position_y;					             
//						INT16U  position_x_offset;					    
//						INT16U  position_y_offset;					  
//						INT16U  pal_bank;                                
//						INT32U  nNum_arr_ptr;                           
//						INT32U	ntext_data_ptr;                         
//						INT32U	npal_ptr;                             
//						TEXT_DATA_FORMAT  image_type;                 
//					-} nTX_image_info, *PnTX_image_info;
//	Return: 		None
//----------------------------------------------------------------------------------------------------
extern void load_text_data(PPU_REGISTER_SETS *p_register_set,INT32U text_index,nTX_image_info *text_info);

#endif	/* __TEXT_DEMO_H__ */
