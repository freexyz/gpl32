/*
 ******************************************************************************
 * fdmm.h -
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
 *	2013.06.03	T.C. Chiu
 *
 ******************************************************************************
 */

#include "application.h"

#ifndef __FDMM_H__
#define __FDMM_H__

#ifdef   __cplusplus
extern   "C" {
#endif


/*
 * definition data type
 */
typedef enum {
	FDMODE_TRACK = 0,
	FDMODE_TRAIN,
	FDMODE_RECOG,
	FDMODE_DUMMY
} FDMODE;


/*
 * definition macros
 */
#define FDIO_GPIO0		(0x00000001UL)
#define FDIO_GPIO1		(0x00000002UL)
#define FDIO_GPIO2		(0x00000004UL)
#define FDIO_GPIO3		(0x00000008UL)
#define FDIO_GPIO4		(0x00000010UL)
#define FDIO_GPIO5		(0x00000020UL)
#define FDIO_GPIO6		(0x00000040UL)
#define FDIO_GPIO7		(0x00000080UL)


/*
 * declaration extern function
 */
extern void		fdio_hi(unsigned long n);
extern void		fdio_lo(unsigned long n);
extern void		fdio_toggle(unsigned long n);
extern void		fdio_set(unsigned long mask, unsigned long n);

extern void		fdmm_track(void);
extern void		fdmm_train(unsigned long c);
extern void		fdmm_recog(unsigned long c);
extern void		fdmm_dummy(INT32U millisecond);
extern INT32U		fdmm_wait(void);
extern void		fdmm_err(void);

extern void		fdmm(unsigned char prio, unsigned long gpio);


#ifdef   __cplusplus
}
#endif

#endif	/* __FDMM_H__ */
