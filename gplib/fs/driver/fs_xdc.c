/*
* Purpose: XDC interface of File system 
*
* Author:
*
* Date: 2008/4/19
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*
* History:
        0  V1.00 :
*/

#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined XD_EN) && (XD_EN == 1)                         //
//================================================================//

                                           
/************************************************************************/

INT32S XDC_Initial(void)
{
	INT16S ret;
	ret = DrvXD_initial(); 				//Daniel 12/17
				if(ret!=0)
   {          
		return -1;			//Daniel  01/09
   }
	return 0;
}


INT32S XDC_Uninitial(void)
{
	INT16S ret;
	ret = DrvXD_uninit();				//Daniel 12/17
	if(ret != 0)
        {          
		return -1;			//Daniel  01/09
        }
	return 0;
}

void XDC_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors = DrvXD_getNsectors();		//Daniel	01/08
	info->nBytesPerSector = 512;
}

INT32S XDC_ReadSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
	INT16S ret;
	ret = DrvXD_read_sector(blkno, blkcnt , buf);				//Daniel 03/19		//04/07
	if(ret != 0)
    {          
		return -1;			//Daniel  03/19
    }
	return 0; 
}

INT32S XDC_WriteSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
	INT16S ret;
	ret = DrvXD_write_sector(blkno, blkcnt, buf);				//Daniel 03/19		//04/07
	if(ret != 0)
    {          
		return -1;			//Daniel  03/19
    }
	return 0;	
}

INT32S XDC_Flush(void)
{
	INT16S ret;
	ret = DrvXD_Flush_RAM_to_XDC();				//Daniel 03/19
	if(ret != 0)
    {          
		return -1;			//Daniel  03/19
    }
	return 0;
}

struct Drv_FileSystem FS_XD_driver = {
	"XDC" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	XDC_Initial ,
	XDC_Uninitial ,
	XDC_GetDrvInfo ,
	XDC_ReadSector ,
	XDC_WriteSector ,
	XDC_Flush ,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined XD_EN) && (XD_EN == 1)                    //
//================================================================//