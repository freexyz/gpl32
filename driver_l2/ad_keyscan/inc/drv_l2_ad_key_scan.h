#ifndef __DRV_L2_AD_KEY_SCAN_H__
#define __DRV_L2_AD_KEY_SCAN_H__

#include "application.h"

extern INT32U ADKEY_IO1, ADKEY_IO2, ADKEY_IO3, ADKEY_IO4;
extern INT32U ADKEY_IO5, ADKEY_IO6, ADKEY_IO7, ADKEY_IO8;
extern INT32U ADKEY_IO1_C, ADKEY_IO2_C, ADKEY_IO3_C, ADKEY_IO4_C;
extern INT32U ADKEY_IO5_C, ADKEY_IO6_C, ADKEY_IO7_C, ADKEY_IO8_C;

INT32U adc_key_scan_init(void);
INT32U adc_key_scan(void);

#endif