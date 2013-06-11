//***************************************************************************************** 
// Topic: Book selection flow
//*****************************************************************************************
#include "book_select.h"

//=============================================================================
// Define
//=============================================================================
// GPID4U
//	content code: 0~9M-1
//	book code   : 9M~15M-1
#define C_MIN_CONTENT_CODE_GPID4U	((INT32U)(0UL))
#define C_MAX_CONTENT_CODE_GPID4U	((INT32U)(9UL*1024UL*1024UL-1UL))
#define C_MIN_BOOK_CODE_GPID4U		((INT32U)(9UL*1024UL*1024UL))
#define C_MAX_BOOK_CODE_GPID4U		((INT32U)(15UL*1024UL*1024UL-1UL))

// GPID4G
//	content code: 15M~16M-1
#define C_MIN_CONTENT_CODE_GPID4G	((INT32U)(15UL*1024UL*1024UL))
#define C_MAX_CONTENT_CODE_GPID4G	((INT32U)(16UL*1024UL*1024UL-1))

//=============================================================================
// Static Variables
//=============================================================================
static BOOLEAN gIsBookSelectEnabled = FALSE;
static INT32U gMinBookCode;
static INT32U gMaxBookCode;
static INT32U gMinContentCode;
static INT32U gMaxContentCode;
static INT32U gCurrBookCode = C_BS_INVALID_BOOK_CODE;

//=============================================================================
// Note: 
//=============================================================================
INT32S
BS_InitByGpidType(INT16U gpid_type)
{
	INT32S ret = C_BS_RC_SUCCESS;

	switch(gpid_type)
	{
	case C_BS_GPID_TYPE_4U:
		// book selection is required for GPID4U
		BS_EnableBookSelect(TRUE);
		BS_SetContentCodeRange(C_MIN_CONTENT_CODE_GPID4U, C_MAX_CONTENT_CODE_GPID4U);
		BS_SetBookCodeRange(C_MIN_BOOK_CODE_GPID4U, C_MAX_BOOK_CODE_GPID4U);
		break;

	case C_BS_GPID_TYPE_4G:
		// book selection is optional for GPID4G
		BS_EnableBookSelect(FALSE);
		BS_SetContentCodeRange(C_MIN_CONTENT_CODE_GPID4G, C_MAX_CONTENT_CODE_GPID4G);
		break;

	default:
		ret = C_BS_RC_INVALID_GPID_TYPE;
		break;
	}

	return ret;
}

//=============================================================================
// Note: 
//=============================================================================
void
BS_EnableBookSelect(BOOLEAN en)
{
	gIsBookSelectEnabled = en;
}

//=============================================================================
// Note: 
//=============================================================================
BOOLEAN
BS_IsBookSelectEnabled(void)
{
	return gIsBookSelectEnabled;
}

//=============================================================================
// Note: 
//=============================================================================
INT32S
BS_SetBookCodeRange(INT32U min, INT32U max)
{
	if (min > max)
		return C_BS_RC_INVALID_RANGE;

	gMinBookCode = min;
	gMaxBookCode = max;
	
	return C_BS_RC_SUCCESS;
}

//=============================================================================
// Note: 
//=============================================================================
INT32S
BS_SetContentCodeRange(INT32U min, INT32U max)
{
	if (min > max)
		return C_BS_RC_INVALID_RANGE;

	gMinContentCode = min;
	gMaxContentCode = max;

	return C_BS_RC_SUCCESS;
}

//=============================================================================
// Note: 
//=============================================================================
INT16U
BS_GetCodeType(INT32U code)
{
	if (gIsBookSelectEnabled)
	{
		if ((code >= gMinBookCode) && (code <= gMaxBookCode))
			return C_BS_CODE_TYPE_BOOK;
	}

	if ((code >= gMinContentCode) && (code <= gMaxContentCode))
		return C_BS_CODE_TYPE_CONTENT;

	return C_BS_CODE_TYPE_UNKNOWN;
}

//=============================================================================
// Note: 
//=============================================================================
INT32S
BS_SetCurrBookCode(INT32U code)
{
	if (gIsBookSelectEnabled == FALSE)
		return C_BS_RC_BOOK_SELECT_DISABLED;

	if (BS_GetCodeType(code) != C_BS_CODE_TYPE_BOOK)
		return C_BS_RC_INVALID_BOOK_CODE;

	gCurrBookCode = code;

	return C_BS_RC_SUCCESS;
}

//=============================================================================
// Note: 
//=============================================================================
INT32U
BS_GetCurrBookCode(void)
{
	return gCurrBookCode;
}