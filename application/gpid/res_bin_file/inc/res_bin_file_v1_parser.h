#ifndef	__RBF_V1_PARSER_H__
#define	__RBF_V1_PARSER_H__

//=============================================================================
// Define
//=============================================================================

//=============================================================================
// External Function
//=============================================================================
extern int RBFV1_Open(unsigned long start_offset);
extern int RBFV1_GetRes(unsigned short code, unsigned short res_idx, RBF_Res_T *res);
extern int RBFV1_GetExtRes(unsigned short res_idx, RBF_Res_T *res);
extern unsigned long RBFV1_GetGpidStartCode(void);
extern unsigned long RBFV1_GetGpidEndCode(void);
extern INT32S RBFV1_SetCurrRcIdx(INT32U res_index);
extern INT32U RBFV1_GetCurrRcIdx(void);
#endif
