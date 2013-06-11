/*
* Purpose: NV RAM access
*
* Author: wschung
*
* Date: 2008/06/03
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 2.01
* History :
*         1 2008/06/06 Created by wschung.  v1.00
*         2 2009/01/13 Using 19 bytes as resoure file name and 4 bytes as file sector count   V2.00
*         3 2009/01/13 Adding GPL32_C for Nand flash supporting
*         4 2009/01/21 Adding NVRAM_V3 for Nand flash APP area supporting
*/
#include "storages.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===========//
#if (defined GPLIB_NVRAM_SPI_FLASH_EN) && (GPLIB_NVRAM_SPI_FLASH_EN == 1) //
//========================================================================//

#if 1//#ifndef NVRAM_MANAGER_FORMAT
#define NVRAM_V1    1
#define NVRAM_V2    2
#define NVRAM_V3    3
#define NVRAM_MANAGER_FORMAT NVRAM_V3
#endif



//Identification of each area
#define SPI_FLASH_TAG   "PGpssiipps"
#define NAND_FLASH_TAG  "PGandnandn"
#define NAND_FLASH_GP10_TAG  	"GPNandTag2"
#define RESOURCE_TAG    "GP"


#define HEADER_OFFSET_SEC_CNT       8
#define HEADER_BCH_BLK_CNT         16
#define HEADER_BOOT_BACKUP_CNT     19

//#define RS_TABLE_SIZE               200//83

#define BOOT_AREA_START			0
#define BOOT_AREA_SIZE			10
#define PHYSIC_BLOCK_START		(BOOT_AREA_START + BOOT_AREA_SIZE)


#if NVRAM_MANAGER_FORMAT == NVRAM_V3
extern INT32U NandBootWriteSector(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr); 
extern INT32U NandBootReadSector(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootFlush(void);
#endif

#if (defined GPLIB_FILE_SYSTEM_EN) && (GPLIB_FILE_SYSTEM_EN == 1) && _OPERATING_SYSTEM != _OS_NONE // jandy add
    extern OS_EVENT *gFS_sem;
    #define _NVNAND_OS_     1
#elif _OPERATING_SYSTEM != _OS_NONE
    OS_EVENT *gFS_sem;
    #define _NVNAND_OS_     1
#else
    #define _NVNAND_OS_     0
#endif

#if (defined APP_HEADER_V20) && (APP_HEADER_V20 == 1)
typedef enum 
{
    HDR_1Byte=1,
    HDR_CHAR=HDR_1Byte,
    HDR_2Bytes=2,
    HDR_SHORT=HDR_2Bytes,
    HDR_3Bytes=3,
    HDR_4Bytes=4,
    HDR_WORD=HDR_4Bytes
} HDR_VAL_LEN;

typedef enum
{
    BCH1K60_BITS_MODE=0x00,  // phy register define, fixed
    BCH1K40_BITS_MODE=0x01,  // phy register define, fixed
    BCH1K24_BITS_MODE=0x02,  // phy register define, fixed
    BCH1K16_BITS_MODE=0x03,  // phy register define, fixed
    BCH512B8_BITS_MODE=0x04,  // phy register define, fixed
    BCH512B4_BITS_MODE=0x05,  // phy register define, fixed
    BCH_NONE=0xFF
} BCHM_ENUM;

typedef struct {
    INT32U nv_tag;
    BCHM_ENUM bthdr_bch;
    INT16U bthdr_page_size;
    INT8U  bthdr_copies; // the copy times of Boot Header (1BTH = 1Page)
    BCHM_ENUM btldr_bch;
    INT8U  bthdr_page_sectors;
    INT16U btldr_page_size;
    INT8U  btldr_page_sectors;
    INT8U  btldr_copies;
    INT32U btldr_start_page_id;
    INT32U btldr_set_pages;
    INT16U block_size;
    INT16U addr_cycle;
    INT16U nand_type; 
    INT16U StrongEn;
    INT16U app_tag;
    INT32U app_start; // app start page id
    INT32U app_size; // app size (unit:sectors)
    INT32U app_perscent;  // app spare area percent rate
    INT32U app_rtcode_size;  // how many sectors of rt code size to run in DRAM
    INT32U app_rtcode_run_addr;
    INT8U  swfuse;
    INT16U total_MB_size;
    INT8U  nf_wp_pin;
}HEADER_NV_INFO;

typedef enum {
    RunTimeCode=0,
    GpResourcePart=1,
    FastBootBin=2,
    QuickImg=3,
    HibrateImg=4, 
    ImageFlag=5,
    CustomizeBtLdr=6
} APP_PART_TYPE;

typedef struct 
{
    INT32U AppTag;
    INT8U ver;
    INT8U DrvType;  // APP Driver Type
    INT16U HdrSize;
    INT32U TotalSizeSectors;
    INT16U PartNums;   
    INT8U opKeyPort;
    INT8U opKeyId;
    INT32U AreaCheckSum;
} APPHDR;

typedef struct 
{
    INT32U PartId;
    INT32U StartSectorId;
    INT32U PartImgSize;  // APP Driver Type
    APP_PART_TYPE PartType;
    INT8U  PartImgVer;
    INT32U DestinationAddr;
} APP_PART_INFO;

typedef struct 
{
    INT8U FileName[20];
    INT32U ImageOffset;
} APP_RES_INFO;

#define HDR_NV_TAG_ID               0
#define HDR_NV_TAG_LEN              HDR_4Bytes
#define HDR_VER_ID                  4
#define HDR_VER_ID_LEN              HDR_1Byte
#define HDR_NF_ADDR_CYC_ID          5
#define HDR_NF_ADDR_CYC_ID_LEN      HDR_1Byte
#define HDR_eMMC_SPI_SDC_MHz_ID     5
#define HDR_eMMC_SPI_SDC_MHz_ID_LEN HDR_1Byte
#define HDR_NF_PAGE_SIZE_ID         6
#define HDR_NF_PAGE_SIZE_ID_LEN     HDR_1Byte
#define HDR_NF_BLK_SIZE_ID          7
#define HDR_NF_BLK_SIZE_ID_LEN      HDR_1Byte
#define HDR_NF_CHECK_KEY_ID         8
#define HDR_NF_CHECK_KEY_ID_LEN     HDR_2Bytes
#define HDR_NOT_NF_CHECK_SUM_ID     8
#define HDR_NOT_NF_CHECK_SUM_ID_LEN HDR_4Bytes
#define HDR_NF_BCH_ID               10
#define HDR_NF_BCH_ID_LEN           HDR_1Byte
#define HDR_NF_BTH_BCH_ID           11
#define HDR_NF_BTH_BCH_ID_LEN       HDR_1Byte
#define HDR_BTHDR_COPIES_ID         12
#define HDR_BTHDR_COPIES_ID_LEN     HDR_2Bytes
#define HDR_NV_BTLDR_START_ID       14
#define HDR_NV_BTLDR_START_ID_LEN   HDR_2Bytes
#define HDR_NV_BTLDR_SIZE_ID        16
#define HDR_NV_BTLDR_SIZE_ID_LEN    HDR_2Bytes
#define HDR_BTLDR_COPIES_ID         18
#define HDR_BTLDR_COPIES_ID_LEN     HDR_2Bytes
#define HDR_NV_RTCODE_START_ID      20
#define HDR_NV_RTCODE_START_ID_LEN  HDR_2Bytes
#define HDR_NF_TYPE_ID              23   // 0:MLC, 1:TLC, 2:SLC, 3:SmallPage 4:NAND ROM
#define HDR_NF_TYPE_ID_LEN          HDR_1Byte
#define HDR_DRAM_TYPE_ID            24
#define HDR_DRAM_TYPE_ID_LEN        HDR_1Byte
#define HDR_DRAM_CNT_ID             25
#define HDR_DRAM_CNT_ID_LEN         HDR_1Byte
#define HDR_NV_DRAM_CLK0_ID         26
#define HDR_NV_DRAM_CLK0_ID_LEN     HDR_1Byte
#define HDR_NV_DRAM_CLK1_ID         27
#define HDR_NV_DRAM_CLK1_ID_LEN     HDR_1Byte
#define HDR_NV_DRAM_CLK2_ID         28
#define HDR_NV_DRAM_CLK2_ID_LEN     HDR_1Byte
#define HDR_NV_DRAM_CLK3_ID         29
#define HDR_NV_DRAM_CLK3_ID_LEN     HDR_1Byte
#define HDR_NV_DRAM_CLK4_ID         30
#define HDR_NV_DRAM_CLK4_ID_LEN     HDR_1Byte
#define HDR_BTLDR_CLK_SET_ID        31
#define HDR_BTLDR_CLK_SET_ID_LEN    HDR_1Byte
#define HDR_BTLDR_WORK_ADDR_ID      32
#define HDR_BTLDR_WORK_ADDR_ID_LEN  HDR_4Bytes
#define HDR_NF_STRON_PAGE_EN_ID     36
#define HDR_NF_STRON_PAGE_EN_ID_LEN HDR_1Byte
#define HDR_PRE_REG_CNT_ID          37
#define HDR_PRE_REG_CNT_ID_LEN      HDR_1Byte
#define HDR_BTLDR_ARM_CLK_DIV4      38
#define HDR_BTLDR_ARM_CLK_DIV4_LEN  HDR_1Byte
#define HDR_NF_ERASE_CYCLE_ID       39
#define HDR_NF_ERASE_CYCLE_ID_LEN   HDR_1Byte
#define HDR_GPZP_SRC_ADDR_ID        40
#define HDR_GPZP_SRC_ADDR_ID_LEN    HDR_4Bytes
#define HDR_NVRAM_MB_SIZE_ID        44
#define HDR_NVRAM_MB_SIZE_ID_LEN    HDR_2Bytes
#define HDR_NF_WP_PIN_ID            46
#define HDR_NF_WP_PIN_LEN           HDR_1Byte
#define HDR_SW_FUSE_ID              47
#define HDR_SW_FUSE_ID_LEN          HDR_1Byte

#define HDR_DRAM_PARA0_START        64
#define HDR_DRAM_PARA1_START        120
#define HDR_DRAM_PARA2_START        176
#define HDR_DRAM_PARA3_START        232
#define HDR_DRAM_PARA4_START        288
#define HDR_PRE_REG_START           328 //344
#define HDR_STRONG_TBL_START        768
#define SDC_GP_TAG_START            0x0
#define SDC_FAT_TAG_START           0x60
#define SDC_MBR_TAG_START           200
#define HDR_PRE_REG_START_ID        328
#define HDR_STRONG_TBL_START_ID     768
#define HDR_RTCODE_SIZE_ID          496   // RtCode Sector Size      
#define HDR_RTCODE_SIZE_ID_LEN  HDR_4Bytes
#define HDR_RTCODE_RUN_ADDR_ID      500     
#define HDR_RTCODE_RUN_ADDR_ID_LEN  HDR_4Bytes
#define HDR_APP_TAG_ID              504     
#define HDR_APP_TAG_ID_LEN          HDR_2Bytes
#define HDR_APP_PERCENT_ID          506     
#define HDR_APP_PERCENT_ID_LEN      HDR_2Bytes
#define HDR_APP_SIZE_ID             508     
#define HDR_APP_SIZE_ID_LEN         HDR_4Bytes

#define HDR_APP_START_PAGE_ID           HDR_NV_RTCODE_START_ID
#define HDR_APP_START_PAGE_ID_LEN       HDR_NV_RTCODE_START_ID_LEN
#define HDR_NV_BTLDR_START_ID_V2        56
#define HDR_NV_BTLDR_START_ID_V2_LEN    HDR_4Bytes
#define HDR_NV_BTLDR_SIZE_ID_V2         60
#define HDR_NV_BTLDR_SIZE_ID_V2_LEN     HDR_4Bytes
#define HDR_APP_START_PAGE_ID_V2        272 //312 //272
#define HDR_APP_START_PAGE_ID_V2_LEN    HDR_4Bytes
#define HDR_RTCODE_RUN_ADDR_ID_V2       276 //316 //276
#define HDR_RTCODE_RUN_ADDR_ID_V2_LEN   HDR_4Bytes
#define HDR_APP_TAG_ID_V2               280     
#define HDR_APP_TAG_ID_V2_LEN           HDR_2Bytes
#define HDR_APP_PERCENT_ID_V2           282     
#define HDR_APP_PERCENT_ID_V2_LEN       HDR_2Bytes
#define HDR_APP_SIZE_ID_V2              284     
#define HDR_APP_SIZE_ID_V2_LEN          HDR_4Bytes

#define HDR_PRE_REG_START_ID        328 //344
#define MAX_BTLDR_APPHDR_SIZE 2   // unit: sectors

HEADER_NV_INFO nv_info;
#endif

INT32U combin_reg_data(INT8U *data, INT32S len);
//NV_FS_HANDLE NVFILEHANDE[5];
//resource

INT32U Resource_area_position;
INT32U User_area_position;
INT16U total_rs;
INT16U RS_TABLE_SIZE;

FLASHRSINDEX *RS_TABLE =NULL; //Index table of RES

//user setting

INT32U tbuf[128];

ALIGN16 INT16U   PageBuffer[2048];

#ifdef C_EMBEDED_STORAGE_NAND
#define NAND_TABLE_SIZE  20
INT16U ND_MAPTABLE[NAND_TABLE_SIZE];
#endif

INT32U storage_detection_isr(void);

//L3
INT16U nv_open(INT8U *path);
INT32U nv_read(INT16S fd, INT32U buf, INT32U size);
INT32U nv_mcu_read(INT16S fd, INT32U buf, INT32U size);
//INT32U nv_fast_read(INT16S fd, INT32U buf, INT32U size);
INT32U nv_lseek(INT16S fd, INT32S foffset, INT16S origin);
INT32U nv_rs_size_get(INT16S fd); // jandy add to querry fd size
//L2
INT32U nvmemory_rs_byte_load(INT8U *name ,INT32U offset_byte, INT32U *pbuf , INT32U byte_count);
INT32U nvmemory_rs_sector_load(INT8U *name ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
//L1
INT32U nvspiflash_retrieval(void);
INT32U nvspiflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);
INT32U nvspiflash_user_set (INT16U itag ,INT32U *pbuf , INT16U secter_count);
INT32U nvspiflash_user_get (INT16U itag ,INT32U *pbuf , INT16U secter_count);
//NV NAND
//L1
INT32U nvnandflash_retrieval(void);
INT32U nvnandflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);

