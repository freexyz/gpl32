#ifndef	__RBF_MGR_H__
#define	__RBF_MGR_H__

#include "application.h"
#include "game_mode.h"

//=============================================================================
// Configuration
//=============================================================================
// Supported res bin file formats.
// One or more formats should be enabled.
#define C_RBF_SUPPORT_RBF_V1	0
#define C_RBF_SUPPORT_RBF_V2	0
#define C_RBF_SUPPORT_RBF_V3	1

//=============================================================================
// Define
//=============================================================================
#define C_RBF_RC_SUCCESS					0
#define C_RBF_RC_FAIL						0xFFFF
#define C_RBF_RC_RES_INDEX_OUT_OF_RANGE		0xFFFE
#define C_RBF_RC_RES_TYPE_NOT_FOUND			0xFFFD
#define C_RBF_RC_OPEN_FILE_FAIL				0xFFFC
#define C_RBF_RC_READ_FILE_FAIL				0xFFFB
#define C_RBF_RC_SEEK_FILE_FAIL				0xFFFA
#define C_RBF_RC_INVALID_FILE_HANDLE		0xFFF9
#define C_RBF_RC_INVALID_SIGNATURE			0xFFF8
#define C_RBF_RC_INVALID_FORMAT_VERSION		0xFFF7
#define C_RBF_RC_INVALID_HDR_SIZE			0xFFF6
#define C_RBF_RC_CODE_OUT_OF_RANGE			0xFFF5
#define C_RBF_RC_SYS_EVENT_UNDEFINED		0xFFF4
#define C_RBF_RC_SYS_EVENT_NOT_FOUND		0xFFF3
#define C_RBF_RC_EXT_RES_TBL_NOT_EXIST		0xFFF2
#define C_RBF_RC_EXT_RES_IDX_OUT_OF_RANGE	0xFFF1
#define C_RBF_RC_NULL_ARG					0xFFF0
#define C_RBF_RC_RES_TBL_NOT_EXIST			0xFFEF
#define C_RBF_RC_INVALID_PAGE_IDX			0xFFEE
#define C_RBF_RC_EVENT_TOO_LONG				0xFFED
#define C_RBF_RC_UNKNOWN_RBF_VERSION		0xFFEC
#define C_RBF_RC_NO_OBJ_PROG_CODE			0xFFEB
#define C_RBF_RC_UNSUPPORTED_OPERATION		0xFFEA
#define C_RBF_RC_NO_START_OBJ				0xFFE9
#define C_RBF_RC_INVALID_OBJ_OFFSET			0xFFE8
#define C_RBF_RC_COORD_NOT_IN_FILE			0xFFE7
#define C_RBF_RC_ZERO_PAGE_BLK_NUM			0xFFE6
#define C_RBF_RC_LOGICAL_CODE_NOT_SUPPROTED	0xFFE5

// 'mode' parameter of RBF_FindBinByBookCode()
#define C_RBF_FB_FIND_NEW_ONLY			0
#define C_RBF_FB_FIND_NEW_AND_CLOSE_OLD	1

// File type. Supported from v3.02.
#define C_RBF_FT_NORMAL		0
#define C_RBF_FT_P2L		1
#define C_RBF_FT_UNKNOWN	0xFFFF

// Logical code
#define C_RBF_UNKNOWN_LOGICAL_CODE	0xFFFFFFFF

// System event
#define SYS_POWER_OFF_TIME	0
#define SYS_HINT_TIME		1
#define	SYS_POWERON			2
#define	SYS_POWEROFF		3
#define	SYS_VOLUP			4
#define	SYS_VOLDOWN			5
#define	SYS_BOOK_CHANGE		6
#define	SYS_VOLDOWN_END		7
#define	SYS_HINT			8
#define	SYS_LOWVOLT			9
#define	SYS_BACKGROUND		10
#define	SYS_BACKGROUND_CODE	11

#define RESOURCE		0
#define	OBJECT			1

#define TWO_INT16U_TO_INT32U(lo, hi)	(((INT32U)(lo)) | (((INT32U)(hi))<<16))

//=============================================================================
// Typedef
//=============================================================================
typedef struct
{
	INT16U type;
	INT32U offset;
	INT32U len;
} RBF_Res_T;

//=============================================================================
// External Function
//=============================================================================

// Not implemented
extern INT32S RBF_GetSysEvent(INT16U id, INT16U *type, INT32U *data);
extern INT32S RBF_SetCurrRcIdx(INT32U res_index);
extern INT32S RBF_SetCurrGpidCode(INT32U code);
extern INT32U RBF_GetBackgroundCode(void);
extern INT32U RBF_GetCurrRcIdx(void);

// Functions supported by all RBF versions
extern INT32S RBF_FindFirstBin(INT8U fs_type);
extern INT32S RBF_Open(INT32U start_offset);
extern INT32S RBF_Close(void);
extern INT16S RBF_GetVersion(void);
extern INT32S RBF_Read(INT32S offset, void* buffer, INT32U len);
extern INT32S RBF_ReadFromCurrOffset(void *buffer, INT32U len);
extern INT32S RBF_SeekToOffset(INT32S offset);
extern INT32S RBF_GetCurrOffset(void);
extern INT16S RBF_GetFd(void);
extern INT32S RBF_SetFd(INT16S fd, INT16S fd_gm);
extern INT32U RBF_GetSpCodeSize(void);
extern BOOLEAN RBF_IsBookExist(void);

// Functions supported only by some RBF versions
extern INT32S RBF_GetRes(INT16U code, INT16U res_idx, RBF_Res_T *res);	// V1
extern INT32S RBF_GetExtRes(INT16U res_idx, RBF_Res_T *res);			// V1
extern INT32U RBF_GetGpidStartCode(void);								// V1
extern INT32U RBF_GetGpidEndCode(void);									// V1
extern INT32S RBF_FindBinByBookCode(INT32U book_code, INT16U mode);		// V2, V3
extern INT32S RBF_ReadGm(INT32S offset, void* buffer, INT32U len);		// V2, V3
extern INT32S RBF_ReadFromCurrOffset_Gm(void *buffer, INT32U len);		// V2, V3
extern INT32S RBF_SeekToOffset_Gm(INT32S offset);						// V2, V3
extern INT32S RBF_GetCurrOffset_Gm(void);								// V2, V3
extern INT32S RBF_GetStartObj(INT32U coord, INT32U *obj_offset);		// V2, V3
extern INT32S RBF_GetStartObj_DontMoveFd(INT32U coord, INT32U *obj_offset);	// V2, V3
extern BOOLEAN RBF_IsSupportSysEvent(void);
extern INT16U RBF_GetFileType(void);											// V3
extern INT32S RBF_GetLogicalCode(INT32U physical_code, INT32U *logical_code);	// V3

#endif