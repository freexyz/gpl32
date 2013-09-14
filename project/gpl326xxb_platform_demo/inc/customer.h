#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

/* Leading Define Start */

#define CUSTOM_ON       1
#define CUSTOM_OFF      0

#define CUSTOMER_H_ENABLE CUSTOM_OFF  /* decision the customer define mode on/off */

//#if CUSTOMER_H_ENABLE == CUSTOM_ON
//#define CUSTOMER_DEFINE
//#endif

#if 1

/*=== IC Version definition choices ===*/
//---------------------------------------------------------------------------
#define	GPL32_A                     0x1                                    //
#define	GPL32_B                     0x2                                    //
#define	GPL32_C                     0x3                                    //
#define	GPL326XX                    0x10                                   //
#define GPL326XX_B                  0x11                                   //
#define GPL326XX_C                  0x12                                   //
#define GPL326XXB					0x17                                   //
#define GP326XXXA                   0x18
#define GPL327XX                  	0x20
#define GP327XXXA					0x21                                   //
#define MCU_VERSION             	GPL326XXB                              //
//---------------------------------------------------------------------------


/*=== Board ID Config ===*/
//---------------------------------------------------------------------------
#define BOARD_TURNKEY_BASE      0x10000000                                 //
#define BOARD_EMU_BASE          0x00000000                                 //
                                                                           //
#define BOARD_EMU_V1_0          (BOARD_EMU_BASE + 0x10)                    //
#define BOARD_EMU_V2_0          (BOARD_EMU_BASE + 0x20)                    //
#define BOARD_DEMO_GPL32XXX 	(BOARD_EMU_BASE + 0x30)
#define BOARD_DEMO_GPL326XX 	(BOARD_EMU_BASE + 0x40)
#define BOARD_DEMO_GPL327XX 	(BOARD_EMU_BASE + 0x50)
#define BOARD_DEMO_GPL326XXB 	(BOARD_EMU_BASE + 0x60)
#define BOARD_DEMO_GP326XXXA 	(BOARD_EMU_BASE + 0x70)
#define BOARD_DEMO_GP327XXXA 	(BOARD_EMU_BASE + 0x80)
#define BOARD_TK35_V1_0         (BOARD_TURNKEY_BASE + 0x10)                //
#define BOARD_TK_V1_0           (BOARD_TURNKEY_BASE + 0x20)                //
#define BOARD_TK_V2_0           (BOARD_TURNKEY_BASE + 0x30)                //
#define BOARD_TK_V3_0           (BOARD_TURNKEY_BASE + 0x40)                //
#define BOARD_TK_V4_0           (BOARD_TURNKEY_BASE + 0x50)                //
#define BOARD_TK_V4_1           (BOARD_TURNKEY_BASE + 0x60)                //
#define BOARD_TK_V4_3           (BOARD_TURNKEY_BASE + 0x70)                //
#define BOARD_TK_A_V1_0         (BOARD_TURNKEY_BASE + 0x80)                //
#define BOARD_GPY0200_EMU_V2_0  (BOARD_TURNKEY_BASE + 0x90)                //
#define BOARD_TK_V4_4           (BOARD_TURNKEY_BASE + 0xA0)                //
#define BOARD_TK_A_V2_0         (BOARD_TURNKEY_BASE + 0xB0)                //
#define BOARD_TK_A_V2_1_5KEY    (BOARD_TURNKEY_BASE + 0xC0)                //
#define BOARD_TK_7D_V1_0_7KEY   (BOARD_TURNKEY_BASE + 0xD0)                //
#define BOARD_TK_8D_V1_0        (BOARD_TURNKEY_BASE + 0xE0)                //
#define BOARD_TK_8D_V2_0        (BOARD_TURNKEY_BASE + 0xF0)                //
#define BOARD_TK_32600_V1_0     (BOARD_TURNKEY_BASE + 0x100)               //
#define BOARD_TK_32600_COMBO    (BOARD_TURNKEY_BASE + 0x110)               //
#define BOARD_TK_32600_7A_V1_0  (BOARD_TURNKEY_BASE + 0x120)               //
#define BOARD_TYPE              BOARD_DEMO_GPL326XXB
//---------------------------------------------------------------------------

