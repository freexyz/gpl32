#ifndef __RESOURCE_VARIABLE_H__
#define __RESOURCE_VARIABLE_H__
/* it's pseudo header definition of turnkey_umi_task.h , no need to include other one*/

typedef enum {
	IMG_SET_12_24_HR,
	IMG_SET_DATE_ORDER,
	IMG_SET_LANGUAGE,
	IMG_SET_SLIDE,
	IMG_SET_PHOTODATE,
	IMG_SET_SLEEP_TIME,
	IMG_CLOCK_CENTER,
	IMG_CLOCK,
	IMG_HOUR,
	IMG_MINUTE = IMG_HOUR + 16,
	IMG_SECOND = IMG_MINUTE + 16,
	IMG_COVER_AVI_ICON = IMG_SECOND + 16,
	IMG_COVER_AVI_ICON_LARGE,
	IMG_LOADING_PHOTO,
	IMG_LOADING_PHOTO_LARGE,
	IMG_LOADING_VIDEO,
	IMG_STORAGE_CF,
	IMG_STORAGE_MS,
	IMG_STORAGE_NAND,
	IMG_STORAGE_SD,
	IMG_STORAGE_USB,
	IMG_STORAGE_EXTREME,
	IMG_POP_PC_CONNECTED,
	IMG_POP_VOLUME,
	IMG_POP_VOLUME_MUTE,
	IMG_POP_INTERVAL,
	IMG_POP_TITLE_BAR,
	IMG_POP_FG_UNSELECT_BAR,
	IMG_POP_BG_BAR,
	IMG_POP_FG_SELECT_BAR,
	IMG_SET_VOLUME,
	IMG_SET_LCD_BRIGHTNESS,
	IMG_SET_ROLLING_BAR_SELECT,
	IMG_SET_ROLLING_BAR_UNSELECT,
	IMG_SET_SETTING_BAR,
	IMG_SET_THUMBNAIL_1,
	IMG_SET_THUMBNAIL_2,
	IMG_SET_THUMBNAIL_3,
	IMG_SET_THUMBNAIL_4,
	IMG_DEFAULT_PHOTO,
	IMG_SET_ALARM_ON,
	IMG_SET_ALARM_OFF,
	IMG_LOADING_VIDEO_LARGE,
  	IMG_LOADING_ANIMATION,
	IMG_SETTING_CLOCK,
	IMG_POP_YES,
	IMG_POP_NO,
	IMG_SET_ALARMCLOCK,
	IMG_SET_ALARM,
	IMG_SET_REPEAT_ONE,
	IMG_SET_REPEAT_ALL,
	IMG_POP_ADAPTER,
	IMG_POP_BATTERY,
	IMG_POP_BATTERY_GREEN,
	IMG_POP_BATTERY_RED,
	IMG_POP_CHARGE_FULL,
	IMG_FULL_SCREEN,
	IMG_FULL_IMAGE,
	IMG_RESTORE_DEFAULTS,
	IMG_SET_POWER_TIME,
	IMG_SET_ON,
	IMG_SET_OFF,
	IMG_THUMBNAIL_OK,
	IMG_SET_UI_1,
	IMG_SET_UI_2,
	IMG_SET_UI_3,
	IMG_RESERVED

} SYS_RESEV_IMAGE;


typedef enum{
	COLOR_GRAY			= 248,
	COLOR_YELLOW		= 249,
	COLOR_GREEN			= 250,
	COLOR_BLUE			= 251,
	COLOR_RED			= 252,
	COLOR_WHITE			= 253,
	COLOR_BLACK			= 254,
	COLOR_TRANSPARENT	= 255
}_DPF_COLOR;


