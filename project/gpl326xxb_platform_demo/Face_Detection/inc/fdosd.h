/*
 ******************************************************************************
 * fdosd.h - face detection on screen display
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
 *	2013.09.30	T.C. Chiu
 *
 ******************************************************************************
 */
#include "application.h"

#ifndef __FDOSD_H__
#define __FDOSD_H__

#ifdef   __cplusplus
extern   "C" {
#endif


/*
 * definition macros
 */
#define fdosd_disp_recog()	fdosd_disp_mode(0)
#define fdosd_disp_train()	fdosd_disp_mode(1)
#define fdosd_disp_track()	fdosd_disp_mode(2)
#define fdosd_disp_slp()	fdosd_disp_mode(3)
#define fdosd_disp_sln()	fdosd_disp_mode(4)
#define fdosd_disp_erase()	fdosd_disp_mode(5)


/*
 * declaration extern function
 */
extern void		fdosd_ppu_init(void);
extern void		fdosd_disp_mode(int n);
extern void		fdosd_2digit_dec(int num);

extern void		fdosd(unsigned char prio);


#ifdef   __cplusplus
}
#endif

#endif	/* __FDOSD_H__ */
