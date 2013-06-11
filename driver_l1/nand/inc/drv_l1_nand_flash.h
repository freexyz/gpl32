#ifndef __drv_l1_NAND_FLASH_H__
#define __drv_l1_NAND_FLASH_H__
#include "project.h"

#ifndef GPL326XXB_BCH60

#include "driver_l1.h"
#include "drv_l1_nand_flash_ext.h"

#ifndef GPL326XX
#define GPL326XX    0x10
#define GPL326XX	0x10
#endif

#ifndef GPL326XXB
#define GPL326XXB   0x11
#define GPL326XX_B	0x11
#endif

/*BCH TABLE MODE*/
#ifndef NAND_BCH_TABLE_MODE  
#define NAND_BCH_FIX_TABLE      0
#define NAND_BCH_RUNTIME_GEN    1
#define NAND_BCH_TABLE_MODE     NAND_BCH_RUNTIME_GEN
#endif 

/* Nand data access mode */
#define NAND_REG_MODE                       0
#define NAND_DMA_MODE                       1
#define NAND_DATA_READ_MODE          NAND_DMA_MODE
#define NAND_DATA_WRITE_MODE         NAND_DMA_MODE

/* Nand flash command */
#define RD_1ST_CMD         	            0x00
#define RD_2ND_HALF_CMD                 0x01	
#define RD_3ND_HALF_CMD			        0x50
#define RD_RANDOM_CMD 	                0x05	
#define RD_2ND_CYCLE_CMD	            0x30	
#define INPUT_DATA_CMD     	            0x80
#define RANDOM_INPUT_DATA_CMD           0x85
#define PROG_DATA_END_CMD  	            0x10
#define BLOCK_ERASE1_CMD   	            0x60
#define BLOCK_ERASE2_CMD   	            0xD0
#define RD_RANDOM_END_CMD               0xE0
#define RD_ID_CMD          	            0x90
#define RD_STATUS_CMD      	            0x70
#define RESET_CMD          	            0xFF
/* Dominant's new definition */
#define PAGE_READ                       0x00
#define PAGE_READ_END                   0x30
#define PAGE_READ_SPARE					0x50
#define READ_FOR_INTERNAL_DATA_MOVE     0x00
#define READ_FOR_INTERNAL_DATA_MOVE_END 0x35
#define PROGRAM_FOR_INTER_DATA_MOVE     0x85
#define PROGRAM_FOR_INTER_DATA_MOVE_END 0x10
#define PROGRAM_CACHE_MODE              0x80
#define PROGRAM_CACHE_MODE_END          0x15
#define BLOCK_ERASE               	    0x60
#define BLOCK_ERASE_END                 0xD0
#define READ_CACHE_MODE_STAR            0x31
#define READ_CACHE_MODE_LAST            0x3F
#define RANDOM_DATA_INPUT               0x85
#define READ_ID                         0x90

/* each sector has 512 Bytes */
#define SECTOR_SIZE                   512

#ifndef _BCH_DECODE_STATUS_ENUM
#define _BCH_DECODE_STATUS_ENUM
typedef enum
{
    BCH_DECODE_OK=0,
    BCH_DECODE_PAGE_0_ERR=1,
    BCH_DECODE_PAGE_1_ERR=2,
    BCH_DECODE_PAGE_2_ERR=3,
    BCH_DECODE_PAGE_3_ERR=4,
    BCH_DECODE_PAGE_4_ERR=5,
    BCH_DECODE_PAGE_5_ERR=6,
    BCH_DECODE_PAGE_6_ERR=7,
    BCH_DECODE_PAGE_7_ERR=8,
    BCH_DECODE_STATUS_MAX
} BCH_DECODE_STATUS_ENUM;
#endif


typedef enum {
   DRV_FALSE,
   DRV_TRUE
} drv_bool;

typedef struct flash_list_tag
{
   INT8U      id;
   INT16U     total_MB_size;
   INT8U  BlockNums_BlockSize_PageSize;
}  flash_list;