// DPF string ID ,every strings used must added an ID Here ,as well as added in the below array.
typedef enum{
	STR_MULTINATIONAL = 1,
	STR_SUN_S ,         // Sun
	STR_MON_S ,     	// Mon
	STR_TUE_S ,     	// Tue
	STR_WED_S ,     	// Wed
	STR_THU_S ,     	// Thu
	STR_FRI_S ,     	// Fri
	STR_SAT_S ,     	// Sat
	STR_SUN ,     	    // SUN
	STR_MON ,     		// MON
	STR_TUE ,     		// TUE
	STR_WED ,     		// WED
	STR_THU ,     		// THU
	STR_FRI ,     		// FRI
	STR_SAT ,     		// SAT
	STR_SUNDAY_S ,     	// Sunday
	STR_MONDAY_S ,     	// Monday
	STR_TUESDAY_S ,     // Tuesday
	STR_WEDNESDAY_S ,   // Wednesday
	STR_THURSDAY_S ,    // Thursday
	STR_FRIDAY_S ,     	// Friday
	STR_SATURDAY_S ,    // Saturday
    STR_SUNDAY,     	// SUNDAY
	STR_MONDAY ,     	// MONDAY
	STR_TUESDAY ,       // TUESDAY
	STR_WEDNESDAY ,     // WEDNESDAY
	STR_THURSDAY ,      // THURSDAY
	STR_FRIDAY ,     	// FRIDAY
	STR_SATURDAY ,      // SATURDAY
	STR_JAN_S,         	// Jan
	STR_FEB_S,     		// Feb
	STR_MAR_S,     		// Mar
	STR_APR_S,     		// Apr
	STR_MAY_S,     		// May
	STR_JUN_S,     		// Jun
	STR_JUL_S,     		// Jul
	STR_AUG_S,			// Aug
	STR_SEP_S,			// Sep
	STR_OCT_S,			// Oct
	STR_NOV_S,			// Nov
	STR_DEC_S,			// Dec
	STR_JAN,         	// JAN
	STR_FEB,     		// FEB
	STR_MAR,     		// MAR
	STR_APR,     		// APR
	STR_MAY,     		// MAY
	STR_JUN,     		// JUN
	STR_JUL,     		// JUL
	STR_AUG,			// AUG
	STR_SEP,			// SEP
	STR_OCT,			// OCT
	STR_NOV,			// NOV
	STR_DEC,			// DEC
	STR_JANUARY_S,      // January
	STR_FEBRUARY_S,     // February
	STR_MARCH_S,     	// March
	STR_APRIL_S,     	// April
	STR_MAY_CAP_S,     	// May
	STR_JUNE_S,     	// June
	STR_JULY_S,     	// July
	STR_AUGUST_S,		// August
	STR_SEPTEMBER_S,	// September
	STR_OCTOBER_S,		// October
	STR_NOVEMBER_S,		// November
	STR_DECEMBER_S,	    // December
   	STR_JANUARY,        // JANUARY
	STR_FEBRUARY,       // FEBRUARY
	STR_MARCH,     	    // MARCH
	STR_APRIL,     	    // APRIL
	STR_MAY_CAP,     	// MAY
	STR_JUNE,     	    // JUNE
	STR_JULY,     	    // JULY
	STR_AUGUST,		    // AUGUST
	STR_SEPTEMBER,	    // SEPTEMBER
	STR_OCTOBER,		// OCTOBER
	STR_NOVEMBER,		// NOVEMBER
	STR_DECEMBER,		// DDECEMBER
	STR_CLOCK_S,		// Clock
	STR_LCDBRIGHT_S,	// Lcdbright
	STR_SOUND_S,		// Sound
	STR_RESERVED,		// STRING RESERVED
	STR_MUSIC,			// Music
	STR_INTERNATION_S,	// internation
	STR_DATE_S,			// Date
	STR_SLIDESHOW,      // Slideshow
	STR_THUMBNAIL,      // Thumbnail
	STR_CALENDAR,       // Calendar
	STR_STORAGE,        // Storage
	STR_SETUP,          // Setup
	STR_PHOTO_VIEW_ORIGINAL,			// Original image
	STR_PHOTO_VIEW_AUTO_CONTRAST,		// Auto-Contrast image
	STR_PHOTO_VIEW_AUTO_LEVEL,		// Auto-level image
	STR_PHOTO_VIEW_NEGATIVE,			// Negative image
	STR_PHOTO_VIEW_GRAYSCALE,			// GrayScale image
	STR_PHOTO_VIEW_MIRROR,			// Mirror image
	STR_PHOTO_VIEW_FACE_DETECTION,	// Face detection
	STR_ALARM_ON,
	STR_ALARM_OFF,
	STR_AM,
	STR_PM,
	STR_YEAR,
	STR_MONTH,
	STR_DAY,
	STR_HOUR,
	STR_MINUTE,
	STR_AMPM,
	STR_POWER_ON_TIME,
	STR_POWER_OFF_TIME,
	STR_LANGUAGE,			    // Language
	STR_TIME_FORMAT,			// Time Format
	STR_DATE_FORMAT,			// Date Format
	STR_WEEK_FORMAT,			// Week Format
	STR_POWER_TIME,
	STR_RESTORE_DEFAULTS,
	STR_PHOTO_DATE,
	STR_ORIENTATION,
	STR_SPACE,  				 // " "
	STR_POWER_ICON,
	STR_THUMBNAIL_IOCN,
	STR_Calendar_ICON,
	STR_Clock_ICON,
	STR_INTERVAL_ICON,
	STR_MUSIC_ICON,
	STR_MENU_ICON,
	STR_TRANSITION_ICON,
	STR_PREV_ICON,
	STR_NEXT_ICON,
	STR_OK_ICON,
	STR_BACK_ICON,
	STR_SLIDESHOW_ICON,
	STR_ALBNUM_ICON,
	STR_PHOTO_ICON,
	STR_VIDEO_ICON,
	STR_STATUS,
	STR_PLAY_ICON,
	STR_STOP_ICON,
	STR_UP_ICON,
	STR_DOWN_ICON,
	STR_VIEW_ICON,
	STR_ROTATE_ICON,
	STR_ZOOM_IN_ICON,
	STR_ZOOM_OUT_ICON,
	STR_FF_ICON,
	STR_BB_ICON,
	STR_CF_IN,						//CF Card is Plug In
	STR_UNKNOW_IN,					//Unknow Device Plug In
	STR_SD_IN,						//SD Card is Plug In
	STR_MS_IN,						//MS Card is Plug In
	STR_USB_IN,						//USB is Plug In
	STR_XD_IN,						//XD Card is Plug In
	STR_CF_OUT,						//CF Card is Plug Out
	STR_UNKNOW_OUT,					//Unknow Device Plug Out
	STR_SD_OUT,						//SD Card is Plug Out
	STR_MS_OUT,						//MS Card is Plug Out
	STR_USB_OUT,					//USB is Plug Out
	STR_XD_OUT,						//XD Card is Plug Out
	STR_STORAGE_INIT,				//Stroage Initial
	STR_DOT,						//.
	STR_REPEAT,							//Repeat
	STR_VOLUME,							//Volume
	STR_MUSIC_LIST,
	STR_TIME_INTERVAL,
	STR_STORAGE_SELECT,
	STR_INCREASE_ICON,
	STR_DECREASE_ICON,
	STR_LEFT_ICON,
	STR_RIGHT_ICON,
	STR_MUTE_ICON,
	STR_LOADING,
	STR_VOLUME_CONTROL,
	STR_SECOND,	
	STR_PC_CONNECTED,
	STR_THUMNAIL_MODE,
	STR_AQUARIUM,
	STR_SLIDE_TRANSITION,
	STR_TYPE_1,
	STR_TYPE_2,
	STR_TYPE_3,
	STR_TYPE_4,
	STR_TYPE_5,
	STR_TYPE_6,
	STR_TYPE_7,
	STR_TYPE_8,
	STR_TYPE_9,
	STR_TYPE_10,
	STR_TYPE_11,
	STR_TYPE_12,
	STR_TYPE_13,
	STR_TYPE_14,
	STR_TYPE_15,
	STR_TYPE_16,
	STR_TYPE_17,
	STR_TYPE_18,
	STR_TYPE_19,
	STR_TYPE_20,	
	STR_ONCE,
	STR_WEEKDAYS,
	STR_WEEKEND,
	STR_EVERYDAY,
	STR_MODE,
	STR_CONNECT_PC_ASK,
	STR_YES,
	STR_NO,
	STR_GENERIC_MASS_STORAGE_USB_DEVICE,
	STR_FULL_IMAGE,
	STR_NO_MEDIA_EXIST,
	STR_SOUND_ON,
	STR_SOUND_OFF,
	STR_MUTE_ON,
	STR_MUTE_OFF,
	STR_POWER_OFF,
	STR_PLAY,
	STR_PAUSE,
	STR_STOP,
	STR_PREVIOUS,
	STR_NEXT,
	STR_SAVE_AS_LOGO,
	STR_THEME,
	STR_ALARM_MUSIC,
	STR_MAX,
	STR_MIN,
	STR_MOTION,
	STR_RANDOM,
	STR_FIRMWARE_VERSION,
	STR_SYSTEM_INFORMATION,
	STR_WARNING,
	STR_DATA_TRANSFERRING,
	STR_CONNECTING_TO_TV,
	STR_PRESS_ANY_KEY_TO_EXIT,
	STR_SWITCH_TO_NEW_CARD_OR_UDISK,
	STR_ARE_YOU_SURE,
	STR_DONT_REMOVE_CARD_OR_UDISK,
	STR_DONT_REMOVE_POWER,
	STR_SCREEN_SAVER,
	STR_RADIO,
	STR_SCAN,
	STR_COPYING,          
	STR_DELETING,          
	STR_COMPLETE,          
	STR_ABORT,          
	STR_HOURS,          
	STR_MINUTES,          
	STR_DAYS,          
	STR_THIS_FILE_FORMAT_IS_NOT_SUPPORTED,
	STR_SUPPORTED_AUDIO_DOESNT_EXIST,
	STR_SUPPORTED_VIDEO_DOESNT_EXIST,
	STR_NO_MEDIA,
	STR_SECOND_S,	
	STR_MINUTE_S,
	STR_HOUR_S,
	STR_PREV_2,
	STR_NEXT_2,
	STR_YYYY_MM_DD,
	STR_MM_DD_YYYY,
	STR_DD_MM_YYYY,
	STR_SLIDESHOW_PLAY,
	STR_SLIDESHOW_PAUSE,	
	STR_VEDIO_PLAY,
	STR_VEDIO_PAUSE,
	STR_MUSIC_PLAY,
	STR_MUSIC_PAUSE,
	STR_REPEAT_ONE,
	STR_REPEAT_ALL,
	STR_UI_STYLE,
	STR_MIDI_1,
	STR_MIDI_2,
	STR_MIDI_3,
	STR_MIDI_4,
	STR_MIDI_5,	
	STR_MAX_NUM

}_STR_ID;



