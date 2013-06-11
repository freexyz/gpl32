/*****************************************************************************
 *               Copyright Generalplus Corp. All Rights Reserved.      
 *                                                                     
 * FileName:       gplib_graphic.h
 * Author:         Lichuanyue  
 * Description:    Created
 * Version:        1.0 
 * Function List:  
 *                 Null
 * History:        
 *                 1>. 2008/6/15 Created
 *****************************************************************************/
#ifndef __GPLIB_GRAPHIC_H__
#define __GPLIB_GRAPHIC_H__

#include "gplib.h"

//extern void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
extern INT32U GraphicInitDrv(GraphicBitmap *bitmap);
//extern void GraphicFinishDrv(GraphicBitmap *bitmap);//,GraphicCoordinates *coordinates);
//extern INT32U GraphicGetFontHeight(INT16U character_code,INT8U language,INT8U font_type);
//extern INT32U GraphicGetFontWidth(INT16U character_code,INT8U language,INT8U font_type);

extern void GraphicSetBitmapBackgroundColor(GraphicBitmap *bitmap,INT16U color);
extern void GraphicSetBitmapFrontColor(GraphicBitmap *bitmap,INT16U color);

extern void GraphicPutLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
extern void GraphicPutRectangle(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);
extern void GraphicPutBiasLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);

extern void GraphicPutCircleCaculate(GraphicBitmap *bitmap,INT32U x0,INT32U y0,INT32U r);
#endif//__GPLIB_GRAPHIC_H__

