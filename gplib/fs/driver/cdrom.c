#define		CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined CDROM_EN) && (CDROM_EN == 1)                     //
//================================================================//
#define CDROM_USE_DISK FS_SD
#define CDROM_ISO_PATH  "C:\\"
static char	cdrom_sd_path[32] = "C:\\test.iso";

INT16S FD_CDROM;

#if 0
INT32S CDROM_Initial(void)
{
	 
	 FD_CDROM=nv_open((INT8U *) "CD.ISO");
  	 if(FD_CDROM == 0xFFFF)
  	 {
  	 	return 0xffff;
  	 }
  	 else 
  	 {
  	 	return 0;
  	 }
	 
}

INT32S CDROM_Uninitial(void)
{
	 return 0;
}

void CDROM_GetDrvInfo(struct DrvInfo *info)
{
	info->nSectors = 0x8000;
	info->nBytesPerSector = 512;
}

INT32S CDROM_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	//memcpy((void *) buf, gRamDisk + (blkno << 9), 512);
	nv_lseek((INT16S)FD_CDROM,blkno*512,SEEK_SET);
  	nv_read((INT16S)FD_CDROM, buf, 512*blkcnt);
    return 0;    
}
#else

INT32S CDROM_Initial(void)
{    
    while(1)
    {
		if(_devicemount(CDROM_USE_DISK))					// Mount device
		{
			//DBG_PRINT("CDROM: Mount Disk Fail[%d]\r\n", CDROM_USE_DISK);	
#if CDROM_USE_DISK == FS_NAND1
            {
                INT32S  nRet;
    			nRet = _format(FS_NAND1, FAT32_Type); 
    			DrvNand_flush_allblk(); 
    			_deviceunmount(FS_NAND1); 
			}
#endif
		}
		else
		{
			//DBG_PRINT("CDROM: Mount Disk success[%d]\r\n", CDROM_USE_DISK);
			break;
		}
	}

    if(CDROM_USE_DISK == FS_SD)
    {
        FD_CDROM = open(cdrom_sd_path, O_RDONLY);	//open file handle
    }    

    if(FD_CDROM == 0xFFFF)
	{
		close(FD_CDROM);
		FD_CDROM = 0;
		//DBG_PRINT("CDROM: open cdrom file failed\r\n");
		return 0xffff;
	}
	else
	{
	    //DBG_PRINT("CDROM: open cdrom file successfully\r\n");
	    return 0;
	}
        
}

INT32S CDROM_Uninitial(void)
{
    if(FD_CDROM > 0)
    {
        close(FD_CDROM);
		FD_CDROM = 0;
    }
    return 0;
}

void CDROM_GetDrvInfo(struct DrvInfo *info)
{
    struct sfn_info file_info;
	sfn_stat(FD_CDROM, &file_info);

	info->nSectors = file_info.f_size >> 9;

	if(file_info.f_size & 0x01ff)
	    info->nSectors += 1;
	info->nBytesPerSector = 512;
}

INT32S CDROM_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
    lseek((INT16S)FD_CDROM, blkno*512, SEEK_SET);
    read((INT16S) FD_CDROM, buf, 512*blkcnt);
    return 0;    
}

#endif

INT32S CDROM_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	//memcpy(gRamDisk + (blkno << 9), (void *) buf, 512);
    return 0;
}

INT32S CDROM_Flush(void)
{
	return 0;
}

/* Eugene added, 20111223 */
INT32S SetCDROMISOName(char* name)
{
    if(gp_strlen((INT8S *)name) > (31 - gp_strlen((INT8S *)CDROM_ISO_PATH)))
        return -1;

    //sprintf(cdrom_sd_path, "C:\\%s", name);
	gp_strcpy((INT8S *)cdrom_sd_path, (INT8S *)CDROM_ISO_PATH);
	gp_strcat((INT8S *)cdrom_sd_path, (INT8S *)name);
    return 0;
}

struct Drv_FileSystem FS_CDROM_driver = {
	"CDROM",
	DEVICE_READ_ALLOW,
	CDROM_Initial,
	CDROM_Uninitial,
	CDROM_GetDrvInfo,
	CDROM_ReadSector,
	CDROM_WriteSector,
	CDROM_Flush,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined CDROM_EN) && (CDROM_EN == 1)                //
//================================================================//