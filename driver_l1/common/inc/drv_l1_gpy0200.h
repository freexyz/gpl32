#ifndef __drv_l1_GPY0200_H__
#define __drv_l1_GPY0200_H__


#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define POR_ON     0x10
#define KEY_SD     0x20
#define BAT32_OFF  0x40
#define OVT_OFF    0x80

extern void gpy0200_init(void);
extern void gpy0200_rtc_time_set(t_rtc *rtc_time,INT8U mask);
extern void gpy0200_rtc_hms_get(t_rtc *rtc_time);
extern void gpy0200_rtc_day_get(t_rtc *rtc_time);
extern void gpy0200_alarm_set(t_rtc *rtc_time, INT8U mask);
extern void gpy0200_alarm_get(t_rtc *rtc_time);
extern INT32S gpy0200_callback_set(INT8U int_idx, void (*user_isr)(void));
extern void gpy0200_rtc_function_set(INT8U mask, INT8U value);
extern void gpy0200_rtc_int_set(INT8U mask, INT8U value);
extern void gpy0200_irq_mask_set(INT8U mask, INT8U value);
extern void gpy0200_ldo33a_turn_off(void);
extern void gpy0200_ldo33b_set(BOOLEAN status);
extern void gpy0200_audio_en_set(BOOLEAN status);
extern INT8U gpy0200_battery_level_get(void);
extern INT8U gpy0200_adapter_plug_status_get(void);
extern void gpy0200_ldo50_en_set(BOOLEAN status);
extern void gpy0200_rtc_alarm_condition_set(INT8U mask,INT8U value);
extern void gpy0200_charger_current_set(INT8U value);
extern INT8U gpy0200_charger_current_get(void);
extern void gpy0200_tft_en_set(BOOLEAN status);
extern void gpy0200_vdac_en_set(BOOLEAN status);
extern void gpy0200_soft_reset(void);
extern void gpy0200_aging_pattern_en_set(BOOLEAN status,INT8U index);
extern void gpy0200_sram_data_wirte(INT8U offset,INT8U *data,INT8U len);
extern void gpy0200_sram_data_read(INT8U offset,INT8U *data,INT8U len);

#endif 		/* __drv_l1_GPY0200_H__ */