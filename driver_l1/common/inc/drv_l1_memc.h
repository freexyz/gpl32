#ifndef __drv_l1_MEMC_H__
#define __drv_l1_MEMC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

// SDRAM controller register 0

// SDRAM controller register 1
#define C_MEMC_CTRL1_REFRESH_CYCLE0		0x0000
#define C_MEMC_CTRL1_INIT4				0x0400
#define C_MEMC_CTRL1_CAS_LATENCY0		0x0000
#define C_MEMC_CTRL1_CAS_LATENCY1		0x1000
#define C_MEMC_CTRL1_CAS_LATENCY2		0x2000
#define C_MEMC_CTRL1_CAS_LATENCY3		0x3000

// SDRAM timing register
#define C_MEMC_TIMING_ROW_CYCLE4		0x0004
#define C_MEMC_TIMING_ROW_CYCLE8		0x0008
#define C_MEMC_TIMING_ROW_ACTIVE3		0x0030
#define C_MEMC_TIMING_ROW_ACTIVE7		0x0070
#define C_MEMC_TIMING_ROW_PRECHARGE1	0x0100
#define C_MEMC_TIMING_ROW_PRECHARGE2	0x0200
#define C_MEMC_TIMING_RAS_CAS_DELAY1	0x0400
#define C_MEMC_TIMING_RAS_CAS_DELAY2	0x0800
#define C_MEMC_TIMING_AP0				0x0000

extern void memc_init(void);


#endif		// __drv_l1_MEMC_H__