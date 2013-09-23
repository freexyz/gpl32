#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "sprite_dataSP_HDR.h"
#include "text_dataTX_HDR.h"

void pokemon_demo(void);

#if  GPLIB_PPU_EN 
static FP32	Scale_Factor[240];
static INT32U	Tx3CosSineBuf[240];

#define C_PPU_DRV_FRAME_NUM		    2
#define PPU_TEXT_SIZE_HPIXEL     	C_DISPLAY_DEV_HPIXEL
#define PPU_TEXT_SIZE_VPIXEL       	C_DISPLAY_DEV_VPIXEL

static PPU_REGISTER_SETS ppu_register_structure;
static PPU_REGISTER_SETS *ppu_register_set;
#endif

void pokemon_demo(void)
{
#if  GPLIB_PPU_EN 
	INT32U frame_size;
    INT32U buffer_ptr,frame_count;
    INT32U i,j,Angle,flag,flag1,flag2,sp_rotate,sp_zoom,sp_mosaic;
    INT32S blend_level,sp_blend_level;
    INT32U sp_num,sp_num_addr;
    SpN_ptr sp_ptr;
    POS_STRUCT Sprite_pos;

    Scale_Factor[0] = 0.5;
	for (i=1;i<240;i++) {
		Scale_Factor[i] = Scale_Factor[i-1] + 0.01;
	}

    //Initiate display device
#if C_DISPLAY_DEVICE == C_TV_VGA
	tv_init();
	tv_start(TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
#else
	tft_init();
	if(PPU_TEXT_SIZE_HPIXEL == 800 && PPU_TEXT_SIZE_VPIXEL == 480) 
		tft_start_with_pwm(TPO_TD070WGCB2,1,1);  
	else
		tft_start_with_pwm(ANALOG_PANEL_TYPE_1,1,1);  
#endif

    /* initial ppu register parameter set structure */
    ppu_register_set = (PPU_REGISTER_SETS *) &ppu_register_structure;
  
    //Initiate PPU hardware engine and PPU register set structure
	gplib_ppu_init(ppu_register_set);
		
	//Now configure PPU software structure
	gplib_ppu_enable_set(ppu_register_set, 1);					            // Enable PPU
	gplib_ppu_char0_transparent_set(ppu_register_set, 1);		            // Enable character 0 transparent mode
	gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);
  
    //TV frame mode
  	gplib_ppu_non_interlace_set(ppu_register_set, 0);			            // Set interlace mode
  	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 0);		        // Enable TV/TFT frame buffer mode
    
    //PPU setting
	gplib_ppu_fb_format_set(ppu_register_set, 1, 1);			            // Set PPU output frame buffer format to YUYV
    
    if(C_DISPLAY_DEVICE == C_TV_VGA){
       gplib_ppu_vga_mode_set(ppu_register_set, 1);							// Enable VGA mode
       gplib_ppu_resolution_set(ppu_register_set, C_TV_RESOLUTION_640X480);	// Set display resolution to 640x480
    }else if(C_DISPLAY_DEVICE < C_TV_QVGA && PPU_TEXT_SIZE_HPIXEL == 800 && PPU_TEXT_SIZE_VPIXEL == 480){
       gplib_ppu_vga_mode_set(ppu_register_set, 0);							// Disable VGA mode
       gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_800X480);// Set display resolution to 800x480
       gplib_ppu_tft_long_burst_set(ppu_register_set,1);
    }else{
       gplib_ppu_vga_mode_set(ppu_register_set, 0);							// Disable VGA mode
       gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_480X234);// Set display resolution to 480x234
    }
    
    gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);                      // bottom to top
    gplib_ppu_palette_type_set(ppu_register_set, 0, 1);                     // text and sprite palette mode
    
    //palette ram init
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 0, (INT32U)_Text_data_2TX_Palette0);
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)_sprite_dataSP_Palette0);
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 3, (INT32U)_sprite_dataSP_Palette1);                 
    
    //Frame buffer malloc
	frame_size = (PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2);
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		if (buffer_ptr) {
	    	INT32U *ptr;
	    	
	    	ptr = (INT32U *) buffer_ptr;
	    	for (j=0; j<frame_size; j++) {
	    	    *ptr++ = 0x00800080;              // Black for YUYV color mode
	    	}
			gplib_ppu_frame_buffer_add(ppu_register_set, buffer_ptr);
		} else {
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
		}
	}
	
	//De-flicker is only valid under VGA + Interlace + Frame mode
	gplib_ppu_deflicker_mode_set(0);			            
	
	if(C_DISPLAY_DEVICE == C_TV_VGA){
	    //window mode set
	    //(0,0)(640,480)
	    gplib_ppu_window_set(ppu_register_set, 0, 0x00000280, 0x000001E0);
	    //(320,100)(520,200)
	    gplib_ppu_window_set(ppu_register_set, 1, 0x81400208, 0x006400C8);	
	}else if(C_DISPLAY_DEVICE < C_TV_QVGA && PPU_TEXT_SIZE_HPIXEL == 800 && PPU_TEXT_SIZE_VPIXEL == 480){
		//window mode set
	    //(0,0)(800,480)
	    gplib_ppu_window_set(ppu_register_set, 0, 0x00000320, 0x000001E0);
	    //(320,100)(520,200)
	    gplib_ppu_window_set(ppu_register_set, 1, 0x81400208, 0x006400C8);
	}else{
		//window mode set
	    //(0,0)(480,234)
	    gplib_ppu_window_set(ppu_register_set, 0, 0x000001E0, 0x00000EA);
	    //(100,50)(300,150)
	    gplib_ppu_window_set(ppu_register_set, 1, 0x8064012C, 0x00320096);
	}
	
	// Now configure TEXT relative elements
	gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT3, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);			                // Disable TEXT direct address mode
    gplib_ppu_text_attribute_source_select(ppu_register_set, C_PPU_TEXT3, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, C_PPU_TEXT3, 2);			            // Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(ppu_register_set, C_PPU_TEXT3, 0);				    // Set TEXT segment address	
	
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}
	//text 2 2D 
	gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT2, 1);	                 // Enable TEXT	
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, C_PPU_TEXT2, buffer_ptr);	 // Set TEXT number array address
  	gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, C_PPU_TEXT2, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, C_PPU_TEXT2, 1, 3);				     // Set TEXT color to YUYV
	gplib_ppu_text_depth_set(ppu_register_set, C_PPU_TEXT2, 1);                      // Set TEXT Depth 1
	gplib_ppu_text_size_set(ppu_register_set, C_PPU_TEXT2, 2);			             // Set TEXT size to 1024x512
	gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT2, 1);                  // Set TEXT window 1
	
	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	buffer_ptr = (INT32U) gp_malloc_align(1024*4,4);	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	if (!buffer_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}	
	//text 3 2.5D 	
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, C_PPU_TEXT3, buffer_ptr);	 // Set TEXT number array address
  	gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, C_PPU_TEXT3, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, C_PPU_TEXT3, 1, 3);				     // Set TEXT color to YUYV 
	gplib_ppu_text_mode_set(ppu_register_set, C_PPU_TEXT3, 2);                       // TEXT 2.5D
	gplib_ppu_text3_25d_y_compress_set(ppu_register_set, 0x10);
	gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT3, 256, 128);
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT3, 256, 128);
	Text25D_CosSineBuf_set(ppu_register_set,(INT32U)Tx3CosSineBuf);
	gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT3, 0);                  //Set TEXT window 0
	
    //text 4 rotate+blending 	
	gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT4, 1);	                 // Enable TEXT
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, C_PPU_TEXT4, (INT32U)_Text2_IndexData);	 // Set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, C_PPU_TEXT4, 0);			     // Disable bitmap mode
	gplib_ppu_text_attribute_source_select(ppu_register_set, C_PPU_TEXT4, 1);	     // Get TEXT attribute from register
	gplib_ppu_text_color_set(ppu_register_set, C_PPU_TEXT4, 0, 0);				     // Set TEXT color to 4 
	gplib_ppu_text_mode_set(ppu_register_set, C_PPU_TEXT4, 1);                       // TEXT rotate
	gplib_ppu_text_size_set(ppu_register_set, C_PPU_TEXT4, 2);			             // Set TEXT size to 1024x512
	gplib_ppu_text_character_size_set(ppu_register_set, C_PPU_TEXT4, 3, 3);          // TEXT Character size set
	gplib_ppu_text_palette_set(ppu_register_set, C_PPU_TEXT4, 0, 0);
	gplib_ppu_text_segment_set(ppu_register_set, C_PPU_TEXT4, (INT32U)_Text_CellData_2);
	gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT4, 256, 128);
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT4, 256, 128);
	gplib_ppu_text_window_select(ppu_register_set, C_PPU_TEXT4, 0);                  // Set TEXT window 0
	
    // Now configure Sprite
	gplib_ppu_sprite_init(ppu_register_set);
	gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                     // Enable Sprite
	gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                            // set sprite coordinate
	gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                     // Set sprite address mode
	gplib_ppu_sprite_number_set(ppu_register_set, 128);                              // Set sprite number
	gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);     // set sprite ram buffer
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_Sprite_CellData);        // sprite cell data
	gplib_ppu_sprite_rotate_enable_set(ppu_register_set, 1);                         // Enable sprite rotate mode               
	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);                           // Enable sprite zoom mode
	gplib_ppu_sprite_mosaic_enable_set(ppu_register_set, 1);                         // Enable sprite mosaic mode
	gplib_ppu_sprite_blend_mode_set(ppu_register_set, 1);                            // Enable sprite blend mode
	
	// Blend level down grade to 16-level when sprite window function is enabled.
	gplib_ppu_sprite_window_enable_set(ppu_register_set, 1);                         // Enable sprite window mode
	gplib_ppu_sprite_25d_mode_set(ppu_register_set,1);                               // Enable sprite 25D mode
    
    //SP_CellIdx.c changes for gplib_ppu
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
    
    if(C_DISPLAY_DEVICE == C_TV_VGA){
        //image and position init    
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
    }else{
        //image and position init    
        set_sprite_display_init(0,200,100,(INT32U)_wing_CellIdx);
        set_sprite_display_init(1,300,90,(INT32U)_Balloon_CellIdx);
        set_sprite_display_init(2,50,170,(INT32U)_a1_CellIdx);
        set_sprite_display_init(3,30,165,(INT32U)_aa01_CellIdx);
        set_sprite_display_init(4,100,160,(INT32U)_cc01_CellIdx);
        set_sprite_display_init(5,140,120,(INT32U)_bb01_CellIdx);
        set_sprite_display_init(6,200,150,(INT32U)_a1_CellIdx);
        set_sprite_display_init(7,250,140,(INT32U)_aa01_CellIdx);
        set_sprite_display_init(8,300,80,(INT32U)_bb01_CellIdx);
        set_sprite_display_init(9,300,130,(INT32U)_cc01_CellIdx);
        set_sprite_display_init(10,100,110,(INT32U)_J6bbabd36_CellIdx);
        set_sprite_display_init(11,50,60,(INT32U)_J2c5cf9e6_CellIdx);
        set_sprite_display_init(12,400,60,(INT32U)_Je2ecc5fb_CellIdx);
    }
	
	//sprite ram init
	if(C_DISPLAY_DEVICE == C_TV_VGA)
	   paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,LeftTop2Center_coordinate,20);
	else if(C_DISPLAY_DEVICE < C_TV_QVGA && PPU_TEXT_SIZE_HPIXEL == 800 && PPU_TEXT_SIZE_VPIXEL == 480)
	   paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_800X480,LeftTop2Center_coordinate,20);
	else
	   paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_480X234,LeftTop2Center_coordinate,20);
	
	// Start TV/TFT after PPU hardware is initiated  	
    gplib_ppu_text_calculate_number_array(ppu_register_set, C_PPU_TEXT2, 1024, 512, (INT32U)_Text_CellData_3);	// Calculate Number array
    gplib_ppu_text_calculate_number_array(ppu_register_set, C_PPU_TEXT3, 1024, 512, (INT32U)_Text_CellData_1);	// Calculate Number array

	// Start PPU and wait until PPU operation is done
	gplib_ppu_go_and_wait_done(ppu_register_set);
	//gplib_ppu_go_without_wait(ppu_register_set);

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
	
	while(1){
	     //text 4
		 gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT4, 1, 1,blend_level);
		 gplib_ppu_text_rotate_zoom_set(ppu_register_set, C_PPU_TEXT4, Angle, 1);
	     
	     //text 3
	     gplib_ppu_text3_25d_set(ppu_register_set, Angle, Scale_Factor);
	     
	     //Start PPU and wait until PPU operation is done
	     gplib_ppu_go_and_wait_done(ppu_register_set);
	     
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
	     
	     if (++Angle > 359) {
	         Angle = 0;
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
         	  set_sprite_image_number(3,(INT32U)_aa01_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc01_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc01_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a1_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa01_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb01_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb01_CellIdx);
           break;
           case 1:
         	  set_sprite_image_number(2,(INT32U)_a2_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa02_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc02_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc02_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a2_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa02_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb02_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb02_CellIdx);       	  
           break;
           case 2:
         	  set_sprite_image_number(2,(INT32U)_a3_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa03_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc03_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc03_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a3_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa03_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb03_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb03_CellIdx);        	  
           break;
           case 3:
         	  set_sprite_image_number(2,(INT32U)_a4_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa04_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc04_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc04_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a4_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa04_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb04_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb04_CellIdx);         	  
           break;
           case 4:
         	  set_sprite_image_number(2,(INT32U)_a5_CellIdx);
         	  set_sprite_image_number(3,(INT32U)_aa05_CellIdx);
              set_sprite_image_number(4,(INT32U)_cc05_CellIdx);
              set_sprite_image_number(5,(INT32U)_cc05_CellIdx);
         	  set_sprite_image_number(6,(INT32U)_a5_CellIdx);
         	  set_sprite_image_number(7,(INT32U)_aa05_CellIdx);
         	  set_sprite_image_number(8,(INT32U)_bb05_CellIdx);
         	  set_sprite_image_number(9,(INT32U)_bb05_CellIdx);         	    
           break;
         }
         
	     if(C_DISPLAY_DEVICE == C_TV_VGA){
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
	     }else if(C_DISPLAY_DEVICE < C_TV_QVGA  && PPU_TEXT_SIZE_HPIXEL == 800 && PPU_TEXT_SIZE_VPIXEL == 480){
	          SetSpritePosition(0,Sprite_Coordinate_800X480,-4,-2,1);
	          SetSpritePosition(1,Sprite_Coordinate_800X480,3,2,1);
	          SetSpritePosition(2,Sprite_Coordinate_800X480,4,0,1);
	          SetSpritePosition(3,Sprite_Coordinate_800X480,-2,0,1);
	          SetSpritePosition(4,Sprite_Coordinate_800X480,3,0,1);
	          SetSpritePosition(5,Sprite_Coordinate_800X480,6,0,1);
	          SetSpritePosition(6,Sprite_Coordinate_800X480,-2,0,1);
	          SetSpritePosition(7,Sprite_Coordinate_800X480,3,0,1);
	          SetSpritePosition(8,Sprite_Coordinate_800X480,3,0,1);
	          SetSpritePosition(9,Sprite_Coordinate_800X480,-5,0,1);
	          SetSpritePosition(10,Sprite_Coordinate_800X480,2,0,1);
	          SetSpritePosition(11,Sprite_Coordinate_800X480,4,0,1);
	          paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_800X480,LeftTop2Center_coordinate,20);
	     }else{
	          SetSpritePosition(0,Sprite_Coordinate_480X234,-4,-2,1);
	          SetSpritePosition(1,Sprite_Coordinate_480X234,3,2,1);
	          SetSpritePosition(2,Sprite_Coordinate_480X234,4,0,1);
	          SetSpritePosition(3,Sprite_Coordinate_480X234,-2,0,1);
	          SetSpritePosition(4,Sprite_Coordinate_480X234,3,0,1);
	          SetSpritePosition(5,Sprite_Coordinate_480X234,6,0,1);
	          SetSpritePosition(6,Sprite_Coordinate_480X234,-2,0,1);
	          SetSpritePosition(7,Sprite_Coordinate_480X234,3,0,1);
	          SetSpritePosition(8,Sprite_Coordinate_480X234,3,0,1);
	          SetSpritePosition(9,Sprite_Coordinate_480X234,-5,0,1);
	          SetSpritePosition(10,Sprite_Coordinate_480X234,2,0,1);
	          SetSpritePosition(11,Sprite_Coordinate_480X234,4,0,1);	         
	          paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_480X234,LeftTop2Center_coordinate,20);
         }
             
         //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(0,(SpN_ptr *)&sp_ptr);
         gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
         gplib_ppu_sprite_attribute_zoom_set((SpN_RAM *)sp_ptr.nSPNum_ptr, sp_zoom);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);	

         //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(1,(SpN_ptr *)&sp_ptr);
         gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
         gplib_ppu_sprite_attribute_zoom_set((SpN_RAM *)sp_ptr.nSPNum_ptr, sp_zoom);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);	
         
	     //get sprite character number of image and sprite start ptr of sprite ram
         Get_sprite_image_info(2,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     gplib_ppu_sprite_attribute_mosaic_set((SpN_RAM *)sp_num_addr, sp_mosaic);
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
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
	     sp_num_addr+=sizeof(SpN_RAM);
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
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(5,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(6,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(7,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(8,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(9,(SpN_ptr *)&sp_ptr);
         sp_num_addr=sp_ptr.nSPNum_ptr;
	     for(i=0;i<sp_ptr.nSP_CharNum;i++)
	     {
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_num_addr, 3);	
	     sp_num_addr+=sizeof(SpN_RAM);
	     }
	     
	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(10,(SpN_ptr *)&sp_ptr);
         //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);	

	     //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(11,(SpN_ptr *)&sp_ptr);
	     gplib_ppu_sprite_attribute_rotate_set((SpN_RAM *)sp_ptr.nSPNum_ptr,sp_rotate);
	     gplib_ppu_sprite_attribute_window_set((SpN_RAM *)sp_ptr.nSPNum_ptr,1);
	     gplib_ppu_sprite_attribute_depth_set((SpN_RAM *)sp_ptr.nSPNum_ptr,1);
	     //sprite 2D mode
         gplib_ppu_sprite_attribute_flip_set((SpN_RAM *)sp_ptr.nSPNum_ptr, 3);
         
         //get sprite character number of image and sprite start ptr of sprite ram
	     Get_sprite_image_info(12,(SpN_ptr *)&sp_ptr);
         Sprite_pos.x0=350;
         Sprite_pos.y0=80;
         Sprite_pos.x1=440;
         Sprite_pos.y1=20;
         Sprite_pos.x2=400;
         Sprite_pos.y2=130;
         Sprite_pos.x3=300;
         Sprite_pos.y3=180;
         spriteV3D_Pos_Coordinate_Change(Sprite_Coordinate_480X234,LeftTop2Center_coordinate,&Sprite_pos);         
         gplib_ppu_sprite_attribute_25d_position_set((SpN_RAM *)sp_ptr.nSPNum_ptr,&Sprite_pos);	
         
	     OSTimeDly(2);
	     frame_count++;
	}
#endif
}




