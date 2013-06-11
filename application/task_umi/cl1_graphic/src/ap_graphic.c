#include "ap_graphic.h"

//-----------------------------------------------------------------------------------
//        Basic graphic function.Generate from gplib graphic driver.                 |
//-----------------------------------------------------------------------------------
INT32U cl1_graphic_init(GraphicBitmap *bitmap);
void   cl1_graphic_finish(GraphicBitmap *bitmap);
void   cl1_graphic_bitmap_front_color_set(GraphicBitmap *bitmap,INT16U color);
void   cl1_graphic_bitmap_background_color_set(GraphicBitmap *bitmap,INT16U color);
void   cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
void   cl1_graphic_char_draw(GraphicBitmap *bitmap,unsigned short character,GraphicCoordinates *coordinates,INT8U language,INT8U font_type);
void   cl1_graphic_line_draw(GraphicBitmap *bitmap,GraphicLineAttirbute *line);
void   cl1_graphic_rectangle_draw(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle);


//-----------------------------------------------------------------------------------
//       Font  function.                                                             |
//-----------------------------------------------------------------------------------

void cl1_graphic_number_font_init(void);

void cl1_graphic_font_get(INT16U character_code,FontTableAttribute *font,INT8U language,INT8U font_type);
INT32U cl1_graphic_font_width_get(INT16U character_code,INT8U language,INT8U font_type);
INT32U cl1_graphic_font_height_get(INT16U character_code,INT8U language,INT8U font_type);
INT32U cl1_graphic_font_line_bytes_get(INT16U character_code,INT8U language,INT8U font_type);

//-----------------------------------------------------------------------------------
//          Strings   Function.                                                      |
//-----------------------------------------------------------------------------------
INT32U cl1_graphic_strings_newline_get(char *strings,INT8U language,INT8U font_type);
INT32U cl1_graphic_strings_width_get_for_no_newline(char *strings,INT8U language,INT8U font_type);
INT32U cl1_graphic_strings_height_get(char *strings,INT8U language,INT8U font_type);
void   cl1_graphic_strings_copy(INT8S *dest,INT8S *source,INT16U length );
void   cl1_graphic_character_sizes_get(INT16U width,INT16U height,INT16U *char_x,INT16U *char_y);


//-----------------------------------------------------------------------------------
//          Application Function.                                                    |
//-----------------------------------------------------------------------------------
//Draw Strings center.
void   cl1_graphic_strings_centers_draw(StrinsCentersDrawPar *parameter,GraphicBitmap* bitmap,char *strings,INT8U language,INT8U font_type);
void   cl1_graphic_free_strings_centers_draw(GraphicBitmap *bitmap);

/*****************************************************************************
 Function Name:
 Purposes     :
 Parameter    :

 Return Value :


 <History>
 Date                         Author                         Modification
 2008/06/24                   Lichuanyue                     Created
*******************************************************************************/
#define FONT_TABLE_NUMBERS  5
t_FONT_TABLE_STRUCT  NumberFontTable[FONT_TABLE_NUMBERS*10];

