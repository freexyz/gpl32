#ifndef __TURNKEY_UMI_TASK_H__
#define __TURNKEY_UMI_TASK_H__
#include "application.h"
#include "nvram_variable.h"
#include "resource_variable.h"

#ifndef _TK_UMI_DEFINE_
#define _TK_UMI_DEFINE_
#define AP_QUEUE_MSG_MAX_LEN    64
#endif

#define UMI_QUEUE_MAX   256
#define AP_QUEUE_MAX    1024
#define UMI_SUSPEND_STACK_SIZE  10
#define UMI_STATE_STACK_SIZE    30
#define UMI_QUEUE_MSG_MAX_LEN  8

extern INT8U  UmiQ_para[UMI_QUEUE_MSG_MAX_LEN];

typedef enum
{
  STATE_NORMAL_MODE=0,
  STATE_RESUME_MODE,
  STATE_NULL_ENTRY_MODE,
  STATE_MODE_MAX
} STATE_ENTRY_MODE_ENUM;

typedef enum {
	EXIT_RESUME=0,  /*no exit*/
    EXIT_SUSPEND,  /* release all resource prepare to entry state suspend mode */   /* after exit, the next state is decision by UMI Task*/
    EXIT_PAUSE_SUSPEND,  /* no release resource suspend, like as suspend for storage INT */  /* after exit, the next state is decision by UMI Task*/
    EXIT_STG_SUSPEND,  /* no release resource suspend, only for storage INT */  /* after exit, the next state is decision by UMI Task*/
    EXIT_USB_SUSPEND,  /* no release resource suspend, only for storage INT */  /* after exit, the next state is decision by UMI Task*/
    EXIT_TO_SHORTCUT,
    EXIT_ALARM_SUSPEND,
    EXIT_BREAK  /* after exit, the next state is define by current state*/
}EXIT_FLAG_ENUM;

typedef enum
{
    STACK_PUSH_OK = 0,
    STACK_POP_OK = STACK_PUSH_OK,
    STACK_PUSH_FAIL,
    STACK_POP_FAIL = STACK_PUSH_FAIL,
    STACK_MONITOR_FAIL
} STACK_STATUS_ENUM;

typedef enum
{
    KeyUnlock,
    KeyLock,
    KeyLock_Max,
    KeyLock_Error=KeyLock_Max
} KEY_LOCK_ENUM;

typedef struct
{
    STATE_ENTRY_MODE_ENUM state_entry_mode;
    void* state_entry_para;
} STATE_ENTRY_PARA;

typedef enum {
    UMI_NORMAL_STATE,
    UMI_SUB_STATE,
    UMI_DUMMY_STATE
} UMI_STATE_TYPE;

typedef enum {
    SOS_NO_ERROR=0,
    SOS_NO_STATE_ASSIGN=0x1,
    SOS_ILL_STATE_ASSIGN,
    SOS_RESET,  /* PC jump 0x00000000 only */
    SOS_RELOAD_RESET,
    UMI_SOS_ENUM_MAX
} UMI_SOS_ENUM;

#ifndef UMI_HOT_KEY_DEF
#define UMI_HOT_KEY_DEF

typedef enum {
    HOT_KEY_NONE=0,
    HOT_KEY_TRIGGLE,
    HOT_KEY_MAX
} UMI_HOT_KEY_ENUM;

#endif //UMI_HOT_KEY_DEF


typedef enum {
    UMI_STATUS_UNLOCK=0,
    UMI_STATUS_LOCK=1
} UMI_STATUS_ENUM;

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
	INT32U	offset_multi_language_str;
	INT32U	offset_multi_language_font;

	INT32U  reserve1[8];
} t_GP_RESOURCE_HEADER;

#endif //_t_GP_RESOURCE_HEADER


extern t_GP_RESOURCE_HEADER *resource_header;

extern INT8U  ApQ_para[AP_QUEUE_MSG_MAX_LEN];

/* extern messages */
extern OS_EVENT    *UMIFsResQ;

/* extern functions */
extern INT8U umi_start_status_get(void);
extern void umi_current_state_set(INT32U current_umi_state);
extern INT32U umi_current_state_get(void);
extern INT16S file_handle_get_by_jpeg_id(INT32U jpeg_id);
extern INT16S file_handle_get_by_video_id(INT32U jpeg_id);
extern INT16S file_handle_close_by_jpeg_id(INT16S file_handle);
extern void suspend_state_initial(INT8U max_stack_size);
extern INT32U suspend_state_stack_pop(void);
extern INT32U suspend_state_stack_push(STATE_ENUM state_msg_id);
extern INT32U suspend_state_stack_size_get(void);

extern void umi_state_stack_initial(INT8U max_stack_size);
extern void suspend_status_suspending(void);
extern void suspend_status_finished(void);
extern INT32U suspend_finish_flag_get(void);
extern INT32U umi_state_stack_push(STATE_ENUM state_msg_id);
extern INT32U umi_state_stack_size_get(void);
extern INT32U umi_current_state_get(void);
extern INT32U umi_previous_state_get(void);
extern INT32U umi_previous_main_state_get(void);
extern INT32U umi_current_main_state_get(void);
extern INT32U umi_preN_main_state_get(INT8U preN_state);
extern INT32U umi_current_no_dummy_state_get(void);
extern void umi_storage_init(void);
extern void umi_storage_nonescan_init(void);
extern void umi_stg_init_flag_set(INT8U TRUE_OR_FALSE);
extern INT8U umi_stg_init_flag_get(void);
extern void umi_null_printf(CHAR *fmt, ...);
extern void umi_calendar_init(void);
extern void umi_open_file_error_escape(void);
extern INT8U umi_leap_flag_get(void);
extern void umi_leap_flag_set(INT8U true_or_flase);
extern EXIT_FLAG_ENUM umi_leaf_state_leap(STATE_ENUM leap_to_leaf_state, EXIT_FLAG_ENUM exit_flag);
extern STATE_ENUM umi_suspend_stack_top_get(void);
extern INT8U umi_flushing_flag_get(void);
extern EXIT_FLAG_ENUM umi_next_state_reg(STATE_ENUM next_state_id, STATE_ENTRY_PARA * entry_para, INT32U para_byte_size, EXIT_FLAG_ENUM exit_flag);
extern INT32U umi_next_state_id_get(void);
extern void umi_next_state_id_reset(void);
extern void umi_service_lock(void);
extern void umi_service_unlock(void);
extern INT32U umi_service_lock_status(void);
extern void umi_apq_refill_reg(INT32U msg_id);
extern void umi_apq_reserve_reg(INT32U msg_id);
extern INT32S umi_Apq_Filter(INT32U filter_msg_id);
extern INT32S umi_Apq_msg_polling(INT32U polling_msg_id, INT32U ms_time_out);
extern INT32S umi_Apq_sniffer(INT32U sniffer_msg_id);
extern INT32S umi_SOS_handle(UMI_SOS_ENUM sos_type);
extern void vga_tft_switch(void);
extern void umi_storage_card_judge(STATE_ENUM state);
extern UMI_STATUS_ENUM umi_lock_status_get(void);


#define C_UMI_TASK_START_PPU_WITHOUT_WAIT		0x00000001
#define C_UMI_TASK_START_PPU_WAIT_AVAILABLE		0x00000002
#define C_UMI_TASK_START_PPU_WAIT_DONE			0x00000003

extern PPU_REGISTER_SETS *turnkey_ppu_register_set;
extern PSPRITE turnkey_sprite_image_head;

extern void ap_umi_ppu_init(void);

extern void ppu_resource_release_table_init(void);
extern INT32S ppu_text_charnum_array_add(INT32U text_index, INT32U ptr);
extern INT32S ppu_text_charnum_array_release(INT32U text_index);
extern INT32S issue_ppu_update_command(INT32U wait);
extern INT32S release_ppu_resource(void *ptr);
extern INT32S umi_task_handle_ppu_done_event(void);

extern INT32S turnkey_sprite_buf_image_add(SPRITE* add_start, SPRITE* add_end);
extern INT32S turnkey_sprite_buf_image_remove(SPRITE* remove_start, SPRITE* remove_end);
extern INT32S turnkey_ppu_sprite_image_total_calculate(void);
extern INT32S turnkey_ppu_sprite_image_to_attribute(void);
extern INT32S turnkey_ppu_sprite_image_charnum_calculate(void);
extern INT32S turnkey_ppu_sprite_image_position_calculate(void);
extern void turnkey_ppu_window_set(INT32U window_index,INT32U pos_x0,INT32U pos_y0,INT32U width,INT32U height,INT32U mask);
extern INT32S tk_umi_ppu_os_init(void);

extern void palette_8bit_to_rgb1555_grayscale(INT16U width, INT16U height, INT8U *src, INT16U *dest, INT8U palette_index);
extern void alpha_channel_to_rgb1555_grayscale(INT16U width, INT16U height, INT16U *src, INT16U *dest, INT8U palette_index);
extern void rgb1555_to_rgb1555_grayscale(INT16U width, INT16U height, INT16U *src, INT16U *dest);
extern void ppu_palette_status_refresh(void);

#define C_CURRENT_DISPLAY_DEVICE_TFT		0
#define C_CURRENT_DISPLAY_DEVICE_TV_VGA		1
#define C_CURRENT_DISPLAY_DEVICE_TV_QVGA	2
#define C_CURRENT_DISPLAY_DEVICE_TV_D1		3
extern INT32U ap_umi_display_device_get(void);
extern INT32S ap_umi_display_device_set(INT32U device);

// APIs provided by ap_umi_timer.c
extern void umi_timer_init(void);
extern INT32S umi_timer_start(INT32U event, INT32U interval, INT8U *timer_id);
extern INT32S umi_timer_stop(INT8U timer_id);

// APIs provided by ap_umi_osd_menu.c
extern void umi_osd_menu_init(void);
extern INT32S umi_osd_menu_enter(void);
extern INT32S umi_osd_menu_state_load(void);
extern INT32S umi_osd_menu_process_message(INT32U *message, KEY_EVENT_ST *key_sts);
extern INT32S umi_osd_menu_process_timer(void);
extern INT32S umi_osd_menu_assign_image(INT32U message, INT8U image_index);
extern INT32S umi_osd_menu_exit(void);
extern void umi_osd_menu_icon_operation(INT32U icon_num,INT32U mode);
extern CHAR* umi_state_id_to_name(STATE_ENUM state_idx);
extern UMI_STATE_TYPE umi_current_state_type_get(void);
// APIs provided by ap_umi_resource.c
typedef struct {
	INT32U state_id;

	INT16U board_image_index;	// 0 means not used
	INT16S board_x;				// left-top X coordinate
	INT16S board_y;				// left-top Y coordinate

	INT8U board_mode;			// 0:always display, 1: auto hide
	INT8U auto_hide_time;		// Time that board will be displayed before hide if no key is pressed. Unit is second
	INT8U board_blending;		// 0~15, 0: Totally transparent, 15: No transparent effect
	INT8U hint_show_mode;		// dispaly close left/middle/right
} t_OSD_MENU_CTRL;

typedef struct {
	INT8U  mode;			// 0= no action, 1=select links to a state, 2=select links to a OSD menu

	INT16U image_index[4];	// Image[0] for Inactive image
							// image[1] for Active image
							// Image[2] for Active image
							// Image[3] for Active image
							// 0 means not used
	INT8U interval;			// Time interval for change image. Unit is second

	INT16S image_x;			// left-top X coordinate
	INT16S image_y;			// left-top Y coordinate

	INT16U sound_index;		// Sound that will be played when key is pressed. 0x0000 means not used

	INT16U hint_index[4];		// 0 means no hint
	INT16S hint_x;			// left-top X coordinate
	INT16S hint_y;			// left-top Y coordinate
	INT8U  hint_color;

	INT32U key_event;
} t_OSD_ICON_CTRL;

typedef struct {
	INT8U mode;						//0: No logo display,
									//1: logo photo,
									//2: logo photo+music,
									//3: logo AVI

	INT8U photo_effect;				//0: No effect,
									//1: Fade,
									//2: Zoom,
									//3: Zoom+Rotate

	INT16U logo_photo_avi_index;	// When mode is 1 or 2, this means photo index. When mode is 3, this means avi index. 0xFF means not used
	INT16U logo_sound_index;		// Sound that will be played when mode is 2. 0xFF means not used
	INT16U reserved;

	INT32U next_state;

} t_STARTUP_STATE_CTRL;

#define C_MAIN_MENU_NORMAL			0
#define C_MAIN_MENU_MIRROR			1
#define C_MAIN_MENU					C_MAIN_MENU_MIRROR

#if C_MAIN_MENU == C_MAIN_MENU_MIRROR
typedef struct {
	INT8U 	display_number;		// Number of items that will be displayed on screen
	INT8U 	item_number;		// Number of available items in this menu
	INT16U 	bg_image_index;		// Background image
	INT16U 	bg_music_index;		// Background music
	INT16S	hint_x;				// left-top X coordinate
	INT16S	hint_y;				// left-top Y coordinate
	INT8U	hint_show_mode;		// dispaly close left/middle/right
	INT8U	hint_color;
} t_MAINMENU_STATE_CTRL;

