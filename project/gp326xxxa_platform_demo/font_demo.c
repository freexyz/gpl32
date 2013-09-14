#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "turnkey_umi_task.h"

INT32U system_palette0[] = {
	0x30844C00, 0x6E94514A, 0x5DAE5E31, 0x733972F7, 0x28467BBD, 0x082259B4, 0x04017FDF, 0x3D91358E,	// 0-15
	0x20AB1888, 0x5238202F, 0x5A795E9A, 0x419562FB, 0x4A1745D6, 0x6F7D290C, 0x355424B1, 0x2D3428D3,	// 16
	0x10686B3C, 0x252B739D, 0x210A671F, 0x1CD1148D, 0x294B148A, 0x4A5C318D, 0x1CCE0845, 0x21172D4E,	// 32
	0x295A14AE, 0x2D552114, 0x001E3196, 0x0017001B, 0x10980010, 0x00011CF9, 0x2D760423, 0x7BDF1086,	// 48
	0x0C73087A, 0x31B814BB, 0x2950252E, 0x39D318C9, 0x211056BE, 0x31912532, 0x423414CB, 0x35F914A6,	// 64
	0x35CF5F1C, 0x39CF56DA, 0x0C853A5A, 0x35D14E97, 0x427619B6, 0x0CCB0CEC, 0x225D5F5E, 0x3EDD1971,	// 80
	0x25B1152F, 0x2DAF0864, 0x6B9F090E, 0x192C2E15, 0x46D9639F, 0x029A2D8D, 0x254A0236, 0x6FDE01B0,	// 96
	0x539D433B, 0x1A111E96, 0x575B3695, 0x43BE77FF, 0x296B233A, 0x337B1319, 0x002167FF, 0x7BFF5BDE,	// 112
	0x364F2149, 0x42910123, 0x08C31165, 0x571546D2, 0x43314F54, 0x02A04B92, 0x370D26C9, 0x63D857B5,	// 128
	0x6FDB5F77, 0x296A6799, 0x7BFE77DD, 0x19C601E0, 0x436E4EF3, 0x2A091A83, 0x33405B55, 0x25831502,	// 144
	0x3E493645, 0x25472DA7, 0x4EAB5380, 0x6B74EFC0, 0x316A7FFE, 0x7FDC3564, 0x6F127B54, 0x7B0D62B0,	// 160
	0x566F2D47, 0x7F974A0D, 0x2D037357, 0x7FBA3567, 0x76AA5692, 0x7E214185, 0x55C56606, 0x49A8560B,	// 176
	0x516141CC, 0x4E2F24E5, 0x146269A1, 0x5EB4398B, 0x66F60821, 0x68E02D4A, 0x20A57FBC, 0x7C007B7A,	// 192
	0x68215C00, 0x28003800, 0x406368A5, 0x6D0848A5, 0x758C0400, 0x6DEF3CE7, 0x7AB57252, 0x7FDE7F39,	// 208
	0x7BDE7FFF, 0x739C77BD, 0x6B5A6F7B, 0x63186739, 0x5AD65EF7, 0x529456B5, 0x4A524E73, 0x42104631,	// 224
	0x39CE3DEF, 0x318C35AD, 0x294A2D6B, 0x21082529, 0x7FE04210, 0x001F03E0, 0x7FFF7C00, 0x80000000 	// 240
};
INT32U system_palette1[] = {
	0x60436021, 0x6085616B, 0x404B7FDE, 0x5E3559B3, 0x4CB049D1, 0x1CA661F6, 0x144454F3, 0x18854931,
	0x5D566618, 0x699A20A8, 0x04016E5B, 0x2CEB0822, 0x733C7F5F, 0x38CE7FDF, 0x75FE769E, 0x49B66AFB,
	0x4175202F, 0x49F64595, 0x56585217, 0x395539CF, 0x39743D75, 0x31333134, 0x41F528D3, 0x2D135EDA,
	0x2D343555, 0x2D5541F7, 0x35B73176, 0x14AE0001, 0x211414AC, 0x2D761CF1, 0x29500422, 0x210918C7,
	0x7BDF318D, 0x3E181CEF, 0x108918CE, 0x46591086, 0x18CB77BE, 0x18C935F8, 0x423335D3, 0x294B35AE,
	0x4A9A0C64, 0x39F014A6, 0x675D3E11, 0x3A7B252A, 0x52B7571D, 0x46544A75, 0x56F95F3B, 0x25B11D70,
	0x5F5E112E, 0x2D8D35D0, 0x090E6B9E, 0x114F08EC, 0x1D4D42FE, 0x1D092614, 0x296C0443, 0x73BD3675,
	0x1E9877DF, 0x3E32331D, 0x1F1B14C6, 0x18E742B6, 0x02740AD8, 0x4BDE0CA5, 0x016B01AD, 0x00C60129,
	0x00420084, 0x212919EF, 0x31CD7BFF, 0x01032A8F, 0x08E30123, 0x05243A70, 0x0D240944, 0x0D65296A,
	0x254946B2, 0x3F30324D, 0x19A6260A, 0x002052F4, 0x10A414C5, 0x19262128, 0x42B05794, 0x5FD731AC,
	0x6FFA4BB0, 0x77DD3A8C, 0x25E77BFE, 0x36296779, 0x63144AAB, 0x6B550420, 0x7FFE39CC, 0x462E6B2E,
	0x779777BA, 0x5AD2318A, 0x14A44A4F, 0x4E707BB8, 0x2D497FDB, 0x0841420D, 0x6AC45291, 0x35AB3DEC,
	0x356745C8, 0x5A4B2925, 0x55E63D44, 0x2528460E, 0x18C520E6, 0x14621083, 0x188428A4, 0x34E64907,
	0x6C00456A, 0x50005C00, 0x6C213C00, 0x6C426821, 0x24426C84, 0x69086CC6, 0x6DAD0400, 0x72100821,
	0x7B3976B5, 0x7FBD7F7B, 0x7BDE7FFF, 0x739C77BD, 0x67396F7B, 0x56B55EF7, 0x4E735294, 0x46314A52,
	0x3DEF4210, 0x35AD39CE, 0x2D6B318C, 0x2529294A, 0x7FE04210, 0x001F03E0, 0x7FFF7C00, 0x80000000
};

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1
#define PPU_TEXT_SIZE_HPIXEL   320//800
#define PPU_TEXT_SIZE_VPIXEL   240//480
#define C_PPU_DRV_FRAME_NUM	     2
#define MAX_SHOW_LINE			30