#ifndef FLASH_8K_LIST_DEF
#define FLASH_8K_LIST_DEF
typedef struct flash_8k_list_tag
{
	INT16U	MainID;
	INT32U 	VenDorID;
	INT32U	TotalMB;
	INT32U  BlockNum;
	INT16U	BlockSize;
	INT32U 	PageSize;
	INT16U  PageSpareSize;
	INT8U   AddressCycle;
	INT8U 	EraseCycle;
	BCH_MODE_ENUM	BCHMode; 
} flash_8k_list;			//yunxiangan add for 8k page nand flash list 2010-12-07
#endif

typedef enum
{
    BCH_DISABLE=0,
    BCH_ENABLE,
    BCH_ENABLE_MAX
} BCH_ENABLE_ENUM;

#ifndef __BCH_PAGE_RW_ENUM_
#define __BCH_PAGE_RW_ENUM_
typedef enum
{
    BCH_PAGE_READ=0,
    BCH_PAGE_WRITE,
    BCH_PAGE_RW_MAX
} BCH_PAGE_RW_ENUM;
#endif

typedef enum
{
    BCH_528_BYTES=0,
    BCH_2112_BYTES,
    BCH_4224_BYTES,
    BCH_PAGE_SIZE_MAX
} BCH_PAGE_SIZE_ENUM;

typedef enum
{
    BCH_NO_CLEAR_PARITY=0,
    BCH_CLEAR_PARITY_FLAG2,
    BCH_CLEAR_PARITY_MAX
} BCH_PARITY_CLR_ENUM;

typedef enum
{
    BCH_READ_PAGE_0_PARITY=0,
    BCH_READ_PAGE_1_PARITY,
    BCH_READ_PAGE_2_PARITY,
    BCH_READ_PAGE_3_PARITY,
    BCH_READ_PAGE_4_PARITY,
    BCH_READ_PAGE_5_PARITY,
    BCH_READ_PAGE_6_PARITY,
    BCH_READ_PAGE_7_PARITY,
    BCH_READ_PAGE_SEL_MAX
} BCH_PAGE_SEL_ENUM;

/* ps. The BCH Page definition is different from nand page */
typedef enum {
    BCH_OK,
    BCH_SPECIAL_CASE,
    BCH_DECODE_FAIL,
    BCH_DECODE_FAIL2,
    BCH_STATUS_MAX
} BCH_STATUS;

/*============ NAND Macro function Define ==========*/
#define NAND_BUSY_WAIT  \
{\
   INT32U nand_ready = 0xFFFFFFFF; \
   INT32U detect_count=0; \
   for (detect_count =0; detect_count < 10; detect_count++); \
   while (nand_ready != 0x00008000) \
   {\
        nand_ready = (DRV_Reg32(NF_CTRL_ADDR)& 0x8000); \
   }\
}
/* Nand Busy Wait2 先等拉 low 再等拉 high */
/* -----|______|----------------*/
#define NAND_BUSY_WAIT2  \
{\
   INT32U nand_ready = 0xFFFFFFFF; \
   INT32U detect_count=0; \
   while (nand_ready != 0x00000000) \
   {\
        detect_count++; \
        nand_ready = (DRV_Reg32(NF_CTRL_ADDR)& 0x8000); \
   }\
   while (nand_ready != 0x00008000) \
   {\
        detect_count++; \
        nand_ready = (DRV_Reg32(NF_CTRL_ADDR)& 0x8000); \
   }\
}

#define NAND_DMA_BUSY_WAIT \
{\
    INT32U nand_dma_ready=0xFFFFFFFF;\
   INT32U detect_count=0; \
   for (detect_count =0; detect_count < 10; detect_count++); \
    while (nand_dma_ready != 0) {nand_dma_ready = (DRV_Reg32(NF_DMA_CTRL_ADDR)& 0x00000001);} \
}

/* internal tools */
#define PAGE_TO_ADDRH(input_page_index, output_ADDR_H) \
{\
    output_ADDR_H = input_page_index ; \
}

/* bsector = 0~3 or 0~1 or 0 */
#define GET_SPARE_ADDRL_BY_BSECTOR_NUM(input_bsector_num, output_ADDR_L) \
{\
    output_ADDR_L = (NAND_ID_Table[NTYPE].page_size + input_bsector_num*spare_sector_offset); \
}

