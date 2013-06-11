#ifndef	__RBF_V3_PARSER_H__
#define	__RBF_V3_PARSER_H__


//=============================================================================
// Define
//=============================================================================
#define C_RBF_V3_FLAG0_EQUAL_PAGE_SIZE		0x0001
#define C_RBF_V3_FLAG0_MSG_IS_SPECIAL_CODE	0x0002
#define C_RBF_V3_FLAG0_FILE_TYPE			0x003C

#define C_RBF_MAX_EVENT_BIT_LEN	16	// At most 2^16 events in a blk

//=============================================================================
// External Function
//=============================================================================
extern INT32S RBFV3_Open(INT32U start_offset, INT16U minor_version);
extern INT32S RBFV3_GetStartObj(INT32U coord, INT32U *obj_offset);
extern INT32S RBFV3_GetRes(INT32U res_index, INT16U *res_type, INT32U *res_offset, INT32U *res_len);
extern INT32S RBFV3_SetCurrRcIdx(INT32U res_index);
extern INT32U RBFV3_GetCurrRcIdx(void);
extern INT32S RBFV3_GetSysEvent(INT16U id, INT32U *data);
extern BOOLEAN RBFV3_IsSupportSysEvent(void);
extern INT16U RBFV3_GetFileType(void);
extern INT32S RBFV3_GetLogicalCode(INT32U physical_code, INT32U *logical_code);

#endif
