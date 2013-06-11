#ifndef __drv_l2_NAND_MANAGE_H__
#define __drv_l2_NAND_MANAGE_H__

#include "project.h"

#ifndef GPL326XXB_BCH60

#include "driver_l2.h"
//------------- NAND define ---------------------------
//#define NULL						0x00		//only flush target block
#define	NAND_BUF_FLUSH_ALL				(1<<0)		//force to flush A exchange , become all free		
#define	NAND_BUF_GET_EXCHANGE				(1<<1)		//get a block 
#define	NAND_ALL_BUF_FLUSH_ALL				(1<<2)		//get a block 
#define NAND_SECTOR2WORD_SHIFT_BIT			0x08
//-----------------------------------------------------

//------------NAND Config define-----------------------//
#define BOOT_AREA_START			1
#define BOOT_AREA_SIZE			40//((gBootAreaSize+511)>>9)			// 10M
#define BOOT_AREA_SPARE_START	(BOOT_AREA_START + BOOT_AREA_SIZE)
#define	BOOT_AREA_SPARE_SIZE	(BOOT_AREA_SIZE/4)
#define BOOT_AREA_MAX_SIZE		30
#define PHYSIC_BLOCK_START		(BOOT_AREA_SPARE_START + BOOT_AREA_SPARE_SIZE) //	gBootAreaSize
//------------END OF CONFIG------------------------------//
//------------C Area Flag define-----------------------
#define NAND_C_AREA_BAD_FLAG				2048//1024
#define NAND_C_AREA_LOGIC_BLK_NUM			2050//1025
#define NAND_C_AREA_COUNT				2052//1026
#define NAND_C_AREA_EXCHANGE_BLK_TYPE			2054//1027
#define NAND_C_AREA_PAGE_TO_PAGE			2056//1028
//--------------------------------------------------------	
#ifndef _NAND_L2_DEFINE_    // jandy add
#define _NAND_L2_DEFINE_    // jandy add
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
    #define MAPTAB_WRITE_BACK_N_NEED					0x01
    #define MAPTAB_WRITE_BACK_NEED						0x10
    #define NAND_FREE_BLK_MARK_BIT						(1<<15)
    #define NF_APP_LOCK                                  0xfffe
    #define NF_PHY_WRITE_FAIL                           0x10
    #define NF_PHY_READ_FAIL                            0x01
#endif //_NAND_L2_DEFINE_  
//---------------------------------------------------------
#define NO_SWAP_BLOCK                                	0x200
#define SET_BAD_BLK_FAIL				0x201
#define ERASE_BLK_0		                        0x202
#define MLCDEBUG_TYPE	                             	0x203
#define OVERSTEPNANDTOTAL	                        0x204
#define ReadMapTableFail	                        0x205
#define HaveSameERR	                                0x206
//---------------------------------------------------------
#define NF_READ						0x01
#define NF_WRITE					0x10
//----------------------------------------------------------
#define NF_INIT_FINSH   				0x0001
//---------------------------------------------------------

#endif  //GPL326XXB_BCH60

#endif		// __drv_l2_NAND_MANAGE_H__


