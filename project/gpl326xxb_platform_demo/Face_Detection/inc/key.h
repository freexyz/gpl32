/*
 ******************************************************************************
 * key.h -
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
 *	2013.06.06	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include "application.h"

#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"

#ifndef __KEY_H__
#define __KEY_H__

#ifdef   __cplusplus
extern   "C" {
#endif

#define MODE_STANDBY	0
#define MODE_TRAIN	1
#define MODE_IDENT	2


extern void		fd_chg_mode(int mode);
extern void		fd_demo(void);

#ifdef   __cplusplus
}
#endif

#endif	/* __KEY_H__ */
