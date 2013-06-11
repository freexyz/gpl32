#ifndef __drv_l1_USB_HOST_H__
#define __drv_l1_USB_HOST_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"


#define		BIT0		0x0001
#define		BIT1		0x0002
#define		BIT2		0x0004
#define		BIT3		0x0008
#define		BIT4		0x0010
#define		BIT5		0x0020
#define		BIT6		0x0040
#define		BIT7		0x0008
#define		BIT8		0x0100
#define		BIT9		0x0200
#define		BIT10		0x0400
#define		BIT11		0x0800
#define		BIT12		0x1000
#define		BIT13		0x2000
#define		BIT14		0x4000
#define		BIT15		0x8000

#ifndef __USBH_TYPEDEF__
#define __USBH_TYPEDEF__
typedef struct  {
		unsigned long DataTransferLength;
		unsigned int CBWFlag;
		unsigned int LUN;
		unsigned int CBLength;
}CBW_Struct,*BW_Struct;


typedef struct  {
		unsigned int OPCode;
		unsigned long LBA;
		unsigned int TXParameter;
}SCSI_Struct;
#endif //__USBH_TYPEDEF__

extern INT32U	g_ifchirp_pass;
extern CBW_Struct  CBWStruct;
extern SCSI_Struct SCSIStruct;
extern INT32U arUSBHostBuffer[64];
extern INT32U arUSBHostCSWBuffer[16]; 
extern INT32U INTINEP;
extern INT32U BulkINEP;
extern INT32U BulkOUTEP;

extern INT32U EP0MaxPacketSize;
extern INT32U ResetUSBHost ;
extern INT32U R_Device_Address;

extern INT32U DeviceClass;
extern INT32U  USBH_MaxLUN  ;
extern INT32U  INTPipeDataToggle ;
extern INT32U  BulkINDataToggle  ;
extern INT32U  BulkOUTDataToggle ;
extern INT32U  R_USBHDMAEN       ;
extern INT32U  R_USBHLUNStatus   ;	
extern INT32U  USBSectorSize	   ;

extern INT32U InterfaceProtocol;
extern INT32U InterfaceNumber;
extern INT32U InterfaceSubClass;
extern INT32U InterfaceNumber;
extern INT32U INTPipeDataToggle;

extern INT16U usbh_vid;
extern INT16U usbh_pid;

extern INT32U usb_host_libraryversion(void);
extern INT32U USB_host_initial(void);
extern INT32U USBH_Device_Reset(void);
extern INT32U Send_EP0_Status_Stage(INT32U idata);
extern INT32U USBH_Check_Status(void);
extern INT32U SCSISendCMD(void);
extern INT32U EP0StandardCommand(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U EP0_Data_Stage_OUT(INT32U Data_Number);
extern INT32U EP0_Data_Stage_IN(INT32U Data_Number);

extern void F_Delay(void);
extern void USBH_Delay(INT32U t);
extern void USB_Host_DevAdr(INT32U addr);
extern void USB_Host_EndPoint(INT32U edp);
extern INT32U USBH_Device_Reset(void);
extern INT32U CheckPortStatus(void);
extern INT32U Data_IN(INT32U DataTransferLength ,INT32U* GetHostBuffer_PTR);
extern INT32U GetConfigurationNumber(void);
extern INT32U DecodeDescriptorConfiguration(void);
extern INT32U GetRequestData(void);
extern void Enable_USB_PLUG_IN(void);
extern void Disable_USB_PLUG_IN(void);
extern void Enable_USB_PLUG_OUT(void);
extern void Disable_USB_PLUG_OUT(void);
extern void USB_Host_Enable(void);
extern INT32U USB_Host_Signal(void);
extern void USB_Host_Clrirqflag(void);
extern void USBPlugOutEN(void);
extern void GetDeviceLength(INT32U Lun);
extern INT32U CSWStatus(INT32U * GetHostBuffer_PTR);
extern INT32U GetMaxLUNNumber(void);
extern void USBH_SELF_RST(void);
extern INT32U USBG_GET_ACK_CNT(void);
extern void usbh_usb20_chirp(void);
#endif /*__drv_l1_USB_HOST_H__*/

