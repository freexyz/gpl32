#ifndef DRV_L2_NAND_MANAGE_H
#define DRV_L2_NAND_MANAGE_H
#include "project.h"

#ifndef GPL326XXB_BCH60

#include "drv_l2_nand_config.h"

#if 0
	#define NF_DBG_WHILE(x)		 
#else
	#define NF_DBG_WHILE(x)		while(x)
#endif

//------------C Area Flag define-----------------------
//--------NAND READ WRITE AREA define---------------------
#define NAND_C_AREA_BAD_FLG_OFS_1					(0x00)	// 1 byte
#define NAND_C_AREA_COUNT_OFS						(0x01)  // 1 byte
#define NAND_C_AREA_LOGIC_BLK_NUM_OFS				(0x02)	// 2 byte
#define NAND_C_AREA_EXCHANGE_BLK_TYPE_OFS			(0x04)	// 1 byte
#define NAND_C_AREA_BAD_FLG_OFS_6					(0x05)	// 1 byte
#define NAND_C_AREA_PAGE_TO_PAGE_OFS				(0x06)	// 2 byte
//yunxiangan add to distinguish the 8k nand // yunxiangan 2010-11-05
#define NAND_C_AREA_HAVE_MANAGE_TAG_LOCATION		(0x08)// 2 byte
//#define NAND_C_AREA_BAD_FLAG_LOCATION_HAVE_GP		(0X2080)
//#define NAND_C_AREA_BAD_FLAG_LOCATION_NO_GP			(0X2000)
//---------------------------------------------------------
#define NF_READ										0x01
#define NF_WRITE									0x10
//----------------------------------------------------------
#define	NAND_BUF_FLUSH_ALL							(1<<0)		//force to flush A exchange , become all free		
#define	NAND_BUF_GET_EXCHANGE						(1<<1)		//get a block 
#define	NAND_ALL_BUF_FLUSH_ALL						(1<<2)		//get a block 
//#define NAND_SECTOR2WORD_SHIFT_BIT					0x08

//--------------------------------------------------------
#define NAND_ERR_UNKNOW_TYPE						0xfff0
#define	NAND_BAD_BLK_USER_MARK				        0xff42
#define NAND_FIRST_SCAN_BAD_BLk_MARK		    	0xff5a
#define NAND_SAVE_HEAD                              0xff60
#define NAND_USR_MARK_BAD_BLK				        0xff42
#define NAND_START_WRTIE                            0xff55
#define NAND_END_WRTIE                              0xff66
#define NAND_GOOD_BLK					            0xffff
#define NAND_ALL_BIT_FULL				            0xffff
#define NAND_ALL_BIT_FULL_B							0xff
#define NAND_ALL_BIT_FULL_W							0xffff
#define NAND_MAPTAB_UNUSE							0x7fff
#define NAND_EMPTY_BLK								0xffff
#define NAND_GOOD_TAG								0xff
#define	NAND_ORG_BAD_BLK							0xff00
#define NAND_ORG_BAD_TAG							0x00
#define	NAND_USER_BAD_BLK							0xff44
#define	NAND_USER_BAD_TAG							0x44
#define NAND_FIRST_SCAN_BAD_BLK						0xff55
#define NAND_FIRST_SCAN_BAD_TAG						0x55
#define NAND_MAPTABLE_BLK							0xff66		// word
#define NAND_MAPTABLE_TAG							0xff66		// word
#define NAND_SAVE_HEAD_BLK							0xff60
#define NAND_SAVE_HEAD_TAG							0xff60
#define NAND_START_WRITE                    		0x88
#define NAND_END_WRITE                      		0x99
#define NAND_EXCHANGE_A_TAG							0xaa
#define NAND_EXCHANGE_B_TAG							0xbb
#define NAND_BANK_INFO_TAG							0xcc
#define NF_EXCHANGE_A_TYPE							NAND_EXCHANGE_A_TAG
#define NF_EXCHANGE_B_TYPE							NAND_EXCHANGE_B_TAG

