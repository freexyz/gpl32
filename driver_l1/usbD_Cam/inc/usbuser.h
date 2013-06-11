/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBUSER.H
 *      Purpose: USB Custom User Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBUSER_H__
#define __USBUSER_H__


/* USB Device Events Callback Functions */
extern void USB_Power_Event     (BOOL  power);
extern void USB_Reset_Event     (void);
extern void USB_Suspend_Event   (void);
extern void USB_Resume_Event    (void);
extern void USB_WakeUp_Event    (void);
extern void USB_SOF_Event       (void);
extern void USB_Error_Event     (DWORD error);

/* USB Endpoint Callback Events */
#define USB_EVT_SETUP       1   /* Setup Packet */
#define USB_EVT_OUT         2   /* OUT Packet */
#define USB_EVT_IN          3   /*  IN Packet */
#define USB_EVT_OUT_NAK     4   /* OUT Packet - Not Acknowledged */
#define USB_EVT_IN_NAK      5   /*  IN Packet - Not Acknowledged */
#define USB_EVT_OUT_STALL   6   /* OUT Packet - Stalled */
#define USB_EVT_IN_STALL    7   /*  IN Packet - Stalled */
#define USB_EVT_OUT_DMA_EOT 8   /* DMA OUT EP - End of Transfer */
#define USB_EVT_IN_DMA_EOT  9   /* DMA  IN EP - End of Transfer */
#define USB_EVT_OUT_DMA_NDR 10  /* DMA OUT EP - New Descriptor Request */
#define USB_EVT_IN_DMA_NDR  11  /* DMA  IN EP - New Descriptor Request */
#define USB_EVT_OUT_DMA_ERR 12  /* DMA OUT EP - Error */
#define USB_EVT_IN_DMA_ERR  13  /* DMA  IN EP - Error */

#define EP3_MAX_PACKET      192//96//0x1FE//96//
#define PAYLOAD_HEADER_SIZE 0x0C

/* USB Endpoint Events Callback Pointers */
extern void (* const USB_P_EP[16])(DWORD event);

/* USB Endpoint Events Callback Functions */
//extern void USB_EndPoint0  (DWORD event);
void USB_EndPoint0(DWORD event);

extern void USB_EndPoint1  (DWORD event);
extern void USB_EndPoint2  (DWORD event);
extern void USB_EndPoint3  (DWORD event);
extern void USB_EndPoint4  (DWORD event);
extern void USB_EndPoint5  (DWORD event);
extern void USB_EndPoint6  (DWORD event);
extern void USB_EndPoint7  (DWORD event);
extern void USB_EndPoint8  (DWORD event);
extern void USB_EndPoint9  (DWORD event);
extern void USB_EndPoint10 (DWORD event);
extern void USB_EndPoint11 (DWORD event);
extern void USB_EndPoint12 (DWORD event);
extern void USB_EndPoint13 (DWORD event);
extern void USB_EndPoint14 (DWORD event);
extern void USB_EndPoint15 (DWORD event);

/* USB Core Events Callback Functions */
extern void USB_Configure_Event (void);
extern void USB_Interface_Event (void);
extern void USB_Feature_Event   (void);

extern void Write_To_Buf(void);

extern volatile DWORD UVC_Delay;
extern volatile DWORD TestCnt;
extern volatile DWORD JPG_Cnt;
extern volatile DWORD Buf_Size;
extern volatile DWORD PTS_Value;
extern volatile DWORD SCR_Value;
extern volatile DWORD JPG_size;
//extern const unsigned char *JPG_data;
extern volatile DWORD JPG_data;
extern volatile BYTE  PicsToggle;
//extern          BYTE SOF_Event_Buf[EP3_MAX_PACKET];
extern          BYTE *SOF_Event_Buf;


#endif  /* __USBUSER_H__ */
