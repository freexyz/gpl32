/*
 ******************************************************************************
 * font.c
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
 *	2013.09.20	T.C. Chiu	first edition
 *
 *
 ******************************************************************************
 */
#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "sprite_dataSP_HDR.h"

#include "text_b1_HDR.h"
#include "sprite_q1_HDR.h"

#define PPU_TEXT_SIZE_HPIXEL	320
#define PPU_TEXT_SIZE_VPIXEL	240
#define C_PPU_DRV_FRAME_NUM	2

static PPU_REGISTER_SETS	ppu_register_structure;
static PPU_REGISTER_SETS	*ppu_register_set;


/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void gpl32_ppu_init(unsigned char resolution)
{
	INT32U		i;
	INT32U		j;
	INT32U		frame_size;
	INT32U		buff_ptr;
	nTX_image_info	image_info;

	// initial ppu register parameter set structure
	ppu_register_set = (PPU_REGISTER_SETS *) &ppu_register_structure;

	// initial PPU hardware engine and PPU register set structure
	gplib_ppu_init(ppu_register_set);

	// now configure PPU software structure
	gplib_ppu_enable_set(ppu_register_set, 1);				// enable PPU

	// TV frame mode
	gplib_ppu_non_interlace_set(ppu_register_set, 0);			// set non-interlace mode
	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 0);		// enable TV/TFT frame buffer mode

	// PPU setting
	gplib_ppu_fb_format_set(ppu_register_set, 1, 1);			// set PPU output frame buffer format to YUYV
	switch (resolution) {
	case 0:		gplib_ppu_vga_mode_set(ppu_register_set, 0);	break;	// disable VGA mode
	default:	gplib_ppu_vga_mode_set(ppu_register_set, 1);	break;
	}
	gplib_ppu_resolution_set(ppu_register_set, resolution);			// set display resolution

	gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);			// bottom to top
	gplib_ppu_long_burst_set(ppu_register_set, 1);

	// frame buffer malloc
	frame_size = PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2;
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		buff_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		DBG_PRINT("ppu_frame = 0x%x\r\n", buff_ptr);
		if (buff_ptr) {
			INT32U *ptr;

			ptr = (INT32U *) buff_ptr;
			for (j=0; j<frame_size; j++) {
				*ptr++ = 0x00800080;	// black for YUYV color mode
			}
			gplib_ppu_frame_buffer_add(ppu_register_set, buff_ptr);
		} else {
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
		}
	}

	gplib_ppu_dual_blend_set(ppu_register_set, 0);

	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buff_ptr = (INT32U) gp_malloc_align(1024*4,4);
	if (!buff_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}
	image_info.image_number	  = 1;
	image_info.position_x	  = 0;
	image_info.position_y	  = 0;
	image_info.pal_bank	  = 0;
	image_info.nNum_arr_ptr	  = buff_ptr;
	image_info.npal_ptr	  = (INT32U) 0;
	image_info.ntext_data_ptr = (INT32U) TEXT_b1_0_Header;
	image_info.image_type	  = TEXT_DATA_C_CODE;
	load_text_data(ppu_register_set, C_PPU_TEXT1, &image_info);
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT1, 0, 0);

	// configure sprite
	gplib_ppu_sprite_init(ppu_register_set);
	gplib_ppu_sprite_enable_set(ppu_register_set, 1);					// enable sprite
	gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);					// value:0=center coordinate 1=top-left coordinate
	gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);					// value:0=relative address mode 1=direct address mode
	gplib_ppu_sprite_number_set(ppu_register_set, 256);					// value:0(1024 sprites) 1(4 sprites) 2(8 sprites) ... 255(1020 sprites)
	gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U) SpriteRAM);		// set sprite ram buffer

	gplib_ppu_sprite_rotate_enable_set(ppu_register_set, 1);				// value:0=disable 1=enable
	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);					// value:0=disable 1=enable
	gplib_ppu_sprite_mosaic_enable_set(ppu_register_set, 1);				// value:0=disable 1=enable
	gplib_ppu_sprite_blend_mode_set(ppu_register_set, 1);					// value:0=4 level blending mode 1=16 or 64 level blending mode
	gplib_ppu_sprite_interpolation_set(ppu_register_set, 1);
	gplib_ppu_sprite_25d_mode_set(ppu_register_set, 1);					// value:0=sprite 2D mode 1= sprite 2.5D mode

	// image and position init