/*=== software ID Config ===*/
//---------------------------------------------------------------------------
#define SOFTWARE_VER            "v1.6.0.0000.0000"                        //
//---------------------------------------------------------------------------

/*=== Key support ===*/
//---------------------------------------------------------------------------
#define KEY_AD_NONE             0
#define KEY_AD_4_MODE           4                                          //
#define KEY_AD_5_MODE           5                                          //
#define KEY_AD_6_MODE           6                                          //
#define KEY_AD_8_MODE           8                                          //
#define KEY_IO_MODE             2                                          //
#define KEY_USER_MODE           3                                          //
#define SUPPORT_AD_KEY          KEY_AD_8_MODE                              //
//---------------------------------------------------------------------------

/*=== web cam ===*/
//---------------------------------------------------------------------------
#define C_UVC						CUSTOM_ON
#define C_USB_AUDIO					CUSTOM_OFF
//---------------------------------------------------------------------------

//rubber key status when TV out
//---------------------------------------------------------------------------
#define TV_OUT_KEY_DISABLE				0                                  //
#define TV_OUT_KEY_AD_4_MODE        	1                                  //
#define TV_OUT_KEY_AD_5_MODE      	    2                                  //
#define TV_OUT_KEY_AD_6_MODE   		    3                                  //
#define SUPPORT_TV_OUT_AD_KEY          	TV_OUT_KEY_DISABLE   		       //
//---------------------------------------------------------------------------

/*=== I-touch support ===*/
//---------------------------------------------------------------------------
#define ITOUCH_NONE             0                                          //
//choice one                                                               //
#define SUPPORT_ITOUCH          ITOUCH_NONE                                //
//---------------------------------------------------------------------------

/*=== rotate sensor support ===*/
//---------------------------------------------------------------------------
#define ROTATE_LEFT_ON	                1                                  //
#define ROTATE_RIGHT_ON	                2                                  //
#define SUPPORT_ROTATE_SENSOR   CUSTOM_ON                                 //
#define SUPPORT_ROTATE_MODE   	ROTATE_LEFT_ON                             //
//---------------------------------------------------------------------------

/*=== App Slide show definition choices ===*/
//---------------------------------------------------------------------------
#define SLIDE_SHOW_M0       0   //motion slide mode                        //
#define SLIDE_SHOW_M1       1   //singal slide mode                        //
#define SLIDE_SHOW_M2       2                                              //
#define SLIDE_SHOW_M3       3                                              //
#define SLIDE_SHOW_LIB      (-1)                                           //
                                                                           //
#define SLIDE_SHOW_MODE     SLIDE_SHOW_M2                                  //
//---------------------------------------------------------------------------

/*=== slide show date on or off ==*/
//---------------------------------------------------------------------------
#define	SLIDE_SHOW_PHOTO_DATE_ON	1                                      //
#define	SLIDE_SHOW_PHOTO_DATE_OFF	0                                      //
#define	SUPPORT_SLIDE_SHOW_PHOTO_DATE_SWITCH	SLIDE_SHOW_PHOTO_DATE_ON   //
//---------------------------------------------------------------------------

/*=== App media mute function mode choices ===*/
//---------------------------------------------------------------------------
#define MEDIA_MUTE_ON		0	//break the media state, mute status keep
#define MEDIA_MUTE_OFF		1	//break the media state, mute status don't keep  	
#define MUTE_FUNCTION_MODE	MEDIA_MUTE_ON  
//---------------------------------------------------------------------------