#if NVRAM_MANAGER_FORMAT == NVRAM_V3
INT32U nvnandflash_user_get(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count);
INT32U nvnandflash_user_set(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count);
#else
INT32U nvnandflash_user_get(INT32U *pbuf_align_16 , INT16U secter_count);
INT32U nvnandflash_user_set(INT32U *pbuf_align_16 , INT16U secter_count);
#endif
//sunxw added NVRAM for jpeg or gif decode use
//L1
INT32U nvram_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count);

void NVRAM_OS_LOCK(void);
void NVRAM_OS_UNLOCK(void);
void nvram_os_init(void);

#if _NVNAND_OS_ == 1
static OS_EVENT *gNV_sem;
#endif

void nvram_os_init(void) 
{
  #if _NVNAND_OS_ == 1
    gNV_sem = OSSemCreate(1);
  #endif
}

void NVRAM_OS_LOCK(void)
{
  #if _NVNAND_OS_ == 1
	INT8U err = NULL;
	
	OSSemPend(gNV_sem, 0, &err);
  
  #endif
}

void NVRAM_OS_UNLOCK(void)
{
  #if _NVNAND_OS_ == 1
	OSSemPost(gNV_sem);
  #endif
}

INT32U nvram_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count)
{
	if (C_NV_FLASH_TYPE == C_SPI_FLASH)
		return nvspiflash_rs_get(itag, offset_secter, pbuf, secter_count);
	else
#if NAND1_EN == 1  	
		return nvnandflash_rs_get(itag, offset_secter, pbuf, secter_count);
#endif
	return 1;
}


INT32U combin_reg_data(INT8U *data, INT32S len)
{
	INT32S i;
	INT32U value = 0;
	for (i=0;i<len;i++) {
		value |= data[i] << i*8;
	}
	return value;
}


INT16U name2index(INT8U *name)
{
#if NVRAM_MANAGER_FORMAT == NVRAM_V1 
  INT16U iffind,i,j ;
  iffind = 0 ;
  for (i =0 ; i<total_rs; i++) {
    for (j =0 ; j<8; j++) {
          if (RS_TABLE[i].name[j] == *(name+j))
          {
           if(j ==7)
            return  i ;
          }
          else
           break;
    }
  }
#else
  INT8U *pData;
  INT16U i,j ;
  for (i =0 ; i<total_rs; i++) {
  	pData = name;
    for (j =0 ; j<19; j++) {
          if (*pData == '.') { 
          	pData++; //by pass [.]
          }
          if ((RS_TABLE[i].name[j] == 0x20) && (j >3)) {
          	return  i;
          }
          if (RS_TABLE[i].name[j] != *pData++) {
          	break; 
          }
    }
  }
#endif
  return 0xffff;
}
//////////////////////////////////////////////////////////
//NV FILE SYSTEM L3
/////////////////////////////////////////////////////////
INT16U nv_open(INT8U *path)
{
  INT16U index;
  index = name2index(path);
  if (index == 0xffff) {  
      DBG_PRINT ("nv_open retry!\r\n");	
	  nvmemory_init();
	  index = name2index(path);
	  if (index == 0xffff) {  
       	 	DBG_PRINT ("nv_open retry fail~~~~~~!\r\n");	
    		return 0xFFFF;
      }
  }
  RS_TABLE[index].offset = 0;
  return index;
}

INT32U nv_mcu_read(INT16S fd, INT32U buf, INT32U byte_size)
{
  INT32U ret;
  //NVRAM_OS_LOCK();
  if  ( (RS_TABLE[fd].offset + byte_size) >  (RS_TABLE[fd].size * 512) ) {NVRAM_OS_UNLOCK();return 1;}
  ret = nvmemory_rs_byte_load(RS_TABLE[fd].name , RS_TABLE[fd].offset ,(INT32U *)buf ,byte_size);
  if (!ret)
  {
    RS_TABLE[fd].offset+=byte_size;
  }
 // NVRAM_OS_UNLOCK();
  return ret;
}


INT32U nv_read(INT16S fd, INT32U buf, INT32U byte_size)
{
  INT32U ret;
  INT32U start_sector,sector_count,render;
  INT32U offset,i;
  INT32S temp_byte;
 // FLASHRSINDEX *TRACE_TABLE =&RS_TABLE[fd]; 
  //user buffer align -4
  if (((4 - (((INT32U) buf) & 0x00000003)) & 0x00000003) != 0)
  {
    return  nv_mcu_read(fd,buf,byte_size);	
  }	
  if (byte_size<512)
  {
    return nv_mcu_read(fd,buf,byte_size);  /* small size, master ARM enough */
  }
  if  ( (RS_TABLE[fd].offset + byte_size) >  (RS_TABLE[fd].size * 512) ) 
  {
     byte_size = byte_size - ((RS_TABLE[fd].offset + byte_size) - (RS_TABLE[fd].size * 512));
     temp_byte = (INT32S) byte_size;
     if  (temp_byte<0) {return 1;}
    //  return 1;
  }

  offset  =   (RS_TABLE[fd].offset & 0x000001ff);  
  //file offset align -4
  if ((512-offset) & 0x00000003 )
  {
      return  nv_mcu_read(fd,buf,byte_size);	
  }      
  start_sector =  (RS_TABLE[fd].offset) / 512 ;
  sector_count = (byte_size) /512; //secters 
  if (offset)
  {
  //Start
    ret = nvmemory_rs_sector_load(RS_TABLE[fd].name,start_sector,(INT32U *)tbuf ,1);       
    for (i =0 ; i<(512-offset) ;i ++)
       *((INT8U *)buf +i) =*((INT8U *)tbuf +i +offset);       
  //Mid
    sector_count = (byte_size -(512-offset)) / 512;
    ret = nvmemory_rs_sector_load(RS_TABLE[fd].name,start_sector+1,(INT32U *) ((INT8U *)buf +(512-offset)),sector_count);       
  //End  
    render = (byte_size -(512-offset)) % 512;
    if (render)
    {
     ret = nvmemory_rs_sector_load(RS_TABLE[fd].name,start_sector+1+sector_count,(INT32U *)tbuf,1);         
     for (i =0 ; i<render ;i ++)
       *((INT8U *)buf +i +(512-offset) +(sector_count*512)) =*((INT8U *)tbuf +i );       
    }    
  }  
  else
  {
     if (sector_count)
 	   ret = nvmemory_rs_sector_load(RS_TABLE[fd].name,start_sector,(INT32U *)buf,sector_count);   
     render = (byte_size) % 512; 
     if (render)
     {
         ret = nvmemory_rs_sector_load(RS_TABLE[fd].name,start_sector+sector_count,(INT32U *)tbuf,1);         
	     for (i =0 ; i<render ;i ++)
    	   *((INT8U *)buf +i +(sector_count*512)) =*((INT8U *)tbuf +i );       
     } 	   
      
  }
  if (!ret)
  {
    RS_TABLE[fd].offset+=byte_size;
  }
  return ret;
}
//#define SEEK_SET 0 /* start of stream (see fseek) */
//#define SEEK_CUR 1 /* current position in stream (see fseek) */
//#define SEEK_END 2 /* end of stream (see fseek) */

