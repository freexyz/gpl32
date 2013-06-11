#ifndef __DRV_L2_SD_H__
#define __DRV_L2_SD_H__

#include "driver_l2.h"

#define	C_MEDIA_MMC_CARD	            	0
#define	C_MEDIA_STANDARD_SD_CARD	 		1
#define C_MEDIA_SDHC_CARD              		2
#define C_MEDIA_MMCHC_CARD					3

#define C_SD_SECTOR_SIZE					512

#define C_RESP_R0							0		// No response is needed
#define C_RESP_R1							1
#define C_RESP_R2							2
#define C_RESP_R3							3
#define C_RESP_R6							6
#define C_RESP_R7							1
#define C_RESP_R1B							7

typedef enum {
	SD_CARD_STATE_INACTIVE = 0,
	SD_CARD_STATE_IDLE,
	SD_CARD_STATE_READY,
	SD_CARD_STATE_IDENTIFICATION,
	SD_CARD_STATE_STANDBY,
	SD_CARD_STATE_TRANSFER,
	SD_CARD_STATE_SENDING_DATA,
	SD_CARD_STATE_RECEIVE_DATA,
	SD_CARD_STATE_PROGRAMMING,
	SD_CARD_STATE_DISCONNECT
} SD_CARD_STATE_ENUM;


// Those definitions depend on command register of SD controller
#define C_SDC_CMD_WITH_DATA		  	0x00000100
#define C_SDC_CMD_WRITE_DATA		0x00000200
#define C_SDC_CMD_MULTI_BLOCK	  	0x00000400
#define C_SDC_CMD_RESP_R0			0x00000000
#define C_SDC_CMD_RESP_R1			0x00001000
#define C_SDC_CMD_RESP_R2			0x00002000
#define C_SDC_CMD_RESP_R3			0x00003000
#define C_SDC_CMD_RESP_R6			0x00006000
#define C_SDC_CMD_RESP_R7  			0x00001000
#define C_SDC_CMD_RESP_R1B		  	0x00007000

// Basic commands (class 0)
#define C_SD_CMD0					(0x00000000|C_SDC_CMD_RESP_R0)		// GO_IDLE_STATE
#define C_SD_CMD1			        (0x00000001|C_SDC_CMD_RESP_R3)		// reserved, use for MMC
#define C_SD_CMD2			        (0x00000002|C_SDC_CMD_RESP_R2)		// ALL_SEND_CID
#define C_SD_CMD3	              	(0x00000003|C_SDC_CMD_RESP_R6)		// SEND_RELATIVE_ADDR
#define C_SD_CMD4		            (0x00000004|C_SDC_CMD_RESP_R0)		// SEND_RELATIVE_ADDR ?
#define C_SD_CMD5	              	// reserved
#define C_SD_CMD6	              	(0x00000006|C_SDC_CMD_WITH_DATA|C_SDC_CMD_RESP_R1)		// SWITCH_FUNCTION
#define C_MMC_CMD6	              	(0x00000006|C_SDC_CMD_RESP_R1B)
#define C_SD_CMD7		            (0x00000007|C_SDC_CMD_RESP_R1B)	// SELECT/DESELECT_CARD ?
#define C_SD_CMD8	              	(0x00000008|C_SDC_CMD_RESP_R7)		// CHECK SD VERSION NUMBER ?
#define C_SD_CMD9	              	(0x00000009|C_SDC_CMD_RESP_R2)		// SEND_CSD ?
#define C_SD_CMD10					(0x0000000a|C_SDC_CMD_RESP_R2)		// SEND_CID ?
#define C_SD_CMD11					// reserved
#define C_SD_CMD12					(0x0000000c|C_SDC_CMD_RESP_R1)		// STOP_TRANSMISSION ?
#define C_SD_CMD13	            	(0x0000000d|C_SDC_CMD_RESP_R1)		// SEND_STATUS ?
#define C_SD_CMD14	            	// reserved
#define C_SD_CMD15	            	(0x0000000f|C_SDC_CMD_RESP_R0)		// GO_INACTIVE_STATE ?

// Block oriented read commands (class 2)
#define C_SD_CMD16					(0x00000010|C_SDC_CMD_RESP_R1)		// SET_BLOCKLEN
#define C_SD_CMD17	            	(0x00000011|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA)		// READ_SINGLE_BLOCK
#define C_SD_CMD18		          	(0x00000012|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_MULTI_BLOCK)		// READ_MULTIPLE_BLOCK
#define C_SD_CMD19					// reserved
#define C_SD_CMD20					// reserved
#define C_SD_CMD21					// reserved
#define C_SD_CMD22					// reserved
#define C_SD_CMD23					// reserved

// Block oriented write commands (class 4)
#define C_SD_CMD24					(0x00000018|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// WRITE_BLOCK
#define C_SD_CMD25					(0x00000019|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA|C_SDC_CMD_MULTI_BLOCK)	// WRITE_MULTIPLE_BLOCK
#define C_SD_CMD26					// reserved for Manfacturer
#define C_SD_CMD27					(0x0000001b|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// PROGRAM_CSD

