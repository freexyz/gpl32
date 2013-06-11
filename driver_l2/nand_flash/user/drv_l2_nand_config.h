#ifndef DRV_L2_NAND_CONFIG_H
#define DRV_L2_NAND_CONFIG_H 

#include "project.h"

#ifdef GPL326XXB_BCH60

#include "gplib.h"
#include "driver_l2_cfg.h"
#include "drv_l2_nand_ext.h"
#include "driver_l1.h"

#define UINT32 INT32U
#define UINT16 INT16U
#define UINT8  INT8U
#define SINT32 INT32S
#define SINT16 INT16S
#define SINT8  INT8S

#define NAND_GOOD_BLK							0xffff
#define NAND_EMPTY_BLK							0xffff
#define NAND_GOOD_TAG							0xff
#define	NAND_ORG_BAD_BLK						0xff00
#define NAND_ORG_BAD_TAG						0x00
#define	NAND_USER_BAD_BLK						0xff44
#define	NAND_USER_BAD_TAG						0x44
#define	NAND_USER_UNSTABLE_BLK					0xff77
#define	NAND_USER_UNSTABLE_TAG					0x77
#define NAND_FIRST_SCAN_BAD_BLK					0xff55
#define NAND_FIRST_SCAN_BAD_TAG					0x55

#define NAND_MAPTABLE_BLK											0xff66		// word
#define NAND_MAPTABLE_TAG											0xff66		// word


#define STATUS_OK               0
#define STATUS_FAIL            -1

#define DRV_L2_NAND_DBG     0
#if DRV_L2_NAND_DBG==1
	#define NF_DBG_PRINT	DBG_PRINT
#else
	#define NF_DBG_PRINT     nf_null_printf
#endif

/*######################################################
#												 		#
#	   Nand Flash Blocks Assignation Configuration		#
#														#
 ######################################################*/
#define APP_AREA_START			11	/* Start Physical Block Address */
#define APP_AREA_SIZE			0x40 	/* APP Area Size|Block numbers. */
#define APP_AREA_SPARE_START	(APP_AREA_START + APP_AREA_SIZE)
#define APP_AREA_SPARE_PERCENT  4
#define	APP_AREA_SPARE_SIZE		(APP_AREA_SIZE/APP_AREA_SPARE_PERCENT)	/* Spare area size */
#define DATA_AREA_START			(APP_AREA_SPARE_START + APP_AREA_SPARE_SIZE)

#define MAX_PARTITION_NUM		10
#define CACHE_LINE_NUM			16

#define GP_NAND_MAX_PAGE_SIZE (8 * 1024)
#define GP_NAND_MAX_SPARE_SIZE 1024

#if _OPERATING_SYSTEM != _OS_NONE
extern  OS_EVENT * gNand_sem;
extern  OS_EVENT * gNand_PHY_sem;
#endif

typedef struct
{
	UINT32 offset;
	UINT32 size;
	UINT8  attr;	
}PART_INFO;


typedef struct 
{
	UINT16	uiAppStart;
	UINT16 	uiAppSize;
	UINT16 	uiAppSpareStart;
	UINT16  uiAppSparePercent;
	UINT16 	uiAppSpareSize;
	UINT16 	uiDataStart;	
	UINT16	uiBankSize;
	UINT16	uiBankRecSize;
	UINT32	uiNoFSAreaSize;
	
	UINT8		uiPartitionNum;	
	PART_INFO Partition[MAX_PARTITION_NUM];	
} NF_CONFIG_INFO;

extern NF_CONFIG_INFO 		gSTNandConfigInfo;

extern INT32U Nand_Part0_Offset;
extern INT32U Nand_Part0_Size;
extern INT16U Nand_Part0_Mode;
extern INT32U Nand_Part1_Offset;
extern INT32U Nand_Part1_Size;
extern INT16U Nand_Part1_Mode;
extern INT32U Nand_Part2_Offset;
extern INT32U Nand_Part2_Size;
extern INT16U Nand_Part2_Mode; 

