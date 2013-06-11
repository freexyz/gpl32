/****************************************************************/
/*                                                              */
/*                          proto.h                             */
/*                                                              */
/*                   Global Function Prototypes                 */
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
/* License along with DOS-C; see the file COPYING.  If not,   */
/* write to the Free Software Foundation, 675 Mass Ave,       */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/
#ifndef __PTOTO_H__
#define __PROTO_H__

#if LFN_FLAG == 1
//mapping common name to LFN api and short name api
	#define 	 fat_unlink 	 fs_lfndelete 	
	#define 	 fat_unlink2 	 fs_lfndelete2    //for minidvr 	
	#define 	 fat_mkdir 		 fs_lfnmkdir
	#define 	 fat_open 		 fs_lfnopen 	
	#define 	 fat_rename 	 fs_lfnrename 	
	#define 	 fat_rmdir 		 fs_lfnrmdir 	
	#define 	 fat_chdir 		 fs_lfncd
	#define 	 fat_findfirst 	 fs_lfnfindfirst
	#define 	 fat_findnext 	 fs_lfnfindnext
	#define 	 fat_deleteall 	 fs_lfndeleteall
    #define      fat_unlink_step        fs_lfndel_step 
    #define      fat_unlink_step_start  fs_lfndel_step_start
    #define      fat_unlink_step_end    fs_lfndel_step_end
    #define      fat_unlink_step        fs_lfndel_step 
    #define      fat_unlink_step_start  fs_lfndel_step_start
    #define      fat_unlink_step_end    fs_lfndel_step_end
#else
	 #define 	 fat_unlink 	 fs_delete 	
	 #define 	 fat_mkdir 		 fs_mkdir
	 #define 	 fat_open 		 fs_fopen 	
	 #define 	 fat_rename 	 fs_rename 	
	 #define 	 fat_rmdir 		 fs_rmdir 	
	 #define 	 fat_chdir 		 fs_cd
	 #define 	 fat_findfirst 	 fs_findfirst
	 #define 	 fat_findnext 	 fs_findnext
	 #define 	 fat_deleteall 	 fs_deleteall 
#endif

#define fat_close		fs_fclose
#define	fat_seek		fs_fseek
#define fat_seek64      fs_fseek64
#define fat_write		fs_fwrite
#define fat_read		fs_fread
#define	fat_getfsize	fs_getfsize
#define	fat_getftime	fs_getftime
//#define fat_setfattr	fs_setfattr
#define	fat_getfattr	fs_getfattr
//#define	fat_setfsize	fs_setfsize
//#define	fat_setftime	fs_setftime
#define	fat_getcwd		fs_getcwd
#define fat_free		fs_free

//INT8U fs_getcurdir( INT8U drive, INT8S *buf);
CHAR * fs_getcwd(CHAR *buf, INT16S buflen);
INT16S fs_fclose(INT16S fd);
INT16S fs_smartchdir(INT32U org_path);

INT32S fs_fread(INT16S fd, INT32U bp, INT32U n);  
INT32S fs_fwrite(INT16S fd, INT32U bp, INT32U n);
INT32S fs_freadB(INT16S fd, INT32U bp, INT16U n);
INT32S fs_fwriteB(INT16S fd, INT32U bp, INT16U n);

CLUSTER fs_free( INT8U disk);
INT64S fs_fseek64(INT16S fd, INT64S foffset, INT16S origin);
INT32S fs_fseek(INT16S fd, INT32S foffset, INT16S origin);
INT32S fs_getfsize(INT16S fd);
INT16S fs_setfattr(INT16S fd, INT16U attrp);
INT16S fs_getfattr(INT16S fd);
BOOLEAN fs_setfsize(INT16S fd, INT32S size);

INT16S fs_setftime(INT16S fd, INT16U acdate, INT16U moddate, INT16U modtime);
INT16S fs_getftime(INT16S fd, dosdate * dp, dostime * tp);

BOOLEAN dir_exists(INT8S * path);
INT16S fs_changedisk( INT8U disk);

INT16S fs_mount(INT8U dsk);
INT16S fs_unmount(INT8U dsk);

INT16S fs_setdirattr(INT8S *dirpath, INT16U attr);
INT16S fs_getdirattr(INT8S *dirpath, INT16U *attr);

