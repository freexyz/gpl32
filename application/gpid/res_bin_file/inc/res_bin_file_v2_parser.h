#ifndef	__RBF_V2_PARSER_H__
#define	__RBF_V2_PARSER_H__


//=============================================================================
// Define
//=============================================================================

//=============================================================================
// External Function
//=============================================================================
extern INT32S RBFV2_Open(INT32U start_offset);
extern INT32S RBFV2_GetStartObj(INT32U coord, INT32U *obj_offset);
extern INT32S RBFV2_GetSysEvent(INT16U id, INT16U *type, INT32U *data);
extern INT32S RBFV2_SetCurrRcIdx(INT32U res_index);
extern INT32U RBFV2_GetCurrRcIdx(void);
extern INT32S RBFV2_GetRes(INT32U code, INT32U res_index, INT16U *res_type, INT32U *res_offset, INT32U *res_len);
extern INT32U RBFV2_GetSpCodeSize(void);

#endif
