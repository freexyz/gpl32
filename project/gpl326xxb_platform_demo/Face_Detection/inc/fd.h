/*
 ******************************************************************************
 * fd.h
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
 *	2013.05.23	T.C. Chiu
 *
 ******************************************************************************
 */
#include "application.h"

#include "fdmm.h"

#ifndef __FD_H__
#define __FD_H__

#ifdef   __cplusplus
extern   "C" {
#endif


/*
 * declaration global variable
 */
extern FDMODE		fdmode;
extern unsigned long	train_count;
extern unsigned long	ident_count;

extern INT32U		Face_Detect_Demo_En;
extern INT32U		drawFace_flag;

extern INT32U		*ownerULBP;

extern unsigned char	face_color[4];


/*
 * declaration extern function
 */
extern int		securitylvl_set(int set_value);

extern INT32U		face_detect_return(void);
extern void		face_frame_set(INT32U frame);

extern void		fd(unsigned char prio);


#ifdef   __cplusplus
}
#endif

#endif	/* __FD_H__ */







