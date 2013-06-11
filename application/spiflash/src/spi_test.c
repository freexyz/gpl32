#include "application.h"

char spi_w_buffer[256];
char spi_r_buffer[256];

void SPI_FLASH_TEST(void){

	INT8U spi_id=0;
	char* ptr_w = spi_w_buffer;
	char* ptr_r = spi_r_buffer;
	int i,j;
	int ret;
	
	for(i=0;i<256;i++)
		*ptr_w++ = i;

	for(i=0;i<256;i++)
		*ptr_r++ = 0;		

	gpio_drving_init_io(IO_C1,3);	
	gpio_drving_init_io(IO_C2,3);	
	gpio_drving_init_io(IO_C3,3);	

	
	SPI_Flash_init();
	SPI_Flash_readID(&spi_id);
	DBG_PRINT("SPI flash init OK, ID = %x \r\n", spi_id);

//	SPI_Flash_erase_chip();
	
	for(i=0;i<0x1000;i++){
		ret = SPI_Flash_read_page(i*256, (INT8U *)spi_r_buffer);
		DBG_PRINT("spi read  sector %d= %d \r\n",i ,ret);	
		
		SPI_Flash_erase_sector(i);
	
		ret = SPI_Flash_read_page(i*256, (INT8U *)spi_r_buffer);
		DBG_PRINT("spi read  sector %d= %d \r\n",i ,ret);			
		
		ret = SPI_Flash_write_page(i*256, (INT8U*)spi_w_buffer);
		DBG_PRINT("spi write  sector %d= %d \r\n",i ,ret);

		ret = SPI_Flash_read_page(i*256, (INT8U *)spi_r_buffer);
		DBG_PRINT("spi read  sector %d= %d \r\n",i ,ret);			
		
		ptr_r = spi_r_buffer;
		
		for(j=0;j<256;j++){
			if(j != *ptr_r){
				DBG_PRINT("Verification Error @ 0x%x",*ptr_r);
				while(1);
			}
			*ptr_r=0;
			ptr_r++;	
		}
		
	}

	DBG_PRINT("SPI FLASH TEST done!");		

}