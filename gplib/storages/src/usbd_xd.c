/*
* Purpose: XD Read/Write modulers of USBD(MSDC)
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
#if (defined XD_EN) && (XD_EN == 1)                         //
//================================================================//
INT32S xdc_read_cmd_phase(INT32U lba,INT32U seccount);
INT32S xdc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S xdc_read_cmdend_phase(void);
INT32S xdc_write_cmd_phase(INT32U lba,INT32U seccount);
INT32S xdc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S xdc_write_cmdend_phase(void);

INT32U xdc_StartSectorNum;
INT32U xdc_TotalSectorcount;
INT32U xdc_BufferPtr;

#if 1
INT32S xdc_read_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts = 1;

  	//  sts = ms_multiread_cmd(lba,seccount);
 // /*
	  xdc_StartSectorNum = lba;//記錄起始sectornum，seccount由外部程序控制
	  xdc_TotalSectorcount = seccount;//*/
	  sts = 0;
	  
  return sts;
}
INT32S xdc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts = 1;
// sts = ms_read_dma(buf,ifwait);

///*
	if (ifwait == 0)
	{
		xdc_BufferPtr = (INT32U)buf;	//記錄buffer addr，為read做準備
	//	sts = DrvXD_read_sector(StartSectorNum + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum ++;
		sts = 0;
	}
	if (ifwait == 1)
	{
		sts = DrvXD_read_sector(xdc_StartSectorNum + 0, seccount , (INT32U)buf);
		xdc_StartSectorNum ++;
	}
	if (ifwait == 2)
	{//此時buffer addr為null，使用ifwait為0時的buffer addr
		sts = DrvXD_read_sector(xdc_StartSectorNum + 0, seccount , xdc_BufferPtr);
		xdc_StartSectorNum ++;
	}//*/
 return sts;
}
INT32S xdc_read_cmdend_phase(void)
{
 INT32S sts = 1;

// sts = ms_readwrite_end();
	sts = 0;

 return sts;
}

INT32S xdc_write_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts =1;
 // sts = ms_multiwrite_cmd(lba,seccount);
 
	///*
	xdc_StartSectorNum = lba;//記錄起始sectornum，seccount由外部程式控制
	xdc_TotalSectorcount = seccount;
	sts = 0;//*/
	
  return sts;
}
INT32S xdc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts =1 ;
// sts = ms_write_dma(buf,ifwait);

///*
	if (ifwait == 0)
	{
		xdc_BufferPtr = (INT32U)buf;	//記錄buffer addr，為read做準備
	//	sts = DrvXD_write_sector(StartSectorNum + Nand_Part0_Offset, 1 , (INT32U)buf);
	//	StartSectorNum ++;
		sts = 0;
	}
	if (ifwait == 1)
	{
		sts = DrvXD_write_sector(xdc_StartSectorNum + 0, seccount , (INT32U)buf);
		xdc_StartSectorNum ++;
	}
	if (ifwait == 2)
	{//此時buffer addr為null，使用ifwait為0時的buffer addr
		sts = DrvXD_write_sector(xdc_StartSectorNum + 0, seccount , xdc_BufferPtr);
		xdc_StartSectorNum ++;
	}//*/

 return sts;
}

INT32S xdc_write_cmdend_phase(void)
{
 INT32S sts = 1;
 //sts = ms_readwrite_end();
 /*	*/
 	//sts = 0;	//Daniel 04/20
 	sts = DrvXD_Flush_RAM_to_XDC();		//Daniel 04/20
 	
 return sts;
}
#endif

struct Usb_Storage_Access USBD_XDC_ACCESS = {
	xdc_read_cmd_phase,
	xdc_read_dma_phase,
	xdc_read_cmdend_phase,
	xdc_write_cmd_phase,
	xdc_write_dma_phase,
	xdc_write_cmdend_phase,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined XD_EN) && (XD_EN == 1)                          //
//================================================================//