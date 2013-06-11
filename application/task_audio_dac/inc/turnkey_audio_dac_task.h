#ifndef __TURNKEY_AUDIO_DAC_TASK_H__
#define __TURNKEY_AUDIO_DAC_TASK_H__
#include "application.h"

extern INT8U  channel;
extern INT32U sample_rate;
extern OS_EVENT    *aud_avi_q;
extern FP64 g_avi_synchroframe; 
extern FP64 g_avi_synchrocnt;
extern INT32S g_avi_datalength;
extern INT16S      *buf_left;
extern INT8S       edd_step;
extern INT32U      last_send_idx;
extern INT32U      total_play_time;
extern INT8U       start_time_send;

extern void audio_dac_task_entry(void *p_arg);

#endif /*__TURNKEY_AUDIO_DAC_TASK_H__*/
