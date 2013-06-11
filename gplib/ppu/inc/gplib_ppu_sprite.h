#ifndef __GPLIB_PPU_SPRITE_H__
#define __GPLIB_PPU_SPRITE_H__


#include "gplib.h"

#define C_PPU_SPRITE_NUMBER				1024

#define	B_TFT_LB						24
#define	PPU_TFT_LONG_BURST				(((INT32U) 1)<<B_TFT_LB)
#define	B_YUV_TYPE						20
#define	MASK_YUV_TYPE					(((INT32U) 0x7)<<B_YUV_TYPE)
#define	B_PPU_LB						19
#define	PPU_LONG_BURST					(((INT32U) 1)<<B_PPU_LB)
#define	B_TFT_SIZE						16
#define	MASK_TFT_SIZE					(((INT32U) 0x7)<<B_TFT_SIZE)
#define	B_SAVE_ROM						15
#define	SAVE_ROM_ENABLE					(((INT32U) 1)<<B_SAVE_ROM)
#define	B_FB_SEL						14
#define	FB_SEL0							(((INT32U) 0)<<B_FB_SEL)
#define	FB_SEL1							(((INT32U) 1)<<B_FB_SEL)
#define	B_SPR_WIN						13
#define	SPR_WIN_DISABLE					(((INT32U) 0)<<B_SPR_WIN)
#define	SPR_WIN_ENABLE					(((INT32U) 1)<<B_SPR_WIN)
#define	B_HVCMP_DIS						12
#define	HVCMP_ENABLE					(((INT32U) 0)<<B_HVCMP_DIS)
#define	HVCMP_DISABLE					(((INT32U) 1)<<B_HVCMP_DIS)
#define	B_FB_MONO						10
#define	MASK_FB_MONO					(((INT32U) 0x3)<<B_FB_MONO)
#define	B_SPR25D						9
#define	PPU_SPR25D_DISABLE				(((INT32U) 0)<<B_SPR25D)
#define	PPU_SPR25D_ENABLE				(((INT32U) 1)<<B_SPR25D)
#define	B_FB_FORMAT						8
#define	PPU_RGB565						(((INT32U) 0)<<B_FB_FORMAT)
#define	PPU_RGBG						(((INT32U) 1)<<B_FB_FORMAT)
#define	B_FB_EN							7
#define	PPU_LINE_BASE					(((INT32U) 0)<<B_FB_EN)
#define	PPU_FRAME_BASE					(((INT32U) 1)<<B_FB_EN)
#define	B_VGA_NONINTL					5
#define	PPU_VGA_INTL					(((INT32U) 0)<<B_VGA_NONINTL)
#define	PPU_VGA_NONINTL					(((INT32U) 1)<<B_VGA_NONINTL)
#define	B_VGA_EN						4
#define	PPU_QVGA						(((INT32U) 0)<<B_VGA_EN)
#define	PPU_VGA							(((INT32U) 1)<<B_VGA_EN)
#define	B_TX_BOTUP						3
#define	TX_UP2BOT						(((INT32U) 0)<<B_TX_BOTUP)
#define	TX_BOT2UP						(((INT32U) 1)<<B_TX_BOTUP)
#define	B_TX_DIRECT						2
#define	TX_RELATIVE_ADDRESS				(((INT32U) 0)<<B_TX_DIRECT)
#define	TX_DIRECT_ADDRESS				(((INT32U) 1)<<B_TX_DIRECT)
#define	B_CHAR0_TRANSPARENT				1
#define	CHAR0_TRANSPARENT_ENABLE		(((INT32U) 1)<<B_CHAR0_TRANSPARENT)
#define	B_PPU_EN						0
#define	PPU_DISABLE						(((INT32U) 0)<<B_PPU_EN)
#define	PPU_ENABLE						(((INT32U) 1)<<B_PPU_EN)


