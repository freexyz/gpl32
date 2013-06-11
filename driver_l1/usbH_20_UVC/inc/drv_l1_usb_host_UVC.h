#ifndef __drv_l1_USB_HOST_UVC_H__
#define __drv_l1_USB_HOST_UVC_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define		BIT0		0x1
#define		BIT1		0x2
#define		BIT2		0x4
#define		BIT3		0x8
#define		BIT4		0x10
#define		BIT5		0x20
#define		BIT6		0x40
#define		BIT7		0x80
#define		BIT8		0x100
#define		BIT9		0x200
#define		BIT10		0x400
#define		BIT11		0x800
#define		BIT12		0x1000
#define		BIT13		0x2000
#define		BIT14		0x4000
#define		BIT15		0x8000
#define		BIT16		0x10000
#define		BIT17		0x20000
#define		BIT18		0x40000
#define		BIT19		0x80000
#define		BIT20		0x100000
#define		BIT21		0x200000
#define		BIT22		0x400000
#define		BIT23		0x800000
#define		BIT24		0x1000000
#define		BIT25		0x2000000
#define		BIT26		0x4000000
#define		BIT27		0x8000000
#define		BIT28		0x10000000
#define		BIT29		0x20000000
#define		BIT30		0x40000000
#define		BIT31		0x80000000



#define	USBH_INT_DPDM_STATUS		BIT0
#define	USBH_INT_SOF				BIT1
#define	USBH_INT_TX_COMPLETE		BIT2
#define	USBH_INT_RX_COMPLETE		BIT3
#define	USBH_INT_AUTO_IN_TX			BIT4
#define	USBH_INT_AUTO_OOUT_TX		BIT5
#define	USBH_INT_VBUS_STATUS		BIT6
#define	USBH_INT_HOST_TX_DATA		BIT7
#define	USBH_INT_IN_TOKEN			BIT8
#define	USBH_INT_TSOFI				BIT9
#define	USBH_INT_TRST				BIT10
#define	USBH_INT_DPO				BIT11
#define	USBH_INT_DMAF				BIT15

#define	USBH_B_INT_DPDM_STATUS		0
#define	USBH_B_INT_SOF				1
#define	USBH_B_INT_TX_COMPLETE		2
#define	USBH_B_INT_RX_COMPLETE		3
#define	USBH_B_INT_AUTO_IN_TX		4
#define	USBH_B_INT_AUTO_OOUT_TX		5
#define	USBH_B_INT_VBUS_STATUS		6
#define	USBH_B_INT_HOST_TX_DATA		7
#define	USBH_B_INT_IN_TOKEN			8
#define	USBH_B_INT_TSOFI			9
#define	USBH_B_INT_TRST				10
#define	USBH_B_INT_DPO				11
#define	USBH_B_INT_DMAF				15

#define	USBH_STATUS_ACK				BIT0
#define	USBH_STATUS_NACK			BIT1
#define	USBH_STATUS_Stall			BIT2
#define	USBH_STATUS_UnPID			BIT3
#define	USBH_STATUS_BitStuffErr		BIT4
#define	USBH_STATUS_DataSeqErr		BIT5
#define	USBH_STATUS_CRC16Err		BIT6
#define	USBH_STATUS_TimeOut			BIT7
#define	USBH_STATUS_NYET			BIT8

// Host Transfer 
#define	USBH_TX_SOF			BIT0
#define	USBH_TX_Setup		BIT1
#define	USBH_TX_InData0		BIT2
#define	USBH_TX_InData1		BIT3
#define	USBH_TX_OutData0	BIT4
#define	USBH_TX_OutData1	BIT5
#define	USBH_TX_Reset		BIT6
// Error Code 
#define USBH_E_NoErr		0x00
#define USBH_E_RX			0x01
#define USBH_E_TX			0x02
#define USBH_E_NACK			0x03
#define USBH_E_Stall		0x04
#define USBH_E_UnPID		0x05
#define USBH_E_BitStuff		0x06
#define USBH_E_DataSeq		0x07
#define USBH_E_CRC16		0x08
#define USBH_E_TimeOut		0x09
#define USBH_E_Para			0x0A


typedef struct
{
	char			cAddr;			// USB address
	char			cEndp;			// USB endpoint
	char			cDMAeEn;		// DMA enable
	char*			cTog;			// USB data toggle
	void *		 	pBuf;			// data buffer address
	unsigned int  	uiLn;			// buffer length
	unsigned int	uiMaxPkt;		// maximum packet size
	unsigned *		pAct;			// active length 
	char*	pErrorCode;
	unsigned int* 	offset;			// Buffer offset (for DMA use)
}ST_USBH_CTRL;

typedef struct
{ 
	unsigned char	ucbmRequestType;
	unsigned char	ucbRequest;
	unsigned short	uswValue;
	unsigned short	uswIndex;
	unsigned short	uswLength;
}ST_USBH_SetupCmd;

typedef struct
{
	char			type;
	char			cAddr;
	char			cEP0_s;
	char 			Speed;		/* 0 for high speed, 1 for full speed */
	unsigned short 	VID;
	unsigned short	PID;
	void* 	para;
}USBH_DEV_INFO;

extern USBH_DEV_INFO	g_stDev[2];

//----- Function Declaration -----
extern unsigned Drv_USBH_GetFrameNum(void);
extern void Drv_USBH_Tick_Wait(unsigned int nSOF);
extern void Drv_USBH_Init(void);
extern void Drv_USBH_unInit(void);
extern unsigned int Drv_USBH_Detect(void);
extern void Drv_USBH_ResetDevice(unsigned int uiTick);
extern unsigned int Drv_USBH_CheckDevice(void);
extern unsigned int Drv_USBH_Speed(void);

extern unsigned Drv_USBH_SendSetupCmd(ST_USBH_CTRL Ctrl);
extern unsigned Drv_USBH_OUT_TOKEN(ST_USBH_CTRL Ctrl); 
extern unsigned Drv_USBH_IN_TOKEN(ST_USBH_CTRL Ctrl);
extern unsigned Drv_USBH_INTIN_TOKEN(ST_USBH_CTRL Ctrl);
extern unsigned Drv_USBH_ISOIN_TOKEN(ST_USBH_CTRL Ctrl);

extern unsigned Drv_USBH_CtrlTrans(ST_USBH_SetupCmd cmd, void* buf, char dev, char*pErrorCode);

#endif /*__drv_l1_USB_HOST_UVC_H__*/

