#ifndef __drv_l1_NOR_FLASH_H__
#define __drv_l1_NOR_FLASH_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define NON_PAGE_MODE     0
#define PAGE_MODE         1

void nor_flash_init96(INT32U CSn, INT32U nor_state);
void nor_flash_chip_erase(INT32U baseaddr);
void nor_flash_sector_erase(INT32U baseaddr,INT32U startaddr);
void nor_flash_single_word_program(INT32U *baseaddr, INT32U *startaddr, INT32U data);

#endif		// __drv_l1_NOR_FLASH_H__
