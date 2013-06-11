#ifndef __drv_l1_TOOLS_H__
#define __drv_l1_TOOLS_H__

#include "driver_l1.h"

/* MCLK/24 = loop times per second */
#define TOOL_TRUE  1
#define TOOL_FALSE 0

#define DRV_WriteReg32(addr,data)       ((*(volatile INT32U *)(addr)) = (INT32U)data)
#define DRV_Reg32(addr)                 (*(volatile INT32U *)(addr))
#define BASWE_CLK                       96000000
#define MCLK_RATE                       MCLK/BASWE_CLK      


#define DRV_LOOP_DELAY(loops)  \
{\
   INT32U loopcnt;\
   loopcnt = (INT32U)loops;\
   while (loopcnt--) {}\
}

#define DRV_USEC_WAIT(USEC)  \
{\
   INT32U usec = USEC*79;\
   while (usec>10) {\
  	usec-= 10 ;\
  }\
}

#define DRV_MSEC_WAIT(MSEC)  \
{\
  INT32U msec = MSEC*8044;\
  while (msec) {\
  	msec-= 1 ;\
  }\
}


#define GPIO_PORTA_PULL_HIGH_PORT(port_num)  \
{ \
    DRV_Reg32(IOA_DATA_ADDR) |= (1 << port_num); \
} 

#define GPIO_PORTB_PULL_HIGH_PORT(port_num)  \
{ \
    DRV_Reg32(IOB_DATA_ADDR) |= (1 << port_num); \
} 

#define GPIO_PORTC_PULL_HIGH_PORT(port_num)  \
{ \
    DRV_Reg32(IOC_DATA_ADDR) |= (1 << port_num); \
} 

#define GPIO_PORTD_PULL_HIGH_PORT(port_num)  \
{ \
    DRV_Reg32(IOD_DATA_ADDR) |= (1 << port_num); \
} 

#define GPIO_PORTA_PULL_LOW_PORT(port_num)  \
{ \
    DRV_Reg32(IOA_DATA_ADDR) &= ~(1 << port_num); \
} 

#define GPIO_PORTB_PULL_LOW_PORT(port_num)  \
{ \
    DRV_Reg32(IOB_DATA_ADDR) &= ~(1 << port_num); \
} 

#define GPIO_PORTC_PULL_LOW_PORT(port_num)  \
{ \
    DRV_Reg32(IOC_DATA_ADDR) &= ~(1 << port_num); \
} 

#define GPIO_PORTD_PULL_LOW_PORT(port_num)  \
{ \
    DRV_Reg32(IOD_DATA_ADDR) &= ~(1 << port_num); \
} 

extern INT32S mem_transfer_dma(INT32U src, INT32U dest, INT32U len);


#endif		// __drv_l1_TOOLS_H__