INT32U nv_lseek(INT16S fd, INT32S foffset, INT16S origin)
{
  INT32U targetaddr;

  //  FLASHRSINDEX *TRACE = &RS_TABLE[fd]; //Index table of RES

  if (origin == SEEK_SET)
  {
    if (foffset >= (RS_TABLE[fd].size * 512) )
     {return 0xffffffff;}
    else
     {RS_TABLE[fd].offset =  foffset ;}
  }
  else if (origin == SEEK_CUR)
  {
    targetaddr =RS_TABLE[fd].offset+foffset;
    if ( targetaddr >=  (RS_TABLE[fd].size * 512) ) {return 1;}
    RS_TABLE[fd].offset = targetaddr ;
  }
  else
  {
   RS_TABLE[fd].offset =  (RS_TABLE[fd].size * 512) - 1 ; 
  }
  return  RS_TABLE[fd].offset;
}

INT32U nv_rs_size_get(INT16S fd)
{
    INT32U ret;
    
    nv_lseek(fd,0,SEEK_SET);
	ret = nv_lseek(fd,0,SEEK_END)+1;
	nv_lseek(fd,0,SEEK_SET);

    return ret;
}


//////////////////////////////////////////////////////////
//
//  Flash access(SPI/Nand/Nor Flash)
//
//////////////////////////////////////////////////////////
INT32U nvmemory_init(void)
{
  INT8U  manID;
  INT8U  data_buf[512];
  if (C_NV_FLASH_TYPE == C_SPI_FLASH)
  {
    SPI_Flash_init();
    spi_clk_set(0,SYSCLK_128); /* spi clock = 48M/8/128 = 46.8k */
    SPI_Flash_readID(data_buf);
    manID = data_buf[0];
    if (manID == 0xd2) {
    }
	else if (manID == 0xc2) {
		 spi_clk_set(0,SYSCLK_4);
	}
	else {
		 spi_clk_set(0,SYSCLK_4);
	}
	nvspiflash_retrieval();
  }else if (C_NV_FLASH_TYPE == C_NAND_FLASH){
#if NAND1_EN == 1    
     if (Nand_Init())
       return 1;       
    if ( nvnandflash_retrieval())
       return 2;
#endif       
  }else {// C_NOR_FLASHY)


  }
  //by pass boot area and calcular others areas's position

  return 0;
}


INT32U nvmemory_rs_byte_load(INT8U *name ,INT32U offset_byte, INT32U *pbuf , INT32U byte_count)
{
 INT16U *tempbuffer;
 INT32U i,j;
 INT32U offset_secor,start_byte;
 INT32U ret,index;
 offset_secor = offset_byte / 512;
 //sector_count = (byte_count  >> 9) +1;
 start_byte   = offset_byte % 512;
 tempbuffer = (INT16U *) gp_malloc_align(2112, 16);
 //begin
 ret = nvmemory_rs_sector_load(name,offset_secor,(INT32U *)tempbuffer,1);
 if (ret >0)
 {
  gp_free((void *) tempbuffer);
  return 1;
 }
 index = 512 - start_byte;
 index = (byte_count > index) ? index :  byte_count;
 for(i=0 ; i <index ;i++)
       *((INT8U *)pbuf +i) =*((INT8U *)tempbuffer +start_byte +i );
 //mid
 byte_count -=index;
 while(byte_count >0)
 {
    offset_secor++;
    ret = nvmemory_rs_sector_load(name,offset_secor,(INT32U *)tempbuffer,1);
    if (ret >0)
    {
      gp_free((void *) tempbuffer);
      return 2;
    }
    if (byte_count>=512) {
        for(j=0 ; j < 512 ;j++)
           *((INT8U *)pbuf +j +index) =*((INT8U *)tempbuffer+j );
        index+=512;
        byte_count-=512;
    } else{
        for(j=0 ; j < byte_count ;j++)
           *((INT8U *)pbuf +j +index) =*((INT8U *)tempbuffer+j );
        break;
    }
 }  
 gp_free((void *) tempbuffer);
 return 0;

 //end
}

INT32U nvmemory_rs_sector_load(INT8U *name ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count)
{
  INT16U itag ;
  itag = name2index(name);
  if (itag ==0xFFFF) return 1;
  if (C_NV_FLASH_TYPE == C_SPI_FLASH)
  {
    return (nvspiflash_rs_get(itag,offset_secter,pbuf,secter_count) );

  }
  else // (C_NV_FLASH_TYPE == C_NAND_FLASH){
  {
#if NAND1_EN == 1    
    return (nvnandflash_rs_get(itag,offset_secter,pbuf,secter_count));
#endif    
  }
  return 1;
}

INT32U nvmemory_user_sector_load(INT16U itag, INT32U *pbuf , INT16U secter_count)
{

  if (C_NV_FLASH_TYPE == C_SPI_FLASH)
  {
    return nvspiflash_user_get ( itag , pbuf , secter_count);

  }else ///if (C_NV_FLASH_TYPE == C_NAND_FLASH){
  {
#if NAND1_EN == 1  
    return nvnandflash_user_get(itag,pbuf,secter_count);
#endif    
  }
    return 1;
}

INT32U nvmemory_user_sector_store(INT16U itag, INT32U *pbuf , INT16U secter_count)
{
  if (C_NV_FLASH_TYPE == C_SPI_FLASH)
  {
    return nvspiflash_user_set ( itag , pbuf , secter_count);

  }else //if (C_NV_FLASH_TYPE == C_NAND_FLASH){
  {
#if NAND1_EN == 1  
    return nvnandflash_user_set(itag,pbuf,secter_count);
#endif    
  }
    return 1;
}

///////////////////////////////////////////////////////
// SPI Flash
///////////////////////////////////////////////////////
//retrieval from flash
#if (C_NV_FLASH_TYPE == C_SPI_FLASH)
#if NVRAM_MANAGER_FORMAT == NVRAM_V1

INT32U nvspiflash_retrieval(void)
{
    INT16U  i,j ;
    INT8U  data_buf[2048];

    NVRAM_OS_LOCK();
 	if (SPI_Flash_read_page(0, data_buf) != STATUS_OK) {
        NVRAM_OS_UNLOCK();
		return 1;
	}
	// Comparing spi flash bootcode tag
	if (gp_strncmp((INT8S *)data_buf, (INT8S *)SPI_FLASH_TAG,10) != 0) {
        NVRAM_OS_UNLOCK();
		return 2;
	}
	// Getting index position
	Resource_area_position = combin_reg_data(&data_buf[HEADER_OFFSET_SEC_CNT<<1],2);
	//Resource_area_position++;
	for ( i= 0 ; i<4 ; i++)
	{
		if (SPI_Flash_read_page( (Resource_area_position<<9) + (i<<8), data_buf + 256*i) != STATUS_OK) {
            NVRAM_OS_UNLOCK();
            return 1;
		}
	}
	// Comparing spi flash resource area tag
	if (gp_strncmp((INT8S *)data_buf, (INT8S *)RESOURCE_TAG,2) != 0) {
            NVRAM_OS_UNLOCK();
            return 2;
	}
	total_rs = (INT16U) combin_reg_data(&data_buf[2],2);
    RS_TABLE_SIZE = total_rs ;   
    if (RS_TABLE !=NULL) 
    {
      gp_free((void *) RS_TABLE);
      RS_TABLE =NULL;
    }
    RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_rs+1)* sizeof(FLASHRSINDEX)));
	if ( total_rs > 101)
	{
      	for ( i= 0 ; i<4 ; i++)
		{
			if (SPI_Flash_read_page( (Resource_area_position<<9) + (i<<8)+1024, data_buf + 256*i +1024) != STATUS_OK) {
                NVRAM_OS_UNLOCK();
                return 1;
			}
		}
	}
	for (i =0 ; i<total_rs+1; i++) {
    	for (j =0 ; j<8; j++)
           RS_TABLE[i].name[j] = data_buf[j+4 + i*10];
      RS_TABLE[i].pos = (INT32U) combin_reg_data(&data_buf[12 + i*10],2) ;

	}
	// The least pos is User area position
	User_area_position = RS_TABLE[i-1].pos;
	for (i =0 ; i<total_rs; i++) {
      RS_TABLE[i].size =  RS_TABLE[i+1].pos - RS_TABLE[i].pos ;

	}
    NVRAM_OS_UNLOCK();
	return 0;
}


#else
#if (defined APP_HEADER_V20) && (APP_HEADER_V20 == 1) 
INT32S spi_sector_read(INT32U start_sector_id, INT8U* sector_buf, INT32U sector_nums)
{
    INT32U SPI_Addr = start_sector_id*512;
    INT32U Page = sector_nums << 1;
    INT32S i;

	for(i=0; i<Page; i++) {
   		if(SPI_Flash_read_page(SPI_Addr, (INT8U *)sector_buf) != STATUS_OK) {
   			return STATUS_FAIL;
   		}
   		SPI_Addr += 256;
   		sector_buf += 256;
    }
    
    return STATUS_OK;
}

INT32S nv_hdr_to_info(INT8U* header, HEADER_NV_INFO *nv_info)
{ 
    INT32U *HdrWord=(INT32U *)&header[0];
    INT16U *HdrShort=(INT16U *)&header[0];
    INT8U HpId;
    //INT8U smart_flow_flag=0;
    INT8U hdr_ver;

    nv_info->block_size = 32;
        

    nv_info->bthdr_page_size = 512; // 次恰為 Intelligent ID 之 PageSIze
    nv_info->bthdr_page_sectors = 1;
           

    hdr_ver = header[HDR_VER_ID];       
	HpId = header[HDR_NF_PAGE_SIZE_ID];
	if(HpId == 0)
	{
		nv_info->btldr_page_size = 512;		
	} else {
		nv_info->btldr_page_size = HpId*1024;
	}	
	nv_info->btldr_page_sectors = nv_info->btldr_page_size/512;
	nv_info->btldr_bch = header[HDR_NF_BCH_ID];
	nv_info->addr_cycle = header[HDR_NF_ADDR_CYC_ID];
    nv_info->block_size = header[HDR_NF_BLK_SIZE_ID]*32; 


    nv_info->bthdr_page_sectors = nv_info->bthdr_page_size/512;
    nv_info->bthdr_copies = HdrShort[HDR_BTHDR_COPIES_ID/2];
    nv_info->btldr_copies = HdrShort[HDR_BTLDR_COPIES_ID/2];

    if(hdr_ver>=0x20) 
    {    /* BootHeader V2.x */
        nv_info->btldr_set_pages = HdrWord[HDR_NV_BTLDR_SIZE_ID_V2/4];
        nv_info->btldr_start_page_id = HdrWord[HDR_NV_BTLDR_START_ID_V2/4];
        nv_info->app_start = HdrWord[HDR_APP_START_PAGE_ID_V2/4];
        nv_info->app_size= HdrWord[HDR_APP_SIZE_ID_V2/4];
        nv_info->app_tag = HdrShort[HDR_APP_TAG_ID_V2/2];
        nv_info->app_perscent= HdrShort[HDR_APP_PERCENT_ID_V2/2];
        nv_info->app_rtcode_run_addr = HdrWord[HDR_RTCODE_RUN_ADDR_ID_V2/4];
    } else {

        nv_info->btldr_set_pages = HdrShort[HDR_NV_BTLDR_SIZE_ID/2];
        nv_info->btldr_start_page_id = HdrShort[HDR_NV_BTLDR_START_ID/2];
    	nv_info->app_start = HdrShort[HDR_APP_START_PAGE_ID/2];
        nv_info->app_size= HdrWord[HDR_APP_SIZE_ID/4];
        nv_info->app_tag = HdrShort[HDR_APP_TAG_ID/2];
        nv_info->app_perscent= HdrShort[HDR_APP_PERCENT_ID/2];
        nv_info->app_rtcode_run_addr = HdrWord[HDR_RTCODE_RUN_ADDR_ID/4];
    }
    
	nv_info->nand_type = header[HDR_NF_TYPE_ID];
	nv_info->StrongEn = header[HDR_NF_STRON_PAGE_EN_ID];
	nv_info->swfuse = header[HDR_SW_FUSE_ID];
    // Total MB Size 是由 GUI 指定的
	nv_info->total_MB_size = HdrShort[HDR_NVRAM_MB_SIZE_ID/2];
	
	//hdr_sw_fuse=nv_info->swfuse;
    //if (smart_flow_flag==1)  /*Re-Building Check Key*/
    //{
    //   HdrShort[HDR_NF_CHECK_KEY_ID/2]=hdr_check_key_count((INT8U *) &header[0]);        
    //}
	return STATUS_OK;	
}

