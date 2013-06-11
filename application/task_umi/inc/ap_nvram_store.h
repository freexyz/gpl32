#ifndef __AP_NVRAM_STORE_H__
#define __AP_NVRAM_STORE_H__
#include "turnkey_umi_task.h"

extern INT32U nvram_read(void* global_struct, INT32U size);
extern INT32U nvram_store(void* global_struct, INT32U size);


#endif