extern void Nand_OS_LOCK(void);
extern void Nand_OS_UNLOCK(void);

extern INT32U DMAmmCopy(INT32U wSourceAddr, INT32U wTargeAddr, INT16U Count); //count is byte num

extern SINT32 NandParseBootHeader(UINT8 *Buffer);
extern UINT16 GetAppStartBlkFromBth(void);
extern UINT16 GetAppSizeOfBlkFromBth(void);
extern UINT16 GetAppPercentFromBth(void);
extern UINT16 GetDataBankLogicSizeFromBth(void);
extern UINT16 GetDataBankRecycleSizeFromBth(void);
extern UINT32 GetNoFSAreaSectorSizeFromBth(void);

extern UINT8 GetPartNumFromBth(void);
extern void GetPartInfoFromBth(UINT8 part,UINT16 *MB_size,UINT8 *attr);

extern SINT32 GetNandConfigInfo(void);
extern UINT16 get_bch_mode(void);
extern void SetBitErrCntAsBadBlk(UINT16 bit_cnt);
extern UINT16 GetBitErrCntAsBadBlk(void);

extern void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode);

extern INT32U Nand_part0_Offset_Get(void);
extern INT32U Nand_part0_mode_Get(void);
extern INT32U Nand_part0_size_Get(void);
extern void setLastPartitionSize(INT32U totalNandSizeInSector);

extern INT32U DrvNand_bchtable_alloc(void);
extern void DrvNand_bchtable_free(void);

extern INT32U NandAppByteSizeGet(void);
extern void NandAppByteSizeSet(INT32U app_size);

extern void NandBootAreaByteSizeSet(INT32U boot_size);
extern INT32U NandBootAreaByteSizeGet(void);

extern INT8U* alloc_manage_temp_buffer(void);
extern void free_manage_temp_buffer(INT8U** bufferPtr);
extern INT8U* alloc_data_misc_use_buffer(void);
extern void free_data_misc_use_buffer(INT8U** bufferPtr);
extern INT8U* alloc_app_misc_use_buffer(void);
extern void free_app_misc_use_buffer(INT8U** bufferPtr);
extern INT8U * alloc_data_ABPage_buffer(INT8U index);
extern INT8U * alloc_data_Cache_buffer(INT8U index);

extern void setSize4Partition(INT8U partitionIndex, INT32U sizeInSector);
extern INT32U getSize4Partition(INT8U partitionIndex);
extern void setMode4Partition(INT8U partitionIndex, INT8U mode);
extern INT8U getMode4Partition(INT8U partitionIndex);
extern void setSize4Partition(INT8U partitionIndex, INT32U offsetInSector);
extern INT32U getSize4Partition(INT8U partitionIndex);
extern void initBufferFF(INT8U * start, INT32U len);

extern void SetNandRomAppAreaStartAddr(INT32U AppAreaStartAddr);
extern INT32U GetNandRomAppAreaStartAddr(void);
extern INT32U GetNandRomDataAreaStartAddr(void);
extern void SetNandRomDataAreaStartAddr(INT32U DataAreaStartAddr);
extern INT32S NandInfoAutoGet(void);
extern INT32U nand_block_byte_size_get(void);

#endif//end #ifdef GPL326XXB_BC60

/************************************************************/

#ifndef GPL326XXB_BCH60 //GPL326XXA

#include "driver_l2.h"
#include "drv_l2_nand_ext.h"
#include "drv_l1_nand_flash_ext.h"


#define DRV_L2_NAND_DBG     0
#if DRV_L2_NAND_DBG==1
	#define NF_DBG_PRINT	DBG_PRINT
#else
	#define NF_DBG_PRINT     nf_null_printf
#endif



/*######################################################
#												 		#
#	   Nand Flash Blocks Assignation Configuration		#
#														#
 ######################################################*/