//yunxiangan add to distinguish the 8k nand 
#define NAND_HAVE_MANAGE_BY_GP						0x5047		// yunxiangan 2010-11-05
#define NAND_HAVE_MANAGE_G_TAG						0x47
#define NAND_HAVE_MANAGE_P_TAG						0x50		
//---------------------------------------------------------------------

#define NAND_FORMAT_START_TAG						0xdd

#define MAPTAB_WRITE_BACK_N_NEED					0x01
#define MAPTAB_WRITE_BACK_NEED						0x10
#define MAPTAB_N_DIRTY								0x01
#define MAPTAB_DIRTY								0x10
#define MAPTAB_VALID								0x01
#define MAPTAB_INVALID								0x10
#define NAND_FREE_BLK_MARK_BIT						(1<<15)
//#define NF_APP_LOCK                                  0xfffe
#define NF_PHY_WRITE_FAIL                           0x10
#define NF_PHY_READ_FAIL                            0x01

#define NAND_MLC									0xf1
#define NAND_SLC_LARGE_BLK							0xf2
#define NAND_SLC_SMALL_BLK							0xf3

#define MAX_BAD_BLK									0x20
#define NAND_Illegal_BLK_SIZE						8
#define ExchangeAMaxNum								2
#define ExchangeBMaxNum								3

typedef struct
{
	INT16U bad_blk[MAX_BAD_BLK];
	INT16U wArrayIndex;
}BAD_BLK_ARRAY;


typedef struct
{
	INT16U	wPhysicBlkNum;
	INT16U	wLogicBlkNum;
	INT16U  wCount;
}IBT;

typedef struct
{
	INT16U	wBank;
	INT16U	wLogicBlk;
	INT16U	wPhysicBlk;
	INT16U	wCurrentPage;	
	INT16U	wLogicalPage;
	INT16U	wCount;	
	INT16U	wType;
	INT16U	wPage2PageTable[256];	// Èç¹ûÓÐ256 Page/Block
}EXCHANGE_A_BLK;


#ifndef _MANAGER_NANDINFO
#define _MANAGER_NANDINFO
typedef struct
{
	INT16U    wPageSectorMask;
	INT16U    wPageSectorSize;
	INT16U    wBlk2Sector;
	INT16U    wBlk2Page;
	INT16U    wPage2Sector;
	INT16U    wSector2Word;
	INT32U    wNandBlockNum;
	INT16U    wPageSize;
	INT16U    wBlkPageNum;
	
	INT16U	  wNandType;
	INT16U	  wBadFlgByteOfs;
	INT16U	  wBadFlgPage1;
	INT16U	  wBadFlgPage2;	
}Manager_NandInfo;
#endif

//--------NAND ERROR INFOMATION define--------------------

#ifndef _NF_STATUS
#define _NF_STATUS
typedef enum
{
	NF_OK 						= 0x0000,
	NF_UNKNOW_TYPE				= 0xff1f,
	NF_USER_CONFIG_ERR			= 0xff2f,
	NF_TOO_MANY_BADBLK			= 0xff3f,
	NF_READ_MAPTAB_FAIL			= 0xff4f,
	NF_SET_BAD_BLK_FAIL			= 0xff5f,
	NF_NO_SWAP_BLOCK			= 0xff6f,
	NF_BEYOND_NAND_SIZE			= 0xff7f,
	NF_READ_ECC_ERR				= 0xff8f,
	NF_READ_BIT_ERR_TOO_MUCH    = 0xff9f,
	NF_ILLEGAL_BLK_MORE_2		= 0xffaf,
	NF_BANK_RECYCLE_NUM_ERR		= 0xffbf,
	NF_EXCHAGE_BLK_ID_ERR		= 0xffcf,
	NF_DIRECT_READ_UNSUPPORT	= 0xffdf,
	NF_DMA_ALIGN_ADDR_NEED		= 0xfff0,
	NF_APP_LOCKED				= 0xfff1,
	NF_NO_BUFFERPTR				= 0xfff2,
	NF_FIX_ILLEGAL_BLK_ERROR	= 0xfff3,
	NF_FORMAT_POWER_LOSE		= 0xfff4,
	NF_NAND_PROTECTED			= 0xfff5,
	NF_FIRST_ALL_BANK_OK        = 0xffef
}NF_STATUS;
#endif


