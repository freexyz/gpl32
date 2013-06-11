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

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined NAND2_EN) && (NAND2_EN == 1)                         //
//================================================================//

INT32S unand_read_cmd_phase_2(INT32U lba,INT32U seccount);
INT32S unand_read_dma_phase_2(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_read_cmdend_phase_2(void);
INT32S unand_write_cmd_phase_2(INT32U lba,INT32U seccount);
INT32S unand_write_dma_phase_2(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_write_cmdend_phase_2(void);

INT32U StartSectorNum_2;
INT32U TotalSectorcount_2;
INT32U BufferPtr_2;

#if 1
INT32S unand_read_cmd_phase_2(INT32U lba,INT32U seccount)
{
  INT32S sts =1;

  	//  sts = ms_multiread_cmd(lba,seccount);
 // /*
	  StartSectorNum_2 = lba;//记录起始sectornum，seccount由外部程序控制
	  TotalSectorcount_2 = seccount;//*/
	  sts = 0;
	  
  return sts;
}
INT32S unand_read_dma_phase_2(INT32U *buf,INT8U ifwait,INT32U seccount)
{
  INT32S sts =1;
// sts = ms_read_dma(buf,ifwait);

///*
	if (ifwait == 0)
	{
		BufferPtr_2 = (INT32U)buf;	//记录buffer addr，为read做准备
	//	sts = DrvNand_read_sector(StartSectorNum_2 + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum_2 += seccount;
		sts = 0;
	}
	if (ifwait == 1)
	{
		sts = DrvNand_read_sector(StartSectorNum_2 + Nand_part1_Offset_Get(), seccount , (INT32U)buf);
        StartSectorNum_2 += seccount;
	}
	if (ifwait == 2)
	{//此时buffer addr为null，使用ifwait为0时的buffer addr
		sts = DrvNand_read_sector(StartSectorNum_2 + Nand_part1_Offset_Get(), seccount , BufferPtr_2);
		StartSectorNum_2 += seccount;
	}//*/
 return sts;
}
INT32S unand_read_cmdend_phase_2(void)
{
 INT32S sts =1;

// sts = ms_readwrite_end();
	sts = 0;

 return sts;
}

INT32S unand_write_cmd_phase_2(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
 // sts = ms_multiwrite_cmd(lba,seccount);
 
	///*
	StartSectorNum_2 = lba;//记录起始sectornum，seccount由外部程序控制
	TotalSectorcount_2 = seccount;
	sts = 0;//*/
	
  return sts;
}
INT32S unand_write_dma_phase_2(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts =1 ;
// sts = ms_write_dma(buf,ifwait);

///*
	if (ifwait == 0)
	{
		BufferPtr_2 = (INT32U)buf;	//记录buffer addr，为read做准备
	//	sts = DrvNand_read_sector(StartSectorNum + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum_2 += seccount;
		sts = 0;
	}
	if (ifwait == 1)
	{
		sts = DrvNand_write_sector(StartSectorNum_2 + Nand_part1_Offset_Get(), seccount , (INT32U)buf);
		StartSectorNum_2 += seccount;
	}
	if (ifwait == 2)
	{//此时buffer addr为null，使用ifwait为0时的buffer addr
		sts = DrvNand_write_sector(StartSectorNum_2 + Nand_part1_Offset_Get(), seccount , BufferPtr_2);
		StartSectorNum_2 += seccount;
	}//*/

 return sts;
}

INT32S unand_write_cmdend_phase_2(void)
{
 INT32S sts =1;
 //sts = ms_readwrite_end();
 /*	*/
 	sts = 0;
 
 return sts;
}
#endif

#if 0
INT32S unand_read_cmd_phase_2(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
  //sts = ms_multiread_cmd(lba,seccount);
  return sts;
}
INT32S unand_read_dma_phase_2(INT32U *buf,INT8U ifwait)
{
 INT32S sts =1;
// sts = ms_read_dma(buf,ifwait);
 return sts;
}
INT32S unand_read_cmdend_phase_2(void)
{
 INT32S sts =1;
// sts = ms_readwrite_end();
 return sts;
}

INT32S unand_write_cmd_phase_2(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
 // sts = ms_multiwrite_cmd(lba,seccount);
  return sts;
}
INT32S unand_write_dma_phase_2(INT32U *buf,INT8U ifwait)
{
 INT32S sts =1 ;
// sts = ms_write_dma(buf,ifwait);
 return sts;
}

INT32S unand_write_cmdend_phase_2(void)
{
 INT32S sts =1;
 //sts = ms_readwrite_end();
 return sts;
}
#endif

struct Usb_Storage_Access USBD_NF_ACCESS_2 = {
	unand_read_cmd_phase_2,
	unand_read_dma_phase_2,
	unand_read_cmdend_phase_2,
	unand_write_cmd_phase_2,
	unand_write_dma_phase_2,
	unand_write_cmdend_phase_2,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined NAND1_EN) && (NAND1_EN == 1)                    //
//================================================================//