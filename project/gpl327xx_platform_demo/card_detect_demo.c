#include "application.h"

typedef enum
{
	CF_PLUG_IN = 1,
	CF_PLUG_OUT,
	SDMSXD_PLUG_IN,
	SDMSXD_PLUG_OUT,
	USB_H_PLUG_IN,
	USB_H_PLUG_OUT,
	USB_D_PLUG_IN,
	USB_D_PLUG_OUT
}card_status;

#define TYPE_CF		0x01
#define TYPE_SD		0x02
#define TYPE_MS		0x04
#define TYPE_USBH	0x08
#define TYPE_USBD	0x10
#define TYPE_NAND	0x20
#define TYPE_XD		0x40

#define C_CFC	0
#define C_SDC	1
#define C_MSC	2
#define C_USBH	3
#define C_USBD	4
#define C_NAND	5
#define C_XDC	6

OS_EVENT *card_detect_q;
void	*card_detect_area[1];

/************************************************************
card detect demo code
*************************************************************/
void cf_card_plug_in_out(void)
{
	if(card_detect_get_plug_status())
		OSQPost(card_detect_q, (void *)CF_PLUG_IN);
	else		
		OSQPost(card_detect_q, (void *)CF_PLUG_OUT);
}

void sdms_card_plug_in_out(void)
{
	if(card_detect_get_plug_status())
		OSQPost(card_detect_q, (void *)SDMSXD_PLUG_IN);
	else
		OSQPost(card_detect_q, (void *)SDMSXD_PLUG_OUT);
}

void usb_h_plug_in_out(void)
{
	if(card_detect_get_plug_status())
		OSQPost(card_detect_q, (void *)USB_H_PLUG_IN);
	else
		OSQPost(card_detect_q, (void *)USB_H_PLUG_OUT);
}

void usb_d_plug_in_out(void)
{
	if(card_detect_get_plug_status())
		OSQPost(card_detect_q, (void *)USB_D_PLUG_IN);
	else
		OSQPost(card_detect_q, (void *)USB_D_PLUG_OUT);
}