// PPU sprite-relative APIs: global control functions
extern INT32S gplib_ppu_sprite_init(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_sprite_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_coordinate_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=center coordinate 1=top-left coordinate
extern INT32S gplib_ppu_sprite_blend_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=4 level blending mode 1=16 or 64 level blending mode
extern INT32S gplib_ppu_sprite_round_robin_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=normal mode 1=round-robin mode
extern INT32S gplib_ppu_sprite_direct_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=relative address mode 1=direct address mode
extern INT32S gplib_ppu_sprite_zoom_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_rotate_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_mosaic_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_number_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value:0(1024 sprites) 1(4 sprites) 2(8 sprites) ... 255(1020 sprites)
extern INT32S gplib_ppu_sprite_special_effect_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_color_dither_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_25d_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=sprite 2D mode 1= sprite 2.5D mode
extern INT32S gplib_ppu_sprite_window_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable sprite window function 1=enable sprite window function
extern INT32S gplib_ppu_sprite_segment_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value: 32-bit segment address
extern INT32S gplib_ppu_sprite_attribute_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value: 32-bit pointer to sprite attribute ram
// PPU sprite-relative APIs: sprite attribute management functions
extern INT32S gplib_ppu_sprite_attribute_2d_position_set(SpN_RAM *sprite_attr, INT16S x0, INT16S y0);	// x0 and y0 represent sprite top/left or center position. Only 10-bits are valid.
extern INT32S gplib_ppu_sprite_attribute_25d_position_set(SpN_RAM *sprite_attr, POS_STRUCT_PTR position);// position defines four coordinates of the sprite
extern INT32S gplib_ppu_sprite_attribute_rotate_set(SpN_RAM *sprite_attr, INT32U value);				// value:0~63
extern INT32S gplib_ppu_sprite_attribute_zoom_set(SpN_RAM *sprite_attr, INT32U value);					// value:0~63
extern INT32S gplib_ppu_sprite_attribute_color_set(SpN_RAM *sprite_attr, INT32U value);					// value:0(2-bit) 1(4-bit) 2(6-bit) 3(8-bit/5-bit/16-bit/RGBG/YUYV/8+6 blending)
extern INT32S gplib_ppu_sprite_attribute_flip_set(SpN_RAM *sprite_attr, INT32U value);					// value:0(No flip) 1(H-flip) 2(V-flip) 3(H+V-flip)
extern INT32S gplib_ppu_sprite_attribute_character_size_set(SpN_RAM *sprite_attr, INT32U hs, INT32U vs);// hs and vs:0(8) 1(16) 2(32) 3(64)
extern INT32S gplib_ppu_sprite_attribute_palette_set(SpN_RAM *sprite_attr, INT32U bank, INT32U palette_idx);// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_sprite_attribute_depth_set(SpN_RAM *sprite_attr, INT32U value);					// value:0~3
extern INT32S gplib_ppu_sprite_attribute_blend64_set(SpN_RAM *sprite_attr, INT32U enable, INT32U value);// enable:0=disable 1=enable, value:0~63
extern INT32S gplib_ppu_sprite_attribute_blend16_set(SpN_RAM *sprite_attr, INT32U enable, INT32U value);// enable:0=disable 1=enable, value:0~15
extern INT32S gplib_ppu_sprite_attribute_window_set(SpN_RAM *sprite_attr, INT32U value);				// value:0~3
extern INT32S gplib_ppu_sprite_attribute_mosaic_set(SpN_RAM *sprite_attr, INT32U value);				// value:0(no effect) 1(2x2 pixels) 2(4x4 pixels) 3(8x8 pixels)
extern INT32S gplib_ppu_sprite_attribute_charnum_calculate(PPU_REGISTER_SETS *p_register_set, SpN_RAM *sprite_attr, INT32U data_ptr);	// data_ptr points to photo data of the sprite
// PPU sprite-relative APIs: other functions that assist to manage sprite
extern INT32S gplib_ppu_sprite_availible_position_get(PPU_REGISTER_SETS *p_register_set, INT16S *left, INT16S *right, INT16S *top, INT16S *bottom);
extern INT32S gplib_ppu_sprite_visible_position_get(PPU_REGISTER_SETS *p_register_set, INT16S *left, INT16S *right, INT16S *top, INT16S *bottom);
// PPU sprite-relative APIs: sprite image management functions
extern INT32S gplib_ppu_sprite_image_add_to_list(PSPRITE *list, PSPRITE add_start, PSPRITE add_end);
extern INT32S gplib_ppu_sprite_image_remove_from_list(PSPRITE *list, PSPRITE remove_start, PSPRITE remove_end);
extern INT32S gplib_ppu_sprite_image_position_set(PSPRITE sp_img, POS_STRUCT_PTR pos);			// position defines four coordinates of the sprite
extern INT32S gplib_ppu_sprite_image_rotate_set(PSPRITE sp_img, INT32S value);					// value:0~63
extern INT32S gplib_ppu_sprite_image_zoom_set(PSPRITE sp_img, INT32S value);					// value:0~63
extern INT32S gplib_ppu_sprite_image_color_set(PSPRITE sp_img, INT32U value);					// value:0(2-bit) 1(4-bit) 2(6-bit) 3(8-bit/5-bit/16-bit/RGBG/YUYV/8+6 blending)
extern INT32S gplib_ppu_sprite_image_flip_set(PSPRITE sp_img, INT32U value);					// value:0(No flip) 1(H-flip) 2(V-flip) 3(H+V-flip)
extern INT32S gplib_ppu_sprite_image_character_size_set(PSPRITE sp_img, INT32U hs, INT32U vs);	// hs and vs:0(8) 1(16) 2(32) 3(64)
extern INT32S gplib_ppu_sprite_image_palette_set(PSPRITE sp_img, INT32U bank, INT32U palette_idx);	// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_sprite_image_depth_set(PSPRITE sp_img, INT32S value);					// value:0~3
extern INT32S gplib_ppu_sprite_image_blend64_set(PSPRITE sp_img, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~63
extern INT32S gplib_ppu_sprite_image_blend16_set(PSPRITE sp_img, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~15
extern INT32S gplib_ppu_sprite_image_window_set(PSPRITE sp_img, INT32U value);					// value:0~3
extern INT32S gplib_ppu_sprite_image_mosaic_set(PSPRITE sp_img, INT32U value);					// value:0(no effect) 1(2x2 pixels) 2(4x4 pixels) 3(8x8 pixels)
extern INT32S gplib_ppu_sprite_reserved_num_set(PPU_REGISTER_SETS *p_register_set, INT16U number);	// number:0~1024
extern INT16U gplib_ppu_sprite_reserved_num_get(void);
extern INT32S gplib_ppu_sprite_image_to_attribute(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);
extern INT32S gplib_ppu_sprite_image_charnum_calculate(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);
extern INT32S gplib_ppu_sprite_image_position_calculate(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);
extern INT32S gplib_ppu_sprite_sfr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_large_sprite_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_interpolation_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_group_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_extend_attribute_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_interpolation_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_interpolation_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_interpolation_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_large_size_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_large_size_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_large_size_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_group_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_group_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_group_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_fraction_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_fraction_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_fraction_attribute_set(SpN_RAM *sprite_attr, POS_STRUCT_PTR_GP32XXX value);
extern INT32S gplib_ppu_sprite_cdm_attribute_set(SpN_RAM *sprite_attr, INT32U value ,CDM_STRUCT_PTR in);
extern INT32S gplib_ppu_sprite_cdm_attribute_enable_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_cdm_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_number_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_start_address_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
#endif 		// __GPLIB_PPU_SPRITE_H__