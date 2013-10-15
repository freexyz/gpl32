#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "sprite_dataSP_HDR.h"
#include "TEXT_Platform_verification_1_HDR.h"
#include "TEXT_Platform_verification_2_HDR.h"
#include "TEXT_Platform_verification_3_HDR.h"

#if  GPLIB_PPU_EN 
static FP32	Scale_Factor[240];
static INT32U	Tx3CosSineBuf[240];
#define C_PPU_DRV_FRAME_NUM		    3
#define PPU_TEXT_SIZE_HPIXEL       640
#define PPU_TEXT_SIZE_VPIXEL       480

#define	DISKUSED		       2

static PPU_REGISTER_SETS ppu_register_structure;
static PPU_REGISTER_SETS *ppu_register_set;
#endif

#define EXSP_ENABLE            1
#define CDM_MODE_ENABLE        1
#define Photo_Disable          0

void GPL326XXX_PPU_Demo(void)
{
#if  GPLIB_PPU_EN 
	INT32U frame_size;
    INT32U buffer_ptr,frame_count;
    INT32U i,Angle,flag,flag1,flag2,sp_rotate,sp_zoom,sp_mosaic;
    INT32S blend_level,sp_blend_level;
    INT32U sp_num,sp_num_addr;
    SpN_ptr sp_ptr;
    V3D_POS_STRUCT Sprite_pos;
    CDM_STRUCT Sprite_cdm;
    nTX_image_info image_info;
    
    Scale_Factor[0] = 0.5;
	for (i=1;i<240;i++) {
		Scale_Factor[i] = Scale_Factor[i-1] + 0.01;
	}
    
    //Initiate display device
//	tv_init();
	tft_init();

    /* initial ppu register parameter set structure */
    ppu_register_set = (PPU_REGISTER_SETS *) &ppu_register_structure;
    
    //Initiate PPU hardware engine and PPU register set structure
	gplib_ppu_init(ppu_register_set);
		
	//Now configure PPU software structure
	gplib_ppu_enable_set(ppu_register_set, 1);					            // Enable PPU
  
    //TV frame mode
  	gplib_ppu_non_interlace_set(ppu_register_set, 0);			            // Set non-interlace mode
  	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 0);		        // Enable TV/TFT frame buffer mode
    
    //PPU setting
	gplib_ppu_fb_format_set(ppu_register_set, 1, 1);			            // Set PPU output frame buffer format to YUYV
    gplib_ppu_vga_mode_set(ppu_register_set, 0);							// Disable VGA mode
    gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_320X240/*C_TFT_RESOLUTION_640X480*/);	// Set display resolution to 640x480
    gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);                      // bottom to top                
    gplib_ppu_long_burst_set(ppu_register_set, 1);
    
    //Frame buffer malloc
	frame_size = (PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2);
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		gplib_ppu_frame_buffer_add(ppu_register_set, buffer_ptr);
	}
	
	#if(Photo_Disable == 1)
      //window mode set
	  //(0,0)(640,480)
	  gplib_ppu_window_set(ppu_register_set, 0, 0x00000280, 0x000001E0);
	  //(256,12)(549,384)
	  gplib_ppu_window_set(ppu_register_set, 1, 0x81000225, 0x000C0180);
	#endif		
	
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}
//	tv_start(TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
	tft_start(C_DISPLAY_DEVICE);

    #if(Photo_Disable == 1)
      image_info.image_number=1;
	  image_info.position_x=0;
	  image_info.position_y=0;
	  image_info.pal_bank=0;
	  image_info.nNum_arr_ptr=buffer_ptr;
	  image_info.npal_ptr=(INT32U)0;
	  image_info.ntext_data_ptr=(INT32U)TEXT_Platform_verification_3_0_Header;
	  image_info.image_type=TEXT_DATA_C_CODE;
	  load_text_data(ppu_register_set,C_PPU_TEXT2,&image_info);
	  gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT2, 1);                  // Set TEXT window 1
	#endif  
	
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}    
    image_info.image_number=1;
	image_info.position_x=256;
	image_info.position_y=128;
	image_info.pal_bank=0;
	image_info.nNum_arr_ptr=buffer_ptr;
	image_info.npal_ptr=(INT32U)0;
	image_info.ntext_data_ptr=(INT32U)TEXT_Platform_verification_1_0_Header;
	image_info.image_type=TEXT_DATA_C_CODE;
	load_text_data(ppu_register_set,C_PPU_TEXT3,&image_info);
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT3, 256, 128);
	Text25D_CosSineBuf_set(ppu_register_set,(INT32U)Tx3CosSineBuf);
	#if(Photo_Disable == 1)
	  gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT3, 0);                  //Set TEXT window 0
	#endif
	
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}    
    image_info.image_number=1;
	image_info.position_x=320;
	image_info.position_y=240;
	image_info.pal_bank=0;
	image_info.nNum_arr_ptr=buffer_ptr;
	image_info.npal_ptr=(INT32U)_TEXT_Platform_verification_2_Palette0;
	image_info.ntext_data_ptr=(INT32U)TEXT_Platform_verification_2_0_Header;
	image_info.image_type=TEXT_DATA_C_CODE;
	load_text_data(ppu_register_set,C_PPU_TEXT4,&image_info);
	gplib_ppu_text_mode_set(ppu_register_set, C_PPU_TEXT4, 1);                       // TEXT rotate
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT4, 320, 240);
	#if(Photo_Disable == 1)
	  gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT4, 0);                  // Set TEXT window 0	
	#endif	
	
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)_sprite_dataSP_Palette0);   
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 3, (INT32U)_sprite_dataSP_Palette1);   
    gplib_ppu_dual_blend_set(ppu_register_set,1);
	
	// Now configure Sprite
	gplib_ppu_sprite_init(ppu_register_set);
	gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                     // Enable Sprite
	gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                            // set sprite coordinate
	gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                     // Set sprite address mode
	gplib_ppu_sprite_number_set(ppu_register_set, 256);                              // Set sprite number
	gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);     // set sprite ram buffer
	
	#if (MCU_VERSION >= GPL326XX)
	  gplib_ppu_sprite_extend_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteExRAM); // value: 32-bit pointer to sprite extend attribute ram
	#endif       
    
    gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_Sprite_CellData);        // sprite cell data
	gplib_ppu_sprite_rotate_enable_set(ppu_register_set, 1);                         // Enable sprite rotate mode               
	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);                           // Enable sprite zoom mode
	gplib_ppu_sprite_mosaic_enable_set(ppu_register_set, 1);                         // Enable sprite mosaic mode
	gplib_ppu_sprite_blend_mode_set(ppu_register_set, 1);                            // Enable sprite blend mode
    gplib_ppu_sprite_interpolation_set(ppu_register_set, 1);
    
    #if(CDM_MODE_ENABLE == 1)
    gplib_ppu_sprite_color_dither_mode_set(ppu_register_set, 1);
    #endif

	#if(Photo_Disable == 1)
	  gplib_ppu_sprite_window_enable_set(ppu_register_set, 1);                         // Enable sprite window mode
	#endif
	gplib_ppu_sprite_25d_mode_set(ppu_register_set,1);                               // Enable sprite 25D mode	
	
	#if(EXSP_ENABLE == 1)
	 gplib_ppu_exsprite_enable_set(ppu_register_set,1);
	#else
	 gplib_ppu_exsprite_enable_set(ppu_register_set,0);
	#endif
	
	#if(CDM_MODE_ENABLE == 1)
	gplib_ppu_exsprite_cdm_enable_set(ppu_register_set,1);
	#endif
    
    gplib_ppu_exsprite_interpolation_set(ppu_register_set,1);
    gplib_ppu_exsprite_number_set(ppu_register_set,1); 
    
    // image and position init    
    set_sprite_init(0,(INT32U)&Sprite0005_SP); 
    set_sprite_init(1,(INT32U)&Sprite0006_SP); 
    set_sprite_init(2,(INT32U)&Sprite0001_SP); 
    set_sprite_init(3,(INT32U)&Sprite0002_SP); 
    set_sprite_init(4,(INT32U)&Sprite0003_SP); 
    set_sprite_init(5,(INT32U)&Sprite0004_SP); 
    set_sprite_init(6,(INT32U)&Sprite0001_SP); 
    set_sprite_init(7,(INT32U)&Sprite0002_SP); 
    set_sprite_init(8,(INT32U)&Sprite0004_SP); 
    set_sprite_init(9,(INT32U)&Sprite0003_SP); 
    set_sprite_init(10,(INT32U)&Sprite0007_SP);
    set_sprite_init(11,(INT32U)&Sprite0008_SP);
    set_sprite_init(12,(INT32U)&Sprite0009_SP); 
    set_sprite_init(13,(INT32U)&Sprite0002_SP);
    set_sprite_init(14,(INT32U)&Sprite0002_SP); 
    set_sprite_init(15,(INT32U)&Sprite0007_SP);
    set_sprite_init(16,(INT32U)&Sprite0009_SP);                                        
    
    set_sprite_display_init(0,200,180,(INT32U)_wing_CellIdx);      
    set_sprite_display_init(1,300,300,(INT32U)_Balloon_CellIdx);   
    set_sprite_display_init(2,50,50,(INT32U)_a1_CellIdx);          
    set_sprite_display_init(3,30,100,(INT32U)_aa01_CellIdx);       
    set_sprite_display_init(4,100,150,(INT32U)_cc01_CellIdx);      
    set_sprite_display_init(5,140,200,(INT32U)_cc01_CellIdx);      
    set_sprite_display_init(6,200,250,(INT32U)_a1_CellIdx);        
    set_sprite_display_init(7,250,300,(INT32U)_aa01_CellIdx);      
    set_sprite_display_init(8,300,350,(INT32U)_bb01_CellIdx);      
    set_sprite_display_init(9,300,200,(INT32U)_bb01_CellIdx);      
    set_sprite_display_init(10,300,280,(INT32U)_J6bbabd36_CellIdx);
    set_sprite_display_init(11,300,100,(INT32U)_J2c5cf9e6_CellIdx);
    set_sprite_display_init(12,400,60,(INT32U)_Je2ecc5fb_CellIdx);
    set_sprite_display_init(13,400,60,(INT32U)_aa01_CellIdx);
    set_sprite_display_init(14,400,180,(INT32U)_wing_CellIdx); 
    set_sprite_display_init(15,200,200,(INT32U)_J6bbabd36_CellIdx);
    set_sprite_display_init(16,300,200,(INT32U)_Je2ecc5fb_CellIdx); 
    paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,20);
	
	buffer_ptr=extend_sprite_malloc_memory(16);
	gplib_ppu_exsprite_start_address_set(ppu_register_set, (INT32U)buffer_ptr);
	
	// extend sprite set
	set_exsprite_init(0,(INT32U)&Sprite0001_SP);
	set_exsprite_display_init(0,50,50,(INT32U)_a1_CellIdx);
	set_exsprite_init(1,(INT32U)&Sprite0007_SP);
	set_exsprite_display_init(1,50,50,(INT32U)_Balloon_CellIdx);	
	paint_ppu_exspriteram(Sprite_Coordinate_640X480,LeftTop2Center_coordinate,2);
	
	// Start PPU and wait until PPU operation is done
	gplib_ppu_go_and_wait_done(ppu_register_set);

    //variable init
	Angle=0;
	blend_level=0;
	sp_blend_level=0;
	flag=0;
	flag1=0;
	flag2=0;
	sp_num=0;
	sp_rotate=0;
	sp_zoom=16;
	sp_mosaic=0;
	frame_count=0;
	
	while(1)
	{
	  	 if(flag==0){
	       if(++blend_level > 63){
	            blend_level=63;
	            flag=1;
	       }
	     }else{
	       if(--blend_level < 0){
	            blend_level=0;
	            flag=0;
	       }
	     }
	     
	     if (++Angle > 359) {
	         Angle = 0;
	     }
	     
	  	 //text 4
		 gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT4, 1, 1,blend_level);
		 gplib_ppu_text_rotate_zoom_set(ppu_register_set, C_PPU_TEXT4, Angle, 1);
	     
	     //text 3
	     gplib_ppu_text3_25d_set(ppu_register_set, Angle, Scale_Factor);
	     
	     if(flag1==0){
	       if(++sp_zoom > 63){
	            sp_zoom=63;
	            flag1=1;
	       }
	     }else{
	       if(--sp_zoom < 16){
	            sp_zoom=16;
	            flag1=0;
	       }
	     }
	     
	     if(flag2==0){
	       if(++sp_blend_level > 16){
	            sp_blend_level=16;
	            flag2=1;
	       }
	     }else{
	       if(--sp_blend_level < 0){
	            sp_blend_level=0;
	            flag2=0;
	       }
	     }	     
	     
	     if (++sp_rotate > 63) {
	         sp_rotate = 0;
	     }
	     
	     if(frame_count % 5 == 0){
	       if (++sp_mosaic > 3)
	           sp_mosaic = 0;
	     }  
	     
	     if(sp_num++ > 3)
	        sp_num=0;
         
         switch(sp_num){
           case 0:
         	  set_sprite_image_number(2,(INT32U)_a1_CellIdx);
         	  set_exsprite_image_number(0,(INT32U)_a1_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa01_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc01_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc01_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a1_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa01_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb01_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb01_CellIdx);
         	  set_sprite_image_number(13,(INT32U)_aa01_CellIdx);
           break;
           case 1:
         	  set_sprite_image_number(2,(INT32U)_a2_CellIdx);
         	  set_exsprite_image_number(0,(INT32U)_a2_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa02_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc02_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc02_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a2_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa02_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb02_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb02_CellIdx);
         	  set_sprite_image_number(13,(INT32U)_aa02_CellIdx);       	  
           break;
           case 2:
         	  set_sprite_image_number(2,(INT32U)_a3_CellIdx);
         	  set_exsprite_image_number(0,(INT32U)_a3_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa03_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc03_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc03_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a3_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa03_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb03_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb03_CellIdx);
         	  set_sprite_image_number(13,(INT32U)_aa03_CellIdx);        	  
           break;
           case 3:
         	  set_sprite_image_number(2,(INT32U)_a4_CellIdx);
         	  set_exsprite_image_number(0,(INT32U)_a4_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa04_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc04_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc04_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a4_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa04_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb04_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb04_CellIdx);
         	  set_sprite_image_number(13,(INT32U)_aa04_CellIdx);         	  
           break;
           case 4:
         	  set_sprite_image_number(2,(INT32U)_a5_CellIdx);
         	  set_exsprite_image_number(0,(INT32U)_a5_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa05_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc05_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc05_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a5_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa05_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb05_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb05_CellIdx);
         	  set_sprite_image_number(13,(INT32U)_aa05_CellIdx);         	    
           break;
         }
	     
         SetSpritePosition(0,Sprite_Coordinate_640X480,-4,-2,1);
	     SetSpritePosition(1,Sprite_Coordinate_640X480,3,2,1);
	     SetSpritePosition(2,Sprite_Coordinate_640X480,4,0,1);
	     SetSpritePosition(3,Sprite_Coordinate_640X480,-2,0,1);
	     SetSpritePosition(4,Sprite_Coordinate_640X480,3,0,1);
	     SetSpritePosition(5,Sprite_Coordinate_640X480,6,0,1);
	     SetSpritePosition(6,Sprite_Coordinate_640X480,-2,0,1);
	     SetSpritePosition(7,Sprite_Coordinate_640X480,3,0,1);
	     SetSpritePosition(8,Sprite_Coordinate_640X480,3,0,1);
	     SetSpritePosition(9,Sprite_Coordinate_640X480,-5,0,1);
	     SetSpritePosition(10,Sprite_Coordinate_640X480,2,0,1);
	     SetSpritePosition(11,Sprite_Coordinate_640X480,4,0,1);	
	     paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,20);
	     paint_ppu_exspriteram(Sprite_Coordinate_640X480,LeftTop2Center_coordinate,2);	     
         
         //get exsprite character number of image and sprite start ptr of sprite ram
         Get_exsprite_image_info(0,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     #if CDM_MODE_ENABLE == 1
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	         sp_num_addr+=2*sizeof(SpN_RAM);
	     }
	     #endif        
         Sprite_pos.V3D_POS1.x0=40;
         Sprite_pos.V3D_POS1.y0=40;
         Sprite_pos.V3D_POS1.x1=140;
         Sprite_pos.V3D_POS1.y1=40;
         Sprite_pos.V3D_POS1.x2=160;
         Sprite_pos.V3D_POS1.y2=140;
         Sprite_pos.V3D_POS1.x3=60;
         Sprite_pos.V3D_POS1.y3=140;
         Sprite_pos.group_id=2;     
         EXSpriteV3D_set(0,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,&sp_ptr,&Sprite_pos);
         Get_exsprite_image_info(1,(SpN_ptr *)&sp_ptr);
         Sprite_pos.V3D_POS1.x0=40+440;
         Sprite_pos.V3D_POS1.y0=40;
         Sprite_pos.V3D_POS1.x1=140+440;
         Sprite_pos.V3D_POS1.y1=40;
         Sprite_pos.V3D_POS1.x2=160+440;
         Sprite_pos.V3D_POS1.y2=140;
         Sprite_pos.V3D_POS1.x3=60+440;
         Sprite_pos.V3D_POS1.y3=140;         
         EXSpriteV3D_set(1,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,&sp_ptr,&Sprite_pos);         	                  
	     #if CDM_MODE_ENABLE == 1
	      Sprite_cdm.cdm0=0x00FFFFFF;
	      Sprite_cdm.cdm1=0x00000000;
	      Sprite_cdm.cdm2=0x00000000;
	      Sprite_cdm.cdm3=0x00000000;
	      gplib_ppu_sprite_cdm_attribute_set((SpN_RAM *)sp_ptr.nSPNum_ptr,0,&Sprite_cdm);
	     #endif         
         
         //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(0,(SpN_ptr *)&sp_ptr);
         gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
         gplib_ppu_sprite_attribute_zoom_set((SpN_RAM *)sp_ptr.nSPNum_ptr, sp_zoom);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);	
         #if CDM_MODE_ENABLE == 1
          gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 1);
         #endif
         
         //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(1,(SpN_ptr *)&sp_ptr);
         gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
         gplib_ppu_sprite_attribute_zoom_set((SpN_RAM *)sp_ptr.nSPNum_ptr, sp_zoom);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);	
         #if CDM_MODE_ENABLE == 1
          gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 1);
         #endif
         
	     //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(2,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        gplib_ppu_sprite_attribute_mosaic_set((SpN_RAM *)sp_num_addr, sp_mosaic);
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(3,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        // Blending level down grade to 16-level when sprite window function is enabled.
	        gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *)sp_num_addr,1,sp_blend_level);
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(4,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        // Blending level down grade to 16-level when sprite window function is enabled.
	        gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *)sp_num_addr,1,sp_blend_level);
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(5,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(6,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(7,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(8,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(9,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	        //sprite 2D mode
            gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);
            #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	        #endif
	        #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	        #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	        #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(10,(SpN_ptr *)&sp_ptr);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);
         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 1);	

	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(11,(SpN_ptr *)&sp_ptr);
	     gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
	     #if(Photo_Disable == 1)
	       gplib_ppu_sprite_attribute_window_set((SpN_RAM *)sp_ptr.nSPNum_ptr,1);
	     #endif
	     gplib_ppu_sprite_attribute_depth_set((SpN_RAM *)sp_ptr.nSPNum_ptr,1);
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);
         #if CDM_MODE_ENABLE == 1
          gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 1);
         #endif

         //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(12,(SpN_ptr *)&sp_ptr);
	     #if CDM_MODE_ENABLE == 1
	      gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 1);
	     #endif
         Sprite_pos.V3D_POS1.x0=350;
         Sprite_pos.V3D_POS1.y0=80;
         Sprite_pos.V3D_POS1.x1=440;
         Sprite_pos.V3D_POS1.y1=20;
         Sprite_pos.V3D_POS1.x2=400;
         Sprite_pos.V3D_POS1.y2=130;
         Sprite_pos.V3D_POS1.x3=300;
         Sprite_pos.V3D_POS1.y3=180;
         SpriteV3D_set(12,Sprite_Coordinate_480X234,LeftTop2Center_coordinate,&sp_ptr,&Sprite_pos);
	     Get_sprite_image_info(13,(SpN_ptr *)&sp_ptr);
	     #if CDM_MODE_ENABLE == 1	
	     sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {	     
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	         sp_num_addr+=2*sizeof(SpN_RAM);
	     } 
	     #endif  
         Sprite_pos.V3D_POS1.x0=240;
         Sprite_pos.V3D_POS1.y0=40;
         Sprite_pos.V3D_POS1.x1=440;
         Sprite_pos.V3D_POS1.y1=40;
         Sprite_pos.V3D_POS1.x2=460;
         Sprite_pos.V3D_POS1.y2=140;
         Sprite_pos.V3D_POS1.x3=260;
         Sprite_pos.V3D_POS1.y3=140;
         Sprite_pos.group_id=2;
         SpriteV3D_set(13,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,&sp_ptr,&Sprite_pos);         
	     Get_sprite_image_info(14,(SpN_ptr *)&sp_ptr);
         Sprite_pos.V3D_POS1.x0=240/2;
         Sprite_pos.V3D_POS1.y0=40/2;
         Sprite_pos.V3D_POS1.x1=440/2;
         Sprite_pos.V3D_POS1.y1=40/2;
         Sprite_pos.V3D_POS1.x2=460/2;
         Sprite_pos.V3D_POS1.y2=140/2;
         Sprite_pos.V3D_POS1.x3=260/2;
         Sprite_pos.V3D_POS1.y3=140/2;
         SpriteV3D_set(14,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,&sp_ptr,&Sprite_pos);	     
	     #if CDM_MODE_ENABLE == 1
	      Sprite_cdm.cdm0=0x00FF0000;
	      Sprite_cdm.cdm1=0x0000FF00;
	      Sprite_cdm.cdm2=0x000000FF;
	      Sprite_cdm.cdm3=0x00FFFFFF;
	      gplib_ppu_sprite_cdm_attribute_set((SpN_RAM *)sp_ptr.nSPNum_ptr,0,&Sprite_cdm);
	     #endif
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(15,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	       // Blending level down grade to 16-level when sprite window function is enabled.
	       gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *)sp_num_addr,1,sp_blend_level);
	       //sprite 2D mode
           gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	       #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	       #endif
	       #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	       #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	       #endif
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(16,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	       // Blending level down grade to 16-level when sprite window function is enabled.
	       gplib_ppu_sprite_attribute_blend16_set((SpN_RAM *)sp_num_addr,1,sp_blend_level);	     
	       //sprite 2D mode
           gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
           #if CDM_MODE_ENABLE == 1	
	         gplib_ppu_sprite_cdm_attribute_enable_set((SpN_RAM *)sp_num_addr, 1);
	       #endif
	       #if CDM_MODE_ENABLE == 0
	          sp_num_addr+=sizeof(SpN_RAM);
	       #else
	          sp_num_addr+=2*sizeof(SpN_RAM);
	       #endif
	     }	     
	     
	     //Start PPU and wait until PPU operation is done
	     gplib_ppu_go(ppu_register_set);
	     #if(EXSP_ENABLE == 0)
	     OSTimeDly(1);
	     #endif
	     frame_count++;
	}
