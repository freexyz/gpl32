/****************************************************************/
/*                                                              */
/*                          globals.h                           */
/*                            DOS-C                             */
/*                                                              */
/*             Global data structures and declarations          */
/*                                                              */
/*                   Copyright (c) 1995, 1996                   */
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
#ifndef __GLOBALS_H__
#define __GLOBALS_H__
/*                                                                      */
/* Global variables                                                     */
/*                                                                      */
extern dmatch sda_tmp_dm;					/* Temporary directory match buffer     */

//extern struct dpb *DPBp;					/* First drive Parameter Block          */
extern struct buffer buffers[NUMBUFF];
extern struct buffer *firstbuf;		/* head of buffers linked list          */

extern struct f_node f_nodes_array[MAX_FILE_NUM];
extern struct f_node *f_nodes;  			/* pointer to the array        */

extern INT16S default_drive;      			/* default drive for dos   */ 
extern struct dpb  DPB[MAX_DISK_NUM];
extern struct cds  CDS[MAX_DISK_NUM];

extern INT16S	gFS_errno;

extern INT16U	gUnicodePage;				//当前使用的unicode page
extern BOOLEAN  gFAT2_WriteEn;              //FAT2 write(or not)

extern	struct nls_table g_nls_table;

extern struct Drv_FileSystem *FileSysDrv[];
extern const struct nls_table nls_ascii_table;

//disk find function var
extern STDiskFindControl	gstDiskFindControl;

/*
extern f_pos	g_cur_pos;
extern f_pos	g_cur_dir_pos;

extern INT16U	g_level;			//cur dir level
extern INT16S	find_begin_file;
extern INT16S	find_begin_dir;

extern STDirTrack	gstDirTrack;	//dir track
extern CLUSTER		g_sfn_cluster;
extern CLUSTER		g_sfn_cluster_offset;
*/

#if USE_GLOBAL_VAR == 1
	extern INT8S			gFS_path1[FD32_LFNPMAX];
	extern INT8S			gFS_name1[FD32_LFNMAX];
	extern tFatFind		gFS_sFindData;
	extern tDirEntry	gFS_slots[LFN_FETCH_SLOTS];
#endif

#ifndef _UNLINK_STEP
#define _UNLINK_STEP
    typedef enum {
        UNLINK_STEP_DISABLE=0,
        UNLINK_STEP_ENABLE=1,
        UNLINK_STEP_WORK=2
    } UNLINK_STEP;
#endif //_UNLINK_STEP



#ifndef _UNLINK_STEP
#define _UNLINK_STEP
    typedef enum {
        UNLINK_STEP_DISABLE=0,
        UNLINK_STEP_ENABLE=1,
        UNLINK_STEP_WORK=2
    } UNLINK_STEP;
#endif //_UNLINK_STEP


extern UNLINK_STEP unlink_step_status_get(void);
extern void unlink_step_init(f_node_ptr fnp, CLUSTER st);


#endif
