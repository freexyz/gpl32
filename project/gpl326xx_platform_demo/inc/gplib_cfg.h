#ifndef __GPLIB_CFG_H__
#define __GPLIB_CFG_H__



//=========================Engineer Mode ("Not" defined CUSTOMER_DEFINE)=====================//
#ifndef CUSTOMER_DEFINE  //For Engineer Development and test                                 //
//===========================================================================================//
    // Memory management module
    #define GPLIB_MEMORY_MANAGEMENT_EN		1
		#define C_MM_IRAM_LINK_NUM			20
		#define C_MM_16BYTE_NUM				32
		#define C_MM_64BYTE_NUM				16
		#define C_MM_256BYTE_NUM			12
		#define C_MM_1024BYTE_NUM			18
		#define C_MM_4096BYTE_NUM			30

    // File system module
    #define	GPLIB_FILE_SYSTEM_EN			1

    // JPEG image encoding/decoding module
    #define GPLIB_JPEG_ENCODE_EN			1
    #define GPLIB_JPEG_DECODE_EN			1

    // Progressive JPEG image decoding module
    #define GPLIB_PROGRESSIVE_JPEG_EN		1
		
	// JPEG UNION MODE
	#define JPEG_DEC_UNION_EN				0
		
    // GPZP image decoding module
    #define GPLIB_GPZP_EN					1

    // GIF image decoding module
    #define GPLIB_GIF_EN                    1

    // BMP image decoding module
    #define GPLIB_BMP_EN                    1

    // PPU control module
    #define GPLIB_PPU_EN                    1

    // SPU control module
    #define GPLIB_SPU_EN					1
    
	//MP3 H/W decode 
	#define GPLIB_MP3_HW_EN					1

    // NVRAM Support
    #define GPLIB_NVRAM_EN                  1

    // SPI flash read/write module
    #define GPLIB_NVRAM_SPI_FLASH_EN        1

    // Print and get string
    #define GPLIB_PRINT_STRING_EN           1
    #define PRINT_BUF_SIZE                  512

    #define GPLIB_CALENDAR_EN               0

    #define SUPPORT_MIDI_LOAD_FROM_NVRAM    0

    #define SUPPORT_MIDI_READ_FROM_SDRAM    0
    
    #define SUPPORT_STG_SUPER_PLUGOUT       0
    
    #define SUPPORT_MIDI_PLAY_WITHOUT_TASK	0
    
    #define APP_HEADER_V20					0
    /*=== USBH detection mode ===*/
    #define USBH_DP_D   	0x0001
    #define USBH_GPIO_D 	0x0002
    #define USBH_GPIO_D_1	0x0003
    #define USBH_SWITCH_D 	0x0004
    #define USBH_GPIO_D_2 	0x0005
    #if((BOARD_TYPE == BOARD_TK_V4_3) || (BOARD_TYPE ==BOARD_TK_A_V1_0) || (BOARD_TYPE == BOARD_TK_V4_4))
      #define USBH_DETECTION_MODE USBH_GPIO_D
    #else
      #define USBH_DETECTION_MODE USBH_DP_D
    #endif
//======================== Engineer Mode END ================================================//
#else                              //Engineer Difne area END                                 //
//===========================================================================================//

//=========================Customer Mode (Defined CUSTOMER_DEFINE)===========================//
//                    //For Release to customer, follow the command of "customer.h"          //
//======================== Not Modify in this Area ==========================================//

//========================= CUSTOMER DEFINE END =============================================//
#endif  // not CUSTOMER_DEFINE                                                               //
//========================= CUSTOMER DEFINE END =============================================//





#ifndef CUSTOMER_DEFINE  /* Engineer Mode */
  #ifndef _FS_DEFINE_
  #define _FS_DEFINE_
    #define FS_NAND1    0
    #define FS_NAND2    1
    #define FS_SD       2
    #define FS_MS       3
    #define FS_CF       4
    #define FS_SD1      5
    #define FS_XD       5
    #define FS_USBH     6
    #define FS_NOR      7
    #define FS_CDROM    8
    #define FS_NAND_APP 9	
    #define FS_RAMDISK  12
    #define FS_DEV_MAX  10	//13 if needs to run RAM disk
/* DEV MODE */
    #define MSC_EN            1
    #define SD_EN             1
    #define CFC_EN            0
	#define NAND_APP_EN		  0  
    #define NAND1_EN          1
    #define NAND2_EN          0
    #define NAND3_EN          0
    #define USB_NUM           1
    #define NOR_EN            0
    #define XD_EN             0
    #define RAMDISK_EN        0
    #define CDROM_EN	      1
    //#define MAX_DISK_NUM     (MSC_EN + SD_EN + CFC_EN + NAND1_EN + NAND2_EN + NAND3_EN + USB_NUM + NOR_EN + RAMDISK_EN)
    #define MAX_DISK_NUM      FS_DEV_MAX
  #endif // _FS_DEFINE_
  #define NVRAM_V1    1
  #define NVRAM_V2    2
  #define NVRAM_V3    3
  #define NVRAM_MANAGER_FORMAT    NVRAM_V3
  
#else /* Customer Mode */
   
#endif



#endif 		// __GPLIB_CFG_H__