/* blockio.c */
struct buffer *getblk(INT32U blkno, INT16S dsk, BOOLEAN overwrite);
#define getblock(blkno, dsk) getblk(blkno, dsk, FALSE);
#define getblockOver(blkno, dsk) getblk(blkno, dsk, TRUE);
BOOLEAN flush1(struct buffer *bp);
BOOLEAN flush_buffers(REG INT16S dsk);
BOOLEAN DeleteBlockInBufferCache(INT32U blknolow, INT32U blknohigh, INT16S dsk, INT16S mode);
struct buffer * searchblock(INT32U blkno, INT16S dsk);
void move_buffer(struct buffer *bp, INT32U firstbp);
BOOLEAN clear_buffers(REG INT16S dsk);
BOOLEAN invalid_buffers(REG INT16S dsk);
BOOLEAN flush_fat_buffers(REG INT16S dsk);	// zurong add for fat buffer flush.2011.01.11

INT16U dskxfer(INT16S dsk, INT32U blkno, INT32U buf, INT16U numblocks, INT16S mode);

struct cds * get_cds(INT16U dsk);

/* dosnames.c */
INT16S ParseDosName(const INT8S *, INT8S *, BOOLEAN);
INT8S check_lfn_dir(INT8U *name);

/* fatdir.c */
void dir_init_fnode(f_node_ptr fnp, CLUSTER dirstart);
f_node_ptr dir_open(const INT8S *dirname);
INT16S dir_read(REG f_node_ptr fnp);
BOOLEAN dir_write(REG f_node_ptr fnp);
void dir_close(REG f_node_ptr fnp);
//INT16S fs_findfirst(INT8S *name, struct f_info *f_info, INT16U attr);
//INT16S fs_findnext(struct f_info *f_info);
//void ConvertName83ToNameSZ(INT8S *destSZ, const INT8S *srcFCBName);

/* fatfs.c */
struct dpb * get_dpb(INT16S dsk);
INT32U clus2phys(CLUSTER cl_no, struct dpb *dpbp);
CLUSTER phys2clus(INT32U sec_no, struct dpb *dpbp);
//INT32S fs_fopen(INT8S * path, INT16U flag, INT16U attrib);
BOOLEAN fcbmatch(const INT8S *fcbname1, const INT8S *fcbname2);
BOOLEAN fcmp_wild(const INT8S * s1, const INT8S * s2, INT16U n);
//INT16S fs_delete(INT8S * path);
//INT16S fs_rmdir(INT8S * path);
//INT16S fs_rename(INT8S * path1, INT8S * path2);

//INT16S fs_mkdir(INT8S * dir);
INT16S fs_lfnfindfirst(INT8S *name, struct f_info *f_info, INT16U attr);
INT16S fs_lfnfindnext(struct f_info *f_info);
INT16S fs_lfndelete(INT8S * dirpath);
INT16S fs_lfndelete2(INT8S * dirpath);
INT16S fs_lfnmkdir(CHAR *dirpath);
INT32S fs_lfnopen(CHAR *path, INT16U flags, INT16U attrib);
INT16S fs_lfnrename(INT8S * path1, INT8S * path2);
INT16S fs_lfnrmdir(INT8S * dirpath);
INT16S fs_lfncd(CHAR *PathName);
INT16S fs_lfndeleteall (INT8S *); 
INT16S fs_get_fnode_pos(f_pos *fpos);

f_node_ptr get_f_node(void);
void release_f_node(f_node_ptr fnp);
#define release_near_f_node(fnp) ((fnp)->f_count = 0)
INT16S media_check(REG struct dpb *dpbp);
f_node_ptr xlt_fd(INT16S fd);
INT16S xlt_fnp(f_node_ptr fnp);

/* fattab.c */
INT16S read_fsinfo(struct dpb *dpbp);
void write_fsinfo(struct dpb *dpbp);

/* newstuff.c */
//INT16S truename(const INT8S *src, INT8S *dest);

/* fatfs.c */
#if WITHFAT32 == 1
INT16S bpb_to_dpb(bpb *bpbp, REG struct dpb *dpbp, BOOLEAN extended); 
#else
INT16S bpb_to_dpb(bpb *bpbp, REG struct dpb *dpbp);
#endif

void	check_flink_sequence(f_node_ptr fnp);
CLUSTER link_fat(struct dpb *dpbp, CLUSTER Cluster1, REG CLUSTER Cluster2);
INT16S dos_extend(f_node_ptr fnp);
CLUSTER next_cluster(struct dpb *dpbp, REG CLUSTER ClusterNum);
CLUSTER fast_next_cluster(f_node_ptr filenp, CLUSTER reletive_clus);
CLUSTER find_fat_free(f_node_ptr fnp);
INT32S fs_GetCluster(INT16S fd);
void fs_getdiskfree ( INT32U *, INT32U *, INT32U *, INT16S);
CLUSTER extend(f_node_ptr);
struct buffer * getFATblock(struct dpb *dpbp, CLUSTER clussec);
INT16S map_cluster(REG f_node_ptr fnp, INT16S mode);

