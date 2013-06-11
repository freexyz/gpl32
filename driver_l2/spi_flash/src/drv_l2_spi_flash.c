/*
* Purpose: spi nor flash driver
*
* Author: zhangzha
*
* Date: 2008/04/22
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.10
* History :
*         1 2008/4/22 zhangzha create
*          
*         2 2008/04/28 zhangzha Modify to call the new spi driver.  
*/

//Include files
#include "drv_l2_spi_flash.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_SPI) && (_DRV_L2_SPI == 1)                   //
//================================================================//


// Functions defined in this file
static void SPI_CS_INIT(void);
static void SPI_CS_HIGH(void);
static void SPI_CS_LOW(void);

static INT32S SPI_Flash_WriteEn(void);
static INT32S SPI_Flash_WriteDisable(void);
static INT32S SPI_Flash_WriteStatus(INT8U status);
static INT32S CheckStatus(void);

static INT8U SST_ManID[]={0xBF,0x8C};
static INT8U SPI_Man_Type =NULL;

#if _OPERATING_SYSTEM == 1
OS_EVENT *spi_sem;
#endif

INT32S SPI_Flash_init(void)
{
	spi_init(C_SPI_NUM);
	//spi_cs_by_internal_set(C_SPI_NUM,64,0);
	spi_cs_by_external_set(C_SPI_NUM);
	spi_clk_set(C_SPI_NUM, SYSCLK_4);	
	SPI_CS_INIT();	
	
#if _OPERATING_SYSTEM == 1
    spi_sem = OSSemCreate(1);
#endif
  
	return STATUS_OK;
}

void SPI_LOCK(void)
{
#if _OPERATING_SYSTEM == 1
	INT8U err;
	OSSemPend(spi_sem, 0, &err);
#endif
}

void SPI_UNLOCK(void)
{
#if _OPERATING_SYSTEM == 1
	OSSemPost(spi_sem);
#endif
}

// byte 1 is manufacturer ID(0xc2), byte 2 is memory type ID ,byte 3 is device ID
INT32S SPI_Flash_readID(INT8U* Id)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[1];
	INT8U   i;
	
	cmd[0] = 0x9f;

	//send command
	SPI_LOCK();
	
	SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
	if (ret != STATUS_OK) {		
		SPI_CS_HIGH();
		SPI_UNLOCK();
		return ret;
	}
	
	// read spi flash id
	ret = spi_transceive_cpu(C_SPI_NUM, Id, 3, Id, 3);
	SPI_CS_HIGH();
	
	SPI_Man_Type =MX; // default MX
	for(i=0;i<sizeof(SST_ManID);i++){ // SST SPI write comand dif from MX
		if(Id[0]==SST_ManID[i]){
			SPI_Flash_WriteStatus(0); // SST SPI need it 
			SPI_Man_Type =SST;
		}
	}
	// Judge SPI_Type
	
	SPI_UNLOCK();
	
	return ret;
}

// input the start address of sector
// a sector size is 4k Byte(0x1000)
INT32S SPI_Flash_erase_sector(INT32U addr)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[4];

	SPI_LOCK();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

	SPI_Flash_WriteEn();

	cmd[0] = 0x20;
	cmd[1] = ((addr>>16) & 0xFF);
	cmd[2] = ((addr>>8) & 0xFF);
	cmd[3] = (addr & 0xFF);
    
    //send command
    SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
	SPI_CS_HIGH();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}
	SPI_UNLOCK();

	return ret;
}

// input the start address of block
// a block size is 64k Byte(0x10000)
INT32S SPI_Flash_erase_block(INT32U addr)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[4];

	SPI_LOCK();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

	SPI_Flash_WriteEn();

	cmd[0] = 0xd8;
	cmd[1] = ((addr>>16) & 0xFF);
	cmd[2] = ((addr>>8) & 0xFF);
	cmd[3] = (addr & 0xFF);
    
    //send command
    SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
	SPI_CS_HIGH();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

	SPI_UNLOCK();
	
	return ret;
}

INT32S SPI_Flash_erase_chip(void)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[1];

	SPI_LOCK();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

	SPI_Flash_WriteEn();

	cmd[0] = 0x60;
    
    //send command
	SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
	SPI_CS_HIGH();
	
	if (CheckStatus() != STATUS_OK) {		
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}
	
	SPI_UNLOCK();
	
	return ret;
}

