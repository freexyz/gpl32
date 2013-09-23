/*****************************************************************************
 *               Copyright Generalplus Corp. All Rights Reserved.
 *
 * FileName:       application_cfg.h
 * Author:         Lichuanyue
 * Description:    Created
 * Version:        1.0
 * Function List:
 *                 Null
 * History:
 *                 1>. 2008/7/15 Created
 *****************************************************************************/

#ifndef __APPLICATION_CFG_H__
#define __APPLICATION_CFG_H__

#define _DPF_HOT_KEY_SUPPORT                0
#define DISPLAY_AVI_FROM_IMAGE_TASK 	    1
#define _GPL32_FONT_TYPE_MONO               1  /* MONO: Monospaced fonts */
#define _GET_MAINMENU_RESOURC_FROM_NVRAM    1
#define _GET_SETUPMENU_RESOURC_FROM_NVRAM   1
#define _UMI_DEBUG_FLAG                     1
#define _MUSIC_AND_PHOTO_VIEW_PAUSE         1
#define _MUSIC_CHANGED_BY_KEY_UP_DOWN       1
#define SLIDESHOW_EFFECT_RANDOM             0
#define SLIDESHOW_EFFECT_HIGHLIGHT          1
#define _UMI_PRINTF_SUPPORT                 0
//Contry language

#define _SUPPORT_MIDI_IN_ITEM_SELECTION  0
#if _SUPPORT_MIDI_IN_ITEM_SELECTION == 1
#define MAX_MIDI_NUMBER  5
typedef enum
{
	MIDI_EXIST_FIRST,
	MIDI_EXIST_LAST
}MIDI_EXIST_MODE;
#endif

//avi code define.
//#define _DPF_PROJECT

/*
*  Languange code
*/
typedef enum {
	#if DPF_SUPPORT_ENGLISH == CUSTOM_ON
	LCD_EN =1,  //English
	#endif
	#if DPF_SUPPORT_SCHINESE == CUSTOM_ON
	LCD_SCH,    //Simple Chinese
	#endif
	#if DPF_SUPPORT_TCHINESE == CUSTOM_ON
	LCD_TCH,	//Traditional Chinese
 	#endif
 	#if DPF_SUPPORT_JAPANESE == CUSTOM_ON
    LCD_JPN,     //Japanese
 	#endif
 	#if DPF_SUPPORT_FRENCH == CUSTOM_ON
    LCD_FR,     //French
 	#endif
	#if DPF_SUPPORT_ITALIAN == CUSTOM_ON
    LCD_IT, 	//Italian
 	#endif
	#if DPF_SUPPORT_SPANISH == CUSTOM_ON
    LCD_ES,     // Spanish
 	#endif
	#if DPF_SUPPORT_PORTUGUESE == CUSTOM_ON
	LCD_PT,     // Portuguese
 	#endif
	#if DPF_SUPPORT_RUSSIAN == CUSTOM_ON
	LCD_RU,     // Russian
 	#endif
	#if DPF_SUPPORT_KOREAN == CUSTOM_ON
    LCD_KR,     // Korean
 	#endif
	#if DPF_SUPPORT_DUTCH == CUSTOM_ON
	LCD_NE,     // Dutch
 	#endif
 	#if DPF_SUPPORT_LAN12== CUSTOM_ON
	LCD_12,     // LAN12
 	#endif
	#if DPF_SUPPORT_LAN13== CUSTOM_ON
	LCD_13,     // LAN13
 	#endif
	#if DPF_SUPPORT_LAN14== CUSTOM_ON
	LCD_14,     // LAN14
 	#endif
	#if DPF_SUPPORT_LAN15== CUSTOM_ON
	LCD_15,     // LAN15
 	#endif
 	LCD_UNI,
 	LCD_MAX
}_DPF_LCD;
/*
#ifndef SUPPORT_AUD_DECODE_MP3
	#define APP_MP3_DECODE_BG_EN		1
#else
	#define APP_MP3_DECODE_BG_EN		SUPPORT_AUD_DECODE_MP3
#endif


#ifndef SUPPORT_AUD_DECODE_WMA
	#define APP_WMA_DECODE_BG_EN		1
#else
	#define APP_WMA_DECODE_BG_EN		SUPPORT_AUD_DECODE_WMA
#endif

#ifndef SUPPORT_AUD_DECODE_WAV
	#define APP_WAV_CODEC_BG_EN			1
#else
	#define APP_WAV_CODEC_BG_EN			SUPPORT_AUD_DECODE_WAV
#endif

#ifndef SUPPORT_AUD_DECODE_G_MIDI
	#define APP_G_MIDI_DECODE_EN		1
#else
	#define APP_G_MIDI_DECODE_EN		SUPPORT_AUD_DECODE_G_MIDI
#endif
*/
#ifndef STCK_MONITOR_STATUS
#define STCK_MONITOR_OFF        0
#define STCK_MONITOR_ON         1
#define STCK_MONITOR_STATUS     STCK_MONITOR_OFF
#endif

