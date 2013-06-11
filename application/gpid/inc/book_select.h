#ifndef	__BOOK_SELECT_H__
#define	__BOOK_SELECT_H__

//#include "GPL16_TypeDef.h"
#include "application.h"

//=============================================================================
// Define
//============================================================================
#define C_BS_RC_SUCCESS					0
#define C_BS_RC_INVALID_RANGE			0xFFFFFFFF
#define C_BS_RC_INVALID_GPID_TYPE		0xFFFFFFFE
#define C_BS_RC_BOOK_SELECT_DISABLED	0xFFFFFFFD
#define C_BS_RC_INVALID_BOOK_CODE		0xFFFFFFFC

#define C_BS_GPID_TYPE_UNKNOWN	0
#define C_BS_GPID_TYPE_4U		1
#define C_BS_GPID_TYPE_4G		2

#define C_BS_CODE_TYPE_UNKNOWN	0
#define C_BS_CODE_TYPE_BOOK		1
#define C_BS_CODE_TYPE_CONTENT	2

#define C_BS_INVALID_BOOK_CODE	0xFFFFFFFF

//=============================================================================
// Public functions
//=============================================================================
extern INT32S BS_InitByGpidType(INT16U gpid_type);
extern void BS_EnableBookSelect(BOOLEAN en);
extern BOOLEAN BS_IsBookSelectEnabled(void);
extern INT32S BS_SetBookCodeRange(INT32U min, INT32U max);
extern INT32S BS_SetContentCodeRange(INT32U min, INT32U max);
extern INT16U BS_GetCodeType(INT32U code);
extern INT32S BS_SetCurrBookCode(INT32U code);
extern INT32U BS_GetCurrBookCode(void);

#endif