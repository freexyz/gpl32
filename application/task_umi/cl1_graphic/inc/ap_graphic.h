#ifndef __GRAPHIC_AP__
#define __GRAPHIC_AP__
#include "turnkey_umi_task.h"


extern INT32U cl1_graphic_init(GraphicBitmap *bitmap);
extern void cl1_graphic_finish(GraphicBitmap *bitmap);
extern void cl1_graphic_bitmap_front_color_set(GraphicBitmap *bitmap,INT16U color);
extern void cl1_graphic_bitmap_background_color_set(GraphicBitmap *bitmap,INT16U color);
extern void cl1_graphic_frame_string_draw(GraphicBitmap *bitmap,char *strings,INT8U language,INT8U font_type,GraphicFrameChar *frame);
extern void cl1_graphic_frame_char_draw(GraphicBitmap *bitmap,	INT16U character,INT8U language,INT8U font_type,GraphicFrameChar *frame);
extern void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern void cl1_graphic_char_draw(GraphicBitmap *bitmap,unsigned short character,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern void cl1_graphic_line_draw(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void cl1_graphic_rectangle_draw(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);
extern void cl1_graphic_strings_centers_draw(StrinsCentersDrawPar *parameter,GraphicBitmap* bitmap,char *strings,INT8U language,INT8U font_type);
extern void cl1_graphic_free_strings_centers_draw(GraphicBitmap *bitmap);

extern INT32U cl1_graphic_font_width_get(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_height_get(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_font_line_bytes_get(INT16U character_code,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_newline_get(char *strings,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_width_to_char_num(char *strings,INT32U total_width,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_width_get_for_no_newline(char *strings,INT8U language,INT8U font_type);
extern INT32U cl1_graphic_strings_height_get(char *strings,INT8U language,INT8U font_type);
extern void   cl1_graphic_strings_copy(INT8S *dest,INT8S *source,INT16U length );
extern void   cl1_graphic_character_sizes_get(INT16U width,INT16U height,INT16U *char_x,INT16U *char_y);

extern INT32U cl1_graphic_multinational_strings_width_get_for_no_newline(char *strings,INT8U language);
extern INT32U cl1_graphic_multinational_font_height_get(INT16U character_code,INT8U language);
extern void cl1_graphic_multinational_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language);

extern void   cl1_graphic_music_string_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates);

#endif