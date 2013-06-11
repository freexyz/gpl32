#ifndef __DRV_L1_INTERRUPT_H__
#define __DRV_L1_INTERRUPT_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define VIC_ALM_SCH_HMS		1
#define VIC_TMB_ABC			2
#define VIC_3				3
#define VIC_4				4
#define VIC_SPI1			5
#define VIC_EXT_AB			6
#define VIC_KEY_CHANGE		7
#define VIC_KEY_SCAN		8
#define VIC_TIMER3		    9
#define VIC_TIMER2			10
#define VIC_11				11
#define VIC_DFR				12
#define VIC_SPI0		    13
#define VIC_UART			14
#define VIC_CF				15
#define VIC_MS				16
#define VIC_TIMER1			17
#define VIC_18				18
#define VIC_SD				19
#define VIC_NFC	    		20
#define VIC_USB 			21
#define VIC_SCALER			22
#define VIC_JPEG			23
#define VIC_DMA				24
#define VIC_TIMER0			25
#define VIC_26				26
#define VIC_PPU				27
#define VIC_ADCF			28
#define VIC_ADC				29
#define VIC_AUDB			30
#define VIC_AUDA			31
#define VIC_UNEXPECT		32
#define VIC_MAX_IRQ			33				// Don't forget to modify this when new interrupt source is added

#define VIC_SPU_PW			1
#define VIC_SPU_BEAT		2
#define VIC_SPU_ENV			3
#define VIC_SPU_FIQ			4
#define VIC_MAX_FIQ			5				// Don't forget to modify this when new fast interrupt source is added

// Interrupt IRQ mask register
#define C_VIC_IRQ_MASK_SET_ALL		0xFFFFFFFF

// Interrupt FIQ mask register
#define C_VIC_FIQ_MASK_SET_ALL		0x0000000F

// Interrupt Global mask register
#define C_VIC_GLOBAL_MASK_CLEAR		0x00000000
#define C_VIC_GLOBAL_MASK_SET		0x00000001


extern INT32U vic_global_mask_set(void);
extern void vic_global_mask_restore(INT32U old_mask);
extern INT32S vic_irq_register(INT32U vic_num, void (*isr)(void));
extern INT32S vic_irq_unregister(INT32U vic_num);
extern INT32S vic_irq_enable(INT32U vic_num);
extern INT32S vic_irq_disable(INT32U vic_num);
extern INT32S vic_fiq_register(INT32U vic_num, void (*isr)(void));
extern INT32S vic_fiq_unregister(INT32U vic_num);
extern INT32S vic_fiq_enable(INT32U vic_num);
extern INT32S vic_fiq_disable(INT32U vic_num);
extern void vic_init(void);


#endif		// __DRV_L1_INTERRUPT_H__