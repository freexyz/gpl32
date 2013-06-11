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
#if (defined NAND1_EN) && (NAND1_EN == 1)                         //
//================================================================//
int	bNandInit = -1;

/************************************************************************/
/* NAND_Initial 
func:	
		��������������ʹ��
		ʹ�ø�λָ�ʹ��������ǿ�Ƹ�λ����
		��ʼ�� ������ ��ת����

input:	void

output:	0		�ɹ�
		��1		ʧ��
   
note:	��ʼ���ɹ���������һ��ȫ�ֱ��� bNandInit 

                                                                  */
/************************************************************************/
INT32S NAND_Initial(void)
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

INT32S NAND_Uninitial(void)
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

void NAND_GetDrvInfo(struct DrvInfo* info)
{
	
	CalculateFATArea();
	if(Nand_part0_size_Get() > (DrvNand_get_Size() - Nand_part0_Offset_Get())) 
	{    
            info->nSectors = DrvNand_get_Size() - Nand_part0_Offset_Get();
	} else {
            info->nSectors = Nand_part0_size_Get();
	}
	info->nBytesPerSector = 512;
}

INT32S NAND_ReadSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    INT16U  ret;

    NVRAM_OS_LOCK();
	ret = DrvNand_read_sector(blkno + Nand_part0_Offset_Get(), blkcnt , buf); 
    NVRAM_OS_UNLOCK();
    return ret;
}

/************************************************************************/
/*		NAND_WriteSector
func:	�����������д������
input:	
output:	0		�ɹ�
		����ֵ	ʧ��

note:	�����˶Ե����Ƿ���Ը�ʽ���Ŀ���
		�������� _PC_UNABLE_FORMAT_UDISK ʱ����ֹ�˶��߼��� �߼�0 �ȵ�д�붯��
		PC ���ܶ�U�̽��и�ʽ��  
                                                                     */
/************************************************************************/
INT32S NAND_WriteSector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
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

INT32S NAND_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_NAND_driver = {
	"NAND" ,
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
	NAND_Initial ,
	NAND_Uninitial ,
	NAND_GetDrvInfo ,
	NAND_ReadSector ,
	NAND_WriteSector ,
	NAND_Flush ,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined NAND1_EN) && (NAND1_EN == 1)                    //
//================================================================//