typedef struct {
	INT8U mode;					// 0: this item selects a state, 1: this item selects a submenu
	INT16U image_index;
	INT16U sound_index;			// Sound that will be played when key is pressed. 0x00 means not used
	INT16U hint_index;			// 0 means no hint
	union {
		INT32U next_state;
		INT32U next_menu_offset;
	} select;
} t_MAINMENU_ITEM_CTRL;
#else
typedef struct {
	INT8U  item_number;			// Number of available items in this menu
	INT8U  hint_show_mode;		// dispaly close left/middle/right
	INT16U bg_image_index;		// Background image
	INT16U bg_music_index;		// Background music
	INT16U reserved;
} t_MAINMENU_STATE_CTRL;

typedef struct {
	INT8U mode;					// 0: this item selects a state, 1: this item selects a submenu
	INT16U image_index[4];
	INT16S image_x;				// Image left-top X coordinate
	INT16S image_y;				// Image left-top Y coordinate
	INT16U sound_index;			// Sound that will be played when key is pressed. 0x00 means not used
	INT16U hint_index;			// 0 means no hint
	INT16S hint_x;				// Hint left-top X coordinate
	INT16S hint_y;				// Hint left-top Y coordinate
	INT8U  hint_color;
	union {
		INT32U next_state;
		INT32U next_menu_offset;
	} select;
} t_MAINMENU_ITEM_CTRL;
#endif

typedef struct {
	INT16U	type;
	INT8U	index_of_pal;
	INT8U	char_size;
	INT32U	length;
	INT16U	width;
	INT16U	height;
	INT32U	raw_data_offset;
} t_IMAGE_STRUCT;


typedef struct {
	INT16U	type;
	INT32U	length;
	INT32U	raw_data_offset;
} t_AUDIO_STRUCT;

typedef struct {
	INT16U	length;
	INT32U	raw_data_offset;
} t_STRING_STRUCT;

typedef struct {
	INT32U	length;
	INT32U	compress_length;	// font compress 1021 neal
	INT32U	raw_data_offset;
} t_FONT_STRUCT;

typedef struct {
	INT8U	font_width;
	INT8U	font_height;
	INT8U	bytes_per_line;
	INT32U	font_content;
} t_FONT_TABLE_STRUCT;

typedef struct {
	INT16U	type;
	INT32U	length;
	INT32U	raw_data_offset;
} t_PALETTE_STRUCT;

typedef struct {
	INT8U delay_second;
	INT8U command;
} DPF_DEMO_STRUCT;



extern INT32S resource_init(void);
extern INT32S resource_read(INT32U offset_byte, INT8U *pbuf, INT32U byte_count);
extern void resource_close(void);

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

// APIs provided by ap_umi_key.c
extern void umi_key_init(void);
extern INT32S umi_key_register(void);
extern void umi_tv_out_key_register(void);
extern void umi_key_transfer(INT32U *message, INT32U key_up, INT32U key_down, INT32U key_left, INT32U key_right, INT32U key_ok);

extern void KeyLock_init(void);
extern void KeyLock_all(void);
extern KEY_LOCK_ENUM KeyLock_Status_get(INT8U key_id);
extern void KeyLock_set(INT8U key_id, KEY_LOCK_ENUM key_lock_cfg);

// turnkey general GUI interface
extern void ap_umi_display_loading_end(void);
extern void ap_umi_display_loading_start(INT16S pos_x, INT16S pos_y);
extern INT32S ap_umi_string_any_start(INT16U pos_x0, INT16U pos_y0, CHAR* string_buff, INT8U string_idx);
extern void ap_umi_string_any_end(INT8U string_idx);
extern INT32S ap_umi_lang_string_any_start(INT16U pos_x0, INT16U pos_y0, CHAR* string_buff, INT8U string_idx, INT8U language);
extern INT32S gui_file_name_shrink_with_ext_name(CHAR * src_str, CHAR * out_str, INT16S out_str_size, INT16S max_char_nums, CHAR * padding_str);
extern INT32S gui_file_name_shrink_by_pixel(CHAR* src_str, CHAR* out_str, INT16S out_str_size,INT16S pixel_w, CHAR* padding_str, INT8U lang, INT8U font_type);
/* turnkey umi task extern parameter */
extern NV_VAL_STRUCT nv_gloable;

/* turnkey umi task extern functions */
extern INT32U nvram_read(void* global_struct, INT32U size);
extern INT32U nvram_store(void* global_struct, INT32U size);
extern INT32S umi_photolist_create(INT8U device);


/* apq manager extern */
#ifndef UMI_HOT_KEY_DEF
#define UMI_HOT_KEY_DEF

typedef enum {
    HOT_KEY_NONE=0,
    HOT_KEY_TRIGGLE,
    HOT_KEY_MAX
} UMI_HOT_KEY_ENUM;

#endif

extern void audio_confirm_process(STAudioConfirm *aud_con);
extern void audio_confirm_handler(STAudioConfirm *aud_con);
extern EXIT_FLAG_ENUM apq_sys_msg_handler(INT32U msg_id);
extern void apq_manager_entry(ST_APQ_HANDLE *apq_handle_cfg);
extern UMI_HOT_KEY_ENUM hot_key_event_flag_get(void);
extern void hot_key_event_flag_set(UMI_HOT_KEY_ENUM hotkey_flag);
extern STATE_ENUM hot_key_event_get(void);
extern void hot_key_disable(void);
extern void hot_key_enable(void);
extern INT8U hot_key_enable_flag_get(void);
extern void hot_key_event_set(STATE_ENUM hotkey_event);
extern INT8U hk_display_switch_flag_get(void);
extern void hk_display_switch_flag_reset(void);
extern void hk_display_switch_flag_set(void);
extern void hot_key_para_set(void *para);
extern void *hot_key_para_get(void);

/* state menu extern */
// menu api
typedef struct
{
  STATE_ENUM state;
} MainMenuIconIDState;


typedef struct
{
  STATE_ENUM state;
} SetupMenuIconIDState;


#define C_MENU_OBJ_INTERVAL				13		/* 10ms * 2 = 20ms */
#define C_MENU_OBJ_TIMES				7		/* 20ms * 5 = 100ms */

#define C_DISPLAY_INVERTED_IMAGE		0x00000001
#define C_DISPLAY_BACKGROUND_IMAGE		0x00000002
#define C_DISPLAY_USER_BG_IMAGE			0x00000004
#define C_SET_AS_MAIN_MENU				0x00000010
#define C_SET_ICON_CYCLE				0x00000020
#define C_SET_AS_NUMBER_SELECT			0x00000040
#define C_DYNAMIC_ICON_POSITION			0x00000100
#define C_SUPPORT_SECOND_ICON_IMAGE		0x00000200
#define C_RIGHT_ROTATE_90				0x00000400
#define C_RIGHT_ROTATE_270				0x00000800    //martymei 1030

#define C_REVISE_POSITION_MASK			0x0000000F
#define C_IMAGE_ROTATE_MASK				0x000000F0
#define C_IMAGE_ROTATE_SFT				4

typedef enum
{
	PHOTO_ROTATE_0 = 0,
	PHOTO_ROTATE_90,
	PHOTO_ROTATE_180,
	PHOTO_ROTATE_270
} ORIENTATION_ENUM;

extern ORIENTATION_ENUM orientation_array[8];

// for user input paremeter
// sprite image
typedef struct
{
	INT32U	revise_position;
	INT16U	image_width;
	INT16U	image_height;
	INT16U	revise_width;
	INT16U	revise_height;
	INT8U	character_x_size;
	INT8U	character_y_size;
	INT8U	image_color_set;
	INT8U	image_palette_set;
	INT8U	*p_image_data;
} STMenuImage;

// for menu api
typedef struct
{
	POS_STRUCT	image_position;
	POS_STRUCT	inverted_image_position;
	POS_STRUCT	left_position_step[C_MENU_OBJ_TIMES];
	POS_STRUCT	left_inverted_position_step[C_MENU_OBJ_TIMES];
	POS_STRUCT	right_position_step[C_MENU_OBJ_TIMES];
	POS_STRUCT	right_inverted_position_step[C_MENU_OBJ_TIMES];
	INT32U		depth;
	STMenuImage	image;						/* point to icon image */
	STMenuImage	image2;						/* point to icon image 2 */
} STMenuIcon;

typedef struct ST_MENU_OBJ	STMenuObj;
typedef struct ST_MENU_OBJ	*hMenuObj;

typedef INT32S (*CL2_MENU_LOAD_IMAGE_FUNC)(hMenuObj hMenu, INT32U image_id, STMenuImage *image);
typedef INT32S (*CL2_MENU_FREE_IMAGE_FUNC)(hMenuObj hMenu, STMenuImage *image);

struct ST_MENU_OBJ
{
	INT16S	menu_x_pos;
	INT16S	menu_y_pos;
	INT16U	menu_width;
	INT16U	menu_height;
	INT16U	icon_number;
	INT32U	image_number;
	INT16U	selected_icon_id;
	INT32U	selected_image_id;
	INT16U	interval;			// move once time
	INT16U	times;				// total move times
	INT16U	now_time;			// now move time count
	INT32U	flag;				// decide if display inverted image,
								// decide if cover image is provide by user or create by this api
								// decide if the image struct link is the ring link or not
								// decide if use user provided background image
	STMenuIcon	*pstMenuIcon;	// point to the menu icon
	INT8U		BackgroundColor;
	INT32U		*pBackgroundBuffer;
	INT32U		*pBgImageNArray;
	STMenuImage	BgImage;		// background image
	INT32U		cover_image[64 * 64 / 2];
	SPRITE		*pSprite;		// point to the sprite image link
	SPRITE		SpriteStart;	// the start of sprite image link
	SPRITE		SpriteEnd;		// the end of sprite image link
#if 0
	INT8U		text_id;		// background text id
#endif
	INT8U		sprite_depth;	// sprite depth set
	INT8U		window_id;		// sprite and text used window id
	INT8U		move_mode;
	STMenuImage	temp_image;		// for move image temp
	STMenuImage	temp_image2;	// for move image temp

	// for number select and string select
	INT32S		start_image_id;
	INT32S		end_image_id;
	INT16U		icon_width;		// icon width
	INT16U		icon_height;	// icon height
	INT8U		language;		// language
	INT8U		font_type;		// font type
	INT8U		font_color;		// font color
	// for string select only
	CHAR		**p_string;

	INT16S		yRange;
	INT8U		blending_level;
	INT32U		top_cover_image_buffer[8 * 8 / 4];

	CL2_MENU_LOAD_IMAGE_FUNC	cl2_menu_load_image;
	CL2_MENU_FREE_IMAGE_FUNC	cl2_menu_free_image;
};

extern hMenuObj cl2_menu_create(
	INT16S		menu_x_pos,				// menu x position
	INT16S		menu_y_pos,				// menu y position
	INT16U		MenuWidth, 				// menu width
	INT16U		MenuHeight, 			// menu height
	INT32U		ImageNumber,			// total image for menu display
	INT16U		IconNumber,				// total icon
	POS_STRUCT	*IconPosition,			// icon position set
	INT8U		BackgroundColor,		// background color
	INT16U		SelectedIconId,			// default selected icon index
	INT32U		SelectedImageId,			// start image index
	CL2_MENU_LOAD_IMAGE_FUNC	cl2_menu_load_image,
	CL2_MENU_FREE_IMAGE_FUNC	cl2_menu_free_image,
	INT32U		flag					// control flag
);

hMenuObj cl2_menu_create_ex(
	INT16S		menu_x_pos,				// menu x position
	INT16S		menu_y_pos,				// menu y position
	INT16U		MenuWidth, 				// menu width
	INT16U		MenuHeight, 			// menu height
	INT32U		ImageNumber,			// total image for menu display
	INT16U		IconNumber,				// total icon
	POS_STRUCT	*IconPosition,			// icon position set
	STMenuImage	*pBgImage,				// background image
	INT8U		bg_color,				// background color
	INT16U		SelectedIconId,			// default selected icon index
	INT32U		SelectedImageId,		// select image index

	INT32S		StartImageId,			// start image index
	INT32S		EndImageId,				// end of image index
	INT16U		icon_width,				// icon width
	INT16U		icon_height,			// icon height
	INT8U		language,				// language
	INT8U		font_type,				// font type
	INT8U		font_color,				// font color
	CHAR		**p_string,

	CL2_MENU_LOAD_IMAGE_FUNC	cl2_menu_load_image,
	CL2_MENU_FREE_IMAGE_FUNC	cl2_menu_free_image,
	INT32U		flag					// control flag
);

hMenuObj cl2_menu_init(
	INT16S		menu_x_pos,				// menu x position
	INT16S		menu_y_pos,				// menu y position
	INT16U		MenuWidth, 				// menu width
	INT16U		MenuHeight, 			// menu height
	INT32U		ImageNumber,			// total image for menu display
	INT16U		IconNumber,				// total icon
	POS_STRUCT	*IconPosition,			// icon position set
	STMenuImage	*pBgImage,				// background image
	INT8U		BackgroundColor,		// background color
	INT16U		SelectedIconId,			// default selected icon index
	INT32U		SelectedImageId,		// start image index
	INT32S		StartImageId,			// start image index
	INT32S		EndImageId,				// end of image index
	INT16U		icon_width,				// icon width
	INT16U		icon_height,			// icon height
	INT8U		language,				// language
	INT8U		font_type,				// font type
	INT8U		font_color,				// font color
	CHAR		**p_string,				// the string you want to display
	INT16U		move_steps,				// move left/right steps count
	CL2_MENU_LOAD_IMAGE_FUNC	cl2_menu_load_image,
	CL2_MENU_FREE_IMAGE_FUNC	cl2_menu_free_image,
	INT32U		flag					// control flag
);

