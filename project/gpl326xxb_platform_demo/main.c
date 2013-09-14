/*
 ******************************************************************************
 * main.c
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.06.03	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include "application.h"

#define UART_MSG			1

#define MainTaskStackSize 1024

//define the demo code
#define AUDIO_DECODE			0
#define AUDIO_ENCODE			0
#define IMAGE_CODEC			0
#define VIDEO_DECODE			0
#define	VIDEO_ENCODE			0
#define MULTIMEDIA_DEMO			0
#define	SACM_DECODE			0
#define	SACM_ENCODE			0
#define	USB_DEMO			0
#define	USB_WEB_CAM_DEMO		0
#define IMAGE_ENCODE_BLOCK_RW		0
#define IMAGE_ENCODE_BLOCK_READ		0
#define GP326XXX_PPU_DEMO		0
#define DYNAMICALLY_CLOCK_DEMO		0
#define ID3_TAG_DEMO			0
#define GPID_DEMO			0
#define USBH_ISO_DEMO			0
#define FACE_DETECT_DEMO		1
#define	COMAIR_RX_DEMO			0

#define PRINT_OUTPUT_NONE		0x00 
#define PRINT_OUTPUT_UART		0x01
#define PRINT_OUTPUT_USB		0x02

extern void Audio_Decode_Demo(void);
extern void Audio_Encode_Demo(void);
extern void Image_Codec_Demo(void);
extern void Video_Decode_Demo(void);
extern void Video_Encode_Demo(void);
extern void Platform_Demo_Code(void);
extern void Sacm_Decode_Demo(void);
extern void Sacm_Encode_Demo(void);
extern void Usb_Device_Demo(void);
extern void Usb_WebCam_Demo(void);
extern void encode_block_rw_demo(void);
extern void encode_block_demo(void);
extern void GPL32XXXX_PPU_Hblank_Demo(void);
extern void GPL326XXX_PPU_Demo(void);
extern void gpl32_dynamic_mclk_demo(void);
extern void id3_tag_demo(void);
extern void gpid_demo(void);
extern void USBH_ISO_Demo(void);
extern void set_print_output_type(INT32U type);
extern void fd_demo(void);
extern void Comair_RX_Demo(void);
extern void PowerDown_Mode(INT8U mode);

INT32U free_memory_start, free_memory_end;
INT32U MainTaskStack[MainTaskStackSize];

void Main_task_entry(void *para)
{
#if AUDIO_DECODE	
	Audio_Decode_Demo();
#endif
#if AUDIO_ENCODE	
	Audio_Encode_Demo();
#endif
#if IMAGE_CODEC	
	Image_Codec_Demo();	
#endif
#if VIDEO_DECODE	
	Video_Decode_Demo();	
#endif
#if VIDEO_ENCODE 	
	Video_Encode_Demo();
#endif
#if MULTIMEDIA_DEMO
	Platform_Demo_Code();
#endif
#if SACM_DECODE
	Sacm_Decode_Demo();
#endif
#if SACM_ENCODE
	Sacm_Encode_Demo();
#endif
#if USB_DEMO
	Usb_Device_Demo();
#endif
#if USB_WEB_CAM_DEMO
	Usb_WebCam_Demo();
#endif
#if USBH_ISO_DEMO
	USBH_ISO_Demo();
#endif
#if IMAGE_ENCODE_BLOCK_RW
	encode_block_rw_demo();
#endif
#if IMAGE_ENCODE_BLOCK_READ
	encode_block_demo();
#endif
#if GP326XXX_PPU_DEMO
    GPL326XXX_PPU_Demo();
#endif
#if DYNAMICALLY_CLOCK_DEMO
	gpl32_dynamic_mclk_demo();
#endif
#if ID3_TAG_DEMO
	id3_tag_demo();
#endif
#if GPID_DEMO
	gpid_demo();
#endif
#if FACE_DETECT_DEMO
	DBG_PRINT("free memory start = 0x%08x\r\n", free_memory_start);
	DBG_PRINT("free memory end   = 0x%08x\r\n", free_memory_end);

//	PowerDown_Mode(0);
	fd_demo();
#endif
#if	COMAIR_RX_DEMO
	Comair_RX_Demo();
#endif
	while(1);
}

void Debug_UART_Port_Enable(void)
{
	uart0_buad_rate_set(UART0_BAUD_RATE);
	uart0_tx_enable();
	uart0_rx_enable();	
}

void Main(void *free_memory)
{
	// Initialize Operating System
	os_init();			
			
	// Initialize drvier layer 1 modules
	drv_l1_init();
	
	// Initialize driver Layer 2 modules
	drv_l2_init();
	
	// Initiate Component Layer modules
	free_memory_start = ((INT32U) free_memory + 3) & (~0x3);	// Align to 4-bytes boundry
	free_memory_end = (INT32U) SDRAM_END_ADDR & ~(0x3);
	gplib_init(free_memory_start, free_memory_end);
		
	// Enable UART port for debug
	Debug_UART_Port_Enable();
	
	//Configure the output type of debug message, NONE, UART, USB or both
#if (UART_MSG == 1)
	set_print_output_type(PRINT_OUTPUT_UART | PRINT_OUTPUT_USB);
#else
	set_print_output_type(PRINT_OUTPUT_NONE);
#endif

	// Initialize platform
	platform_entrance(free_memory);
	
	// Create a user-defined task
	OSTaskCreate(Main_task_entry, (void *) 0, &MainTaskStack[MainTaskStackSize - 1], 42); 
	
	// Start running
	OSStart();
}