INT32S spi_app_read(INT32U start_sector_id, INT8U* sector_buf, INT32U sector_nums)
{
    return spi_sector_read(nv_info.app_start+start_sector_id, sector_buf, sector_nums);
}

INT32S app_hdr_info_get(APPHDR *hdr_info, INT32U* app_hdr_buf)
{
    INT8U* hdr_byte = (INT8U *) &app_hdr_buf[0];
    INT16U* hdr_short = (INT16U *) &app_hdr_buf[0];
    INT32U* hdr_word = (INT32U *) &app_hdr_buf[0];

    if (hdr_word[0]!=0x50415047) {
        return -1; // illegle Part Header Tag
    } 

    hdr_info->AppTag = hdr_word[0];
    hdr_info->ver = hdr_byte[4];
    hdr_info->DrvType = hdr_byte[5];
    hdr_info->HdrSize = hdr_byte[6];
    hdr_info->TotalSizeSectors = hdr_word[8/4];
    hdr_info->PartNums = hdr_short[12/2];
    hdr_info->opKeyId = hdr_byte[14];
    hdr_info->opKeyPort= hdr_byte[15];
    hdr_info->AreaCheckSum= hdr_word[20/4];

    return 0;
}

INT32S app_part_info_get(INT32U PartId, APP_PART_INFO* AppPartInfo, INT32U* app_hdr_buf)
{
    INT8U* hdr_byte = (INT8U *) &app_hdr_buf[0];
    INT16U* hdr_short = (INT16U *) &app_hdr_buf[0];
    INT32U* hdr_word = (INT32U *) &app_hdr_buf[0];
    INT32U N = PartId;

    if (hdr_word[0]!=0x50415047) {
        return -1; // illegle Part Header Tag
    }
    if (PartId >= hdr_short[12/2]) {
        return -2;  // Part ID over flow
    }

    AppPartInfo->PartId = PartId;
    AppPartInfo->StartSectorId = hdr_word[(24+16*N)/4];
    AppPartInfo->PartImgSize = hdr_word[((24+16*N)+4)/4];
    AppPartInfo->PartType= hdr_byte[(24+16*N)+8];
    AppPartInfo->DestinationAddr = hdr_word[((24+16*N)+12)/4];

    return 0;
}
#endif

INT32U nvspiflash_retrieval(void)
{
#if (defined APP_HEADER_V20) && (APP_HEADER_V20 == 1)  
    INT32S i, nRet; 
    INT32U boot_part_id;
    INT32U bt_hdr_buf, app_hdr_buf;
    APP_PART_INFO app_part_info;
    APPHDR app_hdr;

	NVRAM_OS_LOCK();
	bt_hdr_buf = (INT32U)gp_malloc_align(512 + 512*MAX_BTLDR_APPHDR_SIZE, 4);
	app_hdr_buf = bt_hdr_buf + 512;
	if(bt_hdr_buf == 0) {
		nRet = -1; goto __exit;
	}
    
	if(spi_sector_read(0, (INT8U*)bt_hdr_buf, 1) < 0) {
		nRet = -1; goto __exit;
	}
	
	if(nv_hdr_to_info((INT8U*)bt_hdr_buf, &nv_info) != STATUS_OK) {
		nRet = -1; goto __exit;
	}
	
	spi_app_read(0, (INT8U*)app_hdr_buf, 1);
	if(app_hdr_info_get((APPHDR *) &app_hdr, (INT32U *)app_hdr_buf) != STATUS_OK) {
		nRet = -1; goto __exit;
	}
	
	for (i=0;i<app_hdr.PartNums;i++) {
        app_part_info_get(i, (APP_PART_INFO *) &app_part_info, (INT32U *)app_hdr_buf);
        if (app_part_info.PartType==GpResourcePart) {
            boot_part_id = app_part_info.PartId;
            break;
        } else {
            // NOT GPDV Bootable image  
        }
    }
    
    if(i == app_hdr.PartNums) {
    	nRet = -1; goto __exit;
    }
    
    app_part_info_get(boot_part_id, (APP_PART_INFO *) &app_part_info, (INT32U *)app_hdr_buf);
    spi_app_read(app_part_info.StartSectorId, (INT8U*)PageBuffer, 1);
    
	Resource_area_position = nv_info.app_start + app_part_info.StartSectorId;
	if(PageBuffer[0] != 0x5047) {
		nRet = -1; goto __exit; 
	}
	
	total_rs = PageBuffer[1];		//App Rs Num.
    RS_TABLE_SIZE = PageBuffer[2]; 	//App Rs Hdr Size  
    if (RS_TABLE !=NULL) {
		gp_free((void *) RS_TABLE);
      	RS_TABLE =NULL;
    }
    RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_rs+1)* sizeof(FLASHRSINDEX)));
    if(RS_TABLE == 0) {
    	nRet = -1; goto __exit;
    }
    
    if(RS_TABLE_SIZE > 0) {
    	INT32U buf_addr; 
    	APP_RES_INFO *pResc;
    	
    	buf_addr = (INT32U)gp_malloc_align(RS_TABLE_SIZE * 512, 4);
    	if(buf_addr == 0) {
    		nRet = -1; goto __exit;
    	}

    	pResc = (APP_RES_INFO *)(buf_addr + 24);
    	spi_app_read(app_part_info.StartSectorId, (INT8U*)buf_addr, RS_TABLE_SIZE);
    	
    	for(i=0; i<total_rs; i++) {
    		INT8U *pData;
    		INT32U len;
    		
    		len = 0;
    		pData = (INT8U *)pResc->FileName;
    		do {
    			len++;
    			if(*pData == 0x00) {
    				*pData = 0x20;
    				break;
    			}
    			pData++;	
    		}while(len <= 20);   		
    		
    		gp_memcpy((INT8S *)RS_TABLE[i].name, (INT8S *)pResc->FileName, len);
    		RS_TABLE[i].offset = 0;					//in byte
    		RS_TABLE[i].pos = pResc->ImageOffset;	//in sector
    		RS_TABLE[i].size = 0;					//in sector
    		pResc++;
    	}
    	
    	// The least pos is User area position
		User_area_position = RS_TABLE[i-1].pos;
    	for (i =0 ; i<(total_rs - 1); i++) {
    		RS_TABLE[i].size = RS_TABLE[i+1].pos - RS_TABLE[i].pos;
		}
		RS_TABLE[total_rs - 1].size = app_part_info.PartImgSize - RS_TABLE[total_rs - 1].pos;
    	gp_free((void *)buf_addr);
    	nRet = 0;
    } else {
    	nRet = -1;
    }
    
__exit:
	if(bt_hdr_buf) {
		gp_free((void *)bt_hdr_buf);
	}   
    NVRAM_OS_UNLOCK();
    return nRet;
#else
    INT16U  i,j ;
    //INT8U  data_buf[2048];
    //INT16U *PageBuffer;
    INT16U Rspages;

    NVRAM_OS_LOCK();
    //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);
 	if (SPI_Flash_read_page(0, (INT8U*) PageBuffer) != STATUS_OK) {
 	     //gp_free((void *) PageBuffer);
         NVRAM_OS_UNLOCK();
		return 1;
	}
	// Comparing spi flash bootcode tag
	if (gp_strncmp((INT8S *)PageBuffer, (INT8S *)SPI_FLASH_TAG,10) != 0) {
        //gp_free((void *) PageBuffer);
        NVRAM_OS_UNLOCK();
		return 2;
	}
	// Getting index position
	Resource_area_position =combin_reg_data((INT8U *)&PageBuffer[HEADER_OFFSET_SEC_CNT],2);
	for ( i= 0 ; i<4 ; i++)
	{
		if (SPI_Flash_read_page( (Resource_area_position*512) + (256*i),(INT8U*)(PageBuffer + 128*i)) != STATUS_OK) {
 		    //gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
			return 1;
		}
	}
	// Comparing spi flash resource area tag
	if (gp_strncmp((INT8S *)PageBuffer, (INT8S *)RESOURCE_TAG,2) != 0) {
            //gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
			return 2;
	}
	total_rs = (INT16U) combin_reg_data((INT8U *)&PageBuffer[1],2);	
    RS_TABLE_SIZE = total_rs ;   
    if (RS_TABLE !=NULL) 
    {
      gp_free((void *) RS_TABLE);
      RS_TABLE =NULL;
    }
    RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_rs+1)* sizeof(FLASHRSINDEX)));
    Rspages = (((total_rs*23 +4)-1) / 256)+1; //  256 bytes/page
    if (Rspages >4)
    {
      	for ( i= 0 ; i<Rspages-4 ; i++)
		{
			if (SPI_Flash_read_page( (Resource_area_position*512) + (256*i)+1024, (INT8U*)(PageBuffer + 128*i +512)) != STATUS_OK) {
    		    //gp_free((void *) PageBuffer);
                NVRAM_OS_UNLOCK();
				return 1;
			}
		}
	}
   for (i =0 ; i<total_rs+1; i++) {
     for (j =0 ; j<19; j++)
           RS_TABLE[i].name[j] = *((INT8U *)PageBuffer + j+4 +(23*i) );
      RS_TABLE[i].pos = (INT32U) combin_reg_data((INT8U *)PageBuffer +23+ (23*i),4);
   }
	// The least pos is User area position
	User_area_position = RS_TABLE[i-1].pos;
	for (i =0 ; i<total_rs; i++) {
      RS_TABLE[i].size =  RS_TABLE[i+1].pos - RS_TABLE[i].pos ;

	}
    //gp_free((void *) PageBuffer);
    NVRAM_OS_UNLOCK();
	return 0;
#endif	
}
#endif


