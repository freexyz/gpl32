#ifndef __drv_l2_USBH_MSDC_INIT_H__
#define __drv_l2_USBH_MSDC_INIT_H__
#include "usb_host_driver.h"

//*****************************************************************************
extern INT32S DrvUSBHMSDCInitial(void);
extern INT32S DrvUSBHMSDCUninitial(void);
extern INT32S DrvUSBHMSDCUninitial(void);
extern void usbh_pwr_reset_register(void (*fp_pwr_reset)(void));
extern void usbh_pwr_reset(void);
#endif