#ifndef __AP_UMI_PPU_H__
#define __AP_UMI_PPU_H__

#include "turnkey_umi_task.h"


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

extern void palette_8bit_to_rgb1555_grayscale(INT16U width, INT16U height, INT8U *src, INT16U *dest, INT8U palette_index);
extern void alpha_channel_to_rgb1555_grayscale(INT16U width, INT16U height, INT16U *src, INT16U *dest, INT8U palette_index);
extern void rgb1555_to_rgb1555_grayscale(INT16U width, INT16U height, INT16U *src, INT16U *dest);
extern void ppu_palette_status_refresh(void);

extern INT32S tk_umi_ppu_os_init(void);

#define C_CURRENT_DISPLAY_DEVICE_TFT		0
#define C_CURRENT_DISPLAY_DEVICE_TV_VGA		1
#define C_CURRENT_DISPLAY_DEVICE_TV_QVGA	2
#define C_CURRENT_DISPLAY_DEVICE_TV_D1		3
extern INT32U ap_umi_display_device_get(void);
extern INT32S ap_umi_display_device_set(INT32U device);


#endif  // __AP_UMI_PPU_H__