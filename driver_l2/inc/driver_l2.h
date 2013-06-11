#ifndef DRIVER_L2_H
#define DRIVER_L2_H

#include "driver_l1.h"
#include "driver_l2_cfg.h"

/*Nand Flash Driver */
#if (defined _DRV_L2_NAND) && (_DRV_L2_NAND==1)
	#include "./../nand_flash/inc/drv_l2_nand_ext.h"
#endif

// Driver layer functions
extern void drv_l2_init(void);

#if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC == 1)
    typedef struct
    {
        INT32S tm_sec;  /* 0-59 */
        INT32S tm_min;  /* 0-59 */
        INT32S tm_hour; /* 0-23 */
        INT32S tm_mday; /* 1-31 */
        INT32S tm_mon;  /* 1-12 */
        INT32S tm_year;
        INT32S tm_wday; /* 0-6 Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday */
    }EXTERNAL_RTC_TIME;

    typedef struct
    {
        INT32S tm_min;  	/* 0-59 */
        INT32S tm_hour; 	/* 0-23 */
        INT32S tm_weekday;	/* 0- 6 */	//0 means sunday.
    }EXTERNAL_RTC_ALRAM;

    extern void drv_l2_external_rtc_init(void);
    //==================================//
    //function: drv_l2_rtc_first_power_on_check
    //return: 1 means i2c first power on,
    //==================================//
    extern INT32S drv_l2_rtc_first_power_on_check(void);
    extern INT32S drv_l2_rtc_total_set(EXTERNAL_RTC_TIME *ptr);
    extern INT32S drv_l2_rtc_total_get(EXTERNAL_RTC_TIME *ptr);

    #if DRV_L2_EXT_ALAEM_EN
    extern INT32S drv_l2_rtc_alarm_time_set(EXTERNAL_RTC_ALRAM *ptr);
    extern INT32S drv_l2_rtc_alarm_time_get(EXTERNAL_RTC_ALRAM *ptr);
    #endif
#endif

// system control function
#define MAX_SYSTEM_TIMER		7
extern void sys_init_timer(void);
extern INT32S sys_set_timer(void* msg_qsend, void* msg_q_id, INT32U msg_id, INT32U timerid, INT32U interval);
extern INT32S sys_kill_timer(INT32U timerid);
extern INT32S sys_kill_all_timer(void);

extern INT32S sys_registe_timer_isr(void (*TimerIsrFunction)(void));
extern INT32S sys_release_timer_isr(void (*TimerIsrFunction)(void));
extern INT32S sys_128Hz_timer_init(void);
// ad key
#define AD_KEY_1   0
#define AD_KEY_2   1
#define AD_KEY_3   2
#define AD_KEY_4   3
#define AD_KEY_5   4
#define AD_KEY_6   5
#define AD_KEY_7   6
#define AD_KEY_8   7

typedef enum {
	RB_KEY_DOWN,
	RB_KEY_UP,
	RB_KEY_REPEAT,
	MAX_KEY_TYPES
}KEY_TYPES_ENUM;

#if (defined _PROJ_TYPE) && (_PROJ_TYPE == _PROJ_TURNKEY)
extern void turnkey_adkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
#endif

extern void ad_key_initial(void);
extern void ad_key_uninitial(void);
extern INT32U ad_key_get_key(void);
#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
extern INT32U adkey_press_get(INT32U key_mask);
extern INT32U adkey_repeat_get(INT32U key_mask);
#endif
extern void ad_key_set_repeat_time(INT8U start_repeat, INT8U repeat_count);
extern void ad_key_get_repeat_time(INT8U *start_repeat, INT8U *repeat_count);

/* USB Host extern function */

#define USBH_Err_Success		0
#define USBH_Err_Fail			-1
#define USBH_Err_ResetByUser	100
#define USBHPlugStatus_None		0
#define USBHPlugStatus_PlugIn	1
#define USBHPlugStatus_PlugOut	2
#define USBH_Err_NoMedia		3

typedef struct
{
	INT8U storage_id;
	INT8U plug_status;
} DRV_PLUG_STATUS_ST;

typedef enum
{
	DRV_FS_DEV_CF=0,
	DRV_FS_DEV_NAND,
	DRV_FS_DEV_SDMS,
	DRV_FS_DEV_USBH,
	DRV_FS_DEV_USBD,
	DRV_FS_DEV_MAX
}DRV_FS_DEV_ID_ENUM;

typedef enum
{
	DRV_PLGU_OUT,
	DRV_PLGU_IN
}DRV_STORAGE_PLUG_STATUS_ENUM;

//extern void   turnkey_usbh_detection_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
extern INT32U usbh_detection_init(void);
extern void usbh_isr(void);
extern INT8U  usbh_mount_status(void);

