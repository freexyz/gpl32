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
 *	2013.05.23	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "application.h"

#ifndef __FD_H__
#define __FD_H__

#ifdef   __cplusplus
extern   "C" {
#endif

extern INT32U		Face_Detect_Demo_En;
extern INT32U		drawFace_flag;
extern INT32U		train_counter;
extern INT32U		face_verify_flag;
extern INT32U		sensor_frame;
extern INT32U		verify_pass;
extern INT32U		verify_fail;
extern INT32U		face_count;
extern INT32U		*fiMem;
extern INT32U		*ownerULBP;
extern unsigned long	fdmode;

extern void		pic_face(INT32U frame_buffer);

extern int		adjustSecurity_set(int set_value);
extern int		adjustSecurity_init(int level);

extern void		ftrain_start(INT32U frame);
extern INT32U		ftrain_end(void);
extern void		ftrain_init(unsigned char prio);

extern INT32U		face_detect_return(void);
extern void		face_frame_set(INT32U frame);
extern void		fident_init(unsigned char prio);


#ifdef   __cplusplus
}
#endif

#endif	/* __FD_H__ */







