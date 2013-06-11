#ifndef	__AE_PROC_h__
#define	__AE_PROC_h__

#define C_AE_LumNormal		0
#define C_AE_Processing		-1
#define C_AE_OverFlow		-2
#define C_AE_LumTooDark		-3

void AE_Initial(void);
int AE_Proc(INT16U *Sourceaddr,INT16U *Buf);
#endif
