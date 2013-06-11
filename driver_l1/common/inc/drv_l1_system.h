#ifndef __drv_l1_SYSTEM_H__
#define __drv_l1_SYSTEM_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define   GPIO_CLK_SW  		       		0x00000004
#define   TIMERS_CLK_SW        			0x00000010
#define   GTE_CLK_SW           		 	0x00000020
#define   DAC_CLK_SW           		    0x00000040
#define   UART_EFUSE_CLK_SW    			0x00000080
#define   MP3_CLK_SW           			0x00000100
#define   SPI0_SPI1_CLK_SW     		 	0x00000200
#define   ADC_CLK_SW       		   		0x00000400
#define   PPU_LINEBASE_CLK_SW	   		0x00000800
#define   SPU_CLK_SW       		        0x00001000
#define   TFT_CLK_SW          		 	0x00002000
#define   SDC_CLK_SW       		     	0x00008000

#define   MS_CLK_SW        		   		0x00000001
#define   UHOST_CLK_SW     		        0x00000002
#define   UDEVICE_CLK_SW   	  		    0x00000004
#define   STN_CLK_SW   	    		    0x00000008
#define   TV_CLK_SW            			0x00000020
#define   PPU_FRAMEBASE_CLK_SW 			0x00000040
#define   CFC_CLK_SW               		0x00000100
#define   KEYSCAN_TOUCH_SENSOR_CLK_SW  	0x00000200
#define   JPEG_CLK_SW  		            0x00000400
#define   DEFLICKER_CLK_SW 				0x00000800
#define   SCALER_CLK_SW		            0x00001000
#define   NAND_CLK_SW		            0x00002000

#ifndef _SYS_SDRAM_DEF_
#define _SYS_SDRAM_DEF_

#ifndef SDRAM_SIZE
#define SDRAM_SIZE SD_64Mb
#endif

extern INT32U  MCLK;
extern INT32U  MHZ;

typedef enum {
    CL_2=2,
    CL_3=3    
} SDRAM_CL;

typedef enum {
    SD_24MHZ=24,
    SD_48MHZ=48,
    SD_96MHZ=96
} SDRAM_CLK_MHZ;

typedef enum {
    SDRAM_OUT_DLY_LV0=0, 
    SDRAM_OUT_DLY_LV1 ,
    SDRAM_OUT_DLY_LV2 ,
    SDRAM_OUT_DLY_LV3 ,
    SDRAM_OUT_DLY_LV4 ,
    SDRAM_OUT_DLY_LV5 ,
    SDRAM_OUT_DLY_LV6 ,
    SDRAM_OUT_DLY_LV7 ,
    SDRAM_OUT_DLY_LV8 ,
    SDRAM_OUT_DLY_LV9 ,
    SDRAM_OUT_DLY_LV10,
    SDRAM_OUT_DLY_LV11,
    SDRAM_OUT_DLY_LV12,
    SDRAM_OUT_DLY_LV13,
    SDRAM_OUT_DLY_LV14,
    SDRAM_OUT_DLY_LV15        
} SD_CLK_OUT_DELAY_CELL;

typedef enum {
    SDRAM_IN_DLY_LV0=0, 
    SDRAM_IN_DLY_LV1 ,
    SDRAM_IN_DLY_LV2 ,
    SDRAM_IN_DLY_LV3 ,
    SDRAM_IN_DLY_LV4 ,
    SDRAM_IN_DLY_LV5 ,
    SDRAM_IN_DLY_LV6 ,
    SDRAM_IN_DLY_LV7 ,
    SDRAM_IN_DLY_DISABLE
} SD_CLK_IN_DELAY_CELL;

typedef enum {
    SD_CLK_PHASE_0=0,
    SD_CLK_PHASE_180    
} SD_CLK_PHASE;

typedef enum {
    SD_DISABLE=0,
    SD_ENABLE    
} SD_SWITCH;

typedef enum {
    SD_BUS_16bit=0,
    SD_BUS_32bit=1    
} SD_BUS_WIDTH;


typedef enum {
    SD_BANK_1M=0,
    SD_BANK_4M    
} SD_BANK_BOUND;

typedef enum {
    SD_16Mb=0x0,
    SD_2MB=SD_16Mb,
    SD_64Mb=0x1,
    SD_8MB=SD_64Mb,
    SD_128Mb=0x2,
    SD_16MB=SD_128Mb,
    SD_256Mb=0x3,
    SD_32MB=SD_256Mb,
    SD_512Mb=0x4,
    SD_64MB=SD_512Mb,
    SD_1024Mb=0x4,
    SD_128MB=SD_1024Mb
} SD_SIZE;

typedef enum {
    tREFI_3o9u=39,
    tREFI_7o8u=78,    /*suggest: when SDRAM size >= 256Mb*/
    tREFI_15o6u=156,  /*suggest: when SDRAM size < 256Mb*/
    tREFI_31o2u=312
} tREFI_ENUM;

#endif //_SYS_SDRAM_DEF_

extern INT32S sys_sdram_init(SD_CLK_OUT_DELAY_CELL clk_out_dly_cell, SD_CLK_IN_DELAY_CELL clk_in_dly_cell, SD_CLK_PHASE clk_phase, SD_BUS_WIDTH bus_mode, SD_BANK_BOUND bank_bound, SD_SIZE sd_size, SD_SWITCH disable_enable);
extern INT32S sys_sdram_enable(SD_SWITCH sd_disable_enable);
extern INT32S sys_sdram_self_refresh_thread_cycle(INT8U sref_cycle);
extern INT32S sys_sdram_CAS_latency_cycle(SDRAM_CL cl_cycle);
extern INT32S sys_sdram_wakeup_cycle(INT8U RC_cycle);
extern INT32S sys_sdram_refresh_period_ns(INT32U refresh_period_ns);
extern INT32S sys_sdram_MHZ_set(SDRAM_CLK_MHZ sd_clk);
extern INT32S sys_sdram_read_latch_at_CLK_Neg(INT8U ebable_disable);
extern void sys_sdram_input_clock_dly_enable(INT8U ebable_disable);
extern void system_sdram_driving(void);
extern void system_enter_wait_mode(void);
extern void system_enter_halt_mode(void);
extern void system_power_on_ctrl(void);
extern void system_da_ad_pll_en_set(BOOLEAN status);
extern INT32U system_rtc_counter_addr_get(void);
extern void system_rtc_counter_addr_set(INT32U rtc_cnt);
extern void sys_weak_6M_set(BOOLEAN status);
extern void sys_reg_sleep_mode_set(BOOLEAN status);
extern void sys_ir_power_handler(void);
extern void sys_ir_power_set_pwrkey(INT8U code);
extern void system_clks_disable0(INT32U CLK_MASK);
extern void system_clks_disable1(INT32U CLK_MASK);
extern void system_clks_enable0(INT32U CLK_MASK);
extern void system_clks_enable1(INT32U CLK_MASK);
extern void sys_sdram_auto_refresh_set(INT8U cycle);
extern void sys_ir_opcode_clear(void);
extern INT32U sys_ir_opcode_get(void);
extern INT32U sys_ir_power_get_pwrkey(void);
extern INT32S sdram_calibrationed_flag(void);
#endif		// __drv_l1_SYSTEM_H__