INT32U nvspiflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count)
{
   INT16U i;
   INT16U max_sec_count,read_sec_count;
   INT32U start_addr_sec;
   // checking rs tage
   NVRAM_OS_LOCK();
   if ( itag > total_rs)  {NVRAM_OS_UNLOCK();return 1;}
   max_sec_count  = RS_TABLE[itag].size;// RS_TABLE[itag+1].pos - RS_TABLE[itag].pos;
   start_addr_sec = RS_TABLE[itag].pos + Resource_area_position;
   //Add offset
   start_addr_sec += offset_secter;
   read_sec_count = ( (max_sec_count-offset_secter) < secter_count) ?  (max_sec_count-offset_secter) : secter_count ;

   for (i =0 ; i<read_sec_count << 1; i++)
   {
    if (SPI_Flash_read_page( (start_addr_sec*512) + (256*i),(INT8U *)pbuf + 256*i ) != STATUS_OK)
        {
            NVRAM_OS_UNLOCK();
    		return 2;
        }
   }
   NVRAM_OS_UNLOCK();
   return 0;
}

#if 1
//Each stored data is fixed as 512 bytes (1 sector) and is stored secter by secter on FLASH


INT32U nvspiflash_user_set (INT16U itag ,INT32U *pbuf , INT16U secter_count)
{
  INT32U base_adr,i,j,found;
  NVRAM_OS_LOCK();
  if (secter_count > 0x80) {NVRAM_OS_UNLOCK();return 1;}
  //Get Base address
  base_adr = (User_area_position + Resource_area_position); //64K per sector
  i= base_adr & (0x80-1);
  base_adr = base_adr & ~(0x80 -1);
  if (i) base_adr +=0x80; //bounary as 64K (ERASE UNIT)
  base_adr += (itag *0x80*USER_SPI_BLK);
  //Get the least empty secter
  found = 0;
  
 if (itag > 1)
  {
	//Erase All of blocks 
	  for (i =0 ;i <USER_SPI_BLK ;i++)  
		{  
	        SPI_Flash_erase_block(  (base_adr*512) + (i* 0x80*512) );
		}

  	 for (i =0 ;i <(secter_count <<1) ; i++)
	{
		 if (SPI_Flash_write_page(  (base_adr*512) + (i* 256), (INT8U *)pbuf + 256*i ) != STATUS_OK)
		 {      
		    NVRAM_OS_UNLOCK();
		    return 1;
		 }
  	 }
	NVRAM_OS_UNLOCK();
	return 0;	  
	
  }
  else 
  {
	  for (j =0 ;j <USER_SPI_BLK ;j++)  
	  {
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)
		   for (i =0 ;i <0x80 ; i++)
		   {
			 if (SPI_Flash_read_page(  (base_adr+i) * 512 , (INT8U *)tbuf ) != STATUS_OK)
			 {
			    NVRAM_OS_UNLOCK();
			    return 1;
	         }									
		     if ((tbuf[0] ==0xffffffff) &&  (tbuf[1] ==0xffffffff))
		     {
		        found = 1;	
		        break;
		     }
		   }
#else
		   for (i =0 ;i <0x100 ; i++)
		   {
			 if (SPI_Flash_read_page(  (base_adr*512) + (i* 256) , (INT8U *)tbuf ) != STATUS_OK)
	         {     
	            NVRAM_OS_UNLOCK();
	            return 1;
	         }
		     if ((tbuf[0] ==0xffffffff) &&  (tbuf[1] ==0xffffffff))
		     {
		        found = 1;	
		        break;
		     }
		   }
#endif	   
		   if (found) break;	   
		   base_adr +=0x80;
	  }  
		    
	  if ( (found) && ( (128-i) > secter_count ) )
	  {

	   if (itag==0)
	   {
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)  
		    for (j =0 ;j < 2 ;j++)
	    	{
		    	if(SPI_Flash_write_page(( ((base_adr+i) * 512 ) + 256*j ) , (INT8U *)pbuf +256*j ) != STATUS_OK)
	            {      
	                NVRAM_OS_UNLOCK();
	                return 1;
	            }
		    }			
#else
	 		if(SPI_Flash_write_page( (base_adr*512) + (i* 256)    , (INT8U *)pbuf  ) != STATUS_OK)
	        {
	            NVRAM_OS_UNLOCK();
	            return 1;
	        }
#endif    
		   }
		   else //if (itag == 1)
	   {
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)    //add by Lion
			 for (j =0 ;j < (secter_count <<1) ;j++)
	    	{
		    	if(SPI_Flash_write_page(( ((base_adr + i) * 512 ) + 256*j ) , (INT8U *)pbuf +256*j ) != STATUS_OK)
	            {      
	                NVRAM_OS_UNLOCK();
	                return 1;
	            }
		    } 

#else
	   	    for (j =0 ;j < (secter_count <<1) ;j++)
	    	{
		    	if(SPI_Flash_write_page(( ((base_adr) * 512 ) + 256*(j+i) ) , (INT8U *)pbuf +256*j ) != STATUS_OK)
	            {      
	                NVRAM_OS_UNLOCK();
	                return 1;
	            }
		    }
#endif 		    
	   
	   }
	   
	  }      
	  else	//found = 0
	  {
		    if( (found == 1) && ((128-i) <= secter_count))
	  {
		    	base_adr += 0x80;
		    }
		    
	    //Erase All of blocks 
	    for (i =0 ;i <USER_SPI_BLK ;i++)  
		    {  
	        SPI_Flash_erase_block( (base_adr - 0x80 - (i* 0x80)) * 512 );    
		    }
	    //Write at the first page    
	    base_adr= base_adr - (0x80*USER_SPI_BLK) ;  
		   
	   if (itag==0)
	   {    
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)                
	    	for (j =0 ;j < 2 ;j++)
		    {    
		    	if(SPI_Flash_write_page( ((base_adr * 512 ) + (256*j) ) , (INT8U *)pbuf +256*j ) != STATUS_OK)
	            {
	                NVRAM_OS_UNLOCK();
	                return 1;
	            }
		    }			
#else
		    if(SPI_Flash_write_page( (base_adr*512) + (i* 256)+ (256*j) , (INT8U *)pbuf +256*j  ) != STATUS_OK)
	        {
	            NVRAM_OS_UNLOCK();
	            return 1;    
	        }
#endif    
			}
			else
	   {
	   	    for (j =0 ;j < (secter_count <<1) ;j++)
	    	{
			    	//if(SPI_Flash_write_page(( ((base_adr) * 512 ) + 256*(j+i) ) , (INT8U *)pbuf +256*j ) != STATUS_OK)	//Daniel 2010/09/16
			    	if(SPI_Flash_write_page(( ((base_adr) * 512 ) + (256*j) ) , (INT8U *)pbuf +256*j ) != STATUS_OK)
	            {      
	                NVRAM_OS_UNLOCK();
	                return 1;
	            }
		    }	   
	   }
	    
	  }  
	  NVRAM_OS_UNLOCK();
	  return 0;
  }
}

INT32U nvspiflash_user_get (INT16U itag ,INT32U *pbuf , INT16U secter_count)
{
  INT32U base_adr,i,j,found,size;

  NVRAM_OS_LOCK();
  
   if (secter_count > 0x80) {NVRAM_OS_UNLOCK();return 1;}
  base_adr = (User_area_position + Resource_area_position); //64K per sector
  i= base_adr & (0x80-1);	//get block number
  base_adr = base_adr & ~(0x80 -1);		//and 0xFF80
  if (i) base_adr +=0x80; //bounary as 64K
  base_adr += (itag *0x80*USER_SPI_BLK);
  found = 0;
  if (itag > 1)
  {
  	 for (i =0 ;i <(secter_count <<1) ; i++)
	{
		 if (SPI_Flash_read_page(  (base_adr*512) + (i* 256), (INT8U *)pbuf + 256*i ) != STATUS_OK)
		 {      
		    NVRAM_OS_UNLOCK();
		    return 1;
		 }
  	 }
	NVRAM_OS_UNLOCK();
	return 0;	  	
  }
  else
  {
	  for (j =0 ;j <USER_SPI_BLK ;j++)  
	  {
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)
		   for (i =0 ;i <0x80 ; i++)
		   {
			 if (SPI_Flash_read_page(  (base_adr+i) * 512 , (INT8U *)tbuf ) != STATUS_OK)
	         {      
	            NVRAM_OS_UNLOCK();
	            return 1;									
	         }
	         if ((tbuf[0] ==0xffffffff) &&  (tbuf[1] ==0xffffffff))
		     {
		        found = 1;	
		        break;
		     }
		   }
#else
		   for (i =0 ;i <0x100 ; i++)
		   {
			 if (SPI_Flash_read_page(  (base_adr*512) + (i* 256), (INT8U *)tbuf ) != STATUS_OK)
	         {      
	            NVRAM_OS_UNLOCK();
	            return 1;
	         }
		     if ((tbuf[0] ==0xffffffff) &&  (tbuf[1] ==0xffffffff))
		     {
		        found = 1;	
		        break;
		     }
		   }
#endif	   
		   if (found) break;	   
		   base_adr +=0x80;	//means all the sectors of this block are not erased, so changed to next block
	  }        
	  if (found)
	  {
			if (i==0) //checking boundary	//means the first sector is erased!
	      {
			    if (j==0) 
			    {
			    	NVRAM_OS_UNLOCK();
			    	return 0;
			    }
	        else
	        {
	          base_adr= base_adr -0x80;
	          i       = 0x80;
	        }      
	      }   
	   if (itag==0)
	   {         
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)      
		  for (j =0 ;j <2 ;j++)
		  {
			 if (SPI_Flash_read_page(( (base_adr +i-1) * 512) + 256*j  , (INT8U *)pbuf + 256*j ) != STATUS_OK)
				{NVRAM_OS_UNLOCK();return 1;}
		  }
#else
		 if (SPI_Flash_read_page((base_adr*512) + ((i-1)* 256) , (INT8U *)pbuf ) != STATUS_OK)
				{NVRAM_OS_UNLOCK();return 1;}
#endif	  
			}
			else
	   {
#if ( USER_SPI_AREA_SIZE == USER_SPI_512_B)   //add by lion
		 for (j =0 ;j < (secter_count <<1) ;j++)
		 {
			 if (SPI_Flash_read_page(( (base_adr + (i - secter_count))  * 512)  + 256*j  , (INT8U *)pbuf + 256*j ) != STATUS_OK)
				{NVRAM_OS_UNLOCK();return 1;}
		  }
		  
		   size = (pbuf[127] & 0xff000000)>>23;
		  i = i - size;
#else
	      for (j =0 ;j < (secter_count <<1) ;j++)
		  {
			 if (SPI_Flash_read_page(( base_adr  * 512) +(i-(secter_count <<1))*256 + 256*j  , (INT8U *)pbuf + 256*j ) != STATUS_OK)
				{NVRAM_OS_UNLOCK();return 1;}
		  }
		  size = (pbuf[127] & 0xff000000)>>23;
		  i = i - size;
	      for (j =0 ;j < size ;j++)
	      {
			 if (SPI_Flash_read_page(( base_adr  * 512) + (i*256) + 256*j  , (INT8U *)pbuf + 256*j ) != STATUS_OK)
				{NVRAM_OS_UNLOCK();return 1;}      
	      }
#endif     
	   }
	   NVRAM_OS_UNLOCK();
	   return 0;	  
		}	//if(found)
	 NVRAM_OS_UNLOCK(); 
	 return 1;
  }
}

