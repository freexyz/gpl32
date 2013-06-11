#ifndef __ARM_H__
#define __ARM_H__


#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define INT_FIQ     		0x40            // FIQ disable bit
#define INT_IRQ     		0x80            // IRQ disable bit
#define INT_MASK    		0xC0			// Interrupt disable mask (Both FIR and IRQ)
#define MODE_USR   			0x10
#define MODE_FIQ  			0x11
#define MODE_IRQ  			0x12
#define MODE_SVC  			0x13
#define MODE_ABT			0x17
#define MODE_UND  			0x1B
#define MODE_SYS   			0x1F
#define MODE_MASK   		0x1F
#define INSTRUCTION_THUMB  	0x20

#define	EXCEPTION_UNDEF		0x1
#define	EXCEPTION_SWI		0x2
#define	EXCEPTION_PABORT	0x3
#define	EXCEPTION_DABORT	0x4
#define	EXCEPTION_IRQ		0x5
#define	EXCEPTION_FIQ		0x6

// extern function
extern void exception_table_init(void);
extern INT32S register_exception_table(INT32U exception, INT32U handler);
#if _OPERATING_SYSTEM == _OS_NONE
extern void	irq_enable(void);
extern void irq_disable(void);
extern void fiq_enable(void);
extern void fiq_disable(void);
#endif


#endif		// __ARM_H__