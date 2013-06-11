#include "application.h"
#include "MultiMedia_Platform.h"

/* TASK Stack size define */
#define C_AUDIO_TASK_STACK_SIZE		4096
#define C_AUDIO_BG_TASK_STACK_SIZE	2048
#define C_AUDIO_DAC_TASK_STACK_SIZE	1024
#define	C_FILESRV_TASK_STACK_SIZE	1024
#define	C_IMAGE_TASK_STACK_SIZE	    1024
#define C_MIDI_TASK_STACK_SIZE		2048
#define C_IDLE_TASK_STACK_SIZE		64
#define C_AVI_STATUS_Q_AREA_SIZE	5

/* OS Task Stack declare */
INT32U	AudioTaskStack[C_AUDIO_TASK_STACK_SIZE];
INT32U	AudioDacTaskStack[C_AUDIO_DAC_TASK_STACK_SIZE];

#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
INT32U	AudioBGTaskStack[C_AUDIO_BG_TASK_STACK_SIZE];
INT32U	MIDITaskStack[C_MIDI_TASK_STACK_SIZE];
#endif

INT32U	Filesrv[C_FILESRV_TASK_STACK_SIZE];
INT32U 	ImageTaskStack[C_IMAGE_TASK_STACK_SIZE];
INT32U  IdleTaskStack[C_IDLE_TASK_STACK_SIZE];

/* gloabl varaible */
MSG_Q_ID	ApQ;
MSG_Q_ID	Audio_FG_status_Q, Audio_BG_status_Q, MIDI_status_Q;
OS_EVENT	*avi_status_q;		
void 	 	*avi_status_q_area[C_AVI_STATUS_Q_AREA_SIZE];	
OS_EVENT    *SemScanFile;

/* extern varaible */
//DAC 
extern OS_EVENT	*hAudioDacTaskQ, *aud_send_q, *aud_bg_send_q, *aud_avi_q, *aud_right_q;
//Audio
extern MSG_Q_ID AudioTaskQ;
extern OS_EVENT	*audio_wq, *audio_fsq;
//wma seek
extern OS_EVENT  *Mbox_WMA_Play_Seek_Flag,*Mbox_WMA_Play_Seek_Offset;
//Audio BG
extern MSG_Q_ID AudioBGTaskQ;
extern OS_EVENT	*audio_bg_wq, *audio_bg_fsq;
extern OS_EVENT	*midi_wq, *midi_fsq;	// added by Bruce, 2008/10/09
//file serv
extern MSG_Q_ID	fs_msg_q_id, fs_scan_msg_q_id;
//image 
extern MSG_Q_ID image_msg_queue_id;
extern OS_EVENT *image_task_fs_queue_a, *image_task_fs_queue_b;

//========================= idle task ===========================
void idle_task_entry(void *para)
{
	OS_CPU_SR cpu_sr;
	INT16U i;
	
	while (1) {
		OS_ENTER_CRITICAL();
		i=0x5005;
		R_SYSTEM_WAIT = i;
		i = R_CACHE_CTRL;

		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		ASM(NOP);
		OS_EXIT_CRITICAL();	    
	}
}

