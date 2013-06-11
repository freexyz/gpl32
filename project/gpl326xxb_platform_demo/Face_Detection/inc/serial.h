/*
 ******************************************************************************
 * serial.h -
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
 *	2013.05.29	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include "application.h"
#include "drv_l1_uart.h"

#ifndef __SERAIL_H__
#define __SERAIL_H__

#ifdef   __cplusplus
extern   "C" {
#endif

#define CMD_MASK	(0x07)

#define CMD_TRACK	(0x01)
#define CMD_TRAIN	(0x03)
#define CMD_STATE	(0x05)
#define CMD_IDENT	(0x06)
#define CMD_ERASE	(0x07)

#define STA_TRKV	(0x01)
#define STA_TRKH	(0x02)
#define STA_TRAIN	(0x03)
#define STA_TRKAREA	(0x04)
#define STA_STATE	(0x05)
#define STA_IDENT	(0x06)


extern void		serial_send(unsigned char status, unsigned long c);
extern void		serial_init(unsigned char prio);

#ifdef   __cplusplus
}
#endif

#endif	/* __SERAIL_H__ */