//	gplib_ppu_palette_ram_ptr_set(ppu_register_set, 0, (INT32U) &_sprite_q1_Palette1);	// palette 0 (TEXT), bank:0(palette0)~3(palette3), value: 32-bit address of palette ram buffer
//	gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U) &_sprite_q1_Palette1);	// palette 1 (Sprite)
//	gplib_ppu_palette_ram_ptr_set(ppu_register_set, 2, (INT32U) &_sprite_q1_Palette3);	// palette 2 (TEXT)
//	gplib_ppu_palette_ram_ptr_set(ppu_register_set, 3, (INT32U) &_sprite_q1_Palette3);	// palette 3 (Sprite)
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U) _SPRITE_q1_CellData);		// sprite cell data

	set_sprite_init(0, (INT32U) &line_h_SP);
	set_sprite_init(1, (INT32U) &line_h_SP);
	set_sprite_init(2, (INT32U) &line_v_SP);
	set_sprite_init(3, (INT32U) &line_v_SP);
	set_sprite_init(4, (INT32U) &string_SP);

	set_sprite_display_init(0,   0,   0, (INT32U) _line_h_IMG0000_CellIdx);
	set_sprite_display_init(1,   0, 224, (INT32U) _line_h_IMG0000_CellIdx);
	set_sprite_display_init(2,   0,   0, (INT32U) _line_v_IMG0000_CellIdx);
	set_sprite_display_init(3, 304,   0, (INT32U) _line_v_IMG0000_CellIdx);
	set_sprite_display_init(4,  20,  20, (INT32U) _string_IMG0000_CellIdx);

	// configure exsprite
	gplib_ppu_exsprite_enable_set(ppu_register_set, 0);					// value:0=disable 1=enable
}

/*
 *****************************************************************************
 *
 *
 *
 *****************************************************************************
 */
