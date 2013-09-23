/*
* Description: This demo is the G+PPUImageConverter Sprite data verification.
*
* Author: Cater Chen
*
* Date: 2009/10/26
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*/
#include "sprite.h"
#include "driver_l1.h"
#include "drv_l1_sfr.h"

/***************** sprite define **********************/
// Define Sprite CDM Mode Enable
#define SP_CMD_COMPARE()             (R_PPU_SPRITE_CTRL & cdm_enable)
#define EXSP_CMD_COMPARE()           (R_PPU_EXTENDSPRITE_CONTROL & excdm_enable)

// Define Max V3D buffer size
#define SP25D_MAX_CHAR_V             8
#define SP25D_MAX_CHAR_H             8

// Define Sprite Coordinate Fraction Mode Enable
#define SP_FRACTION_ENABLE           0
 
// Define Max sprite ram size and buffer size 
#define Spritebuf_number             256
#define Sprite_ptr_number            512
#define SpriteRAM_number             512
#define SpriteExRAM_number           512
#define ExSpritebuf_number           8
#define ExSpriteRAM_number           16

// Define sprite constant
#define spirte_pos_max_bit           0x3FF
#define max_sp_combination_number    160
#define data_end_word                0xFFFF
#define hflip_bit                    0x0004
#define vflip_bit                    0x0008
#define pos_x                        0
#define sp_init_en                   0x8
#define p1024_enable                 0x10
#define spv3d_enable                 0x200
#define pal_control_0                7
#define pal_control_1                15
#define cdm_enable                   0x20000
#define excdm_enable                 0x2
#define sp_frac_x1                   4
#define sp_frac_x2                   8
#define sp_frac_x3                   12
#define exsp_frac_x0                 8
#define exsp_frac_x1                 12
#define sp_frac_y0                   2
#define sp_frac_y1                   6
#define sp_frac_y2                   10
#define sp_frac_y3                   14
#define exsp_frac_y0                 10
#define exsp_frac_y1                 14
#define b_spn_group                  28
#define mask_group_id                0x3
#define sp_cdm_init_en               (0x1<<11)
#define sp_no_cdm_en                 (0x1<<15)
#define b_cdm_exspn_group            12
/****************************************************/

/***************** fs define **********************/
#define O_RDONLY        0x0000
struct stat_t
{
	INT16U	st_mode;
	INT32S	st_size;
	INT32U	st_mtime;
};
//========================================================
//Function Name:	fstat
//Syntax:		INT16S fstat(INT16S handle, struct stat_t *statbuf);
//Purpose:		get file infomation
//Note:			the file must be open
//Parameters:   handle, statbuf
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S fstat(INT16S handle, struct stat_t *statbuf);
//========================================================
//Function Name:	open
//Syntax:		INT16S open(CHAR *path, INT16S open_flag);
//Purpose:		open/creat file
//Note:
//Parameters:   path, open_flag
//Return:		file handle
//=======================================================
extern INT16S open(CHAR *path, INT16S open_flag);
//========================================================
//Function Name:	close
//Syntax:		INT16S close(INT16S fd);
//Purpose:		close file
//Note:
//Parameters:   fd
//Return:		0, SUCCESS
//				-1, FAILE
//=======================================================
extern INT16S close(INT16S fd);
//========================================================
//Function Name:	read
//Syntax:		INT32S read(INT16S fd, INT32U buf, INT32U size);
//Purpose:		read data
//Note:			the buffer is BYTE address, the size is BYTE size
//Parameters:   fd, buf, size
//Return:		really read size
//=======================================================
extern INT32S read(INT16S fd, INT32U buf, INT32U size);

/****************************************************/

// Memory management
extern void * gp_malloc_align(INT32U size, INT32U align);		// SDRAM allocation. Align value must be power of 2, eg: 2, 4, 8, 16, 32...
extern INT32U PPU_MemSet (INT32U *pDest, INT32U uValue, INT32S nBytes,INT16U mem_copy_8_16_32);

// GPlib PPU API 
extern INT32S gplib_ppu_sprite_attribute_25d_position_set(SpN_RAM *out, POS_STRUCT_PTR in);

// Spritebuf and SpriteRAM Memory management
SPRITE	Spritebuf[Spritebuf_number*sizeof(SPRITE)];
SpN_ptr Sprite_ptr[Sprite_ptr_number*sizeof(SpN_ptr)];
SpN_RAM	SpriteRAM[SpriteRAM_number*sizeof(SpN_RAM)];
FP32 HGridBuff[SP25D_MAX_CHAR_V+1][SP25D_MAX_CHAR_H+1];             //V3D Horizontal grid buffer
FP32 VGridBuff[SP25D_MAX_CHAR_V+1][SP25D_MAX_CHAR_H+1];             //V3D Vertical grid buffer
#if (MCU_VERSION >= GPL326XX)
SpN_EX_RAM SpriteExRAM[SpriteExRAM_number*sizeof(SpN_EX_RAM)];
#endif

// Gobel value
INT32U nV_Size,nH_Size;
INT32U ExSpritebuf,Sprite_ex_ptr,ExSpriteRAM,EXSP_NUM;

// Sprite API
void set_sprite_init(INT32U sprite_number,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  
  sprite_pointer=(SPRITE *)sprite_ptr;
  Spritebuf[sprite_number].nSizeX=(INT16S)sprite_pointer->nSizeX;
  Spritebuf[sprite_number].nSizeY=(INT16S)sprite_pointer->nSizeY;
}

void set_sprite_display_init(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  
  sprite_pointer=(SPRITE *)sprite_ptr;
  Spritebuf[sprite_number].SpCell=(INT32U *)sprite_pointer;
  Spritebuf[sprite_number].nPosX=sprite_pos_x;
  Spritebuf[sprite_number].nPosY=sprite_pos_y;
}

void set_sprite_image_number(INT32U sprite_number,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  
  sprite_pointer=(SPRITE *)sprite_ptr;
  Spritebuf[sprite_number].SpCell=(INT32U *)sprite_pointer;
}

void set_sprite_disable(INT32U sprite_number)
{
  Spritebuf[sprite_number].SpCell=0;
}

void SetSpriteFlip(INT32U sprite_number, INT32U nFlip)
{
	int k;
	
	if( Spritebuf[sprite_number].SpCell != 0 )	// check sprite exist/ non-exist
	{
		k = Spritebuf[sprite_number].uAttr0 & 0xfff3;	
		Spritebuf[sprite_number].uAttr0 = k | (nFlip << 2);
	}	 	
}

void SetSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll)
{
	INT16U screen_width, screen_height;
	INT16S xSize, ySize,xmin, xmax, ymin, ymax;
	PSPRITE nSprite;
	
    nSprite=(PSPRITE)Spritebuf;
	if(nSprite->SpCell!= 0) // check sprite exist/non-exist
	{
			Spritebuf[sp_num].nPosX += x;
			Spritebuf[sp_num].nPosY += y;
			if (scroll)
			{
                switch(display_mode)
                {
                    case Sprite_Coordinate_320X240:
                         screen_width = 320;
		                 screen_height = 240 + 14;
                         break;
                    case Sprite_Coordinate_640X480:
       		             screen_width = 640;
		                 screen_height = 480 + 14;
                         break;         
                    case Sprite_Coordinate_480X234:
       		             screen_width = 480;
		                 screen_height = 234;       
                         break;
                    case Sprite_Coordinate_480X272:
                         screen_width = 480;
		                 screen_height = 272;  
                         break;
                    case Sprite_Coordinate_720X480:
       		             screen_width = 720;
		                 screen_height = 480;         
                         break;     
                    case Sprite_Coordinate_800X480:
       		             screen_width = 800;
		                 screen_height = 480;         
                         break;         
                    case Sprite_Coordinate_800X600:
       		             screen_width = 800;
		                 screen_height = 600;         
                         break;
                    case Sprite_Coordinate_1024X768:
       		             screen_width = 1024;
		                 screen_height = 768;         
                         break;
                    default:
                         screen_width = 640;
		                 screen_height = 480 + 14;
                         break;      
                }
				xSize = nH_Size;	// get x size
				ySize = nV_Size;	// get y size
				   xmin = 0 - xSize;
				   xmax = screen_width + xSize;
				   ymin = 0 - ySize;
				   ymax = screen_height + ySize;
				   if (Spritebuf[sp_num].nPosX > xmax)
					   Spritebuf[sp_num].nPosX = xmin;
				   if (Spritebuf[sp_num].nPosY > ymax)
					   Spritebuf[sp_num].nPosY = ymin;
				   if (Spritebuf[sp_num].nPosX < xmin)
					   Spritebuf[sp_num].nPosX = xmax;
				   if (Spritebuf[sp_num].nPosY < ymin)
					   Spritebuf[sp_num].nPosY = ymax;
			}
	}			
}

void Get_sprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr)
{
   sprite_ptr->nSP_CharNum=Sprite_ptr[sprite_number].nSP_CharNum;
   sprite_ptr->nSP_Hsize=Sprite_ptr[sprite_number].nSP_Hsize;
   sprite_ptr->nSP_Vsize=Sprite_ptr[sprite_number].nSP_Vsize;
   sprite_ptr->nSPNum_ptr=Sprite_ptr[sprite_number].nSPNum_ptr;
#if ((MCU_VERSION >= GPL326XX))
   sprite_ptr->nEXSPNum_ptr=Sprite_ptr[sprite_number].nEXSPNum_ptr;
#endif   
}