#else

// Tag: 0~4  each slot is  64k (0x10000) ( 0x80  sectors )
INT32U nvspiflash_user_set (INT16U itag ,INT32U *pbuf , INT16U secter_count)
{
  INT32U base_adr,i;
  if (secter_count > 0x80) return 1;
  base_adr = (User_area_position + Resource_area_position); //64K per sector
  i= base_adr & (0x80-1);
  base_adr = base_adr & ~(0x80 -1);
  if (i) base_adr +=0x80;
  base_adr += (itag *0x80);
  SPI_Flash_erase_block(base_adr * 512 );
  for (i =0 ; i < (secter_count <<1) ;i++)
  {
    if(SPI_Flash_write_page( (base_adr * 512 ) + (i <<8) , (INT8U *)pbuf + 256*i ) != STATUS_OK)
		return 1;
  }
  return 0;
}
INT32U nvspiflash_user_get (INT16U itag ,INT32U *pbuf , INT16U secter_count)
{
   INT32U base_adr,i;
   base_adr = (User_area_position + Resource_area_position); //64K per sector
   i= base_adr & (0x80-1);
   base_adr = base_adr & ~(0x80 -1);
   if (i) base_adr +=0x80;
   base_adr += (itag *0x80);
   for (i =0 ;i <secter_count << 1 ;i++)
   {
	 if (SPI_Flash_read_page( (base_adr * 512) +(i <<8) , (INT8U *)pbuf + 256*i ) != STATUS_OK)
		return 1;
   }
   return 0;
}
#endif

#if 1//#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
INT32U nvspiflash_file_size_get(INT16U itag)
{
  	if (C_NV_FLASH_TYPE == C_SPI_FLASH)
  	{
		return RS_TABLE[itag].size;
	}
	return RS_TABLE[itag].size;
}
#endif

#endif//  if (C_NV_FLASH_TYPE == C_SPI_FLASH)

///////////////////////////////////////////////////////
//NAND Flash
///////////////////////////////////////////////////////

//retrieval from nand flash
// BOOT_AREA_START & BOOT_AREA_SIZE	are defined at L2 nand manager
#if NAND1_EN == 1

#if NVRAM_MANAGER_FORMAT == NVRAM_V3
/////////////////////////////////////////////////////////////////////////////////////////////
INT32U nvnandflash_retrieval(void)
{
   INT32U i,j,temp,ret ;
   INT16U *PageBuffer2;
   INT16U BCHblkcnt, bootblkcnt,Rspages;

   NVRAM_OS_LOCK();
   if (NandBootInit()) {
        NVRAM_OS_UNLOCK();
        return 1;
   }
   //Geting Head
   //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);
   temp =0 ;
   for (j =0 ; j<3; j++)   
   {
	   for (i =0 ; i< nand_page_nums_per_block_get(); i++)
	   {
		  ret = NandBootReadSector((j*nand_page_nums_per_block_get()*nand_sector_nums_per_page_get()) +(i*nand_sector_nums_per_page_get()),nand_sector_nums_per_page_get(),(INT32U)PageBuffer);
		  if (!ret)
		  {
		  //- Comparing NAND flash bootcode tag  NAND_FLASH_GP10_TAG
	 		  if ((gp_strncmp((INT8S *)PageBuffer, (INT8S *)NAND_FLASH_TAG,10) == 0) || (gp_strncmp((INT8S *)PageBuffer, (INT8S *)NAND_FLASH_GP10_TAG,10) == 0)) {
	 		    temp  =1;
			    break;
   			   }
   		  }
	   }   
	   if (temp ==1) break;
   }
   
   if (j== 3){
        //gp_free((void *) PageBuffer);
        NVRAM_OS_UNLOCK();
	    return 1;   
    }
   //Checking BCH and BOOT's  BLK number (by log)
   BCHblkcnt  = combin_reg_data((INT8U *)&PageBuffer[HEADER_BCH_BLK_CNT],2);
   temp = combin_reg_data((INT8U *)&PageBuffer[HEADER_OFFSET_SEC_CNT],2);        // get secter cnt
   bootblkcnt = ((temp -1) / (nand_page_nums_per_block_get() * nand_sector_nums_per_page_get()) ) +1 ; // get block cnt
   temp = combin_reg_data((INT8U *)&PageBuffer[HEADER_BOOT_BACKUP_CNT],2);
   bootblkcnt = bootblkcnt * (temp+1);
  //- Getting index position is boundary at next block
   Resource_area_position = (BCHblkcnt+bootblkcnt) * nand_page_nums_per_block_get() * nand_sector_nums_per_page_get();      
   ret = NandBootReadSector(Resource_area_position,nand_sector_nums_per_page_get(),(INT32U)PageBuffer);   
   if (ret) {
      // gp_free((void *) PageBuffer);
       NVRAM_OS_UNLOCK();
	   return 2;      
   }
	// Comparing Nand flash resource area tag
   if (gp_strncmp((INT8S *)PageBuffer , (INT8S *)RESOURCE_TAG,2) != 0) {
           // gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
			return 3;
   }
         
   total_rs = (INT16U) combin_reg_data((INT8U *)PageBuffer + 2 ,2);
   RS_TABLE_SIZE = total_rs ;   
   if (RS_TABLE !=NULL) 
   {
      gp_free((void *) RS_TABLE);
      RS_TABLE =NULL;
   }
   RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_rs+1)* sizeof(FLASHRSINDEX)));   
   Rspages = (((((total_rs*23 +4)-1) / 512)+1) -1 ) / nand_sector_nums_per_page_get() +1;
   if (Rspages >1)
   {
     //gp_free((void *) PageBuffer);
     PageBuffer2 = (INT16U *) gp_malloc(Rspages*512*nand_sector_nums_per_page_get());
     ret = NandBootReadSector(Resource_area_position,Rspages*nand_sector_nums_per_page_get(),(INT32U)PageBuffer2);   
   }
   else
   {
     PageBuffer2=PageBuffer;
   
   }
   for (i =0 ; i<total_rs+1; i++) {
     for (j =0 ; j<19; j++)
           RS_TABLE[i].name[j] = *((INT8U *)PageBuffer2 + j+4 +(23*i) );
      RS_TABLE[i].pos = (INT32U) combin_reg_data((INT8U *)PageBuffer2 +23+ (23*i),4);
   }
   // The least pos is User area position
   User_area_position = RS_TABLE[i-1].pos;
   for (i =0 ; i<total_rs; i++) {
      RS_TABLE[i].size =  RS_TABLE[i+1].pos - RS_TABLE[i].pos ;
   }
   if (Rspages >1) 
   {
         gp_free((void *) PageBuffer2);
   }
   NVRAM_OS_UNLOCK();
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
#elif 0 //! NVRAM_MANAGER_FORMAT != NVRAM_V3

INT32U nvnandflash_retrieval(void)
{
   INT8U  isector ;
   INT32U i,j,temp ;
   INT16U *PageBuffer2;
   INT16U BCHblkcnt, bootblkcnt,Rspages;

   NVRAM_OS_LOCK();
   //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);
   //Clear all Mapping Table
   for( i= 0 ; i < NAND_TABLE_SIZE ;i++)
   {
   	   ND_MAPTABLE[i] = 0xFFFF;
   }
   // Good Blocks Mapping Table
   temp =0;

   for (i = BOOT_AREA_START ; i < BOOT_AREA_SIZE ;i++ )
   {
 	 if ( (GetBadFlagFromNand(i) & 0xff) != 0)
 	 {
 	   ND_MAPTABLE[temp] =i;
 	   temp++;
 	 }
   }

   //Find out Head at HEAD +BCH area
   //-find tag within one block.... only one is enough
   temp= ND_MAPTABLE[0];
   temp =temp *nand_page_nums_per_block_get();

   for (i =0 ; i< nand_page_nums_per_block_get(); i++)
   {

	   Nand_ReadPhyPage(temp+i,(INT32U)PageBuffer);
	  //- Comparing NAND flash bootcode tag
	  if ((gp_strncmp((INT8S *)PageBuffer, (INT8S *)NAND_FLASH_TAG,10) == 0) || (gp_strncmp((INT8S *)PageBuffer, (INT8S *)NAND_FLASH_GP10_TAG,10) == 0)) {
	    break;
   	 }
   }

   if (i==nand_page_nums_per_block_get())
   {
        //gp_free((void *) PageBuffer);
        NVRAM_OS_UNLOCK();
		return 2;
   }
   //Checking BCH and BOOT's  BLK number
   BCHblkcnt  = combin_reg_data((INT8U *)&PageBuffer[HEADER_BCH_BLK_CNT],2);
   temp = combin_reg_data((INT8U *)&PageBuffer[HEADER_OFFSET_SEC_CNT],2);        // get secter cnt
   bootblkcnt = ((temp -1) / (nand_page_nums_per_block_get() * nand_sector_nums_per_page_get()) ) +1 ; // get block cnt
   temp = combin_reg_data((INT8U *)&PageBuffer[HEADER_BOOT_BACKUP_CNT],2);
   bootblkcnt = bootblkcnt * (temp+1);

  //- Getting index position is boundary at next block
     //Resource_area_position = ND_MAPTABLE[BCHblkcnt+bootblkcnt] * nand_page_nums_per_block_get() * nand_sector_nums_per_page_get();
   Resource_area_position = (BCHblkcnt+bootblkcnt) * nand_page_nums_per_block_get() * nand_sector_nums_per_page_get();
   temp=  ND_MAPTABLE[BCHblkcnt+bootblkcnt];
   temp = temp * nand_page_nums_per_block_get() ; // block addr by page
   isector = 0;
   Nand_ReadPhyPage(temp,(INT32U)PageBuffer);
    // Comparing Nand flash resource area tag
   if (gp_strncmp((INT8S *)PageBuffer + 512*isector, (INT8S *)RESOURCE_TAG,2) != 0) {
            //gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
			return 2;
   }

   total_rs = (INT16U) combin_reg_data((INT8U *)PageBuffer + 512*isector +2 ,2);
   RS_TABLE_SIZE = total_rs ;   
   if (RS_TABLE !=NULL) 
   {
      gp_free((void *) RS_TABLE);
      RS_TABLE =NULL;
   }
   RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_rs+1)* sizeof(FLASHRSINDEX)));   
#if 0
   for (i =0 ; i<total_rs+1; i++) {
     for (j =0 ; j<8; j++)
           RS_TABLE[i].name[j] = *((INT8U *)PageBuffer + j+4 +(10*i)+ (512*isector) );
      RS_TABLE[i].pos = (INT32U) combin_reg_data((INT8U *)PageBuffer +12+ (10*i)+ (512*isector),2);
   }
