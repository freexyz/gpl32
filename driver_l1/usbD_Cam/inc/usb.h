/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USB.H
 *      Purpose: USB Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USB_H__
#define __USB_H__

/******************************************************************************/
#define ISO_JPG_WIDTH              640//320//
#define ISO_JPG_HEIGHT             480//240//
#define MaxVideoFrameBufSize  	   0x10000//64K

#include "driver_l1.h"
#include "type.h"
/******************************************************************************/
#ifndef __CS_COMPILER__
	PACKED typedef union {
	  WORD W;
	  PACKED struct {
	    BYTE L;
	    BYTE H;
	  } WB;
	} WORD_BYTE;
#else
	typedef union {
	  WORD W;
	  struct {
	    BYTE L;
	    BYTE H;
	  } PACKED WB;
	}PACKED WORD_BYTE;
#endif

/* bmRequestType.Dir */
#define REQUEST_HOST_TO_DEVICE     0
#define REQUEST_DEVICE_TO_HOST     1

/* bmRequestType.Type */
#define REQUEST_STANDARD           0
#define REQUEST_CLASS              1
#define REQUEST_VENDOR             2
#define REQUEST_RESERVED           3

/* bmRequestType.Recipient */
#define REQUEST_TO_DEVICE          0
#define REQUEST_TO_INTERFACE       1
#define REQUEST_TO_ENDPOINT        2
#define REQUEST_TO_OTHER           3

/* bmRequestType Definition */
#ifndef __CS_COMPILER__
	PACKED typedef union _REQUEST_TYPE {
	  PACKED struct _BM {
	    int Recipient : 5;//BYTE
	    int Type      : 2;
	    int Dir       : 1;
	  } BM;
	  BYTE B;
	} REQUEST_TYPE;
#else
	typedef union _REQUEST_TYPE {
	  struct _BM {
	    int Recipient : 5;//BYTE
	    int Type      : 2;
	    int Dir       : 1;
	  } PACKED BM;
	  BYTE B;
	} PACKED REQUEST_TYPE;
#endif

/* USB Standard Request Codes */
#define USB_REQUEST_GET_STATUS                 0
#define USB_REQUEST_CLEAR_FEATURE              1
#define USB_REQUEST_SET_FEATURE                3
#define USB_REQUEST_SET_ADDRESS                5
#define USB_REQUEST_GET_DESCRIPTOR             6
#define USB_REQUEST_SET_DESCRIPTOR             7
#define USB_REQUEST_GET_CONFIGURATION          8
#define USB_REQUEST_SET_CONFIGURATION          9
#define USB_REQUEST_GET_INTERFACE              10
#define USB_REQUEST_SET_INTERFACE              11
#define USB_REQUEST_SYNC_FRAME                 12

/* USB GET_STATUS Bit Values */
#define USB_GETSTATUS_SELF_POWERED             0x01
#define USB_GETSTATUS_REMOTE_WAKEUP            0x02
#define USB_GETSTATUS_ENDPOINT_STALL           0x01

/* USB Standard Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL             0
#define USB_FEATURE_REMOTE_WAKEUP              1

/* USB Default Control Pipe Setup Packet */
#ifndef __CS_COMPILER__
	PACKED typedef struct _USB_SETUP_PACKET {
	  REQUEST_TYPE bmRequestType;
	  BYTE         bRequest;
	  WORD_BYTE    wValue;
	  WORD_BYTE    wIndex;
	  WORD         wLength;
	} USB_SETUP_PACKET;
#else
	typedef struct _USB_SETUP_PACKET {
	  REQUEST_TYPE bmRequestType;
	  BYTE         bRequest;
	  WORD_BYTE    wValue;
	  WORD_BYTE    wIndex;
	  WORD         wLength;
	} PACKED USB_SETUP_PACKET;
#endif

