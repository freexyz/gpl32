#ifndef __TURNKEY_FILESRV_TASK_H__
#define __TURNKEY_FILESRV_TASK_H__
#include "application.h"

extern void filesrv_task_entry(void *p_arg);
extern INT32S FileSrvScanFileContinue(void);
extern void FileSrv_AVIAudio_Read(STAVIAUDIOBufConfig *para);
extern void FileSrv_AVIVideo_Read(INT32U JumpFrameNum);

//========================================================
//Function Name:WaitScanFile
//Syntax:		void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index)
//Purpose:		wait for search file
//Note:
//Parameters:   pstFNodeInfo	/* the point to file node information struct */
//				index			/* the file index you want to find */
//Return:
//=======================================================
extern void ScanFileWait(struct STFileNodeInfo *pstFNodeInfo, INT32S index);

#endif /*__TURNKEY_FILESRV_TASK_H__*/
