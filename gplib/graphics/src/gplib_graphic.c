/*****************************************************************************
 *               Copyright Generalplus Corp. All Rights Reserved.
 *
 * FileName:       gplib_graphic.c
 * Author:         Lichuanyue
 * Description:    Created
 * Version:        1.0
 * Function List:
 *                 Null
 * History:
 *                 1>. 2008/6/15 Created
 *****************************************************************************/

 #include "gplib_graphic.h"

//#define GPLIB_GRAPHIC_DBG
#ifdef GPLIB_GRAPHIC_DBG
#define GRAPHIC_DBG_PRINT DBG_PRINT
#else
#define GRAPHIC_DBG_PRINT do{}while(0)
#endif

/*
*  Variable Declartion.
*/
typedef void (*GraphicFontEntryFunction)(INT16U ,FontTableAttribute*);
#define GraphicFontEntryNull     ((void *)0)

typedef void (*GraphicPutColorToPixel) (INT16U,INT16U,GraphicBitmap*,INT8U);
#define GraphicPutClorToPixelNull    ((void *)0)

/*
*  Function Declartion.
*/
INT32U GraphicApplyBitmapDatabuf(GraphicBitmap *bitmap);
INT32U GraphicInitDrv(GraphicBitmap *bitmap);
void GraphicFinishDrv(GraphicBitmap *bitmap);//,GraphicCoordinates *coordinates);
void GraphicPutChar(GraphicBitmap *bitmap,GraphicCoordinates *coordinates,FontTableAttribute *font);