#define APP_AREA_START			11	/* Start Physical Block Address */
#define APP_AREA_SIZE			0x40 	/* APP Area Size|Block numbers. */
#define APP_AREA_SPARE_START	(APP_AREA_START + APP_AREA_SIZE)
#define APP_AREA_SPARE_PERCENT  4
#define	APP_AREA_SPARE_SIZE		(APP_AREA_SIZE/APP_AREA_SPARE_PERCENT)	/* Spare area size */
#define DATA_AREA_START			(APP_AREA_SPARE_START + APP_AREA_SPARE_SIZE)

/*######################################################
#												 		#
#	    Nand Flash Spec/Type Configuration	        	#
#														#
 ######################################################*/
#define SUPPORT_ALL_PAGE
//#define SUPPORT_2KB_512B_PAGE
//#define SUPPORT_512B_PAGE

#if defined SUPPORT_ALL_PAGE
	#define PAGE_SIZE	 	8192
	#define SPARE_SIZE	 	436
#elif defined SUPPORT_2KB_512B_PAGE	
	#define PAGE_SIZE	 	2048
	#define SPARE_SIZE	 	64
#elif defined SUPPORT_512B_PAGE
	#define PAGE_SIZE	 	512
	#define SPARE_SIZE	 	16
#else
	" Error: Nandflash Config ERROR!! "
#endif

#define BANK_NUM	30

/*######################################################
#												 		#
#	   Nand Flash Driver Bank&Recycle Configuration	    #
#														#
 ######################################################*/ 
#define SP_BANK_LOGIC_SIZE	 200 	// Small Page
#define SP_BANK_RECYCLE_SIZE 20 	// Small Page
#define LP_BANK_LOGIC_SIZE	 512 	// Large Page
#define LP_BANK_RECYCLE_SIZE 64 	// Large Page

#if (PAGE_SIZE==512)
	#define BANK_LOGIC_SIZE		SP_BANK_LOGIC_SIZE
	#define BANK_RECYCLE_SIZE 	SP_BANK_RECYCLE_SIZE
#else  /*default condition is Large page*/
	#define BANK_LOGIC_SIZE		LP_BANK_LOGIC_SIZE
	#define BANK_RECYCLE_SIZE 	LP_BANK_RECYCLE_SIZE
#endif

/*######################	Warning		############################
#																	#
#		Don`t modify the content below,otherwise it will			#
# 	be cause driver can`t work normally or can`t work optimumly!	#
#																	#
###################################################################*/
#ifdef NAND_16BIT
	#define BYTE_WORD_SHIFT		1		// 16 platform
#else
	#define BYTE_WORD_SHIFT		0		// 32 platform
#endif

#ifndef _BANK_INFO
#define _BANK_INFO
typedef struct
{	
	INT16U	wLogicBlkNum;
	INT16U	wRecycleBlkNum;
	INT16U	wMapTableBlk;
	INT16U	wMapTablePage;
	INT16U  wOrgBlkNum;
	INT16U	wUserBadBlkNum;
	INT16U	wExchangeBlkNum;
	INT16U  wStatus;
}BANK_INFO;
#endif


typedef struct
{
	INT16U	Logic2Physic[BANK_LOGIC_SIZE];
	INT16U	RecycleBlk[BANK_RECYCLE_SIZE];
}IMM;

typedef union 
{
	IMM 	Maptable;	
	INT16U  pMaptable[BANK_LOGIC_SIZE+BANK_RECYCLE_SIZE];
}MM;

typedef struct
{
	INT16U	wPageData[PAGE_SIZE>>1]; // 为了兼容16bit complier
	INT8U	wBad_Flag_0;
	INT8U	wCount;
	INT16U	wLogicNum;
	INT8U	wExchangeType;
	INT8U	wBad_Flag_6;
	INT16U  wP2P;	
}WB;

