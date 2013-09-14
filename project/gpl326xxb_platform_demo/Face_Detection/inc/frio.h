/*
 ******************************************************************************
 * frio.h -
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.06.03	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include "application.h"

#ifndef __FRIO_H__
#define __FRIO_H__

#ifdef   __cplusplus
extern   "C" {
#endif

#define FRIO_GPIO0		(0x00000001UL)
#define FRIO_GPIO1		(0x00000002UL)
#define FRIO_GPIO2		(0x00000004UL)
#define FRIO_GPIO3		(0x00000008UL)
#define FRIO_GPIO4		(0x00000010UL)
#define FRIO_GPIO5		(0x00000020UL)
#define FRIO_GPIO6		(0x00000040UL)
#define FRIO_GPIO7		(0x00000080UL)

extern void			frio_hi(unsigned long n);
extern void			frio_lo(unsigned long n);
extern void			frio_toggle(unsigned long n);
extern void			frio_set(unsigned long n, unsigned long mask);
extern void			frio_hi_latency(unsigned long n, unsigned long time);
extern void			frio_init(unsigned char prio);


#ifdef   __cplusplus
}
#endif

#endif	/* __FRIO_H__ */
