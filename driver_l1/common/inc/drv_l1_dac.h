#ifndef __drv_l1_DAC_H__
#define __drv_l1_DAC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define DAC_FEM_IF     (1 << 15)  /* DAC FIFO empty interrupt flag */
#define DAC_FEM_IEN    (1 << 14)  /* FIFO empty interrupt enable */
#define DAC_CH_EN      (1 << 13)  /* CHA,B enable */
#define DAC_VREFEN     (1 << 12)  /* DAC VREF control pin enable */
#define DAC_SIGNED     (1 << 11)  /* signed data */


#define DAC_CHB_SSF    (1 << 12)  /* CHB service frequency */
#define DAC_CHB_CHACFG (1 << 11)  /* CHB use CHA's configuration */
#define DAC_MONO       (1 << 10)  /* Mono mode */

#define DAC_FIFO_FULL  (1 << 15)  /* FIFO full flag */
#define DAC_FIFO_RST   (1 << 8)   /* FIFO reset */
#define DAC_FIFO_LEVEL (0xF << 4) /* FIFO level */

#define DAC_MUTE       (1 << 8)   /* mute */
#define DAC_PGA        (0x3F << 0) /* DAC PGA gain */

#define DAC_IIS_EN     (1 << 0)   /* IIS enable */

typedef enum
{
	DAC_ENABLED,
	DAC_DISABLED
}DAC_STATUS;

typedef enum
{
	DAC_CHA,
	DAC_CHB
}DAC_CHANNELS;

typedef struct
{
	INT16S *cha_data;
	INT16S *chb_data;
	INT32U cha_len;
	INT32U chb_len;
	INT32U cha_count;
	INT32U chb_count;
	INT8S *cha_notify;
	INT8S *chb_notify;
}DAC_CTRL;

extern void dac_init(void);
extern void dac_enable_set(BOOLEAN status);
extern void dac_disable(void);
extern void dac_cha_data_put(INT16S *data,INT32U len, INT8S *notify);
extern void dac_chb_data_put(INT16S *data,INT32U len, INT8S *notify);
extern INT32S dac_cha_data_dma_put(INT16S *data,INT32U len, INT8S *notify);
extern INT32S dac_chb_data_dma_put(INT16S *data,INT32U len, INT8S *notify);
#if _OPERATING_SYSTEM == 1
extern INT32S dac_cha_dbf_put(INT16S *data,INT32U len, OS_EVENT *os_q);
extern INT32S dac_cha_dbf_set(INT16S *data,INT32U len);
extern void dac_dbf_free(void);
extern INT32S dac_dbf_status_get(void);
extern INT32S dac_dma_status_get(void);
#endif
extern void dac_fifo_level_set(INT8U cha_level,INT8U chb_level);
extern void dac_sample_rate_set(INT32U hz);
extern void dac_mono_set(void);
extern void dac_stereo_set(void);
extern void dac_stereo_indi_buffer_set(void);
extern void dac_timer_stop(void);
extern void dac_pga_set(INT8U gain);
extern void dac_vref_set(BOOLEAN status);
extern INT8U dac_pga_get(void);

#endif		// __drv_l1_DAC_H__