//========================= Platform Initialization ===========================
//
// Task Create : Audio, DAC, Image, File, AP(Demo Code) Task
// Global Variable Initialization
// Component Initialization : File system, Memory Manager,
// Driver Initialization: DMA Manager, SDC, USB Host
//=============================================================================
void platform_entrance(void *free_memory)
{
	INT8U ret;
	
	/* Create Semaphore */
    SemScanFile = OSSemCreate(1);
    //add WMA_Seek
	Mbox_WMA_Play_Seek_Flag = OSMboxCreate(NULL);
    Mbox_WMA_Play_Seek_Offset = OSMboxCreate(NULL);

	//enable uc-os timer
	timer_freq_setup(0, OS_TICKS_PER_SEC, 0, OSTimeTick);

	ApQ = msgQCreate(1024, 1024, AP_QUEUE_MSG_MAX_LEN);
	Audio_FG_status_Q = msgQCreate(5, 2, AP_QUEUE_MSG_MAX_LEN);
	Audio_BG_status_Q = msgQCreate(5, 2, AP_QUEUE_MSG_MAX_LEN);
	MIDI_status_Q = msgQCreate(5, 2, AP_QUEUE_MSG_MAX_LEN);
	avi_status_q = OSQCreate(avi_status_q_area, C_AVI_STATUS_Q_AREA_SIZE);	//for avi status
	
	ret = OSTaskCreate(audio_dac_task_entry, (void *) 0, &AudioDacTaskStack[C_AUDIO_DAC_TASK_STACK_SIZE - 1], DAC_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("DAC Task Create[%d]\r\n", DAC_PRIORITY);
	} else {
		DBG_PRINT("DAC Task Create Fail[%d]\r\n", DAC_PRIORITY);	
	}
#endif
	
	ret = OSTaskCreate(audio_task_entry, (void *) 0, &AudioTaskStack[C_AUDIO_TASK_STACK_SIZE - 1], AUD_DEC_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("Audio Task Create[%d]\r\n", AUD_DEC_PRIORITY);
	} else {
		DBG_PRINT("Audio Task Create Fail[%d]\r\n", AUD_DEC_PRIORITY);	
	}
#endif

#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	ret = OSTaskCreate(midi_task_entry, (void *) 0, &MIDITaskStack[C_MIDI_TASK_STACK_SIZE - 1], MIDI_DEC_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("MIDI Task Create[%d]\r\n", MIDI_DEC_PRIORITY);
	} else {
		DBG_PRINT("MIDI Task Create Fail[%d]\r\n", MIDI_DEC_PRIORITY);	
	}
#endif
	
	ret = OSTaskCreate(audio_bg_task_entry, (void *) 0, &AudioBGTaskStack[C_AUDIO_BG_TASK_STACK_SIZE - 1], AUD_DEC_BG_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("Audio BG Task Create[%d]\r\n", AUD_DEC_BG_PRIORITY);
	} else {
		DBG_PRINT("Audio BG Task Create Fail[%d]\r\n", AUD_DEC_BG_PRIORITY);	
	}
#endif
#endif

	ret = OSTaskCreate(filesrv_task_entry,(void *) 0, &Filesrv[C_FILESRV_TASK_STACK_SIZE - 1], FS_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("FileServ Task Create[%d]\r\n", FS_PRIORITY);
	} else {
		DBG_PRINT("FileServ Task Create Fail[%d]\r\n", FS_PRIORITY);	
	}
#endif

	ret = OSTaskCreate(image_task_entry, (void *) 0, &ImageTaskStack[C_IMAGE_TASK_STACK_SIZE - 1], IMAGE_PRIORITY);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("Image Task Create[%d]\r\n", IMAGE_PRIORITY);
	} else {
		DBG_PRINT("Image Task Create Fail[%d]\r\n", IMAGE_PRIORITY);	
	}
#endif

#if 0	
	ret = OSTaskCreate(idle_task_entry, (void *) 0, &IdleTaskStack[C_IDLE_TASK_STACK_SIZE - 1], OS_LOWEST_PRIO-1);
#if DBG_MESSAGE == 1
	if(ret == OS_NO_ERR) {
		DBG_PRINT("Idle Task Create[%d]\r\n", OS_LOWEST_PRIO-1);
	} else {
		DBG_PRINT("Idle Task Create Fail[%d]\r\n", OS_LOWEST_PRIO-1);	
	}
#endif
#endif 
}

void platform_exit(void)
{
	INT8U err;
		
	//DAC
	err = OSTaskDel(DAC_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", DAC_PRIORITY);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", DAC_PRIORITY);
	}
#endif
	
	OSQFlush(hAudioDacTaskQ);
	OSQDel(hAudioDacTaskQ, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail hAudioDacTaskQ\r\n");
	}
#endif
	
	OSQFlush(aud_send_q);
	OSQDel(aud_send_q, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail aud_send_q\r\n");
	}
#endif
	
	OSQFlush(aud_bg_send_q);
	OSQDel(aud_bg_send_q, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail aud_bg_send_q\r\n");
	}
#endif
	
	OSQFlush(aud_avi_q);
	OSQDel(aud_avi_q, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail aud_avi_q\r\n");
	}