// read a page data(256 Byte)
// the addr must be 256 byte allign(low 8 bit addr must be 0)
INT32S SPI_Flash_read_page(INT32U addr, INT8U *buf)
{
	INT32S	ret = STATUS_OK;
  	INT8U	cmd[4];
  	
  	SPI_LOCK();
  	
  	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

  	cmd[0] = 0x03;
    cmd[1] = ((addr>>16) & 0xFF);
    cmd[2] = ((addr>>8) & 0xFF);
    cmd[3] = (addr & 0xFF);
    
    //send command
    SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
	if (ret != STATUS_OK) {
  		SPI_CS_HIGH();
  		SPI_UNLOCK();
  		return ret;
  	}
	
	// read page
	ret = spi_transceive(C_SPI_NUM, buf, 256, buf, 256);
  	SPI_CS_HIGH();
  	
  	SPI_UNLOCK();
  	
  	return ret;
}

// write a page data(256 Byte)
// the addr must be 256 byte allign(low 8 bit addr must be 0)
INT32S SPI_Flash_write_page(INT32U addr, INT8U *buf)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[6];
	INT8U  *ptr;
	INT32S i;
	
	SPI_LOCK();
	
	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}
	
  	SPI_Flash_WriteEn();
  	
  	if(SPI_Man_Type==SST){//SST_Type
		
		ptr = buf;
  	
  		cmd[0] = 0xAD;
  		cmd[1] = ((addr>>16) & 0xFF);
  		cmd[2] = ((addr>>8) & 0xFF);
  		cmd[3] = (addr & 0xFF);
   		cmd[4] = *ptr++;
  		cmd[5] = *ptr++;
    
  		SPI_CS_LOW();
  		ret = spi_transceive_cpu(C_SPI_NUM,cmd, 6, cmd, 6);
  		SPI_CS_HIGH();
  		if (CheckStatus() != STATUS_OK) {
  			SPI_UNLOCK();
			return SPI_FLASH_TIMEOUT;
		}
	
		for (i=0;i<127;i++) {
			cmd[0] = 0xAD;
	  		cmd[1] = *ptr++;
	  		cmd[2] = *ptr++;
	  		SPI_CS_LOW(); 
	  		ret = spi_transceive_cpu(C_SPI_NUM,cmd, 3, cmd, 3);	
	  		SPI_CS_HIGH();
	  		if (CheckStatus() != STATUS_OK) {
				SPI_UNLOCK();
				return SPI_FLASH_TIMEOUT;
			}
		}	
		SPI_Flash_WriteDisable();//end of write
	} else {
  	//if(SPI_Man_Type==MX){ //MX_Type
	  	
	  	cmd[0] = 0x02;
	  	cmd[1] = ((addr>>16) & 0xFF);
	  	cmd[2] = ((addr>>8) & 0xFF);
	  	cmd[3] = (addr & 0xFF);
	    
	    //send command
	    SPI_CS_LOW();
		ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
		if (ret != STATUS_OK) {
	  		SPI_CS_HIGH();
	  		SPI_UNLOCK();
	  		return ret;
	  	}
		
		// write data
		ret = spi_transceive(C_SPI_NUM, buf, 256, buf, 0);	
		SPI_CS_HIGH();
		if (CheckStatus() != STATUS_OK) {
			SPI_UNLOCK();
			return SPI_FLASH_TIMEOUT;
		}
	}
	
	SPI_UNLOCK();
	
	return ret;
}

INT32S SPI_Flash_write_nByte(INT32U addr, INT8U *buf, INT16U BufCnt)
{
    INT32S ret = STATUS_OK;
    INT8U cmd[6];
    INT8U  *ptr;
    INT32S i;
    
    SPI_LOCK();
    
    if (CheckStatus() != STATUS_OK) {
     SPI_UNLOCK();
     return SPI_FLASH_TIMEOUT;
    }
    
    SPI_Flash_WriteEn();
    
    if(SPI_Man_Type==SST){//SST_Type
    
    ptr = buf;
    
    cmd[0] = 0xAD;
    cmd[1] = ((addr>>16) & 0xFF);
    cmd[2] = ((addr>>8) & 0xFF);
    cmd[3] = (addr & 0xFF);
    cmd[4] = *ptr++;
    cmd[5] = *ptr++;
    
    SPI_CS_LOW();
    ret = spi_transceive_cpu(C_SPI_NUM,cmd, 6, cmd, 6);
    SPI_CS_HIGH();
    if (CheckStatus() != STATUS_OK) {
      SPI_UNLOCK();
      return SPI_FLASH_TIMEOUT;
    }
 
   for (i=0;i<127;i++) {
      cmd[0] = 0xAD;
      cmd[1] = *ptr++;
      cmd[2] = *ptr++;
      SPI_CS_LOW(); 
      ret = spi_transceive_cpu(C_SPI_NUM,cmd, 3, cmd, 3); 
      SPI_CS_HIGH();
      if (CheckStatus() != STATUS_OK) {
        SPI_UNLOCK();
        return SPI_FLASH_TIMEOUT;
      }
   }
   SPI_Flash_WriteDisable();//end of write
    } else {
    //if(SPI_Man_Type==MX){ //MX_Type
    
    cmd[0] = 0x02;
    cmd[1] = ((addr>>16) & 0xFF);
    cmd[2] = ((addr>>8) & 0xFF);
    cmd[3] = (addr & 0xFF);
     
     //send command
     SPI_CS_LOW();
     ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
     if (ret != STATUS_OK) {
        SPI_CS_HIGH();
        SPI_UNLOCK();
        return ret;
       }
     
     // write data
     ret = spi_transceive(C_SPI_NUM, buf, BufCnt, buf, 0); 
     SPI_CS_HIGH();
     if (CheckStatus() != STATUS_OK) {
      SPI_UNLOCK();
      return SPI_FLASH_TIMEOUT;
     }
    }
    
    SPI_UNLOCK();
    
    return ret;
}

