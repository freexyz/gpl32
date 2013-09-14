#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"

void DSC_demo(void);
void IL_GPM758A0_Init(void);
void SPI_WriteCmdType2 (INT32S nCmdType, INT32U uCmd);
extern void cmd_delay(INT32U i); 
INT32U uTft_dat,uTft_clk,uTft_cs,q_value;

extern INT16U pic_num;
extern INT32U BLOCK_ENCODE,encode_buf,save_file_ck,decode_mode_ck;
extern FP32 c_xxx_zoom;

#define USE_DISK	FS_SD

#define C_PPU_DRV_FRAME_NUM				2
PPU_REGISTER_SETS ppu_register_structure;
PPU_REGISTER_SETS *ppu_register_set;

#define DISPLAY_DEV_HPIXEL      320
#define DISPLAY_DEV_VPIXEL      240
#define SCALER_DEV_HPIXEL       640
#define SCALER_DEV_VPIXEL       480

#define TV_DISPLAY              0
#define TFT_DISPLAY             1
#define	DISPLAYUSED		    TV_DISPLAY

static INT8U jpeg_422_q50_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x63, 0x6F, 0x64, 0x65, 0x72, 0xFF, 
	// Quality 50
	0xDB, 0x00, 0x43, 0x00, 0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A,	0x10, 0x0E, 0x0D, 0x0E, 0x12, 0x11, 
	0x10, 0x13, 0x18, 0x28, 0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25, 0x1D, 0x28, 0x3A, 0x33, 
	0x3D, 0x3C, 0x39, 0x33, 0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44, 0x57, 0x45, 0x37, 0x38, 
	0x50, 0x6D, 0x51, 0x57, 0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71, 0x79, 0x70, 0x64, 0x78, 
	0x5C, 0x65, 0x67, 0x63, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x11,	0x12, 0x12, 0x18, 0x15, 0x18, 0x2F, 
	0x1A, 0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
    // Quality 50
	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

static INT8U jpeg_422_q70_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x64, 0x65, 0x72, 0x00, 0x00, 0xFF, 
	// Quality 70
	0xDB, 0x00, 0x43, 0x00, 0x0A, 0x07, 0x07, 0x08, 0x07, 0x06, 0x0A, 0x08, 0x08, 0x08, 0x0B, 0x0A, 
	0x0A, 0x0B, 0x0E, 0x18, 0x10, 0x0E, 0x0D, 0x0D, 0x0E, 0x1D, 0x15, 0x10, 0x11, 0x18, 0x23, 0x1F, 
	0x25, 0x24, 0x22, 0x21, 0x22, 0x21, 0x26, 0x2B, 0x37, 0x2F, 0x26, 0x29, 0x34, 0x29, 0x21, 0x22, 
	0x30, 0x41, 0x49, 0x31, 0x39, 0x3B, 0x3E, 0x3E, 0x3E, 0x25, 0x2E, 0x44, 0x49, 0x43, 0x3C, 0x48, 
	0x37, 0x3E, 0x3E, 0x3B, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x0A, 0x0B, 0x0B, 0x0E, 0x0D, 0x0E, 0x1C, 
	0x10, 0x10, 0x1C, 0x3B, 0x28, 0x22, 0x28, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
    // Quality 70
	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