// Erase commands (class 5)
#define C_SD_CMD32					(0x00000020|C_SDC_CMD_RESP_R1)		// ERASE_WR_BLK_START
#define C_SD_CMD33					(0x00000021|C_SDC_CMD_RESP_R1)		// ERASE_WR_BLK_END
#define C_SD_CMD34					// reserved
#define C_SD_CMD35					// reserved
#define C_SD_CMD36					// reserved
#define C_SD_CMD37					// reserved
#define C_SD_CMD38					(0x00000026|C_SDC_CMD_RESP_R1)		// ERASE
#define C_SD_CMD39					// reserved
#define C_SD_CMD40					// reserved
#define C_SD_CMD41					// reserved

// Block oriented write protection commands (class 6)
#define C_SD_CMD28					(0x0000001c|C_SDC_CMD_RESP_R1B)	// SET_WRITE_PROT
#define C_SD_CMD29					(0x0000001d|C_SDC_CMD_RESP_R1B)	// CLR_WRITE_PROT
#define C_SD_CMD30					(0x0000001e|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// SEND_WRITE_PROT
#define C_SD_CMD31					// reserved

// Lock card (class 7)
#define C_SD_CMD42					(0x0000002a|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// LOCK_UNLOCK
#define C_SD_CMD43					// reserved
#define C_SD_CMD44					// reserved
#define C_SD_CMD45					// reserved
#define C_SD_CMD46					// reserved
#define C_SD_CMD47					// reserved
#define C_SD_CMD48					// reserved
#define C_SD_CMD49					// reserved
#define C_SD_CMD50					// reserved
#define C_SD_CMD51					// reserved
#define C_SD_CMD52					// reserved
#define C_SD_CMD53					// reserved
#define C_SD_CMD54					// reserved

// SD Application specific commands (class 8)
#define C_SD_CMD55					(0x00000037|C_SDC_CMD_RESP_R1)		// APP_CMD
#define C_SD_CMD56					(0x00000038|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// GEN_CMD ?
#define C_SD_CMD57					// reserved
#define C_SD_CMD58					// reserved
#define C_SD_CMD59					// reserved
#define C_SD_CMD60					// reserved for manfacturer
#define C_SD_CMD61					// reserved for manfacturer
#define C_SD_CMD62					// reserved for manfacturer
#define C_SD_CMD63					// reserved for manfacturer

// Application specific commands used/reserved only by SD memory card, not by MultiMediaCard.
// All the following ACMDs shall be preceded with APP_CMD command (CMD55).
#define C_SD_ACMD6					(0x00000006|C_SDC_CMD_RESP_R1)		// SET_BUS_WIDTH
#define C_SD_ACMD13					(0x0000000d|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA)	// SD_STATUS
#define C_SD_ACMD17					// reserved
#define C_SD_ACMD18					// reserved for SD security application
#define C_SD_ACMD19 				// reserved
#define C_SD_ACMD20					// reserved
#define C_SD_ACMD21					// reserved
#define C_SD_ACMD22					(0x00000016|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA|C_SDC_CMD_WRITE_DATA)		// SEND_NUM_WR_BLOCKS
#define C_SD_ACMD23					(0x00000017|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA)	// SET_WR_BLK_ERASE_COUNT
#define C_SD_ACMD24					// reserved
#define C_SD_ACMD25					// reserved for SD security application
#define C_SD_ACMD26					// reserved for SD security application
#define C_SD_ACMD38 				// reserved for SD security application
#define C_SD_ACMD39					// reserved
#define C_SD_ACMD40					// reserved
#define C_SD_ACMD41					(0x00000029|C_SDC_CMD_RESP_R3)		// SD_APP_OP_COND
#define C_SD_ACMD42  				(0x0000002a|C_SDC_CMD_RESP_R1)		// SET_CLR_CARD_DETECT
#define C_SD_ACMD43					// reserved for SD security application
#define C_SD_ACMD49					// reserved for SD security application
#define C_SD_ACMD51 				(0x00000033|C_SDC_CMD_RESP_R1|C_SDC_CMD_WITH_DATA)	//SEND_SCR

// For SD io setting
extern INT32S SD_OS_Init(void) ;
extern void SD_IO_Request(INT32U idx) ;
extern void SD_IO_Release(void) ;
// SD and MMC card access APIs for system
extern INT32S drvl2_sd_init(void);
extern void drvl2_sdc_card_info_get(SD_CARD_INFO_STRUCT *sd_info);
extern INT32S drvl2_sd_bus_clock_set(INT32U limit_speed);
extern INT8U drvl2_sdc_card_protect_get(void);			// Return 0 when writing to SD card is allowed
extern void drvl2_sdc_card_protect_set(INT8U value);	// value: 0=allow to write SD card. Other values=Not allow to write SD card
extern INT32S drvl2_sd_set_clock_ext(INT32U bus_speed);	//this function should be called after drvl2_sd_init(); 

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
	// SD and MMC card access APIs for system
	extern INT32S drvl2_sd1_init(void);
	extern void drvl2_sdc1_card_info_get(SD_CARD_INFO_STRUCT *sd_info);
	extern INT32S drvl2_sd1_bus_clock_set(INT32U limit_speed);
	extern INT8U drvl2_sdc1_card_protect_get(void);			// Return 0 when writing to SD card is allowed
	extern void drvl2_sdc1_card_protect_set(INT8U value);	// value: 0=allow to write SD card. Other values=Not allow to write SD card
	extern INT32S drvl2_sd1_set_clock_ext(INT32U bus_speed);	//this function should be called after drvl2_sd_init();
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

#endif		// __DRV_L2_SD_H__