typedef enum{

	FONT_HEIGHT_lv1 = 1,
	FONT_HEIGHT_lv2,
	FONT_HEIGHT_lv3,
	FONT_HEIGHT_lv4,
	FONT_HEIGHT_SLIDE_SHOW

}_DPF_FONT_TYPE;



//>>>>>>>>>>>>>>>>>>>>	FONT SIZE SELECTION	>>>>>>>>>>>>>>>>>>>>
#if (DPF_H_V == DPF_800x480 || DPF_H_V == DPF_800x600 || DPF_H_V == DPF_640x480 || DPF_H_V == DPF_720x480)

	#define		FONT_HEIGHT_LOADING								FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_POP_MESSAGE							FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SLIDE_SHOW_DATE						FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SLIDE_SHOW_DATE_CHAR				FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_MAIN_MENU_HINT						FONT_HEIGHT_lv4

	//	for pop window ( Time Interval, Volume Control, Music List, Storage Source )
	#define		FONT_HEIGHT_POP_WINDOW_TITLE					FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_TITLE			FONT_HEIGHT_lv4

	#define		FONT_HEIGHT_POP_WINDOW_HINT					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_HINT			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_POP_WINDOW_NUMBER					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_NUMBER			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_MUSIC_LIST_SONG						FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_TV_OUT_MUSIC_LIST_SONG				FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_STORAGE_HINT						FONT_HEIGHT_lv4

	//setting
	#define		FONT_HEIGHT_SETTING_MENU						FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_LANGUAGE					FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_TIME_FORMAT_TYPE			FONT_HEIGHT_lv4		//24H or 12H
	#define		FONT_HEIGHT_SETTING_TIME_FORMAT_TIME			FONT_HEIGHT_lv4		//time(24H or 12H)
	#define		FONT_HEIGHT_SETTING_DATE_FORMAT					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_WEEK_FORMAT_WEEK			FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_SETTING_WEEK_FORMAT_MONTH			FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_CLOCK		FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_HINT		FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_DATE		FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_VERSION						FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_PHOTO_DATE					FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_FULL_IMAGE					FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_SLIDESHOW_TYPE				FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_SETTING_SYSTEM_INFO					FONT_HEIGHT_lv4		
	//	calendar
	#define		FONT_HEIGHT_CALENDAR_MODE1_CLOCK				FONT_HEIGHT_lv2		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_YEAR					FONT_HEIGHT_lv2		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_MONTH				FONT_HEIGHT_lv2		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_WEEK					FONT_HEIGHT_lv1		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_DAY					FONT_HEIGHT_lv2		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_TIME					FONT_HEIGHT_lv4		//800X480
	#define		FONT_HEIGHT_CALENDAR_MODE2_AMPM					FONT_HEIGHT_lv3		//800X480

	//	alarm
	#define		FONT_HEIGHT_ALARM_HINT							FONT_HEIGHT_lv2//FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_ALARM_TIME							FONT_HEIGHT_lv3

	//	photo view
	#define		FONT_HEIGHT_PHOTO_VIEW_HINT					FONT_HEIGHT_lv2