#define BLOCK_BPAGE_TO_ADDRH(input_block_idx, input_bpage_idx, output_ADDR_H) \
{\
	if (page_nums_per_block == 64) \
	{output_ADDR_H = input_block_idx << 7 | input_bpage_idx;}\
	else if (page_nums_per_block == 32) \
	{output_ADDR_H = input_block_idx << 6 | input_bpage_idx;} \
}

#define BLOCK_ID_TO_ADDR(input_block_idx, output_ADDR_L, output_ADDR_H) \
{\
	if (page_nums_per_block == 64) \
	{\
	    output_ADDR_L = ((input_block_idx << 6) & 0x0000FFFF);\
        output_ADDR_H = ((input_block_idx << 6)>>16 & 0x0000FFFF);\
    }\
	else if (page_nums_per_block == 32) \
	{\
	    output_ADDR_L = ((input_block_idx << 5) & 0x0000FFFF);\
        output_ADDR_H = ((input_block_idx << 5)>>16 & 0x0000FFFF);\
    }\
}
    
#define ADDRH_TO_BLOCK_IDX(input_ADDR_H, output_block_idx) \
{\
	output_block_idx = input_ADDR_H / page_nums_per_block;  \
}


#define SECTOR_IDX_TO_ADDR(input_sector_idx, output_ADDRL, output_ADDRH) \
{\
    output_ADDRH = (input_sector_idx / sector_nums_per_page);\
    output_ADDRL = ((input_sector_idx % sector_nums_per_page)*SECTOR_SIZE); \
}

#define SECTOR_IDX_TO_ADDR_PSECTOR(input_sector_idx, output_ADDRL, output_ADDRH, output_psector) \
{\
    output_ADDRH   = (input_sector_idx / sector_nums_per_page);\
    output_psector = (input_sector_idx % sector_nums_per_page);\
    output_ADDRL   = (output_psector*SECTOR_SIZE); \
}

#define SECTOR_IDX_TO_ADDR_BSECTOR(input_sector_idx, output_ADDRL, output_ADDRH, output_bsector) \
{\
    output_ADDRH   = (input_sector_idx / sector_nums_per_page);\
    output_bsector = (input_sector_idx % sector_nums_per_block);\
    output_ADDRL   = ((input_sector_idx % sector_nums_per_page)*SECTOR_SIZE); \
}

#define SECTOR_IDX_TO_BLOCK_ID(input_sector_idx, output_block_id) \
{\
    output_block_id = (input_sector_idx / sector_nums_per_block);\
}

/* marcro is ready for block erase */
#define ADDRH_TO_SECTOR_IDX(input_ADDR_H, output_sector_idx) \
{\
    output_sector_idx = (input_ADDR_H / sector_nums_per_page); \
}
/* bpage is mean the offset (unit is page) in the block */
#define ADDRH_TO_BLOCK_IDX_BPAGE(input_ADDR_H, output_block_idx, output_bpage) \
{\
    output_block_idx = (input_ADDR_H / page_nums_per_block); \
    output_bpage = (input_ADDR_H % page_nums_per_block); \
}

#define SECTOR_IDX_TO_BLOCK_IDX(input_sector_idx, output_block_idx) \
{\
    output_block_idx = (input_sector_idx/sector_nums_per_block); \
}

#define SECTOR_IDX_TO_BLOCK_IDX_BPAGE(input_sector_idx, output_block_idx, output_bpage) \
{\
    output_block_idx = (input_sector_idx/sector_nums_per_block); \
    output_bpage = (input_sector_idx%page_nums_per_block); \
}

#define SECTOR_IDX_TO_BLOCK_IDX_BSECTOR(input_sector_idx, output_block_idx, output_bsector) \
{\
    output_block_idx = (input_sector_idx/sector_nums_per_block); \
    output_bsector = (input_sector_idx%sector_nums_per_block); \
}

#define NAND_ECC_CTRL(eccspt_flag, ckp_flag, erst_flag) \
{\
    INT32U  ecc_reg_value;\
    ecc_reg_value = (erst_flag<<0 | ckp_flag<<1 | eccspt_flag<<2);\
    DRV_WriteReg32(ECC_CTRL_ADDR, ecc_reg_value);\
}

