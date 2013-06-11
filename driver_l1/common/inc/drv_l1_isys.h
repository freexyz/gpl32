#include "driver_l1.h"
#include "drv_l1_sfr.h"

#ifndef _SDRAM_DRV
#define _SDRAM_DRV
typedef enum {
    SDRAM_DRV_4mA=0x0000,
    SDRAM_DRV_8mA=0x5555,
    SDRAM_DRV_12mA=0xAAAA,
    SDRAM_DRV_16mA=0xFFFF
} SDRAM_DRV;
#endif

#ifndef _NEG_SAMPLE
#define _NEG_SAMPLE
typedef enum {
    NEG_OFF=0x0,
    NEG_ON=0x2
} NEG_SAMPLE;
#endif

#ifndef PLL_27MHz
#define PLL_27MHz	27000000
#endif

#ifndef PLL_24MHz
#define PLL_24MHz	24000000
#endif

#ifndef PLL_SRC_CLK
#define PLL_SRC_CLK	PLL_27MHz
#endif

extern INT32S sys_pll_set(INT32U PLL_CLK, INT8U sdram_out_dly, INT8U sdram_in_dly, SD_SIZE sdram_size, NEG_SAMPLE neg, SDRAM_DRV sdram_driving);
extern INT32S sys_clk_set(INT32U PLL_CLK);