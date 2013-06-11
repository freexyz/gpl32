/*
* Purpose: SDC Read/Write modulers of USBD(MSDC)
*
* Author: wschung
*
* Date: 2008/09/09
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
* Note : There is no any waiting for SDC DMA..
*/

#include "storages.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined SD_EN) && (SD_EN == 1)                               //
//================================================================//
extern INT32U sd_fs_offset[2]; 
INT8U single;
INT32S usdc_read_cmd_phase(INT32U lba,INT32U seccount);
INT32S usdc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S usdc_read_cmdend_phase(void);
INT32S usdc_write_cmd_phase(INT32U lba,INT32U seccount);
INT32S usdc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S usdc_write_cmdend_phase(void);

INT32S usdc_read_cmd_phase(INT32U lba,INT32U seccount)
{
  	return drvl2_sd_read_start(lba + sd_fs_offset[0], seccount);
}
INT32S usdc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
	return drvl2_sd_read_sector(buf, seccount, ifwait);
}

INT32S usdc_read_cmdend_phase(void)
{
	return drvl2_sd_read_stop();
}

INT32S usdc_write_cmd_phase(INT32U lba,INT32U seccount)
{
    return drvl2_sd_write_start(lba + sd_fs_offset[0], seccount);
}

INT32S usdc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
	return drvl2_sd_write_sector(buf, seccount, ifwait);
}

INT32S usdc_write_cmdend_phase(void)
{
    return drvl2_sd_write_stop();
}


struct Usb_Storage_Access USBD_SD_ACCESS = {
	usdc_read_cmd_phase,
	usdc_read_dma_phase,
	usdc_read_cmdend_phase,
	usdc_write_cmd_phase,
	usdc_write_dma_phase,
	usdc_write_cmdend_phase,
};
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined SD_EN) && (SD_EN == 1)                          //
//================================================================//