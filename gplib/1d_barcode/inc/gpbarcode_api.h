#ifndef __GPBARCODE_API_H__
#define __GPBARCODE_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "define.h"

typedef enum {
	GPBARCODE_NONE		= 0,	
	GPBARCODE_UPC		= 8,
	GPBARCODE_ISBN	    = 10,
	GPBARCODE_EAN		= 13,
	GPBARCODE_CODE128	= 128  /* Code 128 */
} GPBARCODE_DEC_ID;


const char *gpbarcode_get_version(void);

int gpbarcode_dec_get_mem_size(void);

void gpbarcode_dec_init(void *workmem);

int gpbarcode_dec(void *workmem, gpImage *img);

GPBARCODE_DEC_ID gpbarcode_get_decoder_id(void *workmem, int idx);
unsigned char *gpbarcode_get_data(void *workmem, int idx);
int gpbarcode_get_datalen(void *workmem, int idx);
int gpbarcode_get_quality(void *workmem, int idx);

void gpbarcode_dec_set_step(void *workmem, int xstep, int ystep);

#ifdef __cplusplus
}
#endif
#endif