#else
   Rspages = (((((total_rs*23 +4)-1) / 512)+1) -1 ) / nand_sector_nums_per_page_get() +1;
   if (Rspages >1)
   {
     //gp_free((void *) PageBuffer);
     PageBuffer2 = (INT16U *) gp_malloc(Rspages*512*nand_sector_nums_per_page_get());
     for (i =0 ; i<Rspages; i++) 
     {Nand_ReadPhyPage(temp+i,(INT32U)(PageBuffer2+256*nand_sector_nums_per_page_get()*i)); }     
   }
   else
   {
     PageBuffer2=PageBuffer;
   
   }
   for (i =0 ; i<total_rs+1; i++) {
     for (j =0 ; j<19; j++)
           RS_TABLE[i].name[j] = *((INT8U *)PageBuffer2 + j+4 +(23*i)+ (512*isector) );
      RS_TABLE[i].pos = (INT32U) combin_reg_data((INT8U *)PageBuffer2 +23+ (23*i)+ (512*isector),4);
   }
#endif
   // The least pos is User area position
   User_area_position = RS_TABLE[i-1].pos;
   for (i =0 ; i<total_rs; i++) {
      RS_TABLE[i].size =  RS_TABLE[i+1].pos - RS_TABLE[i].pos ;
   }
   if (Rspages >1)
   {
      gp_free((void *) PageBuffer2);
    }
   NVRAM_OS_UNLOCK();
   return 0;
}
#endif//#ifdef NVRAM_V3


#if NVRAM_MANAGER_FORMAT == NVRAM_V3
//////////////////////////////////////////////////////////////////////////////////////////
INT32U nvnandflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count)
{
   INT32U ret;
   INT16U max_sec_count,read_sec_count;
   INT32U start_addr_sec; 

   NVRAM_OS_LOCK();
   //ALIGN16 INT16U    PageBuffer[1056];
   if ( itag > RS_TABLE_SIZE )  {NVRAM_OS_UNLOCK();return 1;}
   max_sec_count  =  RS_TABLE[itag].size;
   start_addr_sec =  RS_TABLE[itag].pos   + Resource_area_position;
      //Add offset
   start_addr_sec += offset_secter;
   read_sec_count = ( (max_sec_count-offset_secter) < secter_count) ?  (max_sec_count-offset_secter) : secter_count ;   

   ret = NandBootReadSector(start_addr_sec,read_sec_count,(INT32U)pbuf);   
   NVRAM_OS_UNLOCK();
   if (ret) {
	   return 1;
   } else {
	   return 0;
   }
}

//secter_count must be fixed
//pbuf_align_16 must been align 16 bytes
INT32U nvnandflash_user_get(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count)
{
   INT32U ret,size;
   NVRAM_OS_LOCK();
   
   //ret = NandBootReadSector((User_area_position  +Resource_area_position),secter_count,(INT32U)pbuf_align_16);   
   if (itag==0)
     ret= DrvNand_read_sector(0,1,(INT32U)pbuf_align_16);   
   else
     ret= DrvNand_read_sector(itag,secter_count,(INT32U)pbuf_align_16);   
   if (itag==1)     
   {
     size = (pbuf_align_16[127] & 0xff000000)>>24;     
     ret= DrvNand_read_sector(itag,size,(INT32U)pbuf_align_16);    
   }
   NVRAM_OS_UNLOCK();
   if (ret) {
	   return 1;
   } else {
	   return 0;
   }

}