#endif
	
	OSQFlush(aud_right_q);
	OSQDel(aud_right_q, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail aud_right_q\r\n");
	}
#endif

	//audio 
	err = OSTaskDel(AUD_DEC_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", AUD_DEC_PRIORITY);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", AUD_DEC_PRIORITY);
	}
#endif	
	
	msgQFlush(AudioTaskQ);
	msgQDelete(AudioTaskQ);
	OSQFlush(audio_wq);
	OSQDel(audio_wq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail audio_wq\r\n");
	}
#endif
	
	OSQFlush(audio_fsq);
	OSQDel(audio_fsq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail audio_fsq\r\n");
	}
#endif

#if (defined MCU_VERSION) && (MCU_VERSION < GPL327XX)
	err = OSTaskDel(MIDI_DEC_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", MIDI_DEC_PRIORITY);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", MIDI_DEC_PRIORITY);
	}
#endif	
	
	msgQFlush(MIDITaskQ);
	msgQDelete(MIDITaskQ);
	OSQFlush(midi_wq);
	OSQDel(midi_wq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail midi_wq\r\n");
	}
#endif
	
	OSQFlush(midi_fsq);
	OSQDel(midi_fsq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail midi_fsq\r\n");
	}
#endif
	
	//audio BG 
	err = OSTaskDel(AUD_DEC_BG_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", AUD_DEC_BG_PRIORITY);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", AUD_DEC_BG_PRIORITY);
	}
#endif	
	
	msgQFlush(AudioBGTaskQ);
	msgQDelete(AudioBGTaskQ);
	OSQFlush(audio_bg_wq);
	OSQDel(audio_bg_wq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail audio_bg_wq\r\n");
	}
#endif
	
	OSQFlush(audio_bg_fsq);
	OSQDel(audio_bg_fsq, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail audio_bg_fsq\r\n");
	}
#endif
#endif
		
	//File server 
	err = OSTaskDel(FS_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {
		DBG_PRINT("OSTaskDel Success[%d]\r\n", FS_PRIORITY);
	} else {	
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", FS_PRIORITY);
	}
#endif

	msgQFlush(fs_msg_q_id);
	msgQDelete(fs_msg_q_id);	
	msgQFlush(fs_scan_msg_q_id);
	msgQDelete(fs_scan_msg_q_id);			

	//image
	err = OSTaskDel(IMAGE_PRIORITY);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", IMAGE_PRIORITY);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", IMAGE_PRIORITY);
	}
#endif

	msgQFlush(image_msg_queue_id);
	msgQDelete(image_msg_queue_id);
	OSQFlush(image_task_fs_queue_a);
	OSQDel(image_task_fs_queue_a, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail image_task_fs_queue_a\r\n");
	}
#endif
	
	OSQFlush(image_task_fs_queue_b);
	OSQDel(image_task_fs_queue_b, OS_DEL_ALWAYS, &err);
#if DBG_MESSAGE == 1
	if(err != OS_NO_ERR) {
		DBG_PRINT("OSQDel Fail image_task_fs_queue_b\r\n");
	}
#endif
	
	//idle
	err = OSTaskDel(OS_LOWEST_PRIO-1);
#if DBG_MESSAGE == 1
	if(err == OS_NO_ERR) {	
		DBG_PRINT("OSTaskDel Success[%d]\r\n", OS_LOWEST_PRIO-1);
	} else {
		DBG_PRINT("OSTaskDel Fail[%d]\r\n", OS_LOWEST_PRIO-1);
	}
#endif
		
	//other 
	msgQFlush(ApQ);
	msgQDelete(ApQ);
	
	msgQFlush(Audio_FG_status_Q);
	msgQDelete(Audio_FG_status_Q);
	
	msgQFlush(Audio_BG_status_Q);
	msgQDelete(Audio_BG_status_Q);
	
	msgQFlush(MIDI_status_Q);
	msgQDelete(MIDI_status_Q);
	
	OSQFlush(avi_status_q);
	OSQDel(avi_status_q, OS_DEL_ALWAYS, &err);	
}