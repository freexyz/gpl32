#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined SD_EN) && (SD_EN == 1)                               //
//================================================================//

#define SD_RW_RETRY_COUNT		3

#define SDC_TAG0		0x73644750		// string: PGds
#define SDC_TAG1		0x64636373		// string: sccd
#define SDC_TAG2		0x7364			// string: ds
#define APP_TAG			0x5047			// string: GP

INT32U sd_fs_offset[2]; 

static INT32U sd_boot_parser_header(void) 
{ 
	INT32U buf[128]; 
	INT16U *u16_buf = (INT16U *)buf;
	INT16U boot_size;
	INT32U app_size = 0, data_start_addr = 0; 
	/* ----- Read boot header ----- */
	if( drvl2_sd_read( 0x00, buf, 1) !=0 )
		return 0;
	/* ----- check boot header tag ----- */ 
	if( (buf[0] != SDC_TAG0) || (buf[1] != SDC_TAG1) || ( u16_buf[4] != SDC_TAG2))
		return 0;
	/* ----- Get boot area size ----- */
	boot_size = u16_buf[8]; 
	if(boot_size == 0) 
		return 0; 
	/* ----- Read APP header ----- */
	if( drvl2_sd_read( boot_size, buf, 1 )!=0 )
		return 0;
	/* ----- Check APP header tag ----- */
	if( u16_buf[0] != APP_TAG )
		return 0;
	/* ----- Get APP area size ----- */
	app_size = (u16_buf[0x18]<<16) + u16_buf[0x17]; 
	if(app_size == 0) 
		return 0; 
	/* ----- Get file system area ----- */
	data_start_addr = boot_size + app_size;
	if( data_start_addr & 0xfff)
		data_start_addr += 0x1000; 

	return data_start_addr & ~0xfff; 
} 



INT32S SD_Initial(void)
{
	INT32S ret = drvl2_sd_init();
	
	sd_fs_offset[0] = 0;
	if(ret == 0 )
		sd_fs_offset[0] = sd_boot_parser_header();
	return ret;
}

INT32S SD_Uninitial(void)
{
     drvl2_sd_card_remove();
	 return 0;
}

void SD_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors = drvl2_sd_sector_number_get() - sd_fs_offset[0];
	info->nBytesPerSector = 512;
}

//read/write speed test function
INT32S SD_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd_read( blkno + sd_fs_offset[0], (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        //if (drvl2_sd_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd_write( blkno + sd_fs_offset[0], (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        if (drvl2_sd_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_SD_driver = {
	"SD",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	SD_Initial,
	SD_Uninitial,
	SD_GetDrvInfo,
	SD_ReadSector,
	SD_WriteSector,
	SD_Flush,
};
//=== SD 1 setting ===//
#if((SD_DUAL_SUPPORT==1)&&(MCU_VERSION>=GPL326XX_C))

INT32S SD1_Initial(void)
{
	return drvl2_sd1_init();
}

INT32S SD1_Uninitial(void)
{
     drvl2_sd1_card_remove();
	 return 0;
}

void SD1_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors = drvl2_sd1_sector_number_get();
	info->nBytesPerSector = 512;
}

//read/write speed test function
INT32S SD1_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd1_read(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        //if (drvl2_sd_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD1_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd1_write(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        if (drvl2_sd1_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD1_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_SD1_driver = {
	"SD1",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	SD1_Initial,
	SD1_Uninitial,
	SD1_GetDrvInfo,
	SD1_ReadSector,
	SD1_WriteSector,
	SD1_Flush,
};
#endif

//=== SD 2 setting ===//
#if((SD_DUAL_SUPPORT==1)&&((MCU_VERSION==GPL326XXB)||(MCU_VERSION==GP326XXXA)))

INT32S SD2_Initial(void)
{
	return drvl2_sd2_init();
}

INT32S SD2_Uninitial(void)
{
     drvl2_sd2_card_remove();
	 return 0;
}

void SD2_GetDrvInfo(struct DrvInfo* info)
{
	info->nSectors = drvl2_sd2_sector_number_get();
	info->nBytesPerSector = 512;
}

//read/write speed test function
INT32S SD2_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd2_read(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        //if (drvl2_sd_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD2_WriteSector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
	INT32S	ret;
	INT32S	i;

    if (fs_sd_ms_plug_out_flag_get()==1) {return 0xFFFFFFFF;}
	for(i = 0; i < SD_RW_RETRY_COUNT; i++)
	{
		ret = drvl2_sd2_write(blkno, (INT32U *) buf, blkcnt);
		if(ret == 0)
		{
			break;
		}
	}
  #if SUPPORT_STG_SUPER_PLUGOUT == 1
    if (ret!=0) 
    {
        if (drvl2_sd2_read(0, (INT32U *) buf, 1)!=0)
        {
            fs_sd_ms_plug_out_flag_en();
            DBG_PRINT ("============>SUPER PLUG OUT DETECTED<===========\r\n");
        }
    }
  #endif
	return ret;
}

INT32S SD2_Flush(void)
{
	return 0;
}

struct Drv_FileSystem FS_SD2_driver = {
	"SD2",
	DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW,
	SD2_Initial,
	SD2_Uninitial,
	SD2_GetDrvInfo,
	SD2_ReadSector,
	SD2_WriteSector,
	SD2_Flush,
};
#endif

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined SD_EN) && (SD_EN == 1)                          //
//================================================================//