extern void cl2_menu_show(hMenuObj hMenu);
extern void cl2_menu_load_all_icon_image(hMenuObj hMenu);
extern INT32S cl2_menu_icon_move_left(hMenuObj hMenu);
extern INT32S cl2_menu_icon_move_left_end(hMenuObj hMenu);
extern INT32S cl2_menu_icon_move_right(hMenuObj hMenu);
extern INT32S cl2_menu_icon_move_right_end(hMenuObj hMenu);
extern INT32S cl2_menu_set_selected_image_id(hMenuObj hMenu, INT32S new_image_id);
extern INT32S cl2_menu_get_selected_image_id(hMenuObj hMenu);
extern INT32S cl2_menu_get_image_number(hMenuObj hMenu);
extern INT32S cl2_menu_appear(hMenuObj hMenu);
extern INT32S cl2_menu_disappear(hMenuObj hMenu);
extern INT32S cl2_menu_destroy(hMenuObj hMenu);
extern INT32S cl2_menu_update_icon_position(hMenuObj hMenu, INT16U icon_id);
extern INT32S cl2_menu_fade_out(hMenuObj hMenu);
extern void cl2_menu_zoom(hMenuObj hMenu, INT16S x, INT16S y, INT16S c_step,INT16S c_total);
extern void cl2_menu_load_image_to_end_of_sprite(hMenuObj hMenu, STMenuImage *p_image, POS_STRUCT *pos);
extern INT32S cl2_menu_add_image(hMenuObj hMenu, INT32U image_id, INT32U image_offset, STMenuImage *p_image);
extern INT32S cl2_menu_remove_image(hMenuObj hMenu, INT32U image_id, INT32U image_offset);
extern INT32S cl2_menu_change_icon_step_position(hMenuObj hMenu, INT32U mode);

extern void cl2_menu_icon_zoom(hMenuObj hMenu,INT16U image_id, INT16S x, INT16S y, INT16S c_step,INT16S c_total);


// number select and string select function
extern hMenuObj cl2_number_select_create(
	INT16S		x_pos,				// menu x position
	INT16S		y_pos,				// menu y position
	INT16U		width, 				// menu width
	INT16U		height, 			// menu height
	INT32S		start_number,		// start number
	INT32S		end_number,			// end number
	INT32S		select_number,		// select number, after create, this number will locate in select bar place
	INT8U		font_color,			// font color
	INT8U		bg_color,			// background color
	STMenuImage	*pBgImage,			// background image
	INT8U		language,			// language
	INT8U		font_type,			// font type
	INT32U		flag
);

extern hMenuObj cl2_string_select_create(
	INT16S		x_pos,				// menu x position
	INT16S		y_pos,				// menu y position
	INT16U		width, 				// menu width
	INT16U		height, 			// menu height
	CHAR		**p_string,			// the string array you want to display
	INT16U		n_string, 			// the string number
	INT16U		select_string_id,	// the select string index
	INT8U		font_color,			// font color
	INT8U		bg_color,			// background color
	STMenuImage	*pBgImage,			// background image
	INT8U		language,			// language
	INT8U		font_type,			// font type
	INT32U		flag
);

// set the window of number select obj and string select obj group
// when create multiple number select or string select obj, it must set the window for these objects.
extern void cl2_number_select_window_set(hMenuObj hMenu, INT16S x_pos, INT16S y_pos, INT16S width, INT16S height);

extern void _int_to_string(INT32S v, INT8U len, CHAR* p_string);
extern void int_to_string(INT32S v, CHAR* p_string);
extern void _int_to_string_no_end(INT32S v, INT8U len, CHAR* p_string);

extern INT32S check_avi_file(INT8U *extname);

/* state icon show  extern */

/* state calendar extern */
//
typedef struct
{
    INT16U year;
    INT8U  month;
    INT8U  month_day;
    INT8U  hour;
    INT8U  minute;
    INT8U  second;   //Revered
    INT8U  week_day;
}CalendarTime;

typedef struct
{
     INT16U position_x;
     INT16U position_y;
     INT16U color;
     INT8U font_type;
	 INT8U language;
}CalendarCharAttribute;

typedef struct
{
    INT8U week_mode;  //0 -> sunday first,1 ->monday first
    INT8U width_space;
    INT8U height_space;
	INT16U sunday_color;
	INT16U normal_color;
	INT16U highlight_color;
	INT16U highlight_line_width;
	INT16U highlight_y_offset;
    CalendarCharAttribute *week;
}CalendarAttribute;

typedef struct{
	unsigned int pole_X[60];
	unsigned int pole_Y[60];
}clockAxis;

typedef struct{
	INT16U circularX;
	INT16U circularY;
	INT16U R;
	INT16U Min_polecolor;
	INT16U Sec_polecolor;
	INT16U background;
}CL1_AclockPara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U  font_type;
	INT16U mode;
	INT16U AMPM_font_type;
	INT16U sec_mode;
	INT16U space_length;
}CL1_DclockPara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U  font_type;
	INT16U space_mode;//space_mode = 0: Null; space_mode = 1: "-" ; space_mode = 2: ","
	INT16U simple_mode;
	INT16U space_length;//space length
	INT8U  Language;    //0x7f:chinese language 0x80:Europe language
	INT8U  sequence_mode;//sequence_mode = 0: YMD; sequence_mode = 1:DMY; sequence_mode = 2:MDY;
	INT8U  caps_mode;//caps_mode = 0:first word£»caps_mode = 1: All words
	INT8U  abbreviate_mode;//abbreviate_mode	= 0:short name£»abbreviate_mode= 0:full name£»
}CL1_DatePara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U  font_type;
}CL1_TimePara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U font_type;
}YearPara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U font_type;
	INT8U  character_mode;//character_mode = 0 english; character_mode = 1 digtial number
	INT8U  caps_mode;//caps_mode = 0:first word£»caps_mode = 1: All words
	INT8U  abbreviate_mode;//abbreviate_mode	= 0:short name£»abbreviate_mode= 0:full name£»
}MonthPara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U font_type;
}DayPara;

typedef struct{
	INT16U DX;
	INT16U DY;
	INT16U Dcolor;
	INT16U Dbackground;
	INT8U  font_type;
	INT8U  caps_mode;//caps_mode = 0:first word£»caps_mode = 1: All words
	INT8U  abbreviate_mode;//abbreviate_mode	= 0:short name£»abbreviate_mode= 0:full name£»
}WeekPara;

typedef struct{
	INT16U circularX;
	INT16U circularY;
	INT16U Bitmap_PosX;
	INT16U Bitmap_PosY;
	INT16U R;
	INT16U Min_polecolor;
	INT16U Sec_polecolor;
	INT16U background;
}CL2_AclockPara;

typedef struct
{
	INT32S twelvehour;
	INT32S ampm_mode; //am = 0;pm = 1
} twelvehourmode;

typedef struct{
	INT16U pos_x;
	INT16U pos_y;
}Stringpos;

extern void cl1_calendar_twobuf_update(GraphicBitmap *bitmap,CalendarAttribute *monthday);//,CalendarTime *CalendarTime)
extern void cl1_calendar_init(GraphicBitmap *bitmap,CalendarAttribute *weekday,CalendarAttribute *monthday);//,CalendarTime *CalendarTime);
extern void cl1_calendar_update(GraphicBitmap *bitmap,CalendarAttribute *monthday);//,CalendarTime *CalendarTime);
extern void cl1_calendar_exit(GraphicBitmap *bitmap);

extern void 	CL1_DClock_Initialize(GraphicBitmap *bitmap,CL1_DclockPara *ptr);
extern void 	CL1_Date_Update(GraphicBitmap *bitmap,CL1_DatePara *CL1_Date_Para);
extern INT16U 	CL1_Date_Initialize(GraphicBitmap *bitmap,CL1_DatePara *CL1_Date_Para);
extern void 	CL1_AClock_Update(GraphicBitmap *bitmap,CL1_AclockPara *ptr);
extern void 	CL1_AClock_Initialize(GraphicBitmap *bitmap,CL1_AclockPara *ptr);
extern INT32U 	CL1_DClock_Update(GraphicBitmap *bitmap,CL1_DclockPara *ptr);
extern void 	CL1_DClock_release(void);
extern void 	CL1_week_release(void);
extern void 	CL1_year_show(GraphicBitmap *bitmap,YearPara *Year_Para);
extern void 	CL1_month_show(GraphicBitmap *bitmap,MonthPara *Month_Para);
extern void 	CL1_month_initi(MonthPara *Month_Para);
extern void 	CL1_month_release(void);
extern INT32U 	CL1_week_show(GraphicBitmap *bitmap,WeekPara *Week_Para);
extern void 	CL1_week_initi(WeekPara *Week_Para);
extern void 	CL1_day_show(GraphicBitmap *bitmap,DayPara *Day_Para);
extern void 	state_calendar_set_interval_entry(void* para1);
extern twelvehourmode hour_24change12(INT32S twentymhour);
extern INT32S  hour_12change24(twelvehourmode twelvehour);
extern INT16S	CL1_time_update(GraphicBitmap *bitmap,CL1_TimePara *CL1_Time_Para);

extern INT32S 	Check_time_range(TIME_T  *tm);
extern void 	CL1_time_init(CL1_TimePara *CL1_Time_Para);
extern void 	CL1_time_release(void);

extern INT16S 	CL1_time_update_settime(GraphicBitmap *bitmap,CL1_TimePara *CL1_Time_Para,TIME_T sTimeT);
extern void 	CL1_AClock_Update_settime(GraphicBitmap *bitmap,CL1_AclockPara *ptr,TIME_T sTimeT);
extern INT16S CL1_time_get_length(CL1_TimePara *CL1_Time_Para);

/*== analog clock m1 ==*/
#define	ACLOCK_IMG_NUM		5//aclock_image_num
#define ACLOCK_APPEAR		1
#define ACLOCK_DISAPPEAR	0

typedef	struct{
	INT8U	aclock_char_size;
	INT16U	aclock_img_type;
	INT16U	aclock_image_width;
	INT16U	aclock_image_length;
} ImgeHeder;

typedef struct{
	INT16U	aclock_x;
	INT16U	aclock_y;
	SPRITE	Clock_Sprite[ACLOCK_IMG_NUM];
	INT16U	AclockImageName[ACLOCK_IMG_NUM];
	ImgeHeder imageheader[ACLOCK_IMG_NUM];
	INT8U	*data_ptr[ACLOCK_IMG_NUM];
	Stringpos  clock_position[ACLOCK_IMG_NUM+1];
	INT8U	flag;
	INT16U	image_depth;
	INT16U  rotate;
} AclockParameter;

extern void 	cl2_aclock_destroy(AclockParameter *aclock_parameter);
extern INT32S	clock_background_update_m1(AclockParameter *aclock_param,INT16U rotate);
extern	AclockParameter *cl2_aclock_init(INT16S	x,INT16S y,INT16U backgroundimg,
									INT16U hourimg,INT16U minuteimg,
									INT16U secondimg,INT16U centerimg,INT16U depth,INT16U rotate);
extern INT32S cl2_analog_clock_update_m1(AclockParameter *aclock_param);
/*== analog clock m1 ==*/
extern void calendar_set_interval_time(INT16U interval);
extern INT16U 	calendar_get_interval_time(void);


extern void state_startup_entry(void* para1);
extern void state_icon_show_entry(void* para1);
extern void state_menu_entry(void* para1);
extern void state_music_entry(void* para1);
extern void state_pets_entry(void* para1);
extern void state_parrot_entry(void* para1);
extern void state_photo_view_entry(void* para1);
extern void state_slide_show_entry(void* para1);
extern void state_calendar_entry(void* para1);
extern void state_setup_entry(void* para1);
extern void state_setting_entry(void* para1);
//extern void state_slide_show_set_interval_entry(void* para1);
extern void state_time_interval_entry(void* para1);
extern void state_usb_entry(void* para1);
extern void state_mirror_entry(void* para1);
extern void state_brightness_control_entry(void* para1);
extern void state_volume_control_entry(void * para1);
extern void state_mass_product_test_entry(void * para1);
extern void state_powertimeset_entry(void* para1);
extern void state_date_set_entry(void * para1);
extern void state_alarm_set_entry(void * para1);
extern void state_item_selection_entry(void * para1);
extern void state_international_control_entry(void* para1);
extern void state_pop_message_entry(void* para1);
extern void state_alarm_happen_entry(void* para1);
extern void state_interrogation_entry(void* para1);
extern void state_save_as_logo_entry(void* para1);
/*== state slide show ==*/
extern void slide_show_set_interval_time(INT16U interval);
extern INT16U slide_show_get_interval_time(void);
//extern void slide_show_music(void);
extern void ap_media_on_off(void);
extern void ap_media_vol_inc(void);
extern void ap_media_vol_dec(void);
extern void ap_media_play(void);


extern ALARM_INFO ALARM_ARRAY[7];
extern INT32U alarm_keyofflg;
extern void ap_Check_alarm_num_and_time(void);

