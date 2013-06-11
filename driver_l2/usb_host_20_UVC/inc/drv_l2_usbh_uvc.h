#ifndef	__drv_l2_USBH_UVC_TASK_H__
#define	__drv_l2_USBH_UVC_TASK_H__

#include "application.h"
#include "drv_l2_usbh.h"
#include <String.h>

//----- main uvc task control message -----
typedef enum
{
	MSG_UVC_INIT = 1,
	MSG_UVC_PREVIEW_EN,
	MSG_UVC_PREVIEW_DIS,			
	MSG_UVC_RECORD_EN,
	MSG_UVC_RECORD_DIS,
	MSG_UVC_FORCE_CAPTURE_EN,
	MSG_UVC_CAPTURE_EN,
	MSG_UVC_CLOSE	
}UVC_MSG_CMD;
//----- main uvc task status message -----
typedef enum
{
	MSG_INIT_SUCCESS=1,
	MSG_INIT_FAIL,
	MSG_USB_FAIL,
	MSG_PREVIEW_INIT_SUCCESS,
	MSG_PREVIEW_INIT_FAIL,
	MSG_PREVIEW_FRAME_END,
	MSG_CAPTURE_FAIL,
	MSG_CAPTURE_SUCCESS
}MSG_UVC_STATUS;

typedef struct
{
	unsigned char dev;
	MSG_Q_ID ret;
}ST_UVC_INIT_CTRL;


typedef struct
{
	unsigned char	format_index;
	unsigned char	frame_index;
	unsigned 		buf_num;
	unsigned char	**buf;
	unsigned		ln;
}ST_UVC_PREVIEW_CTRL;

typedef struct
{
	unsigned type;
	unsigned char* buf;
	unsigned ln;
}ST_UVC_CAPTURE;

typedef struct
{
	unsigned Buf_Num;
	unsigned ln;
}ST_UVC_FRAME_PASER;

extern MSG_Q_ID	uvc_frame_msg_q_id;
//extern MSG_Q_ID	uvc_FrameEnd_msg_q_id;
extern void usbh_uvc_mjpg_task_entry(void *parm);
extern void usbh_uvc_record_task_entry(void *parm);

#endif