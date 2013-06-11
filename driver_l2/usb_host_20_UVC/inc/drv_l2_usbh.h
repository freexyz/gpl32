#ifndef	__drv_l2_USBH_UVC_H__
#define	__drv_l2_USBH_UVC_H__

#include "driver_l2.h"
#include "drv_l1_usb_host_UVC.h"

/* USB class code */
typedef enum
{
	UC_DEVICE 	= 0x00,
	UC_AUDIO	= 0x01,
	UC_CDC_CTRL	= 0x02,
	UC_HID		= 0x03,
	UC_PHYSICAL	= 0x05,
	UC_IMAGE	= 0x06,
	UC_PRINTER	= 0x07,
	UC_MSC		= 0x08,
	UC_HUB		= 0x09,
	UC_CDC		= 0x0A,
	UC_CCID		= 0x0B,
	UC_CS		= 0x0D,
	UC_VIDEO	= 0x0E
}USB_CLASS_CODES;

/* USB descriptor types */
typedef enum
{
	DT_SEVICE			= 0x01,		/* Device descriptor */ 
	DT_CONFIG			= 0x02,		/* Configuration descriptor */
	DT_STRING			= 0x03,		/* String descriptor */
	DT_IF				= 0x04,		/* Interface descriptor */
	DT_ENDP				= 0x05,		/* Endpoint descriptor */
	DT_DEVICE_QUAL		= 0x06,		/* Device qualifier descriptor */
	DT_OTHER_SPEED		= 0x07,		/* Other speed configuration descriptor*/
	DT_IF_POW			= 0x08,		/* Interface power descriptor */
	DT_IF_ASSOCIATION	= 0x0B,		/* Interface association */
	/* Video Class-Specific Descriptor Types */
	DT_VCS_UNDEFINE		= 0x20,		/* Undefined descriptor */
	DT_VCS_DEVICE		= 0x21,		/* Device descriptor */
	DT_VCS_CONFIG		= 0x22,		/* Configuration descriptor */
	DT_VCS_STRING		= 0x23,		/* String descriptor */
	DT_VCS_IF			= 0x24,		/* Interface descriptor */
	DT_VCS_ENDP			= 0x25		/* Endpoint descriptor */
}USB_DESC_TYPE;

typedef enum
{
	VS_UNDEFINED			= 0x00,
	VS_INPUT_HEADER			= 0x01,
	VS_OUTPUT_HEADER 		= 0x02,
	VS_STILL_IMAGE_FRAME	= 0x03,
	VS_FORMAT_UNCOMPRESSED	= 0x04,
	VS_FRAME_UNCOMPRESSED	= 0x05,
	VS_FORMAT_MJPEG			= 0x06,
	VS_FRAME_MJPEG			= 0x07,
	VS_FORMAT_MPEG2TS		= 0x0A,
	VS_FORMAT_DV			= 0x0C,
	VS_COLORFPRMAT			= 0x0D,
	VS_FORMAT_FRAME_BASED	= 0x10,
	VS_FRAME_FRAME_BASED	= 0x11,
	VS_FORMAT_STREAM_BASED	= 0x12
}UVC_VS_SUBTYPE;
//----- USB Video Class Specific Request Codes -----
typedef enum
{
	RC_UNDEFINED	= 0x00,
	SET_CUR			= 0x01,
	GET_CUR			= 0x81,
	GET_MIN			= 0x82,
	GET_MAX			= 0x83,
	GET_RES			= 0x84,
	GET_LEN			= 0x85,
	GET_INFO		= 0x86,
	GET_DEF			= 0x87
}UVC_CS_REQ;
//----- UVC Video Streaming Interface Control Selector -----
typedef enum
{
	VS_CONTROL_UNDEFINED	= 0x00,
	VS_PROBE_CONTROL,
	VS_COMMIT_CONTROL,
	VS_STILL_PROBE_CONTROL,
	VS_STILL_COMMIT_CONTROL,
	VS_STILL_IMAGE_TRIGGER_CONTROL,
	VS_STREAM_ERROR_CODE_CONTROL,
	VS_GENERATE_KEY_FRAME_CONTROL,
	VS_UPDATE_FRAME_SEGMENT_CONTROL,
	VS_SYNCH_DELAY_CONTROL
}UVC_VS_REQ;

typedef enum
{
	UVC_DATA_UNCOMPRESSED_YUV2 = 1,
	UVC_DATA_UNCOMPRESSED_NV12,
	UVC_DATA_MJPEG
}UVC_DATA_FORMAT;

#define UVC_MAX_FORMAT 4
#define UVC_MAX_FRAME  10

typedef struct
{
	unsigned short Width;
	unsigned short Height;
}ST_Resolution;

typedef struct
{
	unsigned short	IMG_NUM;
	ST_Resolution	res[UVC_MAX_FRAME];
}ST_UVC_STILL_INFO;
	
typedef struct
{
	unsigned char		FrameIndex;
	ST_Resolution		res;
	unsigned char		FrameInterType;
	unsigned 			MaxBuffer;
	unsigned			MinFrameInter;
	unsigned			MaxFrameInter;
	unsigned			FrameStep;
	unsigned			FrameInter[10];
}ST_UVC_FRAME_INFO;
	
typedef struct
{
	unsigned char		FormatIndex;
	unsigned char 		FormatType;
	unsigned char		FrameTypeNum;
	ST_UVC_FRAME_INFO	FRAME_INFO[UVC_MAX_FRAME];
	ST_UVC_STILL_INFO	STILL_INFO;
}ST_UVC_FORMAT_INFO;
	
typedef struct		
{
	unsigned char		FormatTypeNum;
	unsigned char 		CaptureMode;
	ST_UVC_FORMAT_INFO	Format_INFO[UVC_MAX_FORMAT];
}ST_UVC_STREAM_INFO;
//----- USB Video Class Interface information -----
typedef struct
{
	unsigned char 	IF;				/* Interface Number */
	unsigned char	Alt_Num;		/* Alternative Interface Number */
	struct
	{
		unsigned char 	Alt;		/* Alternative Interface Index */
		unsigned char 	Endp;		/* Endpoint */
		unsigned short	MaxPkt;		/* Maximum Packet Size */
	}alt[10];
	struct							/* Interrupt Endpoint Information */
	{
		unsigned char 	Endp;		/* Endpoint */
		unsigned char 	Poll;		/* Polling Interval */
		unsigned short	MaxPkt;		/* Maximum Packet Size */
	}Int;
}ST_UVC_IF_INFO;

typedef struct
{
	ST_UVC_STREAM_INFO		STREAM_INFO;
	ST_UVC_IF_INFO			IF_INFO;
}ST_UVC_INFO;

extern unsigned Drv_USBH_DevInit(void);
extern void usbh_uvc_frame_task_entry(void *parm);
#endif
