/*
 ******************************************************************************
 * fdosd.c
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.09.30	T.C. Chiu	first edition
 *
 *
 ******************************************************************************
 */
#include "application.h"
#include "video_codec_callback.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "SPRITE_fd_HDR.h"

#include "fdosd.h"

#define PPU_TEXT_SIZE_HPIXEL	C_DISPLAY_DEV_HPIXEL
#define PPU_TEXT_SIZE_VPIXEL	C_DISPLAY_DEV_VPIXEL
#define C_PPU_DRV_FRAME_NUM	2

extern PPU_REGISTER_SETS	*video_ppu_register_set;
extern PPU_REGISTER_SETS	turnkey_ppu_register_structure_1;


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void fdosd_ppu_init(void)
{
	INT32U		i;
	INT32U		j;
	INT32U		frame_size;
	INT32U		buff_ptr;

	// initial ppu register parameter set structure
	video_ppu_register_set = (PPU_REGISTER_SETS *) &turnkey_ppu_register_structure_1;
	gplib_ppu_init(video_ppu_register_set);							// initial PPU hardware engine and PPU register set structure
	gplib_ppu_enable_set(video_ppu_register_set, 1);					// enable PPU
	gplib_ppu_char0_transparent_set(video_ppu_register_set, 1);				// enable character 0 transparent mode
	gplib_ppu_non_interlace_set(video_ppu_register_set, 0);					// set non-interlace mode
	gplib_ppu_frame_buffer_mode_set(video_ppu_register_set, 1, 0);				// enable TV/TFT frame buffer mode
	gplib_ppu_fb_format_set(video_ppu_register_set, 1, 1);					// set PPU output frame buffer format to YUYV

	if ((C_DISPLAY_DEV_HPIXEL == 320) && (C_DISPLAY_DEV_VPIXEL == 240)) {
		gplib_ppu_vga_mode_set(video_ppu_register_set, 0);				// disable VGA mode
		gplib_ppu_resolution_set(video_ppu_register_set, C_TV_RESOLUTION_320X240);	// set display resolution to 320X240
	} else if ((C_DISPLAY_DEV_HPIXEL == 640) && (C_DISPLAY_DEV_VPIXEL == 480)) {
		gplib_ppu_vga_mode_set(video_ppu_register_set, 1);				// enable VGA mode
		gplib_ppu_resolution_set(video_ppu_register_set, C_TV_RESOLUTION_640X480);	// set display resolution to 640X480
	} else {
		gplib_ppu_vga_mode_set(video_ppu_register_set, 1);				// enable VGA mode
		gplib_ppu_resolution_set(video_ppu_register_set, C_TV_RESOLUTION_720X480);	// set display resolution to 640X480
	}

	gplib_ppu_bottom_up_mode_set(video_ppu_register_set, 1);				// bottom to top
	gplib_ppu_text_direct_mode_set(video_ppu_register_set, 0);				// TEXT address mode
	gplib_ppu_long_burst_set(video_ppu_register_set, 1);					// enable TFT long burst

	gplib_ppu_deflicker_mode_set(0);							// disable deflicker is only valid under VGA + Interlace + Frame mode
	gplib_ppu_yuv_type_set(video_ppu_register_set, 3);					// set 32-bit color format to Y1UY0V

//	gplib_ppu_dual_blend_set(video_ppu_register_set, 1);
//	gplib_ppu_palette_type_set(video_ppu_register_set, 0, 1);				// text and sprite palette mode

	// frame buffer malloc
	frame_size = PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2;
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		buff_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		DBG_PRINT("ppu_frame = 0x%x\r\n", buff_ptr);
		if (buff_ptr) {
			INT32U *ptr;

			ptr = (INT32U *) buff_ptr;
			for (j=0; j<frame_size; j++) {
				*ptr++ = 0x00800080;						// black for YUYV color mode
			}
			gplib_ppu_frame_buffer_add(video_ppu_register_set, buff_ptr);
		} else {
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
		}
	}

	// setup TEXT1
	buff_ptr = (INT32U) gp_malloc_align(1024*4, 4);
	if (!buff_ptr) {
		DBG_PRINT("fdosd task failed to allocate TEXT1 memory\r\n");
		while (1);
	}
	gplib_ppu_text_enable_set(video_ppu_register_set, C_PPU_TEXT1, 1);			// enable TEXT
	gplib_ppu_text_compress_disable_set(video_ppu_register_set, 1);				// disable TEXT1/TEXT2 horizontal/vertical compress function
	gplib_ppu_text_attribute_source_select(video_ppu_register_set, C_PPU_TEXT1, 1);		// get TEXT attribute from register
	gplib_ppu_text_size_set(video_ppu_register_set, C_PPU_TEXT1, C_TXN_SIZE_512X256);	// set TEXT size to 1024x512
	gplib_ppu_text_segment_set(video_ppu_register_set, C_PPU_TEXT1, 0);			// set TEXT segment address

	gplib_ppu_text_number_array_ptr_set(video_ppu_register_set, C_PPU_TEXT1, buff_ptr);	// set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(video_ppu_register_set, C_PPU_TEXT1, 1);			// enable bitmap mode
	gplib_ppu_text_color_set(video_ppu_register_set, C_PPU_TEXT1, 1, 3);			// set TEXT color to YUYV

	gplib_ppu_text_position_set(video_ppu_register_set, C_PPU_TEXT1, 0, 0);
	gplib_ppu_text_offset_set(video_ppu_register_set, C_PPU_TEXT1, 0, 0);

	// setup TEXT2
	buff_ptr = (INT32U) gp_malloc_align(1024*4, 4);
	if (!buff_ptr) {
		DBG_PRINT("fdosd task failed to allocate TEXT2 memory\r\n");
		while (1);
	}
	gplib_ppu_text_enable_set(video_ppu_register_set, C_PPU_TEXT2, 0);			// enable TEXT
	gplib_ppu_text_attribute_source_select(video_ppu_register_set, C_PPU_TEXT2, 1);		// get TEXT attribute from register
	gplib_ppu_text_size_set(video_ppu_register_set, C_PPU_TEXT2, C_TXN_SIZE_512X256);	// set TEXT size to 1024x512
	gplib_ppu_text_segment_set(video_ppu_register_set, C_PPU_TEXT2, 0);			// set TEXT segment address

	gplib_ppu_text_number_array_ptr_set(video_ppu_register_set, C_PPU_TEXT2, buff_ptr);	// set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(video_ppu_register_set, C_PPU_TEXT2, 1);			// enable bitmap mode
	gplib_ppu_text_color_set(video_ppu_register_set, C_PPU_TEXT2, 1, 0);			// set TEXT color to YUYV

	gplib_ppu_text_position_set(video_ppu_register_set, C_PPU_TEXT2, 0, 0);
	gplib_ppu_text_offset_set(video_ppu_register_set, C_PPU_TEXT2, 0, 0);

