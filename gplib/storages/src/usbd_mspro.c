/*
* Purpose: MS/MSPRO Read/Write modulers of USBD(MSDC)
*
* Author: wschung
*
* Date: 2008/09/09
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

#include "storages.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined MSC_EN) && (MSC_EN == 1)                             //
//================================================================//

INT32S umsc_read_cmd_phase(INT32U lba,INT32U seccount);
INT32S umsc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S umsc_read_cmdend_phase(void);
INT32S umsc_write_cmd_phase(INT32U lba,INT32U seccount);
INT32S umsc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S umsc_write_cmdend_phase(void);


INT32S umsc_read_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts;
  sts = ms_multiread_cmd(lba,seccount);
  return sts;
}
INT32S umsc_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts,i;
 if  (ifwait ==2)
 {
   sts = ms_read_dma(buf,ifwait);
 }
 else if (ifwait ==0)
 {
   if (seccount > 1)
   {
	   for (i=0;i<seccount-1; i++)
	   {
    	  sts = ms_read_dma(buf+(i*128),1);
	      if (sts!=0) return sts;
       }
   }   
   sts = ms_read_dma(buf+ ((seccount-1)*128),ifwait); 
  }
  else if (ifwait ==1)
  {
   for (i=0;i<seccount; i++)
   {
      sts = ms_read_dma(buf+(i*128),1);
      if (sts!=0) return sts;
   } 
  }
 return sts;
}
INT32S umsc_read_cmdend_phase(void)
{
 INT32S sts;
 sts = ms_readwrite_end();
 return sts;
}

INT32S umsc_write_cmd_phase(INT32U lba,INT32U seccount)
{
  INT32S sts;
  sts = ms_multiwrite_cmd(lba,seccount);
  return sts;
}
INT32S umsc_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 INT32S sts,i;
 if  (ifwait ==2)
 {
   sts = ms_write_dma(buf,ifwait);
 }
 else if (ifwait ==0)
 {
   if (seccount > 1)
   {
	   for (i=0;i<seccount-1; i++)
	   {
    	  sts = ms_write_dma(buf+(i*128),1);
	      if (sts!=0) return sts;
       }
   }   
   sts = ms_write_dma(buf+ ((seccount-1)*128),ifwait); 
  }
  else if (ifwait ==1)
  {
   for (i=0;i<seccount; i++)
   {
      sts = ms_write_dma(buf+(i*128),1);
      if (sts!=0) return sts;
   } 
  }
  return sts;
}

INT32S umsc_write_cmdend_phase(void)
{
 INT32S sts;
 sts = ms_readwrite_end();
 return sts;
}

struct Usb_Storage_Access USBD_MS_ACCESS = {
	umsc_read_cmd_phase,
	umsc_read_dma_phase,
	umsc_read_cmdend_phase,
	umsc_write_cmd_phase,
	umsc_write_dma_phase,
	umsc_write_cmdend_phase,
};
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined MSC_EN) && (MSC_EN == 1)                        //
//================================================================//