/* USB Descriptor Types */
// see InterfaceAssociationDescriptor_ecn.pdf
#define USB_DEVICE_DESCRIPTOR_TYPE                 1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE          2
#define USB_STRING_DESCRIPTOR_TYPE                 3
#define USB_INTERFACE_DESCRIPTOR_TYPE              4
#define USB_ENDPOINT_DESCRIPTOR_TYPE               5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE       6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE     7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE        8
#define USB_OTG_DESCRIPTOR_TYPE                    9
#define USB_DEBUG_DESCRIPTOR_TYPE                 10
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE 11 

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_MISCELLANEOUS         0xEF
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK                0xC0
#define USB_CONFIG_BUS_POWERED                 0x80
#define USB_CONFIG_SELF_POWERED                0x40
#define USB_CONFIG_REMOTE_WAKEUP               0x20

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) | 0x00)
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30


#define USB_POWER           0
#define USB_IF_NUM          4
#define USB_EP_NUM          32
#define USB_MAX_PACKET0     8//64
#define USB_DMA             0
#define USB_DMA_EP          0x00000000

#define USB_POWER_EVENT     0
#define USB_RESET_EVENT     1
#define USB_SUSPEND_EVENT   0
#define USB_RESUME_EVENT    0
#define USB_WAKEUP_EVENT    0
#define USB_SOF_EVENT       1
#define USB_ERROR_EVENT     0
#define USB_EP_EVENT        0x000F
#define USB_CONFIGURE_EVENT 1
#define USB_INTERFACE_EVENT 0
#define USB_FEATURE_EVENT   0


/*
// <e0> USB Class Support
//   <e1> Human Interface Device (HID)
//     <o2> Interface Number <0-255>
//   </e>
//   <e3> Mass Storage
//     <o4> Interface Number <0-255>
//   </e>
//   <e5> Audio Device
//     <o6> Control Interface Number <0-255>
//     <o7> Streaming Interface 1 Number <0-255>
//     <o8> Streaming Interface 2 Number <0-255>
//   </e>
//   <e9> Communication Device
//     <o10> Control Interface Number <0-255>
//     <o11> Bulk Interface Number <0-255>
//     <o12> Max Communication Device Buffer Size
//        <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes 
//   </e>
//   <e14> Video Device
//     <o15> Video Control Interface Number <0-255>
//     <o16> Video Streaming Interface Number <0-255>
//   </e>
// </e>
*/

#define USB_CLASS           1
#define USB_HID             0
#define USB_HID_IF_NUM      0
#define USB_MSC             0
#define USB_MSC_IF_NUM      0
#define USB_AUDIO           0
#define USB_ADC_CIF_NUM     0
#define USB_ADC_SIF1_NUM    1
#define USB_ADC_SIF2_NUM    2
#define USB_CDC  			0
#define USB_CDC_CIF_NUM     0
#define USB_CDC_DIF_NUM     1
#define USB_CDC_BUFSIZE     64
#define USB_NO_13           0
#define USB_UVC  			1
#define USB_UVC_VCIF_NUM    1
#define USB_UVC_VSIF_NUM    2


//#define COMPOSITE_VIDEO

