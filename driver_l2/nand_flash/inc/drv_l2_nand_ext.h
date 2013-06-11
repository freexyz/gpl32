#ifndef DRV_L2_NAND_EXT_H
#define DRV_L2_NAND_EXT_H

/************* Common for GPL326XXB and GPL326XXA ************************/
#include "project.h"
#define GP_NAND_MAX_PAGE_SIZE (8 * 1024)
#define GP_NAND_MAX_SPARE_SIZE 1024

#if _OPERATING_SYSTEM != _OS_NONE
#include "os.h"
#endif


#if (defined _OPERATING_SYSTEM) && (_OPERATING_SYSTEM != _OS_NONE)
extern  OS_EVENT * gNand_sem;
extern  OS_EVENT * gNand_PHY_sem;
#endif

#ifndef NAND_READ_ALLOW
#define NAND_READ_ALLOW	    0x1 //partition mode
#endif  //NAND_READ_ALLOW

#ifndef NAND_WRITE_ALLOW
#define NAND_WRITE_ALLOW	0x2 //partition mode
#endif //NAND_WRITE_ALLOW

#define HEADER_OFFSET_APP_FLAG 					0x1c0	/*4 bytes*/
#define HEADER_OFFSET_APP_START 				0x1c4	/*1 bytes*/
#define HEADER_OFFSET_APP_SIZE  				0x1c5	/*2 bytes*/
#define HEADER_OFFSET_APP_PRENT 				0x1c7	/*1 bytes*/


#define HEADER_OFFSET_USER_SETTING_SIZE_BLK		0x1d0	/*4 bytes*/
#define HEADER_OFFSET_PARTITION_NUM				0x1d7	/*1 bytes*/ 	//0,1,2
#define HEADER_OFFSET_NAND_PART0_SIZE_BLK   	0x1d8   /*4 bytes*/		//size = 0:auto	
#define HEADER_OFFSET_NAND_PART1_SIZE_BLK		0x1dc   /*4 bytes*/		//size = 0:auto
#define HEADER_OFFSET_NAND_PART0_ATTRIBUTE   	0x1e3   /*1 bytes*/		//1,2,3
#define HEADER_OFFSET_NAND_PART1_ATTRIBUTE   	0x1e7   /*1 bytes*/		//1,2,3

#define HEADER_BANK_BLOCKS   					0x1e8	/*4 bytes*/	
#define HEADER_RECYCLE_BLOCKS					0x1ec	/*4 bytes*/	

#define HEADER_NAND_MB_SIZE                     0x1f8   /*4 bytes*/
#define HEADER_MAX_MCLK_MHz                     0x1fe   /*1 byte*/
#define HEADER_OFFSET_WP_PIN_ID                 0x1ff   /*1 byte*/

#define GPL6_BOOTAREA_BCH_SET_PAGE_NUMS		4	
#define GPL6_BOOTAREA_BCH_CHG_CYCLE		    (GPL6_BOOTAREA_BCH_SET_PAGE_NUMS*2)

extern void Nand_OS_LOCK(void);
extern void Nand_OS_UNLOCK(void);

extern INT32U Nand_part0_size_Get(void);
extern INT32U NandAppByteSizeGet(void);
extern INT32U NandBootAreaByteSizeGet(void);
extern INT32U CalculateFATArea(void);
extern INT16U GetBadFlagFromNand(INT16U wPhysicBlkNum);
extern INT32U nand_block_checkbad(INT32U block_id, INT8U* data_buf);

//APP-Zone API
extern void   NandBootEnableWrite(void);
extern void   NandBootDisableWrite(void);
extern INT32U NandBootFormat(INT16U format_type);
extern INT32U NandBootFlush(void);
extern INT32U NandBootInit(void);
extern INT32U NandBootWriteSector(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootReadSector(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);

//Data-Zone API
extern INT16U DrvNand_initial(void);
extern INT16U DrvNand_lowlevelformat(void);
extern INT32U DrvNand_get_Size(void);
extern INT16U DrvNand_write_sector(INT32U , INT16U , INT32U );
extern INT16U DrvNand_read_sector(INT32U , INT16U , INT32U );
extern INT16U DrvNand_flush_allblk(void);

extern void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode);

extern void Nand_part0_Offset_Set(INT32U nand_fs_sector_offset);
extern void Nand_part0_mode_Set(INT16U mode);
extern void Nand_part0_size_Set(INT32U nand_fs_sector_size);
extern void Nand_part1_Offset_Set(INT32U nand_fs_sector_offset);
extern void Nand_part1_mode_Set(INT16U mode);
extern void Nand_part1_size_Set(INT32U nand_fs_sector_size);

