#ifndef __DRV_l2_SPI_FLASH_H__
#define __DRV_l2_SPI_FLASH_H__

#include "driver_l2.h"

// The spi flash is connect to spi 0
#define C_SPI_NUM					SPI_0			// use spi 0
#define C_SPI_CS_PIN				81//0x51			// IOF1(STDMEM_CS1)

#define C_SPI_TIME_OUT				3000			// time out count

#define SPI_FLASH_BUSY              -1
#define SPI_FLASH_TIMEOUT           -2

#define SPI_FLASH_SR_WIP            0x01

#define MX				            0x01
#define SST            				0x02

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


#endif	/*__drv_l1_SENSOR_H__*/