#include "application.h"
#include "drv_l1_nor_flash.h"

#define NOR_FLASH_TAG       "PGonnrroon"

typedef struct RegSet_s {
	INT32U reg_addr;
	INT32U reg_value;
}RegSet_t;

typedef struct NorHeader_s {
	INT8U tag[12];
	INT32U SdramDstAddr;
	INT16U SdramSectorCnt;
	INT8U  RegCnt;
	INT8U  JumpSdramFlag;
	INT32U NorSrcAddr;

	INT32U StartRunAddr;
	INT16U SdramCtrl0;
	INT16U SdramCtrl1;
	INT16U SdramTiming;
	INT16U SdramCbrcyc;
	INT16U SdramMisc;
	INT8U Reserved2[2];
 	RegSet_t RegSet[11];
}NorHeader_t;

extern INT32U NorBootHeader_Start;
extern INT32U NorBootHeader_End;
extern INT32U NorBootLoader_Start;
extern INT32U NorBootLoader_End;
extern INT32U RunTimeCode_Start;
extern INT32U RunTimeCode_End;

#define CS1_Base_Addr	0xB0000000
INT32S NorWrite_BootLoader(INT32U *pBootLoader, INT32U bootloader_size, INT32U NorAddr, INT32U runtime_code_size)
{
	INT32S i;
	INT32U size, temp;
	INT32U *pSrc, *pDst;
	NorHeader_t *pHeader;
	DBG_PRINT("Nor Write Header.\r\n");
	
	pHeader = (NorHeader_t *)&NorBootHeader_Start;
	
	pHeader->SdramSectorCnt = runtime_code_size / 512;
	if(runtime_code_size % 512) {
		pHeader->SdramSectorCnt ++;
	}
	
	pHeader->RegCnt = 0x00;
#if 0 //run in sdram	
	pHeader->JumpSdramFlag = 1;
	pHeader->SdramDstAddr = 0x20000000;
	pHeader->NorSrcAddr = NorAddr;//0x10002000;
	pHeader->StartRunAddr = 0x00000000;
#else //run in nor flash
	pHeader->JumpSdramFlag = 0;	
	pHeader->SdramDstAddr = 0x20000000;
	pHeader->NorSrcAddr = NorAddr;//0x10002000;
	pHeader->StartRunAddr = NorAddr;
#endif	

	pHeader->SdramCtrl0 = 0x3113;
	pHeader->SdramCtrl1 = 0x2000;
	pHeader->SdramTiming = 0xF8A;
	pHeader->SdramCbrcyc = 0x2EC;
	pHeader->SdramMisc = 0x0006;
	
	size = sizeof(NorHeader_t) >> 2;
	pSrc = (INT32U *)pHeader;
	pDst = (INT32U *)CS1_Base_Addr;
	for(i=0; i<size; i++) {
		temp = *pSrc;
		nor_flash_single_word_program((INT32U*)CS1_Base_Addr, pDst, temp);
		pSrc++;
		pDst++;
		
		if((i%32) == 0) {
			DBG_PRINT(".");
		}
	}
	
	DBG_PRINT("Nor Write BootLoader\r\n");
	size = bootloader_size >> 2;
	if(bootloader_size % 4) {
		size++; 
	}
	
	pSrc = pBootLoader;
	pDst = (INT32U *)(CS1_Base_Addr + 0x80);
	for(i=0; i<size; i++) {
		temp = *pSrc;
		nor_flash_single_word_program((INT32U*)CS1_Base_Addr, pDst, temp);
		pSrc++;
		pDst++;
		
		if((i%32) == 0) {
			DBG_PRINT(".");
		}
	}
	
	DBG_PRINT("Nor Write Finish.\r\n");
	return 0;
}

INT32S NorWrite_RunCode(INT32U *pRunCode, INT32U runtime_code_size)
{
	INT32S i;
	INT32U size, temp;
	INT32U *pSrc, *pDst, *pVerify;
	
	DBG_PRINT("Nor Write Run Time Code\r\n");
	size = runtime_code_size >> 2;
	if(runtime_code_size % 4) {
		size ++;
	}
	
	pSrc = pVerify = pRunCode;
	pDst = (INT32U *)(CS1_Base_Addr + 0x2000);
	for(i=0; i<size; i++) {
		temp = *pSrc;
		nor_flash_single_word_program((INT32U*)CS1_Base_Addr, pDst, temp);
		pSrc++;
		pDst++;
		
		if((i%512) == 0) {
			DBG_PRINT(".");
		}
	}
	
	DBG_PRINT("Nor Verify.\r\n");
	pDst = (INT32U *)(CS1_Base_Addr + 0x2000);
	for(i=0; i<(size-1); i++) {
		temp = *pDst;
		if(temp != *pVerify) {
			DBG_PRINT("Nor Verify Fail.\r\n");
			return -1;
		}
		pDst++;
		pVerify++;
		
		if((i%512) == 0) {
			DBG_PRINT(".");
		}
	}
	DBG_PRINT("Nor Verify Success.\r\n");
	return 0;
}

void Nor_Flash_Write_Demo(void)
{
	INT32S nRet;
	INT32U bootloader_size;
	INT32U runtimecode_size;
	INT32S total_size;
	INT32U nor_addr;
	
	DBG_PRINT("Nor Init...\r\n");
	nor_flash_init96(1, NON_PAGE_MODE);
	
	bootloader_size = (INT32U)&NorBootLoader_End - (INT32U)&NorBootLoader_Start; 
	runtimecode_size = (INT32U)&RunTimeCode_End - (INT32U)&RunTimeCode_Start; 	
#if 0	
	DBG_PRINT("Nor Chip Erase.\r\n");
	nor_flash_chip_erase(CS1_Base_Addr);
#else
	total_size = bootloader_size + runtimecode_size;
	nor_addr = CS1_Base_Addr;
	while(total_size > 0) {
		DBG_PRINT("Nor sector Erase = 0x%x\r\n", nor_addr);
		nor_flash_sector_erase(CS1_Base_Addr, nor_addr);
		nor_addr += 0x10000;
		total_size -= 0x10000;
	}	
#endif	
#if 1
	nRet = NorWrite_BootLoader((INT32U *)&NorBootLoader_Start, 
								bootloader_size, 
								0x10002000, 
								runtimecode_size);
#endif
#if 1	
	nRet = NorWrite_RunCode((INT32U *)&RunTimeCode_Start, runtimecode_size);
#endif	
	while(1);
}

