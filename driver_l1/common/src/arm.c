#include "arm.h"


extern INT32U pHandler_Undef;
extern INT32U pHandler_SWI;
extern INT32U pHandler_PAbort;
extern INT32U pHandler_DAbort;
extern INT32U pHandler_IRQ;
extern INT32U pHandler_FIQ;


void debug_undefined(INT32U address)
{
	address = address;
  #if (defined BOARD_TYPE) && (BOARD_TYPE >= BOARD_TURNKEY_BASE)
  	R_SYSTEM_WATCHDOG_CTRL = 0x8002;		// Enable watchdog to reset system after 0.5 second
  #else
	while (1);
  #endif
}

void debug_swi(INT32U address)
{
	address = address;
  #if (defined BOARD_TYPE) && (BOARD_TYPE >= BOARD_TURNKEY_BASE)
  	R_SYSTEM_WATCHDOG_CTRL = 0x8002;		// Enable watchdog to reset system after 0.5 second
  #else
	while (1);
  #endif
}

void debug_prefetch_abort(INT32U address)
{
  #if (defined BOARD_TYPE) && (BOARD_TYPE >= BOARD_TURNKEY_BASE)
  	R_SYSTEM_WATCHDOG_CTRL = 0x8002;		// Enable watchdog to reset system after 0.5 second
  #else
	while (1);
  #endif
}

void debug_data_abort(INT32U address)
{
  #if (defined BOARD_TYPE) && (BOARD_TYPE >= BOARD_TURNKEY_BASE)
  	R_SYSTEM_WATCHDOG_CTRL = 0x8002;		// Enable watchdog to reset system after 0.5 second
  #else
	while (1);
  #endif
}

void debug_irq(void)
{
	while (1);
}

void debug_fiq(void)
{
	while (1);
}

void exception_table_init(void)
{
	pHandler_Undef = (INT32U) debug_undefined;
	pHandler_SWI = (INT32U) debug_swi;
	pHandler_PAbort = (INT32U) debug_prefetch_abort;
	pHandler_DAbort = (INT32U) debug_data_abort;
	pHandler_IRQ = (INT32U) debug_irq;
	pHandler_FIQ = (INT32U) debug_fiq;
}

INT32S register_exception_table(INT32U exception, INT32U handler)
{
	switch (exception) {
	case EXCEPTION_UNDEF:
		pHandler_Undef = (INT32U) handler;
		break;
	case EXCEPTION_SWI:
		pHandler_SWI = (INT32U) handler;
		break;
	case EXCEPTION_PABORT:
		pHandler_PAbort = (INT32U) handler;
		break;
	case EXCEPTION_DABORT:
		pHandler_DAbort = (INT32U) handler;
		break;
	case EXCEPTION_IRQ:
		pHandler_IRQ = (INT32U) handler;
		break;
	case EXCEPTION_FIQ:
		pHandler_FIQ = (INT32U) handler;
		break;
	default:
		return STATUS_FAIL;
	}
	return STATUS_OK;
}

#if _OPERATING_SYSTEM == _OS_NONE      // Hard protect should not be used when OS is available
void irq_enable(void)
{
    ASM(MRS R0, CPSR);
    ASM(BIC R0, R0, #INT_IRQ);
    ASM(MSR CPSR_c, R0);
}

void irq_disable(void)
{
    ASM(MRS R0, CPSR);
    ASM(ORR R0, R0, #INT_IRQ);
    ASM(MSR CPSR_c, R0);
}

void fiq_enable(void)
{
    ASM(MRS R0, CPSR);
    ASM(BIC R0, R0, #INT_FIQ);
    ASM(MSR CPSR_c, R0);
}

void fiq_disable(void)
{
    ASM(MRS R0, CPSR);
    ASM(ORR R0, R0, #INT_FIQ);
    ASM(MSR CPSR_c, R0);
}
#endif