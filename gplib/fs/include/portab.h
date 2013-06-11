/****************************************************************/
/*                                                              */
/*                           portab.h                           */
/*                                                              */
/*                 DOS-C portability typedefs, etc.             */
/*                                                              */
/*                         May 1, 1995                          */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/* changelog:                                                   */
/* wanghuidi,2004.12.10, adjust code for portablity             */
/****************************************************************/
#ifndef __PORTAB_H__
#define __PORTAB_H__

/****************************************************************/
/*platform dependant*/
/****************************************************************/
#if MALLOC_USE == 1
	// unsp 环境下对外部内存的使用
	#define		FS_EXTMEM_ALLOC		ucBSExtMalloc
	#define		FS_EXTMEM_FREE		ucBSExtFree
	// unsp 环境下对内部内存的使用
	#define		FS_MEM_ALLOC		ucBSMalloc
	#define		FS_MEM_FREE			ucBSFree
#else
    // unsp 环境下对外部内存的使用
	#define		FS_EXTMEM_ALLOC
	#define		FS_EXTMEM_FREE
	// unsp 环境下对内部内存的使用
	#define		FS_MEM_ALLOC
	#define		FS_MEM_FREE
#endif
	
/**************************************************************************
 *                  D A T A    P A C K    C O N T R O L                   *
 **************************************************************************/
#define REG				register
#define FP_OFF(fp)		((INT32U) (fp))
#define	LITTLE_ENDIAN

#ifdef unSP
	#define WORD_PACK_VALUE	1	// 1 for 2 byte
	#define STATIC
	#define PACKED 
#else
	#define WORD_PACK_VALUE	0
	//#define STATIC			static		// zurong modify
	#define	STATIC
    
	//#define PACKED			__attribute__((__packed__))
    #ifdef __ADS_COMPILER__   /*jandy add for dual compiler */
	    #define PACKED 			__packed
    #else
        #ifdef __CS_COMPILER__
        #define PACKED			__attribute__((__packed__))
        #else 
        #define PACKED 			__packed
        #endif
    #endif    
    
#endif

/***************************************************************/
/*                                                              */
/* Convienence defines                                          */
/*                                                              */
#ifndef max
#define max(a,b)       (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)       (((a) < (b)) ? (a) : (b))
#endif

/*                                                              */
/* Common byte, 16 bit and 32 bit types                         */
/*                                                              */
#if WITHFAT32 == 1
	#define CLUSTER INT32U
#else
	#define CLUSTER INT16U
#endif


#endif		// __PORTAB_H__

