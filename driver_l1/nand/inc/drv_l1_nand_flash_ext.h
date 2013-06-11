#ifndef DRV_L1_NAND_FLASH_EXT_H
#define DRV_L1_NAND_FLASH_EXT_H

/*******************  For Both GPL326XXA and GPL326XXB *************************/
#include "project.h"

#ifndef _BCH_MODE_ENUM_
#define _BCH_MODE_ENUM_
typedef enum
{
#ifdef GPL326XXB_BCH60
    BCH1K60_BITS_MODE=0x00, 
    BCH1K40_BITS_MODE=0x01, 
    BCH1K24_BITS_MODE=0x02,  
    BCH1K16_BITS_MODE=0x03,  
    BCH512B8_BITS_MODE=0x04, 
    BCH512B4_BITS_MODE=0x05,
#else
    BCH_4_BITS_MODE=0x0,
    BCH_8_BITS_MODE=0x2,
    BCH_12_BITS_MODE=0x800,
    BCH_AUTO_MODE=0x1000,
    BCH_MODE_MAX,
#endif	
    BCH_OFF=0xFF       
} BCH_MODE_ENUM;
#endif //_BCH_MODE_ENUM_

#ifndef _NAND_STATUS_ENUM_
#define _NAND_STATUS_ENUM_
typedef enum 
{
	NAND_TRUE=0,
	NAND_OK=0,
	NAND_ROM_FIND=1,
	NAND_UNKNOWN=2,
	NAND_FAIL=3,
	NAND_PAD_FAIL=4,
	NAND_DMA_ALIGN_ADDRESS_NEED,
	NAND_DMA_LENGTH_ERROR,
	NAND_BCH_LENGTH_ERROR,
	NAND_PAGE_USED,
	NAND_BLOCK_BREAK,
    NAND_BLOCK_VALID_PAGE_FULL,
	NAND_BIT_ERROR_CORRECTED,
	NAND_PROGRAM_FAIL,
	NAND_ERASE_FAIL,
	NAND_INTER_MOVE_NOT_SAME_PLANE,
	NAND_INTER_MOVE_ADDR_MOD_2_DIFF,
	NAND_NOT_SUPPORT_OVER_BLOCK_PAGE_PROG,
	NAND_TIMEOUT,
	NAND_BCH_ERROR,
	NAND_STATUS_MAX
} NAND_STATUS_ENUM; 
#endif

#ifndef NAND_SHARE_MODE_DEF
#define NAND_SHARE_MODE_DEF
typedef enum
{
    NF_SHARE_PIN_MODE,
    NF_NON_SHARE_MODE
} NAND_SAHRE_MODE_ENUM;
#endif

extern INT8U nf_otp_bch4_flag_get(void);
extern INT32U DrvL1_Nand_Init(void);

extern INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);

/* *****************   Specific for GPL326XXB  *************************************/
#ifdef GPL326XXB_BCH60 //GPL326XXB

#define NAND_ORG_BAD_TAG					0x00
#define NAND_GOOD_TAG						0xff

#define UINT32 INT32U
#define UINT16 INT16U
#define UINT8  INT8U
#define SINT32 INT32S
#define SINT16 INT16S
#define SINT8  INT8S

#define NAND_MLC									0xf1
#define NAND_SLC_LARGE_BLK							0xf2
#define NAND_SLC_SMALL_BLK							0xf3

#ifndef _NFIO_CFG
#define _NFIO_CFG
typedef enum {
    NAND_IO_IOI_IOF   =0x00,  
    NAND_IO_IOB_IOC   =0x01,  
    NAND_IO_IOA_IOC   =0x02,  
    NAND_IO_IOB_IOG   =0x03,   
    NAND_IO_IOA_IOG   =0x04, 
    NAND_IO_IOI_IOG   =0x05,
    NAND_IO_AUTO_SCAN =0xFF
} NFIO_CFG;
#endif

extern void nand_iopad_sel(NFIO_CFG nand_io_sel);
extern void nand_share_mode_reg(NAND_SAHRE_MODE_ENUM share_or_nonshare);
extern void nf_otp_bch4_flag_set(INT8U nand_otp_bch4_en);
extern INT32U nand_block_byte_size_get(void);
extern void Nand_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum,INT32U* NandID);
extern INT8U good_block_check(INT16U blk_id, INT32U WorkBuffer);
extern SINT32 NandSetBootHeader(UINT8 *Buffer);
extern INT16U GetNandType(void);
extern UINT16 GetAppStartBlkFromBth(void);
extern UINT16 GetAppPercentFromBth(void);
extern UINT16 GetDataBankLogicSizeFromBth(void);
extern UINT16 GetDataBankRecycleSizeFromBth(void);
extern UINT32 GetNoFSAreaSectorSizeFromBth(void);
extern UINT8 GetPartNumFromBth(void);
extern void GetPartInfoFromBth(UINT8 part,UINT16 *MB_size,UINT8 *attr);

