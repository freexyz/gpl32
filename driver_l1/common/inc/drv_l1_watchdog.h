#ifndef __drv_l1_WATCHDOG_H__
#define __drv_l1_WATCHDOG_H__

#include "driver_l1.h"
#include "drv_l1_tools.h"


#define WATCHDOG_OK     1
#define WATCHDOG_FAIL   0

#define WATCH_DOG_BASE              0xD0000000
#define RESET_FLAG_ADDR             0xD0000018
#define LVR_CTRL_ADDR               (WATCH_DOG_BASE+0x00000020)
#define WATCHDOG_CTRL_ADDR          (WATCH_DOG_BASE+0x00000028)
#define WATCHDOG_CLEAR_ADDR         (WATCH_DOG_BASE+0x0000002C)

#define WATCHDOG_CLEAR_VALUE       0xA005


/* Mask define */
#define WATCHDOG_ENABLE_MASK       0x00008000

extern void watchdog_init(void);
extern INT32U watchdog_ctrl(INT32U WDGPD, INT32U WDGS, INT32U WDGEN);
extern INT32U watchdog_clear(void);

#endif		// __drv_l1_TOOLS_H__