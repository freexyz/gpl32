#ifndef __GPLIB_H__
#define __GPLIB_H__

#include "driver_l2.h"
#include "gplib_cfg.h"


// Component layer functions
extern void gplib_init(INT32U free_memory_start, INT32U free_memory_end);

//Time and data reminder
typedef struct
{
    INT32S tm_sec;  /* 0-59 */
    INT32S tm_min;  /* 0-59 */
    INT32S tm_hour; /* 0-23 */
    INT32S tm_mday; /* 1-31 */
    INT32S tm_mon;  /* 1-12 */
    INT32S tm_year;
    INT32S tm_wday; /* 0-6 Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday */
}TIME_T;

#define ALARM_DAY       0
#define ALARM_ANNUAL    1

extern INT32S ap_alarm_set(void);
extern INT32S calendar_init(void);
extern INT32S cal_time_get(TIME_T  *tm);
extern INT32S cal_time_set(TIME_T tm);
extern INT32S cal_internal_time_set(TIME_T tm);
extern INT32S cal_alarm_set(INT8U alarm_idx, TIME_T at, INT8U rpt_mode, void (*alarm_handler)());
extern INT32S cal_alarm_status_set(INT8U alarm_idx, INT8U status);
extern void  cal_time_zone_set(INT8U t_zone);
extern void cal_factory_date_time_set(TIME_T  *tm);
extern void cal_day_store_get_register(void (*fp_store)(INT32U),INT32U (*fp_get)(void),void (*fp_config_store)(void));

// Memory management
extern void gp_mm_init(INT32U free_memory_start, INT32U free_memory_end);
extern void * gp_malloc_align(INT32U size, INT32U align);		// SDRAM allocation. Align value must be power of 2, eg: 2, 4, 8, 16, 32...
extern void * gp_malloc(INT32U size);							// SDRAM allocation
extern void * gp_iram_malloc_align(INT32U size, INT32U align);	// IRAM allocation. Align value must be power of 2, eg: 2, 4, 8, 16, 32...
extern void * gp_iram_malloc(INT32U size);						// IRAM allocation
extern void gp_free(void *ptr);									// Both SDRAM and IRAM can be freed by this function
extern INT32U mm_free_get(void); /*mm debug mode only*/

// File System
//FS driver
INT32S NAND_Initial(void);
INT32S NAND_Uninitial(void);

#define FAT16_Type	        		0x01
#define FAT32_Type	        		0x02
#define FAT12_Type	        		0x03
#define FORCE_FAT32_Type	        0x12
#define FORCE_FAT16_Type	        0x11

/*-----------------  seek flags  ------------------*/
#define SEEK_SET     	0 		/* offset from beginning of the file*/
#define SEEK_CUR     	1 		/* offset from current location     */
#define SEEK_END     	2 		/* offset from eof  */

/***************** open flags (the 2nd parameter)**********************/
#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_ACCMODE       0x0003

#define O_TRUNC         0x0200 	/*    both */
#define O_CREAT         0x0400
#define O_EXCL		    0x4000	/* not fcntl */

/* File attribute constants for _findfirst() */
#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

/* FAT file system attribute bits                               */
#define D_NORMAL        0       /* normal                       */
#define D_RDONLY        0x01    /* read-only file               */
#define D_HIDDEN        0x02    /* hidden                       */
#define D_SYSTEM        0x04    /* system                       */
#define D_VOLID         0x08    /* volume id                    */
#define D_DIR           0x10    /* subdir                       */
#define D_ARCHIVE       0x20    /* archive bit                  */

#define D_FILE			(0x40)	/* all attribute but D_DIR		*/
#define D_FILE_1		(0x80)	/* contain D_NORMAL,D_RDONLY,D_ARCHIVE */
#define D_ALL (D_FILE | D_RDONLY | D_HIDDEN | D_SYSTEM | D_DIR | D_ARCHIVE)

#define UNI_GBK			0
#define UNI_BIG5		1
#define UNI_SJIS		2

#define UNI_ENGLISH		0x8003
#define UNI_ARABIC		0x8004
#define UNI_UNICODE		0x8100

// file system error code
/* Internal system error returns                                */
#define SUCCESS         0       /* Function was successful      */
#define DE_INVLDFUNC    -1      /* Invalid function number      */
#define DE_FILENOTFND   -2      /* File not found               */
#define DE_PATHNOTFND   -3      /* Path not found               */
#define DE_TOOMANY      -4      /* Too many open files          */
#define DE_ACCESS       -5      /* Access denied                */
#define DE_INVLDHNDL    -6      /* Invalid handle               */
#define DE_MCBDESTRY    -7      /* Memory control blocks shot   */
#define DE_NOMEM        -8      /* Insufficient memory          */
#define DE_INVLDMCB     -9      /* Invalid memory control block */
#define DE_INVLDENV     -10     /* Invalid enviornement         */
#define DE_INVLDFMT     -11     /* Invalid format               */
#define DE_INVLDACC     -12     /* Invalid access               */
#define DE_INVLDDATA    -13     /* Invalid data                 */
#define DE_INVLDDRV     -15     /* Invalid drive                */
#define DE_RMVCUDIR     -16     /* Attempt remove current dir   */
#define DE_DEVICE       -17     /* Not same device              */
#define DE_MAX_FILE_NUM       -18     /* No more files                */
#define DE_WRTPRTCT     -19     /* No more files                */
#define DE_BLKINVLD     -20     /* invalid block                */
#define DE_INVLDBUF     -24     /* invalid buffer size, ext fnc */
#define DE_SEEK         -25     /* error on file seek           */
#define DE_HNDLDSKFULL  -28     /* handle disk full (?)         */
#define DE_INVLDPARM    -87     /* invalid parameter			*/
#define DE_UNITABERR    -88     /* unitab error					*/
#define DE_TOOMANYFILES	-89		/* to many files				*/

#define DE_DEADLOCK	-36
#define DE_LOCK		-39

#define DE_INVLDCDFILE	-48		/* invalid cd file name */
#define DE_NOTEMPTY		-49		/* DIR NOT EMPTY */
#define DE_ISDIR		-50		/* Is a directory name          */
#define DE_FILEEXISTS   -80		/* File exists                  */
#define DE_DEVICEBUSY	-90
#define DE_NAMETOOLONG	-100	/* specified path name too long */
#define DE_FILENAMEINVALID -110	/* Invalid */

struct stat_t
{
	INT16U	st_mode;
	INT32S	st_size;
	INT32U	st_mtime;
};

struct _diskfree_t {
	INT32U total_clusters;
	INT32U avail_clusters;
	INT32U sectors_per_cluster;
	INT32U bytes_per_sector;
};

struct deviceinfo 	 {
	INT8S device_name[16]; 		 // device name
	INT8S device_enable; 			 // device enable status
	INT8S device_typeFAT; 		 // device FAT type
	INT32U device_availspace; 	 // device available space
	INT32U device_capacity; 		 // device capacity
};

// data structure for _setftime()
struct timesbuf 	 {
	INT16U modtime;
	INT16U moddate;
	INT16U accdate;
};

struct f_info {
	INT8U	f_attrib;
	INT16U	f_time;
	INT16U	f_date;
	INT32U	f_size;
	INT16U	entry;
	INT8S	f_name[256];
	INT8S	f_short_name[8 + 3 + 1];
};

typedef struct {
	INT32U  f_entry;
	INT16U  f_offset;
	INT8S   f_dsk;
	INT8S	f_is_root;		// to differentiate the root folder and the first folder in root folder, in disk with FAT16/FAT12 format
} f_pos, *f_ppos;

struct sfn_info {
    INT8U   f_attrib;
	INT16U  f_time;
	INT16U  f_date;
	INT32U  f_size;
    INT8S    f_name[9];
    INT8S    f_extname[4];
    f_pos	f_pos;
};

struct nls_table {
	CHAR			*charset;
	INT16U			Status;
	INT16S			(*init_nls)(void);
	INT16S			(*exit_nls)(void);
	INT16U			(*uni2char)(INT16U uni);
	INT16U			(*char2uni)(INT8U **rawstring);
};

typedef struct
{
	f_pos		cur_pos;
	f_pos		cur_dir_pos;
	INT16U		level;
	INT16S		find_begin_file;
	INT16S		find_begin_dir;
	INT16U		index;

	INT32U		sfn_cluster;
	INT32U		sfn_cluster_offset;

	INT8U		dir_change_flag;
	INT8U		root_dir_flag;			// if the root folder have found the file, this flag is setted
	INT16U		dir_offset[16];
	INT32U		cluster[16];
	INT32U	 	cluster_offset[16];
} STDiskFindControl;

struct STFileNodeInfo
{
	INT8S		disk;					//disk
	INT16U		attr;					//set the attribute to be found
	INT8S		extname[4];				//extension
	INT8S		*pExtName;				//for extend disk find funtion, support find multi extend name
	INT8S		*path;					//the file in the the path to be found
	INT16U		*FileBuffer;			//buffer point for file nodes
	INT16U		FileBufferSize;			//buffer size, every 20 words contain a file node, so must be multiple of 20
	INT16U		*FolderBuffer;			//buffer point for folder nodes
	INT16U		FolderBufferSize;		//buffer size, every 20 words contain a file node, so must be multiple of 20

	// the following parameter user do not care
	INT8S		flag;
	INT8U		root_dir_flag;			// if the root folder have found the file, this flag is setted
	// 08.02.27 add for search more then one kinds extern name of file
	INT16U		MarkDistance;
	INT32S		MaxFileNum;
	INT32S		MaxFolderNum;
	// 08.02.27 add end
	
	INT32S		(*filter)(INT8S *name);
};

typedef struct
{
	INT8U	name[11 + 1];
	INT16U	f_time;
	INT16U	f_date;
} STVolume;

// file system global variable
extern OS_EVENT *gFS_sem;
extern INT16U			gUnicodePage;
extern const struct nls_table	nls_ascii_table;
//extern const struct nls_table	nls_arabic_table;
extern const struct nls_table	nls_cp936_table;
extern const struct nls_table	nls_cp950_table;
extern const struct nls_table	nls_cp932_table;
//extern const struct nls_table	nls_cp1252_table;

/***************************************************************************/
/*        F U N C T I O N    D E C L A R A T I O N S	     			   */
/***************************************************************************/
//========================================================
//Function Name:	fs_get_version
//Syntax:		const char *fs_get_version(void);
//Purpose:		get file system library version
//Note:			the return version string like "GP$xyzz" means ver x.y.zz
//Parameters:   void
//Return:		the library version
//=======================================================
extern const char *fs_get_version(void);

//========================================================
//Function Name:	file_search_start
//Syntax:		INT32S file_search_start(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl)
//Purpose:		search all the files of disk start
//Note:
//Parameters:   stFNodeInfo
//				pstDiskFindControl
//Return:		0 means SUCCESS, -1 means faile
//=======================================================
extern INT32S file_search_start(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl);

//========================================================
//Function Name:	file_search_continue
//Syntax:		INT32S file_search_continue(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl)
//Purpose:		search all the files of disk continue
//Note:
//Parameters:   stFNodeInfo
//				pstDiskFindControl
//Return:		0 means SUCCESS, 1 means search end, -1 means faile
//=======================================================
extern INT32S file_search_continue(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl);

extern INT32S file_search_in_folder_start(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl);
extern INT32S file_search_in_folder_continue(struct STFileNodeInfo *stFNodeInfo, STDiskFindControl *pstDiskFindControl);

//========================================================
//Function Name:	getfirstfile
//Syntax:		f_ppos getfirstfile(INT16S dsk, CHAR *extname, struct sfn_info* f_info, INT16S attr);
//Purpose:		find the first file of the disk(will find into the folder)
//Note:
//Parameters:   dsk, extname, f_info, attr
//Return:		f_ppos
//=======================================================
extern f_ppos getfirstfile(INT16S dsk, CHAR *extname, struct sfn_info* f_info, INT16S attr,INT32S (*filter)(INT8S* str));

//========================================================
//Function Name:	getnextfile
//Syntax:		f_ppos getnextfile(INT16S dsk, CHAR *extname, struct sfn_info* f_info, INT16S attr);
//Purpose:		find the next file of the disk(will find into the folder)
//Note:
//Parameters:   dsk, extname, f_info, attr
//Return:		f_ppos
//=======================================================
extern f_ppos getnextfile(INT16S dsk, CHAR * extname, struct sfn_info* f_info, INT16S attr,INT32S (*filter)(INT8S* str));

//========================================================
//Function Name:	sfn_open
//Syntax:		INT16S sfn_open(f_ppos ppos);
//Purpose:		open the file that getfirstfile/getnextfile find
//Note:
//Parameters:   ppos
//Return:		file handle
//=======================================================
extern INT16S sfn_open(f_ppos ppos);

//========================================================
//Function Name:	sfn_stat
//Syntax:		INT16S sfn_stat(INT16S fd, struct sfn_info *sfn_info);
//Purpose:		get file attribute of an opened file
//Note:
//Parameters:   fd, sfn_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S sfn_stat(INT16S fd, struct sfn_info *sfn_info);

//========================================================
//Function Name:	GetFileInfo
//Syntax:		INT16S FsgetFileInfo(INT16S fd, struct f_info *f_info);
//Purpose:		get long file name infomation through file handle
//Note:
//Parameters:   fd, f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S FsgetFileInfo(INT16S fd, struct f_info *f_info);

//========================================================
//Function Name:	GetFileInfo
//Syntax:		INT16S GetFileInfo(f_ppos ppos, struct f_info *f_info);
//Purpose:		get long file name infomation that getfirstfile/getnextfile find
//Note:
//Parameters:   ppos, f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S GetFileInfo(f_ppos ppos, struct f_info *f_info);

//========================================================
//Function Name:	GetFolderInfo
//Syntax:		INT16S GetFolderInfo(f_ppos ppos, struct f_info *f_info);
//Purpose:		get long folder name infomation that getfirstfile/getnextfile find
//Note:
//Parameters:   ppos, f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S GetFolderInfo(f_ppos ppos, struct f_info *f_info);

//========================================================
//Function Name:	sfn_unlink
//Syntax:		INT16S sfn_unlink(f_ppos ppos);
//Purpose:		delete the file that getfirstfile/getnextfile find
//Note:
//Parameters:   ppos
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S sfn_unlink(f_ppos ppos);

//========================================================
//Function Name:	StatFileNumByExtName
//Syntax:		INT16S StatFileNumByExtName(INT16S dsk, CHAR *extname, INT32U *filenum);
//Purpose:		get the file number of the disk that have the same extend name
//Note:
//Parameters:   dsk, extname, filenum
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S StatFileNumByExtName(INT16S dsk, CHAR *extname, INT32U *filenum);

//========================================================
//Function Name:	GetFileNumEx
//Syntax:		INT16S GetFileNumEx(struct STFileNodeInfo *stFNodeInfo, INT32U *nFolderNum, INT32U *nFileNum);
//Purpose:		get the file number and the folder number of the disk that have the same extend name
//Note:
//Parameters:   stFNodeInfo, nFolderNum, nFileNum
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S GetFileNumEx(struct STFileNodeInfo *stFNodeInfo, INT32U *nFolderNum, INT32U *nFileNum);

//========================================================
//Function Name:	GetFileNodeInfo
//Syntax:		f_ppos GetFileNodeInfo(struct STFileNodeInfo *stFNodeInfo, INT32U nIndex, struct sfn_info* f_info);
//Purpose:		get the file node infomation
//Note:			before run this function, ensure you have execute the function "GetFileNumEx()"
//				0 <= nIndex < nMaxFileNum
//Parameters:   stFNodeInfo, nIndex, f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern f_ppos GetFileNodeInfo(struct STFileNodeInfo *stFNodeInfo, INT32U nIndex, struct sfn_info* f_info);

//========================================================
//Function Name:	GetFolderNodeInfo
//Syntax:		f_ppos GetFolderNodeInfo(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, struct sfn_info* f_info);
//Purpose:		get the folder node infomation
//Note:			before run this function, ensure you have execute the function "GetFileNumEx()"
//				0 <= nIndex < nMaxFileNum
//Parameters:   stFNodeInfo, nFolderIndex, f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern f_ppos GetFolderNodeInfo(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, struct sfn_info* f_info);

//========================================================
//Function Name:	GetFileNumOfFolder
//Syntax:		INT16S GetFileNumOfFolder(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, INT16U *nFile);
//Purpose:		get the file number of a folder
//Note:			before run this function, ensure you have execute the function "GetFileNumEx()"
//Parameters:   stFNodeInfo, nFolderIndex, nFile
//Return:		0, SUCCESS
//				-1, FAILE
//================================== =====================
extern INT16S GetFileNumOfFolder(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, INT16U *nFile);

//========================================================
//Function Name:	FolderIndexToFileIndex
//Syntax:		INT16S FolderIndexToFileIndex(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, INT32U *nFileIndex);
//Purpose:		convert folder id to file id(the index number of first file in this folder)
//Note:			before run this function, ensure you have execute the function "GetFileNumEx()"
//Parameters:   stFNodeInfo, nFolderIndex, nFileIndex
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S FolderIndexToFileIndex(struct STFileNodeInfo *stFNodeInfo, INT32U nFolderIndex, INT32U *nFileIndex);

//========================================================
//Function Name:	FileIndexToFolderIndex
//Syntax:		INT16S FileIndexToFolderIndex(struct STFileNodeInfo *stFNodeInfo, INT32U nFileIndex, INT32U *nFolderIndex);
//Purpose:		convert file id to folder id(find what folder id that the file is in)
//				fileindexõfileڵfolderindex
//Note:			before run this function, ensure you have execute the function "GetFileNumEx()"
//Parameters:   stFNodeInfo, nFolderIndex, nFileIndex
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S FileIndexToFolderIndex(struct STFileNodeInfo *stFNodeInfo, INT32U nFileIndex, INT32U *nFolderIndex);

//========================================================
//Function Name:	get_fnode_pos
//Syntax:		INT16S get_fnode_pos(f_pos *fpos);
//Purpose:		get the file node position after findfirst/findnext, and then you can open this file by sfn_open
//Note:			before run this function, ensure you have execute the function "_findfirst()/_findnext()"
//Parameters:   fpos
//Return:		0, SUCCESS
//=======================================================
extern INT16S get_fnode_pos(f_pos *fpos);

//f_ppos getfirstfileEx(INT8S *path, CHAR *extname, struct sfn_info *f_info, INT16S attr);
//f_ppos getnextfileEx(CHAR * extname, struct sfn_info* f_info, INT16S attr);

//========================================================
//Function Name:	dosdate_decode
//Syntax:		void dosdate_decode(INT16U dos_date, INT16U *pyear, INT8U *pmonth, INT8U *pday);
//Purpose:		convert the dos_data to year, month, day
//Note:
//Parameters:   dos_date, pyear, pmonth, pday
//Return:		void
//=======================================================
extern void dosdate_decode(INT16U dos_date, INT16U *pyear, INT8U *pmonth, INT8U *pday);

//========================================================
//Function Name:	dostime_decode
//Syntax:		void dostime_decode(INT16U dos_time, INT8U *phour, INT8U *pminute, INT8U *psecond);
//Purpose:		convert the dos_time to hour, minute, second
//Note:
//Parameters:   dos_time, phour, pminute, psecond
//Return:		void
//=======================================================
extern void dostime_decode(INT16U dos_time, INT8U *phour, INT8U *pminute, INT8U *psecond);

//========================================================
//Function Name:	time_decode
//Syntax:		INT8S *time_decode(INT16U *tp, CHAR *timec);
//Purpose:		convert *tp to a string like "hh:mm:ss"
//Note:
//Parameters:   tp, timec
//Return:		the point of string
//=======================================================
extern INT8S *time_decode(INT16U *tp, CHAR *timec);

//========================================================
//Function Name:	date_decode
//Syntax:		INT8S *date_decode(INT16U *dp, CHAR *datec);
//Purpose:		convert *dp to a string like "yyyy-mm-dd"
//Note:
//Parameters:   dp, datec
//Return:		the point of string
//=======================================================
extern INT8S *date_decode(INT16U *dp, CHAR *datec);

//========================================================
//Function Name:	fs_safexit
//Syntax:		void fs_safexit(void);
//Purpose:		close all the opened files except the registed file
//Note:
//Parameters:   NO
//Return:		void
//=======================================================
extern void fs_safexit(void);

//========================================================
//Function Name:	fs_registerfd
//Syntax:		void fs_registerfd(INT16S fd);
//Purpose:		regist opened file so when you call function fs_safexit() this file will not close
//Note:
//Parameters:   fd
//Return:		void
//=======================================================
extern void fs_registerfd(INT16S fd);

//========================================================
//Function Name:	disk_safe_exit
//Syntax:		void disk_safe_exit(INT16S dsk);
//Purpose:		close all the opened files of the disk
//Note:
//Parameters:   dsk
//Return:		void
//=======================================================
extern void disk_safe_exit(INT16S dsk);

//========================================================
//Function Name:	open
//Syntax:		INT16S open(CHAR *path, INT16S open_flag);
//Purpose:		open/creat file
//Note:
//Parameters:   path, open_flag
//Return:		file handle
//=======================================================
extern INT16S open(CHAR *path, INT16S open_flag);

//========================================================
//Function Name:	close
//Syntax:		INT16S close(INT16S fd);
//Purpose:		close file
//Note:
//Parameters:   fd
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S close(INT16S fd);

//========================================================
//Function Name:	read
//Syntax:		INT32S read(INT16S fd, INT32U buf, INT32U size);
//Purpose:		read data
//Note:			the buffer is BYTE address, the size is BYTE size
//Parameters:   fd, buf, size
//Return:		really read size
//=======================================================
extern INT32S read(INT16S fd, INT32U buf, INT32U size);

//========================================================
//Function Name:	write
//Syntax:		INT32S write(INT16S fd, INT32U buf, INT32U size);
//Purpose:		write data
//Note:			the buffer is BYTE address, the size is BYTE size
//Parameters:   fd, buf, size
//Return:		really write size
//=======================================================
extern INT32S write(INT16S fd, INT32U buf, INT32U size);

//========================================================
//Function Name:	lseek64
//Syntax:		INT64S lseek64(INT16S handle, INT64S offset0,INT16S fromwhere);
//Purpose:		change data point of file
//Note:			use lseek(fd, 0, SEEK_CUR) can get current offset of file.
//Parameters:   fd, offset, fromwhere
//Return:		data point
//=======================================================
extern INT64S lseek64(INT16S handle, INT64S offset0,INT16S fromwhere);

//========================================================
//Function Name:	lseek
//Syntax:		INT32S lseek(INT16S fd,INT32S offset,INT16S fromwhere);
//Purpose:		change data point of file
//Note:			use lseek(fd, 0, SEEK_CUR) can get current offset of file.
//Parameters:   fd, offset, fromwhere
//Return:		data point
//=======================================================
extern INT32S lseek(INT16S handle, INT32S offset0,INT16S fromwhere);

//========================================================
//Function Name:	unlink
//Syntax:		INT16S unlink(CHAR *filename);
//Purpose:		delete the file
//Note:
//Parameters:   filename
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S unlink(CHAR *filename);

//========================================================
//Function Name:	rename
//Syntax:		INT16S _rename(CHAR *oldname, CHAR *newname);
//Purpose:		change file name
//Note:
//Parameters:   oldname, newname
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _rename(CHAR *oldname, CHAR *newname);

//========================================================
//Function Name:	mkdir
//Syntax:		INT16S mkdir(CHAR *pathname);
//Purpose:		cread a folder
//Note:
//Parameters:   pathname
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S mkdir(CHAR *pathname);

//========================================================
//Function Name:	rmdir
//Syntax:		INT16S rmdir(CHAR *pathname);
//Purpose:		delete a folder
//Note:			the folder must be empty
//Parameters:   pathname
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S rmdir(CHAR *pathname);

//========================================================
//Function Name:	chdir
//Syntax:		INT16S chdir(CHAR *path);
//Purpose:		change current path to new path
//Note:
//Parameters:   path
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S chdir(CHAR *path);

