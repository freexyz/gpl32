#ifndef __drv_l2_USBH_WRITE_MULTI_SECTOR_H__
#define __drv_l2_USBH_WRITE_MULTI_SECTOR_H__
#include "usb_host_driver.h"

extern INT32S DrvUSBHWriteMultiSector(INT32U StartLBA, INT32U* StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32U CBI_Write2A(INT32U LUN, INT32U LBA, INT32U SectorCnt, INT32U* DataBuffer);

#endif