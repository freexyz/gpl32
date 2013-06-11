/*
 ******************************************************************************
 * frdbm.h - face recognition database manage
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
 *	2013.05.24	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include "application.h"

#ifndef __FRDBM_H__
#define __FRDBM_H__

#ifdef   __cplusplus
extern   "C" {
#endif

#define LBP_SZ			(1888)		/* the size   of Local Binary Pattern (LBP) */
#define LBP_NUM			(20)		/* the number of Local Binary Pattern (LBP) */
#define NAME_STRING_SZ		(256)		/* name string size */
#define NAME_AUDIO_SZ		(1024)		/* name audio size */

#define FRDB_NUM		(1)		/* the number of face recognition database */
//#define FRDB_NUM		(5)		/* the number of face recognition database */
#define FRDB_CARRIER_ALIGN	(4096)
#define FRDB_ALIGN(addr)	(((addr) + (FRDB_CARRIER_ALIGN-1)) & ~(FRDB_CARRIER_ALIGN-1))


struct frdb {
	unsigned char 	lbp[LBP_SZ*LBP_NUM];	/* point to LBP database */
	unsigned char	name_s[NAME_STRING_SZ];
	unsigned char	name_a[NAME_AUDIO_SZ];
	unsigned long	id;
	unsigned long	lbp_valid;		/* LBP valid */
	unsigned long	c_train;
	unsigned long	c_ident;
};

struct frdbm {
	unsigned long	base;			/* base address */
	unsigned long	offset;			/* offset address */
	unsigned long	curr_rd;
	unsigned long	curr_wr;
	unsigned long	curr;

	unsigned long	c_train;		/* command bitmap */
	unsigned long	s_train;		/* status  bitmap */
	unsigned long	c_ident;
	unsigned long	s_ident;

	struct frdb	*db[FRDB_NUM];
	int		(*frdb_carrier_init)(void);
	int		(*frdb_erase)(unsigned long addr, unsigned long size, unsigned char *buf);
	int		(*frdb_load)(unsigned long addr, unsigned long size, unsigned char *buf);
	int		(*frdb_store)(unsigned long addr, unsigned long size, unsigned char *buf);
};


extern int		frdb_init(unsigned long addr);
extern int		frdb_erase(unsigned long n);
extern int		frdb_load(unsigned long n);
extern int		frdb_store(unsigned long n);
extern unsigned long	frdb_state(void);

extern unsigned long	frdb_get_curr(void);
extern unsigned long	frdb_get_base(void);
extern unsigned long	frdb_get_offset(void);
extern void		frdb_set_c_ident(unsigned long bmap);
extern void		frdb_set_c_train(unsigned long bmap);
extern int		frdb_is_valid(unsigned long n);

extern unsigned char	*frdb_get_invalid(void);
extern unsigned char	*frdb_get_valid(void);
extern unsigned char	*frdb_get_c_train(void);
extern unsigned char	*frdb_get_c_ident(void);


#ifdef   __cplusplus
}
#endif

#endif	/* __FRDBM_H__ */