INT16S fast_map_cluster(f_node_ptr fnp, INT32U InLength, INT16U* OutLength, INT16S mode);
INT16S fast_map_cluster_end(f_node_ptr fnp, INT16U nLength);

CLUSTER fast_extend(f_node_ptr fnp, INT32U InLength, INT16U* OutLength);
CLUSTER fast_link_fat(struct dpb * dpbp , CLUSTER start_cls, INT16U length);
CLUSTER fast_find_fat_free(f_node_ptr fnp, INT32U InLength, INT16U* OutLength);

INT32S fast_rdblock(INT16S fd, INT32U lpBuf, INT32U count, INT16S mode);
INT32S fast_wrblock(INT16S fd, INT32U lpBuf, INT32U count, INT16S mode);
INT32S rwblock(INT16S fd, INT32U buffer, INT16U count, INT16S mode);

INT16S fat_format (INT16S dsk, INT8U FATType, INT16U ReservedSectorNum);
INT16S fat_sformat (INT16S dsk, INT32U totalsectors, INT32U realsectors);

int fs_StatFileNumByExtName(int dsk, INT8S * extname, INT32U *filenum);
INT16S fs_UpdataDir(INT16S fd);
INT16S dir_lfn_updata(f_node_ptr fnp, tDirEntry *shortdir);
INT16S fs_SplitFile(INT16S tagfd, INT16S srcfd, INT32U splitpoint);
INT16S fs_InserPartFile(INT16S tagfd, INT16S srcfd, INT32U tagoff, INT32U srclen);
INT16S fs_DeletePartFile(INT16S fd, INT32U offset, INT32U length);
INT16S fs_FileRepair(INT16S fd);

f_node_ptr descend_path(CHAR *Path, tFatFind *D);
INT16S true_current_path(CHAR *path); 
INT16S check_path_length (INT8S * p_name, INT16U len);
void pre_truename(INT8S *dest);
void pre_truename_dir(CHAR *dest);

f_node_ptr split_path(INT8S *, INT8S *);
void split_path_LFN(CHAR *FullPath, CHAR *Path, CHAR *Name);
INT16S fat_find(f_node_ptr fnp, CHAR *FileSpec, INT32S Flags, tFatFind *FindData);
INT16S fat_creat(f_node_ptr fnp, INT8S *Name, INT8S Attr);
INT16S delete_dir_entry(f_node_ptr fnp);
INT16S delete_dir_entry2(f_node_ptr fnp);

BOOLEAN find_fname(f_node_ptr, INT8S *, INT16S);
void wipe_out(f_node_ptr);
void wipe_out_clusters(struct dpb *dpbp, CLUSTER st);
INT16S alloc_find_free(f_node_ptr fnp, INT8S *path, INT8S *fcbname);
void init_direntry(struct dirent *dentry, INT16U attrib,CLUSTER cluster);

INT16S remove_lfn_entries(f_node_ptr fnp);
INT8S check_lfn(CHAR *name);
INT16S extend_dir(f_node_ptr fnp);

INT8U fs_toupper(INT8U c);
INT8U fs_tolower(INT8U c);

void _5c_to_ff(CHAR *);
void _ff_to_5c(CHAR *);

INT8U GetBytefromWord(void *p, INT16S pos);
INT16U getWordfromWord(void *p, INT16S pos);
INT32U getLongfromWord(void *p, INT16S pos);

void shrink_file(f_node_ptr);
INT16S dir_lfnread(f_node_ptr fnp, tDirEntry *shortdir);
INT16S dir_lfnwrite(f_node_ptr fnp, tDirEntry *shortdir);
INT16S lfn_readdir(f_node_ptr Dir, tFatFind *Ff);
INT16S gen_short_fname(f_node_ptr fnp, CHAR *LongName, CHAR *ShortName);
INT16S split_lfn(tLfnEntry *Slot, tDirEntry *D, CHAR *LongName, INT16S *NumSlots);
INT16S find_empty_dir_entries(f_node_ptr fnp, INT16S NumEntries);

