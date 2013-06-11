/* 
* Purpose: Nor flash driver/interface
*
* Author:
*
* Date: 2008/5/9
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

//Include files
#include "drv_l1_nor_flash.h"


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_NOR) && (_DRV_L1_NOR == 1)                   //
//================================================================//


// Constant definitions used in this file only go here

// Type definitions used in this file only go here
#define	BIT31			  0x80000000
#define	LOW_BYTE		  0X0000FFFF

#define REG32_RD(addr, data)	data = (*((volatile INT32U *)addr))
#define REG32_WR(addr, data)	(*((volatile INT32U *)addr)) = data
// Global inline functions(Marcos) used in this file only go here

// Declaration of global variables and functions defined in other files

// Variables defined in this file

// Functions defined in this file

/*
Note:
1.GPL32_A/B/C Memory Module V1.0 used SRAM/NOR CS Select Pin Only in CS0/CS1. 
2.Non-Cache Memory Mapping:
CS0(SRAM)      Start address at 0x80000000.
CS1(NOR-Flash) Start address at 0x90000000.
3.SRAM/NOR-Flash:Spansion-71PL256ND(Page Mode Flash Memory, 32MByte SRAM+32MByte NOR-Flash)
4.SRAM Page type=Burst 4 cycle, NOR-Flash=Burst 8 cycle

A.CS Control Register (CS0/1/2/3)
   Address	     Function	Reset	R/W	 B13-b12	 b11	B10 - b8	 B7 - b4	B3-B2	 B1 - b0
   0xd0200000              0x00f5    R	   --	     --	    PageCycle   NormCycle  MemWidth	  Mode
   ~             P_CSnCTRL
   0xd0200014			     --      W	 PageType	  0	    PageCycle	NormCycle  MemWidth	  Mode

   //Control Register instruction.		
   1.P_CSnCTRL: n : 0~3
   2.Page type 
			00: disable page access
			01: Burst 4 cycle
			10: Burst 8 cycle
			11: Burst 16 cycle  
   3.PageCycle: Access cycle, 0~15T; T: System clock for page mode, 
   4.NormCycle: Access cycle, 0~15T; T: System clock
   5.MemWidth: Memory width, 8(00)/16(01)/32(10) bits
   6.Mode: Memory type, ROM(00)/SRAM(01)/ FLASH(10)/Reserved(11)

B.NOR-Flash Address Control Register 
  Address	     Function	Reset	R/W	   B[23:0]
  0xd0200024	rFlashADDR	0x00	 R	FlashAddr[24:0]
		                     --	     W	FlashAddr[24:0]
*/

void nor_flash_cmd(INT32U addr, INT32U data)
{   
    R_MEM_NOR_FLASH_ADDR= addr<<1;
    REG32_WR(addr, data);
}

void nor_flash_write(INT32U addr, INT32U data)
{  
    R_MEM_NOR_FLASH_ADDR= addr;
    REG32_WR(addr, data);  
}
	    
void nor_flash_wait_data_polling(INT32U *addr)
{
    INT32U data;
 	
 	REG32_RD((INT32U) addr, data);
    while((data&BIT31)==0){
        REG32_RD((INT32U) addr, data);
    }       
}

void nor_flash_chip_erase(INT32U baseaddr)
{
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x00aa);
  nor_flash_cmd ((INT32U)baseaddr+0x2aa, 0x0055);
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x0080);
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x00aa);
  nor_flash_cmd ((INT32U)baseaddr+0x2aa, 0x0055);
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x0010);
  nor_flash_wait_data_polling((INT32U *)baseaddr); 
}

void nor_flash_single_word_program(INT32U *baseaddr, INT32U *startaddr, INT32U data)
{    
    INT32U	Taddr,nBytes,nBytesH,nBytesL, TData;
    
    nBytes=data;
    nBytes=(nBytes&LOW_BYTE);
    nBytesL=nBytes;
    nBytes=data;
    nBytes=(nBytes>>16);
    nBytesH=nBytes;    
    //Low-16bit
    nor_flash_cmd((INT32U)baseaddr+0x555, 0x00aa);
    nor_flash_cmd((INT32U)baseaddr+0x2aa, 0x0055);
    nor_flash_cmd((INT32U)baseaddr+0x555, 0x00a0);           
    nor_flash_write((INT32U)startaddr, nBytesL);
    
    REG32_RD((INT32U) startaddr, TData);     
    TData=(TData&LOW_BYTE);
    TData=(TData-nBytesL);
    while(TData!=0){
         REG32_RD((INT32U) startaddr, TData);     
         TData=(TData&LOW_BYTE);
         TData=(TData-nBytesL);
    }
    //Addr++
    Taddr=(INT32U)startaddr;
    Taddr=Taddr+2;
    startaddr=(INT32U *)Taddr;    
    //HI-16bit
    nor_flash_cmd((INT32U)baseaddr+0x555, 0x00aa);
    nor_flash_cmd((INT32U)baseaddr+0x2aa, 0x0055);
    nor_flash_cmd((INT32U)baseaddr+0x555, 0x00a0);           
    nor_flash_write((INT32U)startaddr, nBytesH);   
     
    REG32_RD((INT32U) startaddr, TData);     
    TData=(TData&LOW_BYTE);
    TData=(TData-nBytesH);
    while(TData!=0){
         REG32_RD((INT32U) startaddr, TData);     
         TData=(TData&LOW_BYTE);
         TData=(TData-nBytesH);
    }
}

void nor_flash_init96(INT32U CSn, INT32U nor_state)
{
	//Set IOH[0] as memory Output Enable (OE) Pin. 
	R_FUNPOS1 &= ~(1 << 2);

	switch(nor_state)
    {
    case NON_PAGE_MODE:
    	if(CSn == 0) {
    	 	//R_MEMC_CTRL0=0x00000085;//80ns/sram
	     	R_MEM_CS0_CTRL=0x00000086;//80ns/nor-flash
         } else if(CSn == 1) {
         	R_MEM_CS1_CTRL=0x00000086;//80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 12;
         } else if(CSn == 2) {
         	R_MEM_CS2_CTRL=0x00000086;//80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 13;
         } else if(CSn == 3) {
         	R_MEM_CS3_CTRL=0x00000086;//80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 14;
         }
         break;
    case PAGE_MODE:
    	if(CSn == 0) {
    		//R_MEMC_CTRL0=0x00000085;//80ns/sram
	    	R_MEM_CS0_CTRL=0x00001486;//Burst 4 cycle/40ns/80ns/nor-flash
        } else if(CSn == 1) {
         	R_MEM_CS1_CTRL=0x00001486;//Burst 4 cycle/40ns/80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 12;
         } else if(CSn == 2) {
         	R_MEM_CS2_CTRL=0x00001486;//Burst 4 cycle/40ns/80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 13;
         } else if(CSn == 3) {
         	R_MEM_CS3_CTRL=0x00001486;//Burst 4 cycle/40ns/80ns/nor-flash
         	R_MEM_IO_CTRL |= 1 << 14;
         }   
         break;
    }	
}

void nor_flash_sector_erase(INT32U baseaddr,INT32U startaddr)
{
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x00aa);
  nor_flash_cmd ((INT32U)baseaddr+0x2aa, 0x0055);
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x0080);
  nor_flash_cmd ((INT32U)baseaddr+0x555, 0x00aa);
  nor_flash_cmd ((INT32U)baseaddr+0x2aa, 0x0055);
  nor_flash_write((INT32U)startaddr, 0x0030);
  nor_flash_wait_data_polling((INT32U *)baseaddr); 
}


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_NOR) && (_DRV_L1_NOR == 1)              //
//================================================================//