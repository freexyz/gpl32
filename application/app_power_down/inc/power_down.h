#ifndef __POWER_DOWN_H__
#define __POWER_DOWN_H__

#include "application.h"

/* wake up source enable */
#define EXTA_WAKEUP				1
#define EXTB_WAKEUP				1
#define RTC_WAKEUP				1

/* bit define */
#define	BIT0					(1<<0)
#define	BIT1					(1<<1)
#define	BIT2					(1<<2)
#define	BIT3					(1<<3)
#define	BIT4					(1<<4)
#define	BIT5					(1<<5)
#define	BIT6					(1<<6)
#define	BIT7					(1<<7)
#define	BIT8					(1<<8)
#define	BIT9					(1<<9)
#define	BIT10					(1<<10)
#define	BIT11					(1<<11)
#define	BIT12					(1<<12)
#define	BIT13					(1<<13)
#define	BIT14					(1<<14)
#define	BIT15					(1<<15)

//Functions for GPIO IO. GPIO input includes:

//GPIO on IOA
#define GPIO_IRLINKPWR_CNTRL	BIT7
#define GPIO_PALVIDEO			BIT6
#define GPIO_PWRBTN				BIT5
#define GPIO_CARTSEATED			BIT4
#define GPIO_CART_TYPE			BIT3
#define GPIO_NF_WP				BIT2
#define GPIO_WAKELED			BIT1
#define GPIO_3V3_SDRAM_CTRL		BIT0

//GPIO on IOB
#define GPIO_MFG_TEST			BIT5
#define GPIO_USB_DETECT			BIT3
#define GPIO_ATAPLED_0			BIT0

//GPIO on IOC
#define GPIO_POWERBUTTON		BIT10
#define GPIO_ATAPLED_1			BIT4
#define GPIO_ATAPLED_2			BIT5

//GPIO on IOG
#define GPIO_ATAPLED_3			BIT5
#define GPIO_ATAPLED_4			BIT6

#define ADC_LINE_NUM_0			(1<<0)
#define ADC_LINE_NUM_1			(1<<1)
#define ADC_LINE_NUM_2			(1<<2)
#define ADC_LINE_NUM_3			(1<<3)

#endif
