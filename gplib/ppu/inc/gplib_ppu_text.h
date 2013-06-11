#ifndef __GPLIB_PPU_TEXT_H__
#define __GPLIB_PPU_TEXT_H__


#include "gplib.h"


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


#define	B_TXN_PB						22
#define	MASK_TXN_PALETTE_BANK			(((INT32U) 0x3)<<B_TXN_PB)
#define	B_TXN_SPECIAL_EFFECT			20
#define	MASK_TXN_SPECIAL_EFFECT			(((INT32U) 0x3)<<B_TXN_SPECIAL_EFFECT)
#define	B_TXN_WINDOW					17
#define	MASK_TXN_WINDOW					(((INT32U) 0x3)<<B_TXN_WINDOW)
#define	B_TXN_SIZE						14
#define	MASK_TXN_SIZE					(((INT32U) 0x7)<<B_TXN_SIZE)
#define	B_TXN_DEPTH						12
#define	MASK_TXN_DEPTH					(((INT32U) 0x3)<<B_TXN_DEPTH)
#define	B_TXN_PALETTE					8
#define	MASK_TXN_PALETTE				(((INT32U) 0xF)<<B_TXN_PALETTE)
#define	B_TXN_VS						6
#define	MASK_TXN_VS						(((INT32U) 0x3)<<B_TXN_VS)
#define	B_TXN_HS						4
#define	MASK_TXN_HS						(((INT32U) 0x3)<<B_TXN_HS)
#define	B_TXN_FLIP						2
#define	MASK_TXN_FLIP					(((INT32U) 0x3)<<B_TXN_FLIP)
#define	TXN_NO_FLIP						0
#define	TXN_H_FLIP						1
#define	TXN_V_FLIP						2
#define	TXN_HV_FLIP						3
#define	B_TXN_COLOR						0
#define	MASK_TXN_COLOR					(((INT32U) 0x3)<<B_TXN_COLOR)


#define	B_TXN_BLDLVL					10
#define	MASK_TXN_BLDLVL					(((INT32U) 0x3F)<<B_TXN_BLDLVL)
#define	B_TXN_BLDMODE					9
#define	TXN_BLDMODE64_DISABLE			(((INT32U) 0)<<B_TXN_BLDMODE)
#define	TXN_BLDMODE64_ENABLE			(((INT32U) 1)<<B_TXN_BLDMODE)
#define	B_TXN_BLD						8
#define	TXN_BLD_DISABLE					(((INT32U) 0)<<B_TXN_BLD)
#define	TXN_BLD_ENABLE					(((INT32U) 1)<<B_TXN_BLD)
#define	B_TXN_RGBM						7
#define	TXN_RGB15M						(((INT32U) 0x0)<<B_TXN_RGBM)
#define	TXN_RGB15P						(((INT32U) 0x1)<<B_TXN_RGBM)
#define	B_TXN_MODE						5
#define	MASK_TXN_MODE					(((INT32U) 0x3)<<B_TXN_MODE)
#define	B_TXN_MVE						4
#define	TXN_MVE_DISABLE					(((INT32U) 0)<<B_TXN_MVE)
#define	TXN_MVE_ENABLE					(((INT32U) 1)<<B_TXN_MVE)
#define	B_TXN_EN						3
#define	TXN_DISABLE						(((INT32U) 0)<<B_TXN_EN)
#define	TXN_ENABLE						(((INT32U) 1)<<B_TXN_EN)
#define	B_TXN_WALL						2
#define	TXN_WALL_DISABLE				(((INT32U) 0)<<B_TXN_WALL)
#define	TXN_WALL_ENABLE					(((INT32U) 1)<<B_TXN_WALL)
#define	B_TXN_REGM						1
#define	TXN_REGMODE						(((INT32U) 1)<<B_TXN_REGM)
#define	B_TXN_BMP						0
#define	TXN_BMP							(((INT32U) 1)<<B_TXN_BMP)
#define	TXN_CHAR						(((INT32U) 0)<<B_TXN_BMP)