void paint_ppu_spriteram(PPU_REGISTER_SETS *p_register_set,Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number)
{
   INT16U temp_x,temp_y,cx,cy;
   INT16U uAttribute0,uAttribute1;
   INT32U i,j,sprite_large_size_enable;
   INT32S nPosXN, nPosYN;
   INT32S nScrWidth, nScrHeight;
   PSPRITE nSprite; 
   PSpN_RAM sprite_attr;
#if ((MCU_VERSION >= GPL326XX))
   PSpN_EX_RAM sprite_ex_attr;
#endif    
   PSpN16_CellIdx pSpCell;
   PSpN_ptr  sp_ptr;

   switch(display_mode)
   {
       case Sprite_Coordinate_320X240:
            nScrWidth = 320;
		    nScrHeight = 240 + 14;
            break;
       case Sprite_Coordinate_640X480:
       		nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;         
       case Sprite_Coordinate_480X234:
       		nScrWidth = 480;
		    nScrHeight = 234;       
            break;
       case Sprite_Coordinate_480X272:
            nScrWidth = 480;
		    nScrHeight = 272;  
            break;
       case Sprite_Coordinate_720X480:
       		nScrWidth = 720;
		    nScrHeight = 480;         
            break;     
       case Sprite_Coordinate_800X480:
       		nScrWidth = 800;
		    nScrHeight = 480;         
            break;         
       case Sprite_Coordinate_800X600:
       		nScrWidth = 800;
		    nScrHeight = 600;         
            break;
       case Sprite_Coordinate_1024X768:
       		nScrWidth = 1024;
		    nScrHeight = 768;         
            break;
       default:
            nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;      
    }                      
    sprite_large_size_enable=0;
    sprite_attr=(PSpN_RAM)SpriteRAM;
    PPU_MemSet ((INT32U *)sprite_attr,0,SpriteRAM_number*sizeof(SpN_RAM),32);
    nSprite=(PSPRITE)Spritebuf;
    sp_ptr=(PSpN_ptr)Sprite_ptr;
    PPU_MemSet ((INT32U *)sp_ptr,0,Sprite_ptr_number*sizeof(PSpN_ptr),32);
#if ((MCU_VERSION >= GPL326XX))
    sprite_ex_attr=(PSpN_EX_RAM)SpriteExRAM;
    PPU_MemSet ((INT32U *)sprite_ex_attr,0,SpriteExRAM_number*sizeof(SpN_EX_RAM),32);
#endif    
    for(i=0;i<sprite_number;i++) {// Check sprite number?
	   if(nSprite->SpCell!= 0) { // Check sprite if exist?			  		
		  //sprite ram start address
		  sp_ptr->nSPNum_ptr=(INT32U)sprite_attr;
       #if ((MCU_VERSION >= GPL326XX))
		  //sprite extend ram start address
		  sp_ptr->nEXSPNum_ptr=(INT32U)sprite_ex_attr;
       #endif 		  
		  // MAX Sprite Characters Combination Mode	
		  for(j=0; j<max_sp_combination_number; j++) 
		  {	            				     
			  pSpCell = (PSpN16_CellIdx) nSprite->SpCell+j;
			  if( pSpCell->nCharNumLo == data_end_word ) 
			  {
			        // Check Sprite Characters Combination Flag	
					sp_ptr->nSP_CharNum=j;
					sp_ptr->nSP_Hsize=nH_Size;
			        sp_ptr->nSP_Vsize=nV_Size; 
					break;
			  } 
			  uAttribute0 = 0;
			  uAttribute1 = 0;
			  // Sprite RAM - Character Number
			  sprite_attr->nCharNumLo_16 = pSpCell->nCharNumLo;
			  // SpN Rotate
			  sprite_attr->uPosX_16 = pSpCell->nRotate;
			  // SpN Zoom
			  sprite_attr->uPosY_16 = pSpCell->nZoom;
			  // Sprite RAM - Attribute 0
			  if((p_register_set->ppu_palette_control & p1024_enable) == p1024_enable)
			  {
			    if(pSpCell->nPaletteBank == SP_PBANK3){
			       uAttribute0 |= (1<<pal_control_1);
			       uAttribute1 |= (1<<pal_control_0);
			    }else if(pSpCell->nPaletteBank == SP_PBANK2){
			       uAttribute0 |= (1<<pal_control_1);
			       uAttribute1 |= (0<<pal_control_0);
			    }else if(pSpCell->nPaletteBank == SP_PBANK1){
		           uAttribute0 |= (0<<pal_control_1);
			       uAttribute1 |= (1<<pal_control_0);
		        }else {
		           uAttribute0 |= (0<<pal_control_1);
			       uAttribute1 |= (0<<pal_control_0);
		        }
		      }else{
			    if(pSpCell->nPaletteBank == SP_PBANK3)
			       uAttribute0 |= (1<<pal_control_1);
		        else
		           uAttribute0 |= (0<<pal_control_1);
		      }
		      uAttribute0 |= pSpCell->nBlend;
		      uAttribute0 |= pSpCell->nDepth;
		      uAttribute0 |= pSpCell->nPalette;
		      uAttribute0 |= pSpCell->nVS; 
		      uAttribute0 |= pSpCell->nHS;
		      uAttribute0 |= pSpCell->nFlipV;
		      uAttribute0 |= pSpCell->nFlipH;
		      uAttribute0 |= pSpCell->nColor;  
			  sprite_attr->attr0 = uAttribute0;
		      // Sprite RAM - Attribute 1
		      uAttribute1 |= pSpCell->nCharNumHi;
		      uAttribute1 |= pSpCell->nMosaic;
		      uAttribute1 |= pSpCell->nBldLvl;
		      sprite_attr->attr1 = uAttribute1;
          #if ((MCU_VERSION >= GPL326XX))
		      // Sprite Extend RAM - Attribute 1
		      uAttribute1 = 0;
		      uAttribute1 |= pSpCell->nSPGroup;
		      uAttribute1 |= pSpCell->nSPLargeSize;
		      uAttribute1 |= pSpCell->nSPInterpolation;
			  sprite_ex_attr->ex_attr1 = uAttribute1;
		      // Sprite size compare
		      if(pSpCell->nSPLargeSize==SPLarge_ENABLE)
		      {
		         sprite_large_size_enable=1;
		         //Get sprite H size
		         if(pSpCell->nHS==SP_HSIZE_L32)
		           nH_Size=32;
		         else if(pSpCell->nHS==SP_HSIZE_L64)
		           nH_Size=64;
		         else if(pSpCell->nHS==SP_HSIZE_L128)  
			       nH_Size=128;
		         else if(pSpCell->nHS==SP_HSIZE_L256)  
			       nH_Size=256;
			     //Get sprite V size  
			     if(pSpCell->nVS==SP_VSIZE_L32)
		           nV_Size=32;
		         else if(pSpCell->nVS==SP_VSIZE_L64)
		           nV_Size=64;
		         else if(pSpCell->nVS==SP_VSIZE_L128)  
			       nV_Size=128;
		         else if(pSpCell->nVS==SP_VSIZE_L256)  
			       nV_Size=256;
			  }    			  
          #endif
              if(sprite_large_size_enable == 0)
              {			  
		          //Get sprite H size
		          if(pSpCell->nHS==SP_HSIZE8)
		            nH_Size=8;
		          else if(pSpCell->nHS==SP_HSIZE16)
		            nH_Size=16;
		          else if(pSpCell->nHS==SP_HSIZE32)  
			        nH_Size=32;
		          else if(pSpCell->nHS==SP_HSIZE64)  
			        nH_Size=64;
			      //Get sprite V size  
			      if(pSpCell->nVS==SP_VSIZE8)
		            nV_Size=8;
		          else if(pSpCell->nVS==SP_VSIZE16)
		            nV_Size=16;
		          else if(pSpCell->nVS==SP_VSIZE32)  
			        nV_Size=32;
		          else if(pSpCell->nVS==SP_VSIZE64)  
			        nV_Size=64; 
			  }  
			  temp_x=pSpCell->nPosX;
			  temp_y=pSpCell->nPosY;
			  if((p_register_set->ppu_enable & spv3d_enable) == spv3d_enable)
			  {
			     cx = (nSprite->nSizeX & spirte_pos_max_bit) - nH_Size;  // get maximum x position
                 cy = nV_Size - (nSprite->nSizeY & spirte_pos_max_bit);  // get minimum y position
                 if((nSprite->uAttr0 & hflip_bit) != 0)          // HFlip 						
                    temp_x = cx - temp_x;					
                 if((nSprite->uAttr0 & vflip_bit) != 0)          // VFlip				
                    temp_y = cy - temp_y;
              }    
			  // coordinates change  			     
			  if (coordinate_mode == Center2LeftTop_coordinate) 
			  {
				 // Center2LeftTop_coordinate
				 nPosXN = (nScrWidth>>1) + (nH_Size>>1) + 1 + nSprite->nPosX + temp_x;
				 //nPosXN = (nScrWidth>>1) + temp_x;
				 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY + temp_y - (nV_Size>>1);	                               
                 //nPosYN = (nScrHeight>>1) - temp_y; 				            
			  } 
			  else if (coordinate_mode == LeftTop2Center_coordinate)
			  {
				 // LeftTop2Center_coordinate
				 nPosXN = nSprite->nPosX + temp_x - (nScrWidth>>1) + (nH_Size>>1) + 1;
				 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1);
			  } 
			  else
			  {
			  	 // PPU_hardware_coordinate
			  	 // fix x, y the origin of the coordinates by hardware
				 nPosXN = nSprite->nPosX + temp_x;
				 nPosYN = nSprite->nPosY + temp_y;
			  } 			    
			  // Sprite RAM - Rotate / Position X
			  sprite_attr->uPosX_16 |= (nPosXN & spirte_pos_max_bit);
			  // Sprite RAM - Zoom / Position Y
			  sprite_attr->uPosY_16 |= (nPosYN & spirte_pos_max_bit);
			  sprite_attr++;
			  if((p_register_set->sprite_control & cdm_enable) == cdm_enable)
			    sprite_attr++;
          #if ((MCU_VERSION >= GPL326XX))
			  sprite_ex_attr++;			  
          #endif			  
	      }                  
	   }
	 nSprite++;
	 sp_ptr++;	  
   }
   p_register_set->update_register_flag |= C_UPDATE_REG_SET_SPRITE_ATTRIBUTE;
#if (MCU_VERSION >= GPL326XX)
   p_register_set->update_register_flag |= C_UPDATE_REG_SET_SPRITE_EX_ATTRIBUTE;
#endif    	        
}

// Sprite Bin API
void load_sprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN)
{
    INT16U *p;
    INT32U addr0,addr;
    INT32U sprite_idx_start,sprite_offset,sprite_image_num;
    SPRITE *sprite_pointer;
    //parse header for sprite.bin
    sprite_idx_start = (INT32U)SPRITE_ATX_BIN;
    p=(INT16U *)(sprite_idx_start+(sp_num*sizeof(SPRITE_BIN_HEADER_L)));
	sprite_image_num = *(p+1);
	addr0 = (*(p+2)<<1);
	addr = (*(p+3)<<1);
	addr0 += addr << 16;
	//find the SPRITE start address 
	sprite_pointer =(SPRITE *)(sprite_idx_start + addr0);
    Spritebuf[display_sp_num].nSizeX=(INT16S)sprite_pointer->nSizeX;
    Spritebuf[display_sp_num].nSizeY=(INT16S)sprite_pointer->nSizeY;
	//find the SPRITE.SpCell start address
	sprite_offset=(INT32U)sprite_pointer->SpCell;
	sprite_idx_start+=sprite_offset<<1;
	sprite_pointer=(SPRITE *)sprite_idx_start;
	//SPRITE.SpCell pointer for sprite buffer
	Spritebuf[display_sp_num].SpCell=(INT32U *)sprite_pointer;
	Spritebuf[display_sp_num].nPosX=sp_pos_x;
    Spritebuf[display_sp_num].nPosY=sp_pos_y;	 
}

void set_sprite_image_number_bin(INT16U display_sp_num,INT16U sp_num,INT16U sp_image_num,INT32U* SPRITE_ATX_BIN)
{
    INT16U *p;
    INT32U addr0,addr;
    INT32U sprite_idx_start,sprite_offset,sprite_image_num;
    SPRITE *sprite_pointer;
    
    //parse header for sprite.bin
    sprite_idx_start = (INT32U)SPRITE_ATX_BIN;
    p=(INT16U *)(sprite_idx_start+(sp_num*sizeof(SPRITE_BIN_HEADER_L)));
	sprite_image_num = *(p+1);
	if((sp_image_num == 0) || (sp_image_num > sprite_image_num))
	   sp_image_num=1;     
	addr0 = (*(p+2)<<1);
	addr = (*(p+3)<<1);
	addr0 += addr << 16;
	//find the SPRITE start address 
	sprite_pointer =(SPRITE *)(sprite_idx_start + addr0);
	//check of sprute image number 
	sprite_pointer+=(sp_image_num-1);
    //find the SPRITE.SpCell start address
	sprite_offset=(INT32U)sprite_pointer->SpCell;
	sprite_idx_start+=sprite_offset<<1; 
	sprite_pointer=(SPRITE *)sprite_idx_start;
	//SPRITE.SpCell pointer for sprite buffer
	Spritebuf[display_sp_num].SpCell=(INT32U *)sprite_pointer;
}