extern SpN_RAM	SpriteRAM[1024*sizeof(SpN_RAM)];
PPU_REGISTER_SETS turnkey_ppu_register_structure;
PPU_REGISTER_SETS *turnkey_ppu_register_set;
PSPRITE turnkey_sprite_image_head;

SPRITE 	g_sprite_data[MAX_SHOW_LINE];

extern INT8U  Display_Device;

void GPL32_PPU_Init(INT8U TV_TFT, INT8U display_resolution);
void sprite_attr_caculate (PSPRITE add_start, INT8U Sprite_hs, INT8U sprite_vs, INT32U color);
void sprite2D_Pos_Center_to_LeftTop(INT8U display_mode, POS_STRUCT_PTR p_sprite_pos);
void string_add_and_caculate(char *string, PSPRITE p_sprite_data, INT16U postion_x, INT16U postion_y);
void string_free(INT32U string_number);
void show_string(void);
INT32U	adjust_array_pos(INT16U offset_index, INT16U array_x, INT16U array_y);
//==================================================
// font demo code:
// use gprs.pak font library and put the font in sd card 
// show font with ppu
//==================================================
void font_demo(void)
{
	char 	string_info[6][64] = {{"-->"},{"play avi mode"}, {"play jpeg mode"}, {"copy sd folder"}, {"show nand file info"}, {"format nand"}};
	char    string_buffer[64];
	INT8S	mode_index;
	INT16U  x, y, array_x, array_y;
	INT32S  i, nRet;
	PSPRITE p_sprite_data, add_start, add_end;	
	
	while(1)
	{
		if(_devicemount(USE_DISK))             
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
#if USE_DISK == FS_NAND1			
			nRet = DrvNand_lowlevelformat();
		 	DBG_PRINT("DrvNand_lowlevelformat = %d\r\n", nRet);
		 	nRet =  _format(FS_NAND1,FAT32_Type);
		 	DBG_PRINT("format = %d\r\n", nRet);
		 	DrvNand_flush_allblk();	
#endif			
			nRet = _deviceunmount(FS_NAND1);		
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}
		
	tv_init();
	tft_init();
	if(Display_Device)
	{
		if((PPU_TEXT_SIZE_HPIXEL == 800)&&(PPU_TEXT_SIZE_VPIXEL == 480))
			tft_start_with_pwm(HSD070IDW1,1,1);
		else if((PPU_TEXT_SIZE_HPIXEL == 320)&&(PPU_TEXT_SIZE_VPIXEL == 240))
			TFT_TD025THED1_Init();
		//init ppu
		if((PPU_TEXT_SIZE_HPIXEL == 800)&&(PPU_TEXT_SIZE_VPIXEL == 480))
			GPL32_PPU_Init(Display_Device, C_TFT_RESOLUTION_800X480);
		else if((PPU_TEXT_SIZE_HPIXEL == 640)&&(PPU_TEXT_SIZE_VPIXEL == 480))
			GPL32_PPU_Init(Display_Device, C_TFT_RESOLUTION_640X480);
		else if((PPU_TEXT_SIZE_HPIXEL == 320)&&(PPU_TEXT_SIZE_VPIXEL == 240))
			GPL32_PPU_Init(Display_Device, C_TFT_RESOLUTION_320X240);
	}
	else
	{
		if((PPU_TEXT_SIZE_HPIXEL == 640)&&(PPU_TEXT_SIZE_VPIXEL == 480))
			tv_start (TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
		else if((PPU_TEXT_SIZE_HPIXEL == 320)&&(PPU_TEXT_SIZE_VPIXEL == 240))
			tv_start (TVSTD_NTSC_J, TV_QVGA, TV_NON_INTERLACE);
		//init ppu
		if((PPU_TEXT_SIZE_HPIXEL == 640)&&(PPU_TEXT_SIZE_VPIXEL == 480))
			GPL32_PPU_Init(Display_Device, C_TV_RESOLUTION_640X480);
		else if((PPU_TEXT_SIZE_HPIXEL == 320)&&(PPU_TEXT_SIZE_VPIXEL == 240))
			GPL32_PPU_Init(Display_Device, C_TV_RESOLUTION_320X240);
	}
	
	//init key scan
	adc_key_scan_init(); 
	//font init
	do{
		nRet = umi_resource_init();	
	}while(nRet);	

	mode_index = 0;
	  
	// show string_info[0]~string_buffer[5]
	i = 0;
	array_x = 100;
	array_y = 50;
	p_sprite_data = &g_sprite_data[0];
	add_start = p_sprite_data;
	while(1)
	{
		p_sprite_data = &g_sprite_data[i];		
		if(i == 0)
		{
			x = array_x - 32; 
			y = array_y;  
		}
		else
		{
			x = array_x; 
			y = array_y + ((i-1)*16);
		}
		gp_strcpy((INT8S*)string_buffer, (INT8S*)string_info[i]);
		string_add_and_caculate(string_buffer, p_sprite_data, x, y);
		
		i++;
		if(i>=6)
		{
			add_end = p_sprite_data;
			p_sprite_data->next = NULL;
			break;
		}
		p_sprite_data->next = &g_sprite_data[i];
	}
#if GPLIB_PPU_EN	
	gplib_ppu_sprite_image_add_to_list(&turnkey_sprite_image_head, (PSPRITE)add_start, (PSPRITE)add_end);
#endif
	show_string();
	
	while(1)
	{
		adc_key_scan();
		if(ADKEY_IO1)  	
		{	//up 
			mode_index++;
			if(mode_index > 4)
				mode_index = 0;		
			//update sprite
			adjust_array_pos(mode_index, array_x - 32, array_y);
		}
		else if(ADKEY_IO2)
		{	//down
			mode_index--;
			if(mode_index < 0)
				mode_index = 4;
			//update sprite
			adjust_array_pos(mode_index, array_x - 32, array_y);
		}
		else if(ADKEY_IO3)		
		{	
		
		}
		else if(ADKEY_IO4)
		{	//exit 
			string_free(5);
			//clear display
			show_string(); 
			break;
		}
		else if(ADKEY_IO5)
		{
		
		}								
	}
}

void GPL32_PPU_Init(INT8U TV_TFT, INT8U display_resolution)
{
#if GPLIB_PPU_EN	
	INT32U i, j;
	INT32U frame_size, buffer_ptr;
	
	/* initial ppu register parameter set structure */
    turnkey_ppu_register_set = &turnkey_ppu_register_structure;
	gplib_ppu_init(turnkey_ppu_register_set);
	
	gplib_ppu_enable_set(turnkey_ppu_register_set, 1);				// Enable PPU
	gplib_ppu_char0_transparent_set(turnkey_ppu_register_set, 1);	// Enable character 0 transparent mode
	
	 // TV frame mode
  	gplib_ppu_non_interlace_set(turnkey_ppu_register_set, 0);		// Set interlace mode
  	gplib_ppu_frame_buffer_mode_set(turnkey_ppu_register_set, 1, 0);// Enable TV/TFT frame buffer mode
	gplib_ppu_fb_format_set(turnkey_ppu_register_set, 1, 1);		// Set PPU output frame buffer format to YUYV
	
	if(TV_TFT) //tft
	{
    	gplib_ppu_vga_mode_set(turnkey_ppu_register_set, 0);					// Disable VGA mode
		gplib_ppu_resolution_set(turnkey_ppu_register_set, display_resolution);	// Set display resolution to 320x240
    }
    else	//TV
    {
    	if(display_resolution == C_TV_RESOLUTION_320X240)
    	{
    		gplib_ppu_vga_mode_set(turnkey_ppu_register_set, 0);
    		gplib_ppu_resolution_set(turnkey_ppu_register_set, C_TV_RESOLUTION_320X240);	
    	}
    	else if(display_resolution == C_TV_RESOLUTION_640X480)
    	{
    		gplib_ppu_vga_mode_set(turnkey_ppu_register_set, 1);
    		gplib_ppu_resolution_set(turnkey_ppu_register_set, C_TV_RESOLUTION_640X480);
    	}
    }
    
    gplib_ppu_bottom_up_mode_set(turnkey_ppu_register_set, 1);      //bottom to top
    gplib_ppu_palette_type_set(turnkey_ppu_register_set, 0, 1);     //text and sprite palette mode
    gplib_ppu_text_direct_mode_set(turnkey_ppu_register_set, 0);	// Enable TEXT direct address mode
    gplib_ppu_deflicker_mode_set(0);			            		// De-flicker is only valid under VGA + Interlace + Frame mode	
    gplib_ppu_yuv_type_set(turnkey_ppu_register_set, 3);			// Set 32-bit color format to Y1UY0V
    
    frame_size = PPU_TEXT_SIZE_HPIXEL * PPU_TEXT_SIZE_VPIXEL * 2; 
    for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) 
    {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		DBG_PRINT("ppu_frame = 0x%x\r\n", buffer_ptr);
		if (buffer_ptr) 
		{
	    	INT32U *ptr;
	    	ptr = (INT32U *) buffer_ptr;
	    	for (j=0; j<frame_size; j++) 
	    	{
	    	    *ptr++ = 0x00800080;              // Black for YUYV color mode
	    	}
			gplib_ppu_frame_buffer_add(turnkey_ppu_register_set, buffer_ptr);
		} 
		else 
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
	}
	
	/*  //init text1 
	number_array_ptr = (INT32U) gp_malloc(1024*4);	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	if (!number_array_ptr) {
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
	}

	gplib_ppu_text_enable_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(turnkey_ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(turnkey_ppu_register_set, 0);			                // Enable TEXT direct address mode
    gplib_ppu_text_attribute_source_select(turnkey_ppu_register_set, C_PPU_TEXT1, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(turnkey_ppu_register_set, C_PPU_TEXT1, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);				    // Set TEXT segment address	
	gplib_ppu_text_number_array_ptr_set(turnkey_ppu_register_set, C_PPU_TEXT1, number_array_ptr); // Set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(turnkey_ppu_register_set, C_PPU_TEXT1, 1, 3);				     // Set TEXT color to YUYV 
	gplib_ppu_text_mode_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0);                       // TEXT 2D
	gplib_ppu_text_position_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0, 0);
	gplib_ppu_text_offset_set(turnkey_ppu_register_set, C_PPU_TEXT1, 0, 0);
	*/
	
	/* show sprite */
	gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 0, (INT32U) &system_palette0[0]);	// Palette 0(TEXT), bank:0(palette0)~3(palette3), value: 32-bit address of palette ram buffer
    gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 1, (INT32U) &system_palette0[0]);	// Palette 1(Sprite)
    gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 2, (INT32U) &system_palette1[0]);	// Palette 2(TEXT)
    gplib_ppu_palette_ram_ptr_set(turnkey_ppu_register_set, 3, (INT32U) &system_palette1[0]);	// Palette 3(Sprite)
    
	gplib_ppu_sprite_init(turnkey_ppu_register_set);
	gplib_ppu_sprite_enable_set(turnkey_ppu_register_set, 1);			// Enable Sprite
	gplib_ppu_sprite_coordinate_set(turnkey_ppu_register_set, 0);
	gplib_ppu_sprite_direct_mode_set(turnkey_ppu_register_set, 1);		// Set sprite direct address mode
	gplib_ppu_sprite_number_set(turnkey_ppu_register_set, 128);			
	gplib_ppu_sprite_attribute_ram_ptr_set(turnkey_ppu_register_set, (INT32U)SpriteRAM);	//set sprite ram buffer
