#ifndef __NVRAM_BARIABLE_H__
#define __NVRAM_BARIABLE_H__
/* it's pseudo header definition of turnkey_umi_task.h , no need to include other one*/
/* Non-Volitile globale variable struct */
typedef enum {
    NV_SPI=0,
    NV_NAND,
    NV_NOR,
    NV_PSRAM,
    NV_MAX
} NV_ID_ENUM;

typedef enum {
    LANGUAGE_ENGLISH=0,
    LANGUAGE_CHINESE_GB,
    LANGUAGE_CHINESE_ZH,
    LANGUAGE_MAX
} LANGUAGE_ID_ENUM;

typedef enum {
    BL_LEVEL_1=0,
    BL_LEVEL_2,
    BL_LEVEL_3,
    BL_LEVEL_4,
    BL_LEVEL_5,
    BL_LEVEL_6,
    BL_LEVEL_7,
    BL_LEVEL_MAX
} BACK_LIGHT_LEVEL_ENUM;

typedef enum {
    VOL_LEVEL_1=0,
    VOL_LEVEL_2,
    VOL_LEVEL_3,
    VOL_LEVEL_4,
    VOL_LEVEL_5,
    VOL_LEVEL_6,
    VOL_LEVEL_7,
    VOL_LEVEL_MAX
} VOLUME_LEVEL_ENUM;

typedef enum {
    KEY_TONE_LEVEL_1=0,
    KEY_TONE_LEVEL_2,
    KEY_TONE_LEVEL_3,
    KEY_TONE_LEVEL_MAX
} KEY_TONE_LEVEL_ENUM;

typedef enum {
    STORAGE_USB=0,
    STORAGE_SD,
    STORAGE_CF,
    STORAGE_NAND,
    STORAGE_MS,
    STORAGE_MSPRO,
    STORAGE_XD,
    STORAGE_MAX
} STORAGE_TYPE_ENUM;

typedef enum {
    LAST_STATE_STARTUP=0,
    LAST_STATE_ICONSHOW,
    LAST_STATE_SLIDESHOW,
    LAST_STATE_MENU,
    LAST_STATE_PHOTO_VIEW,
    LAST_STATE_MAX
} LAST_STATE_ENUM;


typedef struct {
    LANGUAGE_ID_ENUM        language;
    BACK_LIGHT_LEVEL_ENUM   bk_light;
    VOLUME_LEVEL_ENUM       vol_level;
    KEY_TONE_LEVEL_ENUM     key_vol_level;
    STORAGE_TYPE_ENUM       current_storage_id;
    LAST_STATE_ENUM              last_state;
} NV_VAL_STRUCT;

#endif /*__NVRAM_BARIABLE_H__*/