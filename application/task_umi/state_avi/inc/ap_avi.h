#ifndef __AP_SLIDE_SHOW_H__
#define __AP_SLIDE_SHOW_H__
#include "state_avi.h"

extern STAVIBufConfig		*avi_decode_buf;
extern STAVIBufConfig		*avi_display_buf;
extern STAVIAUDIOBufConfig *avi_audio_buf_wi;
extern STAVIAUDIOBufConfig *avi_audio_buf_ri;
extern INT32U 				*charram;
extern STAviShowControl	*pstAviShowControl;

#endif /*__AP_SLIDE_SHOW_H__*/