INT32U GraphicCalculatePixelPosition(INT16U x,INT16U y,GraphicBitmap *bitmap);
void GraphicPutPixel(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelToYUYV(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelToRGBG(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo16BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo15BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo8BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo6BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo4BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo2BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);
void GraphicPutPixelTo8BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index);


void GraphicSetBitmapBackgroundColor(GraphicBitmap *bitmap,INT16U color);
void GraphicSetBitmapFrontColor(GraphicBitmap *bitmap,INT16U color);

void GraphicPutLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
void GraphicPutHorizontalLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
void GraphicPutVerticalLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
void GraphicPutBiasLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
void GraphicPutBiasLineCaculate(GraphicBitmap *bitmap,INT32U x0,INT32U y0,INT32U x1,INT32U y1);


void GraphicPutRectangle(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);
void GraphicPutRectangleNoFillColor(GraphicBitmap * bitmap, GraphicRectangleAttirbute * rectangle);
void GraphicPutRectangleFillColor(GraphicBitmap * bitmap, GraphicRectangleAttirbute * rectangle);



void GraphicOSLock(void);
void GraphicOSUnlock(void);

GraphicPutColorToPixel GraphicPutColorToPixelArray[GRAPHIC_COLOR_MAX_MODE] =
{
    GraphicPutClorToPixelNull,//GRAPHIC_COLOR_MODE_NULL
    GraphicPutPixelTo2BIT,    //GRAPHIC_2_BIT_COLOR_MODE
    GraphicPutPixelTo4BIT,    //GRAPHIC_4_BIT_COLOR_MODE
    GraphicPutPixelTo6BIT,    //GRAPHIC_6_BIT_COLOR_MODE
    GraphicPutPixelTo8BIT,    //GRAPHIC_8_BIT_COLOR_MODE
    GraphicPutPixelTo15BIT,   //GRAPHIC_15_BIT_COLOR_MODE
    GraphicPutPixelTo16BIT,   //GRAPHIC_16_BIT_COLOR_MODE
    GraphicPutPixelToRGBG,    //GRAPHIC_RGBG_COLOR_MODE
    GraphicPutPixelToYUYV,    //GRAPHIC_YUYV_COLOR_MODER
};



/*
*  Others.
*/


//-------------------------------------------------------------------
//	                                                                |
//           Graphic Flow Relative Function                         |
//																	|
//-------------------------------------------------------------------
/*****************************************************************************
 Function Name: GraphicApplyBitmapDatabuf
 Purposes     : apply working memory
 Parameter    :
  				bitmap -> working memory
 Return Value :
          		GRAPHIC_INIT_SUCESS -> init success
          		GRAPHIC_INIT_FAIL   -> init fail
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
INT32U GraphicApplyBitmapDatabuf(GraphicBitmap *bitmap)
{
	void *buf;
	INT32U i = 0;
	bitmap->bitmap_databuf = NULL;
	switch (bitmap->bitmap_color_palette)
	{
		case GRAPHIC_2_BIT_COLOR_MODE:
			break;

		case GRAPHIC_4_BIT_COLOR_MODE:
		    buf = gp_malloc_align(((bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size)/2),4);
			if(buf == NULL)
			{
			    #ifdef GPLIB_GRAPHIC_DBG
			    GRAPHIC_DBG_PRINT("Graphic apply data buffer Fail !\r\n");
			    #endif
				return GRAPHIC_INIT_FAIL;
			}
			gp_memset((INT8S *)buf, bitmap ->bitmap_background_color,(bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size)/2);
			bitmap->bitmap_databuf = (INT16U *)buf;
			break;

		case GRAPHIC_6_BIT_COLOR_MODE:
			break;

		case GRAPHIC_8_BIT_COLOR_MODE:
			buf = gp_malloc((bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size));
			if(buf == NULL)
			{
			    #ifdef GPLIB_GRAPHIC_DBG
			    GRAPHIC_DBG_PRINT("Graphic apply data buffer Fail !\r\n");
			    #endif
				return GRAPHIC_INIT_FAIL;
			}
			gp_memset((INT8S *)buf, bitmap ->bitmap_background_color,(bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size));
			bitmap->bitmap_databuf = (INT16U *)buf;
			break;

		case GRAPHIC_15_BIT_COLOR_MODE:
		case GRAPHIC_16_BIT_COLOR_MODE:
		case GRAPHIC_RGBG_COLOR_MODE:
		case GRAPHIC_YUYV_COLOR_MODER:
			buf = gp_malloc((bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size)<<1);
			if(buf == NULL)
			{
				#ifdef GPLIB_GRAPHIC_DBG
				GRAPHIC_DBG_PRINT("Graphic apply data buffer Fail !\r\n");
				#endif
				return GRAPHIC_INIT_FAIL;
			}
			for(i = 0 ;i< bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size;i++)
			{
				*((INT16U*) buf+i )=  bitmap ->bitmap_background_color;
			}
			//gp_memset((INT8S *)buf, bitmap ->bitmap_background_color,(bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size)<<1);
			bitmap->bitmap_databuf = (INT16U *)buf;
			break;

		default :
			break;
	}

	return GRAPHIC_INIT_SUCESS;
}


/*****************************************************************************
 Function Name: GraphicInitDrv
 Purposes     : Init all the nesecerry par of graphic
 Parameter    :
  				bitmap -> working memory
 Return Value :
          		GRAPHIC_INIT_SUCESS -> init success
          		GRAPHIC_INIT_FAIL   -> init fail
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
INT32U GraphicInitDrv(GraphicBitmap *bitmap)
{
	//void *buf;

	if((bitmap->bitmap_cell_x_size%8) !=0 )
	{
		bitmap->bitmap_cell_x_size = (bitmap->bitmap_cell_x_size/8+1)*8;
	}

	if((bitmap->bitmap_cell_y_size%8) !=0 )
	{
		bitmap->bitmap_cell_y_size = (bitmap->bitmap_cell_x_size/8+1)*8;
	}

	if((bitmap->bitmap_real_x_size%bitmap->bitmap_cell_x_size) != 0)
	{
		bitmap->bitmap_extend_x_size = (bitmap->bitmap_real_x_size/bitmap->bitmap_cell_x_size + 1 )*bitmap->bitmap_cell_x_size;
	}
	else
	{
		bitmap->bitmap_extend_x_size = bitmap->bitmap_real_x_size;
	}

	if((bitmap->bitmap_real_y_size%bitmap->bitmap_cell_y_size) != 0)
	{
		bitmap->bitmap_extend_y_size = (bitmap->bitmap_real_y_size/bitmap->bitmap_cell_y_size + 1 )*bitmap->bitmap_cell_y_size;
	}
	else
	{
		bitmap->bitmap_extend_y_size = bitmap->bitmap_real_y_size;
	}

	return GraphicApplyBitmapDatabuf(bitmap);
}

/*****************************************************************************
 Function Name:
 Purposes     :
 Parameter    :

 Return Value :


 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutChar(GraphicBitmap *bitmap,GraphicCoordinates *coordinates,FontTableAttribute *font)
{
	INT16U i = 0,j= 0,k = 0;
	INT16U pos_x = 0,pos_y = 0;

	pos_x = coordinates ->start_x;
	pos_y = coordinates ->start_y;

	for(i = 0;i< font->font_height;i++)
	{
		k =0;
		for(j = 0;j< font->font_width;j++)
		{
			if( ((j%8) == 0)&& (j!=0))
			{
				k++;
			}
			if(*(font->font_content + k + i*font->bytes_per_line)&(0x1<<(7-j%8)))
			{
				GraphicPutPixel(pos_x+j,pos_y+i,bitmap,1);
			}
		}
	}
}




//-------------------------------------------------------------------
//	                                                                |
//          Pixel Relative Function                                 |
//																	|
//-------------------------------------------------------------------
/*****************************************************************************
 Function Name: GraphicPutPixel
 Purposes     : put pixel to bitmap according to color palette
 Parameter    :
  				x   -> start x
 				y   -> start y
 				bitmap -> working memory
 			    index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixel(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	GraphicPutColorToPixelArray[bitmap->bitmap_color_palette](x,y,bitmap,index);
}


/*****************************************************************************
 Function Name: GraphicCalculatePixelPosition
 Purposes     : caculate the pixel position in bitmap
 Parameter    :
  				x   -> start x
 				y   -> start y
 				bitmap -> working memory
 Return Value :
          		pixel_position
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
INT32U GraphicCalculatePixelPosition(INT16U x,INT16U y,GraphicBitmap *bitmap)
{

	INT16U index_x = 0,index_y = 0;
	INT32U pixel_position =0;

	index_x = bitmap ->bitmap_extend_x_size/bitmap ->bitmap_cell_x_size;
	index_y = bitmap ->bitmap_extend_y_size/bitmap ->bitmap_cell_y_size;

	pixel_position =   (y/bitmap->bitmap_cell_y_size)*(bitmap->bitmap_cell_x_size*bitmap->bitmap_cell_y_size*index_x)\
					  +(x/bitmap->bitmap_cell_x_size)*(bitmap->bitmap_cell_x_size*bitmap->bitmap_cell_y_size)\
					  +(y%bitmap->bitmap_cell_y_size)*(bitmap->bitmap_cell_x_size)\
					  +(x%bitmap->bitmap_cell_x_size);

	return pixel_position;
}


/*****************************************************************************
 Function Name: GraphicPutPixelToYUYV
 Purposes     : change one pixel to YUYV color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelToYUYV(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;

	if(x > bitmap->bitmap_real_x_size||y>bitmap ->bitmap_real_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	else
	{
		data = bitmap ->bitmap_background_color;
	}

	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelToYUYV: pixel_position = %d \r\n",pixel_position);
	#endif

	*(bitmap ->bitmap_databuf +pixel_position) = data;

}

/*****************************************************************************
 Function Name: GraphicPutPixelToRGBG
 Purposes     : change one pixel to RGBG color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelToRGBG(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;

	if(x > bitmap->bitmap_real_x_size||y>bitmap ->bitmap_real_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	else
	{
		data = bitmap ->bitmap_background_color;
	}

	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelToRGBG: pixel_position = %d \r\n",pixel_position);
	#endif

	*(bitmap ->bitmap_databuf +pixel_position) = data;
}


/*****************************************************************************
 Function Name: GraphicPutPixelTo16BIT
 Purposes     : change one pixel to 2bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo16BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;

	if(x > bitmap->bitmap_real_x_size||y>bitmap ->bitmap_real_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	else
	{
		data = bitmap ->bitmap_background_color;
	}

	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelToYUYV: pixel_position = %d \r\n",pixel_position);
	#endif

	*(bitmap ->bitmap_databuf +pixel_position) = data;
}


/*****************************************************************************
 Function Name: GraphicPutPixelTo15BIT
 Purposes     : change one pixel to 15bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo15BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;

	if(x > bitmap->bitmap_real_x_size||y>bitmap ->bitmap_real_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	else
	{
		data = bitmap ->bitmap_background_color;
	}

	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelToYUYV: pixel_position = %d \r\n",pixel_position);
	#endif

	*(bitmap ->bitmap_databuf +pixel_position) = data;
}


/*****************************************************************************
 Function Name: GraphicPutPixelTo8BIT
 Purposes     : change one pixel to 8bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo8BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;

	if(x > bitmap->bitmap_extend_x_size||y>bitmap ->bitmap_extend_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	#if 1
	else
	{
		data = bitmap ->bitmap_background_color;
	}
	#endif
	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelTo8BIT: pixel_position = %d \r\n",pixel_position);
	#endif

	*((INT8U*)bitmap ->bitmap_databuf +pixel_position) = data;

}


/*****************************************************************************
 Function Name: GraphicPutPixelTo6BIT
 Purposes     : change one pixel to 6bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo6BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{

}


/*****************************************************************************
 Function Name: GraphicPutPixelTo4BIT
 Purposes     : change one pixel to 4bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo4BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{
	INT16U data;
	INT32U pixel_position =0;
	INT8U pixel ;

	if(x > bitmap->bitmap_extend_x_size||y>bitmap ->bitmap_extend_y_size)
		return ;

	if(index == 1)
	{
		data = bitmap ->bitmap_front_color;
	}
	#if 1
	else
	{
		data = bitmap ->bitmap_background_color;
	}
	#endif
	pixel_position = GraphicCalculatePixelPosition(x,y,bitmap);
	pixel = data&0xf;

	#ifdef GPLIB_GRAPHIC_DBG
	GRAPHIC_DBG_PRINT("GraphicPutPixelTo4BIT: pixel_position = %d \r\n",pixel_position);
	#endif

	if(pixel_position%2)
	{
		*((INT8U*)bitmap ->bitmap_databuf + pixel_position/2) &= 0xf0;
		*((INT8U*)bitmap ->bitmap_databuf + pixel_position/2) |= pixel;
	}
	else
	{
		*((INT8U*)bitmap ->bitmap_databuf + pixel_position/2) &= 0x0f;
		*((INT8U*)bitmap ->bitmap_databuf + pixel_position/2) |= pixel<<4;
	}
}
/*****************************************************************************
 Function Name: GraphicPutPixelTo2BIT
 Purposes     : change one pixel to 2bit color mode
 Parameter    : x   -> start x
 				y   -> start y
 				bitmap -> working memory
 				index  -> color to set
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutPixelTo2BIT(INT16U x,INT16U y,GraphicBitmap *bitmap,INT8U index)
{

}

/*****************************************************************************
 Function Name: GraphicFinishDrv
 Purposes     : Release graphic resource
 Parameter    : bitmap  -> the working space of all the graphics

 				coordinates -> the coordinates
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicFinishDrv(GraphicBitmap *bitmap)//,GraphicCoordinates *coordinates)
{
	gp_free((void *) bitmap ->bitmap_databuf );
	gp_free((void *) bitmap );
	//gp_free((void *) coordinates );
}



//-------------------------------------------------------------------
//	                                                                |
//           Graphic  OS Relative Function                          |
//																	|
//-------------------------------------------------------------------

/*****************************************************************************
 Function Name: GraphicOSLock
 Purposes     : Protect the graphics operation
 Parameter    : Null
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicOSLock(void)
{
	#if _OPERATING_SYSTEM != _OS_NONE

	#endif
}

/*****************************************************************************
 Function Name: GraphicOSUnlock
 Purposes     : Protect the graphics operation
 Parameter    : Null
 Return Value :
          		Null
 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicOSUnlock(void)
{
	#if _OPERATING_SYSTEM != _OS_NONE

	#endif
}

/*****************************************************************************
 Function Name: GraphicSetBackgroundColor
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/

void GraphicSetBitmapBackgroundColor(GraphicBitmap *bitmap,INT16U color)
{
	if(bitmap ->bitmap_background_color == color)
	{
		return;
	}
	else
	{
		bitmap ->bitmap_background_color = color;
	}

	switch (bitmap->bitmap_color_palette)
	{
		case GRAPHIC_2_BIT_COLOR_MODE:
			break;

		case GRAPHIC_4_BIT_COLOR_MODE:
			break;

		case GRAPHIC_6_BIT_COLOR_MODE:
			break;

		case GRAPHIC_8_BIT_COLOR_MODE:

			gp_memset((INT8S *)bitmap->bitmap_databuf, bitmap ->bitmap_background_color,(bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size));
			break;

		case GRAPHIC_15_BIT_COLOR_MODE:
		case GRAPHIC_16_BIT_COLOR_MODE:
		case GRAPHIC_RGBG_COLOR_MODE:
		case GRAPHIC_YUYV_COLOR_MODER:

			gp_memset((INT8S *)bitmap->bitmap_databuf, bitmap ->bitmap_background_color,(bitmap->bitmap_extend_x_size *bitmap->bitmap_extend_y_size)<<1);
			break;

		default :
			break;
	}

}

/*****************************************************************************
 Function Name: GraphicSetBitmapFrontColor
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicSetBitmapFrontColor(GraphicBitmap *bitmap,INT16U color)
{
	if(bitmap ->bitmap_front_color == color)
	{
		return;
	}
	else
	{
		bitmap ->bitmap_front_color = color;
	}

}


/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line)
{

	if(line ->start_y == line ->end_y)
	{
		GraphicPutHorizontalLine(bitmap,line);
	}
	else if(line ->start_x == line ->end_x)
	{
		GraphicPutVerticalLine(bitmap,line);
	}
	else
	{
		GraphicPutBiasLine(bitmap,line);
	}
}


/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
//horizontal line "-"
void GraphicPutHorizontalLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line)
{
	INT16U i = 0,j = 0;
	INT16U temp ;
	GraphicSetBitmapFrontColor(bitmap,line ->color);

	if(line ->start_x > bitmap ->bitmap_extend_x_size)
		return;

	if(line ->start_y > bitmap ->bitmap_extend_y_size)
		return;

	if(line ->start_x >line ->end_x)
	{
		temp = line ->end_x;
		line ->end_x = line ->start_x;
		line ->start_x = temp;
	}

	for(i = 0; i<line ->width;i++)
	{
		for(j =0;j<=(line->end_x - line->start_x);j++)
		{
			if((line ->start_x +j)>= bitmap ->bitmap_extend_x_size)
			{
				break;
			}
			GraphicPutPixel(line ->start_x+j, line->start_y+i,bitmap,1);
		}
	}
}

/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
//vertical line  "|"
void GraphicPutVerticalLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line)
{

	INT16U i = 0,j = 0;
	INT16U temp ;

	if(line ->start_x > bitmap ->bitmap_extend_x_size)
		return;

	if(line ->start_y > bitmap ->bitmap_extend_y_size)
		return;


	GraphicSetBitmapFrontColor(bitmap,line ->color);

	if(line ->start_y >line ->end_y)
	{
		temp = line ->end_y;
		line ->end_y = line ->start_y;
		line ->start_y= temp;
	}

	for(i = 0; i<line ->width;i++)
	{
		for(j =0;j<=(line ->end_y - line ->start_y);j++)
		{
			if((line ->start_y +j) >= bitmap ->bitmap_extend_y_size)
				break;

			GraphicPutPixel(line ->start_x+i, line->start_y+j,bitmap,1);
		}
	}

}
/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutBiasLine(GraphicBitmap *bitmap,GraphicLineAttirbute *line)
{
	INT16U loop ;
	GraphicSetBitmapFrontColor(bitmap,line ->color);
	for(loop = 0;loop < line ->width;loop++ )
	{
		GraphicPutBiasLineCaculate(bitmap,line ->start_x +loop,line ->start_y,line ->end_x+loop,line ->end_y);
	}

}

/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutBiasLineCaculate(GraphicBitmap *bitmap,INT32U x0,INT32U y0,INT32U x1,INT32U y1)
{
	INT32S dx;
	INT32S dy;
	INT8S  dx_sym;
	INT8S  dy_sym;
	INT32S dx_x2;
	INT32S dy_x2;
	INT32S di;

	dx = x1 - x0;
	dy = y1 - y0;

	if(dx > 0)
		dx_sym = 1;
	else
	{
		if(dx < 0)
			dx_sym = -1;
		else
			return ;//Horitial line no need draw here.
	}

	if(dy > 0)
		dy_sym = 1;
	else
	{
		if(dy < 0)
			dy_sym = -1;
		else
			return ;//vertial line no need draw here.
	}

	dx = dx_sym * dx;
	dy = dy_sym * dy;

	dx_x2 = dx*2;
	dy_x2 = dy*2;

	if(dx >= dy)
	{
		di = dy_x2 - dx;
		while (x0 != x1)
		{
			GraphicPutPixel(x0, y0,bitmap,1);
			x0 += dx_sym;
			if(di < 0)
				di += dy_x2;
			else
			{
				di += dy_x2 - dx_x2;
				y0 += dy_sym;
			}

		}
		GraphicPutPixel(x0, y0,bitmap,1);
	}
	else
	{
		di = dx_x2 - dy;
		while (y0 != y1)
		{
			GraphicPutPixel(x0, y0,bitmap,1);
			y0 += dy_sym;
			if(di <0)
				di+=dx_x2;
			else
			{
				di += dx_x2 - dy_x2;
				x0 += dx_sym;
			}
		}
		GraphicPutPixel(x0, y0,bitmap,1);
	}
}

/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutRectangle(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle)
{
	if(rectangle ->fill_color == bitmap ->bitmap_background_color)
	{
		GraphicPutRectangleNoFillColor(bitmap,rectangle);
	}
	else
	{
		GraphicPutRectangleFillColor(bitmap,rectangle);
	}

}
/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutRectangleNoFillColor(GraphicBitmap * bitmap, GraphicRectangleAttirbute * rectangle)
{
	GraphicLineAttirbute *line;
	line = (GraphicLineAttirbute*)gp_malloc(sizeof(GraphicLineAttirbute));
	if(line == NULL)
	{
		DBG_PRINT("Failed to allocate line in GraphicPutRectangleNoFillColor()\r\n");
		return;
	}

	line->color = rectangle ->line_color;
	line->width= rectangle ->line_width;

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x + rectangle ->width - rectangle ->line_width;
	line->end_y   = rectangle ->start_y;
	GraphicPutHorizontalLine(bitmap,line);

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y + rectangle ->height - rectangle ->line_width ;//+ 1;
	line->end_x   = rectangle ->start_x + rectangle ->width  - rectangle ->line_width;
	line->end_y   = rectangle ->start_y + rectangle ->height - rectangle ->line_width ;//+ 1;
	GraphicPutHorizontalLine(bitmap,line);

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x;
	line->end_y   = rectangle ->start_y + rectangle ->height-1;
	GraphicPutVerticalLine(bitmap,line);


	line->start_x = rectangle ->start_x + rectangle ->width - rectangle ->line_width ;//+ 1;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x + rectangle ->width - rectangle ->line_width ;//+ 1;
	line->end_y   = rectangle ->start_y + rectangle ->height-1;
	GraphicPutVerticalLine(bitmap,line);

	gp_free((void *)line);
}
/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutRectangleFillColor(GraphicBitmap * bitmap, GraphicRectangleAttirbute * rectangle)
{

	GraphicLineAttirbute *line;
	INT16U loop = 0;

	line = (GraphicLineAttirbute*)gp_malloc(sizeof(GraphicLineAttirbute));
	if(line == NULL)
	{
		DBG_PRINT("Failed to allocate line in GraphicPutRectangleFillColor()\r\n");
		return;
	}

	line->color = rectangle ->line_color;
	line->width= rectangle ->line_width;

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x + rectangle ->width - rectangle ->line_width;
	line->end_y   = rectangle ->start_y;
	GraphicPutHorizontalLine(bitmap,line);

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y + rectangle ->height - rectangle ->line_width ;//+ 1;
	line->end_x   = rectangle ->start_x + rectangle ->width  - rectangle ->line_width;
	line->end_y   = rectangle ->start_y + rectangle ->height - rectangle ->line_width ;//+ 1;
	GraphicPutHorizontalLine(bitmap,line);

	line->start_x = rectangle ->start_x;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x;
	line->end_y   = rectangle ->start_y + rectangle ->height - 1;
	GraphicPutVerticalLine(bitmap,line);

	line->start_x = rectangle ->start_x + rectangle ->width - rectangle ->line_width ;//+ 1;
	line->start_y = rectangle ->start_y;
	line->end_x   = rectangle ->start_x + rectangle ->width - rectangle ->line_width ;//+ 1;
	line->end_y   = rectangle ->start_y + rectangle ->height - 1;
	GraphicPutVerticalLine(bitmap,line);

	line->start_x = rectangle ->start_x + rectangle ->line_width;
	line->start_y = rectangle ->start_y + rectangle ->line_width;
	line->end_x   = rectangle ->start_x + rectangle ->width  - rectangle ->line_width -1;
	line->end_y   = rectangle ->start_y + rectangle ->line_width;
	line->color = rectangle ->fill_color;
	line->width = 1;

	for(loop = 0;loop < (rectangle ->height  - 2*rectangle ->line_width);loop++)
	{
		GraphicPutHorizontalLine(bitmap,line);
		line->start_y += line->width;
		line->end_y   += line->width;
	}


	gp_free((void *)line);


}

/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutCircle(GraphicBitmap *bitmap,GraphicCircleAttirbute *circle)
{
	//GraphicPutCircleCaculate(bitmap,);
}
/*****************************************************************************
 Function Name:
 Purposes     :
 Arguments    :
 Return Value :

 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
void GraphicPutCircleCaculate(GraphicBitmap *bitmap,INT32U x0,INT32U y0,INT32U r)
{
	INT32S draw_x0,draw_y0;
	INT32S draw_x1,draw_y1;
	INT32S draw_x2,draw_y2;
	INT32S draw_x3,draw_y3;
	INT32S draw_x4,draw_y4;
	INT32S draw_x5,draw_y5;
	INT32S draw_x6,draw_y6;
	INT32S draw_x7,draw_y7;

	INT32S xx,yy;
	INT32S di;

	if(r == 0)return;

	draw_x0 = draw_x1 = x0;
	draw_y0 = draw_y1 = y0 + r;
	if(draw_y0 < bitmap->bitmap_extend_y_size)
		GraphicPutPixel(draw_x0,draw_y0,bitmap,1);	//90

	draw_x2 = draw_x3 = x0;
	draw_y2 = draw_y3 = y0 - r;
	if(draw_y2 >= 0)
		GraphicPutPixel(draw_x2,draw_y2,bitmap,1);	//270

	draw_x4 = draw_x6 = x0 + r;
	draw_y4 = draw_y6 = y0;
	if(draw_y0 < bitmap->bitmap_extend_x_size)
		GraphicPutPixel(draw_x4,draw_y4,bitmap,1);	//0

	draw_x5 = draw_x7 = x0 - r;
	draw_y5 = draw_y7 = y0;
	if(draw_x5 >= 0)
		GraphicPutPixel(draw_x5,draw_y5,bitmap,1);	//180

	if(r == 1)return;

	//Bresenhan arithmetic
	di = 3 -2*r;
	xx = 0;
	yy = r;

	while(xx < yy)
	{
		if(di < 0)
			di += 4*xx +6;
		else
		{
			di += 4*(xx -yy)+10;
			yy--;
			draw_y0 --;
			draw_y1 --;
			draw_y2 ++;
			draw_y3 ++;
			draw_x4 --;
			draw_x5 ++;
			draw_x6 --;
			draw_x7 ++;
		}
		xx ++;

		draw_x0 ++;
		draw_x1 --;
		draw_x2 ++;
		draw_x3 --;
		draw_y4 ++;
		draw_y5 ++;
		draw_y6 --;
		draw_y7 --;

		if((draw_x0 <= bitmap ->bitmap_extend_x_size)&&(draw_y0 >= 0))
			GraphicPutPixel(draw_x0,draw_y0,bitmap,1);

		if((draw_x1 >= 0)&&(draw_y1 >= 0))
			GraphicPutPixel(draw_x1,draw_y1,bitmap,1);

		if((draw_x2 <= bitmap ->bitmap_extend_x_size)&&(draw_y2 <= bitmap ->bitmap_extend_y_size))
			GraphicPutPixel(draw_x2,draw_y2,bitmap,1);

		if((draw_x3 >= 0)&&(draw_y3 <= bitmap ->bitmap_extend_y_size))
			GraphicPutPixel(draw_x3,draw_y3,bitmap,1);

		if((draw_x4 <= bitmap ->bitmap_extend_x_size)&&(draw_y4 >= 0))
			GraphicPutPixel(draw_x4,draw_y4,bitmap,1);

		if((draw_x5 >= 0)&&(draw_y5 >= 0))
			GraphicPutPixel(draw_x5,draw_y5,bitmap,1);

		if((draw_x6 <= bitmap ->bitmap_extend_x_size)&&(draw_y6 <= bitmap ->bitmap_extend_y_size))
			GraphicPutPixel(draw_x6,draw_y6,bitmap,1);

		if((draw_x7 >= 0)&&(draw_y7 <= bitmap ->bitmap_extend_y_size ))
			GraphicPutPixel(draw_x7,draw_y7,bitmap,1);
	}
}