#ifndef MSGQ_MONITOR_STATUS
#define MSGQ_MONITOR_OFF        0
#define MSGQ_MONITOR_ON         1
#define MSGQ_MONITOR_STATUS     STCK_MONITOR_OFF
#endif
/*
#define APP_A1600_DECODE_BG_EN		0
#define APP_A1800_DECODE_BG_EN		0
#define APP_A1800_ENCODE_BG_EN		0
#define APP_A6400_DECODE_BG_EN		0
#define APP_S880_DECODE_BG_EN		0

#define APP_MP3_DECODE_FG_EN        0
#define APP_WMA_DECODE_FG_EN        0
#define APP_WAV_CODEC_FG_EN         0
#define APP_A1600_DECODE_FG_EN		0
#define APP_A1800_DECODE_FG_EN		0
#define APP_A1800_ENCODE_FG_EN		0
#define APP_A6400_DECODE_FG_EN		0
#define APP_S880_DECODE_FG_EN		0
*/
#ifndef CUSTOMER_DEFINE
#define	CARD_PLUG_M0        		0 		// Pop up select card windows
#define	CARD_PLUG_M1        		1 		// Show message
#define	CARD_PLUG_MODE      		CARD_PLUG_M1
#endif

//---------------------------------------------------------------------------
// OSD menu buffer allocate mode choices
#define CUSTOM_ON					1
#define CUSTOM_OFF					0
//choice one
#define OSD_MENU_BUFFER_MODE		CUSTOM_ON
//---------------------------------------------------------------------------

/*=== App Icon show definition choices ===*/
//---------------------------------------------------------------------------
#define ICON_SHOW_M0        0                                              //
#define ICON_SHOW_M1        1     // Thumbnail mode with NAND backup function
#define ICON_SHOW_M2        2                                              //
#define ICON_SHOW_M3        3                                              //
#define ICON_SHOW_LIB       (-1)                                           //
//choice one                                                               //
#define ICON_SHOW_MODE      ICON_SHOW_M1                                   //
//---------------------------------------------------------------------------

/*=== App Volume Control definition choices ===*/
//---------------------------------------------------------------------------
#define VOLUME_CONTROL_NONE          0                                     //
#define VOLUME_CONTROL_M0            1                                     //
#define VOLUME_CONTROL_M1            2                                     //
//choice one                                                               //
#define VOLUME_CONTROL_MODE         VOLUME_CONTROL_M1                      //
#define POP_UP_MSG_VOLUME_STR		CUSTOM_ON							   //
//---------------------------------------------------------------------------

/*=== App Volume Brightness Style definition choices ===*/
//---------------------------------------------------------------------------
#define VOLUME_BRIGHTNESS_STYLE1     0                                     //
#define VOLUME_BRIGHTNESS_STYLE2     1                                     //
//choice one                                                               //
#define VOLUME_BRIGHTNESS_STYLE_CHOICES     VOLUME_BRIGHTNESS_STYLE1       //
//---------------------------------------------------------------------------

/*=== App Setting Control definition choices ===*/
//---------------------------------------------------------------------------
#define SETTING_CONTROL_NONE          0                                    //
#define SETTING_CONTROL_M0            1                                    //
#define SETTING_CONTROL_M1            2                                    //
//choice one                                                               //
#define SETTING_CONTROL_MODE        SETTING_CONTROL_M1                     //
//---------------------------------------------------------------------------

/*=== App Setting Volume Brightness Control definition choices ===*/
//---------------------------------------------------------------------------
#define SETTING_VOL_BRI_CONTROL_NONE          0                  			//
#define SETTING_VOL_BRI_CONTROL_M0            1                 		    //
#define SETTING_VOL_BRI_CONTROL_M1            2                  		    //
//choice one                                                                //
#define SETTING_VOLUME_BRIGHTNESS_CONTROL_MODE	SETTING_VOL_BRI_CONTROL_M1  //
//---------------------------------------------------------------------------


