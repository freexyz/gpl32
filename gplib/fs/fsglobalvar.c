#include "fsystem.h"

/*                                                                      */
/* Global variables                                                     */
/*                                                                      */
dmatch			sda_tmp_dm;					/* Temporary directory match buffer     */

struct dpb		*DPBp;					/* First drive Parameter Block          */
struct buffer	*firstbuf;		/* head of buffers linked list          */

struct f_node	f_nodes_array[MAX_FILE_NUM];
struct f_node	*f_nodes;  			/* pointer to the array        */


INT16S			default_drive;      			/* default drive for dos   */ 
struct dpb		DPB[MAX_DISK_NUM];
struct cds		CDS[MAX_DISK_NUM];

struct buffer	buffers[NUMBUFF];

INT16S	gFS_errno;

INT16U	gUnicodePage;				//当前使用的unicode page

struct nls_table	g_nls_table;

//disk find function var
STDiskFindControl	gstDiskFindControl;

/*
f_pos	g_cur_pos;
f_pos	g_cur_dir_pos;

INT16U	g_level;						//cur dir level
INT16S	find_begin_file;
INT16S	find_begin_dir;

STDirTrack	gstDirTrack;	//dir track

CLUSTER	g_sfn_cluster;
CLUSTER g_sfn_cluster_offset;
*/

//to reduce stack memery using
#if USE_GLOBAL_VAR == 1
	INT8S			gFS_path1[FD32_LFNPMAX];	//260
	INT8S			gFS_name1[FD32_LFNMAX];		//255
	tFatFind		gFS_sFindData;				//LongName 260
	tDirEntry		gFS_slots[LFN_FETCH_SLOTS];	//21 * sizeof(tDirEntry)
#endif

#if (defined FS_FORMAT_SPEEDUP) && (FS_FORMAT_SPEEDUP == 1)
#if FS_FORMAT_SPEEDUP_BUFFER_SOURCE == GLOBAL_BUFFER
	INT8U		gFS_format_speedup_buffer[C_FS_FORMAT_BUFFER_SIZE];
#endif
#endif