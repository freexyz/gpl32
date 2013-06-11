/*
* Purpose: Nand Flash Read/Write modulers of USBD(MSDC)
*
* Author: wsf
*
* Date: 2008/10/28
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

#include "storages.h"
#include "drv_l2_nand_manage.h"
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined NAND_APP_EN) && (NAND_APP_EN == 1)                         //
//================================================================//




extern INT16U gNFHWInitFlag;
extern INT16U gAPPinitFlag;

static INT32S Check_APP_Init_Status();
INT32S unand_app_read_cmd_phase(INT32U lba,INT32U seccount);
INT32S unand_app_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_app_read_cmdend_phase(void);
INT32S unand_app_write_cmd_phase(INT32U lba,INT32U seccount);
INT32S unand_app_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_app_write_cmdend_phase(void);

static INT32U StartSectorNum;
static INT32U TotalSectorcount;
static INT32U BufferPtr;

#if 1
INT32S unand_app_read_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
	
  	//  sts = ms_multiread_cmd(lba,seccount);
 // /*
	StartSectorNum = lba;//记录起始sectornum，seccount由外部程序控制
	TotalSectorcount = seccount;//*/
	sts = 0;
	sts = Check_APP_Init_Status();
  return sts;
}
INT32S unand_app_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts =1;
// sts = ms_read_dma(buf,ifwait);

///*
	if (ifwait == 0)
	{
		BufferPtr = (INT32U)buf;	//记录buffer addr，为read做准备
	//	sts = DrvNand_read_sector(StartSectorNum + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum += seccount;
		sts = 0;
	}
	if (ifwait == 1)
	{
		//DEL sts = DrvNand_read_sector(StartSectorNum + Nand_Part0_Offset, seccount , (INT32U)buf);
		sts = NandBootReadSector((INT32U)StartSectorNum, seccount , (INT32U)buf);
		StartSectorNum += seccount;
	}
	if (ifwait == 2)
	{//此时buffer addr为null，使用ifwait为0时的buffer addr
		//DEL sts = DrvNand_read_sector(StartSectorNum + Nand_Part0_Offset, seccount , BufferPtr);
		sts = NandBootReadSector((INT32U)StartSectorNum, seccount , BufferPtr);
		StartSectorNum += seccount;
	}//*/
 return sts;
}
INT32S unand_app_read_cmdend_phase(void)
{
 INT32S sts =1;

// sts = ms_readwrite_end();
	sts = 0;

 return sts;
}

INT32S unand_app_write_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
 // sts = ms_multiwrite_cmd(lba,seccount);
 
   	//gFlushAPPType = 1;
   	NandBootEnableWrite();
	///*
	StartSectorNum = lba;//记录起始sectornum，seccount由外部程序控制
	TotalSectorcount = seccount;
		sts = 0;//*/
	sts = Check_APP_Init_Status();	
  return sts;
}


INT32S unand_app_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts =1 ;
// sts = ms_write_dma(buf,ifwait);
///*
	if (ifwait == 0)
	{
		BufferPtr = (INT32U)buf;	//记录buffer addr，为read做准备
	//	sts = DrvNand_read_sector(StartSectorNum + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum += seccount;
		sts = 0;
	}
	if (ifwait == 1)
	{
		//DEL sts = DrvNand_write_sector(StartSectorNum + Nand_Part0_Offset, seccount , (INT32U)buf);
		sts = NandBootWriteSector((INT32U)StartSectorNum, seccount , (INT32U)buf);
		StartSectorNum += seccount;
	}
	if (ifwait == 2)
	{//此时buffer addr为null，使用ifwait为0时的buffer addr
		//DEL sts = DrvNand_write_sector(StartSectorNum + Nand_Part0_Offset, seccount , BufferPtr);
		sts = NandBootWriteSector((INT32U)StartSectorNum, seccount , BufferPtr);
		StartSectorNum += seccount;
	}//*/

 return sts;
}

INT32S unand_app_write_cmdend_phase(void)
{
 INT32S sts =1;
 //sts = ms_readwrite_end();
 /*	*/
 NandBootDisableWrite();
 	sts = 0;
 
 return sts;
}


INT32S Check_APP_Init_Status()
{
 	INT32S sts =0;
	//HW init
	if(!gNFHWInitFlag)  //no initial ,so must be initial nand flash in this
	{
		sts = GetNandParam();  //HW nand flash initial
		if(sts)           //if HW nand flash initial fail return fail
			return sts; 
		gNFHWInitFlag = 1; //set HWInitFlag 
	}	

	//App Init
 	if(gAPPinitFlag != 1)
  	{		
  		if(!NandBootInit())
   		{
  			gAPPinitFlag = 1;
  			sts = 0;
   		}
   		else
   		{
   			sts = 0xffff;
   			//break;
   		}
  	}
  	
  return sts;
}

#endif

#if 0
INT32S unand_app_read_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
  //sts = ms_multiread_cmd(lba,seccount);
  return sts;
}
INT32S unand_app_read_dma_phase(INT32U *buf,INT8U ifwait)
{
 INT32S sts =1;
// sts = ms_read_dma(buf,ifwait);
 return sts;
}
INT32S unand_app_read_cmdend_phase(void)
{
 INT32S sts =1;
// sts = ms_readwrite_end();
 return sts;
}

INT32S unand_app_write_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
 // sts = ms_multiwrite_cmd(lba,seccount);
  return sts;
}
INT32S unand_app_write_dma_phase(INT32U *buf,INT8U ifwait)
{
 INT32S sts =1 ;
// sts = ms_write_dma(buf,ifwait);
 return sts;
}

INT32S unand_app_write_cmdend_phase(void)
{
 INT32S sts =1;
 //sts = ms_readwrite_end();
 return sts;
}
#endif

struct Usb_Storage_Access USBD_NF_APP_ACCESS = {
	unand_app_read_cmd_phase,
	unand_app_read_dma_phase,
	unand_app_read_cmdend_phase,
	unand_app_write_cmd_phase,
	unand_app_write_dma_phase,
	unand_app_write_cmdend_phase,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined NAND1_EN) && (NAND1_EN == 1)                    //
//================================================================//