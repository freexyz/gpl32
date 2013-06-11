#include "fsystem.h"

void fs_sd_ms_plug_out_flag_en(void);
INT32U fs_sd_ms_plug_out_flag_get(void);
void fs_sd_ms_plug_out_flag_reset(void);
INT32U fs_usbh_plug_out_flag_en(void);
INT8U fs_usbh_plug_out_flag_get(void);
void fs_usbh_plug_out_flag_reset(void);


static INT32U supre_plug_out_flag=0;
static INT32U super_usbh_plug_out_flag=0;
//static INT32U supre_cf_plug_out_flag=0;

void fs_sd_ms_plug_out_flag_en(void)
{
    supre_plug_out_flag=1;
}

INT32U fs_sd_ms_plug_out_flag_get(void)
{
#if SUPPORT_STG_SUPER_PLUGOUT == 1
    return supre_plug_out_flag;
#else
    return 0;
#endif
}

void fs_sd_ms_plug_out_flag_reset(void)
{
    supre_plug_out_flag=0;
}

INT32U fs_usbh_plug_out_flag_en(void)
{
    return super_usbh_plug_out_flag=1;
}

INT8U fs_usbh_plug_out_flag_get(void)
{
#if SUPPORT_STG_SUPER_PLUGOUT == 1
    return super_usbh_plug_out_flag;
#else
    return 0;
#endif
}

void fs_usbh_plug_out_flag_reset(void)
{
    super_usbh_plug_out_flag=0;
}

#if 0
void fs_cf_plug_out_flag_en(void)
{
    supre_cf_plug_out_flag=1;
}

INT8U fs_cf_plug_out_flag_get(void)
{
#if SUPPORT_STG_SUPER_PLUGOUT == 1
    return supre_cf_plug_out_flag;
#else
    return 0;
#endif
}

void fs_cf_plug_out_flag_reset(void)
{
    supre_cf_plug_out_flag=0;
}

typedef enum {
    FS_STG_NONE=0,
    FS_STG_SD=0x1,
    FS_STG_MSC=0x10,
    FS_STG_USBH=0x100,
    FS_STG_CF=0x1000,
    FS_STG_MAX
} FS_STG_ID;

typedef enum {
    FS_SUPER_NONE=0,
    FS_SUPER_PLUGOUT=1
} FS_SUPER_PLUG_STATUS;

INT32U fs_stg_plug_out_flag=0;

void fs_stg_sw_plugout_reg(FS_STG_ID fs_stg_id)
{
    fs_stg_plug_out_flag |= fs_stg_id;
}

void fs_stg_sw_plugout_reset(FS_STG_ID fs_stg_id)
{
    fs_stg_plug_out_flag &= ~(fs_stg_id);
}

void fs_stg_sw_plugout_reset_all(void)
{
    fs_stg_plug_out_flag = 0;
}

FS_SUPER_PLUG_STATUS fs_stg_sw_plugout_status_get(FS_STG_ID fs_stg_id)
{
    if ( (fs_stg_plug_out_flag | fs_stg_id) )
    {
        return FS_SUPER_PLUGOUT;
    }
    else
    {
        return FS_SUPER_NONE;
    }
}
#endif
