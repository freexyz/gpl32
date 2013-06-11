/* Created by AllenLin */

#include "gplib.h"

/* CRC Poly */
#define CRC32_POLY     0xedb88320

INT32U    CRC32_tbl[256] ;
static    void CRC32tbl_init(void);

void CRC_cal(INT8U *pucBuf, INT32U len, INT8U *CRC)
{
    INT32U     i_CRC, value ;
    INT32U     i;

    if (!CRC32_tbl[1])
        CRC32tbl_init() ;
    
    i_CRC = 0xffffffff ;
    for (i = 0; i < len; i++) {
        i_CRC = (i_CRC >> 8) ^ CRC32_tbl[(i_CRC ^ pucBuf[i]) & 0xff] ;
    }

    value = ~i_CRC ;

    CRC[0] = (value & 0x000000ff) >> 0;
    CRC[1] = (value & 0x0000ff00) >> 8;
    CRC[2] = (value & 0x00ff0000) >> 16;
    CRC[3] = (value & 0xff000000) >> 24;

}

void CRC32tbl_init(void)
{
    INT32U    i,j;
    INT32U    c ;

    for (i = 0; i < 256; ++i) {
        c = i ;
        for (j = 0; j < 8; j++) { 
            c = (c & 1) ? (c >> 1) ^ CRC32_POLY : (c >> 1) ;
        }
        CRC32_tbl[i] = c ;
    }
}