// read start from "addr", read "nByte" byte to "buf"
// the address can be any byte address
INT32S SPI_Flash_read(INT32U addr, INT8U *buf, INT32U nByte)
{
	INT32S	ret = STATUS_OK;
  	INT8U	cmd[4];
  	
  	SPI_LOCK();
  	
  	if (CheckStatus() != STATUS_OK) {
		SPI_UNLOCK();
		return SPI_FLASH_TIMEOUT;
	}

  	cmd[0] = 0x03;
    cmd[1] = ((addr>>16) & 0xFF);
    cmd[2] = ((addr>>8) & 0xFF);
    cmd[3] = (addr & 0xFF);
    
    //send command
    SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 4, cmd, 4);
	if (ret != STATUS_OK) {
  		SPI_CS_HIGH();
  		SPI_UNLOCK();
  		return ret;
  	}
	
	// read page
	ret = spi_transceive(C_SPI_NUM, buf, nByte, buf, nByte);
  	SPI_CS_HIGH();
  	
  	SPI_UNLOCK();
  	
  	return ret;
}

static INT32S SPI_Flash_WriteEn(void)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[1];
	
	cmd[0] = 0x06;
	
	//send command
	SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
	SPI_CS_HIGH();

	return ret;
}

static INT32S SPI_Flash_WriteDisable(void)
{
 	INT32S	ret = STATUS_OK;
	INT8U	cmd[1];
	
	cmd[0] = 0x06;
	
	//send command
 	cmd[0] = 0x04;
	SPI_CS_LOW();
    ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
    SPI_CS_HIGH();
    return ret;
}

static INT32S SPI_Flash_ReadStatus(void)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[1];
	
	cmd[0] = 0x05;
	
	//send command
	SPI_CS_LOW();
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
	if (ret != STATUS_OK) {		
		SPI_CS_HIGH();
		return ret;
	}
	
	// read status
	ret = spi_transceive_cpu(C_SPI_NUM, cmd, 1, cmd, 1);
	if (ret != STATUS_OK) {		
		SPI_CS_HIGH();
		return ret;
	}
	SPI_CS_HIGH();
	
	return (INT32S)cmd[0];
}

static INT32S SPI_Flash_WriteStatus(INT8U status)
{
	INT32S	ret = STATUS_OK;
	INT8U	cmd[2];
	
	SPI_Flash_WriteEn();
	
	cmd[0] = 0x01;
	cmd[1] = status;
	
	SPI_CS_LOW();
    ret = spi_transceive_cpu(C_SPI_NUM, cmd, 2, cmd, 2);
    if (ret != STATUS_OK) {		
		SPI_CS_HIGH();
		return ret;
	}
	SPI_CS_HIGH();
 	return ret;
}

#if 0
static void drv_us_wait(INT16U USEC)  
{
   INT32U usec = USEC*79;
   while (usec>10) {
  	usec-= 10 ;
  }
}
#endif
static INT32S CheckStatus(void)
{
	INT32U	i = 0;
	
  	while((SPI_Flash_ReadStatus() & SPI_FLASH_SR_WIP) != 0) {
  		drv_msec_wait(1);		
  		if(++i >= C_SPI_TIME_OUT) {
  			return SPI_FLASH_TIMEOUT;
  		}
  	}
  	return STATUS_OK;	
}

static void SPI_CS_INIT(void)
{
	//(*((volatile INT32U *) 0xD0200060)) &= (~0x1000);
	gpio_set_memcs(1,0); //cs1 set as IO
	gpio_init_io(C_SPI_CS_PIN,GPIO_OUTPUT);
	gpio_set_port_attribute(C_SPI_CS_PIN, 1);
	gpio_write_io(C_SPI_CS_PIN, 1);
}

static void SPI_CS_HIGH(void)
{
	gpio_write_io(C_SPI_CS_PIN, 1);
}
 
static void SPI_CS_LOW(void)
{
	gpio_write_io(C_SPI_CS_PIN, 0);
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_SPI) && (_DRV_L2_SPI == 1)              //
//================================================================//