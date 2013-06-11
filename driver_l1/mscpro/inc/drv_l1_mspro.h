#ifndef __DRV_L1_MSPRO_H__
#define __DRV_L1_MSPRO_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#ifndef C_MS_TIMEOUT
#define	C_MS_TIMEOUT				0x0007ffff
#endif

extern INT32U MS_Page_Size;
extern INT32U MS_Block_Size;
extern INT32U MS_UsrDataBlk_Size;
extern INT32U MS_RWPage_Size;
extern INT32U MS_TotalPage_Size;
extern INT16U *g_LtP_table;

extern INT8U   ms_ifnextCmd ;
extern INT32U  ms_nextAddr ;
extern INT32U  ms_remainSector;
extern INT8U   ms_sendtotalCount;
extern INT8U   ms_sendsectorCount;
extern INT8S   msprodone;

extern INT32S ms_initail(void);
extern INT32S mspro_write_pages(INT32U addr ,INT32U * pBuf,INT32U count); //1 count as 512 bytes
extern INT32S mspro_read_pages(INT32U addr ,INT32U * pBuf,INT32U count);  //1 count as 512 bytes

extern INT32S ms_read_multi_pages(INT32U logaddr ,INT32U * pBuf,INT32U count);
extern INT32S ms_write_notpro (INT32U logaddr,INT32U *pBuf, INT32U count); //1 count as 512 bytes
extern INT32S ms_read_pages ( INT32U logaddr,INT32U *pBuf, INT32U count); //1 count as 512 bytes


extern INT32S msc_multiread_cmd(INT32U blkno, INT32U blkcnt);
extern INT32S msc_read_dam(INT32U *buf,INT8U ifwait);
extern INT32S msc_readwrite_end(void);
extern INT32S msc_write_dam(INT32U *buf,INT8U ifwait);
extern INT32S msc_multiwrite_cmd(INT32U blkno, INT32U blkcnt);

//for MS driver 


extern INT32S MS_INIT(INT32U *sw_rbuffer);//MS initail
extern INT32U MS_SET_RW_ADDR(INT32U wsize,INT32U waddr,INT32U rsize,INT32U raddr);
extern INT32U MS_READ_REG(INT32U rsize0,INT32U raddr0);// must be less than 4 bytes
extern INT32U MS_WRITE_REG(INT32U wsize0,INT32U waddr0,INT32U data0);// must be less than 4 bytes
extern INT32U MS_GET_INT(void);
extern INT32U MS_SET_CMD(INT32U ARGCMD);

extern INT32U MS_Page_Size;
extern INT32U MS_Block_Size;
extern INT32U MS_UsrDataBlk_Size;
extern INT32U MS_RWPage_Size;
extern INT32U MS_TotalPage_Size;
extern INT16U *g_LtP_table;

#endif  //__DRV_L1_MSPRO_H__