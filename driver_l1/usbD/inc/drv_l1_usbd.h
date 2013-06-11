#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define		EP0_Setup_PKT		BIT0
#define		EP0_OUT_PKT			BIT1
#define		EP0_OUT_NACK		BIT2
#define		EP0_IN_PKT_Clear	BIT3
#define		EP0_IN_NACK			BIT4
#define		EP0_Status_Clear	BIT5
#define		EP0_Status_NACK		BIT6
#define		BULK_IN_PKT_Clear	BIT7
#define		BULK_IN_NACK		BIT8
#define		BULK_OUT_PKT_Set	BIT9
#define		BULK_OUT_NACK		BIT10
#define		INT_IN_PKT_Clear	BIT11
#define		INT_IN_NACK			BIT12
#define		Suspend				BIT13
#define		Resume				BIT14
#define		USB_RESET			BIT15


#define		EP0_SETUP_Stage		0x0001
#define		EP0_Data_Stage		0x0002
#define		EP0_Status_Stage	0x0003
//MSDC
#define		SCSI_CBW_Stage		0x0004
#define		SCSI_Data_Stage		0x0005
#define		SCSI_CSW_Stage		0x0006
#define		Bulk_IN_Stall		0x0001
#define		Bulk_OUT_Stall		0x0002

#define		C_Device_Descriptor_Table		1
#define		C_Config_Descriptor_Table		2
#define		C_String0_Descriptor_Table		3
#define		C_String1_Descriptor_Table		4

//#define		C_String2_Descriptor_Table		5
//#define		C_String3_Descriptor_Table		6

#define		C_Interface_Descriptor_Table	8
#define		C_Endpoint_Descriptor_Table		9

//usb2.0
#define		C_Qualifier_Descriptor_Table	11
#define		C_OtherSpeed_Descriptor_Table	12	

#define		C_MaxLUN_Table					0x000A

#define     DEVDST_LENGTH  0


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


#define R_Timeout_count 0xf888;

//USB STD
extern unsigned int  R_USB_State_Machine ; 
extern unsigned int   USB_Status          ;
extern unsigned int   R_USB_Suspend       ;   
extern INT32U   s_usbd_user_string  ;
extern char     *user_string_pt;
extern INT32U   user_string_size;
//extern unsigned int   USBWriteProtect     ; 

extern INT8U	CDROMDelay;//huanghe 090604 for cdrom autorun 
extern INT64U	Start_CDROM_Tick;

extern void usbd_interrupt_register(void (*pt_function)(void));
extern void usbd_ep0descriptor_register(INT32U (*pt_function)(INT32U nTableType));
extern void usbd_suspend_register(void (*pt_function)(void));
extern void usbd_resume_register(void (*pt_function)(void));
extern void usbd_reset_register(void (*pt_function)(void));
extern void usb_initial(void);
extern void usb_uninitial(void);
extern void usb_reset(void);
extern void usb_isr(void);
extern void usb_std_service(INT32U unUseLoop);

extern void usbd_set_trigger(void);
extern unsigned int   USB_CheckRegister_Complete(volatile unsigned int *RegisterAddr,unsigned int CheckValue,unsigned int CheckResult);
extern unsigned int   USB_CheckRegister_Complete_Long(volatile unsigned int *RegisterAddr,unsigned int CheckValue,unsigned int CheckResult);





