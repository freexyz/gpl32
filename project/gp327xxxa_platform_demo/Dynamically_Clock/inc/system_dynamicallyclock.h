#include "application.h"

//Module status polling timeout counter
#define polling_timeout           3
//SDRAM Refresh Timing
#define Common_Refresh_Period     15600//16,64,128Mb:15.6usx1000=15600,256,512Mb/1Gb:7.8usx1000=7800
//define SDRAM Timing here 
#define SDRAM_tRC        68 
#define SDRAM_tRAS       45 
#define SDRAM_tRP        20 
#define SDRAM_tRCD       20 
//define Nand Timing here 
#define Nand_tRP         15 
#define Nand_tH          10 
#define Nand_tWP         15 
typedef enum
{
   CLK_48MHZ=48,
   CLK_63MHZ=63,
   CLK_78MHZ=78,
   CLK_96MHZ=96,
   CLK_MAX
} CLK_ENUM;
extern INT32S system_clock_set(CLK_ENUM target_clk);
extern void module_pollingstatus(void);
extern void system_module_init(void);
extern INT32S sdc_set_clock(INT32U speed);         //Speed unit:Hz.