/*=== App calendar definition choices ===*/
//---------------------------------------------------------------------------
#define CALENDAR_NONE       0 //no calendar mode                           //
#define CALENDAR_M0         1 //luxury clock mode 800x600 or 800x480 or 320x240    //
#define CALENDAR_M1         2 //picture rotate 90'						   //
//choice one                                                               //
#define CALENDAR_MODE       CALENDAR_M0                                    //
//---------------------------------------------------------------------------

/*=== App OSD menu definition choices ===*/
//---------------------------------------------------------------------------
/*Mode Rule:  EVEN-(TVOUT SUPPORT) / ODD-(TFT ONLY) */                     //
#define OSD_MENU_M0         0  	// TFT(OSDSTYLE)+TV(DIRSTYLE)              //
#define OSD_MENU_M1         1  	// TFT(OSDSTYLE)                           //
#define OSD_MENU_M2         2  	// TFT(DIRSTYLE)+TV(DIRSTYLE)              //
#define OSD_MENU_M3         3  	// TFT(DIRSTYLE)                           //
//choice one                                                               //
#define OSD_MENU_MODE       OSD_MENU_M3                                    //
                                                                           //
//---------------------------------------------------------------------------

/*=== App OSD menu hint definition choices ===*/
//---------------------------------------------------------------------------
#define OSD_MENU_HINT_OFF		0  //user can't use osd hint function      //
#define OSD_MENU_HINT_ON		1  //user can use osd hint function        //
//choice one                                                               //
#define OSD_MENU_HINT_MODE	OSD_MENU_HINT_OFF                              //
//---------------------------------------------------------------------------

/*=== App OSD menu setup state hint definition choices ===*/
//---------------------------------------------------------------------------
#define OSD_MENU_SETUP_HINT_OFF	0  //user can't use osd hint function      //
#define OSD_MENU_SETUP_HINT_ON	1  //user can use osd hint function        //
//choice one                                                               //
#define OSD_MENU_SETUP_HINT_MODE OSD_MENU_SETUP_HINT_ON                    //
//---------------------------------------------------------------------------

/*=== App Item Selection definition choices ===*/
//---------------------------------------------------------------------------
#define ITEM_SELECTION_NONE          0                                      //
#define ITEM_SELECTION_M0            1                                      //
#define ITEM_SELECTION_M1            2                                      //
//choice one                                                                //
#define ITEM_SELECTION_MODE        ITEM_SELECTION_M1                        //
//---------------------------------------------------------------------------
/*=== App PET ===*/
//---------------------------------------------------------------------------
#define PET_NONE       	    0                                              //
#define PET_AQUARIUM        1                                              //
#define PET_MODE            PET_AQUARIUM                                   //
//---------------------------------------------------------------------------

/*=== App ESheep ===*/
//---------------------------------------------------------------------------
#define GP_ESHEEP_DISABLE       0	                                       //
#define GP_ESHEEP_ENABLE        1 										   //
#define GP_ESHEEP_MODE			GP_ESHEEP_DISABLE						   //
//---------------------------------------------------------------------------

/*=== App Parrot ===*/
//---------------------------------------------------------------------------
#define E_PARROT_DISABLE       	0	                                       //
#define E_PARROT_ENABLE        	1 										   //
#define E_PARROT_MODE			E_PARROT_DISABLE						   //
//---------------------------------------------------------------------------

/*=== App Calendar ===*/
//---------------------------------------------------------------------------
#define RESOURCE_SHRINK_ON				1								   //
#define RESOURCE_SHRINK_OFF				0								   //
#define GP_CALENDAR_SHRINK_MODE			RESOURCE_SHRINK_OFF				   //
//---------------------------------------------------------------------------

/*=== App Vedio ===*/
//---------------------------------------------------------------------------
#define CONTINUOUS_PLAY_ON				1								   //
#define CONTINUOUS_PLAY_OFF				0								   //
#define AVI_PLAY_MODE					CONTINUOUS_PLAY_OFF				   //
																		   //