void gpl32_ppu_demo(void)
{
	INT32U		i, flag1, flag2, color, string, loop_count;
	INT32S		blend_level, sp_blend_level;
	INT32U		sp_num_addr;
	SpN_ptr		sp_ptr;


	// initial display device
	tft_init();
	tft_start(C_DISPLAY_DEVICE);

	gpl32_ppu_init(C_TFT_RESOLUTION_320X240);

	color		= 0;
	string		= 0;
	loop_count	= 0;
	flag1		= 0;
	flag2		= 0;
	blend_level	= 0;
	sp_blend_level	= 0;
	while (1) {
		if (flag1 == 0) {
			if (++blend_level > 63) {
				blend_level	= 63;
				flag1		= 1;
			}
		} else {
			if (--blend_level < 0) {
				blend_level	= 0;
				flag1		= 0;
			}
		}
//		gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT3, 1, 1, 63);

		switch (flag2) {
		case 0:
		default:
			sp_blend_level = 0;
			break;
		case 1:
			if (++sp_blend_level > 16) {
				sp_blend_level	= 16;
				flag2		= 2;
			}
			break;
		case 2:
			if (--sp_blend_level < 0) {
				sp_blend_level	= 0;
				flag2		= 1;
			}
			break;
		}

		// get sprite character number of image and sprite start ptr of sprite ram
		switch (color) {
		case 0:
			set_sprite_image_number(0, (INT32U) _line_h_IMG0000_CellIdx);
			set_sprite_image_number(1, (INT32U) _line_h_IMG0000_CellIdx);
			set_sprite_image_number(2, (INT32U) _line_v_IMG0000_CellIdx);
			set_sprite_image_number(3, (INT32U) _line_v_IMG0000_CellIdx);
			break;
		case 1:
			set_sprite_image_number(0, (INT32U) _line_h_IMG0001_CellIdx);
			set_sprite_image_number(1, (INT32U) _line_h_IMG0001_CellIdx);
			set_sprite_image_number(2, (INT32U) _line_v_IMG0001_CellIdx);
			set_sprite_image_number(3, (INT32U) _line_v_IMG0001_CellIdx);
			break;
		default:
		case 2:
			set_sprite_image_number(0, (INT32U) _line_h_IMG0002_CellIdx);
			set_sprite_image_number(1, (INT32U) _line_h_IMG0002_CellIdx);
			set_sprite_image_number(2, (INT32U) _line_v_IMG0002_CellIdx);
			set_sprite_image_number(3, (INT32U) _line_v_IMG0002_CellIdx);
			break;
		}
		Get_sprite_image_info(0, (SpN_ptr *) &sp_ptr);
		sp_num_addr = sp_ptr.nSPNum_ptr;
		for (i=0; i<sp_ptr.nSP_CharNum; i++) {
			gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *) sp_num_addr, 1, sp_blend_level);
			gplib_ppu_sprite_attribute_flip_set((SpN_RAM *) sp_num_addr, 3);
			sp_num_addr += sizeof(SpN_RAM);
		}
		Get_sprite_image_info(1, (SpN_ptr *) &sp_ptr);
		sp_num_addr = sp_ptr.nSPNum_ptr;
		for (i=0; i<sp_ptr.nSP_CharNum; i++) {
			gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *) sp_num_addr, 1, sp_blend_level);
			gplib_ppu_sprite_attribute_flip_set((SpN_RAM *) sp_num_addr, 3);
			sp_num_addr += sizeof(SpN_RAM);
		}
		Get_sprite_image_info(2, (SpN_ptr *) &sp_ptr);
		sp_num_addr = sp_ptr.nSPNum_ptr;
		for (i=0; i<sp_ptr.nSP_CharNum; i++) {
			gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *) sp_num_addr, 1, sp_blend_level);
			gplib_ppu_sprite_attribute_flip_set((SpN_RAM *) sp_num_addr, 3);
			sp_num_addr += sizeof(SpN_RAM);
		}
		Get_sprite_image_info(3, (SpN_ptr *) &sp_ptr);
		sp_num_addr = sp_ptr.nSPNum_ptr;
		for (i=0; i<sp_ptr.nSP_CharNum; i++) {
			gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *) sp_num_addr, 1, sp_blend_level);
			gplib_ppu_sprite_attribute_flip_set((SpN_RAM *) sp_num_addr, 3);
			sp_num_addr += sizeof(SpN_RAM);
		}


		// get sprite character number of image and sprite start ptr of sprite ram
		switch (string) {
		case 0:
			set_sprite_image_number(4, (INT32U) _string_IMG0000_CellIdx);
			break;
		case 1:
			set_sprite_image_number(4, (INT32U) _string_IMG0001_CellIdx);
			break;
		case 2:
		default:
			set_sprite_image_number(4, (INT32U) _string_IMG0002_CellIdx);
			break;
		}
		Get_sprite_image_info(4, (SpN_ptr *) &sp_ptr);
		sp_num_addr = sp_ptr.nSPNum_ptr;
		for (i=0; i<sp_ptr.nSP_CharNum; i++) {
//			gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *) sp_num_addr, 1, 16);
			gplib_ppu_sprite_attribute_flip_set((SpN_RAM *) sp_num_addr, 3);
			sp_num_addr += sizeof(SpN_RAM);
		}

		// Start PPU and wait until PPU operation is done
		gplib_ppu_go(ppu_register_set);
		paint_ppu_spriteram(ppu_register_set, Sprite_Coordinate_320X240, LeftTop2Center_coordinate, 10);

		OSTimeDly(2);

		// auto-test
		if (++loop_count > 200) {
			loop_count = 0;
			if (flag2) {
				flag2 = 0;
			} else {
				flag2 = 1;
				if (++color > 3) {
					color = 0;
				}
			}

			if (++string > 3) {
				string = 0;
			}
		}
	}
}
