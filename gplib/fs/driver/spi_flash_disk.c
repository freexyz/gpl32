/*
* Purpose: SPI flash interface of File system 
*
* Author:
*
* Date: 2009/6/15
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*
* History:
        0  V1.00 :
*/

#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if NOR_EN == 1                                                   //
//================================================================//

#if 0
INT16S g_nvram_fd;

INT32S spi_flash_disk_initial(void)
{
    g_nvram_fd = nv_open((INT8U*)"PHOTO.BIN");
    if(g_nvram_fd >= 0) return 0;
    return -1;
}

INT32S spi_flash_disk_uninitial(void)
{
    return 0;
}

void spi_flash_disk_get_drv_info(struct DrvInfo* info)
{
    info->nSectors = nv_rs_size_get(g_nvram_fd);
    info->nBytesPerSector = 512;
}

INT32S spi_flash_disk_read_sector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    INT16U  ret;

    nv_lseek(g_nvram_fd, blkno << 9, SEEK_SET);
    ret = nv_read(g_nvram_fd, buf, blkcnt << 9);
    return ret;
}

INT32S spi_flash_disk_write_sector(INT32U blkno , INT32U blkcnt ,  INT32U buf)
{
    return -1;
}

INT32S spi_flash_disk_flush(void)
{
    return 0;
}

#else


//#define SPIF_PART0_OFFSET   (0x200000 / 512)
//#define C_SPIFDISK_SIZE    (0x400000-(SPIF_PART0_OFFSET*512))
#define BLOCK_SIZE         4096
#define BLOCK_ADDR_SHIFT   12

#define C_EMPTY            0
#define C_VALID            1
#define C_MODIFIED         2

#define C_BUFFERSIZE       0x400  //4k

#define     S_Copy_Success          0x00
#define     S_Copy_Failed           0xff

struct Spi_Flash_instruction
{
 INT8U reserve;
 INT8U sector_num_index;                     //sector page0 所描述的是第几个sector(1~15)
 INT8U Copy_to_Block15_Success;              //copy到Block15成功
 INT8U Copy_Success;                         //copy到正常位置成功
 INT32U Copy_to_Sector;                      //copy到哪一个sector位置
}SF_Index;

typedef struct 
{
    INT32U Flag; 
    INT32U BlkNo;   //the block number
    INT32U Buffer[C_BUFFERSIZE];
   }SPI_FLASH_BUFFER_STRUCT;

//static INT16U gSFEnable = 0;
SPI_FLASH_BUFFER_STRUCT  spi_flash_buffer_struct;
INT32U spi_flash_part0_offset;
INT32U spi_flash_size;
INT16S SF_Write_To_Backup_Sector(INT32U Addr, INT8U *pBuf);
extern INT32S SPI_Flash_write_nByte(INT32U addr, INT8U *buf, INT16U BufCnt);

#if 0//_OPERATING_SYSTEM != _OS_NONE
OS_EVENT *gSPIF_sem = 0;
#endif

static void SPIF_SEM_INIT(void)
{
#if 0//_OPERATING_SYSTEM != _OS_NONE
    if(gSPIF_sem == 0)
     {
      gSPIF_sem = OSSemCreate(1);
     }
#endif
}

static void SPIF_LOCK(void)
{
#if 0//_OPERATING_SYSTEM != _OS_NONE
    INT8U err = NULL;
    
    OSSemPend(gSPIF_sem, 0, &err);
#endif
}

static void SPIF_UNLOCK(void)
{
#if 0//_OPERATING_SYSTEM != _OS_NONE
    OSSemPost(gSPIF_sem);
#endif
}

// read 4kB
INT32S driver_l2_SPIF_read(INT32U blkno, INT8U *buf)
{
    return SPI_Flash_read(blkno << BLOCK_ADDR_SHIFT,(INT8U *)buf, BLOCK_SIZE);
}

//=======================================================================
// WRITE 4kB
//=======================================================================
INT32S driver_l2_SPIF_write(INT32U blkno, INT8U *buf)
{
    INT32U addr;
    INT32U i;
    INT32S ret;
    
    addr = blkno << BLOCK_ADDR_SHIFT;
    ret = SPI_Flash_erase_sector(addr);
    if(ret)
     {
      return -1;
     }
    for(i = 0; i < BLOCK_SIZE / 256; i++)
     {
      ret = SPI_Flash_write_page(addr, buf);
      if(ret)
       {
        return -2;
       }
      addr += 256;
      buf += 256;
     }
    return 0;
}