//========================================================
//Function Name:	getcwd
//Syntax:		INT32U getcwd(CHAR *buffer, INT16S maxlen );
//Purpose:		get current path
//Note:
//Parameters:   buffer, maxlen
//Return:		the path name string point
//=======================================================
extern INT32U getcwd(CHAR *buffer, INT16S maxlen );

//========================================================
//Function Name:	fstat
//Syntax:		INT16S fstat(INT16S handle, struct stat_t *statbuf);
//Purpose:		get file infomation
//Note:			the file must be open
//Parameters:   handle, statbuf
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S fstat(INT16S handle, struct stat_t *statbuf);

//========================================================
//Function Name:	stat
//Syntax:		INT16S stat(CHAR *path, struct stat_t *statbuf);
//Purpose:		get file infomation
//Note:
//Parameters:   path, statbuf
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S stat(CHAR *path, struct stat_t *statbuf);

//========================================================
//Function Name:	_findfirst
//Syntax:		INT16S _findfirst(CHAR *name, struct f_info *f_info, INT16U attr);
//Purpose:		find the first file in one folder
//Note:
//Parameters:   name, f_info, attr
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _findfirst(CHAR *name, struct f_info *f_info, INT16U attr);

//========================================================
//Function Name:	_findnext
//Syntax:		INT16S _findnext(struct f_info *f_info);
//Purpose:		find next file in one folder
//Note:
//Parameters:   f_info
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _findnext(struct f_info *f_info);

//========================================================
//Function Name:	_getdiskfree
//Syntax:		INT16S _getdiskfree(INT16S dsk, struct _diskfree_t *st_free);
//Purpose:		get disk total space and free space
//Note:
//Parameters:   dsk, st_free
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _getdiskfree(INT16S dsk, struct _diskfree_t *st_free);

//========================================================
//Function Name:	vfsFreeSpace
//Syntax:		INT32S vfsFreeSpace(INT16S driver);
//Purpose:		get disk free space
//Note:
//Parameters:   dsk, st_free
//Return:		the free space of the disk
//=======================================================
//INT32S vfsFreeSpace(INT16S driver);
extern INT64U vfsFreeSpace(INT16S driver);

//========================================================
//Function Name:	_changedisk
//Syntax:		INT16S _changedisk(INT8U disk);
//Purpose:		change current disk to another disk
//Note:
//Parameters:   disk
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
#define _changedisk     fs_changedisk
extern INT16S _changedisk(INT8U disk);

//========================================================
//Function Name:	_copy
//Syntax:		INT16S _copy(CHAR *path1, CHAR *path2);
//Purpose:		copy file
//Note:
//Parameters:   srcfile, destfile
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
INT16S _copy(CHAR *path1, CHAR *path2);

//========================================================
//Function Name:	fs_init
//Syntax:		void fs_init(void);
//Purpose:		initial all file system global variable
//Note:
//Parameters:   NO
//Return:		void
//=======================================================
extern void fs_init(void);

//========================================================
//Function Name:	fs_uninit
//Syntax:		INT16S fs_uninit(void);
//Purpose:		free file system resource, and unmount all disk
//Note:
//Parameters:   NO
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S fs_uninit(void);

//========================================================
//Function Name:	tellcurrentfiledir
//Syntax:		INT16U tellcurrentfiledir(void);
//Purpose:		get current directory entry point
//Note:
//Parameters:   NO
//Return:		directory entry point
//=======================================================
extern INT16U tellcurrentfiledir(void);

//========================================================
//Function Name:	telldir
//Syntax:		INT16U telldir(void);
//Purpose:		get next directory entry point
//Note:
//Parameters:   NO
//Return:		directory entry point
//=======================================================
extern INT16U telldir(void);

//========================================================
//Function Name:	seekdir
//Syntax:		void seekdir(INT16U pos);
//Purpose:		set directory entry point, and next time, if you call _findnext(),
//				the function will find file from this point
//Note:
//Parameters:   directory entry point
//Return:		NO
//=======================================================
extern void seekdir(INT16U pos);     //the parameter "pos" must be the return value of "telldir"

//========================================================
//Function Name:	rewinddir
//Syntax:		void rewinddir(void);
//Purpose:		reset directory entry point to 0
//Note:
//Parameters:   NO
//Return:		NO
//=======================================================
extern void rewinddir(void);

//========================================================
//Function Name:	_setfattr
//Syntax:		INT16S _setfattr(CHAR *filename, INT16U attr);
//Purpose:		set file attribute
//Note:
//Parameters:   filename, attr
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _setfattr(CHAR *filename, INT16U attr);

//========================================================
//Function Name:	_setdirattr
//Syntax:		INT16S _setdirattr(CHAR *dirname, INT16U attr);
//Purpose:		set dir attribute
//Note:
//Parameters:   filename, attr
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _setdirattr(CHAR *dirname, INT16U attr);

//========================================================
//Function Name:	_getdirattr
//Syntax:		INT16S _getdirattr(CHAR *dirname, INT16U *attr);
//Purpose:		get dir attribute
//Note:
//Parameters:   filename, attr
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _getdirattr(CHAR *dirname, INT16U *attr);

//========================================================
//Function Name:	_devicemount
//Syntax:		INT16S _devicemount(INT16S disked);
//Purpose:		mount disk, then you can use the disk
//Note:
//Parameters:   disk
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _devicemount(INT16S disked);

//========================================================
//Function Name:	_deviceunmount
//Syntax:		INT16S _deviceunmount(INT16S disked);
//Purpose:		unmount disk
//Note:
//Parameters:   disk
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _deviceunmount(INT16S disked);

//========================================================
//Function Name:	_getfserrcode
//Syntax:		INT16S _getfserrcode(void);
//Purpose:		get error code(see error.h)
//Note:
//Parameters:   NO
//Return:		error code
//=======================================================
extern INT16S _getfserrcode(void);

//========================================================
//Function Name:	_clsfserrcode
//Syntax:		void _clsfserrcode(void);
//Purpose:		clear error code to 0
//Note:
//Parameters:   NO
//Return:		void
//=======================================================
extern void _clsfserrcode(void);

//========================================================
//Function Name:	_format
//Syntax:		INT16S _format(INT8U drv, INT8U fstype);
//Purpose:		format disk to FAT32 or FAT16 type
//Note:
//Parameters:   dsk, fstype
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _format(INT8U drv, INT8U fstype);

//========================================================
//Function Name:	_deleteall
//Syntax:		INT16S _deleteall(CHAR *filename);
//Purpose:		delete all file and folder in one folder
//Note:
//Parameters:   path
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S _deleteall(CHAR *filename);

//========================================================
//Function Name:	GetSectorsPerCluster
//Syntax:		UINT16 GetSectorsPerCluster(UINT16 dsk)
//Purpose:		get Sector number per cluster
//Note:
//Parameters:   dsk
//Return:		sector number
//=======================================================
extern INT16U GetSectorsPerCluster(INT16U dsk);

//========================================================
//Function Name:	_GetCluster
//Syntax:		INT32S _GetCluster(INT16S fd);
//Purpose:		get cluster id that data point now locate
//Note:
//Parameters:   fd
//Return:		cluster id
//=======================================================
extern INT32S _GetCluster(INT16S fd);

//========================================================
//Function Name:	Clus2Phy
//Syntax:		INT32S Clus2Phy(INT16U dsk, INT32U cl_no);
//Purpose:		convert cluster id to sector address
//Note:
//Parameters:   dsk, cl_no
//Return:		sector address
//=======================================================
extern INT32S Clus2Phy(INT16U dsk, INT32U cl_no);

//========================================================
//Function Name:	DeletePartFile
//Syntax:		INT16S DeletePartFile(INT16S fd, INT32U offset, INT32U length);
//Purpose:		delete part of file, from "offset", delete "length" byte
//Note:			the file system will convert the "offset" and "length" to cluster size
//Parameters:   fd, offset, length
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S DeletePartFile(INT16S fd, INT32U offset, INT32U length);

//========================================================
//Function Name:	InserPartFile
//Syntax:		INT16S InserPartFile(INT16S tagfd, INT16S srcfd, INT32U tagoff, INT32U srclen);
//Purpose:		insert the src file to tag file
//Note:			the file system will convert the "offset" and "length" to cluster size
//Parameters:   tagfd, srcfd, tagoff, srclen
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S InserPartFile(INT16S tagfd, INT16S srcfd, INT32U tagoff, INT32U srclen);

//========================================================
//Function Name:	InserPartFile
//Syntax:		INT16 InserPartFile(INT16 tagfd, INT16 srcfd, UINT32 tagoff, UINT32 srclen)
//Purpose:		split tag file into two file, one is remain in tag file, one is in src file
//Note:			the file system will convert the "offset" and "length" to cluster size
//Parameters:   tagfd, srcfd, splitpoint
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S SplitFile(INT16S tagfd, INT16S srcfd, INT32U splitpoint);

//========================================================
//Function Name:	ChangeCodePage
//Syntax:		INT16U ChangeCodePage(INT16U wCodePage);
//Purpose:		select unicode page
//Note:			if the code page is not exsit, the file system will default change code page to "ascii"
//Parameters:   wCodePage
//Return:		code page
//=======================================================
extern INT16U ChangeCodePage(INT16U wCodePage);

//========================================================
//Function Name:	GetCodePage
//Syntax:		INT16U GetCodePage(void);
//Purpose:		get unicode page
//Note:
//Parameters:   NO
//Return:		code page
//=======================================================
extern INT16U GetCodePage(void);

//========================================================
//Function Name:	ChangeUnitab
//Syntax:		INT16S ChangeUnitab(struct nls_table *st_nls_table);
//Purpose:		change unicode convert struct
//Note:
//Parameters:   st_nls_table
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S ChangeUnitab(struct nls_table *st_nls_table);

//========================================================
//Function Name:	checkfattype
//Syntax:		INT16S checkfattype(INT8U disk);
//Purpose:		get the fat type of the disk(FAT16 or FAT32)
//Note:
//Parameters:   disk
//Return:		fat type
//=======================================================
extern INT16S checkfattype(INT8U disk);

//========================================================
//Function Name:	UpdataDir
//Syntax:		INT16S UpdataDir(INT16S fd);
//Purpose:		updata dir information but not close the file
//Note:
//Parameters:   fd
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S UpdataDir(INT16S fd);

//========================================================
//Function Name:	FileRepair
//Syntax:		INT16S FileRepair(INT16S fd);
//Purpose:		if the file is destroy for some reason, this function will repair the file
//Note:			it can't deal with some complicated condition
//Parameters:   fd
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S FileRepair(INT16S fd);

//========================================================
//Function Name:	sformat
//Syntax:		INT16S sformat(INT8U drv, INT32U totalsectors, INT32U realsectors);
//Purpose:		format some disk that size is less than 16 MB
//Note:
//Parameters:   drv, totalsectors, realsectors
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S sformat(INT8U drv, INT32U totalsectors, INT32U realsectors);

//========================================================
//Function Name:	GetDiskOfFile
//Syntax:		INT16S GetDiskOfFile(INT16S fd);
//Purpose:		get the disk id of an opened file
//Note:
//Parameters:   fd
//Return:		disk id, 0 is disk "A", 1 is disk "B", and itc...
//=======================================================
extern INT16S GetDiskOfFile(INT16S fd);

//========================================================
//Function Name:	CreatFileBySize
//Syntax:		INT16S CreatFileBySize(CHAR *path, INT32U size);
//Purpose:		creat a file, and allocate "size" byte space
//Note:			size is byte size
//Parameters:   filename, size
//Return:		file handle
//=======================================================
extern INT16S CreatFileBySize(CHAR *path, INT32U size);

//========================================================
//Function Name:	get_first_file_in_folder
//Syntax:		f_ppos get_first_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir);
//Purpose:		find the first file in a folder
//Note:
//Parameters:   pstDiskFindControl:
//				path: the folder to be found
//				extname: all the file have this extend name can be found. if "*", find all file
//				f_info: the file be found
//				find_child_dir: 1 means find the file in child folder, 0 means not find
//Return:		file position
//=======================================================
extern f_ppos get_first_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir, INT32S (*filter)(INT8S *str));

//========================================================
//Function Name:	get_next_file_in_folder
//Syntax:		f_ppos get_next_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir);
//Purpose:		find the next file in a folder
//Note:
//Parameters:   pstDiskFindControl:
//				path: the folder to be found
//				extname: all the file have this extend name can be found. if "*", find all file
//				f_info: the file be found
//				find_child_dir: 1 means find the file in child folder, 0 means not find
//Return:		file position
//=======================================================
extern f_ppos get_next_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir, INT32S (*filter)(INT8S *str));

//========================================================
//Function Name:	flie_cat
//Syntax:		flie_cat(INT16S file1_handle, INT16S file2_handle);
//Purpose:		cat two file to one file, file1_handle.
//Note:
//Parameters:   file1_handle, file2_handle
//Return:		0: success, other fail
//=======================================================
extern INT16S file_cat(INT16S file1_handle, INT16S file2_handle);

//========================================================
//Function Name:	unlink2
//Syntax:		INT16S unlink2(CHAR *filename);
//Purpose:		delete the file
//Note:
//Parameters:   filename
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S unlink2(CHAR *filename);

extern INT16S get_volume(INT8U disk_id, STVolume *pstVolume);
extern INT16S set_volume(INT8U disk_id, INT8U *p_volum);
extern INT16S handle_error_code (INT16S errcode);
extern INT16S gFS_errno;

// background unlink 
extern INT32U FsBgUnlink_get_workmem_size(void);
extern INT32S FsBgUnlink_open(void* pWorkMem, INT32U prio);
extern INT32S FsBgUnlink_file(void* pWorkMem, CHAR *FileName, INT32U bgWorkFlag, INT32U msec);
extern INT32S FsBgUnlink_close(void* pWorkMem);

extern INT32U fs_sd_ms_plug_out_flag_get(void);
extern void fs_sd_ms_plug_out_flag_reset(void);
extern INT8U fs_usbh_plug_out_flag_get(void);
extern void fs_usbh_plug_out_flag_reset(void);
// PPU library relative definitions and APIs

// Text Control Register Constatnt Definitions
#define	B_TFT_LB						24
#define	PPU_TFT_LONG_BURST				(((INT32U) 1)<<B_TFT_LB)
#define	B_YUV_TYPE						20
#define	MASK_YUV_TYPE					(((INT32U) 0x7)<<B_YUV_TYPE)
#define	B_PPU_LB						19
#define	PPU_LONG_BURST					(((INT32U) 1)<<B_PPU_LB)
#define	B_TFT_SIZE						16
#define	MASK_TFT_SIZE					(((INT32U) 0x7)<<B_TFT_SIZE)
#define	B_SAVE_ROM						15
#define	SAVE_ROM_ENABLE					(((INT32U) 1)<<B_SAVE_ROM)
#define	B_FB_SEL						14
#define	FB_SEL0							(((INT32U) 0)<<B_FB_SEL)
#define	FB_SEL1							(((INT32U) 1)<<B_FB_SEL)
#define	B_SPR_WIN						13
#define	SPR_WIN_DISABLE					(((INT32U) 0)<<B_SPR_WIN)
#define	SPR_WIN_ENABLE					(((INT32U) 1)<<B_SPR_WIN)
#define	B_HVCMP_DIS						12
#define	HVCMP_ENABLE					(((INT32U) 0)<<B_HVCMP_DIS)
#define	HVCMP_DISABLE					(((INT32U) 1)<<B_HVCMP_DIS)
#define	B_FB_MONO						10
#define	MASK_FB_MONO					(((INT32U) 0x3)<<B_FB_MONO)
#define	B_SPR25D						9
#define	PPU_SPR25D_DISABLE				(((INT32U) 0)<<B_SPR25D)
#define	PPU_SPR25D_ENABLE				(((INT32U) 1)<<B_SPR25D)
#define	B_FB_FORMAT						8
#define	PPU_RGB565						(((INT32U) 0)<<B_FB_FORMAT)
#define	PPU_RGBG						(((INT32U) 1)<<B_FB_FORMAT)
#define	B_FB_EN							7
#define	PPU_LINE_BASE					(((INT32U) 0)<<B_FB_EN)
#define	PPU_FRAME_BASE					(((INT32U) 1)<<B_FB_EN)
#define	B_VGA_NONINTL					5
#define	PPU_VGA_INTL					(((INT32U) 0)<<B_VGA_NONINTL)
#define	PPU_VGA_NONINTL					(((INT32U) 1)<<B_VGA_NONINTL)
#define	B_VGA_EN						4
#define	PPU_QVGA						(((INT32U) 0)<<B_VGA_EN)
#define	PPU_VGA							(((INT32U) 1)<<B_VGA_EN)
#define	B_TX_BOTUP						3
#define	TX_UP2BOT						(((INT32U) 0)<<B_TX_BOTUP)
#define	TX_BOT2UP						(((INT32U) 1)<<B_TX_BOTUP)
#define	B_TX_DIRECT						2
#define	TX_RELATIVE_ADDRESS				(((INT32U) 0)<<B_TX_DIRECT)
#define	TX_DIRECT_ADDRESS				(((INT32U) 1)<<B_TX_DIRECT)
#define	B_CHAR0_TRANSPARENT				1
#define	CHAR0_TRANSPARENT_ENABLE		(((INT32U) 1)<<B_CHAR0_TRANSPARENT)
#define	B_PPU_EN						0
#define	PPU_DISABLE						(((INT32U) 0)<<B_PPU_EN)
#define	PPU_ENABLE						(((INT32U) 1)<<B_PPU_EN)


#define	B_TXN_PB						22
#define	MASK_TXN_PALETTE_BANK			(((INT32U) 0x3)<<B_TXN_PB)
#define	B_TXN_SPECIAL_EFFECT			20
#define	MASK_TXN_SPECIAL_EFFECT			(((INT32U) 0x3)<<B_TXN_SPECIAL_EFFECT)
#define	B_TXN_WINDOW					17
#define	MASK_TXN_WINDOW					(((INT32U) 0x3)<<B_TXN_WINDOW)
#define	B_TXN_SIZE						14
#define	MASK_TXN_SIZE					(((INT32U) 0x7)<<B_TXN_SIZE)
#define	B_TXN_DEPTH						12
#define	MASK_TXN_DEPTH					(((INT32U) 0x3)<<B_TXN_DEPTH)
#define	B_TXN_PALETTE					8
#define	MASK_TXN_PALETTE				(((INT32U) 0xF)<<B_TXN_PALETTE)
#define	B_TXN_VS						6
#define	MASK_TXN_VS						(((INT32U) 0x3)<<B_TXN_VS)
#define	B_TXN_HS						4
#define	MASK_TXN_HS						(((INT32U) 0x3)<<B_TXN_HS)
#define	B_TXN_FLIP						2
#define	MASK_TXN_FLIP					(((INT32U) 0x3)<<B_TXN_FLIP)
#define	TXN_NO_FLIP						0
#define	TXN_H_FLIP						1
#define	TXN_V_FLIP						2
#define	TXN_HV_FLIP						3
#define	B_TXN_COLOR						0
#define	MASK_TXN_COLOR					(((INT32U) 0x3)<<B_TXN_COLOR)


#define	B_TXN_BLDLVL					10
#define	MASK_TXN_BLDLVL					(((INT32U) 0x3F)<<B_TXN_BLDLVL)
#define	B_TXN_BLDMODE					9
#define	TXN_BLDMODE64_DISABLE			(((INT32U) 0)<<B_TXN_BLDMODE)
#define	TXN_BLDMODE64_ENABLE			(((INT32U) 1)<<B_TXN_BLDMODE)
#define	B_TXN_BLD						8
#define	TXN_BLD_DISABLE					(((INT32U) 0)<<B_TXN_BLD)
#define	TXN_BLD_ENABLE					(((INT32U) 1)<<B_TXN_BLD)
#define	B_TXN_RGBM						7
#define	TXN_RGB15M						(((INT32U) 0x0)<<B_TXN_RGBM)
#define	TXN_RGB15P						(((INT32U) 0x1)<<B_TXN_RGBM)
#define	B_TXN_MODE						5
#define	MASK_TXN_MODE					(((INT32U) 0x3)<<B_TXN_MODE)
#define	B_TXN_MVE						4
#define	TXN_MVE_DISABLE					(((INT32U) 0)<<B_TXN_MVE)
#define	TXN_MVE_ENABLE					(((INT32U) 1)<<B_TXN_MVE)
#define	B_TXN_EN						3
#define	TXN_DISABLE						(((INT32U) 0)<<B_TXN_EN)
#define	TXN_ENABLE						(((INT32U) 1)<<B_TXN_EN)
#define	B_TXN_WALL						2
#define	TXN_WALL_DISABLE				(((INT32U) 0)<<B_TXN_WALL)
#define	TXN_WALL_ENABLE					(((INT32U) 1)<<B_TXN_WALL)
#define	B_TXN_REGM						1
#define	TXN_REGMODE						(((INT32U) 1)<<B_TXN_REGM)
#define	B_TXN_BMP						0
#define	TXN_BMP							(((INT32U) 1)<<B_TXN_BMP)
#define	TXN_CHAR						(((INT32U) 0)<<B_TXN_BMP)


// Sprite Control Register Constant Definitions
#define	B_SP_FRAC_EN					22
#define	SP_FRACTION_COORDINATE_ENABLE	(((INT32U) 1)<<B_SP_FRAC_EN)
#define	B_SP_GPR_EN					    21
#define	SP_GROUP_ENABLE			        (((INT32U) 1)<<B_SP_GPR_EN)
#define	B_SP_LS_EN					    20
#define	SP_LARGE_SIZE_ENABLE			(((INT32U) 1)<<B_SP_LS_EN)
#define	B_SP_INTP_EN					19
#define	SP_INTERPOLATION_ENABLE			(((INT32U) 1)<<B_SP_INTP_EN)
#define	B_SP_FAR_EN						18
#define	SP_FAR_ADDRESS_ENABLE			(((INT32U) 1)<<B_SP_FAR_EN)
#define	B_SP_CDM_EN						17
#define	SP_COLOR_DITHER_ENABLE			(((INT32U) 1)<<B_SP_CDM_EN)
#define	B_SP_EFFECT_EN					16
#define	SP_SPECIAL_EFFECT_ENABLE		(((INT32U) 1)<<B_SP_EFFECT_EN)
#define	B_SP_NUMBER						8
#define	MASK_SP_NUMBER					(((INT32U) 0xFF)<<B_SP_NUMBER)
#define	B_SP_ZOOMEN						7
#define	SP_ZOOM_ENABLE					(((INT32U) 1)<<B_SP_ZOOMEN)
#define	B_SP_ROTEN						6
#define	SP_ROTATE_ENABLE				(((INT32U) 1)<<B_SP_ROTEN)
#define	B_SP_MOSEN						5
#define	SP_MOSAIC_ENABLE				(((INT32U) 1)<<B_SP_MOSEN)
#define	B_SP_DIRECT						4
#define	SP_DIRECT_ADDRESS_MODE			(((INT32U) 1)<<B_SP_DIRECT)
#define	B_SP_BLDMODE					2
#define	SP_BLDMODE64_ENABLE				(((INT32U) 1)<<B_SP_BLDMODE)
#define	B_COORD_SEL						1
#define	SP_COORD1						(((INT32U) 1)<<B_COORD_SEL)
#define	SP_COORD0						(((INT32U) 0)<<B_COORD_SEL)
#define	B_SP_EN							0
#define	SP_ENABLE						(((INT32U) 1)<<B_SP_EN)
#define	SP_DISABLE						(((INT32U) 0)<<B_SP_EN)

// Extended Sprite Control Register Constant Definitions
#define	B_EXSP_FRAC_EN					5
#define	EXSP_FRACTION_COORDINATE_ENABLE	(((INT32U) 1)<<B_EXSP_FRAC_EN)
#define	B_EXSP_GPR_EN					4
#define	EXSP_GROUP_ENABLE			    (((INT32U) 1)<<B_EXSP_GPR_EN)
#define	B_EXSP_LS_EN					3
#define	EXSP_LARGE_SIZE_ENABLE			(((INT32U) 1)<<B_EXSP_LS_EN)
#define	B_EXSP_INTP_EN					2
#define	EXSP_INTERPOLATION_ENABLE	    (((INT32U) 1)<<B_EXSP_INTP_EN)
#define	B_EXSP_CDM_EN					1
#define	EXSP_CDM_ENABLE	                (((INT32U) 1)<<B_EXSP_CDM_EN)

