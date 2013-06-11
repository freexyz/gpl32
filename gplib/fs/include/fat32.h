/**************************************************************************
 * FreeDOS 32 FAT Driver                                                  *
 * by Salvo Isaja                                                         *
 *                                                                        *
 * Copyright (C) 2001-2003, Salvatore Isaja                               *
 *                                                                        *
 * This is "fat.h" - Driver's constants, structures and prototypes        *
 *                                                                        *
 *                                                                        *
 * This file is part of the FreeDOS 32 FAT Driver.                        *
 *                                                                        *
 * The FreeDOS 32 FAT Driver is free software; you can redistribute it    *
 * and/or modify it under the terms of the GNU General Public License     *
 * as published by the Free Software Foundation; either version 2 of the  *
 * License, or (at your option) any later version.                        *
 *                                                                        *
 * The FreeDOS 32 FAT Driver is distributed in the hope that it will be   *
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with the FreeDOS 32 FAT Driver; see the file COPYING;            *
 * if not, write to the Free Software Foundation, Inc.,                   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                *
 **************************************************************************/

#ifndef __FD32_FAT_H
#define __FD32_FAT_H

/* Use the following defines to add features to the FAT driver */
/* TODO: The FAT driver currently doesn't work with buffers disabled! */
/* TODO: The name cache is totally broken! */
#define FATBUFFERS   /* Uncomment this to use the buffered I/O        */
#define FATLFN       /* Define this to use Long File Names            */
#define FATWRITE     /* Define this to enable writing facilities      */
#define FATSHARE     /* Define this to enable sharing support         */
//#define FATNAMECACHE /* Define this to enable the opening name cache  */
#define FATREMOVABLE /* Define this to enable removable media support */
#define FAT_FD32DEV  /* Define this to enable FD32 devices support    */

/* 4-characters signatures to identify correct FAT driver structures */
#define FAT_VOLSIG 0x46415456 /* "FATV": FAT volume signature */
#define FAT_FILSIG 0x46415446 /* "FATF": FAT file signature   */

/* FIX ME: These should be read from CONFIG.SYS (or similar) */
#define FAT_MAX_FILES   20
#define FAT_MAX_BUFFERS 30

/* FAT Types */
typedef enum { FAT12, FAT16, FAT32 } tFatType;

/* EOC (End Of Clusterchain) check macros.                               */
/* These expressions are true (nonzero) if the value of a FAT entry is   */
/* an EOC for the FAT type. An EOC indicates the last cluster of a file. */
#define  FAT12_EOC(EntryValue)  (EntryValue >= 0x0FF8)
#define  FAT16_EOC(EntryValue)  (EntryValue >= 0xFFF8)
#define  FAT32_EOC(EntryValue)  (EntryValue >= 0x0FFFFFF8)

/* Bad cluster marks.                                                    */
/* Set a FAT entry to the FATxx_BAD value to mark the cluster as bad.    */
/*                                                                       */
/* The FAT file system specification says that to avoid confusion, no    */
/* FAT32 volume should ever be configured such that 0x0FFFFFF7 is an     */
/* allocatable cluster number. In fact an entry that would point to the  */
/* cluster 0x0FFFFFF7 would be recognised as Bad instead. Since values   */
/* greater or equal than 0x0FFFFFF8 are interpreted as EOC, I think we   */
/* can assume that the max cluster for a FAT32 volume is 0x0FFFFFF6.     */
/* That problem doesn't exist on FAT12 and FAT16 volumes, in fact:       */
/* 0x0FF7 = 4087 is greater than 4086 (max cluster for a FAT12 volume)   */
/* 0xFFF7 = 65527 is greater than 65526 (max cluster for a FAT16 volume) */
#define FAT12_BAD 0x0FF7
#define FAT16_BAD 0xFFF7
#define FAT32_BAD 0x0FFFFFF7

/* Special codes for the first byte of a directory entry */
#define FREEENT  0xE5 /* The directory entry is free             */
#define ENDOFDIR 0x00 /* This and the following entries are free */

