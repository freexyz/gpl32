#ifndef	__DRIVERLAYER_H__
#define	__DRIVERLAYER_H__
//	write your header here

#define DEVICE_READ_ALLOW		0x1
#define DEVICE_WRITE_ALLOW		0x2

//===========================================================================
//Write your NAND FLASH partion define here
#define	NAND_PART0_OFFSET		0
#define	NAND_PART0_SIZE			0x3c000UL
#define	NAND_PART0_MODE			(DEVICE_READ_ALLOW | DEVICE_WRITE_ALLOW)

#define	NAND_PART1_OFFSET		0x3c000UL
#define	NAND_PART1_SIZE			0
#define	NAND_PART1_MODE			(DEVICE_READ_ALLOW | DEVICE_WRITE_ALLOW)

#define	NAND_PART2_OFFSET		0x3c000UL
#define	NAND_PART2_SIZE			0
#define	NAND_PART2_MODE			(DEVICE_READ_ALLOW | DEVICE_WRITE_ALLOW)

#define	UNITAB_SECTOR_OFFSET	0x3c000UL
#define	NAND_RESERVE_OFFSET		0x3c400UL

struct DrvInfo {
	INT32U nSectors;
	INT32U nBytesPerSector;
};

struct Drv_FileSystem {
	INT8U 	Name[8];
	INT8U 	Status;
    INT32S	(*Drv_Initial)(void);
    INT32S	(*Drv_Uninitial)(void);
    void 	(*Drv_GetDrvInfo)(struct DrvInfo* info);
    INT32S	(*Drv_ReadSector)(INT32U, INT32U, INT32U);
    INT32S	(*Drv_WriteSector)(INT32U,INT32U, INT32U);
    INT32S	(*Drv_Flush)(void);
    INT32S	(*Drv_Tbl_Sync)(void);  // for FAST Table sync solution
};

//extern const INT32U Nand_Reserve_Offset;

#ifndef	CREAT_DRIVERLAYER_STRUCT
extern struct Drv_FileSystem FS_MSC_driver;
extern struct Drv_FileSystem FS_SD_driver;
extern struct Drv_FileSystem FS_SD1_driver;
extern struct Drv_FileSystem FS_CFC_driver;
extern struct Drv_FileSystem FS_NAND_App_driver; //Added by Eric 2012/11/17 for Nand App area R/W
extern struct Drv_FileSystem FS_NAND_driver;
extern struct Drv_FileSystem FS_NAND2_driver;
extern struct Drv_FileSystem FS_NAND3_driver;
extern struct Drv_FileSystem FS_USB_driver;
extern struct Drv_FileSystem FS_USB2_driver;
extern struct Drv_FileSystem FS_USB3_driver;
extern struct Drv_FileSystem FS_USB4_driver;
extern struct Drv_FileSystem FS_RAMDISK_driver;
extern struct Drv_FileSystem FS_XD_driver;			//Daniel  	12/17
extern struct Drv_FileSystem FS_CDROM_driver;			//huanghe  	090602
extern struct Drv_FileSystem FS_SPI_FLASH_DISK_driver;
extern struct Drv_FileSystem FS_SD2_driver;
#endif		//end of '#ifndef	CREAT_DRIVERLAYER_STRUCT'

#endif		//end of '#ifndef	__DRIVERLAYER_H__'