// Load FILE API
INT32U* LoadFILE_BIN(INT8U* pFileName)
 {
	INT16S fd,ret;
#if (_OPERATING_SYSTEM == _OS_NONE)	
	INT32U i;
#endif	
	INT32U* SaveAddr;
	struct stat_t statetest;
	
	//TimeDly;
	#if (_OPERATING_SYSTEM == _OS_NONE)
	   for(ret = 0 ; ret < 10000 ; ret++) {
	      i = 1;
	   }
	#else
	   OSTimeDly(5);
	#endif
	//open file 
	fd = open((CHAR *)pFileName, O_RDONLY);
	//get file status
	fstat(fd, &statetest);
	//malloc memory for sdram
	SaveAddr = (INT32U *)gp_malloc_align( statetest.st_size, 4);
	//fs init success 
	if (fd >= 0)
	{
		ret = read(fd, (INT32U)(SaveAddr), statetest.st_size);
		close(fd);
	}
	else
	{
		#if (_OPERATING_SYSTEM == _OS_NONE)
		   while(1);
		#else
		   while(1)
		     OSTimeDly(1);
		#endif
	}
	return SaveAddr;
}

// Extended Sprite API
void set_exsprite_init(INT32U sprite_number,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  SPRITE *sprite_buf_pointer;
  
  sprite_pointer=(SPRITE *)sprite_ptr;
  sprite_buf_pointer=(SPRITE *)(ExSpritebuf+sprite_number*sizeof(SPRITE)); 
  sprite_buf_pointer->nSizeX=(INT16S)sprite_pointer->nSizeX;
  sprite_buf_pointer->nSizeY=(INT16S)sprite_pointer->nSizeY;
}

void set_exsprite_image_number(INT32U sprite_number,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  SPRITE *sprite_buf_pointer;
    
  sprite_pointer=(SPRITE *)sprite_ptr;
  sprite_buf_pointer=(SPRITE *)(ExSpritebuf+sprite_number*sizeof(SPRITE));
  sprite_buf_pointer->SpCell=(INT32U *)sprite_pointer;
}

void set_exsprite_display_init(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y,INT32U sprite_ptr)
{
  SPRITE *sprite_pointer;
  SPRITE *sprite_buf_pointer;
  
  sprite_pointer=(SPRITE *)sprite_ptr;
  sprite_buf_pointer=(SPRITE *)(ExSpritebuf+sprite_number*sizeof(SPRITE));
  sprite_buf_pointer->SpCell=(INT32U *)sprite_pointer;
  sprite_buf_pointer->nPosX=sprite_pos_x;
  sprite_buf_pointer->nPosY=sprite_pos_y;  
}

void Get_exsprite_image_info(INT32U sprite_number,SpN_ptr *sprite_ptr)
{
   SpN_ptr *sprite_buf_pointer;
   
   sprite_buf_pointer=(SpN_ptr *)(Sprite_ex_ptr+sprite_number*sizeof(SpN_ptr));
   sprite_ptr->nSP_CharNum=sprite_buf_pointer->nSP_CharNum;
   sprite_ptr->nSP_Hsize=sprite_buf_pointer->nSP_Hsize;
   sprite_ptr->nSP_Vsize=sprite_buf_pointer->nSP_Vsize;
   sprite_ptr->nSPNum_ptr=sprite_buf_pointer->nSPNum_ptr;
 #if ((MCU_VERSION >= GPL326XX))
   sprite_ptr->nEXSPNum_ptr=sprite_buf_pointer->nEXSPNum_ptr;
 #endif  
}

void set_exsprite_disable(INT32U sprite_number)
{
  SPRITE *sprite_buf_pointer;
  
  sprite_buf_pointer=(SPRITE *)(ExSpritebuf+sprite_number*sizeof(SPRITE));  
  sprite_buf_pointer->SpCell=0;
}

void SetEXSpritePosition(INT16U sp_num,Sprite_Coordinate_Mode display_mode, INT16S x, INT16S y, INT16U scroll)
{
	INT16U screen_width, screen_height;
	INT16S xSize, ySize,xmin, xmax, ymin, ymax;
	PSPRITE nSprite;
    
    nSprite=(PSPRITE)(ExSpritebuf+sp_num*sizeof(SPRITE));
	if(nSprite->SpCell!= 0) // check sprite exist/non-exist
	{
			nSprite->nPosX += x;
			nSprite->nPosY += y;
			if (scroll)
			{
                switch(display_mode)
                {
                    case Sprite_Coordinate_320X240:
                         screen_width = 320;
		                 screen_height = 240 + 14;
                         break;
                    case Sprite_Coordinate_640X480:
       		             screen_width = 640;
		                 screen_height = 480 + 14;
                         break;         
                    case Sprite_Coordinate_480X234:
       		             screen_width = 480;
		                 screen_height = 234;       
                         break;
                    case Sprite_Coordinate_480X272:
                         screen_width = 480;
		                 screen_height = 272;  
                         break;
                    case Sprite_Coordinate_720X480:
       		             screen_width = 720;
		                 screen_height = 480;         
                         break;     
                    case Sprite_Coordinate_800X480:
       		             screen_width = 800;
		                 screen_height = 480;         
                         break;         
                    case Sprite_Coordinate_800X600:
       		             screen_width = 800;
		                 screen_height = 600;         
                         break;
                    case Sprite_Coordinate_1024X768:
       		             screen_width = 1024;
		                 screen_height = 768;         
                         break;
                    default:
                         screen_width = 640;
		                 screen_height = 480 + 14;
                         break;      
                }
				xSize = nH_Size;	// get x size
				ySize = nV_Size;	// get y size
				   xmin = 0 - xSize;
				   xmax = screen_width + xSize;
				   ymin = 0 - ySize;
				   ymax = screen_height + ySize;
				   if (nSprite->nPosX > xmax)
					   nSprite->nPosX = xmin;
				   if (nSprite->nPosY > ymax)
					   nSprite->nPosY = ymin;
				   if (nSprite->nPosX < xmin)
					   nSprite->nPosX = xmax;
				   if (nSprite->nPosY < ymin)
					   nSprite->nPosY = ymax;
			}
	}			
}