//#define 	TRIGGER_ALARM				0x00aa
//#define 	TRIGGER_DAY					0x5a5a

#define 	ALARM_OFF					0
#define		ALARM_ON					1

typedef struct{
	INT8U	hour;
	INT8U	minute;
}TIME_COMPARE;

#define     REPEAT_OFF					0
#define     REPEAT_ONCE					1
#define     REPEAT_WEEKDAYS				2
#define     REPEAT_WEEKEND				3
#define     REPEAT_EVERYDAY				4

#define INVALID_ALARM           0x0
#define POWER_ON_ALARM      0x1
#define POWER_OFF_ALARM    0x2
#define TIME_ALARM               0x4


/*== state icon show ==*/
typedef struct
{
    MSG_AUD_ENUM result_type;
    INT32S result;
} ICON_SHOW_HANDLE;

extern INT32U thumb_level_nums_get(void);
/*== state avi show ==*/
#define AVI_TIMER 			TIMER_C
extern STAVIBufConfig		*avi_decode_buf;
extern STAVIBufConfig		*avi_display_buf;
extern STAVIAUDIOBufConfig  *avi_audio_buf_wi;
extern STAVIAUDIOBufConfig  *avi_audio_buf_ri;
extern OS_EVENT				*audio_fs_result_q;

extern INT16S tk_avi_audiofilehandle_get(void);
extern INT32S tk_avi_get_audio_data(INT32U framenum);
extern INT32S tk_avi_read_audio_data(CHAR *buf,INT32U len);
extern INT32U tk_avi_decode_file_id(INT32U FileNum);
extern INT32U tk_avi_get_audio_samplerate(void);
extern INT32S tk_avi_get_width(void);
extern INT32S tk_avi_get_height(void);
extern INT32S tk_avi_find_vdpos(INT32U JumpFrameNum);
extern INT32U tk_avi_close_file_id(INT32U FileNum);
extern INT32S tk_avi_get_hassound(void);
extern INT32S tk_avi_get_scale(void);
extern INT32S tk_avi_get_rate(void);
extern INT8U* tk_avi_get_waveformatex(void);
extern FP64   tk_avi_get_fps(void);
extern INT32S tk_avi_vediodata_find(INT32U SilpFrameNum);
extern INT32S avi_memory_free(void);
extern void   state_avi_entry(STATE_ENTRY_PARA* para1);
extern void   avi_ppu_set(STAVIBufConfig* bufconfig);
extern INT32S ap_umi_ppu_go_check(void);
extern void   ap_umi_only_ppu_go_enable(void);
extern void	  ap_umi_only_ppu_go_disable(void);
/*== stat music ==*/

/* stat music */
typedef struct {
INT8U   aud_state;
BOOLEAN mute;
INT8U   volume;
INT8U   play_style;
}audio_status_st;

typedef enum
{
	PLAY_ONCE,
	PLAY_REPEAT,
	PLAY_SEQUENCE,
	PLAY_MIDI,
	PLAY_SPECIAL_REPEAT,
	PLAY_SPECIAL_BYNAME_REPEAT,
	PLAY_SHUFFLE_MODE
}AUDIO_PLAY_STYLE;

typedef enum
{
	STATE_IDLE,
	STATE_PLAY,
	STATE_PAUSED,
	STATE_AUD_FILE_ERR
}AUDIO_STATE_ENUM;

#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
extern INT8U midi_file_exist;
extern INT8U total_midi_number;
extern INT16U midi_file_handle;
extern INT16U midi_file_size;
#endif

extern INT32S music_file_idx;
extern INT32S music_file_err_cnt;
extern audio_status_st aud_status;
extern audio_status_st aud_bg_status;

extern void state_music_entry(void* para1);
extern void ap_media_on_off(void);
extern void ap_media_vol_inc(void);
extern void ap_media_vol_dec(void);

extern void ap_music_init(void);
extern  INT32S audio_bg_shuffle_mode_init(void);
extern  void audio_bg_shuffle_table_free(void);
extern  INT32U audio_shuffle_mode_get_file_idx(void);
extern void audio_bg_shuffle_play_process(void);
extern void audio_play_process(void);
extern void audio_bg_play_process(void);
extern void audio_play_spi_process(void);
extern void audio_next_process(void);
extern void audio_bg_next_process(void);
extern void audio_prev_process(void);
extern void audio_bg_prev_process(void);
extern void audio_send_stop(void);
extern void audio_bg_send_stop(void);
extern INT8U audio_bg_get_state(void);
extern void audio_res_play_process(INT32S result);
extern void audio_res_bg_play_process(INT32S result);
extern void audio_res_play_spi_process(INT32S result);
extern void audio_res_resume_process(INT32S result);
extern void audio_res_pause_process(INT32S result);
extern void audio_res_stop_process(INT32S result);
extern void audio_mute_ctrl_set(BOOLEAN status);
extern void audio_bg_mute_ctrl_set(BOOLEAN status);
extern void audio_vol_inc_set(void);
extern void audio_bg_vol_inc_set(void);
extern void audio_vol_dec_set(void);
extern void audio_bg_vol_dec_set(void);
extern INT8U audio_fg_vol_get(void);
extern void audio_play_style_set(INT8U play_style);
extern void audio_bg_play_style_set(INT8U play_style);
extern INT8U  audio_bg_play_style_get( void );
extern void audio_vol_set(INT8U vol);
extern void audio_bg_vol_set(INT8U vol);
extern void audio_bg_send_pause(void);
extern void audio_bg_send_resume(void);
extern void audio_bg_play_pause_process(void);
extern void audio_bg_special_effect_play(INT32U idx);
extern void audio_bg_special_effect_play_by_name(INT8U *name,INT8U audio_type,INT8U rs_location);
extern void audio_bg_play_prev(void);
extern void audio_bg_play_next(void);
extern void audio_bg_timer_process(void);

/* OBJ OSD Menu */
/*== OSD MENU ==*/
//OSD_MENU USE ONLY

/*define */
#define SHADOW_WINDOW_NUM 	2
#define EFFECT_WINDOW_NUM 	1


typedef enum
{
	ICON_NONE = 0,
   	ICON_POWER,
   	ICON_LEFT,
   	ICON_UP,
   	ICON_DOWN,
   	ICON_RIGHT,
   	ICON_BACK,
   	ICON_OK
}OSD_SOURCE;

typedef enum
{
	NO_ICON = 0,
	M_ICON,
	B_ICON
}ICON_STATUS;

typedef struct
{
   INT16U		icon_pos_num;
   INT8U   		*icon_source_ptr[3];
   CHAR			*string[3];
}icon_list;

typedef struct
{
	INT32U 		 icon_status;
	INT32S 		 iconnum_display;
	INT8U*   	 source_ptr[3];
	CHAR*		 string[3];
	SPRITE*      icon_image;
	SPRITE*      icon_image_shadow;
}OSD_MENU_CONFIG;

typedef struct {
	INT32U 					total_icon_num;
	INT32U					menu_status;
	INT32U 					menu_effect;
	INT32U					language;
	INT32U 					string_size;
	INT32S 					board_posy;
	INT32S 					icon_posy;
	OSD_MENU_CONFIG     	osd_icon[7];
	GraphicBitmap			osd_bitmap;
}OSDMenuStruct;

extern const INT16U	_icon_data[7][4096];
extern INT32S cl2_osd_menu_init(INT32U IconTotal_num,void* ptr);
extern INT32S cl2_osd_menu_effect_select(INT32U mode);
extern INT32S cl2_osd_menu_display(void);
extern INT32S cl2_osd_menu_disappear(void);
extern INT32S cl2_osd_menu_icon_display(void);
extern INT32S cl2_osd_menu_icon_disappear(void);
extern INT32S cl2_osd_menu_iconchange(INT32U icon_pos,INT32U icon_num);
extern INT32S cl2_osd_menu_uninit(void);
extern INT32S cl2_osd_menu_icon_motion(INT32U icon_pos);
extern INT32S cl2_osd_menu_avi_init(INT32U IconTotal_num,void* ptr);
extern INT32S cl2_osd_menu_avi_uninit(void);
extern void   cl2_osd_menu_cursor_select(INT32U icon_pos);
extern void   cl2_osd_menu_cursor_on(void);
extern void   cl2_osd_menu_cursor_off(void);
extern void   cl2_osd_menu_cursor_show(void);
extern INT32S cl2_osd_menu_cursor_status_get(void);
extern void   cl2_osd_menu_cursor_leftmove(void);
extern void   cl2_osd_menu_cursor_rightmove(void);
extern INT32U cl2_osd_menu_cursor_pos_get(void);
extern INT32S cl2_osd_menu_string_language_set(INT8U language);
extern INT32S cl2_osd_menu_string_show_on(void);
extern INT32S cl2_osd_menu_string_show_off(void);
extern INT32S cl2_osd_menu_string_size_size(INT8U size);
#if (defined OSD_MENU_HINT_MODE) && (OSD_MENU_HINT_MODE == 1)
extern INT32S umi_osd_menu_hint_disappear_time_set(INT32S time);
extern void umi_osd_menu_hint_mode_set(INT32S mode);
extern INT32S umi_osd_menu_hint_mode_get(void);
#endif


typedef struct{
	INT16U pos_x;
	INT16U pos_y;
	INT16U font_color;
	INT16U background_color;
}StrinsCentersDrawPar;

typedef struct {
	SPRITE *sprite;
	//GraphicBitmap *bitmap;

}StringsCentersDrawSpriteImage,*PStringsCentersDrawSpriteImage;


extern StringsCentersDrawSpriteImage *cl2_strings_centers_draw(StrinsCentersDrawPar *parameter,char *strings,INT8U language,INT8U font_type);
extern void cl2_strings_centers_draw_add(StringsCentersDrawSpriteImage *buf);
extern void cl2_strings_centers_draw_remove(StringsCentersDrawSpriteImage *buf);

#if 1 //item selection releted.
#define C_ITEM_SELECTION_OBJ_INTERVAL			13		/* 10ms * 2 = 20ms */
#define C_ITEM_SELECTION_OBJ_TIMES				8		/* 20ms * 5 = 100ms */
//#define C_DISPLAY_INVERTED_IMAGE		0x00000001
//#define C_DISPLAY_BACKGROUND_IMAGE	0x00000002
#define C_SET_AS_ITEM_SELECTION			0x00000010
#define C_SET_AS_AUDIO_INDEPENDENT		0x00000080
//#define C_SET_ICON_CYCLE				0x00000020
#define C_ITEM_SELECTION_PAGE_SCROLL_NUM	10

// for user input paremeter
// sprite image

typedef struct
{
	INT16U ItemWidth;
	INT16U ItemHeight;
	INT16U ItemBgColor;
	INT16U ItemFrontColor;
	INT16U ItemHighlightBgColor;
	INT16U ItemHighlightFrontColor;
	INT16U ItemFontSize;
	INT16U ItemFontLanguage;
	INT16U ItemContentStartx;  //The start x of Item Show Contents  >=0 && <= ItemWidth
	INT16U ItemContentStarty;  //The start y of Item Show Contents  >=0 && <= ItemHeight

}STItemSelectionContent;


typedef struct
{
	INT16U	image_width;
	INT16U	image_height;
	INT8U	character_x_size;
	INT8U	character_y_size;
	INT8U	image_color_set;
	INT8U	image_palette_set;
	INT8U	*p_image_data;
} STItemSelectionImage;

// for item_selection api
#define C_EXIT_PPU_RELESE	1
#define C_EXIT_PPU_FREE		2

typedef struct
{
	POS_STRUCT	image_position;
	//POS_STRUCT	inverted_image_position;
	POS_STRUCT	image_position_step[C_ITEM_SELECTION_OBJ_TIMES];
	//POS_STRUCT	inverted_image_position_step[C_ITEM_SELECTION_OBJ_TIMES];
	//INT16U		cover_image[64 * 64];
	STItemSelectionImage	image;						/* point to icon image */
} STItemSelectionIcon;

typedef struct ST_ITEM_SELECTION_OBJ	STItemSelectionObj;
typedef struct ST_ITEM_SELECTION_OBJ	*hItemSelectionObj;

typedef INT32S (*ITEM_SELECTION_LOAD_IMAGE_FUNC)(hItemSelectionObj pItemSelection, INT32U image_id, STItemSelectionImage *image);
typedef INT32S (*ITEM_SELECTION_FREE_IMAGE_FUNC)(hItemSelectionObj pItemSelection, STItemSelectionImage *image,INT8U exit_flag);

struct ST_ITEM_SELECTION_OBJ
{
	INT16S	item_selection_x_pos;
	INT16S	item_selection_y_pos;
	INT16U	item_selection_width;
	INT16U	item_selection_height;
	INT16U	icon_number;
	INT16U	image_number;
	INT16U	selected_icon_id;
	INT16U	selected_image_id;
	INT16U	interval;			   // move once time
	INT16U	times;				   // total move times
	INT16U	now_time;			   // now move time count
	INT32U	flag;				   // decide if display inverted image,
								   // decide if cover image is provide by user or create by this api
								   // decide if the image struct link is the ring link or not
								   // decide if use user provided background image
	STItemSelectionIcon	*pstItemSelectionIcon;	// point to the item_selection icon
	INT8U	BackgroundColor;
	INT32U	*pBackgroundBuffer;
	INT32U	*pBgImageNArray;
	SPRITE	*pSprite;		       // point to the sprite image link
	SPRITE	SpriteStart;	       // the start of sprite image link
	SPRITE	SpriteEnd;	           // the end of sprite image link
	SpN_RAM	*pSpN_RAM;
	//INT8U	text_id;		       // background text id
	INT8U	sprite_depth;	       // sprite depth set
	INT8U	window_id;		       // sprite and text used window id