/*=== App SETTING RESOURCE SHRINK ===*/
//---------------------------------------------------------------------------
#define RESOURCE_SHRINK_ON				1								   //
#define RESOURCE_SHRINK_OFF				0								   //
#define SETTING_SHRINK_MODE				RESOURCE_SHRINK_OFF				   //
//---------------------------------------------------------------------------


/*=== App Thumbnail Show Style ===*/
//---------------------------------------------------------------------------
#define THUMB_PITHY         0 //with larger photo list in a frame          //
#define THUMB_DETAIL        1 //with photo info but smaller icons          //
#define THUMB_SHOW_STYLE    THUMB_DETAIL                                    //
//---------------------------------------------------------------------------

/*=== App Save as logo ===*/
//---------------------------------------------------------------------------
#define	AP_SAVE_AS_LOGO_EN	 CUSTOM_OFF

//---------------------------------------------------------------------------

/*=== App Mass production test ===*/
//---------------------------------------------------------------------------
#define	AP_MASS_PRO_TEST_EN 	CUSTOM_OFF

//---------------------------------------------------------------------------

/*=== App Music Shuffle Mode ===*/
//---------------------------------------------------------------------------
#define	AP_AUDIO_SHUFFLE_MODE_EN 	CUSTOM_OFF

//---------------------------------------------------------------------------

/*=== App USBD SD&MS only display one lun when connect to PC  ===*/
//---------------------------------------------------------------------------
#define AP_USBD_SDMS_LUN_IN_ONE_LUN     CUSTOM_OFF                           //


/*=== TV OUT Support ===*/
#if  ((OSD_MENU_MODE%2)==0)
#define DPF_TV_OUT                 CUSTOM_ON
#else
#define DPF_TV_OUT                 CUSTOM_OFF
#endif

#if DPF_TV_OUT == CUSTOM_ON
    #define LGP_LB064V02            0x3000
    #define C_TV_VGA                0xE251
    #define C_TV_D1                 0xE252
    //choice one
    #define C_TVOUT_DEVICE          C_TV_VGA
#endif

#if (OSD_MENU_MODE == OSD_MENU_M2 || OSD_MENU_MODE == OSD_MENU_M3 )
#define OSD_MENU_DIR		CUSTOM_ON		//osd direction key,it only use to TFT,should not be use TV out
#else
#define OSD_MENU_DIR		CUSTOM_OFF
#endif

/* ===  For Center Display    ===*/
#if (defined OSD_MENU_DIR)  && (OSD_MENU_DIR == CUSTOM_ON)
	#define OSD_MENU_WIDTH   	(0)
#else
	#if (DPF_H_V == DPF_800x480) || (DPF_H_V == DPF_800x600)
	#define OSD_MENU_WIDTH   	(88)
	#elif (DPF_H_V == DPF_480x234) || (DPF_H_V == DPF_320x240)
	#define OSD_MENU_WIDTH   	(48)
	#endif
#endif

/*=== Option Define for SLIDESHOW_WITH_VIDEO ===*/
//---------------------------------------------------------------------------
#define SLIDESHOW_WITH_VIDEO  CUSTOM_OFF
#if (defined SLIDESHOW_WITH_VIDEO)  && (SLIDESHOW_WITH_VIDEO == CUSTOM_ON)
//Key Mode---------------------------
#define SLIDEVIDEO_KEY_NORMAL       0
#define SLIDEVIDEO_KEY_NOACTION     1
#define SLIDEVIDEO_KEY_EXIT         2
#define SLIDEVIDEO_KEY_MODE         SLIDEVIDEO_KEY_NORMAL
//Play Mode--------------------------
#define SLIDEVIDEO_PLAY_REPEAT      0
#define SLIDEVIDEO_PLAY_ONCE        1
#define SLIDEVIDEO_PLAY_MODE        SLIDEVIDEO_PLAY_REPEAT
//Music OnOFF------------------------
#define SLIDEVIDEO_MUSIC_MUTE       CUSTOM_OFF
//Interval---------------------------
#define C_UNDEFINE  0
#define C_5SECOND   5
#define C_10SECOND  10
#define C_30SECOND  30
#define C_1MINUTE   1*60
#define C_5MINUTE   5*60
#define C_30MINUTE  30*60
#define C_1HOUR     1*60*60
#define C_3HOUR     3*60*60
#define C_12HOUR    12*60*60
#define C_1DAY      24*60*60
#define SLIDEVIDEO_INTERVAL  C_UNDEFINE
#endif