void card_detect_demo(void)
{
	INT8U  err;
	INT32U msg_id;
	INT32S nRet;
	
	card_detect_q = OSQCreate(card_detect_area, 1);
	if (card_detect_q == NULL)
		while(1);
	
	card_detect_set_cf_callback(cf_card_plug_in_out);
	card_detect_set_sdms_callback(sdms_card_plug_in_out);
	card_detect_set_usb_h_callback(usb_h_plug_in_out);
	card_detect_set_usb_d_callback(usb_d_plug_in_out);
	card_detect_init(TYPE_CF|TYPE_SD|TYPE_MS|TYPE_USBH|TYPE_USBD|TYPE_NAND);
	
	if(card_detection(C_CFC))
	{  
		nRet = _devicemount(FS_CF);
		if(!nRet)
			DBG_PRINT("CF MOUNT Success\r\n");
		else 
			DBG_PRINT("CF MOUNT Fail!!!\r\n");
	}
	if(card_detection(C_SDC))
	{  
		nRet = _devicemount(FS_SD);
		if(!nRet)
			DBG_PRINT("SD MOUNT Success\r\n");
		else 
			DBG_PRINT("SD MOUNT Fail!!!\r\n");
	}
	if(card_detection(C_MSC))
	{  
		nRet = _devicemount(FS_MS);
		if(!nRet)
			DBG_PRINT("MS MOUNT Success\r\n");
		else 
			DBG_PRINT("MS MOUNT Fail!!!\r\n");
	}
	if(card_detection(C_USBH))
	{  
		nRet = _devicemount(FS_USBH);
		if(!nRet)
			DBG_PRINT("USB Host MOUNT Success\r\n");
		else 
			DBG_PRINT("USB Host MOUNT Fail!!!\r\n");
	}
	if(card_detection(C_USBD))
	{
		DBG_PRINT("USBD plug in\r\n");
	}  
	if(card_detection(C_NAND))
	{
		nRet = _devicemount(FS_NAND1);
		if(!nRet)
			DBG_PRINT("NAND1 MOUNT Success\r\n");
		else 
			DBG_PRINT("NAND1 MOUNT Fail!!!\r\n");
	}  
	if(card_detection(C_XDC))
	{
		nRet = _devicemount(FS_XD);
		if(!nRet)
			DBG_PRINT("XD MOUNT Success\r\n");
		else 
			DBG_PRINT("XD MOUNT Fail!!!\r\n");
	}  
	
	while(1)
	{
		msg_id = (INT32U) OSQPend(card_detect_q, 0, &err);
		switch(msg_id)
		{
		case CF_PLUG_IN:
	 		cfc_detection();
			if(card_detection(C_CFC))
			{
				nRet = _devicemount(FS_CF);
				if(!nRet)
					DBG_PRINT("CF PLUG-IN & MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("CF PLUG-IN & MOUNT-FAIL!!!\r\n");
		 	}
			break;
		case CF_PLUG_OUT:
			disk_safe_exit(FS_CF);
            nRet = _deviceunmount(FS_CF);
			if(!nRet)
				DBG_PRINT("CF PLUG-OUT & UN-MOUNT-SUCCESS\r\n");
			else
				DBG_PRINT("CF PLUG-OUT & UN-MOUNT-FAIL!!!\r\n");
	 		break;
	 
		case SDMSXD_PLUG_IN:
	 		sdms_detection();
			if(card_detection(C_SDC))
			{
				nRet = _devicemount(FS_SD);
				if(!nRet)
					DBG_PRINT("SD PLUG-IN & MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("SD PLUG-IN & MOUNT-FAIL!!!\r\n");
			}
			else if(card_detection(C_MSC))
			{
				nRet = _devicemount(FS_MS);
				if(!nRet)
					DBG_PRINT("MS PLUG-IN & MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("MS PLUG-IN & MOUNT-FAIL!!!\r\n");		
			}
			else if(card_detection(C_XDC))
			{
				nRet = _devicemount(FS_XD);
				if(!nRet)
					DBG_PRINT("XD PLUG-IN & MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("XD PLUG-IN & MOUNT-FAIL!!!\r\n");			
			}		
	 		break;
	 	case SDMSXD_PLUG_OUT:
	 		if(card_detection(C_SDC))
	 		{
				disk_safe_exit(FS_SD);
	            nRet = _deviceunmount(FS_SD);
				if(!nRet)
					DBG_PRINT("SD PLUG-OUT & UN-MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("SD PLUG-OUT & UN-MOUNT-FAIL!!!\r\n");
            }
            else if(card_detection(C_MSC))
			{
				disk_safe_exit(FS_MS);
	            nRet = _deviceunmount(FS_MS);
				if(!nRet)
					DBG_PRINT("MS PLUG-OUT & UN-MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("MS PLUG-OUT & UN-MOUNT-FAIL!!!\r\n");
			}
			else if(card_detection(C_XDC))
			{
				disk_safe_exit(FS_XD);
	            nRet = _deviceunmount(FS_XD);
				if(!nRet)
					DBG_PRINT("XD PLUG-OUT & UN-MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("XD PLUG-OUT & UN-MOUNT-FAIL!!!\r\n");
			}
	 		break;
	 		
	 	case USB_H_PLUG_IN:
	 		if(card_detection(C_USBH))
	 		{
	 			nRet = _devicemount(FS_USBH);
				if(!nRet)
					DBG_PRINT("USB Host PLUG-IN & MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("USB Host PLUG-IN & MOUNT-FAIL!!!\r\n");
	 		}
	 		break;
	 		
	 	case USB_H_PLUG_OUT:
	 		if(!card_detection(C_USBH))
	 		{
				disk_safe_exit(FS_USBH);
	            nRet = _deviceunmount(FS_USBH);
				if(!nRet)
					DBG_PRINT("USB Host PLUG-OUT & UN-MOUNT-SUCCESS\r\n");
				else
					DBG_PRINT("USB Host PLUG-OUT & UN-MOUNT-FAIL!!!\r\n");
	 		}
	 		break;
	 	
	 	case USB_D_PLUG_IN:
	 		if(card_detection(C_USBD))
	 		{
	 			DBG_PRINT("USB Device PLUG-IN\r\n");
	 		}
	 		break;
	 		
	 	case USB_D_PLUG_OUT:
	 		if(!card_detection(C_USBD))
	 		{
	 			DBG_PRINT("USB Device PLUG-OUT\r\n");
	 		}
	 		break;
	 	}	
	}
}

