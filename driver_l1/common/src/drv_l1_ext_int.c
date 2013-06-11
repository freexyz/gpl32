/* 
* Purpose: External interrupt driver/interface
*
* Author: Allen Lin
*
* Date: 2008/05/28
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/

#include "drv_l1_ext_int.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_EXT_INT) && (_DRV_L1_EXT_INT == 1)           //
//================================================================//

void (*exta_callback)(void);
void (*extb_callback)(void);

void extab_int_isr(void);

void ext_int_init(void)
{
	extab_int_clr(EXTA);
	extab_int_clr(EXTB);
	R_INT_KECON &= ~0xFF;
	extab_user_isr_clr(EXTA);
	extab_user_isr_clr(EXTB);
	vic_irq_register(VIC_EXT_AB,extab_int_isr);
	vic_irq_enable(VIC_EXT_AB);	
}

void extab_int_clr(INT8U ext_src)
{
	if (ext_src == EXTA) {
		R_INT_KECON |= EXTA_INT;
	}
	else {
		R_INT_KECON |= EXTB_INT;
	} 
}

void extab_edge_set(INT8U ext_src, INT8U edge_type)
{
	if (ext_src == EXTA) {
		R_INT_KECON &= ~EXTA_POL;
		R_INT_KECON |= (edge_type << 4);
	}
	else {
		R_INT_KECON &= ~EXTB_POL;
		R_INT_KECON |= (edge_type << 5);
	} 
}

void extab_enable_set(INT8U ext_src, BOOLEAN status)
{
	if (ext_src == EXTA) {
		if (status == TRUE) {
			R_INT_KECON |= EXTA_IEN;
		}
		else {
			R_INT_KECON &= ~EXTA_IEN;
		}
	}
	else {
		if (status == TRUE) {
			R_INT_KECON |= EXTB_IEN;
		}
		else {
			R_INT_KECON &= ~EXTB_IEN;
		}
	}	
}

void extab_user_isr_set(INT8U ext_src,void (*user_isr)(void))
{
	if (user_isr == 0) {
		return;
	}
	if (ext_src == EXTA) {
		exta_callback = user_isr;
	}
	else {
		extb_callback = user_isr;
	}
}

void extab_user_isr_clr(INT8U ext_src)
{
	if (ext_src == EXTA) {
		exta_callback = 0;
	}
	else {
		extb_callback = 0;
	}
}

void extab_int_isr(void)
{
	INT32U status;
	status = R_INT_KECON;
	
	if (status & EXTA_INT) {
		R_INT_KECON |= EXTA_INT;
		if (exta_callback != 0) {
			(*exta_callback)();
		}
	}
	
	if (status & EXTB_INT) {
		R_INT_KECON |= EXTB_INT;
		if (extb_callback != 0) {
			(*extb_callback)();
		}
	}
}


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_EXT_INT) && (_DRV_L1_EXT_INT == 1)      //
//================================================================//