INT32S spi_flash_buffer_flush(void)
{
    INT32S ret;
    INT32U Information_Address = 0x000000;
    
    if(spi_flash_buffer_struct.Flag == (C_VALID + C_MODIFIED))
     {
      SF_Write_To_Backup_Sector(spi_flash_buffer_struct.BlkNo, (INT8U*)spi_flash_buffer_struct.Buffer);
      ret = driver_l2_SPIF_write(spi_flash_buffer_struct.BlkNo, (INT8U*)spi_flash_buffer_struct.Buffer);
      if(ret != 0)
       {
        return ret;
       }
      SF_Index.Copy_Success = S_Copy_Success;
      SPI_Flash_write_page((Information_Address+(SF_Index.sector_num_index<<8)), &SF_Index.reserve);
      if(SF_Index.sector_num_index >= 15)
       {
        SPI_Flash_erase_block(Information_Address);
       }
     }
    spi_flash_buffer_struct.Flag = C_EMPTY;
    return 0;
}

INT32S spi_flash_disk_read_sector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
     INT32S ret = 0;
     INT32U blk;
     INT16U cnt;
     DMA_STRUCT str_dma;
     
     if(blkno + blkcnt >= spi_flash_size - spi_flash_part0_offset)
      {
       DBG_PRINT("\t\t++++++++error+++++++++\r\n");
      }
     
     SPIF_LOCK();
     blkno += spi_flash_part0_offset;
     while(blkcnt)
     {
       blk = blkno >> 3;
       if((blkno & 0x7) || (blkcnt < 8))
        {
         if(!((spi_flash_buffer_struct.Flag & C_VALID) && (spi_flash_buffer_struct.BlkNo == blk) ))
          {
           ret = spi_flash_buffer_flush();
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }
           
           ret = driver_l2_SPIF_read(blk, (INT8U *)spi_flash_buffer_struct.Buffer); //这里来调用读取4kbyte data read
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }
           spi_flash_buffer_struct.Flag = C_VALID;
           spi_flash_buffer_struct.BlkNo = blk;
          }
           
         if((blkno & 7) + blkcnt < 8)
             cnt = blkcnt;
         else
             cnt = 8 - (blkno & 7);
         
         
         str_dma.s_addr = (INT32U)(spi_flash_buffer_struct.Buffer + ((blkno & 0x7) << 7));
         str_dma.t_addr = (INT32U)buf;
         str_dma.count =  cnt << 9;
         str_dma.width = 1;
         str_dma.timeout = 0;
         str_dma.notify = NULL;
         ret = dma_transfer_wait_ready(&str_dma);
         if(ret != 0)
          {
           return ret;
          }
        
         //gp_memcpy((INT8S *) buf,(INT8S *)(spi_flash_buffer_struct.Buffer + ((blkno & 7) << 7)), blkcnt << 9);
         
         blkcnt -= cnt;
         blkno += cnt;
         buf += (cnt << 9);
        }
       else
        {
         cnt = blkcnt >> 3;
         while(cnt)
          {
           ret = driver_l2_SPIF_read(blk, (INT8U*)buf);
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }
           blk += 1;
           cnt -= 1;
           buf += BLOCK_SIZE;
           blkcnt -= BLOCK_SIZE / 512;
           blkno += BLOCK_SIZE / 512;
          }
        }
     }
     
     SPIF_UNLOCK();
     return ret;
}

