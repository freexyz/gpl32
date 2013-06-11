#ifndef __drv_l1_MIC_H__
#define __drv_l1_MIC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


#define MIC_FIFO_LEVEL     (0xF << 4) /* FIFO level */
#define MIC_AUTO_CH_SEL    (0x3 << 4)
#define MIC_AUTO_ASIEN     (1 << 14)
#define MIC_ADBEN          (1 << 15)
#define MIC_ASIF           (1 << 15)
#define MIC_ASIEN          (1 << 14)
#define MIC_ASMEN          (1 << 3)
#define MIC_CNVRDY         (1 << 7)
#define MIC_STRCNV         (1 << 6)
#define MIC_RIEN           (1 << 14)
#define MIC_MIASE          (1 << 5)
#define MIC_ASIME          (1 << 4)
#define MIC_ADCRIF         (1 << 15)
#define MIC_ASADC_DMA      (1 << 11)
#define MIC_ASTMS          7
#define MIC_ASEN           (1 << 7)
#define MIC_EN             (1 << 6)
#define MIC_BITTRANS       (1 << 12)
#define MIC_AGCEN          (1 << 13)

#define MIC_ERR_WRONG_LEN  -1

#define MIC_BLOCK_LEN      2048

typedef struct
{
	INT8S *notify;
	INT16U *mic_data;
	INT32U data_len;
	INT32U count;
}MIC_CTRL;

extern void mic_init(void);
extern void mic_auto_int_set(BOOLEAN status);
extern void mic_fifo_level_set(INT8U level);
extern void mic_fifo_clear(void);
extern INT32S mic_auto_sample_start(void);
extern void mic_auto_sample_stop(void);
extern INT32S mic_sample_rate_set(INT8U timer_id, INT32U hz);
extern INT32S mic_timer_stop(INT8U timer_id);
extern void mic_vref_enable_set(BOOLEAN status);
extern void mic_agc_enable_set(BOOLEAN status);
extern INT32S mic_auto_data_dma_get(INT16U *data, INT32U len, INT8S *notify);
extern INT32S mic_auto_data_get(INT16U *data, INT32U len, INT8S *notify);

#if (defined MCU_VERSION) && (MCU_VERSION == GPL326XXB)
//add for DAGC
extern void mic_set_pga_gain(INT16U booster_gain, INT16U pga_gain);
extern INT16U mic_get_pga_gain(void);
extern INT16U mic_get_booster_gain(void);
extern void mic_dagc_enable(INT16U enable);
extern INT16U mic_dagc_get_status(void);
extern void mic_dagc_set_update_freq(INT16U update_freq);
extern void mic_dagc_setup(INT16U mode, INT16U threshold, INT16U att_scale, INT16U rel_scale, INT16U dis_zc);
extern INT16U mic_dagc_get_threshold(void);
extern void mic_dagc_set_att_rel_time(INT16U att_time, INT16U rel_time);
extern INT16U mic_dagc_get_att_time(void);
extern INT16U mic_dagc_get_rel_time(void);
#endif

#endif		// __drv_l1_MIC_H__
