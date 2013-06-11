#ifndef __NVPACK_H__
#define __NVPACK_H__
/*
* Purpose: NV PACKER access
*
* Author: Dominant
*
* Date: 2008/04/23
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*         1 2009/04/22 Created by dominant.  v1.00
*/
#include "storages.h"


extern INT16U nvpack_init(INT8U* nvpack_upper_name);
extern INT16S nvpack_uninit(void);
extern INT16U nvp_fopen(INT8U* inpack_upper_name);
extern INT32S nvp_fread(INT16U nvp_infd, INT32U pbuf, INT32U byte_size);
extern INT16S nvp_fclose(INT16U nvp_infd);
extern INT32U nvp_lseek(INT16U nvp_infd, INT32S foffset);
extern INT32U nvp_fsize_get(INT16U nvp_infd);


#endif  //__NVPACK_H__

