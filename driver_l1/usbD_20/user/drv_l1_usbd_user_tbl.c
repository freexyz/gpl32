#include "driver_l1.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ========================================//
//#if ((defined _DRV_L1_USBD) || (defined _DRV_L1_USBH)) && ((_DRV_L1_USBD == 1)||(_DRV_L1_USBH == 1))   //
#if ((defined _DRV_L1_USBD) ) &&(_DRV_L1_USBD ==CUSTOM_ON) 
//=====================================================================================================//

/****************************************************************************
*	USB device descriptors
****************************************************************************/
char Device_Descriptor_TBL[]={
     0x12,
 	 0x01,				        //bDescriptorType	: Device
	 0x00, 0x02,				//bcdUSB			: version 2.00
	 0x00, 						//bDeviceClass
	 0x00, 						//bDeviceSubClass
	 0x00,						//bDeviceProtocol
	#if  MCU_VERSION  >= GPL326XX	
     0x40,  					//bMaxPacketSize0	
    #else
     0x08                       //bMaxPacketSize0	
    #endif   
	 0x3F, 0x1B,				//idVendor
	 0x52, 0x0C,				//idProduct
	 0x00, 0x01,				//bcdDevice
	 0x01,						//iManufacturer
	 0x02,						//iProduct
	 0x03,						//iSerialNumber
	 0x01,						//bNumConfigurations
};

#if  MCU_VERSION >= GPL326XX
char Qualifier_Descriptor_TBL[]=
{
	0x0A,                   //bLength: 0x0A byte
	0x06,                   //bDescriptorType: DEVICE_QUALIFIER
	0x00, 0x02,             //bcdUSB: version 200 // 0x00,0x02 
	0x00,                   //bDeviceClass: 
	0x00,                   //bDeviceSubClass:
	0x00,                   //bDeviceProtocol: 
	0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
	0x01,                   //bNumConfigurations: 1 configuration
	0x00					//bReserved
};
#endif
char Config_Descriptor_TBL[] ={//Configuration (0x09 byte)
    0x09,                   //bLength: 0x09 byte
    0x02,                   //bDescriptorType: CONFIGURATION
    0x20,                   //wTotalLength:
    0x00,
    0x01,                   //bNumInterfaces: 1 interfaces
    0x01,                   //bConfigurationValue: configuration 1
    0x00,                   //iConfiguration: index of string
    0xC0,                    //bmAttributes: bus/self powered, Not Support Remote-Wakeup//Modify Q For USB IF  120919
    0xFA,                   //MaxPower: 500 mA
// Interface_Descriptor
    0x09,                   //bLength: 0x09 byte
    0x04,                   //bDescriptorType: INTERFACE
    0x00,                   //bInterfaceNumber: interface 0
    0x00,                   //bAlternateSetting: alternate setting 0
    0x02,                   //bNumEndpoints: 2 endpoints(EP1,EP2)
    0x08,                   //bInterfaceClass: Mass Storage Devices Class
    0x06,                   //bInterfaceSubClass:
    0x50,                   //bInterfaceProtocol
    0x02,                   //iInterface: index of string  0x00
//  Endpoint1
    0x07,                   //bLength: 0x07 byte
    0x05,                   //bDescriptorType: ENDPOINT
    0x81,                   //bEndpointAddress: IN endpoint 1 --
    0x02,                   //bmAttributes: Bulk
    0x40, 0x00,             //wMaxPacketSize: 64 byte
    0x00,                   //bInterval: ignored
//Endpoint2
    0x07,                   //bLength: 0x07 byte
    0x05,                   //bDescriptorType: ENDPOINT
    0x02,                   //bEndpointAddress: OUT endpoint 2 -- 
    0x02,                   //bmAttributes: Bulk
    0x40, 0x00,             //wMaxPacketSize: 64 byte
    0x00,                   //bInterval: ignored
};
 
char String0_Descriptor[]={
	 0x04,		//bLength
	 0x03,		//bDescriptorType
	 0x09, 0x04,//bString
};