#endif
}

INT32U cater_flag;
void user_define_isr(INT32U PPU_MODE);

void GPL32XXXX_PPU_Hblank_Demo(void)
{
#if  GPLIB_PPU_EN 
	INT32U frame_size;
    INT32U buffer_ptr,frame_count;
    INT32U i,Angle;
    nTX_image_info image_info;
    
    //Initiate display device
	tv_init();
    
    /* initial ppu register parameter set structure */
    ppu_register_set = (PPU_REGISTER_SETS *) &ppu_register_structure;
    
    //Initiate PPU hardware engine and PPU register set structure
	gplib_ppu_init(ppu_register_set);
		
	//Now configure PPU software structure
	gplib_ppu_enable_set(ppu_register_set, 1);					            // Enable PPU
  
    //TV frame mode
  	gplib_ppu_non_interlace_set(ppu_register_set, 0);			            // Set non-interlace mode
  	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 0);		        // Enable TV/TFT frame buffer mode
    
    //PPU setting
	gplib_ppu_fb_format_set(ppu_register_set, 1, 1);			            // Set PPU output frame buffer format to YUYV
    gplib_ppu_vga_mode_set(ppu_register_set, 0);							// Disable VGA mode
    gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_640X480);	// Set display resolution to 640x480
    gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);   
    
    //Frame buffer malloc
	frame_size = (PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2);
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		gplib_ppu_frame_buffer_add(ppu_register_set, buffer_ptr);
	}
	tv_start(TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}    
    image_info.image_number=1;
	image_info.position_x=0;
	image_info.position_y=0;
	image_info.pal_bank=0;
	image_info.nNum_arr_ptr=buffer_ptr;
	image_info.npal_ptr=(INT32U)0;
	image_info.ntext_data_ptr=(INT32U)TEXT_Platform_verification_1_0_Header;
	image_info.image_type=TEXT_DATA_C_CODE;
	load_text_data(ppu_register_set,C_PPU_TEXT3,&image_info);
    gplib_ppu_text_mode_set(ppu_register_set, C_PPU_TEXT3, 0);                          //TEXTV3D of text mode   
    
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)_sprite_dataSP_Palette0);   
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 3, (INT32U)_sprite_dataSP_Palette1);   
	
	// Now configure Sprite
	gplib_ppu_sprite_init(ppu_register_set);
	gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                     // Enable Sprite
	gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                            // set sprite coordinate
	gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                     // Set sprite address mode
	gplib_ppu_sprite_number_set(ppu_register_set, 256);                              // Set sprite number
	gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);     // set sprite ram buffer
	
	#if ((MCU_VERSION >= GPL326XX))
	  gplib_ppu_sprite_extend_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteExRAM); // value: 32-bit pointer to sprite extend attribute ram
	#endif       
    
    gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_Sprite_CellData);        // sprite cell data
	gplib_ppu_sprite_rotate_enable_set(ppu_register_set, 1);                         // Enable sprite rotate mode
	set_sprite_init(0,(INT32U)&Sprite0009_SP);
    set_sprite_display_init(0,300,150,(INT32U)_Je2ecc5fb_CellIdx); 
    paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,1);
	// Start PPU and wait until PPU operation is done
	gplib_ppu_go_and_wait_done(ppu_register_set);
	OSTimeDly(10); 
    frame_size=0;
	cater_flag=1;
	Angle=0;
	while(1)
	{       
       if(Angle)
       {
			if(frame_size==0)
			{
				gplib_ppu_hblnk_set(1);
				gplib_ppu_hblnk_line_offset_set(240);          
				frame_size=1;
			}
			user_define_isr(1);
			gplib_ppu_go_hblank_first(ppu_register_set);
			user_define_isr(0);
			gplib_ppu_hblnk_go_now(ppu_register_set);
	   }
	   else
	   {
			if(frame_size)
			{
				gplib_ppu_hblnk_set(0);
				gplib_ppu_hblnk_line_offset_set(0);          
				frame_size=0;
			}	
		         
			for(i=0;i<512;i++)
			{	    
			    set_sprite_init(i,(INT32U)&Sprite0009_SP);
			    set_sprite_display_init(i,i*16,(i%20)*32,(INT32U)_Je2ecc5fb_CellIdx); 	            
			}
			paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,512);
		    gplib_ppu_go(ppu_register_set);	   
	   }
	   OSTimeDly(1);
	   if(frame_count++ > 100)
	   {
	        frame_count=0;
	        if(Angle)
	          Angle=0;
	        else
	          Angle=1;
	   }
	           	 
    }