/* FAT 32-byte Directory Entry structure */
#ifdef __ADS_COMPILER__
	PACKED typedef struct
	{
	  INT8S  Name[FNAME_SIZE + FEXT_SIZE];
	  INT8S  Attr;
	  INT8S  NTRes;
	  INT8S  CrtTimeTenth;
	  INT16S  CrtTime;
	  INT16S  CrtDate;
	  INT16S  LstAccDate;
	  INT16U  FstClusHI;
	  INT16S  WrtTime;
	  INT16S  WrtDate;
	  INT16U  FstClusLO;	
	  INT32U FileSize;
	} tDirEntry;   

	/* FAT 32-byte Long File Name Directory Entry structure */
	PACKED typedef struct
	{
	  INT8S Order;        /* Sequence number for slot        */
	  INT16S Name0_4[5];   /* First 5 Unicode characters      */
	  INT8S Attr;         /* Attributes, always 0x0F         */
	  INT8S Reserved;     /* Reserved, always 0x00           */
	  INT8S Checksum;     /* Checksum of 8.3 name            */
	  INT16S Name5_10[6];  /* 6 more Unicode characters       */
	  INT16U FstClus;      /* First cluster number, must be 0 */
	  INT16S Name11_12[2]; /* Last 2 Unicode characters       */
	} tLfnEntry;
#elif defined __CS_COMPILER__
	typedef struct
	{
	  INT8S  Name[FNAME_SIZE + FEXT_SIZE];
	  INT8S  Attr;
	  INT8S  NTRes;
	  INT8S  CrtTimeTenth;
	  INT16S  CrtTime;
	  INT16S  CrtDate;
	  INT16S  LstAccDate;
	  INT16U  FstClusHI;
	  INT16S  WrtTime;
	  INT16S  WrtDate;
	  INT16U  FstClusLO;	
	  INT32U FileSize;
	} PACKED tDirEntry;   

	/* FAT 32-byte Long File Name Directory Entry structure */
	typedef struct
	{
	  INT8S Order;        /* Sequence number for slot        */
	  INT16S Name0_4[5];   /* First 5 Unicode characters      */
	  INT8S Attr;         /* Attributes, always 0x0F         */
	  INT8S Reserved;     /* Reserved, always 0x00           */
	  INT8S Checksum;     /* Checksum of 8.3 name            */
	  INT16S Name5_10[6];  /* 6 more Unicode characters       */
	  INT16U FstClus;      /* First cluster number, must be 0 */
	  INT16S Name11_12[2]; /* Last 2 Unicode characters       */
	} PACKED tLfnEntry;
#else
	PACKED typedef struct
	{
	  INT8S  Name[FNAME_SIZE + FEXT_SIZE];
	  INT8S  Attr;
	  INT8S  NTRes;
	  INT8S  CrtTimeTenth;
	  INT16S  CrtTime;
	  INT16S  CrtDate;
	  INT16S  LstAccDate;
	  INT16U  FstClusHI;
	  INT16S  WrtTime;
	  INT16S  WrtDate;
	  INT16U  FstClusLO;	
	  INT32U FileSize;
	} tDirEntry;   

	/* FAT 32-byte Long File Name Directory Entry structure */
	PACKED typedef struct
	{
	  INT8S Order;        /* Sequence number for slot        */
	  INT16S Name0_4[5];   /* First 5 Unicode characters      */
	  INT8S Attr;         /* Attributes, always 0x0F         */
	  INT8S Reserved;     /* Reserved, always 0x00           */
	  INT8S Checksum;     /* Checksum of 8.3 name            */
	  INT16S Name5_10[6];  /* 6 more Unicode characters       */
	  INT16U FstClus;      /* First cluster number, must be 0 */
	  INT16S Name11_12[2]; /* Last 2 Unicode characters       */
	} tLfnEntry;
#endif

/* Finddata block for the internal "readdir" function (fat_readdir) */
typedef struct
{ 
  tDirEntry SfnEntry;
  INT32S     EntryOffset;
  INT16S     LfnEntries;

  INT8S      ShortName[36];	//(8+3)*3(utf8)+1(dot)+1(0) ShortName[18]; 	 //larger buffer, modify by zhangyan 051031
  INT8S      LongName[FD32_LFNPMAX << (1 - WORD_PACK_VALUE)];  // 将长文件名放在最下边 lanzhu @[1/17/2006]	   
} tFatFind;

/* Packs the high and low word of a tDirEntry's first cluster in a INT32S */
#define FIRSTCLUSTER(D) (((INT32U) D.FstClusHI << 16) + (INT32U) D.FstClusLO)

/* Nonzero if two tFile or tFileId structs refer to the same file */
#define SAMEFILE(F1, F2) (!memcmp(F1, F2, sizeof(tFileId)))

/* Nonzero if the file is a FAT12/FAT16 root directory */
#define ISROOT(F) ((F->DirEntrySector == 0) && (F->V->FatType != FAT32))

#endif /* #ifndef __FD32_FAT_H */

