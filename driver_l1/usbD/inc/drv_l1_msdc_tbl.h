//#include "boot.h"
#include "driver_l1.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if ((defined _DRV_L1_USBD) ) &&(_DRV_L1_USBD ==CUSTOM_ON)  //                //
//================================================================//


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
#define InquiryLength 44 
static char SCSIInquiryData[] = {

	0x00,
	0x80,
	0x00,
	0x00,
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

static char SCSIInquiryData_CDROM[] = {

	0x05,
	0x02,//0x04,
	0x01,//0x00,
	0x00,
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

static char ScsiModeSense10CDROM[] = {

	0x00, 0x1A, 			//Mode Data Length	:the length in bytes of the following data (not including itself).
	0x01, 					//Medium Type Code	:120 mm CD data only
	0x00, 					//WP				:00H,the medium is write enabled.;80H,the medium is write disabled.
	0x00, 0x00, 0x00, 0x00, //Reserved			:
	0x2A, 					//page code			:0x2A is CD Capabilities and Mechanical Status Page
	0x12,					//Paeg length		:
	0x07, 					//read support		:Method2(1bit),CD-RW Read(1bit),CD-R Read(1bit)
	0x07,					//write support		:Test Write(1bit),CD-RW Write(1bit),CD-R Write(1bit)
	0x70, 					//					:support Multi Session,Mode 2 Form 2,Mode 2 Form 1
	0x00, 					//					:
	0x28,					//bit7-5 is Loading Mechanism Type: Tray type loading mechanism
								//bit4 is Eject Command: 
	0x00, 					//					:
		//0x56, 0xA0, 			//Reserved	
	0x00, 0x00,
	0x00, 0x00,				//Number of Volume Levels Supported:
	0x00, 0x00, 			//Buffer Size Supported:If the Logical Unit does not have a buffer cache, the value returned shall be zero.	
		//0x56, 0xA0, 			//Reserved
	0x00, 0x00,
	0x00,					//Reserved
	0x00,					//Reserved
	0x00, 0x00,				//Reserved
};

static char SCSISENSE[]={
        //  SK   ASC  ASCQ
	  0x00, 0x00, 0x00,    //0x00 - NO SENSE
      0x01, 0x17, 0x01,    //0x01 - RECOVERD DATA WITH RETRIES
      0x01, 0x18, 0x00,    //0x02 - RECOVERD DATA WITH ECC
      0x02, 0x04, 0x01,    //0x03 - LOGICAL DRIVE NOT READY - BECOMING READY
      0x02, 0x04, 0x02,    //0x04 - LOGICAL DRIVE NOT READY - INITIALIZATION REQUIRED
      0x02, 0x04, 0x04,    //0x05 - LOGICAL UNIT NOT READY - FORMAT IN PROGRESS
      0x02, 0x04, 0xFF,    //0x06 - LOGICAL DRIVE NOT READY - DEVICE IS BUSY
      0x02, 0x06, 0x00,    //0x07 - NO REFERENCE POSITION FOUND
      0x02, 0x08, 0x00,    //0x08 - LOGICAL UNIT COMMUNICATION FAILURE
      0x02, 0x08, 0x01,    //0x09 - LOGICAL UNIT COMMUNICATION TIME-OUT
      0x02, 0x08, 0x80,    //0x0A - LOGICAL UNIT COMMUNICATION OVERRUN
      0x02, 0x3A, 0x00,    //0x0B - MEDIUM NOT PRESENT
      0x02, 0x54, 0x00,    //0x0C - USB TO HOST SYSTEM INTERFACE FAILURE
      0x02, 0x80, 0x00,    //0x0D - INSUFFICIENT RESOURCES
      0x02, 0xFF, 0xFF,    //0x0E - UNKNOWN ERROR
      0x03, 0x02, 0x00,    //0x0F - NO SEEK COMPLETE
      0x03, 0x03, 0x00,    //0x10 - WRITE FAULT
      0x03, 0x10, 0x00,    //0x11 - ID CRC ERROR
      0x03, 0x11, 0x00,    //0x12 - UNRECOVERD READ ERROR
      0x03, 0x12, 0x00,    //0x13 - ADDRESS MARK NOT FOUND FOR ID FIELD
      0x03, 0x13, 0x00,    //0x14 - ADDRESS MARK NOT FOUND FOR DATA FIELD
      0x03, 0x14, 0x00,    //0x15 - RECORDED ENTITY NOT FOUND
      0x03, 0x30, 0x01,    //0x16 - CANNOT READ MEDIUM - UNKNOWN FORMAT
      0x03, 0x31, 0x01,    //0x17 - FORMAT COMMAND FAILED
      0x04, 0x40, 0x80,    //0x18 - DIAGNOSTIC FAILURE ON COMPONENT NN (80H-FFH)
      0x05, 0x1A, 0x00,    //0x19 - PARAMETER LIST LENGTH ERROR
      0x05, 0x20, 0x00,    //0x1A - INVALID COMMAND OPERATION CODE
      0x05, 0x21, 0x00,    //0x1B - LOGICAL BLOCK ADDRESS OUT OF RANGE
      0x05, 0x24, 0x00,    //0x1C - INVALID FIELD IN COMMAND PACKET   
      0x05, 0x25, 0x00,    //0x1D - LOGICAL UNIT NOT SUPPORTED
      0x05, 0x26, 0x00,    //0x1E - INVALID FIELD IN PARAMETER LIST
      0x05, 0x26, 0x01,    //0x1F - PARAMETER NOT SUPPORTED 
      0x05, 0x26, 0x02,    //0x20 - PARAMETER VALUE INVALID
      0x05, 0x39, 0x00,    //0x21 - SAVING PARAMETERS NOT SUPPORT  
      0x06, 0x28, 0x00,    //0x22 - NOT READY TO READY TRANSITION - MEDIA CHANGED
      0x06, 0x29, 0x00,    //0x23 - POWER ON RESET OR BUS DEVICE RESET OCCURRED
      0x06, 0x2F, 0x00,    //0x24 - COMMANDS CLEARED BY ANOTHER INITIATOR
      0x07, 0x27, 0x00,    //0x25 - WRITE PROTECTED MEDIA
      0x0B, 0x4E, 0x00,    //0x26 - OVERLAPPED COMMAND ATTEMPED		
	  0xFF, 0xFF, 0xFF,	//0x27 - USB vendor command not have "GP" signature
  };


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_MSC) && (_DRV_L1_MSC == 1)              //
//================================================================//