#ifndef __GPLIB_FS_BG_UNLINK_H__
#define __GPLIB_FS_BG_UNLINK_H__

#include "gplib.h"

extern INT32U FsBgUnlink_get_workmem_size(void);
extern INT32S FsBgUnlink_open(void* pWorkMem, INT32U prio);
extern INT32S FsBgUnlink_file(void* pWorkMem, CHAR *FileName, INT32U bOnceFlag, INT32U ms);
extern INT32S FsBgUnlink_close(void* pWorkMem);

#endif 		// __GPLIB_FS_BG_UNLINK_H__