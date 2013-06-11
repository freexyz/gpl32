#ifndef __GPLIB_PPU_H__
#define __GPLIB_PPU_H__


#include "gplib.h"

#define	B_DEFEN_EN						25
#define	PPU_DEFEN_ENABLE			    (((INT32U) 1)<<B_DEFEN_EN)
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

#define	B_TXT_BITMAP_FLIP				25
#define	TXT_BITMAP_FLIP_DISABLE		    (((INT32U) 0)<<B_TXT_BITMAP_FLIP)
#define	TXT_BITMAP_FLIP_ENABLE			(((INT32U) 1)<<B_TXT_BITMAP_FLIP)
#define	B_TXT_NEWSPECBMP				16
#define	TXT_NEWSPECBMP_DISABLE		    (((INT32U) 0)<<B_TXT_NEWSPECBMP)
#define	TXT_NEWSPECBMP_ENABLE			(((INT32U) 1)<<B_TXT_NEWSPECBMP)
#define	B_TXT_TFTVTQ					13
#define	TXT_TFTVTQ_DISABLE				(((INT32U) 0)<<B_TXT_TFTVTQ)
#define	TXT_TFTVTQ_ENABLE				(((INT32U) 1)<<B_TXT_TFTVTQ)
#define	B_TXT_INTPMODE					10
#define	TXT_INTPMODE0				    (((INT32U) 0)<<B_TXT_INTPMODE)
#define	TXT_INTPMODE1				    (((INT32U) 1)<<B_TXT_INTPMODE)
#define	TXT_INTPMODE2				    (((INT32U) 2)<<B_TXT_INTPMODE)
#define	TXT_INTPMODE3				    (((INT32U) 3)<<B_TXT_INTPMODE)
#define	B_TXT_NEWCMP					9
#define	TXT_NEWCMP_DISABLE				(((INT32U) 0)<<B_TXT_NEWCMP)
#define	TXT_NEWCMP_ENABLE				(((INT32U) 1)<<B_TXT_NEWCMP)
#define	B_TXT_ALPHA					    8
#define	TXT_ALPHA_DISABLE				(((INT32U) 0)<<B_TXT_ALPHA)
#define	TXT_ALPHA_ENABLE				(((INT32U) 1)<<B_TXT_ALPHA)
#define	B_DUAL_BLEND					6
#define	DUAL_BLEND_DISABLE				(((INT32U) 0)<<B_DUAL_BLEND)
#define	DUAL_BLEND_ENABLE				(((INT32U) 1)<<B_DUAL_BLEND)
#define	B_SPRITE_RGBA					3
#define	SPRITE_RGBA_DISABLE				(((INT32U) 0)<<B_SPRITE_RGBA)
#define	SPRITE_RGBA_ENABLE				(((INT32U) 1)<<B_SPRITE_RGBA)
#define	B_TEXT_RGBA				        2
#define	TEXT_RGBA_DISABLE		        (((INT32U) 0)<<B_TEXT_RGBA)
#define	TEXT_RGBA_ENABLE		        (((INT32U) 1)<<B_TEXT_RGBA)
#define	B_FB_LOCK_EN					1
#define	FB_LOCK_DISABLE					(((INT32U) 0)<<B_FB_LOCK_EN)
#define	FB_LOCK_ENABLE					(((INT32U) 1)<<B_FB_LOCK_EN)
#define	B_FREE_INIT					    31
#define	MASK_FREE_INIT_SIZE			    (((INT32U) 0x1)<<B_FREE_INIT)
#define	B_FREE_H_SIZE					16
#define	MASK_FREE_H_SIZE			    (((INT32U) 0x7FF)<<B_FREE_H_SIZE)
#define	B_FREE_V_SIZE					0
#define	MASK_FREE_V_SIZE			    (((INT32U) 0x7FF)<<B_FREE_V_SIZE)