	STItemSelectionImage ItemBgSpriteImage;
	INT8U   ItemBgbuffer[8*8];
	STItemSelectionImage ItemHighlightSpriteImage;

	#if 1
	INT8U   ItemHighlightbuffer[32*32];
	#else
	INT8U   ItemHighlightbuffer[8*8];
	#endif

	STItemSelectionImage ItemMusicMenuBackgroud;
	INT8U   ItemMusicMenuBackgroudbuffer[8*8];
	SPRITE	MusicMenuBackgroudSprite;
	POS_STRUCT ItemMusicMenuBackgroudPosition;

	STItemSelectionContent ItemContentAttribute;
	ITEM_SELECTION_LOAD_IMAGE_FUNC ItemSelectionObjLoadImage;
	ITEM_SELECTION_FREE_IMAGE_FUNC ItemSelectionObjFreeImage;



};


typedef struct {
	SPRITE 	scorll_bar_bk[4];
	PSPRITE  scorll_bar_bk_start;
	PSPRITE  scorll_bar_bk_end;
	INT32U	outline_ver_buffer[8 * 8 / 4];
	INT32U	outline_hor_buffer[8 * 8 / 4];

} t_ITEM_SELECTION_SCORLL_BAR_BK;

extern hItemSelectionObj cl2_item_selection_create(
	INT16S		item_selection_x_pos,				// item_selection x position
	INT16S		item_selection_y_pos,				// item_selection y position
	INT16U		ItemSelectionWidth, 				// item_selection width
	INT16U		ItemSelectionHeight, 			    // item_selection height
	INT16U		ImageNumber,			            // total image for item_selection display
	INT16U		IconNumber,				            // total icon
	POS_STRUCT	*IconPosition,			            // icon position set
	INT8U		BackgroundColor,		            // background color
	INT16U		SelectedIconId,			            // default selected icon index
	INT16U		SelectedImageId,			        // start image index
	STItemSelectionContent ContentAttribute,
	ITEM_SELECTION_LOAD_IMAGE_FUNC ItemSelectionObjLoadImage,
	ITEM_SELECTION_FREE_IMAGE_FUNC ItemSelectionObjFreeImage,
	INT32U		flag					            // control flag

);
INT32S cl2_item_selection_move_up(hItemSelectionObj   hItemSelection);
INT32S cl2_item_selection_page_up(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_move_up_end(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_move_up_effect(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_move_down(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_move_down_end(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_move_down_effect(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_draw_current_highlight(hItemSelectionObj hItemSelection);
INT32S cl2_item_selection_resume_prev_highlight(hItemSelectionObj hItemSelection);
extern INT32S cl2_item_selection_destroy(hItemSelectionObj hItemSelection);
#endif

#if 1
typedef struct VOLUME_CONTROL_STRUCT VolumeControlAttribute;
typedef struct VOLUME_CONTROL_STRUCT *pVolumeControlAttribute;

typedef INT8S (*VolumeControlLoadFunc)(VolumeControlAttribute *parameter);

typedef struct
{
	INT8U  depth;
	INT8U  window;
	INT8U  blending;

	INT16U width;
	INT16U height;
	INT16S start_x;
	INT16S start_y;
	INT16U backgound_color;
	INT16U line_color;
	INT16U line_width;

	INT16U pic_start_x;

	INT8U  total_bar_number;
	INT16U bar_width;
	INT16U bar_gap;
	INT16U first_bar_end_y;
	INT16U first_bar_height;
	INT16U last_bar_height;
	INT16U normal_bar_color;
	INT16U highlight_bar_color;

	INT8S  current_volume;


}VolumeControlParameter;

typedef struct{
	INT16U *img_databuf;
	INT16U img_width;
	INT16U img_height;
	INT8U img_char_x;
	INT8U img_char_y;
	INT16U color_set;
	INT16U palette_set;

}VolumeImageAttribute;

struct VOLUME_CONTROL_STRUCT
{
	VolumeControlParameter parameter;
	SPRITE background_sprite;
	SPRITE pic_sprite;
	SPRITE bar_start_sprite;
	SPRITE bar_end_sprite;


	//VolumeImageAttribute imgage[4];

	#if 1
	INT16U *background_img_databuf;
	INT16U background_img_width;
	INT16U background_img_height;
	INT8U background_img_char_x;
	INT8U background_img_char_y;
	INT16U background_color_set;
	INT16U background_palette_set;

	INT16U *pic_img_databuf;
	INT16U pic_img_width;
	INT16U pic_img_height;
	INT8U pic_img_char_x;
	INT8U pic_img_char_y;
	INT16U pic_color_set;
	INT16U pic_palette_set;

	INT16U *normal_bar_img_databuf;
	INT16U normal_bar_img_width;
	INT16U normal_bar_img_height;
	INT8U normal_bar_img_char_x;
	INT8U normal_bar_img_char_y;
	INT16U normal_bar_color_set;
	INT16U normal_bar_palette_set;

	INT16U *highlight_bar_img_databuf;
	INT16U highlight_bar_img_width;
	INT16U highlight_bar_img_height;
	INT8U highlight_bar_img_char_x;
	INT8U highlight_bar_img_char_y;
	INT16U highlight_bar_color_set;
	INT16U highlight_bar_palette_set;
	#endif
	POS_STRUCT *position;

	INT8U  sprite_depth;
	//VolumeControlLoadFunc LoadData[4];
	VolumeControlLoadFunc LoadBackGroundData;   //Background data.
	VolumeControlLoadFunc LoadPictureData;      //Picture data.
	VolumeControlLoadFunc LoadBarNormalData;    //Normal bar data.
	VolumeControlLoadFunc LoadBarHighlightData; //Highlight bar data.
};
extern pVolumeControlAttribute cl2_volume_control_create(VolumeControlParameter *parameter,VolumeControlLoadFunc Load[4]);
extern void cl2_volume_control_bar_sprite_image_set(VolumeControlAttribute *volume);
extern void cl2_volume_control_bar_normal_link(VolumeControlAttribute *volume,INT8U i,INT8U mode);
extern void cl2_volume_control_bar_highlight_link(VolumeControlAttribute *volume,INT8U i,INT8U mode);
extern void cl2_volume_control_up(VolumeControlAttribute *volume);
extern void cl2_volume_control_down(VolumeControlAttribute *volume);
extern void cl2_volume_control_char_id_get(INT8U char_x,INT8U char_y,INT8U *X_id,INT8U *Y_id);
extern void cl2_volume_control_highlight_set(VolumeControlAttribute *volume);
extern void cl2_volume_control_free(pVolumeControlAttribute volume);
extern void cl2_volume_control_position_in_set(VolumeControlAttribute *volume,INT16S x0,INT16S y0,INT16S c_step,INT16S c_total);
extern void cl2_zoom_position_get(POS_STRUCT *input_pos,POS_STRUCT *output_pos,INT16S x0,INT16S y0,INT16S c_step,INT16S c_total);
extern void cl2_zoom_one_position_get(INT16S x0,INT16S y0,INT16S x1,INT16S y1, INT16S *x_get,INT16S *y_get,INT16S c_step,INT16S c_total);
#endif


#if 1/* graphic draw extern */
extern INT32U cl1_graphic_init(GraphicBitmap *bitmap);
extern void cl1_graphic_finish(GraphicBitmap *bitmap);
extern void cl1_graphic_bitmap_front_color_set(GraphicBitmap *bitmap,INT16U color);
extern void cl1_graphic_bitmap_background_color_set(GraphicBitmap *bitmap,INT16U color);

extern void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern void cl1_graphic_char_draw(GraphicBitmap *bitmap,unsigned short character,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern void cl1_graphic_line_draw(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void cl1_graphic_rectangle_draw(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);

extern  void cl1_graphic_number_font_init(void);

extern INT32U cl1_graphic_font_width_get(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_height_get(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_line_bytes_get(INT16U character_code,INT8U language,INT8U font_type);

extern void   cl1_graphic_strings_centers_draw(StrinsCentersDrawPar *parameter,GraphicBitmap* bitmap,char *strings,INT8U language,INT8U font_type);
extern void   cl1_graphic_free_strings_centers_draw(GraphicBitmap *bitmap);
extern INT32U cl1_graphic_strings_newline_get(char *strings,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_width_get_for_no_newline(char *strings,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_height_get(char *strings,INT8U language,INT8U font_type);
extern void   cl1_graphic_strings_copy(INT8S *dest,INT8S *source,INT16U length );
extern void   cl1_graphic_character_sizes_get(INT16U width,INT16U height,INT16U *char_x,INT16U *char_y);

extern INT32U cl1_graphic_multinational_strings_width_get_for_no_newline(char *strings,INT8U language);
extern INT32U cl1_graphic_multinational_font_height_get(INT16U character_code,INT8U language);
extern void cl1_graphic_multinational_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language);

extern void   cl1_graphic_music_string_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates);
extern void   cl1_graphic_font_get(INT16U character_code,FontTableAttribute *font,INT8U language,INT8U font_type);
#endif

// NEW cl1/cl2 bmp and graphic object declare
typedef enum {
    CELL_H8    = 0x0,
    CELL_H16   = 0x1,
    CELL_H32   = 0x2,
    CELL_H64   = 0x3,
    CELL_V8    = 0x0,
    CELL_V16   = 0x1,
    CELL_V32   = 0x2,
    CELL_V64   = 0x3,
    CELL_8x8   = (CELL_H8<<4 | CELL_V8),
    CELL_8x16  = (CELL_H8<<4 | CELL_V16),
    CELL_8x32  = (CELL_H8<<4 | CELL_V32),
    CELL_8x64  = (CELL_H8<<4 | CELL_V64),
    CELL_16x8  = (CELL_H16<<4 | CELL_V8),
    CELL_16x16 = (CELL_H16<<4 | CELL_V16),
    CELL_16x32 = (CELL_H16<<4 | CELL_V32),
    CELL_16x64 = (CELL_H16<<4 | CELL_V64),
    CELL_32x8  = (CELL_H32<<4 | CELL_V8),
    CELL_32x16 = (CELL_H32<<4 | CELL_V16),
    CELL_32x32 = (CELL_H32<<4 | CELL_V32),
    CELL_32x64 = (CELL_H32<<4 | CELL_V64),
    CELL_64x8  = (CELL_H64<<4 | CELL_V8),
    CELL_64x16 = (CELL_H64<<4 | CELL_V16),
    CELL_64x32 = (CELL_H64<<4 | CELL_V32),
    CELL_64x64 = (CELL_H64<<4 | CELL_V64)
} CELL_MODE_ENUM;

typedef enum {
    TEXT_H32     = 0x0,
    TEXT_H64     = 0x1,
    TEXT_H128    = 0x2,
    TEXT_H256    = 0x3,
    TEXT_V32     = 0x0,
    TEXT_V64     = 0x1,
    TEXT_V128    = 0x2,
    TEXT_V256    = 0x3,
    TEXT_32x32   = (TEXT_H32<<4  | TEXT_V32),
    TEXT_32x64   = (TEXT_H32<<4  | TEXT_V64),
    TEXT_32x128  = (TEXT_H32<<4  | TEXT_V128),
    TEXT_32x256  = (TEXT_H32<<4  | TEXT_V256),
    TEXT_64x32   = (TEXT_H64<<4  | TEXT_V32),
    TEXT_64x64   = (TEXT_H64<<4  | TEXT_V64),
    TEXT_64x128  = (TEXT_H64<<4  | TEXT_V128),
    TEXT_64x256  = (TEXT_H64<<4  | TEXT_V256),
    TEXT_128x32  = (TEXT_H128<<4 | TEXT_V32),
    TEXT_128x64  = (TEXT_H128<<4 | TEXT_V64),
    TEXT_128x128 = (TEXT_H128<<4 | TEXT_V128),
    TEXT_128x256 = (TEXT_H128<<4 | TEXT_V256),
    TEXT_256x32  = (TEXT_H256<<4 | TEXT_V32),
    TEXT_256x64  = (TEXT_H256<<4 | TEXT_V64),
    TEXT_256x128 = (TEXT_H256<<4 | TEXT_V128),
    TEXT_256x256 = (TEXT_H256<<4 | TEXT_V256)
} TEXT_MODE_ENUM;

// cl1 font ram module extern
extern INT32S cl1_font_char_get(INT16U language, INT8U font_type, INT16U character_code, FontTableAttribute * font);
extern INT32U cl1_font_ram_size_get(t_GP_RESOURCE_HEADER *gprs_header, INT16U language,INT8U font_type);
extern INT32S cl1_rs_font_load(t_GP_RESOURCE_HEADER *gprs_header, INT16U language,INT8U font_type, INT32U font_ram_buf_addr);
extern INT32U font_ram_init(t_GP_RESOURCE_HEADER * gprs_header, INT16U language, INT8U font_type);
extern INT32U font_ram_uninit(INT16U language,INT8U font_type);
extern INT32U cl1_font_height_get(INT8U language, INT8U font_type);

// cl1 bitmap extern
extern INT32S cl1_bitmap_data_clean(GraphicBitmap *bitmap);
extern INT32U cl1_bitmap_data_size_get(GraphicBitmap *bitmap);

// cl2 printf
extern INT32S cl2_printf(char* print_str,...);
extern INT8S cl2_printf_init(INT16S start_x0, INT16S start_y0, INT16U width, INT16U height, INT8U frame_lines);

////for scroll bar
#if 1
typedef struct
{
	INT16U start_x;
	INT16U start_y;
	INT16U width;
	INT16U height;
	INT16U backcolor;
	INT16U scrollcolor;

	INT32U current_id;
	INT32U window_num;
	INT32U total_num;
	INT32U window_id;

	INT16U scrollbartype;

	SPRITE *pScrollBar_Sprite;
	GraphicBitmap *pScrollBar_Bitmap;

}ScrollBarAttribute, *PScrollBarAttribute;

extern PScrollBarAttribute cl2_scrollbar_init
(
	INT16U start_x,			//background bar start_x
	INT16U start_y,			//background bar start_y
	INT16U width,			//background bar width
	INT16U height,			//background bar height
	INT16U backcolor,		//background bar color
	INT16U scrollcolor,		//scroll bar color

	INT16U scrollbartype	//scroll bar type
);	//return struct
extern INT32S cl2_scrollbar_update(PScrollBarAttribute pScrollBar_Para, INT32U current_num, INT32U window_num, INT32U total_num);
extern INT32S cl2_scrollbar_page_update(PScrollBarAttribute pScrollBar_Para, INT32U current_id, INT32U window_id,INT32U window_num, INT32U total_num);
extern void cl2_scrollbar_uninit(PScrollBarAttribute pScrollBar_Para);	//free memory
extern INT32S cl2_scrollbar_pos_cfg(PScrollBarAttribute pScrollBar_Para, INT16S x0, INT16S y0);
#endif

// Loading animation
extern INT32S umi_loading_animation_start(void);
extern INT32S umi_loading_animation_update_timer(void);
extern INT32S umi_loading_animation_end(void);

// for component L2 show messages
typedef enum
{
  MSG_STORAGE_PLUG,
  MSG_VOLUME,
  MSG_POWER_OFF,
  MSG_NORMAL,
  MSG_NORMAL_LCD_EN,
  MSG_MAX
} POP_UP_MSG_ENUM;

typedef enum{
	MSG_STR_PRINT,
	MSG_STR_CLEAR
}POP_UP_MSG_DRAW_TYPE;

extern void pop_up_msg_init(void);
extern void pop_up_msg_start(void *status, INT8U msg_enum);
extern void pop_up_msg_pseudotimer_handle(INT8U msg_enum);
extern void pop_up_stg_msg_mount_sts_set(void);
extern void pop_up_stg_msg_mount_fail_set(INT8U sts);
extern INT8U pop_up_msg_type_get(void);
extern INT32U pop_up_string_choose(INT8U language, INT16U index);
extern INT32U color_choose(INT8U color);

// for interrogation
#if (SUPPORT_POWER_OFF_NOTE==CUSTOM_ON)
extern INT8U power_off_note_flag;
#endif
extern void ap_interrog_choose_motion(void);
extern void	ap_interrogation_init(INT16U string_enum);
extern void	ap_interrog_move_left(void);
extern void	ap_interrog_move_right(void);
extern INT8U ap_interrog_ok(void);
extern void ap_interrogation_exit(void);

// for component L2 charge module
extern INT32S battery_init(void);
extern void charge_module_show(void);
extern void adapter_plug_in_show(void);
extern void adapter_plug_out_show(void);
extern void battery_check_and_refresh(void);
extern void battery_low_voltage_remind(void);
extern void battery_exit(void);

extern SYSTEM_USER_OPTION Global_User_Optins;
extern void umi_config_initial(INT8S status);
extern void umi_user_config_store(void);
extern INT32S umi_user_config_load(void);
extern void umi_scaler_color_matrix_load(void);

// language
extern void umi_language_set(INT8U language);
extern INT8U umi_language_get(void);

// date display format
extern void umi_date_format_display_set(INT8U date_format_display);
extern INT8U umi_date_format_display_get(void);

// time display format
extern void umi_time_format_display_set(INT8U time_format_display);
extern INT8U umi_time_format_display_get(void);

// week display format
extern void umi_week_format_display_set(INT8U week_format_display);
extern INT8U umi_week_format_display_get(void);

// calendar display format
extern void umi_calendar_format_display_set(INT8U calendar_format_display);
extern INT8U umi_calendar_format_display_get(void);

// alarm
extern ALARM_STRUCT umi_alarm1_struct_get(void);
extern ALARM_STRUCT umi_alarm2_struct_get(void);
extern void umi_alarm_onoff_set(INT32U alarmid,INT32U onoff);
extern void umi_alarm1_struct_set(ALARM_PARAMETER alarm_parameter);
extern void umi_alarm2_struct_set(ALARM_PARAMETER alarm_parameter);
extern void   umi_alarm_happen_music_set(INT32U alarm_music_idx,INT8U alarm_module);
extern INT32U umi_alarm_happen_music_get(INT8U alarm_module);
extern void umi_alarm_id_set(INT8U	alarm_id);
extern INT8U umi_alarm_id_get(void);
extern void umi_alarm_onoff_set(INT32U alarmid,INT32U onoff);
extern void umi_alarm_mute_set(INT8U alarmid,INT8U alarm_mute);
extern INT8U umi_alarm_mute_get(INT8U alarmid);

// power onoff time
#if((defined _POWERTIME_SETTING)&&(_POWERTIME_SETTING == CUSTOM_ON))
typedef enum {
	POWERTIME_FLAG_OFF = 0,
	POWERTIME_FLAG_ONCE,
	POWERTIME_FLAG_WEEKDAY,
	POWERTIME_FLAG_WEEKEND,
	POWERTIME_FLAG_EVERYDAY
} POWERTIME_FLAG;

extern INT8U umi_powertime_mudual_get(void);
extern void umi_powertime_modual_set(INT8U i);
extern void umi_powertime_struct_set(POWERONOFF_STRUCT powertime_para, INT8U powertime_id);
extern POWERONOFF_STRUCT umi_powertime_struct_get(INT8U powertime_id);
//extern void umi_powertime_id_set(INT8U	powertime_id);
extern INT8U umi_powertime_id_get(void);
extern void umi_powertime_active_id_set(INT8S powertime_active_id);
extern INT8S umi_powertime_active_id_get(void);
extern void umi_powertime_happened_set(INT8U id, INT8U flag);
extern INT8U umi_powertime_happened_get(INT8U id);
extern INT8U umi_powertime_active_happened_get(void);
extern void umi_powertime_active_happened_set(INT8U flag);
extern TIME_COMPARE umi_powertime_time_get(INT8U id);
extern INT8S umi_powertime_type_get(INT8U id);
extern INT8S umi_powertime_active_type_get(void);
extern INT8S ap_alarm_powertime_happen_confirm(void);
extern void ap_alarm_powertime_once_to_off(void);
extern TIME_COMPARE umi_powertime_active_time_get(void);
extern INT8S umi_powertime_active_mode_get(void);
extern INT8U umi_powertime_mode_get(INT8U id);
extern void umi_powertime_mode_set(INT8U id, INT8U mode);


#endif

/*
// power onoff time 1
extern void umi_power_onoff_time1_set(INT8U *time);
extern INT8U* umi_power_onoff_time1_get(void);

// power onoff time 2
extern void umi_power_onoff_time2_set(INT8U *time);
extern INT8U* umi_power_onoff_time2_get(void);

// power onoff time 3
extern void umi_power_onoff_time3_set(INT8U *time);
extern INT8U* umi_power_onoff_time3_get(void);

// power onoff time 4
extern void umi_power_onoff_time4_set(INT8U *time);
extern INT8U* umi_power_onoff_time4_get(void);
*/
// lcd backlight
extern void umi_lcd_backlight_set(INT8U lcd_backlight);
extern INT8U umi_lcd_backlight_get(void);

// sound valume
extern void umi_sound_volume_set(INT8U sound_volume);
extern INT8U umi_sound_volume_get(void);
extern void ap_volume_control_mute(void);

// current storage
#define STORAGE_CHANGE_AUTO   0
#define STORAGE_CHANGE_MANUAL 1

extern void umi_current_storage_set(INT8U current_storage);
extern INT8U umi_current_storage_get(void);

// current photo index
extern void umi_current_photo_index_set(INT32U current_photo_index);
extern INT32U umi_current_photo_index_get(void);

// current video index
extern void umi_current_video_index_set(INT32U current_video_index);
extern INT32U umi_current_video_index_get(void);

// slideshow duration
extern void umi_slideshow_duration_set(INT8U slideshow_duration);
extern INT8U umi_slideshow_duration_get(void);

// slideshow back ground music
extern void umi_slideshow_bg_music_set(INT32U slideshow_bg_music);
extern INT32U umi_slideshow_bg_music_get(void);

// slideshow transition
extern void umi_slideshow_transition_set(INT8U slideshow_transition);
extern INT8U umi_slideshow_transition_get(void);

// slideshow photo date on
extern void umi_slideshow_photo_date_on_set(INT8U slideshow_photo_date_on);
extern INT8U umi_slideshow_photo_date_on_get(void);

// calendar duration
extern void umi_calendar_duration_set(INT8U calendar_duration);
extern INT8U umi_calendar_duration_get(void);

// calendar back ground music
extern void umi_calendar_bg_music_set(INT32U calendar_bg_music);
extern INT32U umi_calendar_bg_music_get(void);

// calendar display mode
extern void umi_calendar_displaymode_set(INT8U calendar_displaymode);
extern INT8U umi_calendar_displaymode_get(void);

//music play mode.
extern void umi_music_play_mode_set(INT8U music_play_mode);
extern INT8U umi_music_play_mode_get(void);

extern INT8U umi_music_play_onoff_get(void);
extern void umi_music_play_onoff_set(INT8U on_off);
extern void umi_alarm_modual_set(INT8U i);
extern INT8U umi_alarm_mudual_get(void);

extern void umi_alarm_volume_set(INT8U alarmid,INT8U alarm_volume);
extern INT8U umi_alarm_volume_get(INT8U alarmid);

#if 1//_SUPPORT_MIDI_IN_ITEM_SELECTION == 1
extern void umi_midi_mode_set(INT8U mode);
extern INT8U umi_midi_mode_get(void);
#endif

extern void umi_thumbnail_mode_set(INT8U mode);
extern INT8U umi_thumbnail_mode_get(void);

extern void umi_factory_date_get(INT8U *date);
extern void umi_factory_time_get(INT8U *time);

extern void umi_full_screen_on_set(INT8U full_screen_on);
extern INT8U umi_full_screen_on_get(void);


extern void ap_auto_pet_init( INT8U index );
extern INT8U umi_sleep_time_index_get( void );
extern void umi_sleep_time_index_set( INT8U index );
extern void umi_scaler_color_matrix_get( COLOR_MATRIX_CFG *d_color_matrix );
extern void umi_scaler_color_matrix_set( COLOR_MATRIX_CFG *d_color_matrix );


extern void umi_default_restores_set(void);
extern void umi_file_copy_counter_set(INT32U i);
extern INT32U umi_file_copy_counter_get(void);

extern void umi_base_day_set(INT32U day);
extern INT32U umi_base_day_get(void);

extern void state_storage_entry(void* para1);
extern void ap_storage_change_show_strings(void);
extern void storage_change_framewin_title(void);
extern void ap_storage_change_show_image(void);
extern void ap_storage_photo_music_idx_reset(void);
extern INT8U pseudo_stg_mode_get(void);

extern void umi_ui_style_set(INT8U mode);
extern INT8U umi_ui_style_get(void);

// component L2 framewin
//====================================================================
// egui base define and struct
#define C_OBJ_VISIBLE				1
#define C_OBJ_INVISIBLE				0
#define C_OBJ_NORMAL				0x10000000
#define C_OBJ_ROTATE90				0x20000000
#define C_OBJ_ROTATE270			0x40000000


#define TEXT_ALIGN_CENTER					0x00000001
#define TEXT_ALIGN_LEFT						0x00000002
#define TEXT_ALIGN_RIGHT					0x00000004

typedef struct { INT16S x,y; } EGUI_POINT;
typedef struct { INT16S x0,y0,x1,y1; } EGUI_RECT;

typedef struct T_EGUI_OBJ
{
	EGUI_RECT		Rect;			/* outer dimensions of window */
	EGUI_RECT		InvalidRect;	/* invalid rectangle */
//	EGUI_CALLBACK	*cb;			/* ptr to notification callback */
	PSPRITE			p_sprite_start;
	PSPRITE			p_sprite_end;
	INT32U			Status;			/* Some status flags */
	struct T_EGUI_OBJ	*pParent;
	struct T_EGUI_OBJ	*pFirstChild;
	struct T_EGUI_OBJ	*pNext;
} t_EGUI_OBJ;

typedef struct
{
	INT16U	image_width;
	INT16U	image_height;
	INT8U	character_x_size;
	INT8U	character_y_size;
	INT8U	image_color_set;
	INT8U	image_palette_set;
	INT8U	*p_image_data;
} t_egui_image;

typedef struct {
	t_EGUI_OBJ	obj;
} t_DESKTOP_OBJ;

extern void egui_invalid_obj(t_EGUI_OBJ *obj);
extern void egui_add_to_parent(t_EGUI_OBJ *parent, t_EGUI_OBJ *obj);
extern t_DESKTOP_OBJ *egui_desktop_create(INT16S start_x, INT16S start_y, INT16U width, INT16U height,
				INT32U flag);
extern INT8U cl2_character_size_caculate(INT16U size);
extern INT8U cl2_character_set_caculate(INT8U character_size);
extern void rect_to_pos(EGUI_RECT *rect, POS_STRUCT *pos);
extern void sprite_clear(SPRITE *pSprite);
#if 0
extern INT32S draw_outline_board(PSPRITE p_sprite, EGUI_RECT *p_rect, INT32U *hor_buffer, INT32U *ver_buffer,
	INT16U outline_color, INT8U outline_width, INT8U depth, INT8U blending);
extern INT32S egui_draw_string(PSPRITE p_sprite, EGUI_RECT *p_rect, CHAR *p_string, INT16U text_color,
	INT8U text_font_size, INT8U	text_align, INT8U language, INT16U outline_color, INT16U outline_width,
	INT32U depth, INT32U blending);
extern INT32S egui_draw_bar(PSPRITE p_sprite, EGUI_RECT *p_rect, INT16U color, t_egui_image *p_image, INT8U *p_buffer,
	INT32U depth, INT32U blending);
#else
extern INT32S egui_draw_string(PSPRITE p_sprite, EGUI_RECT *p_rect, CHAR *p_string, INT16U text_color,
	INT8U text_font_size, INT8U	text_align, INT8U language, INT16U outline_color, INT16U outline_width,
	INT32U depth, INT32U blending,INT32U status);
extern INT32S draw_outline_board(PSPRITE p_sprite, EGUI_RECT *p_rect, INT32U *hor_buffer, INT32U *ver_buffer,
	INT16U outline_color, INT8U outline_width, INT8U depth, INT8U blending,INT32U status);
extern INT32S egui_draw_bar(PSPRITE p_sprite, EGUI_RECT *p_rect, INT16U color, t_egui_image *p_image, INT8U *p_buffer,
	INT32U depth, INT32U blending,INT32U status);
#endif
//====================================================================
// framewin
#define C_FRAMEWIN_SPRITE_NUMBER			7
#define C_FRAMEWIN_FRAME_BG_SPRITE_ID		0
#define C_FRAMEWIN_TITLE_BG_SPRITE_ID		1
#define C_FRAMEWIN_TITLE_SPRITE_ID			2
#define C_FRAMEWIN_OUTLINE_BOARD_SPRITE_ID	3 //(3 ~ 6)

#define C_FRAMEWIN_TITLE_BLENDING_OFFSET	0
#define C_FRAMEWIN_TITLE_BG_BLENDING_OFFSET	4
#define C_FRAMEWIN_OUTLINE_BLENDING_OFFSET	8
#define C_FRAMEWIN_BG_BLENDING_OFFSET		12

#define C_FRAMEWIN_UPDATE_TITLE				0x00010000
#define C_FRAMEWIN_UPDATE_TITLE_BG			0x00020000
#define C_FRAMEWIN_UPDATE_OUTLINE_BOARD		0x00040000
#define C_FRAMEWIN_UPDATE_BOARD				0x00080000

typedef struct {
	t_EGUI_OBJ	obj;
	// title
	CHAR		*p_title;
	INT8U		title_font_size;
	INT8U		title_align;
	INT16U		title_text_color;
	INT16U		title_bg_color;
	INT16U		title_bg_height;
	INT32U		title_bg_buffer[8 * 8 / 4];
	t_egui_image *p_title_bg_image;
	t_egui_image title_bg_image;
	// frame bg
	INT16U		frame_bg_color;
	INT32U		frame_bg_buffer[8 * 8 / 4];
	t_egui_image *p_frame_bg_image;
	t_egui_image frame_bg_image;
	// outline board color
	INT16U		outline_board_color;
	INT16U		outline_board_width;
	INT32U		outline_ver_buffer[8 * 8 / 4];
	INT32U		outline_hor_buffer[8 * 8 / 4];
	// ppu parameter
	INT32U		depth;
	INT32U		blending;
	SPRITE		sprite_temp[C_FRAMEWIN_SPRITE_NUMBER];
	POS_STRUCT	pos_temp[C_FRAMEWIN_SPRITE_NUMBER];
	INT32U		flag;
} t_FRAMEWIN;

extern t_FRAMEWIN *cl2_framewin_create(
	INT16S		start_x,
	INT16S		start_y,
	INT16U		width,
	INT16U		height,
	// title
	CHAR		*p_title,
	INT8U		title_font_size,
	INT8U		title_align,
	INT16U		title_text_color,
	INT16U		title_bg_color,
	INT16U		title_bg_height,
	t_egui_image *p_title_bg_image,
	// frame bg
	INT16U		frame_bg_color,
	t_egui_image *p_frame_bg_image,
	// outline board color
	INT16U		outline_board_color,
	INT16U		outline_board_width,
	// ppu parameter
	INT32U		depth,
	INT32U		blending,
	// link and flag
	t_EGUI_OBJ	*p_parent,
	INT32U	flag
);
extern INT32S cl2_framewin_paint(t_FRAMEWIN *p_framewin_obj);
extern INT32S cl2_framewin_redraw(t_FRAMEWIN *p_framewin_obj);
extern INT32S cl2_framewin_destroy(t_FRAMEWIN *p_framewin_obj);
extern INT32S cl2_framewin_change_depth(t_FRAMEWIN *p_framewin, INT32U depth);
extern INT32S cl2_framewin_set_blending_level(t_FRAMEWIN *p_framewin, INT32U blending_level);
extern INT32S cl2_framewin_set_title(t_FRAMEWIN *p_framewin, CHAR *title, INT8U align, INT16U font_color, INT8U font_size);
extern INT32S cl2_framewin_set_title_bg(t_FRAMEWIN *p_framewin, t_egui_image *p_image);
extern INT32S cl2_framewin_set_frame_bg(t_FRAMEWIN *p_framewin, t_egui_image *p_image);
extern void cl2_framewin_zoom(t_FRAMEWIN *p_framewin_obj, INT16S x, INT16S y, INT16S c_step,INT16S c_total);

//====================================================================
// progbar
#define C_PROGBAR_SPRITE_NUMBER				8
#define C_PROGBAR_TITLE_SPRITE_ID			0
#define C_PROGBAR_BG_BAR_SPRITE_ID			1
#define C_PROGBAR_FG_BAR_SPRITE_ID			2
#define C_PROGBAR_PROCESS_VALUE_SPRITE_ID	3
#define C_PROGBAR_OUTLINE_BOARD_SPRITE_ID	4 //(4~7)

#define C_PROGBAR_TITLE_BLENDING_OFFSET		0
#define C_PROGBAR_BAR_BLENDING_OFFSET	4
#define C_PROGBAR_OUTLINE_BLENDING_OFFSET	8
#define C_FRAMEWIN_VALUE_BLENDING_OFFSET	12

#define C_PROGBAR_SHOW_TITLE				0x00010000
#define C_PROGBAR_SHOW_PROCESS_VALUE		0x00020000

#define C_PROGBAR_UPDATE_TITLE				0x00040000
#define C_PROGBAR_UPDATE_PROCESS_VALUE		0x00080000
#define C_PROGBAR_UPDATE_BAR_OUTLINE		0x00100000
#define C_PROGBAR_UPDATE_BG_BAR				0x00200000
#define C_PROGBAR_UPDATE_FG_BAR				0x00400000

typedef struct
{
	t_EGUI_OBJ	obj;
	// title
	INT16S		title_x;
	INT16S		title_y;
	INT16U		title_color;
	INT8U		title_font_size;
	INT8U		title_outline_width;
	INT16U		title_outline_color;
	CHAR		*p_title;
	// value
	INT16S		value_x;
	INT16S		value_y;
	INT16U		value_color;
	INT8U		value_font_size;
	INT8U		value_outline_width;
	INT16U		value_outline_color;
	INT32S		current_value;
	INT32S		value_min;
	INT32S		value_max;
	// bar
	INT16S		bar_x;
	INT16S		bar_y;
	INT16U		bar_width;
	INT16U		bar_height;
	INT16U		bar_outline_color;
	INT8U		bar_outline_width;
	INT32U		bar_outline_hor_buffer[8 * 8 / 4];
	INT32U		bar_outline_ver_buffer[8 * 8 / 4];
	INT16U		fg_color;
	INT32U		fg_buffer[8 * 8 / 4];
	t_egui_image	*p_fg_image;
	t_egui_image	fg_image;
	INT16U		bg_color;
	INT32U		bg_buffer[8 * 8 / 4];
	t_egui_image	*p_bg_image;
	t_egui_image	bg_image;
	// ppu parameter
	INT32U		depth;
	INT32U		blending;
	// sprite buffer
	SPRITE		sprite_temp[C_PROGBAR_SPRITE_NUMBER];
	POS_STRUCT	pos_temp[C_PROGBAR_SPRITE_NUMBER];
} t_PROGBAR_OBJ;

extern t_PROGBAR_OBJ *cl2_progbar_create(
	INT16S	start_x,
	INT16S	start_y,
	INT16U	width,
	INT16U	height,
	// title
	INT16S		title_x,
	INT16S		title_y,
	INT16U		title_color,
	INT8U		title_font_size,
	INT8U		title_outline_width,
	INT16U		title_outline_color,
	CHAR		*p_title,
	// value
	INT16S		value_x,
	INT16S		value_y,
	INT16U		value_color,
	INT8U		value_font_size,
	INT8U		value_outline_width,
	INT16U		value_outline_color,
	INT32S		current_value,
	INT32S		value_min,
	INT32S		value_max,
	// bar
	INT16S		bar_x,
	INT16S		bar_y,
	INT16U		bar_width,
	INT16U		bar_height,
	INT16U		bar_outline_color,
	INT8U		bar_outline_width,
	INT16U		fg_color,
	t_egui_image	*p_fg_image,
	INT16U		bg_color,
	t_egui_image	*p_bg_image,
	// ppu parameter
	INT32U		depth,
	INT32U		blending,
	// link and flag
	t_EGUI_OBJ	*p_parent,
	INT32U	flag
);
extern INT32S cl2_progbar_paint(t_PROGBAR_OBJ *progbar_obj);
extern INT32S cl2_progbar_redraw(t_PROGBAR_OBJ *p_progbar_obj);
extern INT32S cl2_progbar_destroy(t_PROGBAR_OBJ *p_progbar_obj);
extern INT32S cl2_progbar_update(t_PROGBAR_OBJ *p_progbar_obj, INT32S current_value);
extern INT32S cl2_progbar_change_title_color(t_PROGBAR_OBJ *p_progbar_obj, INT16U title_color, INT16U title_outline_color);
extern INT32S cl2_progbar_change_title_position(t_PROGBAR_OBJ *p_progbar_obj, INT16S title_x, INT16S title_y);
extern INT32S cl2_progbar_change_title(t_PROGBAR_OBJ *p_progbar_obj, CHAR *p_title);
extern INT32S cl2_progbar_change_value_color(t_PROGBAR_OBJ *p_progbar_obj, INT16U value_color, INT16U value_outline_color);
extern INT32S cl2_progbar_change_value_position(t_PROGBAR_OBJ *p_progbar_obj, INT16S value_x, INT16S value_y);
extern INT32S cl2_progbar_change_fg_bar_color(t_PROGBAR_OBJ *p_progbar_obj, INT16U fg_color, t_egui_image *p_fg_image);
extern void cl2_progbar_zoom(t_PROGBAR_OBJ *p_progbar_obj, INT16S x, INT16S y, INT16S c_step,INT16S c_total);

//====================================================================
// bitmap
#define C_BITMAP_SPRITE_NUMBER		1

typedef struct
{
	t_EGUI_OBJ		obj;
	INT32U			depth;
	INT32U			blending;
	t_egui_image	bitmap;
	t_egui_image	*p_bitmap;
	SPRITE			sprite_temp[C_BITMAP_SPRITE_NUMBER];
	POS_STRUCT		pos_temp[C_BITMAP_SPRITE_NUMBER];
} t_BITMAP_OBJ;

extern t_BITMAP_OBJ *cl2_bitmap_create(
	INT16S	start_x,
	INT16S	start_y,
	INT16U	width,
	INT16U	height,
	// ppu parameter
	INT32U	depth,
	INT32U	blending,
	t_egui_image *p_bitmap,
	t_EGUI_OBJ	*p_parent,
	INT32U	flag
);
extern INT32S cl2_bitmap_paint(t_BITMAP_OBJ *p_bitmap_obj);
extern INT32S cl2_bitmap_destroy(t_BITMAP_OBJ *p_bitmap_obj);
extern INT32S cl2_bitmap_set_visible(t_BITMAP_OBJ *p_bitmap_obj, INT32U status);
extern INT32S cl2_bitmap_change_bitmap(t_BITMAP_OBJ *p_bitmap_obj, t_egui_image *p_new_bitmap);
extern void cl2_bitmap_zoom(t_BITMAP_OBJ *p_bitmap_obj, INT16S x, INT16S y, INT16S c_step,INT16S c_total);

//====================================================================
// text
#define C_TEXT_SPRITE_NUMBER			6
#define C_TEXT_BG_SPRITE_ID				0
#define C_TEXT_STRING_SPRITE_ID			1
#define C_TEXT_OUTLINE_SPRITE_ID		2 //(2 ~ 5)

#define C_TEXT_BLEDING_OFFSET			0
#define C_TEXT_BG_BLENDING_OFFSET		4
#define C_TEXT_OUTLINE_BLENDING_OFFSET	8

#define C_TEXT_SHOW_BG					0x00010000
#define C_TEXT_SHOW_OUTLINE_BOARD		0x00020000
#define C_TEXT_UPDATE_BG				0x00040000
#define C_TEXT_UPDATE_OUTLINE_BOARD		0x00080000
#define C_TEXT_UPDATE_STRING					0x00100000

typedef struct
{
	t_EGUI_OBJ		obj;
	// text
	INT16S			text_x;
	INT16S			text_y;
	INT16U			text_color;
	INT8U			text_font_size;
	INT8U			text_align;
	INT8U			text_outline_width;
	INT8U			text_outline_color;
	INT8U			language;
	CHAR			*p_string;
	// text bg
	INT16U			bg_color;
	INT32U			bg_buffer[8 * 8 / 4];
	// outline
	INT16U			outline_color;
	INT16U			outline_width;
	INT32U			outline_hor_buffer[8 * 8 / 4];
	INT32U			outline_ver_buffer[8 * 8 / 4];
	// depth and blending
	INT32U			depth;
	INT32U			blending;
	// sprite and position temp
	SPRITE			sprite_temp[C_TEXT_SPRITE_NUMBER];
	POS_STRUCT		pos_temp[C_TEXT_SPRITE_NUMBER];
} t_TEXT_OBJ;

extern t_TEXT_OBJ *cl2_text_create(
	INT16S	start_x,
	INT16S	start_y,
	INT16U	width,
	INT16U	height,
	// text
	INT16S	text_x,
	INT16S	text_y,
	INT16U	text_color,
	INT8U	text_font_size,
	INT8U	text_align,
	INT8U	text_outline_width,
	INT8U	text_outline_color,
	INT8U	language,
	CHAR	*p_string,
	// text bg
	INT16U	bg_color,
	// text outline
	INT16U	outline_color,
	INT16U	outline_width,
	// ppu parameter
	INT32U	depth,
	INT32U	blending,
	t_EGUI_OBJ	*p_parent,
	INT32U	flag
);
extern INT32S cl2_text_paint(t_TEXT_OBJ *p_text_obj);
extern INT32S cl2_text_redraw(t_TEXT_OBJ *p_text_obj);
extern INT32S cl2_text_destroy(t_TEXT_OBJ *p_text_obj);
extern INT32S cl2_text_set_visible(t_TEXT_OBJ *p_text_obj, INT32U status);
extern INT32S cl2_text_change_string(t_TEXT_OBJ *p_text_obj, CHAR *p_string);
extern void cl2_text_zoom(t_TEXT_OBJ *p_text_obj, INT16S x, INT16S y, INT16S c_step,INT16S c_total);

// Auto demo
#define DPF_AUTO_DEMO_EN	1
extern INT32S ap_umi_auto_demo_start(void);
extern INT32S ap_umi_auto_demo_check_sd(void);
extern INT32S ap_umi_auto_demo_handle_timer(void);
extern INT32S ap_umi_auto_demo_pause(void);
extern INT32S ap_umi_auto_demo_resume(void);
extern INT32S ap_umi_auto_demo_stop(void);
extern INT8U auto_run_status;

#ifndef _AUTO_RUN_STATUS_ENUM
#define _AUTO_RUN_STATUS_ENUM
typedef enum {
  	ENUM_AUTO_RUN_STATUS_STOP = 0x0,
  	ENUM_AUTO_RUN_STATUS_PLAY,
	ENUM_AUTO_RUN_STATUS_PAUSE
} AUTO_RUN_STATUS_ENUM;
#endif

// Firmware upgrade from SD card
extern void ap_umi_firmware_upgrade(void);


// Image cache management module relative definitions and APIs
// Skip image flag
#define C_IMAGE_CACHE_SKIP_MJPEG_AVI			0x01
#define C_IMAGE_CACHE_SKIP_MJPEG_MOV			0x02
#define C_IMAGE_CACHE_SKIP_PROGRESSIVE			0x04
#define C_IMAGE_CACHE_SKIP_GIF					0x08
#define C_IMAGE_CACHE_SKIP_BMP					0x10

typedef enum {
  	ENUM_IMAGE_STATUS_NOT_PARSED = 0x0,
  	ENUM_IMAGE_STATUS_PARSING,			// A parse command has been sent to image task
	ENUM_IMAGE_STATUS_PARSED,			// Image header parse done
	ENUM_IMAGE_STATUS_DECODING,			// A decode command has been sent to image task
	ENUM_IMAGE_STATUS_DECODED,			// Image decode to bitmap mode done. Waiting for scale command
	ENUM_IMAGE_STATUS_SCALING,			// A scaler command has been sent to image task
	ENUM_IMAGE_STATUS_READY,			// Decoded image buffer is stored in cache
	ENUM_IMAGE_STATUS_USED,				// Decoded image buffer has been taken by application
	ENUM_IMAGE_STATUS_SKIPPED,			// Parsing or decoding or scaling has been stopped by user
	ENUM_IMAGE_STATUS_ERROR,
	ENUM_IMAGE_STATUS_MAX
} IMAGE_CACHE_STATUS_ENUM;

// Photo view relative definitions and APIs
#define C_IMAGE_CACHE_CTRL_SEARCH_METHOD			0x0001		// 0=Ring mode, 1=Line mode
#define C_IMAGE_CACHE_CTRL_FILE_TYPE				0x0002		// 0=Photo+Video, 1=Video only
#define C_IMAGE_CACHE_CTRL_SCALE_TWICE				0x0004		// 0=disable, 1=enable
#define C_IMAGE_CACHE_CTRL_SAVE_INFO				0x0008		// 0=disable, 1=enable

typedef enum {
  	CMD_ID_NORMAL_IMAGE = 0x0,
  	CMD_ID_ROTATE_IMAGE,				// Used by photo view 7A rotation
  	CMD_ID_FOR_SCALE_IMAGE				// Used by photo view scale twice and thumbnail view encode backup
} IMAGE_CMD_ID_ENUM;

#define C_IMAGE_CACHE_FILENAME_LEN				48
typedef struct {
	INT32U image_index;					// Starts from 0
	INT8S cache_status;
	INT8U image_type;					// Refer to TK_IMAGE_TYPE_ENUM
	INT8U image_orient;
	INT8U reserved;
	INT8U filename[C_IMAGE_CACHE_FILENAME_LEN];
	INT32U file_size;
	INT8U photo_date_time[20];
	INT16U image_original_width;
	INT16U image_original_height;
	INT16U image_scaled_width;
	INT16U image_scaled_height;
	INT32U *data_ptr;
} IMAGE_CACHE_STRUCT;

extern INT32S image_cache_init(void);
extern void image_cache_ctrl_flag_set(INT16U flag);
extern void image_cache_ctrl_flag_clear(INT16U flag);
extern void image_cache_total_number_set(INT32U num);
extern void image_cache_current_index_set(INT32U index);
extern INT32U image_cache_index_diff_get(INT32U index1, INT32U index2);
extern void image_cache_skip_type_set(INT16U type);
extern void image_cache_filename_size_set(INT32U pixel);
extern void image_cache_parse_number_set(INT32U num);
extern void image_cache_decode_number_set(INT32U num);
extern INT32U image_cache_decode_number_get(void);

extern void image_cache_scale_method_set(INT8U method, INT32U width, INT32U height);
extern void image_cache_scale_format_set(INT32U format);
extern INT32S image_cache_max_limited_size_get(INT32U original_width, INT32U original_height, INT32U limited_width, INT32U limited_height, INT32U *zoom_width, INT32U *zoom_height);
extern INT32S image_cache_mim_limited_size_get(INT32U original_width, INT32U original_height, INT32U limited_width, INT32U limited_height, INT32U *zoom_width, INT32U *zoom_height);

extern INT32S image_cache_query(INT32U index, INT32U *cache_ptr);
extern INT32S image_cache_forward_search(INT32U search_count, INT32U *cache_ptr);
extern INT32S image_cache_backward_search(INT32U search_count, INT32U *cache_ptr);
extern INT32S image_cache_buffer_lock(INT32U index, INT32U *buffer_ptr);
extern INT32S image_cache_buffer_unlock(INT32U index, INT32U *buffer_ptr);
extern INT32S image_cache_update_check(void);
extern INT32S image_cache_video_extension_check(INT32U index);
extern INT32S image_cache_parse_cmd_done(IMAGE_HEADER_PARSE_STRUCT *parse_image_reply);
extern INT32S image_cache_decode_cmd_done(IMAGE_DECODE_STRUCT *decode_image_reply);
extern INT32S image_cache_scale_cmd_done(IMAGE_SCALE_STRUCT *scale_reply);
extern INT8U image_cache_busy_status_get(void);

extern void image_cache_flush(void);

// This structure is used by single-line and multi-line thumbnail views
typedef struct {
	// Input string
	INT8U *string_ptr;
	// Output image
	INT32U buf_ptr;
	INT16U valid_width;
	INT16U extend_width;
	INT16U valid_height;
	INT16U extend_height;
	INT8U char_size;
	// Display image
	INT8U align_mode;	// 0=left_top, 1=left_middle, 2=left_bottom, 3=center_top, 4=center_middle, 5=center_bottom, 6=right_top, 7=right+middle, 8=right_bottom
	INT8U depth;
	INT8U color;
	INT16S pos_x;
	INT16S pos_y;
	SPRITE string_sprite;
} STRING_IMG_STRUCT;

//Mass production test
extern INT8S ap_umi_mass_pro_test_start(void);

// for date format and time format
#define YMD			0
#define MDY			1
#define DMY			2

#define Digital_12  0
#define Digital_24  1

extern void umi_power_off_handler(void);

extern void synchRTC(void);


extern void cl2_music_progtime_create(INT16S start_x,
                               INT16S start_y,
                               INT32U depth,
                               INT32U blending,
                               INT8U  timeobj_type  /* 0:play time boject, 1: totoal time object */
                               );

extern void cl2_music_playprog_update(INT32U time,INT8U timeobj_type);
extern void cl2_music_playprog_destory(void);

#if SUPPORT_AI_FM_MODE == CUSTOM_ON
//State Music
extern void 	ap_audio_independent_playing_time_update_start(void);
extern void 	ap_audio_independent_playing_time_update_stop(void);
extern void 	ap_audio_independent_update_playing_time(ST_AUDIO_PLAY_TIME *playing_time);
extern INT8U 	AI_No_Photo_Mode_Get(void);
#endif

#if SETUP_SUPPORT_AUTO_PET==CUSTOM_ON
extern void ap_auto_pet_timer_irq(void);
#endif

extern void umi_standby_after_power_reset(void);

#if	(SUPPORT_TIME_INTERVAL_MODE!=SUPPORT_TIME_INTERVAL_M0)
// time interval
extern INT32U time_interval_slide_show_get_second_count( INT8U interval_time );
extern void time_interval_slide_show_set_index( INT8U interval_time );
extern INT32U time_interval_slide_show_get_index( void );
extern INT32U time_interval_calendar_get_index( void );
extern void time_interval_calendar_set_index( INT32U interval_time );
extern INT32U time_interval_calendar_get_second_count( INT8U interval_time );
#endif


//TFT back light for usbd state
extern void usbd_ftf_blight_power_on(void);
extern INT32U usbd_tftblight_on;
#endif  /* __TURNKEY_UMI_TASK_H__ */

// for TV out
extern void umi_tv_out_coordinate_trans(INT16S *ptr, INT16U num);
extern void umi_tv_out_X_axis_shift_trans(INT16S *ptr);
extern INT32S umi_display_device_switch(INT32U device);

//unicode strings
extern void ap_umi_language_init(void);
extern void ap_umi_uni_string_draw(GraphicBitmap *bitmap,INT8S *strings,GraphicCoordinates *coordinates,INT8U ascii_font_type);
extern INT32U ap_umi_lang_strings_width_get_for_no_newline(INT8S *strings,INT8U ascii_font_type);
extern INT32U ap_umi_lang_strings_height_get(INT8S *strings,INT8U ascii_font_type);
extern INT32U ap_umi_lang_font_width_get(INT16U uni_code,INT8U ascii_font_type);
#if(SLIDESHOW_WITH_VIDEO == CUSTOM_ON)
extern INT32U slideshow_total_file;
extern INT8U g_key_exit_flag;
#endif

#if SUPPORT_AI_FM_MODE == CUSTOM_ON
//FM module
extern void 	ap_State_FM_Set(INT8U FM_State);
extern INT8U 	ap_State_FM_Get(void);
#endif

extern INT32S spi_flash_disk_flush(void);

extern INT8U auto_run_status;