#endif
}

void sprite_attr_caculate (PSPRITE add_start, INT8U Sprite_hs, INT8U sprite_vs, INT32U color)
{
	INT8U  vs, hs, cor;
	INT32U attr0;
	INT32U attr1;
	
	switch(Sprite_hs)
	{
		case 8:		hs = 0;	break;
		case 16:	hs = 1;	break;
		case 32:	hs = 2;	break;
		case 64:	hs = 3;	break;
	}
	switch(sprite_vs)
	{
		case 8:		vs = 0;	break;
		case 16:	vs = 1;	break;
		case 32:	vs = 2;	break;
		case 64:	vs = 3;	break;
	}
	
	switch(color)
	{
		case 4:		cor = 0;	break;
		case 16:	cor = 1;	break;
		case 64:	cor = 2;	break;
		case 256:	cor = 3;	break;
	
	}
	
	attr0 = cor& 0x03 | (hs& 0x03)<<4 | (vs& 0x03)<<6;
	attr1 = 0x0;
	
	add_start->attr0 = attr0;
	add_start->attr1 = attr1;
}

void sprite2D_Pos_Center_to_LeftTop(INT8U display_mode, POS_STRUCT_PTR p_sprite_pos)
{
    INT32S nPosXN,nPosYN,temp_x,temp_y,nScrWidth,nScrHeight;
    
    switch(display_mode)
    {
       case 0x00://Sprite_Coordinate_320X240:
            nScrWidth = 320;
		    nScrHeight = 240 + 14;
            break;
       case 0x01://Sprite_Coordinate_640X480:
       		nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;         
       case 0x02://Sprite_Coordinate_480X234:
       		nScrWidth = 480;
		    nScrHeight = 234;       
            break;
       case 0x03://Sprite_Coordinate_480X272:
            nScrWidth = 480;
		    nScrHeight = 272;  
            break;
       case 0x04://Sprite_Coordinate_720X480:
       		nScrWidth = 720;
		    nScrHeight = 480;         
            break;     
       case 0x05://Sprite_Coordinate_800X480:
       		nScrWidth = 800;
		    nScrHeight = 480;         
            break;         
       case 0x06://Sprite_Coordinate_800X600:
       		nScrWidth = 800;
		    nScrHeight = 600;         
            break;
       case 0x07://Sprite_Coordinate_1024X768:
       		nScrWidth = 1024;
		    nScrHeight = 768;         
            break;
       default:
            nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;      
    }
    //x0,y0
	temp_x=p_sprite_pos->x0;
    temp_y=p_sprite_pos->y0; 
    nPosXN = temp_x + 0 - (nScrWidth>>1) /*+ (sprite_H>>1)*/ + 1;		//roy 20090121
    nPosYN = (nScrHeight>>1) - 1 - temp_y + 0 /*- (sprite_V>>1)*/;		//roy 20090121
    p_sprite_pos->x0=nPosXN;
    p_sprite_pos->y0=nPosYN;
}