void paint_ppu_exspriteram(Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number)
{
   INT16U temp_x,temp_y,cx,cy;
   INT16U uAttribute0,uAttribute1;
   INT32U i,j,sprite_large_size_enable;
   INT32S nPosXN, nPosYN;
   INT32S nScrWidth, nScrHeight;
   PSPRITE nSprite;           
   PSpN_RAM sprite_cdm_attr;    
   PSpN16_CellIdx pSpCell;
   PSpN_ptr  sp_ptr;

   switch(display_mode)
   {
       case Sprite_Coordinate_320X240:
            nScrWidth = 320;
		    nScrHeight = 240 + 14;
            break;
       case Sprite_Coordinate_640X480:
       		nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;         
       case Sprite_Coordinate_480X234:
       		nScrWidth = 480;
		    nScrHeight = 234;       
            break;
       case Sprite_Coordinate_480X272:
            nScrWidth = 480;
		    nScrHeight = 272;  
            break;
       case Sprite_Coordinate_720X480:
       		nScrWidth = 720;
		    nScrHeight = 480;         
            break;     
       case Sprite_Coordinate_800X480:
       		nScrWidth = 800;
		    nScrHeight = 480;         
            break;         
       case Sprite_Coordinate_800X600:
       		nScrWidth = 800;
		    nScrHeight = 600;         
            break;
       case Sprite_Coordinate_1024X768:
       		nScrWidth = 1024;
		    nScrHeight = 768;         
            break;
       default:
            nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;      
    }                      
    sprite_large_size_enable=0;     
	sprite_cdm_attr=(PSpN_RAM)ExSpriteRAM;
	if(sprite_cdm_attr!=0)
	   PPU_MemSet ((INT32U *)sprite_cdm_attr,0,EXSP_NUM*2*sizeof(SpN_RAM),32);    
    nSprite=(PSPRITE)ExSpritebuf;   
    sp_ptr=(PSpN_ptr)Sprite_ex_ptr;
    if(sp_ptr!=0)
      PPU_MemSet ((INT32U *)sp_ptr,0,EXSP_NUM*sizeof(PSpN_ptr),32);  
    if(EXSP_CMD_COMPARE())
    {
	    for(i=0;i<sprite_number;i++) {// Check sprite number?
		   if(nSprite->SpCell!= 0) { // Check sprite if exist?			  		
			  //sprite ram start address
			  sp_ptr->nSPNum_ptr=(INT32U)sprite_cdm_attr; 		  
			  // MAX Sprite Characters Combination Mode	
			  for(j=0; j<max_sp_combination_number; j++) 
			  {	            				     
				  pSpCell = (PSpN16_CellIdx) nSprite->SpCell+j;
				  if( pSpCell->nCharNumLo == data_end_word ) 
				  {
				        // Check Sprite Characters Combination Flag	
						sp_ptr->nSP_CharNum=j;
						sp_ptr->nSP_Hsize=nH_Size;
				        sp_ptr->nSP_Vsize=nV_Size; 
						break;
				  } 
				  uAttribute0 = 0;
				  uAttribute1 = 0;
				  // Sprite RAM - Character Number
				  sprite_cdm_attr->nCharNumLo_16 = pSpCell->nCharNumLo;
				  // SpN Rotate
				  sprite_cdm_attr->uPosX_16 = pSpCell->nRotate;
				  // SpN Zoom
				  sprite_cdm_attr->uPosY_16 = pSpCell->nZoom;
				  // Sprite RAM - Attribute 0
				  if((R_PPU_PALETTE_CTRL & p1024_enable) == p1024_enable)
				  {
				    if(pSpCell->nPaletteBank == SP_PBANK3){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK2){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK1){
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
			        }else {
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
			        }
			      }else{
				    if(pSpCell->nPaletteBank == SP_PBANK3)
				       uAttribute0 |= (1<<pal_control_1);
			        else
			           uAttribute0 |= (0<<pal_control_1);
			      }
			      uAttribute0 |= pSpCell->nBlend;
			      uAttribute0 |= pSpCell->nDepth;
			      uAttribute0 |= pSpCell->nPalette;
			      uAttribute0 |= pSpCell->nVS; 
			      uAttribute0 |= pSpCell->nHS;
			      uAttribute0 |= pSpCell->nFlipV;
			      uAttribute0 |= pSpCell->nFlipH;
			      uAttribute0 |= pSpCell->nColor;  
				  sprite_cdm_attr->attr0 = uAttribute0;
			      // Sprite RAM - Attribute 1
			      uAttribute1 |= pSpCell->nCharNumHi;
			      uAttribute1 |= pSpCell->nMosaic;
			      uAttribute1 |= pSpCell->nBldLvl;
			      sprite_cdm_attr->attr1 = uAttribute1;
	          #if (MCU_VERSION >= GPL326XX)
			      // Sprite Extend RAM - Attribute 1
			      sprite_cdm_attr++;
			      uAttribute1 = 0;
			      uAttribute1 |= pSpCell->nSPGroup;
			         
			      sprite_cdm_attr->uPosX_16 |= uAttribute1;
			      uAttribute1 |= pSpCell->nSPLargeSize;
			      if(uAttribute1==SPLarge_DISABLE)
			        uAttribute1 = 0;
			      else
			        uAttribute1 = 0x4000;
			      sprite_cdm_attr->uPosX_16 |= uAttribute1;
			      uAttribute1 |= pSpCell->nSPInterpolation;
			      if(uAttribute1==SPInterpolation_DISABLE)
			        uAttribute1 = 0;
			      else
			        uAttribute1 = 0x800;			      
				  sprite_cdm_attr->uPosX_16 |= uAttribute1;
				  sprite_cdm_attr--;
			      // Sprite size compare
			      if(pSpCell->nSPLargeSize==SPLarge_ENABLE)
			      {
			         sprite_large_size_enable=1;
			         //Get sprite H size
			         if(pSpCell->nHS==SP_HSIZE_L32)
			           nH_Size=32;
			         else if(pSpCell->nHS==SP_HSIZE_L64)
			           nH_Size=64;
			         else if(pSpCell->nHS==SP_HSIZE_L128)  
				       nH_Size=128;
			         else if(pSpCell->nHS==SP_HSIZE_L256)  
				       nH_Size=256;
				     //Get sprite V size  
				     if(pSpCell->nVS==SP_VSIZE_L32)
			           nV_Size=32;
			         else if(pSpCell->nVS==SP_VSIZE_L64)
			           nV_Size=64;
			         else if(pSpCell->nVS==SP_VSIZE_L128)  
				       nV_Size=128;
			         else if(pSpCell->nVS==SP_VSIZE_L256)  
				       nV_Size=256;
				  }    			  
	          #endif
	              if(sprite_large_size_enable == 0)
	              {			  
			          //Get sprite H size
			          if(pSpCell->nHS==SP_HSIZE8)
			            nH_Size=8;
			          else if(pSpCell->nHS==SP_HSIZE16)
			            nH_Size=16;
			          else if(pSpCell->nHS==SP_HSIZE32)  
				        nH_Size=32;
			          else if(pSpCell->nHS==SP_HSIZE64)  
				        nH_Size=64;
				      //Get sprite V size  
				      if(pSpCell->nVS==SP_VSIZE8)
			            nV_Size=8;
			          else if(pSpCell->nVS==SP_VSIZE16)
			            nV_Size=16;
			          else if(pSpCell->nVS==SP_VSIZE32)  
				        nV_Size=32;
			          else if(pSpCell->nVS==SP_VSIZE64)  
				        nV_Size=64; 
				  }  
				  temp_x=pSpCell->nPosX;
				  temp_y=pSpCell->nPosY;
				  if((R_PPU_ENABLE & spv3d_enable) == spv3d_enable)
				  {
				     cx = (nSprite->nSizeX & spirte_pos_max_bit) - nH_Size;  // get maximum x position
	                 cy = nV_Size - (nSprite->nSizeY & spirte_pos_max_bit);  // get minimum y position
	                 if((nSprite->uAttr0 & hflip_bit) != 0)          // HFlip 						
	                    temp_x = cx - temp_x;					
	                 if((nSprite->uAttr0 & vflip_bit) != 0)          // VFlip				
	                    temp_y = cy - temp_y;
	              }    
				  // coordinates change  			     
				  if (coordinate_mode == Center2LeftTop_coordinate) 
				  {
					 // Center2LeftTop_coordinate
					 nPosXN = (nScrWidth>>1) + (nH_Size>>1) + 1 + nSprite->nPosX + temp_x;
					 //nPosXN = (nScrWidth>>1) + temp_x;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY + temp_y - (nV_Size>>1);	                               
	                 //nPosYN = (nScrHeight>>1) - temp_y; 				            
				  } 
				  else if (coordinate_mode == LeftTop2Center_coordinate)
				  {
					 // LeftTop2Center_coordinate
					 nPosXN = nSprite->nPosX + temp_x - (nScrWidth>>1) + (nH_Size>>1) + 1;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1);
				  } 
				  else
				  {
				  	 // PPU_hardware_coordinate
				  	 // fix x, y the origin of the coordinates by hardware
					 nPosXN = nSprite->nPosX + temp_x;
					 nPosYN = nSprite->nPosY + temp_y;
				  }   
				  // Sprite RAM - Rotate / Position X
				  sprite_cdm_attr->uPosX_16 |= (nPosXN & spirte_pos_max_bit);
				  // Sprite RAM - Zoom / Position Y
				  sprite_cdm_attr->uPosY_16 |= (nPosYN & spirte_pos_max_bit);
				  sprite_cdm_attr++;
                  #if ((MCU_VERSION >= GPL326XX))
                     sprite_cdm_attr->uPosX_16|=sp_no_cdm_en;
                  #endif
                  sprite_cdm_attr++;				   				  
		      }                  
		   }
		 nSprite++;
		 sp_ptr++;	  
	   }
    }
    else
    {
      #if (MCU_VERSION >= GPL326XX)
	    for(i=0;i<sprite_number;i++) {// Check sprite number?
		   if(nSprite->SpCell!= 0) { // Check sprite if exist?			  		
			  //sprite ram start address
			  sp_ptr->nSPNum_ptr=(INT32U)sprite_cdm_attr; 		  
			  // MAX Sprite Characters Combination Mode	
			  for(j=0; j<max_sp_combination_number; j++) 
			  {	            				     
				  pSpCell = (PSpN16_CellIdx) nSprite->SpCell+j;
				  if( pSpCell->nCharNumLo == data_end_word ) 
				  {
				        // Check Sprite Characters Combination Flag	
						sp_ptr->nSP_CharNum=j;
						sp_ptr->nSP_Hsize=nH_Size;
				        sp_ptr->nSP_Vsize=nV_Size; 
						break;
				  } 
				  uAttribute0 = 0;
				  uAttribute1 = 0;
				  // Sprite RAM - Character Number
				  sprite_cdm_attr->nCharNumLo_16 = pSpCell->nCharNumLo;
				  // SpN Rotate
				  sprite_cdm_attr->uPosX_16 = pSpCell->nRotate;
				  // SpN Zoom
				  sprite_cdm_attr->uPosY_16 = pSpCell->nZoom;
				  // Sprite RAM - Attribute 0
				  if((R_PPU_PALETTE_CTRL & p1024_enable) == p1024_enable)
				  {
				    if(pSpCell->nPaletteBank == SP_PBANK3){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK2){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK1){
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
			        }else {
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
			        }
			      }else{
				    if(pSpCell->nPaletteBank == SP_PBANK3)
				       uAttribute0 |= (1<<pal_control_1);
			        else
			           uAttribute0 |= (0<<pal_control_1);
			      }
			      uAttribute0 |= pSpCell->nBlend;
			      uAttribute0 |= pSpCell->nDepth;
			      uAttribute0 |= pSpCell->nPalette;
			      uAttribute0 |= pSpCell->nVS; 
			      uAttribute0 |= pSpCell->nHS;
			      uAttribute0 |= pSpCell->nFlipV;
			      uAttribute0 |= pSpCell->nFlipH;
			      uAttribute0 |= pSpCell->nColor;  
				  sprite_cdm_attr->attr0 = uAttribute0;
			      // Sprite RAM - Attribute 1
			      uAttribute1 |= pSpCell->nCharNumHi;
			      uAttribute1 |= pSpCell->nMosaic;
			      uAttribute1 |= pSpCell->nBldLvl;
			      sprite_cdm_attr->attr1 = uAttribute1;
	          #if (MCU_VERSION >= GPL326XX)
			      // Sprite Extend RAM - Attribute 1
			      sprite_cdm_attr++;
			      uAttribute1 = 0;
		          uAttribute1 |= pSpCell->nSPGroup;
		          uAttribute1 |= pSpCell->nSPLargeSize;
		          uAttribute1 |= pSpCell->nSPInterpolation;			      
				  sprite_cdm_attr->nCharNumLo_16 |= uAttribute1;
				  sprite_cdm_attr--;
			      // Sprite size compare
			      if(pSpCell->nSPLargeSize==SPLarge_ENABLE)
			      {
			         sprite_large_size_enable=1;
			         //Get sprite H size
			         if(pSpCell->nHS==SP_HSIZE_L32)
			           nH_Size=32;
			         else if(pSpCell->nHS==SP_HSIZE_L64)
			           nH_Size=64;
			         else if(pSpCell->nHS==SP_HSIZE_L128)  
				       nH_Size=128;
			         else if(pSpCell->nHS==SP_HSIZE_L256)  
				       nH_Size=256;
				     //Get sprite V size  
				     if(pSpCell->nVS==SP_VSIZE_L32)
			           nV_Size=32;
			         else if(pSpCell->nVS==SP_VSIZE_L64)
			           nV_Size=64;
			         else if(pSpCell->nVS==SP_VSIZE_L128)  
				       nV_Size=128;
			         else if(pSpCell->nVS==SP_VSIZE_L256)  
				       nV_Size=256;
				  }    			  
	          #endif
	              if(sprite_large_size_enable == 0)
	              {			  
			          //Get sprite H size
			          if(pSpCell->nHS==SP_HSIZE8)
			            nH_Size=8;
			          else if(pSpCell->nHS==SP_HSIZE16)
			            nH_Size=16;
			          else if(pSpCell->nHS==SP_HSIZE32)  
				        nH_Size=32;
			          else if(pSpCell->nHS==SP_HSIZE64)  
				        nH_Size=64;
				      //Get sprite V size  
				      if(pSpCell->nVS==SP_VSIZE8)
			            nV_Size=8;
			          else if(pSpCell->nVS==SP_VSIZE16)
			            nV_Size=16;
			          else if(pSpCell->nVS==SP_VSIZE32)  
				        nV_Size=32;
			          else if(pSpCell->nVS==SP_VSIZE64)  
				        nV_Size=64; 
				  }  
				  temp_x=pSpCell->nPosX;
				  temp_y=pSpCell->nPosY;
				  if((R_PPU_ENABLE & spv3d_enable) == spv3d_enable)
				  {
				     cx = (nSprite->nSizeX & spirte_pos_max_bit) - nH_Size;  // get maximum x position
	                 cy = nV_Size - (nSprite->nSizeY & spirte_pos_max_bit);  // get minimum y position
	                 if((nSprite->uAttr0 & hflip_bit) != 0)          // HFlip 						
	                    temp_x = cx - temp_x;					
	                 if((nSprite->uAttr0 & vflip_bit) != 0)          // VFlip				
	                    temp_y = cy - temp_y;
	              }    
				  // coordinates change  			     
				  if (coordinate_mode == Center2LeftTop_coordinate) 
				  {
					 // Center2LeftTop_coordinate
					 nPosXN = (nScrWidth>>1) + (nH_Size>>1) + 1 + nSprite->nPosX + temp_x;
					 //nPosXN = (nScrWidth>>1) + temp_x;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY + temp_y - (nV_Size>>1);	                               
	                 //nPosYN = (nScrHeight>>1) - temp_y; 				            
				  } 
				  else if (coordinate_mode == LeftTop2Center_coordinate)
				  {
					 // LeftTop2Center_coordinate
					 nPosXN = nSprite->nPosX + temp_x - (nScrWidth>>1) + (nH_Size>>1) + 1;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1);
				  } 
				  else
				  {
				  	 // PPU_hardware_coordinate
				  	 // fix x, y the origin of the coordinates by hardware
					 nPosXN = nSprite->nPosX + temp_x;
					 nPosYN = nSprite->nPosY + temp_y;
				  }   
				  // Sprite RAM - Rotate / Position X
				  sprite_cdm_attr->uPosX_16 |= (nPosXN & spirte_pos_max_bit);
				  // Sprite RAM - Zoom / Position Y
				  sprite_cdm_attr->uPosY_16 |= (nPosYN & spirte_pos_max_bit);
				  sprite_cdm_attr++;
                  #if ((MCU_VERSION >= GPL326XX))
                     sprite_cdm_attr->uPosX_16|=sp_no_cdm_en;
                  #endif				  				  
				  sprite_cdm_attr++;  				  
		      }                  
		   }
		 nSprite++;
		 sp_ptr++;	  
	   }	  	    
	  #else
	    for(i=0;i<sprite_number;i++) {// Check sprite number?
		   if(nSprite->SpCell!= 0) { // Check sprite if exist?			  		
			  //sprite ram start address
			  sp_ptr->nSPNum_ptr=(INT32U)sprite_cdm_attr; 		  
			  // MAX Sprite Characters Combination Mode	
			  for(j=0; j<max_sp_combination_number; j++) 
			  {	            				     
				  pSpCell = (PSpN16_CellIdx) nSprite->SpCell+j;
				  if( pSpCell->nCharNumLo == data_end_word ) 
				  {
				        // Check Sprite Characters Combination Flag	
						sp_ptr->nSP_CharNum=j;
						sp_ptr->nSP_Hsize=nH_Size;
				        sp_ptr->nSP_Vsize=nV_Size; 
						break;
				  } 
				  uAttribute0 = 0;
				  uAttribute1 = 0;
				  // Sprite RAM - Character Number
				  sprite_cdm_attr->nCharNumLo_16 = pSpCell->nCharNumLo;
				  // SpN Rotate
				  sprite_cdm_attr->uPosX_16 = pSpCell->nRotate;
				  // SpN Zoom
				  sprite_cdm_attr->uPosY_16 = pSpCell->nZoom;
				  // Sprite RAM - Attribute 0
				  if((R_PPU_PALETTE_CTRL & p1024_enable) == p1024_enable)
				  {
				    if(pSpCell->nPaletteBank == SP_PBANK3){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK2){
				       uAttribute0 |= (1<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
				    }else if(pSpCell->nPaletteBank == SP_PBANK1){
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (1<<pal_control_0);
			        }else {
			           uAttribute0 |= (0<<pal_control_1);
				       uAttribute1 |= (0<<pal_control_0);
			        }
			      }else{
				    if(pSpCell->nPaletteBank == SP_PBANK3)
				       uAttribute0 |= (1<<pal_control_1);
			        else
			           uAttribute0 |= (0<<pal_control_1);
			      }
			      uAttribute0 |= pSpCell->nBlend;
			      uAttribute0 |= pSpCell->nDepth;
			      uAttribute0 |= pSpCell->nPalette;
			      uAttribute0 |= pSpCell->nVS; 
			      uAttribute0 |= pSpCell->nHS;
			      uAttribute0 |= pSpCell->nFlipV;
			      uAttribute0 |= pSpCell->nFlipH;
			      uAttribute0 |= pSpCell->nColor;  
				  sprite_cdm_attr->attr0 = uAttribute0;
			      // Sprite RAM - Attribute 1
			      uAttribute1 |= pSpCell->nCharNumHi;
			      uAttribute1 |= pSpCell->nMosaic;
			      uAttribute1 |= pSpCell->nBldLvl;
			      sprite_cdm_attr->attr1 = uAttribute1; 		      
			      // Sprite size compare    			  		  
		          // Get sprite H size
		          if(pSpCell->nHS==SP_HSIZE8)
		            nH_Size=8;
		          else if(pSpCell->nHS==SP_HSIZE16)
		            nH_Size=16;
		          else if(pSpCell->nHS==SP_HSIZE32)  
			        nH_Size=32;
		          else if(pSpCell->nHS==SP_HSIZE64)  
			        nH_Size=64;
			      // Get sprite V size  
			      if(pSpCell->nVS==SP_VSIZE8)
		            nV_Size=8;
		          else if(pSpCell->nVS==SP_VSIZE16)
		            nV_Size=16;
		          else if(pSpCell->nVS==SP_VSIZE32)  
			        nV_Size=32;
		          else if(pSpCell->nVS==SP_VSIZE64)  
			        nV_Size=64;   
				  temp_x=pSpCell->nPosX;
				  temp_y=pSpCell->nPosY;
				  if((R_PPU_ENABLE & spv3d_enable) == spv3d_enable)
				  {
				     cx = (nSprite->nSizeX & spirte_pos_max_bit) - nH_Size;  // get maximum x position
	                 cy = nV_Size - (nSprite->nSizeY & spirte_pos_max_bit);  // get minimum y position
	                 if((nSprite->uAttr0 & hflip_bit) != 0)          // HFlip 						
	                    temp_x = cx - temp_x;					
	                 if((nSprite->uAttr0 & vflip_bit) != 0)          // VFlip				
	                    temp_y = cy - temp_y;
	              }    
				  // coordinates change  			     
				  if (coordinate_mode == Center2LeftTop_coordinate) 
				  {
					 // Center2LeftTop_coordinate
					 nPosXN = (nScrWidth>>1) + (nH_Size>>1) + 1 + nSprite->nPosX + temp_x;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY + temp_y - (nV_Size>>1);	                               				            
				  } 
				  else if (coordinate_mode == LeftTop2Center_coordinate)
				  {
					 // LeftTop2Center_coordinate
					 nPosXN = nSprite->nPosX + temp_x - (nScrWidth>>1) + (nH_Size>>1) + 1;
					 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1);
				  } 
				  else
				  {
				  	 // PPU_hardware_coordinate
				  	 // fix x, y the origin of the coordinates by hardware
					 nPosXN = nSprite->nPosX + temp_x;
					 nPosYN = nSprite->nPosY + temp_y;
				  }   
				  // Sprite RAM - Rotate / Position X
				  sprite_cdm_attr->uPosX_16 |= (nPosXN & spirte_pos_max_bit);
				  // Sprite RAM - Zoom / Position Y
				  sprite_cdm_attr->uPosY_16 |= (nPosYN & spirte_pos_max_bit);
				  sprite_cdm_attr++;
		      }                  
		   }
		 nSprite++;
		 sp_ptr++;	  
	   }	   
	  #endif   
    }       
}

