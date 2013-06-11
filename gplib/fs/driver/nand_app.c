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
#include "drv_l2_nand_config.h"
#include "drv_l2_nand_manage.h"
#include "driver_l2.h"
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined NAND_APP_EN) && (NAND_APP_EN == 1)                         //
//================================================================//
//static int	gAPPinitFlag = -1;
INT16U gNFHWInitFlag = 0;
INT16U gAPPinitFlag = 0;

static INT32U GetNFConfig_Update(void);
INT32U VendorNFAPPFormat(void);

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
INT32S NAND_APP_Initial(void)
{
	INT16S ret;
	
	//HW init
	if(!gNFHWInitFlag)  //no initial ,so must be initial nand flash in this
	{
		ret = GetNandParam();  //HW nand flash initial 
		if(ret)			//if HW nand flash initial fail return fail
			return ret;
		gNFHWInitFlag = 1; //set HWInitFlag 
	}	
	
	//App init
	if(!gAPPinitFlag)
	{
	    GetNFConfig_Update();
		ret = NandBootInit();
		if(!ret)
			gAPPinitFlag = 1;
		else
		{
			//format app area
			VendorNFAPPFormat();
			ret = NandBootInit();
			if(ret!=0)
			{
				gAPPinitFlag = 0;
				DBG_PRINT("======NAND_App_Initial Fail====\r\n");
				return ret;
			}			
			
		}
		if(gAPPinitFlag == 1)
			DBG_PRINT ("Nand Flash APP init OK \r\n");
		else
			DBG_PRINT ("Nand Flash APP init Fail \r\n");	
	}
	
			
	DBG_PRINT("======NAND_APP_Initial success====\r\n");
	return 0;
}

static INT32U GetNFConfig_Update(void)
{
	INT32U ret;	
	ret = NandInfoAutoGet();
	DrvL1_Nand_Init();
	return ret;
}

INT32U VendorNFAPPFormat(void)
{
	INT16U wFormatType;
	INT16U wFormatStep, wFormatTotalSteps;
	INT32U dwRet;
	
	GetNFConfig_Update();
	
	GetNandParam();
	if(!gNandUsrInfo.uiAppSize)
	    wFormatTotalSteps = 0;
	else
    	wFormatTotalSteps = ((gNandUsrInfo.uiAppSize + gNandUsrInfo.uiAppSpareSize) >> 5) + 4; //clac how much 64 blk??

	NandBootEnableWrite();    //erase APP Area,enable write
	
	//DEL wFormatType = GetCMDValueEx(17);
	
	//DEL wFormatStep =  GetCMDValueEx(21);
	//DEL dwRet = NandBootFormat_UP(wFormatType,wFormatStep); 
	
	wFormatType = 0x10;
	for(wFormatStep = 0; wFormatStep < wFormatTotalSteps; wFormatStep++)	
		dwRet = NandBootFormat_UP(wFormatType,wFormatStep);
	
	if(dwRet == 0)
	{
		DBG_PRINT ("Nand Flash APP Format OK \r\n");
	}
	else
	{
		DBG_PRINT ("Nand Flash APP Format Fail \r\n");
		DBG_PRINT("Nand Flash APP Format Fail Return is %x\r\n", dwRet);
	}
	
	NandBootDisableWrite();	//disable write
	return dwRet;

}

INT32S NAND_APP_Uninitial(void)
{
	if(gAPPinitFlag == 1)
	{
		NandBootFlush();
		NandBootDisableWrite();
		gAPPinitFlag = 0;
	}
	DBG_PRINT("======NAND_APP_Uninitial success====\r\n");
	return 0;
}

void NAND_APP_GetDrvInfo(struct DrvInfo* info)
{
	
	//DEL CalculateFATArea();
	//DEL if(Nand_part0_size_Get() > (DrvNand_get_Size() - Nand_part0_Offset_Get())) 
	//DEL {    
    //DEL    info->nSectors = DrvNand_get_Size() - Nand_part0_Offset_Get();
	//DEL } else 
	//DEL {
    //DEL    info->nSectors = Nand_part0_size_Get();
	//DEL }

	GetNandParam();
	info->nSectors = gNandUsrInfo.uiAppSize<<gNandInfo.wBlk2Sector;
	info->nBytesPerSector = 512;

	
	DBG_PRINT("======NAND App Area nSectors = 0x%x====\r\n",info->nSectors);
}

INT32S NAND_APP_ReadSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    INT16U  ret;

    NVRAM_OS_LOCK();
	ret = DrvNand_read_sector(blkno + Nand_part0_Offset_Get(), blkcnt , buf); 
    NVRAM_OS_UNLOCK();
    return ret;
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
INT32S NAND_APP_WriteSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    INT16U  ret;

    NVRAM_OS_LOCK();
	if(( Nand_part0_mode_Get() & DEVICE_WRITE_ALLOW ) == 0)
    {   
        NVRAM_OS_UNLOCK();
		return -1;
    }

    ret = DrvNand_write_sector(blkno + Nand_part0_Offset_Get(), blkcnt, buf);	
    NVRAM_OS_UNLOCK();
	return ret;
}

INT32S NAND_APP_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_NAND_App_driver = {
	"NANDApp" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	NAND_APP_Initial ,
	NAND_APP_Uninitial ,
	NAND_APP_GetDrvInfo ,
	NAND_APP_ReadSector ,
	NAND_APP_WriteSector ,
	NAND_APP_Flush ,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined NAND1_EN) && (NAND1_EN == 1)                    //
//================================================================//