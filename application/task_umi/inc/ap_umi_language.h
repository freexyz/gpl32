#ifndef __AP_UMI_LANGUAGE_H__
#define __AP_UMI_LANGUAGE_H__

#include "turnkey_umi_task.h"

typedef struct
{
	INT32U  start_value;
	INT32U  end_value;
	INT32U  base_addr;
	INT8U   font_height;
	INT8U   font_byte_per_line;
	INT8U   font_header_only;
	INT8U   dummy;
	INT8U   *uni_font_address;
} st_uni_range_tbl;

extern void ap_umi_language_init(void);
extern void ap_umi_uni_string_draw(GraphicBitmap *bitmap,INT8S *strings,GraphicCoordinates *coordinates,INT8U ascii_font_type);
extern INT32U ap_umi_lang_strings_width_get_for_no_newline(INT8S *strings,INT8U ascii_font_type);
extern INT32U ap_umi_lang_strings_height_get(INT8S *strings,INT8U ascii_font_type);
extern INT32U ap_umi_lang_font_width_get(INT16U uni_code,INT8U ascii_font_type);
#endif /* __AP_UMI_LANGUAGE_H__ */