void putLfnEnt(tLfnEntry *pLfnEnt, tDirEntry *pDirEnt);
struct buffer * getblock_from_off(f_node_ptr fnp, INT16U secsize);
void swap_deleted(INT8U *name);
INT16S allocate_sfn_dir_entry(f_node_ptr fnp, tDirEntry *D, CHAR *FileName);
INT16S allocate_lfn_dir_entries(f_node_ptr fnp, tDirEntry *D, CHAR *FileName);
INT16S check_ifis_lfn(UTF8 *Source, INT16S SourceSize, INT16S DestSize);
INT16S fd32_get_origin_short_name(CHAR *Dest, CHAR *Source);
INT8S *itoa(INT16S value, INT8S *string, INT16S radix);
INT16S fetch_lfn(tDirEntry *Buffer, INT16S BufferPos, INT16S *Name);
INT8S * fs_strlwr(INT8S * upin);
void copy_char_in_lfn_slot(tLfnEntry *Slot, INT16S SlotPos, UTF16 Ch);

INT16S Read_DBR(INT16S dsk,bpb *bpbp);

INT16S fs_LoadFatTable(REG f_node_ptr fnp);

//查找当前目录下的第一个文件
f_ppos sfn_findfirst(STDiskFindControl *pstDiskFindControl, INT8S dsk, CHAR *extname, struct sfn_info *sfn_info, INT16S attr,INT32S (*filter)(INT8S *str));
//查找当前目录下的文件
f_ppos sfn_findnext(STDiskFindControl *pstDiskFindControl, INT8S dsk, CHAR *extname, struct sfn_info *sfn_info, INT16S attr, INT32S (*filter)(INT8S *str));
//进入子目录
f_ppos sfn_changedir(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
//切换到上层目录
f_ppos sfn_fatherdir(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
//change disk
f_ppos sfn_changedisk(STDiskFindControl *pstDiskFindControl, INT16S dsk);
INT16S sfn_readdir(f_node_ptr Dir);


f_ppos sfn_paperchangedisk(STDiskFindControl *pstDiskFindControl, INT16S dsk);


//获取当前目录的位置
void sfn_getcurpos(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
//设置当前目录的位置
void sfn_setcurpos(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
//读取长目录项
INT16S sfn_readlfn(f_node_ptr Dir, tFatFind *Ff);

//获取当前文件所在目录的f_pos
void sfn_getcurdirpos(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
//设置当前文件所在目录的f_pos
void sfn_setcurdirpos(STDiskFindControl *pstDiskFindControl, f_ppos ppos);
INT8S sfn_cmpdirpos(f_ppos ppos1, f_ppos ppos2);

INT16S CompressFileNodeInfoBuffer(struct STFileNodeInfo *stFNodeInfo);

INT32U fast_GetTotalSpeedupCluster(REG f_node_ptr fnp);
INT32U fast_GetNextInflexion(REG f_node_ptr fnp, CLUSTER cluster);

INT16S handle_error_code (INT16S);

void MemPackByteCopyFar(INT32U s, INT32U d, INT16U n);
void _MemPackWord2ByteCopyFar( INT32U  s, INT32U  dest, INT16U cnt);
void _MemPackByte2WordCopyFar( INT32U  sour, INT32U  d, INT16U cnt);

void copyfarpoint( INT32U  s, INT8S  *d);

INT32U fs_read_dword(INT32U addr);
INT16U fs_read_word(INT32U addr);
void fs_write_dword(INT32U addr, INT32U val);
void fs_write_word(INT32U addr, INT16U val);

INT8S * fs_strupr(INT8S * lowin);

#define merge_file_changes(fnp, collect)
#define save_far_f_node(fp) 

void fs_memset_word(void* p, INT16U val, INT32U size);

#define fmemset  memset
#define fmemcpy  memcpy

/*                                                              */
/* special word packing prototypes                              */
/*                                                              */
#ifdef LITTLE_ENDIAN
#	define getbyte(vp) (*(INT8U *)(vp))
#	define putbyte(vp, b) (*(INT8U *)(vp)=b)
	INT32U getlong(void *);
	INT16U getword(void *);
	void putlong(void *, INT32U);
	void putword(void *, INT16U);
#	define putdirent(dp, bp) memcpy(bp, dp, DIRENT_SIZE);
#	define getdirent(bp, dp) memcpy(dp, bp, DIRENT_SIZE);

#elif defined BIG_ENDIAN
#	define getbyte(vp) (*(INT8U *)(vp))
#	define putbyte(vp, b) (*(INT8U *)(vp)=b)
	INT32U getlong(void *);
	INT16U getword(void *);
	void putlong(void *, INT32U);
	void putword(void *, INT16U);
	void getdirent(INT8U * bp, struct dirent * dp);
	void putdirent(struct dirent *dp, INT8U *bp);

#elif defined unSP
#	define getbyte(vp) (*(INT8U *)(vp))
#	define putbyte(vp, b) (*(INT8U *)(vp)=b)
	INT16U getword(void *);
	void putword(void *, INT16U);
	INT32U getlong(void *);
	void putlong(void *, INT32U);
	void getdirent(INT8U * bp, struct dirent * dp);
	void putdirent(struct dirent *dp, INT8U *bp);

#else
#	define getlong(vp) (*(INT32U *)(vp))
#	define getword(vp) (*(INT16U *)(vp))
#	define getbyte(vp) (*(INT8U *)(vp))
#	define putbyte(vp, b) (*(INT8U *)(vp)=b)
#	define putlong(vp, l) (*(INT32U *)(vp)=l)
#	define putword(vp, w) (*(INT16U *)(vp)=w)
#	define putdirent(dp, bp) memcpy(dp, bp, DIRENT_SIZE);
#	define getdirent(bp, dp) memcpy(bp, dp, DIRENT_SIZE);
#endif

dostime time_encode(dostime_t *t);
INT8S * time_decode( INT16U *tp, CHAR *timec);
INT8S * date_decode( INT16U *dp, CHAR *datec);
dosdate dos_getdate(void);
dostime dos_gettime(void);

#if WITHFAT32 == 1
	CLUSTER getdstart(struct dpb *dpbp, struct dirent *dentry);
	void setdstart(struct dpb *dpbp, struct dirent *dentry, CLUSTER value);
	BOOLEAN checkdstart(struct dpb *dpbp, struct dirent *dentry, CLUSTER value);
#else
	#define getdstart(dpbp, dentry) \
		((dentry)->dir_start)
	#define setdstart(dpbp, dentry, value) \
		(((dentry)->dir_start) = (INT16U)(value))
	#define checkdstart(dpbp, dentry, value) \
		(((dentry)->dir_start) == (INT16U)(value))
#endif


///* NAMES.C - File name management services */
INT16S  fd32_gen_short_fname(CHAR *Dest, CHAR *Source, INT32S Flags);
INT16S  fd32_build_fcb_name(CHAR *Dest, CHAR *Source);
INT16S  fd32_expand_fcb_name(INT8S *Dest, INT8S *Source);
INT16S  fd32_compare_fcb_names(INT8S *Name1, INT8S *Name2);

/* LFN.C - Long file names support procedures */
INT8S lfn_checksum   (tDirEntry *D);

#define ChartToUTF8(Chart, UTF8Chart) oemcp_to_utf8(Chart, UTF8Chart)
#define UnicodeToUTF8(Chart, UTF8Chart) oemuni_to_utf8(Chart, UTF8Chart)
#define UTF8ToChart(Dest, Source) utf8_to_oemcp((UTF8 *) Source, (FD32_LFNPMAX << (1 - WORD_PACK_VALUE)), (INT8S *) Dest, UNI_LENGTH_MAX)

INT16S ChangeUnitab(struct nls_table *st_nls_table);
INT16S char2uni(INT8U **rawstring, INT16U *uni);
INT16S uni2char(INT16U uni, INT8U *out);

/* UTF-16 management functions */
INT16S fd32_utf16to32(const UTF16 *s, UTF32 *Ch);
INT16S fd32_utf32to16(UTF32 Ch, UTF16 *s);

/* UTF-8 management functions */
INT16S fd32_utf8to32(const UTF8 *s, UTF32 *Ch);
INT16S fd32_utf32to8(UTF32 Ch, UTF8 *s);
INT16S utf8_stricmp(const UTF8 *s1, const UTF8 *s2);
INT16S utf8_strupr(UTF8 *Dest, const UTF8 *Source);

/* Conversion functions between UTF-8 and UTF-16 */
INT16S fd32_utf8to16(const UTF8 *Utf8, UTF16 *Utf16);
INT16S fd32_utf16to8(const UTF16 *Utf16, UTF8 *Utf8);
INT16S fd32_utf16to8_limit(const UTF16 *Utf16, UTF8 *Utf8, INT16S space_limit);

/* Case conversion functions */
UTF32 unicode_toupper(UTF32 Ch);

INT16S oemcp_to_utf8 (INT8S *Source, UTF8 *Dest);
INT16S oemuni_to_utf8 (INT8S *Source, UTF8 *Dest);
INT16S utf8_to_oemcp (UTF8 *Source, INT16S SourceSize, INT8S *Dest, INT16S DestSize);
INT16S oemcp_skipchar(INT8S *Dest);

INT16S fs_sfn_open(f_ppos ppos, INT16S open_flag);
INT16S fs_sfn_unlink(f_ppos ppos);
INT16S fs_sfn_stat(INT16S fd, struct sfn_info *sfn_info);
f_ppos fs_getfirstfile(STDiskFindControl *pstDiskFindControl, INT16S dsk, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S speedup,INT32S (*filter)(INT8S *str));
f_ppos fs_getnextfile(STDiskFindControl *pstDiskFindControl, INT16S dsk, INT8S * extname, struct sfn_info* f_info, INT16S attr, INT8S speedup,INT32S (*filter)(INT8S *str));


f_ppos fs_getpapernextfile(STDiskFindControl *pstDiskFindControl, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S speedup, INT32S (*filter)(INT8S *str));
f_ppos fs_getpaperfirstfile(STDiskFindControl *pstDiskFindControl,CHAR *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S speedup, INT32S (*filter)(INT8S *str));

//f_ppos fs_getfirstfile_1(STDiskFindControl *pstDiskFindControl, INT16S dsk, INT8S *extname, struct sfn_info* f_info, INT16S attr);
//f_ppos fs_getnextfile_1(STDiskFindControl *pstDiskFindControl, INT16S dsk, INT8S * extname, struct sfn_info* f_info, INT16S attr);
f_ppos fs_get_first_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir,INT32S (*filter)(INT8S *str));
f_ppos fs_get_next_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir,INT32S (*filter)(INT8S *str));
INT16S SearchFileInFolder(struct STFileNodeInfo *stFNodeInfo,INT32U *nFolderNum, INT32U *nFileNum);

INT16S fs_set_volume(INT8U disk_id, INT8U *p_volume);
INT16S fs_get_volume(INT8U disk_id, STVolume *pstVolume);

int  FS_OS_Init (void);
int  FS_OS_Exit (void);
void FS_OS_LOCK(void);
void FS_OS_UNLOCK(void);

INT32U FS_SeekSpeedup_Malloc(INT32U len);
void FS_SeekSpeedup_Free(INT32U addr);

void FS_OS_GetTime(dostime_t *dt);
void FS_OS_GetDate(dosdate_t *dd);

/********************************************************/
#define DEBUG_INFO	  0

#if DEBUG_INFO 

	 #define dbg_info(arg1) 	 printf(arg1) 	
	 #define dbg_info1(arg1,arg2) 	 printf(arg1,arg2) 	
	 #define dbg_info2(arg1,arg2,arg3) 	 printf(arg1,arg2,arg3) 	
	 #define dbg_info3(arg1,arg2,arg3,arg4) 	 printf(arg1,arg2,arg3,arg4) 	
	 #define dbg_info4(arg1,arg2,arg3,arg4,arg5) 	 printf(arg1,arg2,arg3,arg4,arg5) 	
	 #define dbg_dump(buf, size)    \
	 do { \
	  INT16S dbg_i;   \
	  for (dbg_i=0; dbg_i < size; dbg_i++) 	      \
      {									   \
        printf("%4bx ", buf[dbg_i]); 				  \
          if ((dbg_i % 16) == 15)  printf("\n"); 			 \
      }  									   \
	 }while (0);
#else

	 #define dbg_info(arg1)
	 #define dbg_info1(arg1,arg2) 
	 #define dbg_info2(arg1,arg2,arg3) 
	 #define dbg_info3(arg1,arg2,arg3,arg4) 	
	 #define dbg_info4(arg1,arg2,arg3,arg4,arg5) 	
	 #define dbg_dump(buf, size)   \
	 do { \
	  INT16S dbg_i;   \
	  for (dbg_i=0; dbg_i < size; dbg_i++) 	      \
      {									   \
        printf("%3x ", buf[dbg_i]); 				  \
          if ((dbg_i % 16) == 15)  printf("\n"); 			 \
      }  									   \
	 }while (0); 
#endif

/**************************************************************************/
/* Define the DEBUG symbol in order to activate driver's log output */
#ifdef DEBUG
 #define LOG_PRINTF(s) fd32_log_printf s
#else
 #define LOG_PRINTF(s)
#endif

#endif
