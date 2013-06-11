#include "driver_l2.h"


void drv_l2_init(void)
{
	SD_OS_Init();
  #if (defined _DRV_L2_SYS_TIMER_EN) &&  (_DRV_L2_SYS_TIMER_EN == 1)
	sys_init_timer();
  #endif
  
  #if (defined _DRV_L2_EXT_RTC) && (_DRV_L2_EXT_RTC==DRV_L2_ON)
  	drv_l2_external_rtc_init();
  #endif
  
  #if (defined _DRV_L2_IR) && (_DRV_L2_IR==DRV_L2_ON)
	drv_l2_ir_init();
  #endif

  #if (defined _DRV_L2_ITOUCH) && (_DRV_L2_ITOUCH == DRV_L2_ON)  
	 // drv_l2_itouch_init();
  #endif

  #if (defined _DRV_L2_NAND) && (_DRV_L2_NAND == DRV_L2_ON)
	NandInfoAutoGet();
	//DrvNand_lowlevelformat();
	DrvNand_initial();
  #endif
  
}
