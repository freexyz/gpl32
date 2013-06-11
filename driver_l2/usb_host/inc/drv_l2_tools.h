#ifndef __drv_l2_TOOLS_H__
#define __drv_l2_TOOLS_H__

#include "driver_l2.h"

/* MCLK/24 = loop times per second */
#define drv_l2_TOOL_TRUE  1
#define drv_l2_TOOL_FALSE 0

#define BASWE_CLK                       48000000
#define MCLK_RATE                       MCLK/BASWE_CLK      


#define drv_l2_LOOP_DELAY(loops)  \
{\
   INT32U loopcnt;\
   while (loopcnt--) {}\
}

#define drv_l2_USEC_WAIT(USEC)  \
{\
   INT32U usec = USEC*79;\
   while (usec>10) {\
  	usec-= 10 ;\
  }\
}

#define drv_l2_MSEC_WAIT(MSEC)  \
{\
  INT32U msec = MSEC*8044;\
  while (msec) {\
  	msec-= 1 ;\
  }\
}

#endif		// __drv_l2_TOOLS_H__