/*
* Description: This file provides APIs to load resource.
*
* Author: Neal Lu
*
* Date: 2008/12/05
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/
#include "ap_umi_resource_load.h"

#if (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SPI || RESOURCE_DEV_TYPE == DEVICE_NAND)
INT16U resource_handle;
#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SD)
INT16S resource_handle;
#endif

INT32S resource_init(void)
{
#if (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SPI || RESOURCE_DEV_TYPE == DEVICE_NAND)
	resource_handle=nv_open((INT8U *) "GPRS.PAK");
	if (resource_handle == 0xFFFF)
	{
		DBG_PRINT("Failed to open resource %s from SPI flash\r\n", "GPRS.PAK");
		return STATUS_FAIL;
	}

#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SD)

    OSTimeDly(5);
    _devicemount(FS_SD);

	resource_handle = open((char*) "C:\\GPRS.PAK", O_RDONLY);
	if (resource_handle < 0)
	{
		DBG_PRINT("Failed to open file %s from SD card\r\n", "GPRS.PAK");
		return STATUS_FAIL;
	}
#endif

	return STATUS_OK;

}



INT32S resource_read(INT32U offset_byte, INT8U *pbuf, INT32U byte_count)
{
#if (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SPI || RESOURCE_DEV_TYPE == DEVICE_NAND)
	nv_lseek(resource_handle, offset_byte, SEEK_SET);
	if (nv_read(resource_handle, (INT32U) pbuf, byte_count))
		{
			return STATUS_FAIL;
		}

#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SD)
	lseek(resource_handle, offset_byte, SEEK_SET);
	if (read(resource_handle, (INT32U) pbuf, byte_count) <= 0)
	{
		return STATUS_FAIL;
	}
#endif

	return STATUS_OK;
}



void resource_close(void)
{

#if (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SPI || RESOURCE_DEV_TYPE == DEVICE_NAND)

#elif (defined RESOURCE_DEV_TYPE) && (RESOURCE_DEV_TYPE == DEVICE_SD)

	close(resource_handle);

#endif

}



