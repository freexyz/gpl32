#ifndef __STATE_USB_DVICE_WEBCAMERA_DEMO_H__
#define __STATE_USB_DVICE_WEBCAMERA_DEMO_H__
#include "storages.h"
#define CUSTOM_ON							1
#define CUSTOM_OFF							0
#define C_USBDEVICE_PIN   					IO_F5
#define C_MODE_PIN   						IO_A0
#define USB_TASK_ENTRY	   					CUSTOM_ON 
#define	TSK_PRI_VID_USB_DEVICE_WEBCAMERA		31

extern INT8U   s_usbmode_check_pin;
extern BOOLEAN usb_plug_states_flag;
extern OS_EVENT *USBTaskQ;
extern BOOLEAN Usb_Dvice_WebCam_Demo_turn_on_flag;

#endif