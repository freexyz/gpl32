#ifndef __drv_l1_EXT_INT_H__
#define __drv_l1_EXT_INT_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define EXTB_INT 0x80
#define EXTA_INT 0x40
#define EXTB_POL 0x20
#define EXTA_POL 0x10
#define EXTB_IEN 0x02

#if BOARD_TYPE == BOARD_TK_32600_V1_0
#define EXTA_IEN 0x04
#else
#define EXTA_IEN 0x01
#endif

extern void ext_int_init(void);
extern void extab_int_clr(INT8U ext_src);
extern void extab_edge_set(INT8U ext_src, INT8U edge_type);
extern void extab_enable_set(INT8U ext_src, BOOLEAN status);
extern void extab_user_isr_set(INT8U ext_src,void (*user_isr)(void));
extern void extab_user_isr_clr(INT8U ext_src);

#endif		// __drv_l1_EXT_INT_H__