#endif    
}

INT32U sp_rotate1=0;
INT32S sp_rotate2=63;
void user_define_isr(INT32U PPU_MODE)
{
#if  GPLIB_PPU_EN 
	INT32U i;
	SpN_ptr sp_ptr;
	    
    if(PPU_MODE)  // 0~240
    {
	    if(++sp_rotate1 > 63)
	      sp_rotate1 = 0;
	      
	    for(i=0;i<512;i++)
	    {	    
		    set_sprite_init(i,(INT32U)&Sprite0009_SP);
	        set_sprite_display_init(i,i*2,(i%170)*64,(INT32U)_Je2ecc5fb_CellIdx); 	            
        }
        paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,512);
        for(i=0;i<512;i++)
        {
        	Get_sprite_image_info(i,(SpN_ptr *)&sp_ptr);
	        gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate1);
        }   
    }
    else                  // 240~480
    {
	    if(--sp_rotate2 < 0)
	      sp_rotate2 = 63;	    
	    for(i=0;i<512;i++)
	    {
		    set_sprite_init(i,(INT32U)&Sprite0008_SP);
	        set_sprite_display_init(i,i*2,((i%170)+3)*64,(INT32U)_J2c5cf9e6_CellIdx); 	           
        }
        paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,512); 
        for(i=0;i<512;i++)
        {
 	        Get_sprite_image_info(i,(SpN_ptr *)&sp_ptr);
	        gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate2);       
        }            
    }
    gplib_ppu_hblnk_irq_wait();         
#endif        
}