#elif (DPF_H_V == DPF_320x240 || DPF_H_V == DPF_480x234 || DPF_H_V == DPF_480x272)

	#define		FONT_HEIGHT_LOADING								FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_POP_MESSAGE							FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SLIDE_SHOW_DATE						FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SLIDE_SHOW_DATE_CHAR				FONT_HEIGHT_lv3
	
	#if OSD_MENU_MODE == OSD_MENU_M3
		#define		FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv2
	#else
		#define		FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv1
	#endif
	
	#define		FONT_HEIGHT_MAIN_MENU_HINT						FONT_HEIGHT_lv4

	//	for pop window ( Time Interval, Volume Control, Music List, Storage Source )
	#define		FONT_HEIGHT_POP_WINDOW_TITLE					FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_TITLE			FONT_HEIGHT_lv4

	#define		FONT_HEIGHT_POP_WINDOW_HINT					FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_HINT			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_POP_WINDOW_NUMBER					FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_TV_OUT_POP_WINDOW_NUMBER			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_MUSIC_LIST_SONG						FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_TV_OUT_MUSIC_LIST_SONG				FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_STORAGE_HINT						FONT_HEIGHT_lv3

	//setting
	#define		FONT_HEIGHT_SETTING_MENU						FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_LANGUAGE					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_TIME_FORMAT_TYPE			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_TIME_FORMAT_TIME			FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_DATE_FORMAT					FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_SETTING_WEEK_FORMAT_WEEK			FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_SETTING_WEEK_FORMAT_MONTH			FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_CLOCK		FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_HINT		FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_DATE		FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_VERSION						FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_SETTING_PHOTO_DATE					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_SETTING_FULL_IMAGE					FONT_HEIGHT_lv3	
	#define		FONT_HEIGHT_SETTING_SLIDESHOW_TYPE				FONT_HEIGHT_lv3
