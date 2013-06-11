//================Function Declaration=================
//author: erichan
//date  :2010-08-12 
//================Include Area=========================
#include "state_cam.h"
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
#include "drv_l1_usbd_Cam.h"
#else
#include "drv_l1_usbd.h"
#include "drv_l1_msdc.h"
//#include "drv_l1_usbd_tbl.h"
#include "usbdesc.h"
#include "usb.h"
#include "usbcore.h"
#include "uvc.h"
#include "usbuser.h"
#include "ap_usb.h"
#include "avi_encoder_app.h"
#endif

#if C_UVC == CUSTOM_ON
/////////////////////////////////////////////////////////////////////////////////
//================Define Area==========================
#define USB_CAM_QUEUE_MAX_LEN    	 20
#define C_USB_CAM_STATE_STACK_SIZE   128
#define USB_AUDIO_QUEUE_MAX_LEN    	 8
#define C_USB_AUDIO_STACK_SIZE   128
//================Struct Area==========================

//================Function Declaration=================
INT32S usb_cam_task_create(INT8U pori);
INT32S usb_cam_task_del(void);
void SendOneFrameTaskEntry(void *param);
#if C_USB_AUDIO == CUSTOM_ON
INT32S usb_audio_task_create(INT8U pori, INT16U usb_audio_task_create);
INT32S usb_audio_task_del(void);
void USBAudioTaskEntry(void *param);
#endif
//=====================================================
INT32U USBCamStateStack[C_USB_CAM_STATE_STACK_SIZE];
void *USBCamApQ_Stack[USB_CAM_QUEUE_MAX_LEN];

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
void *iso_task_Q_Stack[USB_CAM_QUEUE_MAX_LEN];
INT32U  iso_send_len;
#endif

#if C_USB_AUDIO == CUSTOM_ON
INT32U USBAudioStack[C_USB_AUDIO_STACK_SIZE];
void *USBAudioQ_Stack[USB_AUDIO_QUEUE_MAX_LEN];
#endif

#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
INT32U size;
INT32U addr;
#endif

OS_EVENT  *USBCamApQ;
#if C_USB_AUDIO == CUSTOM_ON
OS_EVENT  *USBAudioApQ;
#endif
//=====================================================
extern BYTE *SOF_Event_Buf;
extern volatile DWORD UVC_Delay;
extern volatile DWORD TestCnt;
extern volatile DWORD JPG_Cnt;
//extern volatile DWORD Buf_Size;
extern volatile DWORD PTS_Value;
extern volatile BYTE  PicsToggle;

extern OS_EVENT  *usbwebcam_frame_q;
extern INT8U s_usbd_pin;
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX)
extern INT32U SendISOData;
extern INT32U SendAudioData;
#endif

//=====================================================
INT32S usb_cam_entrance(INT16U usetask, INT16U jpeg_width, INT16U jpeg_height, INT16U aud_sample_rate)
{
	INT32S nRet;
	INT32U tmp;
	
	// USB Cam Init
	UVC_Delay = 0;
	TestCnt = 0;
	JPG_Cnt = 0;
	PTS_Value = 0;
	PicsToggle = 0;
	
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)	
   	tmp = (INT32U)gp_malloc_align(192,16);
    if(tmp == 0) {
    	return STATUS_FAIL;
    }
    
	SOF_Event_Buf =(char *) tmp;
	usb_initial_Cam();
	usb_initial();
	
	vic_irq_register(VIC_USB, usb_isr_Cam);
	vic_irq_enable(VIC_USB);
    
    nRet = usb_cam_task_create(USB_CAM_GPL325XX);
    if(nRet < 0) {
    	DBG_PRINT("usb_cam_task_create fail !!!");
		return STATUS_FAIL;
	}
	
#else
    tmp = (INT32U)gp_malloc_align(EP7_MAX_PACKET,16);
    if(tmp == 0) {
    	return STATUS_FAIL;
    }
    
	SOF_Event_Buf =(char *) tmp;
	DBG_PRINT("usb_initial start \r\n");
    usb_initial();
    
#if C_USB_AUDIO == CUSTOM_ON
    usb_os_event_init();
#endif
	usbd_desc_jpg_size_set(jpeg_width, jpeg_height);
    DBG_PRINT("usb_initial end \r\n");
    vic_irq_register(VIC_USB, usb_isr);//usb_isr);
	vic_irq_enable(VIC_USB);
	nRet = usb_cam_task_create(USB_CAM_PRIORITY);
    if(nRet < 0) {
    	DBG_PRINT("usb_cam_task_create fail !!!");
		return STATUS_FAIL;
	}
	
#if C_USB_AUDIO == CUSTOM_ON   	
    nRet = usb_audio_task_create(USB_AUDIO_PRIORITY, aud_sample_rate);
    if(nRet < 0) {
    	DBG_PRINT("usb_audio_task_create fail !!!");
		return STATUS_FAIL;
	}
	
    OSTaskChangePrio (AUD_ENC_PRIORITY, USB_AUD_ENC_PRIORITY);
#endif
#endif
	return STATUS_OK;
}

void usb_cam_exit(void)
{
	gp_free((void *) SOF_Event_Buf);
	usb_cam_task_del();
	
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)	
	vic_irq_disable(VIC_USB);
	usb_uninitial_Cam();
	usb_uninitial();
	
#else
#if C_USB_AUDIO == CUSTOM_ON
	usb_audio_task_del();
	usb_os_event_uninit();
	OSTaskChangePrio (USB_AUD_ENC_PRIORITY,AUD_ENC_PRIORITY);
