/****************************************************************/
/*                                                              */
/*                            dcb.h                             */
/*                                                              */
/*                DOS Device Control Block Structure            */
/*                                                              */
/*                       November 20, 1991                      */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/
#ifndef __DCB_H__
#define __DCB_H__

/* Internal drive parameter block                               */
struct dpb {
	INT8S dpb_unit;                /* unit for error reporting     */
	INT16U dpb_secsize;            /* sector size                  */
	INT8U dpb_clsmask;            /* mask (sectors/cluster-1)     */
	INT8U dpb_shftcnt;            /* log base 2 of cluster size   */
	//zhangzha modified
	//INT16U dpb_fatstrt;          /* FAT start sector             */
	INT32U dpb_fatstrt;            /* FAT start sector             */
	INT8U dpb_fats;               /* # of FAT copies              */
	INT16U dpb_dirents;            /* # of dir entries             */
	//zhangzha modified
	//INT16U dpb_data;               /* start of data area           */
	INT32U dpb_data;               /* start of data area           */
	INT16U dpb_size;               /* # of clusters+1 on media     */
	INT16U dpb_fatsize;            /* # of sectors / FAT           */
	//zhangzha modified
	//INT16U dpb_dirstrt;            /* start sec. of root dir       */
	INT32U dpb_dirstrt;            /* start sec. of root dir       */
	INT16U dpb_cluster;            /* cluster # of first free      */
								/* -1 if not known              */
#if WITHFAT32 == 0
	INT16U dpb_nfreeclst;          /* number of free clusters      */
  								/* -1 if not known              */
#else
	union {
		struct {
			INT16U dpb_nfreeclst_lo;
			INT16U dpb_nfreeclst_hi;
		} dpb_nfreeclst_st;
		INT32U _dpb_xnfreeclst;      /* number of free clusters      */
									/* -1 if not known              */
	} dpb_nfreeclst_un;
#define dpb_nfreeclst dpb_nfreeclst_un.dpb_nfreeclst_st.dpb_nfreeclst_lo
#define dpb_xnfreeclst dpb_nfreeclst_un._dpb_xnfreeclst

	INT16U dpb_xflags;             /* extended flags, see bpb      */
	//zhangzha modified
	//INT16U dpb_xfsinfosec;         /* FS info sector number,       */
	INT32U dpb_xfsinfosec;         /* FS info sector number,       */
								/* 0xFFFF if unknown            */
	//zhangzha modified
	//INT16U dpb_xbackupsec;         /* backup boot sector number    */
	INT32U dpb_xbackupsec;         /* backup boot sector number    */
								/* 0xFFFF if unknown            */
	INT32U dpb_xdata;
	INT32U dpb_xsize;              /* # of clusters+1 on media     */
	INT32U dpb_xfatsize;           /* # of sectors / FAT           */
	INT32U dpb_xrootclst;          /* starting cluster of root dir */
	INT32U dpb_xcluster;           /* cluster # of first free      */
								/* -1 if not known              */
	INT8U dpb_fsinfoflag; 			 /* change flag for fsinfo */
#endif
	INT8U dpb_mounted; 			 /* the device has been mounted */
};

#define UNKNCLUSTER      0x0000 /* see RBIL INT 21/AH=52 entry */
#define XUNKNCLSTFREE    0xffffffffl    /* unknown for DOS */
#define UNKNCLSTFREE     0xffff /* unknown for DOS */

#define FSINFODIRTYFLAG	0x01	//add by zhangyan
#define FSINFOLOADED	0x02	//add by matao

#endif