char String1_Descriptor[] ={
     0x40,       //bLength
	 0x03,		//bDescriptorType
	 'G', 0x00,	//bString
	 'e', 0x00,
	 'n', 0x00,
	 'e', 0x00,
 	 'r', 0x00,
 	 'i', 0x00, 	 
 	 'c', 0x00, 	 
  	 ' ', 0x00, 	 
   	 'U', 0x00, 	 
  	 'S', 0x00, 	    	 
  	 'B', 0x00, 	    	 
  	 ' ', 0x00, 	   	   	 
  	 'M', 0x00, 	 
  	 'a', 0x00, 	 
  	 's', 0x00, 	 
  	 's', 0x00, 	   	 
  	 ' ', 0x00, 	 
  	 'S', 0x00, 	 
  	 't', 0x00, 	   	   	 	 
  	 'o', 0x00, 	   	   	 	 
  	 'r', 0x00, 	   	   	 	   	 
  	 'a', 0x00, 	   	   	 	   	 
  	 'g', 0x00, 	  
   	 'e', 0x00, 	  
   	 ' ', 0x00, 	   	   	 
  	 'D', 0x00, 	 
  	 'e', 0x00, 	   	   	 	 
  	 'v', 0x00, 	   	   	 	 
  	 'i', 0x00, 	   	   	 	   	 
  	 'c', 0x00, 	   	   	 	   	 
  	 'e', 0x00, 	   	   	 	   	    	   	  	   	 	   	 
 };

char String2_Descriptor[]={
     0x22,//bLength
	 0x03,		//bDescriptorType
	 'G', 0x00,	//bString
	 'E', 0x00,
	 'N', 0x00,
	 'E', 0x00,
	 'R', 0x00,
	 'A', 0x00,
	 'L', 0x00,
	 'P', 0x00,
	 'L', 0x00,
	 'U', 0x00,
	 'S', 0x00,
	 '-', 0x00,
	 'M', 0x00,
	 'S', 0x00,
	 'D', 0x00,
	 'C', 0x00,
};

char String3_Descriptor[]={//Modify Q For USB IF  120919
     0x22, //bLength
	 0x03,		//bDescriptorType
	 '0', 0x00,	//bString
	 '1', 0x00,	//bString
	 '2', 0x00,
	 '3', 0x00,
	 '4', 0x00,	
	 '5', 0x00,
	 '6', 0x00,
	 '7', 0x00,	
	 '8', 0x00,
	 '9', 0x00,
	 'A', 0x00,	
	 'B', 0x00,
	 'C', 0x00,
	 'D', 0x00,	
	 'E', 0x00,
	 'F', 0x00,
};

/****************************************************************************
*	MSDC related tables
****************************************************************************/
//MSDC table
     
/*
static char DeviceVersionInfo[] = {

	0x10,	//0~3: IC body
	0x00,
	0x00,
	0x01,
	0xFF,	//4~7: IC Version, 0xFFFFFF means mask rom
	0xFF,
	0xFF,	//
	0x03,
	0x08,	//Year
	0x01,	//Month
	0x23,	//Day
};

*/
char SCSIInquiryData[] = {

	0x00,
	0x80,
	0x00,
	0x01,
	0x1F,
	0x00,
	0x00,
	0x00,
	'G','E','N','P','L','U','S',
	0x20,
	'U','S','B','-','M','S','D','C',' ','D','I','S','K',
	' ',
	'A',
	0x20,
	'1','.','0','0','G','P','-','P','R','O','D','.',
};

char SCSIInquiryData_CDROM[] = {

	0x05,
	0x80,
    0x02,//0x04,
	0x01,//0x00,
	0x1F,
	0x00,
	0x00,
	0x00,
	'G','E','N','P','L','U','S',
	0x20,
	'U','S','B','-','C','D','R','M',' ','D','I','S','K',
	' ',
	'A',
	0x20,
	'1','.','0','0','G','P','-','P','R','O','D','.',
};
//=== This is for code configuration DON'T REMOVE or MODIFY it ===========================================//
#endif //((defined _DRV_L1_USBD) || (defined _DRV_L1_USBH)) && ((_DRV_L1_USBD == 1)||(_DRV_L1_USBH == 1)) //
//========================================================================================================//