#endif //GPL326XXB

/* *****************   Specific for GPL326XXA  *************************************/
#ifndef GPL326XXB_BCH60  //GPL326XXA

#ifndef GPL32_GP10
#define GPL32_GP10	0x10
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


#define NAND_READ_ALLOW	    0x1
#define NAND_WRITE_ALLOW	0x2

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
// This is pseudo header for extern to driver_l1.h 

typedef struct NandInfo_Str
{
	INT32U    wNandID;
	INT16U    wNandType;
	INT16U    wAddrCycle;
	INT32U    wNandBlockNum;
	INT16U    wPageSize;
	INT16U    wPageSectorSize;
	INT16U    wPage_with_spare_size;
	INT16U    wBlkPageNum;
}Physical_NandInfo;

typedef enum {
    NAND_WP_ENABLE=0,
    NAND_WP_DISABLE=1,
    NAND_WP_NON_DEFINE
}NAND_WP_ENUM;

typedef enum {
    NF_PROGRAM_DENY =0x00,  /*Write Protect*/
    NF_PROGRAM_ERR7 =0x01,  /*Write Protect*/
    NF_PROGRAM_ERR8 =0x02,  /*Write Protect*/
    NF_PROGRAM_ERR9 =0x03,  /*Write Protect*/  
    NF_PROGRAM_ERR10=0x04,  /*Write Protect*/ 
    NF_PROGRAM_ERR11=0x05,  /*Write Protect*/
    NF_PROGRAM_ERR12=0x06,  /*Write Protect*/  
    NF_PROGRAM_ERR13=0x07,  /*Write Protect*/
    NF_PROGRAM_WELL =0x80,  /*Program Well*/
    NF_PROGRAM_ERR0 =0x81,  /*Program Error*/
    NF_PROGRAM_ERR1 =0x82,  /*Program Error1*/
    NF_PROGRAM_ERR2 =0x83,  /*Program Error2*/
    NF_PROGRAM_ERR3 =0x84,  /*Program Error3*/  
    NF_PROGRAM_ERR4 =0x85,  /*Program Error4*/
    NF_PROGRAM_ERR5 =0x86,  /*Program Error5*/
    NF_PROGRAM_ERR6 =0x87   /*Program Error6*/
} NFCMD_STATUS;

#ifndef NAND_CS_ENUM_DEF
#define NAND_CS_ENUM_DEF
typedef enum
{
    NAND_CS1=0x2,   /* IOF1 */
    NAND_CS2=0x4,   /* IOF2 */
    NAND_CS3=0x8    /* IOF3 */
} NAND_CS_ENUM;
#endif

#ifndef _TAG_ENUM
#define _TAG_ENUM
typedef enum {
    TAG_GPL325XX=6,
    TAG_GPL326XX=10
} TAG_ENUM;
#endif
//extern INT8U nf_8k_flag;
//extern INT8U nf_ecc_nand_flag;
//extern INT16U gNand_spare_size;
extern Physical_NandInfo gPhysical_NandInfo;

extern INT32U NandReset(void);

extern   void nand_flash_bchtable_alloc(INT16S *alpha_to ,INT16U *index_of);

extern void nand_wp_pin_reg(INT8U gpio_pin_number);
extern void DrvNand_WP_Enable(void);
extern void DrvNand_WP_Disable(void);
extern NAND_WP_ENUM DrvNand_WP_Status_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern INT32U Nand_blk_size_get(void);
extern INT32U nand_sector_nums_per_page_get(void);
extern INT32U nand_page_nums_per_block_get(void);
extern void nand_protect_block_nums_set(INT32U nand_protect_nums);
extern void nand_small_page_force_set(void);
extern INT16U nand_page_size_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern void nand_total_MByte_size_set(INT32U nand_MB_size);
extern INT16U bch_mode_get(void);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void nand_addr_cycle_set(INT16U addr_cycle);
extern void nand_page_nums_per_block_set(INT16U blk_page_size);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void DrvNand_WP_Initial(void);
extern void nand_set_hw_bch(BCH_MODE_ENUM type);
extern void nand_page_size_set(INT16U page_size);
extern INT32U Nand_blk_number_get(void); 

extern void nf_8k_en(INT8U nand_8k_en);
extern INT8U nf_8k_get(void);
extern void nf_ecc_nand_en(INT8U ecc_nand_en);
extern INT8U nf_ecc_nand_get(void);
extern void nf_spare_size_set(INT16U Nand_spare_size);
extern INT16U nf_spare_size_get(void);

extern INT16U nand_bch_err_bits_get(void);
extern NFCMD_STATUS nand_write_status_get(void);

extern INT8U gpl32_version_get(void);
extern INT8U nf_rom_flag_get(void);

#endif //GPL326XXA

#endif //DRV_L1_NAND_FLASH_EXT_H