#if (DPF_H_V == DPF_480x234)		
	#define		FONT_HEIGHT_SETTING_SYSTEM_INFO					FONT_HEIGHT_lv3
#elif (DPF_H_V == DPF_320x240)
	#define		FONT_HEIGHT_SETTING_SYSTEM_INFO					FONT_HEIGHT_lv2
#endif	
	//	calendar
	#define		FONT_HEIGHT_CALENDAR_MODE1_TIME					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_CALENDAR_MODE1_AMPM					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_CALENDAR_MODE1_DATE					FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_CALENDAR_MODE2_MONTH				FONT_HEIGHT_lv3
	#define		FONT_HEIGHT_CALENDAR_MODE2_DAY					FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_CALENDAR_MODE2_YEAR					FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_CALENDAR_MODE2_WEEK					FONT_HEIGHT_lv1
	#define		FONT_HEIGHT_CALENDAR_MODE2_TIME					FONT_HEIGHT_lv4
	#define		FONT_HEIGHT_CALENDAR_MODE1_CLOCK				FONT_HEIGHT_lv2
	//	alarm
	#define		FONT_HEIGHT_ALARM_HINT							FONT_HEIGHT_lv2
	#define		FONT_HEIGHT_ALARM_TIME							FONT_HEIGHT_lv2

	//	photo view
	#define		FONT_HEIGHT_PHOTO_VIEW_HINT					    FONT_HEIGHT_lv2