extern INT32S gplib_ppu_init(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=disable 1=enable
extern INT32S gplib_ppu_char0_transparent_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_bottom_up_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=from top to bottom 1=from bottom to top
extern INT32S gplib_ppu_vga_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=QVGA mode 1=VGA mode
extern INT32S gplib_ppu_non_interlace_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// vlaue:0=interlace mode 1=non-interlace mode
extern INT32S gplib_ppu_frame_buffer_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U enable, INT32U select);	// enable:0(select:0=TV is line mode and TFT is frame mode) 1=both TV and TFT are frame buffer mode
extern INT32S gplib_ppu_fb_format_set(PPU_REGISTER_SETS *p_register_set, INT32U format, INT32U mono);	// format:0(mono:0=RGB565 1=Mono 2=4-color 3=16-color) 1(mono:0=RGBG 1=YUYV 2=RGBG 3=YUYV)
extern INT32S gplib_ppu_save_rom_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// vlaue:0=disable save rom mode 1=enable save rom mode
extern INT32S gplib_ppu_resolution_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:see the constants defined below
#define	C_TFT_RESOLUTION_320X240		0
#define	C_TFT_RESOLUTION_640X480		1
#define	C_TFT_RESOLUTION_480X234		2
#define	C_TFT_RESOLUTION_480X272		3
#define	C_TFT_RESOLUTION_720X480		4
#define	C_TFT_RESOLUTION_800X480		5
#define	C_TFT_RESOLUTION_800X600		6
#define	C_TFT_RESOLUTION_1024X768		7
#define	C_TV_RESOLUTION_320X240			0
#define	C_TV_RESOLUTION_640X480			0
#define	C_TV_RESOLUTION_720X480			4
extern INT32S gplib_ppu_yuv_type_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value[1:0]:0=BGRG/VYUY 1=GBGR/YVYU 2=RGBG/UYVY 3=GRGB/YUYV, value[2]:0=UV is unsigned(YCbCr) 1=UV is signed(YUV)
extern INT32S gplib_ppu_tft_long_burst_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable TFT long burst 1=enable TFT long burst
extern INT32S gplib_ppu_long_burst_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable PPU long burst 1=enable PPU long burst
extern INT32S gplib_ppu_blend4_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0~3
extern INT32S gplib_ppu_rgb565_transparent_color_set(PPU_REGISTER_SETS *p_register_set, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~0xFFFF
extern INT32S gplib_ppu_fade_effect_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0~255
extern INT32S gplib_ppu_window_set(PPU_REGISTER_SETS *p_register_set, INT32U window_index, INT32U window_x, INT32U window_y);	// windows_index:0(window 1)~3(window 4), window_x:mask + start_x + end_x, window_y:start_y + end_y
extern INT32S gplib_ppu_palette_type_set(PPU_REGISTER_SETS *p_register_set, INT32U p1024, INT32U type);		// p1024:0~1, type:0~3
extern INT32S gplib_ppu_palette_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U bank, INT32U value);	// bank:0(palette0)~3(palette3), value: 32-bit address of palette ram buffer
extern INT32S gplib_ppu_frame_buffer_add(PPU_REGISTER_SETS *p_register_set, INT32U buffer);		// buffer: 32-bit address of frame buffer
extern INT32S gplib_ppu_deflicker_mode_set(INT32U value);						// value:0=disable deflicker function 1=enable deflicker function after PPU is done
extern INT32S gplib_ppu_go_without_wait(PPU_REGISTER_SETS *p_register_set);		// This function returns immediately if frame buffer is not available or PPU is still busy
extern INT32S gplib_ppu_go(PPU_REGISTER_SETS *p_register_set);					// This function returns when PPU registers are updated, it will not wait for PPU frame buffer output to complete
extern INT32S gplib_ppu_go_and_wait_done(PPU_REGISTER_SETS *p_register_set);	// This function returns when PPU registers are updated and operation is done
extern INT32S gplib_ppu_dual_blend_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_deflicker_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_rgba_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_alpha_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_rgba_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_free_mode_size_rigister_set(INT16U INTL,INT16U H_size,INT16U V_size);
extern INT32S gplib_ppu_text_new_specialbmp_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_new_compression_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_free_size_set(PPU_REGISTER_SETS *p_register_set, INT16U INTL,INT16U H_size,INT16U V_size);
extern INT32S gplib_ppu_text_bitmap_flip_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_go_hblank_first(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_hblnk_set(INT32U value);
extern INT32S gplib_ppu_hblnk_line_offset_set(INT32U line_offset);
extern INT32S gplib_ppu_hblnk_irq_wait(void);
extern INT32S gplib_ppu_hblnk_go_now(PPU_REGISTER_SETS *p_register_set);

#endif 		// __GPLIB_PPU_H__