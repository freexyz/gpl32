/*
* Purpose: Ms/MSpro interface of File system 
*
* Author:
*
* Date: 2008/5/07
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
*
* History:
        0  V1.00 :
*       1. V1.01 : 1.Adding ms_table_init for table initial
                   and allocte memorys for table
                   by wschung 2008/05/28 
*/
#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined MSC_EN) && (MSC_EN == 1)                             //
//================================================================//


#define MSPRO_RW_RETRY_COUNT		3

#define C_DATA3_PIN       38//IOC[6]
#define C_DATA1_PIN       40//IOC[8]
#define C_DATA2_PIN       41//IOC[9]

INT16U *buffer_ms =NULL;

INT32S MSP_Initial(void)
{
    gpio_write_io(C_DATA3_PIN, 0); 
    gpio_write_io(C_DATA1_PIN, 0); 
    gpio_write_io(C_DATA2_PIN, 0); 
    //buffer allocation H1 
    if (buffer_ms == NULL)
       buffer_ms = (INT16U*) gp_malloc(8192*2);
    if (buffer_ms == NULL)
    {
    	 return 1;
    }
    ms_table_init(buffer_ms);
	return ms_initial();
}

INT32S MSP_Uninitial(void)
{
    ms_uninitial();
    gp_free(buffer_ms);
    buffer_ms =NULL;
	return 0;
}

void MSP_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors =  drvmsc_gettotalsector();
	info->nBytesPerSector = 512;
}

INT32S MSP_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < MSPRO_RW_RETRY_COUNT; i++)
	{
		ret = ms_read_multi_sector(blkno, blkcnt, buf);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        if (ms_read_multi_sector(0, 1, buf)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>MSC SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}


INT32S MSP_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < MSPRO_RW_RETRY_COUNT; i++)
	{
		ret = ms_write_multi_sector(blkno, blkcnt, buf);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        if (ms_read_multi_sector(0, 1, buf)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>MSC SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif    
	return ret;
}

INT32S MSP_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_MSC_driver = {
	"MSPRO",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	MSP_Initial,
	MSP_Uninitial,
	MSP_GetDrvInfo,
	MSP_ReadSector,
	MSP_WriteSector,
	MSP_Flush,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined MSC_EN) && (MSC_EN == 1)                        //
//================================================================//