extern INT16U		gBitErrCntAsBadBlk;

//////////////////------  APP Global Varibles Decalre  ///////////////////////
extern INT16U  		gAPPCurrPage;
extern INT16U  		gAPPCurrLogicBlock;
extern INT16U  		gAPPCurrPhysicBlock;
extern INT16U  		gAPPCurrSector;
extern INT16U  		gAPPBadBlkNum;
extern INT32U  		gAPPLastpage;
extern INT16U  		gAPPformatFlag;
extern INT16U   	gAPPSpeedUp_Flag;
extern INT16U 		gSysCodeWriFlag;
extern TT   		APP_tt_BK;
//extern BAD_BLK_OFFSET 	gbadflag_info_app;
extern BAD_BLK_ARRAY  	bad_appblk_info;

//////////////////------  Data Global Varibles Decalre  ///////////////////////

extern TT 					*tt_basic_ptr;
extern TT 					*tt_extend_ptr;
extern INT32U				gWorkBufferNum;

extern ALIGN16 BANK_INFO  	gBankInfo[BANK_NUM];

extern ALIGN16 MM			mm[1];
extern INT16U 				gMaptableChanged[1];

extern EXCHANGE_A_BLK		gExchangeABlock[ExchangeAMaxNum];
extern EXCHANGE_A_BLK		gExchangeBBlock[ExchangeBMaxNum];
extern EXCHANGE_A_BLK		*gpWorkBufferBlock;

extern Manager_NandInfo 	gNandInfo;
extern NF_USER_INFO 		gNandUsrInfo;

//extern BAD_BLK_OFFSET 		gbadflag_info;
extern BAD_BLK_ARRAY 		bad_blk_info;

extern INT16U				gLogic2PhysicAreaStartBlk;
extern INT32U				gBankInfoCurPage;

extern INT16U    			gCurrentBankNum;
extern INT16U 				gTotalBankNum;

extern INT32U 				gLastPage_Write;
extern INT32U 				gLastPage_Read;

extern INT16U       		gInitialFlag;
extern INT16U          		gPhyBlkNum;

extern INT32U 				gNandSize;
extern INT32U 				gFirstFATStart;
extern INT32U 				gFirstFATDataStart;
extern INT32U 				gSecondFATStart;
extern INT32U 				gSecondFADataStart;

extern INT8U				gCheckOrgBadBlockBCHStatus;
extern INT16U 				gBitErrCntAsBadBlk;

extern const INT32U 		FATNandTable[];

extern INT8U				gNandHaveManageByGpFlag; //yunxiangan add 2010-11-16
extern INT8U				gNandHaveManageByGpFlag_App;
extern INT32U				gAppAreaStartAddr;		//add for support nand OTP
extern INT32U				gDataAreaStartAddr;

