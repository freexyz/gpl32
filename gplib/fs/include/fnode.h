/****************************************************************/
/*                                                              */
/*                           fnode.h                            */
/*                                                              */
/*              Internal File Node for FAT File System          */
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
#ifndef __FNODE_H__
#define __FNODE_H__

struct f_node {
  INT16U f_count;                /* number of uses of this file  */
  INT16S f_mode;                 /* read, write, read-write, etc */

  INT16U f_flags;                /* file flags                   */

  struct dirent f_dir;          /* this file's dir entry image  */

  INT16U f_diroff;               /* offset/32 of the dir entry   */


  CLUSTER f_dirstart;           /* the starting cluster of dir  */
  								/* when dir is not root         */
  struct dpb *f_dpb;        /* the block device for file    */

  INT32U f_offset;               /* byte offset for next op      */
  CLUSTER f_cluster_offset;     /* relative cluster number within file */
  CLUSTER f_cluster; 
  
  //#ifdef _QLZ_CODE_FOR_SPEED_UP
  INT16U common_flag;	// for extend use
						// now just bit0 used !!! 	
  
  INT16U	f_link_attribute;
  INT32U link_divide_value; 
  INT32U  f_start_cluster; 
  
  INT32U plink_key_point;
  INT32U plink_cache_buf;
  //#endif           /* the cluster we are at        */
  
#if _DIR_UPDATA == 1
  INT32U f_dir_address;	/* the dir byte address */
  CLUSTER f_dir_cluster; /* the dir of the file was in this cluster */
#endif
};

#ifndef F_REGISTER_OPEN
	#define	F_REGISTER_OPEN	(0x01) 
#endif

struct ST_FAT16_INFLEXION
{
	INT16U 	cls_id;
	INT16U	length;
};

struct ST_FAT32_INFLEXION
{
	INT32U	cls_id;
	INT32U	length;
};

typedef struct 
{
	INT32U	total_cluster;
	INT32U	inflexion_count;
} ST_INFLEXION_HEADER;

#define	FLINK_ATTR_LINER			(0x01)
#define FLINK_ATTR_ALL_CACHE		(0x02)
#define FLINK_ATTR_KEY_CACHE		(0x04)
#define FLINK_ATTR_INFLEXION_CACHE	(0x08)
#define KEY_CLUSTER_LENGTH			(0x100)

#define F_DMOD  1               /* directory has been modified  */
#define F_DDIR  2               /* fnode is assigned to dir     */
#define F_DDATE 4               /* date set using setdate       */


//every C_MaxMarkDistance will make a mark
//the max number is 512, more then it will make same variable overflow
#define C_MaxMarkDistance		512

typedef struct f_node *f_node_ptr;

struct	STFileNodeMark
{
	INT32U		entry;						//标记所在的目录的位置
	INT16U		level;						//标记所在的目录深度
	INT16U		offset;						//标记在目录中的偏移量
	INT16U		dirtrack[C_MAX_DIR_LEVEL];	//标记的父目录路径
};

struct	STFolderNodeInfo
{
  #if SUPPORT_GET_FOLDER_NAME == 1
	INT32U		entry;
	INT16U		offset;
  #endif
	INT16U		filenum;
};

/*
typedef struct 
{
	INT16U	dir_offset[C_MAX_DIR_LEVEL];
	CLUSTER	cluster[C_MAX_DIR_LEVEL];
	CLUSTER cluster_offset[C_MAX_DIR_LEVEL];
} STDirTrack;
*/

#endif

