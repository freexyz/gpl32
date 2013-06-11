#ifndef __DRV_L1_PPU_H__
#define __DRV_L1_PPU_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

// PPU function control
#define C_PPU_DRV_MAX_FRAME_NUM				8
#define C_HBLANK_QUEUE_MAX		            10
#define HBLANK_IEQ_END                      0x8001
#define HBLANK_FRAME_END                    0x8002
#define PPU_FRAME_REGISTER_WAIT             0

// PPU control register
#define C_PPU_CTRL_ENABLE					0x00000001
#define C_PPU_CTRL_VGA_MODE					0x00000010
#define C_PPU_CTRL_NON_INTERLACE_MODE		0x00000020
#define C_PPU_CTRL_FRAME_MODE				0x00000080

// PPU frame mode control register
#define C_PPU_FRAME_TFT_UPDATED				0x00002000
#define C_PPU_FRAME_OUTPUT_FIELD			0x00004000
#define C_PPU_FRAME_TV_UPDATED				0x00008000

// PPU sprite control register
#define C_PPU_SPRITE_COLOR_DITHER_MODE		0x00020000

// Interrupt control register
#define C_PPU_INT_EN_PPU_VBLANK				0x00000001
#define C_PPU_INT_EN_VIDEO_POSITION			0x00000002
#define C_PPU_INT_EN_DMA_COMPLETE			0x00000004
#define C_PPU_INT_EN_PALETTE_ERROR			0x00000008
#define C_PPU_INT_EN_TEXT_UNDERRUN			0x00000010
#define C_PPU_INT_EN_SPRITE_UNDERRUN		0x00000020
#define C_PPU_INT_EN_SENSOR_FRAME_END		0x00000040
#define C_PPU_INT_EN_MOTION_DETECT			0x00000080
#define C_PPU_INT_EN_SENSOR_POSITION_HIT	0x00000100
#define C_PPU_INT_EN_MOTION_UNDERRUN		0x00000200
#define C_PPU_INT_EN_TV_UNDERRUN			0x00000400
#define C_PPU_INT_EN_TV_VBLANK				0x00000800
#define C_PPU_INT_EN_TFT_UNDERRUN			0x00001000
#define C_PPU_INT_EN_TFT_VBLANK				0x00002000
#define C_PPU_INT_EN_PPU_HBLANK				0x00004000

#define C_PPU_INT_EN_PPU_MASK				0x00007C3D

// Interrupt pending register
#define C_PPU_INT_PEND_PPU_VBLANK			0x00000001
#define C_PPU_INT_PEND_VIDEO_POSITION		0x00000002
#define C_PPU_INT_PEND_DMA_COMPLETE			0x00000004
#define C_PPU_INT_PEND_PALETTE_ERROR		0x00000008
#define C_PPU_INT_PEND_TEXT_UNDERRUN		0x00000010
#define C_PPU_INT_PEND_SPRITE_UNDERRUN		0x00000020
#define C_PPU_INT_PEND_SENSOR_FRAME_END		0x00000040
#define C_PPU_INT_PEND_MOTION_DETECT		0x00000080
#define C_PPU_INT_PEND_SENSOR_POSITION_HIT	0x00000100
#define C_PPU_INT_PEND_MOTION_UNDERRUN		0x00000200
#define C_PPU_INT_PEND_TV_UNDERRUN			0x00000400
#define C_PPU_INT_PEND_TV_VBLANK			0x00000800
#define C_PPU_INT_PEND_TFT_UNDERRUN			0x00001000
#define C_PPU_INT_PEND_TFT_VBLANK			0x00002000
#define C_PPU_INT_PEND_PPU_HBLANK			0x00004000
#define C_PPU_INT_PEND_PPU_MASK				0x00007C3D

// Definitions for driver control and status
#define C_PPU_CONTROL_LINE_MODE_UPDATE		0x0001
#define C_PPU_CONTROL_WAIT_FRAME_DONE		0x0002
#define C_PPU_STATUS_FRAME_MODE_BUSY		0x0001

#define PPU_EN_HBLANK                       0x8000
#define PPU_HBLANK_OFFSET_MASK              0x3FF

extern void ppu_init(void);
extern void ppu_display_resolution_lock(void);
extern void ppu_display_resolution_unlock(void);
extern INT32S ppu_frame_buffer_add(INT32U *frame_buf);
extern INT32S ppu_frame_buffer_get(void);
extern INT32S ppu_frame_buffer_display(INT32U *frame_buf);
extern INT32S ppu_tft_static_frame_set(INT32U *frame_buf);
extern INT32S ppu_tv_static_frame_set(INT32U *frame_buf);
extern INT32S ppu_deflicker_mode_set(INT32U value);
extern INT32S ppu_go(PPU_REGISTER_SETS *p_register_set, INT32U wait_start, INT32U wait_done);
extern INT8U ppu_frame_mode_busy_get(void);
extern INT32S ppu_notifier_register(void (*notifier)(void));
extern void ppu_notifier_unregister(void);
extern void set_ppu_free_control(INT8U INTL,INT16U H_size,INT16U V_size);
extern INT32S Hblenk_Enable_Set(INT32U value);
extern void Hblenk_Line_Offset_Set(INT32U line_offset);
extern void Hblenk_Go(void);
extern INT32S Hblenk_Wait(void);
extern INT32S set_ppu_go(PPU_REGISTER_SETS *p_register_set);
extern INT32S ppu_setting_update(PPU_REGISTER_SETS *p_register_set);
#endif		// __DRV_L1_PPU_H__