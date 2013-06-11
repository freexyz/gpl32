/*
* Description: This file provides APIs to read resource and user config from resource database
*
* Author: Neal Lu
*
* Date: 2008/10/22
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/
#include "ap_umi_resource.h"


#define MAX_SYSTEM_IMAGE			80

t_GP_RESOURCE_HEADER *resource_header;
INT8U *ir_key_code;
INT32U *ir_key_msg_id;
t_OSD_MENU_CTRL *osd_menu_table;
t_MAINMENU_STATE_CTRL *main_menu_table;


INT16U osd_palette_index;
INT16U ap_palette_index;
t_AUDIO_STRUCT audio_header_temp;
INT16U audio_last_idx;
INT16U name2indx[MAX_SYSTEM_IMAGE];


INT32S resource_ir_table_load(void);
INT32S resource_ap_image_index_load(void);
INT32S resource_image_load(INT16U index, t_IMAGE_STRUCT *image_header);


INT32S umi_resource_init(void)
{
	INT32U size;
	INT32U buffer_ptr;

	if (resource_init())
	{
		DBG_PRINT("Failed to get resource_handle in umi_resource_init\r\n");

		return STATUS_FAIL;
	}

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	ir_key_code = NULL;
	ir_key_msg_id = NULL;
	osd_menu_table = NULL;
	main_menu_table = NULL;
	audio_last_idx = NULL;

	osd_palette_index = 0;
	ap_palette_index = 0;
#endif
  	resource_header = (t_GP_RESOURCE_HEADER *) gp_malloc(sizeof(t_GP_RESOURCE_HEADER));
	if (!resource_header)
	{
		DBG_PRINT("Failed to allocate resource_header in umi_resource_init\r\n");

		return STATUS_FAIL;
	}
	if (resource_read( 0, (INT8U *) resource_header, sizeof(t_GP_RESOURCE_HEADER)))
	{
		DBG_PRINT("Failed to read resource_header in umi_resource_init\r\n");
		gp_free((void *) resource_header);
		resource_header = NULL;

		return STATUS_FAIL;
	}

	size = sizeof(t_GP_RESOURCE_HEADER) +
		   (resource_header->irkey_num) +
		   (resource_header->irkey_num * 4) +
		   (sizeof(t_OSD_MENU_CTRL) + sizeof(t_OSD_ICON_CTRL)*(resource_header->osd_menu_item_num)) +
		   (sizeof(t_MAINMENU_STATE_CTRL) + (resource_header->main_menu_item_num) * (sizeof(POS_STRUCT) + sizeof(t_MAINMENU_ITEM_CTRL)));

	buffer_ptr = (INT32U) gp_malloc(size);
	if (!buffer_ptr) {
		DBG_PRINT("Failed to allocate buffer_ptr in umi_resource_init\r\n");
		gp_free((void *) resource_header);
		resource_header = NULL;

		return STATUS_FAIL;
	}
	gp_memcpy((INT8S *) buffer_ptr, (INT8S *) resource_header, sizeof(t_GP_RESOURCE_HEADER));
	gp_free((void *) resource_header);
	resource_header = (t_GP_RESOURCE_HEADER *) buffer_ptr;
#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
	ir_key_code = (INT8U *) (resource_header + 1);
	ir_key_msg_id = (INT32U *) (ir_key_code + resource_header->irkey_num);
	osd_menu_table = (t_OSD_MENU_CTRL *) (ir_key_msg_id + resource_header->irkey_num);
	main_menu_table = (t_MAINMENU_STATE_CTRL *) ((INT32U) osd_menu_table + (sizeof(t_OSD_MENU_CTRL) + sizeof(t_OSD_ICON_CTRL)*(resource_header->osd_menu_item_num)));

	// Load ir key table
	resource_ir_table_load();

	// Load system reserved image name to index table
	resource_ap_image_index_load();
#endif
	return STATUS_OK;
}

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
INT32U resource_version_get(void)
{
	return resource_header->version;
}

INT16U resource_ir_key_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->irkey_num;
}

INT32S resource_ir_table_load(void)
{
	//if no ir key, return fail
	if (!resource_header || resource_header->irkey_num == 0) {
		return STATUS_FAIL;
	}

	//read ir key code
	if (resource_read((INT32U) resource_header->offset_ir_key, (INT8U *) ir_key_code, resource_header->irkey_num))
	{
		DBG_PRINT("Failed to read IR key code in resource_ir_table_load()\r\n");

		return STATUS_FAIL;
	}

	//read ir key event
	if (resource_read((INT32U) (resource_header->offset_ir_key+resource_header->irkey_num), (INT8U *) ir_key_msg_id, (resource_header->irkey_num)*4))
	{
		DBG_PRINT("Failed to read IR key event in resource_ir_table_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

void resource_ir_key_map_get(INT8U **keycode, INT32U **key_msg)
{
	*keycode = ir_key_code;
	*key_msg = ir_key_msg_id;
}

INT16U resource_rubber_key_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->rbkey_num;
}

INT32S resource_rubber_key_load(INT32U state, INT32U *state_msg_of_rubber_key)
{
	INT32U length, offset;
	INT32U i, j;

	if (!resource_header) {
		return STATUS_FAIL;
	}

    length = sizeof(INT32U) * (resource_header->rbkey_num+1);
    if (length == 0)
    {
		return STATUS_FAIL;
	}

	offset = resource_header->offset_rubber_key;
	for (i=0; i < resource_header->rbkey_state_num; i++)
	{
		if (!resource_read(offset, (INT8U *) state_msg_of_rubber_key, length))
		{
			if (state_msg_of_rubber_key[0] == state)
			{
				//get data
				for (j=0; j < resource_header->rbkey_num; j++)
				{
					state_msg_of_rubber_key[j] = state_msg_of_rubber_key[j+1];
				}
				return STATUS_OK;
			}
		}
		else
		{
			DBG_PRINT("Failed to read rubber key map in resource_rubber_key_load()\r\n");

			return STATUS_FAIL;
		}

		offset += length;
	}

	return STATUS_FAIL;
}

INT16U resource_osd_menu_item_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->osd_menu_item_num;
}

INT32S resource_osd_menu_load(INT32U state, t_OSD_MENU_CTRL **osd_menu_ctrl)
{
	INT32U length, offset, i;

	if (!resource_header || !osd_menu_table) {
    	return STATUS_FAIL;
    }

    length = sizeof(t_OSD_MENU_CTRL) + sizeof(t_OSD_ICON_CTRL)*(resource_header->osd_menu_item_num);
	offset = resource_header->offset_osd_menu;
	for (i=0; i < resource_header->osd_menu_num; i++)
	{
		if (!resource_read(offset, (INT8U *) osd_menu_table, length))
		{
			if (osd_menu_table->state_id == state)
			{
				//get data
				*osd_menu_ctrl = (t_OSD_MENU_CTRL *) osd_menu_table;
				return STATUS_OK;
			}
		}
		else
		{
			DBG_PRINT("Failed to load OSD menu in resource_osd_menu_load()\r\n");

			return STATUS_FAIL;
		}

		offset += length;
	}

	return STATUS_FAIL;
}

INT32S resource_osd_image_load(INT16U index, t_IMAGE_STRUCT *image_header)
{
	INT32S result;

	result = resource_image_load(index, image_header);
	if (result != STATUS_OK) {
		return STATUS_FAIL;
	}

	// 0=256 index, 1=alpha channel, 2=RGB1555, 3=JPEG
	if ((image_header->type & 0x3) < 2) {
		if (image_header->index_of_pal && (image_header->index_of_pal!=osd_palette_index)) {
			osd_palette_index = image_header->index_of_pal;
			if (resource_palette_load(osd_palette_index, (INT16U *) turnkey_ppu_register_set->ppu_palette1_ptr) == 0) {
				gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 1, turnkey_ppu_register_set->ppu_palette1_ptr);
			}
		}
	}

	return STATUS_OK;
}

INT32S resource_ap_image_index_load(void)
{
	if (!resource_header) {
		return STATUS_FAIL;
	}

	if (resource_read(resource_header->offset_sys_image_idx, (INT8U *) &name2indx, sizeof(INT16U)*MAX_SYSTEM_IMAGE))
	{
		DBG_PRINT("Failed to read system image index in resource_ap_image_index_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

INT16U resource_ap_image_index_get(INT16U defined_name)
{
	return name2indx[defined_name];
}

INT32S resource_ap_image_load(INT16U index, t_IMAGE_STRUCT *image_header)
{
	INT32S result;

	result = resource_image_load(index, image_header);
	if (result != STATUS_OK) {
		return STATUS_FAIL;
	}

	// 0=256 index, 1=alpha channel, 2=RGB1555, 3=JPEG
	if ((image_header->type & 0x3) < 2) {
		if (image_header->index_of_pal && (image_header->index_of_pal!=ap_palette_index)) {
			ap_palette_index = image_header->index_of_pal;
			if (resource_palette_load(ap_palette_index, (INT16U *) turnkey_ppu_register_set->ppu_palette3_ptr) == 0) {
				gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 3, turnkey_ppu_register_set->ppu_palette3_ptr);
			}
		}
	}

	return STATUS_OK;
}

INT32S resource_image_load(INT16U index, t_IMAGE_STRUCT *image_header)
{
	IMAGE_DECODE_STRUCT decode_image_request;
	INT32U offset;
	INT32U *input_buffer;
	INT32U output_length;
	INT32U *output_buffer;

	if (!resource_header || !image_header) {
		return STATUS_FAIL;
	}

	if (index < 1) {
		image_header->raw_data_offset = NULL;

		return STATUS_FAIL;
	}

	offset = (resource_header->offset_image)+(sizeof(t_IMAGE_STRUCT)*(index-1));
	if (resource_read(offset, (INT8U *) image_header, sizeof(t_IMAGE_STRUCT))) {
		DBG_PRINT("Failed to read image header in resource_image_load()\r\n");
		image_header->raw_data_offset = NULL;

		return STATUS_FAIL;
	}
	if (!image_header->length || !image_header->width || !image_header->height || !image_header->raw_data_offset) {
		DBG_PRINT("Image(index=%d) is incorrect in resource_image_load()\r\n", index);
		image_header->raw_data_offset = NULL;

		return STATUS_FAIL;
	}

	input_buffer = (INT32U *) gp_malloc(image_header->length);
	if (!input_buffer) {
		DBG_PRINT("Failed to allocate input_buffer in resource_image_load()\r\n");
		image_header->raw_data_offset = NULL;

		return STATUS_FAIL;
	}

	if (resource_read(image_header->raw_data_offset, (INT8U *) input_buffer, image_header->length)) {
	    DBG_PRINT("Failed to read input_buffer in resource_image_load()\r\n");
	    gp_free((void *) input_buffer);
	    image_header->raw_data_offset = NULL;

		return STATUS_FAIL;
	}

	if (image_header->type & 0x8000) {	//GPZP
		if ((image_header->type & 0xF)==0x0) {
			output_length = (image_header->width)*(image_header->height);	//256 color
		} else {
			output_length = (image_header->width)*(image_header->height)*2;	//alpha channel
		}

		output_buffer = (INT32U *) gp_malloc(output_length);
		if (!output_buffer) {
			DBG_PRINT("Failed to allocate output_buffer in resource_image_load()\r\n");
			gp_free((void *) input_buffer);
			image_header->raw_data_offset = NULL;

			return STATUS_FAIL;
		}

		if (gpzp_decode((INT8U *) (input_buffer+1), (INT8U *) output_buffer) < 0) {
			DBG_PRINT("Failed to call gpzp_decode() in resource_image_load()\r\n");
			gp_free((void *) input_buffer);
			gp_free((void *) output_buffer);
			image_header->raw_data_offset = NULL;

			return STATUS_FAIL;
		}

		image_header->raw_data_offset = (INT32U) output_buffer;
		gp_free((void *) input_buffer);

	} else if ((image_header->type&0xFF) == 0x3) {		// JPEG file
		INT16U char_x, char_y, width, height;

		char_x = 8 << (image_header->char_size&0x3);
		char_y = 8 << ((image_header->char_size>>2)&0x3);
		width = (image_header->width + char_x - 1) & (~(char_x-1));
		height = (image_header->height + char_y - 1) & (~(char_y-1));
		output_buffer = (INT32U *) gp_malloc((width * height) << 1);
		if(output_buffer == NULL) {
			DBG_PRINT("Failed to allocate output_buffer for JPEG in resource_image_load()\r\n");
			gp_free((void *) input_buffer);
			image_header->raw_data_offset = NULL;

			return STATUS_FAIL;
		}
		decode_image_request.cmd_id = ((umi_current_state_get() - (INT32U) STATE_BASE) << IMAGE_CMD_STATE_SHIFT_BITS) & IMAGE_CMD_STATE_MASK;
		decode_image_request.image_source = (INT32S) input_buffer;
		decode_image_request.source_size = image_header->length;
		decode_image_request.source_type = TK_IMAGE_SOURCE_TYPE_BUFFER;
		switch (image_header->char_size & 0xF) {
		case 0x2:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV8X32;
			break;
		case 0x3:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV8X64;
			break;
		case 0x6:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV16X32;
			break;
		case 0x7:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV16X64;
			break;
		case 0xA:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV32X32;
			break;
		case 0xF:
		default:
			decode_image_request.output_format = C_SCALER_CTRL_OUT_YUYV64X64;
			image_header->char_size = 0x0F;
			break;
		}
		decode_image_request.output_ratio = 0;
		decode_image_request.output_buffer_width = width;
		decode_image_request.output_buffer_height = height;
		decode_image_request.output_image_width = width;
		decode_image_request.output_image_height = height;
		decode_image_request.output_buffer_pointer = (INT32U) output_buffer;
		msgQSend(image_msg_queue_id, MSG_IMAGE_DECODE_REQ, (void *) &decode_image_request, sizeof(IMAGE_DECODE_STRUCT), MSG_PRI_NORMAL);

		image_header->raw_data_offset = (INT32U) output_buffer;

	} else {
		image_header->raw_data_offset = (INT32U) input_buffer;
	}

	return STATUS_OK;
}

INT16U resource_audio_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->audio_open_item;
}

INT32S resource_audio_header_load(INT16U index, t_AUDIO_STRUCT *audio_header)
{
	INT32U offset;

	if (!resource_header || index<1 || !audio_header) {
		return STATUS_FAIL;
	}

	if (index != audio_last_idx)
	{
		//index 0 is instrument file, audio file start at index 1.
		offset = (resource_header->offset_audio)+(sizeof(t_AUDIO_STRUCT)*(index-1));

		offset = (resource_header->offset_audio) + (sizeof(t_AUDIO_STRUCT) * (index-1));

		if (resource_read(offset, (INT8U *) audio_header, sizeof(t_AUDIO_STRUCT))) //read audio header
		{
			DBG_PRINT("Failed to read audio header in resource_audio_header_load()\r\n");

			return STATUS_FAIL;
		}
		gp_memcpy ((INT8S *) &audio_header_temp, (INT8S *) audio_header, sizeof(t_AUDIO_STRUCT));
		audio_last_idx=index;
	}
	else
	{
		gp_memcpy ((INT8S *) audio_header, (INT8S *) &audio_header_temp, sizeof(t_AUDIO_STRUCT));
	}

	return STATUS_OK;
}

INT32S resource_audio_load(INT32U off_sector, INT32U *buffer, INT32U sector_cnt)
{
	if (!buffer || !sector_cnt) {
		return STATUS_FAIL;
	}

	off_sector = (off_sector<<9);	//transfer to offset_byte
	sector_cnt = (sector_cnt<<9);	//transfer to length_byte

	if (resource_read(audio_header_temp.raw_data_offset + off_sector, (INT8U *) buffer, sector_cnt)) //read audio
	{
		DBG_PRINT("Failed to read audio data in resource_audio_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

#if 0
INT32S resource_instrument_load(INT16U instrument_index, INT32U *buffer)
{
	t_AUDIO_STRUCT instrument_header;
	INT32U offset;

	if (!resource_header || !buffer) {
		return STATUS_FAIL;
	}

	//instrument_index is 0
	offset = (resource_header->offset_audio) + sizeof(t_AUDIO_STRUCT)*((INT32U) instrument_index);
	if (resource_read(offset, (INT8U *) &instrument_header, sizeof(t_AUDIO_STRUCT))) 			//read instrument header
	{
		DBG_PRINT("Failed to read instrument header in resource_instrument_load()\r\n");

		return STATUS_FAIL;
	}


	if (resource_read(instrument_header.raw_data_offset, (INT8U *) buffer, instrument_header.length)) //read instrument
	{
		DBG_PRINT("Failed to read instrument data in resource_instrument_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}
#endif
#endif

INT8U resource_character_width_get(INT16U language)
{
	if (!resource_header) {
		return 1;
	}
	return (resource_header->str_ch_width[language-1]);
}

INT32S resource_string_length_get(INT16U language, INT16U index)
{
	INT32U offset;
	t_STRING_STRUCT string_header;

	if (!resource_header || index<1)
	{
		return STATUS_FAIL;
	}

	offset = (resource_header->offset_string[language-1]) + (sizeof(t_STRING_STRUCT)*(index-1));

	if (resource_read(offset, (INT8U *) &string_header, sizeof(t_STRING_STRUCT))) //read string header
	{
		DBG_PRINT("Failed to read string header in resource_string_length_get()\r\n");

		return STATUS_FAIL;
	}
	return (INT32S)((string_header.length)*(resource_header->str_ch_width[language-1]));
}

INT32S resource_string_load(INT16U language, INT16U index, INT8U *buffer)
{
	INT32U offset;
	t_STRING_STRUCT string_header;

	if (!resource_header || index<1 || !buffer) {
		return STATUS_FAIL;
	}

	offset = (resource_header->offset_string[language-1]) + (sizeof(t_STRING_STRUCT)*(index-1));

	if (resource_read(offset, (INT8U *) &string_header, sizeof(t_STRING_STRUCT))) //read string header
	{
		DBG_PRINT("Failed to read string header in resource_string_load()\r\n");

		return STATUS_FAIL;
	}

	if (resource_read(string_header.raw_data_offset, (INT8U *) buffer, (string_header.length)*(resource_header->str_ch_width[language-1]))) //read string
	{
		DBG_PRINT("Failed to read string data in resource_string_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

// lang, font index, need charactor, out font table
INT32S resource_font_table_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table)
{
	INT32U offset;
	t_FONT_STRUCT font_header;

	if (!resource_header || index<1 || !font_table) {
		return STATUS_FAIL;
	}

	offset = (resource_header->offset_font[language-1]) + (sizeof(t_FONT_STRUCT)*(index-1));
	if (resource_read(offset, (INT8U *) &font_header, sizeof(t_FONT_STRUCT))) //read font header
	{
		DBG_PRINT("Failed to read font header in resource_font_table_load()\r\n");

		return STATUS_FAIL;
	}

	//get target charactor table offset
	offset = font_header.raw_data_offset + (target_char*sizeof(t_FONT_TABLE_STRUCT));

	if (resource_read(offset, (INT8U *) font_table, sizeof(t_FONT_TABLE_STRUCT))) //read font table
	{
		DBG_PRINT("Failed to read font table in resource_font_table_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

INT32S resource_font_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table)
{
	INT32U offset, len;
	INT32U *input_buffer;
	t_FONT_STRUCT font_header;

	if (!resource_header || !font_table) {
		return STATUS_FAIL;
	}

	offset = (resource_header->offset_font[language-1]) + (sizeof(t_FONT_STRUCT)*(index-1));
	if (resource_read(offset, (INT8U *) &font_header, sizeof(t_FONT_STRUCT))) //read font header
	{
		DBG_PRINT("Failed to read font header in resource_font_load()\r\n");

		return STATUS_FAIL;
	}

	//get target charactor table offset
	offset = font_header.raw_data_offset + (target_char*sizeof(t_FONT_TABLE_STRUCT));
	if (resource_read(offset, (INT8U *) font_table, sizeof(t_FONT_TABLE_STRUCT))) //read font table
	{
		DBG_PRINT("Failed to read font table in resource_font_load()\r\n");

		return STATUS_FAIL;
	}

	len = font_table->font_height*font_table->bytes_per_line;
	input_buffer = (INT32U *) gp_malloc(len);
	if (!input_buffer)
	{
		DBG_PRINT("Failed to allocate input_buffer in resource_font_load()\r\n");

		return STATUS_FAIL;
	}

	if (resource_read((font_header.raw_data_offset)+(font_table->font_content), (INT8U *) input_buffer, len)) //read font content
	{
	    gp_free(input_buffer);
	    DBG_PRINT("Failed to read font data in resource_font_load()\r\n");

		return STATUS_FAIL;
	}

	font_table->font_content = (INT32U) input_buffer;

	return STATUS_OK;
}

INT16U resource_palette_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->palette_item;
}

INT32S resource_palette_load(INT16U index, INT16U *buffer)
{
	INT32U offset;
	t_PALETTE_STRUCT palette_header;

	if (!resource_header || index<1 || !buffer)
	{
		return STATUS_FAIL;
	}

	offset = (resource_header->offset_palette)+(sizeof(t_PALETTE_STRUCT)*(index-1));

	if (resource_read(offset, (INT8U *) &palette_header, sizeof(t_PALETTE_STRUCT))) //read palette header
	{
		DBG_PRINT("Failed to read palette header in resource_palette_load()\r\n");

		return STATUS_FAIL;
	}

	if (resource_read( palette_header.raw_data_offset, (INT8U *) buffer, 512 )) //read palette
	{
		DBG_PRINT("Failed to read palette data in resource_palette_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
INT32S resource_user_option_load(SYSTEM_USER_OPTION *user_option)
{

	if (!resource_header || !user_option)
	{
		return STATUS_FAIL;
	}

	if (resource_read(resource_header->offset_factor_default_option, (INT8U *) user_option, sizeof(SYSTEM_USER_OPTION)))
	{
		DBG_PRINT("Failed to read user config in resource_user_items_load()\r\n");

		return STATUS_FAIL;
	}


	return STATUS_OK;
}

INT32S resource_startup_logo_load(t_STARTUP_STATE_CTRL *startup_ctrl)
{
	if (!resource_header) {
		return STATUS_FAIL;
	}

	if (resource_read(resource_header->offset_startup_ctrl, (INT8U *) startup_ctrl, sizeof(t_STARTUP_STATE_CTRL))) //read startup control header
	{
		DBG_PRINT("Failed to read startup settings in resource_startup_logo_load()\r\n");

		return STATUS_FAIL;
	}
	return STATUS_OK;
}

INT16U resource_main_menu_item_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->main_menu_item_num;
}

INT32S resource_main_menu_load(INT32U index, t_MAINMENU_STATE_CTRL **main_menu_ctrl)
{
	INT32U length, offset;

	if (!resource_header || !main_menu_table) {
		return STATUS_FAIL;
	}
#if C_MAIN_MENU == C_MAIN_MENU_COVER_FLOW
    length = sizeof(t_MAINMENU_STATE_CTRL) + ((resource_header->main_menu_item_num) * (sizeof(POS_STRUCT) + sizeof(t_MAINMENU_ITEM_CTRL)));
#else
    length = sizeof(t_MAINMENU_STATE_CTRL) + ((resource_header->main_menu_item_num) * sizeof(t_MAINMENU_ITEM_CTRL));
#endif
	offset = (resource_header->offset_main_menu) + length*(index-1);
	if (resource_read(offset, (INT8U *) main_menu_table, length))
	{
		DBG_PRINT("Failed to read main menu settings in resource_main_menu_load()\r\n");

		return STATUS_FAIL;
	}

	*main_menu_ctrl = (t_MAINMENU_STATE_CTRL *) main_menu_table;

	return STATUS_OK;
}

INT16U resource_language_num_get(void)
{
	if (!resource_header) {
		return 0;
	}
	return resource_header->language_num;
}

//>>>>>>>>>>>>>>>> for auto demo >>>>>>>>>>>>>>>>>>
#if DPF_AUTO_DEMO_EN == 1
INT16U resource_demo_num_get(void)
{
	if (!resource_header) 
	{
		return 0;
	}
	
	return resource_header->auto_demo_num;
}

INT32S resource_demo_instruction_load(DPF_DEMO_STRUCT *instr_table , INT16U size)
{

	if (!resource_header) 	
	{
		return STATUS_FAIL;
	}
	
	if (resource_read(resource_header->offset_auto_demo, (INT8U *) instr_table, size)) //read audio header
	{
		DBG_PRINT("Failed to read auto demo info. in resource_demo_instruction_load()\r\n");

		return STATUS_FAIL;
	}

	return STATUS_OK;
}

#endif


INT32S umi_resource_exit(void)
{
	//free header memory
	if (resource_header != NULL) {
		gp_free(resource_header);
		resource_header = NULL;
	}

	//free ir map table memory
	if (ir_key_code) {
		gp_free(ir_key_code);
		ir_key_code = NULL;
	}
	ir_key_msg_id = NULL;
	osd_menu_table = NULL;
	main_menu_table = NULL;

	resource_close();

	return 0;
}
#endif
