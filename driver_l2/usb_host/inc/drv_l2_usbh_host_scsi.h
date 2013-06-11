#ifndef	__drv_l2_USBH_HOST_SCSI_H__
#define	__drv_l2_USBH_HOST_SCSI_H__
#include "usb_host_driver.h"

extern INT32U SCSICBWWithINData(INT32U* StoreAddr);
extern INT32U SCSICBWWithNOData(void);
extern INT32U SCSICBWWithDataOut(INT32U* StoreAddr);
extern INT32U BulkInData(INT32U DataTransferLength, INT32U* StoreAddr);
#endif