// Sprite RAM Constant Definitions
#define	B_ESPN_INTERPOLATION            11	
#define MASK_ESPN_INTERPOLATION         (0x1 << B_ESPN_INTERPOLATION)
#define	B_ESPN_LG_SIZE                  14	
#define MASK_ESPN_LG_SIZE               (0x1 << B_ESPN_LG_SIZE)
#define	B_ESPN_GROUP_ID                 12	
#define MASK_ESPN_GROUP_ID              (0x3 << B_ESPN_GROUP_ID)

#define	B_SPN_INTERPOLATION             3	
#define MASK_SPN_INTERPOLATION          (0x1 << B_SPN_INTERPOLATION)
#define	B_SPN_LG_SIZE                   2	
#define MASK_SPN_LG_SIZE                (0x1 << B_SPN_LG_SIZE)
#define	B_SPN_GROUP_ID                  0	
#define MASK_SPN_GROUP_ID               (0x3 << B_SPN_GROUP_ID)

#define	B_ESPN_FRAC_1                   8	
#define MASK_ESPN_FRAC_1                (0x3 << B_ESPN_FRAC_1)
#define	B_ESPN_FRAC_2                   10	
#define MASK_ESPN_FRAC_2                (0x3 << B_ESPN_FRAC_2)
#define	B_ESPN_FRAC_3                   12	
#define MASK_ESPN_FRAC_3                (0x3 << B_ESPN_FRAC_3)
#define	B_ESPN_FRAC_4                   14	
#define MASK_ESPN_FRAC_4                (0x3 << B_ESPN_FRAC_4)

#define	B_ESPN_FRAC_X0                  0	
#define MASK_ESPN_FRAC_X0               (0x3 << B_ESPN_FRAC_X0)
#define	B_ESPN_FRAC_Y0                  2	
#define MASK_ESPN_FRAC_Y0               (0x3 << B_ESPN_FRAC_Y0)
#define	B_ESPN_FRAC_X1                  4	
#define MASK_ESPN_FRAC_X1               (0x3 << B_ESPN_FRAC_X1)
#define	B_ESPN_FRAC_Y1                  6	
#define MASK_ESPN_FRAC_Y1               (0x3 << B_ESPN_FRAC_Y1)
#define	B_ESPN_FRAC_X2                  8	
#define MASK_ESPN_FRAC_X2               (0x3 << B_ESPN_FRAC_X2)
#define	B_ESPN_FRAC_Y2                  10	
#define MASK_ESPN_FRAC_Y2               (0x3 << B_ESPN_FRAC_Y2)
#define	B_ESPN_FRAC_X3                  12	
#define MASK_ESPN_FRAC_X3               (0x3 << B_ESPN_FRAC_X3)
#define	B_ESPN_FRAC_Y3                  14	
#define MASK_ESPN_FRAC_Y3               (0x3 << B_ESPN_FRAC_Y3)

// Sprite RAM Constant Definitions
#define	B_SPN_CHARNUM_LO				0
#define	MASK_SPN_CHARNUM_LO				0xFFFF

#define	B_SPN_CDM                       16	
#define MASK_CDM_MODE                   0xFFFF
#define	B_SPN_POSX						0
#define	MASK_SPN_POSX					0x3FF
#define	B_SPN_ROTATE					10
#define	MASK_SPN_ROTATE					(0x3F << 10)
#define	B_SPN_Y1_LO						10
#define	MASK_SPN_Y1_LO					(0x3F << 10)
#define	B_SPN_POSY						0
#define	MASK_SPN_POSY					0x3FF
#define	B_SPN_ZOOM						10
#define	MASK_SPN_ZOOM					(0x3F << 10)
#define	B_SPN_Y2_LO						10
#define	MASK_SPN_Y2_LO					(0x3F << 10)

#define	B_SPN_COLOR						0
#define	MASK_SPN_COLOR					(0x3 << B_SPN_COLOR)
#define	B_SPN_FLIP						2
#define	MASK_SPN_FLIP					(0x3 << B_SPN_FLIP)
#define	B_SPN_HS						4
#define	MASK_SPN_HS						(0x3 << B_SPN_HS)
#define	B_SPN_VS						6
#define	MASK_SPN_VS						(0x3 << B_SPN_VS)
#define	B_SPN_PALETTE					8
#define	MASK_SPN_PALETTE				(0xF << B_SPN_PALETTE)
#define	B_SPN_DEPTH						12
#define	MASK_SPN_DEPTH					(0x3 << B_SPN_DEPTH)
#define	B_SPN_BLD						14
#define	SPN_BLD_ENABLE					(0x1 << B_SPN_BLD)
#define	B_SPN_PB_HIGH					15
#define	MASK_SPN_PB_HIGH				(0x1 << B_SPN_PB_HIGH)

#define	B_SPN_CHARNUM_HI				0
#define	MASK_SPN_CHARNUM_HI				MASK_8_BITS
#define	B_SPN_PB_LOW					7
#define	MASK_SPN_PB_LOW					(0x1 << B_SPN_PB_LOW)
#define	B_SPN_WIN						8
#define	MASK_SPN_WIN					(0x3 << B_SPN_WIN)
#define	B_SPN_BLD_64_LVL				8
#define	MASK_SPN_BLD_64_LVL				(0x3F << B_SPN_BLD_64_LVL)
#define	B_SPN_BLD_16_LVL				10
#define	MASK_SPN_BLD_16_LVL				(0xF << B_SPN_BLD_16_LVL)
#define	B_SPN_MOSAIC					14
#define	MASK_SPN_MOSAIC					(0x3 << B_SPN_MOSAIC)

#define	B_SPN_X1						0
#define	MASK_SPN_X1						MASK_10_BITS
#define	B_SPN_Y3_LO						10
#define	MASK_SPN_Y3_LO					MASK_6_BITS

#define	B_SPN_X2						0
#define	MASK_SPN_X2						MASK_10_BITS
#define	B_SPN_Y1_HI						10
#define	MASK_SPN_Y1_HI					MASK_4_BITS
#define	B_SPN_Y3_HI1					14
#define	MASK_SPN_Y3_HI1					MASK_2_BITS

#define	B_SPN_X3						0
#define	MASK_SPN_X3						MASK_10_BITS
#define	B_SPN_Y2_HI						10
#define	MASK_SPN_Y2_HI					MASK_4_BITS
#define	B_SPN_Y3_HI2					14
#define	MASK_SPN_Y3_HI2					MASK_2_BITS

typedef struct {
	INT16U	nCharNumLo_16;
	INT16S	uPosX_16;
	INT16S	uPosY_16;
	INT16U	attr0;
	INT16U	attr1;
	INT16U	uX1_16;
	INT16U	uX2_16;
	INT16U	uX3_16;
} SpN_RAM, *PSpN_RAM;

typedef struct {
	INT16S	ex_attr0;                           //  0.  Sprite Extend Attribute 0
	INT16U	ex_attr1;                           //  1.  Sprite Extend Attribute 1
} SpN_EX_RAM, *PSpN_EX_RAM;

typedef struct {
	INT16S x0;
	INT16S y0;
	INT16S x1;
	INT16S y1;
	INT16S x2;
	INT16S y2;
	INT16S x3;
	INT16S y3;
} POS_STRUCT, *POS_STRUCT_PTR;

typedef struct {
	FP32 x0;
	FP32 y0;
	FP32 x1;
	FP32 y1;
	FP32 x2;
	FP32 y2;
	FP32 x3;
	FP32 y3;
} POS_STRUCT_GP32XXX, *POS_STRUCT_PTR_GP32XXX;

typedef struct {
	INT32U cdm0;
	INT32U cdm1;
	INT32U cdm2;
	INT32U cdm3;
} CDM_STRUCT, *CDM_STRUCT_PTR;

typedef struct t_sprite_image {
	struct t_sprite_image *next;

	INT16U image_width;
	INT16U image_height;

	INT32U image_data_ptr;
	INT32U attr_ptr;

	POS_STRUCT pos;
	INT8U rotate;
	INT8U zoom;
	INT16U attr0;
	INT16U attr1;
} SPRITE, *PSPRITE;

// PPU global control APIs
extern INT32S gplib_ppu_init(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=disable 1=enable
extern INT32S gplib_ppu_char0_transparent_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_bottom_up_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=from top to bottom 1=from bottom to top
extern INT32S gplib_ppu_vga_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=QVGA mode 1=VGA mode
extern INT32S gplib_ppu_non_interlace_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// vlaue:0=interlace mode 1=non-interlace mode
extern INT32S gplib_ppu_frame_buffer_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U enable, INT32U select);	// enable:0(select:0=TV is line mode and TFT is frame mode) 1=both TV and TFT are frame buffer mode
extern INT32S gplib_ppu_fb_format_set(PPU_REGISTER_SETS *p_register_set, INT32U format, INT32U mono);	// format:0(mono:0=RGB565 1=Mono 2=4-color 3=16-color) 1(mono:0=RGBG 1=YUYV 2=RGBG 3=YUYV)
extern INT32S gplib_ppu_save_rom_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// vlaue:0=disable save rom mode 1=enable save rom mode
extern INT32S gplib_ppu_resolution_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:see the constants defined below
#define	C_TFT_RESOLUTION_320X240		0
#define	C_TFT_RESOLUTION_640X480		1
#define	C_TFT_RESOLUTION_480X234		2
#define	C_TFT_RESOLUTION_480X272		3
#define	C_TFT_RESOLUTION_720X480		4
#define	C_TFT_RESOLUTION_800X480		5
#define	C_TFT_RESOLUTION_800X600		6
#define	C_TFT_RESOLUTION_1024X768		7
#define	C_TV_RESOLUTION_320X240			0
#define	C_TV_RESOLUTION_640X480			0
#define	C_TV_RESOLUTION_720X480			4
extern INT32S gplib_ppu_yuv_type_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value[1:0]:0=BGRG/VYUY 1=GBGR/YVYU 2=RGBG/UYVY 3=GRGB/YUYV, value[2]:0=UV is unsigned(YCbCr) 1=UV is signed(YUV)
extern INT32S gplib_ppu_tft_long_burst_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable TFT long burst 1=enable TFT long burst
extern INT32S gplib_ppu_long_burst_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable PPU long burst 1=enable PPU long burst
extern INT32S gplib_ppu_blend4_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0~3
extern INT32S gplib_ppu_rgb565_transparent_color_set(PPU_REGISTER_SETS *p_register_set, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~0xFFFF
extern INT32S gplib_ppu_fade_effect_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0~255
extern INT32S gplib_ppu_window_set(PPU_REGISTER_SETS *p_register_set, INT32U window_index, INT32U window_x, INT32U window_y);	// windows_index:0(window 1)~3(window 4), window_x:mask + start_x + end_x, window_y:start_y + end_y
extern INT32S gplib_ppu_palette_type_set(PPU_REGISTER_SETS *p_register_set, INT32U p1024, INT32U type);		// p1024:0~1, type:0~3
extern INT32S gplib_ppu_palette_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U bank, INT32U value);	// bank:0(palette0)~3(palette3), value: 32-bit address of palette ram buffer
extern INT32S gplib_ppu_frame_buffer_add(PPU_REGISTER_SETS *p_register_set, INT32U buffer);		// buffer: 32-bit address of frame buffer
extern INT32S gplib_ppu_deflicker_mode_set(INT32U value);						// value:0=disable deflicker function 1=enable deflicker function after PPU is done
extern INT32S gplib_ppu_go_without_wait(PPU_REGISTER_SETS *p_register_set);		// This function returns immediately if frame buffer is not available or PPU is still busy
extern INT32S gplib_ppu_go(PPU_REGISTER_SETS *p_register_set);					// This function returns when PPU registers are updated, it will not wait for PPU frame buffer output to complete
extern INT32S gplib_ppu_go_and_wait_done(PPU_REGISTER_SETS *p_register_set);	// This function returns when PPU registers are updated and operation is done
extern INT32S gplib_ppu_dual_blend_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_deflicker_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_rgba_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_alpha_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_rgba_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_free_mode_size_rigister_set(INT16U INTL,INT16U H_size,INT16U V_size);
extern INT32S gplib_ppu_text_new_specialbmp_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_text_new_compression_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_free_size_set(PPU_REGISTER_SETS *p_register_set, INT16U INTL,INT16U H_size,INT16U V_size);
extern INT32S gplib_ppu_go_hblank_first(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_hblnk_set(INT32U value);
extern INT32S gplib_ppu_hblnk_line_offset_set(INT32U line_offset);
extern INT32S gplib_ppu_hblnk_irq_wait(void);
extern INT32S gplib_ppu_hblnk_go_now(PPU_REGISTER_SETS *p_register_set);

// PPU TEXT-relative APIs: TEXT initiate function
extern INT32S gplib_ppu_text_init(PPU_REGISTER_SETS *p_register_set, INT32U text_index);
// PPU TEXT-relative APIs: TEXT mode selection functions
extern INT32S gplib_ppu_text_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value:0=disable 1=enable
extern INT32S gplib_ppu_text_compress_disable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=allow TEXT1 and TEXT2 to use horizontal/vertical compress 1=only allow 2D and rotate mode for TEXT1 and TEXT2
extern INT32S gplib_ppu_text_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(2D) 1(HCMP/Rotate) 2(VCMP/2.5D) 3(HCMP+VCMP)
extern INT32S gplib_ppu_text_direct_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=relative mode 1=direct address mode
extern INT32S gplib_ppu_text_wallpaper_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_text_attribute_source_select(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value:0=get attribut from TxN_A_PTR 1=get attribut from register
extern INT32S gplib_ppu_text_horizontal_move_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0=disable 1=enable
// PPU TEXT-relative APIs: TEXT input setting functions
extern INT32S gplib_ppu_text_size_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(512x256) 1(512x512) ... 7(4096x4096)
#define	C_TXN_SIZE_512X256				0
#define	C_TXN_SIZE_512X512				1
#define	C_TXN_SIZE_1024X512				2
#define	C_TXN_SIZE_1024X1024			3
#define	C_TXN_SIZE_2048X1024			4
#define	C_TXN_SIZE_2048X2048			5
#define	C_TXN_SIZE_4096X2048			6
#define	C_TXN_SIZE_4096X4096			7
extern INT32S gplib_ppu_text_character_size_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U txn_hs, INT32U txn_vs);	// txn_hs:0~3, txn_vs:0~3
extern INT32S gplib_ppu_text_bitmap_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value:0=character mode 1=bitmap mode
extern INT32S gplib_ppu_text_color_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U rgb_mode, INT32U color);	// rgb_mode:0~1, color=0~3
extern INT32S gplib_ppu_text_palette_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U bank, INT32U palette_idx);	// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_text_segment_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value: 32-bit segment address
extern INT32S gplib_ppu_text_attribute_array_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value: 32-bit pointer to attribute array
extern INT32S gplib_ppu_text_number_array_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);		// value: 32-bit pointer to number array
extern INT8U gplib_ppu_text_number_array_update_flag_get(INT32U text_index);
extern void gplib_ppu_text_number_array_update_flag_clear(void);
extern INT32S gplib_ppu_text_calculate_number_array(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U photo_width, INT32U photo_height, INT32U data_ptr);
// PPU TEXT-relative APIs: TEXT output control functions
extern INT32S gplib_ppu_text_position_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U pos_x, INT32U pos_y);	// pos_x:0~0xFFF, pos_y:0~0xFFF
extern INT32S gplib_ppu_text_offset_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U offset_x, INT32U offset_y);// offset_x:0~0x3FF, offset_y:0~0x3FF
extern INT32S gplib_ppu_text_depth_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0~3
extern INT32S gplib_ppu_text_blend_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U enable, INT32U mode, INT32U value);	// enable:0~1, mode:0(4 level) 1(64 level), value:0~63
extern INT32S gplib_ppu_text_flip_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);				// value:0(no flip) 1(horizontal flip) 2(vertical flip) 3(h+v flip)
extern INT32S gplib_ppu_text_sine_cosine_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT16U r_sine, INT16U r_cosine);	// r_sine:0~0x1FFF, r_cosine:0~0x1FFF
extern INT32S gplib_ppu_text_window_select(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);			// value:0(window1) 1(window2) 2(window3) 3(window4)
extern INT32S gplib_ppu_text_special_effect_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT32U value);	// value:0(no effect) 1(negative color) 2(grayscale) 3(mono color)
extern INT32S gplib_ppu_text_vertical_compress_set(PPU_REGISTER_SETS *p_register_set, INT32U value, INT32U offset, INT32U step);// value:0~0x3FF, offset:0~0x3FF, step:0~0x3FF
extern INT32S gplib_ppu_text_horizontal_move_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);					// value: 32-bit pointer to horizontal move control ram
extern INT32S gplib_ppu_text1_horizontal_compress_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value: 32-bit pointer to TEXT1 horizontal compression control ram
extern INT32S gplib_ppu_text_rotate_zoom_set(PPU_REGISTER_SETS *p_register_set, INT32U text_index, INT16S angle, FP32 factor_k);
extern INT32S gplib_ppu_text3_25d_set(PPU_REGISTER_SETS *p_register_set, INT16S angle, FP32 *factor_ptr);
extern INT32S gplib_ppu_text3_25d_y_compress_set(PPU_REGISTER_SETS *p_register_set, INT32U value);						// value:0~0x3F
extern INT32S Text25D_CosSineBuf_set(PPU_REGISTER_SETS *p_register_set, INT32U CosSineBuf);

