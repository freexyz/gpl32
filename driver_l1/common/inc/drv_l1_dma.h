#ifndef __drv_l1_DMA_H__
#define __drv_l1_DMA_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define C_DMA_CHANNEL_NUM			4
#define C_DMA_CH0					0
#define C_DMA_CH1					1
#define C_DMA_CH2					2
#define C_DMA_CH3					3

#define C_DMA_STATUS_WAITING		0
#define C_DMA_STATUS_DONE			1
#define C_DMA_STATUS_TIMEOUT		-1

#define C_DMA_IO_ADDR_START			0xC0000000
#define C_DMA_IO_ADDR_END			0xDFFFFFFF

// DMA Control register
#define	C_DMA_CTRL_ENABLE			0x00000001
#define	C_DMA_CTRL_BUSY				0x00000002
#define	C_DMA_CTRL_SOFTWARE			0x00000000
#define	C_DMA_CTRL_EXTERNAL			0x00000004
#define	C_DMA_CTRL_NORMAL_INT		0x00000008
#define	C_DMA_CTRL_DEST_INCREASE	0x00000000
#define	C_DMA_CTRL_DEST_DECREASE	0x00000010
#define	C_DMA_CTRL_SRC_INCREASE		0x00000000
#define	C_DMA_CTRL_SRC_DECREASE		0x00000020
#define	C_DMA_CTRL_DEST_FIX			0x00000040
#define	C_DMA_CTRL_SRC_FIX			0x00000080
#define	C_DMA_CTRL_INT				0x00000100
#define	C_DMA_CTRL_RESET			0x00000200
#define	C_DMA_CTRL_M2M				0x00000000
#define	C_DMA_CTRL_M2IO				0x00000400
#define	C_DMA_CTRL_IO2M				0x00000800
#define	C_DMA_CTRL_8BIT				0x00000000
#define	C_DMA_CTRL_16BIT			0x00001000
#define	C_DMA_CTRL_32BIT			0x00002000
#define	C_DMA_CTRL_SINGLE_TRANS		0x00000000
#define	C_DMA_CTRL_DEMAND_TRANS		0x00004000
#define	C_DMA_CTRL_DBF				0x00008000
#define	C_DMA_CTRL_SINGLE_ACCESS	0x00000000
#define	C_DMA_CTRL_BURST4_ACCESS	0x00010000
#define	C_DMA_CTRL_BURST8_ACCESS	0x00020000

// DMA Transmit Count register
#define C_DMA_COUNT_MAX				0x00FFFFFF

// DMA Sprite register
//#define C_DMA_SPRITE_MAX			0x000003FF
#define C_DMA_SPRITE_MAX			0x00003FFF


// DMA Transparent register
#define C_DMA_TRANSPARENT_MAX		0x0000FFFF

// DMA Line register
//#define C_DMA_LINE_MAX				0x000007FF
#define	C_DMA_LINE_MAX					0x00003FFF

// DMA Miscellaneuous register
#define C_DMA_MISC_STATE_MASK		0x00000007
#define C_DMA_MISC_TIMEOUT_SHIFT	4
#define C_DMA_MISC_TIMEOUT_MAX		255
#define C_DMA_MISC_TIMEOUT_MASK		0x00000FF0
#define C_DMA_MISC_TRANSPARENT_EN	0x00001000
#define C_DMA_MISC_SPRITE_TO_FB		0x00000000
#define C_DMA_MISC_FB_TO_SPRITE		0x00002000
#define C_DMA_MISC_ERROR_WRITE		0x00004000
#define C_DMA_MISC_DMA_REQUEST		0x00008000

// DMA External IO register
#define	C_DMA0_IO_SHIFT				0
#define	C_DMA1_IO_SHIFT				4
#define	C_DMA2_IO_SHIFT				8
#define	C_DMA3_IO_SHIFT				12
#define	C_DMA_IO_MASK				0xF

#define C_DMA_IO_CFC				0x00
#define C_DMA_IO_DAC_CHA			0x01		// Left channel
#define C_DMA_IO_UART_TX			0x02
#define C_DMA_IO_UART_RX			0x03
#define C_DMA_IO_SDC				0x04
#define C_DMA_IO_MSC				0x05
#define C_DMA_IO_6					0x06
#define C_DMA_IO_DAC_CHB			0x07		// Right channel
#define C_DMA_IO_ADC				0x08
#define C_DMA_IO_SPI0_TX			0x09
#define C_DMA_IO_SPI0_RX			0x0A
#define C_DMA_IO_SPI1_TX			0x0B
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB)	&& (MCU_VERSION < GPL327XX)
	#define C_DMA_IO_SDC1				0x0B
#endif
#define C_DMA_IO_SPI1_RX			0x0C
#define C_DMA_IO_13					0x0D
#define C_DMA_IO_SPU_LEFT			0x0E
#define C_DMA_IO_SPU_RIGHT			0x0F
#define C_DMA_IO_MAX				0x0F
#define C_DMA_MEMORY				0xFF

// DMA channel enable register
#define C_DMA_CE_DONT_RESET			0x00000001

// DMA INT register
#define	C_DMA0_INT_PEND				0x00000001
#define	C_DMA1_INT_PEND				0x00000002
#define	C_DMA2_INT_PEND				0x00000004
#define	C_DMA3_INT_PEND				0x00000008
#define	C_DMA0_TIMEOUT				0x00000010
#define	C_DMA1_TIMEOUT				0x00000020
#define	C_DMA2_TIMEOUT				0x00000040
#define	C_DMA3_TIMEOUT				0x00000080
#define	C_DMA0_BUSY					0x00000100
#define	C_DMA1_BUSY					0x00000200
#define	C_DMA2_BUSY					0x00000400
#define	C_DMA3_BUSY					0x00000800


extern void dma_init(void);
extern INT32S dma_transfer(DMA_STRUCT *dma_struct);
extern INT32S dma_transfer_wait_ready(DMA_STRUCT *dma_struct);
#if _OPERATING_SYSTEM != _OS_NONE
extern INT32S dma_transfer_with_queue(DMA_STRUCT *dma_struct, OS_EVENT *os_q);
extern INT32S dma_transfer_with_double_buf(DMA_STRUCT *dma_struct, OS_EVENT *os_q);
extern INT32S dma_transfer_double_buf_set(DMA_STRUCT *dma_struct);
extern INT32S dma_transfer_double_buf_free(DMA_STRUCT *dma_struct);
#endif
extern INT32S dma_memory_fill(INT32U t_addr, INT8U value, INT32U byte_count);
extern INT32S dma_buffer_copy(INT32U s_addr, INT32U t_addr, INT32U byte_count, INT32U s_width, INT32U t_width);		// All parameters must be multiple of 2, s_width(source buffer width) must <= t_width(target buffer width)
extern INT32S dma_buffer_copy_extend(INT32U s_addr, INT32U t_addr, INT32U byte_count, INT32U s_width, INT32U t_width, INT32U dir, INT32U mode);

#endif		// __drv_l1_DMA_H__