extern INT32S MSDC_Write2A(INT32U StartLBA, INT32U * StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32S MSDC_Read28(INT32U StartLBA, INT32U* StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32S DrvUSBHMSDCInitial(void);
extern INT32S DrvUSBHLUNInit(INT32U LUN);
extern INT32U DrvUSBHGetSectorSize(void);
extern INT32U DrvUSBHGetCurrent(void);
extern INT32S DrvUSBHMSDCUninitial(void);

//for XDC usage start						//Daniel 12/17
#ifndef __CS_COMPILER__
	PACKED typedef struct
	{
		INT8U  	wPageData[512];
		INT32U 	wReserved_Area;
		INT8U 	wDataStatus;
		INT8U 	wBlkStatus;
		INT16U 	wBlkAddr1;
		INT8U   wECC_2[3];
		INT16U 	wBlkAddr2;
		//INT8U 	wBlkAddr3;
		INT8U 	wECC_1[3];
	}XB;
#else
	typedef struct
	{
		INT8U  	wPageData[512];
		INT32U 	wReserved_Area;
		INT8U 	wDataStatus;
		INT8U 	wBlkStatus;
		INT16U 	wBlkAddr1;
		INT8U   wECC_2[3];
		INT16U 	wBlkAddr2;
		//INT8U 	wBlkAddr3;
		INT8U 	wECC_1[3];
	}PACKED XB;
#endif

typedef union{
	XB PageTag;
	INT8U WorkBuffer[528];
}TX;
//for XDC usage end

//IR
typedef enum {
	IR_KEY_DOWN_TYPE,
	IR_KEY_REPEAT_TYPE,
	MAX_IR_KEY_TYPES
}IR_KEY_TYPES_ENUM;


// XD extern define and funnctions
extern INT16U DrvXD_initial(void);
extern INT16U DrvXD_uninit(void);
extern INT32U DrvXD_getNsectors(void);
extern INT16U DrvXD_write_sector(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr);
extern INT16U DrvXD_read_sector(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);
extern INT16U DrvXD_Flush_RAM_to_XDC(void);		//Daniel 03/11



extern void drv_l2_ir_init(void);
extern INT32S drv_l2_getirkey(void);

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
extern void turnkey_irkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

#define IR_KEY_NONE_TYPE    MAX_IR_KEY_TYPES
#define KEY_CODE_NONE   0x0

extern IR_KEY_TYPES_ENUM bin_ir_press_status_get(void);
extern INT8U bin_ir_keycode_get(void);
#endif

// spi flash driver
extern INT32S SPI_Flash_init(void);
// byte 1 is manufacturer ID,byte 2 is memory type ID ,byte 3 is device ID
extern INT32S SPI_Flash_readID(INT8U* Id);
// sector earse(4k byte)
extern INT32S SPI_Flash_erase_sector(INT32U addr);
// block erase(64k byte)
extern INT32S SPI_Flash_erase_block(INT32U addr);
// chip erase
extern INT32S SPI_Flash_erase_chip(void);
// read a page(256 byte)
extern INT32S SPI_Flash_read_page(INT32U addr, INT8U *buf);
// write a page(256 byte)
extern INT32S SPI_Flash_write_page(INT32U addr, INT8U *buf);
// read n byte
extern INT32S SPI_Flash_read(INT32U addr, INT8U *buf, INT32U nByte);

// Key Scan driver
extern INT32U IO1, IO2, IO3, IO4, IO5, IO6, IO7, IO8;
extern void key_Scan(void);

extern void itouch_init(void);
extern void itouch_reduce_write (INT8U salve_reg_addr, INT8U write_data);
extern INT8U itouch_read_nBytes (INT8U slave_id, INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
extern INT8U itouch_reduce_read_nBytes (INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
extern INT8U itouch_version_get(void);
extern void itouch_sensitive_set(INT8U level);
extern INT8U itouch_sensitive_get(void);
extern void turnkey_itouch_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);


// SD and MMC card access APIs for system
typedef struct {
	INT32U csd[4];
	INT32U scr[2];
	INT32U cid[4];
	INT32U ocr;
	INT16U rca;
} SD_CARD_INFO_STRUCT;
extern INT32S drvl2_sd_init(void);
extern void drvl2_sdc_card_info_get(SD_CARD_INFO_STRUCT *sd_info);
extern INT32S drvl2_sd_bus_clock_set(INT32U limit_speed);
extern INT8U drvl2_sdc_card_protect_get(void);			// Return 0 when writing to SD card is allowed
extern void drvl2_sdc_card_protect_set(INT8U value);	// value: 0=allow to write SD card. Other values=Not allow to write SD card
// For SD io setting
extern INT32S SD_OS_Init(void) ;
extern void SD_IO_Request(INT32U idx) ;
extern void SD_IO_Release(void) ;
// SD and MMC card access APIs for file-system
extern INT32U drvl2_sd_sector_number_get(void);
extern INT32S drvl2_sd_read(INT32U start_sector, INT32U *buffer, INT32U sector_count);
extern INT32S drvl2_sd_write(INT32U start_sector, INT32U *buffer, INT32U sector_count);
extern INT32S drvl2_sd_card_remove(void);
// SD and MMC card access APIs for USB device
extern INT32S drvl2_sd_read_start(INT32U start_sector, INT32U sector_count);
extern INT32S drvl2_sd_read_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data read and return immediately, 1=start data read and return when done, 2=query read result
extern INT32S drvl2_sd_read_stop(void);
extern INT32S drvl2_sd_write_start(INT32U start_sector, INT32U sector_count);
extern INT32S drvl2_sd_write_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data write and return immediately, 1=start data write and return when done, 2=query write result
extern INT32S drvl2_sd_write_stop(void);
#if((SD_DUAL_SUPPORT==1)&&(MCU_VERSION>=GPL326XX_C))
	extern INT32S drvl2_sd1_init(void);
	extern void drvl2_sdc1_card_info_get(SD_CARD_INFO_STRUCT *sd_info);
	extern INT32S drvl2_sd1_bus_clock_set(INT32U limit_speed);
	extern INT8U drvl2_sdc1_card_protect_get(void);			// Return 0 when writing to SD card is allowed
	extern void drvl2_sdc1_card_protect_set(INT8U value);	// value: 0=allow to write SD card. Other values=Not allow to write SD card

	// SD and MMC card access APIs for file-system
	extern INT32U drvl2_sd1_sector_number_get(void);
	extern INT32S drvl2_sd1_read(INT32U start_sector, INT32U *buffer, INT32U sector_count);
	extern INT32S drvl2_sd1_write(INT32U start_sector, INT32U *buffer, INT32U sector_count);
	extern INT32S drvl2_sd1_card_remove(void);
	// SD and MMC card access APIs for USB device
	extern INT32S drvl2_sd1_read_start(INT32U start_sector, INT32U sector_count);
	extern INT32S drvl2_sd1_read_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data read and return immediately, 1=start data read and return when done, 2=query read result
	extern INT32S drvl2_sd1_read_stop(void);
	extern INT32S drvl2_sd1_write_start(INT32U start_sector, INT32U sector_count);
	extern INT32S drvl2_sd1_write_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data write and return immediately, 1=start data write and return when done, 2=query write result
	extern INT32S drvl2_sd1_write_stop(void);
#endif

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XXB)	&& (MCU_VERSION < GPL327XX)
	// SD and MMC card access APIs for system
	extern INT32S drvl2_sd2_init(void);
	extern void drvl2_sdc2_card_info_get(SD_CARD_INFO_STRUCT *sd_info);
	extern INT32S drvl2_sd2_bus_clock_set(INT32U limit_speed);
	extern INT8U drvl2_sdc2_card_protect_get(void);			// Return 0 when writing to SD card is allowed
	extern void drvl2_sdc2_card_protect_set(INT8U value);	// value: 0=allow to write SD card. Other values=Not allow to write SD card
	extern INT32S drvl2_sd2_set_clock_ext(INT32U bus_speed);	//this function should be called after drvl2_sd_init();
	// SD and MMC card access APIs for file-system
	extern INT32U drvl2_sd2_sector_number_get(void);
	extern INT32S drvl2_sd2_read(INT32U start_sector, INT32U *buffer, INT32U sector_count);
	extern INT32S drvl2_sd2_write(INT32U start_sector, INT32U *buffer, INT32U sector_count);
	extern INT32S drvl2_sd2_card_remove(void);
	// SD and MMC card access APIs for USB device
	extern INT32S drvl2_sd2_read_start(INT32U start_sector, INT32U sector_count);
	extern INT32S drvl2_sd2_read_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data read and return immediately, 1=start data read and return when done, 2=query read result
	extern INT32S drvl2_sd2_read_stop(void);
	extern INT32S drvl2_sd2_write_start(INT32U start_sector, INT32U sector_count);
	extern INT32S drvl2_sd2_write_sector(INT32U *buffer, INT32U sector_count, INT8U wait_flag);	// wait_flag: 0=start data write and return immediately, 1=start data write and return when done, 2=query write result
	extern INT32S drvl2_sd2_write_stop(void);
#endif

extern void rotate_sensor_initial(void);
extern void turnkey_rotate_sensor_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
extern INT32S rotate_sensor_status_get(void);

#if SUPPORT_AI_FM_MODE == CUSTOM_ON
//FM module
extern void FMRev_Tune(INT32U freq);
extern void FMRev_Initial(void);
extern void FM_SearchAll(void);
extern void FMRev_Exit(void);
extern INT32U FM_Current_Frequency_Get(void);
extern void FM_Current_Frequency_Set(INT32U Freq);
extern INT32U FM_Temp_Frequency_Get(void);
extern void FM_Temp_Frequency_Set(INT32U Freq);
extern void FMRev_Volume(INT32U vol);
extern void MUTE_FM_REV(void);
extern void UnMUTE_FM_REV(void);

extern BOOLEAN FMRev_SearchUP(INT32U freq);
extern BOOLEAN FMRev_SearchDOWN(INT32U freq);

extern INT32U FM_Auto_Scan_Temp_Frequency_Get(void);
extern void FM_Auto_Scan_Temp_Frequency_Set(INT32U Freq);

#endif	//#if SUPPORT_AI_FM_MODE == CUSTOM_ON

#endif		// __DRIVER_L2_H__