#endif

	#define		TV_D1_FONT_HEIGHT_LOADING								FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_POP_MESSAGE							FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SLIDE_SHOW_DATE						FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SLIDE_SHOW_DATE_CHAR				FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv2
	#define		TV_D1_FONT_HEIGHT_MAIN_MENU_HINT						FONT_HEIGHT_lv4

	//	for pop window ( Time Interval, Volume Control, Music List, Storage Source )
	#define		TV_D1_FONT_HEIGHT_POP_WINDOW_TITLE					FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_TV_OUT_POP_WINDOW_TITLE			FONT_HEIGHT_lv4

	#define		TV_D1_FONT_HEIGHT_POP_WINDOW_HINT					FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_TV_OUT_POP_WINDOW_HINT			FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_POP_WINDOW_NUMBER					FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_TV_OUT_POP_WINDOW_NUMBER			FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_MUSIC_LIST_SONG						FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_TV_OUT_MUSIC_LIST_SONG				FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_STORAGE_HINT						FONT_HEIGHT_lv4

	//setting
	#define		TV_D1_FONT_HEIGHT_SETTING_MENU						FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_LANGUAGE					FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_TIME_FORMAT_TYPE			FONT_HEIGHT_lv4		//24H or 12H
	#define		TV_D1_FONT_HEIGHT_SETTING_TIME_FORMAT_TIME			FONT_HEIGHT_lv4		//time(24H or 12H)
	#define		TV_D1_FONT_HEIGHT_SETTING_DATE_FORMAT				FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_SETTING_WEEK_FORMAT_WEEK			FONT_HEIGHT_lv1
	#define		TV_D1_FONT_HEIGHT_SETTING_WEEK_FORMAT_MONTH			FONT_HEIGHT_lv2
	#define		TV_D1_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_CLOCK	FONT_HEIGHT_lv2
	#define		TV_D1_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_HINT		FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_DATE		FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_VERSION					FONT_HEIGHT_lv2
	#define		TV_D1_FONT_HEIGHT_SETTING_PHOTO_DATE				FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_FULL_IMAGE				FONT_HEIGHT_lv4
	#define		TV_D1_FONT_HEIGHT_SETTING_SLIDESHOW_TYPE			FONT_HEIGHT_lv4

	//	calendar
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE1_CLOCK				FONT_HEIGHT_lv2		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_YEAR					FONT_HEIGHT_lv2		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_MONTH				FONT_HEIGHT_lv2		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_WEEK					FONT_HEIGHT_lv1		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_DAY					FONT_HEIGHT_lv2		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_TIME					FONT_HEIGHT_lv4		//800X480
	#define		TV_D1_FONT_HEIGHT_CALENDAR_MODE2_AMPM					FONT_HEIGHT_lv3		//800X480

	//	alarm
	#define		TV_D1_FONT_HEIGHT_ALARM_HINT							FONT_HEIGHT_lv2//FONT_HEIGHT_lv3
	#define		TV_D1_FONT_HEIGHT_ALARM_TIME							FONT_HEIGHT_lv3

	//	photo view
	#define		TV_D1_FONT_HEIGHT_PHOTO_VIEW_HINT					FONT_HEIGHT_lv2



	#define		TV_VGA_FONT_HEIGHT_LOADING								FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_POP_MESSAGE							FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SLIDE_SHOW_DATE						FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SLIDE_SHOW_DATE_CHAR				FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv2
	#define		TV_VGA_FONT_HEIGHT_MAIN_MENU_HINT						FONT_HEIGHT_lv4

	//	for pop window ( Time Interval, Volume Control, Music List, Storage Source )
	#define		TV_VGA_FONT_HEIGHT_POP_WINDOW_TITLE					FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_TV_OUT_POP_WINDOW_TITLE			FONT_HEIGHT_lv4

	#define		TV_VGA_FONT_HEIGHT_POP_WINDOW_HINT					FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_TV_OUT_POP_WINDOW_HINT			FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_POP_WINDOW_NUMBER					FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_TV_OUT_POP_WINDOW_NUMBER			FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_MUSIC_LIST_SONG						FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_TV_OUT_MUSIC_LIST_SONG				FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_STORAGE_HINT						FONT_HEIGHT_lv4

	//setting
	#define		TV_VGA_FONT_HEIGHT_SETTING_MENU						FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_LANGUAGE					FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_TIME_FORMAT_TYPE			FONT_HEIGHT_lv4		//24H or 12H
	#define		TV_VGA_FONT_HEIGHT_SETTING_TIME_FORMAT_TIME			FONT_HEIGHT_lv4		//time(24H or 12H)
	#define		TV_VGA_FONT_HEIGHT_SETTING_DATE_FORMAT					FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_SETTING_WEEK_FORMAT_WEEK			FONT_HEIGHT_lv1
	#define		TV_VGA_FONT_HEIGHT_SETTING_WEEK_FORMAT_MONTH			FONT_HEIGHT_lv2
	#define		TV_VGA_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_CLOCK		FONT_HEIGHT_lv2
	#define		TV_VGA_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_HINT		FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_DATE		FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_VERSION						FONT_HEIGHT_lv2
	#define		TV_VGA_FONT_HEIGHT_SETTING_PHOTO_DATE				FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_FULL_IMAGE				FONT_HEIGHT_lv4
	#define		TV_VGA_FONT_HEIGHT_SETTING_SLIDESHOW_TYPE			FONT_HEIGHT_lv4

	//	calendar
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE1_CLOCK				FONT_HEIGHT_lv2		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_YEAR					FONT_HEIGHT_lv2		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_MONTH				FONT_HEIGHT_lv2		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_WEEK					FONT_HEIGHT_lv1		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_DAY					FONT_HEIGHT_lv2		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_TIME					FONT_HEIGHT_lv4		//800X480
	#define		TV_VGA_FONT_HEIGHT_CALENDAR_MODE2_AMPM					FONT_HEIGHT_lv3		//800X480

	//	alarm
	#define		TV_VGA_FONT_HEIGHT_ALARM_HINT							FONT_HEIGHT_lv2//FONT_HEIGHT_lv3
	#define		TV_VGA_FONT_HEIGHT_ALARM_TIME							FONT_HEIGHT_lv3

	//	photo view
	#define		TV_VGA_FONT_HEIGHT_PHOTO_VIEW_HINT					FONT_HEIGHT_lv2


	#define		TV320x240_FONT_HEIGHT_LOADING								FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_POP_MESSAGE							FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SLIDE_SHOW_DATE						FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SLIDE_SHOW_DATE_CHAR				FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_OSD_HINT							FONT_HEIGHT_lv2
	#define		TV320x240_FONT_HEIGHT_MAIN_MENU_HINT						FONT_HEIGHT_lv4

	//	for pop window ( Time Interval, Volume Control, Music List, Storage Source )
	#define		TV320x240_FONT_HEIGHT_POP_WINDOW_TITLE					FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_TV_OUT_POP_WINDOW_TITLE			FONT_HEIGHT_lv4

	#define		TV320x240_FONT_HEIGHT_POP_WINDOW_HINT					FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_TV_OUT_POP_WINDOW_HINT			FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_POP_WINDOW_NUMBER					FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_TV_OUT_POP_WINDOW_NUMBER			FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_MUSIC_LIST_SONG					FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_TV_OUT_MUSIC_LIST_SONG			FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_STORAGE_HINT						FONT_HEIGHT_lv4

	//setting
	#define		TV320x240_FONT_HEIGHT_SETTING_MENU						FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SETTING_LANGUAGE					FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SETTING_TIME_FORMAT_TYPE			FONT_HEIGHT_lv4		//24H or 12H
	#define		TV320x240_FONT_HEIGHT_SETTING_TIME_FORMAT_TIME			FONT_HEIGHT_lv4		//time(24H or 12H)
	#define		TV320x240_FONT_HEIGHT_SETTING_DATE_FORMAT				FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_SETTING_WEEK_FORMAT_WEEK			FONT_HEIGHT_lv1
	#define		TV320x240_FONT_HEIGHT_SETTING_WEEK_FORMAT_MONTH			FONT_HEIGHT_lv2
	#define		TV320x240_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_CLOCK	FONT_HEIGHT_lv2
	#define		TV320x240_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_HINT		FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SETTING_DATE_TIME_FORMAT_DATE		FONT_HEIGHT_lv4
	#define		TV320x240_FONT_HEIGHT_SETTING_VERSION					FONT_HEIGHT_lv2
	#define		TV320x240_FONT_HEIGHT_SETTING_PHOTO_DATE				FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_SETTING_FULL_IMAGE				FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_SETTING_SLIDESHOW_TYPE			FONT_HEIGHT_lv3
	
	//	calendar
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE1_CLOCK				FONT_HEIGHT_lv2		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_YEAR				FONT_HEIGHT_lv2		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_MONTH				FONT_HEIGHT_lv2		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_WEEK				FONT_HEIGHT_lv1		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_DAY				FONT_HEIGHT_lv2		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_TIME				FONT_HEIGHT_lv4		//800X480
	#define		TV320x240_FONT_HEIGHT_CALENDAR_MODE2_AMPM				FONT_HEIGHT_lv3		//800X480

	//	alarm
	#define		TV320x240_FONT_HEIGHT_ALARM_HINT							FONT_HEIGHT_lv2//FONT_HEIGHT_lv3
	#define		TV320x240_FONT_HEIGHT_ALARM_TIME							FONT_HEIGHT_lv3

	//	photo view
	#define		TV320x240_FONT_HEIGHT_PHOTO_VIEW_HINT					FONT_HEIGHT_lv2


#endif /*__NVRAM_VARIABLE_H__*/