//	gplib_ppu_text_calculate_number_array(video_ppu_register_set, C_PPU_TEXT2, 320, 240, (INT32U)_track_IMG0000_CellData);

	// configure sprite
	gplib_ppu_sprite_init(video_ppu_register_set);
	gplib_ppu_sprite_enable_set(video_ppu_register_set, 1);					// enable sprite
	gplib_ppu_sprite_coordinate_set(video_ppu_register_set, 0);				// value:0=center coordinate 1=top-left coordinate
	gplib_ppu_sprite_direct_mode_set(video_ppu_register_set, 0);				// value:0=relative address mode 1=direct address mode
	gplib_ppu_sprite_number_set(video_ppu_register_set, 256);				// value:0(1024 sprites) 1(4 sprites) 2(8 sprites) ... 255(1020 sprites)
	gplib_ppu_sprite_attribute_ram_ptr_set(video_ppu_register_set, (INT32U) SpriteRAM);	// set sprite ram buffer

	gplib_ppu_sprite_rotate_enable_set(video_ppu_register_set, 1);				// value:0=disable 1=enable
	gplib_ppu_sprite_zoom_enable_set(video_ppu_register_set, 1);				// value:0=disable 1=enable
	gplib_ppu_sprite_mosaic_enable_set(video_ppu_register_set, 1);				// value:0=disable 1=enable
	gplib_ppu_sprite_blend_mode_set(video_ppu_register_set, 1);				// value:0=4 level blending mode 1=16 or 64 level blending mode
	gplib_ppu_sprite_interpolation_set(video_ppu_register_set, 1);
	gplib_ppu_sprite_25d_mode_set(video_ppu_register_set, 1);				// value:0=sprite 2D mode 1= sprite 2.5D mode

	gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 0, (INT32U) &_SPRITE_fd_Palette1);	// palette 0 (TEXT), bank:0(palette0)~3(palette3), value: 32-bit address of palette ram buffer
	gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 1, (INT32U) &_SPRITE_fd_Palette1);	// palette 1 (Sprite)
	gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 2, (INT32U) &_SPRITE_fd_Palette3);	// palette 2 (TEXT)
	gplib_ppu_palette_ram_ptr_set(video_ppu_register_set, 3, (INT32U) &_SPRITE_fd_Palette3);	// palette 3 (Sprite)
	gplib_ppu_sprite_segment_set(video_ppu_register_set, (INT32U) _SPRITE_fd_CellData);		// sprite cell data
	set_sprite_init(0, (INT32U) &mode_SP);
	set_sprite_init(1, (INT32U) &char_SP);
	set_sprite_display_init(0,   0,   0, (INT32U) _mode_IMG0002_CellIdx);
	set_sprite_display_init(1, 284,   0, (INT32U) _char_IMG0000_CellIdx);
	set_sprite_display_init(2, 300,   0, (INT32U) _char_IMG0000_CellIdx);

	// configure exsprite
	gplib_ppu_exsprite_enable_set(video_ppu_register_set, 0);				// value:0=disable 1=enable
}