extern INT16U gNFHWInitFlag;  //added by Eric 2012/11/17 for hw/app init.
extern INT16U gAPPinitFlag;
//////////////////------  APP Funcitons Decalre  ///////////////////////
extern INT32U CalculateFATArea(void);
extern INT32U NandBootBuildL2P(INT32U format_type);
extern INT32U NandBootFormat(INT16U format_type);
extern INT32U NandBootInit(void);
extern INT16U GetRecycleInfo(INT16U wPhysicBlk);
extern INT16U CheckOrgBadBlk(INT16U wPhysicBlkNum);
extern INT32U NandBootWriteSector(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootWriteSector_OnePageUpdate(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootReadSector(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootLoaderRead(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);
extern INT32U NandBootWriteFail(void);
extern INT32U NandBootFlush(void);
extern INT16U GetPhysicBlockNum(INT16U wLogicNum);
extern INT32U PutSwapPhysicBlock(INT16U wPhysicBlock);
extern INT32U GetSwapPhysicBlock(void);
extern INT32U UpdateL2PTbl(INT16U wLogicNum,INT16U wPhysicNum);
extern INT32U GetNFInfo(void);
//extern INT32U BootReadPage(INT32U wPhysicPageNum);
extern INT32U BootWritePage(INT32U wPhysicPageNum);
extern INT32U BootEraseBlock(INT16U BlockNum);
extern INT32U BootCopyMultiPhyscialPage(INT16U wSRCBlkNum, INT16U wTARBlkNum, INT16U wStartPage, INT16U wEndPage,INT16U wLogicBlk);
extern void BootSetBadFlag(INT16U wPhysicBlkNum, INT16U ErrFlag);
extern void NandBootDisableWrite(void);
extern void NandBootEnableWrite(void);
extern INT16U ChangeReadErrBlk_APP(void);
extern INT16U IsAPPBlk_GoodOrBad(INT16U wblknum, INT16U wblktemp);
extern INT16U GetMapTblInfo(INT16U wPhysicBlk);
extern INT32U BootReadPage(INT32U wPhysicPageNum,INT32U DataAddr);
extern INT16U NandBootEraseLogicBlk(INT16U wLogicBlk);
extern INT16S CopyPageToPage(INT16U wSrcBlock,INT16U wTargetBlock,INT16U wPage);
extern void SetNandRomAppAreaStartAddr(INT32U AppAreaStartAddr);
extern INT32U GetNandRomAppAreaStartAddr(void);

//extern //////////////////------  Data Funcitons Decalre  ///////////////////////
extern INT16U DrvNand_UnIntial(void);
extern INT16U DrvNand_initial(void);
extern INT16U DrvNand_initial_Update(void);
extern INT16S FixBankExchangeBlk(INT16U wTargetBankNum,INT32U wBankStartBlk,INT32U wBankEndBlk);
extern void GlobalVarInit(void);
extern INT32U CalculateFATArea(void);
extern INT16U FindMapTableCurrentPage(INT16U wPhysicBlk);
extern INT32S GetNandParam(void);
extern INT32U DrvNand_get_Size(void);
extern INT16U GetBadFlagFromNand(INT16U wPhysicBlkNum);
extern void SetBadFlagIntoNand(INT16U wPhysicBlkNum, INT16U ErrFlag);
extern void SetBadFlagIntoNand_BootArea(INT16U wPhysicBlkNum, INT16U ErrFlag);
extern INT16U ReadLogicNumFromNand(INT16U wPhysicBlkNum);
extern INT16S ReadMapTableFromNand(INT16U wBankNum);
extern INT16U WriteMapTableToNand(INT16U wBankNum);
extern void ChangeBank(INT16U wBankNum);
extern void FlushCurrentMaptable(void);
extern void UpdateMaptable(INT16U wSwapBlkNum, INT16U wLogicNum);
extern INT16U GetFreeBlkFromRecycleFifo(void);
extern void PutFreeBlkIntoRecycleFifo(INT16U wPhysicBlkNum);
extern INT32S NandFlushA(INT16U wBlkIndex, INT16U TargetBank, INT32S wMode, INT16U wType);
extern INT16U	DrvNand_flush_allblk(void);
extern INT32S CopyMultiPhysicalPage(INT16U wSRCBlkNum, INT16U wTARBlkNum, INT16U wStartPage, INT16U wEndPage, INT16U wLogicBlkNum,INT16U wType);
extern INT32S ReadDataFromNand(INT32U wPhysicPageNum, TT *tt_ptr);
extern INT32S WriteDataIntoNand(INT32U wPhysicPageNum,TT *tt_ptr);
extern INT32S EraseNandBlk(INT16U wPhysicBlkNum);
extern INT16S FixupIllegalBlock(INT16U* pIllegalTable , INT16U wCount);
extern INT16U FindBlkCurrentPage(EXCHANGE_A_BLK *Blk);
extern INT16U DrvNand_lowlevelformat_UP(INT16U step);
extern INT16U DrvNand_lowlevelformat(void);
extern void WriteBackWorkbuffer(EXCHANGE_A_BLK *pBlk);
extern INT16U GetExchangeBlock(INT16U wType, INT16U wBank, INT16U wLogicBlk);
extern INT32S ReadNandAreaA(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr,INT16U wReadType);
extern INT32S WriteNandAreaA(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr,INT16U wWriteType);
extern INT16U DrvNand_read_sector(INT32U wReadLBA, INT16U wLen, INT32U DataBufAddr);
extern INT16U DrvNand_write_sector(INT32U wWriteLBA, INT16U wLen, INT32U DataBufAddr);
extern INT16U NandXsfer(INT32U wLBA, INT16U wLen, INT32U wAddr,INT16U XsferMode);
extern INT16U APPGetCArea(INT16U byte_ofs,TT *WorkBuffer);
extern INT16U GetCArea(INT16U byte_ofs,TT *WorkBuffer);
extern void APPPutCArea(INT16U byte_ofs,INT16U data, TT *WorkBuffer);
extern void PutCArea(INT16U byte_ofs,INT16U data, TT *WorkBuffer);
extern void SaveBadBlkInfo(INT16U wBlkNum);
extern INT16U ChangeReadErrBlk(void);
extern INT16U IsBlk_GoodOrBad(INT16U wblknum, INT16U wblktemp);
extern INT32S Set_CalculateFATArea (INT16U partnum, INT32U DataStartSec);
extern INT32S Default_CalculateFATArea (void);
extern void SetWorkBuffer(TT* p_tt_basic, TT* p_tt_extend);
extern INT32U combin_reg_data(INT8U *data,INT32S len);
extern INT16U	GetNextGoodBlock(INT16U cur_block);
extern INT16S	BankInfoAreaInit(void);
extern INT16S	BankInfoAreaRead(INT32U pBuf, INT32U bytes);
extern INT16S	BankInfoAreaWrite(INT32U pBuf, INT32U bytes);
extern INT16S	NandWriteBankInfoAreaExt(INT32U pBuf, INT32U bytes,INT16U mode);
extern INT32U DMAmmCopy(INT32U wSourceAddr, INT32U wTargeAddr, INT16U Count);
extern INT32S Nand_OS_Init(void);
extern INT32U Nand_Init(void);
extern INT32S Nand_OS_Exit(void);
extern void Nand_OS_LOCK(void);
extern void Nand_OS_UNLOCK(void);
extern void Sys_Exception(INT32S errorcode);
extern void GetNandInfo(NF_USER_INFO *user_info);
extern void SetNandInfo(INT16U app_start,INT16U app_size,INT16U app_Percent,  INT16U data_BankSize, INT16U data_BankRecSize);
extern void Nand_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum,INT32U* NandID);
extern void nf_memset_w(void *address, INT16U value, INT16U len);
extern void nf_memcpy_w(void *taraddr, void *srcaddr, INT16U len);
extern void SetWorkBuffer(TT* p_tt_basic, TT* p_tt_extend);
extern INT32U CalculateFATArea(void);
extern INT32U GetNandBadBlkNumber(INT16U wBadType);
extern INT32U DrvNand_RecoveryBlk(void);
extern INT32S NandSetOrgLogicBlock(INT16U wLogicBlk,INT16U wType);
extern INT16U GetBitErrCntAsBadBlk(void);
extern void SetCheckOrgBadBlkBCHOff(void);
extern INT8U GetCheckOrgBadBlkBCHStatus(void);
extern void SetNandRomDataAreaStartAddr(INT32U DataAreaStartAddr);
extern INT32U GetNandRomDataAreaStartAddr(void);

#endif //GPL326XXB_BCH60

#endif		// __drv_l2_NAND_MANAGE_H__
