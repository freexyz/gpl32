/*
* Purpose: Interrupt Rquest Controller driver/interface
*
* Author: Tristan Yang
*
* Date: 2007/09/21
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/

#include "drv_l1_memc.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_MEMC) && (_DRV_L1_MEMC == 1)                 //
//================================================================//


void memc_init(void)
{
	if (MHZ == 96) {
        //R_MEM_CS1_CTRL  = C_MEMC_CTRL1_CAS_LATENCY2 | C_MEMC_CTRL1_INIT4 | C_MEMC_CTRL1_REFRESH_CYCLE0; 
        R_MEM_SDRAM_TIMING = C_MEMC_TIMING_RAS_CAS_DELAY2 | C_MEMC_TIMING_ROW_PRECHARGE2 | C_MEMC_TIMING_ROW_ACTIVE7 | C_MEMC_TIMING_ROW_CYCLE8;
        if ((SDRAM_END_ADDR-SDRAM_START_ADDR+1) < 0x02000000) {
        	R_MEM_SDRAM_CBRCYC = 0x05DA;		
        } else {
        	R_MEM_SDRAM_CBRCYC = 0x02EC;   		// for 256Mb/512Mb SDRAM 
        }
	} else {
        //R_MEM_CS1_CTRL  = C_MEMC_CTRL1_CAS_LATENCY2 | C_MEMC_CTRL1_INIT4 | C_MEMC_CTRL1_REFRESH_CYCLE0; 
        R_MEM_SDRAM_TIMING = C_MEMC_TIMING_RAS_CAS_DELAY1 | C_MEMC_TIMING_ROW_PRECHARGE1 | C_MEMC_TIMING_ROW_ACTIVE3 | C_MEMC_TIMING_ROW_CYCLE4;
        if ((SDRAM_END_ADDR-SDRAM_START_ADDR+1) < 0x02000000) {
        	R_MEM_SDRAM_CBRCYC = 0x02EC;
		} else {
			R_MEM_SDRAM_CBRCYC = 0x0176;   		// for 256Mb/512Mb SDRAM 
		}
	}
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_MEMC) && (_DRV_L1_MEMC == 1)            //
//================================================================//