void fdosd_disp_mode(int n)
{
	switch (n) {
	default:
	case 0:	set_sprite_image_number(0, (INT32U) _mode_IMG0000_CellIdx);	break;		// recognition
	case 1:	set_sprite_image_number(0, (INT32U) _mode_IMG0001_CellIdx);	break;		// training
	case 2:	set_sprite_image_number(0, (INT32U) _mode_IMG0002_CellIdx);	break;		// tracking
	case 3:	set_sprite_image_number(0, (INT32U) _mode_IMG0003_CellIdx);	break;		// security level +
	case 4:	set_sprite_image_number(0, (INT32U) _mode_IMG0004_CellIdx);	break;		// security level -
	case 5:	set_sprite_image_number(0, (INT32U) _mode_IMG0005_CellIdx);	break;		// erase
	}
}

void fdosd_2digit_dec(int num)
{
	int	n;

	if (num >= 100) {
		num = 99;
	}

	n = num / 10;
	switch (n) {
	default:
	case 0:	set_sprite_image_number(1, (INT32U) _char_IMG0000_CellIdx);	break;
	case 1:	set_sprite_image_number(1, (INT32U) _char_IMG0001_CellIdx);	break;
	case 2:	set_sprite_image_number(1, (INT32U) _char_IMG0002_CellIdx);	break;
	case 3:	set_sprite_image_number(1, (INT32U) _char_IMG0003_CellIdx);	break;
	case 4:	set_sprite_image_number(1, (INT32U) _char_IMG0004_CellIdx);	break;
	case 5:	set_sprite_image_number(1, (INT32U) _char_IMG0005_CellIdx);	break;
	case 6:	set_sprite_image_number(1, (INT32U) _char_IMG0006_CellIdx);	break;
	case 7:	set_sprite_image_number(1, (INT32U) _char_IMG0007_CellIdx);	break;
	case 8:	set_sprite_image_number(1, (INT32U) _char_IMG0008_CellIdx);	break;
	case 9:	set_sprite_image_number(1, (INT32U) _char_IMG0009_CellIdx);	break;
	}

	n = num % 10;
	switch (n) {
	default:
	case 0:	set_sprite_image_number(2, (INT32U) _char_IMG0000_CellIdx);	break;
	case 1:	set_sprite_image_number(2, (INT32U) _char_IMG0001_CellIdx);	break;
	case 2:	set_sprite_image_number(2, (INT32U) _char_IMG0002_CellIdx);	break;
	case 3:	set_sprite_image_number(2, (INT32U) _char_IMG0003_CellIdx);	break;
	case 4:	set_sprite_image_number(2, (INT32U) _char_IMG0004_CellIdx);	break;
	case 5:	set_sprite_image_number(2, (INT32U) _char_IMG0005_CellIdx);	break;
	case 6:	set_sprite_image_number(2, (INT32U) _char_IMG0006_CellIdx);	break;
	case 7:	set_sprite_image_number(2, (INT32U) _char_IMG0007_CellIdx);	break;
	case 8:	set_sprite_image_number(2, (INT32U) _char_IMG0008_CellIdx);	break;
	case 9:	set_sprite_image_number(2, (INT32U) _char_IMG0009_CellIdx);	break;
	}
}

