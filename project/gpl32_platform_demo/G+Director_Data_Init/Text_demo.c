/*
* Description: This task is the G+PPUImageConverter text data verification.
*
* Author: Cater Chen
*
* Date: 2009/02/12
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "Text_demo.h"

#define HIGH_HALF_WORD      0xFFFF0000

/*******************************************************************************
 * FUNCTION NAME: PPU_MemCpy
 * DESCRIPTION: Memory copy function for PPU driver
 * INPUT:
 *  - INT32U *pDest             : Destination address pointer
 *  - INT32U *pSrc              : Source address pointer
 *  - INT32S nBytes             : Byte numbers
 * OUTPUT:
 *  - INT32U *pDest             : Destination data
 * RETURN:
 *  - STATUS_OK                 : Complete this function
 *  - STATUS_FAIL               : Invalid input parameters
 * NOTE:
 *  - This function is activated by CPU.
 ******************************************************************************/
INT16U CPU_MemCpy (INT32U *pDest, INT32U *pSrc, INT32S nBytes, INT16U mem_copy_8_16_32)
{
    INT32S  i;
    INT32U  *pPtrDest, *pPtrSrc;
    CHAR    *pPtr8Dest, *pPtr8Src;
    INT16U  *pPtr16Dest, *pPtr16Src;

    if(mem_copy_8_16_32==32){
       //  32-bit memory copy
       pPtrDest = pDest;
       pPtrSrc = pSrc;
       for (i=0;i<(nBytes/sizeof (INT32U));i++) {
           *(pPtrDest++) = *(pPtrSrc++);
       }
    }else if(mem_copy_8_16_32==16){    
       //  32-bit memory copy
       pPtr16Dest = (INT16U *)pDest;
       pPtr16Src = (INT16U *)pSrc;
       for (i=0;i<(nBytes/sizeof (INT16U));i++) {
           *(pPtr16Dest++) = *(pPtr16Src++);
       }
    
    }else{   
       //  8-bit memory copy
       pPtr8Dest = (CHAR *)pPtrDest;
       pPtr8Src = (CHAR *)pPtrSrc;
       for (i=0;i<nBytes;i++) {
           *(pPtr8Dest++) = *(pPtr8Src++);
       }
    }  
    return STATUS_OK;
}

void Update_Number_Array(PnTX_Num_Arr text_info)
{
     INT16U TXaddr_mode;
     INT32U NumX,NumY,xsize,ysize,IdxTab,ptIdx,i,temp;
     
     IdxTab=text_info->index_ptr;
     ptIdx=text_info->Num_arr_ptr;
     temp = text_info->nImage_width % text_info->nChar_width;
     if(temp)
       NumX = (text_info->nImage_width / text_info->nChar_width)+1;	    //number of horizontal character in real text
     else
       NumX = text_info->nImage_width / text_info->nChar_width;	    //number of horizontal character in real text
     temp = text_info->nImage_height % text_info->nChar_height;
     if(temp)
       NumY = (text_info->nImage_height / text_info->nChar_height)+1;	    //number of vertical character in real text
     else  
	   NumY = text_info->nImage_height / text_info->nChar_height;	    //number of vertical character in real text
	 xsize = text_info->ntext_width / text_info->nChar_width;	    //number of horizontal character in number array					 
	 ysize = text_info->ntext_height / text_info->nChar_height;	    //number of vertical character in number array
	 TXaddr_mode=text_info->nTxAddr_mode;
     if(TXaddr_mode==0){
        for(i=0; i<ysize; i++) 	//Update number array
            CPU_MemCpy((INT32U *)(ptIdx+(xsize*2)*i),(INT32U *)(IdxTab+(NumX*2)*i),(xsize*2),16);
     }else{
        for(i=0; i<ysize; i++) 	//Update number array
            CPU_MemCpy((INT32U *)(ptIdx+(xsize*4)*i),(INT32U *)(IdxTab+(NumX*4)*i),(xsize*4),32);
     }       
}

