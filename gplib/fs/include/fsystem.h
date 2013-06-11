/*
 * file system all head File fsystem.h
 * Copyright (C) 2007 GeneralPlus
 *
 */

#ifndef __FSYSTEM_H__
#define __FSYSTEM_H__

#include	"gplib.h"
#include 	"fs.h"
#include	"portab.h"

#if MALLOC_USE == 1
	#include "ucBS.h"
#endif

#include	<string.h>
//#include	"vfs.h"
#include	"error.h"
#include	"buffer.h"
#include	"fs_driver.h"
#include	"device.h"
#include	"date.h"
#include	"cds.h"
#include	"dcb.h"
#include	"fat.h"
#include	"file.h"
#include	"fat32.h"
#include	"fnode.h"
#include	"dirmatch.h"
#include	"speedup.h"
#include	"format.h"

#include	"errors32.h"
#include	"filesys32.h"
#include	"unicode32.h"

#include	"proto.h"

#endif /* __FSYSTEM_H__ */
extern INT32U fs_sd_ms_plug_out_flag_get(void);
extern void fs_sd_ms_plug_out_flag_reset(void);
extern INT8U fs_usbh_plug_out_flag_get(void);
extern void fs_usbh_plug_out_flag_reset(void);

extern void fs_sd_ms_plug_out_flag_en(void);
extern INT32U fs_usbh_plug_out_flag_en(void);
BOOLEAN FAT16_Format(INT16S dsk, INT32U sector_offset,  INT32U sector_number, INT8U *buf);
BOOLEAN FAT32_Format(INT16S dsk, INT32U sector_offset,  INT32U sector_number, INT8U *buf);
BOOLEAN FAT16_sFormat(INT16S dsk, INT32U totalsectors, INT32U realsectors, INT8U *buf);
f_ppos fs_getnextfile(STDiskFindControl *pstDiskFindControl, INT16S dsk, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S speedup,INT32S (*filter)(INT8S *str));
f_ppos fs_get_first_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir,INT32S (*filter)(INT8S *str));
f_ppos fs_get_next_file_in_folder(STDiskFindControl *pstDiskFindControl, INT8S *path, INT8S *extname, struct sfn_info* f_info, INT16S attr, INT8S find_child_dir,INT32S (*filter)(INT8S *str));
INT16S GetFileNumInFolderEx(struct STFileNodeInfo *stFNodeInfo,INT32U *nFolderNum, INT32U *nFileNum);	// 获取当前folder个数以及file个数
BOOLEAN fcmp_lfnwild(const INT8S *match, const INT8S *dir);
INT16S get_dir_from_str(CHAR *in_str, CHAR *out_str);
INT16S fs_FastLoadFatTable(REG f_node_ptr fnp, INT16U *buffer);
INT16S fd32_build_volume_name(CHAR *Dest, CHAR *Source);

extern void fs_lfndel_step_start(void);
extern void fs_lfndel_step_end(void);
extern INT32S fs_lfndel_step(void);

extern INT32S unlink_step_run(void);
extern void unlink_step_reg(void);
extern void unlink_step_end(void);
/* end of file */