void string_add_and_caculate(char *string, PSPRITE p_sprite_data, INT16U postion_x, INT16U postion_y)
{
	StrinsCentersDrawPar loading_image_hint_parameter = {PPU_TEXT_SIZE_HPIXEL, PPU_TEXT_SIZE_VPIXEL, 253, 255};
	GraphicBitmap bitmap;
	POS_STRUCT  sprite_postion;
	
	cl1_graphic_strings_centers_draw(&loading_image_hint_parameter, &bitmap, (CHAR *) string, 1, 1);
	p_sprite_data->image_width = bitmap.bitmap_extend_x_size;
	p_sprite_data->image_height = bitmap.bitmap_extend_y_size;
	p_sprite_data->image_data_ptr = (INT32U)bitmap.bitmap_databuf;
	p_sprite_data->attr_ptr = NULL;
	
	sprite_postion.x0 = postion_x;
	sprite_postion.y0 = postion_y;
	if((PPU_TEXT_SIZE_HPIXEL == 800) && (PPU_TEXT_SIZE_VPIXEL == 480))
		sprite2D_Pos_Center_to_LeftTop(5, &sprite_postion);
	else if((PPU_TEXT_SIZE_HPIXEL == 640) && (PPU_TEXT_SIZE_VPIXEL == 480))
		sprite2D_Pos_Center_to_LeftTop(1, &sprite_postion);
	else if((PPU_TEXT_SIZE_HPIXEL == 320) && (PPU_TEXT_SIZE_VPIXEL == 240))
		sprite2D_Pos_Center_to_LeftTop(0, &sprite_postion);
		
	p_sprite_data->pos.x0 = sprite_postion.x0;
	p_sprite_data->pos.y0 = sprite_postion.y0;
	p_sprite_data->rotate = 0;
	p_sprite_data->zoom = 0;
	sprite_attr_caculate((PSPRITE)p_sprite_data, bitmap.bitmap_cell_x_size, bitmap.bitmap_cell_y_size, 256); //color = 256
}