//---------------------------------------------------------------------------

/*=== App Time Interval definition choices ===*/
//---------------------------------------------------------------------------
#define TIME_INTERVAL_NONE          0                                     	//
#define TIME_INTERVAL_M0            1                                    	//
#define TIME_INTERVAL_M1            2                                    	//
//choice one                                                             	//
#define TIME_INTERVAL_MODE        TIME_INTERVAL_M0                        	//

//---------------------------------------------------------------------------

/*=== usb device "connect to PC" interrogation support ===*/
/*===        switch on setting configuration       ===*/
//---------------------------------------------------------------------------
#if (DPF_H_V == DPF_800x480)											   //
																		   //
	#define SUPPORT_USBD_INTERROGATION					CUSTOM_OFF		   //
	#define THUMBNAIL_FORMAT_SWITCH		   				CUSTOM_OFF         //
	#define REPEAT_FORMATE_SWITCH		   				CUSTOM_OFF         //
	#define SETUP_SUPPORT_FULL_IMAGE_FORMATE		   	CUSTOM_ON          //
																		   //
#elif (DPF_H_V == DPF_480x234)                                			   //
																		   //
	#define SUPPORT_USBD_INTERROGATION	   				CUSTOM_OFF		   //
	#if (SUPPORT_AD_KEY == KEY_AD_5_MODE)								   //
		#define THUMBNAIL_FORMAT_SWITCH		   			CUSTOM_ON          //
		#define REPEAT_FORMATE_SWITCH		   			CUSTOM_ON          //
	#else																   //
		#define THUMBNAIL_FORMAT_SWITCH		   			CUSTOM_OFF         //
		#define REPEAT_FORMATE_SWITCH		   			CUSTOM_OFF         //
	#endif																   //
	#define SETUP_SUPPORT_FULL_IMAGE_FORMATE		   	CUSTOM_ON          //
																		   //
#elif (DPF_H_V == DPF_320x240)											   //
																		   //
	#define SUPPORT_USBD_INTERROGATION	   				CUSTOM_ON		   //
	#define THUMBNAIL_FORMAT_SWITCH		   				CUSTOM_ON          //
	#define REPEAT_FORMATE_SWITCH		   				CUSTOM_ON          //
	#define SETUP_SUPPORT_FULL_IMAGE_FORMATE		   	CUSTOM_OFF         //
																		   //
#else																	   //
																		   //
	#define SUPPORT_USBD_INTERROGATION	  			 	CUSTOM_OFF		   //
	#define THUMBNAIL_FORMAT_SWITCH		   				CUSTOM_OFF         //
	#define REPEAT_FORMATE_SWITCH		   				CUSTOM_OFF         //
	#define SETUP_SUPPORT_FULL_IMAGE_FORMATE		   	CUSTOM_OFF         //
																		   //
#endif																	   //
//---------------------------------------------------------------------------

#define	BYPASS_VIDEO_IN_USB			CUSTOM_OFF
#define SHOW_IMAGE_DECODE_STATUS	CUSTOM_ON

/*CL1/Cl2 Object define*/
#define _APP_CL2_EXPLORER           CUSTOM_OFF
#if _UMI_PRINTF_SUPPORT == CUSTOM_ON
 #define _APP_CL1_FONTS              CUSTOM_ON
 #define _APP_CL2_PRINTF             CUSTOM_ON
#else
 #define _APP_CL1_FONTS              CUSTOM_OFF
 #define _APP_CL2_PRINTF             CUSTOM_OFF
#endif
#define _APP_CL2_PRINT_SCREEN        CUSTOM_OFF


#define _ICON_SHOW_SCROLL_BAR_HIDE_EN  CUSTOM_OFF
#define DISPLAY_PHOTO_CYCLE			CUSTOM_OFF
#define SEARCH_ALL_FILE				CUSTOM_OFF
#define SHOW_VIDEO_FILE_SIZE		CUSTOM_OFF

#if _UMI_PRINTF_SUPPORT==1
#define UMI_PRINT cl2_printf
#else
#define UMI_PRINT umi_null_printf
#endif

