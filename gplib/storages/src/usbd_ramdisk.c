/*
* Purpose: RAM disk Read/Write funciton handler of USBD(MSDC)
*
* Author: Eugene Hsu
*
* Date: 2012/11/05
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
* Note : How to initialize RAM disk for USB MSDC device
*		1. application\task_umi\state_usb\src\ap_usb.c
*			-Add a RAM disk LUN => SetLuntpye(0,LUNTYPE_RAMDISK); in usb_msdc_state_initial()
*		2. project\gpl326xx_platform_demo\inc\gplib_cfg.h
*			-Change FS_DEV_MAX definition from 9 to 13 => #define FS_DEV_MAX  13
*			-Change RAMDISK_EN from 0 to 1 => #define RAMDISK_EN 1
*
*		3. Default RAM disk size is 16MB, please change USBD_RAMDISK_SIZE and C_RAMDISK_SIZE definition simultaneously to modify the disk size.
*			Where USBD_RAMDISK_SIZE is in gplib\storages\src\usbd_ramdisk.c
*				  C_RAMDISK_SIZE is in \gplib\fs\driver\ramdisk.c	
*/			

#include "string.h"
#include "storages.h"

#if (defined RAMDISK_EN) && (RAMDISK_EN == 1)

#define	USBD_RAMDISK_SIZE			(0x1000000)	/* This must be same as C_RAMDISK_SIZE definition in ramdisk.c(filesystem) */
#define	USBD_RAMDISK_SECTOR_NUM		(USBD_RAMDISK_SIZE >> 9)
#define SDRAM_OFFSET				0//(0x80000000)	/* to access physical SD memory area */

extern INT8U gRamDisk[];
INT32U ramcurlba;

INT32S usbd_ramdisk_read_cmd_phase(INT32U lba,INT32U seccount)
{
	//DBG_PRINT("Read command phase, lba %d, seccount %d, bufptr %p\r\n", lba, seccount, gRamDisk);
	ramcurlba = lba;
  	return 0;
}
INT32S usbd_ramdisk_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{

	if(ifwait == 0 || ifwait == 1)
	{
		if((ramcurlba + seccount) <= USBD_RAMDISK_SECTOR_NUM)
		{
			INT8U* ptr = (INT8U*)(gRamDisk + (ramcurlba << 9) + SDRAM_OFFSET);
			INT8U* bufptr = (INT8U*)((INT32U)buf + SDRAM_OFFSET);
			INT32U cnt = 512*seccount;
			
			//DBG_PRINT("Read DMA phase, ifwait = %d, curlba = %d, seccount = %d\r\n", ifwait, ramcurlba, seccount);
			/* only process ifwait == 0 and == 1 case */
			cache_invalid_range((INT32U)ptr, 512*seccount);
			memcpy((INT8S*)bufptr, (INT8S*)ptr, cnt);	
        	ramcurlba += seccount;
        	cache_drain_range((INT32U)bufptr, 512*seccount);
     		return 0;
        }
        else
        {
        	return -1;
    	}		
	}
	else
	{
		/* ifwait != 0 or 1 case, nothing to do now */
		return 0;
	}	
}

INT32S usbd_ramdisk_read_cmdend_phase(void)
{
	ramcurlba = 0;
	return 0;
}

INT32S usbd_ramdisk_write_cmd_phase(INT32U lba,INT32U seccount)
{
	//DBG_PRINT("Write command phase, lba %d, seccount %d\r\n", lba, seccount);
 	ramcurlba = lba;
    return 0;
}

INT32S usbd_ramdisk_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
    	    	
	if(ifwait == 0)
	{
		if((ramcurlba + seccount) <= USBD_RAMDISK_SECTOR_NUM)
		{
			INT8U* ptr = (INT8U*)(gRamDisk + (ramcurlba << 9) + SDRAM_OFFSET);
			INT8U* bufptr = (INT8U*)((INT32U)buf + SDRAM_OFFSET);
			INT32U cnt = 512*seccount;
				
	    	/* only process ifwait == 0 case */
	    	//DBG_PRINT("Write DMA phase, ramcurlba %d, sdram 0x%p,buf 0x%p \r\n", ramcurlba , ptr, buf);
	    	cache_invalid_range((INT32U)bufptr, 512*seccount);
	    	memcpy((INT8S*)ptr, (INT8S*)bufptr, cnt);			
			ramcurlba += seccount;
			cache_drain_range((INT32U)ptr, 512*seccount);
			return 0;
		}			
		else
		{
			return -1;
		}	
    }
    else
    {
    	/* for ifwait != 0 case, nothing to do here */
		return 0;
	}
}

INT32S usbd_ramdisk_write_cmdend_phase(void)
{
   ramcurlba = 0;
   return 0;
}


struct Usb_Storage_Access USBD_RAMDISK_ACCESS = {
	usbd_ramdisk_read_cmd_phase,
	usbd_ramdisk_read_dma_phase,
	usbd_ramdisk_read_cmdend_phase,
	usbd_ramdisk_write_cmd_phase,
	usbd_ramdisk_write_dma_phase,
	usbd_ramdisk_write_cmdend_phase,
};
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined SD_EN) && (SD_EN == 1)                          //
//================================================================//