void cl1_graphic_number_font_init(void)
{
	INT8U i = 0,j = 0;	
	for(i = 0;i < FONT_TABLE_NUMBERS;i ++)
	{
		for(j = 0;j < 10;j++)
		{
			resource_font_load( 1, FONT_HEIGHT_lv1 + i, 16+ j,(t_FONT_TABLE_STRUCT *) &NumberFontTable[i*10 +j]);
		}
	}
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
void cl1_graphic_number_font_load(INT16U index, INT16U target_char, t_FONT_TABLE_STRUCT *font_table)
{
	*font_table = NumberFontTable[(index - FONT_HEIGHT_lv1)*10 + (target_char - 16) ];

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
void cl1_graphic_font_get(INT16U character_code,FontTableAttribute *font,INT8U language,INT8U font_type)
{
	if(language == 1)
	{
		character_code-= 0x0020;
	}

	#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY) //For speed up load number font 
	if(language == 1 && ((character_code <= 25 )&&(character_code >= 16)))
	{
		cl1_graphic_number_font_load((INT16U) font_type,character_code,(t_FONT_TABLE_STRUCT*) font);
	}
	else
	{
		resource_font_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) font );
	}
	#else
		resource_font_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) font );
	#endif 
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
INT32U cl1_graphic_font_width_get(INT16U character_code,INT8U language,INT8U font_type)
{
	INT8U width = 0;
	FontTableAttribute font;
	
	if(language == 1)
	{
		character_code -= 0x0020;
	}
	
#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY) //For speed up load number font 
	if(language == 1 && ((character_code <= 25 )&&(character_code >= 16)))
		cl1_graphic_number_font_load((INT16U) font_type,character_code,(t_FONT_TABLE_STRUCT*) &font);
	else
		resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#else
	resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#endif 

	width = font.font_width;

	return width;
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
INT32U cl1_graphic_font_line_bytes_get(INT16U character_code,INT8U language,INT8U font_type)
{
	INT8U bytes = 0;
	FontTableAttribute font;

	if(language==1)
	{
		character_code-=0x0020;
	}
	
#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)//For speed up load number font 
	if(language == 1 && ((character_code <= 25 )&&(character_code >= 16)))
		cl1_graphic_number_font_load((INT16U) font_type,character_code,(t_FONT_TABLE_STRUCT*) &font);
	else	
		resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#else
	resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#endif 

	bytes = font.bytes_per_line;
	
	return bytes;
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
INT32U cl1_graphic_font_height_get(INT16U character_code,INT8U language,INT8U font_type)
{
	INT8U height = 0;
	FontTableAttribute font;
	
	if(language==1)
	{
		character_code-=0x0020;
	}
	
#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY) //For speed up load number font 
	if(language == 1 && ((character_code <= 25 )&&(character_code >= 16)))
		cl1_graphic_number_font_load((INT16U) font_type,character_code,(t_FONT_TABLE_STRUCT*) &font);
	else	
		resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#else
	resource_font_table_load( (INT16U) language, (INT16U) font_type, character_code, (t_FONT_TABLE_STRUCT*) &font );
#endif 


	height = font.font_height;
	
	return height;
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
void cl1_graphic_frame_string_draw(GraphicBitmap *bitmap,	
	                             char *strings,
                             	 INT8U language,
	 							 INT8U font_type,	 
	                             GraphicFrameChar *frame)
{
	INT32U iStrLoc = 0;
	INT16U character = 0;
	INT8U char_width_mode;

  	char_width_mode=resource_character_width_get(language);
    while(1)
	{
		if (char_width_mode == 1)
		{
   			character = (INT16U) strings[iStrLoc];
   			if (character == 0)
   				break;

   			if((character<0x20)||character>0x7f)
				character = '_';

			iStrLoc ++;
		}
		else
		{
   			character = ((INT16U *)strings)[iStrLoc];
   			if (character == 0)
   				break;

			iStrLoc ++;
		}	
		cl1_graphic_frame_char_draw(bitmap,character,language,font_type,frame);
	}
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
void cl1_graphic_frame_char_draw(GraphicBitmap *bitmap,	
	                             INT16U character,
                             	 INT8U language,
	 							 INT8U font_type,	 
	                             GraphicFrameChar *frame)	  
{
	INT16U pos_x = 0,pos_y = 0;
	FontTableAttribute font;
	GraphicCoordinates coordinates;
	
	pos_x = frame ->start_x;
	pos_y = frame ->start_y;
	cl1_graphic_font_get(character,&font,language,font_type);
	if(((pos_x+font.font_width) >bitmap->bitmap_extend_x_size)||\
	   ((pos_y+font.font_height) >bitmap->bitmap_extend_y_size))
		return ;

	bitmap ->bitmap_front_color = frame ->frame_color;

	if(frame ->frame_width >2)
		frame ->frame_width = 1;
	
	if(frame ->frame_width != 0)
	{
		coordinates.start_x = pos_x + frame ->frame_width;
		coordinates.start_y = pos_y;
		GraphicPutChar(bitmap,&coordinates,&font);
		coordinates.start_x = pos_x - frame ->frame_width;
		coordinates.start_y = pos_y;
		GraphicPutChar(bitmap,&coordinates,&font);
		coordinates.start_x = pos_x ;
		coordinates.start_y = pos_y +frame ->frame_width;
		GraphicPutChar(bitmap,&coordinates,&font);
		coordinates.start_x = pos_x ;
		coordinates.start_y = pos_y -frame ->frame_width;
		GraphicPutChar(bitmap,&coordinates,&font);
	}
	bitmap ->bitmap_front_color = frame ->front_color;
	coordinates.start_x = pos_x;
	coordinates.start_y = pos_y;
	GraphicPutChar(bitmap,&coordinates,&font);
	frame ->start_x = pos_x + font.font_width;
	frame ->start_y = pos_y ;
#if 1
	if(!((language== 1)&& (character >=0x0030 &&character <= 0x0039)))
#endif  
	gp_free((void *)font.font_content);
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
INT32U cl1_graphic_init(GraphicBitmap *bitmap)
{
	return GraphicInitDrv(bitmap);
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
void cl1_graphic_finish(GraphicBitmap *bitmap)
{
	gp_free((void *) bitmap ->bitmap_databuf );
	
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
void cl1_graphic_strings_draw(GraphicBitmap *bitmap,char *strings,GraphicCoordinates *coordinates,INT8U language,INT8U font_type)
{

    INT32U iStrLoc = 0;
	INT16U character = 0;
	INT8U char_width_mode;

  	char_width_mode=resource_character_width_get(language);
    while(1)
	{
		if (char_width_mode == 1)
		{
   			character = (INT16U) strings[iStrLoc];
   			if (character == 0)
   				break;

   			if((character<0x20)||character>0x7f)
				character = '_';

			iStrLoc ++;
		}
		else
		{
   			character = ((INT16U *)strings)[iStrLoc];
   			if (character == 0)
   				break;

			iStrLoc ++;
		}
		cl1_graphic_char_draw(bitmap,character,coordinates,language,font_type);		
	}
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
void cl1_graphic_char_draw(GraphicBitmap *bitmap,unsigned short character,GraphicCoordinates *coordinates,INT8U language,INT8U font_type)
{

	INT16U pos_x = 0,pos_y = 0;
	FontTableAttribute font;
	
	pos_x = coordinates ->start_x;
	pos_y = coordinates ->start_y;
	
	cl1_graphic_font_get(character,&font,language,font_type);

	if(((pos_x+font.font_width) >bitmap->bitmap_extend_x_size)||((pos_y+font.font_height) >bitmap->bitmap_extend_y_size))
		return ;

	GraphicPutChar(bitmap,coordinates,&font);

	coordinates ->start_x += font.font_width;
	coordinates ->start_y = coordinates ->start_y;

#if 1
	if(!((language== 1)&& (character >=0x0030 &&character <= 0x0039)))
#endif 
	{
		gp_free((void *)font.font_content);	//Neal added
	}

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
void cl1_graphic_line_draw(GraphicBitmap *bitmap,GraphicLineAttirbute *line)
{
	GraphicPutLine(bitmap,line);
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
void cl1_graphic_rectangle_draw(GraphicBitmap *bitmap,GraphicRectangleAttirbute *rectangle)
{
	GraphicPutRectangle(bitmap,rectangle);
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
void cl1_graphic_bitmap_front_color_set(GraphicBitmap *bitmap,INT16U color)
{
	GraphicSetBitmapFrontColor(bitmap,color);
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
void cl1_graphic_bitmap_background_color_set(GraphicBitmap *bitmap,INT16U color)
{
	GraphicSetBitmapBackgroundColor(bitmap,color);
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
INT32U cl1_graphic_strings_newline_get(char *strings,INT8U language,INT8U font_type)
{
	INT32U newline   = 0;
	INT16U character = 0;
    INT32U iStrLoc   = 0;

    while( (character = ((INT8U *)strings)[iStrLoc]) != '\0' )
	{
		if(character == '\n')
			newline ++;
		iStrLoc ++;
	}

	return newline;
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
//For No NewLine
INT32U cl1_graphic_strings_width_get_for_no_newline(char *strings,INT8U language,INT8U font_type)
{

	INT16U width     = 0;
      INT32U iStrLoc = 0;
	INT16U character = 0;
	INT8U char_width_mode;

  	char_width_mode = resource_character_width_get(language);
    while(1)
	{
		if (char_width_mode == 1)
		{
   			character = (INT16U) strings[iStrLoc];
   			if (character == 0)
   				break;

   			if((character<0x20)||character>0x7f)
				character = '_';
			width += cl1_graphic_font_width_get(character,language,font_type);
			iStrLoc ++;
		}
		else
		{
   			character = ((INT16U *)strings)[iStrLoc];
   			if (character == 0)
   				break;
			width += cl1_graphic_font_width_get(character,language,font_type);
			iStrLoc ++;
		}
	}

	return width;
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

INT32U cl1_graphic_strings_height_get(char *strings,INT8U language,INT8U font_type)
{

	INT16U height = 0;
    INT32U iStrLoc = 0;
	INT16U character = 0;
	INT8U char_width_mode;

  	char_width_mode=resource_character_width_get(language);
    while(1)
	{
		if (char_width_mode == 1)
	{
   			character = (INT16U) strings[iStrLoc];
   			if (character == 0)
   				break;

   			if((character<0x20)||character>0x7f)
				character = '_';
			height = cl1_graphic_font_height_get(character,language,font_type);
			iStrLoc ++;
	}
	else
	{
   			character = ((INT16U *)strings)[iStrLoc];
   			if (character == 0)
   				break;
			height = cl1_graphic_font_height_get(character,language,font_type);
   			iStrLoc ++;
   		}
	}

	return height;
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
void cl1_graphic_strings_copy(INT8S *dest,INT8S *source,INT16U length )
{
	INT16U i = 0;
	gp_memset((INT8S*)dest, 0, 255);
	for(i = 0;i< length;i++)
	{
		*(dest+i)=*(source + i);
	}
	*(dest+length) = '\0';

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
void cl1_graphic_character_sizes_get(INT16U width,INT16U height,INT16U *char_x,INT16U *char_y)
{
	if(width >32)
		*char_x = 64;
	else if(width >16)
		*char_x = 32;
	else if(width >8)
		*char_x =16;
	else
	    *char_x = 8;

	if(height >32)
		*char_y = 64;
	else if(height >16)
		*char_y = 32;
	else if(height >8)
		*char_y =16;
	else
	    *char_y = 8;

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
void cl1_graphic_strings_centers_draw(StrinsCentersDrawPar *parameter,GraphicBitmap *bitmap,char *strings,INT8U language,INT8U font_type)
{
	
	INT32U bitmap_height = 0,bitmap_width  = 0;
	GraphicCoordinates coordinates;
	GraphicLineAttirbute line;


	bitmap_height = cl1_graphic_strings_height_get(strings,language,font_type);
	bitmap_width  = cl1_graphic_strings_width_get_for_no_newline(strings,language,font_type);


	bitmap ->bitmap_data_mode     = 0;
	bitmap ->bitmap_color_palette = GRAPHIC_8_BIT_COLOR_MODE;
	bitmap ->bitmap_front_color   = parameter ->font_color;
	bitmap ->bitmap_background_color = parameter ->background_color;

	coordinates.start_x = 0;
	coordinates.start_y = 0;

	bitmap ->bitmap_real_x_size = bitmap_width;
    bitmap ->bitmap_real_y_size = bitmap_height;
    cl1_graphic_character_sizes_get(bitmap->bitmap_real_x_size,bitmap->bitmap_real_y_size,&bitmap->bitmap_cell_x_size,&bitmap->bitmap_cell_y_size);

	cl1_graphic_init(bitmap);

	
	cl1_graphic_strings_draw(bitmap, strings,&coordinates, language, font_type);
	

	line.width   = bitmap->bitmap_extend_y_size - bitmap->bitmap_real_y_size;
	if(line.width != 0)
	{
		line.color   = COLOR_TRANSPARENT;
		line.start_x = 0;
		line.start_y = bitmap->bitmap_real_y_size;
		line.end_x   = line.start_x + bitmap->bitmap_extend_x_size;
		line.end_y   = line.start_y ;
		cl1_graphic_line_draw(bitmap,&line);
	}
	line.width = bitmap->bitmap_extend_y_size;
	if(line.width != 0)
	{
		line.color   = COLOR_TRANSPARENT;
		line.start_x = bitmap->bitmap_real_x_size;
		line.start_y = 0;
		line.end_x   = bitmap->bitmap_extend_x_size;
		line.end_y   = line.start_y ;
		cl1_graphic_line_draw(bitmap,&line);
	}


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
void cl1_graphic_free_strings_centers_draw(GraphicBitmap *bitmap)
{
	gp_free((void*)bitmap ->bitmap_databuf);
}

