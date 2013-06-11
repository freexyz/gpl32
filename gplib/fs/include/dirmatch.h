/****************************************************************/
/*                                                              */
/*                          dirmatch.h                          */
/*                                                              */
/*               FAT File System Match Data Structure           */
/*                                                              */
/*                       January 4, 1992                        */
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
#ifndef __DIRMATCH_H__
#define __DIRMATCH_H__

typedef struct {
	INT8U dm_drive;
#if LFN_FLAG == 1
	#if CHEZAI_MP3 == 1
		INT8S dm_name_pat[FNAME_SIZE + FEXT_SIZE];
	#else
	INT8S dm_name_pat[FD32_LFNPMAX];
	#endif
#else
	INT8S dm_name_pat[FNAME_SIZE + FEXT_SIZE];
#endif
	INT8S dm_attr_srch;
	INT16U dm_entry;
	CLUSTER dm_cluster_offset;     /* 2012-08-20,add by duxiatang */
  	CLUSTER dm_cluster;             /* 2012-08-20,add by duxiatang */
#if ADD_ENTRY == 1
	INT16U dm_currentfileentry;		//2006.7.12 add
#endif
#if WITHFAT32 == 1
	INT32U dm_dircluster;
#else
	INT16U dm_dircluster;
	INT16U reserved;
#endif
	
	struct {
		INT32U                        /* directory has been modified  */
		f_dmod:1;
		INT32U                        /* directory is the root        */
		f_droot:1;
		INT32U                        /* fnode is new and needs fill  */
		f_dnew:1;
		INT32U                        /* fnode is assigned to dir     */
		f_ddir:1;
		INT32U                        /* filler to avoid a bad bug (feature?) in */
		f_filler:12;                /* TC 2.01           */
	} dm_flags;                   /* file flags                   */
	
	INT8S dm_attr_fnd;             /* found file attribute         */
	dostime dm_time;                 /* file time                    */
	dosdate dm_date;                 /* file date                    */
	INT32S dm_size;                 /* file size                    */
	INT8S dm_name[FNAME_SIZE + FEXT_SIZE + 2];     /* file name    */
} dmatch;

#endif