#define NAND_ECC_BANK_CTRL(EccBank, eccspt_flag, ckp_flag, erst_flag) \
{\
    INT32U  ecc_reg_value;\
    ecc_reg_value = (erst_flag<<0 | ckp_flag<<1 | eccspt_flag<<2 | EccBank<<8);\
    DRV_WriteReg32(ECC_CTRL_ADDR, ecc_reg_value);\
}

#define NAND_BCH_CTRL(PAGE_SEL, PARITY_CLR, BCH_WR, PAGE_SIZE, BCH_MODE, BCH_EN) \
{\
    INT32U  bch_reg_value;\
    if (BCH_MODE != BCH_OFF) {\
    bch_reg_value = (BCH_EN | BCH_MODE | PAGE_SIZE<<2 | BCH_WR<<4 | PARITY_CLR<<5 | PAGE_SEL <<8);\
    R_NF_BCH_CTRL = bch_reg_value;\
    } else {\
    R_NF_BCH_CTRL =0;}\
}


/*===Nand Smart ID table about define Start===*/
 /*Page Size*/
#define _512_BYTES   0x0 /*00b*/                                                                                                                                     
#define _1024_BYTES  0x1 /*01b*/                                                                                                                                     
#define _2048_BYTES  0x2 /*10b*/                                                                                                                                     
#define _4096_BYTES  0x3 /*11b*/ 
#define PAGESIZE_MASK   0x3  
#define GET_PAGE_SIZE(x) (x&PAGESIZE_MASK)
 /*Total Block Nums*/
#define _1024_BLOCKS  (0x0) /*000b*/                                                                                                                                
#define _2048_BLOCKS  (0x4) /*001b*/                                                                                                                                
#define _4096_BLOCKS  (0x8) /*010b*/                                                                                                                                
#define _8192_BLOCKS  (0xC) /*011b*/ 
#define _16384_BLOCKS (0x10) /*100b*/ 
#define BLOCKNUM_MASK   0x1C  
#define GET_BLOCK_NUMS(x) (x&BLOCKNUM_MASK)                                                                                                                   
                                                                                                                                                              
  /*Block Size*/
#define _32_PAGES    (0x0) /*00b*/                                                                                                                                
#define _64_PAGES    (0x20) /*01b*/                                                                                                                                
#define _128_PAGES   (0x40) /*10b*/                                                                                                                                
#define _256_PAGES   (0x60) /*11b*/  
#define BLOCKSIZE_MASK  0x60
#define GET_BLOCK_SIZE(x) (x&BLOCKSIZE_MASK)                                                                                                                  
 /*Erase Cyccle*/                                                                                                                                                             
#define _ERASE_CYC2  (0x0) /*0*/                                                                                                                               
#define _ERASE_CYC3  (0x80) /*1*/                                                                                                                          
#define ERASE_CYC_MASK  0x80                                                                                                       
#define GET_ERASE_CYC(x) (x&ERASE_CYC_MASK)
#define _MB
#define ID_TBL_SIZE 24
#define TBL_SIZE    ID_TBL_SIZE

/*For 8k page nand list 2010-11-07*/
#define ID_8K_TBL_SIZE 	8
#define TBL_8K_SIZE ID_8K_TBL_SIZE 

#define PAGE_SIZE_2K	2048
#define PAGE_SIZE_4K	4096
#define PAGE_SIZE_8K	8192

#define PAGE_SPARE_SIZE_32BYTE	32
#define PAGE_SPARE_SIZE_64BYTE	64
#define PAGE_SPARE_SIZE_128BYTE	128
#define PAGE_SPARE_SIZE_192BYTE	192
#define PAGE_SPARE_SIZE_256BYTE	256
#define PAGE_SPARE_SIZE_384BYTE	384

#define ADDRESS_CYCLE_4CYCLE	4
#define ADDRESS_CYCLE_5CYCLE	5
#define ERASE_CYCLE_3CYCLE	3
#define ERASE_CYCLE_2CYCLE	2

#define BLOCK_NUM_1024_BLOCKS  1024                                                                                                                              
#define BLOCK_NUM_2048_BLOCKS  2048
#define BLOCK_NUM_4096_BLOCKS  4096                                                                                                                              
#define BLOCK_NUM_8192_BLOCKS  8192