void load_text_data(PPU_REGISTER_SETS *p_register_set,INT32U text_index,nTX_image_info *text_info)
{
     PPic_Header text_information;
     PText_Header text_information_c;
     nTX_Num_Arr text_num_array;
     INT32U Header_Base_Addr,Text_index_ptr,Text_data_ptr,temp_index_ptr,temp_data_ptr,image_temp;
     INT16U nCell_X,nCell_Y,number_array_updata,special_bmp,*temp_info,*temp_x,*temp_y,text_x,text_y;
     INT16U *index_temp_h,*data_temp_h,*index_temp_l,*data_temp_l,temp_buf;
          
     if(text_info->image_type==TEXT_DATA_BIN){
        //.bin address of SDRAM
        Header_Base_Addr=text_info->ntext_data_ptr;
        special_bmp=0;
        
        //Text enable
        gplib_ppu_text_enable_set(p_register_set,text_index, 1);
     
        // Get TEXT attribute from register	
        gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	                        
     
        // Get TEXT header information	
        text_information=(PPic_Header)text_info->ntext_data_ptr;
        if(text_info->image_number==0)
           text_info->image_number=1;  
        if(text_information->nTx_image < text_info->image_number)
           text_info->image_number=text_information->nTx_image;
        
        //Text mode¡G0: Character mode; 1: Bitmap mode
        gplib_ppu_text_bitmap_mode_set(p_register_set, text_index, text_information->nTx_mode);
     
        //text number array calculate selection 
        number_array_updata=text_information->nTx_mode;
			     
        //Addressing mode¡G0: Relative; 1: Direct  
        gplib_ppu_text_direct_mode_set(p_register_set, text_information->nAddr_mode);
     
        //text number array calculate selection 
        text_num_array.nTxAddr_mode=text_information->nAddr_mode;			                	     
     
        //Color mode¡G0:4,1:16,2:64,3:256,4:32768,5:65536,6:RGBG0,7: RGBG1,8: RGBG2,9: RGBG3,10:YUYV0, 11:YUYV1, 12:YUYV2, 13:YUYV3.   
        switch(text_information->nColor_mode)
        {
           case TXN_COLOR_4:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 0);				                    
                break;        
           case TXN_COLOR_16:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 1);
                break;
           case TXN_COLOR_64:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 2);
                break;                
           case TXN_COLOR_256:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 3); 
                break;       
           case TXN_COLOR_32768:
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_65536:
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 1);
                text_num_array.nTxAddr_mode=1;
                break;        
           case TXN_COLOR_RGBG0:
                gplib_ppu_yuv_type_set(p_register_set, 0);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2); 
                text_num_array.nTxAddr_mode=1;
                break;       
           case TXN_COLOR_RGBG1:
                gplib_ppu_yuv_type_set(p_register_set, 1);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_RGBG2:
                gplib_ppu_yuv_type_set(p_register_set, 2);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;                
           case TXN_COLOR_RGBG3:
                gplib_ppu_yuv_type_set(p_register_set, 3);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;       
           case TXN_COLOR_YUYV0:
                gplib_ppu_yuv_type_set(p_register_set, 0);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;   
           case TXN_COLOR_YUYV1:
                gplib_ppu_yuv_type_set(p_register_set, 1);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_YUYV2:
                gplib_ppu_yuv_type_set(p_register_set, 2);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;                
           case TXN_COLOR_YUYV3:
                gplib_ppu_yuv_type_set(p_register_set, 3);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;
     #if ((MCU_VERSION == GPL326XX))            
           case TXN_COLOR_ALPHA_CHANNEL_BLEND:          
	            gplib_ppu_text_alpha_set(p_register_set,1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
	            gplib_ppu_text_palette_set(p_register_set, text_index, 0, 9);                                
                text_num_array.nTxAddr_mode=1;
                break;                 
           case TXN_COLOR_ARGB4444:               
	            gplib_ppu_text_alpha_set(p_register_set,1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
	            gplib_ppu_text_palette_set(p_register_set, text_index, 0, 11);                                
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_RGBA:               
	            gplib_ppu_text_rgba_set(p_register_set, 1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);                                
                text_num_array.nTxAddr_mode=1;
                break;
     #endif                                       
        }
        
        //Hi-color or palette mode 
     #if ((MCU_VERSION == GPL326XX)) 
        if((text_information->nColor_mode <= TXN_COLOR_256)||(text_information->nColor_mode==TXN_COLOR_ALPHA_CHANNEL_BLEND))
     #else        
        if(text_information->nColor_mode <= TXN_COLOR_256)
     #endif   
        {
           //P1024 share mode¡G0: disable; 1:enable
           if(text_information->nP1024_mode==PAL1024_ENABLE){
              gplib_ppu_palette_type_set(p_register_set, 1,0);
              //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
              gplib_ppu_text_palette_set(p_register_set, text_index, text_information->nPal_type, text_information->nBank_sel);
              gplib_ppu_palette_ram_ptr_set(p_register_set, text_information->nPal_type, text_info->npal_ptr+((256<<1)*text_info->pal_bank));
           }else{
              //Palette type¡G0: 16-bit ; 1: 25-bit
              if(text_information->nPal_type==PAL25_SEPARATE){
                 gplib_ppu_palette_type_set(p_register_set, 0,3);
                 //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
                 gplib_ppu_text_palette_set(p_register_set, text_index, text_information->nPal_sel, text_information->nBank_sel);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, 0, text_info->npal_ptr);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, 2, text_info->npal_ptr+(256<<1));
              }else{
                 gplib_ppu_palette_type_set(p_register_set, 0,1);
                 //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
                 gplib_ppu_text_palette_set(p_register_set, text_index, text_information->nPal_sel, text_information->nBank_sel);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, text_information->nPal_type, text_info->npal_ptr+((256<<1)*text_info->pal_bank));
              }   
           
           }		                	          				                    
        }
     
        //Depth number¡G0 - 3
        gplib_ppu_text_depth_set(p_register_set, text_index, text_information->nDepth_num);
     
        //Flip mode¡G0:No Flip, 1:H Flip, 2:V Flip, 3:H/V Flip
        gplib_ppu_text_flip_set(p_register_set, text_index, text_information->nFlip_mode);	
     
        //Blend level¡G0 - 63
        gplib_ppu_text_blend_set(p_register_set, text_index, 0, 0, text_information->nBlend_level);
     
        //Virtual 3D Mode or not¡G0:No, 1:Yes
        if(text_information->nV3D_Mode==TEXTV3D_ENABLE){
           // TEXT 2.5D
     	   gplib_ppu_text_mode_set(p_register_set, C_PPU_TEXT3, 2);                          //TEXTV3D of text mode                       
	       gplib_ppu_text3_25d_y_compress_set(p_register_set, 0x10);                         //Do not use text 25d y compress.
	       // Disable TEXT1/TEXT2 horizontal/vertical compress function
           gplib_ppu_text_compress_disable_set(p_register_set, 1);                           //When text 25d enable.
        }
          
        //Image width N¡GThe number of images and Image height N¡GThe number of images
        temp_x=(INT16U *)(text_information->nImage_size_offset+Header_Base_Addr);
        if(text_info->image_number!=1){
           temp_x+=((text_info->image_number-1)*2);
           temp_info=temp_x;
           temp_y=temp_info+1;
        }
        temp_info=temp_x;
        temp_y=temp_info+1;
        text_x=*temp_x;
        text_y=*temp_y;
        text_num_array.nImage_width=text_x;
        text_num_array.nImage_height=text_y;
        if(text_x <= 512){
           if(text_y <= 256){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_512X256);
              //Update Number Array info
              text_num_array.ntext_width=512;
              text_num_array.ntext_height=256;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_512X512);
              //Update Number Array info
              text_num_array.ntext_width=512;
              text_num_array.ntext_height=512;
           }        
        }else if(text_x <= 1024){    
           if(text_y <= 512){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_1024X512);
              //Update Number Array info
              text_num_array.ntext_width=1024;
              text_num_array.ntext_height=512;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_1024X1024);
              //Update Number Array info
              text_num_array.ntext_width=1024;
              text_num_array.ntext_height=1024;
           }     
        }else if(text_x <= 2048){ 
           if(text_y <= 1024){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_2048X1024);
              //Update Number Array info
              text_num_array.ntext_width=2048;
              text_num_array.ntext_height=1024;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_2048X2048);
              //Update Number Array info
              text_num_array.ntext_width=2048;
              text_num_array.ntext_height=2048;
           }     
        }else{
           if(text_y <= 2048){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_4096X2048);
              //Update Number Array info
              text_num_array.ntext_width=4096;
              text_num_array.ntext_height=2048;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_4096X4096);
              //Update Number Array info
              text_num_array.ntext_width=4096;
              text_num_array.ntext_height=4096;
           }     
        }
        
        if(text_information->nTx_mode==TEXT_CHARACTER){
           if(text_num_array.ntext_width >= 2048){
              //Character width¡G8, 16, 32, 64, 128, 256 and Character height¡G8, 16, 32, 64, 128, 256
              switch(text_information->nChar_height)
              {
                  case TXN_VS32:
                       nCell_Y=0;				                    
                       break;        
                  case TXN_VS64:
                       nCell_Y=1;
                       break;
                  case TXN_VS128:
                       nCell_Y=2;
                       break;                
                  case TXN_VS256:
                       nCell_Y=3; 
                       break;                
               }
               //Update Number Array info
               text_num_array.nChar_height=text_information->nChar_height;
               switch(text_information->nChar_width)
               {
                  case TXN_HS32:
                       nCell_X=0;				                    
                       break;        
                  case TXN_HS64:
                       nCell_X=1;
                       break;
                  case TXN_HS128:
                       nCell_X=2;
                       break;                
                  case TXN_HS256:
                       nCell_X=3; 
                       break;               
               }
               gplib_ppu_text_character_size_set(p_register_set, text_index, nCell_X, nCell_Y);
               //Update Number Array info
               text_num_array.nChar_width=text_information->nChar_width;
           }else{
               //Character width¡G8, 16, 32, 64, 128, 256 and Character height¡G8, 16, 32, 64, 128, 256
               switch(text_information->nChar_height)
               {
                  case TXN_VS8:
                       nCell_Y=0;				                    
                       break;        
                  case TXN_VS16:
                       nCell_Y=1;
                       break;
                  case TXN_VS32:
                       nCell_Y=2;
                       break;                
                  case TXN_VS64:
                       nCell_Y=3; 
                       break;                
               }
               //Update Number Array info
               text_num_array.nChar_height=text_information->nChar_height;
               switch(text_information->nChar_width)
               {
                  case TXN_HS8:
                       nCell_X=0;				                    
                       break;        
                  case TXN_HS16:
                       nCell_X=1;
                       break;
                  case TXN_HS32:
                       nCell_X=2;
                       break;                
                  case TXN_HS64:
                       nCell_X=3; 
                       break;               
                }
                gplib_ppu_text_character_size_set(p_register_set, text_index, nCell_X, nCell_Y);
                //Update Number Array info
                text_num_array.nChar_width=text_information->nChar_width;     
           }
        }
        
        if(text_information->nTx_mode==TEXT_BITMAP){
           //A special mode for BMP mode
           if(text_num_array.nImage_width==320){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 0);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==640){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 1);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==512){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 2);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==1024){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;              
           }
     #if ((MCU_VERSION == GPL326XX))             
           else if(text_num_array.nImage_width==2048){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==4096){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }
     #endif                 
        }
        
        //text position set
        gplib_ppu_text_position_set(p_register_set, text_index, text_info->position_x, text_info->position_y);
     
        //Image cell index table offset at____CellIndex.bin N¡GThe number of images
        if(text_information->nTx_mode==TEXT_CHARACTER){
           index_temp_l=(INT16U *)(Header_Base_Addr+text_information->index_offset+(text_info->image_number-1)*4);
           index_temp_h=(INT16U *)((Header_Base_Addr+text_information->index_offset+(text_info->image_number-1)*4)+2);
           temp_index_ptr=*index_temp_l;
           temp_buf=*index_temp_h;
           temp_index_ptr|=(temp_buf<<16);
        }
        data_temp_l=(INT16U *)(Header_Base_Addr+text_information->data_offset+(text_info->image_number-1)*4);
        data_temp_h=(INT16U *)((Header_Base_Addr+text_information->data_offset+(text_info->image_number-1)*4)+2);
        temp_data_ptr=*data_temp_l;
        temp_buf=*data_temp_h;
        temp_data_ptr|=(temp_buf<<16);
        Text_index_ptr=Header_Base_Addr+temp_index_ptr;
        Text_data_ptr=Header_Base_Addr+temp_data_ptr;
        
        //Update Number Array info
        if(text_information->nTx_mode==TEXT_CHARACTER)
           text_num_array.index_ptr=Text_index_ptr;
        text_num_array.data_ptr=Text_data_ptr;
        text_num_array.Num_arr_ptr=text_info->nNum_arr_ptr;
        if(number_array_updata==0){
           Update_Number_Array((PnTX_Num_Arr)&text_num_array);
           gplib_ppu_text_number_array_ptr_set(p_register_set, text_index, text_info->nNum_arr_ptr);
           gplib_ppu_text_segment_set(p_register_set, text_index, Text_data_ptr);
        }else{
           if(special_bmp==0){
              gplib_ppu_text_segment_set(p_register_set, text_index, 0); 
              gplib_ppu_text_number_array_ptr_set(p_register_set, text_index, text_info->nNum_arr_ptr);
              gplib_ppu_text_calculate_number_array(p_register_set, text_index, text_x, text_y,Text_data_ptr);
           }
           else
              gplib_ppu_text_segment_set(p_register_set, text_index, Text_data_ptr);
        }
     }else{
        //c header address of SDRAM
        Header_Base_Addr=text_info->ntext_data_ptr;
        special_bmp=0;
        
        //Text enable
        gplib_ppu_text_enable_set(p_register_set,text_index, 1);
     
        // Get TEXT attribute from register	
        gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	                        
     
        // Get TEXT header information	
        text_information_c=(PText_Header)text_info->ntext_data_ptr;
        if(text_info->image_number==0)
           text_info->image_number=1;  
        if(text_information_c->nTx_image < text_info->image_number)
           text_info->image_number=text_information_c->nTx_image;
        if((text_info->pal_bank==0) || (text_info->pal_bank > 4))
           text_info->pal_bank=1;
             
        //Text mode¡G0: Character mode; 1: Bitmap mode
        gplib_ppu_text_bitmap_mode_set(p_register_set, text_index, text_information_c->nTx_mode);
     
        //text number array calculate selection 
        number_array_updata=text_information_c->nTx_mode;
			     
        //Addressing mode¡G0: Relative; 1: Direct  
        gplib_ppu_text_direct_mode_set(p_register_set, text_information_c->nAddr_mode);
     
        //text number array calculate selection 
        text_num_array.nTxAddr_mode=text_information_c->nAddr_mode;			                	     
     
        //Color mode¡G0:4,1:16,2:64,3:256,4:32768,5:65536,6:RGBG0,7: RGBG1,8: RGBG2,9: RGBG3,10:YUYV0, 11:YUYV1, 12:YUYV2, 13:YUYV3.   
        switch(text_information_c->nColor_mode)
        {
           case TXN_COLOR_4:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 0);				                    
                break;        
           case TXN_COLOR_16:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 1);
                break;
           case TXN_COLOR_64:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 2);
                break;                
           case TXN_COLOR_256:
                gplib_ppu_text_color_set(p_register_set, text_index, 0, 3); 
                break;       
           case TXN_COLOR_32768:
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_65536:
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 1);
                text_num_array.nTxAddr_mode=1;
                break;        
           case TXN_COLOR_RGBG0:
                gplib_ppu_yuv_type_set(p_register_set, 0);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2); 
                text_num_array.nTxAddr_mode=1;
                break;       
           case TXN_COLOR_RGBG1:
                gplib_ppu_yuv_type_set(p_register_set, 1);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_RGBG2:
                gplib_ppu_yuv_type_set(p_register_set, 2);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;                
           case TXN_COLOR_RGBG3:
                gplib_ppu_yuv_type_set(p_register_set, 3);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);
                text_num_array.nTxAddr_mode=1;
                break;       
           case TXN_COLOR_YUYV0:
                gplib_ppu_yuv_type_set(p_register_set, 0);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;   
           case TXN_COLOR_YUYV1:
                gplib_ppu_yuv_type_set(p_register_set, 1);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_YUYV2:
                gplib_ppu_yuv_type_set(p_register_set, 2);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;                
           case TXN_COLOR_YUYV3:
                gplib_ppu_yuv_type_set(p_register_set, 3);
                gplib_ppu_text_color_set(p_register_set, text_index, 1, 3);
                text_num_array.nTxAddr_mode=1;
                break;
     #if ((MCU_VERSION == GPL326XX))            
           case TXN_COLOR_ALPHA_CHANNEL_BLEND:          
	            gplib_ppu_text_alpha_set(p_register_set,1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
	            gplib_ppu_text_palette_set(p_register_set, text_index, 0, 9);                                
                text_num_array.nTxAddr_mode=1;
                break;                 
           case TXN_COLOR_ARGB4444:               
	            gplib_ppu_text_alpha_set(p_register_set,1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 0);
	            gplib_ppu_text_palette_set(p_register_set, text_index, 0, 11);                                
                text_num_array.nTxAddr_mode=1;
                break;
           case TXN_COLOR_RGBA:               
	            gplib_ppu_text_rgba_set(p_register_set, 1);
	            gplib_ppu_text_color_set(p_register_set, text_index, 1, 2);                                
                text_num_array.nTxAddr_mode=1;
                break;
     #endif                                       
        }
        
        //Hi-color or palette mode 
     #if ((MCU_VERSION == GPL326XX)) 
        if((text_information_c->nColor_mode <= TXN_COLOR_256)||(text_information_c->nColor_mode==TXN_COLOR_ALPHA_CHANNEL_BLEND))
     #else         
        if(text_information_c->nColor_mode <= TXN_COLOR_256)
     #endif        
        {
           //P1024 share mode¡G0: disable; 1:enable
           if(text_information_c->nP1024_mode==PAL1024_ENABLE){
              gplib_ppu_palette_type_set(p_register_set, 1,0);
              //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
              gplib_ppu_text_palette_set(p_register_set, text_index, text_information_c->nPal_type, text_information_c->nBank_sel);
              gplib_ppu_palette_ram_ptr_set(p_register_set, text_information_c->nPal_type, text_info->npal_ptr+(512*(text_info->pal_bank-1)));
           }else{
              //Palette type¡G0: 16-bit ; 1: 25-bit
              if(text_information_c->nPal_type==PAL25_SEPARATE){
                 gplib_ppu_palette_type_set(p_register_set, 0,3);
                 //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
                 gplib_ppu_text_palette_set(p_register_set, text_index, text_information_c->nPal_sel, text_information_c->nBank_sel);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, 0, text_info->npal_ptr);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, 2, text_info->npal_ptr+512);
              }else{
                 gplib_ppu_palette_type_set(p_register_set, 0,1);
                 //Bank selection¡G0 - 15 and Palette selection¡G0 - 3
                 gplib_ppu_text_palette_set(p_register_set, text_index, text_information_c->nPal_sel, text_information_c->nBank_sel);
                 gplib_ppu_palette_ram_ptr_set(p_register_set, text_information_c->nPal_type, text_info->npal_ptr+(512*(text_info->pal_bank-1)));
              }   
           
           }		                	     				                    
        }
     
        //Depth number¡G0 - 3
        gplib_ppu_text_depth_set(p_register_set, text_index, text_information_c->nDepth_num);
     
        //Flip mode¡G0:No Flip, 1:H Flip, 2:V Flip, 3:H/V Flip
        gplib_ppu_text_flip_set(p_register_set, text_index, text_information_c->nFlip_mode);	
     
        //Blend level¡G0 - 63
        gplib_ppu_text_blend_set(p_register_set, text_index, 0, 0, text_information_c->nBlend_level);
     
        //Virtual 3D Mode or not¡G0:No, 1:Yes
        if(text_information_c->nV3D_Mode==TEXTV3D_ENABLE){
           // TEXT 2.5D
     	   gplib_ppu_text_mode_set(p_register_set, C_PPU_TEXT3, 2);                          //TEXTV3D of text mode                       
	       gplib_ppu_text3_25d_y_compress_set(p_register_set, 0x10);                         //Do not use text 25d y compress.
	       // Disable TEXT1/TEXT2 horizontal/vertical compress function
           gplib_ppu_text_compress_disable_set(p_register_set, 1);                           //When text 25d enable.
        }
   
        //Image width N¡GThe number of images and Image height N¡GThe number of images        
        image_temp=*(text_information_c->nImage_size+(text_info->image_number-1));
        text_x=image_temp;
        text_y=(image_temp & HIGH_HALF_WORD)>>16;
        text_num_array.nImage_width=text_x;
        text_num_array.nImage_height=text_y;
        if(text_x <= 512){
           if(text_y <= 256){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_512X256);
              //Update Number Array info
              text_num_array.ntext_width=512;
              text_num_array.ntext_height=256;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_512X512);
              //Update Number Array info
              text_num_array.ntext_width=512;
              text_num_array.ntext_height=512;
           }        
        }else if(text_x <= 1024){    
           if(text_y <= 512){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_1024X512);
              //Update Number Array info
              text_num_array.ntext_width=1024;
              text_num_array.ntext_height=512;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_1024X1024);
              //Update Number Array info
              text_num_array.ntext_width=1024;
              text_num_array.ntext_height=1024;
           }     
        }else if(text_x <= 2048){ 
           if(text_y <= 1024){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_2048X1024);
              //Update Number Array info
              text_num_array.ntext_width=2048;
              text_num_array.ntext_height=1024;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_2048X2048);
              //Update Number Array info
              text_num_array.ntext_width=2048;
              text_num_array.ntext_height=2048;
           }     
        }else{
           if(text_y <= 2048){
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_4096X2048);
              //Update Number Array info
              text_num_array.ntext_width=4096;
              text_num_array.ntext_height=2048;
           }else{
              gplib_ppu_text_size_set(p_register_set, text_index,TXN_4096X4096);
              //Update Number Array info
              text_num_array.ntext_width=4096;
              text_num_array.ntext_height=4096;
           }     
        }
        
        if(text_information_c->nTx_mode==TEXT_CHARACTER){
           if(text_num_array.ntext_width >= 2048){
              //Character width¡G8, 16, 32, 64, 128, 256 and Character height¡G8, 16, 32, 64, 128, 256
              switch(text_information_c->nChar_height)
              {
                  case TXN_VS32:
                       nCell_Y=0;				                    
                       break;        
                  case TXN_VS64:
                       nCell_Y=1;
                       break;
                  case TXN_VS128:
                       nCell_Y=2;
                       break;                
                  case TXN_VS256:
                       nCell_Y=3; 
                       break;                
               }
               //Update Number Array info
               text_num_array.nChar_height=text_information_c->nChar_height;
               switch(text_information_c->nChar_width)
               {
                  case TXN_HS32:
                       nCell_X=0;				                    
                       break;        
                  case TXN_HS64:
                       nCell_X=1;
                       break;
                  case TXN_HS128:
                       nCell_X=2;
                       break;                
                  case TXN_HS256:
                       nCell_X=3; 
                       break;               
               }
               gplib_ppu_text_character_size_set(p_register_set, text_index, nCell_X, nCell_Y);
               //Update Number Array info
               text_num_array.nChar_width=text_information_c->nChar_width;
           }else{
               //Character width¡G8, 16, 32, 64, 128, 256 and Character height¡G8, 16, 32, 64, 128, 256
               switch(text_information_c->nChar_height)
               {
                  case TXN_VS8:
                       nCell_Y=0;				                    
                       break;        
                  case TXN_VS16:
                       nCell_Y=1;
                       break;
                  case TXN_VS32:
                       nCell_Y=2;
                       break;                
                  case TXN_VS64:
                       nCell_Y=3; 
                       break;                
               }
               //Update Number Array info
               text_num_array.nChar_height=text_information_c->nChar_height;
               switch(text_information_c->nChar_width)
               {
                  case TXN_HS8:
                       nCell_X=0;				                    
                       break;        
                  case TXN_HS16:
                       nCell_X=1;
                       break;
                  case TXN_HS32:
                       nCell_X=2;
                       break;                
                  case TXN_HS64:
                       nCell_X=3; 
                       break;               
                }
                gplib_ppu_text_character_size_set(p_register_set, text_index, nCell_X, nCell_Y);
                //Update Number Array info
                text_num_array.nChar_width=text_information_c->nChar_width;     
           }
        }
        
        if(text_information_c->nTx_mode==TEXT_BITMAP){
           //A special mode for BMP mode
           if(text_num_array.nImage_width==320){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 0);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==640){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 1);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==512){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 2);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==1024){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;              
           }
     #if ((MCU_VERSION == GPL326XX))             
           else if(text_num_array.nImage_width==2048){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }else if(text_num_array.nImage_width==4096){
              gplib_ppu_text_character_size_set(p_register_set, text_index, 3, 3);
              gplib_ppu_text_attribute_source_select(p_register_set, text_index, 1);	    
              special_bmp=1;
           }
     #endif
        }
             
        //text position set
        gplib_ppu_text_position_set(p_register_set, text_index, text_info->position_x, text_info->position_y);
     
        //Image cell index table offset at____CellIndex.bin N¡GThe number of images
        Text_index_ptr=*(text_information_c->index_data+(text_info->image_number-1));
        Text_data_ptr=*(text_information_c->cell_data+(text_info->image_number-1));
        
        //Update Number Array info
        if(text_information_c->nTx_mode==TEXT_CHARACTER)
           text_num_array.index_ptr=Text_index_ptr;
        text_num_array.data_ptr=Text_data_ptr;
        text_num_array.Num_arr_ptr=text_info->nNum_arr_ptr;
        if(number_array_updata==0){
           Update_Number_Array((PnTX_Num_Arr)&text_num_array);
           gplib_ppu_text_number_array_ptr_set(p_register_set, text_index, text_info->nNum_arr_ptr);
           gplib_ppu_text_segment_set(p_register_set, text_index, Text_data_ptr);
        }else{
           if(special_bmp==0){
              gplib_ppu_text_segment_set(p_register_set, text_index, 0); 
              gplib_ppu_text_number_array_ptr_set(p_register_set, text_index, text_info->nNum_arr_ptr);
              gplib_ppu_text_calculate_number_array(p_register_set, text_index, text_x, text_y,Text_data_ptr);
           }
           else
              gplib_ppu_text_segment_set(p_register_set, text_index, Text_data_ptr);    
        }     
     }                 
}