INT32S spi_flash_disk_write_sector(INT32U blkno, INT32U blkcnt, INT32U buf)
{
     INT16S ret = 0;
     INT32U blk;
     INT16U cnt;
     INT32U Information_Address = 0x000000;
     DMA_STRUCT str_dma;
     
     SPIF_LOCK();
     blkno += spi_flash_part0_offset;
     while(blkcnt)
      {
       blk = blkno >> 3;
       if((blkno & 0x7) || (blkcnt < 8))
        {
         if(!((spi_flash_buffer_struct.Flag & C_VALID) && (spi_flash_buffer_struct.BlkNo == blk)))
          {
           ret = spi_flash_buffer_flush();
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }
           ret = driver_l2_SPIF_read(blk, (INT8U *)spi_flash_buffer_struct.Buffer);
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }
           spi_flash_buffer_struct.Flag = C_VALID;
           spi_flash_buffer_struct.BlkNo = blk;
          }
         
         if((blkno & 7) + blkcnt < 8)
             cnt = blkcnt;
         else
             cnt = 8 - (blkno & 7);
         
         
         str_dma.s_addr = (INT32U)buf;
         str_dma.t_addr = (INT32U)(spi_flash_buffer_struct.Buffer + ((blkno & 0x7) << 7));
         str_dma.count =  cnt << 9;
         str_dma.width = 1;
         str_dma.timeout = 0;
         str_dma.notify = NULL;
         ret = dma_transfer_wait_ready(&str_dma);
         if(ret != 0)
          {
           return ret;
          }
        
         //gp_memcpy((INT8S *)(spi_flash_buffer_struct.Buffer + ((blkno & 7) << 7)),(INT8S *) buf, cnt << 9);
         spi_flash_buffer_struct.Flag |= C_MODIFIED;
         
         blkcnt -= cnt;
         blkno += cnt;
         buf += (cnt << 9);
        }
       else
        {
         cnt = blkcnt >> 3;
         while(cnt)
          {
           SF_Write_To_Backup_Sector(blk, (INT8U*)buf);
           ret = driver_l2_SPIF_write(blk, (INT8U*)buf);
           if(ret != 0)
            {
             SPIF_UNLOCK();
             return ret;
            }

           SF_Index.Copy_Success = S_Copy_Success;
           SPI_Flash_write_page((Information_Address+(SF_Index.sector_num_index<<8)), &SF_Index.reserve);
           if(SF_Index.sector_num_index >= 15)
            {
             SPI_Flash_erase_block(Information_Address);
            }

           blk += 1;
           cnt -= 1;
           buf += BLOCK_SIZE;
           blkcnt -= BLOCK_SIZE / 512;
           blkno += BLOCK_SIZE / 512;
          }
        }
      }
     SPIF_UNLOCK();
     return ret;
}

void SF_Index_initial(void)
{
 SF_Index.sector_num_index        = 1;
 SF_Index.Copy_to_Block15_Success = S_Copy_Failed;
 SF_Index.Copy_to_Sector          = S_Copy_Failed;
 SF_Index.Copy_Success            = S_Copy_Failed;
}

//==============================================================================
//function:SF_Write_To_Backup_Sector
//var.
//        pBuf:
//         LBA:
//uFSSectorCnt:
// SF_Index.sector_num_index的范围是 1--15
//==============================================================================
INT16S SF_Write_To_Backup_Sector(INT32U Addr, INT8U *pBuf)
{
 INT8U i;
 INT8U  Sector_Index[15];
 INT32U Information_Address = 0x000000;
 INT32U ulAddr;
 
 SPI_Flash_read(Information_Address, Sector_Index, 15);
 for(i=0; i<15; ++i)
  {
   if(Sector_Index[i]!=0)break;
  }
 if(i>=15)                               //在没有空间可写的情况下，清除整个Block15.
  {
   SPI_Flash_erase_block(Information_Address);
   memset(Sector_Index,0xff,15);
   i=0;
  }
 Sector_Index[i]=0;
 SF_Index.sector_num_index = i+1;
 SPI_Flash_write_nByte(Information_Address, Sector_Index, SF_Index.sector_num_index);
 ulAddr = Information_Address+(SF_Index.sector_num_index<<12);
 for (i=0; i<16; ++i)                    //4KB WorkBuffer = 16 * page buffer (256B)
  {
   SPI_Flash_write_page(ulAddr, (pBuf + (i<<8)));
   ulAddr += 256ul;
  }
 
 SF_Index.Copy_to_Block15_Success = S_Copy_Success;
 SF_Index.Copy_to_Sector          = Addr>>3;
 SF_Index.Copy_Success            = S_Copy_Failed;
 //
 // 从block15 sector0 page1开始写
 // 不写SF_Index.Copy_Success
 SPI_Flash_write_page((Information_Address+(SF_Index.sector_num_index<<8)), &SF_Index.reserve);
 return 0;
}