static INT8U jpeg_422_q80_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x64, 0x65, 0x72, 0x00, 0x00, 0xFF, 
	// Quality 80
	0xDB, 0x00, 0x43, 0x00, 0x06, 0x04, 0x05, 0x06, 0x05, 0x04, 0x06, 0x06, 0x05, 0x06, 0x07, 0x07, 
	0x06, 0x08, 0x0A, 0x10, 0x0A, 0x0A, 0x09, 0x09, 0x0A, 0x14, 0x0E, 0x0F, 0x0C, 0x10, 0x17, 0x14, 
	0x18, 0x18, 0x17, 0x14, 0x16, 0x16, 0x1A, 0x1D, 0x25, 0x1F, 0x1A, 0x1B, 0x23, 0x1C, 0x16, 0x16, 
	0x20, 0x2C, 0x20, 0x23, 0x26, 0x27, 0x29, 0x2A, 0x29, 0x19, 0x1F, 0x2D, 0x30, 0x2D, 0X28, 0x30, 
	0x25, 0x28, 0x29, 0x28, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x07, 0x07, 0x07, 0x0A, 0x08, 0x0A, 0x13, 
	0x0A, 0x0A, 0x13, 0x28, 0x1A, 0x16, 0x1A, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 
	0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 
	0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 
	0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
    // Quality 80
	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

static INT8U jpeg_422_q90_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x64, 0x65, 0x72, 0x00, 0x00, 0xFF, 
	// Quality 90
	0xDB, 0x00, 0x43, 0x00, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x03, 
	0x03, 0x04, 0x05, 0x08, 0x05, 0x05, 0x04, 0x04, 0x05, 0x0A, 0x07, 0x07, 0x06, 0x08, 0x0C, 0x0A, 
	0x0C, 0x0C, 0x0B, 0x0A, 0x0B, 0x0B, 0x0D, 0x0E, 0x12, 0x10, 0x0D, 0x0E, 0x11, 0x0E, 0x0B, 0x0B, 
	0x10, 0x16, 0x10, 0x11, 0x13, 0x14, 0x15, 0x15, 0x15, 0x0B, 0x0F, 0x17, 0x18, 0x16, 0x14, 0x18, 
	0x12, 0x14, 0x15, 0x14, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x09, 
	0x05, 0x05, 0x09, 0x14, 0x0D, 0x0B, 0x0D, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
    // Quality 90
	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

static INT8U jpeg_422_q100_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x63, 0x6F, 0x64, 0x65, 0x72, 0xFF, 
	// Quality 100
	0xDB, 0x00, 0x43, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
    // Quality 100
	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