INT32U extend_sprite_malloc_memory(INT16U exsp_num)
{
	//malloc memory for sdram
	EXSP_NUM=exsp_num;
	ExSpriteRAM = (INT32U)gp_malloc_align(EXSP_NUM*2*sizeof(SpN_RAM),16);
	PPU_MemSet((INT32U *)ExSpriteRAM,0,EXSP_NUM*2*sizeof(SpN_RAM),32);       
    Sprite_ex_ptr = (INT32U)gp_malloc_align(EXSP_NUM*sizeof(SpN_EX_RAM),4);
    PPU_MemSet((INT32U *)Sprite_ex_ptr,0,EXSP_NUM*sizeof(SpN_EX_RAM),32);
    ExSpritebuf = (INT32U)gp_malloc_align(EXSP_NUM*sizeof(SPRITE),4);
    PPU_MemSet((INT32U *)ExSpritebuf,0,EXSP_NUM*sizeof(SPRITE),32);
    
	return ExSpriteRAM;
}
// Extend Sprite Bin API
void load_exsprite_bin(INT16U display_sp_num,INT16U sp_num,INT16S sp_pos_x,INT16S sp_pos_y,INT32U* SPRITE_ATX_BIN)
{
    INT16U *p;
    INT32U addr0,addr;
    INT32U sprite_idx_start,sprite_offset,sprite_image_num;
    SPRITE *sprite_pointer;
    SPRITE *sprite_buf_pointer;  
   
    sprite_buf_pointer=(SPRITE *)(ExSpritebuf+display_sp_num*sizeof(SPRITE));    
    //parse header for sprite.bin
    sprite_idx_start = (INT32U)SPRITE_ATX_BIN;
    p=(INT16U *)(sprite_idx_start+(sp_num*sizeof(SPRITE_BIN_HEADER_L)));
	sprite_image_num = *(p+1);
	addr0 = (*(p+2)<<1);
	addr = (*(p+3)<<1);
	addr0 += addr << 16;
	//find the SPRITE start address 
	sprite_pointer =(SPRITE *)(sprite_idx_start + addr0);
    sprite_buf_pointer->nSizeX=(INT16S)sprite_pointer->nSizeX;
    sprite_buf_pointer->nSizeY=(INT16S)sprite_pointer->nSizeY;
	//find the SPRITE.SpCell start address
	sprite_offset=(INT32U)sprite_pointer->SpCell;
	sprite_idx_start+=sprite_offset<<1;
	sprite_pointer=(SPRITE *)sprite_idx_start;
	//SPRITE.SpCell pointer for sprite buffer
	sprite_buf_pointer->SpCell=(INT32U *)sprite_pointer;
	sprite_buf_pointer->nPosX=sp_pos_x;
    sprite_buf_pointer->nPosY=sp_pos_y;	 
}

void set_exsprite_image_number_bin(INT16U display_sp_num,INT16U sp_num,INT16U sp_image_num,INT32U* SPRITE_ATX_BIN)
{
    INT16U *p;
    INT32U addr0,addr;
    INT32U sprite_idx_start,sprite_offset,sprite_image_num;
    SPRITE *sprite_pointer;
    SPRITE *sprite_buf_pointer;  
   
    sprite_buf_pointer=(SPRITE *)(ExSpritebuf+display_sp_num*sizeof(SPRITE));    
    //parse header for sprite.bin
    sprite_idx_start = (INT32U)SPRITE_ATX_BIN;
    p=(INT16U *)(sprite_idx_start+(sp_num*sizeof(SPRITE_BIN_HEADER_L)));
	sprite_image_num = *(p+1);
	if((sp_image_num == 0) || (sp_image_num > sprite_image_num))
	   sp_image_num=1;     
	addr0 = (*(p+2)<<1);
	addr = (*(p+3)<<1);
	addr0 += addr << 16;
	//find the SPRITE start address 
	sprite_pointer =(SPRITE *)(sprite_idx_start + addr0);
	//check of sprute image number 
	sprite_pointer+=(sp_image_num-1);
    //find the SPRITE.SpCell start address
	sprite_offset=(INT32U)sprite_pointer->SpCell;
	sprite_idx_start+=sprite_offset<<1; 
	sprite_pointer=(SPRITE *)sprite_idx_start;
	//SPRITE.SpCell pointer for sprite buffer
	sprite_buf_pointer->SpCell=(INT32U *)sprite_pointer;
}

