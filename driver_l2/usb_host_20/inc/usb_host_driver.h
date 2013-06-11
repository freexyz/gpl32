#ifndef	__USB_HOST_DRIVER_H__
#define	__USB_HOST_DRIVER_H__
#include "driver_l2.h"


extern INT32S from_GetDesc_to_GetMaxLUN(void);

extern INT32U StandardCommandWithINData(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U StandardCommandWithoutData(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U GetMaxLUN(INT32U Number);
extern INT32U SCSICBWWithINData(INT32U* StoreAddr);
extern INT32U SCSICBWWithNOData(void);
extern INT32U SCSICBWWithDataOut(INT32U* StoreAddr);
extern INT32U BulkInData(INT32U DataTransferLength, INT32U* StoreAddr);
extern INT32U BulkOutData(INT32U DataTransferLength, INT32U * StoreAddr);

extern INT32S CBI_Read28(INT32U LUN, INT32U LBA, INT32U SectorCnt, INT32U * DataBuffer);
extern INT32S CBI_Status(void);
extern INT32U CheckLUNStatus(INT32U LUN);
extern INT32U BulkOnly_RequestSense(INT32U LUN);
extern void Update_USBLUN(INT32U ret, INT32U LUN);
extern INT32S StandardCommand_CBI(INT32U bmRequestType, INT32U bRequest, INT32U wValue, INT32U wIndex, INT32U wLength);
extern INT32U SCSI_Data_Stage_IN(unsigned long DataTransferLength, INT32U * StoreAddr);
extern INT32U SCSI_Data_Stage_OUT(unsigned long DataTransferLength, INT32U * StoreAddr);
extern INT32U SCSISendCMD(void);
extern INT32U CBI_Write2A(INT32U LUN, INT32U LBA, INT32U SectorCnt, INT32U* DataBuffer);
extern INT32S MSDC_Write2A(INT32U StartLBA, INT32U * StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32S MSDC_Read28(INT32U StartLBA, INT32U* StoreAddr, INT32U SectorCnt, INT32U LUN);
extern INT32S DrvUSBHMSDCInitial(void);
extern INT32S DrvUSBHLUNInit(INT32U LUN);    
extern INT32U DrvUSBHGetSectorSize(void);
extern INT32S DrvUSBHMSDCUninitial(void);
extern void usbh_usb20_chirp(void);
/*drv_l2_USBH_LUN_INIT.C*/
extern INT32U CBI_Byte1;
extern INT16U usbh_vid;
extern INT16U usbh_pid;
extern INT32U	g_ifchirp_pass;
extern INT32U  CapacitySize	   ;
//----------------------------------------------------------
// Constant Definitions
//----------------------------------------------------------

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

//----------------------------------------------------------
// Standard Request Codes
//----------------------------------------------------------
#define Get_Status			0
#define Clear_Feature		1
//#define Reserved			2
#define Set_Feature			3
//#define Reserved			4
#define Set_Address			5
#define Get_Descriptor		6
#define Set_Descriptor		7
#define Get_Configuration	8
#define Set_Configuration	9
#define Get_Interface		10
#define Set_Interface		11
#define Synch_Frame			12

//----------------------------------------------------------
// Descriptor Types
//----------------------------------------------------------
#define Device			0x0100
#define Configuration	0x0200
#define String_Index0	0x0300
#define String_Index1	0x0301
#define String_Index2	0x0302
#define String_Index3	0x0303
#define Interface		0x0400
#define Endpoint		0x0500

//----------------------------------------------------------
// Data Transfer
//----------------------------------------------------------
#define	IN_Data0	0
#define IN_Data1	1
#define OUT_Data0	2
#define OUT_Data1	3

//----------------------------------------------------------
// Constant
//----------------------------------------------------------
#define EP0				0
#define BulkIN			1
#define BulkOut			2
#define ResendNumber	5
#define SCSIDataIN		0x80
#define SCSIDataOUT		0x00
#define CSWRXByte		13
#define MSDCClass		0x08
#define HIDClass		0x03
#define	PROTOCOL_CBI		0x00	//hht : add 2006/6/7
#define PROTOCOL_CBInoINT	0x01
#define	PROTOCOL_BULKONLY	0x50
//----------------------------------------------------------
// SCSI Command
//----------------------------------------------------------
#define C_TestUnitReady					0x00
#define C_RequestSense					0x03
#define C_Inquiry						0x12
#define C_ModeSense						0x1A
#define C_PreventAllowMediumRemoval		0x1E
#define C_ReadFormatCapacities			0x23
#define C_ReadCapacity					0x25
#define C_Read10						0x28
#define C_Write10						0x2A
#define C_Verify						0x2F

//----------------------------------------------------------
// Request Sense Data
//----------------------------------------------------------
#define Medium_Not_Present			0xFFF8

//----------------------------------------------------------
// LUN
//----------------------------------------------------------
#define LUN0	0x0001
#define LUN1	0x0002
#define LUN2	0x0004
#define LUN3	0x0008


//----------------------------------------------------------
// Error 
//----------------------------------------------------------
#define USBH_Err_Success		0
#define USBH_Err_Fail			-1
#define USBH_Err_ResetByUser	100
#define USBHPlugStatus_None		0
#define USBHPlugStatus_PlugIn	1
#define USBHPlugStatus_PlugOut	2
#define USBH_Err_NoMedia		3


#endif