/* USB Standard Device Descriptor */
#ifndef __CS_COMPILER__
	PACKED typedef struct _USB_DEVICE_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bcdUSB;
	  BYTE  bDeviceClass;
	  BYTE  bDeviceSubClass;
	  BYTE  bDeviceProtocol;
	  BYTE  bMaxPacketSize0;
	  WORD  idVendor;
	  WORD  idProduct;
	  WORD  bcdDevice;
	  BYTE  iManufacturer;
	  BYTE  iProduct;
	  BYTE  iSerialNumber;
	  BYTE  bNumConfigurations;
	} USB_DEVICE_DESCRIPTOR;

	/* USB 2.0 Device Qualifier Descriptor */
	PACKED typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bcdUSB;
	  BYTE  bDeviceClass;
	  BYTE  bDeviceSubClass;
	  BYTE  bDeviceProtocol;
	  BYTE  bMaxPacketSize0;
	  BYTE  bNumConfigurations;
	  BYTE  bReserved;
	} USB_DEVICE_QUALIFIER_DESCRIPTOR;


	/* USB Standard Configuration Descriptor */
	PACKED typedef struct _USB_CONFIGURATION_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  wTotalLength;
	  BYTE  bNumInterfaces;
	  BYTE  bConfigurationValue;
	  BYTE  iConfiguration;
	  BYTE  bmAttributes;
	  BYTE  bMaxPower;
	} USB_CONFIGURATION_DESCRIPTOR;

	/* USB Standard Interface Descriptor */
	PACKED typedef struct _USB_INTERFACE_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  BYTE  bInterfaceNumber;
	  BYTE  bAlternateSetting;
	  BYTE  bNumEndpoints;
	  BYTE  bInterfaceClass;
	  BYTE  bInterfaceSubClass;
	  BYTE  bInterfaceProtocol;
	  BYTE  iInterface;
	} USB_INTERFACE_DESCRIPTOR;

	/* USB Standard Endpoint Descriptor */
	PACKED typedef struct _USB_ENDPOINT_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  BYTE  bEndpointAddress;
	  BYTE  bmAttributes;
	  WORD  wMaxPacketSize;
	  BYTE  bInterval;
	} USB_ENDPOINT_DESCRIPTOR;

	/* USB String Descriptor */
	PACKED typedef struct _USB_STRING_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bString/*[]*/;
	} USB_STRING_DESCRIPTOR;

	/* USB Common Descriptor */
	PACKED typedef struct _USB_COMMON_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	} USB_COMMON_DESCRIPTOR;
#else
	typedef struct _USB_DEVICE_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bcdUSB;
	  BYTE  bDeviceClass;
	  BYTE  bDeviceSubClass;
	  BYTE  bDeviceProtocol;
	  BYTE  bMaxPacketSize0;
	  WORD  idVendor;
	  WORD  idProduct;
	  WORD  bcdDevice;
	  BYTE  iManufacturer;
	  BYTE  iProduct;
	  BYTE  iSerialNumber;
	  BYTE  bNumConfigurations;
	} PACKED USB_DEVICE_DESCRIPTOR;

	/* USB 2.0 Device Qualifier Descriptor */
	typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bcdUSB;
	  BYTE  bDeviceClass;
	  BYTE  bDeviceSubClass;
	  BYTE  bDeviceProtocol;
	  BYTE  bMaxPacketSize0;
	  BYTE  bNumConfigurations;
	  BYTE  bReserved;
	} PACKED USB_DEVICE_QUALIFIER_DESCRIPTOR;


	/* USB Standard Configuration Descriptor */
	typedef struct _USB_CONFIGURATION_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  wTotalLength;
	  BYTE  bNumInterfaces;
	  BYTE  bConfigurationValue;
	  BYTE  iConfiguration;
	  BYTE  bmAttributes;
	  BYTE  bMaxPower;
	} PACKED USB_CONFIGURATION_DESCRIPTOR;

	/* USB Standard Interface Descriptor */
	typedef struct _USB_INTERFACE_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  BYTE  bInterfaceNumber;
	  BYTE  bAlternateSetting;
	  BYTE  bNumEndpoints;
	  BYTE  bInterfaceClass;
	  BYTE  bInterfaceSubClass;
	  BYTE  bInterfaceProtocol;
	  BYTE  iInterface;
	} PACKED USB_INTERFACE_DESCRIPTOR;

	/* USB Standard Endpoint Descriptor */
	typedef struct _USB_ENDPOINT_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  BYTE  bEndpointAddress;
	  BYTE  bmAttributes;
	  WORD  wMaxPacketSize;
	  BYTE  bInterval;
	} PACKED USB_ENDPOINT_DESCRIPTOR;

	/* USB String Descriptor */
	typedef struct _USB_STRING_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	  WORD  bString/*[]*/;
	} PACKED USB_STRING_DESCRIPTOR;

	/* USB Common Descriptor */
	typedef struct _USB_COMMON_DESCRIPTOR {
	  BYTE  bLength;
	  BYTE  bDescriptorType;
	} PACKED USB_COMMON_DESCRIPTOR;
#endif

#endif  /* __USB_H__ */