#define VOLUME_CONTROL_STATE_ON			1								   //
#define VOLUME_CONTROL_STATE_OFF		0								   //
#define VOLUME_CONTROL_STATE_MODE		VOLUME_CONTROL_STATE_OFF		   //
//---------------------------------------------------------------------------

/*=== App File Scan Style ===*/
//---------------------------------------------------------------------------
#define FS_SCAN_COMBINATION         0 //{(photo+video),audio}              //
#define FS_SCAN_SEPARATION          1 //{photo,video,audio}                //
#define FS_SCAN_STYLE               FS_SCAN_COMBINATION                    //
//---------------------------------------------------------------------------

/*=== card plug in/out mode ===*/
//---------------------------------------------------------------------------
#define	CARD_PLUG_M0        0 //pop up select card windows                 //
#define	CARD_PLUG_M1        1 //use message windows                        //
//choice one                                                               //
#define	CARD_PLUG_MODE      CARD_PLUG_M1                                   //
//---------------------------------------------------------------------------

/*=== file name language support ===*/
//---------------------------------------------------------------------------
#define S_CHINESE_FILE_NAME         CUSTOM_OFF                             //
#define T_CHINESE_FILE_NAME         CUSTOM_OFF                             //
//---------------------------------------------------------------------------

/*=== Storage Devices select ===*/
//---------------------------------------------------------------------------
#define STG_MODE_0              0   //STG_SD_MSC                           //
#define STG_MODE_1              1   //STG_SD_MSC_USB                       //
#define STG_MODE_2              2   //STG_SD_MSC_XD_USB                    //
#define STG_MODE_3              3   //STG_SD_MSC_NAND1_USB                 //
#define STG_MODE_4              4   //STG_SD_MSC_NAND1_XD_USB              //
#define STG_MODE_5              5   //STG_SD_MSC_NAND1_CF_XD_USB           //
#define STG_MODE_6              6   //STG_SD_MSC_NAND1_NAND2_CF_XD_USB     //
#define STG_MODE_7              7   //STG_SD_MSC_CF_XD_USB                 //
#define STG_MODE_8              8   //STG_SD				               //
#define STG_MODE_9              9   //STG_SD_USB			               //
#define STG_MODE_10             10  //STG_AUTORUN_NAND1                    //
#define STG_MODE_11             11  //STG_SD_MSC_NOR_USB                   //
#define STG_MODE_12             12  //STG_SD_MSC_NAND1_USB_CDROM           //
#define STG_MODE_13             13  //STG_SD_MSC_CDROM                     //                                                      //
#define STG_MODE_14             14  //STG_SD_MSC_NAND1_NAND2(hiden)_USB    //
//choice one                                                               //
#define STG_SUPPORT_MODE        STG_MODE_3                                 //
//---------------------------------------------------------------------------

/*=== BOOT and Resource Device select ===*/
//---------------------------------------------------------------------------
#define DEVICE_SPI              1                                          //
#define DEVICE_NAND             2                                          //
#define DEVICE_NOR              3                                          //
#define DEVICE_SD               4                                          //
//choice one                                                               //
#define BOOT_MODE               DEVICE_NAND                                 //
#define RESOURCE_DEV_TYPE       DEVICE_NAND                           	   //
//---------------------------------------------------------------------------

/*=== DEBUG Devices ===*/
//---------------------------------------------------------------------------
#define SUPPORT_JTAG            CUSTOM_ON                                  //
#define CONSOLE_DBG_MSG         CUSTOM_ON                                  //
                                                                           //
//enable UART , if console mode debug need                                 //
#if (CONSOLE_DBG_MSG==CUSTOM_ON)                                           //
	#define SUPPORT_UART            CUSTOM_ON                              //
#elif (CONSOLE_DBG_MSG==CUSTOM_OFF)                                        //
	#define SUPPORT_UART            CUSTOM_OFF                             //
#endif                                                                     //
//---------------------------------------------------------------------------

