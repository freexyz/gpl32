#include "drv_l1_watchdog.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_WATCHDOG) && (_DRV_L1_WATCHDOG == 1)         //
//================================================================//

/* prototype definition */
void watchdog_init(void);

INT32U lvr_ctrl(INT32U LVROFF);
INT32U watchdog_ctrl(INT32U WDGPD, INT32U WDGS, INT32U WDGEN);
INT32U watchdog_clear(void);

typedef enum
{
    LOW_VOLTAGE_RESET_ENABLE=0,
    LOW_VOLTAGE_RESET_DISABLE,    
    LVROFF_MAX
} LVROFF_enum;

typedef enum
{
    WATCHDOG_DISABLE=0,
    WATCHDOG_ENABLE,
    WDGEN_MAX
} WDGEN_enum;

typedef enum
{
    RESET_SYSTEM=0,
    RESET_CPU,    
    WDGS_MAX
} WDGS_enum;

typedef enum
{
    WATCH_PERIOD_2_SEC=0,
    WATCH_PERIOD_1_SEC,  
    WATCH_PERIOD_0o5_SEC,
    WATCH_PERIOD_0o25_SEC,
    WATCH_PERIOD_0o125_SEC,
    WATCH_PERIOD_62o5_SEC,
    WDGPD_MAX
} WDGPD_enum;
    


// Provide your WatchDog driver and ISR here
void watchdog_init(void)
{
    DRV_Reg32(0xd000002c) = 0xA005;  /* clear watchdog flag */
    DRV_Reg32(0xD0000018) = 0x0010;  /* reset watchdog timeout flag */
}

INT32U lvr_ctrl(INT32U LVROFF)
{
    DRV_Reg32(LVR_CTRL_ADDR) = (LVROFF<<1);
    return WATCHDOG_OK;
}

INT32U watchdog_ctrl(INT32U WDGPD, INT32U WDGS, INT32U WDGEN)
{
    INT32U trace;
    trace = (WDGPD | WDGS<<14 | WDGEN<<15);
    DRV_Reg32(WATCHDOG_CTRL_ADDR) = (WDGPD | WDGS<<14 | WDGEN<<15);
    return WATCHDOG_OK;
}

#if 0
INT32U watchdog_reset_cpu(void)
{
    DRV_Reg32(WATCHDOG_CLEAR_ADDR) = 0x0000FFFF;
    return WATCHDOG_OK;
}
#endif

INT32U watchdog_clear(void)
{
   if ( (DRV_Reg32(WATCHDOG_CTRL_ADDR) & WATCHDOG_ENABLE_MASK) == WATCHDOG_ENABLE_MASK) /*When watch dog enabled*/
   {
    DRV_Reg32(WATCHDOG_CLEAR_ADDR) = (WATCHDOG_CLEAR_VALUE & 0x0000FFFF);
    return WATCHDOG_OK;
    } 

    return WATCHDOG_FAIL;
}


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_WATCHDOG) && (_DRV_L1_WATCHDOG == 1)    //
//================================================================//