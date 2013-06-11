#ifndef __drv_l1_TFT_H__
#define __drv_l1_TFT_H__


#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define TFT_EN                  (1<<0)
#define TFT_CLK_SEL             (7<<1)
#define TFT_MODE                (0xF<<4)
#define TFT_VSYNC_INV           (1<<8)
#define TFT_HSYNC_INV           (1<<9)
#define TFT_DCLK_INV            (1<<10)
#define TFT_DE_INV              (1<<11)
#define TFT_H_COMPRESS          (1<<12)
#define TFT_MEM_BYTE_EN         (1<<13)
#define TFT_INTERLACE_MOD       (1<<14)
#define TFT_VSYNC_UNIT          (1<<15)

#define TFT_REG_POL             (1<<4)
#define TFT_REG_REV             (1<<5)
#define TFT_UD_I                (1<<6)
#define TFT_RL_I                (1<<7)
#define TFT_DITH_EN             (1<<8)
#define TFT_DITH_MODE           (1<<9)
#define TFT_DATA_MODE           (3<<10)
#define TFT_SWITCH_EN           (1<<12)
#define TFT_GAMMA_EN            (1<<13)
#define TFT_DCLK_SEL            (3<<14)
#define TFT_DCLK_DELAY          (7<<18)
#define TFT_SLIDE_EN            (1<<21)

#define TFT_MODE_UPS051         0x0
#define TFT_MODE_UPS052         0x10
#define TFT_MODE_CCIR656        0x20
#define TFT_MODE_PARALLEL       0x30
#define TFT_MODE_TCON           0x40

#define TFT_MODE_MEM_CMD_WR     0x80
#define TFT_MODE_MEM_CMD_RD     0x90
#define TFT_MODE_MEM_DATA_WR    0xa0
#define TFT_MODE_MEM_DATA_RD    0xb0
#define TFT_MODE_MEM_CONTI      0xc0
#define TFT_MODE_MEM_ONCE       0xd0


#define TFT_DATA_MODE_8         0x0             
#define TFT_DATA_MODE_565	    0x400
#define TFT_DATA_MODE_666       0x800
#define TFT_DATA_MODE_888       0xC00

#define TFT_DCLK_SEL_0          0x0000
#define TFT_DCLK_SEL_90         0x4000
#define TFT_DCLK_SEL_180        0x8000
#define TFT_DCLK_SEL_270        0xC000

#define TFT_DCLK_DELAY_0        0x000000
#define TFT_DCLK_DELAY_1        0x040000
#define TFT_DCLK_DELAY_2        0x080000
#define TFT_DCLK_DELAY_3        0x0C0000
#define TFT_DCLK_DELAY_4        0x100000
#define TFT_DCLK_DELAY_5        0x140000
#define TFT_DCLK_DELAY_6        0x180000
#define TFT_DCLK_DELAY_7        0x1C0000

#define TFT_REG_POL_1LINE       0x0
#define TFT_REG_POL_2LINE       0x1
#define TFT_NEW_POL_EN          0x80

#define PWM_VSET                0x70
#define PWM_VSET_BIT            4

extern void tft_init(void);
extern void tft_start(INT32U model);
extern void tft_clk_set(INT32U clk);
extern void tft_dith_en_set(BOOLEAN status);
extern void tft_slide_en_set(BOOLEAN status);
extern INT32S tft_led_backlight_level_set(INT8U level);
extern INT32S tft_ccfl_backlight_level_set(INT8U level);
extern INT32S tft_pwm0_en_set(BOOLEAN status);
extern INT32S tft_backlight_en_set(BOOLEAN status);
extern void  tft_start_with_pwm(INT32U model,BOOLEAN pwm0_vghl_en,BOOLEAN pwm1_backlight_en);
extern void tft_gamma_en_set(BOOLEAN status);
extern void tft_display_buffer_set(INT32U value);
extern void tft_display_range_save(void);
extern void tft_restricted_display(INT32U w, INT32U h);
extern void tft_display_range_restore(void);

void tft_gpm733_init(void);
void tft_toppoly_ups052_init(void);
void tft_innolux_at080tn03_init(void);
void tft_auo_a080sn01_init(void);
void tft_wintek_wdf4827y_init(void);
void tft_samsung_ltp700wvf01_init(void);
void tft_innolux_at080tn42_init(void);
void tft_auo_a102v201_init(void);
void tft_tpo_td070wgcb2_init(void);
void tft_tpo_td035ttea1_init(void);
void tft_auo_c080vw02_init(void);
void tft_auo_c070vw02_init(void);
void tft_lgp_lb064v02_init(void);
void tft_tianma_tm035kdh03_init(void);
void tft_innolux_at070tn92_init(void);

/* i80 panel */
void tft_fox_fl320wqc11_init(void);
void tft_gpm_lm765a0_init(void);
void tft_gpm_lm765h0_init(void);

#if 0 /* display by himax */
void tft_innolux_at070tn07_init(void);
void tft_auo_c065gw02_init(void);
void tft_innolux_at056tn03_init(void);
void tft_auo_a040cn01_init(void);
void tft_pmv_035bg_init(void);
#endif
/* analog panel */
void tft_gpy0200_analog_panel_init(void);
void tft_gpy0200_auo_c065gw02_init(void);
void tft_gpy0200_innolux_at056tn03_init(void);
void tft_gpy0200_auo_a040cn01_init(void);
void tft_gpy0200_pmv_035bg_init(void);

void tft_auo_a024cn02v9_init(void);
void tft_tpo_td025thea7_init(void);
void tft_tpo_td025thd1_init(void);
void tft_HSD070IDW1_init(void);
void tft_GPM879A0_init(void);
void tft_AUO_A035QN02_init(void);
void tft_gp_gpm758a0s_init(void);
void tft_chilin_lq035nc111_init(void);

void tft_tft_en_set(BOOLEAN status);
void tft_mode_set(INT32U mode);
void tft_mem_mode_ctrl_set(INT32U mem_ctrl);
void tft_signal_inv_set(INT32U mask, INT32U value);
void tft_h_compress_set(BOOLEAN status);
void tft_mem_unit_set(BOOLEAN status);
void tft_interlace_set(BOOLEAN status);
void tft_vsync_unit_set(BOOLEAN status);
void tft_data_mode_set(INT32U mode);
void tft_rb_switch_set(BOOLEAN status);
void tft_dclk_sel_set(INT32U sel);
void tft_dclk_delay_set(INT32U delay);
void tft_reg_pol_set(INT8U pol);
void tft_new_pol_en_set(BOOLEAN status);
void tft_auo_spi_write(INT8U reg, INT8U data);
void tft_GPM879A0_spi_write(INT8U reg, INT8U data);
void tft_AUO_A035QN02_spi_write(INT8U reg, INT16U data);
void tft_auo_a070vw02_init(void);
void tft_cpt_claa070ld0acw_init(void);
void tft_innolux_at080tn52_init(void);
void tft_chimei_q08009_701_init(void);
void tft_auo_m150xn07_init(void);

void tft_tpo_spi_write(INT32U data, INT32U len);
void tft_tpo_spi_data_build(INT8U *in_data,INT8U mode, INT8U *out_data, INT32U *out_len);

#endif 		/* __drv_l1_TFT_H__ */