/**********************************************************
* Purpose: USB MIDI class ap code
*
* Author: Eugenehsu
*
* Date: 2012/07/27
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
* History :
*              
**********************************************************/
#include "application.h"
#include "drv_l1_usbd_midi.h"

//#define USBD_IN_MIDI_DEMO

INT32S process_usb_midi_in_handler(void)
{
#ifdef 	USBD_IN_MIDI_DEMO
	INT8U buf[4] = {0};
	INT8U senddata = 0;
	
	static INT8U NoteOn = 1;
	static INT8U NoteOff = 0;
	static INT32U testcnt = 0;
	static INT8U Pitch = 0;
	

	testcnt ++;
			
	if(testcnt < 30)
	{
		DBG_PRINT("BULK IN, Pitch 0x%x, NoteOn\r\n", Pitch);
	  	if(NoteOn == 1)
	  	{
	  		NoteOn = 0;
	  		NoteOff = 1;
	  		
			buf[0] =0x09;
			buf[1] =0x90;
		  	buf[2] =Pitch;
		  	buf[3] =0x7F;
		  	senddata = 1;
		 } 		
	}
	else if(testcnt <= 60)
	{
		//DBG_PRINT("BULK IN, Pitch 0x%x, NoteOff\r\n", Pitch);	
	  	if(NoteOff == 1)
	  	{
	  		NoteOff = 0;
	  		NoteOn = 1;
	  		buf[0] =0x08;
			buf[1] =0x80;
	  		buf[2] =Pitch;
	  		buf[3] =0x00;
	  		senddata = 1;
	  	}
	  	
	  	if(testcnt == 60)
	  	{
			testcnt = 0;
		
			if(Pitch == 0x70)
				Pitch = 0;
			else	
				Pitch++;
			
			buf[0] =0x08;
			buf[1] =0x80;
	  		buf[2] =Pitch;
	  		buf[3] =0x00;
			senddata = 1;				
		}	 					  
	}	
	
	/* Send buf to host */
	if(senddata)
	{
		MIDI_USB_Send_In((void*)buf, 4);
		return 0;
	}
	else
	{
		//DBG_PRINT("Let Bulk_IN_Stage break\r\n");	
		return -1;
	}		
#else
	return -1;	
#endif	
}
	
INT32S process_usb_midi_out_handler(void)
{

	INT8U temp[4];
	
	temp[0] = MIDI_Get_Out_Pkt(0);
	temp[1] = MIDI_Get_Out_Pkt(1);
	temp[2] = MIDI_Get_Out_Pkt(2);
	temp[3] = MIDI_Get_Out_Pkt(3);
	
	DBG_PRINT("MIDI get 0x%2x 0x%2x 0x%2x 0x%2x in out pkt\r\n", temp[0], temp[1], temp[2], temp[3]);
	
	return 0;
}	 

void Usb_MIDI_Demo(void)
{
	DBG_PRINT("USB MIDI device demo init...\r\n");
	
	/* Due to enable ISR for USB host and device in MSDC init flow, disable USB ISR and disable USB host for MIDI init */
	vic_irq_unregister(VIC_USB);
	vic_irq_disable(VIC_USB);
	USB_Host_Disable();
	
	MIDI_USB_Initial();
	/* Register Bulk IN/OUT function for processing the message from keyboard or PC tools */
	MIDI_USB_Register_in_out_fn(process_usb_midi_in_handler, process_usb_midi_out_handler);
	
	while(1)
	{
		MIDI_USB_ISR();
		MIDI_USB_ServiceLoop(0);
	}	
	
}