#define BLOCK_SIZE_64_PAGE	   64
#define BLOCK_SIZE_128_PAGE	   128
#define BLCOK_SIZE_256_PAGE	   256
//extern INT8U   nf_8k_flag;
//extern INT16U  gNand_spare_size;

extern INT16U  bch_table1[8192];  /*2bytes alignment address need*/
extern INT16U  bch_table2[8192];  /*2bytes alignment address need*/

extern flash_list NAND_ID_Table[ID_TBL_SIZE];
extern Physical_NandInfo gPhysical_NandInfo;
extern INT32U nand_block_Byte_size;
extern INT16U page_with_spare_size;
extern INT16U page_nums_per_block;
extern INT8U  sector_nums_per_page;
extern NAND_STATUS_ENUM nand_smart_id_init(INT16U main_id, INT32U vendor_id);
/*===Nand Smart ID table about define End===*/

extern BCH_MODE_ENUM   bch_mode;
extern void nand_flash_uninit(void);
extern INT32U nand_flash_init(void);
extern INT32U NandReset(void);
extern INT32U Nand_ReadPhyPage_Boot(INT32U wPhysicPageNum, INT32U WorkBuffer); 
extern INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);
extern void nand_flash_bchtable_alloc(INT16S *alpha_to ,INT16U *index_of);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
extern INT32U nand_erase_by_block_id(INT32U block_id);

/* extern Definition for GP4's MLC solution */
extern INT32U NandReset(void);
extern INT32U NAND_ReadID(INT32U * ID);
extern INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
extern INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);
extern void nand_small_page_cs_pin_reg(NAND_CS_ENUM nand_cs);
extern void nand_wp_pin_reg(INT8U gpio_pin_number);
extern void nand_share_mode_reg(NAND_SAHRE_MODE_ENUM share_or_nonshare);
extern void nand_large_page_cs_reg(NAND_CS_ENUM nand_large_page_cs);
extern INT32U DrvL1_Nand_Init(void);
extern void nand_wp_pin_reg(INT8U gpio_pin_number);
extern void DrvNand_WP_Enable(void);
extern void DrvNand_WP_Disable(void);
extern NAND_WP_ENUM DrvNand_WP_Status_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern INT32U Nand_blk_size_get(void);
extern INT32U nand_sector_nums_per_page_get(void);
extern INT32U nand_page_nums_per_block_get(void);
extern void nand_protect_block_nums_set(INT32U nand_protect_nums);
extern void nand_total_MByte_size_set(INT32U nand_MB_size);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern INT32U BCH_decode(INT16S total_page, INT16U *database);

extern void nand_small_page_force_set(void);
extern INT16U nand_page_size_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern void nand_total_MByte_size_set(INT32U nand_MB_size);
extern BCH_MODE_ENUM bch_mode_get(void);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void nand_addr_cycle_set(INT16U addr_cycle);
extern void nand_page_nums_per_block_set(INT16U blk_page_size);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void DrvNand_WP_Initial(void);
extern void nand_set_hw_bch(BCH_MODE_ENUM type);
extern void nand_page_size_set(INT16U page_size);
extern INT32U Nand_blk_number_get(void); 
extern INT16U nand_bch_err_bits_get(void);
extern INT32U nand_read_id_all(INT16U* MainID, INT32U* VendorID);
extern void nand_ctrl_timing_set(INT16U timging_reg_val);  // Option function, add by dominant, 2010/05/27
extern NFCMD_STATUS nand_write_status_get(void); 
extern INT8U gpl32_version_get(void);
extern void page_offset_set(INT32U byte_offset);
extern void nf_8k_en(INT8U nand_8k_en);
extern void nf_ecc_nand_en(INT8U ecc_nand_en);
extern void nf_rom_flag_set(INT8U nand_rom_en);
extern INT8U nf_rom_flag_get(void);
extern void nf_otp_bch4_flag_set(INT8U nand_otp_bch4_en);
extern INT8U nf_otp_bch4_flag_get(void);
#endif //GPL326XXB_BCH60

#endif		// __drv_l1_NAND_FLASH_H__