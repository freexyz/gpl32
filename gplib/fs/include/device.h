/****************************************************************/
/*                                                              */
/*                           device.h                           */
/*                      Device Driver Header File               */
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
#ifndef __DEVICE_H__
#define __DEVICE_H__


/* internal transfer direction flags                                    */
#define XFR_READ        1
#define XFR_WRITE       2
#define XFR_BYTEMODE	4	
#define XFR_WORDMODE	8	
#define NOT_DMA_SRAM	16	/* 2005-1-1 Yongliang add for _copy () */
							/* flag to update fcb_rndm field */
#define XFR_FCB_RANDOM  4

#define RDONLY          0
#define WRONLY          1
#define RDWR            2


/* Blockio constants                                                    */
#define DSKWRITE        1       /* dskxfr function parameters   */
#define DSKREAD         2
#define DSKWRITEINT26   3
#define DSKREADINT25    4

#define BPB_START_SECTOR	(0x3f)
#define UNSP_FS_RESERVED	(BPB_START_SECTOR+0x10)

/*                                                                      */
/* Bios Parameter Block structure                                       */
/*                                                                      */
#define FAT_NO_MIRRORING 0x80

#define BPB_SIZEOF 31           /* size of the standard BPB */

typedef struct {
	INT16U bpb_nbyte;            /* Bytes per Sector             */
	INT8U bpb_nsector;          /* Sectors per Allocation Unit  */
	INT16U bpb_nreserved;        /* # Reserved Sectors           */
	INT8U bpb_nfat;             /* # FAT's                      */
	INT16U bpb_ndirent;          /* # Root Directory entries     */
	INT16U bpb_nsize;            /* Size in sectors              */
	INT16U bpb_nfsect;           /* FAT size in sectors          */
	INT32U bpb_hidden;           /* Hidden sectors               */
	INT32U bpb_huge;             /* Size in sectors if           */
								/* bpb_nsize == 0               */
#if WITHFAT32 == 1
	INT32U bpb_xnfsect;          /* FAT size in sectors if       */
								/* bpb_nfsect == 0              */
	INT16U bpb_xflags;           /* extended flags               */
								/* bit 7: disable mirroring     */
								/* bits 6-4: reserved (0)       */
								/* bits 3-0: active FAT number  */
	INT32U bpb_xrootclst;        /* starting cluster of root dir */
	INT16U bpb_xfsinfosec;       /* FS info sector number,       */
								/* 0xFFFF if unknown            */
	INT16U bpb_xbackupsec;       /* backup boot sector number    */
								/* 0xFFFF if unknown            */
#endif
} bpb;

/* File system information structure */
struct fsinfo {
  INT32U fi_signature;          /* must be 0x61417272 */
  INT32S fi_nfreeclst;           /* number of free clusters, -1 if unknown */
  INT32S fi_cluster;             /* most recently allocated cluster, -1 if unknown */
  INT8U fi_reserved[12];
};

#endif
