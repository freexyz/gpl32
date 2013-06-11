#ifndef __AP_UMI_FIRMWARE_UPGRADE_H__
#define __AP_UMI_FIRMWARE_UPGRADE_H__

#include "turnkey_umi_task.h"


#define C_UPGRADE_BUFFER_SIZE		0x40000			// Must be multiple of 64K
#define C_UPGRADE_NAND_PAGE_SIZE	0x800			// Must be multiple of 2K
#define C_UPGRADE_SPI_BLOCK_SIZE	0x10000			// 64K
#define C_UPGRADE_SPI_WRITE_SIZE	0x100			// 256 bytes
#define C_UPGRADE_FAIL_RETRY		20				// Must larger than 0

extern void ap_umi_firmware_upgrade(void);


#endif		// __AP_UMI_FIRMWARE_UPGRADE_H__