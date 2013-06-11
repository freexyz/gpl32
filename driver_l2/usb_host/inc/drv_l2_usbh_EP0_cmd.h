#ifndef	__drv_l2_USBH_EP0_CMD_H__
#define	__drv_l2_USBH_EP0_CMD_H__
#include "usb_host_driver.h"

extern INT32U StandardCommandWithINData(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U StandardCommandWithoutData(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U GetMaxLUN(INT32U Number);

#endif