// PPU TEXT-relative APIs: TEXT initiate function
extern INT32S gplib_ppu_text_init(PPU_REGISTER_SETS *p_register_set, INT32U text_index);
// PPU TEXT-relative APIs: TEXT mode selection functions
extern INT32S gplib_ppu_text_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value:0=disable 1=enable
extern INT32S gplib_ppu_text_compress_disable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=allow TEXT1 and TEXT2 to use horizontal/vertical compress 1=only allow 2D and rotate mode for TEXT1 and TEXT2
extern INT32S gplib_ppu_text_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(2D) 1(HCMP/Rotate) 2(VCMP/2.5D) 3(HCMP+VCMP)
extern INT32S gplib_ppu_text_direct_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=relative mode 1=direct address mode
extern INT32S gplib_ppu_text_wallpaper_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_text_attribute_source_select(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value:0=get attribut from TxN_A_PTR 1=get attribut from register
extern INT32S gplib_ppu_text_horizontal_move_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0=disable 1=enable
// PPU TEXT-relative APIs: TEXT input setting functions
extern INT32S gplib_ppu_text_size_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(512x256) 1(512x512) ... 7(4096x4096)
#define	C_TXN_SIZE_512X256				0
#define	C_TXN_SIZE_512X512				1
#define	C_TXN_SIZE_1024X512				2
#define	C_TXN_SIZE_1024X1024			3
#define	C_TXN_SIZE_2048X1024			4
#define	C_TXN_SIZE_2048X2048			5
#define	C_TXN_SIZE_4096X2048			6
#define	C_TXN_SIZE_4096X4096			7
extern INT32S gplib_ppu_text_character_size_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U txn_hs, INT32U txn_vs);	// txn_hs:0~3, txn_vs:0~3
extern INT32S gplib_ppu_text_bitmap_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value:0=character mode 1=bitmap mode
extern INT32S gplib_ppu_text_color_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U rgb_mode, INT32U color);	// rgb_mode:0~1, color=0~3
extern INT32S gplib_ppu_text_palette_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U bank, INT32U palette_idx);	// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_text_segment_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value: 32-bit segment address
extern INT32S gplib_ppu_text_attribute_array_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value: 32-bit pointer to attribute array
extern INT32S gplib_ppu_text_number_array_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value: 32-bit pointer to number array
extern INT8U gplib_ppu_text_number_array_update_flag_get(INT32U text_index);
extern void gplib_ppu_text_number_array_update_flag_clear(void);
extern INT32S gplib_ppu_text_calculate_number_array(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U photo_width, INT32U photo_height, INT32U data_ptr);
// PPU TEXT-relative APIs: TEXT output control functions
extern INT32S gplib_ppu_text_position_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U pos_x, INT32U pos_y);	// pos_x:0~0xFFF, pos_y:0~0xFFF
extern INT32S gplib_ppu_text_offset_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U offset_x, INT32U offset_y);// offset_x:0~0x3FF, offset_y:0~0x3FF
extern INT32S gplib_ppu_text_depth_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0~3
extern INT32S gplib_ppu_text_blend_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U enable, INT32U mode, INT32U value);	// enable:0~1, mode:0(4 level) 1(64 level), value:0~63
extern INT32S gplib_ppu_text_flip_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(no flip) 1(horizontal flip) 2(vertical flip) 3(h+v flip)
extern INT32S gplib_ppu_text_sine_cosine_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT16U r_sine, INT16U r_cosine);	// r_sine:0~0x1FFF, r_cosine:0~0x1FFF
extern INT32S gplib_ppu_text_window_select(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value:0(window1) 1(window2) 2(window3) 3(window4)
extern INT32S gplib_ppu_text_special_effect_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0(no effect) 1(negative color) 2(grayscale) 3(mono color)
extern INT32S gplib_ppu_text_vertical_compress_set(PPU_REGISTER_SETS *p_register_set, INT32U value, INT32U offset, INT32U step);// value:0~0x3FF, offset:0~0x3FF, step:0~0x3FF
extern INT32S gplib_ppu_text_horizontal_move_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);					// value: 32-bit pointer to horizontal move control ram
extern INT32S gplib_ppu_text1_horizontal_compress_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value: 32-bit pointer to TEXT1 horizontal compression control ram
extern INT32S gplib_ppu_text_rotate_zoom_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT16S angle, FP32 factor_k);
extern INT32S gplib_ppu_text3_25d_set(PPU_REGISTER_SETS *p_register_set, INT16S angle, FP32 *factor_ptr);
extern INT32S gplib_ppu_text3_25d_y_compress_set(PPU_REGISTER_SETS *p_register_set, INT32U value);						// value:0~0x3F
extern INT32S Text25D_CosSineBuf_set(PPU_REGISTER_SETS *p_register_set, INT32U CosSineBuf);

#endif 		// __GPLIB_PPU_TEXT_H__