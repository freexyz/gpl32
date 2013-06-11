#ifndef __drv_l1_CFC_H__
#define __drv_l1_CFC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define	C_CFC_TIMEOUT				0x0007ffff

#define CFC_SECTOR_SIZE			512


// set access time
#define C_SETUP_TIME			70
#define C_HOLD_TIME				20
#define C_ACCESS_TIME			165

#define C_SETUP_COUNT			(C_SETUP_TIME * MHZ / 1000 - 1)
#define C_HOLD_COUNT			(C_HOLD_TIME * MHZ / 1000 - 1)
#define C_ACCESS_COUNT			(C_ACCESS_TIME * MHZ / 1000 - 1)


#define	SUCCESS			0
#define ERR_FAILE		-1
#define	ERR_TIMEOUT		-2
#define	ERR_DMA_FAIL	-3
#define ERR_DMA_TIMEOUT	-4

void    cfc_uninit(void);
INT32S	cfc_initial(void);
INT32S	cfc_read_sector(INT32U sector, INT32U *buffer);
INT32S	cfc_write_sector(INT32U sector, INT32U *buffer);
INT32S	cfc_read_multi_sector(INT32U sector, INT32U *buffer, INT32U num);
INT32S	cfc_write_multi_sector(INT32U sector, INT32U *buffer, INT32U num);

#endif
