#ifndef __drv_l1_CACHE_H__
#define __drv_l1_CACHE_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define C_CACHE_SIZE					(16*1024)
#define C_CACHE_BANK_NUM				4
#define C_CACHE_AREA_START				0x00000000
#define C_CACHE_AREA_END				0x3FFFFFFF

#define C_CACHE_INDEX_MASK				0x00000FF0
#define C_CACHE_INDEX_SHIFT				4
#define C_CACHE_TAG_MASK				0x3FFFF000
#define C_CACHE_TAG_SHIFT				12

// Cache Valid_Lock_Tag register
#define C_CACHE_VLT_TAG_MASK			0x0003FFFF
#define C_CACHE_VLT_LOCK				0x00040000
#define C_CACHE_VLT_VALID				0x00080000
#define C_CACHE_VLT_INDEX_SHIFT			4
#define C_CACHE_VLT_BANK_SHIFT			20

// Cache control register
#define C_CACHE_CTRL_EN					0x00000001
#define C_CACHE_CTRL_WRITE_BUFFER_EN	0x00000002
#define C_CACHE_CTRL_WRITE_BACK_EN		0x00000004

// Cache config register
#define C_CACHE_CFG_BURST_EN			0x00000001
#define C_CACHE_CFG_SINGLE_EN			0x00000002
#define C_CACHE_CFG_VLT_EN				0x00000004

// Invalid cache line register
#define C_CACHE_INV_LINE_INVALID		0x00000001
#define C_CACHE_INV_LINE_DRAIN			0x00000002
#define C_CACHE_INV_LINE_INDEX_MASK		0x00000FF0
#define C_CACHE_INV_LINE_BANK_SHIFT		2
#define C_CACHE_INV_LINE_EN				0x80000000

// Cache lockdown register
#define C_CACHE_LOCK_TAG_INDEX_MASK		0x3FFFFFF0
#define C_CACHE_LOCK_BANK_SHIFT			2
#define C_CACHE_LOCK_EN					0x80000000

// Drain write buffer register
#define C_CACHE_WRITE_BUF_DRAIN_EN		0x00000001

// Drain cache line register
#define C_CACHE_DRAIN_LINE_INDEX_MASK	0x00000FF0
#define C_CACHE_DRAIN_LINE_BANK_SHIFT	2
#define C_CACHE_DRAIN_LINE_EN			0x80000002

// Invalid cache bank register
#define C_CACHE_INV_BANK_INVALID		0x00000001
#define C_CACHE_INV_BANK_DRAIN			0x00000002
#define C_CACHE_INV_BANK_BANK_SHIFT		2
#define C_CACHE_INV_BANK_EN				0x80000000

// Invalid cache range register
#define C_CACHE_INV_RANGE_INVALID		0x00000001
#define C_CACHE_INV_RANGE_DRAIN			0x00000002
#define C_CACHE_INV_RANGE_SIZE_MAX		0x3FFFFFF0
#define C_CACHE_INV_RANGE_ADDR_MASK		0x3FFFFFF0
#define C_CACHE_INV_RANGE_EN			0x80000000

extern INT32S cache_lockdown(INT32U addr, INT32U size);
extern INT32S cache_drain_range(INT32U addr, INT32U size);
extern INT32S cache_invalid_range(INT32U addr, INT32U size);
extern INT32S cache_burst_mode_enable(void);
extern void cache_burst_mode_disable(void);
extern void cache_init(void);
extern void cache_enable(void);
extern void cache_disable(void);
extern void cache_write_back_enable(void);
extern void cache_write_back_disable(void);


#endif		// __drv_l1_CACHE_H__