/*******************************************************************************
 * FUNCTION NAME: PPU_MemSet
 * DESCRIPTION: Memory set function for PPU driver
 * INPUT:
 *  - INT32U *pDest             : Destination address pointer
 *  - INT32U uValue             : Value set
 *  - INT32S nBytes             : Byte numbers
 * OUTPUT:
 *  - INT32U *pDest             : Destination data
 * RETURN:
 *  - STATUS_OK                 : Complete this function
 *  - STATUS_FAIL               : Invalid input parameters
 * NOTE:
*  - This function is activated by CPU.
 ******************************************************************************/
INT32U PPU_MemSet (INT32U *pDest, INT32U uValue, INT32S nBytes,INT16U mem_copy_8_16_32)
{
    INT32S  i;
    INT32U  *pPtrDest;
    INT16U  *pPtr16Dest;
    CHAR    *pPtr8Dest;

    pPtrDest = pDest;
    if (!pPtrDest) {
        return STATUS_FAIL;
    }
    if(mem_copy_8_16_32==32){
      //  32-bit memory set
        pPtrDest = pDest;
        for (i=0;i<(nBytes/sizeof (INT32U));i++) {
            *(pPtrDest++) = uValue;
        }
    }else if(mem_copy_8_16_32==16){
      //  16-bit memory set      
        pPtr16Dest = (INT16U *)pDest;
        for (i=0;i<(nBytes/sizeof (INT16U));i++) {
            *(pPtr16Dest++) = uValue;
        }
    }else{
        //  8-bit memory set
        pPtr8Dest = (CHAR *)pPtrDest;
        for (i=0;i<nBytes;i++) {
            *(pPtr8Dest++) = ((uValue&(((INT32U)0xF)<<(i<<3)))>>(i<<3));
        }
    }
    return STATUS_OK;
}