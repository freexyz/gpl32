#include "driver_l2.h"

#if ((_DRV_L2_NAND != 1) && (_DRV_L2_XDC == 1))
	OS_EVENT* 	gNand_sem = NULL;	
#endif



