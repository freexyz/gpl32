#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined CFC_EN) && (CFC_EN == 1)                             //
//================================================================//


#define CF_RW_RETRY_COUNT		3

#define C_DATA3_PIN       38//IOC[6]
#define C_DATA1_PIN       40//IOC[8]
#define C_DATA2_PIN       41//IOC[9]

INT32S CFC_Initial(void)
{
    gpio_write_io(C_DATA3_PIN, 0); 
    gpio_write_io(C_DATA1_PIN, 0); 
    gpio_write_io(C_DATA2_PIN, 0); 
	return cfc_initial();
}

INT32S CFC_Uninitial(void)
{
     cfc_uninit();
	 return 0;
}

void CFC_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors =  cfc_get_total_sector();
	info->nBytesPerSector = 512;
}

INT32S CFC_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;
	
	for(i = 0; i < CF_RW_RETRY_COUNT; i++)
	{
		ret = cfc_read_multi_sector(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
	return ret;
}

INT32S CFC_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;
	
	for(i = 0; i < CF_RW_RETRY_COUNT; i++)
	{
		ret = cfc_write_multi_sector(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
	return ret;
}

INT32S CFC_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_CFC_driver = {
	"CFC",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	CFC_Initial,
	CFC_Uninitial,
	CFC_GetDrvInfo,
	CFC_ReadSector,
	CFC_WriteSector,
	CFC_Flush,
};


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined CFC_EN) && (CFC_EN == 1)                        //
//================================================================//