extern INT32U Nand_part1_Offset_Get(void);
extern INT32U Nand_part1_mode_Get(void);
extern INT32U Nand_part1_size_Get(void);

extern INT32U Nand_part0_Offset_Get(void);
extern INT32U Nand_part0_mode_Get(void);

extern void   Nand_Partition_Num_Set(INT16U number);
extern INT16U Nand_Partition_Num_Get(void);

extern INT32S NandInfoAutoGet(void);
extern INT32U DrvNand_bchtable_alloc(void); //not implemented
extern void DrvNand_bchtable_free(void); //not implemented

/************* Specific for GPL326XXB *************************************/
#ifdef GPL326XXB_BCH60//GPL326XXB

#define GP_NAND_MAX_PAGE_SIZE (8 * 1024)
#define GP_NAND_MAX_SPARE_SIZE 1024

extern void setSize4Partition(INT8U partitionIndex, INT32U sizeInSector);
extern INT32U getSize4Partition(INT8U partitionIndex);

extern void setMode4Partition(INT8U partitionIndex, INT8U mode);
extern INT8U getMode4Partition(INT8U partitionIndex);

extern INT32U nand_sector_nums_per_page_get(void);
extern INT32U nand_page_nums_per_block_get(void);
#endif //GPL326XXB

/************* Specific for GPL326XXA ************************************/

#ifndef GPL326XXB_BCH60 //GPL326XXA
#ifndef GPL32_GP10
#define GPL32_GP10	0x10
#endif

#ifndef _TAG_ENUM
#define _TAG_ENUM
typedef enum {
    TAG_GPL325XX=6,
    TAG_GPL326XX=10
} TAG_ENUM;
#endif


#define APP_PART_TAG		0x5a5a5a5a
#define NAND_FLASH_GP6_TAG  "PGandnandn"
#define NAND_FLASH_GP10_TAG "GPNandTag2"
#define NAND_ROM_GP10_TAG   "GPNandTag2R"
#define MAX_GOOD_HEADER_BLKS            20
#define NAND_HEADER_MAX_BLOCKS          MAX_GOOD_HEADER_BLKS
#define HEADER_OFFSET_NAND_ROM          10
#define HEADER_OFFSET_BCH_ENABLE        36
#define HEADER_OFFSET_BCH_BIT           37
#define HEADER_OFFSET_ND_DEFINE         18
#define HEADER_OFFSET_ND_PAGE           20
#define HEADER_OFFSET_ND_ADDR_CYC       21
#define HEADER_OFFSET_ND_BLOCK          22
#define HEADER_DENSIZTY_CODE_PAGES      23
#define HEADER_GP6_DENSIZTY_CODE_PAGES  10
#define HEADER_OFFSET_ND_HASH_KEY       26  /*2 Bytes*/

#define HEADER_OFFSET_NEXT_GOOD_BLK_CNT  	24
#define HEADER_OFFSET_BOOTLOADSEC_CNT       16
#define HEADER_OFFSET_BCHPAGE_CNT      		18
#define HEADER_OFFSET_BOOT_BACKUP_NUM  		38
#define HEADER_OFFSET_NF_START_RUN_ADDR     40

//#define HEADER_OFFSET_SEC_CNT       16
#define HEADER_OFFSET_SEC_CNT       8

extern void bch_polling_exchange(void);
extern INT8U nand_tag_match_querry(INT8U *page_buf);
extern INT8S nand_option_table_detect(INT8U* head_data);
extern TAG_ENUM nand_header_read(INT8U* data_buf);
extern INT32U nand_fs_byte_size_get(void);
extern void SetNandRomAppAreaStartAddr(INT32U AppAreaStartAddr);
extern INT32U GetNandRomAppAreaStartAddr(void);
extern void SetNandRomDataAreaStartAddr(INT32U DataAreaStartAddr);
extern INT32U GetNandRomDataAreaStartAddr(void);
extern INT32U Nand_libraryversion(void);
extern INT32U DrvNand_BadBlk(INT16U mode);
extern INT32U DrvNand_RecoveryBlk(void);
extern void   DrvNand_Save_info(INT16U *Addr);
extern INT32U DrvNand_Get_info(INT16U *Addr);
extern void SetNandInfo(INT16U app_start,INT16U app_size,INT16U app_Percent,  INT16U data_BankSize, INT16U data_BankRecSize);
extern void Nand_FS_Offset_Set(INT32U nand_fs_sector_offset);
extern INT32U Nand_FS_Offset_Get(void);
extern INT32U NandAppByteSizeGet(void);
extern INT32U NandBootAreaByteSizeGet(void);
extern INT8U nf_rom_flag_get(void);
extern INT8U nf_otp_bch4_flag_get(void);
#endif //GPL326XXA

#endif //DRV_L2_NAND_EXT_H