// Sprite V3D API
void spriteV3D_Pos_Coordinate_Change(Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,POS_STRUCT_PTR in)
{
    INT32S nPosXN,nPosYN,temp_x,temp_y,nScrWidth,nScrHeight;
    POS_STRUCT_PTR Sprite_pos;
    
    if(coordinate_mode!=PPU_hardware_coordinate)
    {    
	    switch(display_mode)
	    {
	       case Sprite_Coordinate_320X240:
	            nScrWidth = 320;
			    nScrHeight = 240 + 14;
	            break;
	       case Sprite_Coordinate_640X480:
	       		nScrWidth = 640;
			    nScrHeight = 480 + 14;
	            break;         
	       case Sprite_Coordinate_480X234:
	       		nScrWidth = 480;
			    nScrHeight = 234;       
	            break;
	       case Sprite_Coordinate_480X272:
	            nScrWidth = 480;
			    nScrHeight = 272;  
	            break;
	       case Sprite_Coordinate_720X480:
	       		nScrWidth = 720;
			    nScrHeight = 480;         
	            break;     
	       case Sprite_Coordinate_800X480:
	       		nScrWidth = 800;
			    nScrHeight = 480;         
	            break;         
	       case Sprite_Coordinate_800X600:
	       		nScrWidth = 800;
			    nScrHeight = 600;         
	            break;
	       case Sprite_Coordinate_1024X768:
	       		nScrWidth = 1024;
			    nScrHeight = 768;         
	            break;
	       default:
	            nScrWidth = 640;
			    nScrHeight = 480 + 14;
	            break;      
	    }
	    Sprite_pos=in;

	    //x0,y0
		temp_x=Sprite_pos->x0;
	    temp_y=Sprite_pos->y0; 
	    if(coordinate_mode==1)
	    {
	      // Center2LeftTop_coordinate
	      nPosXN = (nScrWidth>>1) + temp_x;
	      nPosYN = (nScrHeight>>1) - temp_y;       
	    }
	    else if(coordinate_mode==2)
	    {
	      // LeftTop2Center_coordinate
	      nPosXN = temp_x  - (nScrWidth>>1);
	      nPosYN = (nScrHeight>>1) - temp_y;  
	    }
	    Sprite_pos->x0=nPosXN;
	    Sprite_pos->y0=nPosYN;
	    
	    //x1,y1
		temp_x=Sprite_pos->x1;
	    temp_y=Sprite_pos->y1; 
	    if(coordinate_mode==1)
	    {
	      // Center2LeftTop_coordinate
	      nPosXN = (nScrWidth>>1) + temp_x;
	      nPosYN = (nScrHeight>>1) - temp_y;      
	    }
	    else if(coordinate_mode==2)
	    {
	      // LeftTop2Center_coordinate
	      nPosXN = temp_x  - (nScrWidth>>1);
	      nPosYN = (nScrHeight>>1) - temp_y;         
	    }
	    Sprite_pos->x1=nPosXN;
	    Sprite_pos->y1=nPosYN; 
	    
	    //x2,y2
		temp_x=Sprite_pos->x2;
	    temp_y=Sprite_pos->y2; 
	    if(coordinate_mode==1)
	    {
	      // Center2LeftTop_coordinate
	      nPosXN = (nScrWidth>>1) + temp_x;
	      nPosYN = (nScrHeight>>1) - temp_y;       
	    }
	    else if(coordinate_mode==2)
	    {
	      // LeftTop2Center_coordinate
	      nPosXN = temp_x  - (nScrWidth>>1);
	      nPosYN = (nScrHeight>>1) - temp_y;       
	    }
	    Sprite_pos->x2=nPosXN;
	    Sprite_pos->y2=nPosYN;	    
	    
	    //x3,y3
		temp_x=Sprite_pos->x3;
	    temp_y=Sprite_pos->y3; 
	    if(coordinate_mode==1)
	    {
	      // Center2LeftTop_coordinate
	      nPosXN = (nScrWidth>>1) + temp_x;
	      nPosYN = (nScrHeight>>1) - temp_y;      
	    }
	    else if(coordinate_mode==2)
	    {
	      // LeftTop2Center_coordinate
	      nPosXN = temp_x  - (nScrWidth>>1);
	      nPosYN = (nScrHeight>>1) - temp_y;
	    }
	    Sprite_pos->x3=nPosXN;
	    Sprite_pos->y3=nPosYN;	    
    }      
}

void SpCellXPos25d(INT32U xnum, INT32U ynum, INT32U xpos0, INT32U xpos1, INT32U xpos2, INT32U xpos3)
{
	INT32U i, j;
	FP32 GridPoint[SP25D_MAX_CHAR_V+1][2];
		
	//find 2-point of each grid line
	for (i=0;i<=ynum;i++)	
	{
		GridPoint[i][0] = xpos0 + i*(xpos3-xpos0)/ynum; //start position of horizontal grid line
		GridPoint[i][1] = xpos1 + i*(xpos2-xpos1)/ynum; //end position of horizontal grid line  
	}	
	//get grid line division   
	for (i=0;i<=ynum;i++)
	{
		for (j=0;j<=xnum;j++)	
			HGridBuff[i][j] = (FP32)(GridPoint[i][0] + j*(GridPoint[i][1]-GridPoint[i][0])/xnum);
 
 	}					   		
}		

void SpCellYPos25d(INT32U xnum, INT32U ynum, INT32U ypos0, INT32U ypos1, INT32U ypos2, INT32U ypos3)
{
	INT32U i, j;
	FP32 GridPoint[SP25D_MAX_CHAR_V+1][2];
		
	//find 2-point of each grid line
	for (i=0;i<=ynum;i++)
	{
		 GridPoint[i][0] = ypos0 + i*(ypos3-ypos0)/ynum; //start position of horizontal grid line   
		 GridPoint[i][1] = ypos1 + i*(ypos2-ypos1)/ynum; //end position of horizontal grid line
	}	
	//get grid line division   
	for (i=0;i<=ynum;i++)
	{
		for (j=0;j<=xnum;j++)	
			VGridBuff[i][j] = (FP32)(GridPoint[i][0] + j*(GridPoint[i][1]-GridPoint[i][0])/xnum);
	}							   		
}

