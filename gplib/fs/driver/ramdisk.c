#define		CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined RAMDISK_EN) && (RAMDISK_EN == 1)                     //
//================================================================//

#define	C_RAMDISK_SIZE		(0x1000000)		/* allocate 16 MB SD RAM */
#define C_RAMDISK_SECTOR_NUM	(C_RAMDISK_SIZE >> 9)
INT8U	gRamDisk[C_RAMDISK_SIZE];

INT32S RAMDISK_Initial(void)
{
	 return 0;
}

INT32S RAMDISK_Uninitial(void)
{
	 return 0;
}

void RAMDISK_GetDrvInfo(struct DrvInfo *info)
{
	info->nSectors = C_RAMDISK_SECTOR_NUM;
	info->nBytesPerSector = 512;
}

INT32S RAMDISK_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	memcpy((void *) buf, gRamDisk + (blkno << 9), 512);
    return 0;    
}

INT32S RAMDISK_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	memcpy(gRamDisk + (blkno << 9), (void *) buf, 512);
    return 0;
}

INT32S RAMDISK_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_RAMDISK_driver = {
	"RAMDISK",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	RAMDISK_Initial,
	RAMDISK_Uninitial,
	RAMDISK_GetDrvInfo,
	RAMDISK_ReadSector,
	RAMDISK_WriteSector,
	RAMDISK_Flush,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined RAMDISK_EN) && (RAMDISK_EN == 1)                //
//================================================================//