typedef union
{
	WB PageTag;
	INT16U WorkBuffer[(PAGE_SIZE+SPARE_SIZE)>>1]; // 为了兼容16bit complier	
}TT;

typedef struct 	
{
	INT16U	wLogicBlk;
	INT16U	wPhysicBlk;
}L2P;

#ifndef _NF_USER_INFO
#define _NF_USER_INFO
typedef struct 
{
	INT16U 	uiPageSize;
	INT16U	uiPageSpareSize;	
	INT16U	uiBankNum;
	INT16U	uiBankSize;
	INT16U	uiBankRecSize;	
	INT16U	uiMM_Num;
	INT16U	uiAppStart;
	INT16U 	uiAppSize;
	INT16U 	uiAppSpareStart;
	INT16U  uiAppSparePercent;
	INT16U 	uiAppSpareSize;
	INT16U 	uiDataStart;
	INT16U	uiDirReadSupport;
} NF_USER_INFO;
#endif

typedef struct
{
	INT32U BCH_BLK_NUM;
	INT32U BCH_BADBLK_NUM;
	INT32U BCH_BADPAGE_NUM;
	
	INT32U BOOTLOAD_NUM;
	INT32U BOOTLOAD_BAD_NUM;
}UsrBootArea;

extern 	TT   APP_tt;
extern 	ALIGN16 L2P  gL2PTable[0x200];//size=0x200, so ram size= 0x200*sizeof(L2P)=2048<=page size//gL2PTable[APP_AREA_SPARE_SIZE];

#if _OPERATING_SYSTEM != _OS_NONE
extern  OS_EVENT * gNand_sem;
extern  OS_EVENT * gNand_PHY_sem;
#endif
extern INT32U Nand_Part0_Offset;
extern INT32U Nand_Part0_Size;
extern INT16U Nand_Part0_Mode;
extern INT32U Nand_Part1_Offset;
extern INT32U Nand_Part1_Size;
extern INT16U Nand_Part1_Mode;
extern INT32U Nand_Part2_Offset;
extern INT32U Nand_Part2_Size;
extern INT16U Nand_Part2_Mode; 


extern void Nand_OS_LOCK(void);
extern void Nand_OS_UNLOCK(void);
extern INT32S NandInfoAutoGet(void);
extern void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode);
extern INT32U NandAppByteSizeGet(void);
extern INT32U NandBootAreaByteSizeGet(void);
extern INT32U Nand_part0_Offset_Get(void);
extern INT32U Nand_part0_mode_Get(void);
extern INT32U Nand_part0_size_Get(void);
extern INT16U GetBadFlagFromNand(INT16U wPhysicBlkNum);
extern INT32U DrvNand_bchtable_alloc(void);
extern NF_USER_INFO gNandUsrInfo;
extern void SetWorkBuffer(TT* p_tt_basic, TT* p_tt_extend);
extern INT32U CalculateFATArea(void);
extern INT32U NandBootFormat_UP(INT16U format_type, INT16U format_step);
extern void nf_null_printf(CHAR *fmt, ...);
extern INT32U ib6_smart_para_get(INT16U *smart_page_size, INT16U *smart_blk_size);

extern void SetBitErrCntAsBadBlk(INT16U bit_cnt);
extern INT16U GetBitErrCntAsBadBlk(void);
extern INT8U nand_header_max_sys_clk_get(void);

extern void   Nand_Partition_Num_Set(INT16U number);
extern INT16U Nand_Partition_Num_Get(void);
extern INT32U NandBootAreaInfoGet(UsrBootArea *info);

extern INT32U combin_reg_data(INT8U *data, INT32S len);
extern void SetNandRomAppAreaStartAddr(INT32U AppAreaStartAddr);
extern void SetNandRomDataAreaStartAddr(INT32U DataAreaStartAddr);

#endif//end ifndef GPL326XXB_BCH60

#endif 


