#ifndef __STATE_CAM_H__
#define __STATE_CAM_H__
/* parents header include */
#include "video_codec_callback.h"
#include "type.h"

INT32S usb_cam_entrance(INT16U usetask, INT16U width, INT16U height, INT16U aud_sample_rate);
void usb_cam_exit(void);
INT32S usb_audio_task_create(INT8U pori, INT16U aud_sample_rate);
INT32S usb_audio_task_del(void);

#endif  /*__STATE_CALENDAR_H__*/