void DSC_demo(void)
{ 
    INT8U path[24];
	INT16S fd;
	INT32U encode_output_ptr,fifo_a,fifo_b,mcu_size,counter;
	INT32U encode_state,encode_size;
	IMAGE_ENCODE_ARGUMENT encode_info;
	IMAGE_SCALE_ARGUMENT scaler_arg;
	
	while(1)
	{
		if( _devicemount(USE_DISK))             // Mount device #0 = SD Card(c:\\)
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
			_deviceunmount(USE_DISK);
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}
    
    // Initiate tft Interface;
	if(DISPLAYUSED==TV_DISPLAY){
	  tv_init();
	  tv_start(TVSTD_NTSC_M,TV_QVGA,TV_NON_INTERLACE); 
	}else{
	  tft_init();
	  IL_GPM758A0_Init(); 
    }
    
    //user defined isr init
    user_defined_image_decode_entrance(DISPLAYUSED); 
    
    //sensor and isr init 
    image_sensor_start(); 
    
    //key scan init
    adc_key_scan_init(); 
    
    //jpeg encode callback used
    BLOCK_ENCODE=1;
    decode_mode_ck=1;
    
    //variable init
    counter=0;
    pic_num=0;
    q_value=50;
    
    while(1)
	{
		adc_key_scan();
	    if(ADKEY_IO1)	// Start to decode the specified file
		{
			c_xxx_zoom+=0.1;
	        if (c_xxx_zoom > 4) {
			    c_xxx_zoom=4;
	        }
		}
		else if(ADKEY_IO2)		// Stop decoding the current file
		{
		    c_xxx_zoom-=0.1;
	        if (c_xxx_zoom < 1) {
			c_xxx_zoom=1;
			}
		}
		else if(ADKEY_IO3)		// Stop decoding the current file
		{
		     if(counter++ > 4)
                counter=0;
                		     
		     switch(counter)
		     {   
                case 0:
                    q_value=50;                               //encode quality
                    DBG_PRINT("Q=50\r\n");
                    
                break;
       
                case 1:
                    q_value=70;
                    DBG_PRINT("Q=70\r\n");	
                break;
       
                case 2:
                    q_value=80;
                    DBG_PRINT("Q=80\r\n");	
                break;
       
                case 3:
                    q_value=90;
                    DBG_PRINT("Q=90\r\n");	
                break;
       
                case 4:
                    q_value=100;
                    DBG_PRINT("Q=100\r\n");	
                break;  
             }	    
		}
		else if(ADKEY_IO4)		// Stop decoding the current file
		{
		    save_file_ck=1;
		    //image scaler output infomation
            scaler_arg.inputvisiblewidth=DISPLAY_DEV_HPIXEL;
	        scaler_arg.inputvisibleheight=DISPLAY_DEV_VPIXEL;
	        scaler_arg.InputWidth=DISPLAY_DEV_HPIXEL;
	        scaler_arg.InputHeight=DISPLAY_DEV_VPIXEL;
	        scaler_arg.inputoffsetx=0;
	        scaler_arg.inputoffsety=0;
	        scaler_arg.outputwidthfactor=(DISPLAY_DEV_HPIXEL<<16)/SCALER_DEV_HPIXEL;   //scale x factor
	        scaler_arg.outputheightfactor=(16<<16)/32;                                 //scale y factor
	        scaler_arg.OutputWidth=SCALER_DEV_HPIXEL;
	        scaler_arg.OutputHeight=32;                                                //scale output y size length 
	        scaler_arg.InputBufPtr=(INT8U *)encode_buf;
	        scaler_arg.OutBoundaryColor=(INT32U)0x008080;
	        scaler_arg.InputFormat=SCALER_INPUT_FORMAT_YUYV;
            scaler_arg.OutputFormat=IMAGE_OUTPUT_FORMAT_YUV422;
	        //image encode infomation
	        encode_info.BlockLength=32;                                       //encode block read length
	        encode_info.quantizationquality=q_value;                               //encode quality
            encode_info.EncodeMode=IMAGE_ENCODE_BLOCK_READ_WRITE;                   //encode mode setting
            encode_info.InputWidth=SCALER_DEV_HPIXEL;                         //width of input image
            encode_info.InputHeight=SCALER_DEV_VPIXEL;                        //Heigh of input image
            encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUV_SEPARATE;   //encode input format
            encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;       //encode input format
            encode_info.scalerinfoptr=&scaler_arg;
            encode_output_ptr = (INT32U) gp_malloc_align((SCALER_DEV_HPIXEL*encode_info.BlockLength), 16);//malloc decode frame buffer
            encode_info.OutputBufPtr=(INT8U *)encode_output_ptr;              //encode output buffer
            if (USE_DISK==FS_SD){
               switch(encode_info.quantizationquality)
               {
                   case 50:  
         	            sprintf(path, "c:\\blockrw_q50_%03d.jpeg", pic_num);
                   break;
           
                   case 70:  
         	            sprintf(path, "c:\\blockrw_q70_%03d.jpeg", pic_num);
                   break;
           
                   case 80:  
         	            sprintf(path, "c:\\blockrw_q80_%03d.jpeg", pic_num);
                   break;
           
                   case 90:  
         	            sprintf(path, "c:\\blockrw_q90_%03d.jpeg", pic_num);
                   break;
           
                   case 100:  
         	            sprintf(path, "c:\\blockrw_q100_%03d.jpeg", pic_num);
                   break;                    
                }
            }       
            else if(USE_DISK==FS_NAND1)
    	         sprintf(path, "a:\\blockrw%03d.jpeg", pic_num);
            else
    	         sprintf(path, "g:\\blockrw%03d.jpeg", pic_num);	
            // Save encoded data to file
	        fd = open((CHAR *) path, O_CREAT|O_RDWR);
	        switch(encode_info.quantizationquality)
	        {
                  case 50:	
	                   if (fd >= 0) {
		                   jpeg_422_q50_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
	                       jpeg_422_q50_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
	                       jpeg_422_q50_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
		                   jpeg_422_q50_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);
		                   if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
		                      jpeg_422_q50_header_block_rw[0xA4] =  0x21;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/8);
			                  mcu_size=fifo_a*fifo_b;
		                   }else{
			                  jpeg_422_q50_header_block_rw[0xA4] =  0x22;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/16);
		                      mcu_size=fifo_a*fifo_b;
		                   }
		                   jpeg_422_q50_header_block_rw[0x260] = (mcu_size >> 8);
		                   jpeg_422_q50_header_block_rw[0x261] = (mcu_size & 0xFF);
		                   write(fd, (INT32U) &jpeg_422_q50_header_block_rw[0], sizeof(jpeg_422_q50_header_block_rw));
                       }
                  break;
      
                  case 70:	
	                   if (fd >= 0) {
		                   jpeg_422_q70_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
	                       jpeg_422_q70_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
	                       jpeg_422_q70_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
		                   jpeg_422_q70_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);
		                   if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
		                      jpeg_422_q70_header_block_rw[0xA4] =  0x21;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/8);
			                  mcu_size=fifo_a*fifo_b;
		                   }else{
			                  jpeg_422_q70_header_block_rw[0xA4] =  0x22;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/16);
		                      mcu_size=fifo_a*fifo_b;
		                   }
		                   jpeg_422_q70_header_block_rw[0x260] = (mcu_size >> 8);
		                   jpeg_422_q70_header_block_rw[0x261] = (mcu_size & 0xFF);
		                   write(fd, (INT32U) &jpeg_422_q70_header_block_rw[0], sizeof(jpeg_422_q70_header_block_rw));
                       }
                  break;
      
                  case 80:	
	                   if (fd >= 0) {
		                   jpeg_422_q80_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
	                       jpeg_422_q80_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
	                       jpeg_422_q80_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
		                   jpeg_422_q80_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);
		                   if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
		                      jpeg_422_q80_header_block_rw[0xA4] =  0x21;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/8);
			                  mcu_size=fifo_a*fifo_b;
		                   }else{
			                  jpeg_422_q80_header_block_rw[0xA4] =  0x22;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/16);
		                      mcu_size=fifo_a*fifo_b;
		                   }
		                   jpeg_422_q80_header_block_rw[0x260] = (mcu_size >> 8);
		                   jpeg_422_q80_header_block_rw[0x261] = (mcu_size & 0xFF);
		                   write(fd, (INT32U) &jpeg_422_q80_header_block_rw[0], sizeof(jpeg_422_q80_header_block_rw));
                       }
                  break;
      
                  case 90:	
	                   if (fd >= 0) {
		                   jpeg_422_q90_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
	                       jpeg_422_q90_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
	                       jpeg_422_q90_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
		                   jpeg_422_q90_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);
		                   if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
		                      jpeg_422_q90_header_block_rw[0xA4] =  0x21;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/8);
			                  mcu_size=fifo_a*fifo_b;
		                   }else{
			                  jpeg_422_q90_header_block_rw[0xA4] =  0x22;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/16);
		                      mcu_size=fifo_a*fifo_b;
		                   }
		                   jpeg_422_q90_header_block_rw[0x260] = (mcu_size >> 8);
		                   jpeg_422_q90_header_block_rw[0x261] = (mcu_size & 0xFF);
		                   write(fd, (INT32U) &jpeg_422_q90_header_block_rw[0], sizeof(jpeg_422_q90_header_block_rw));
                       }
                  break;
      
                  case 100:	
	                   if (fd >= 0) {
		                   jpeg_422_q100_header_block_rw[0x9E] = (SCALER_DEV_VPIXEL >> 8);
	                       jpeg_422_q100_header_block_rw[0x9F] = (SCALER_DEV_VPIXEL & 0xFF);
	                       jpeg_422_q100_header_block_rw[0xA0] = (SCALER_DEV_HPIXEL >> 8);
		                   jpeg_422_q100_header_block_rw[0xA1] = (SCALER_DEV_HPIXEL & 0xFF);
		                   if(encode_info.OutputFormat==IMAGE_ENCODE_OUTPUT_FORMAT_YUV422){
		                      jpeg_422_q100_header_block_rw[0xA4] =  0x21;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/8);
			                  mcu_size=fifo_a*fifo_b;
		                   }else{
			                  jpeg_422_q100_header_block_rw[0xA4] =  0x22;
			                  fifo_a=SCALER_DEV_HPIXEL/16;
			                  fifo_b=(encode_info.BlockLength/16);
		                      mcu_size=fifo_a*fifo_b;
		                   }
		                   jpeg_422_q100_header_block_rw[0x260] = (mcu_size >> 8);
		                   jpeg_422_q100_header_block_rw[0x261] = (mcu_size & 0xFF);
		                   write(fd, (INT32U) &jpeg_422_q100_header_block_rw[0], sizeof(jpeg_422_q100_header_block_rw));
                       }
                  break;
            }
            encode_info.FileHandle=fd;
            encode_info.UseDisk=USE_DISK;
            //image encode function
            image_encode_entrance();//global variable initial for image encoder
            encode_size=image_encode_start(encode_info);
            while (1) {
		          encode_state=image_encode_status();
                  if (encode_state==IMAGE_CODEC_DECODE_END) {
                      DBG_PRINT("image encode ok\r\n");
			      break;
		          }else if(encode_state==IMAGE_CODEC_DECODE_FAIL) {
			          DBG_PRINT("image encode failed\r\n");
			      break;
		          }	
            }     
            pic_num++;
            image_encode_stop();//image encode stop 
		    DBG_PRINT("image encode finish\r\n");
		    save_file_ck=0;
		}    
		OSTimeDly(1);
    }	
}

