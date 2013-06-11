#ifndef __AP_UMI_GUI_H__
#define __AP_UMI_GUI_H__

#include "turnkey_umi_task.h"

extern void ap_umi_display_loading_end(void);
extern void ap_umi_display_loading_start(INT16S pos_x, INT16S pos_y);
extern INT32S ap_umi_string_any_start(INT16U pos_x0, INT16U pos_y0, CHAR* string_buff, INT8U string_idx);
extern void ap_umi_string_any_end(INT8U string_idx);
extern INT32S ap_umi_lang_string_any_start(INT16U pos_x0, INT16U pos_y0, CHAR* string_buff, INT8U string_idx, INT8U language);
extern INT32S gui_file_name_shrink_with_ext_name(CHAR * src_str, CHAR * out_str, INT16S out_str_size, INT16S max_char_nums, CHAR * padding_str);
extern INT32S gui_file_name_shrink_by_pixel(CHAR* src_str, CHAR* out_str, INT16S out_str_size,INT16S pixel_w, CHAR* padding_str, INT8U lang, INT8U font_type);

#endif