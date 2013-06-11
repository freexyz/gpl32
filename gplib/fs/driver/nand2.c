/*
* Purpose: Nand interface of File system 
*
* Author:
*
* Date: 2008/4/19
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
*
* History:
        0  V1.00 :
*       1. V1.01 : 1.Adding DrvNand_bchtable_alloc for bch table initial
                   and allocte memorys for bch tables
        2. v1.02 : modify nand flash initial                  
                   by wschung 2008/05/28 
*/

#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined NAND2_EN) && (NAND2_EN == 1)                         //
//================================================================//
extern int	bNandInit;

/************************************************************************/
/* NAND_Initial 
func:	
		对物理器件进行使能
		使用复位指令，使器件进行强制复位动作
		初始化 驱动层 的转换表

input:	void

output:	0		成功
		－1		失败
   
note:	初始化成功后，设置了一个全局变量 bNandInit 

                                                                  */
/************************************************************************/
INT32S NAND_Initial_2(void)
{
	INT16S ret;
    
	if(bNandInit == -1)
	{		
	   DrvNand_bchtable_alloc();
		ret = DrvNand_initial();	                         //L2 API 
		bNandInit = 0;
		if(ret!=0)
		{
			if (ret == 0xfff0) //ID not suppot
			{
				bNandInit = -1;
				//return ret;
			}
			else
			{
				DrvNand_lowlevelformat();
				ret = DrvNand_initial();
				if(ret!=0)
				{
					bNandInit = -1;
					//return ret;
				}		   
			}
		}	
		if ( bNandInit != 0)			
		{		
			DrvNand_bchtable_free();
		}
	}
	
	return 0;
}

INT32S NAND_Uninitial_2(void)
{
	if(bNandInit == 0)
	{
		DrvNand_flush_allblk();
		DrvNand_bchtable_free();
		bNandInit = -1;
	}
	//Nand_Uninit();
	return 0;
}

void NAND_GetDrvInfo_2(struct DrvInfo* info)
{
	info->nSectors = Nand_part1_Offset_Get();
	info->nBytesPerSector = 512;
}

INT32S NAND_ReadSector_2(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
	return DrvNand_read_sector(blkno + Nand_part1_Offset_Get(), blkcnt , buf); 
}

/************************************************************************/
/*		NAND_WriteSector
func:	往物理介质上写入数据
input:	
output:	0		成功
		其他值	失败

note:	增加了对电脑是否可以格式化的控制
		当定义了 _PC_UNABLE_FORMAT_UDISK 时，禁止了对逻辑盘 逻辑0 扇的写入动作
		PC 不能对U盘进行格式化  
                                                                     */
/************************************************************************/
INT32S NAND_WriteSector_2(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
	if(( Nand_part1_mode_Get() & DEVICE_WRITE_ALLOW ) == 0)
		return -1;
	return DrvNand_write_sector(blkno + Nand_part1_Offset_Get(), blkcnt, buf);	
}

INT32S NAND_Flush_2(void)
{
	return 0;
}

struct Drv_FileSystem FS_NAND2_driver = {
	"NAND" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	NAND_Initial_2,
	NAND_Uninitial_2,
	NAND_GetDrvInfo_2,
	NAND_ReadSector_2,
	NAND_WriteSector_2,
	NAND_Flush_2,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined NAND2_EN) && (NAND2_EN == 1)                    //
//================================================================//