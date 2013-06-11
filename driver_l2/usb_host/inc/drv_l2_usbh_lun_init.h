#ifndef __drv_l2_USBH_LUN_INIT_H__
#define __drv_l2_USBH_LUN_INIT_H__
#include "usb_host_driver.h"

extern INT32S StandardCommand_CBI(INT32U bmRequestType,INT32U bRequest,INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U CBI_Byte1;
extern INT32S DrvUSBHLUNInit(INT32U LUN);
extern INT32U DrvUSBHGetSectorSize(void);
#endif