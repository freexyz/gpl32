#ifndef __drv_l1_ADC_H__
#define __drv_l1_ADC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


#define ADC_FIFO_LEVEL     (0xF << 4) /* FIFO level */
#define ADC_AUTO_CH_SEL    (0x3 << 4)
#define ADC_MANUAL_CH_SEL  0x7
#define ADC_AUTO_ASIEN     (1 << 14)
#define ADC_ADBEN          (1 << 15)
#define ADC_ASIF           (1 << 15)
#define ADC_ASIEN          (1 << 14)
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
	#define ADC_ASMEN      (1 << 2)
#else //GPL326XXX
	#define ADC_ASMEN      (1 << 3)
#endif
#define ADC_CNVRDY         (1 << 7)
#define ADC_STRCNV         (1 << 6)
#define ADC_RIEN           (1 << 14)
#define ADC_MIASE          (1 << 5)
#define ADC_ASIME          (1 << 4)
#define ADC_ADCRIF         (1 << 15)
#define ADC_ASADC_DMA      (1 << 11)
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
	#define ADC_ASTMS      3
#else //GPL326XXX
	#define ADC_ASTMS      7
#endif
#define ADC_ASEN           (1 << 7)
#define ADC_TP_IF          (1 << 15)
#define ADC_TP_EN          (1 << 13)
#define ADC_TP_MODE        (1 << 11)
#define ADC_TP_INT_EN      (1 << 14)

#define ADC_ERR_WRONG_LEN  -1

#define ADC_BLOCK_LEN      2048

typedef struct
{
	INT8S *notify;
	INT16U *adc_data;
	INT32U data_len;
	INT32U count;
}ADC_CTRL;

void adc_auto_int_set(BOOLEAN status);

extern void adc_init(void);
extern void adc_fifo_level_set(INT8U level);
extern void adc_auto_ch_set(INT8U ch);
extern void adc_manual_ch_set(INT8U ch);
extern void adc_manual_callback_set(void (*user_isr)(INT16U data));
extern INT32S adc_manual_sample_start(void);
extern INT32S adc_auto_sample_start(void);
extern INT32S adc_auto_data_dma_get(INT16U *data, INT32U len, INT8S *notify);
extern INT32S adc_auto_data_get(INT16U *data, INT32U len, INT8S *notify);
extern void   adc_auto_sample_stop(void);
extern INT32S adc_sample_rate_set(INT8U timer_id, INT32U hz);
extern INT32S adc_timer_stop(INT8U timer_id);
extern void adc_fifo_clear(void);
extern INT32S adc_user_line_in_en(INT8U line_id,BOOLEAN status);
extern void tp_callback_set(void (*user_isr)(void));
extern void adc_tp_en(BOOLEAN status);
extern void adc_tp_int_en(BOOLEAN status);
extern void adc_vref_enable_set(BOOLEAN status);
#endif		// __drv_l1_ADC_H__