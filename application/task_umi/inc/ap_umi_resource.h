#ifndef __AP_UMI_RESOURCE_H__
#define __AP_UMI_RESOURCE_H__


#include "turnkey_umi_task.h"

#ifndef _t_GP_RESOURCE_HEADER
#define _t_GP_RESOURCE_HEADER

typedef struct {
	CHAR  id[4];
	INT32U  version;
	INT32U  reserve0;

	INT16U	irkey_num;
	INT16U	rbkey_state_num;
	INT16U	rbkey_num;
	INT16U	osd_menu_num;
	INT16U	osd_menu_item_num;
	INT16U	main_menu_num;
	INT16U	main_menu_item_num;
	INT16U	palette_item;
	INT16U	image_item;
	INT16U	audio_item;
	INT16U	audio_open_item;
	INT16U	language_num;
	INT16U	string_item;
	INT8U	str_ch_width[20];	//1 byte or 2 byte
	INT16U	font_item;
	INT16U	auto_demo_num;
	INT16U	others_bin_num;

	INT32U	offset_startup_ctrl;
	INT32U	offset_ir_key;
	INT32U	offset_rubber_key;
	INT32U	offset_osd_menu;
	INT32U	offset_main_menu;
	INT32U	offset_popmessage;
	INT32U	offset_audio;
	INT32U	offset_palette;
	INT32U	offset_image;
	INT32U	offset_video;
	INT32U	offset_string[20];
	INT32U	offset_font[20];
	INT32U	offset_sys_image_idx;
	INT32U	offset_factor_default_option;
	INT32U	offset_auto_demo;
	INT32U	offset_others_bin;

	INT32U  reserve1[8];
} t_GP_RESOURCE_HEADER;

#endif //_t_GP_RESOURCE_HEADER



extern INT32S umi_resource_init(void);

extern INT32U resource_version_get(void);

extern INT16U resource_ir_key_num_get(void);
extern void resource_ir_key_map_get(INT8U **keycode, INT32U **key_msg);

extern INT16U resource_rubber_key_num_get(void);
extern INT32S resource_rubber_key_load(INT32U state, INT32U *state_msg_of_rubber_key);

extern INT16U resource_osd_menu_item_num_get(void);
extern INT32S resource_osd_menu_load(INT32U state, t_OSD_MENU_CTRL **osd_menu_ctrl);

extern void resource_palette_status_refresh(void);
extern INT32S resource_palette0_image_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT32S resource_palette1_image_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT32S resource_palette2_image_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT32S resource_palette3_image_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT32S resource_jpeg_image_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT16U resource_ap_image_index_get(INT16U defined_name);
extern INT32S resource_osd_image_load(INT16U index, t_IMAGE_STRUCT *image_header, INT32U *output_buffer);
extern INT32S resource_image_header_load(INT16U index, t_IMAGE_STRUCT *image_header);
extern INT32U resource_osd_image_buff_size_get(t_IMAGE_STRUCT *image_header);

extern INT16U resource_audio_num_get(void);
extern INT32S resource_audio_header_load(INT16U index, t_AUDIO_STRUCT *audio_header);
extern INT32S resource_audio_load(INT32U off_sector, INT32U *buffer, INT32U sector_cnt);

extern INT8U resource_character_width_get(INT16U language);
extern INT32S resource_string_length_get(INT16U language, INT16U index);
extern INT32S resource_string_load(INT16U language, INT16U index, INT8U *buffer);

extern INT32S resource_font_table_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);
extern INT32S resource_font_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);

extern INT8U resource_font_compression_check(void);
extern INT32S resource_multinational_string_length_get(INT16U language, INT16U index);
extern INT32S resource_multinational_string_load(INT16U language, INT16U index, INT8U *buffer);
extern INT32S resource_multinational_font_table_load(INT16U language, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);
extern INT32S resource_multinational_font_load(INT16U language, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);
extern INT32S resource_total_compressive_font_load(INT16U language);
extern INT32S resource_compressive_font_table_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);
extern INT32S resource_compressive_font_load(INT16U language, INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table);

extern INT16U resource_palette_num_get(void);
extern INT32S resource_palette_load(INT16U index, INT16U *buffer);

extern INT32S resource_startup_logo_load(t_STARTUP_STATE_CTRL *startup_ctrl);

extern INT16U resource_main_menu_item_num_get(void);
extern INT32S resource_main_menu_load(INT32U index, t_MAINMENU_STATE_CTRL **main_menu_ctrl);

extern INT32S resource_user_option_load(SYSTEM_USER_OPTION *user_option);

extern INT16U resource_language_num_get(void);

extern INT16U resource_demo_num_get(void);
extern INT32S resource_demo_instruction_load(DPF_DEMO_STRUCT *instr_table , INT16U size);

extern INT32S umi_resource_exit(void);

extern INT32S resource_music_font_load(INT32U index, t_FONT_TABLE_STRUCT *font_table);


#endif  	// __AP_UMI_RESOURCE_H__
