#ifndef __drv_l2_USBH_READ_MULTI_SECTOR_H__
#define __drv_l2_USBH_READ_MULTI_SECTOR_H__
#include "usb_host_driver.h"


extern INT32S DrvUSBHReadMultiSector(INT32U LBA, INT32U* StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32U CheckLUNStatus(INT32U LUN);

#endif