#include "application.h"

#define MainTaskStackSize 1024

//define the demo code
#define AUDIO_DECODE		      0
#define AUDIO_ENCODE		      0
#define IMAGE_CODEC				  0
#define VIDEO_DECODE		      0
#define	VIDEO_ENCODE		      0
#define MULTIMEDIA_DEMO		      1
#define	SACM_DECODE			      0
#define	SACM_ENCODE			      0
#define	USB_DEMO			      0
#define	USB_WEB_CAM_DEMO	      0
#define USB_MIDI_DEMO			  0
#define IMAGE_ENCODE_BLOCK_RW	  0
#define IMAGE_ENCODE_BLOCK_READ	  0
#define PPU_DEMO				  0
#define DYNAMICALLY_CLOCK_DEMO    0
#define ID3_TAG_DEMO              0
#define GPID_DEMO				  0

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
extern void Usb_MIDI_Demo(void);
extern void encode_block_rw_demo(void);
extern void encode_block_demo(void);
extern void pokemon_demo(void);
extern void gpl32_dynamic_mclk_demo(void);
extern void id3_tag_demo(void);
extern void gpid_demo(void);

INT32U free_memory_start, free_memory_end;
INT32U MainTaskStack[MainTaskStackSize];

void Main_task_entry(void *para)
{
	gpio_init();
	
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
#if (USB_MIDI_DEMO == 1) && (USB_DEMO == 0)
	Usb_MIDI_Demo();
#endif
#if IMAGE_ENCODE_BLOCK_RW
	encode_block_rw_demo();
#endif
#if IMAGE_ENCODE_BLOCK_READ
	encode_block_demo();
#endif
#if PPU_DEMO
	pokemon_demo();
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
	
	// Initialize platform
	platform_entrance(free_memory);
	
	// Create a user-defined task
	OSTaskCreate(Main_task_entry, (void *) 0, &MainTaskStack[MainTaskStackSize - 1], 32); 
	
	// Start running
	OSStart();
}