/*=== audio codec support ===*/
//---------------------------------------------------------------------------
#define SUPPORT_AUD_DECODE_G_MIDI   CUSTOM_ON                              //
#define SUPPORT_AUD_DECODE_MP3      CUSTOM_ON                              //
#define SUPPORT_AUD_DECODE_WMA      CUSTOM_ON                              //
#define SUPPORT_AUD_DECODE_WAV      CUSTOM_ON                              //
#define SUPPORT_AUD_RECODER         CUSTOM_ON                              //
//---------------------------------------------------------------------------

/*=== charge module support ===*/
//---------------------------------------------------------------------------
#define SUPPORT_CHARGE_MODULE	   CUSTOM_OFF                               //
//---------------------------------------------------------------------------

/*=== alarm function configuration ===*/
//---------------------------------------------------------------------------
#define _ALARM_SETTING	CUSTOM_ON                                          //
//---------------------------------------------------------------------------

/*=== powertime function configuration ===*/
//---------------------------------------------------------------------------
#define _POWERTIME_SETTING CUSTOM_ON                                       //
//---------------------------------------------------------------------------

/* GPY02xx support */
//---------------------------------------------------------------------------
#define GPY02XX_NONE      0                                                //
#define BODY_GPY0200      1                                                //
#define BODY_GPY0201      2                                                //
#define BODY_GPY0202      3                                                //
//choice one                                                               //
#define SUPPORT_GPY02XX   GPY02XX_NONE                                     //
//---------------------------------------------------------------------------

/* support external circuit for adapter plug in/out detection */
#define SUPPORT_EXT_ADP_DETECTION  CUSTOM_OFF


/*=== IR support ===*/
//---------------------------------------------------------------------------
#define IR_NONE                 0                                          //
#define IR_TYPE_1               1  /* dpf turnkey ir map 1 */              //
//choice one                                                               //
#define SUPPORT_IR             	IR_TYPE_1                                  //
//---------------------------------------------------------------------------

/* IR wakeup support */
//---------------------------------------------------------------------------
#if SUPPORT_IR == IR_NONE                                                  //
	#define SUPPORT_IR_WAKEUP CUSTOM_OFF                                   //
#else                                                                      //
	#define SUPPORT_IR_WAKEUP CUSTOM_OFF                                    //
#endif                                                                     //
//---------------------------------------------------------------------------

/* HARDWARE IR support */
//---------------------------------------------------------------------------
#define SUPPORT_HARDWARE_IR 		CUSTOM_OFF                              //
//---------------------------------------------------------------------------

/*=== multinational language support ===*/
//---------------------------------------------------------------------------
#define DPF_SUPPORT_ENGLISH   		CUSTOM_ON 						   	   //
#define DPF_SUPPORT_SCHINESE  		CUSTOM_ON	                           //
#define DPF_SUPPORT_TCHINESE  		CUSTOM_ON	                           //
#define DPF_SUPPORT_JAPANESE   		CUSTOM_ON                             //
#define DPF_SUPPORT_FRENCH    		CUSTOM_ON                             //
#define DPF_SUPPORT_ITALIAN   		CUSTOM_ON                             //
#define DPF_SUPPORT_SPANISH   		CUSTOM_ON                             //
#define DPF_SUPPORT_PORTUGUESE		CUSTOM_ON                             //
#define DPF_SUPPORT_RUSSIAN   		CUSTOM_ON                             //
#define DPF_SUPPORT_KOREAN   		CUSTOM_ON                             //
#define DPF_SUPPORT_DUTCH		   	CUSTOM_ON                             //
#define DPF_SUPPORT_LAN12			CUSTOM_OFF                             //
#define DPF_SUPPORT_LAN13			CUSTOM_OFF                             //
#define DPF_SUPPORT_LAN14			CUSTOM_OFF                             //
#define DPF_SUPPORT_LAN15			CUSTOM_OFF                             //
//---------------------------------------------------------------------------
#define SUPPORT_UNICODE_FILE_NAME   CUSTOM_OFF