void string_free(INT32U string_number)
{
#if GPLIB_PPU_EN
	INT32U  i;
	PSPRITE add_start, add_end;	
	
	add_start = &g_sprite_data[0];
	add_end = &g_sprite_data[string_number - 1];
	gplib_ppu_sprite_image_remove_from_list(&turnkey_sprite_image_head, add_start, add_end);
	
	for(i = 0;i < string_number; i++)
		gp_free((void *)g_sprite_data[i].image_data_ptr);
#endif
}

void show_string(void)
{
#if GPLIB_PPU_EN
	gplib_ppu_sprite_image_to_attribute(turnkey_ppu_register_set, turnkey_sprite_image_head);
	gplib_ppu_sprite_image_charnum_calculate(turnkey_ppu_register_set, turnkey_sprite_image_head);
	gplib_ppu_sprite_image_position_calculate(turnkey_ppu_register_set, turnkey_sprite_image_head);
    gplib_ppu_go_and_wait_done(turnkey_ppu_register_set);
#endif
} 

INT32U	adjust_array_pos(INT16U offset_index, INT16U array_x, INT16U array_y)
{ 
#if GPLIB_PPU_EN
	POS_STRUCT sprite_postion;
	
	sprite_postion.x0 = array_x;
	sprite_postion.y0 = array_y + offset_index * 16;
	
	if((PPU_TEXT_SIZE_HPIXEL == 800) && (PPU_TEXT_SIZE_VPIXEL == 480))
		sprite2D_Pos_Center_to_LeftTop(5, &sprite_postion);
	else if((PPU_TEXT_SIZE_HPIXEL == 640) && (PPU_TEXT_SIZE_VPIXEL == 480))
		sprite2D_Pos_Center_to_LeftTop(1, &sprite_postion);
	else if((PPU_TEXT_SIZE_HPIXEL == 320) && (PPU_TEXT_SIZE_VPIXEL == 240))
		sprite2D_Pos_Center_to_LeftTop(0, &sprite_postion);
		
	turnkey_sprite_image_head->pos.x0 = sprite_postion.x0;
	turnkey_sprite_image_head->pos.y0 = sprite_postion.y0;
	
	gplib_ppu_sprite_image_position_calculate(turnkey_ppu_register_set, turnkey_sprite_image_head);
    gplib_ppu_go_and_wait_done(turnkey_ppu_register_set);
#endif    	
    return 0;
}