#if ((MCU_VERSION >= GPL326XX))
void sp_frac_set(FP32 rem,PSpN_EX_RAM ex_ram,INT32U pos_xy,INT32U pos_num)
{
      
     if(pos_xy==pos_x){
       if((rem >= 0)&&(rem < 0.25)){
          if(pos_num==0)
            ex_ram->ex_attr0&=~3;
          else if(pos_num==1)
            ex_ram->ex_attr0&=~(3<<sp_frac_x1);
          else if(pos_num==2)
            ex_ram->ex_attr0&=~(3<<sp_frac_x2);
          else
            ex_ram->ex_attr0&=~(3<<sp_frac_x3);            
       }else if((rem >= 0.25)&&(rem < 0.5)){
          if(pos_num==0)
            ex_ram->ex_attr0|=1;
          else if(pos_num==1)
            ex_ram->ex_attr0|=(1<<sp_frac_x1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(1<<sp_frac_x2);
          else
            ex_ram->ex_attr0|=(1<<sp_frac_x3);       
       }else if((rem >= 0.5)&&(rem < 0.75)){
          if(pos_num==0)
            ex_ram->ex_attr0|=2;
          else if(pos_num==1)
            ex_ram->ex_attr0|=(2<<sp_frac_x1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(2<<sp_frac_x2);
          else
            ex_ram->ex_attr0|=(2<<sp_frac_x3);               
       }else if((rem >= 0.75)&&(rem < 1)){
          if(pos_num==0)
            ex_ram->ex_attr0|=3;
          else if(pos_num==1)
            ex_ram->ex_attr0|=(3<<sp_frac_x1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(3<<sp_frac_x2);
          else
            ex_ram->ex_attr0|=(3<<sp_frac_x3);       
       }        
     }else{
       if((rem >= 0)&&(rem < 0.25)){
          if(pos_num==0)
            ex_ram->ex_attr0&=~(3<<sp_frac_y0);
          else if(pos_num==1)
            ex_ram->ex_attr0&=~(3<<sp_frac_y1);
          else if(pos_num==2)
            ex_ram->ex_attr0&=~(3<<sp_frac_y2);
          else
            ex_ram->ex_attr0&=~(3<<sp_frac_y3);            
       }else if((rem >= 0.25)&&(rem < 0.5)){
          if(pos_num==0)
            ex_ram->ex_attr0|=(1<<sp_frac_y0);
          else if(pos_num==1)
            ex_ram->ex_attr0|=(1<<sp_frac_y1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(1<<sp_frac_y2);
          else
            ex_ram->ex_attr0|=(1<<sp_frac_y3);       
       }else if((rem >= 0.5)&&(rem < 0.75)){
          if(pos_num==0)
            ex_ram->ex_attr0|=(2<<sp_frac_y0);
          else if(pos_num==1)
            ex_ram->ex_attr0|=(2<<sp_frac_y1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(2<<sp_frac_y2);
          else
            ex_ram->ex_attr0|=(2<<sp_frac_y3);               
       }else if((rem >= 0.75)&&(rem < 1)){
          if(pos_num==0)
            ex_ram->ex_attr0|=(3<<sp_frac_y0);
          else if(pos_num==1)
            ex_ram->ex_attr0|=(3<<sp_frac_y1);
          else if(pos_num==2)
            ex_ram->ex_attr0|=(3<<sp_frac_y2);
          else
            ex_ram->ex_attr0|=(3<<sp_frac_y3);       
       }        
     }	
}

void exsp_frac_set(FP32 rem,INT32U ex_ram,INT32U pos_xy,INT32U pos_num)
{
     PSpN_RAM sprite_cdm_attr;
     
     sprite_cdm_attr=(PSpN_RAM)ex_ram;
     sprite_cdm_attr++;     
     if(SP_CMD_COMPARE())
     {
	     if(pos_xy==pos_x){
	       if((rem >= 0)&&(rem < 0.25)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16&=~(3<<exsp_frac_x0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16&=~(3<<exsp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0&=~(3<<exsp_frac_x0);
	          else
	            sprite_cdm_attr->attr0&=~(3<<exsp_frac_x1);            
	       }else if((rem >= 0.25)&&(rem < 0.5)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(1<<exsp_frac_x0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(1<<exsp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(1<<exsp_frac_x0);
	          else
	            sprite_cdm_attr->attr0|=(1<<exsp_frac_x1);       
	       }else if((rem >= 0.5)&&(rem < 0.75)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(2<<exsp_frac_x0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(2<<exsp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(2<<exsp_frac_x0);
	          else
	            sprite_cdm_attr->attr0|=(2<<exsp_frac_x1);               
	       }else if((rem >= 0.75)&&(rem < 1)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(3<<exsp_frac_x0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(3<<exsp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(3<<exsp_frac_x0);
	          else
	            sprite_cdm_attr->attr0|=(3<<exsp_frac_x1);       
	       }        
	     }else{
	       if((rem >= 0)&&(rem < 0.25)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16&=~(3<<exsp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16&=~(3<<exsp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0&=~(3<<exsp_frac_y0);
	          else
	            sprite_cdm_attr->attr0&=~(3<<exsp_frac_y1);            
	       }else if((rem >= 0.25)&&(rem < 0.5)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(1<<exsp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(1<<exsp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(1<<exsp_frac_y0);
	          else
	            sprite_cdm_attr->attr0|=(1<<exsp_frac_y1);       
	       }else if((rem >= 0.5)&&(rem < 0.75)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(2<<exsp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(2<<exsp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(2<<exsp_frac_y0);
	          else
	            sprite_cdm_attr->attr0|=(2<<exsp_frac_y1);               
	       }else if((rem >= 0.75)&&(rem < 1)){
	          if(pos_num==0)
	            sprite_cdm_attr->uX1_16|=(3<<exsp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uX1_16|=(3<<exsp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->attr0|=(3<<exsp_frac_y0);
	          else
	            sprite_cdm_attr->attr0|=(3<<exsp_frac_y1);       
	       }        
	     }	
     }
     else
     {    
	     if(pos_xy==pos_x){
	       if((rem >= 0)&&(rem < 0.25)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=0;
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_x2);
	          else
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_x3);            
	       }else if((rem >= 0.25)&&(rem < 0.5)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=1;
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_x2);
	          else
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_x3);       
	       }else if((rem >= 0.5)&&(rem < 0.75)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=2;
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_x2);
	          else
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_x3);               
	       }else if((rem >= 0.75)&&(rem < 1)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=3;
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_x1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_x2);
	          else
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_x3);       
	       }        
	     }else{
	       if((rem >= 0)&&(rem < 0.25)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_y2);
	          else
	            sprite_cdm_attr->uPosX_16|=(0<<sp_frac_y3);            
	       }else if((rem >= 0.25)&&(rem < 0.5)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_y2);
	          else
	            sprite_cdm_attr->uPosX_16|=(1<<sp_frac_y3);       
	       }else if((rem >= 0.5)&&(rem < 0.75)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_y2);
	          else
	            sprite_cdm_attr->uPosX_16|=(2<<sp_frac_y3);               
	       }else if((rem >= 0.75)&&(rem < 1)){
	          if(pos_num==0)
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_y0);
	          else if(pos_num==1)
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_y1);
	          else if(pos_num==2)
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_y2);
	          else
	            sprite_cdm_attr->uPosX_16|=(3<<sp_frac_y3);       
	       }        
	     }
     }      
}
#endif

#if ((MCU_VERSION >= GPL326XX))
void sp_group_set(PSpN_EX_RAM ex_ram,INT8U group_id)
{
     ex_ram->ex_attr1|=(group_id & mask_group_id);	
}

void sp_interpolation_enable(PSpN_EX_RAM ex_ram)
{
     ex_ram->ex_attr1|=sp_init_en;	
}

void sp_interpolation_disable(PSpN_EX_RAM ex_ram)
{
     ex_ram->ex_attr1 &= ~sp_init_en;	
}

void exsp_group_set(INT32U ex_ram,INT8U group_id)
{
     PSpN_RAM sprite_cdm_attr;
     
     if(SP_CMD_COMPARE())
     {
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->uPosX_16 |=((group_id & mask_group_id)<<b_spn_group);	
     }
     else
     {    
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->nCharNumLo_16|=(group_id & mask_group_id);
     } 	
}

void exsp_interpolation_enable(INT32U ex_ram)
{
     PSpN_RAM sprite_cdm_attr;
     
     if(SP_CMD_COMPARE())
     {
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->uPosX_16|=sp_cdm_init_en;
     }
     else
     {    
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->nCharNumLo_16|=sp_init_en;
     }      	
}
void exsp_interpolation_disable(INT32U ex_ram)
{
     PSpN_RAM sprite_cdm_attr;
     
     if(SP_CMD_COMPARE())
     {
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->uPosX_16 &= ~sp_cdm_init_en;
     }
     else
     {    
        sprite_cdm_attr=(PSpN_RAM)ex_ram;
        sprite_cdm_attr++;
        sprite_cdm_attr->nCharNumLo_16 &= ~sp_init_en;
     }      	
}
#endif

void SpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,V3D_POS_STRUCT_PTR in)
{
    INT32S nPosXN,nPosYN;
    INT32U i,k,h,nSP_Hsize,nSP_Vsize,sp_num_addr,temp_x,temp_y,INT,temp_ck;
    PSPRITE nSprite;
    PSpN16_CellIdx pSpCell;    
#if ((SP_FRACTION_ENABLE == 1))     
    POS_STRUCT_GP32XXX Sprite_pos;
    FP32 REM;
#else 
    POS_STRUCT Sprite_pos;
#endif    
    PSpN_ptr sp_ptr;
#if ((MCU_VERSION >= GPL326XX))
    PSpN_EX_RAM sprite_ex_attr;
#endif    
    FP32 SUM;

	nSP_Hsize=(Spritebuf[sprite_number].nSizeX);
	nSP_Vsize=(Spritebuf[sprite_number].nSizeY);
	nPosXN=Spritebuf[sprite_number].nPosX;
	nPosYN=Spritebuf[sprite_number].nPosY;
#if ((MCU_VERSION >= GPL326XX))
	Sprite_pos.x0=in->V3D_POS1.x0;
	Sprite_pos.y0=in->V3D_POS1.y0;
	Sprite_pos.x1=in->V3D_POS1.x1;
	Sprite_pos.y1=in->V3D_POS1.y1;
	Sprite_pos.x2=in->V3D_POS1.x2;
	Sprite_pos.y2=in->V3D_POS1.y2;
	Sprite_pos.x3=in->V3D_POS1.x3;
	Sprite_pos.y3=in->V3D_POS1.y3;
#else
	Sprite_pos.x0=in->V3D_POS2.x0;
	Sprite_pos.y0=in->V3D_POS2.y0;
	Sprite_pos.x1=in->V3D_POS2.x1;
	Sprite_pos.y1=in->V3D_POS2.y1;
	Sprite_pos.x2=in->V3D_POS2.x2;
	Sprite_pos.y2=in->V3D_POS2.y2;
	Sprite_pos.x3=in->V3D_POS2.x3;
	Sprite_pos.y3=in->V3D_POS2.y3;	
#endif	
	sp_ptr=sprite_ptr;
#if ((MCU_VERSION >= GPL326XX))
	sprite_ex_attr=(PSpN_EX_RAM)sp_ptr->nEXSPNum_ptr;
#endif	
	temp_ck=(INT32U)&Spritebuf[sprite_number];
	nSprite=(PSPRITE)temp_ck;
	sp_num_addr=sp_ptr->nSPNum_ptr;	
	if(sp_ptr->nSP_CharNum > 1)
    {
		temp_ck=nSP_Hsize%sp_ptr->nSP_Hsize;
		temp_x=nSP_Hsize/sp_ptr->nSP_Hsize;
		if(temp_ck)
		   temp_x++;
		temp_ck=nSP_Vsize%sp_ptr->nSP_Vsize;   
		temp_y=nSP_Vsize/sp_ptr->nSP_Vsize;
		if(temp_ck)
		   temp_y++;
		if((Sprite_pos.x0 < 0)||(Sprite_pos.x1 < 0)||(Sprite_pos.x2 < 0)||(Sprite_pos.x3 < 0)||
		(Sprite_pos.y0 < 0)||(Sprite_pos.y1 < 0)||(Sprite_pos.y2 < 0)||(Sprite_pos.y3 < 0))              
		   spriteV3D_Pos_Coordinate_Change(display_mode,Center2LeftTop_coordinate,(POS_STRUCT_PTR)&Sprite_pos);       
		SpCellXPos25d(temp_x,temp_y,Sprite_pos.x0,Sprite_pos.x1,Sprite_pos.x2,Sprite_pos.x3);
		SpCellYPos25d(temp_x,temp_y,Sprite_pos.y0,Sprite_pos.y1,Sprite_pos.y2,Sprite_pos.y3);
		for(i=0;i<sp_ptr->nSP_CharNum;i++)
		{
			pSpCell = (PSpN16_CellIdx) nSprite->SpCell+i;
			temp_x=pSpCell->nPosX;
			temp_y=pSpCell->nPosY;
			h = temp_x/(sp_ptr->nSP_Hsize);
			k = temp_y/(sp_ptr->nSP_Vsize);		   	   
			//x0
			SUM = (FP32)HGridBuff[k][h];
			INT = (INT32U)SUM;
			Sprite_pos.x0 = INT;
			//x1
			SUM = (FP32)HGridBuff[k][h+1];
			INT = (INT32U)SUM-nPosXN;
			INT = (INT32U)INT+nPosXN;
			Sprite_pos.x1 = INT;       			
			//x2
			SUM = (FP32)HGridBuff[k+1][h+1];
			INT = (INT32U)SUM-nPosXN;
			INT = (INT32U)INT+nPosXN;
			Sprite_pos.x2 = INT;			
			//x3
			SUM = (FP32)HGridBuff[k+1][h];
			INT = (INT32U)SUM-nPosXN;
			INT = (INT32U)INT+nPosXN;
			Sprite_pos.x3 = INT;       			
			//y0
			SUM = (FP32)VGridBuff[k][h];
			INT = (INT32U)SUM;
			Sprite_pos.y0 = INT;			
			//y1
			SUM = (FP32)VGridBuff[k][h+1];
			INT = (INT32U)SUM-nPosYN;
			INT = (INT32U)INT+nPosYN;
			Sprite_pos.y1 = INT;       		
			//y2
			SUM = (FP32)VGridBuff[k+1][h+1];
			INT = (INT32U)SUM-nPosYN;
			INT = (INT32U)INT+nPosYN;
			Sprite_pos.y2 = INT;			
			//y3
			SUM = (FP32)VGridBuff[k+1][h];
			INT = (INT32U)SUM-nPosYN;
			INT = (INT32U)INT+nPosYN;
			Sprite_pos.y3 = INT;       
		#if ((MCU_VERSION >= GPL326XX))
			sp_group_set(sprite_ex_attr,in->group_id);
			sp_interpolation_disable(sprite_ex_attr);
		#endif			
			spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
			gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);
			sp_num_addr+=sizeof(SpN_RAM);
			if(SP_CMD_COMPARE())
			   sp_num_addr+=sizeof(SpN_RAM);
        #if ((MCU_VERSION >= GPL326XX))
			sprite_ex_attr++;
			if(SP_CMD_COMPARE())
			   sprite_ex_attr++;
		#endif	
		}
	}
	else
	{
	 #if ((SP_FRACTION_ENABLE == 1))  		
		//x0
		SUM = (FP32)Sprite_pos.x0;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.x0 = INT;	    
	    sp_frac_set(REM,sprite_ex_attr,0,0);		
		//x1
		SUM = (FP32)Sprite_pos.x1;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.x1 = INT;       	    
	    sp_frac_set(REM,sprite_ex_attr,0,1);	    
	    //x2
	    SUM = (FP32)Sprite_pos.x2;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.x2 = INT;	    
	    sp_frac_set(REM,sprite_ex_attr,0,2);		
		//x3
		SUM = (FP32)Sprite_pos.x3;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.x3 = INT;       	    
	    sp_frac_set(REM,sprite_ex_attr,0,3);	    
	    //y0
		SUM = (FP32)Sprite_pos.y0;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.y0 = INT;	    
	    sp_frac_set(REM,sprite_ex_attr,1,0);		
		//y1
		SUM = (FP32)Sprite_pos.y1;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.y1 = INT;       	    
	    sp_frac_set(REM,sprite_ex_attr,1,1);	    
	    //y2
	    SUM = (FP32)Sprite_pos.y2;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.y2 = INT;
	    sp_frac_set(REM,sprite_ex_attr,1,2);		
		//y3
		SUM = (FP32)Sprite_pos.y3;
		INT = (INT32U)SUM;
		REM = (FP32)SUM-INT;
	    Sprite_pos.y3 = INT;       	    
	    sp_frac_set(REM,sprite_ex_attr,1,3);
	    sp_interpolation_enable(sprite_ex_attr);	
	 #endif        
	    spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
	    gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);   	
	} 	 
       
}

void EXSpriteV3D_set(INT32U sprite_number,Sprite_Coordinate_Mode display_mode,Coordinate_Change_Mode coordinate_mode,SpN_ptr *sprite_ptr,V3D_POS_STRUCT_PTR in)
{
    INT32S nPosXN,nPosYN;
    INT32U i,k,h,nSP_Hsize,nSP_Vsize,sp_num_addr,temp_x,temp_y,INT,temp_ck;
    PSPRITE nSprite;
    PSpN16_CellIdx pSpCell; 
#if ((SP_FRACTION_ENABLE == 1))    
    POS_STRUCT_GP32XXX Sprite_pos;
    FP32 REM;    
#else 
    POS_STRUCT Sprite_pos;
#endif    
    PSpN_ptr sp_ptr;
    PSpN_RAM sprite_cdm_attr;   
    FP32 SUM;
    SPRITE *sprite_buf_pointer;  
   
    sprite_buf_pointer=(SPRITE *)(ExSpritebuf+sprite_number*sizeof(SPRITE)); 
	nSP_Hsize=(sprite_buf_pointer->nSizeX);
	nSP_Vsize=(sprite_buf_pointer->nSizeY);
	nPosXN=sprite_buf_pointer->nPosX;
	nPosYN=sprite_buf_pointer->nPosY;
#if ((MCU_VERSION >= GPL326XX))
	Sprite_pos.x0=in->V3D_POS1.x0;
	Sprite_pos.y0=in->V3D_POS1.y0;
	Sprite_pos.x1=in->V3D_POS1.x1;
	Sprite_pos.y1=in->V3D_POS1.y1;
	Sprite_pos.x2=in->V3D_POS1.x2;
	Sprite_pos.y2=in->V3D_POS1.y2;
	Sprite_pos.x3=in->V3D_POS1.x3;
	Sprite_pos.y3=in->V3D_POS1.y3;
#else
	Sprite_pos.x0=in->V3D_POS2.x0;
	Sprite_pos.y0=in->V3D_POS2.y0;
	Sprite_pos.x1=in->V3D_POS2.x1;
	Sprite_pos.y1=in->V3D_POS2.y1;
	Sprite_pos.x2=in->V3D_POS2.x2;
	Sprite_pos.y2=in->V3D_POS2.y2;
	Sprite_pos.x3=in->V3D_POS2.x3;
	Sprite_pos.y3=in->V3D_POS2.y3;	
#endif
	sp_ptr=sprite_ptr;	
    sprite_cdm_attr=(PSpN_RAM)sp_ptr->nSPNum_ptr;
	nSprite=(PSPRITE)ExSpritebuf;
	sp_num_addr=sp_ptr->nSPNum_ptr;	
	if(sp_ptr->nSP_CharNum > 1)
    {
		temp_ck=nSP_Hsize%sp_ptr->nSP_Hsize;
		temp_x=nSP_Hsize/sp_ptr->nSP_Hsize;
		if(temp_ck)
		   temp_x++;
		temp_ck=nSP_Vsize%sp_ptr->nSP_Vsize;   
		temp_y=nSP_Vsize/sp_ptr->nSP_Vsize;
		if(temp_ck)
		   temp_y++;
		if((Sprite_pos.x0 < 0)||(Sprite_pos.x1 < 0)||(Sprite_pos.x2 < 0)||(Sprite_pos.x3 < 0)||
		(Sprite_pos.y0 < 0)||(Sprite_pos.y1 < 0)||(Sprite_pos.y2 < 0)||(Sprite_pos.y3 < 0))              
		   spriteV3D_Pos_Coordinate_Change(display_mode,Center2LeftTop_coordinate,(POS_STRUCT_PTR)&Sprite_pos);       
		SpCellXPos25d(temp_x,temp_y,Sprite_pos.x0,Sprite_pos.x1,Sprite_pos.x2,Sprite_pos.x3);
		SpCellYPos25d(temp_x,temp_y,Sprite_pos.y0,Sprite_pos.y1,Sprite_pos.y2,Sprite_pos.y3);
		for(i=0;i<sp_ptr->nSP_CharNum;i++)
		{
			if(EXSP_CMD_COMPARE())
			{
			   // SPRITE CMD ENABLE
			   pSpCell = (PSpN16_CellIdx) nSprite->SpCell+i;
			   temp_x=pSpCell->nPosX;
			   temp_y=pSpCell->nPosY;
			   h = temp_x/(sp_ptr->nSP_Hsize);
			   k = temp_y/(sp_ptr->nSP_Vsize);		   	   
			   //x0
			   SUM = (FP32)HGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.x0 = INT;
			   //x1
			   SUM = (FP32)HGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x1 = INT;       			
			   //x2
			   SUM = (FP32)HGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;;
			   Sprite_pos.x2 = INT;			
			   //x3
			   SUM = (FP32)HGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x3 = INT;       		
			   //y0
			   SUM = (FP32)VGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.y0 = INT;		
			   //y1
			   SUM = (FP32)VGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y1 = INT;       			
			   //y2
			   SUM = (FP32)VGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y2 = INT;			
			   //y3
			   SUM = (FP32)VGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y3 = INT;       	
		     #if ((MCU_VERSION >= GPL326XX))
			   exsp_group_set((INT32U)sprite_cdm_attr,in->group_id);
			   exsp_interpolation_disable((INT32U)sprite_cdm_attr);
		     #endif			
			   spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
			   gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);
			   sp_num_addr+=2*sizeof(SpN_RAM);			 			
			   sprite_cdm_attr++;
			   sprite_cdm_attr++;			   
	        }
	        else
	        {
             #if ((MCU_VERSION >= GPL326XX))
			   // SPRITE CMD DISABLE
			   pSpCell = (PSpN16_CellIdx) nSprite->SpCell+i;
			   temp_x=pSpCell->nPosX;
			   temp_y=pSpCell->nPosY;
			   h = temp_x/(sp_ptr->nSP_Hsize);
			   k = temp_y/(sp_ptr->nSP_Vsize);		   	   
			   //x0
			   SUM = (FP32)HGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.x0 = INT;
			   //x1
			   SUM = (FP32)HGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x1 = INT;       			
			   //x2
			   SUM = (FP32)HGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;;
			   Sprite_pos.x2 = INT;			
			   //x3
			   SUM = (FP32)HGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x3 = INT;       		
			   //y0
			   SUM = (FP32)VGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.y0 = INT;		
			   //y1
			   SUM = (FP32)VGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y1 = INT;       			
			   //y2
			   SUM = (FP32)VGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y2 = INT;			
			   //y3
			   SUM = (FP32)VGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y3 = INT;       		
			   exsp_group_set((INT32U)sprite_cdm_attr,in->group_id);
			   exsp_interpolation_disable((INT32U)sprite_cdm_attr);		
			   spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
			   gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);
			   sp_num_addr+=2*sizeof(SpN_RAM);			 			
			   sprite_cdm_attr++;
			   sprite_cdm_attr++;
	         #else
			   // SPRITE CMD DISABLE
			   pSpCell = (PSpN16_CellIdx) nSprite->SpCell+i;
			   temp_x=pSpCell->nPosX;
			   temp_y=pSpCell->nPosY;
			   h = temp_x/(sp_ptr->nSP_Hsize);
			   k = temp_y/(sp_ptr->nSP_Vsize);		   	   
			   //x0
			   SUM = (FP32)HGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.x0 = INT;
			   //x1
			   SUM = (FP32)HGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x1 = INT;       			
			   //x2
			   SUM = (FP32)HGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;;
			   Sprite_pos.x2 = INT;			
			   //x3
			   SUM = (FP32)HGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosXN;
			   INT = (INT32U)INT+nPosXN;
			   Sprite_pos.x3 = INT;       		
			   //y0
			   SUM = (FP32)VGridBuff[k][h];
			   INT = (INT32U)SUM;
			   Sprite_pos.y0 = INT;		
			   //y1
			   SUM = (FP32)VGridBuff[k][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y1 = INT;       			
			   //y2
			   SUM = (FP32)VGridBuff[k+1][h+1];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y2 = INT;			
			   //y3
			   SUM = (FP32)VGridBuff[k+1][h];
			   INT = (INT32U)SUM-nPosYN;
			   INT = (INT32U)INT+nPosYN;
			   Sprite_pos.y3 = INT;       				
			   spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
			   gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);
			   sp_num_addr+=sizeof(SpN_RAM);						
			   sprite_cdm_attr++;         
	         #endif	      
	        }  
		}
	}
	else
	{
	 	if(EXSP_CMD_COMPARE())
	 	{		 
		 #if ((SP_FRACTION_ENABLE == 1))  		
			//x0
			SUM = (FP32)Sprite_pos.x0;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x0 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,0);		
			//x1
			SUM = (FP32)Sprite_pos.x1;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x1 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,1);	    
		    //x2
		    SUM = (FP32)Sprite_pos.x2;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x2 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,2);		
			//x3
			SUM = (FP32)Sprite_pos.x3;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x3 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,3);	    
		    //y0
			SUM = (FP32)Sprite_pos.y0;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y0 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,0);		
			//y1
			SUM = (FP32)Sprite_pos.y1;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y1 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,1);	    
		    //y2
		    SUM = (FP32)Sprite_pos.y2;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y2 = INT;
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,2);		
			//y3
			SUM = (FP32)Sprite_pos.y3;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y3 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,3);
		 #endif
         #if ((MCU_VERSION >= GPL326XX))
            exsp_interpolation_enable((INT32U)sprite_cdm_attr);			 
		 #endif          		         
		    spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
		    gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);	 		 	
	 	}
	 	else
	 	{		 	
		 #if ((SP_FRACTION_ENABLE == 1))  		
			//x0
			SUM = (FP32)Sprite_pos.x0;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x0 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,0);		
			//x1
			SUM = (FP32)Sprite_pos.x1;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x1 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,1);	    
		    //x2
		    SUM = (FP32)Sprite_pos.x2;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x2 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,2);		
			//x3
			SUM = (FP32)Sprite_pos.x3;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.x3 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,0,3);	    
		    //y0
			SUM = (FP32)Sprite_pos.y0;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y0 = INT;	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,0);		
			//y1
			SUM = (FP32)Sprite_pos.y1;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y1 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,1);	    
		    //y2
		    SUM = (FP32)Sprite_pos.y2;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y2 = INT;
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,2);		
			//y3
			SUM = (FP32)Sprite_pos.y3;
			INT = (INT32U)SUM;
			REM = (FP32)SUM-INT;
		    Sprite_pos.y3 = INT;       	    
		    exsp_frac_set(REM,(INT32U)sprite_cdm_attr,1,3);
		 #endif
         #if ((MCU_VERSION >= GPL326XX))
            exsp_interpolation_enable((INT32U)sprite_cdm_attr);			 
		 #endif		         
		    spriteV3D_Pos_Coordinate_Change(display_mode,coordinate_mode,(POS_STRUCT_PTR)&Sprite_pos);         
		    gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_num_addr,(POS_STRUCT_PTR)&Sprite_pos);	 	
	 	}   	
	} 	 
       
}