INT8U Power_Up_Data_Config(void)
{
 INT8U i;
 INT8U Add_Offset;
 INT8U Sector_Index[15]={0};
 INT8U gWorkBuffer[4096]={0};    //4096 bytes
 INT32U Information_Address = 0x000000;
 INT32U ulAddr;

 SPI_Flash_read(Information_Address, Sector_Index, 15);

 for(i=0; i<15; ++i)
  {
   if(Sector_Index[i]!=0)break;
  }
 if(i == 0)
  return 0;
 if(i >= 15)
  {
   SPI_Flash_erase_block(Information_Address);
   return 0;
  }
 Add_Offset = i;
 SPI_Flash_read((Information_Address+(Add_Offset<<8)),
                &SF_Index.reserve,
                5 );
 if(SF_Index.Copy_Success == S_Copy_Success)
  return 0;
 if(SF_Index.Copy_to_Block15_Success != S_Copy_Success)
  return 0;
 for(i=0;i<16;++i)
  {
   SPI_Flash_read((Information_Address + (Add_Offset<<12) + (i<<8)),
                  (gWorkBuffer + (i<<8)),
                  256 );
  }
 ulAddr = SF_Index.Copy_to_Sector << 15;
 ulAddr &= ~0x0ffful;
 SPI_Flash_erase_sector(ulAddr);
 for (i=0; i<16; ++i)                    //4KB WorkBuffer = 16 * page buffer (256B)
  {
   SPI_Flash_write_page(ulAddr, &gWorkBuffer[i<<8]);
   ulAddr += 256ul;
  }
 SF_Index.Copy_Success = S_Copy_Success;
 SPI_Flash_write_nByte((Information_Address+(Add_Offset<<8)),
                      &SF_Index.reserve,
                      5 );
 return 0;
}

void spi_flash_buffer_init(void)
{
    spi_flash_buffer_struct.Flag = C_EMPTY;
    spi_flash_buffer_struct.BlkNo = 0;
}

INT32S spi_flash_disk_initial(void)
{
     INT32S ret;
     INT8U manID;
     INT32U buffer[32 / 4];
     
     ret = SPI_Flash_init();
     if(ret != STATUS_OK)
      {
       return ret;
      }
     
     spi_clk_set(0, SYSCLK_128); /* spi clock = 48M/8/128 = 46.8k */
     SPI_Flash_readID((INT8U*)buffer);
     manID = (INT8U)buffer[0];
     if (manID == 0xd2) {
     }
     else if (manID == 0xc2) {
      spi_clk_set(0,SYSCLK_4);
     }
     else {
      spi_clk_set(0,SYSCLK_4);
     }
     
     ret = SPI_Flash_read(0, (INT8U*)buffer, 32);
     if(ret != STATUS_OK)
     {
      return ret;
     }
     
     spi_flash_part0_offset = 0x80;//(INT32U)((INT8U*)buffer)[0x17] * 65536 / 512;
     spi_flash_size = 0x780;//(INT32U)((INT8U*)buffer)[0x15] * 65536 / 512;
     if((spi_flash_part0_offset == 0) || (spi_flash_size == 0))
     {
      return STATUS_FAIL;
     }
     spi_flash_buffer_init();
     SF_Index_initial();
     Power_Up_Data_Config();
     SPIF_SEM_INIT();
     return STATUS_OK;
}

INT32S spi_flash_disk_uninitial(void)
{
	return 0;
}

void spi_flash_disk_get_drv_info(struct DrvInfo *info)
{
    info->nSectors = spi_flash_size - spi_flash_part0_offset - 128 * 5;//C_SPIFDISK_SIZE / 512;//0x8000;
    info->nBytesPerSector = 512;
}

INT32S spi_flash_disk_flush(void)
{
    INT32S ret;
    
    SPIF_LOCK();
    ret = spi_flash_buffer_flush();
    if(ret != 0)
     {
      DBG_PRINT("\t\t====spi flush fail!\r\n");
     }
    SPIF_UNLOCK();
    return ret;
}

#endif

struct Drv_FileSystem FS_SPI_FLASH_DISK_driver = {
    "SPINOR" ,
    DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
    spi_flash_disk_initial ,
    spi_flash_disk_uninitial ,
    spi_flash_disk_get_drv_info ,
    spi_flash_disk_read_sector ,
    spi_flash_disk_write_sector ,
    spi_flash_disk_flush ,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //((NOR_EN == 1)                                           //
//================================================================//