#ifndef __GPLIB_MM_GPLUS_H__
#define __GPLIB_MM_GPLUS_H__


#include "gplib.h"

extern void gp_mm_init(INT32U free_memory_start, INT32U free_memory_end);
extern void * gp_malloc_align(INT32U size, INT32U align);		// SDRAM allocation. Align value must be power of 2, eg: 2, 4, 8, 16, 32...
extern void * gp_malloc(INT32U size);							// SDRAM allocation
extern void * gp_iram_malloc_align(INT32U size, INT32U align);	// IRAM allocation. Align value must be power of 2, eg: 2, 4, 8, 16, 32...
extern void * gp_iram_malloc(INT32U size);						// IRAM allocation
extern void gp_free(void *ptr);									// Both SDRAM and IRAM can be freed by this function


#endif 		// __GPLIB_MM_GPLUS_H__