void IL_GPM758A0_Init()
{
		    //Initial 3-wire GPIO interface
			uTft_dat = 25;     //IOB9
			uTft_clk = 24;     //IOB8
			uTft_cs = 26;      //IOB10
			//Reset TFT control register at first
            R_TFT_CTRL=0;
			
            if(0) //Giantplus 3.2" Serial RGB
			{
				SPI_WriteCmdType2 (0, 0x0B010000);
				SPI_WriteCmdType2 (0, 0x02360000);
				SPI_WriteCmdType2 (0, 0x0C220000);
				SPI_WriteCmdType2 (0, 0x01160000);
				SPI_WriteCmdType2 (0, 0x10a70000);
				SPI_WriteCmdType2 (0, 0x11570000);
				SPI_WriteCmdType2 (0, 0x12730000);
				SPI_WriteCmdType2 (0, 0x13720000);
				SPI_WriteCmdType2 (0, 0x14730000);
				SPI_WriteCmdType2 (0, 0x15550000);
				SPI_WriteCmdType2 (0, 0x16170000);
				SPI_WriteCmdType2 (0, 0x17620000);
				
				R_TFT_HS_WIDTH			= 1;				//			=HPW
				R_TFT_H_START			= 241; //252;				//273		=HPW+HBP
				R_TFT_H_END				= 241+960; //252+1280;	    	//			=HPW+HBP+HDE
				R_TFT_H_PERIOD			= 1560;				//			=HPW+HBP+HDE+HFP
				R_TFT_VS_WIDTH			= 1;				//			=VPW				(DCLK)
				R_TFT_V_START			= 18;			    //			=VPW+VBP			(LINE)
				R_TFT_V_END				= 18+240;		    //			=VPW+VBP+VDE		(LINE)
				R_TFT_V_PERIOD			= 262;		    	//			=VPW+VBP+VDE+VFP	(LINE)
				R_TFT_CTRL           = 0x0307; //0x0327
			}
			else//Giantplus 3.2CCIR656
			{
				gpio_init_io (0x000017, GPIO_OUTPUT);
				gpio_write_io (0x000017, 0);
				cmd_delay (1000);
				gpio_write_io (0x000017, 1);
				cmd_delay (1000);

				SPI_WriteCmdType2 (0, 0x0B010000);
				SPI_WriteCmdType2 (0, 0x06BF0000);//add by yc				
				SPI_WriteCmdType2 (0, 0x0A590000);//add by yc
				cmd_delay (100);

				SPI_WriteCmdType2 (0, 0x02360000);
				SPI_WriteCmdType2 (0, 0x0C220000);
				SPI_WriteCmdType2 (0, 0x01160000);
				SPI_WriteCmdType2 (0, 0x10a70000);
				SPI_WriteCmdType2 (0, 0x11570000);
				SPI_WriteCmdType2 (0, 0x12730000);
				SPI_WriteCmdType2 (0, 0x13720000);
				SPI_WriteCmdType2 (0, 0x14730000);
				SPI_WriteCmdType2 (0, 0x15550000);
				SPI_WriteCmdType2 (0, 0x16170000);
				SPI_WriteCmdType2 (0, 0x17620000);
				R_TFT_HS_WIDTH			= 0;				//			=HPW
				R_TFT_H_START			= 273; //241; //252;				//273		=HPW+HBP
				R_TFT_H_END				= 273+1280; //241+960; //252+1280;	    	//			=HPW+HBP+HDE
				R_TFT_H_PERIOD			= 273+1280+2;				//			=HPW+HBP+HDE+HFP
				R_TFT_VS_WIDTH			= 0;				//			=VPW				(DCLK)
				R_TFT_V_START			= 10;//999 21;//ggg 1+17;			    //			=VPW+VBP			(LINE)
				R_TFT_V_END				= 253;//ggg 17+240+1;		    //			=VPW+VBP+VDE		(LINE)
				R_TFT_V_PERIOD			= 263;//ggg 1+17+240+4;		    	//			=VPW+VBP+VDE+VFP	(LINE)
				R_TFT_CTRL           = 0x0727; //0x0307 //0x0327

			}
}

