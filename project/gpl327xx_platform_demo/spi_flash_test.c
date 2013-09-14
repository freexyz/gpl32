#include "application.h"
extern INT32S spi_flash_disk_initial(void);
extern INT8U Power_Up_Data_Config(void);
extern INT32S spi_flash_disk_write_sector(INT32U blkno, INT32U blkcnt, INT32U buf);
extern INT32S spi_flash_disk_read_sector(INT32U blkno, INT32U blkcnt, INT32U buf);

void SF_Test()
{
 INT32U ul;
 INT32U ulAddr;
 INT16U i, j;
 INT8U buf12[4096];
 INT8U buf13[4096];
 INT8U Flash_ID[2];
 INT8U Count=0;
 
 for (ul=0; ul<4096; ++ul) {
  buf12[ul] = ++Count;
  buf13[ul] = 0;
 }
 
 ulAddr = 0;
 j = 0;
 spi_flash_disk_initial();
 
 SPI_Flash_readID(Flash_ID);

 Power_Up_Data_Config();
 spi_flash_disk_write_sector(0, 13, (INT32U)buf12);
 spi_flash_disk_read_sector(0, 13, (INT32U)buf13);
 
 for (i=0; i<256; ++i) {
  if (buf13[i] != buf12[i])
   return;
 }
 return;
}