#ifndef __STORAGE_GPLIB_H__
#define __STORAGE_GPLIB_H__

#include "gplib.h"

#define  C_CARD_DECTECTION_PIN  64

//Embeded memory
#define  C_SPI_FLASH    0
#define  C_NAND_FLASH   1
#define  C_NOR_FLASH    2

#if (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE==DEVICE_SPI)
    #define  C_NV_FLASH_TYPE C_SPI_FLASH
#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE==DEVICE_NAND)
    #define  C_NV_FLASH_TYPE C_NAND_FLASH
#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE==C_NOR_FLASH)
    #define  C_NV_FLASH_TYPE C_NOR_FLASH
#else
    #define  C_NV_FLASH_TYPE C_SPI_FLASH  /* Module Default Device Config */
#endif

//SPI USER AREA
#if (C_NV_FLASH_TYPE == C_SPI_FLASH)
	#define USER_SPI_BLK 1
	#define USER_SPI_256_B   1
	#define USER_SPI_512_B   2
	#define USER_SPI_AREA_SIZE USER_SPI_512_B
#endif	



#define C_EMBEDED_STORAGE_SPI   1
#define C_EMBEDED_STORAGE_NAND  1

#if (defined NVRAM_MANAGER_FORMAT) && (NVRAM_MANAGER_FORMAT >= NVRAM_V2)
#ifndef _FLASHRSINDEX_DEF_
#define _FLASHRSINDEX_DEF_
typedef struct 
{
    //INT8U  tag;     // none
    INT8U  name[19]; //FILE NAME
    INT32U pos;     //Phyical address
    INT32U offset;  //ADD
    INT32U size;    //by sector
} FLASHRSINDEX;
#endif

#else
typedef struct 
{
    //INT8U  tag;     // none
    INT8U  name[8]; //FILE NAME
    INT32U pos;     //Phyical address
    INT32U offset;  //ADD
    INT32U size;    //by sector
} FLASHRSINDEX;
#endif


typedef struct {
   INT16U   debouce;       // 
   INT16U   noise_debouce;       // 
   INT16U   noise_flag;
} PIN_BOUCE, *pPIN_BOUCE;
////////////////////////////////////////////
//add by crf
typedef enum
{
	MSG_USBD_PLUG_IN = 0x8000,
	MSG_USBD_PLUG_OUT,
	MSG_USBCAM_PLUG_IN,
	MSG_USBD_INITIAL
}TASK_USB_ENUM;
/////////////////////////////////////////////


extern struct Usb_Storage_Access USBD_MS_ACCESS;
extern struct Usb_Storage_Access USBD_SD_ACCESS;
extern struct Usb_Storage_Access USBD_CFC_ACCESS;
extern struct Usb_Storage_Access USBD_NF_APP_ACCESS;
extern struct Usb_Storage_Access USBD_NF_ACCESS;
extern struct Usb_Storage_Access USBD_NF_ACCESS_2;
extern struct Usb_Storage_Access USBD_XDC_ACCESS;
extern struct Usb_Storage_Access USBD_SPI_ACCESS;
extern struct Usb_Storage_Access USBD_RAMDISK_ACCESS;

extern INT16U nv_open(INT8U *path);                                 
extern INT32U nv_read(INT16S fd, INT32U buf, INT32U size);          
extern INT32U nv_lseek(INT16S fd, INT32S foffset, INT16S origin);   
extern INT32S nv_fast_read(INT16S fd, INT8U* buf, INT32U byte_size);
extern INT32U nv_rs_size_get(INT16S fd); // jandy add to querry fd size

#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
extern BIN_STG_PLUG bin_stg_plug_get(void);
extern BIN_STG_PLUG_TYPE bin_stg_change_type_get(void);
extern void bin_stg_init(void);
#endif
extern INT32U combin_reg_data(INT8U *data, INT32S len);
extern INT16U nvpack_init(INT8U* nvpack_upper_name);
extern INT16S nvpack_uninit(void);
extern INT16U nvp_fopen(INT8U* inpack_upper_name);
extern INT32S nvp_fread(INT16U nvp_infd, INT32U pbuf, INT32U byte_size);
extern INT16S nvp_fclose(INT16U nvp_infd);
extern INT32U nvp_lseek(INT16U nvp_infd, INT32S foffset);
extern INT32U nvp_fsize_get(INT16U nvp_infd);
extern void NVRAM_OS_LOCK(void);
extern void NVRAM_OS_UNLOCK(void);
extern void nvram_os_init(void);
extern INT32S spi_flash_disk_initial(void);
#endif