// PPU sprite-relative APIs: global control functions
extern INT32S gplib_ppu_sprite_init(PPU_REGISTER_SETS *p_register_set);
extern INT32S gplib_ppu_sprite_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_coordinate_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=center coordinate 1=top-left coordinate
extern INT32S gplib_ppu_sprite_blend_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value:0=4 level blending mode 1=16 or 64 level blending mode
extern INT32S gplib_ppu_sprite_direct_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=relative address mode 1=direct address mode
extern INT32S gplib_ppu_sprite_zoom_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_rotate_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_mosaic_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_number_set(PPU_REGISTER_SETS *p_register_set, INT32U value);				// value:0(1024 sprites) 1(4 sprites) 2(8 sprites) ... 255(1020 sprites)
extern INT32S gplib_ppu_sprite_special_effect_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_color_dither_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=disable 1=enable
extern INT32S gplib_ppu_sprite_25d_mode_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value:0=sprite 2D mode 1= sprite 2.5D mode
extern INT32S gplib_ppu_sprite_window_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);		// value:0=disable sprite window function 1=enable sprite window function
extern INT32S gplib_ppu_sprite_segment_set(PPU_REGISTER_SETS *p_register_set, INT32U value);			// value: 32-bit segment address
extern INT32S gplib_ppu_sprite_attribute_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);	// value: 32-bit pointer to sprite attribute ram
extern INT32S gplib_ppu_sprite_sfr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_large_sprite_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_interpolation_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_group_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_sprite_extend_attribute_ram_ptr_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_interpolation_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_interpolation_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_interpolation_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_large_size_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_large_size_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_large_size_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_group_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_group_attribute_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_group_attribute_set(SpN_EX_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_sprite_fraction_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_fraction_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_fraction_attribute_set(SpN_RAM *sprite_attr, POS_STRUCT_PTR_GP32XXX value);
extern INT32S gplib_ppu_sprite_cdm_attribute_set(SpN_RAM *sprite_attr, INT32U value ,CDM_STRUCT_PTR in);
extern INT32S gplib_ppu_sprite_cdm_attribute_enable_set(SpN_RAM *sprite_attr, INT32U value);
extern INT32S gplib_ppu_exsprite_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_cdm_enable_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_number_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
extern INT32S gplib_ppu_exsprite_start_address_set(PPU_REGISTER_SETS *p_register_set, INT32U value);
// PPU sprite-relative APIs: sprite attribute management functions
extern INT32S gplib_ppu_sprite_attribute_2d_position_set(SpN_RAM *sprite_attr, INT16S x0, INT16S y0);	// x0 and y0 represent sprite top/left or center position. Only 10-bits are valid.
extern INT32S gplib_ppu_sprite_attribute_25d_position_set(SpN_RAM *sprite_attr, POS_STRUCT_PTR position);// position defines four coordinates of the sprite
extern INT32S gplib_ppu_sprite_attribute_rotate_set(SpN_RAM *sprite_attr, INT32U value);				// value:0~63
extern INT32S gplib_ppu_sprite_attribute_zoom_set(SpN_RAM *sprite_attr, INT32U value);					// value:0~63
extern INT32S gplib_ppu_sprite_attribute_color_set(SpN_RAM *sprite_attr, INT32U value);					// value:0(2-bit) 1(4-bit) 2(6-bit) 3(8-bit/5-bit/16-bit/RGBG/YUYV/8+6 blending)
extern INT32S gplib_ppu_sprite_attribute_flip_set(SpN_RAM *sprite_attr, INT32U value);					// value:0(No flip) 1(H-flip) 2(V-flip) 3(H+V-flip)
extern INT32S gplib_ppu_sprite_attribute_character_size_set(SpN_RAM *sprite_attr, INT32U hs, INT32U vs);// hs and vs:0(8) 1(16) 2(32) 3(64)
extern INT32S gplib_ppu_sprite_attribute_palette_set(SpN_RAM *sprite_attr, INT32U bank, INT32U palette_idx);// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_sprite_attribute_depth_set(SpN_RAM *sprite_attr, INT32U value);					// value:0~3
extern INT32S gplib_ppu_sprite_attribute_blend64_set(SpN_RAM *sprite_attr, INT32U enable, INT32U value);// enable:0=disable 1=enable, value:0~63
extern INT32S gplib_ppu_sprite_attribute_blend16_set(SpN_RAM *sprite_attr, INT32U enable, INT32U value);// enable:0=disable 1=enable, value:0~15
extern INT32S gplib_ppu_sprite_attribute_window_set(SpN_RAM *sprite_attr, INT32U value);				// value:0~3
extern INT32S gplib_ppu_sprite_attribute_mosaic_set(SpN_RAM *sprite_attr, INT32U value);				// value:0(no effect) 1(2x2 pixels) 2(4x4 pixels) 3(8x8 pixels)
extern INT32S gplib_ppu_sprite_attribute_charnum_calculate(PPU_REGISTER_SETS *p_register_set, SpN_RAM *sprite_attr, INT32U data_ptr);	// data_ptr points to photo data of the sprite
// PPU sprite-relative APIs: other functions that assist to manage sprite
extern INT32S gplib_ppu_sprite_availible_position_get(PPU_REGISTER_SETS *p_register_set, INT16S *left, INT16S *right, INT16S *top, INT16S *bottom);
extern INT32S gplib_ppu_sprite_visible_position_get(PPU_REGISTER_SETS *p_register_set, INT16S *left, INT16S *right, INT16S *top, INT16S *bottom);
// PPU sprite-relative APIs: sprite image management functions
extern INT32S gplib_ppu_sprite_image_add_to_list(PSPRITE *list, PSPRITE add_start, PSPRITE add_end);
extern INT32S gplib_ppu_sprite_image_remove_from_list(PSPRITE *list, PSPRITE remove_start, PSPRITE remove_end);
extern INT32S gplib_ppu_sprite_image_position_set(PSPRITE sp_img, POS_STRUCT_PTR pos);			// position defines four coordinates of the sprite
extern INT32S gplib_ppu_sprite_image_rotate_set(PSPRITE sp_img, INT32S value);					// value:0~63
extern INT32S gplib_ppu_sprite_image_zoom_set(PSPRITE sp_img, INT32S value);					// value:0~63
extern INT32S gplib_ppu_sprite_image_color_set(PSPRITE sp_img, INT32U value);					// value:0(2-bit) 1(4-bit) 2(6-bit) 3(8-bit/5-bit/16-bit/RGBG/YUYV/8+6 blending)
extern INT32S gplib_ppu_sprite_image_flip_set(PSPRITE sp_img, INT32U value);					// value:0(No flip) 1(H-flip) 2(V-flip) 3(H+V-flip)
extern INT32S gplib_ppu_sprite_image_character_size_set(PSPRITE sp_img, INT32U hs, INT32U vs);	// hs and vs:0(8) 1(16) 2(32) 3(64)
extern INT32S gplib_ppu_sprite_image_palette_set(PSPRITE sp_img, INT32U bank, INT32U palette_idx);	// bank:0~3, palette_idx:0~15
extern INT32S gplib_ppu_sprite_image_depth_set(PSPRITE sp_img, INT32S value);					// value:0~3
extern INT32S gplib_ppu_sprite_image_blend64_set(PSPRITE sp_img, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~63
extern INT32S gplib_ppu_sprite_image_blend16_set(PSPRITE sp_img, INT32U enable, INT32U value);	// enable:0=disable 1=enable, value:0~15
extern INT32S gplib_ppu_sprite_image_window_set(PSPRITE sp_img, INT32U value);					// value:0~3
extern INT32S gplib_ppu_sprite_image_mosaic_set(PSPRITE sp_img, INT32U value);					// value:0(no effect) 1(2x2 pixels) 2(4x4 pixels) 3(8x8 pixels)
// PPU sprite-relative APIs: functions that apply sprite images settings to sprite attribute ram
extern INT32S gplib_ppu_sprite_reserved_num_set(PPU_REGISTER_SETS *p_register_set, INT16U number);	// number:0~1024
extern INT16U gplib_ppu_sprite_reserved_num_get(void);
extern INT32S gplib_ppu_sprite_image_to_attribute(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);
extern INT32S gplib_ppu_sprite_image_charnum_calculate(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);
extern INT32S gplib_ppu_sprite_image_position_calculate(PPU_REGISTER_SETS *p_register_set, PSPRITE sp_img);


// JPEG general API
extern INT32U gplib_jpeg_version_get(void);
extern void gplib_jpeg_default_quantization_table_load(INT32U quality);
extern void gplib_jpeg_default_huffman_table_load(void);

// JPEG encoder definitions
#define C_JPEG_FORMAT_YUV_SEPARATE		0x0
#define C_JPEG_FORMAT_YUYV				0x1

// JPEG encoder initiate API
extern void jpeg_encode_init(void);

// JPEG encoder input buffer relative APIs
extern INT32S jpeg_encode_input_size_set(INT16U width, INT16U height);			// Width and height of the image that will be compressed
extern INT32S jpeg_encode_input_format_set(INT32U format);						// format: C_JPEG_FORMAT_YUV_SEPARATE or C_JPEG_FORMAT_YUYV

// JPEG encoder output buffer relative APIs
extern INT32S jpeg_encode_yuv_sampling_mode_set(INT32U encode_mode);			// encode_mode: C_JPG_CTRL_YUV422 or C_JPG_CTRL_YUV420(only valid for C_JPEG_FORMAT_YUV_SEPARATE format)
extern INT32S jpeg_encode_output_addr_set(INT32U addr);							// The address that VLC(variable length coded) data will be output

// JPEG encoder start, restart and stop APIs
extern INT32S jpeg_encode_once_start(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// If input format is YUV separate, both y_addr, u_addr and v_addr must be 8-byte alignment. If input format is YUYV, only y_addr(16-byte alignment) is used.
extern INT32S jpeg_encode_on_the_fly_start(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U len);	// If input format is YUV separate, both y_addr, u_addr and v_addr must be 8-byte alignment. If input format is YUYV, only y_addr(16-byte alignment) is used.
extern void jpeg_encode_stop(void);

// JPEG encoder status query API
extern INT32S jpeg_encode_status_query(INT32U wait);
extern INT32U jpeg_encode_vlc_cnt_get(void);

// JPEG decoder definitions
#define JPEG_PARSE_OK				0
#define JPEG_PARSE_NOT_DONE			1
#define JPEG_PARSE_DONE				2
#define JPEG_PARSE_THUMBNAIL_DONE	3

#define JPEG_PARSE_FAIL				-1
#define JPEG_SET_HW_ERROR			-2
#define JPEG_SIZE_ERROR				-3
#define JPEG_REENTRY_ERROR			-4

// JPEG decoder initiate API
extern void jpeg_decode_init(void);

// JPEG decoder header parser APIs
extern INT32S jpeg_decode_parse_header(INT8U *buf_start, INT32U len);	// buf_start points to buffer address of JPEG file
extern INT16U jpeg_decode_image_width_get(void);					// Get image width after header parsing finish
extern INT16U jpeg_decode_image_height_get(void);					// Get image height after header parsing finish
extern INT16U jpeg_decode_image_extended_width_get(void);			// Get image extended width(according to YUV mode)
extern INT16U jpeg_decode_image_extended_height_get(void);			// Get image extended height(according to YUV mode)
extern INT16U jpeg_decode_image_yuv_mode_get(void);					// Get image YUV mode after header parsing finish
extern INT8U jpeg_decode_image_progressive_mode_get(void);			// Get image progressive mode (0=baseline, 1=progressive)
extern INT8U jpeg_decode_image_orientation_get(void);				// Get image orientation(1~8)
extern INT32S jpeg_decode_date_time_get(INT8U *target);
extern INT8U jpeg_decode_thumbnail_exist_get(void);					// Get information about thumbnail image(0=No thumbnail image, 1=Thumbnail image exists)
extern INT16U jpeg_decode_thumbnail_width_get(void);
extern INT16U jpeg_decode_thumbnail_height_get(void);
extern INT16U jpeg_decode_thumbnail_extended_width_get(void);
extern INT16U jpeg_decode_thumbnail_extended_height_get(void);
extern INT16U jpeg_decode_thumbnail_yuv_mode_get(void);
extern INT8U * jpeg_decode_image_vlc_addr_get(void);				// Get start address of entropy encoded data after header parsing finish

// JPEG decoder input relative APIs
extern INT32S jpeg_decode_vlc_maximum_length_set(INT32U len);		// Specify the maximum length that JPEG will read to decode image. Using this API is optional. It is used to prevent JPEG engine from hangging when bad JPEG file is read. The value of length should be >= real VLC length(eg: using file size as its value).

// JPEG decoder output relative APIs
extern INT32S jpeg_decode_output_set(INT32U y_addr, INT32U u_addr, INT32U v_addr, INT32U line);	// Addresses must be 8-byte alignment, line:C_JPG_FIFO_DISABLE/C_JPG_FIFO_ENABLE/C_JPG_FIFO_16LINE/C_JPG_FIFO_32LINE/C_JPG_FIFO_64LINE/C_JPG_FIFO_128LINE/C_JPG_FIFO_256LINE
extern void jpeg_decode_thumbnail_image_enable(void);				// Decode thumbnail image of file instead of original image
extern void jpeg_decode_thumbnail_image_disable(void);				// Decode original image of file
extern INT32S jpeg_decode_clipping_range_set(INT16U start_x, INT16U start_y, INT16U width, INT16U height);	// x, y, width, height must be at lease 8-byte alignment
extern INT32S jpeg_decode_clipping_mode_enable(void);
extern INT32S jpeg_decode_clipping_mode_disable(void);
extern INT32S jpeg_decode_level2_scaledown_enable(void);			// When scale-down level 2 mode is enabled, output size will be 1/2 in width and height
extern INT32S jpeg_decode_level2_scaledown_disable(void);
extern INT32S jpeg_decode_level8_scaledown_enable(void);			// When scale-down level 8 mode is enabled, output size will be 1/8 in width and height
extern INT32S jpeg_decode_level8_scaledown_disable(void);

// JPEG decoder start, restart and stop APIs
#define jpeg_decode_single_vlc_set	jpeg_decode_once_start
#define jpeg_decode_vlc_on_the_fly	jpeg_decode_on_the_fly_start
extern INT32S jpeg_decode_once_start(INT8U *buf, INT32U len);		// When the complete VLC data is available, using this API to decode JPEG image once.
extern INT32S jpeg_decode_on_the_fly_start(INT8U *buf, INT32U len);	// When only partial of the VLC data is available, using this API to decode it. buf must be 16-byte alignment except the first one
extern INT32S jpeg_decode_output_restart(void);						// Restart decompression when output FIFO buffer is full. This function should be called after Scaler has finished handling the previous FIFO buffer.
extern void jpeg_decode_stop(void);

// JPEG decoder status query API
extern INT32S jpeg_decode_status_query(INT32U wait);				// If wait is 0, this function returns immediately. Return value:C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_OUTPUT_FULL/C_JPG_STATUS_DECODE_DONE/C_JPG_STATUS_ENCODE_DONE/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR

// Progressive JPEG decoder APIs
#define JPEG_SUSPENDED								0x80000001	// Suspended due to lack of input data

#define JPEG_BS_NOT_ENOUGH							0	// Data in the bitstream buffer are not enough
#define JPEG_REACHED_SOS							1	// Reached start of new scan
#define JPEG_REACHED_EOI							2	// Reached end of image
#define JPEG_ROW_COMPLETED							3	// Completed one iMCU row
#define JPEG_SCAN_COMPLETED							4	// Completed last iMCU row of a scan
#define JPEG_FILE_END								5	// File Read End
#define JPEG_MCU_COMPLETED							6
#define JPEG_PASS_AC_REFINE							7	// To pass ac refine data segment
#define JPEG_HEADER_OK								8	// Found valid image datastream

#define C_PROGRESSIVE_JPEG_WORKING_MEMORY_SIZE    	6240
#define C_PROGRESSIVE_JPEG_READ_FILE_BUFFER_SIZE	4096

void progressive_jpeg_dec_init(INT8U *working_mem, INT8U *bs_buf);
INT32S progressive_jpeg_parsing(INT8U *working_mem, INT32U wi);				// Header parsing
void progressive_jpeg_reset(INT8U *working_mem);							// Make output row index be zero
INT32S progressive_jpeg_hf_dec(INT8U *working_mem, INT32U wi);				// Huffman decoder
INT32U progressive_jpeg_dec_get_ri(INT8U *working_mem);						// Get read index
void progressive_jpeg_set_file_end(INT8U *working_mem);						// Tell library that reading file is ending
const INT8U *progressive_jpeg_get_version(void);							// Get library version
INT32U progressive_jpeg_get_width(INT8U *working_mem);						// Get image width
INT32U progressive_jpeg_get_height(INT8U *working_mem);						// Get image height
INT32U progressive_jpeg_get_component(INT8U *working_mem);					// Get the numbers of component
INT32U progressive_jpeg_get_hv(INT8U *working_mem, INT32S component_idx);	// Get the horizontal & vertical sampling in the component_idx of this component
INT32U progressive_jpeg_get_max_h(INT8U *working_mem);						// Get the max horizontal sampling in this image
INT32U progressive_jpeg_get_max_v(INT8U *working_mem);						// Get the max vertical sampling in this image
INT32U progressive_jpeg_get_MCUsize(INT8U *working_mem, INT32S component_idx);			// Get the MCU size in the component_idx of this component
INT32U progressive_jpeg_get_width_in_blocks(INT8U *working_mem, INT32S component_idx);	// Get the numbers of block in horizontal
INT32U progressive_jpeg_get_height_in_blocks(INT8U *working_mem, INT32S component_idx);	// Get the numbers of block in vertical
INT32U progressive_jpeg_get_coefmemsize(INT8U *working_mem);							// Get the size of dct coefficients needed
INT32U progressive_jpeg_get_blocksperrow(INT8U *working_mem, INT32S component_idx);		// Get the blocks of one row in the component_idx of this component
void progressive_jpeg_set_coefmem(INT8U *working_mem, INT8U *coef_mem);					// Set the dct coefficients memory
INT32U progressive_jpeg_get_comp_in_scan(INT8U *working_mem);							// Get the numbers of component in this scan
INT32U progressive_jpeg_get_comp_idx(INT8U *working_mem, INT32S id);					// Get the index of component
INT16U * progressive_jpeg_get_quant_tbl(INT8U *working_mem, INT32S component_idx);		// Get quantization table pointer

// Motion-JPEG decoder init API
extern void mjpeg_decode_init(void);

// Motion-JPEG decoder APIs for setting output parameters
extern INT32S mjpeg_decode_output_format_set(INT32U format);
extern INT32S mjpeg_decode_output_addr_set(INT32U y_addr, INT32U u_addr, INT32U v_addr);	// Must be 4-byte alignment

// Motion-JPEG decoder APIs for start, restart and stop function
#define mjpeg_decode_single_vlc_set	mjpeg_decode_once_start
#define mjpeg_decode_vlc_on_the_fly	mjpeg_decode_on_the_fly_start
extern INT32S mjpeg_decode_once_start(INT8U *buf, INT32U len);			// The complete VLC data is available, decode JPEG image once.
extern INT32S mjpeg_decode_on_the_fly_start(INT8U *buf, INT32U len);	// Partial of the VLC data is available. buf must be 16-byte alignment except the first call to this API
extern void mjpeg_decode_stop(void);

// Motion-JPEG decoder API for status query
extern INT32S mjpeg_decode_status_query(INT32U wait);					// If wait is 0, this function returns immediately. Return value: C_JPG_STATUS_SCALER_DONE/C_JPG_STATUS_INPUT_EMPTY/C_JPG_STATUS_STOP/C_JPG_STATUS_TIMEOUT/C_JPG_STATUS_INIT_ERR


// GIF decode
#define C_GIF_DEC_WORKING_MEMORY_SIZE  	68000
#define C_GIF_DEC_FS_BUFFER_SIZE 		2048

#define C_GIF_STATUS_OK					0x00000000
#define C_GIF_STATUS_LINE_DONE			0x00000001
#define C_GIF_STATUS_FRAME_HEADER		0x80000000
#define C_GIF_STATUS_END_OF_FRAME		0x80000001
#define C_GIF_STATUS_END_OF_IMAGE		0x80000002
#define C_GIF_STATUS_NOT_DONE			0x80000080
#define C_GIF_STATUS_IMAGE_TOO_LARGE	0x80000082
#define C_GIF_STATUS_FILE_HAS_NO_FRAME	0x80000083
#define C_GIF_STATUS_ALLOC_SRAM			0x80000084
#define C_GIF_STATUS_NOT_A_GIF			0x80000085

extern INT32S gif_dec_init(CHAR* p_workmem, INT8U *fs_buf);
extern INT32S gif_dec_get_ri(CHAR *p_workmem);
extern INT32S gif_dec_parsing_file_header(CHAR *p_workmem, INT32S wi, INT32S flag);
extern INT32S gif_dec_parsing_frame_header(CHAR *p_workmem, INT32S wi);
extern INT32S gif_dec_get_still_or_motion(CHAR *p_workmem);
extern INT32S gif_dec_run(CHAR *p_workmem, INT16S *p_output, INT32S wi);
extern INT32S gif_dec_get_screen_width(CHAR *p_workmem);
extern INT32S gif_dec_get_screen_height(CHAR *p_workmem);
extern INT32S gif_dec_get_img_width(CHAR *p_workmem);
extern INT32S gif_dec_get_img_height(CHAR *p_workmem);
extern INT32S gif_dec_get_X_offset(CHAR *p_workmem);
extern INT32S gif_dec_get_Y_offset(CHAR *p_workmem);
extern INT32S gif_dec_get_next_ypos(CHAR *p_workmem);
extern INT32S gif_dec_get_cur_ypos(CHAR *p_workmem);
extern void	gif_dec_set_AtFileEnd(CHAR *p_workmem);			//set the flag that arriving at file end.return value is zero for not arriving at file end,no-zero for arriving at file end.
extern INT32S gif_dec_set_decoder_bmpbuffer(CHAR *p_workmem, INT16S *bmpbuffer);
extern INT8U* gif_dec_get_fill_rgb(CHAR *p_workmem);
extern INT32U gif_dec_get_delayTime(CHAR *p_workmem);
extern INT32S gif_dec_get_interlace(CHAR *p_workmem);
extern INT32S gif_dec_get_imageNum(CHAR *p_workmem);


#define BMP_PARSE_OK					0
#define BMP_PARSE_NOT_DONE				1
#define BMP_PARSE_FAIL					-1

#define C_BMP_STATUS_INPUT_EMPTY		0x00000001
#define C_BMP_STATUS_OUTPUT_FULL		0x00000002
#define C_BMP_STATUS_DECODE_DONE		0x00000004
#define C_BMP_STATUS_STOP				0x00000008
#define C_BMP_STATUS_INIT_ERR			0x00000010

extern void bmp_decode_init(void);
extern INT32S bmp_decode_parse_header(INT8U *buf_start, INT32U len);
extern INT16U bmp_decode_image_width_get(void);
extern INT16U bmp_decode_image_height_get(void);
extern INT8U bmp_decode_line_direction_get(void);				// 0=Bottom to top, 1=Top to bottom
extern INT8U * bmp_decode_data_addr_get(void);
extern INT32S bmp_decode_output_set(INT32U *output_addr, INT32U fifo_line_num);
extern INT32S bmp_decode_on_the_fly_start(INT8U *input_addr, INT32U len);
extern INT32S bmp_decode_output_restart(void);
extern void bmp_decode_stop(void);
extern INT32S bmp_decode_status_query(void);

// GPZP
extern INT32S gpzp_decode(INT8U * input_buffer ,INT8U  * output_buffer);


// Photo post-processing
#define AUTO_CONTRAST_MEMORY_SIZE	1032
#define AUTO_LEVEL_MEMORY_SIZE		3080

extern INT32S ContrastInit(INT8U *pworkmem, INT32U width, INT32U height);
extern INT32S LevelInit(INT8U *pworkmem, INT32U width, INT32U height);
extern INT32S AutoContrast(INT8U *pdata, INT8U *pworkmem);
extern INT32S AutoLevel(INT8U *pdata, INT8U *pworkmem);


// Print and get string from/to UART
extern void print_string(CHAR *fmt, ...);
extern void get_string(CHAR *s);


/* MP3 */
#define MP3_DEC_FRAMESIZE					1152	// ???
#define MP3_DEC_BITSTREAM_BUFFER_SIZE   	4096    // size in bytes
#if GPLIB_MP3_HW_EN == 1
	#define MP3_DEC_MEMORY_SIZE 			14020	//(20632-8)	// 18456
#else
	#define MP3_DEC_MEMORY_SIZE 			13500
	#define MP3_DECODE_RAM     				7176
#endif
/* MP3 error code */
#define MP3_DEC_ERR_NONE			0x00000000	/* no error */

#define MP3_DEC_ERR_BUFLEN	   	   	0x80000001	/* input buffer too small (or EOF) */
#define MP3_DEC_ERR_BUFPTR	   	   	0x80000002	/* invalid (null) buffer pointer */

#define MP3_DEC_ERR_NOMEM	   	   	0x80000031	/* not enough memory */

#define MP3_DEC_ERR_LOSTSYNC	   	0x80000101	/* lost synchronization */
#define MP3_DEC_ERR_BADLAYER	   	0x80000102	/* reserved header layer value */
#define MP3_DEC_ERR_BADBITRATE	   	0x80000103	/* forbidden bitrate value */
#define MP3_DEC_ERR_BADSAMPLERATE  	0x80000104	/* reserved sample frequency value */
#define MP3_DEC_ERR_BADEMPHASIS	   	0x80000105	/* reserved emphasis value */
#define MP3_DEC_ERR_BADMPEGID		0x80000106	//for error mpegid add by zgq on 20080508

#define MP3_DEC_ERR_BADCRC	   	   	0x80000201	/* CRC check failed */
#define MP3_DEC_ERR_BADBITALLOC	   	0x80000211	/* forbidden bit allocation value */
#define MP3_DEC_ERR_BADSCALEFACTOR  0x80000221	/* bad scalefactor index */
#define MP3_DEC_ERR_BADMODE         0x80000222	/* bad bitrate/mode combination */
#define MP3_DEC_ERR_BADFRAMELEN	    0x80000231	/* bad frame length */
#define MP3_DEC_ERR_BADBIGVALUES    0x80000232	/* bad big_values count */
#define MP3_DEC_ERR_BADBLOCKTYPE    0x80000233	/* reserved block_type */
#define MP3_DEC_ERR_BADSCFSI	    0x80000234	/* bad scalefactor selection info */
#define MP3_DEC_ERR_BADDATAPTR	    0x80000235	/* bad main_data_begin pointer */
#define MP3_DEC_ERR_BADPART3LEN	    0x80000236	/* bad audio data length */
#define MP3_DEC_ERR_BADHUFFTABLE    0x80000237	/* bad Huffman table select */
#define MP3_DEC_ERR_BADHUFFDATA	    0x80000238	/* Huffman data overrun */
#define MP3_DEC_ERR_BADSTEREO	    0x80000239	/* incompatible block_type for JS */

#if GPLIB_MP3_HW_EN == 1
// MP3 Decoder Version
extern const char * mp3_dec_get_version(void);
// MP3 Decoder Initial
extern int mp3_dec_init(void *p_workmem, void *p_bsbuf);
// MP3 set ring size
extern int mp3_dec_set_ring_size(void *p_workmem, int size);
// MP3 header parsing
extern int mp3_dec_parsing(void *p_workmem, int wi);
// MP3 Decoder
extern int mp3_dec_run(void *p_workmem, short *p_pcmbuf, int wi, int granule);
// Get Read Index
extern int mp3_dec_get_ri(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: read index of bitstream ring buffer

extern void mp3_dec_set_ri(char *mp3dec_workmem, int ri);

// Get mpeg id
extern const char *mp3_dec_get_mpegid(void *p_workmem);
// return value: MP3 Decoder Working memory size
extern int mp3_dec_get_mem_block_size (void);
// return error number.
extern int mp3_dec_get_errno(void *p_workmem);
// return layer.
extern int mp3_dec_get_layer(void *p_workmem);
// return channel.
extern int mp3_dec_get_channel(void *p_workmem);
// return bitrate in kbps.
extern int mp3_dec_get_bitrate(void *p_workmem);
// return sampling rate in Hz.
extern int mp3_dec_get_samplerate(void *p_workmem);

extern int mp3_dec_end(void *p_workmem,int wi);

// set EQ table
extern void mp3_dec_set_eq_table(void *mp3dec_workmem, short *eqtable);

// set EQ band out
extern void mp3_dec_set_band_addr(void *mp3dec_workmem, short *band);

#else
// get library version
extern const unsigned char * mp3_dec_get_version(void);
// MP3 Decoder Initial
extern int mp3_dec_init(char *p_workmem, unsigned char *p_bsbuf, char *ram);
// MP3 header parsing
extern int mp3_dec_parsing(char *p_workmem, unsigned int wi);
// MP3 Decoder
extern int mp3_dec_run(char *p_workmem, short *p_pcmbuf, unsigned int wi);
// Get Read Index
extern int mp3_dec_get_ri(char *p_workmem);
// Set Read Index
extern void mp3_dec_set_ri(char *mp3dec_workmem, int ri);
// Set bitstream buffer size
extern void mp3_dec_set_bs_buf_size(char *mp3dec_workmem, int bs_buf_size);

// Get mpeg id
extern const char *mp3_dec_get_mpegid(char *p_workmem);
int mp3_dec_get_mem_block_size (void);
// return value: MP3 Decoder Working memory size
// return error number.
extern int mp3_dec_get_errno(char *p_workmem);
// return layer.
extern int mp3_dec_get_layer(char *p_workmem);
// return channel.
extern int mp3_dec_get_channel(char *p_workmem);
// return bitrate in kbps.
extern int mp3_dec_get_bitrate(char *p_workmem);
// return sampling rate in Hz.
extern int mp3_dec_get_samplerate(char *p_workmem);
// set EQ
extern void mp3_dec_set_EQ(unsigned char *mp3dec_workmem, unsigned short *EQ_table);
// set volume gain, Q.14 (1 = 0x4000)
extern void mp3_dec_set_volume(char *mp3dec_workmem, unsigned short vol);
#endif

//---------------------------------------------------------------------------
#ifndef __MP3ENC_H__
#define __MP3ENC_H__

#define  MP3_ENC_WORKMEM_SIZE  31704
// error code
#define MP3ENC_ERR_INVALID_CHANNEL		0x80000001
#define MP3ENC_ERR_INVALID_SAMPLERATE	0x80000002
#define MP3ENC_ERR_INVALID_BITRATE		0x80000003

extern int mp3enc_init(
	void *pWorkMem,
	int nChannels,
	int nSamplesPreSec,
	int nKBitsPreSec,
	int Copyright,
	char *Ring,
	int RingBufSize,
	int RingWI);

extern int mp3enc_encframe(void *pWorkMem, const short *PCM);
extern int mp3enc_end(void *pWorkMem);
const char *mp3enc_GetErrString(int ErrCode);
const char *mp3enc_GetVersion(void);
int mp3enc_GetWorkMemSize(void);

#endif	// __MP3ENC_H__

//---------------------------------------------------------------------------
/* WMA */
#define WMA_DEC_FRAMESIZE        2048
#define WMA_DEC_MEMORY_SIZE      8192*2
#define WMA_DEC_BITSTREAM_BUFFER_SIZE 16384

extern int			wma_dec_run(char *p_workmem, short *p_pcmbuf, int wi);	//decode function
extern int  		wma_dec_init(char* wmadec_workmem, unsigned char* bs_buf, char *wmahw_mem, int bs_len);//initialize
extern int			wma_dec_parsing(char *wmadec_workmem,int wi);			//parsing header of the wma file
extern int			wma_dec_get_ri(char *wmadec_workmem);					//get read index of the ringbuf
extern int			wma_dec_get_errno(char *wmadec_workmem);				//get error information
extern const unsigned char * wma_dec_get_version(void);						//get version of the project
extern int			wma_dec_get_mem_block_size(void);						//get memory block size
extern int			wma_dec_get_samplerate(char *wmadec_workmem);			//get samplerate of the wma file
extern int			wma_dec_get_channel(char *wmadec_workmem);				//get channel of the wma file
extern int			wma_dec_get_bitspersample(char *wmadec_workmem);		//get the bits per sample
extern int			wma_dec_get_bitrate(char *wmadec_workmem);				//get the bitrate of the wma file
extern void			wma_dec_set_EQ(char *wmadec_workmem, unsigned short *p_EQ_table);//set EQ parameters of the wma file
extern int			wma_dec_get_playtime(char *wmadec_workmem);				//get the play time of the wma file in ms
extern void			wma_dec_reset_offset(char *wmadec_workmem);				//reset offset value which is to jump from ringbuffer.
extern void			wma_dec_set_ri(char *wmadec_workmem, int ri);			//set ri pointer of ringbuffer.
extern int			wma_dec_get_offset(char *wmadec_workmem);				//get offset value of ringbuffer.
extern void			wma_dec_set_volume(char *wmadec_workmem, unsigned short volume);//set EQ parameters of the wma file

// SUCCESS codes
#define WMA_OK						0x00000000
#define WMA_S_NEWPACKET				0x00000001
#define WMA_S_NO_MORE_FRAME			0x00000002
#define WMA_S_NO_MORE_SRCDATA		0x00000003
#define WMA_S_LOSTPACKET			0x00000004
#define WMA_S_CORRUPTDATA			0x00000005

// ERROR codes
#define WMA_E_FAIL					0x80004005
#define WMA_E_INVALIDARG			0x80070057
#define WMA_E_NOTSUPPORTED			0x80040000
#define WMA_E_NOTSUPPORTED_DRM		0x80040001
#define WMA_E_NOTSUPPORTED_CODEC    0x80040008
#define WMA_E_NOTSUPPORTED_VERSION	0x80040009
#define WMA_E_BROKEN_FRAME			0x80040002
#define WMA_E_DATANOTENOUGH			0x80040003//add by zgq
#define WMA_E_ONHOLD				0x80040004
#define WMA_E_NO_MORE_SRCDATA		0x80040005
#define WMA_E_WRONGSTATE			0x8004000A

#define WMA_E_BAD_PACKET_HEADER		0x80000011
#define WMA_E_NO_MORE_FRAMES		0x80000012
#define WMA_E_BAD_DRM_TYPE			0x80000013
#define WMA_E_INTERNAL				0x80000014
//#define WMA_E_NOMOREDATA_THISTIME	0x80000015
#define WMA_E_INVALIDHEADER			0x80000017
#define WMA_E_BUFFERTOOSMALL		0x80000018
#define WMA_E_WRONGVALUE			0x80000019


/* WAVE */
#define WAVE_OUT_INTERLEAVED

#define WAV_DEC_FRAMESIZE				2048
#define WAV_DEC_BITSTREAM_BUFFER_SIZE   4096
#define WAV_DEC_MEMORY_SIZE				100

//=======wave format tag====================
#define	WAVE_FORMAT_PCM				(0x0001)
#define	WAVE_FORMAT_ADPCM			(0x0002)
#define	WAVE_FORMAT_ALAW			(0x0006)
#define	WAVE_FORMAT_MULAW			(0x0007)
#define WAVE_FORMAT_IMA_ADPCM		(0x0011)

#define WAVE_FORMAT_A1800			(0x1000)
#define WAVE_FORMAT_MP3				(0x2000)

//========wave decoder error NO.====================
#define WAV_DEC_NO_ERROR    			0
#define WAV_DEC_INBUFFER_NOT_ENOUGH		0x80000001
#define WAV_DEC_RIFF_HEADER_NOT_FOUND	0x80000002
#define WAV_DEC_HEADER_NOT_FOUND		0x80000003
#define WAV_DEC_CHUNK_ALIGN_ERROR		0x80000004
#define WAV_DEC_DATA_CHUNK_NOT_FOUND	0x80000005
#define WAV_DEC_FMT_CHUNK_TOO_SHORT		0x80000006
#define WAV_DEC_CHANNEL_ERROR			0x80000007
#define WAV_DEC_BIT_DEPTH_ERROR			0x80000008
#define WAV_DEC_CHUNK_EXTEND_ERROR		0x80000009
#define WAV_DEC_MSADPCM_COEF_ERROR		0x8000000A
#define WAV_DEC_UNKNOWN_FORMAT			0x8000000B

int wav_dec_init(void *p_workmem, const unsigned char *p_bsbuf);
// p_bsbuf:      pointer to bit-stream buffer
// p_workmem:    pointer to working memory
// return value: 0 success

int wav_dec_parsing(void *p_workmem, int wi);//, WAVEFORMATEX *p_wave_format
// p_workmem:    pointer to working memory
// wi:           write index of bitstream ring buffer
// return value: 0 success
int wav_dec_set_param(void *p_workmem, const unsigned char *p_WaveFormatEx);
//this function is only used for AVI

void wav_dec_set_ring_buf_size(void *p_workmem, int size);

int wav_dec_run(void *p_workmem, short *p_pcmbuf, int wi);
// p_ pcmbuf:    pointer to PCM buffer
// p_workmem:    pointer to working memory
// wi:           write index of bitstream ring buffer
// return value:
//	positive : samples of PCM
//  zero:      not enough bitstream data
//  negtive:   error

int wav_dec_get_ri(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: read index of bitstream ring buffer

int wav_dec_get_mem_block_size(void);
// return value: Wave Decoder Working memory size

int wav_dec_get_wFormatTag(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: wave format tag

int wav_dec_get_nChannels(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: number of channels

int wav_dec_get_SampleRate(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: sample rate

int wav_dec_get_nAvgBytesPerSec(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: average bytes per second

int wav_dec_get_nBlockAlign(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: block align

int wav_dec_get_wBitsPerSample(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: bits per sample
const INT8U * wav_dvr_get_version(void);

//---------------------------------------------------------------------------
/* WAVE Encode*/
#ifndef __WAV_ENC_H__
#define __WAV_ENC_H__

//=======Constant Definition================
#define WAV_ENC_MEMORY_BLOCK_SIZE		40
#define DVRWAVE_FRAME_SIZE				512

//========wave decoder error NO.==================
#define WAV_ENC_NO_ERROR    			0
#define WAV_ENC_UNKNOWN_FORMAT			0x80000100
#define WAV_ENC_CHANNEL_ERROR			0x80000101

int wav_enc_init(void *p_workmem);

int wav_enc_run(void *p_workmem,short *p_pcmbuf,unsigned char *p_bsbuf);

int wav_enc_get_header(void *p_workmem, void *p_header,int length, int numsamples);

int wav_enc_get_mem_block_size(void);

int wav_enc_get_SamplePerFrame(void *p_workmem);

int wav_enc_get_BytePerPackage(void *p_workmem);

int wav_enc_get_HeaderLength(void *p_workmem);

int wav_enc_Set_Parameter(void *p_workmem, int Channel,int SampleRate,int FormatTag);

#endif //__WAV_ENC_H__

//---------------------------------------------------------------------------
//A1800 decode
#ifndef __A1800DEC_H__
#define __A1800DEC_H__

#define A1800DEC_MEMORY_BLOCK_SIZE		5824//1676

#define A18_DEC_FRAMESIZE        320
#define A18_DEC_BITSTREAM_BUFFER_SIZE 4096

#define A18_OK						0x00000001
#define A18_E_NO_MORE_SRCDATA		0x80040005

extern int  A18_dec_SetRingBufferSize(void *obj, int size);
extern int  a1800dec_run(void *obj, int write_index, short * pcm_out);
extern int  a1800dec_init(void *obj, const unsigned char* bs_buf);
extern int  a1800dec_parsing(void *obj, int write_index);
extern int  a1800dec_read_index(void *obj);
extern int  a1800dec_GetMemoryBlockSize(void);
extern int  a1800dec_errno(void *obj);

extern const char* A18_dec_get_version(void);
extern int  A18_dec_get_bitrate(void *obj);
extern int  A18_dec_get_samplerate(void *obj);
extern int	A18_dec_get_channel(void *obj);
extern int	A18_dec_get_bitspersample(void *obj);
#endif //__A1800DEC_H__

//---------------------------------------------------------------------------
/* a1800 encode */
#ifndef __A1800ENC_H__
#define __A1800ENC_H__

#define	A18_ENC_FRAMESIZE			320		// input pcm size per frame
#define	A18_ENC_MEMORY_SIZE			5784	//

#define A18_ENC_NO_ERROR			0
#define A18_E_MODE_ERR				0x80000004

extern int A18_enc_run(unsigned char *p_workmem, const short *p_pcmbuf, unsigned char *p_bsbuf);
extern int A18_enc_init(unsigned char *p_workmem);
extern int A18_enc_get_BitRate(unsigned char *p_workmem);
extern int A18_enc_get_PackageSize(unsigned char *p_workmem);
extern int A18_enc_get_errno(char *A18enc_workmem);
extern const char* A18_enc_get_version(void);
extern int A18_enc_get_mem_block_size(void);
extern void A18_enc_set_BitRate(unsigned char *p_workmem, int BitRate);
#endif //!__A1800ENC_H__

//---------------------------------------------------------------------------
//A1600
#ifndef __A16_DEC_H__
#define __A16_DEC_H__

#define A16_DEC_FRAMESIZE        128
#define A16_DEC_MEMORY_SIZE      164
#define A16_DEC_BITSTREAM_BUFFER_SIZE 1024

#define     A16_IS_NOT_AT_FILE_END			0		//not arrive at the file end.must be no-zero.
#define     A16_IS_AT_FILE_END				1		//arrive at the file end.must be zero.

#define A16_OK						0
#define A16_E_NO_MORE_SRCDATA			0x80000000
#define A16_E_READ_IN_BUFFER			0x80000001
#define A16_CODE_FILE_FORMAT_ERR		0x80000002
#define A16_E_FILE_END				0x80000003
#define A16_E_MODE_ERR				0x80000004

extern int			A16_dec_run(char *p_workmem, short *p_pcmbuf, int wi);
extern int			A16_dec_init(char* A16dec_workmem, unsigned char* bs_buf);
extern int			A16_dec_parsing(char *A16dec_workmem,int wi);
extern int			A16_dec_get_ri(char *A16dec_workmem);
extern int			A16_dec_get_errno(char *A16dec_workmem);
extern const char * A16_dec_get_version(void);
extern int			A16_dec_get_mem_block_size(void);
extern int			A16_dec_get_bitrate(char *A16dec_workmem);
extern int			A16_dec_get_samplerate(char *A16dec_workmem);
extern int			A16_dec_get_channel(char *A16dec_workmem);
extern int			A16_dec_get_bitspersample(char *A16dec_workmem);
extern void			A16_dec_set_AtFileEnd(char *A16dec_workmem);
extern int			A16_dec_get_FileLen(char *A16dec_workmem);

#endif //!__A16_DEC_H__


//-----------------------------------------------------------------------added by Bruce, 2008/09/26
//A6400
#ifndef __a6400_dec_h__
#define __a6400_dec_h__

/////////////////////////////////////////////////////////////////////////////
//		Constant Definition
/////////////////////////////////////////////////////////////////////////////
#define A6400_DEC_FRAMESIZE					1152	// ???
#define A6400_DEC_BITSTREAM_BUFFER_SIZE   	4096    // size in bytes
#if GPLIB_MP3_HW_EN == 1
	#define A6400_DEC_MEMORY_SIZE 			14016	//(20632-8)	// 18456
#else
	#define A6400_DEC_MEMORY_SIZE 			13512
	#define A6400_DECODE_RAM     			7176
#endif

/////////////////////////////////////////////////////////////////////////////
//		Error Code
/////////////////////////////////////////////////////////////////////////////
#define A6400_DEC_ERR_NONE				0x00000000	/* no error */

#define A6400_DEC_ERR_BUFLEN	   	   	0x80000001	/* input buffer too small (or EOF) */
#define A6400_DEC_ERR_BUFPTR	   	   	0x80000002	/* invalid (null) buffer pointer */

#define A6400_DEC_ERR_NOMEM	   	   		0x80000031	/* not enough memory */

#define A6400_DEC_ERR_LOSTSYNC	   		0x80000101	/* lost synchronization */
#define A6400_DEC_ERR_BADLAYER	   		0x80000102	/* reserved header layer value */
#define A6400_DEC_ERR_BADBITRATE	   	0x80000103	/* forbidden bitrate value */
#define A6400_DEC_ERR_BADSAMPLERATE  	0x80000104	/* reserved sample frequency value */
#define A6400_DEC_ERR_BADEMPHASIS	   	0x80000105	/* reserved emphasis value */
#define A6400_DEC_ERR_BADMPEGID			0x80000106	//for error mpegid add by zgq on 20080508

#define A6400_DEC_ERR_BADCRC	   	   	0x80000201	/* CRC check failed */
#define A6400_DEC_ERR_BADBITALLOC	   	0x80000211	/* forbidden bit allocation value */
#define A6400_DEC_ERR_BADSCALEFACTOR  	0x80000221	/* bad scalefactor index */
#define A6400_DEC_ERR_BADMODE         	0x80000222	/* bad bitrate/mode combination */
#define A6400_DEC_ERR_BADFRAMELEN	    0x80000231	/* bad frame length */
#define A6400_DEC_ERR_BADBIGVALUES    	0x80000232	/* bad big_values count */
#define A6400_DEC_ERR_BADBLOCKTYPE    	0x80000233	/* reserved block_type */
#define A6400_DEC_ERR_BADSCFSI	    	0x80000234	/* bad scalefactor selection info */
#define A6400_DEC_ERR_BADDATAPTR	    0x80000235	/* bad main_data_begin pointer */
#define A6400_DEC_ERR_BADPART3LEN	    0x80000236	/* bad audio data length */
#define A6400_DEC_ERR_BADHUFFTABLE    	0x80000237	/* bad Huffman table select */
#define A6400_DEC_ERR_BADHUFFDATA	    0x80000238	/* Huffman data overrun */
#define A6400_DEC_ERR_BADSTEREO	    	0x80000239	/* incompatible block_type for JS */

/////////////////////////////////////////////////////////////////////////////
//		Function Definition
/////////////////////////////////////////////////////////////////////////////

#if GPLIB_MP3_HW_EN == 1
// A6400 Decoder Version
extern const char * a6400_dec_get_version(void);

// A6400 Decoder Initial
extern int a6400_dec_init(void *p_workmem, char *p_bsbuf);

// A6400 header parsing
extern int a6400_dec_parsing(void *p_workmem, int wi);

// A6400 Decoder
extern int a6400_dec_run(void *p_workmem, short *p_pcmbuf, int wi, int granule);

// Get Read Index
extern int a6400_dec_get_ri(void *p_workmem);
// p_workmem:    pointer to working memory
// return value: read index of bitstream ring buffer

// Get mpeg id
//extern const char *A6400_dec_get_mpegid(void *p_workmem);

extern int a6400_dec_get_mem_block_size (void);
// return value: A6400 Decoder Working memory size

// return error number.
extern int a6400_dec_get_errno(void *p_workmem);

// return layer.
//extern int a6400_dec_get_layer(void *p_workmem);

// return channel.
extern int a6400_dec_get_channel(void *p_workmem);

// return bitrate in kbps.
extern int a6400_dec_get_bitrate(void *p_workmem);

// return sampling rate in Hz.
extern int a6400_dec_get_samplerate(void *p_workmem);

extern int a6400_dec_end(void *p_workmem,int wi);

#else
// A6400 Decoder Initial
int a6400_dec_init(char *p_workmem, unsigned char *p_bsbuf, char *ram);


// A6400 header parsing
int a6400_dec_parsing(char *p_workmem, unsigned int wi);

// A6400 Decoder
int a6400_dec_run(char *p_workmem, short *p_pcmbuf, unsigned int wi);


// Get Read Index
int a6400_dec_get_ri(char *p_workmem);
// p_workmem:    pointer to working memory
// return value: read index of bitstream ring buffer

// Set Read Index
void a6400_dec_set_ri(char *a6400dec_workmem, int ri);

// Set bitstream buffer size
void a6400_dec_set_bs_buf_size(char *a6400dec_workmem, int bs_buf_size);

// Get mpeg id
const char *a6400_dec_get_mpegid(char *p_workmem);

int a6400_dec_get_mem_block_size (void);
// return value: A6400 Decoder Working memory size

// return error number.
int a6400_dec_get_errno(char *p_workmem);

// return layer.
int a6400_dec_get_layer(char *p_workmem);

// return channel.
int a6400_dec_get_channel(char *p_workmem);

// return bitrate in kbps.
int a6400_dec_get_bitrate(char *p_workmem);

// return sampling rate in Hz.
int a6400_dec_get_samplerate(char *p_workmem);

// get library version
const unsigned char * a6400_dec_get_version(void);

// set EQ
void a6400_dec_set_EQ(unsigned char *a6400dec_workmem, unsigned short *EQ_table);


// set volume gain, Q.14 (1 = 0x4000)
extern void a6400_dec_set_volume(char *a6400dec_workmem, unsigned short vol);
#endif

#endif  // __a6400_dec_h__

//-----------------------------------------------------------------------added by Bruce, 2008/09/26
//S880
#ifndef __S880_DEC_H__
#define __S880_DEC_H__

#define L_FRAME16k   320                   /* Frame size at 16kHz                        */

#define S880_DEC_FRAMESIZE        L_FRAME16k
//#define S880_DEC_MEMORY_SIZE      2072
//#define S880_DEC_MEMORY_SIZE      (2072+1296)		//wenli, 2008.9.12
#define S880_DEC_MEMORY_SIZE      (2072+1296+128+184)		//wenli, 2008.9.18

#define S880_DEC_BITSTREAM_BUFFER_SIZE 1024

#define     S880_IS_NOT_AT_FILE_END			0		//not arrive at the file end.must be no-zero.
#define     S880_IS_AT_FILE_END				1		//arrive at the file end.must be zero.

#define S880_OK							0
#define S880_E_NO_MORE_SRCDATA			0x80000000
#define S880_E_READ_IN_BUFFER			0x80000001
#define S880_CODE_FILE_FORMAT_ERR		0x80000002
#define S880_E_FILE_END					0x80000003
#define S880_ERR_INVALID_MODE 			0x80000004

extern int			S880_dec_run(char *p_workmem, short *p_pcmbuf, int wi);
extern int			S880_dec_init(char* S880dec_workmem, unsigned char* bs_buf);
extern int			S880_dec_parsing(char *S880dec_workmem,int wi);
extern int			S880_dec_get_ri(char *S880dec_workmem);
extern int			S880_dec_get_errno(char *S880dec_workmem);
extern const char * S880_dec_get_version(void);
extern int			S880_dec_get_mem_block_size(void);
extern int			S880_dec_get_bitrate(char *S880dec_workmem);
extern int			S880_dec_get_samplerate(char *S880dec_workmem);
extern int			S880_dec_get_channel(char *S880dec_workmem);
extern int			S880_dec_get_bitspersample(char *S880dec_workmem);
extern void			S880_dec_set_AtFileEnd(char *S880dec_workmem);
extern int			S880_dec_get_FileLen(char *S880dec_workmem);

#endif //!__S880_DEC_H__
//==========================================================================
#ifndef __NEAACDEC_H__
#define __NEAACDEC_H__


/* A decode call can eat up to FAAD_MIN_STREAMSIZE bytes per decoded channel,
   so at least so much bytes per channel should be available in this stream */
#define FAAD_MIN_STREAMSIZE	 768 /* 6144 bits/channel */
#define AAC_DEC_FRAMESIZE	1024
#define AAC_DEC_BITSTREAM_BUFFER_SIZE 4096

//#define SUPPORT_5_1_CHANNELS

#ifdef SUPPORT_5_1_CHANNELS
#define MAX_CHANNELS		 6
#else
#define MAX_CHANNELS		 2
#endif


#ifdef SUPPORT_5_1_CHANNELS
#define	AAC_DEC_MEMORY_BLOCK_SIZE	20816//24956
#else
#define	AAC_DEC_MEMORY_BLOCK_SIZE	12672//16700
#endif



// ***************************************************************************
#define AAC_OK														0x0


#define UNSUPPORTED_FILE_FORMAT_MP4									0x80000001
#define NOT_MONO_OR_STEREO											0x80000002

#define NOT_LC_OBJECT_TYPE											0x80000011
#define UNABLE_TO_FIND_ADTS_SYNCWORD								0x80000012

#define GAIN_CONTROL_NOT_YET_IMPLEMENTED                            0x80000021
#define PULSE_CODING_NOT_ALLOWED_IN_SHORT_BLOCKS                    0x80000022
#define SCALEFACTOR_OUT_OF_RANGE                                    0x80000023
#define CHANNEL_COUPLING_NOT_YET_IMPLEMENTED                        0x80000024
#define ERROR_DECODING_HUFFMAN_CODEWORD					            0x80000025
#define NON_EXISTENT_HUFFMAN_CODEBOOK_NUMBER_FOUND                  0x80000026
#define INVALID_NUMBER_OF_CHANNELS                                  0x80000027
#define MAXIMUM_NUMBER_OF_BITSTREAM_ELEMENTS_EXCEEDED               0x80000028
#define INPUT_DATA_BUFFER_TOO_SMALL                                 0x80000029
#define ARRAY_INDEX_OUT_OF_RANGE                                    0x8000002A
#define MAXIMUM_NUMBER_OF_SCALEFACTOR_BANDS_EXCEEDED                0x8000002B
#define QUANTISED_VALUE_OUT_OF_RANGE								0x8000002C
#define UNEXPECTED_CHANNEL_CONFIGURATION_CHANGE                     0x8000002D
#define ERROR_IN_PROGRAM_CONFIG_ELEMENT                             0x8000002E
#define PCE_SHALL_BE_THE_FIRST_ELEMENT_IN_A_FRAME                   0x8000002F
#define BITSTREAM_VALUE_NOT_ALLOWED_BY_SPECIFICATION                0x80000030
// ***************************************************************************
/* object types for AAC */
#define MAIN       1
#define LC         2
#define SSR        3
#define LTP        4
#define HE_AAC     5
#define ER_LC     17
#define ER_LTP    19
#define LD        23
#define DRM_ER_LC 27 /* special object type for DRM */
// ***************************************************************************



int aac_dec_init(void *pWorkMem, int downMatrix, const unsigned char *bs_buf);

int aac_dec_parsing(void *pWorkMem, int wi);

/*static const uint32_t sample_rates[] =
{
	96000, 88200, 64000, 48000, 44100, 32000,
	24000, 22050, 16000, 12000, 11025,  8000
};*/

int aac_dec_set_param(void *pWorkMem,
					  unsigned char objectTypeIndex,			// object type (only support LC profile)
					  unsigned char samplingFrequencyIndex,		// sample rate index (refer to sample_rates[])
					  unsigned char channelsConfiguration);		// channels

int aac_dec_run(void *pWorkMem, int wi, short *pcm_out);

unsigned long aac_dec_get_samplerate(void *pWorkMem);
unsigned char aac_dec_get_channel(void *pWorkMem);
int	aac_dec_get_bitspersample(void *pWorkMem);

#ifdef SUPPORT_5_1_CHANNELS
int aac_dec_if_lfe_channel_exists(void *pWorkMem);
#endif

int aac_dec_get_mem_block_size(void);

int aac_dec_get_read_index(void *pWorkMem);

int aac_dec_get_errno(void *pWorkMem);
const char *aac_dec_get_version(void);
int aac_dec_SetRingBufferSize(void *pWorkMem, int size);
void aac_dec_set_ri(void *pWorkMem, int ri_addr);

#endif

//==========================================================================
#ifndef __MP3ENC_H__
#define __MP3ENC_H__

#define	MP3_ENC_WORKMEM_SIZE  	31704

// layer3.c //
extern int mp3enc_init(
	void *pWorkMem,
	int nChannels,
	int nSamplesPreSec,
	int nKBitsPreSec,
	int Copyright,
	char *Ring,
	int RingBufSize,
	int RingWI);

extern int mp3enc_encframe(void *pWorkMem, const short *PCM);
extern int mp3enc_end(void *pWorkMem);

#define MP3ENC_ERR_INVALID_CHANNEL		0x80000001
#define MP3ENC_ERR_INVALID_SAMPLERATE	0x80000002
#define MP3ENC_ERR_INVALID_BITRATE		0x80000003

const char *mp3enc_GetErrString(int ErrCode);
const char *mp3enc_GetVersion(void);
int mp3enc_GetWorkMemSize(void);

#endif	// __MP3ENC_H__
//==========================================================================
#ifndef __CONSTANT_PITCH_H__
#define __CONSTANT_PITCH_H__

// Initial Link and Unlink functions
int ConstantPitch_Link(
	void *pWorkMem,
	void *pFrontEndWorkMem,
	int (*GetOutput)(void *, short *, int),
	int fs,
	int ch,
	int OptionFlag);
	
void ConstantPitch_Unlink(void *pWorkMem);

// version
const char *ConstantPitch_GetVersion(void);

// functions that used for memory allocated by ConstantPitch
void *ConstantPitch_Create(int InFrameSize, int fs, int ch, int none);
void ConstantPitch_Del(void *pWorkMem);

// set functions
extern int ConstantPitch_SetParam(void *pWorkMem, int pitch_idx, int none);

// get output function
int ConstantPitch_GetOutput(void *pWorkMem, short *DstData, int Len);
//int Robot_GetOutput(void *pWorkMem, short *DstData, int Len);

void ConstantPitch_Del(void *pWorkMem);

// get functions
int ConstantPitch_GetSampleRate(void *pWorkMem);
int ConstantPitch_GetChannel(void *pWorkMem);

#endif // __CONSTANT_PITCH_H__
//==========================================================================
#ifndef __ECHO_H__
#define __ECHO_H__

// version
const char *Echo_GetVersion(void);

// functions that used for memory allocated by Echo
void *Echo_Create(int InFrameSize, int fs, int ch, int OptFlag);
void Echo_Del(void *pWorkMem);

// functions that used for memory allocated by User
int Echo_GetMemBlockSize(int InFrameSize, int fs, int ch, int OptFlag);
void Echo_Initial(void *pWorkMem, int InFrameSize, int fs, int ch, int OptFlag);

// Initial Link and Unlink functions
int Echo_Link(
	void *pWorkMem,
	void *pFrontEndWorkMem,
	int (*GetOutput)(void *, short *, int),
	int fs,
	int ch,
	int OptionFlag);

// opt:
#define OPTFLAG_LEFT_CH_ONLY	1
// This flag only take effect if ch == 2
// If this flag is set, Echo module will convert 2-ch inuput to 1-ch(Left only) output
	
void Echo_Unlink(void *pWorkMem);

// set functions
extern int Echo_SetParam(void *pWorkMem, int delay_len, int weight_idx);

// get output function
int Echo_GetOutput(void *pWorkMem, short *DstData, int Len);

// get functions
int Echo_GetSampleRate(void *pWorkMem);
int Echo_GetChannel(void *pWorkMem);

#endif // __ECHO_H__
//==========================================================================
#ifndef __VOICE_CHANGER_H__
#define __VOICE_CHANGER_H__

// version
const char *VoiceChanger_GetVersion(void);

// functions that used for memory allocated by VoiceChanger
void *VoiceChanger_Create(int InFrameSize, int fs, int ch, int OptFlag);

void VoiceChanger_Del(void *pWorkMem);

// functions that used for memory allocated by User
int VoiceChanger_GetMemBlockSize(int InFrameSize, int fs, int ch, int OptFlag);
void VoiceChanger_Initial(void *pWorkMem, int InFrameSize, int fs, int ch, int OptFlag);

// Initial Link and Unlink functions
void VoiceChanger_Link(
	void *pWorkMem,
	void *pFrontEndWorkMem,
	int (*GetOutput)(void *, short *, int),
	int fs,
	int ch,
	int OptionFlag);

// opt:
#define OPTFLAG_LEFT_CH_ONLY	1
// This flag only take effect if ch == 2
// If this flag is set, VoiceChanger module will convert 2-ch inuput to 1-ch(Left only) output
	
void VoiceChanger_Unlink(void *pWorkMem);

// set functions
extern void VoiceChanger_SetParam(void *pWorkMem, int speed, int pitch);
// speed : can be 0~24. Default 12, means using original speed as output.
// This setting represents the following times of original speed:
//	0.5x,	0.5x,	0.562x,	0.625x,	0.625x,	0.687x,	0.687x,	0.75x,	0.812x,	0.812x,	0.875x,	0.938x,	1.0x,
//	1.063x,	1.125x,	1.188x,	1.25x,	1.313x,	1.375x,	1.5x,	1.563x,	1.688x,	1.813x,	1.877x,	2.0x,
//
// pitch : can be 0~24. Default 12, means using original pitch as output.
// This setting represents the following times of original pitch:
//	0.5x,	0.5x,	0.562x,	0.625x,	0.625x,	0.687x,	0.687x,	0.75x,	0.812x,	0.812x,	0.875x,	0.938x,	1.0x,
//	1.063x,	1.125x,	1.188x,	1.25x,	1.313x,	1.375x,	1.5x,	1.563x,	1.688x,	1.813x,	1.877x,	2.0x,
//

// get output function
int VoiceChanger_GetOutput(void *pWorkMem, short *DstData, int Len);

// get functions
int VoiceChanger_GetSampleRate(void *pWorkMem);
int VoiceChanger_GetChannel(void *pWorkMem);

#endif // __VOICE_CHANGER_H__
//==========================================================================
#ifndef __unsample_api_h__
#define __unsample_api_h__

#define OPTION_FLAG_LEFT_CH_ONLY	1  // only working when ch=2 and upsample left channel

const char *UpSample_GetVersion(void);

// functions for allocate memory by user
unsigned int UpSample_GetMemSize(int decfrsize);
void UpSample_Init(void *pWorkMem, int decfrsize);									// added by Jacky Lu

// functions for allocate memory by Upsample Library itself
void *UpSample_Create(int decfrsize);
void UpSample_Del(void *pWorkMem);

// functions for Audio-Source-Interface
void UpSample_Link(void *pWorkMem, void *FrontEnd, int (*pfnGetData)(void *, short *, int), int fs, int ch, int factor, int OptionFlag);
void UpSample_Unlink(void *pWorkMem);
int UpSample_GetSampleRate(void *pWorkMem);
int UpSample_GetChannel(void *pWorkMem);

int UpSample_GetOutput(void *pWorkMem, short *DstData, int Len); // added by Jacky Lu

#endif
//==========================================================================
#ifndef __downsample_api_h__
#define __downsample_api_h__

const char *DownSample_get_version(void);

int DownSample_GetMemSize(int MaxEncFrameSize, int MaxDownSampleFactor);
void DownSample_Init(void *pWorkMem, int MaxEncFrameSize, int MaxDownSampleFactor);

void *DownSample_Create(int MaxEncFrameSize, int MaxDownSampleFactor);
void DownSample_Del(void *pWorkMem);

void DownSample_Link(
	void *pWorkMem,
	void *pBackEnd,
	int (*pBackEnd_PutData)(void *, const short*, int),
	int fs,
	int channel,
	int factor);

int DownSample_PutData(void *pWorkMem, const short *InData, int InLen);

int DownSample_GetSampleRate(void *_pWorkMem);
int DownSample_GetChannel(void *pWorkMem);
#endif
//==========================================================================
#ifndef __LPF_H__
#define __LPF_H__

#define Max_LoopCnt 10  //�̤j���Ƴ]�w
extern void LPF_init(long coef_frq,short coef_loop);
extern INT16U LPF_process(INT16U FilterBuf);
#endif // __LPF_H__
//==========================================================================
#ifndef __AVIPACKER_H__
#define __AVIPACKER_H__

#define AVIPACKER_RESULT_OK						0
#define AVIPACKER_RESULT_PARAMETER_ERROR		0x80000000

#define AVIPACKER_RESULT_FILE_OPEN_ERROR		0x80000001
#define AVIPACKER_RESULT_BS_BUF_TOO_SMALL		0x80000002
#define AVIPACKER_RESULT_BS_BUF_OVERFLOW		0x80000003
#define AVIPACKER_RESULT_FILE_WRITE_ERR			0x80000004
#define AVIPACKER_RESULT_FILE_SEEK_ERR			0x80000005

#define AVIPACKER_RESULT_MEM_ALIGN_ERR			0x80000006
#define AVIPACKER_RESULT_OS_ERR					0x80000007

#define AVIPACKER_RESULT_IDX_FILE_OPEN_ERROR	0x80000008
#define AVIPACKER_RESULT_IDX_BUF_TOO_SMALL		0x80000009
#define AVIPACKER_RESULT_IDX_BUF_OVERFLOW		0x8000000A
#define AVIPACKER_RESULT_IDX_FILE_WRITE_ERR		0x8000000B
#define AVIPACKER_RESULT_IDX_FILE_SEEK_ERR		0x8000000C
#define AVIPACKER_RESULT_IDX_FILE_READ_ERR		0x8000000D

#define AVIPACKER_RESULT_FILE_READ_ERR			0x8000000E
#define AVIPACKER_RESULT_IGNORE_CHUNK			0x8000000F

#define AVIPACKER_RESULT_FRAME_OVERFLOW			0x80000010

#define AVIPACKER_RESULT_FILE_CAT_ERR			0x80000011

#ifndef AVIIF_KEYFRAME
#define AVIIF_KEYFRAME							0x00000010
#endif // AVIIF_KEYFRAME

typedef struct
{
	unsigned short	left;
	unsigned short	top;
	unsigned short	right;
	unsigned short	bottom;
} GP_AVI_RECT;

typedef struct
{
	unsigned char	fccType[4];
	unsigned char	fccHandler[4];
	unsigned int	dwFlags;
	unsigned short	wPriority;
	unsigned short	wLanguage;
	unsigned int	dwInitialFrames;
	unsigned int	dwScale;
	unsigned int	dwRate;
	unsigned int	dwStart;
	unsigned int	dwLength;
	unsigned int	dwSuggestedBufferSize;
	unsigned int	dwQuality;
	unsigned int	dwSampleSize;
	GP_AVI_RECT		rcFrame;
} GP_AVI_AVISTREAMHEADER;	// strh

typedef struct
{
	unsigned int	biSize;
	unsigned int	biWidth;
	unsigned int	biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned char	biCompression[4];
	unsigned int	biSizeImage;
	unsigned int	biXPelsPerMeter;
	unsigned int	biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
    unsigned int	Unknown[7];
} GP_AVI_BITMAPINFO;	// strf

typedef struct
{
	unsigned short	wFormatTag;
	unsigned short	nChannels;
	unsigned int	nSamplesPerSec;
	unsigned int	nAvgBytesPerSec;
	unsigned short	nBlockAlign;
	unsigned short	wBitsPerSample;
	unsigned short	cbSize;				// useless when wFormatTag is WAVE_FORMAT_PCM
	unsigned short  ExtInfo[16];		// use when the wFormatTag is MS_ADPCM 
} GP_AVI_PCMWAVEFORMAT;	// strf

// start avi packer 
int AviPackerV3_Open(
	void *WorkMem,
	int								fid,			// Record file ID
	int								fid_idx,		// Temporary file ID for IDX	
	const GP_AVI_AVISTREAMHEADER	*VidHdr,		// Video stearm header
	int								VidFmtLen,		// Size of video stream format, count in byte
	const GP_AVI_BITMAPINFO			*VidFmt,		// Video stream format
	const GP_AVI_AVISTREAMHEADER	*AudHdr,		// Audio stearm header = NULL if no audio stream
	int								AudFmtLen,		// Size of audio stream format, count in byte. If zero => no audio stream
	const GP_AVI_PCMWAVEFORMAT		*AudFmt,		// Audio stream format. = NULL if no audio stream
	unsigned char					prio,			// Task priority
	void							*pRingBuf,		// File write buffer
	int								RingBufSize,	// File write buffer size, count in byte
	void							*pIdxRingBuf,	// Index buffer
	int								IdxRingBufSize);// Index buffer size, count in byte

// put raw data to avi packer
int AviPackerV3_PutData(
	void *WorkMem,
	unsigned long fourcc,	// Chunk name of AVI stream '00dc' or '01wb'
	long cbLen,				// number of byte to write
	const void *ptr,		// pointer to write data, MUST be 4-byte alignment
	int nSamples,			// number of samples in this chunk of this stream
	int ChunkFlag);			// CHUNK Flag

// stop avi packer
int AviPackerV3_Close(void *WorkMem);

// get avi packer version
const char *AviPackerV3_GetVersion(void);

// avi packer error handle
void AviPackerV3_SetErrHandler(void *WorkMem, int (*ErrHandler)(int ErrCode));

// Add info string to INFO LIST
int AviPackerV3_AddInfoStr(void *WorkMem, const char *fourcc, const char *info_string);

int AviPackerV3_GetWorkMemSize(void);
#endif // __AVIPACKER_H__
//============================================================================

//Graphic
typedef struct{
    INT16S start_x;
    INT16S start_y;
}GraphicCoordinates,*PGraphicCoordinates;

typedef struct{
	INT8U   bitmap_data_mode;  		//Character 0,bitmap 1 ;
    INT8S   bitmap_color_palette; 	//Input data mode eg,YUYV/RGBG/RGB/......
    INT16U	bitmap_front_color;		//Front color ;
    INT16U	bitmap_background_color;//background color ;
	INT16U	bitmap_real_x_size;		//User set data buffer x size
	INT16U	bitmap_real_y_size;     //User set data buffer y size
	INT16U	bitmap_cell_x_size;		//User set data buffer character x size
	INT16U	bitmap_cell_y_size;		//User set data buffer character y size

	INT16U  bitmap_extend_x_size;	//Graphic Drv adjust
	INT16U  bitmap_extend_y_size;
    INT16U  *bitmap_databuf;
}GraphicBitmap,*PGraphicBitmap;

typedef enum{
    GRAPHIC_COLOR_MODE_NULL  = 0,
    GRAPHIC_2_BIT_COLOR_MODE ,
    GRAPHIC_4_BIT_COLOR_MODE ,
    GRAPHIC_6_BIT_COLOR_MODE ,
    GRAPHIC_8_BIT_COLOR_MODE ,
    GRAPHIC_15_BIT_COLOR_MODE ,
    GRAPHIC_16_BIT_COLOR_MODE ,
    GRAPHIC_RGBG_COLOR_MODE ,
    GRAPHIC_YUYV_COLOR_MODER ,
    GRAPHIC_COLOR_MAX_MODE
}GraphicColorMode;

#define GRAPHIC_INIT_SUCESS (1)
#define GRAPHIC_INIT_FAIL   (0)

typedef struct
{
	INT8U font_width;
	INT8U font_height;
	INT8U bytes_per_line;
	INT8U *font_content;
}FontTableAttribute,*PFontTableAttribute;
typedef struct
{
	INT16U start_x;
	INT16U start_y;
	INT16U end_x;
	INT16U end_y;
	INT16U width;
	INT16U color;
}GraphicLineAttirbute,*PGraphicLineAttirbute;

typedef struct
{
	INT16U start_x;
	INT16U start_y;
	INT16U width;
	INT16U height;
	INT16U line_width;
	INT16U line_color;
	INT16U fill_color;
}GraphicRectangleAttirbute,*PGraphicRectangleAttirbute;

typedef struct
{
	INT16U start_x;
	INT16U start_y;
	INT16U radius;
	INT16U line_width;
	INT16U line_color;
	INT16U fill_color;
}GraphicCircleAttirbute,*PGraphicCircleAttirbute;

#if 0
typedef struct {
  INT16U c0;
  INT16U c1;
} GUI_FONT_TRANSLIST;

typedef struct {
  INT16U FirstChar;
  INT16U LastChar;
  const GUI_FONT_TRANSLIST  * pList;
} GUI_FONT_TRANSINFO;

typedef struct {
  INT8U XSize;
  INT8U XDist;
  INT8U BytesPerLine;
  const unsigned char  * pData;
} GUI_CHARINFO;

typedef struct  {
  INT16U First;                                	         /* first character               */
  INT16U Last;                                           /* last character                */
  const GUI_CHARINFO  * paCharInfo;           /* address of first character    */
  const struct GUI_FONT_PROP  * pNext;        /* pointer to next               */
} GUI_FONT_PROP;

typedef struct  {
  INT8U YSize;
  INT8U YDist;
  INT8U XMag;
  INT8U YMag;
  GUI_CHARINFO *pData;
}GUI_FONT;
#endif

extern void GraphicPutPixel(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
//extern void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern INT32U GraphicInitDrv(GraphicBitmap *bitmap);
extern void GraphicFinishDrv(GraphicBitmap *bitmap);//,GraphicCoordinates *coordinates);
//extern INT32U GraphicGetFontHeight(INT16U character_code,INT8U language,INT8U font_type);
//extern INT32U GraphicGetFontWidth(INT16U character_code,INT8U language,INT8U font_type);
extern void GraphicPutChar(GraphicBitmap *bitmap,GraphicCoordinates *coordinates,FontTableAttribute *font);

extern void GraphicSetBitmapBackgroundColor(GraphicBitmap *bitmap,INT16U color);
extern void GraphicSetBitmapFrontColor(GraphicBitmap *bitmap,INT16U color);

extern void GraphicPutLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void GraphicPutRectangle(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);
extern void GraphicPutBiasLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void GraphicPutCircleCaculate(GraphicBitmap *bitmap,INT32U x0,INT32U y0,INT32U r);

extern INT16U GraphicCalendarConfirmWeek(INT16U year, INT8U month, INT8U day);
extern INT8U GraphicCalendarCalculateDays(INT8U month, INT16U year);

//------------------------------------------------------------------------------
// BIN Loader
//------------------------------------------------------------------------------

#ifndef _BINLOADER_DEF_
#define _BINLOADER_DEF_

typedef enum
{
    GPZP_DECOMPRESS_OFF=0,
    GPZP_DECOMPRESS_ON=0xA8B9
} GPZP_DECOMPRESS_SWITCH;

typedef enum
{
    BIN_INIT=0x00,
    BIN_WORKING=0x2BCDEF5A,
    BIN_TURNKEY
} BIN_STATUS;

typedef enum
{
    BIN_PLUG_NONE=0xA0,
    BIN_PLUG_IN=0xA1,
    BIN_PLUG_OUT=0xA2
} BIN_STG_PLUG;

typedef enum
{
    BIN_STG_CF_PLUG=0x00,
    BIN_STG_NAND_PLUG,
    BIN_STG_SDMS_PLUG,
    BIN_STG_USBH_PLUG,
    BIN_STG_USBD_PLUG,
    BIN_STG_NONE_PLUG=0xAA
} BIN_STG_PLUG_TYPE;

typedef enum
{
    BIN_KEY_1=0,
    BIN_KEY_2=1,
    BIN_KEY_3=2,
    BIN_KEY_4=3,
    BIN_KEY_5=4,
    BIN_KEY_6=5,
    BIN_KEY_7=6,
    BIN_KEY_8=7,
    BIN_KEY_POWER=0x10,
    BIN_KEY_NONE=0xAA
} BIN_KEY_ID;

typedef enum
{
    BIN_EVENT_NONE=0,
    BIN_EVENT_MAINMENU_ENTRY=BIN_EVENT_NONE,
    BIN_EVENT_SLIDE_SHOW_ENTRY,
    BIN_EVENT_THUMBNAIL_ENTRY,
    BIN_EVENT_CALENDAR_ENTRY,
    BIN_EVENT_SETUP_ENTRY,
    BIN_EVENT_USB_DEVICE_PC_IN,
    BIN_EVENT_POWER_OFF,
    BIN_EVENT_RESET,
    BIN_EVENT_MAX
} BIN_EVENT_ID;


#endif //_BINLOADER_DEF_

extern int bin_loder_init(void);
extern void bin_switch_name_set(char* rom_name);
extern int bin_switch_rs_offset_get(void);
extern void bin_compress_status_set(GPZP_DECOMPRESS_SWITCH decode_flag);
extern void bin_switcher_entry(void);
extern void bin_switch_to_main(void);
extern void bin_switch_to_nozp_main(void);
extern void bin_switch_to_rom(char* rom_name);
extern void bin_switch_to_gpzp_rom(char* rom_name);
extern void JumpToAddr(INT32U JumpAddr);
extern void bin_switch_to_rom(char* rom_name);
extern void bootswitcher_handler(void);


//------------------------------------------------------------------------------
// storages
//------------------------------------------------------------------------------

struct Usb_Storage_Access {
    INT32S	(*usdc_read_cmd_phase)(INT32U lba,INT32U seccount);
    INT32S	(*usdc_read_dma_phase)(INT32U *buf,INT8U ifwait,INT32U seccount);
    INT32S 	(*usdc_read_cmdend_phase)(void);
    INT32S	(*usdc_write_cmd_phase)(INT32U lba,INT32U seccount);
    INT32S	(*usdc_write_dma_phase)(INT32U *buf,INT8U ifwait,INT32U seccount);
    INT32S 	(*usdc_write_cmdend_phase)(void);
};

#ifndef _FLASHRSINDEX_DEF_
#define _FLASHRSINDEX_DEF_
typedef struct
{
    //INT8U  tag;     // none
    INT8U  name[19]; //FILE NAME
    INT32U pos;     //Phyical address
    INT32U offset;  //ADD
    INT32U size;    //by sector
} FLASHRSINDEX;
#endif

//NV function
//extern  SYSTEM_USER_OPTION Global_User_Optins;

extern INT32U nvmemory_init(void);

extern INT16U nv_open(INT8U *path);
extern INT32U nv_read(INT16S fd, INT32U buf, INT32U size);
extern INT32U nv_lseek(INT16S fd, INT32S foffset, INT16S origin);
extern INT32S nv_fast_read(INT16S fd, INT8U* buf, INT32U byte_size);
extern INT32U nv_rs_size_get(INT16S fd); // jandy add to querry fd size

extern INT32U nvmemory_rs_byte_load(INT8U *name ,INT32U offset_byte, INT32U *pbuf , INT32U byte_count);
extern INT32U nvmemory_rs_sector_load(INT8U *name ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
extern INT32U nvmemory_user_sector_load(INT16U itag , INT32U *pbuf , INT16U secter_count);
extern INT32U nvmemory_user_sector_store(INT16U itag , INT32U *pbuf , INT16U secter_count);

//NV SPI
extern INT32U nvspiflash_retrieval(void);
extern INT32U nvspiflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
extern INT32U nvspiflash_user_set (INT16U itag ,INT32U *pbuf , INT16U secter_count);
extern INT32U nvspiflash_user_get (INT16U itag ,INT32U *pbuf , INT16U secter_count);
#if 1//#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
INT32U nvspiflash_file_size_get(INT16U itag);
#endif
//NV NAND
extern INT32U nvnandflash_retrieval(void);
extern INT32U nvnandflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
extern INT32U nvnandflash_user_get(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count);
extern INT32U nvnandflash_user_set(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count);
extern void NVRAM_OS_LOCK(void);
extern void NVRAM_OS_UNLOCK(void);
extern void nvram_os_init(void);
//NV RAM
extern INT32U nvram_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
//NV PACKER
extern INT16U nvpack_init(INT8U* nvpack_upper_name);
extern INT16S nvpack_uninit(void);
extern INT16U nvp_fopen(INT8U* inpack_upper_name);
extern INT32S nvp_fread(INT16U nvp_infd, INT32U pbuf, INT32U byte_size);
extern INT16S nvp_fclose(INT16U nvp_infd);
extern INT32U nvp_lseek(INT16U nvp_infd, INT32S foffset);
extern INT32U nvp_fsize_get(INT16U nvp_infd);

//Card detection
extern void storages_cfc_reset(void);
extern void storages_init(INT16U devicetpye );
extern void storage_polling_start(void);
extern void storage_polling_stop(void);
#ifdef _PROJ_TYPE
extern void turnkey_stor_detection_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
extern void turnkey_usbh_detection_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
extern void turnkey_stor_noise_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
#endif
extern void storage_sdms_detection(void);
extern void storage_cfc_detection(void);
extern INT16U storage_detection(INT16U type);
extern void storage_polling_usbh(void);
extern void storage_usbd_state_exit(void);
extern INT16U storage_usbd_detection(void);
extern INT16U storage_usbh_detection(void);

#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
extern BIN_STG_PLUG bin_stg_plug_get(void);
extern BIN_STG_PLUG_TYPE bin_stg_change_type_get(void);
extern void bin_stg_init(void);
#endif
extern INT32U storage_wpb_detection(void);
extern INT32U combin_reg_data(INT8U *data, INT32S len);


//------------------------------------------------------------------------
//SPU
#define C_SPU_PW_FIQ			1
#define C_SPU_BEAT_FIQ			2
#define C_SPU_ENV_FIQ			3
#define C_SPU_FIQ				4
#define C_MAX_FIQ				5

extern INT16U *T_SPU_MIDI_Table[];
/*
extern INT32U *T_InstrumentSectionAddr[];
extern INT32U *T_InstrumentStartSection[];
extern INT32U *T_InstrumentPitchTable[];
extern INT32U *T_DrumAddr[];
*/

//extern void SPU_Init(void);
//extern void Load_ALP_Header(INT32U *pAddr);
//extern void SPU_PlayNote(INT8U uiPitch, INT32U *uiAddr, INT8U uiPen, INT8U uiVelocity, INT8U uiSPUChannel);
//extern void SPU_PlayDrum(INT8U uiDrumIndex, INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
extern void SPU_PlayPCM_NoEnv_FixCH(INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
//extern void SPU_PlayTone(INT8U uiPitch, INT32U *pAddr, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
void SPU_PlayTone1(INT8U uiPitch, INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);

extern void SPU_MIDI_Play(INT8U repeat_en);//repeat_en: 1 repeat;   0 no repeat
extern void SPU_MIDI_Repeat(INT8U repeat_en);//repeat_en: 1 repeat;   0 no repeat
extern void SPU_MIDI_Stop(void);
//extern void SPU_MIDI_Service(void);
//extern void F_CheckDuration(void);
extern void SPU_EnableChannelFIQ(void);
extern void SPU_DisableChannelFIQ(void);
//extern void SPU_EnableEnvelopeFIQ(void);
//extern void SPU_DisableEnvelopeFIQ(void);
extern void SPU_EnableBeatCountFIQ(void);
extern void SPU_DisableBeatCountFIQ(void);
//extern void SPU_EnablePostWaveFIQ(void);
//extern void SPU_DisablePostWaveFIQ(void);
extern void SPU_MIDI_Pause(void);
extern void SPU_MIDI_Resume(void);
extern void SPU_pause_channel(INT8U spu_channel);
extern void SPU_resume_channel(INT8U spu_channel);
extern void SPU_Initial(void);
extern void SPU_MIDI_Set_MIDI_Volume(INT32U MIDI_Volume);//0~127
extern INT32U SPU_MIDI_Get_Status(void);
extern INT8U SPU_Get_SingleChannel_Status(INT8U SPU_Channel);

//extern void F_StopExpiredCH(void);
//extern void F_GetSeqCmd(void);
//extern void ProcessNoteEvent(void);
//extern void ProcessTempoEvent(void);
//extern void ProcessBeatCountEvent(void);
//extern void ProcessPitchBendEvent(void);
//extern void ProcessProgramChangeEvent(void);
//extern void ProcessControlEvent(void);
//extern INT32U FindEmptyChannel(void);
//extern INT32U Calculate_Pan(INT32U SPU_Ch);
extern void SPU_StopChannel(INT32U StopChannel);
//extern void ProcessEndEvent(void);
extern void SPU_MIDI_SetStopCallBackFunction(void (*StopCallBack)(void));
extern void SPU_MIDI_SetDataEntryEventCallback(void (*DataEntryEvent_callback)(void));	
extern void SPU_MIDI_GetControlEventInfo(INT8U *para);									
extern INT32U SPU_MIDI_GetCurDt(void);													
extern void SPU_MIDI_PlayDt(INT32U MidiStartDt, INT32U MidiStopDt);						
extern void SPU_MIDI_SetPlayDtEndCallback(void (*PlayDtStopCallBack)(void));			
extern void SPU_AttachISR(INT8U FIQ_ID, void (*ISR)(void));
extern void SPU_ReleaseISR(INT8U FIQ_ID);
extern void SPU_MIDI_Set_SPU_Channel_Mask(INT32U SPU_CH_MASK);
extern void SPU_MIDI_SetMidiMask(INT32U MIDI_Ch);
//------------------------------------------------spu_bin
extern INT32S  SPU_Free_Midi(void);
extern INT32S	SPU_Free_ToneColor_1(void);//free ڶɫ(free the second group tonecolor)
extern void		SPU_MIDI_Set_MIDI_Pan(INT32U MIDI_Pan);
//extern INT32U SPU_get_midi_ring_buffer_ri(void);
//extern INT16U get_a_word(void);
#if SUPPORT_MIDI_LOAD_FROM_NVRAM == 1
extern void     SPU_resource_read_register(void* rs_read);
#endif
extern void	 SPU_Set_midi_ring_buffer_addr(INT32U ring_buffer_addr);
extern INT32S	SPU_load_tonecolor(INT16S fd_idi, INT32U MIDI_Index, INT8U mode);//mode:  0:NAND flash logic area, 2:NAND flash app area, 3:from file system
extern INT32S 	SPU_load_tonecolor_1(INT16S fd_idi, INT32U MIDI_Index, INT8U mode);//loadڶɫ(load the second group tonecolor)
extern INT32S		SPU_check_fill_midi_ring_buffer(void);
extern void SPU_MIDI_SetMidiCH_ChangeColor_Mask(INT32U MIDI_Ch_Mask);
extern void SPU_MIDI_SetMidiCH_ChangeColor(INT32U MIDI_CH, INT32U color_index);
extern void TM_PlayTone(INT32U uiMidiCh, INT8U uiPitch, INT8U uiPan, INT8U uiVelocity);
extern void TM_StopTone(INT32U uiMidiCh, INT8U uiPitch);
extern void SPU_MIDI_Set_Tempo(INT8U tempo);
extern void SPU_Set_idi_addr(INT32U idi_addr);//(set "*.idi" data's start addr in nandflash(or in a file))idiNand flashеʼַFSļеƫƵַ
extern void SPU_Set_adpcm_comb_addr(INT32U adpcm_comb_addr, INT32U ram_buffer_addr);//adpcm_combNand flashеʼַFSļеƫƵַ,ԼADPCMʱRAMַram_buffer_addr=0:user_memory_malloc()ַҪṩ㹻RAMĵַ
			//adpcm_comb_addr:the adpcm combo data's start addr in nandflash(or in a file);
			//ram_buffer_addr:the ram addr to load the adpcm data; 0--it will malloc a fit ram area when you call SPU_load_comb_adpcm_to_RAM(); others--the ram addr defined by user;
extern INT32S SPU_load_comb_adpcm_to_RAM(INT16S fd_adpcm_comb, INT32U adpcm_index, INT8U mode);//mode:  0:NAND flash logic area, 2:NAND flash app area, 3:from file system
extern INT32S SPU_Free_adpcm_data_temp_buffer(void);//(free adpcm temp buffer)ͷڴADPCMݵڴ
extern INT32S SPU_play_comb_adpcm_FixCH(INT8U uiPan, INT8U uiVelocity, INT8U uiSPUChannel);
//--------------------------------------------------------------------------------------------------
extern void* user_memory_malloc(INT32U size);
extern void user_memory_free(void* buffer_addr);
extern INT32S	read_a_sector(INT16S fd, INT32U data_offset_addr, INT32U buffer_addr,INT8U mode);
extern INT32S	read_two_sector(INT16S fd, INT32U data_offset_addr, INT32U buffer_addr,INT8U mode);
//mode:  0:NAND flash logic area, 
//       2:NAND flash app area, 
//       3:from file system, 
//       6:from sdram without fs
//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------//лɫͲ
extern INT32U	T_InstrumentStartSection_1[129];//лɫͲ
extern INT32U	T_InstrumentPitchTable_1[500];//лɫͲ
extern INT32U	*T_InstrumentSectionAddr_1[500];//лɫͲ
//---------------------------------------------------------------------
#define MIDI_RING_BUFFER_SIZE	512*8//4096 byte
// added by Bruce, 2008/10/31
//extern INT32U *T_InstrumentSectionAddr[];
//extern INT32U T_InstrumentStartSection[];
//extern INT32U T_InstrumentPitchTable[];
//extern INT32U *T_DrumAddr[];
//extern INT16S static_fd_idi;
//extern INT32U total_inst,total_drum;
//extern INT32U currt_midi_data_offset;
//extern INT32U midi_length;
//extern INT32U midi_ring_buffer_addr;
//extern INT32U midi_ring_buffer_ri, midi_ring_buffer_wi;
// added by Bruce, 2008/10/31
//------------------------------------------------------------------------------
//#define _GPL32_GRAPHIC_NORMAL_

#ifdef _GPL32_GRAPHIC_NORMAL_
#define _LOAD_FONT_FROM_NVRAM_  1

#ifndef GUI_FLASH
	#define GUI_FLASH
#endif

typedef struct
{
	INT8U font_width;
	INT8U font_height;
	INT8U bytes_per_line;
	const INT8U *font_content;
}GraphicFontInfo,*pGraphicFontInfo;

#define GRAPHIC_MAX_FONT_X_SIZE 64
#define GRAPHIC_MAX_FONT_Y_SIZE 64

#define _FONT_ENGLISH_ARAIL_8_  1   //height 14,width alterable,size equal to windows system size "8"
#define _FONT_ENGLISH_ARAIL_12_ 1   //height 18,width alterable,size equal to windows system size "12"
#define _FONT_ENGLISH_ARAIL_16_ 1	//height 24,width alterable,size equal to windows system size "16"
#define _FONT_ENGLISH_ARAIL_20_ 1	//height 32,width alterable,size equal to windows system size "20"

#define _FONT_SCHINESE_SIMSUN_8_      1          //height 14,width not sure,size equal to windows system size "8"
#define _FONT_SCHINESE_SIMSUN_12_     1          //height 18,width not sure,size equal to windows system size "12"
#define _FONT_SCHINESE_SIMSUN_16_     1          //height 24,width not sure.size equal to windows system size "16"
#define _FONT_SCHINESE_SIMSUN_20_     1          //height 32,width not sure.size equal to windows system size "20"

typedef enum
{
#if _FONT_ENGLISH_ARAIL_8_ == 1
	FONT_ENGLISH_ARAIL_8,
#endif

#if _FONT_ENGLISH_ARAIL_12_ == 1
	FONT_ENGLISH_ARAIL_12,
#endif

#if _FONT_ENGLISH_ARAIL_16_ == 1
	FONT_ENGLISH_ARAIL_16,
#endif

#if _FONT_ENGLISH_ARAIL_20_ == 1
	FONT_ENGLISH_ARAIL_20,
#endif

#if _FONT_SCHINESE_SIMSUN_8_ == 1
	FONT_SCHINESE_SIMSUN_8,
#endif

#if _FONT_SCHINESE_SIMSUN_12_ == 1
	FONT_SCHINESE_SIMSUN_12,
#endif

#if _FONT_SCHINESE_SIMSUN_16_ == 1
	FONT_SCHINESE_SIMSUN_16,
#endif

#if _FONT_SCHINESE_SIMSUN_20_ == 1
	FONT_SCHINESE_SIMSUN_20,
#endif

	FONT_MAX_XXX
}FONT_TYPE;

#if _LOAD_FONT_FROM_NVRAM_ == 0
//English Font: The best way to load font is build font into the C code.
//But Simple Chinese Font must storage in NVRAM,for it is so big code size,
//and will copy so much SDRAM Memory...
extern GUI_FLASH const GraphicFontInfo Arial_11_8_CharInfo[] ;
extern GUI_FLASH const GraphicFontInfo Arial_16_12_CharInfo[];
extern GUI_FLASH const GraphicFontInfo Arial_21_16_CharInfo[];
extern GUI_FLASH const GraphicFontInfo Arial_27_20_CharInfo[];
#endif

typedef enum
{   //Pixel data mode.
	DRV_GRAPHIC_2_BIT_DATA = 0,
    DRV_GRAPHIC_4_BIT_DATA,
    DRV_GRAPHIC_6_BIT_DATA,
    DRV_GRAPHIC_8_BIT_DATA,
    DRV_GRAPHIC_15_BIT_DATA,
    DRV_GRAPHIC_16_BIT_DATA,
    DRV_GRAPHIC_RGBG_DATA,
    DRV_GRAPHIC_YUYV_DATA,
    DRV_GRAPHIC_DATA_MAX
}GraphicDataMode;

typedef enum
{   //Graphic write mode.
	DRV_GRAPHIC_COPY_PUT = 0,
	DRV_GRAPHIC_XOR_PUT,
	DRV_GRAPHIC_OR_PUT,
	DRV_GRAPHIC_AND_PUT,
	DRV_GRAPHIC_NOT_PUT,
	DRV_GRAPHIC_PUT_MAX

}GraphicWriteMode;

typedef enum
{
	//Line write mode.
	DRV_GRAPHIC_SOLID_LINE = 0,
	DRV_GRAPHIC_DOTTED_LINE,
	DRV_GRAPHIC_CENTER_LINE,
	DRV_GRAPHIC_DASHED_LINE,
	DRV_GRAPHIC_USERBIT_LINE
}GraphicLineMode;

typedef enum
{
	//Line width
	DRV_GRAPHIC_NORMAL_WIDTH = 1,
	DRV_GRAPHIC_TWO_WIDTH,
	DRV_GRAPHIC_THREE_WIDTH
}GraphicLineWidth;


typedef enum
{
	//Font Put mode.
	DRV_GRAPHIC_FONT_COVER_PUT,
	DRV_GRAPHIC_FONT_TREANS_PUT
}GraphicFontPutMode;

#define _GRAPHIC_DEFAULT_BK_COLOR  	   1
#define _GRAPHIC_DEFAULT_PALETTE       0
#define _GRAPHIC_DEFAULT_FRONT_COLOR   2
#define _GRAPHIC_DEFAULT_FILL_COLOR    3
#define _GRAPHIC_DEFAULT_POSITION_X    0
#define _GRAPHIC_DEFAULT_POSITION_Y    0
#define _GRAPHIC_DEFAULT_FONT_TYPE     0
#define _GRAPHIC_DEFAULT_FONT_LANGUAGE 0
#define _GRAPHIC_DEFAULT_WRITE_MODE    DRV_GRAPHIC_COPY_PUT
#define _GRAPHIC_DEFAULT_LINE_MODE     DRV_GRAPHIC_SOLID_LINE
#define _GRAPHIC_DEFAULT_LINE_WIDTH    DRV_GRAPHIC_NORMAL_WIDTH
#define _GRAPHIC_DEFAULT_FONT_PUT_MODE     DRV_GRAPHIC_FONT_COVER_PUT


typedef struct{
	INT8U   GraphicDataMode;        //Graphic data mode .
    INT8U   GraphicColorPalette; 	//No used.For future used.

    INT16U  GraphicFrameWidth;      //Width of Frame buffer.
    INT16U  GraphicFrameHeight;     //Height of Frame buffer.
	INT32U *GraphicFramebuf; 		//Pointer to Frame buffer .

	INT32U  GraphicFrontColor;      //Front Color to operation.
	INT32U  GraphicBackgroundColor; //Background Color to operation.
	INT32U  GraphicFillColor;       //Fill color for graphic.

	INT16S  GraphicPositonX;        //Current start position of x
	INT16S  GraphicPositonY;		//Current start position of y

	INT8U   GraphicFontType;        //Current Font Type.
	INT8U   GraphicFontLanguage;    //No use. For future used.

	INT8U   GraphicWriteMode;       //Write Mode.

	INT8U   GraphicLineMode;        //Line Mode.
	INT8U   GraphicLineWidth;       //Line Width;

	INT8U   GraphicFontPutMode;     //Font Mode;

  	#if _OPERATING_SYSTEM == 1
	OS_EVENT  *GraphicProcessSem;   //Graphic Protect Sem.
	#endif

}GraphicFrameAttribute;

extern GraphicFrameAttribute GraphicGlobalControl;
extern GraphicFrameAttribute *pGraphicGlobalControl;


/*---Drv Graphic Grobal Var Set or Get ---*/
/*****************************************************************************
 Function Name:      DrvGraphicDataModeSet
 Purposes     :      Graphic data mode grobal var set.
 Parameter    :      DataMode ->graphic data format.
 					 Refered to :Enum GraphicDataMode
                     Not call this function otherwise you want to built new graphic working buffer.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                    Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDataModeSet(INT8U DataMode);
/*****************************************************************************
 Function Name:      DrvGraphicDataModeGet
 Purposes     :      Graphic data mode grobal var get.
 Parameter    :      Null
 Return Value :
          			 Grobal data mode value.Refered to :Enum GraphicDataMode
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicDataModeGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicColorPaletteSet
 Purposes     :      Graphic palette grobal var set.<No used revered for furture used>
 Parameter    :      ColorPalette
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicColorPaletteSet(INT8U ColorPalette);
/*****************************************************************************
 Function Name:      DrvGraphicColorPaletteGet
 Purposes     :      Graphic palette grobal var get.<No used revered for furture used>
 Parameter    :      Null
 Return Value :
          			 grobal palette mode value.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicColorPaletteGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFrameWidthSet
 Purposes     :      Graphic working buffer width grobal var set.
 Parameter    :      FrameWidth -> Width of working buffer
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFrameWidthSet(INT16U FrameWidth);
/*****************************************************************************
 Function Name:      DrvGraphicFrameWidthGet
 Purposes     :      Graphic working buffer width grobal var get.
 Parameter    :      Null
 Return Value :
          			 graphic working buffer width
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicFrameWidthGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFrameHeightSet
 Purposes     :      Graphic working buffer height grobal var set.
 Parameter    :      FrameHeight -> Height of working buffer
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFrameHeightSet(INT16U FrameHeight);
/*****************************************************************************
 Function Name:      DrvGraphicFrameHeightGet
 Purposes     :      Graphic working buffer height grobal var get.
 Parameter    :      Null
 Return Value :
          			 Height of working buffer
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicFrameHeightGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFramebufSet
 Purposes     :      Graphic working buffer addr grobal var set.
                     Strongly suggest not call this var otherwise you want to set new working buffer.
 Parameter    :      FramebufAddr -> addr of working buffer
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFramebufSet(INT32U *FramebufAddr);
/*****************************************************************************
 Function Name:      DrvGraphicFramebufGet
 Purposes     :      Graphic working buffer addr grobal var get.
 Parameter    :      Null
 Return Value :
          			 Addr of working buffer
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT32U *DrvGraphicFramebufGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFrontColorSet
 Purposes     :      Graphic working front color grobal var set.
 Parameter    :      FrontColor -> FrontColor of working buffer
 					 Just fill this par used the real color of frambuffer.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFrontColorSet(INT32U FrontColor);
/*****************************************************************************
 Function Name:      DrvGraphicFrontColorGet
 Purposes     :      Graphic working front color grobal var get.
 Parameter    :      Null
 Return Value :
          			 Front color
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicFrontColorGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicBackgroundColorSet
 Purposes     :      Graphic working background color grobal var set.
 Parameter    :      BackGroundColor -> background color of working buffer
 					 Just fill this par use the real color of frambuffer.

 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicBackgroundColorSet(INT16U BackGroundColor);
/*****************************************************************************
 Function Name:      DrvGraphicBackgroundColorGet
 Purposes     :      Graphic working background color grobal var get.
 Parameter    :      Null
 Return Value :
          			 Height of working buffer
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicBackgroundColorGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFillColorSet
 Purposes     :      Graphic working fill color grobal var set.
 Parameter    :      FillColor -> Fill color  of working buffer
 					 Just fill this par use the real color of frambuffer.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFillColorSet(INT16U FillColor);
/*****************************************************************************
 Function Name:      DrvGraphicFillColorGet
 Purposes     :      Graphic working fill color grobal var get.
 Parameter    :      Null
 Return Value :
          			 Fill color of working buffer
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicFillColorGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicPositonXSet
 Purposes     :      Graphic working coordinate of x  grobal var set.
 Parameter    :      x -> coordinate of x.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPositonXSet(INT16S x);
/*****************************************************************************
 Function Name:      DrvGraphicPositonXGet
 Purposes     :      Graphic working coordinate of x  grobal var get.
 Parameter    :      Null
 Return Value :
          			 Coordinate of x
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16S DrvGraphicPositonXGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicPositonYSet
 Purposes     :      Graphic working y coordinate grobal var set.
 Parameter    :      y -> y coordinate
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPositonYSet(INT16S y);
/*****************************************************************************
 Function Name:      DrvGraphicPositonYGet
 Purposes     :      Graphic working y coordinate grobal var get.
 Parameter    :      Null
 Return Value :
          			 Coordinate of y
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16S DrvGraphicPositonYGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicPositonMoveTo
 Purposes     :      Graphic working x,y coordinates grobal vars set.
 Parameter    :      x -> coordinate of x.
                     y -> coordinate of y.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPositonMoveTo(INT16S x,INT16S y);
/*****************************************************************************
 Function Name:      DrvGraphicFontTypeSet
 Purposes     :      Graphic working Font Type grobal var set.
 Parameter    :      FontType -> Height of working buffer.Used define in Enum FONT_TYPE
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                    Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFontTypeSet(INT8U FontType);
/*****************************************************************************
 Function Name:      DrvGraphicFontTypeGet
 Purposes     :      Graphic working buffer height grobal var get.
 Parameter    :      Null
 Return Value :
          			 Current font type (Enum FONT_TYPE)
 <History>
 Date                         Author                         Modification
 2008/09/24                    Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicFontTypeGet(void);

/*****************************************************************************
 Function Name:      DrvGraphicFontLanguageSet
 Purposes     :      Graphic working lauguage  grobal var set.
                     (This Function is no used, revered for future used.)
 Parameter    :      FontLanguage
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFontLanguageSet(INT8U FontLanguage);
/*****************************************************************************
 Function Name:      DrvGraphicFontLanguageGet
 Purposes     :      Graphic working lauguage  grobal var get.
 Parameter    :      Null
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicFontLanguageGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicWriteModeSet
 Purposes     :      Graphic working buffer write mode grobal var get.
 Parameter    :      WriteMode -> Write Mode of working buffer(Enum GraphicWriteMode)
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicWriteModeSet(INT8U WriteMode);
/*****************************************************************************
 Function Name:      DrvGraphicWriteModeGet
 Purposes     :      Graphic working write mode grobal var get.
 Parameter    :      Null
 Return Value :
          			 Write mode(Enum GraphicWriteMode)
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicWriteModeGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicLineModeSet
 Purposes     :      Graphic working line mode grobal var set.
 Parameter    :      LineMode (Enum GraphicLineMode :Only Support DRV_GRAPHIC_SOLID_LINE )
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicLineModeSet(INT8U LineMode);
/*****************************************************************************
 Function Name:      DrvGraphicWriteModeGet
 Purposes     :      Graphic working line mode grobal var get.
 Parameter    :      Null
 Return Value :
          			 Line mode(Enum GraphicLineMode:Only Support DRV_GRAPHIC_SOLID_LINE )
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicLineModeGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicLineWidthSet
 Purposes     :      Graphic working line width grobal var set.
 Parameter    :      LineMode (Enum :GraphicLineWidth)
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicLineWidthSet(INT8U LineMode);
/*****************************************************************************
 Function Name:      DrvGraphicLineWidthGet
 Purposes     :      Graphic working line width grobal var get.
 Parameter    :      Null
 Return Value :
          			 Line width(Enum:GraphicLineWidth)
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicLineWidthGet(void);
/*****************************************************************************
 Function Name:      DrvGraphicFontPutModeSet
 Purposes     :      Graphic working Font Put Mode grobal var set.
 Parameter    :      FontPutMode -> Refered to :Enum GraphicFontPutMode
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicFontPutModeSet(INT8U FontPutMode);
/*****************************************************************************
 Function Name:      DrvGraphicFontPutModeGet
 Purposes     :      Graphic working Font Put Mode grobal var get.
 Parameter    :      Null
 Return Value :
          			 FontPutMode - >Font put mode .Refered to :Enum GraphicFontPutMode
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT8U DrvGraphicFontPutModeGet(void);

// ------- Application Function ----------
/*****************************************************************************
 Function Name:      DrvInitGraphic
 Purposes     :      Init graphic working buffer and global var.
 Parameter    :      width -> graphic working buffer width.
					 height -> graphic working buffer height.
					 addr -> graphic working buffer address.
					 data_mode ->graphic data format. refered to :Enum GraphicDataMode
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvInitGraphic(INT16U width,INT16U height,INT32U *addr,INT8U data_mode);
/*****************************************************************************
 Function Name:      DrvGraphicDefault
 Purposes     :      Set the default value of graphic global var except of data buffer width/height/addr/datamode.
 Parameter    :      Null
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDefault(void);
/*****************************************************************************
 Function Name:      DrvGraphicGlobalPtrSet
 Purposes     :      Change the global var.
 Parameter    :      Null
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicGlobalPtrSet(GraphicFrameAttribute * control);
/*****************************************************************************
 Function Name:      DrvGraphicClose
 Purposes     :      Close the graphic drv <No Used>.
 Parameter    :      Null
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicClose(void);

/*****************************************************************************
 Function Name:      DrvGraphicGetFontInfo
 Purposes     :      Get font Info include :Font content/width/height/linebytes.
 Parameter    :      font - >Struct GraphicFontInfo to storage font info.
 					 code - > char local code.
 Return Value :
          			 Null
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicGetFontInfo(GraphicFontInfo *font,INT16U code);
/*****************************************************************************
 Function Name:      DrvGraphicGetFontLineBytes
 Purposes     :      Get font line bytes.
 Parameter    :      code - > char local code.
 Return Value :
          			 Line bytes of font
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicGetFontLineBytes(INT16U code);
/*****************************************************************************
 Function Name:      DrvGraphicGetFontWidth
 Purposes     :      Get font width
 Parameter    :      code - > char local code.
 Return Value :
          			 Font width.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicGetFontWidth(INT16U code);
/*****************************************************************************
 Function Name:      DrvGraphicGetFontHeight
 Purposes     :      Get font height.
 Parameter    :      code - > char local code.
 Return Value :
          			 Font height.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicGetFontHeight(INT16U code);
/*****************************************************************************
 Function Name:      DrvGraphicPutPixel
 Purposes     :      Put pixel to  x,y
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
				 	 color - > color of pixel.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPutPixel(INT16S x,INT16S y,INT32U color);
/*****************************************************************************
 Function Name:      DrvGraphicGetPixel
 Purposes     :      Get x,y position pixel.
 Parameter    :      code - > char local code.
 Return Value :
          			 Pixel color value.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT32U DrvGraphicGetPixel(INT16S x,INT16S y);


/*****************************************************************************
 Function Name:      DrvGraphicDrawLine
 Purposes     :      Put line from (startx,starty) to (endx,endy)
 Parameter    :      startx - > start coordinate of x.
				 	 starty - > start coordinate of y.
					 endx - > end coordinate of x.
				 	 endy - > end coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawLine(INT16S startx,INT16S starty,INT16S endx,INT16S endy);
/*****************************************************************************
 Function Name:      DrvGraphicDrawLineRel
 Purposes     :      Put line from (x0,y0) to (x0 + relx,y0+rely)
 Parameter    :      relx - > distance of horizontal.
				 	 rely - > distance of vertical.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawLineRel(INT16U relx,INT16U rely);
/*****************************************************************************
 Function Name:      DrvGraphicDrawLineRel
 Purposes     :      Put line from (x0,y0) to (x,y)
 Parameter    :      x - > end coordinate of x .
				 	 y - > end coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawLineTo(INT16S x,INT16S y);
/*****************************************************************************
 Function Name:      DrvGraphicDrawHorizontalLine
 Purposes     :      Put horizontal from (startx,starty) to (endx,endy), starty must equal to endy
 Parameter    :      startx - > start coordinate of x.
				 	 starty - > start coordinate of y.
					 endx - > end coordinate of x.
				 	 endy - > end coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawHorizontalLine(INT16S startx,INT16S starty,INT16S endx,INT16S endy);
/*****************************************************************************
 Function Name:      DrvGraphicDrawHorizontalLine
 Purposes     :      Put vertical from (startx,starty) to (endx,endy), startx must equal to endx
 Parameter    :      startx - > start coordinate of x.
				 	 starty - > start coordinate of y.
					 endx - > end coordinate of x.
				 	 endy - > end coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicVerticalLine(INT16S startx,INT16S starty,INT16S endx,INT16S endy);
/*****************************************************************************
 Function Name:      DrvGraphicDrawBiasLine
 Purposes     :      Put bias line from (startx,starty) to (endx,endy)
 Parameter    :      startx - > start coordinate of x.
				 	 starty - > start coordinate of y.
					 endx - > end coordinate of x.
				 	 endy - > end coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawBiasLine(INT16S startx,INT16S starty,INT16S endx,INT16S endy);
/*****************************************************************************
 Function Name:      DrvGraphicDrawRectangle
 Purposes     :      Draw rectangle normal.
 Parameter    :      left - > left coordinate of x.
				 	 top - > top coordinate of y.
					 right - > right coordinate of x.
				 	 bottom - > bottom coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawRectangle(INT16S left,INT16S top,INT16S right,INT16S bottom);
/*****************************************************************************
 Function Name:      DrvGraphicDrawRectangleFill
 Purposes     :      Draw rectangle with  fill color.
 Parameter    :      left - > left coordinate of x.
				 	 top - > top coordinate of y.
					 right - > right coordinate of x.
				 	 bottom - > bottom coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawRectangleFill(INT16S left,INT16S top,INT16S right,INT16S bottom);
/*****************************************************************************
 Function Name:      DrvGraphicDrawBar
 Purposes     :      Draw bar with fill color.
 Parameter    :      left - > left coordinate of x.
				 	 top - > top coordinate of y.
					 right - > right coordinate of x.
				 	 bottom - > bottom coordinate of y.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawBar(INT16S left,INT16S top,INT16S right,INT16S bottom);
/*****************************************************************************
 Function Name:      DrvGraphicDrawCircle
 Purposes     :      Draw circle normal.
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 radius - > radius of circle.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawCircle(INT16S x,INT16S y,INT16U radius);
/*****************************************************************************
 Function Name:      DrvGraphicDrawCircleFill
 Purposes     :      Draw circle with fill color.
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 radius - > radius of circle.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawCircleFill(INT16S x,INT16S y,INT16U radius);
/*****************************************************************************
 Function Name:      DrvGraphicDrawEllipse
 Purposes     :      Draw ellipse normal.
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 xradius - > x radius of ellipse.
					 yradius - > y radius of ellipse.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawEllipse(INT16S x,INT16S y,INT16U xradius,INT16U yradius);
/*****************************************************************************
 Function Name:      DrvGraphicDrawEllipseFill
 Purposes     :      Draw ellipse with fill color.
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 xradius - > x radius of ellipse.
					 yradius - > y radius of ellipse.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicDrawEllipseFill(INT16S x,INT16S y,INT16U xradius,INT16U yradius);
/*****************************************************************************
 Function Name:      DrvGraphicPutImage
 Purposes     :      Put bitmap to graphic working buffer.
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 bitmap -> data whose first 2bytes is width ,the second 2 bytes is height.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPutImage(INT16S x,INT16S y,INT16U *bitmap);
/*****************************************************************************
 Function Name:      DrvGraphicGetImage
 Purposes     :      Get the data of working buffer.
 Parameter    :      left - > left coordinate of x.
				 	 top - > top coordinate of y.
					 right - > right coordinate of x.
				 	 bottom - > bottom coordinate of y.
				 	 bitmap -> pointer to storage data.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicGetImage(INT16S left,INT16S top,INT16S right,INT16S bottom,INT16U *bitmap);
/*****************************************************************************
 Function Name:      DrvGraphicGetImageWidth
 Purposes     :      Get the width of bitmap.
 Parameter    :
					 bitmap -> data whose first 2bytes is width ,the second 2 bytes is height.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicGetImageWidth(INT16U *bitmap);
/*****************************************************************************
 Function Name:      DrvGraphicGetImageHeight
 Purposes     :      Get the height of bitmap
 Parameter    :
					 bitmap -> data whose first 2bytes is width ,the second 2 bytes is height.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern INT16U DrvGraphicGetImageHeight(INT16U *bitmap);
/*****************************************************************************
 Function Name:      DrvGraphicPutChar
 Purposes     :      Draw one char to (x0,y0)
 Parameter    :
					 charcode -> local character code.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPutChar(INT16U charcode);
/*****************************************************************************
 Function Name:      DrvGraphicPutCharXy
 Purposes     :      Draw one char to (x,y)
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 charcode -> local character code.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicPutCharXy(INT16S x,INT16S y,INT16U charcode);
/*****************************************************************************
 Function Name:      DrvGraphicOutText
 Purposes     :      Draw strings  to (x0,y0)
 Parameter    :
					 str -> string pointer which will put to graphic working buffer.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicOutText(char *str);
/*****************************************************************************
 Function Name:      DrvGraphicOutText
 Purposes     :      Draw strings  to (x,y)
 Parameter    :      x - > coordinate of x.
				 	 y - > coordinate of y.
					 str -> string pointer which will put to graphic working buffer.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicOutTextXy(INT16S x,INT16S y,char* str);
/*****************************************************************************
 Function Name:      DrvGraphicOutText
 Purposes     :      Draw strings  to (x1 - x0,y1 -y0)area of working buffer.
 Parameter    :      x0 - > start coordinate of x0.
				 	 y0 - > start coordinate of y0.
				 	 x1 - > end coordinate of x0.
				 	 y1 - > end coordinate of y0.
					 str -> string pointer which will put to graphic working buffer.
 Return Value :
          			 Null.
 <History>
 Date                         Author                         Modification
 2008/09/24                   Lichuanyue                     Created
*******************************************************************************/
extern void DrvGraphicOutTextArea(INT16S x0,INT16S y0,INT16S x1,INT16S y1,char* str);

#endif

extern void CRC_cal(INT8U *pucBuf, INT32U len, INT8U *CRC);



/* Bin Switcher configuration API of parameter and flags */
extern int bin_switch_rs_offset_get(void);
extern void bin_status_set(INT32U bin_status);
extern INT32U bin_status_get(void);
extern void bin_compress_status_set(GPZP_DECOMPRESS_SWITCH decode_flag);
extern GPZP_DECOMPRESS_SWITCH bin_compress_status_get(void);
extern void bin_stg_plug_status_set(BIN_STG_PLUG plug_status);
extern BIN_STG_PLUG bin_stg_plug_status_get(void);
extern void bin_event_id_set(BIN_EVENT_ID bin_event_id);
extern BIN_EVENT_ID bin_event_id_get(void);
extern void bin_stg_plug_type_set(BIN_STG_PLUG_TYPE plug_status);
extern BIN_STG_PLUG_TYPE bin_stg_plug_type_get(void);
extern void bin_key_press_set(BIN_KEY_ID key_pressed_id);
extern BIN_KEY_ID bin_key_press_get(void);
extern void bin_customer_ap_flag_set(INT32U customer_ap_flag);
extern INT32U bin_customer_ap_flag_get(void);
extern void bin_rtc_counter_backup_set(INT32U couner_val);
extern INT32U bin_rtc_counter_backup_get(void);


/* power key process */

extern INT8U pwkey_press_get(void);
extern void pwkey_process_init(void);

/*tft adhustment tbale*/
#if (defined TFT_ADJUST_TABLE) && (TFT_ADJUST_TABLE == CUSTOM_ON)
 extern INT8U * tft_adjust_get_table(void);
#endif

//ID3 tag ///////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Definition
//=============================================================================
// Return values
#define C_ID3_RET_SUCCESS											0
#define C_ID3_RET_FAIL												(-1)
#define C_ID3_RET_INVALID_TAG									(-2)
#define C_ID3_RET_INVALID_SEARCH_ORDER				(-3)
#define C_ID3_RET_FRAME_NOT_FOUND							(-4)
#define C_ID3_RET_INVALID_FILE_HANDLE					(-5)
#define C_ID3_RET_INVALID_FIELD_TYPE					(-6)
#define C_IDE_RET_FILE_SIZE_TOO_SMALL					(-7)
#define C_ID3_RET_FILE_SEEK_FAIL							(-8)
#define C_ID3_RET_V1_ID_MISMATCH							(-9)
#define C_ID3_RET_INVALID_FRAME_ID						(-10)
#define C_ID3_RET_INVALID_FRAME_SIZE					(-11)
#define C_ID3_RET_INVALID_FRAME_CONTENT				(-12)
#define C_ID3_RET_V2_ID_MISMATCH							(-13)
#define C_ID3_RET_V2_UNSUPPORTED_VERSION			(-14)
#define C_ID3_RET_FILE_READ_FAIL							(-15)
#define C_ID3_RET_V2_TAG_NOT_EXIST						(-16)

// ID3 tag search order
#define C_ID3_SO_V2_FIRST	0	// Search v2 first. If none is found, search v1. This is the default.
#define C_ID3_SO_V1_FIRST	1	// Search v1 first. If none is found, search v2.
#define C_ID3_SO_V2_ONLY	2	// Only search ID3 tag v2
#define C_ID3_SO_V1_ONLY	3	// Only search ID3 tag v1
#define C_ID3_SO_MAX			C_ID3_SO_V1_ONLY

// ID3 tag version
#define C_ID3_V1							0x10
#define C_ID3_V2							0x20
#define C_ID3_V23							0x23
#define C_ID3_V24							0x24
#define C_ID3_UNKNOWN_VERSION	0xFF

// Predefined frame IDs
#define C_ID3_FID_TITLE		"TIT2"
#define C_ID3_FID_ARTIST	"TPE1"
#define C_ID3_FID_ALBUM		"TALB"
#define C_ID3_FID_YEAR		"TYER"
#define C_ID3_FID_COMMENT	"COMM"
#define C_ID3_FID_TRACK		"TRCK"
#define C_ID3_FID_GENRE		"TCON"
#define C_ID3_FID_APIC		"APIC"

// ID3v2
#define C_ID3_V2_FRAME_ID_LEN			4	// Length of a frame ID

//=============================================================================
// Data Type
//=============================================================================
typedef struct
{
	INT16S fd;
	INT32S file_size;
	INT8U search_order;
	struct
	{
		BOOLEAN is_exist;
	} id3_v1;
	
	struct
	{
		BOOLEAN is_exist;	// ID3v2 tag exists or not
		INT8U version;		// ID3v2.3 or ID3v2.4
		INT8U flags;			// flags in header
		INT32U tag_size;	// ID3v2 tag size
		INT32U extended_hdr_size;
		CHAR last_found_frame[C_ID3_V2_FRAME_ID_LEN];
		INT32S last_found_frame_offset;
	} id3_v2;
} ID3_Tag_T;

//=============================================================================
// API
//=============================================================================
INT32S id3_create(ID3_Tag_T *tag, INT16S fd);
INT32S id3_set_search_order(ID3_Tag_T *tag, INT8U order);
INT32S id3_get_frame_size(ID3_Tag_T *tag, CHAR *frame_id, INT32U *size, INT8U *version);
INT32S id3_get_frame_content(ID3_Tag_T *tag, CHAR *frame_id, INT8U *content, INT8U *version);
INT32U id3_get_v2_tag_size(ID3_Tag_T *tag);

/**************************************************************************/
// GPID.h
/**************************************************************************/
#define	RET_OK					( 0 )		//success
#define	RET_ERR					( -1 )		//failed

//Return code for GPID4
#define	C_GPID4_SUCCESS			(  0 )		//success
#define	C_GPID4_FAILURE			( -1 )		//failed
#define C_GPID4_NO_REF_DOT      ( -2 )
#define C_GPID4_ERR_GET_DATA    ( -3 )
#define C_GPID4_ERR_ECC_Check	( -4 )

#define C_OID_VERSION4

#define	C_IMAGE_WIDTH			( 144 )		//BMP Image Width size   
#define	C_IMAGE_HEIGHT			( 144 )		//BMP Image height size

//OID release memory use size
//	----------------
//	|               |
//	|               |		//OID process use memory
//	|               |
//	----------------
//	|               |		//OID extend memory
//	----------------

#define	C_GPID_MEMORY_EXTEND_H	( 3 )
#define	C_GPID_MEMORY_WIDTH			( C_IMAGE_WIDTH )
#define	C_GPID_MEMORY_HEIGHT		( C_IMAGE_HEIGHT + C_GPID_MEMORY_EXTEND_H )

INT16S GPID4_GetLuminance( INT16U * pGPIDDataBuf );
long GPID4_Decode( INT16U * pGPIDDataBuf );
void GPID4_GetDecodeResult( long * pGPIDresult );


/**************************************************************************/
// GpidApi.h
/**************************************************************************/
//======================================================
// Define
//======================================================
// The return value of API_Gpid3Service()
#define C_GPID_SERVICE_DECODE_DONE				(0)
#define C_GPID_SERVICE_IGNORE							(-1)
#define C_GPID_SERVICE_MINOR_DECODE_ERROR	(-2)
#define C_GPID_SERVICE_MAJOR_DECODE_ERROR	(-3)

// The values for Gpid3_state
#define C_GPID_STATE_START				0
#define C_GPID_STATE_WAIT_IMAGE		1
#define C_GPID_STATE_DECODE_DONE	2

// GPID code
#define C_GPID_INVALID_GPID_CODE	0xFFFFFFFF

// sensor frame end action
#define SENSOR_FEA_VIDEO_ENCODE			0
#define SENSOR_FEA_OID					1

//======================================================
// Function declaration
//======================================================
extern int API_GpidInitial(void);
extern int API_GpidSetBufFlag(unsigned int flag);
extern INT32S API_GpidService(INT32U *code);
extern void API_GpidSetType(INT8U type);
extern INT8U API_GpidGetType(void);
extern void ISR_ServiceGpid(void);
extern void SensorSleepDisable(void);
extern INT32S GPID_Decode(INT32U OIDMemory);
extern INT16U GPID_GetLuminance(INT32U Sourceaddr);
extern void GPID_GetDecodeResult(INT32U GPIDCodeAddr);

extern void user_defined_set_sensor_irq(INT32U enable);
extern void user_defined_set_sensor_fea(INT8U action);
extern void user_defined_video_codec_entrance(void);



/**************************************************************************/
// COMAIR_TX.h
/**************************************************************************/
#ifndef __COMAIR_TEST_H__
#define __COMAIR_TEST_H__

#define	F0	19126				/* Tone F0, frequency = 19126Hz */
#define	F1	19688				/* Tone F1, frequency = 19688Hz */
#define	F2	21374				/* Tone F2, frequency = 21374Hz */
#define	F3	20812				/* Tone F3, frequency = 20812Hz */
#define	F4	20250				/* Tone F4, frequency = 20250Hz */
#define	C_COMAIR_TIMERA_DATA	40000		/* Initial timer target(frequency) : 40KHz */
#define	F0_PRELOAD				0x10000 - (MCLK/2/F0)
#define	F1_PRELOAD				0x10000 - (MCLK/2/F1)
#define	F2_PRELOAD				0x10000 - (MCLK/2/F2)
#define	F3_PRELOAD				0x10000 - (MCLK/2/F3)
#define	F4_PRELOAD				0x10000 - (MCLK/2/F4)
#define	F_OFF_PRELOAD			0x10000 - (MCLK/2/C_COMAIR_TIMERA_DATA)
#define	C_COMAIR_IOTX_SEND_MAX_COUNT	163		/* 4096(Hz) * 0.04(s) = 163. Every 1/4096 sec will execute COMAIR_IOTX_Send() once, so 40ms will execute 163 times. */
#define	C_COMAIR_IOTX_RAMP_TIMER		3		/* In order to prevent generating beep sounds between each tones in one command, ramp up/down 3 preloads in 1/4096 sec. */

typedef enum {
	COMAIR_SEND_ERROR_NONE=0,
	COMAIR_SEND_ERROR_CMD_START_TO_SEND,
	COMAIR_SEND_ERROR_CMD_PROCESSING,
	COMAIR_SEND_ERROR_CMD_INVALID
} COMAIR_SEND_STATUS;

typedef enum {
	COMAIR_SEND_STANDBY=0,
	COMAIR_SEND_RAMPDOWN,
	COMAIR_SEND_RAMPUP,
	COMAIR_SEND_SENDING
} COMAIR_SEND_RESULT;

typedef enum {
	COMAIR_CMD_INVALID=0,
	COMAIR_CMD_VALID
} COMAIR_CMD_CHECK;

typedef enum {
	SINGLE_CMD=0,
	MULTI_CMD
} COMAIR_CMD_TYPE;

/* Global API */
void comair_tx_init(void);		/* Initialize COMAIR timers, pincode, and command table */
extern COMAIR_SEND_STATUS send_comair_cmd_single_cmd(INT8U cmd);	/* Send single command */
extern COMAIR_SEND_STATUS send_comair_cmd_multi_cmd(INT16U cmd);	/* Send multiple command */
extern void set_comair_pincode_cmd_gen(INT16U pin);	/* setup pincode and generate commands by pincode */
extern void start_comair_timer(void);		/* start timers which relate with COMAIR */
extern void stop_comair_timer(void);		/* stop sending command */
extern BOOLEAN get_comair_timer_status(void);


//=============================================================
//=============================================================
/* Local API */
void send_comair_command(void);	/* Start to send COMAIR commands */
COMAIR_CMD_CHECK cmd_validation(INT16U cmd);
//COMAIR_SEND_RESULT get_comair_status(void);		/* Return current play_status */
void comair_iotx_send_isr(void);
void comair_pincode_set(INT16U pin);
void comair_cmd_generator(INT8U command);
void comair_update_pwm_timer(void);

#endif	/*__COMAIR_TEST_H__ */
#endif 		// __GPLIB_H__