/*=== time interval ===*/
//---------------------------------------------------------------------------
#define SUPPORT_TIME_INTERVAL_M0    0	// 5~10 second both slide show and calender
#define SUPPORT_TIME_INTERVAL_M1    1	// more-select separateness        //
#define SUPPORT_TIME_INTERVAL_M2    2	// more-select synchronization     //
#define SUPPORT_TIME_INTERVAL_MODE SUPPORT_TIME_INTERVAL_M1                //
//---------------------------------------------------------------------------

/*=== power off note ===*/
//---------------------------------------------------------------------------
#define SUPPORT_POWER_OFF_NOTE		CUSTOM_OFF                             //
#define	POWER_OFF_NOTE_M0			0x10                                   //
#define	POWER_OFF_NOTE_M1			0x11                                   //
#define POWER_OFF_NOTE_MODE			POWER_OFF_NOTE_M0                      //
//---------------------------------------------------------------------------

/*=== sleep time ===*/
//---------------------------------------------------------------------------
#define SETUP_SUPPORT_AUTO_PET		            CUSTOM_ON                  //
                                                                           //
#define SUPPORT_STANDBY_AFTER_POWER_RESET       CUSTOM_OFF                 //
//
#define SECONDIMG_SHOW	                        CUSTOM_ON                  //
/* system enter last state when press power off key  */                    //
#define SUPPORT_POWER_ON_ENTER_PRE_STATE        CUSTOM_ON                  //
/* only turn off backlight when power off */                               //
#define SUPPORT_STANDBY_ONLY_BL_OFF             CUSTOM_OFF                 //
//---------------------------------------------------------------------------

/*=== Audio Independent and FM mode support ===*/
//---------------------------------------------------------------------------
#define SUPPORT_AI_FM_MODE			CUSTOM_OFF                             //
//---------------------------------------------------------------------------

/*=== Diffrent UI style  support ===*/
//---------------------------------------------------------------------------
#define SETUP_SUPPORT_UI_STYLE		   	                     CUSTOM_OFF    //
//---------------------------------------------------------------------------

/*=== GP Special File format support*/
//---------------------------------------------------------------------------
#define GP_FILE_FORMAT_NONE            0                                   //
#define GP_FILE_FORMAT_SET_1           1 //GP Format 1 is *.GA/*.GV/*.GI   //
#define GP_FILE_FORMAT_SUPPORT         GP_FILE_FORMAT_NONE                 //
//---------------------------------------------------------------------------

/*=== United UI support ===*/
//---------------------------------------------------------------------------
#define SUPPORT_UNITED_UI_MODE			CUSTOM_OFF                         //
//---------------------------------------------------------------------------

/*=== Define the menu type of slideshow transition in Setting ===*/
//---------------------------------------------------------------------------
#define ROUGH					0//Random effects or motion effect  	   //
#define DETAIL					1//List all the Random effects in detail   //

#define SLIDESHOW_TRANSITION_MENU_TYPE_IN_SETTING			ROUGH          //
//---------------------------------------------------------------------------

/*=== Define the type of Volume Display mode ===*/
//---------------------------------------------------------------------------
#define VOLUME_DISPLAY_MODE0		0//show volume with a slide-up message //
#define VOLUME_DISPLAY_MODE1		1//show volume with a pop-up bar	   //

#define VOLUME_DISPLAY_MODE				VOLUME_DISPLAY_MODE0           	   //
//---------------------------------------------------------------------------

/*=== Define the type of controlling slideshow effect and music mode ===*/
//---------------------------------------------------------------------------
#define SYNCHRONOUS_MODE	0//control slideshow effect and music with 1 key//
#define SEPERATE_MODE		1//control slideshow effect and music with 2 key//

#define CONTROL_SLIDESHOW_EFFECT_AND_MUSIC			SEPERATE_MODE   	    //
//---------------------------------------------------------------------------

#endif //CUSTOMER_DEFINE


#endif //__CUSTOMER_H__