//secter_count must be fixed
//pbuf must been align 16 bytes
INT32U nvnandflash_user_set(INT16U itag ,INT32U *pbuf_align_16 , INT16U secter_count)
{
   INT32U ret;
   //INT8U  read_buffer[512];
   
   NVRAM_OS_LOCK();
   //NandBootEnableWrite();
  //ret = NandBootWriteSector((User_area_position  +Resource_area_position),secter_count,(INT32U)pbuf_align_16);   
   if (itag==0)
	   ret= DrvNand_write_sector(0,1,(INT32U)pbuf_align_16);   
   else
       ret= DrvNand_write_sector(itag,secter_count,(INT32U)pbuf_align_16);   
   
   DrvNand_flush_allblk();
   //dret = DrvNand_read_sector(0,secter_count,(INT32U)read_buffer);
   
   //NandBootFlush();
   //NandBootDisableWrite();
   if (ret){
       NVRAM_OS_UNLOCK();
	   return 1;
   } else {
       //NandBootFlush();
       //DrvNand_flush_allblk();
       NVRAM_OS_UNLOCK();
	   return 0;
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
#else  // !NVRAM_V3

INT32U nvnandflash_rs_get(INT16U itag ,INT32U offset_secter, INT32U *pbuf , INT16U secter_count)
{
#if (defined _DRV_L1_DMA) && (_DRV_L1_DMA == 1)
   DMA_STRUCT dma_struct;
#else
   INT16U j;
#endif
   INT16U i,isector,ipage;
   INT16U max_sec_count,read_sec_count;
   INT32U start_addr_sec;
   INT32U temp,pbufindx;
   //INT16U *PageBuffer;
   NVRAM_OS_LOCK();
   //ALIGN16 INT16U    PageBuffer[1056];
   if ( itag > RS_TABLE_SIZE )  {NVRAM_OS_UNLOCK();return 1;}

   //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);
   // checking rs tage
  // for (i =0 ; i<RS_TABLE_SIZE; i++) {
  //    if (RS_TABLE[i].tag == itag )
  //      break;
  // }
   max_sec_count  =  RS_TABLE[itag].size;
   start_addr_sec =  RS_TABLE[itag].pos   + Resource_area_position;
      //Add offset
   start_addr_sec += offset_secter;
   read_sec_count = ( (max_sec_count-offset_secter) < secter_count) ?  (max_sec_count-offset_secter) : secter_count ;

   temp= ND_MAPTABLE[start_addr_sec / (nand_page_nums_per_block_get() * nand_sector_nums_per_page_get()) ];
   if (temp ==0xFFFF ) return 1;
   temp = temp * nand_page_nums_per_block_get(); // block addr by page
   temp = temp | ( (start_addr_sec /nand_sector_nums_per_page_get() ) & (nand_page_nums_per_block_get()   -1)) ;
   isector = start_addr_sec % nand_sector_nums_per_page_get();
   //Start
   pbufindx =0;
   if (isector > 0 )
   {
    Nand_ReadPhyPage(temp,(INT32U)PageBuffer);
#if (defined _DRV_L1_DMA) && (_DRV_L1_DMA == 1)
    if ( secter_count <= (nand_sector_nums_per_page_get()-isector))
        dma_struct.count = secter_count*128;
    else
	    dma_struct.count = (INT32U) (nand_sector_nums_per_page_get()-isector)*128;    
	dma_struct.s_addr = (INT32U) PageBuffer + (isector*512);
	dma_struct.t_addr = (INT32U) pbuf;
	dma_struct.width = DMA_DATA_WIDTH_4BYTE;
	dma_struct.timeout = 64; 
	if (dma_transfer_wait_ready(&dma_struct))
    {
        NVRAM_OS_UNLOCK();
	    return -1;
    }
    pbufindx    +=256*(nand_sector_nums_per_page_get()-isector);
    if ( secter_count <= (nand_sector_nums_per_page_get()-isector))
	{
        //gp_free((void *) PageBuffer);
        NVRAM_OS_UNLOCK();
        return 0;
    }
    secter_count-=(nand_sector_nums_per_page_get()-isector);
#else
    for(i =0; i < (nand_sector_nums_per_page_get()-isector);i++)
    {
     	  for(j=0;j<256;j++) //512 = (128 x 4 ) bytes
      	  {
	      		*((INT16U*)pbuf+j+i*256)= PageBuffer[j+(isector+i)*256];
    	  }
	      pbufindx +=256;
    	  secter_count--;
	      if ( secter_count ==0)
	      {
      	 		//gp_free((void *) PageBuffer);
                NVRAM_OS_UNLOCK();
      		 	return 0 ;
    	  }
    }
#endif
    temp++;
   }
   //Mid
   ipage   = secter_count / nand_sector_nums_per_page_get();
   isector = secter_count % nand_sector_nums_per_page_get();
   for(i =0; i < ipage ; i++)
   {
    if ((temp & (nand_page_nums_per_block_get()-1)) ==0) //if new block
    {
        temp= ND_MAPTABLE[temp/ (nand_page_nums_per_block_get())];
        if (temp ==0xFFFF ) {
         //gp_free((void *) PageBuffer);
         NVRAM_OS_UNLOCK();
         return 2;
        }
        temp = temp * nand_page_nums_per_block_get(); // block addr by page
    }
    Nand_ReadPhyPage(temp,(INT32U)( (INT16U *)pbuf+pbufindx));
    pbufindx +=nand_sector_nums_per_page_get()*256 ;//1024; //2048  =(4 x 128)
    temp++;
   }
   //End
   if (isector >0)
   {
      if ((temp & (nand_page_nums_per_block_get()-1)) ==0) //if new block
      {
        temp= ND_MAPTABLE[temp/ (nand_page_nums_per_block_get())];
        if (temp ==0xFFFF ) {
         //gp_free((void *) PageBuffer);
         NVRAM_OS_UNLOCK();
         return 2;
         }
        temp = temp * nand_page_nums_per_block_get(); // block addr by page
      }
      Nand_ReadPhyPage(temp,(INT32U)PageBuffer);
#if (defined _DRV_L1_DMA) && (_DRV_L1_DMA == 1)
 	  dma_struct.s_addr = (INT32U) PageBuffer;
	  dma_struct.t_addr = (INT32U) pbuf + (pbufindx<<1);
	  dma_struct.width = DMA_DATA_WIDTH_4BYTE;
	  dma_struct.count = (INT32U) (isector*128);
	  dma_struct.timeout = 64;
	  if (dma_transfer_wait_ready(&dma_struct))
      {
        NVRAM_OS_UNLOCK();
	    return -1;
      }
#else
      for(i =0; i < isector ;i++)
      {
       for(j=0;j<256;j++) //512 = (128 x 4 ) bytes
         *((INT16U*)pbuf +j+ pbufindx)= PageBuffer[j+i*256];
       pbufindx +=256;
      }
#endif
   }
   //gp_free((void *) PageBuffer);
   NVRAM_OS_UNLOCK();
   return 0;
}

INT16U USE_LINK_TBL[10];
INT8U  USE_WRITE_TAG[10];
//secter_count must be fixed
//pbuf_align_16 must been align 16 bytes
INT32U nvnandflash_user_get(INT32U *pbuf_align_16 , INT16U secter_count)
{
   INT16U i,j;
   INT32U user_start_link_blkadr,user_org_blkadr;
   INT16U user_blk_count;
   INT16U lbamark,bchmark,firstblk,firstflag,allblkempty;
   INT16U ipage,isector;
   INT32U pbufindx,current_page_adr,user_current_blk_adr;
   INT32U ret;
   //ALIGN16 INT16U   PageBuffer[1056];
   //INT16U *PageBuffer;
   
   NVRAM_OS_LOCK();
   //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);

//   if (secter_count <0) return 1;

   //Clear all user mapping blocks
   for(i=0; i<10; i++)
   {
     USE_LINK_TBL[i]  = 0xFFFF;
     USE_WRITE_TAG[i] = 0;
   }
   //1. Find first block addres  & buid mapping table from anothers
   user_start_link_blkadr = (User_area_position  +Resource_area_position)/ (nand_page_nums_per_block_get() * nand_sector_nums_per_page_get());
   user_org_blkadr = user_start_link_blkadr;
   //If location not at starting of the block
   if ((( (User_area_position +Resource_area_position) / nand_sector_nums_per_page_get() ) & (nand_page_nums_per_block_get()  -1)) !=0)
       user_start_link_blkadr++;
   for(i=0; i<10; i++)
   {
     if (ND_MAPTABLE[user_start_link_blkadr] != 0xFFFF)
          USE_LINK_TBL[i] = ND_MAPTABLE[user_start_link_blkadr];
     else
         break;
       user_start_link_blkadr++;
   }
   user_blk_count = i;
   //2. Searching each user blocks to find out current data loaction by first page
   firstflag =1;
   allblkempty  =1;

   for(i=0; i<user_blk_count; i++)
   {
     ret = Nand_ReadPhyPage( USE_LINK_TBL[i] * nand_page_nums_per_block_get(),(INT32U)PageBuffer);
     if(ret)
     {
        //gp_free((void *) PageBuffer);
        NVRAM_OS_UNLOCK();
        return 2;
     }
     lbamark=combin_reg_data((INT8U *)&PageBuffer + 2050,2);
     bchmark=combin_reg_data((INT8U *)&PageBuffer + 2059,2);
     if ((lbamark = 0xFFFF) && (bchmark == 0xFFFF)) //iF a empty block
     {
        USE_WRITE_TAG[i] = 1;
     }else
     {
        if (firstflag) //the first nonempty block
        {
          firstblk  =i;
          firstflag =0;
        }
        allblkempty  =0;
     }
   }

   if(allblkempty ==1) //get orginal blk
      user_current_blk_adr = ND_MAPTABLE[user_org_blkadr] * nand_page_nums_per_block_get();
   else               //get the current block
      user_current_blk_adr = USE_LINK_TBL[firstblk] * nand_page_nums_per_block_get() ;

   //3. Searching each pages within block (back tracing)
 
   for(i=nand_page_nums_per_block_get() -1 ; i>0; i--) //find the last botton nonempty page in the block
   {
     ret = Nand_ReadPhyPage( user_current_blk_adr +i,(INT32U)PageBuffer);
     if(ret)
     if(ret) {
       //gp_free((void *) PageBuffer);
       NVRAM_OS_UNLOCK();
       return 3;
      }
     lbamark=combin_reg_data((INT8U *)&PageBuffer + 2050,2);
     bchmark=combin_reg_data((INT8U *)&PageBuffer + 2059,2);
    if ((lbamark != 0xFFFF) || (bchmark != 0xFFFF))
        break;
   }

   current_page_adr = user_current_blk_adr +i; //storage last page address
   //4. Reading User data
   ipage   = secter_count / nand_sector_nums_per_page_get();
   isector = secter_count % nand_sector_nums_per_page_get();
   pbufindx = ipage*nand_sector_nums_per_page_get()*128;
   //end part
   if (isector == 0)
   {
     pbufindx = pbufindx - nand_sector_nums_per_page_get()*128;
     isector =4;
     ipage--; //left page --
   }
   for(i =0; i < isector ;i++)
   {
     for(j=0;j<256;j++) //512 = (256 x 2 ) bytes
          *((INT16U*)pbuf_align_16 +j+ pbufindx)= PageBuffer[j+i*256];
      pbufindx +=256;
   }
    //mid part
   if (ipage > 0) //left page
   {
      current_page_adr = current_page_adr - ipage ;
      pbufindx = 0 ;

	 for(i =0; i < ipage ;i++)
	 {   
     	 ret = Nand_ReadPhyPage(current_page_adr,(INT32U)((INT16U*)pbuf_align_16+pbufindx));
     	 if(ret) {
     	    //gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
            return 4;
         }
	     pbufindx +=512;
	     current_page_adr++;
   	 }
   }
   //gp_free((void *) PageBuffer);
   NVRAM_OS_UNLOCK();
   return 0;
}

//secter_count must be fixed
//pbuf must been align 16 bytes
INT32U nvnandflash_user_set(INT32U *pbuf_align_16 , INT16U secter_count)
{
   INT16U i;
   INT32U user_start_link_blkadr,user_org_blkadr;
   INT16U user_blk_count;
   INT16U lbamark,bchmark,firstblk,firstflag,allblkempty;
   INT32U current_page_adr,user_current_blk_adr;

   INT16U ipage ,left_page;
   INT32U ret;

   //
   // INT16U *PageBuffer;

   NVRAM_OS_LOCK();
   
   //PageBuffer = (INT16U *) gp_malloc_align(2112, 16);//ALIGN16 INT16U   PageBuffer[1056];
//   if (secter_count <0) return 1;
   //Clear all user mapping blocks
   for(i=0; i<10; i++)
   {
     USE_LINK_TBL[i]  = 0xFFFF;
     USE_WRITE_TAG[i] = 0;
   }
   //1. Find first block addres  & buid mapping table
   user_start_link_blkadr = (User_area_position +Resource_area_position) / (nand_page_nums_per_block_get() * nand_sector_nums_per_page_get());
   user_org_blkadr = user_start_link_blkadr;
   if ((( (User_area_position +  Resource_area_position)  / nand_sector_nums_per_page_get() ) & (nand_page_nums_per_block_get()  -1)) !=0)
       user_start_link_blkadr++;
   for(i=0; i<10; i++)
   {
     if (ND_MAPTABLE[user_start_link_blkadr] != 0xFFFF)
          USE_LINK_TBL[i] = ND_MAPTABLE[user_start_link_blkadr];
     else
         break;
       user_start_link_blkadr++;
   }
   user_blk_count = i;
   //2. Searching each user blocks to find out current data loaction by first page
   firstflag =1;
   allblkempty  =1;

   for(i=0; i<user_blk_count; i++)
   {
     ret =Nand_ReadPhyPage( USE_LINK_TBL[i] * nand_page_nums_per_block_get(),(INT32U)PageBuffer);

     if(ret) {
            //gp_free((void *) PageBuffer);
            NVRAM_OS_UNLOCK();
            return 2;
     }
     lbamark=combin_reg_data((INT8U *)&PageBuffer + 2050,2);
     bchmark=combin_reg_data((INT8U *)&PageBuffer + 2059,2);
     if ((lbamark = 0xFFFF) && (bchmark == 0xFFFF)) //iF a empty block
     {
        USE_WRITE_TAG[i] = 1;
     }else
     {
        if (firstflag) //the first nonempty block
        {
          firstblk  =i;
          firstflag =0;
        }
        allblkempty  =0;
     }
   }

   if(allblkempty ==1) {//get orginal blk
      user_current_blk_adr = ND_MAPTABLE[user_org_blkadr] * page_nums_per_block;}
   else{
                       //get the current block
      user_current_blk_adr = USE_LINK_TBL[firstblk] * page_nums_per_block;}

   //3. Searching each pages within block

   for(i=page_nums_per_block -1 ; i>0; i--) //find the last botton nonempty page in the block
   {

     ret = Nand_ReadPhyPage( user_current_blk_adr +i,(INT32U)PageBuffer);

     if(ret) {
       //gp_free((void *) PageBuffer);
       NVRAM_OS_UNLOCK();
       return 4;
     }
     lbamark=combin_reg_data((INT8U *)&PageBuffer +2050,2);
     bchmark=combin_reg_data((INT8U *)&PageBuffer +2059,2);
    if ((lbamark != 0xFFFF) || (bchmark != 0xFFFF))
        break;
   }

   current_page_adr = user_current_blk_adr +i; //storage last page address

   //4.checkif enough space to program
   left_page = page_nums_per_block -(current_page_adr - user_current_blk_adr)-1;
   if ( secter_count <= (left_page <<2) ) //write to current block
   {
     user_current_blk_adr = current_page_adr + 1;
   }else //change current block to another block
   {
     //erase current block and checking bad block
     if (!allblkempty)
     {
     	ret = Nand_ErasePhyBlock(user_current_blk_adr);
	    if(ret & 0x1) //check bad block
	    {
			//Erase fail , do not put this block into recycle Fifo.
            *(INT16U*)&(PageBuffer[1024]) = 0xff42;
         	*(INT16U*)&(PageBuffer[1025]) = 0xff42;
          	*(INT16U*)&(PageBuffer[1026]) = 0xff42;
         	*(INT16U*)&(PageBuffer[1027]) = 0xff42;
         	*(INT16U*)&(PageBuffer[1028]) = 0xff42;
            
         	ret = Nand_WritePhyPage(user_current_blk_adr + page_nums_per_block -1, (INT32U)PageBuffer); // Last page
            if(ret)
			{
				ret = Nand_WritePhyPage(user_current_blk_adr + page_nums_per_block -3,(INT32U)PageBuffer); // Last-2  page
                if(ret) {
				  //gp_free((void *) PageBuffer);
                  NVRAM_OS_UNLOCK();
                  return 5;
                }
			}
     	}
     }
     //find next empty block
     if (firstblk != user_blk_count-1)
      firstblk++;
     else
      firstblk=0;
     user_current_blk_adr = USE_LINK_TBL[firstblk] * page_nums_per_block ;
   }
   //5. programming data
   *(INT16U*)&(PageBuffer[1024]) = 0xFFFF;
   *(INT16U*)&(PageBuffer[1025]) = 0x55AA;
   *(INT16U*)&(PageBuffer[1026]) = 0xFFFF;
   *(INT16U*)&(PageBuffer[1027]) = 0xFFFF;
   *(INT16U*)&(PageBuffer[1028]) = 0xFFFF;
   ipage   = secter_count / nand_sector_nums_per_page_get();
   if ((secter_count % nand_sector_nums_per_page_get()) > 0) ipage++;

   for (i = 0; i< ipage; i++)
   {
     ret = Nand_WritePhyPage(user_current_blk_adr + i ,(INT32U) (pbuf_align_16 + i*128));
 	 if(ret) {
 	   //gp_free((void *) PageBuffer);
       NVRAM_OS_UNLOCK();
       return 6;
     }
   }   
  //gp_free((void *) PageBuffer);
  NVRAM_OS_UNLOCK();
  return 0 ;
}



#endif//#ifdef NVRAM_V3

INT32S nv_fast_read(INT16S fd, INT8U* buf, INT32U byte_size)
{
	INT32U addr;
	addr = (RS_TABLE[fd].pos+Resource_area_position)*512;
	return SPI_Flash_read(addr, buf, byte_size);
}

#endif//NAND1_EN == 1
//=== This is for code configuration DON'T REMOVE or MODIFY it ================//
#endif //(defined GPLIB_NVRAM_SPI_FLASH_EN) && (GPLIB_NVRAM_SPI_FLASH_EN == 1) //
//=============================================================================//
