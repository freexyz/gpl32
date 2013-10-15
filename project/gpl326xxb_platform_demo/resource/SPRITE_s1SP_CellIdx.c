#include "sprite.h"

const INT16U ALIGN4 _Sprite001_IMG0000_CellIdx[]={
	   1,	SP_ROTATE0,	   0,	SP_ZOOM0,	   0,	SP_PBANK0,	SPBLEND_DISABLE,	SP_DEPTH1,	SP_PALETTE3,	SP_VSIZE64,	SP_HSIZE64,	SPVFLIP_DISABLE,	SPHFLIP_DISABLE,	SP_COLOR256,	SP_MOSAIC0,	SPBLEND_LEVEL0,	0,	SpriteNoGroupID,	SPLarge_DISABLE,	SPInterpolation_DISABLE,
	0xffff, 0xffff };
const SPRITE ALIGN4 Sprite001_SP[]={ 
 { 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, (const INT32U *)_Sprite001_IMG0000_CellIdx } 
};
const SPRITE* s1SpritesPool[]={ 
	Sprite001_SP
};