#define LIMIT_IMAGE_WIDTH_HEIGHT		CUSTOM_ON
#if DPF_H_V >= DPF_800x480
#define IMAGE_WIDTH_HEIGHT_LIMITATION	(640 * 480)
#else
#define IMAGE_WIDTH_HEIGHT_LIMITATION	(800 * 600)
#endif

/*****************************************************
*****************************************************/
#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	#define APP_G_MIDI_DECODE_EN	1
#else
	#define APP_G_MIDI_DECODE_EN	0	//fixed 0
#endif

//define multi-media function use  
#define DBG_MESSAGE			1		//for debug use

//define the special effect when use video encode
#define AUDIO_SFX_HANDLE	0
#define VIDEO_SFX_HANDLE	0

//define audio algorithm
#define APP_WAV_CODEC_EN		1	//mudt enable when use Audio Encode and AVI Decode 	
#define APP_MP3_DECODE_EN		1
#define APP_MP3_ENCODE_EN		1
#define APP_A1800_DECODE_EN		1
#define APP_A1800_ENCODE_EN		1
#define APP_WMA_DECODE_EN		1
#define APP_A1600_DECODE_EN		1
#define APP_A6400_DECODE_EN		1
#define APP_S880_DECODE_EN		1
#define APP_ACC_DECODE_EN		0

#define APP_CONST_PITCH_EN		0
#define APP_ECHO_EN				0
#define	APP_VOICE_CHANGER_EN	0
#define APP_UP_SAMPLE_EN		1
#define	APP_DOWN_SAMPLE_EN		1
#define A1800_DOWN_SAMPLE_EN	1

#if APP_WAV_CODEC_EN
	#define APP_WAV_CODEC_FG_EN		1	//must enable when use Audio Encode and AVI Decode 	
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
		#define APP_WAV_CODEC_BG_EN     1
	#else
		#define APP_WAV_CODEC_BG_EN 	0 //fixed 0
	#endif
#else
	#define APP_WAV_CODEC_FG_EN		0
	#define APP_WAV_CODEC_BG_EN     0
#endif

#if APP_MP3_DECODE_EN
	#define APP_MP3_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
		#define APP_MP3_DECODE_BG_EN	1
	#else
		#define APP_MP3_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_MP3_DECODE_FG_EN	0
	#define APP_MP3_DECODE_BG_EN	0
#endif

#if APP_A1800_DECODE_EN
	#define APP_A1800_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_A1800_DECODE_BG_EN  1
	#else
		#define APP_A1800_DECODE_BG_EN  0 //fixed 0
	#endif
#else
	#define APP_A1800_DECODE_FG_EN	0
	#define APP_A1800_DECODE_BG_EN  0	
#endif

#if APP_WMA_DECODE_EN
	#define APP_WMA_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_WMA_DECODE_BG_EN	1
	#else
		#define APP_WMA_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_WMA_DECODE_FG_EN	0
	#define APP_WMA_DECODE_BG_EN	0
#endif

#if APP_A1600_DECODE_EN
	#define APP_A1600_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_A1600_DECODE_BG_EN	1
	#else
		#define APP_A1600_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_A1600_DECODE_FG_EN	0
	#define APP_A1600_DECODE_BG_EN	0
#endif

#if APP_A6400_DECODE_EN
	#define APP_A6400_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_A6400_DECODE_BG_EN	1
	#else
		#define APP_A6400_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_A6400_DECODE_FG_EN	0
	#define APP_A6400_DECODE_BG_EN	0
#endif

#if APP_S880_DECODE_EN
	#define APP_S880_DECODE_FG_EN	1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_S880_DECODE_BG_EN	1
	#else
		#define APP_S880_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_S880_DECODE_FG_EN	0
	#define APP_S880_DECODE_BG_EN	0
#endif

#if APP_ACC_DECODE_EN
	#define APP_AAC_DECODE_FG_EN 1
	#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)	
		#define APP_AAC_DECODE_BG_EN	1
	#else
		#define APP_AAC_DECODE_BG_EN	0 //fixed 0
	#endif
#else
	#define APP_AAC_DECODE_FG_EN 0
	#define APP_AAC_DECODE_BG_EN 0
#endif

#define APP_LPF_ENABLE				0	//low pass filter enable

#endif		// __APPLICATION_CFG_H__