void SPI_WriteCmdType2 (INT32S nCmdType, INT32U uCmd)
{
	INT32S	nBits;
	INT32S	i;

	nBits = (nCmdType + 2) << 3;
	uCmd &= ~((((INT32U)1) << (nBits - 9 * nCmdType)) - 1);
	//	Initial SPI GPIO interface
	gpio_init_io (uTft_dat, GPIO_OUTPUT);
	gpio_init_io (uTft_clk, GPIO_OUTPUT);
	gpio_init_io (uTft_cs, GPIO_OUTPUT);
	//	Initial SPI
	gpio_write_io (uTft_dat, 0);
	gpio_write_io (uTft_clk, 0);
	gpio_write_io (uTft_cs, 1);
	cmd_delay (5);
	
	//	Activate CS low to start transaction
	gpio_write_io (uTft_cs, 0);
	cmd_delay (1);
	for (i=0;i<nBits;i++) {
		//	Activate SPI data
		if ((uCmd & 0x80000000) == 0x80000000) {
			gpio_write_io (uTft_dat, 1);
		}
		if ((uCmd & 0x80000000) == 0) {
			gpio_write_io (uTft_dat, 0);
		}
		cmd_delay (1);
		//	Toggle SPI clock
		gpio_write_io (uTft_clk, 1);
		cmd_delay (1);
		gpio_write_io (uTft_clk, 0);
		cmd_delay (1);
		uCmd <<= 1;
	}
	//	Pull low data
	gpio_write_io (uTft_dat, 0);

	//	Activate CS high to stop transaction
	gpio_write_io (uTft_cs, 1);
	cmd_delay (1);
}