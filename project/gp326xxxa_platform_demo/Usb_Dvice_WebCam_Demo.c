#include "application.h"
#include "state_cam.h"
#include "Usb_Dvice_WebCam_Demo.h"
#include "state_usb.h"
#define USB_TASK_QUEUE_MAX		32
#define USB_TASK_STACK_SIZE		512
BOOLEAN get_mode_check_pin(void);
INT32U USBTaskStack[USB_TASK_STACK_SIZE];
void *usb_task_q_stack[USB_TASK_QUEUE_MAX];
void usbd_mode_pin_detection_init(void);
void usbcamera_init(void);
void usb_states_entry(void* para);
void Usb_Dvice_WebCam_Demo(void)
{
	INT8S  err;
	err = OSTaskCreate(usb_states_entry, (void *) 0, &USBTaskStack[USB_TASK_STACK_SIZE - 1], TSK_PRI_VID_USB_DEVICE_WEBCAMERA);
	if(err < 0){
		DBG_PRINT("Create Usb_Dvice_WebCam_Demo task fail\r\n");		
		while(1);
	}
}
void usb_states_entry(void* para)
{
	INT32U msg_id;
	INT8U err;
	DBG_PRINT("in usb task\r\n");	
	Usb_Dvice_WebCam_Demo_turn_on_flag =1;
	usbd_mode_pin_detection_init();
	storages_init(0x30);//Nand
	USBTaskQ = OSQCreate(usb_task_q_stack, USB_TASK_QUEUE_MAX);  
	while(1){
			msg_id = (INT32U) OSQPend(USBTaskQ, 0, &err);
			if((!msg_id) || (err != OS_NO_ERR)) {
	  	     	continue;
	  	  }	
		switch(msg_id){
//			case MSG_USBD_INITIAL:
//				storages_init(0x30);//Nand

			case MSG_USBD_PLUG_IN:  	
				DBG_PRINT("usb device in\r\n");		
				state_usb_entry((void *)1);	
				DBG_PRINT("usb device out\r\n");	
			break;
			case MSG_USBCAM_PLUG_IN:
				usbcamera_init(); 	
				DBG_PRINT("P-CAM in\r\n");		
				usb_cam_entrance(0, 640, 480, 8000);	
				usb_webcam_state();
	
				DBG_PRINT("P-CAM Exit\r\n");
				video_encode_webcam_stop();
				OSTimeDly(30);
				usb_cam_exit();	
				video_encode_exit();
				break;
			default:
				break;
		}
	}
}
BOOLEAN get_usbdevice_pin_states(void){
	BOOLEAN ret;
	ret = gpio_read_io(C_USBDEVICE_PIN);
	return ret;
}
BOOLEAN get_mode_check_pin(void){// camera or masstorge{
	BOOLEAN ret;
	ret = gpio_read_io(C_MODE_PIN);
	return ret;
}
void usbd_mode_pin_detection_init(void){
    gpio_init_io(C_MODE_PIN,GPIO_INPUT);
  	gpio_set_port_attribute(C_MODE_PIN, ATTRIBUTE_HIGH);
  	gpio_write_io(C_MODE_PIN,DATA_LOW);
}
void usbcamera_init(void){
	VIDEO_ARGUMENT arg;
	
#if C_DISPLAY_DEVICE >= C_TV_QVGA
    tv_init();
#if C_DISPLAY_DEVICE == C_TV_QVGA
	tv_start (TVSTD_NTSC_J_NONINTL, TV_QVGA, TV_NON_INTERLACE);	
#elif C_DISPLAY_DEVICE == C_TV_VGA
	tv_start (TVSTD_NTSC_J, TV_HVGA, TV_INTERLACE);
#elif C_DISPLAY_DEVICE == C_TV_D1
	tv_start (TVSTD_NTSC_J, TV_D1, TV_NON_INTERLACE);
#else
	while(1);
#endif
#else
	tft_init();
	tft_start(C_DISPLAY_DEVICE);
#endif

	user_defined_video_codec_entrance();
	video_encode_entrance();     // Initialize and allocate the resources needed by Video decoder	
		 	
	arg.bScaler = 1;
	arg.TargetWidth = 640;
	arg.TargetHeight = 480;
	arg.SensorWidth	= 640;
	arg.SensorHeight = 480;
	arg.DisplayWidth = C_DISPLAY_DEV_HPIXEL;	//display width
	arg.DisplayHeight = C_DISPLAY_DEV_VPIXEL;	//display height
	arg.DisplayBufferWidth = C_DISPLAY_DEV_HPIXEL;	//display buffer width
	arg.DisplayBufferHeight = C_DISPLAY_DEV_VPIXEL;//display buffer height
	
	arg.VidFrameRate = 30;
	arg.AudSampleRate = 8000;
	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; //for display
	video_encode_webcam_start(arg);
}