#endif
	vic_irq_disable(VIC_USB);
	usb_uninitial();
	
#endif
}

#if C_USB_AUDIO == CUSTOM_ON
INT32S usb_audio_task_create(INT8U pori, INT16U aud_sample_rate)
{
	INT8U err;
	
	USBAudioApQ = OSQCreate(USBAudioQ_Stack, USB_AUDIO_QUEUE_MAX_LEN);
    if(!USBAudioApQ) {
    	return STATUS_FAIL;
    } 
    
    err = OSTaskCreate(USBAudioTaskEntry, (void*)NULL, &USBAudioStack[C_USB_AUDIO_STACK_SIZE - 1], pori);
	if(err != OS_NO_ERR) {
		return STATUS_FAIL;
	}
		
	iso_send_len = aud_sample_rate/1000*2;
	return STATUS_OK;
}

INT32S usb_audio_task_del(void)
{
	INT8U   err;
    
	OSTaskDel(USB_AUDIO_PRIORITY);
	OSQFlush(USBAudioApQ);
	OSQDel(USBAudioApQ, OS_DEL_ALWAYS, &err);
	USBAudioApQ = NULL;
	return STATUS_OK;
}
#endif
//=====================================================
INT32S usb_cam_task_create(INT8U pori)
{
	INT8U err;

	//creat usb cam message
	USBCamApQ = OSQCreate(USBCamApQ_Stack, USB_CAM_QUEUE_MAX_LEN);
    if(!USBCamApQ) {
    	return STATUS_FAIL;
    }
    
    //creat usb cam task
    err = OSTaskCreate(SendOneFrameTaskEntry,  (void*)NULL,&USBCamStateStack[C_USB_CAM_STATE_STACK_SIZE - 1], pori);
	if(err != OS_NO_ERR) {
		return STATUS_FAIL;
	}
	return STATUS_OK;
}

INT32S usb_cam_task_del(void)
{
	INT8U   err;
    
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)     
	OSTaskDel(USB_CAM_GPL325XX);
#else
	OSTaskDel(USB_CAM_PRIORITY);
#endif	
	OSQFlush(USBCamApQ);
	OSQDel(USBCamApQ, OS_DEL_ALWAYS, &err);
	USBCamApQ = NULL;
	return STATUS_OK;
}

#if C_USB_AUDIO == CUSTOM_ON
void USBAudioTaskEntry(void *param)
{
	INT8U   err;
	INT32S  samp = 0;
	ISOTaskMsg  *audiobuf;
	INT32U  addr;
	INT32U  len;
	INT32S  ret;
	
	DBG_PRINT("WebCamAudEntry\r\n"); 
	while(1) {
		
		audiobuf = (ISOTaskMsg *) OSQPend(USBAudioApQ, 0, &err);
		if(err != OS_NO_ERR)
		    continue;
		    		
		addr = audiobuf->AddrFrame;
		len = audiobuf->FrameSize;
		if(!s_usbd_pin) SendAudioData = 0;
		if(SendAudioData) {
			samp = 0;
			while(1) {
				ret = Drv_USBD_AUDIO_ISOIN_DMA_START((INT8U*)(addr+samp),iso_send_len);
				if (ret != STATUS_OK) {
					break;
				}
				samp += iso_send_len;
				ret = Drv_USBD_AudioISOI_DMA_Finish();
				if (ret != STATUS_OK) {
					break;
				}
				if ((samp >= len) || (!SendAudioData)) {
					break;
				}
			}
		}
	}
}
#endif

extern BOOL Drv_USBD_ISOI_DMA_START(char *buf,INT16U ln);
extern BOOL Drv_USBD_ISOI_DMA_Finish(void);
//=====================================================
void SendOneFrameTaskEntry(void *param)
{
	INT8U err;
    ISOTaskMsg  *isosend0;
     
#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
	DBG_PRINT("WebCamVidEntry\r\n"); 
	
    while(1)
	{
		if(s_usbd_pin == 0) {
			OSTimeDly(5);
			continue;
		} 
		
		if(s_usbd_pin && PicsToggle == 1 && (JPG_Cnt == 0||TestCnt==0)) {
			isosend0 = (ISOTaskMsg *)OSQPend(USBCamApQ, 0, &err);//if encode one frame end
			size = isosend0->FrameSize;
			addr = isosend0->AddrFrame;
			PicsToggle = 0;
			JPG_Cnt = 0;
			TestCnt = 1;
			OSQPost(usbwebcam_frame_q, (void *)addr);
		}
		usb_std_service_Cam(0);
	}
	
#else
	BOOL sendok = 0;
    BOOL ret;
    INT32U  addr;
	INT32U  len;
	
	DBG_PRINT("WebCamVidEntry\r\n"); 
	while(1)
	{
		isosend0 = (ISOTaskMsg *)OSQPend(USBCamApQ, 0, &err);//if encode one frame end
		sendok = 0;
		addr = isosend0->AddrFrame;
		len = isosend0->FrameSize;
		if(!s_usbd_pin) SendISOData = 0;
		if(SendISOData)//if open video device and usb plug 
		{ 
			while(!sendok)
			{
				sendok = USB_SOF_Event(len,addr);
               	Drv_USBD_ISOI_DMA_START((char*)SOF_Event_Buf,EP7_MAX_PACKET);
			   	ret = Drv_USBD_ISOI_DMA_Finish();
			   	if(ret==FALSE||(!SendISOData))//time out or  alt is 0 then  quit this frame
			   	{
			    	break;
			   	}         
			 }
	   	}
	    OSQPost(usbwebcam_frame_q, (void *)addr);
    }
#endif
}
#endif

