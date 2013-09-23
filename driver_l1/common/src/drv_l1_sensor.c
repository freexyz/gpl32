#include	"drv_l1_sensor.h"

#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)  
#include "drv_l1_mipi.h"
#endif

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1)             //
//================================================================//
/****************************************************
*		constant 									*
****************************************************/
// Sensor Interface Control Register Definitions
/*
R_CSI_TG_CTRL0			-	Timing Generator Control 1
*/
#define	CSIEN					(1<<0)			// Sensor controller enable bit
#define CAP						(1<<1)			// Capture / preview mode
#define HREF					(1<<2)			// HREF / HSYNC mode
#define	CSI_RGB888IN			(0<<3)
#define	CSI_RGB565IN			(1<<3)			// RGB565 / RGB888 input @YUVIN=0B
#define CLKIINV					(1<<4)			// Invert input clock
#define YUVIN					(1<<5)			// YUV / RGB input
#define YUVOUT					(1<<6)			// YUV / RGB output
#define BSEN					(1<<7)			// Enable blue screen effect
#define CCIR656					(1<<8)			// CCIR656 / CCIR601 Interface
#define FGET_RISE				(1<<9)			// Field latch timing at the rising edge of VSYNC
#define HRST_FALL				(0<<10)			// Horizontal counter reset at the rising edge of HSYNC
#define HRST_RISE				(1<<10)			// Horizontal counter reset at the rising edge of HSYNC
#define VADD_FALL				(0<<11)			// Vertical counter increase at the rising edge of HSYNC
#define VADD_RISE				(1<<11)			// Vertical counter increase at the rising edge of HSYNC
#define VRST_FALL				(0<<12)			// Vertical counter reset at the rising edge of VSYNC
#define VRST_RISE				(1<<12)			// Vertical counter reset at the rising edge of VSYNC
#define YUV_YUYV				(1<<13)			// YUYV(GBGR) / UYVY (BGRG) selection
#define FIELDINV    			(1<<14)			// Invert field input
#define CSI_INTERLACE  			(1<<15)			// Interlace / non-interlace mode
/*
R_CSI_TG_CTRL1			-	Timing Generator Control 2
*/
#define D_TYPE0         		(0<<0)			// Data latch delay 1 clock
#define D_TYPE1         		(1<<0)			// Data latch delay 2 clock
#define D_TYPE2         		(2<<0)			// Data latch delay 3 clock
#define D_TYPE3         		(3<<0)			// Data latch delay 4 clock
#define CLKOINV					(1<<3)			// Invert output clock
#define	CSI_RGB565				(0<<4)
#define CSI_RGB1555				(1<<4)			// RGB1555 / RGB565 mode output
#define INVYUVO          		(1<<6)			// Invert output UV's bit 7
#define CLKOEN					(1<<7)			// CSI output clock enable
#define CUTEN					(1<<8)			// Screen CUT enable
#define INVYUVI					(1<<9)			// Invert input UV's bit 7
#define YONLY					(1<<10)			// Only Y output enable
#define CLK_SEL27M				(0<<11)			// 27MHz
#define CLK_SEL48M				(1<<11)			// 48MHz
#define	CSI_CELL				(1<<12)
#define	CSI_CELL32X32			(1<<13)
#define	CSI_NOSTOP				(1<<14)
#define	CSI_HIGHPRI			    (1<<15)
/*
P_MD_CTRL			-	Motion Detection Control Register
*/
#define MD_EN					(1<<1)			// Enable H/W motion detection (MD)
#define MD_FRAME_1				(0<<2)			// MD detects every frame
#define MD_FRAME_2				(1<<2)			// MD detects every 2 frame
#define MD_FRAME_4				(2<<2)			// MD detects every 4 frame
#define MD_FRAME_8				(3<<2)			// MD detects every 8 frame
#define MD_VGA					(1<<4)			// VGA / QVGA size
#define MD_YUV					(1<<5)			// YUV / RGB data type
#define MD_MODE_AVE				(0<<6)			// Average mode
#define MD_MODE_SINGLE			(1<<6)			// Single mode
#define MD_MODE_AVEIIR			(2<<6)			// Average IIR mode
#define MD_MODE_SINGLEIIR		(3<<6)			// Single IIR mode
#define MD_8X8					(1<<8)			// 8x8 (only in QVGA mode) / 16x16


//====================================================================================================
// SCCB Control C Code
// using SDA and SDL to control sensor interface
//====================================================================================================

//====================================================================================================
//	Description:	Delay function
//	Syntax:			sccb_delay (
//						INT16U i
//					);
//	Return: 		None
//====================================================================================================
void sccb_delay (
	INT32U i
) 
{
	INT32U j;

	for(j=0; j<(i<<8); j++) {
		i = i;
	}
}

//===================================================================
//	Description:	Start of data transmission
//	Function:		sccb_start
//	Syntax:			void sccb_start (void)
//	Input Paramter:
//					none
//	Return: 		none
//	Note:			Please refer to SCCB spec
//===================================================================
void sccb_start (void)
{
	gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL
	gpio_write_io(SCCB_SDA, DATA_HIGH);					//SDA
	sccb_delay (1);
	gpio_write_io(SCCB_SDA, DATA_LOW);					//SDA
	sccb_delay (1);
	gpio_write_io(SCCB_SCL, DATA_LOW);					//SCL
	sccb_delay (1);
}

//===================================================================
//	Description:	Stop of data transmission
//	Function:		sccb_stop
//	Syntax:			void sccb_stop (void)
//	Input Paramter:
//					none
//	Return: 		none
//	Note:			Please refer to SCCB spec
//===================================================================
void sccb_stop (void)
{
	gpio_write_io(SCCB_SCL, DATA_LOW);					//SCL
	gpio_write_io(SCCB_SDA, DATA_LOW);					//SDA
	sccb_delay (1);
	gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL
	sccb_delay (1);
	gpio_write_io(SCCB_SDA, DATA_HIGH);					//SDA
	sccb_delay (1);
}

//===================================================================
//	Description:	Phase write process
//	Function:		sccb_w_phase
//	Syntax:			void sccb_w_phase (INT16U value)
//	Input Paramter:
//					INT16U value:	output data
//	Return: 		none
//	Note:			Please refer to SCCB spec
//===================================================================
INT32S sccb_w_phase (INT16U value, INT8U ack)
{
	INT32U i;
	INT32S ret = 0;

	for(i=0;i<8;i++){
		gpio_write_io(SCCB_SCL, DATA_LOW);					//SCL0
		sccb_delay (1);
		if(value & 0x80) {
			gpio_write_io(SCCB_SDA, DATA_HIGH);				//SDA1
		} else {
			gpio_write_io(SCCB_SDA, DATA_LOW);				//SDA0
		}
		
		gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL1
		sccb_delay(1);
		value <<= 1;
	}
	
	// The 9th bit transmission
	gpio_write_io(SCCB_SCL, DATA_LOW);						//SCL0
	gpio_init_io(SCCB_SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	sccb_delay(1);
	gpio_write_io(SCCB_SCL, DATA_HIGH);						//SCL1
	
	// ckeck ack = low
	if(ack) {
		for(i=0; i<0x20000; i++) {
			if(gpio_read_io(SCCB_SDA) == 0) {
				break;
			}
		}		
	}

	if(i == 0x20000) {
		ret = -1;
	}
	
	sccb_delay(1);
	gpio_write_io(SCCB_SCL, DATA_LOW);						//SCL0
	gpio_init_io(SCCB_SDA, GPIO_OUTPUT);					//SDA is Hi-Z mode
	return ret;
}

//===================================================================
//	Description:	Phase read process
//	Function:		sccb_r_phase
//	Syntax:			INT16U sccb_r_phase (INT8U phase)
//	Input Paramter:
//					INT8U phase	: In phase 2, the nineth bit is ACK.
//								  In phase 3, the nineth bit is NAK
//	Return: 		INT16U			: input data
//	Note:			Please refer to SCCB spec
//===================================================================
INT16U sccb_r_phase (INT8U phase)
{
	INT16U i;
	INT16U data = 0x00;

	gpio_init_io(SCCB_SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	for(i=0;i<8;i++) {
		gpio_write_io(SCCB_SCL, DATA_LOW);					//SCL0
		sccb_delay(1);
		gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL1
		data <<= 1;
		data |=( gpio_read_io(SCCB_SDA));
		sccb_delay(1);
	}
	
	// The 9th bit transmission
	gpio_write_io(SCCB_SCL, DATA_LOW);						//SCL0
	gpio_init_io(SCCB_SDA, GPIO_OUTPUT);					//SDA is output mode
	if(phase) {
		gpio_write_io(SCCB_SDA, DATA_LOW);					//SDA0, the nineth bit is ACK must be 1
	} else {
		gpio_write_io(SCCB_SDA, DATA_HIGH);					//SDA0, the nineth bit is NAK must be 1
	}
	
	sccb_delay(1);
	gpio_write_io(SCCB_SCL, DATA_HIGH);						//SCL1
	sccb_delay(1);
	gpio_write_io(SCCB_SCL, DATA_LOW);						//SCL0
	return data;		
}

//====================================================================================================
//	Description:	SCCB Initialization
//	Syntax:			void sccb_init (
//						void
//					);
//	Return: 		None
//====================================================================================================
void sccb_init (
	void
){
	//init IO
	gpio_set_port_attribute(SCCB_SCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(SCCB_SDA, ATTRIBUTE_HIGH);
	gpio_init_io(SCCB_SCL, GPIO_OUTPUT);				//set dir	 	
	gpio_init_io(SCCB_SDA, GPIO_OUTPUT);				//set dir
	gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL1		
	gpio_write_io(SCCB_SDA, DATA_HIGH);					//SDA0	 
}

//====================================================================================================
//	Description:	SCCB write register process. 
//	Syntax:			INT32S sccb_write (
//						INT8U id,				// Slave ID
//						INT32U addr_bits,		// Register Address bit number
//						INT32U addr,			// Register Address
//						INT32U data_bits,		// Register Data bit number
//						INT32U data				// Register Data
//					);
//====================================================================================================
INT32S sccb_write(
	INT8U id, 
	INT32U addr_bits, 
	INT32U addr, 
	INT32U data_bits,
	INT32U data
)
{
	INT8U temp, ack = 1;
	INT32S ret;

	// 3-Phase write transmission
	// Transmission start 
	sccb_start();			
	
	// Phase 1, SLAVE_ID
	ret = sccb_w_phase(id, 1);	
	if(ret < 0) {
		goto __exit;
	}	
	
	// Phase 2, Register address
	while(addr_bits >= 8) {
		addr_bits -= 8;
		temp = addr >> addr_bits;
		ret = sccb_w_phase(temp, ack);
		if(ret < 0) {
			goto __exit;
		}
	}
		
	// Phase 3, Register Data
	while(data_bits >= 8) {
		data_bits -= 8;
		if(data_bits) {
			ack = 1; //ACK
		} else {
			ack = 0; //NACK
		}
		
		temp = data >> data_bits;
		ret = sccb_w_phase(temp, ack);
		if(ret < 0) {
			goto __exit;
		}
	}
	
__exit:
	// Transmission stop	
	sccb_stop();	
	return ret;	
}

//====================================================================================================
//	Description:	SCCB read register process. 
//	Syntax:			INT32S sccb_read (
//						INT8U id,				// Slave ID
//						INT32U addr_bits,		// Register Address bit number
//						INT32U addr,			// Register Address
//						INT32U data_bits,		// Register Data bit number
//						INT32U *data			// Register Data
//					);
//====================================================================================================
INT32S 
sccb_read(
	INT8U id, 
	INT32U addr_bits, 
	INT32U addr, 
	INT32U data_bits, 
	INT32U *data
)
{
	INT8U temp, ack = 1;
	INT32U read_data;
	INT32S ret;
	
	// 2-Phase write transmission cycle
	// Transmission start
	sccb_start();	
	
	// Phase 1, SLAVE_ID
	ret = sccb_w_phase(id, 1);		
	if(ret < 0) {
		goto __exit;
	}
	
	// Phase 2, Register Address
	while(addr_bits >= 8) {
		addr_bits -= 8;
		temp = addr >> addr_bits;
		ret = sccb_w_phase(temp, ack);
		if(ret < 0) {
			goto __exit;
		}
	}
	
	// Transmission stop
	sccb_stop();	

	// 2-Phase read transmission cycle
	// Transmission start
	sccb_start();	
	
	// Phase 1 (read)
	ret = sccb_w_phase(id | 0x01, 1);	
	if(ret < 0) {
		goto __exit;
	}
	
	// Phase 2 (read)
	read_data = 0;
	while(data_bits >= 8) {
		data_bits -= 8;
		if(data_bits) {
			ack = 1; //ACK
		} else {	
			ack = 0; //NACK
		}
		
		temp = sccb_r_phase(ack);
		read_data <<= 8;
		read_data |= temp;
	}
	*data = read_data;
	
__exit:
	// Transmission stop
	sccb_stop();	
	return ret;
}

void sccb_lock(void)
{
	OSSchedLock();
}

void sccb_unlock(void)
{
	OSSchedUnlock();
}

INT32S sccb_write_Reg8Data8(INT8U id, INT8U addr, INT8U data) 
{
	INT32S ret;
	
	sccb_lock();
	ret = sccb_write(id, 8, addr, 8, data);
	sccb_unlock();
	return ret;
}

INT32S sccb_read_Reg8Data8(INT8U id, INT8U addr, INT8U *data) 
{
	INT32S ret;
	INT32U rbdata;
	
	sccb_lock();
	ret = sccb_read(id, 8, addr, 8, &rbdata);
	if(ret >= 0) {
		*data = rbdata & 0xFF;
	}
	sccb_unlock();
	return ret;
}

INT32S sccb_write_Reg8Data16(INT8U id, INT8U addr, INT16U data) 
{
	INT32S ret;
	
	sccb_lock();
	ret = sccb_write(id, 8, addr, 16, data);
	sccb_unlock();
	return ret;
}

INT32S sccb_read_Reg8Data16(INT8U id, INT8U addr, INT16U *data) 
{
	INT32S ret;
	INT32U rbdata;
	
	sccb_lock();
	ret = sccb_read(id, 8, addr, 16, &rbdata);
	if(ret >= 0) {
		*data = rbdata & 0xFFFF;
	}
	sccb_unlock();
	return ret;
}

INT32S sccb_write_Reg16Data8(INT8U id, INT16U addr, INT8U data) 
{
	INT32S ret;
	
	sccb_lock();
	ret = sccb_write(id, 16, addr, 8, data);
	sccb_unlock();
	return ret;
}

INT32S sccb_read_Reg16Data8(INT8U id, INT16U addr, INT8U *data) 
{
	INT32S ret;
	
	sccb_lock();
	ret = sccb_read(id, 16, addr, 8, (INT32U *)data);
	sccb_unlock();
	return ret;
}

INT32S sccb_write_Reg16Data16(INT8U id, INT16U addr, INT16U data) 
{
	INT32S ret;
	
	sccb_lock();
	ret = sccb_write(id, 16, addr, 16, data);
	sccb_unlock();
	return ret;
}

INT32S sccb_read_Reg16Data16(INT8U id,	INT16U addr, INT16U *data) 
{
	INT32S ret;
	INT32U rbdata;
	
	sccb_lock();
	ret = sccb_read(id, 16, addr, 16, &rbdata);
	if(ret >= 0) {
		*data = rbdata & 0xFFFF;
	}
	sccb_unlock();
	return ret;
}

#ifdef	__HSB0357_DRV_C__
//====================================================================================================
//	Description:	HSB0357 Initialization
//	Syntax:			void HSB0357_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void HSB0357_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	INT16S nReso;

	gpio_drving_init_io(IO_F14,IOF14_DRV_8mA);//set driving capability of MCLK(IO_F14) to 8mA
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000;
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000;
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000;
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000;
#endif

	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_write_Reg16Data8(HSB0357_ID, 0x8A04 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x8A00 , 0x22);
	sccb_write_Reg16Data8(HSB0357_ID, 0x8A01 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x8A02 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0035 , 0xD1);//D1=24M,C1=48M
	sccb_write_Reg16Data8(HSB0357_ID, 0x0036 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0011 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0012 , 0xE0);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0014 , 0x02);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0015 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0100 , 0x04);

	///////////////////////////////////////////////

	sccb_write_Reg16Data8(HSB0357_ID, 0x0022 , 0x00);	// Reset
	sccb_write_Reg16Data8(HSB0357_ID, 0x0020 , 0x08);	// Reset         //20100901	W 30 08->0x68

	sccb_write_Reg16Data8(HSB0357_ID, 0x0023 , 0x7F);	//W 30 0023 18,//W 30 0023 7F,	  	//20100901	0x58->0x88
	sccb_write_Reg16Data8(HSB0357_ID, 0x008F , 0x00);	//W 30 008F FF,

	sccb_write_Reg16Data8(HSB0357_ID, 0x0004 , 0x11);	//
	//sccb_write_Reg16Data8(HSB0357_ID, 0x000F , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x000F , 0x08);	//set bit 3 of 0x000F as 1 to fix frame rate@30FPS
	sccb_write_Reg16Data8(HSB0357_ID, 0x0012 , 0x0C);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0013 , 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0015 , 0x02);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0016 , 0x01);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x0025 , 0x00);	// ISP=24M
	sccb_write_Reg16Data8(HSB0357_ID, 0x0027 , 0x30);	// YCbYCr

	sccb_write_Reg16Data8(HSB0357_ID, 0x0040 , 0x0F);	// BLC Ph1
	sccb_write_Reg16Data8(HSB0357_ID, 0x0053 , 0x0F);	// BLC Ph2
	sccb_write_Reg16Data8(HSB0357_ID, 0x0075 , 0x40);	// Auto Negative Pump

	sccb_write_Reg16Data8(HSB0357_ID, 0x0041 , 0x02);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0045 , 0xCA);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0046 , 0x4F);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x004A , 0x0A);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x004B , 0x72);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x004D , 0xBF);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x004E , 0x30); //
	sccb_write_Reg16Data8(HSB0357_ID, 0x0055 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0053 , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0070 , 0x44);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0071 , 0xB9);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0072 , 0x55);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0073 , 0x10);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x0081 , 0xD2);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0082 , 0xA6);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0083 , 0x70);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0085 , 0x11);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0086 , 0xA7);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0088 , 0xE1);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x008A , 0x2D);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x008D , 0x20);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0090 , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0091 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0092 , 0x11);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0093 , 0x12);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0094 , 0x13);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0095 , 0x17);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x00A0 , 0xC0);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x011F , 0x44);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0121 , 0x80);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0122 , 0x6B);	// SIM_BPC,LSC_ON,A6_ON
	sccb_write_Reg16Data8(HSB0357_ID, 0x0123 , 0xA5);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0124 , 0xD2);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0125 , 0xDE);	// Disable F2B function//30FPS@VGA
	//sccb_write_Reg16Data8(HSB0357_ID, 0x0125 , 0xDE);	// 60FPS@QVGA
	sccb_write_Reg16Data8(HSB0357_ID, 0x0126 , 0x71);	// 70=EV_SEL 3x5,60=EV_SEL 3x3

	sccb_write_Reg16Data8(HSB0357_ID, 0x0140 , 0x14);	// BPC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0141 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0142 , 0x14);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0143 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0144 , 0x0C);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0145 , 0x03);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0146 , 0x40);	// New BPC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0147 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0148 , 0x70);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0149 , 0x0C);

	sccb_write_Reg16Data8(HSB0357_ID, 0x014A , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x014B , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x014C , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x014D , 0x2E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x014E , 0x05);
	sccb_write_Reg16Data8(HSB0357_ID, 0x014F , 0x05);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0150 , 0x0D);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0155 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0156 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0157 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0158 , 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0159 , 0x0A);

	sccb_write_Reg16Data8(HSB0357_ID, 0x0160 , 0x14);	// SIM_BPC_th
	sccb_write_Reg16Data8(HSB0357_ID, 0x0161 , 0x14);	// SIM_BPC_th Alpha
	sccb_write_Reg16Data8(HSB0357_ID, 0x0162 , 0x0A);	// SIM_BPC_Directional
	sccb_write_Reg16Data8(HSB0357_ID, 0x0163 , 0x0A);	// SIM_BPC_Directional Alpha

	sccb_write_Reg16Data8(HSB0357_ID, 0x01B0 , 0x33);	// G1G2 Balance
	sccb_write_Reg16Data8(HSB0357_ID, 0x01B1 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x01B2 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x01B3 , 0x30);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x01B4 , 0x18);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x01D8 , 0x50);	// Bayer Denoise
	sccb_write_Reg16Data8(HSB0357_ID, 0x01DE , 0x70);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x01E4 , 0x08);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x01E5 , 0x08);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x0220 , 0x00);	//LSC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0221 , 0xBD);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0222 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0223 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0224 , 0x8A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0225 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0226 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0227 , 0x8A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0229 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022A , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022B , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022C , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022D , 0x11);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022E , 0x10);
	sccb_write_Reg16Data8(HSB0357_ID, 0x022F , 0x11);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0230 , 0x10);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0231 , 0x11);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0233 , 0x11);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0234 , 0x10);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0235 , 0x46);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0236 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0237 , 0x46);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0238 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023B , 0x46);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023C , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023D , 0xF8);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023E , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023F , 0xF8);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0240 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0243 , 0xF8);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0244 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0250 , 0x03);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0251 , 0x0D);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0252 , 0x08);
                       
	sccb_write_Reg16Data8(HSB0357_ID, 0x0280 , 0x0B);	//Gamma
	sccb_write_Reg16Data8(HSB0357_ID, 0x0282 , 0x15);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0284 , 0x2A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0286 , 0x4C);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0288 , 0x5A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x028A , 0x67);
	sccb_write_Reg16Data8(HSB0357_ID, 0x028C , 0x73);
	sccb_write_Reg16Data8(HSB0357_ID, 0x028E , 0x7D);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0290 , 0x86);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0292 , 0x8E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0294 , 0x9E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0296 , 0xAC);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0298 , 0xC0);
	sccb_write_Reg16Data8(HSB0357_ID, 0x029A , 0xD2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x029C , 0xE2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x029E , 0x27);
                       
	sccb_write_Reg16Data8(HSB0357_ID, 0x02A0 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C0 , 0xCC);	//D CCM
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C1 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C2 , 0xBB);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C3 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C4 , 0x11);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C5 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C6 , 0x2E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C7 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C8 , 0x6C);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02C9 , 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CA , 0x3E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CB , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CC , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CD , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CE , 0xBE);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02CF , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02D0 , 0xC2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02D1 , 0x01);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F0 , 0xA3);	// CCM DIFF
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F1 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F2 , 0xE9);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F3 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F4 , 0x45);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F5 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F6 , 0x1D);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F7 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F8 , 0x45);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02F9 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FA , 0x63);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FB , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FC , 0x3E);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FD , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FE , 0x39);
	sccb_write_Reg16Data8(HSB0357_ID, 0x02FF , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0300 , 0x79);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0301 , 0x00);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0328 , 0x00);	// Win Pick P Min
	sccb_write_Reg16Data8(HSB0357_ID, 0x0329 , 0x04);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x032D , 0x66);	// Initial Channel Gain
	sccb_write_Reg16Data8(HSB0357_ID, 0x032E , 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x032F , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0330 , 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0331 , 0x66);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0332 , 0x01);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0333 , 0x00);	// AWB channel offset
	sccb_write_Reg16Data8(HSB0357_ID, 0x0334 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0335 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x033E , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x033F , 0x00);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0340 , 0x38);	// AWB
	sccb_write_Reg16Data8(HSB0357_ID, 0x0341 , 0x41);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0342 , 0x04);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0343 , 0x2C);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0344 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0345 , 0x37);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0346 , 0x80);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0347 , 0x3D);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0348 , 0x76);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0349 , 0x42);
	sccb_write_Reg16Data8(HSB0357_ID, 0x034A , 0x6B);
	sccb_write_Reg16Data8(HSB0357_ID, 0x034B , 0x68);
	sccb_write_Reg16Data8(HSB0357_ID, 0x034C , 0x58);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0350 , 0x90);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0351 , 0x90);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0352 , 0x18);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0353 , 0x18);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0354 , 0x73);	// R Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x0355 , 0x45);	// G Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x0356 , 0x78);	// B Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x0357 , 0xD0);	// R+B Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x0358 , 0x05);	// R comp Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x035A , 0x05);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x035B , 0xA0);	// R+B Min
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0381 , 0x5E);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0382 , 0x3A);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0383 , 0x20);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x038A , 0x80);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x038B , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x038C , 0xD1);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x038E , 0x4C);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x038F , 0x05);	//AE Max exposure
	sccb_write_Reg16Data8(HSB0357_ID, 0x0390 , 0xD0);	//AE Max exposure
	sccb_write_Reg16Data8(HSB0357_ID, 0x0391 , 0x7C);	//AE Min exposure
	sccb_write_Reg16Data8(HSB0357_ID, 0x0393 , 0x80);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0395 , 0x12);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0398 , 0x01);	// Frame rate control
	sccb_write_Reg16Data8(HSB0357_ID, 0x0399 , 0xF0);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039A , 0x03);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039B , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039C , 0x04);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039D , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039E , 0x06);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x039F , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A0 , 0x09);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A1 , 0x50);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A6 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A7 , 0x10);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A8 , 0x36);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A9 , 0x40);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03AE , 0x26);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03AF , 0x22);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B0 , 0x0A);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B1 , 0x08);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B3 , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B5 , 0x08);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B7 , 0xA0);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B9 , 0xD0);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x03BB , 0x5F);	// AE Winding
	sccb_write_Reg16Data8(HSB0357_ID, 0x03BE , 0x00);	// AE Winding
	sccb_write_Reg16Data8(HSB0357_ID, 0x03BF , 0x1D);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x03C0 , 0x2E);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x03C3 , 0x0F);	//
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x03D0 , 0xE0);	//
                       
	sccb_write_Reg16Data8(HSB0357_ID, 0x0420 , 0x86);	// BLC Offset
	sccb_write_Reg16Data8(HSB0357_ID, 0x0421 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0422 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0423 , 0x00);
                     
	sccb_write_Reg16Data8(HSB0357_ID, 0x0430 , 0x00);	// ABLC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0431 , 0x60);	//Max
	sccb_write_Reg16Data8(HSB0357_ID, 0x0432 , 0x30);	//ABLC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0433 , 0x30);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0434 , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0435 , 0x40);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0436 , 0x00);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x0450 , 0xFF);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0451 , 0xFF);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0452 , 0xB0);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0453 , 0x70);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0454 , 0x00);
                        
	sccb_write_Reg16Data8(HSB0357_ID, 0x045A , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x045B , 0x10);
	sccb_write_Reg16Data8(HSB0357_ID, 0x045C , 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x045D , 0xA0);

	sccb_write_Reg16Data8(HSB0357_ID, 0x0465 , 0x02);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0466 , 0x04);

	sccb_write_Reg16Data8(HSB0357_ID, 0x047A , 0x20);	// ODEL Rayer denoise
	sccb_write_Reg16Data8(HSB0357_ID, 0x047B , 0x20);	// ODEL Y denoise

	sccb_write_Reg16Data8(HSB0357_ID, 0x0480 , 0x46);	//Sat
	sccb_write_Reg16Data8(HSB0357_ID, 0x0481 , 0x06);	//Sat by Alpha

	sccb_write_Reg16Data8(HSB0357_ID, 0x04B0 , 0x40);	//Contrast
	sccb_write_Reg16Data8(HSB0357_ID, 0x04B1 , 0x00);	//Contrast
	sccb_write_Reg16Data8(HSB0357_ID, 0x04B3 , 0x7F);	//Contrast
	sccb_write_Reg16Data8(HSB0357_ID, 0x04B4 , 0x00);	//Contrast
	sccb_write_Reg16Data8(HSB0357_ID, 0x04B6 , 0x20);	//Contrast
	sccb_write_Reg16Data8(HSB0357_ID, 0x04B9 , 0x40);	//Contrast


	sccb_write_Reg16Data8(HSB0357_ID, 0x0540 , 0x00);	//60Hz Flicker step
	sccb_write_Reg16Data8(HSB0357_ID, 0x0541 , 0x7C);	//60Hz Flicker step
	sccb_write_Reg16Data8(HSB0357_ID, 0x0542 , 0x00);	//50Hz Flicker step
	sccb_write_Reg16Data8(HSB0357_ID, 0x0543 , 0x95);	//50Hz Flicker step

	sccb_write_Reg16Data8(HSB0357_ID, 0x0580 , 0x01);	// Y Blurring
	sccb_write_Reg16Data8(HSB0357_ID, 0x0581 , 0x04);	// Y Blurring Alpha

	sccb_write_Reg16Data8(HSB0357_ID, 0x0590 , 0x20);	// UV denoise
	sccb_write_Reg16Data8(HSB0357_ID, 0x0591 , 0x30);	// UV denoise Alpha

	sccb_write_Reg16Data8(HSB0357_ID, 0x0594 , 0x02);	// UV Gray TH
	sccb_write_Reg16Data8(HSB0357_ID, 0x0595 , 0x08);	// UV Gray TH Alpha

	sccb_write_Reg16Data8(HSB0357_ID, 0x05A0 , 0x08);	// Sharpness Curve Edge THL
	sccb_write_Reg16Data8(HSB0357_ID, 0x05A1 , 0x0C);	// Edge THL Alpha
	sccb_write_Reg16Data8(HSB0357_ID, 0x05A2 , 0x50);	// Edge THH
	sccb_write_Reg16Data8(HSB0357_ID, 0x05A3 , 0x60);	// Edge THH Alpha
	sccb_write_Reg16Data8(HSB0357_ID, 0x05B0 , 0x18);	// Edge strength
	sccb_write_Reg16Data8(HSB0357_ID, 0x05B1 , 0x06);	//

	sccb_write_Reg16Data8(HSB0357_ID, 0x05D0 , 0x2D);	// F2B Start Mean
	sccb_write_Reg16Data8(HSB0357_ID, 0x05D1 , 0x07);	// F2B

	sccb_write_Reg16Data8(HSB0357_ID, 0x05E4 , 0x04);	// Windowing
	sccb_write_Reg16Data8(HSB0357_ID, 0x05E5 , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05E6 , 0x83);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05E7 , 0x02);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05E8 , 0x04);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05E9 , 0x00);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05EA , 0xE3);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x05EB , 0x01);	//



	//Pink Issue	20101118 by Harris
	sccb_write_Reg16Data8(HSB0357_ID, 0x0090, 0x00);	// Analog Table
	sccb_write_Reg16Data8(HSB0357_ID, 0x0091, 0x01);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0092, 0x02);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0093, 0x03);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0094, 0x07);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0095, 0x08);
	sccb_write_Reg16Data8(HSB0357_ID, 0x00A0, 0xC4);
	sccb_write_Reg16Data8(HSB0357_ID, 0x01E5, 0x0A);	// Bayer De-Noise A0 Strength
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A6, 0x0D);	// AE Frame Rate Control
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A7, 0x0A);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A8, 0x23);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03A9, 0x33);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03AE, 0x20);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03AF, 0x17);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B0, 0x07);
	sccb_write_Reg16Data8(HSB0357_ID, 0x03B1, 0x08);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0221, 0xD8);	// LSC
	sccb_write_Reg16Data8(HSB0357_ID, 0x0224, 0x85);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0227, 0x85);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0235, 0xF2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0236, 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0237, 0xF2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x0238, 0x00);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023B, 0xF2);
	sccb_write_Reg16Data8(HSB0357_ID, 0x023C, 0x00);
	//END


	sccb_write_Reg16Data8(HSB0357_ID, 0x0000, 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0100, 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0101, 0x01);	//
	sccb_write_Reg16Data8(HSB0357_ID, 0x0005, 0x01);	// Turn on rolling shutter

	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// @60FPS QVGA
	{
		sccb_write_Reg16Data8(HSB0357_ID, 0x000D, 0x01);
		sccb_write_Reg16Data8(HSB0357_ID, 0x000E, 0x11);
		sccb_write_Reg16Data8(HSB0357_ID, 0x000F, 0x08);
		sccb_write_Reg16Data8(HSB0357_ID, 0x0011, 0x9C);
		sccb_write_Reg16Data8(HSB0357_ID, 0x0020, 0x08);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E4, 0x02);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E5, 0x00);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E6, 0x41);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E7, 0x01);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E8, 0x04);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05E9, 0x00);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05EA, 0xF3);
		sccb_write_Reg16Data8(HSB0357_ID, 0x05EB, 0x00);
		sccb_write_Reg16Data8(HSB0357_ID, 0x0000, 0x01);
		sccb_write_Reg16Data8(HSB0357_ID, 0x0100, 0x01);
		sccb_write_Reg16Data8(HSB0357_ID, 0x0101, 0x01);
	}

	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif

}
#endif
#ifdef	__OV6680_DRV_C__
//====================================================================================================
//	Description:	OV6680 Initialization
//	Syntax:			void OV6680_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV6680_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
}
#endif

#ifdef	__OV9655_DRV_C__
//====================================================================================================
//	Description:	OV9655 Initialization
//	Syntax:			void OV9655_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV9655_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
#else
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg8Data8(OV9655_ID, 0x12, 0x80);
	sccb_delay(200);

	sccb_write_Reg8Data8(OV9655_ID,0xb5,0x00);
	sccb_write_Reg8Data8(OV9655_ID,0x35,0x00);
	sccb_write_Reg8Data8(OV9655_ID,0xa8,0xc1);
	sccb_write_Reg8Data8(OV9655_ID,0x3a,0x80);
	sccb_write_Reg8Data8(OV9655_ID,0x3d,0x99);
	sccb_write_Reg8Data8(OV9655_ID,0x77,0x02);
	sccb_write_Reg8Data8(OV9655_ID,0x13,0xe7);
	sccb_write_Reg8Data8(OV9655_ID,0x26,0x72);
	sccb_write_Reg8Data8(OV9655_ID,0x27,0x08);
	sccb_write_Reg8Data8(OV9655_ID,0x28,0x08);
	sccb_write_Reg8Data8(OV9655_ID,0x29,0x15);
	sccb_write_Reg8Data8(OV9655_ID,0x2c,0x08);
	sccb_write_Reg8Data8(OV9655_ID,0xab,0x04);
	sccb_write_Reg8Data8(OV9655_ID,0x6e,0x00);
	sccb_write_Reg8Data8(OV9655_ID,0x6d,0x55);
	sccb_write_Reg8Data8(OV9655_ID,0x00,0x32);
	sccb_write_Reg8Data8(OV9655_ID,0x10,0x7b);
	sccb_write_Reg8Data8(OV9655_ID,0xbb,0xae);
	                     
                       
#if CSI_CLOCK == CSI_CLOCK_27MHZ
	sccb_write_Reg8Data8(OV9655_ID,0x11,0x40);	// Use external Clock Directly
#else
	sccb_write_Reg8Data8(OV9655_ID,0x11,0x00);
#endif
	
	if(((nWidthH == 320) &&(nWidthV == 240))||((nWidthH == 640) &&(nWidthV == 480)))
	{
		if ((nWidthH == 320) &&(nWidthV == 240))
		{
			sccb_write_Reg8Data8(OV9655_ID,0x72,0x11);
			sccb_write_Reg8Data8(OV9655_ID,0x3e,0x02);
			sccb_write_Reg8Data8(OV9655_ID,0x74,0x10);
			sccb_write_Reg8Data8(OV9655_ID,0x76,0x01);
			sccb_write_Reg8Data8(OV9655_ID,0x75,0x10);
			sccb_write_Reg8Data8(OV9655_ID,0x73,0x01);
			sccb_write_Reg8Data8(OV9655_ID,0xc7,0x81);
		}           
		else if ((nWidthH == 640) &&(nWidthV == 480))
		{
			sccb_write_Reg8Data8(OV9655_ID,0x11,0x00);
			sccb_write_Reg8Data8(OV9655_ID,0x72,0x00);
			sccb_write_Reg8Data8(OV9655_ID,0x3e,0x0c);
			sccb_write_Reg8Data8(OV9655_ID,0x74,0x3a);
			sccb_write_Reg8Data8(OV9655_ID,0x76,0x01);
			sccb_write_Reg8Data8(OV9655_ID,0x75,0x35);
			sccb_write_Reg8Data8(OV9655_ID,0x73,0x00);
			sccb_write_Reg8Data8(OV9655_ID,0xc7,0x80);
		}

		sccb_write_Reg8Data8(OV9655_ID,0xc3,0x4e);
		sccb_write_Reg8Data8(OV9655_ID,0x33,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0xa4,0x50);
		sccb_write_Reg8Data8(OV9655_ID,0xaa,0x92);
		sccb_write_Reg8Data8(OV9655_ID,0xc2,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0xc1,0xc8);
		//sccb_write_Reg8Data8(OV9655_ID,0x1e,0x04);//del by george 08272007
		sccb_write_Reg8Data8(OV9655_ID,0x1e,0x24);//enable mirror effect 
		sccb_write_Reg8Data8(OV9655_ID,0xa9,0xfa);
		sccb_write_Reg8Data8(OV9655_ID,0x0e,0x61);
		sccb_write_Reg8Data8(OV9655_ID,0x39,0x57);
		sccb_write_Reg8Data8(OV9655_ID,0x0f,0x42);
		sccb_write_Reg8Data8(OV9655_ID,0x24,0x3c);
		sccb_write_Reg8Data8(OV9655_ID,0x25,0x36);
              
		if (uFlag & FT_CSI_YUVIN)
			sccb_write_Reg8Data8(OV9655_ID,0x12,0x62);
		else        
			sccb_write_Reg8Data8(OV9655_ID,0x12,0x60);

		if ((nWidthH == 320) &&(nWidthV == 240))
		{
			sccb_write_Reg8Data8(OV9655_ID,0x03,0x02);
			sccb_write_Reg8Data8(OV9655_ID,0x32,0x12);
			sccb_write_Reg8Data8(OV9655_ID,0x17,0x18);
			sccb_write_Reg8Data8(OV9655_ID,0x18,0x04);
			sccb_write_Reg8Data8(OV9655_ID,0x19,0x01);
			sccb_write_Reg8Data8(OV9655_ID,0x1a,0x81);
			sccb_write_Reg8Data8(OV9655_ID,0x36,0x3a);
			sccb_write_Reg8Data8(OV9655_ID,0x69,0x0a);
			sccb_write_Reg8Data8(OV9655_ID,0x8c,0x80);
		} 
		else if ((nWidthH == 640) &&(nWidthV == 480))
		{
			sccb_write_Reg8Data8(OV9655_ID,0x03,0x12);
			sccb_write_Reg8Data8(OV9655_ID,0x32,0xff);
			sccb_write_Reg8Data8(OV9655_ID,0x17,0x16);
			sccb_write_Reg8Data8(OV9655_ID,0x18,0x02);
			sccb_write_Reg8Data8(OV9655_ID,0x19,0x01);
			sccb_write_Reg8Data8(OV9655_ID,0x1a,0x3d);
			sccb_write_Reg8Data8(OV9655_ID,0x36,0xfa);
			sccb_write_Reg8Data8(OV9655_ID,0x69,0x0a);
			sccb_write_Reg8Data8(OV9655_ID,0x8c,0x8d);
		}                      
	
		sccb_write_Reg8Data8(OV9655_ID,0xc0,0xaa);
		sccb_write_Reg8Data8(OV9655_ID,0x40,0xc0);
                         
		sccb_write_Reg8Data8(OV9655_ID,0xc6,0x85);
		sccb_write_Reg8Data8(OV9655_ID,0xcb,0xf0);
		sccb_write_Reg8Data8(OV9655_ID,0xcc,0xd8);
		sccb_write_Reg8Data8(OV9655_ID,0x71,0x78);
		sccb_write_Reg8Data8(OV9655_ID,0xa5,0x68);
		sccb_write_Reg8Data8(OV9655_ID,0x6f,0x9e);
		sccb_write_Reg8Data8(OV9655_ID,0x42,0xc0);
		sccb_write_Reg8Data8(OV9655_ID,0x3f,0x82);
		sccb_write_Reg8Data8(OV9655_ID,0x8a,0x23);
		sccb_write_Reg8Data8(OV9655_ID,0x14,0x3a);
		sccb_write_Reg8Data8(OV9655_ID,0x3b,0xcc);
		sccb_write_Reg8Data8(OV9655_ID,0x34,0x3d);
              
		if ((nWidthH == 320) &&(nWidthV == 240))
			sccb_write_Reg8Data8(OV9655_ID,0x41,0x41);
		else if ((nWidthH == 640) &&(nWidthV == 480))
			sccb_write_Reg8Data8(OV9655_ID,0x41,0x40);
			
		sccb_write_Reg8Data8(OV9655_ID,0xc9,0xe0);
		sccb_write_Reg8Data8(OV9655_ID,0xca,0xe8);
		sccb_write_Reg8Data8(OV9655_ID,0xcd,0x93);
		sccb_write_Reg8Data8(OV9655_ID,0x7a,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0x7b,0x1c);
		sccb_write_Reg8Data8(OV9655_ID,0x7c,0x28);
		sccb_write_Reg8Data8(OV9655_ID,0x7d,0x3c);
		sccb_write_Reg8Data8(OV9655_ID,0x7e,0x5a);
		sccb_write_Reg8Data8(OV9655_ID,0x7f,0x68);
		sccb_write_Reg8Data8(OV9655_ID,0x80,0x76);
		sccb_write_Reg8Data8(OV9655_ID,0x81,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0x82,0x88);
		sccb_write_Reg8Data8(OV9655_ID,0x83,0x8f);
		sccb_write_Reg8Data8(OV9655_ID,0x84,0x96);
		sccb_write_Reg8Data8(OV9655_ID,0x85,0xa3);
		sccb_write_Reg8Data8(OV9655_ID,0x86,0xaf);
		sccb_write_Reg8Data8(OV9655_ID,0x87,0xc4);
		sccb_write_Reg8Data8(OV9655_ID,0x88,0xd7);
		sccb_write_Reg8Data8(OV9655_ID,0x89,0xe8);
		sccb_write_Reg8Data8(OV9655_ID,0x4f,0x98);
		sccb_write_Reg8Data8(OV9655_ID,0x50,0x98);
		sccb_write_Reg8Data8(OV9655_ID,0x51,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x52,0x28);
		sccb_write_Reg8Data8(OV9655_ID,0x53,0x70);
		sccb_write_Reg8Data8(OV9655_ID,0x54,0x98);
		sccb_write_Reg8Data8(OV9655_ID,0x58,0x1a);
              
		//sccb_write(OV9655_ID,0x6b,0x5a);
		sccb_write_Reg8Data8(OV9655_ID,0x6b,0x0a);		// Bypass PLL
		
		sccb_write_Reg8Data8(OV9655_ID,0x90,0x86);
		sccb_write_Reg8Data8(OV9655_ID,0x91,0x84);
		sccb_write_Reg8Data8(OV9655_ID,0x9f,0x75);
		sccb_write_Reg8Data8(OV9655_ID,0xa0,0x73);
		sccb_write_Reg8Data8(OV9655_ID,0x16,0x24);
		sccb_write_Reg8Data8(OV9655_ID,0x2a,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x2b,0x00);

		sccb_write_Reg8Data8(OV9655_ID,0xac,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xad,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xae,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xaf,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xb2,0xf2);
		sccb_write_Reg8Data8(OV9655_ID,0xb3,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xb4,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xb6,0xaf);
		sccb_write_Reg8Data8(OV9655_ID,0xb6,0xaf);
	
		sccb_write_Reg8Data8(OV9655_ID,0x05,0x2b);
		sccb_write_Reg8Data8(OV9655_ID,0x06,0x35);
		sccb_write_Reg8Data8(OV9655_ID,0x07,0x36);
		sccb_write_Reg8Data8(OV9655_ID,0x08,0x3b);
		sccb_write_Reg8Data8(OV9655_ID,0x2d,0xf4);
		sccb_write_Reg8Data8(OV9655_ID,0x2e,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0x2f,0x35);
		sccb_write_Reg8Data8(OV9655_ID,0x4a,0xea);
		sccb_write_Reg8Data8(OV9655_ID,0x4b,0xe6);
		sccb_write_Reg8Data8(OV9655_ID,0x4c,0xe6);
		sccb_write_Reg8Data8(OV9655_ID,0x4d,0xe6);
		sccb_write_Reg8Data8(OV9655_ID,0x4e,0xe6);
		sccb_write_Reg8Data8(OV9655_ID,0x70,0x0b);
		sccb_write_Reg8Data8(OV9655_ID,0xa6,0x40);
		sccb_write_Reg8Data8(OV9655_ID,0xbc,0x04);
		sccb_write_Reg8Data8(OV9655_ID,0xbd,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0xbe,0x03);
		sccb_write_Reg8Data8(OV9655_ID,0xbf,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0xbf,0x01);
                         
		sccb_write_Reg8Data8(OV9655_ID,0x43,0x14);
		sccb_write_Reg8Data8(OV9655_ID,0x44,0xf0);
		sccb_write_Reg8Data8(OV9655_ID,0x45,0x46);
		sccb_write_Reg8Data8(OV9655_ID,0x46,0x62);
		sccb_write_Reg8Data8(OV9655_ID,0x47,0x2a);
		sccb_write_Reg8Data8(OV9655_ID,0x48,0x3c);
		sccb_write_Reg8Data8(OV9655_ID,0x59,0x85);
		sccb_write_Reg8Data8(OV9655_ID,0x5a,0xa9);
		sccb_write_Reg8Data8(OV9655_ID,0x5b,0x64);
		sccb_write_Reg8Data8(OV9655_ID,0x5c,0x84);
		sccb_write_Reg8Data8(OV9655_ID,0x5d,0x53);
		sccb_write_Reg8Data8(OV9655_ID,0x5e,0xe );
		sccb_write_Reg8Data8(OV9655_ID,0x6c,0x0c);
		sccb_write_Reg8Data8(OV9655_ID,0x6d,0x55);
		sccb_write_Reg8Data8(OV9655_ID,0x6e,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x6f,0x9e);
                         
		sccb_write_Reg8Data8(OV9655_ID,0x62,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x63,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x64,0x2 );
		sccb_write_Reg8Data8(OV9655_ID,0x65,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0x66,0x1 );
		sccb_write_Reg8Data8(OV9655_ID,0x9d,0x2 );
		sccb_write_Reg8Data8(OV9655_ID,0x9e,0x2 );
                         
		sccb_write_Reg8Data8(OV9655_ID,0x29,0x15);
		sccb_write_Reg8Data8(OV9655_ID,0xa9,0xef);
	}
	else if ((nWidthH == 1280) &&(nWidthV == 1024))
	{	// For SXGA
		sccb_write_Reg8Data8(OV9655_ID,0x13,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x00,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x01,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0x02,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0x03,0x1b);
		sccb_write_Reg8Data8(OV9655_ID,0x0e,0x61);
		sccb_write_Reg8Data8(OV9655_ID,0x0f,0x42);
		sccb_write_Reg8Data8(OV9655_ID,0x10,0xf0);
		
	//	sccb_write_Reg8Data8(OV9655_ID,0x11,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x11,0x40);	// use external clock directly

		if (uFlag & FT_CSI_YUVIN)
			sccb_write_Reg8Data8(OV9655_ID,0x12,0x02);
		else
			sccb_write_Reg8Data8(OV9655_ID,0x12,0x00);

		sccb_write_Reg8Data8(OV9655_ID,0x14,0x3a);
		sccb_write_Reg8Data8(OV9655_ID,0x16,0x24);
		sccb_write_Reg8Data8(OV9655_ID,0x17,0x1d);
		sccb_write_Reg8Data8(OV9655_ID,0x18,0xbd);
		sccb_write_Reg8Data8(OV9655_ID,0x19,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0x1a,0x81);
		sccb_write_Reg8Data8(OV9655_ID,0x1e,0x04);
		sccb_write_Reg8Data8(OV9655_ID,0x24,0x3c);
		sccb_write_Reg8Data8(OV9655_ID,0x25,0x36);
		sccb_write_Reg8Data8(OV9655_ID,0x26,0x72);
		sccb_write_Reg8Data8(OV9655_ID,0x27,0x08);
		sccb_write_Reg8Data8(OV9655_ID,0x28,0x08);
		sccb_write_Reg8Data8(OV9655_ID,0x2a,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x2b,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x2c,0x08);
		sccb_write_Reg8Data8(OV9655_ID,0x32,0xff);
		sccb_write_Reg8Data8(OV9655_ID,0x33,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x34,0x3d);
		sccb_write_Reg8Data8(OV9655_ID,0x35,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x36,0xF0);
		sccb_write_Reg8Data8(OV9655_ID,0x39,0x57);
		sccb_write_Reg8Data8(OV9655_ID,0x3a,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0x3b,0xac);
		sccb_write_Reg8Data8(OV9655_ID,0x3d,0x99);
		sccb_write_Reg8Data8(OV9655_ID,0x3e,0x0c);
		sccb_write_Reg8Data8(OV9655_ID,0x3f,0x42);
		sccb_write_Reg8Data8(OV9655_ID,0x41,0x40);
		sccb_write_Reg8Data8(OV9655_ID,0x42,0xc0);	
                         
		//sccb_write_Reg8Data8(OV9655_ID,0x6b,0x5a);
		sccb_write_Reg8Data8(OV9655_ID,0x6b,0x0a);		// Bypass PLL
                         
		sccb_write_Reg8Data8(OV9655_ID,0x71,0x78);
		sccb_write_Reg8Data8(OV9655_ID,0x72,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0x73,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0x74,0x3a);
		sccb_write_Reg8Data8(OV9655_ID,0x75,0x35);
		sccb_write_Reg8Data8(OV9655_ID,0x76,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0x77,0x02);
		sccb_write_Reg8Data8(OV9655_ID,0x7a,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0x7b,0x1C);
		sccb_write_Reg8Data8(OV9655_ID,0x7c,0x28);
		sccb_write_Reg8Data8(OV9655_ID,0x7d,0x3C);
		sccb_write_Reg8Data8(OV9655_ID,0x7e,0x5A);
		sccb_write_Reg8Data8(OV9655_ID,0x7f,0x68);
		sccb_write_Reg8Data8(OV9655_ID,0x80,0x76);
		sccb_write_Reg8Data8(OV9655_ID,0x81,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0x82,0x88);
		sccb_write_Reg8Data8(OV9655_ID,0x83,0x8f);
		sccb_write_Reg8Data8(OV9655_ID,0x84,0x96);
		sccb_write_Reg8Data8(OV9655_ID,0x85,0xa3);
		sccb_write_Reg8Data8(OV9655_ID,0x86,0xaf);
		sccb_write_Reg8Data8(OV9655_ID,0x87,0xc4);
		sccb_write_Reg8Data8(OV9655_ID,0x88,0xd7);
		sccb_write_Reg8Data8(OV9655_ID,0x89,0xe8);
		sccb_write_Reg8Data8(OV9655_ID,0x8a,0x23);
		sccb_write_Reg8Data8(OV9655_ID,0x8c,0x0d);
		sccb_write_Reg8Data8(OV9655_ID,0x90,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0x91,0x20);
                         
		sccb_write_Reg8Data8(OV9655_ID,0x9f,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xa0,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xa4,0x50);
		sccb_write_Reg8Data8(OV9655_ID,0xa5,0x68);
		sccb_write_Reg8Data8(OV9655_ID,0xa8,0xc1);
		sccb_write_Reg8Data8(OV9655_ID,0xa9,0xfa);
		sccb_write_Reg8Data8(OV9655_ID,0xaa,0x92);
		sccb_write_Reg8Data8(OV9655_ID,0xab,0x04);
		sccb_write_Reg8Data8(OV9655_ID,0xac,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xad,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xae,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xaf,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xb2,0xf2);
		sccb_write_Reg8Data8(OV9655_ID,0xb3,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xb4,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0xb5,0x52);
		sccb_write_Reg8Data8(OV9655_ID,0xb6,0xaf);
		sccb_write_Reg8Data8(OV9655_ID,0xbb,0xae);
		sccb_write_Reg8Data8(OV9655_ID,0xb5,0x00);
		sccb_write_Reg8Data8(OV9655_ID,0xc1,0xc8);
		sccb_write_Reg8Data8(OV9655_ID,0xc2,0x01);
		sccb_write_Reg8Data8(OV9655_ID,0xc3,0x4e);
		sccb_write_Reg8Data8(OV9655_ID,0xC6,0x85);
		sccb_write_Reg8Data8(OV9655_ID,0xc7,0x80);
		sccb_write_Reg8Data8(OV9655_ID,0xc9,0xe0);
		sccb_write_Reg8Data8(OV9655_ID,0xca,0xe8);
		sccb_write_Reg8Data8(OV9655_ID,0xcb,0xf0);
		sccb_write_Reg8Data8(OV9655_ID,0xcc,0xd8);
		sccb_write_Reg8Data8(OV9655_ID,0xcd,0x93);

		sccb_write_Reg8Data8(OV9655_ID,0x4f,0x98); 
		sccb_write_Reg8Data8(OV9655_ID,0x50,0x98); 
		sccb_write_Reg8Data8(OV9655_ID,0x51,0x00); 
		sccb_write_Reg8Data8(OV9655_ID,0x52,0x28); 
		sccb_write_Reg8Data8(OV9655_ID,0x53,0x70); 
		sccb_write_Reg8Data8(OV9655_ID,0x54,0x98); 
		sccb_write_Reg8Data8(OV9655_ID,0x3b,0xcc); 
              
		sccb_write_Reg8Data8(OV9655_ID,0x43,0x14);
		sccb_write_Reg8Data8(OV9655_ID,0x44,0xf0);
		sccb_write_Reg8Data8(OV9655_ID,0x45,0x46);
		sccb_write_Reg8Data8(OV9655_ID,0x46,0x62);
		sccb_write_Reg8Data8(OV9655_ID,0x47,0x2a);
		sccb_write_Reg8Data8(OV9655_ID,0x48,0x3c);
		sccb_write_Reg8Data8(OV9655_ID,0x59,0x85);
		sccb_write_Reg8Data8(OV9655_ID,0x5a,0xa9);
		sccb_write_Reg8Data8(OV9655_ID,0x5b,0x64);
		sccb_write_Reg8Data8(OV9655_ID,0x5c,0x84);
		sccb_write_Reg8Data8(OV9655_ID,0x5d,0x53);
		sccb_write_Reg8Data8(OV9655_ID,0x5e,0xe );
		sccb_write_Reg8Data8(OV9655_ID,0x6c,0x0c);
		sccb_write_Reg8Data8(OV9655_ID,0x6d,0x55);
		sccb_write_Reg8Data8(OV9655_ID,0x6e,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x6f,0x9e);
              
		sccb_write_Reg8Data8(OV9655_ID,0x62,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x63,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x64,0x2 );
		sccb_write_Reg8Data8(OV9655_ID,0x65,0x20);
		sccb_write_Reg8Data8(OV9655_ID,0x66,0x0 );
		sccb_write_Reg8Data8(OV9655_ID,0x9d,0x2 );
		sccb_write_Reg8Data8(OV9655_ID,0x9e,0x2 );
              
		sccb_write_Reg8Data8(OV9655_ID,0x29,0x15);
		sccb_write_Reg8Data8(OV9655_ID,0xa9,0xef);

		sccb_write_Reg8Data8(OV9655_ID,0x13,0xe7);
//		sccb_write_Reg8Data8(OV9655_ID,0x1e,0x34);//Flip & Mirror
	}	
	//Flip & Mirror
	sccb_write_Reg8Data8(OV9655_ID,0x1e,0x34);
				
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ			
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);		
#endif
}
#endif

#ifdef	__OV9660_DRV_C__
//====================================================================================================
//	Description:	OV9660 Initialization
//	Syntax:			void OV9660_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV9660_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	INT16S nReso;

	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif			
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	// Whether compression or not?
	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// VGA
	{
#ifdef	__TV_QVGA__
		R_CSI_TG_HRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_VRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_HWIDTH = nWidthH;					// Horizontal frame width
		R_CSI_TG_VHEIGHT = nWidthV*2;				// Vertical frame width	
#endif	// __TV_QVGA__
	}
	else
	{
		R_CSI_TG_HRATIO = 0;					
		R_CSI_TG_VRATIO = 0;
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL1 = 0x0080;
	drv_msec_wait(100);
	
	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg8Data8(OV9660_ID, 0x12, 0x80);
	sccb_delay(200);

	if((nWidthH == 1280) && (nWidthV == 1024))
	{
		//IO output                   
		sccb_write_Reg8Data8(OV9660_ID,0xd5,0xff);
		sccb_write_Reg8Data8(OV9660_ID,0xd6,0x3f);
		//Clock 24MHz 15FPS              
		sccb_write_Reg8Data8(OV9660_ID,0x3d,0x3c);
		//sccb_write_Reg8Data8(OV9660_ID,0x11,0x81);
		sccb_write_Reg8Data8(OV9660_ID,0x2a,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x2b,0x00);
		//Power control              
		//sccb_write_Reg8Data8(OV9660_ID,0x3a,0xd9);//cater
		sccb_write_Reg8Data8(OV9660_ID,0x3a,0xf1);
		sccb_write_Reg8Data8(OV9660_ID,0x3b,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x3c,0x58);
		sccb_write_Reg8Data8(OV9660_ID,0x3e,0x50);
		sccb_write_Reg8Data8(OV9660_ID,0x71,0x00);
		//Sync signal               
		sccb_write_Reg8Data8(OV9660_ID,0x15,0x00);
		//Data Format YUV            
		sccb_write_Reg8Data8(OV9660_ID,0xD7,0x10);
		sccb_write_Reg8Data8(OV9660_ID,0x6a,0x24);
		sccb_write_Reg8Data8(OV9660_ID,0x85,0xe7);
		//Sample Option            
		//sccb_write_Reg8Data8(OV9660_ID,0x6,0x0);//cater
		sccb_write_Reg8Data8(OV9660_ID,0x63,0x01);
		//Windowing                  
		sccb_write_Reg8Data8(OV9660_ID,0x17,0x0c);
		sccb_write_Reg8Data8(OV9660_ID,0x18,0x5c);
		sccb_write_Reg8Data8(OV9660_ID,0x19,0x01);
		sccb_write_Reg8Data8(OV9660_ID,0x1a,0x82);
		sccb_write_Reg8Data8(OV9660_ID,0x03,0x0f);
		sccb_write_Reg8Data8(OV9660_ID,0x2b,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x32,0x34);
		//BLC                        
		//sccb_write_Reg8Data8(OV9660_ID,0x36,0xb4);//cater
		sccb_write_Reg8Data8(OV9660_ID,0x36,0x94);
		sccb_write_Reg8Data8(OV9660_ID,0x65,0x10);
		sccb_write_Reg8Data8(OV9660_ID,0x70,0x02);
		//sccb_write_Reg8Data8(OV9660_ID,0x71,0x9c);//cater
		sccb_write_Reg8Data8(OV9660_ID,0x71,0x5c);
		sccb_write_Reg8Data8(OV9660_ID,0x64,0x24);
		//AEC;	Average;9 zone;      
		sccb_write_Reg8Data8(OV9660_ID,0x43,0x00);
		//sccb_write_Reg8Data8(OV9660_ID,0x5D,0x55);
		//sccb_write_Reg8Data8(OV9660_ID,0x5E,0x57);
		//sccb_write_Reg8Data8(OV9660_ID,0x5F,0x21);
		sccb_write_Reg8Data8(OV9660_ID,0x5c,0x00);	
		sccb_write_Reg8Data8(OV9660_ID,0x5D,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x5E,0x33);
		sccb_write_Reg8Data8(OV9660_ID,0x5F,0x20);	
		// Brightness        
		sccb_write_Reg8Data8(OV9660_ID,0x24,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0x25,0x30);
		sccb_write_Reg8Data8(OV9660_ID,0x26,0x82);
		//BF 60Hz                    
		//sccb_write_Reg8Data8(OV9660_ID,0x14,0x68);
		sccb_write_Reg8Data8(OV9660_ID,0x14,0x38);
		sccb_write_Reg8Data8(OV9660_ID,0x0C,0x32);
		//sccb_write_Reg8Data8(OV9660_ID,0x0C,0x38);
		sccb_write_Reg8Data8(OV9660_ID,0x4F,0x9E);
		sccb_write_Reg8Data8(OV9660_ID,0x50,0x84);
		sccb_write_Reg8Data8(OV9660_ID,0x5A,0x67);
		//LC enable;	Largon9306; 
		sccb_write_Reg8Data8(OV9660_ID,0x7d,0x30);
		sccb_write_Reg8Data8(OV9660_ID,0x7e,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x82,0x03);
		sccb_write_Reg8Data8(OV9660_ID,0x7f,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x83,0x07);
		sccb_write_Reg8Data8(OV9660_ID,0x80,0x03);
		sccb_write_Reg8Data8(OV9660_ID,0x81,0x04);;
		//AWB advance;	Largon9306;  
		/*
		sccb_write_Reg8Data8(OV9660_ID,0x96,0xf0);
		sccb_write_Reg8Data8(OV9660_ID,0x97,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x92,0x33);
		sccb_write_Reg8Data8(OV9660_ID,0x94,0x5a);
		sccb_write_Reg8Data8(OV9660_ID,0x93,0x3a);
		sccb_write_Reg8Data8(OV9660_ID,0x95,0x48);
		sccb_write_Reg8Data8(OV9660_ID,0x91,0xfc);
		sccb_write_Reg8Data8(OV9660_ID,0x90,0xff);
		sccb_write_Reg8Data8(OV9660_ID,0x8e,0x4e);
		sccb_write_Reg8Data8(OV9660_ID,0x8f,0x4e);
		sccb_write_Reg8Data8(OV9660_ID,0x8d,0x13);
		sccb_write_Reg8Data8(OV9660_ID,0x8c,0x0c);
		sccb_write_Reg8Data8(OV9660_ID,0x8b,0x0c);
		sccb_write_Reg8Data8(OV9660_ID,0x86,0x9e);
		sccb_write_Reg8Data8(OV9660_ID,0x87,0x11);
		sccb_write_Reg8Data8(OV9660_ID,0x88,0x22);
		sccb_write_Reg8Data8(OV9660_ID,0x89,0x05);
		sccb_write_Reg8Data8(OV9660_ID,0x8a,0x03);
		*/                   
		// simple awb 6_28   
		sccb_write_Reg8Data8(OV9660_ID,0x88,0xa2);
		sccb_write_Reg8Data8(OV9660_ID,0x86,0x9e);
		sccb_write_Reg8Data8(OV9660_ID,0x96,0xff);
		sccb_write_Reg8Data8(OV9660_ID,0x97,0x00);
		// Gamma enable              
		sccb_write_Reg8Data8(OV9660_ID,0x9b,0x0e);
		sccb_write_Reg8Data8(OV9660_ID,0x9c,0x1c);
		sccb_write_Reg8Data8(OV9660_ID,0x9d,0x34);
		sccb_write_Reg8Data8(OV9660_ID,0x9e,0x5a);
		sccb_write_Reg8Data8(OV9660_ID,0x9f,0x68);
		sccb_write_Reg8Data8(OV9660_ID,0xa0,0x76);
		sccb_write_Reg8Data8(OV9660_ID,0xa1,0x82);
		sccb_write_Reg8Data8(OV9660_ID,0xa2,0x8e);
		sccb_write_Reg8Data8(OV9660_ID,0xa3,0x98);
		sccb_write_Reg8Data8(OV9660_ID,0xa4,0xa0);
		sccb_write_Reg8Data8(OV9660_ID,0xa5,0xb0);
		sccb_write_Reg8Data8(OV9660_ID,0xa6,0xbe);
		sccb_write_Reg8Data8(OV9660_ID,0xa7,0xd2);
		sccb_write_Reg8Data8(OV9660_ID,0xa8,0xe2);
		sccb_write_Reg8Data8(OV9660_ID,0xa9,0xee);
		sccb_write_Reg8Data8(OV9660_ID,0xaa,0x18);
		//De-noise enable auto       
		sccb_write_Reg8Data8(OV9660_ID,0xAB,0xe7);
		sccb_write_Reg8Data8(OV9660_ID,0xb0,0x43);
		sccb_write_Reg8Data8(OV9660_ID,0xac,0x04);
		sccb_write_Reg8Data8(OV9660_ID,0x84,0x40);
		//Sharpness                  
		sccb_write_Reg8Data8(OV9660_ID,0xad,0x98);
		//sccb_write_Reg8Data8(OV9660_ID,0xd9,0x24);//cater
		sccb_write_Reg8Data8(OV9660_ID,0xd9,0x49);
		//sccb_write_Reg8Data8(OV9660_ID,0xda,0x00);//cater
		sccb_write_Reg8Data8(OV9660_ID,0xda,0x02);
		sccb_write_Reg8Data8(OV9660_ID,0xae,0x10);
		//Scaling                    
		sccb_write_Reg8Data8(OV9660_ID,0xab,0xe7);
		sccb_write_Reg8Data8(OV9660_ID,0xb9,0xa0);
		sccb_write_Reg8Data8(OV9660_ID,0xba,0x80);
		sccb_write_Reg8Data8(OV9660_ID,0xbb,0xa0);
		sccb_write_Reg8Data8(OV9660_ID,0xbc,0x80);
		//CMX                        
		sccb_write_Reg8Data8(OV9660_ID,0xbd,0x08);
		sccb_write_Reg8Data8(OV9660_ID,0xbe,0x19);
		sccb_write_Reg8Data8(OV9660_ID,0xbf,0x02);
		sccb_write_Reg8Data8(OV9660_ID,0xc0,0x0A);
		sccb_write_Reg8Data8(OV9660_ID,0xc1,0x2a);
		sccb_write_Reg8Data8(OV9660_ID,0xc2,0x34);
		sccb_write_Reg8Data8(OV9660_ID,0xc3,0x2d);
		sccb_write_Reg8Data8(OV9660_ID,0xc4,0x2d);
		sccb_write_Reg8Data8(OV9660_ID,0xc5,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0xc6,0x98);
		sccb_write_Reg8Data8(OV9660_ID,0xc7,0x18);
		sccb_write_Reg8Data8(OV9660_ID,0x69,0x48);
		//UVave                      
		sccb_write_Reg8Data8(OV9660_ID,0x74,0xc0);
		//UVadj                      
		//sccb_write_Reg8Data8(OV9660_ID,0x7c,0x28);//cater
		//sccb_write_Reg8Data8(OV9660_ID,0x7c,0x1d);
		sccb_write_Reg8Data8(OV9660_ID,0x65,0x11);
		sccb_write_Reg8Data8(OV9660_ID,0x66,0x00);
		//sccb_write_Reg8Data8(OV9660_ID,0x41,0xc0);
		sccb_write_Reg8Data8(OV9660_ID,0x41,0x80);
		//sccb_write_Reg8Data8(OV9660_ID,0x5b,0x24);
		//sccb_write_Reg8Data8(OV9660_ID,0x60,0x82);
		sccb_write_Reg8Data8(OV9660_ID,0x5b,0x2c);
		sccb_write_Reg8Data8(OV9660_ID,0x60,0x86);	
		//sccb_write_Reg8Data8(OV9660_ID,0x05,0x07);
		//sccb_write_Reg8Data8(OV9660_ID,0x03,0x0f);
		//sccb_write_Reg8Data8(OV9660_ID,0xd2,0x94);
		sccb_write_Reg8Data8(OV9660_ID,0x05,0x02);
		sccb_write_Reg8Data8(OV9660_ID,0x03,0x03);
		//sccb_write_Reg8Data8(OV9660_ID,0xd2,0x98);
		sccb_write_Reg8Data8(OV9660_ID,0xd2,0x9b);
		//SAT & Brightness           
		//sccb_write_Reg8Data8(OV9660_ID,0xc7,0x90);
		//sccb_write_Reg8Data8(OV9660_ID,0xc8,0x06);
		sccb_write_Reg8Data8(OV9660_ID,0xc7,0x18);
		sccb_write_Reg8Data8(OV9660_ID,0xc8,0x04);	
		sccb_write_Reg8Data8(OV9660_ID,0xcb,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0xcc,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0xcf,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0xd0,0x20);
		sccb_write_Reg8Data8(OV9660_ID,0xd1,0x00);
		//sccb_write_Reg8Data8(OV9660_ID,0xd1,0x20);
		sccb_write_Reg8Data8(OV9660_ID,0xc7,0x18);
		//BLC                        
		sccb_write_Reg8Data8(OV9660_ID,0x0d,0x82);
		sccb_write_Reg8Data8(OV9660_ID,0x0d,0x80);
	    //PE control
		sccb_write_Reg8Data8(OV9660_ID,0x13,0xe7);
		sccb_write_Reg8Data8(OV9660_ID,0x00,0x3f); //8X
		//sccb_write_Reg8Data8(OV9660_ID,0x00,0x1f);
		//sccb_write_Reg8Data8(OV9660_ID,0x2A,0x20);
		//sccb_write_Reg8Data8(OV9660_ID,0x2B,0x8f);
		//sccb_write_Reg8Data8(OV9660_ID,0x2A,0x10);
		//sccb_write_Reg8Data8(OV9660_ID,0x2B,0x47);		
		sccb_write_Reg8Data8(OV9660_ID,0x11,0x00); //8fps
		//sccb_write_Reg8Data8(OV9660_ID,0x11,0x80); //10fps
		sccb_write_Reg8Data8(OV9660_ID,0x3d,0x3c);	 
		//AEC
		//sccb_write_Reg8Data8(OV9660_ID,0x45,0x01);
		//sccb_write_Reg8Data8(OV9660_ID,0x10,0x03);
		//sccb_write_Reg8Data8(OV9660_ID,0x04,0x28);
	    sccb_write_Reg8Data8(OV9660_ID,0x45,0x01);
	    sccb_write_Reg8Data8(OV9660_ID,0x10,0x06);
	    sccb_write_Reg8Data8(OV9660_ID,0x04,0x2b);	
	    //AWB
		sccb_write_Reg8Data8(OV9660_ID,0x01,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0x02,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0x16,0x40);
		sccb_write_Reg8Data8(OV9660_ID,0xd2,0x8c);
		sccb_write_Reg8Data8(OV9660_ID,0x7c,0x28);
		sccb_write_Reg8Data8(OV9660_ID,0x65,0x11);	
		sccb_write_Reg8Data8(OV9660_ID,0x66,0x00);
		sccb_write_Reg8Data8(OV9660_ID,0x41,0xa0);
		sccb_write_Reg8Data8(OV9660_ID,0x5b,0x28);	
		sccb_write_Reg8Data8(OV9660_ID,0x60,0x84);
		sccb_write_Reg8Data8(OV9660_ID,0x05,0x07);
		sccb_write_Reg8Data8(OV9660_ID,0x03,0x43);
		//sccb_write_Reg8Data8(OV9660_ID,0x03,0x83);
		sccb_write_Reg8Data8(OV9660_ID,0x0f,0x4E);	
		sccb_write_Reg8Data8(OV9660_ID,0x71,0x5f);
		sccb_write_Reg8Data8(OV9660_ID,0x06,0x50);
		sccb_write_Reg8Data8(OV9660_ID,0x14,0x48);	
	}
	else if((nWidthH == 640) && (nWidthV == 480))
	{
		//IO output
		sccb_write_Reg8Data8(OV9660_ID, 0xd5, 0xff);
		sccb_write_Reg8Data8(OV9660_ID, 0xd6, 0x3f);
		sccb_write_Reg8Data8(OV9660_ID, 0x3d, 0x3c);//cater
		//Clock 24Mhz 15 FPS
		sccb_write_Reg8Data8(OV9660_ID, 0x11, 0x81);
		sccb_write_Reg8Data8(OV9660_ID, 0x2a, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x2b, 0x00);
		//Power control
		//sccb_write_Reg8Data8(OV9660_ID, 0x3a, 0xd9);
		sccb_write_Reg8Data8(OV9660_ID, 0x3a, 0xf1);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x3b, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x3c, 0x58);
		sccb_write_Reg8Data8(OV9660_ID, 0x3e, 0x50);
		sccb_write_Reg8Data8(OV9660_ID, 0x71, 0x00);
		//Sync signal
		sccb_write_Reg8Data8(OV9660_ID, 0x15, 0x00);
		//Data Format YUV
		sccb_write_Reg8Data8(OV9660_ID, 0xd7, 0x10);
		sccb_write_Reg8Data8(OV9660_ID, 0x6a, 0x24);
		sccb_write_Reg8Data8(OV9660_ID, 0x85, 0xe7);
		//Sample Option
		//sccb_write_Reg8Data8(OV9660_ID, 0x63, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x63, 0x01);//cater
		//Windowing
		sccb_write_Reg8Data8(OV9660_ID, 0x12, 0x40);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x4d, 0x09);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x17, 0x0c);
		sccb_write_Reg8Data8(OV9660_ID, 0x18, 0x5c);
		sccb_write_Reg8Data8(OV9660_ID, 0x19, 0x02);
		//sccb_write_Reg8Data8(OV9660_ID, 0x19, 0x01);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x1a, 0x3f);
		//sccb_write_Reg8Data8(OV9660_ID, 0x1a, 0x82);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x03, 0x03);
		//sccb_write_Reg8Data8(OV9660_ID, 0x03, 0x0f);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x32, 0xb4);	
		sccb_write_Reg8Data8(OV9660_ID, 0x2b, 0x00);
		//sccb_write_Reg8Data8(OV9660_ID, 0x32, 0x34);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0x5c, 0x80);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x5c, 0x00);//kobe
		// H V
		//sccb_write_Reg8Data8(OV9660_ID, 0x33, 0x08);
		//sccb_write_Reg8Data8(OV9660_ID, 0x04, 0xB0);
		//BLC
		sccb_write_Reg8Data8(OV9660_ID, 0x36, 0xb4);
		//sccb_write_Reg8Data8(OV9660_ID, 0x36, 0x94);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x65, 0x10);
		sccb_write_Reg8Data8(OV9660_ID, 0x70, 0x02);
		sccb_write_Reg8Data8(OV9660_ID, 0x71, 0x9f);
		sccb_write_Reg8Data8(OV9660_ID, 0x64, 0xa4);
		//sccb_write_Reg8Data8(OV9660_ID, 0x71, 0x5c);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0x64, 0x24);//cater	
		//AEC; 	
		sccb_write_Reg8Data8(OV9660_ID, 0x43, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x5d, 0x55);
		sccb_write_Reg8Data8(OV9660_ID, 0x5e, 0x57);
		sccb_write_Reg8Data8(OV9660_ID, 0x5f, 0x21);
		//Brightness
		sccb_write_Reg8Data8(OV9660_ID, 0x24, 0x3e);
		sccb_write_Reg8Data8(OV9660_ID, 0x25, 0x38);
		sccb_write_Reg8Data8(OV9660_ID, 0x26, 0x72);
		//BF 60Hz
		sccb_write_Reg8Data8(OV9660_ID, 0x14, 0x68);
		sccb_write_Reg8Data8(OV9660_ID, 0x0c, 0x38);
		sccb_write_Reg8Data8(OV9660_ID, 0x4f, 0x4f);
		sccb_write_Reg8Data8(OV9660_ID, 0x50, 0x42);
		//sccb_write_Reg8Data8(OV9660_ID, 0x4f, 0x9e);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0x50, 0x84);//cater	
		sccb_write_Reg8Data8(OV9660_ID, 0x5a, 0x67);	
		//LC enable	
		sccb_write_Reg8Data8(OV9660_ID, 0x7d, 0x30);
		sccb_write_Reg8Data8(OV9660_ID, 0x7e, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x82, 0x03);
		sccb_write_Reg8Data8(OV9660_ID, 0x7f, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x83, 0x07);	
		sccb_write_Reg8Data8(OV9660_ID, 0x80, 0x03);
		sccb_write_Reg8Data8(OV9660_ID, 0x81, 0x04);		
		//AWB     
		//sccb_write_Reg8Data8(OV9660_ID, 0x96, 0xf0);
		sccb_write_Reg8Data8(OV9660_ID, 0x96, 0xff);
		sccb_write_Reg8Data8(OV9660_ID, 0x97, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0x92, 0x33);
		sccb_write_Reg8Data8(OV9660_ID, 0x94, 0x5a);
		sccb_write_Reg8Data8(OV9660_ID, 0x93, 0x3a);	
		sccb_write_Reg8Data8(OV9660_ID, 0x95, 0x48);
		sccb_write_Reg8Data8(OV9660_ID, 0x91, 0xfc);	
		sccb_write_Reg8Data8(OV9660_ID, 0x90, 0xff);
		sccb_write_Reg8Data8(OV9660_ID, 0x8e, 0x4e);
		sccb_write_Reg8Data8(OV9660_ID, 0x8f, 0x4e);
		sccb_write_Reg8Data8(OV9660_ID, 0x8d, 0x13);
		sccb_write_Reg8Data8(OV9660_ID, 0x8c, 0x0c);	
		sccb_write_Reg8Data8(OV9660_ID, 0x8b, 0x0c);
		//sccb_write_Reg8Data8(OV9660_ID, 0x86, 0x9e);
		sccb_write_Reg8Data8(OV9660_ID, 0x86, 0x9d);//cater	
		sccb_write_Reg8Data8(OV9660_ID, 0x87, 0x11);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0x88, 0x22);
		sccb_write_Reg8Data8(OV9660_ID, 0x88, 0xa2);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x89, 0x05);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x8a, 0x03);//cater
		//Gamma enable
		sccb_write_Reg8Data8(OV9660_ID, 0x9b, 0x0e);
		sccb_write_Reg8Data8(OV9660_ID, 0x9c, 0x1c);
		sccb_write_Reg8Data8(OV9660_ID, 0x9d, 0x34);
		sccb_write_Reg8Data8(OV9660_ID, 0x9e, 0x5a);
		sccb_write_Reg8Data8(OV9660_ID, 0x9f, 0x68);	
		sccb_write_Reg8Data8(OV9660_ID, 0xa0, 0x76);
		sccb_write_Reg8Data8(OV9660_ID, 0xa1, 0x82);	
		sccb_write_Reg8Data8(OV9660_ID, 0xa2, 0x8e);
		sccb_write_Reg8Data8(OV9660_ID, 0xa3, 0x98);
		sccb_write_Reg8Data8(OV9660_ID, 0xa4, 0xa0);
		sccb_write_Reg8Data8(OV9660_ID, 0xa5, 0xb0);
		sccb_write_Reg8Data8(OV9660_ID, 0xa6, 0xbe);	
		sccb_write_Reg8Data8(OV9660_ID, 0xa7, 0xd2);
		sccb_write_Reg8Data8(OV9660_ID, 0xa8, 0xe2);	
		sccb_write_Reg8Data8(OV9660_ID, 0xa9, 0xee);
		sccb_write_Reg8Data8(OV9660_ID, 0xaa, 0x18);
		//De-noise enable auto
		sccb_write_Reg8Data8(OV9660_ID, 0xab, 0xe7);
		sccb_write_Reg8Data8(OV9660_ID, 0xb0, 0x43);	
		sccb_write_Reg8Data8(OV9660_ID, 0xac, 0x04);
		sccb_write_Reg8Data8(OV9660_ID, 0x84, 0x40);
		//Sharpness
		//sccb_write_Reg8Data8(OV9660_ID, 0xad, 0x82);
		//sccb_write_Reg8Data8(OV9660_ID, 0xd9, 0x11);	
		//sccb_write_Reg8Data8(OV9660_ID, 0xda, 0x00);
		/*
		sccb_write_Reg8Data8(OV9660_ID, 0xad, 0x98);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xd9, 0x49);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xda, 0x02);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xae, 0x10);
		*/
		sccb_write_Reg8Data8(OV9660_ID, 0xad, 0x98);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xd9, 0x60);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xda, 0x10);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0xae, 0x20);			
		//Scaling
		sccb_write_Reg8Data8(OV9660_ID, 0xab, 0xe7);
		sccb_write_Reg8Data8(OV9660_ID, 0xb9, 0x50);	
		sccb_write_Reg8Data8(OV9660_ID, 0xba, 0x3c);
		sccb_write_Reg8Data8(OV9660_ID, 0xbb, 0x50);
		sccb_write_Reg8Data8(OV9660_ID, 0xbc, 0x3c);
		//sccb_write_Reg8Data8(OV9660_ID, 0xb9, 0xa0);//cater	
		//sccb_write_Reg8Data8(OV9660_ID, 0xba, 0x80);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0xbb, 0xa0);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0xbc, 0x80);//cater	
		//CMX
		sccb_write_Reg8Data8(OV9660_ID, 0xbd, 0x08);
		sccb_write_Reg8Data8(OV9660_ID, 0xbe, 0x19);	
		sccb_write_Reg8Data8(OV9660_ID, 0xbf, 0x02);
		sccb_write_Reg8Data8(OV9660_ID, 0xc0, 0x08);
		sccb_write_Reg8Data8(OV9660_ID, 0xc1, 0x2a);				
		sccb_write_Reg8Data8(OV9660_ID, 0xc2, 0x34);
		sccb_write_Reg8Data8(OV9660_ID, 0xc3, 0x2d);	
		sccb_write_Reg8Data8(OV9660_ID, 0xc4, 0x2d);
		sccb_write_Reg8Data8(OV9660_ID, 0xc5, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0xc6, 0x98);
		sccb_write_Reg8Data8(OV9660_ID, 0xc7, 0x18);
		sccb_write_Reg8Data8(OV9660_ID, 0x69, 0x48);	
		//UVave
		sccb_write_Reg8Data8(OV9660_ID, 0x74, 0xc0);
		//UVadj	
		//sccb_write_Reg8Data8(OV9660_ID, 0x7c, 0x28);
		sccb_write_Reg8Data8(OV9660_ID, 0x7c, 0x1d);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x65, 0x11);				
		sccb_write_Reg8Data8(OV9660_ID, 0x66, 0x00);
		//sccb_write_Reg8Data8(OV9660_ID, 0x41, 0xc0);
		sccb_write_Reg8Data8(OV9660_ID, 0x41, 0x80);//cater	
		//sccb_write_Reg8Data8(OV9660_ID, 0x5b, 0x24);
		sccb_write_Reg8Data8(OV9660_ID, 0x5b, 0x2c);//cater
		sccb_write_Reg8Data8(OV9660_ID, 0x60, 0x82);
		sccb_write_Reg8Data8(OV9660_ID, 0x05, 0x07);
		sccb_write_Reg8Data8(OV9660_ID, 0x60, 0x86);
		sccb_write_Reg8Data8(OV9660_ID, 0x05, 0x02);	
		sccb_write_Reg8Data8(OV9660_ID, 0x03, 0x03);
		//sccb_write_Reg8Data8(OV9660_ID, 0xd2, 0x94);
		sccb_write_Reg8Data8(OV9660_ID, 0xd2, 0x98);//cater
		//SAT & Brightness
		sccb_write_Reg8Data8(OV9660_ID, 0xc7, 0x90);
		sccb_write_Reg8Data8(OV9660_ID, 0xc8, 0x06);
		sccb_write_Reg8Data8(OV9660_ID, 0xcb, 0x40);	
		sccb_write_Reg8Data8(OV9660_ID, 0xcc, 0x40);
		sccb_write_Reg8Data8(OV9660_ID, 0xcf, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0xd0, 0x20);
		sccb_write_Reg8Data8(OV9660_ID, 0xd1, 0x00);
		sccb_write_Reg8Data8(OV9660_ID, 0xc7, 0x18);	
		//BLC
		sccb_write_Reg8Data8(OV9660_ID, 0x0d, 0x92);
		sccb_write_Reg8Data8(OV9660_ID, 0x0d, 0x90);
		//sccb_write_Reg8Data8(OV9660_ID, 0x0d, 0x82);//cater
		//sccb_write_Reg8Data8(OV9660_ID, 0x0d, 0x80);//cater	
		//PE control
		sccb_write_Reg8Data8(OV9660_ID, 0x13, 0xe7);
		sccb_write_Reg8Data8(OV9660_ID, 0x00, 0x3f);//8X
		sccb_write_Reg8Data8(OV9660_ID, 0x11, 0x81);//7.5fps	
	    //AEC
		sccb_write_Reg8Data8(OV9660_ID, 0x45, 0x01);
		sccb_write_Reg8Data8(OV9660_ID, 0x10, 0x06);
		sccb_write_Reg8Data8(OV9660_ID, 0x04, 0x2b);    
	    //AWB
		sccb_write_Reg8Data8(OV9660_ID, 0x01, 0x40);
		sccb_write_Reg8Data8(OV9660_ID, 0x02, 0x40);
		sccb_write_Reg8Data8(OV9660_ID, 0x16, 0x40);    
		sccb_write_Reg8Data8(OV9660_ID, 0x1e, 0xf9);//WPC ON
		sccb_write_Reg8Data8(OV9660_ID, 0x83, 0x06);//LC OFF
	}
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif

#ifdef	__OV7680_DRV_C__
//====================================================================================================
//	Description:	OV7680 Initialization
//	Syntax:			void OV7680_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U nFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV7680_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U nFlag				// Flag Type
) {
}
#endif

#ifdef	__OV7670_DRV_C__

#define CSI_5FPS			0
#define CSI_7FPS			1
#define CSI_10FPS			2
#define CSI_15FPS			3
#define CSI_27FPS			4
#define	CSI_30FPS			5
#define CSI_FPS				CSI_30FPS

#define	OV7670_NIGHT_MODE	0

//====================================================================================================
//	Description:	OV7670 Initialization
//	Syntax:			void OV7670_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV7670_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	INT16S nReso;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	// Whether compression or not?
	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// VGA
	{
#ifdef	__TV_QVGA__
		R_CSI_TG_HRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_VRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_HWIDTH = nWidthH;					// Horizontal frame width
		R_CSI_TG_VHEIGHT = nWidthV*2;				// Vertical frame width	
#endif	// __TV_QVGA__
	}
	else
	{
		R_CSI_TG_HRATIO = 0;					
		R_CSI_TG_VRATIO = 0;					
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg8Data8(OV7670_ID, 0x12, 0x80);
	sccb_delay(200);

	//if((nWidthH == 640) &&(nWidthV == 480))
	{
#if 1	//VGA 15FPS
#if CSI_FPS	== CSI_30FPS
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x02);	 // 30fps
#elif CSI_FPS	== CSI_27FPS
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x04);    // 27fps	
#elif CSI_FPS	== CSI_15FPS
		//sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x06);    // 14.5fps
		//sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x09);    // 13.5fps
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x08);    // 15fps		
#elif CSI_FPS == CSI_10FPS 
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x06);    // 10fps
#elif CSI_FPS == CSI_07FPS 
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x09);    // 6.7fps
#endif				
		sccb_write_Reg8Data8(OV7670_ID, 0x3a, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x12, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x17, 0x13);
		sccb_write_Reg8Data8(OV7670_ID, 0x18, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x32, 0xb6);
		sccb_write_Reg8Data8(OV7670_ID, 0x19, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x1a, 0x7a);
		sccb_write_Reg8Data8(OV7670_ID, 0x03, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x3e, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x70, 0x3a);
		sccb_write_Reg8Data8(OV7670_ID, 0x71, 0x35);
		sccb_write_Reg8Data8(OV7670_ID, 0x72, 0x11);
		sccb_write_Reg8Data8(OV7670_ID, 0x73, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0xa2, 0x02);

		sccb_write_Reg8Data8(OV7670_ID, 0x7a, 0x24);
		sccb_write_Reg8Data8(OV7670_ID, 0x7b, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x7c, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x7d, 0x17);
		sccb_write_Reg8Data8(OV7670_ID, 0x7e, 0x32);
		sccb_write_Reg8Data8(OV7670_ID, 0x7f, 0x3f);
		sccb_write_Reg8Data8(OV7670_ID, 0x80, 0x4c);
		sccb_write_Reg8Data8(OV7670_ID, 0x81, 0x58);
		sccb_write_Reg8Data8(OV7670_ID, 0x82, 0x64);
		sccb_write_Reg8Data8(OV7670_ID, 0x83, 0x6f);
		sccb_write_Reg8Data8(OV7670_ID, 0x84, 0x7a);
		sccb_write_Reg8Data8(OV7670_ID, 0x85, 0x8c);
		sccb_write_Reg8Data8(OV7670_ID, 0x86, 0x9e);
		sccb_write_Reg8Data8(OV7670_ID, 0x87, 0xbb);
		sccb_write_Reg8Data8(OV7670_ID, 0x88, 0xd2);
		sccb_write_Reg8Data8(OV7670_ID, 0x89, 0xe5);

		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe0);
		sccb_write_Reg8Data8(OV7670_ID, 0x00, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x10, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x0d, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x14, 0x18);
		sccb_write_Reg8Data8(OV7670_ID, 0xa5, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0xab, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0x24, 0x95);
		sccb_write_Reg8Data8(OV7670_ID, 0x25, 0x33);
		sccb_write_Reg8Data8(OV7670_ID, 0x26, 0xe3);
		sccb_write_Reg8Data8(OV7670_ID, 0x9f, 0x78);
		sccb_write_Reg8Data8(OV7670_ID, 0xa0, 0x68);
		sccb_write_Reg8Data8(OV7670_ID, 0xa1, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0xa6, 0xd8);
		sccb_write_Reg8Data8(OV7670_ID, 0xa7, 0xd8);
		sccb_write_Reg8Data8(OV7670_ID, 0xa8, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0xa9, 0x90);
		sccb_write_Reg8Data8(OV7670_ID, 0xaa, 0x94);
		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe5);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x0e, 0x61);
		sccb_write_Reg8Data8(OV7670_ID, 0x0f, 0x4b);
		sccb_write_Reg8Data8(OV7670_ID, 0x16, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x1e, 0x3f); 	// Flip & Mirror
		sccb_write_Reg8Data8(OV7670_ID, 0x21, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x22, 0x91);
		sccb_write_Reg8Data8(OV7670_ID, 0x29, 0x07);
		sccb_write_Reg8Data8(OV7670_ID, 0x33, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0x35, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0x37, 0x1d);
		sccb_write_Reg8Data8(OV7670_ID, 0x38, 0x71);
		sccb_write_Reg8Data8(OV7670_ID, 0x39, 0x2a);
		sccb_write_Reg8Data8(OV7670_ID, 0x3c, 0x78);
		sccb_write_Reg8Data8(OV7670_ID, 0x4d, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x4e, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x69, 0x00);
#if CSI_FPS	== CSI_30FPS		
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x8a);	// pclk*6	
		sccb_write_Reg8Data8(OV7670_ID, 0x2d, 0x40);	//dummy byte
		sccb_write_Reg8Data8(OV7670_ID, 0x2e, 0x00);	
#elif CSI_FPS == CSI_27FPS		
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0xca);	// pclk*8
#elif CSI_FPS == CSI_15FPS
		//sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x8a);	// pclk*6
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0xca);	// pclk*8	
#elif CSI_FPS == CSI_10FPS       
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x4a);	// pclk*4	
#elif CSI_FPS == CSI_07FPS       
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x4a);
#endif		                       
		sccb_write_Reg8Data8(OV7670_ID, 0x74, 0x10);
		sccb_write_Reg8Data8(OV7670_ID, 0x8d, 0x4f);
		sccb_write_Reg8Data8(OV7670_ID, 0x8e, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x8f, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x90, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x91, 0x00);
                                 
		sccb_write_Reg8Data8(OV7670_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x9a, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0xb0, 0x84);
		sccb_write_Reg8Data8(OV7670_ID, 0xb1, 0x0c);
		sccb_write_Reg8Data8(OV7670_ID, 0xb2, 0x0e);
		sccb_write_Reg8Data8(OV7670_ID, 0xb3, 0x82);
		sccb_write_Reg8Data8(OV7670_ID, 0xb8, 0x0a);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x43, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x44, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0x45, 0x44);
		sccb_write_Reg8Data8(OV7670_ID, 0x46, 0x7a);
		sccb_write_Reg8Data8(OV7670_ID, 0x47, 0x27);
		sccb_write_Reg8Data8(OV7670_ID, 0x48, 0x3c);
		sccb_write_Reg8Data8(OV7670_ID, 0x59, 0xbc);
		sccb_write_Reg8Data8(OV7670_ID, 0x5a, 0xde);
		sccb_write_Reg8Data8(OV7670_ID, 0x5b, 0x54);
		sccb_write_Reg8Data8(OV7670_ID, 0x5c, 0x8a);
		sccb_write_Reg8Data8(OV7670_ID, 0x5d, 0x4b);
		sccb_write_Reg8Data8(OV7670_ID, 0x5e, 0x0f);
		sccb_write_Reg8Data8(OV7670_ID, 0x6c, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x6d, 0x55);
		sccb_write_Reg8Data8(OV7670_ID, 0x6e, 0x11);
		sccb_write_Reg8Data8(OV7670_ID, 0x6f, 0x9e);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x6a, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x01, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x02, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe7);

		sccb_write_Reg8Data8(OV7670_ID, 0x4f, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x50, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x51, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x52, 0x22);
		sccb_write_Reg8Data8(OV7670_ID, 0x53, 0x5e);
		sccb_write_Reg8Data8(OV7670_ID, 0x54, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x58, 0x9e);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x62, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x63, 0x8f);
		sccb_write_Reg8Data8(OV7670_ID, 0x65, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x64, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x94, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x95, 0x0c);
		sccb_write_Reg8Data8(OV7670_ID, 0x66, 0x05);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x41, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x3f, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x75, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0x76, 0xe1);
		sccb_write_Reg8Data8(OV7670_ID, 0x4c, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x77, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x3d, 0xc2);
		sccb_write_Reg8Data8(OV7670_ID, 0x4b, 0x09);
		sccb_write_Reg8Data8(OV7670_ID, 0xc9, 0x60);
		sccb_write_Reg8Data8(OV7670_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7670_ID, 0x56, 0x40);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x34, 0x11);
#if OV7670_NIGHT_MODE
		sccb_write_Reg8Data8(OV7670_ID, 0x3b, 0xca);	//enable night mode
#else
		sccb_write_Reg8Data8(OV7670_ID, 0x3b, 0x4a);	//disable  night mode
#endif
		sccb_write_Reg8Data8(OV7670_ID, 0xa4, 0x88);
		sccb_write_Reg8Data8(OV7670_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x97, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x98, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x99, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x9a, 0x84);
		sccb_write_Reg8Data8(OV7670_ID, 0x9b, 0x29);
		sccb_write_Reg8Data8(OV7670_ID, 0x9c, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0x9d, 0x98);
		sccb_write_Reg8Data8(OV7670_ID, 0x9e, 0x7f);
		sccb_write_Reg8Data8(OV7670_ID, 0x78, 0x04);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0f);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x10);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x7e);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0c);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x0f);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0d);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x09);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0xc0);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x26);
		
//		sccb_write_Reg8Data8(OV7670_ID, 0x3b, 0x00);    // 60Hz = 0x00, 50Hz = 0x08
		sccb_write_Reg8Data8(OV7670_ID, 0x3b, 0x08);    // 60Hz = 0x00, 50Hz = 0x08
//		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x4a);
//		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x0a);

/*

		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x81);    // 15fps
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x92, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x9d, 0x55);
		sccb_write_Reg8Data8(OV7670_ID, 0x9e, 0x47);
		sccb_write_Reg8Data8(OV7670_ID, 0xa5, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0xab, 0x03);
*/
#else
		sccb_write_Reg8Data8(OV7670_ID, 0x12, 0x80);
		//sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x11, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x3a, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x12, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x17, 0x13);
		sccb_write_Reg8Data8(OV7670_ID, 0x18, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x32, 0xb6);
		sccb_write_Reg8Data8(OV7670_ID, 0x19, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x1a, 0x7a);
		sccb_write_Reg8Data8(OV7670_ID, 0x03, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x3e, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x70, 0x3a);
		sccb_write_Reg8Data8(OV7670_ID, 0x71, 0x35);
		sccb_write_Reg8Data8(OV7670_ID, 0x72, 0x11);
		sccb_write_Reg8Data8(OV7670_ID, 0x73, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0xa2, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x7a, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x7b, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x7c, 0x19);
		sccb_write_Reg8Data8(OV7670_ID, 0x7d, 0x32);
		sccb_write_Reg8Data8(OV7670_ID, 0x7e, 0x5c);
		sccb_write_Reg8Data8(OV7670_ID, 0x7f, 0x6a);
		sccb_write_Reg8Data8(OV7670_ID, 0x80, 0x74);
		sccb_write_Reg8Data8(OV7670_ID, 0x81, 0x7f);
		sccb_write_Reg8Data8(OV7670_ID, 0x82, 0x88);
		sccb_write_Reg8Data8(OV7670_ID, 0x83, 0x8f);
		sccb_write_Reg8Data8(OV7670_ID, 0x84, 0x96);
		sccb_write_Reg8Data8(OV7670_ID, 0x85, 0xa3);
		sccb_write_Reg8Data8(OV7670_ID, 0x86, 0xaf);
		sccb_write_Reg8Data8(OV7670_ID, 0x87, 0xc4);
		sccb_write_Reg8Data8(OV7670_ID, 0x88, 0xd7);
		sccb_write_Reg8Data8(OV7670_ID, 0x89, 0xe8);
		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe0);
		sccb_write_Reg8Data8(OV7670_ID, 0x00, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x10, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x0d, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x42, 0x80);
		//;bright           
		sccb_write_Reg8Data8(OV7670_ID, 0x14, 0x28);
		sccb_write_Reg8Data8(OV7670_ID, 0xa5, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0xab, 0x07);
		//;bright            
		sccb_write_Reg8Data8(OV7670_ID, 0x24, 0x95);
		sccb_write_Reg8Data8(OV7670_ID, 0x25, 0x33);
		sccb_write_Reg8Data8(OV7670_ID, 0x26, 0xe3);
		sccb_write_Reg8Data8(OV7670_ID, 0x9f, 0x78);
		sccb_write_Reg8Data8(OV7670_ID, 0xa0, 0x68);
		sccb_write_Reg8Data8(OV7670_ID, 0xa1, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0xa6, 0xd8);
		sccb_write_Reg8Data8(OV7670_ID, 0xa7, 0xd8);
		sccb_write_Reg8Data8(OV7670_ID, 0xa8, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0xa9, 0x90);
		sccb_write_Reg8Data8(OV7670_ID, 0xaa, 0x94);
		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe5);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x0e, 0x61);
		sccb_write_Reg8Data8(OV7670_ID, 0x0f, 0x4b);
		sccb_write_Reg8Data8(OV7670_ID, 0x16, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x1e, 0x17);
		sccb_write_Reg8Data8(OV7670_ID, 0x21, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0x22, 0x91);
		sccb_write_Reg8Data8(OV7670_ID, 0x29, 0x07);
		sccb_write_Reg8Data8(OV7670_ID, 0x33, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0x35, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0x37, 0x1d);
		sccb_write_Reg8Data8(OV7670_ID, 0x38, 0x71);
		sccb_write_Reg8Data8(OV7670_ID, 0x39, 0x2a);
		sccb_write_Reg8Data8(OV7670_ID, 0x3c, 0x78);
		sccb_write_Reg8Data8(OV7670_ID, 0x4d, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x4e, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x69, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x6b, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x74, 0x19);
		sccb_write_Reg8Data8(OV7670_ID, 0x8d, 0x4f);
		sccb_write_Reg8Data8(OV7670_ID, 0x8e, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x8f, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x90, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x91, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x92, 0x66);
		sccb_write_Reg8Data8(OV7670_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x9a, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0xb0, 0x84);
		sccb_write_Reg8Data8(OV7670_ID, 0xb1, 0x0c);
		sccb_write_Reg8Data8(OV7670_ID, 0xb2, 0x0e);
		sccb_write_Reg8Data8(OV7670_ID, 0xb3, 0x82);
		sccb_write_Reg8Data8(OV7670_ID, 0xb8, 0x0a);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x43, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x44, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0x45, 0x34);
		sccb_write_Reg8Data8(OV7670_ID, 0x46, 0x58);
		sccb_write_Reg8Data8(OV7670_ID, 0x47, 0x28);
		sccb_write_Reg8Data8(OV7670_ID, 0x48, 0x3a);
		sccb_write_Reg8Data8(OV7670_ID, 0x59, 0x88);
		sccb_write_Reg8Data8(OV7670_ID, 0x5a, 0x88);
		sccb_write_Reg8Data8(OV7670_ID, 0x5b, 0x44);
		sccb_write_Reg8Data8(OV7670_ID, 0x5c, 0x67);
		sccb_write_Reg8Data8(OV7670_ID, 0x5d, 0x49);
		sccb_write_Reg8Data8(OV7670_ID, 0x5e, 0x0e);
		sccb_write_Reg8Data8(OV7670_ID, 0x64, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x65, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x66, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0x94, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x95, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x6c, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0x6d, 0x55);
		sccb_write_Reg8Data8(OV7670_ID, 0x6e, 0x11);
		sccb_write_Reg8Data8(OV7670_ID, 0x6f, 0x9e);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x6a, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x01, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x02, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x13, 0xe7);
		sccb_write_Reg8Data8(OV7670_ID, 0x4f, 0x73);
		sccb_write_Reg8Data8(OV7670_ID, 0x50, 0x73);
		sccb_write_Reg8Data8(OV7670_ID, 0x51, 0x04);
		sccb_write_Reg8Data8(OV7670_ID, 0x52, 0x22);
		sccb_write_Reg8Data8(OV7670_ID, 0x53, 0x5e);
		sccb_write_Reg8Data8(OV7670_ID, 0x54, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x58, 0x9a);
		sccb_write_Reg8Data8(OV7670_ID, 0x41, 0x08);
		sccb_write_Reg8Data8(OV7670_ID, 0x3f, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x75, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0x76, 0xe1);
		sccb_write_Reg8Data8(OV7670_ID, 0x4c, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x77, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x3d, 0xc2);
		sccb_write_Reg8Data8(OV7670_ID, 0x4b, 0x09);
		sccb_write_Reg8Data8(OV7670_ID, 0xc9, 0x60);
		sccb_write_Reg8Data8(OV7670_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7670_ID, 0x56, 0x40);
                        
		sccb_write_Reg8Data8(OV7670_ID, 0x34, 0x11);
		sccb_write_Reg8Data8(OV7670_ID, 0x3b, 0x1a);
		sccb_write_Reg8Data8(OV7670_ID, 0xa4, 0x89);
		sccb_write_Reg8Data8(OV7670_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x97, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x98, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x99, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x9a, 0x84);
		sccb_write_Reg8Data8(OV7670_ID, 0x9b, 0x29);
		sccb_write_Reg8Data8(OV7670_ID, 0x9c, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0x9d, 0x99);
		sccb_write_Reg8Data8(OV7670_ID, 0x9e, 0x7f);
		sccb_write_Reg8Data8(OV7670_ID, 0x78, 0x04);
                         
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0xf0);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0f);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x10);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x7e);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0a);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0b);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x01);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0c);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x0f);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x0d);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x20);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x09);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x80);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x02);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0xc0);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x03);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x40);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x05);
		sccb_write_Reg8Data8(OV7670_ID, 0xc8, 0x30);
		sccb_write_Reg8Data8(OV7670_ID, 0x79, 0x26);
		//sccb_write_Reg8Data8(OV7670_ID, 0x2d, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x2d, 0x00);
		//sccb_write_Reg8Data8(OV7670_ID, 0x2e, 0x00);
		sccb_write_Reg8Data8(OV7670_ID, 0x2e, 0x01);
#endif
	}
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif
#ifdef	__OV2655_DRV_C__
//====================================================================================================
//	Description:	OV2655 Initialization
//	Syntax:			void OV2655_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV2655_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
		
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)						// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	R_CSI_TG_HRATIO = 0;							//no scaler					
	R_CSI_TG_VRATIO = 0;

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg16Data8(OV2655_ID, 0x3012, 0x80);
	sccb_delay(200);
				
	if ((nWidthH == 320) &&(nWidthV == 240))
	{	//QVGA
		sccb_write_Reg16Data8(OV2655_ID, 0x3012, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x308c, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x308d, 0xe); 
		sccb_write_Reg16Data8(OV2655_ID, 0x360b, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x30b0, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x30b1, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x30b2, 0x4); 
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x300f, 0xa6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3010, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x3082, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x30f4, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3090, 0x3); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3091, 0xc0);
		sccb_write_Reg16Data8(OV2655_ID, 0x30ac, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x30d1, 0x8); 
		sccb_write_Reg16Data8(OV2655_ID, 0x30a8, 0x55);
		sccb_write_Reg16Data8(OV2655_ID, 0x3015, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3093, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x307e, 0xe5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3079, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x30aa, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3017, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f3, 0x83);
		sccb_write_Reg16Data8(OV2655_ID, 0x306a, 0xc); 
		sccb_write_Reg16Data8(OV2655_ID, 0x306d, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x336a, 0x3c);
		sccb_write_Reg16Data8(OV2655_ID, 0x3076, 0x6a);
		sccb_write_Reg16Data8(OV2655_ID, 0x30d9, 0x95);
		sccb_write_Reg16Data8(OV2655_ID, 0x3016, 0x82);
		sccb_write_Reg16Data8(OV2655_ID, 0x3601, 0x30);
		sccb_write_Reg16Data8(OV2655_ID, 0x304e, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f1, 0x82);
		sccb_write_Reg16Data8(OV2655_ID, 0x306f, 0x14);
		sccb_write_Reg16Data8(OV2655_ID, 0x302a, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x302b, 0x6a);
		sccb_write_Reg16Data8(OV2655_ID, 0x3012, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3011, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3013, 0xf7);
		sccb_write_Reg16Data8(OV2655_ID, 0x3018, 0x70);
		sccb_write_Reg16Data8(OV2655_ID, 0x3019, 0x60);
		sccb_write_Reg16Data8(OV2655_ID, 0x301a, 0xd4);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0x5); 
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0x6); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0x5d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x30af, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3048, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3049, 0x4e);
		sccb_write_Reg16Data8(OV2655_ID, 0x304a, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304b, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x304c, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x304d, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a3, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3013, 0xf7);
		sccb_write_Reg16Data8(OV2655_ID, 0x3014, 0x44);
		sccb_write_Reg16Data8(OV2655_ID, 0x3071, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0x5d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3073, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0x5); 
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0x6); 
		sccb_write_Reg16Data8(OV2655_ID, 0x304d, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x304a, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3095, 0x7); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3096, 0x16);
		sccb_write_Reg16Data8(OV2655_ID, 0x3097, 0x1d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3020, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3021, 0x18);
		sccb_write_Reg16Data8(OV2655_ID, 0x3022, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3023, 0x6); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3024, 0x6); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3025, 0x58);
		sccb_write_Reg16Data8(OV2655_ID, 0x3026, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3027, 0x61);
		sccb_write_Reg16Data8(OV2655_ID, 0x3088, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3089, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x308a, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x308b, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3316, 0x64);
		sccb_write_Reg16Data8(OV2655_ID, 0x3317, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3318, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3319, 0x8); 
		sccb_write_Reg16Data8(OV2655_ID, 0x331a, 0x14);
		sccb_write_Reg16Data8(OV2655_ID, 0x331b, 0xf); 
		sccb_write_Reg16Data8(OV2655_ID, 0x331c, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x331d, 0x38);
		sccb_write_Reg16Data8(OV2655_ID, 0x3100, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3320, 0xfa);
		sccb_write_Reg16Data8(OV2655_ID, 0x3321, 0x11);
		sccb_write_Reg16Data8(OV2655_ID, 0x3322, 0x92);
		sccb_write_Reg16Data8(OV2655_ID, 0x3323, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3324, 0x97);
		sccb_write_Reg16Data8(OV2655_ID, 0x3325, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3326, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x3327, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3328, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3329, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x332a, 0x58);
		sccb_write_Reg16Data8(OV2655_ID, 0x332b, 0x50);
		sccb_write_Reg16Data8(OV2655_ID, 0x332c, 0xbe);
		sccb_write_Reg16Data8(OV2655_ID, 0x332d, 0xce);
		sccb_write_Reg16Data8(OV2655_ID, 0x332e, 0x2e);
		sccb_write_Reg16Data8(OV2655_ID, 0x332f, 0x36);
		sccb_write_Reg16Data8(OV2655_ID, 0x3330, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3331, 0x44);
		sccb_write_Reg16Data8(OV2655_ID, 0x3332, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3333, 0xa); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3334, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3335, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3336, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3337, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3338, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3339, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x333a, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x333b, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3380, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3381, 0x5b);
		sccb_write_Reg16Data8(OV2655_ID, 0x3382, 0x5); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3383, 0x22);
		sccb_write_Reg16Data8(OV2655_ID, 0x3384, 0x9d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3385, 0xc0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3386, 0xb6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3387, 0xb5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3388, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3389, 0x98);
		sccb_write_Reg16Data8(OV2655_ID, 0x338a, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3340, 0x9); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3341, 0x19);
		sccb_write_Reg16Data8(OV2655_ID, 0x3342, 0x2f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3343, 0x45);
		sccb_write_Reg16Data8(OV2655_ID, 0x3344, 0x5a);
		sccb_write_Reg16Data8(OV2655_ID, 0x3345, 0x69);
		sccb_write_Reg16Data8(OV2655_ID, 0x3346, 0x75);
		sccb_write_Reg16Data8(OV2655_ID, 0x3347, 0x7e);
		sccb_write_Reg16Data8(OV2655_ID, 0x3348, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x3349, 0x96);
		sccb_write_Reg16Data8(OV2655_ID, 0x334a, 0xa3);
		sccb_write_Reg16Data8(OV2655_ID, 0x334b, 0xaf);
		sccb_write_Reg16Data8(OV2655_ID, 0x334c, 0xc4);
		sccb_write_Reg16Data8(OV2655_ID, 0x334d, 0xd7);
		sccb_write_Reg16Data8(OV2655_ID, 0x334e, 0xe8);
		sccb_write_Reg16Data8(OV2655_ID, 0x334f, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3350, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3351, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3352, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3353, 0x1e);
		sccb_write_Reg16Data8(OV2655_ID, 0x3354, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3355, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x3356, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3357, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3358, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3359, 0x1b);
		sccb_write_Reg16Data8(OV2655_ID, 0x335a, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x335b, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x335c, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x335d, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x335e, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x335f, 0x1b);
		sccb_write_Reg16Data8(OV2655_ID, 0x3360, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3361, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x3363, 0x70);
		sccb_write_Reg16Data8(OV2655_ID, 0x3364, 0x7f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3365, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3366, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3301, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x338b, 0x1b);
		sccb_write_Reg16Data8(OV2655_ID, 0x338c, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x338d, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3370, 0xd0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3371, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3372, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3373, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3374, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3375, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3376, 0x4); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3377, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3378, 0x4); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3379, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3069, 0x86);
		sccb_write_Reg16Data8(OV2655_ID, 0x307c, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3087, 0x2); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3090, 0x3); 
		sccb_write_Reg16Data8(OV2655_ID, 0x30aa, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a3, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a1, 0x41);
		sccb_write_Reg16Data8(OV2655_ID, 0x3300, 0xfc);
		sccb_write_Reg16Data8(OV2655_ID, 0x3302, 0x11);
		sccb_write_Reg16Data8(OV2655_ID, 0x3400, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3606, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3601, 0x30);
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f3, 0x83);
		sccb_write_Reg16Data8(OV2655_ID, 0x304e, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x363b, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x363c, 0xf2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3086, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3086, 0x0);
		//30FPS   
		sccb_write_Reg16Data8(OV2655_ID, 0x3011, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x302c, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3071, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0xba);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0x5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3073, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x9a);
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0x7);			
	}
	else if((nWidthH == 640) &&(nWidthV == 480))
	{	//VGA			
		sccb_write_Reg16Data8(OV2655_ID, 0x3012, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x308c, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x308d, 0xe);
		sccb_write_Reg16Data8(OV2655_ID, 0x360b, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x30b0, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x30b1, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x30b2, 0x4);
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x300f, 0xa6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3010, 0x82);
		sccb_write_Reg16Data8(OV2655_ID, 0x3082, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f4, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x3090, 0x3);
		sccb_write_Reg16Data8(OV2655_ID, 0x3091, 0xc0);
		sccb_write_Reg16Data8(OV2655_ID, 0x30ac, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x30d1, 0x8);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a8, 0x55);
		sccb_write_Reg16Data8(OV2655_ID, 0x3015, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3093, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x307e, 0xe5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3079, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x30aa, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3017, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f3, 0x83);
		sccb_write_Reg16Data8(OV2655_ID, 0x306a, 0xc);
		sccb_write_Reg16Data8(OV2655_ID, 0x306d, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x336a, 0x3c);
		sccb_write_Reg16Data8(OV2655_ID, 0x3076, 0x6a);
		sccb_write_Reg16Data8(OV2655_ID, 0x30d9, 0x95);
		sccb_write_Reg16Data8(OV2655_ID, 0x3016, 0x82);
		sccb_write_Reg16Data8(OV2655_ID, 0x3601, 0x30);
		sccb_write_Reg16Data8(OV2655_ID, 0x304e, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f1, 0x82);
		sccb_write_Reg16Data8(OV2655_ID, 0x306f, 0x14);
		sccb_write_Reg16Data8(OV2655_ID, 0x302a, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x302b, 0x6a);
		sccb_write_Reg16Data8(OV2655_ID, 0x3012, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3011, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3013, 0xf7);
		sccb_write_Reg16Data8(OV2655_ID, 0x3018, 0x70);
		sccb_write_Reg16Data8(OV2655_ID, 0x3019, 0x60);
		sccb_write_Reg16Data8(OV2655_ID, 0x301a, 0xd4);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0x5);
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0x6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0x5d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x30af, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3048, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3049, 0x4e);
		sccb_write_Reg16Data8(OV2655_ID, 0x304a, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304b, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x304c, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x304d, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a3, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3013, 0xf7);
		sccb_write_Reg16Data8(OV2655_ID, 0x3014, 0x44);
		sccb_write_Reg16Data8(OV2655_ID, 0x3071, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0x5d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3073, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0x5);
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0x6);
		sccb_write_Reg16Data8(OV2655_ID, 0x304d, 0x42);
		sccb_write_Reg16Data8(OV2655_ID, 0x304a, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x304f, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3095, 0x7);
		sccb_write_Reg16Data8(OV2655_ID, 0x3096, 0x16);
		sccb_write_Reg16Data8(OV2655_ID, 0x3097, 0x1d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3020, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x3021, 0x18);
		sccb_write_Reg16Data8(OV2655_ID, 0x3022, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3023, 0x6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3024, 0x6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3025, 0x58);
		sccb_write_Reg16Data8(OV2655_ID, 0x3026, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3027, 0x61);
		sccb_write_Reg16Data8(OV2655_ID, 0x3088, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3089, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x308a, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x308b, 0xe0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3316, 0x64);
		sccb_write_Reg16Data8(OV2655_ID, 0x3317, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3318, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3319, 0x8);
		sccb_write_Reg16Data8(OV2655_ID, 0x331a, 0x28);
		sccb_write_Reg16Data8(OV2655_ID, 0x331b, 0x1e);
		sccb_write_Reg16Data8(OV2655_ID, 0x331c, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x331d, 0x38);
		sccb_write_Reg16Data8(OV2655_ID, 0x3100, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3320, 0xfa);
		sccb_write_Reg16Data8(OV2655_ID, 0x3321, 0x11);
		sccb_write_Reg16Data8(OV2655_ID, 0x3322, 0x92);
		sccb_write_Reg16Data8(OV2655_ID, 0x3323, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x3324, 0x97);
		sccb_write_Reg16Data8(OV2655_ID, 0x3325, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3326, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x3327, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3328, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3329, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x332a, 0x58);
		sccb_write_Reg16Data8(OV2655_ID, 0x332b, 0x50);
		sccb_write_Reg16Data8(OV2655_ID, 0x332c, 0xbe);
		sccb_write_Reg16Data8(OV2655_ID, 0x332d, 0xce);
		sccb_write_Reg16Data8(OV2655_ID, 0x332e, 0x2e);
		sccb_write_Reg16Data8(OV2655_ID, 0x332f, 0x36);
		sccb_write_Reg16Data8(OV2655_ID, 0x3330, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3331, 0x44);
		sccb_write_Reg16Data8(OV2655_ID, 0x3332, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3333, 0xa);
		sccb_write_Reg16Data8(OV2655_ID, 0x3334, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3335, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3336, 0xf0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3337, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3338, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3339, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x333a, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x333b, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3380, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3381, 0x5b);
		sccb_write_Reg16Data8(OV2655_ID, 0x3382, 0x5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3383, 0x22);
		sccb_write_Reg16Data8(OV2655_ID, 0x3384, 0x9d);
		sccb_write_Reg16Data8(OV2655_ID, 0x3385, 0xc0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3386, 0xb6);
		sccb_write_Reg16Data8(OV2655_ID, 0x3387, 0xb5);
		sccb_write_Reg16Data8(OV2655_ID, 0x3388, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3389, 0x98);
		sccb_write_Reg16Data8(OV2655_ID, 0x338a, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3340, 0x9);
		sccb_write_Reg16Data8(OV2655_ID, 0x3341, 0x19);
		sccb_write_Reg16Data8(OV2655_ID, 0x3342, 0x2f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3343, 0x45);
		sccb_write_Reg16Data8(OV2655_ID, 0x3344, 0x5a);
		sccb_write_Reg16Data8(OV2655_ID, 0x3345, 0x69);
		sccb_write_Reg16Data8(OV2655_ID, 0x3346, 0x75);
		sccb_write_Reg16Data8(OV2655_ID, 0x3347, 0x7e);
		sccb_write_Reg16Data8(OV2655_ID, 0x3348, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x3349, 0x96);
		sccb_write_Reg16Data8(OV2655_ID, 0x334a, 0xa3);
		sccb_write_Reg16Data8(OV2655_ID, 0x334b, 0xaf);
		sccb_write_Reg16Data8(OV2655_ID, 0x334c, 0xc4);
		sccb_write_Reg16Data8(OV2655_ID, 0x334d, 0xd7);
		sccb_write_Reg16Data8(OV2655_ID, 0x334e, 0xe8);
		sccb_write_Reg16Data8(OV2655_ID, 0x334f, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3350, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3351, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3352, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3353, 0x1e);
		sccb_write_Reg16Data8(OV2655_ID, 0x3354, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3355, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x3356, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x3357, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x3358, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3359, 0x1b);
		sccb_write_Reg16Data8(OV2655_ID, 0x335a, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x335b, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x335c, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x335d, 0x25);
		sccb_write_Reg16Data8(OV2655_ID, 0x335e, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x335f, 0x1b);
		sccb_write_Reg16Data8(OV2655_ID, 0x3360, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3361, 0x84);
		sccb_write_Reg16Data8(OV2655_ID, 0x3363, 0x70);
		sccb_write_Reg16Data8(OV2655_ID, 0x3364, 0x7f);
		sccb_write_Reg16Data8(OV2655_ID, 0x3365, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3366, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3301, 0xff);
		sccb_write_Reg16Data8(OV2655_ID, 0x338b, 0x21);
		sccb_write_Reg16Data8(OV2655_ID, 0x338c, 0x1f);
		sccb_write_Reg16Data8(OV2655_ID, 0x338d, 0x90);
		sccb_write_Reg16Data8(OV2655_ID, 0x3370, 0xd0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3371, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3372, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3373, 0x40);
		sccb_write_Reg16Data8(OV2655_ID, 0x3374, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3375, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3376, 0x4);
		sccb_write_Reg16Data8(OV2655_ID, 0x3377, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3378, 0x4);
		sccb_write_Reg16Data8(OV2655_ID, 0x3379, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x3069, 0x86);
		sccb_write_Reg16Data8(OV2655_ID, 0x307c, 0x10);
		sccb_write_Reg16Data8(OV2655_ID, 0x3087, 0x2);
		sccb_write_Reg16Data8(OV2655_ID, 0x3090, 0x3);
		sccb_write_Reg16Data8(OV2655_ID, 0x30aa, 0x32);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a3, 0x80);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a1, 0x81);
		sccb_write_Reg16Data8(OV2655_ID, 0x3300, 0xfc);
		sccb_write_Reg16Data8(OV2655_ID, 0x3302, 0x11);
		sccb_write_Reg16Data8(OV2655_ID, 0x3400, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3606, 0x20);
		sccb_write_Reg16Data8(OV2655_ID, 0x3601, 0x30);
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x30f3, 0x83);
		sccb_write_Reg16Data8(OV2655_ID, 0x304e, 0x88);
		sccb_write_Reg16Data8(OV2655_ID, 0x363b, 0x1);
		sccb_write_Reg16Data8(OV2655_ID, 0x363c, 0xf2);
		sccb_write_Reg16Data8(OV2655_ID, 0x30a1, 0x81);
		sccb_write_Reg16Data8(OV2655_ID, 0x338b, 0x21);
		sccb_write_Reg16Data8(OV2655_ID, 0x338d, 0x90);
		sccb_write_Reg16Data8(OV2655_ID, 0x3086, 0x0);
		sccb_write_Reg16Data8(OV2655_ID, 0x3086, 0x0);
		//15FPS               
		//sccb_write_Reg16Data8(OV2655_ID, 0x3011, 0x1); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3011, 0x3);// set CLKO=SYS/2 to get 15FPS 
		sccb_write_Reg16Data8(OV2655_ID, 0x300e, 0x34);
		sccb_write_Reg16Data8(OV2655_ID, 0x302c, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3071, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3070, 0x5d);
		sccb_write_Reg16Data8(OV2655_ID, 0x301c, 0xc); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3073, 0x0); 
		sccb_write_Reg16Data8(OV2655_ID, 0x3072, 0x4d);
		sccb_write_Reg16Data8(OV2655_ID, 0x301d, 0xf); 
	}
	else
	{	//Capture Test
		//1600X1200 UXGA
		sccb_write_Reg16Data8(OV2655_ID,0x3012,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x308c,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x308d,0xe); 
		sccb_write_Reg16Data8(OV2655_ID,0x360b,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x30b0,0xff);
		sccb_write_Reg16Data8(OV2655_ID,0x30b1,0xff);
		sccb_write_Reg16Data8(OV2655_ID,0x30b2,0x4); 
		sccb_write_Reg16Data8(OV2655_ID,0x300e,0x34);
		sccb_write_Reg16Data8(OV2655_ID,0x300f,0xa6);
		sccb_write_Reg16Data8(OV2655_ID,0x3010,0x81);
		sccb_write_Reg16Data8(OV2655_ID,0x3082,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x30f4,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x3090,0x3); 
		sccb_write_Reg16Data8(OV2655_ID,0x3091,0xc0);
		sccb_write_Reg16Data8(OV2655_ID,0x30ac,0x42);
		sccb_write_Reg16Data8(OV2655_ID,0x30d1,0x8); 
		sccb_write_Reg16Data8(OV2655_ID,0x30a8,0x55);
		sccb_write_Reg16Data8(OV2655_ID,0x3015,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x3093,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x307e,0xe5);
		sccb_write_Reg16Data8(OV2655_ID,0x3079,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x30aa,0x32);
		sccb_write_Reg16Data8(OV2655_ID,0x3017,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x30f3,0x83);
		sccb_write_Reg16Data8(OV2655_ID,0x306a,0xc); 
		sccb_write_Reg16Data8(OV2655_ID,0x306d,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x336a,0x3c);
		sccb_write_Reg16Data8(OV2655_ID,0x3076,0x6a);
		sccb_write_Reg16Data8(OV2655_ID,0x30d9,0x95);
		sccb_write_Reg16Data8(OV2655_ID,0x3016,0x82);
		sccb_write_Reg16Data8(OV2655_ID,0x3601,0x30);
		sccb_write_Reg16Data8(OV2655_ID,0x304e,0x88);
		sccb_write_Reg16Data8(OV2655_ID,0x30f1,0x82);
		sccb_write_Reg16Data8(OV2655_ID,0x3011,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x3013,0xf7);
		sccb_write_Reg16Data8(OV2655_ID,0x3018,0x70);
		sccb_write_Reg16Data8(OV2655_ID,0x3019,0x60);
		sccb_write_Reg16Data8(OV2655_ID,0x301a,0xd4);
		sccb_write_Reg16Data8(OV2655_ID,0x301c,0x12);
		sccb_write_Reg16Data8(OV2655_ID,0x301d,0x16);
		sccb_write_Reg16Data8(OV2655_ID,0x3070,0x3e);
		sccb_write_Reg16Data8(OV2655_ID,0x3072,0x34);
		sccb_write_Reg16Data8(OV2655_ID,0x30af,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3048,0x1f);
		sccb_write_Reg16Data8(OV2655_ID,0x3049,0x4e);
		sccb_write_Reg16Data8(OV2655_ID,0x304a,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x304f,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x304b,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x304c,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x304d,0x42);
		sccb_write_Reg16Data8(OV2655_ID,0x304f,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x30a3,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x3013,0xf7);
		sccb_write_Reg16Data8(OV2655_ID,0x3014,0x44);
		sccb_write_Reg16Data8(OV2655_ID,0x3071,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3070,0x3e);
		sccb_write_Reg16Data8(OV2655_ID,0x3073,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3072,0x34);
		sccb_write_Reg16Data8(OV2655_ID,0x301c,0x12);
		sccb_write_Reg16Data8(OV2655_ID,0x301d,0x16);
		sccb_write_Reg16Data8(OV2655_ID,0x304d,0x42);
		sccb_write_Reg16Data8(OV2655_ID,0x304a,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x304f,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x3095,0x7); 
		sccb_write_Reg16Data8(OV2655_ID,0x3096,0x16);
		sccb_write_Reg16Data8(OV2655_ID,0x3097,0x1d);
		sccb_write_Reg16Data8(OV2655_ID,0x3020,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x3021,0x18);
		sccb_write_Reg16Data8(OV2655_ID,0x3022,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3023,0xc); 
		sccb_write_Reg16Data8(OV2655_ID,0x3024,0x6); 
		sccb_write_Reg16Data8(OV2655_ID,0x3025,0x58);
		sccb_write_Reg16Data8(OV2655_ID,0x3026,0x4); 
		sccb_write_Reg16Data8(OV2655_ID,0x3027,0xbc);
		sccb_write_Reg16Data8(OV2655_ID,0x3088,0x6); 
		sccb_write_Reg16Data8(OV2655_ID,0x3089,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x308a,0x4); 
		sccb_write_Reg16Data8(OV2655_ID,0x308b,0xb0);
		sccb_write_Reg16Data8(OV2655_ID,0x3316,0x64);
		sccb_write_Reg16Data8(OV2655_ID,0x3317,0x4b);
		sccb_write_Reg16Data8(OV2655_ID,0x3318,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x331a,0x64);
		sccb_write_Reg16Data8(OV2655_ID,0x331b,0x4b);
		sccb_write_Reg16Data8(OV2655_ID,0x331c,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3100,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3320,0xfa);
		sccb_write_Reg16Data8(OV2655_ID,0x3321,0x11);
		sccb_write_Reg16Data8(OV2655_ID,0x3322,0x92);
		sccb_write_Reg16Data8(OV2655_ID,0x3323,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x3324,0x97);
		sccb_write_Reg16Data8(OV2655_ID,0x3325,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x3326,0xff);
		sccb_write_Reg16Data8(OV2655_ID,0x3327,0x10);
		sccb_write_Reg16Data8(OV2655_ID,0x3328,0x10);
		sccb_write_Reg16Data8(OV2655_ID,0x3329,0x1f);
		sccb_write_Reg16Data8(OV2655_ID,0x332a,0x58);
		sccb_write_Reg16Data8(OV2655_ID,0x332b,0x50);
		sccb_write_Reg16Data8(OV2655_ID,0x332c,0xbe);
		sccb_write_Reg16Data8(OV2655_ID,0x332d,0xce);
		sccb_write_Reg16Data8(OV2655_ID,0x332e,0x2e);
		sccb_write_Reg16Data8(OV2655_ID,0x332f,0x36);
		sccb_write_Reg16Data8(OV2655_ID,0x3330,0x4d);
		sccb_write_Reg16Data8(OV2655_ID,0x3331,0x44);
		sccb_write_Reg16Data8(OV2655_ID,0x3332,0xf0);
		sccb_write_Reg16Data8(OV2655_ID,0x3333,0xa); 
		sccb_write_Reg16Data8(OV2655_ID,0x3334,0xf0);
		sccb_write_Reg16Data8(OV2655_ID,0x3335,0xf0);
		sccb_write_Reg16Data8(OV2655_ID,0x3336,0xf0);
		sccb_write_Reg16Data8(OV2655_ID,0x3337,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x3338,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x3339,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x333a,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x333b,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3380,0x20);
		sccb_write_Reg16Data8(OV2655_ID,0x3381,0x5b);
		sccb_write_Reg16Data8(OV2655_ID,0x3382,0x5); 
		sccb_write_Reg16Data8(OV2655_ID,0x3383,0x22);
		sccb_write_Reg16Data8(OV2655_ID,0x3384,0x9d);
		sccb_write_Reg16Data8(OV2655_ID,0x3385,0xc0);
		sccb_write_Reg16Data8(OV2655_ID,0x3386,0xb6);
		sccb_write_Reg16Data8(OV2655_ID,0x3387,0xb5);
		sccb_write_Reg16Data8(OV2655_ID,0x3388,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x3389,0x98);
		sccb_write_Reg16Data8(OV2655_ID,0x338a,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3340,0x9); 
		sccb_write_Reg16Data8(OV2655_ID,0x3341,0x19);
		sccb_write_Reg16Data8(OV2655_ID,0x3342,0x2f);
		sccb_write_Reg16Data8(OV2655_ID,0x3343,0x45);
		sccb_write_Reg16Data8(OV2655_ID,0x3344,0x5a);
		sccb_write_Reg16Data8(OV2655_ID,0x3345,0x69);
		sccb_write_Reg16Data8(OV2655_ID,0x3346,0x75);
		sccb_write_Reg16Data8(OV2655_ID,0x3347,0x7e);
		sccb_write_Reg16Data8(OV2655_ID,0x3348,0x88);
		sccb_write_Reg16Data8(OV2655_ID,0x3349,0x96);
		sccb_write_Reg16Data8(OV2655_ID,0x334a,0xa3);
		sccb_write_Reg16Data8(OV2655_ID,0x334b,0xaf);
		sccb_write_Reg16Data8(OV2655_ID,0x334c,0xc4);
		sccb_write_Reg16Data8(OV2655_ID,0x334d,0xd7);
		sccb_write_Reg16Data8(OV2655_ID,0x334e,0xe8);
		sccb_write_Reg16Data8(OV2655_ID,0x334f,0x20);
		sccb_write_Reg16Data8(OV2655_ID,0x3350,0x32);
		sccb_write_Reg16Data8(OV2655_ID,0x3351,0x25);
		sccb_write_Reg16Data8(OV2655_ID,0x3352,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x3353,0x1e);
		sccb_write_Reg16Data8(OV2655_ID,0x3354,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3355,0x84);
		sccb_write_Reg16Data8(OV2655_ID,0x3356,0x32);
		sccb_write_Reg16Data8(OV2655_ID,0x3357,0x25);
		sccb_write_Reg16Data8(OV2655_ID,0x3358,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x3359,0x1b);
		sccb_write_Reg16Data8(OV2655_ID,0x335a,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x335b,0x84);
		sccb_write_Reg16Data8(OV2655_ID,0x335c,0x32);
		sccb_write_Reg16Data8(OV2655_ID,0x335d,0x25);
		sccb_write_Reg16Data8(OV2655_ID,0x335e,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x335f,0x1b);
		sccb_write_Reg16Data8(OV2655_ID,0x3360,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3361,0x84);
		sccb_write_Reg16Data8(OV2655_ID,0x3363,0x70);
		sccb_write_Reg16Data8(OV2655_ID,0x3364,0x7f);
		sccb_write_Reg16Data8(OV2655_ID,0x3365,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3366,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3301,0xff);
		sccb_write_Reg16Data8(OV2655_ID,0x338b,0x1b);
		sccb_write_Reg16Data8(OV2655_ID,0x338c,0x1f);
		sccb_write_Reg16Data8(OV2655_ID,0x338d,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x3370,0xd0);
		sccb_write_Reg16Data8(OV2655_ID,0x3371,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3372,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3373,0x40);
		sccb_write_Reg16Data8(OV2655_ID,0x3374,0x10);
		sccb_write_Reg16Data8(OV2655_ID,0x3375,0x10);
		sccb_write_Reg16Data8(OV2655_ID,0x3376,0x4); 
		sccb_write_Reg16Data8(OV2655_ID,0x3377,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3378,0x4); 
		sccb_write_Reg16Data8(OV2655_ID,0x3379,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x3069,0x86);
		sccb_write_Reg16Data8(OV2655_ID,0x307c,0x10);
		sccb_write_Reg16Data8(OV2655_ID,0x3087,0x2); 
		sccb_write_Reg16Data8(OV2655_ID,0x3090,0x3); 
		sccb_write_Reg16Data8(OV2655_ID,0x30aa,0x32);
		sccb_write_Reg16Data8(OV2655_ID,0x30a3,0x80);
		sccb_write_Reg16Data8(OV2655_ID,0x30a1,0x41);
		sccb_write_Reg16Data8(OV2655_ID,0x3300,0xfc);
		sccb_write_Reg16Data8(OV2655_ID,0x3302,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x3400,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3606,0x20);
		sccb_write_Reg16Data8(OV2655_ID,0x3601,0x30);
		sccb_write_Reg16Data8(OV2655_ID,0x300e,0x34);
		sccb_write_Reg16Data8(OV2655_ID,0x30f3,0x83);
		sccb_write_Reg16Data8(OV2655_ID,0x304e,0x88);
		sccb_write_Reg16Data8(OV2655_ID,0x363b,0x1); 
		sccb_write_Reg16Data8(OV2655_ID,0x363c,0xf2);
		sccb_write_Reg16Data8(OV2655_ID,0x3023,0xc); 
		sccb_write_Reg16Data8(OV2655_ID,0x3319,0x4c);
		sccb_write_Reg16Data8(OV2655_ID,0x3086,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3086,0x0); 
//12.5FPS                 
		sccb_write_Reg16Data8(OV2655_ID,0x3011,0x01);
		sccb_write_Reg16Data8(OV2655_ID,0x300e,0x36); 
		sccb_write_Reg16Data8(OV2655_ID,0x302c,0x04);
		sccb_write_Reg16Data8(OV2655_ID,0x3071,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3070,0x98);
		sccb_write_Reg16Data8(OV2655_ID,0x301c,0x07); 
		sccb_write_Reg16Data8(OV2655_ID,0x3073,0x0); 
		sccb_write_Reg16Data8(OV2655_ID,0x3072,0x81); 
		sccb_write_Reg16Data8(OV2655_ID,0x301d,0x08); 
//	sccb_write_Reg16Data8(OV2655_ID,0x300f,0x08); 	//bypass OV2655 internal PLL	
	}                       
	                        
	//V Flip	              
	sccb_write_Reg16Data8(OV2655_ID, 0x307c, 0x01);
	//Mirror                
	sccb_write_Reg16Data8(OV2655_ID, 0x3090, 0x08);			
		                      
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif	
}
#endif
#ifdef	__OV3640_DRV_C__
//====================================================================================================
//	Description:	OV3640 Initialization
//	Syntax:			void OV3640_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV3640_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) 
{
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif	
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;								// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)										// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}

	R_CSI_TG_HRATIO = 0;							//no scaler
	R_CSI_TG_VRATIO = 0;

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

#if 0	
	if((nWidthH == 2048) &&(nWidthV == 1536)) {
		R_CSI_TG_HRATIO = 0;		
		R_CSI_TG_VRATIO = 0;		
		R_CSI_TG_HWIDTH	= 2048;
		R_CSI_TG_VHEIGHT = 1536;
	} else if((nWidthH == 1024) &&(nWidthV == 768)) {
		R_CSI_TG_HRATIO = 0x0102;		//from 2048 to 1024
		R_CSI_TG_VRATIO = 0x0102;		//from 1536 to 768
		R_CSI_TG_HWIDTH	= 2048;
		R_CSI_TG_VHEIGHT = 1536;
	} else if((nWidthH == 640) &&(nWidthV == 480)) {
		R_CSI_TG_HRATIO = 0x0510;		//from 2048 to 640
		R_CSI_TG_VRATIO = 0x0510;		//from 1536 to 480
		R_CSI_TG_HWIDTH	= 2048;
		R_CSI_TG_VHEIGHT = 1536;
	}
#endif

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x80);		//reset
	sccb_delay(200);

#if 0	
	//init QXGA 2048X1536
	{
		sccb_write_Reg16Data8(OV3640_ID, 0x3012,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x304d,0x45);
		sccb_write_Reg16Data8(OV3640_ID, 0x30a7,0x5e);
		sccb_write_Reg16Data8(OV3640_ID, 0x3087,0x02); 
		sccb_write_Reg16Data8(OV3640_ID, 0x309c,0x1a);
		sccb_write_Reg16Data8(OV3640_ID, 0x30a2,0xe4);
		sccb_write_Reg16Data8(OV3640_ID, 0x30aa,0x42);
		sccb_write_Reg16Data8(OV3640_ID, 0x30b0,0xff);
		sccb_write_Reg16Data8(OV3640_ID, 0x30b1,0xff);
		sccb_write_Reg16Data8(OV3640_ID, 0x30b2,0x10);
		sccb_write_Reg16Data8(OV3640_ID, 0x300e,0x32);
		sccb_write_Reg16Data8(OV3640_ID, 0x300f,0x21);
		sccb_write_Reg16Data8(OV3640_ID, 0x3010,0x20);
		sccb_write_Reg16Data8(OV3640_ID, 0x3011,0x01); 
		sccb_write_Reg16Data8(OV3640_ID, 0x304c,0x82);
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7,0x10);
		sccb_write_Reg16Data8(OV3640_ID, 0x30d9,0x0d); 
		sccb_write_Reg16Data8(OV3640_ID, 0x30db,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3016,0x82);
		sccb_write_Reg16Data8(OV3640_ID, 0x3018,0x38);
		sccb_write_Reg16Data8(OV3640_ID, 0x3019,0x30);
		sccb_write_Reg16Data8(OV3640_ID, 0x301a,0x61);
		sccb_write_Reg16Data8(OV3640_ID, 0x307d,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3087,0x02); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3082,0x20);
		sccb_write_Reg16Data8(OV3640_ID, 0x3015,0x12);
		sccb_write_Reg16Data8(OV3640_ID, 0x3014,0x0c); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3013,0xf7); //AE enable
		sccb_write_Reg16Data8(OV3640_ID, 0x303c,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x303d,0x18);
		sccb_write_Reg16Data8(OV3640_ID, 0x303e,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x303f,0x0c); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3030,0x62);
		sccb_write_Reg16Data8(OV3640_ID, 0x3031,0x26);
		sccb_write_Reg16Data8(OV3640_ID, 0x3032,0xe6);
		sccb_write_Reg16Data8(OV3640_ID, 0x3033,0x6e);
		sccb_write_Reg16Data8(OV3640_ID, 0x3034,0xea);
		sccb_write_Reg16Data8(OV3640_ID, 0x3035,0xae);
		sccb_write_Reg16Data8(OV3640_ID, 0x3036,0xa6);
		sccb_write_Reg16Data8(OV3640_ID, 0x3037,0x6a);
		sccb_write_Reg16Data8(OV3640_ID, 0x3104,0x02); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3105,0xfd);
		sccb_write_Reg16Data8(OV3640_ID, 0x3106,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3107,0xff);
		sccb_write_Reg16Data8(OV3640_ID, 0x3300,0x13);
		sccb_write_Reg16Data8(OV3640_ID, 0x3301,0xde);
		sccb_write_Reg16Data8(OV3640_ID, 0x3302,0xcf);
		sccb_write_Reg16Data8(OV3640_ID, 0x3312,0x26);
		sccb_write_Reg16Data8(OV3640_ID, 0x3314,0x42);
		sccb_write_Reg16Data8(OV3640_ID, 0x3313,0x2b);
		sccb_write_Reg16Data8(OV3640_ID, 0x3315,0x42);
		sccb_write_Reg16Data8(OV3640_ID, 0x3310,0xd0);
		sccb_write_Reg16Data8(OV3640_ID, 0x3311,0xbd);
		sccb_write_Reg16Data8(OV3640_ID, 0x330c,0x18);
		sccb_write_Reg16Data8(OV3640_ID, 0x330d,0x18);
		sccb_write_Reg16Data8(OV3640_ID, 0x330e,0x56);
		sccb_write_Reg16Data8(OV3640_ID, 0x330f,0x5c);
		sccb_write_Reg16Data8(OV3640_ID, 0x330b,0x1c);
		sccb_write_Reg16Data8(OV3640_ID, 0x3306,0x5c);
		sccb_write_Reg16Data8(OV3640_ID, 0x3307,0x11);
		sccb_write_Reg16Data8(OV3640_ID, 0x336a,0x52);
		sccb_write_Reg16Data8(OV3640_ID, 0x3370,0x46);
		sccb_write_Reg16Data8(OV3640_ID, 0x3376,0x38);
		sccb_write_Reg16Data8(OV3640_ID, 0x3300,0x13);
		sccb_write_Reg16Data8(OV3640_ID, 0x30b8,0x20);
		sccb_write_Reg16Data8(OV3640_ID, 0x30b9,0x17);
		sccb_write_Reg16Data8(OV3640_ID, 0x30ba,0x04); 
		sccb_write_Reg16Data8(OV3640_ID, 0x30bb,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3507,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x350a,0x4f);
		sccb_write_Reg16Data8(OV3640_ID, 0x3100,0x32);
		sccb_write_Reg16Data8(OV3640_ID, 0x3304,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3400,0x02); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3404,0x22);
		sccb_write_Reg16Data8(OV3640_ID, 0x3500,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3600,0xc0);
		sccb_write_Reg16Data8(OV3640_ID, 0x3610,0x60);
		sccb_write_Reg16Data8(OV3640_ID, 0x350a,0x4f);
		sccb_write_Reg16Data8(OV3640_ID, 0x3088,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3089,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x308a,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x308b,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x308d,0x04); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3086,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3086,0x00); 
                          
		//QXGA YUV            
		sccb_write_Reg16Data8(OV3640_ID, 0x3012,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3020,0x01); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3021,0x1d);
		sccb_write_Reg16Data8(OV3640_ID, 0x3022,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3023,0x0a); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3024,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3025,0x18);
		sccb_write_Reg16Data8(OV3640_ID, 0x3026,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3027,0x0c); 
		sccb_write_Reg16Data8(OV3640_ID, 0x302a,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x302b,0x20);
		sccb_write_Reg16Data8(OV3640_ID, 0x3075,0x44);
		sccb_write_Reg16Data8(OV3640_ID, 0x300d,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7,0x10);
		sccb_write_Reg16Data8(OV3640_ID, 0x3069,0x44);
		sccb_write_Reg16Data8(OV3640_ID, 0x303e,0x01); 
		sccb_write_Reg16Data8(OV3640_ID, 0x303f,0x80);
		sccb_write_Reg16Data8(OV3640_ID, 0x3302,0xef);
		sccb_write_Reg16Data8(OV3640_ID, 0x335f,0x68);
		sccb_write_Reg16Data8(OV3640_ID, 0x3360,0x18);
		sccb_write_Reg16Data8(OV3640_ID, 0x3361,0x0c); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3362,0x68);
		sccb_write_Reg16Data8(OV3640_ID, 0x3363,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3364,0x04); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3403,0x42);
		sccb_write_Reg16Data8(OV3640_ID, 0x3088,0x08); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3089,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x308a,0x06); 
		sccb_write_Reg16Data8(OV3640_ID, 0x308b,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3100,0x02); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3301,0xde);
		sccb_write_Reg16Data8(OV3640_ID, 0x3304,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3400,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x3404,0x00); 
		sccb_write_Reg16Data8(OV3640_ID, 0x304c,0x81);
		sccb_write_Reg16Data8(OV3640_ID, 0x3011,0x5);                                          
	}
	//V Flip & Mirror
	sccb_write_Reg16Data8(OV3640_ID, 0x307c, 0x13); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x09); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3090, 0xc9); 
		
#else
	//Init ov3640
	sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x90);	// [7]:Reset; [6:4]=001->XGA mode
	sccb_write_Reg16Data8(OV3640_ID, 0x30a9, 0xdb);	// for 1.5V
	sccb_write_Reg16Data8(OV3640_ID, 0x304d, 0x45);
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x16);
	sccb_write_Reg16Data8(OV3640_ID, 0x309c, 0x1a);
	sccb_write_Reg16Data8(OV3640_ID, 0x30a2, 0xe4);
	sccb_write_Reg16Data8(OV3640_ID, 0x30aa, 0x42);
	sccb_write_Reg16Data8(OV3640_ID, 0x30b0, 0xff);
	sccb_write_Reg16Data8(OV3640_ID, 0x30b1, 0xff);
	sccb_write_Reg16Data8(OV3640_ID, 0x30b2, 0x10);
	sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x32);
	sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0x21);
	sccb_write_Reg16Data8(OV3640_ID, 0x3010, 0x20);
	sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x01);
	sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);
	sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x10);
	sccb_write_Reg16Data8(OV3640_ID, 0x30d9, 0x0d);
	sccb_write_Reg16Data8(OV3640_ID, 0x30db, 0x08);
	sccb_write_Reg16Data8(OV3640_ID, 0x3016, 0x82);
	sccb_write_Reg16Data8(OV3640_ID, 0x3018, 0x48);	// Luminance High Range=72 after Gamma=0x86=134; 0x40->134
	sccb_write_Reg16Data8(OV3640_ID, 0x3019, 0x40);	// Luminance Low Range=64 after Gamma=0x8f=143; 0x38->125
	sccb_write_Reg16Data8(OV3640_ID, 0x301a, 0x82);
	sccb_write_Reg16Data8(OV3640_ID, 0x307d, 0x00);
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x02);
	sccb_write_Reg16Data8(OV3640_ID, 0x3082, 0x20);
	sccb_write_Reg16Data8(OV3640_ID, 0x3070, 0x00);	// 50Hz Banding MSB
	sccb_write_Reg16Data8(OV3640_ID, 0x3071, 0xbb);	// 50Hz Banding LSB
	sccb_write_Reg16Data8(OV3640_ID, 0x3072, 0x00);	// 60Hz Banding MSB
	sccb_write_Reg16Data8(OV3640_ID, 0x3073, 0xa6);	// 60Hz Banding LSB
	sccb_write_Reg16Data8(OV3640_ID, 0x301c, 0x07);	// max_band_step_50hz
	sccb_write_Reg16Data8(OV3640_ID, 0x301d, 0x08);	// max_band_step_60hz
	sccb_write_Reg16Data8(OV3640_ID, 0x3015, 0x12);	// [6:4]:1 dummy frame; [2:0]:AGC gain 8x
	sccb_write_Reg16Data8(OV3640_ID, 0x3014, 0x84);	// [7]:50hz; [6]:auto banding detection disable; [3]:night modedisable
	sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);	// AE_en
	sccb_write_Reg16Data8(OV3640_ID, 0x3030, 0x11);	// Avg_win_Weight0
	sccb_write_Reg16Data8(OV3640_ID, 0x3031, 0x11);	// Avg_win_Weight1
	sccb_write_Reg16Data8(OV3640_ID, 0x3032, 0x11);	// Avg_win_Weight2
	sccb_write_Reg16Data8(OV3640_ID, 0x3033, 0x11);	// Avg_win_Weight3
	sccb_write_Reg16Data8(OV3640_ID, 0x3034, 0x11);	// Avg_win_Weight4
	sccb_write_Reg16Data8(OV3640_ID, 0x3035, 0x11);	// Avg_win_Weight5
	sccb_write_Reg16Data8(OV3640_ID, 0x3036, 0x11);	// Avg_win_Weight6
	sccb_write_Reg16Data8(OV3640_ID, 0x3037, 0x11);	// Avg_win_Weight7
	sccb_write_Reg16Data8(OV3640_ID, 0x3038, 0x01);	// Avg_Win_Hstart=285
	sccb_write_Reg16Data8(OV3640_ID, 0x3039, 0x1d);	// Avg_Win_Hstart=285
	sccb_write_Reg16Data8(OV3640_ID, 0x303a, 0x00);	// Avg_Win_Vstart=10
	sccb_write_Reg16Data8(OV3640_ID, 0x303b, 0x0a);	// Avg_Win_Vstart=10
	sccb_write_Reg16Data8(OV3640_ID, 0x303c, 0x02);	// Avg_Win_Width=512x4=2048
	sccb_write_Reg16Data8(OV3640_ID, 0x303d, 0x00);	// Avg_Win_Width=512x4=2048
	sccb_write_Reg16Data8(OV3640_ID, 0x303e, 0x01);	// Avg_Win_Height=384x4=1536
	sccb_write_Reg16Data8(OV3640_ID, 0x303f, 0x80);	// Avg_Win_Height=384x4=1536
	sccb_write_Reg16Data8(OV3640_ID, 0x3047, 0x00);	// [7]:avg_based AE
	sccb_write_Reg16Data8(OV3640_ID, 0x30b8, 0x20);
	sccb_write_Reg16Data8(OV3640_ID, 0x30b9, 0x17);
	sccb_write_Reg16Data8(OV3640_ID, 0x30ba, 0x04);
	sccb_write_Reg16Data8(OV3640_ID, 0x30bb, 0x08);
	sccb_write_Reg16Data8(OV3640_ID, 0x30a9, 0xdb);	// for 1.5V

	sccb_write_Reg16Data8(OV3640_ID, 0x3104, 0x02);
	sccb_write_Reg16Data8(OV3640_ID, 0x3105, 0xfd);
	sccb_write_Reg16Data8(OV3640_ID, 0x3106, 0x00);
	sccb_write_Reg16Data8(OV3640_ID, 0x3107, 0xff);
	sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x02);

	sccb_write_Reg16Data8(OV3640_ID, 0x3300, 0x13);	// [0]: LENC disable; [1]: AF enable
	sccb_write_Reg16Data8(OV3640_ID, 0x3301, 0xde);	// [1]: BC_en; [2]: WC_en; [4]: CMX_en
	sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xcf);	// [0]: AWB_en; [1]: AWB_gain_en; [2]: Gamma_en; [7]: Special_Effect_en
	sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0xfc);	// [4]: Add bias to gamma result; [5]: Enable Gamma bias function
	sccb_write_Reg16Data8(OV3640_ID, 0x3306, 0x5c);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3307, 0x11);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3308, 0x00);	// [7]: AWB_mode=advanced
	sccb_write_Reg16Data8(OV3640_ID, 0x330b, 0x1c);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x330c, 0x18);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x330d, 0x18);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x330e, 0x56);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x330f, 0x5c);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3310, 0xd0);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3311, 0xbd);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3312, 0x26);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3313, 0x2b);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3314, 0x42);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3315, 0x42);	// Reserved ???
	sccb_write_Reg16Data8(OV3640_ID, 0x331b, 0x09);	// Gamma YST1
	sccb_write_Reg16Data8(OV3640_ID, 0x331c, 0x18);	// Gamma YST2
	sccb_write_Reg16Data8(OV3640_ID, 0x331d, 0x30);	// Gamma YST3
	sccb_write_Reg16Data8(OV3640_ID, 0x331e, 0x58);	// Gamma YST4
	sccb_write_Reg16Data8(OV3640_ID, 0x331f, 0x66);	// Gamma YST5
	sccb_write_Reg16Data8(OV3640_ID, 0x3320, 0x72);	// Gamma YST6
	sccb_write_Reg16Data8(OV3640_ID, 0x3321, 0x7d);	// Gamma YST7
	sccb_write_Reg16Data8(OV3640_ID, 0x3322, 0x86);	// Gamma YST8
	sccb_write_Reg16Data8(OV3640_ID, 0x3323, 0x8f);	// Gamma YST9
	sccb_write_Reg16Data8(OV3640_ID, 0x3324, 0x97);	// Gamma YST10
	sccb_write_Reg16Data8(OV3640_ID, 0x3325, 0xa5);	// Gamma YST11
	sccb_write_Reg16Data8(OV3640_ID, 0x3326, 0xb2);	// Gamma YST12
	sccb_write_Reg16Data8(OV3640_ID, 0x3327, 0xc7);	// Gamma YST13
	sccb_write_Reg16Data8(OV3640_ID, 0x3328, 0xd8);	// Gamma YST14
	sccb_write_Reg16Data8(OV3640_ID, 0x3329, 0xe8);	// Gamma YST15
	sccb_write_Reg16Data8(OV3640_ID, 0x332a, 0x20);	// Gamma YSLP15
	sccb_write_Reg16Data8(OV3640_ID, 0x332b, 0x00);	// [3]: WB_mode=auto
	sccb_write_Reg16Data8(OV3640_ID, 0x332d, 0x64);	// [6]:de-noise auto mode; [5]:edge auto mode; [4:0]:edge threshold
	sccb_write_Reg16Data8(OV3640_ID, 0x3355, 0x06);	// Special_Effect_CTRL: [1]:Sat_en; [2]: Cont_Y_en
	sccb_write_Reg16Data8(OV3640_ID, 0x3358, 0x40);	// Special_Effect_Sat_U
	sccb_write_Reg16Data8(OV3640_ID, 0x3359, 0x40);	// Special_Effect_Sat_V
	sccb_write_Reg16Data8(OV3640_ID, 0x336a, 0x52);	// LENC R_A1
	sccb_write_Reg16Data8(OV3640_ID, 0x3370, 0x46);	// LENC G_A1
	sccb_write_Reg16Data8(OV3640_ID, 0x3376, 0x38);	// LENC B_A1

	sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x00);	// [2:0];Format input source=DSP TUV444
	sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);	// DVP Win Addr
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, 0x00);	// [5:0]: yuyv

	sccb_write_Reg16Data8(OV3640_ID, 0x3507, 0x06);	// ???
	sccb_write_Reg16Data8(OV3640_ID, 0x350a, 0x4f);	// ???

	sccb_write_Reg16Data8(OV3640_ID, 0x3600, 0xc0);	// VSYNC_CTRL

	sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xcf);	// [0]: AWB_enable
	sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x01);	// PCLK/2
	sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x10);	// [6:4]=001->XGA mode
	sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);	// AE_enable
	sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);	// HS=285
	sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);	// HS=285
	sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);	// VS = 6
	sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x06);	// VS = 6
	sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);	// HW=2072
	sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);	// HW=2072
	sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x03);	// VW=772
	sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x04);	// VW=772
	sccb_write_Reg16Data8(OV3640_ID, 0x3028, 0x09);	// HTotalSize=2375
	sccb_write_Reg16Data8(OV3640_ID, 0x3029, 0x47);	// HTotalSize=2375
	sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x03);	// VTotalSize=784
	sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x10);	// VTotalSize=784
	sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);
	sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x24);	// VSYNCOPT
	sccb_write_Reg16Data8(OV3640_ID, 0x3086, 0x00);	// Sleep/Wakeup
	sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x04);	// x_output_size=1024
	sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x00);	// x_output_size=1024
	sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x03);	// y_output_size=768
	sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0x00);	// y_output_size=768
	sccb_write_Reg16Data8(OV3640_ID, 0x308d, 0x04);
	sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x90);	// ???
	sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);	// [5]: Scale_en, [0]: AWB_enable
	sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x34);	// Scale_VH_in
	sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x0c);	// Scale_H_in = 0x40c = 1036
	sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x04);	// Scale_V_in = 0x304 = 772
	sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x34);	// Scale_VH_out
	sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x08);	// Scale_H_out = 0x408 = 1032
	sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0x04);	// Scale_V_out = 0x304 = 772
	sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x32);
	sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0x21);
	sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);	// for 30 FPS
	sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);

	//V Flip & Mirror
	sccb_write_Reg16Data8(OV3640_ID, 0x307c, 0x13); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x09); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3090, 0xc9); 
	
	if(nWidthH == 640 && nWidthV == 480){
		//set output size to 640x480	
		sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x06);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x24);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x90);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3069, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x34);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x0c);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x12);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x88);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0xe4);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);   //

		sccb_write_Reg16Data8(OV3640_ID, 0x302c, 0x0e);   //EXHTS
		sccb_write_Reg16Data8(OV3640_ID, 0x302d, 0x00);   //EXVTS[15:8]
		sccb_write_Reg16Data8(OV3640_ID, 0x302e, 0x10);   //EXVTS[7:0]

		sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x02);   // x_output_size=640
		sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x80);   // x_output_size=640
		sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x01);   // y_output_size=480
		sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0xe0);   // y_output_size=480
		
		sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x32);
		sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0x21);
		sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);	// for 30 FPS
		sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);
	} else if(nWidthH == 1024 && nWidthV == 768) {	
		//set output size to 1024x768
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xcf);	// [0]: AWB_enable
		sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x01);	// PCLK/2
		sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x10);	// [6:4]=001->XGA mode
		sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);	// AE_enable
		sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);	// HS=285
		sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);	// HS=285
		sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);	// VS = 6
		sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x06);	// VS = 6
		sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);	// HW=2072
		sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);	// HW=2072
		sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x03);	// VW=772
		sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x04);	// VW=772
		sccb_write_Reg16Data8(OV3640_ID, 0x3028, 0x09);	// HTotalSize=2375
		sccb_write_Reg16Data8(OV3640_ID, 0x3029, 0x47);	// HTotalSize=2375
		sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x03);	// VTotalSize=784
		sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x10);	// VTotalSize=784
		sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);
		sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x24);	// VSYNCOPT
		sccb_write_Reg16Data8(OV3640_ID, 0x3086, 0x00);	// Sleep/Wakeup
		sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x04);	// x_output_size=1024
		sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x00);	// x_output_size=1024
		sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x03);	// y_output_size=768
		sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0x00);	// y_output_size=768
		sccb_write_Reg16Data8(OV3640_ID, 0x308d, 0x04);
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x90);	// ???
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);	// [5]: Scale_en, [0]: AWB_enable
		sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x34);	// Scale_VH_in
		sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x0c);	// Scale_H_in = 0x40c = 1036
		sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x04);	// Scale_V_in = 0x304 = 772
		sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x34);	// Scale_VH_out
		sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x08);	// Scale_H_out = 0x408 = 1032
		sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0x04);	// Scale_V_out = 0x304 = 772
		sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x32);
		sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0x21);
		sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);	// for 30 FPS
		sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x82);
	} else { 
		//set output size to 2048*1536
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xce);	//[0]: AWB_disable
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xcf); //[0]: AWB_enable 0401add
		sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x00);	// PCLK/1
		sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x39);
		sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0x21);
		sccb_write_Reg16Data8(OV3640_ID, 0x3010, 0x20);
		sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);
		sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x00);	// [6:4]=000->QXGA mode
		sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf2);	// AE_disable
		sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);	// AE_enable 0401add
		sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);	// HS=285
		sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);	// HS=285
		sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);	// VS = 10
		sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x0a);	// VS = 10
		sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);	// HW=2072
		sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);	// HW=2072
		sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x06);	// VW=1548
		sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x0c);	// VW=1548
		sccb_write_Reg16Data8(OV3640_ID, 0x3028, 0x09);	// HTotalSize=2375
		sccb_write_Reg16Data8(OV3640_ID, 0x3029, 0x47);	// HTotalSize=2375
		sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x06);	// VTotalSize=1568
		sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x20);	// VTotalSize=1568
		sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x81);	// ???
		sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x44);	// VSYNCOPT
		sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x08);	// x_output_size=2048
		sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x00);	// x_output_size=2048
		sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x06);	// y_output_size=1536
		sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0x00);	// y_output_size=1536
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x10);	// ???
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xee);	// [5]: Scale_en, [0]: AWB_disable
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);	// [5]: Scale_en, [0]: AWB_enable 0401add
		sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x68);	// Scale_VH_in
		sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x18);	// Scale_H_in = 0x818 = 2072
		sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x0c);	// Scale_V_in = 0x60c = 1548
		sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x68);	// Scale_VH_out
		sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x08);	// Scale_H_out = 0x808 = 2056
		sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0x04);	// Scale_V_out = 0x604 = 1540
	}
#endif

	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif
#ifdef	__OV5642_DRV_C__
//====================================================================================================
//	Description:	OV5642 Initialization
//	Syntax:			void OV5642_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV5642_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
#else
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}

#if 0	
	// Whether compression or not?
	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// VGA
	{
#ifdef	__TV_QVGA__
		R_CSI_TG_HRATIO =0x0101;					// Scale to 1/2
		R_CSI_TG_VRATIO =0x0101;					// Scale to 1/2
//		R_CSI_TG_HWIDTH = nWidthH;					// Horizontal frame width
//		R_CSI_TG_VHEIGHT = nWidthV*2;				// Vertical frame width	
#endif	// __TV_QVGA__
	}
#endif

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable


	// CMOS Sensor Initialization Start...
	sccb_init();
//	sccb_delay (200);
//	sccb_write_Reg16Data8(OV5642_ID, 0x3012, 0x80);
//	sccb_delay (200);

	if((nWidthH == 320) && (nWidthV == 240)) 
	{
		//QVGA
		sccb_write_Reg16Data8(OV5642_ID, 0x3103, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3008, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3017, 0x7f);
		sccb_write_Reg16Data8(OV5642_ID, 0x3018, 0xfc);
		sccb_write_Reg16Data8(OV5642_ID, 0x3810, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3615, 0xf0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3000, 0xf8);
		sccb_write_Reg16Data8(OV5642_ID, 0x3001, 0x48);
		sccb_write_Reg16Data8(OV5642_ID, 0x3002, 0x5c);
		sccb_write_Reg16Data8(OV5642_ID, 0x3003, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3000, 0xf8);
		sccb_write_Reg16Data8(OV5642_ID, 0x3001, 0x48);
		sccb_write_Reg16Data8(OV5642_ID, 0x3002, 0x5c);
		sccb_write_Reg16Data8(OV5642_ID, 0x3003, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3004, 0xff);
		sccb_write_Reg16Data8(OV5642_ID, 0x3005, 0xb7);
		sccb_write_Reg16Data8(OV5642_ID, 0x3006, 0x43);
		sccb_write_Reg16Data8(OV5642_ID, 0x3007, 0x37);
		sccb_write_Reg16Data8(OV5642_ID, 0x3011, 0x9); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3012, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3010, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x460c, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x3815, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x370d, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x370c, 0xa0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3602, 0xfc);
		sccb_write_Reg16Data8(OV5642_ID, 0x3612, 0xff);
		sccb_write_Reg16Data8(OV5642_ID, 0x3634, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3613, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3605, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3621, 0xc7);
		sccb_write_Reg16Data8(OV5642_ID, 0x3622, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3604, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x3603, 0x27);
		sccb_write_Reg16Data8(OV5642_ID, 0x3603, 0x27);
		sccb_write_Reg16Data8(OV5642_ID, 0x4000, 0x21);
		sccb_write_Reg16Data8(OV5642_ID, 0x401d, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3600, 0x54);
		sccb_write_Reg16Data8(OV5642_ID, 0x3605, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3606, 0x3f);
		sccb_write_Reg16Data8(OV5642_ID, 0x3c01, 0x80);
		sccb_write_Reg16Data8(OV5642_ID, 0x5000, 0x4f);
		sccb_write_Reg16Data8(OV5642_ID, 0x5020, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5181, 0x79);
		sccb_write_Reg16Data8(OV5642_ID, 0x5182, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5185, 0x22);
		sccb_write_Reg16Data8(OV5642_ID, 0x5197, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5001, 0x7f);
		sccb_write_Reg16Data8(OV5642_ID, 0x5500, 0xa); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5504, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5505, 0x7f);
		sccb_write_Reg16Data8(OV5642_ID, 0x5080, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x300e, 0x18);
		sccb_write_Reg16Data8(OV5642_ID, 0x4610, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x471d, 0x5); 
		sccb_write_Reg16Data8(OV5642_ID, 0x4708, 0x6); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3710, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x3632, 0x41);
		sccb_write_Reg16Data8(OV5642_ID, 0x3702, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x3620, 0x52);
		sccb_write_Reg16Data8(OV5642_ID, 0x3631, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3808, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3809, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x380a, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380b, 0xf0);
		sccb_write_Reg16Data8(OV5642_ID, 0x380e, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380f, 0xe8);
		sccb_write_Reg16Data8(OV5642_ID, 0x501f, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5000, 0x4f);
		sccb_write_Reg16Data8(OV5642_ID, 0x4300, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x3503, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3501, 0x3e);
		sccb_write_Reg16Data8(OV5642_ID, 0x3502, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x350b, 0x12);
		sccb_write_Reg16Data8(OV5642_ID, 0x3503, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3824, 0x11);
		sccb_write_Reg16Data8(OV5642_ID, 0x3501, 0x3e);
		sccb_write_Reg16Data8(OV5642_ID, 0x3502, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x350b, 0x12);
		sccb_write_Reg16Data8(OV5642_ID, 0x380c, 0x7); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380d, 0xa); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380e, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380f, 0xe8);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0d, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0e, 0x3); 
//	sccb_write_Reg16Data8(OV5642_ID, 0x3818, 0xc1);
		sccb_write_Reg16Data8(OV5642_ID, 0x3818, 0xa1);	//VFlip set/clear 0x3818[5] 
		sccb_write_Reg16Data8(OV5642_ID, 0x3705, 0xdb);
		sccb_write_Reg16Data8(OV5642_ID, 0x370a, 0x81);
		sccb_write_Reg16Data8(OV5642_ID, 0x3801, 0x50);
//	sccb_write_Reg16Data8(OV5642_ID, 0x3621, 0xc7);	
		sccb_write_Reg16Data8(OV5642_ID, 0x3621, 0xe7);	//Mirror
		sccb_write_Reg16Data8(OV5642_ID, 0x3801, 0x50);
		sccb_write_Reg16Data8(OV5642_ID, 0x3803, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3827, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3810, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3804, 0x5); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3805, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5682, 0x5); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5683, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3806, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3807, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x5686, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5687, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a00, 0x78);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a1a, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a13, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a18, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a19, 0x7c);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a08, 0x12);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a09, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0a, 0xf); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0b, 0xa0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3004, 0xff);
		sccb_write_Reg16Data8(OV5642_ID, 0x350c, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x350d, 0xe8);
		sccb_write_Reg16Data8(OV5642_ID, 0x3500, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3501, 0x3e);
		sccb_write_Reg16Data8(OV5642_ID, 0x3502, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x350a, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x350b, 0x12);
		sccb_write_Reg16Data8(OV5642_ID, 0x3503, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x528a, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x528b, 0x6); 
		sccb_write_Reg16Data8(OV5642_ID, 0x528c, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x528d, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x528e, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x528f, 0x50);
		sccb_write_Reg16Data8(OV5642_ID, 0x5290, 0x60);
		sccb_write_Reg16Data8(OV5642_ID, 0x5292, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5293, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5294, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5295, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5296, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5297, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5298, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5299, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x529a, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x529b, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x529c, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x529d, 0x28);
		sccb_write_Reg16Data8(OV5642_ID, 0x529e, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x529f, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0f, 0x3c);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a10, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a1b, 0x3c);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a1e, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a11, 0x70);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a1f, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x3030, 0x2b);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a02, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a03, 0x7d);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a04, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a14, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a15, 0x7d);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a16, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a00, 0x78);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a08, 0x12);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a09, 0xc0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0a, 0xf); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0b, 0xa0);
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0d, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3a0e, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5193, 0x70);
		sccb_write_Reg16Data8(OV5642_ID, 0x3620, 0x52);
		sccb_write_Reg16Data8(OV5642_ID, 0x3703, 0xb2);
		sccb_write_Reg16Data8(OV5642_ID, 0x3704, 0x18);
		sccb_write_Reg16Data8(OV5642_ID, 0x589b, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x589a, 0xc5);
		sccb_write_Reg16Data8(OV5642_ID, 0x528a, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x528b, 0x6); 
		sccb_write_Reg16Data8(OV5642_ID, 0x528c, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x528d, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x528e, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x528f, 0x50);
		sccb_write_Reg16Data8(OV5642_ID, 0x5290, 0x60);
		sccb_write_Reg16Data8(OV5642_ID, 0x5293, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5295, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5297, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5299, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x529b, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x529d, 0x28);
		sccb_write_Reg16Data8(OV5642_ID, 0x529f, 0x30);
		sccb_write_Reg16Data8(OV5642_ID, 0x5300, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5301, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x5302, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5303, 0x7c);
		sccb_write_Reg16Data8(OV5642_ID, 0x530c, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x530d, 0xc); 
		sccb_write_Reg16Data8(OV5642_ID, 0x530e, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x530f, 0x80);
		sccb_write_Reg16Data8(OV5642_ID, 0x5310, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x5311, 0x80);
		sccb_write_Reg16Data8(OV5642_ID, 0x5380, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5381, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5382, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5383, 0x4e);
		sccb_write_Reg16Data8(OV5642_ID, 0x5384, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5385, 0xf); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5386, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5387, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5388, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5389, 0x15);
		sccb_write_Reg16Data8(OV5642_ID, 0x538a, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x538b, 0x31);
		sccb_write_Reg16Data8(OV5642_ID, 0x538c, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x538d, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x538e, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x538f, 0xf); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5390, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5391, 0xab);
		sccb_write_Reg16Data8(OV5642_ID, 0x5392, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5393, 0xa2);
		sccb_write_Reg16Data8(OV5642_ID, 0x5394, 0x8); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5480, 0x14);
		sccb_write_Reg16Data8(OV5642_ID, 0x5481, 0x21);
		sccb_write_Reg16Data8(OV5642_ID, 0x5482, 0x36);
		sccb_write_Reg16Data8(OV5642_ID, 0x5483, 0x57);
		sccb_write_Reg16Data8(OV5642_ID, 0x5484, 0x65);
		sccb_write_Reg16Data8(OV5642_ID, 0x5485, 0x71);
		sccb_write_Reg16Data8(OV5642_ID, 0x5486, 0x7d);
		sccb_write_Reg16Data8(OV5642_ID, 0x5487, 0x87);
		sccb_write_Reg16Data8(OV5642_ID, 0x5488, 0x91);
		sccb_write_Reg16Data8(OV5642_ID, 0x5489, 0x9a);
		sccb_write_Reg16Data8(OV5642_ID, 0x548a, 0xaa);
		sccb_write_Reg16Data8(OV5642_ID, 0x548b, 0xb8);
		sccb_write_Reg16Data8(OV5642_ID, 0x548c, 0xcd);
		sccb_write_Reg16Data8(OV5642_ID, 0x548d, 0xdd);
		sccb_write_Reg16Data8(OV5642_ID, 0x548e, 0xea);
		sccb_write_Reg16Data8(OV5642_ID, 0x548f, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x5490, 0x5); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5491, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5492, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5493, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x5494, 0x3); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5495, 0x60);
		sccb_write_Reg16Data8(OV5642_ID, 0x5496, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5497, 0xb8);
		sccb_write_Reg16Data8(OV5642_ID, 0x5498, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x5499, 0x86);
		sccb_write_Reg16Data8(OV5642_ID, 0x549a, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x549b, 0x5b);
		sccb_write_Reg16Data8(OV5642_ID, 0x549c, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x549d, 0x3b);
		sccb_write_Reg16Data8(OV5642_ID, 0x549e, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x549f, 0x1c);
		sccb_write_Reg16Data8(OV5642_ID, 0x54a0, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a1, 0x4); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a2, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a3, 0xed);
		sccb_write_Reg16Data8(OV5642_ID, 0x54a4, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a5, 0xc5);
		sccb_write_Reg16Data8(OV5642_ID, 0x54a6, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a7, 0xa5);
		sccb_write_Reg16Data8(OV5642_ID, 0x54a8, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54a9, 0x6c);
		sccb_write_Reg16Data8(OV5642_ID, 0x54aa, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54ab, 0x41);
		sccb_write_Reg16Data8(OV5642_ID, 0x54ac, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54ad, 0x20);
		sccb_write_Reg16Data8(OV5642_ID, 0x54ae, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x54af, 0x16);
		sccb_write_Reg16Data8(OV5642_ID, 0x3633, 0x7); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3702, 0x10);
		sccb_write_Reg16Data8(OV5642_ID, 0x3703, 0xb2);
		sccb_write_Reg16Data8(OV5642_ID, 0x3704, 0x18);
		sccb_write_Reg16Data8(OV5642_ID, 0x370b, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x370d, 0x2); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3620, 0x52);
		sccb_write_Reg16Data8(OV5642_ID, 0x3808, 0x1); 
		sccb_write_Reg16Data8(OV5642_ID, 0x3809, 0x40);
		sccb_write_Reg16Data8(OV5642_ID, 0x380a, 0x0); 
		sccb_write_Reg16Data8(OV5642_ID, 0x380b, 0xf0);
		//Mirror  
//	sccb_write_Reg16Data8(OV5642_ID, 0x3621, 0xe7);		
//	sccb_write_Reg16Data8(OV5642_ID, 0x3824, 0x01);
	}          
             
	if((nWidthH == 640) && (nWidthV == 480)) 
	{           
		//VGA 17FPS
		sccb_write_Reg16Data8(OV5642_ID,0x3103,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x3008,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3017,0x7f);
		sccb_write_Reg16Data8(OV5642_ID,0x3018,0xfc);
		sccb_write_Reg16Data8(OV5642_ID,0x3810,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3615,0xf0);
		sccb_write_Reg16Data8(OV5642_ID,0x3000,0xf8);
		sccb_write_Reg16Data8(OV5642_ID,0x3001,0x48);
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x5c);
		sccb_write_Reg16Data8(OV5642_ID,0x3003,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3000,0xf8);
		sccb_write_Reg16Data8(OV5642_ID,0x3001,0x48);
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x5c);
		sccb_write_Reg16Data8(OV5642_ID,0x3003,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3004,0xff);
		sccb_write_Reg16Data8(OV5642_ID,0x3005,0xb7);
		sccb_write_Reg16Data8(OV5642_ID,0x3006,0x43);
		sccb_write_Reg16Data8(OV5642_ID,0x3007,0x37);
		sccb_write_Reg16Data8(OV5642_ID,0x3011,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x3010,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x460c,0x22);
		sccb_write_Reg16Data8(OV5642_ID,0x3815,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x370d,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x370c,0xa0);
		sccb_write_Reg16Data8(OV5642_ID,0x3602,0xfc);
		sccb_write_Reg16Data8(OV5642_ID,0x3612,0xff);
		sccb_write_Reg16Data8(OV5642_ID,0x3634,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3613,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3605,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x3621,0xc7);
		sccb_write_Reg16Data8(OV5642_ID,0x3622,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3604,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x3603,0x27);
		sccb_write_Reg16Data8(OV5642_ID,0x3603,0x27);
		sccb_write_Reg16Data8(OV5642_ID,0x4000,0x21);
		sccb_write_Reg16Data8(OV5642_ID,0x401d,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3600,0x54);
		sccb_write_Reg16Data8(OV5642_ID,0x3605,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x3606,0x3f);
		sccb_write_Reg16Data8(OV5642_ID,0x3c01,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x5000,0x4f);
		sccb_write_Reg16Data8(OV5642_ID,0x5020,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x5181,0x79);
		sccb_write_Reg16Data8(OV5642_ID,0x5182,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5185,0x22);
		sccb_write_Reg16Data8(OV5642_ID,0x5197,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x5001,0x7f);
		sccb_write_Reg16Data8(OV5642_ID,0x5500,0xa); 
		sccb_write_Reg16Data8(OV5642_ID,0x5504,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5505,0x7f);
		sccb_write_Reg16Data8(OV5642_ID,0x5080,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x300e,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x4610,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x471d,0x5); 
		sccb_write_Reg16Data8(OV5642_ID,0x4708,0x6); 
		sccb_write_Reg16Data8(OV5642_ID,0x3710,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3632,0x41);
		sccb_write_Reg16Data8(OV5642_ID,0x3702,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x52);
		sccb_write_Reg16Data8(OV5642_ID,0x3631,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x3808,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3809,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x380a,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x380b,0xe0);
		sccb_write_Reg16Data8(OV5642_ID,0x380e,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x380f,0xe8);
		sccb_write_Reg16Data8(OV5642_ID,0x501f,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5000,0x4f);
		sccb_write_Reg16Data8(OV5642_ID,0x4300,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3503,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3501,0x3e);
		sccb_write_Reg16Data8(OV5642_ID,0x3502,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x350b,0x12);
		sccb_write_Reg16Data8(OV5642_ID,0x3503,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3824,0x11);
		sccb_write_Reg16Data8(OV5642_ID,0x3501,0x3e);
		sccb_write_Reg16Data8(OV5642_ID,0x3502,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x350b,0x12);
		sccb_write_Reg16Data8(OV5642_ID,0x380c,0xc); 
		sccb_write_Reg16Data8(OV5642_ID,0x380d,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x380e,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x380f,0xe8);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0d,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a0e,0x6); 
		sccb_write_Reg16Data8(OV5642_ID,0x3818,0xc1);
		sccb_write_Reg16Data8(OV5642_ID,0x3705,0xdb);
		sccb_write_Reg16Data8(OV5642_ID,0x370a,0x81);
		sccb_write_Reg16Data8(OV5642_ID,0x3801,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x3621,0xc7);
		sccb_write_Reg16Data8(OV5642_ID,0x3801,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x3803,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x3827,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x3810,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3804,0x5); 
		sccb_write_Reg16Data8(OV5642_ID,0x3805,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5682,0x5); 
		sccb_write_Reg16Data8(OV5642_ID,0x5683,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3806,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x3807,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x5686,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x5687,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3a00,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1a,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a13,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3a18,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a19,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a08,0x9); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a09,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0a,0x7); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a0b,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3004,0xff);
		sccb_write_Reg16Data8(OV5642_ID,0x350c,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x350d,0xe8);
		sccb_write_Reg16Data8(OV5642_ID,0x3500,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3501,0x3e);
		sccb_write_Reg16Data8(OV5642_ID,0x3502,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x350a,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x350b,0x12);
		sccb_write_Reg16Data8(OV5642_ID,0x3503,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x528a,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x528b,0x6); 
		sccb_write_Reg16Data8(OV5642_ID,0x528c,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x528d,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x528e,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x528f,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x5290,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x5292,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5293,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x5294,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5295,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x5296,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5297,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x5298,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5299,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x529a,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x529b,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x529c,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x529d,0x28);
		sccb_write_Reg16Data8(OV5642_ID,0x529e,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x529f,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0f,0x3c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a10,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1b,0x3c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1e,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3a11,0x70);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1f,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3030,0x2b);
		sccb_write_Reg16Data8(OV5642_ID,0x3a02,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a03,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x3a04,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a14,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a15,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x3a16,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a00,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a08,0x9); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a09,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0a,0x7); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a0b,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0d,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x3a0e,0x6); 
		sccb_write_Reg16Data8(OV5642_ID,0x5193,0x70);
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x52);
		sccb_write_Reg16Data8(OV5642_ID,0x3703,0xb2);
		sccb_write_Reg16Data8(OV5642_ID,0x3704,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x589b,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x589a,0xc5);
		sccb_write_Reg16Data8(OV5642_ID,0x528a,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x528b,0x6); 
		sccb_write_Reg16Data8(OV5642_ID,0x528c,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x528d,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x528e,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x528f,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x5290,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x5293,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x5295,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x5297,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x5299,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x529b,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x529d,0x28);
		sccb_write_Reg16Data8(OV5642_ID,0x529f,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x5300,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5301,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5302,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5303,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x530c,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x530d,0xc); 
		sccb_write_Reg16Data8(OV5642_ID,0x530e,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x530f,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x5310,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5311,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x5380,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x5381,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5382,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5383,0x4e);
		sccb_write_Reg16Data8(OV5642_ID,0x5384,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5385,0xf); 
		sccb_write_Reg16Data8(OV5642_ID,0x5386,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5387,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5388,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x5389,0x15);
		sccb_write_Reg16Data8(OV5642_ID,0x538a,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x538b,0x31);
		sccb_write_Reg16Data8(OV5642_ID,0x538c,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x538d,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x538e,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x538f,0xf); 
		sccb_write_Reg16Data8(OV5642_ID,0x5390,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5391,0xab);
		sccb_write_Reg16Data8(OV5642_ID,0x5392,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5393,0xa2);
		sccb_write_Reg16Data8(OV5642_ID,0x5394,0x8); 
		sccb_write_Reg16Data8(OV5642_ID,0x5480,0x14);
		sccb_write_Reg16Data8(OV5642_ID,0x5481,0x21);
		sccb_write_Reg16Data8(OV5642_ID,0x5482,0x36);
		sccb_write_Reg16Data8(OV5642_ID,0x5483,0x57);
		sccb_write_Reg16Data8(OV5642_ID,0x5484,0x65);
		sccb_write_Reg16Data8(OV5642_ID,0x5485,0x71);
		sccb_write_Reg16Data8(OV5642_ID,0x5486,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x5487,0x87);
		sccb_write_Reg16Data8(OV5642_ID,0x5488,0x91);
		sccb_write_Reg16Data8(OV5642_ID,0x5489,0x9a);
		sccb_write_Reg16Data8(OV5642_ID,0x548a,0xaa);
		sccb_write_Reg16Data8(OV5642_ID,0x548b,0xb8);
		sccb_write_Reg16Data8(OV5642_ID,0x548c,0xcd);
		sccb_write_Reg16Data8(OV5642_ID,0x548d,0xdd);
		sccb_write_Reg16Data8(OV5642_ID,0x548e,0xea);
		sccb_write_Reg16Data8(OV5642_ID,0x548f,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x5490,0x5); 
		sccb_write_Reg16Data8(OV5642_ID,0x5491,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x5492,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x5493,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5494,0x3); 
		sccb_write_Reg16Data8(OV5642_ID,0x5495,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x5496,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x5497,0xb8);
		sccb_write_Reg16Data8(OV5642_ID,0x5498,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x5499,0x86);
		sccb_write_Reg16Data8(OV5642_ID,0x549a,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x549b,0x5b);
		sccb_write_Reg16Data8(OV5642_ID,0x549c,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x549d,0x3b);
		sccb_write_Reg16Data8(OV5642_ID,0x549e,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x549f,0x1c);
		sccb_write_Reg16Data8(OV5642_ID,0x54a0,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a1,0x4); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a2,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a3,0xed);
		sccb_write_Reg16Data8(OV5642_ID,0x54a4,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a5,0xc5);
		sccb_write_Reg16Data8(OV5642_ID,0x54a6,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a7,0xa5);
		sccb_write_Reg16Data8(OV5642_ID,0x54a8,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54a9,0x6c);
		sccb_write_Reg16Data8(OV5642_ID,0x54aa,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54ab,0x41);
		sccb_write_Reg16Data8(OV5642_ID,0x54ac,0x1); 
		sccb_write_Reg16Data8(OV5642_ID,0x54ad,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x54ae,0x0); 
		sccb_write_Reg16Data8(OV5642_ID,0x54af,0x16);
		sccb_write_Reg16Data8(OV5642_ID,0x3633,0x7); 
		sccb_write_Reg16Data8(OV5642_ID,0x3702,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3703,0xb2);
		sccb_write_Reg16Data8(OV5642_ID,0x3704,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x370b,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x370d,0x2); 
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x52);

	}
	if((nWidthH == 2592) && (nWidthV == 1944)) 	
	{
		//2592X1944
		sccb_write_Reg16Data8(OV5642_ID,0x3103,0x03);
		sccb_write_Reg16Data8(OV5642_ID,0x3008,0x82);
		sccb_write_Reg16Data8(OV5642_ID,0x3017,0x7f);
		sccb_write_Reg16Data8(OV5642_ID,0x3018,0xfc);
		sccb_write_Reg16Data8(OV5642_ID,0x3810,0xc2);
		sccb_write_Reg16Data8(OV5642_ID,0x3615,0xf0);
		sccb_write_Reg16Data8(OV5642_ID,0x3000,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3001,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3003,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3030,0x2b);
		sccb_write_Reg16Data8(OV5642_ID,0x3011,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x3010,0x70);
		sccb_write_Reg16Data8(OV5642_ID,0x3604,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x3622,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x3621,0x27);
		sccb_write_Reg16Data8(OV5642_ID,0x3709,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x4000,0x21);
		sccb_write_Reg16Data8(OV5642_ID,0x401d,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x3600,0x54);
		sccb_write_Reg16Data8(OV5642_ID,0x3605,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x3606,0x3f);
		sccb_write_Reg16Data8(OV5642_ID,0x3c01,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x300d,0x21);
		sccb_write_Reg16Data8(OV5642_ID,0x3623,0x22);
		sccb_write_Reg16Data8(OV5642_ID,0x5000,0x4f);
		sccb_write_Reg16Data8(OV5642_ID,0x5020,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x5181,0x79);
		sccb_write_Reg16Data8(OV5642_ID,0x5182,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5185,0x22);
		sccb_write_Reg16Data8(OV5642_ID,0x5197,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x5500,0x0a);
		sccb_write_Reg16Data8(OV5642_ID,0x5504,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5505,0x7f);
		sccb_write_Reg16Data8(OV5642_ID,0x5080,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x300e,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x4610,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x471d,0x05);
		sccb_write_Reg16Data8(OV5642_ID,0x4708,0x06);
		sccb_write_Reg16Data8(OV5642_ID,0x3710,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x370d,0x0e);
		sccb_write_Reg16Data8(OV5642_ID,0x3632,0x41);
		sccb_write_Reg16Data8(OV5642_ID,0x3702,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x37);
		sccb_write_Reg16Data8(OV5642_ID,0x3631,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x370c,0xa0);
		sccb_write_Reg16Data8(OV5642_ID,0x3808,0x0a);
		sccb_write_Reg16Data8(OV5642_ID,0x3809,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x380a,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x380b,0x98);
		sccb_write_Reg16Data8(OV5642_ID,0x380c,0x0c);
		sccb_write_Reg16Data8(OV5642_ID,0x380d,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x380e,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x380f,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3801,0x8a);
		sccb_write_Reg16Data8(OV5642_ID,0x501f,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5000,0x4f);
		sccb_write_Reg16Data8(OV5642_ID,0x5001,0x4f);
		sccb_write_Reg16Data8(OV5642_ID,0x4300,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x4300,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x460b,0x35);
		sccb_write_Reg16Data8(OV5642_ID,0x471d,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x0c);
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x4713,0x03);
		sccb_write_Reg16Data8(OV5642_ID,0x471c,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x460c,0x22);
		sccb_write_Reg16Data8(OV5642_ID,0x3815,0x44);
		sccb_write_Reg16Data8(OV5642_ID,0x3503,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x3501,0x73);
		sccb_write_Reg16Data8(OV5642_ID,0x3502,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x350b,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3818,0xc8);
		sccb_write_Reg16Data8(OV5642_ID,0x3621,0x27);
		sccb_write_Reg16Data8(OV5642_ID,0x3801,0x8a);
		sccb_write_Reg16Data8(OV5642_ID,0x3a00,0x78);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1a,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x3a13,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3a18,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a19,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a08,0x12);
		sccb_write_Reg16Data8(OV5642_ID,0x3a09,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0a,0x0f);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0b,0xa0);
		sccb_write_Reg16Data8(OV5642_ID,0x3004,0xff);
		sccb_write_Reg16Data8(OV5642_ID,0x350c,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x350d,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0d,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0e,0x06);
		sccb_write_Reg16Data8(OV5642_ID,0x3500,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3501,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3502,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x350a,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x350b,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3503,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0f,0x3c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a10,0x32);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1b,0x3c);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1e,0x32);
		sccb_write_Reg16Data8(OV5642_ID,0x3a11,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x3a1f,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x3030,0x2b);
		sccb_write_Reg16Data8(OV5642_ID,0x3a02,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a03,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x3a04,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a14,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a15,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x3a16,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x3a00,0x78);
		sccb_write_Reg16Data8(OV5642_ID,0x3a08,0x09);
		sccb_write_Reg16Data8(OV5642_ID,0x3a09,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0a,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0b,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0d,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3a0e,0x0d);
		sccb_write_Reg16Data8(OV5642_ID,0x4407,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x5193,0x70);
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x57);
		sccb_write_Reg16Data8(OV5642_ID,0x3703,0x98);
		sccb_write_Reg16Data8(OV5642_ID,0x3704,0x1c);
		sccb_write_Reg16Data8(OV5642_ID,0x589b,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x589a,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x528a,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x528b,0x06);
		sccb_write_Reg16Data8(OV5642_ID,0x528c,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x528d,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x528e,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x528f,0x50);
		sccb_write_Reg16Data8(OV5642_ID,0x5290,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x5293,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x5295,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x5297,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x5299,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x529b,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x529d,0x28);
		sccb_write_Reg16Data8(OV5642_ID,0x529f,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x5300,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5301,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5302,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5303,0x7c);
		sccb_write_Reg16Data8(OV5642_ID,0x530c,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x530d,0x0c);
		sccb_write_Reg16Data8(OV5642_ID,0x530e,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x530f,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x5310,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5311,0x80);
		sccb_write_Reg16Data8(OV5642_ID,0x5380,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x5381,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5382,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5383,0x4e);
		sccb_write_Reg16Data8(OV5642_ID,0x5384,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5385,0x0f);
		sccb_write_Reg16Data8(OV5642_ID,0x5386,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5387,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5388,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x5389,0x15);
		sccb_write_Reg16Data8(OV5642_ID,0x538a,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x538b,0x31);
		sccb_write_Reg16Data8(OV5642_ID,0x538c,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x538d,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x538e,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x538f,0x0f);
		sccb_write_Reg16Data8(OV5642_ID,0x5390,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5391,0xab);
		sccb_write_Reg16Data8(OV5642_ID,0x5392,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5393,0xa2);
		sccb_write_Reg16Data8(OV5642_ID,0x5394,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x5480,0x14);
		sccb_write_Reg16Data8(OV5642_ID,0x5481,0x21);
		sccb_write_Reg16Data8(OV5642_ID,0x5482,0x36);
		sccb_write_Reg16Data8(OV5642_ID,0x5483,0x57);
		sccb_write_Reg16Data8(OV5642_ID,0x5484,0x65);
		sccb_write_Reg16Data8(OV5642_ID,0x5485,0x71);
		sccb_write_Reg16Data8(OV5642_ID,0x5486,0x7d);
		sccb_write_Reg16Data8(OV5642_ID,0x5487,0x87);
		sccb_write_Reg16Data8(OV5642_ID,0x5488,0x91);
		sccb_write_Reg16Data8(OV5642_ID,0x5489,0x9a);
		sccb_write_Reg16Data8(OV5642_ID,0x548a,0xaa);
		sccb_write_Reg16Data8(OV5642_ID,0x548b,0xb8);
		sccb_write_Reg16Data8(OV5642_ID,0x548c,0xcd);
		sccb_write_Reg16Data8(OV5642_ID,0x548d,0xdd);
		sccb_write_Reg16Data8(OV5642_ID,0x548e,0xea);
		sccb_write_Reg16Data8(OV5642_ID,0x548f,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x5490,0x05);
		sccb_write_Reg16Data8(OV5642_ID,0x5491,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5492,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x5493,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x5494,0x03);
		sccb_write_Reg16Data8(OV5642_ID,0x5495,0x60);
		sccb_write_Reg16Data8(OV5642_ID,0x5496,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x5497,0xb8);
		sccb_write_Reg16Data8(OV5642_ID,0x5498,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x5499,0x86);
		sccb_write_Reg16Data8(OV5642_ID,0x549a,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x549b,0x5b);
		sccb_write_Reg16Data8(OV5642_ID,0x549c,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x549d,0x3b);
		sccb_write_Reg16Data8(OV5642_ID,0x549e,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x549f,0x1c);
		sccb_write_Reg16Data8(OV5642_ID,0x54a0,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x54a1,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x54a2,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54a3,0xed);
		sccb_write_Reg16Data8(OV5642_ID,0x54a4,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54a5,0xc5);
		sccb_write_Reg16Data8(OV5642_ID,0x54a6,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54a7,0xa5);
		sccb_write_Reg16Data8(OV5642_ID,0x54a8,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54a9,0x6c);
		sccb_write_Reg16Data8(OV5642_ID,0x54aa,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54ab,0x41);
		sccb_write_Reg16Data8(OV5642_ID,0x54ac,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x54ad,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x54ae,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x54af,0x16);
		sccb_write_Reg16Data8(OV5642_ID,0x3633,0x07);
		sccb_write_Reg16Data8(OV5642_ID,0x3702,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x3703,0xb2);
		sccb_write_Reg16Data8(OV5642_ID,0x3704,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x370b,0x40);
		sccb_write_Reg16Data8(OV5642_ID,0x370d,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x3620,0x52);
		                      
		//;YUV                
		sccb_write_Reg16Data8(OV5642_ID,0x3002,0x1c);
		sccb_write_Reg16Data8(OV5642_ID,0x460c,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x471c,0xd0);
		sccb_write_Reg16Data8(OV5642_ID,0x3815,0x01);
		sccb_write_Reg16Data8(OV5642_ID,0x501f,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5002,0xe0);
		sccb_write_Reg16Data8(OV5642_ID,0x4300,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x3818,0xc0);
		sccb_write_Reg16Data8(OV5642_ID,0x3810,0xc2);
              
		//;auto sharp+1
		sccb_write_Reg16Data8(OV5642_ID,0x530c,0x4);
		sccb_write_Reg16Data8(OV5642_ID,0x530d,0x18);
		sccb_write_Reg16Data8(OV5642_ID,0x5312,0x20);
              
		//;@@ Auto -1  
		//;de-noise Y
		sccb_write_Reg16Data8(OV5642_ID,0x528a,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x528b,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x528c,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x528d,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x528e,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x528f,0x28);
		sccb_write_Reg16Data8(OV5642_ID,0x5290,0x30);
		//;de-noise UV        
		sccb_write_Reg16Data8(OV5642_ID,0x5292,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5293,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5294,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5295,0x02);
		sccb_write_Reg16Data8(OV5642_ID,0x5296,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5297,0x08);
		sccb_write_Reg16Data8(OV5642_ID,0x5298,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5299,0x10);
		sccb_write_Reg16Data8(OV5642_ID,0x529a,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x529b,0x20);
		sccb_write_Reg16Data8(OV5642_ID,0x529c,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x529d,0x28);
		sccb_write_Reg16Data8(OV5642_ID,0x529e,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x529f,0x30);
		sccb_write_Reg16Data8(OV5642_ID,0x5282,0x00);
		                      
		//;de-noise cip       
		sccb_write_Reg16Data8(OV5642_ID,0x5304,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5305,0x00);
		sccb_write_Reg16Data8(OV5642_ID,0x5314,0x03); 
		sccb_write_Reg16Data8(OV5642_ID,0x5315,0x04);
		sccb_write_Reg16Data8(OV5642_ID,0x5319,0x02);
	}		
	//V Flip & Mirror
	sccb_write_Reg16Data8(OV5642_ID, 0x3818, 0xa1);	//VFlip set/clear 0x3818[5] 		
	sccb_write_Reg16Data8(OV5642_ID, 0x3621, 0xe7);	//Mirror
	
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif	
}
#endif
#ifdef	__OV7675_DRV_C__
//====================================================================================================
//	Description:	OV7675 Initialization
//	Syntax:			void OV7675_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV7675_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif			
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	R_CSI_TG_HRATIO = 0;							//no scaler					
	R_CSI_TG_VRATIO = 0;	
	
	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg8Data8(OV7675_ID, 0x12, 0x80);
	sccb_delay(200);
	
	if(nWidthH == 320 && nWidthV == 240)
	{
		//QVGA, YUV, 27Mhz, 60fps
		sccb_write_Reg8Data8(OV7675_ID, 0x09, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0xC1, 0x7F);
#if CSI_FPS	== CSI_15FPS
		sccb_write_Reg8Data8(OV7675_ID, 0x11, 0x82);   // 15fps
#elif CSI_FPS == CSI_30FPS
		sccb_write_Reg8Data8(OV7675_ID, 0x11, 0x81);	// 30fps
#else		
		sccb_write_Reg8Data8(OV7675_ID, 0x11, 0x80);	// 60fps
#endif		
		sccb_write_Reg8Data8(OV7675_ID, 0x3A, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC0);
		sccb_write_Reg8Data8(OV7675_ID, 0x12, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x15, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x17, 0x13);
		sccb_write_Reg8Data8(OV7675_ID, 0x18, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x32, 0xBF);
		sccb_write_Reg8Data8(OV7675_ID, 0x19, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x1A, 0x7A);
		sccb_write_Reg8Data8(OV7675_ID, 0x03, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x0C, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x3E, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x70, 0x3A);
		sccb_write_Reg8Data8(OV7675_ID, 0x71, 0x35);
		sccb_write_Reg8Data8(OV7675_ID, 0x72, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x73, 0xF0);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0xA2, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x7A, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x7B, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x7C, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x7D, 0x1A);
		sccb_write_Reg8Data8(OV7675_ID, 0x7E, 0x3F);
		sccb_write_Reg8Data8(OV7675_ID, 0x7F, 0x4E);
		sccb_write_Reg8Data8(OV7675_ID, 0x80, 0x5B);
		sccb_write_Reg8Data8(OV7675_ID, 0x81, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x82, 0x75);
		sccb_write_Reg8Data8(OV7675_ID, 0x83, 0x7F);
		sccb_write_Reg8Data8(OV7675_ID, 0x84, 0x89);
		sccb_write_Reg8Data8(OV7675_ID, 0x85, 0x9A);
		sccb_write_Reg8Data8(OV7675_ID, 0x86, 0xA6);
		sccb_write_Reg8Data8(OV7675_ID, 0x87, 0xBD);
		sccb_write_Reg8Data8(OV7675_ID, 0x88, 0xD3);
		sccb_write_Reg8Data8(OV7675_ID, 0x89, 0xE8);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE0);
		sccb_write_Reg8Data8(OV7675_ID, 0x00, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x10, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x0D, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x14, 0x28);
		sccb_write_Reg8Data8(OV7675_ID, 0xA5, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0xAB, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x24, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x25, 0x58);
		sccb_write_Reg8Data8(OV7675_ID, 0x26, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x9F, 0x78);
		sccb_write_Reg8Data8(OV7675_ID, 0xA0, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0xA1, 0x03);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0xA6, 0xD8);
		sccb_write_Reg8Data8(OV7675_ID, 0xA7, 0xD8);
		sccb_write_Reg8Data8(OV7675_ID, 0xA8, 0xF0);
		sccb_write_Reg8Data8(OV7675_ID, 0xA9, 0x90);
		sccb_write_Reg8Data8(OV7675_ID, 0xAA, 0x14);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE5);
		sccb_write_Reg8Data8(OV7675_ID, 0x0E, 0x61);
		sccb_write_Reg8Data8(OV7675_ID, 0x0F, 0x4B);
		sccb_write_Reg8Data8(OV7675_ID, 0x16, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x1e, 0x37); 	// Flip & Mirror
		sccb_write_Reg8Data8(OV7675_ID, 0x21, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x22, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x29, 0x07);
		sccb_write_Reg8Data8(OV7675_ID, 0x33, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x35, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x37, 0x1D);
		sccb_write_Reg8Data8(OV7675_ID, 0x38, 0x71);
		sccb_write_Reg8Data8(OV7675_ID, 0x39, 0x2A);
		sccb_write_Reg8Data8(OV7675_ID, 0x3C, 0x78);
		sccb_write_Reg8Data8(OV7675_ID, 0x4D, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x4E, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x69, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x6B, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x74, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x8D, 0x4F);
		sccb_write_Reg8Data8(OV7675_ID, 0x8E, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x8F, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x90, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x91, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x96, 0x00);
		                    
		sccb_write_Reg8Data8(OV7675_ID, 0x9A, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0xB0, 0x84);
		sccb_write_Reg8Data8(OV7675_ID, 0xB1, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0xB2, 0x0E);
		sccb_write_Reg8Data8(OV7675_ID, 0xB3, 0x82);
		sccb_write_Reg8Data8(OV7675_ID, 0xB8, 0x10); //0x08 for 1.8 volts, 0x10 for 2.8 volts to DOVDD	
		sccb_write_Reg8Data8(OV7675_ID, 0x43, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x44, 0xF2);
		sccb_write_Reg8Data8(OV7675_ID, 0x45, 0x39);
		sccb_write_Reg8Data8(OV7675_ID, 0x46, 0x62);
		sccb_write_Reg8Data8(OV7675_ID, 0x47, 0x3D);
		sccb_write_Reg8Data8(OV7675_ID, 0x48, 0x55);
		sccb_write_Reg8Data8(OV7675_ID, 0x59, 0x83);
		sccb_write_Reg8Data8(OV7675_ID, 0x5A, 0x0D);
		sccb_write_Reg8Data8(OV7675_ID, 0x5B, 0xCD);
		sccb_write_Reg8Data8(OV7675_ID, 0x5C, 0x8C);
		sccb_write_Reg8Data8(OV7675_ID, 0x5D, 0x77);
		sccb_write_Reg8Data8(OV7675_ID, 0x5E, 0x16);
		sccb_write_Reg8Data8(OV7675_ID, 0x6C, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x6D, 0x65);
		sccb_write_Reg8Data8(OV7675_ID, 0x6E, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x6F, 0x9E);
		sccb_write_Reg8Data8(OV7675_ID, 0x6A, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x01, 0x56);
		sccb_write_Reg8Data8(OV7675_ID, 0x02, 0x44);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE7);
		sccb_write_Reg8Data8(OV7675_ID, 0x4F, 0x88);
		sccb_write_Reg8Data8(OV7675_ID, 0x50, 0x8B);
		sccb_write_Reg8Data8(OV7675_ID, 0x51, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x52, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x53, 0x8C);
		sccb_write_Reg8Data8(OV7675_ID, 0x54, 0x9D);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x55, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x56, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x57, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x58, 0x9A);
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x08);
		sccb_write_Reg8Data8(OV7675_ID, 0x3F, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x75, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x76, 0x60);
		sccb_write_Reg8Data8(OV7675_ID, 0x4C, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x77, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x4D, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0xC9, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x56, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x34, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x3B, 0x12);
		sccb_write_Reg8Data8(OV7675_ID, 0xA4, 0x88);
		sccb_write_Reg8Data8(OV7675_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x97, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x98, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x99, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x9A, 0x84);
		sccb_write_Reg8Data8(OV7675_ID, 0x9B, 0x29);
		sccb_write_Reg8Data8(OV7675_ID, 0x9C, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x9D, 0x99);
		sccb_write_Reg8Data8(OV7675_ID, 0x9E, 0x7F);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x78, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0xF0);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x7E);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0D);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0xC0);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x26);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x62, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x63, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x64, 0x06);
		sccb_write_Reg8Data8(OV7675_ID, 0x65, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x66, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x94, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x95, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0x2A, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x2B, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x15, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x3A, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC3);
		sccb_write_Reg8Data8(OV7675_ID, 0x19, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x1A, 0x7B);
		sccb_write_Reg8Data8(OV7675_ID, 0x2A, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x2B, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x18, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x92, 0x88);
		sccb_write_Reg8Data8(OV7675_ID, 0x93, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0xB9, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x19, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x1A, 0x3E);
		sccb_write_Reg8Data8(OV7675_ID, 0x17, 0x13);
		sccb_write_Reg8Data8(OV7675_ID, 0x18, 0x3B);
		sccb_write_Reg8Data8(OV7675_ID, 0x03, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0xE6, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0xD2, 0x1C);
		sccb_write_Reg8Data8(OV7675_ID, 0x66, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x62, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x63, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x65, 0x07);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x64, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0x94, 0x0E);
		sccb_write_Reg8Data8(OV7675_ID, 0x95, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x4F, 0x87);
		sccb_write_Reg8Data8(OV7675_ID, 0x50, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x51, 0x1E);
		sccb_write_Reg8Data8(OV7675_ID, 0x52, 0x15);
		sccb_write_Reg8Data8(OV7675_ID, 0x53, 0x7C);
		sccb_write_Reg8Data8(OV7675_ID, 0x54, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x58, 0x1E);
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x76, 0xE0);
		sccb_write_Reg8Data8(OV7675_ID, 0x24, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x25, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x26, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x7A, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0x7B, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0x7C, 0x16);
		sccb_write_Reg8Data8(OV7675_ID, 0x7D, 0x28);
		sccb_write_Reg8Data8(OV7675_ID, 0x7E, 0x48);
		sccb_write_Reg8Data8(OV7675_ID, 0x7F, 0x57);
		sccb_write_Reg8Data8(OV7675_ID, 0x80, 0x64);
		sccb_write_Reg8Data8(OV7675_ID, 0x81, 0x71);
		sccb_write_Reg8Data8(OV7675_ID, 0x82, 0x7E);
		sccb_write_Reg8Data8(OV7675_ID, 0x83, 0x89);
		sccb_write_Reg8Data8(OV7675_ID, 0x84, 0x94);
		sccb_write_Reg8Data8(OV7675_ID, 0x85, 0xA8);
		sccb_write_Reg8Data8(OV7675_ID, 0x86, 0xBA);
		sccb_write_Reg8Data8(OV7675_ID, 0x87, 0xD7);
		sccb_write_Reg8Data8(OV7675_ID, 0x88, 0xEC);
		sccb_write_Reg8Data8(OV7675_ID, 0x89, 0xF9);
		sccb_write_Reg8Data8(OV7675_ID, 0x09, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x92, 0x3F);	
	}           
	else if(nWidthH == 640 && nWidthV == 480)
	{           
		//VGA, YUV, 27Mhz, 30fps
		sccb_write_Reg8Data8(OV7675_ID, 0x09, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0xC1, 0x7F);
#if CSI_FPS	== CSI_15FPS
		sccb_write_Reg8Data8(OV7675_ID, 0x11, 0x81);   // 15fps
#else
		sccb_write_Reg8Data8(OV7675_ID, 0x11, 0x80);	// 30fps
#endif
		sccb_write_Reg8Data8(OV7675_ID, 0x3A, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC0);
		sccb_write_Reg8Data8(OV7675_ID, 0x12, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x15, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x17, 0x13);
		sccb_write_Reg8Data8(OV7675_ID, 0x18, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x32, 0xBF);
		sccb_write_Reg8Data8(OV7675_ID, 0x19, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x1A, 0x7A);
		sccb_write_Reg8Data8(OV7675_ID, 0x03, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x0C, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x3E, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x70, 0x3A);
		sccb_write_Reg8Data8(OV7675_ID, 0x71, 0x35);
		sccb_write_Reg8Data8(OV7675_ID, 0x72, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x73, 0xF0);
		sccb_write_Reg8Data8(OV7675_ID, 0xA2, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x7A, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x7B, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x7C, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x7D, 0x1A);
		sccb_write_Reg8Data8(OV7675_ID, 0x7E, 0x3F);
		sccb_write_Reg8Data8(OV7675_ID, 0x7F, 0x4E);
		sccb_write_Reg8Data8(OV7675_ID, 0x80, 0x5B);
		sccb_write_Reg8Data8(OV7675_ID, 0x81, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x82, 0x75);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x83, 0x7F);
		sccb_write_Reg8Data8(OV7675_ID, 0x84, 0x89);
		sccb_write_Reg8Data8(OV7675_ID, 0x85, 0x9A);
		sccb_write_Reg8Data8(OV7675_ID, 0x86, 0xA6);
		sccb_write_Reg8Data8(OV7675_ID, 0x87, 0xBD);
		sccb_write_Reg8Data8(OV7675_ID, 0x88, 0xD3);
		sccb_write_Reg8Data8(OV7675_ID, 0x89, 0xE8);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE0);
		sccb_write_Reg8Data8(OV7675_ID, 0x00, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x10, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x0D, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x14, 0x28);
		sccb_write_Reg8Data8(OV7675_ID, 0xA5, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0xAB, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x24, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x25, 0x58);
		sccb_write_Reg8Data8(OV7675_ID, 0x26, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x9F, 0x78);
		sccb_write_Reg8Data8(OV7675_ID, 0xA0, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0xA1, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0xA6, 0xD8);
		sccb_write_Reg8Data8(OV7675_ID, 0xA7, 0xD8);
		sccb_write_Reg8Data8(OV7675_ID, 0xA8, 0xF0);
		sccb_write_Reg8Data8(OV7675_ID, 0xA9, 0x90);
		sccb_write_Reg8Data8(OV7675_ID, 0xAA, 0x14);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE5);
		sccb_write_Reg8Data8(OV7675_ID, 0x0E, 0x61);
		sccb_write_Reg8Data8(OV7675_ID, 0x0F, 0x4B);
		sccb_write_Reg8Data8(OV7675_ID, 0x16, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x1e, 0x37); 	// Flip & Mirror
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x21, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0x22, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x29, 0x07);
		sccb_write_Reg8Data8(OV7675_ID, 0x33, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x35, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x37, 0x1D);
		sccb_write_Reg8Data8(OV7675_ID, 0x38, 0x71);
		sccb_write_Reg8Data8(OV7675_ID, 0x39, 0x2A);
		sccb_write_Reg8Data8(OV7675_ID, 0x3C, 0x78);
		sccb_write_Reg8Data8(OV7675_ID, 0x4D, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x4E, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x69, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x6B, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x74, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x8D, 0x4F);
		sccb_write_Reg8Data8(OV7675_ID, 0x8E, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x8F, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x90, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x91, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x9A, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0xB0, 0x84);
		sccb_write_Reg8Data8(OV7675_ID, 0xB1, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0xB2, 0x0E);
		sccb_write_Reg8Data8(OV7675_ID, 0xB3, 0x82);
		sccb_write_Reg8Data8(OV7675_ID, 0xB8, 0x10); //0x08 for 1.8 volts, 0x10 for 2.8 volts to DOVDD	
		sccb_write_Reg8Data8(OV7675_ID, 0x43, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x44, 0xF2);
		sccb_write_Reg8Data8(OV7675_ID, 0x45, 0x39);
		sccb_write_Reg8Data8(OV7675_ID, 0x46, 0x62);
		sccb_write_Reg8Data8(OV7675_ID, 0x47, 0x3D);
		sccb_write_Reg8Data8(OV7675_ID, 0x48, 0x55);
		sccb_write_Reg8Data8(OV7675_ID, 0x59, 0x83);
		sccb_write_Reg8Data8(OV7675_ID, 0x5A, 0x0D);
		sccb_write_Reg8Data8(OV7675_ID, 0x5B, 0xCD);
		sccb_write_Reg8Data8(OV7675_ID, 0x5C, 0x8C);
		sccb_write_Reg8Data8(OV7675_ID, 0x5D, 0x77);
		sccb_write_Reg8Data8(OV7675_ID, 0x5E, 0x16);
		sccb_write_Reg8Data8(OV7675_ID, 0x6C, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0x6D, 0x65);
		sccb_write_Reg8Data8(OV7675_ID, 0x6E, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x6F, 0x9E);
		sccb_write_Reg8Data8(OV7675_ID, 0x6A, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x01, 0x56);
		sccb_write_Reg8Data8(OV7675_ID, 0x02, 0x44);
		sccb_write_Reg8Data8(OV7675_ID, 0x13, 0xE7);
		sccb_write_Reg8Data8(OV7675_ID, 0x4F, 0x88);
		sccb_write_Reg8Data8(OV7675_ID, 0x50, 0x8B);
		sccb_write_Reg8Data8(OV7675_ID, 0x51, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x52, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x53, 0x8C);
		sccb_write_Reg8Data8(OV7675_ID, 0x54, 0x9D);
		sccb_write_Reg8Data8(OV7675_ID, 0x55, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x56, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x57, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x58, 0x9A);
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x08);
		sccb_write_Reg8Data8(OV7675_ID, 0x3F, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x75, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x76, 0x60);
		sccb_write_Reg8Data8(OV7675_ID, 0x4C, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x77, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x4D, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0xC9, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x56, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x34, 0x11);
		sccb_write_Reg8Data8(OV7675_ID, 0x3B, 0x12);
		sccb_write_Reg8Data8(OV7675_ID, 0xA4, 0x88);
		sccb_write_Reg8Data8(OV7675_ID, 0x96, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x97, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x98, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x99, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x9A, 0x84);
		sccb_write_Reg8Data8(OV7675_ID, 0x9B, 0x29);
		sccb_write_Reg8Data8(OV7675_ID, 0x9C, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x9D, 0x99);
		sccb_write_Reg8Data8(OV7675_ID, 0x9E, 0x7F);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x78, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0xF0);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x7E);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0A);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x01);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x0D);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x20);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x80);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x02);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0xC0);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0xC8, 0x30);
		sccb_write_Reg8Data8(OV7675_ID, 0x79, 0x26);
		          
		sccb_write_Reg8Data8(OV7675_ID, 0x62, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x63, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x64, 0x06);
		sccb_write_Reg8Data8(OV7675_ID, 0x65, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x66, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x94, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x95, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0x2A, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x2B, 0xC2);
		sccb_write_Reg8Data8(OV7675_ID, 0x15, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x3A, 0x04);
		sccb_write_Reg8Data8(OV7675_ID, 0x3D, 0xC3);
		sccb_write_Reg8Data8(OV7675_ID, 0x19, 0x03);
		sccb_write_Reg8Data8(OV7675_ID, 0x1A, 0x7B);
		sccb_write_Reg8Data8(OV7675_ID, 0x2A, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x2B, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x18, 0x01);
		                    
		sccb_write_Reg8Data8(OV7675_ID, 0x66, 0x05);
		sccb_write_Reg8Data8(OV7675_ID, 0x62, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x63, 0x0B);
		sccb_write_Reg8Data8(OV7675_ID, 0x65, 0x07);
		sccb_write_Reg8Data8(OV7675_ID, 0x64, 0x0F);
		sccb_write_Reg8Data8(OV7675_ID, 0x94, 0x0E);
		sccb_write_Reg8Data8(OV7675_ID, 0x95, 0x10);
		sccb_write_Reg8Data8(OV7675_ID, 0x4F, 0x87);
		sccb_write_Reg8Data8(OV7675_ID, 0x50, 0x68);
		sccb_write_Reg8Data8(OV7675_ID, 0x51, 0x1E);
		sccb_write_Reg8Data8(OV7675_ID, 0x52, 0x15);
		sccb_write_Reg8Data8(OV7675_ID, 0x53, 0x7C);
		sccb_write_Reg8Data8(OV7675_ID, 0x54, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x58, 0x1E);
		                     
		sccb_write_Reg8Data8(OV7675_ID, 0x41, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x76, 0xE0);
		sccb_write_Reg8Data8(OV7675_ID, 0x24, 0x40);
		sccb_write_Reg8Data8(OV7675_ID, 0x25, 0x38);
		sccb_write_Reg8Data8(OV7675_ID, 0x26, 0x91);
		sccb_write_Reg8Data8(OV7675_ID, 0x7A, 0x09);
		sccb_write_Reg8Data8(OV7675_ID, 0x7B, 0x0C);
		sccb_write_Reg8Data8(OV7675_ID, 0x7C, 0x16);
		sccb_write_Reg8Data8(OV7675_ID, 0x7D, 0x28);
		sccb_write_Reg8Data8(OV7675_ID, 0x7E, 0x48);
		sccb_write_Reg8Data8(OV7675_ID, 0x7F, 0x57);
                         
		sccb_write_Reg8Data8(OV7675_ID, 0x80, 0x64);
		sccb_write_Reg8Data8(OV7675_ID, 0x81, 0x71);
		sccb_write_Reg8Data8(OV7675_ID, 0x82, 0x7E);
		sccb_write_Reg8Data8(OV7675_ID, 0x83, 0x89);
		sccb_write_Reg8Data8(OV7675_ID, 0x84, 0x94);
		sccb_write_Reg8Data8(OV7675_ID, 0x85, 0xA8);
		sccb_write_Reg8Data8(OV7675_ID, 0x86, 0xBA);
		sccb_write_Reg8Data8(OV7675_ID, 0x87, 0xD7);
		sccb_write_Reg8Data8(OV7675_ID, 0x88, 0xEC);
		sccb_write_Reg8Data8(OV7675_ID, 0x89, 0xF9);
		sccb_write_Reg8Data8(OV7675_ID, 0x09, 0x00);
		sccb_write_Reg8Data8(OV7675_ID, 0x92, 0x3F);
	}           
	
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif
#ifdef	__OV2643_DRV_C__
//====================================================================================================
//	Description:	OV2643 Initialization
//	Syntax:			void OV2643_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV2643_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
	// input clock : 6~27MHz or 56MHz 
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)						// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	R_CSI_TG_HRATIO = 0;							//no scaler					
	R_CSI_TG_VRATIO = 0;

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	sccb_write_Reg8Data8(OV2643_ID, 0x12, 0x80);
	sccb_delay(200);
				
	if (nWidthH == 400 && nWidthV == 300)
	{	//CIF 400x300, 30fps 
		sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xc4, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x48); 
		sccb_write_Reg8Data8(OV2643_ID, 0xdd, 0xa5);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x10, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0x11, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x14);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		
		sccb_write_Reg8Data8(OV2643_ID, 0x20, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0x21, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0x22, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x23, 0x06);
		sccb_write_Reg8Data8(OV2643_ID, 0x24, 0x32);
		sccb_write_Reg8Data8(OV2643_ID, 0x26, 0x25);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x84);
		sccb_write_Reg8Data8(OV2643_ID, 0x29, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x2a, 0xdc);
		sccb_write_Reg8Data8(OV2643_ID, 0x2b, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x2c, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x1d, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x25, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x84);
		sccb_write_Reg8Data8(OV2643_ID, 0x28, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x12, 0x3a);	//mirror and flip
		sccb_write_Reg8Data8(OV2643_ID, 0x39, 0xd0);
		sccb_write_Reg8Data8(OV2643_ID, 0xcd, 0x13);	
		
		sccb_write_Reg8Data8(OV2643_ID, 0x13, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x14, 0xa7);
		sccb_write_Reg8Data8(OV2643_ID, 0x15, 0x42);
		sccb_write_Reg8Data8(OV2643_ID, 0x3c, 0xa4);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x60);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x50);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0xe2);
		sccb_write_Reg8Data8(OV2643_ID, 0x37, 0xe8);
		sccb_write_Reg8Data8(OV2643_ID, 0x16, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0x43, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x40, 0xfb);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x2f, 0xec);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0d, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0xd0, 0x93);
		sccb_write_Reg8Data8(OV2643_ID, 0xdc, 0x2b);
		sccb_write_Reg8Data8(OV2643_ID, 0xd9, 0x41);
		sccb_write_Reg8Data8(OV2643_ID, 0xd3, 0x02);
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0xde, 0x7c);
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x2c);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x24);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0x71);
		sccb_write_Reg8Data8(OV2643_ID, 0x9b, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x9c, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9d, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9e, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x65, 0x12);
		sccb_write_Reg8Data8(OV2643_ID, 0x66, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x67, 0x39);
		sccb_write_Reg8Data8(OV2643_ID, 0x68, 0x4e);
		sccb_write_Reg8Data8(OV2643_ID, 0x69, 0x62);
		sccb_write_Reg8Data8(OV2643_ID, 0x6a, 0x74);
		sccb_write_Reg8Data8(OV2643_ID, 0x6b, 0x85);
		sccb_write_Reg8Data8(OV2643_ID, 0x6c, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x6d, 0x9e);
		sccb_write_Reg8Data8(OV2643_ID, 0x6e, 0xb2);
		sccb_write_Reg8Data8(OV2643_ID, 0x6f, 0xc0);
		sccb_write_Reg8Data8(OV2643_ID, 0x70, 0xcc);
		sccb_write_Reg8Data8(OV2643_ID, 0x71, 0xe0);
		sccb_write_Reg8Data8(OV2643_ID, 0x72, 0xee);
		sccb_write_Reg8Data8(OV2643_ID, 0x73, 0xf6);
		sccb_write_Reg8Data8(OV2643_ID, 0x74, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0xab, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0xac, 0x5b);
		sccb_write_Reg8Data8(OV2643_ID, 0xad, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0xae, 0x1b);
		sccb_write_Reg8Data8(OV2643_ID, 0xaf, 0x76);
		sccb_write_Reg8Data8(OV2643_ID, 0xb0, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb1, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb2, 0x8c);
		sccb_write_Reg8Data8(OV2643_ID, 0xb3, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xb4, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0xbC, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x4d, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x4e, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x4f, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x50, 0x56);
		sccb_write_Reg8Data8(OV2643_ID, 0x51, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x52, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x53, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x54, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x55, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x56, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x57, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x58, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x59, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x5a, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x5b, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x5c, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x5d, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x5e, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x5f, 0x00);
		
		sccb_write_Reg8Data8(OV2643_ID, 0x60, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x41, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xb5, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0xb6, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0xb9, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0xba, 0x28);
		sccb_write_Reg8Data8(OV2643_ID, 0xbf, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0xc0, 0x3e);
		sccb_write_Reg8Data8(OV2643_ID, 0xa3, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa4, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa5, 0x09);
		sccb_write_Reg8Data8(OV2643_ID, 0xa6, 0x16);
		sccb_write_Reg8Data8(OV2643_ID, 0x9f, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa0, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa7, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa8, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa1, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xa2, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xaa, 0xa6);
		sccb_write_Reg8Data8(OV2643_ID, 0x75, 0x6a);
		sccb_write_Reg8Data8(OV2643_ID, 0x76, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0x77, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x78, 0x21);
		sccb_write_Reg8Data8(OV2643_ID, 0x79, 0xe1);
		sccb_write_Reg8Data8(OV2643_ID, 0x7a, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x7c, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x7d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7e, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7f, 0x7c);
		sccb_write_Reg8Data8(OV2643_ID, 0x80, 0x58);
		sccb_write_Reg8Data8(OV2643_ID, 0x81, 0x2a);
		sccb_write_Reg8Data8(OV2643_ID, 0x82, 0xc5);
		sccb_write_Reg8Data8(OV2643_ID, 0x83, 0x46);
		sccb_write_Reg8Data8(OV2643_ID, 0x84, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x85, 0x54);
		sccb_write_Reg8Data8(OV2643_ID, 0x86, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x87, 0xf8);
		sccb_write_Reg8Data8(OV2643_ID, 0x88, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x89, 0x70);
		sccb_write_Reg8Data8(OV2643_ID, 0x8a, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x8b, 0xf0); 
	}                      
	else if(nWidthH == 800 && nWidthV == 600)
	{	//SVGA               
		sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xc4, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x48); 
		sccb_write_Reg8Data8(OV2643_ID, 0xdd, 0xa5);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x10, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0x11, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x14);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x20, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0x21, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0x22, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x23, 0x06);
		sccb_write_Reg8Data8(OV2643_ID, 0x24, 0x32);
		sccb_write_Reg8Data8(OV2643_ID, 0x26, 0x25);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x84);
		sccb_write_Reg8Data8(OV2643_ID, 0x29, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x2a, 0xdc);
		sccb_write_Reg8Data8(OV2643_ID, 0x2b, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x2c, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x1d, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x25, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x84);
		sccb_write_Reg8Data8(OV2643_ID, 0x28, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x12, 0x39);	//mirror and flip
		sccb_write_Reg8Data8(OV2643_ID, 0x39, 0xd0);
		sccb_write_Reg8Data8(OV2643_ID, 0xcd, 0x13);	
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x13, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x14, 0xa7);
		sccb_write_Reg8Data8(OV2643_ID, 0x15, 0x42);
		sccb_write_Reg8Data8(OV2643_ID, 0x3c, 0xa4);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x60);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x50);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0xe2);
		sccb_write_Reg8Data8(OV2643_ID, 0x37, 0xe8);
		sccb_write_Reg8Data8(OV2643_ID, 0x16, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0x43, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x40, 0xfb);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x2f, 0xec);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0d, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0xd0, 0x93);
		sccb_write_Reg8Data8(OV2643_ID, 0xdc, 0x2b);
		sccb_write_Reg8Data8(OV2643_ID, 0xd9, 0x41);
		sccb_write_Reg8Data8(OV2643_ID, 0xd3, 0x02);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0xde, 0x7c);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x2c);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x24);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0x71);
		sccb_write_Reg8Data8(OV2643_ID, 0x9b, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x9c, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9d, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9e, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x65, 0x12);
		sccb_write_Reg8Data8(OV2643_ID, 0x66, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x67, 0x39);
		sccb_write_Reg8Data8(OV2643_ID, 0x68, 0x4e);
		sccb_write_Reg8Data8(OV2643_ID, 0x69, 0x62);
		sccb_write_Reg8Data8(OV2643_ID, 0x6a, 0x74);
		sccb_write_Reg8Data8(OV2643_ID, 0x6b, 0x85);
		sccb_write_Reg8Data8(OV2643_ID, 0x6c, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x6d, 0x9e);
		sccb_write_Reg8Data8(OV2643_ID, 0x6e, 0xb2);
		sccb_write_Reg8Data8(OV2643_ID, 0x6f, 0xc0);
		sccb_write_Reg8Data8(OV2643_ID, 0x70, 0xcc);
		sccb_write_Reg8Data8(OV2643_ID, 0x71, 0xe0);
		sccb_write_Reg8Data8(OV2643_ID, 0x72, 0xee);
		sccb_write_Reg8Data8(OV2643_ID, 0x73, 0xf6);
		sccb_write_Reg8Data8(OV2643_ID, 0x74, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0xab, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0xac, 0x5b);
		sccb_write_Reg8Data8(OV2643_ID, 0xad, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0xae, 0x1b);
		sccb_write_Reg8Data8(OV2643_ID, 0xaf, 0x76);
		sccb_write_Reg8Data8(OV2643_ID, 0xb0, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb1, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb2, 0x8c);
		sccb_write_Reg8Data8(OV2643_ID, 0xb3, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xb4, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0xbC, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x4d, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x4e, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x4f, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x50, 0x56);
		sccb_write_Reg8Data8(OV2643_ID, 0x51, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x52, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x53, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x54, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x55, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x56, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x57, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x58, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x59, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x5a, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x5b, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x5c, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x5d, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x5e, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x5f, 0x00);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0x60, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x41, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xb5, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0xb6, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0xb9, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0xba, 0x28);
		sccb_write_Reg8Data8(OV2643_ID, 0xbf, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0xc0, 0x3e);
		sccb_write_Reg8Data8(OV2643_ID, 0xa3, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa4, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa5, 0x09);
		sccb_write_Reg8Data8(OV2643_ID, 0xa6, 0x16);
		sccb_write_Reg8Data8(OV2643_ID, 0x9f, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa0, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa7, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa8, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa1, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xa2, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xaa, 0xa6);
		sccb_write_Reg8Data8(OV2643_ID, 0x75, 0x6a);
		sccb_write_Reg8Data8(OV2643_ID, 0x76, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0x77, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x78, 0x21);
		sccb_write_Reg8Data8(OV2643_ID, 0x79, 0xe1);
		sccb_write_Reg8Data8(OV2643_ID, 0x7a, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x7c, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x7d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7e, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7f, 0x7c);
		sccb_write_Reg8Data8(OV2643_ID, 0x80, 0x58);
		sccb_write_Reg8Data8(OV2643_ID, 0x81, 0x2a);
		sccb_write_Reg8Data8(OV2643_ID, 0x82, 0xc5);
		sccb_write_Reg8Data8(OV2643_ID, 0x83, 0x46);
		sccb_write_Reg8Data8(OV2643_ID, 0x84, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x85, 0x54);
		sccb_write_Reg8Data8(OV2643_ID, 0x86, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x87, 0xf8);
		sccb_write_Reg8Data8(OV2643_ID, 0x88, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x89, 0x70);
		sccb_write_Reg8Data8(OV2643_ID, 0x8a, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x8b, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x24); //15fps
		//driving capability for 30fps
		//sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0xdf); 
	}
	else if(nWidthH == 1280 && nWidthV == 720)
	{	//HD720P
		sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xc4, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x48); 
		sccb_write_Reg8Data8(OV2643_ID, 0xdd, 0xa5);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x10, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0x11, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x14);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x20, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0x21, 0x25);
		sccb_write_Reg8Data8(OV2643_ID, 0x22, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x23, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0x24, 0x50);
		sccb_write_Reg8Data8(OV2643_ID, 0x26, 0x2d);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x29, 0x06);
		sccb_write_Reg8Data8(OV2643_ID, 0x2a, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x2b, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x2c, 0xee);
		sccb_write_Reg8Data8(OV2643_ID, 0x1d, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x25, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x27, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x28, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x12, 0x78);	//mirror and flip
		sccb_write_Reg8Data8(OV2643_ID, 0x39, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xcd, 0x12);	
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x13, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x14, 0xa7);
		sccb_write_Reg8Data8(OV2643_ID, 0x15, 0x42);
		sccb_write_Reg8Data8(OV2643_ID, 0x3c, 0xa4);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x60);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x50);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0xe2);
		sccb_write_Reg8Data8(OV2643_ID, 0x37, 0xe8);
		sccb_write_Reg8Data8(OV2643_ID, 0x16, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0x43, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x40, 0xfb);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x2f, 0xec);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0d, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0xd0, 0x93);
		sccb_write_Reg8Data8(OV2643_ID, 0xdc, 0x2b);
		sccb_write_Reg8Data8(OV2643_ID, 0xd9, 0x41);
		sccb_write_Reg8Data8(OV2643_ID, 0xd3, 0x02);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x2c);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x24);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0x71);
		sccb_write_Reg8Data8(OV2643_ID, 0x9b, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x9c, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9d, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9e, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x65, 0x12);
		sccb_write_Reg8Data8(OV2643_ID, 0x66, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x67, 0x39);
		sccb_write_Reg8Data8(OV2643_ID, 0x68, 0x4e);
		sccb_write_Reg8Data8(OV2643_ID, 0x69, 0x62);
		sccb_write_Reg8Data8(OV2643_ID, 0x6a, 0x74);
		sccb_write_Reg8Data8(OV2643_ID, 0x6b, 0x85);
		sccb_write_Reg8Data8(OV2643_ID, 0x6c, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x6d, 0x9e);
		sccb_write_Reg8Data8(OV2643_ID, 0x6e, 0xb2);
		sccb_write_Reg8Data8(OV2643_ID, 0x6f, 0xc0);
		sccb_write_Reg8Data8(OV2643_ID, 0x70, 0xcc);
		sccb_write_Reg8Data8(OV2643_ID, 0x71, 0xe0);
		sccb_write_Reg8Data8(OV2643_ID, 0x72, 0xee);
		sccb_write_Reg8Data8(OV2643_ID, 0x73, 0xf6);
		sccb_write_Reg8Data8(OV2643_ID, 0x74, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0xab, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0xac, 0x5b);
		sccb_write_Reg8Data8(OV2643_ID, 0xad, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0xae, 0x1b);
		sccb_write_Reg8Data8(OV2643_ID, 0xaf, 0x76);
		sccb_write_Reg8Data8(OV2643_ID, 0xb0, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb1, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb2, 0x8c);
		sccb_write_Reg8Data8(OV2643_ID, 0xb3, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xb4, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0xbC, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x4d, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x4e, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x4f, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x50, 0x56);
		sccb_write_Reg8Data8(OV2643_ID, 0x51, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x52, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x53, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x54, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x55, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x56, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x57, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x58, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x59, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x5a, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x5b, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x5c, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x5d, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x5e, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x5f, 0x00);
		                     
		sccb_write_Reg8Data8(OV2643_ID, 0x60, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x41, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xb5, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0xb6, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0xb9, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0xba, 0x28);
		sccb_write_Reg8Data8(OV2643_ID, 0xbf, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0xc0, 0x3e);
		sccb_write_Reg8Data8(OV2643_ID, 0xa3, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa4, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa5, 0x09);
		sccb_write_Reg8Data8(OV2643_ID, 0xa6, 0x16);
		sccb_write_Reg8Data8(OV2643_ID, 0x9f, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa0, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa7, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa8, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa1, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xa2, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xaa, 0xa6);
		sccb_write_Reg8Data8(OV2643_ID, 0x75, 0x6a);
		sccb_write_Reg8Data8(OV2643_ID, 0x76, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0x77, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x78, 0x21);
		sccb_write_Reg8Data8(OV2643_ID, 0x79, 0xe1);
		sccb_write_Reg8Data8(OV2643_ID, 0x7a, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x7c, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x7d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7e, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7f, 0x7c);
		sccb_write_Reg8Data8(OV2643_ID, 0x80, 0x58);
		sccb_write_Reg8Data8(OV2643_ID, 0x81, 0x2a);
		sccb_write_Reg8Data8(OV2643_ID, 0x82, 0xc5);
		sccb_write_Reg8Data8(OV2643_ID, 0x83, 0x46);
		sccb_write_Reg8Data8(OV2643_ID, 0x84, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x85, 0x54);
		sccb_write_Reg8Data8(OV2643_ID, 0x86, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x87, 0xf8);
		sccb_write_Reg8Data8(OV2643_ID, 0x88, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x89, 0x70);
		sccb_write_Reg8Data8(OV2643_ID, 0x8a, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x8b, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x34); //10fps
		//driving capability for 30fps
		//sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0xdf); 
	}
	else if(nWidthH == 1600 && nWidthV == 1200)
	{	//UXGA
		sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xc4, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x48);
		sccb_write_Reg8Data8(OV2643_ID, 0xdd, 0xa5);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x10, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0x11, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x14);
		sccb_write_Reg8Data8(OV2643_ID, 0x0e, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x21, 0x25);
		sccb_write_Reg8Data8(OV2643_ID, 0x23, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0x12, 0x38);	//mirror and flip
		sccb_write_Reg8Data8(OV2643_ID, 0x39, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xcd, 0x12);	
		sccb_write_Reg8Data8(OV2643_ID, 0x13, 0xff);
		sccb_write_Reg8Data8(OV2643_ID, 0x14, 0xa7);
		sccb_write_Reg8Data8(OV2643_ID, 0x15, 0x42);
		sccb_write_Reg8Data8(OV2643_ID, 0x3c, 0xa4);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x60);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x50);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0xe2);
		sccb_write_Reg8Data8(OV2643_ID, 0x37, 0xe8);
		sccb_write_Reg8Data8(OV2643_ID, 0x16, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0x43, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x40, 0xfb);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x2f, 0xec);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x0d, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0xd0, 0x93);
		sccb_write_Reg8Data8(OV2643_ID, 0xdc, 0x2b);
		sccb_write_Reg8Data8(OV2643_ID, 0xd9, 0x41);
		sccb_write_Reg8Data8(OV2643_ID, 0xd3, 0x02);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0x3d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x0c, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x18, 0x2c);
		sccb_write_Reg8Data8(OV2643_ID, 0x19, 0x24);
		sccb_write_Reg8Data8(OV2643_ID, 0x1a, 0x71);
		sccb_write_Reg8Data8(OV2643_ID, 0x9b, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x9c, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9d, 0x7d);
		sccb_write_Reg8Data8(OV2643_ID, 0x9e, 0x69);
		sccb_write_Reg8Data8(OV2643_ID, 0x35, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x36, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0x65, 0x12);
		sccb_write_Reg8Data8(OV2643_ID, 0x66, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x67, 0x39);
		sccb_write_Reg8Data8(OV2643_ID, 0x68, 0x4e);
		sccb_write_Reg8Data8(OV2643_ID, 0x69, 0x62);
		sccb_write_Reg8Data8(OV2643_ID, 0x6a, 0x74);
		sccb_write_Reg8Data8(OV2643_ID, 0x6b, 0x85);
		sccb_write_Reg8Data8(OV2643_ID, 0x6c, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x6d, 0x9e);
		sccb_write_Reg8Data8(OV2643_ID, 0x6e, 0xb2);
		sccb_write_Reg8Data8(OV2643_ID, 0x6f, 0xc0);
		sccb_write_Reg8Data8(OV2643_ID, 0x70, 0xcc);
		sccb_write_Reg8Data8(OV2643_ID, 0x71, 0xe0);
		sccb_write_Reg8Data8(OV2643_ID, 0x72, 0xee);
		sccb_write_Reg8Data8(OV2643_ID, 0x73, 0xf6);
		sccb_write_Reg8Data8(OV2643_ID, 0x74, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0xab, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0xac, 0x5b);
		sccb_write_Reg8Data8(OV2643_ID, 0xad, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0xae, 0x1b);
		sccb_write_Reg8Data8(OV2643_ID, 0xaf, 0x76);
		sccb_write_Reg8Data8(OV2643_ID, 0xb0, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb1, 0x90);
		sccb_write_Reg8Data8(OV2643_ID, 0xb2, 0x8c);
		sccb_write_Reg8Data8(OV2643_ID, 0xb3, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xb4, 0x98);
		sccb_write_Reg8Data8(OV2643_ID, 0xbC, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x4d, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x4e, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x4f, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x50, 0x56);
		sccb_write_Reg8Data8(OV2643_ID, 0x51, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x52, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x53, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x54, 0x30);
		sccb_write_Reg8Data8(OV2643_ID, 0x55, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x56, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x57, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0x58, 0x00);
		sccb_write_Reg8Data8(OV2643_ID, 0x59, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x5a, 0x03);
		sccb_write_Reg8Data8(OV2643_ID, 0x5b, 0x20);
		sccb_write_Reg8Data8(OV2643_ID, 0x5c, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x5d, 0x5c);
		sccb_write_Reg8Data8(OV2643_ID, 0x5e, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x5f, 0x00);
		                    
		sccb_write_Reg8Data8(OV2643_ID, 0x60, 0x66);
		sccb_write_Reg8Data8(OV2643_ID, 0x41, 0x1f);
		sccb_write_Reg8Data8(OV2643_ID, 0xb5, 0x01);
		sccb_write_Reg8Data8(OV2643_ID, 0xb6, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0xb9, 0x40);
		sccb_write_Reg8Data8(OV2643_ID, 0xba, 0x28);
		sccb_write_Reg8Data8(OV2643_ID, 0xbf, 0x0c);
		sccb_write_Reg8Data8(OV2643_ID, 0xc0, 0x3e);
		sccb_write_Reg8Data8(OV2643_ID, 0xa3, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa4, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa5, 0x09);
		sccb_write_Reg8Data8(OV2643_ID, 0xa6, 0x16);
		sccb_write_Reg8Data8(OV2643_ID, 0x9f, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa0, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa7, 0x0a);
		sccb_write_Reg8Data8(OV2643_ID, 0xa8, 0x0f);
		sccb_write_Reg8Data8(OV2643_ID, 0xa1, 0x10);
		sccb_write_Reg8Data8(OV2643_ID, 0xa2, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xa9, 0x04);
		sccb_write_Reg8Data8(OV2643_ID, 0xaa, 0xa6);
		sccb_write_Reg8Data8(OV2643_ID, 0x75, 0x6a);
		sccb_write_Reg8Data8(OV2643_ID, 0x76, 0x11);
		sccb_write_Reg8Data8(OV2643_ID, 0x77, 0x92);
		sccb_write_Reg8Data8(OV2643_ID, 0x78, 0x21);
		sccb_write_Reg8Data8(OV2643_ID, 0x79, 0xe1);
		sccb_write_Reg8Data8(OV2643_ID, 0x7a, 0x02);
		sccb_write_Reg8Data8(OV2643_ID, 0x7c, 0x05);
		sccb_write_Reg8Data8(OV2643_ID, 0x7d, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7e, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x7f, 0x7c);
		sccb_write_Reg8Data8(OV2643_ID, 0x80, 0x58);
		sccb_write_Reg8Data8(OV2643_ID, 0x81, 0x2a);
		sccb_write_Reg8Data8(OV2643_ID, 0x82, 0xc5);
		sccb_write_Reg8Data8(OV2643_ID, 0x83, 0x46);
		sccb_write_Reg8Data8(OV2643_ID, 0x84, 0x3a);
		sccb_write_Reg8Data8(OV2643_ID, 0x85, 0x54);
		sccb_write_Reg8Data8(OV2643_ID, 0x86, 0x44);
		sccb_write_Reg8Data8(OV2643_ID, 0x87, 0xf8);
		sccb_write_Reg8Data8(OV2643_ID, 0x88, 0x08);
		sccb_write_Reg8Data8(OV2643_ID, 0x89, 0x70);
		sccb_write_Reg8Data8(OV2643_ID, 0x8a, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x8b, 0xf0);
		sccb_write_Reg8Data8(OV2643_ID, 0x0f, 0x34); //5fps
		//driving capability for 15fps
		//sccb_write_Reg8Data8(OV2643_ID, 0xc3, 0xdf);	
	}           
			          
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif	        
}               
#endif          

#ifdef	__OV2659_DRV_C__
//====================================================================================================
//	Description:	OV2659 Initialization
//	Syntax:			void OV2659_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void OV2659_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag			// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;								// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555) {									// RGB1555?
		uCtrlReg2 |= CSI_RGB1555;
	}
	
	if (uFlag & FT_CSI_CCIR656)	{									// CCIR656?
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	} else {
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	
	if (uFlag & FT_CSI_YUVIN) {										// YUVIN?
		uCtrlReg1 |= YUVIN;
	}
	
	if (uFlag & FT_CSI_YUVOUT) {									// YUVOUT?
		uCtrlReg1 |= YUVOUT;
	}
	
	// Whether compression or not?
	R_CSI_TG_HRATIO = 0;					
	R_CSI_TG_VRATIO = 0;					

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
			
	if(nWidthH == 800 && nWidthV == 600) {
		//@@ YUV SVGA 800x600 15fps
		sccb_write_Reg16Data8(OV2659_ID, 0x0103, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x3000, 0x0f);
		sccb_write_Reg16Data8(OV2659_ID, 0x3001, 0xff);
		sccb_write_Reg16Data8(OV2659_ID, 0x3002, 0xff);
		sccb_write_Reg16Data8(OV2659_ID, 0x0100, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x3633, 0x3d);
		sccb_write_Reg16Data8(OV2659_ID, 0x3620, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x3631, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x3612, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3630, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x4702, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x370c, 0x34);
		sccb_write_Reg16Data8(OV2659_ID, 0x3004, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x3005, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x3800, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3801, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3802, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3803, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3804, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3805, 0x5f);
		sccb_write_Reg16Data8(OV2659_ID, 0x3806, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3807, 0xb7);
		sccb_write_Reg16Data8(OV2659_ID, 0x3808, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x3809, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x380a, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x380b, 0x58);
		sccb_write_Reg16Data8(OV2659_ID, 0x380c, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x380d, 0x14);
		sccb_write_Reg16Data8(OV2659_ID, 0x380e, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x380f, 0x68);
		sccb_write_Reg16Data8(OV2659_ID, 0x3811, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x3813, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x3814, 0x31);
		sccb_write_Reg16Data8(OV2659_ID, 0x3815, 0x31);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a02, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a03, 0x68);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a08, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a09, 0x5c);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0a, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0b, 0x4d);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0d, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0e, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a14, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a15, 0x28);
		sccb_write_Reg16Data8(OV2659_ID, 0x3623, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3634, 0x76);
		sccb_write_Reg16Data8(OV2659_ID, 0x3701, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x3702, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x3703, 0x24);
		sccb_write_Reg16Data8(OV2659_ID, 0x3704, 0x24);
		sccb_write_Reg16Data8(OV2659_ID, 0x3705, 0x0c);
		sccb_write_Reg16Data8(OV2659_ID, 0x3820, 0x81);
		sccb_write_Reg16Data8(OV2659_ID, 0x3821, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x370a, 0x52);
		sccb_write_Reg16Data8(OV2659_ID, 0x4608, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x4609, 0x80);
		sccb_write_Reg16Data8(OV2659_ID, 0x4300, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x5086, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x5000, 0xfb);
		sccb_write_Reg16Data8(OV2659_ID, 0x5001, 0x1f);
		sccb_write_Reg16Data8(OV2659_ID, 0x5002, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5025, 0x0e);
		sccb_write_Reg16Data8(OV2659_ID, 0x5026, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x5027, 0x34);
		sccb_write_Reg16Data8(OV2659_ID, 0x5028, 0x4c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5029, 0x62);
		sccb_write_Reg16Data8(OV2659_ID, 0x502a, 0x74);
		sccb_write_Reg16Data8(OV2659_ID, 0x502b, 0x85);
		sccb_write_Reg16Data8(OV2659_ID, 0x502c, 0x92);
		sccb_write_Reg16Data8(OV2659_ID, 0x502d, 0x9e);
		sccb_write_Reg16Data8(OV2659_ID, 0x502e, 0xb2);
		sccb_write_Reg16Data8(OV2659_ID, 0x502f, 0xc0);
		sccb_write_Reg16Data8(OV2659_ID, 0x5030, 0xcc);
		sccb_write_Reg16Data8(OV2659_ID, 0x5031, 0xe0);
		sccb_write_Reg16Data8(OV2659_ID, 0x5032, 0xee);
		sccb_write_Reg16Data8(OV2659_ID, 0x5033, 0xf6);
		sccb_write_Reg16Data8(OV2659_ID, 0x5034, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x5070, 0x1c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5071, 0x5b);
		sccb_write_Reg16Data8(OV2659_ID, 0x5072, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x5073, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x5074, 0x94);
		sccb_write_Reg16Data8(OV2659_ID, 0x5075, 0xb4);
		sccb_write_Reg16Data8(OV2659_ID, 0x5076, 0xb4);
		sccb_write_Reg16Data8(OV2659_ID, 0x5077, 0xaf);
		sccb_write_Reg16Data8(OV2659_ID, 0x5078, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x5079, 0x98);
		sccb_write_Reg16Data8(OV2659_ID, 0x507a, 0x21);
		sccb_write_Reg16Data8(OV2659_ID, 0x5035, 0x6a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5036, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x5037, 0x92);
		sccb_write_Reg16Data8(OV2659_ID, 0x5038, 0x21);
		sccb_write_Reg16Data8(OV2659_ID, 0x5039, 0xe1);
		sccb_write_Reg16Data8(OV2659_ID, 0x503a, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x503c, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x503d, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x503e, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x503f, 0x64);
		sccb_write_Reg16Data8(OV2659_ID, 0x5040, 0x58);
		sccb_write_Reg16Data8(OV2659_ID, 0x5041, 0x2a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5042, 0xc5);
		sccb_write_Reg16Data8(OV2659_ID, 0x5043, 0x2e);
		sccb_write_Reg16Data8(OV2659_ID, 0x5044, 0x3a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5045, 0x3c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5046, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x5047, 0xf8);
		sccb_write_Reg16Data8(OV2659_ID, 0x5048, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5049, 0x70);
		sccb_write_Reg16Data8(OV2659_ID, 0x504a, 0xf0);
		sccb_write_Reg16Data8(OV2659_ID, 0x504b, 0xf0);
		sccb_write_Reg16Data8(OV2659_ID, 0x500c, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x500d, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x500e, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x500f, 0x5c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5010, 0x48);
		sccb_write_Reg16Data8(OV2659_ID, 0x5011, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5012, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x5013, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x5014, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x5015, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x5016, 0x7c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5017, 0x40);
		sccb_write_Reg16Data8(OV2659_ID, 0x5018, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5019, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x501a, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x501b, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x501c, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x501d, 0x7c);
		sccb_write_Reg16Data8(OV2659_ID, 0x501e, 0x3a);
		sccb_write_Reg16Data8(OV2659_ID, 0x501f, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5020, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x506e, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x5064, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5065, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x5066, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x5067, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x506c, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x506d, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x506f, 0xa6);
		sccb_write_Reg16Data8(OV2659_ID, 0x5068, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5069, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x506a, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x506b, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x507e, 0x40);
		sccb_write_Reg16Data8(OV2659_ID, 0x507f, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x507b, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x507a, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x5084, 0x0c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5085, 0x3e);
		sccb_write_Reg16Data8(OV2659_ID, 0x5005, 0x80);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0f, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a10, 0x28);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1b, 0x32);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1e, 0x26);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a11, 0x60);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1f, 0x14);
		sccb_write_Reg16Data8(OV2659_ID, 0x5060, 0x69);
		sccb_write_Reg16Data8(OV2659_ID, 0x5061, 0x7d);
		sccb_write_Reg16Data8(OV2659_ID, 0x5062, 0x7d);
		sccb_write_Reg16Data8(OV2659_ID, 0x5063, 0x69);

		sccb_write_Reg16Data8(OV2659_ID, 0x3004, 0x20);
	} else {
		//@@ YUV Full 1600x1200 5fps
		sccb_write_Reg16Data8(OV2659_ID, 0x0103, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x3000, 0x0f);
		sccb_write_Reg16Data8(OV2659_ID, 0x3001, 0xff);
		sccb_write_Reg16Data8(OV2659_ID, 0x3002, 0xff);
		sccb_write_Reg16Data8(OV2659_ID, 0x0100, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x3633, 0x3d);
		sccb_write_Reg16Data8(OV2659_ID, 0x3620, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x3631, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x3612, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3630, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x4702, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x370c, 0x34);
		sccb_write_Reg16Data8(OV2659_ID, 0x3004, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x3005, 0x24);
		sccb_write_Reg16Data8(OV2659_ID, 0x3800, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3801, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3802, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3803, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3804, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3805, 0x5f);
		sccb_write_Reg16Data8(OV2659_ID, 0x3806, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3807, 0xbb);
		sccb_write_Reg16Data8(OV2659_ID, 0x3808, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3809, 0x40);
		sccb_write_Reg16Data8(OV2659_ID, 0x380a, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x380b, 0xb0);
		sccb_write_Reg16Data8(OV2659_ID, 0x380c, 0x07);
		sccb_write_Reg16Data8(OV2659_ID, 0x380d, 0x9f);
		sccb_write_Reg16Data8(OV2659_ID, 0x380e, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x380f, 0xd0);
		sccb_write_Reg16Data8(OV2659_ID, 0x3811, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x3813, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3814, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x3815, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a02, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a03, 0xd0);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a08, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a09, 0xb8);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0a, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0b, 0x9a);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0d, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0e, 0x06);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a14, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a15, 0x50);
		sccb_write_Reg16Data8(OV2659_ID, 0x3623, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x3701, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x3702, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x3703, 0x48);
		sccb_write_Reg16Data8(OV2659_ID, 0x3704, 0x48);
		sccb_write_Reg16Data8(OV2659_ID, 0x3705, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x3820, 0x80);
		sccb_write_Reg16Data8(OV2659_ID, 0x3821, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x370a, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x4608, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x4609, 0x80);
		sccb_write_Reg16Data8(OV2659_ID, 0x4300, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x5086, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x5000, 0xfb);
		sccb_write_Reg16Data8(OV2659_ID, 0x5001, 0x1f);
		sccb_write_Reg16Data8(OV2659_ID, 0x5002, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5025, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x5026, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x5027, 0x39);
		sccb_write_Reg16Data8(OV2659_ID, 0x5028, 0x4e);
		sccb_write_Reg16Data8(OV2659_ID, 0x5029, 0x62);
		sccb_write_Reg16Data8(OV2659_ID, 0x502a, 0x74);
		sccb_write_Reg16Data8(OV2659_ID, 0x502b, 0x85);
		sccb_write_Reg16Data8(OV2659_ID, 0x502c, 0x92);
		sccb_write_Reg16Data8(OV2659_ID, 0x502d, 0x9e);
		sccb_write_Reg16Data8(OV2659_ID, 0x502e, 0xb2);
		sccb_write_Reg16Data8(OV2659_ID, 0x502f, 0xc0);
		sccb_write_Reg16Data8(OV2659_ID, 0x5030, 0xcc);
		sccb_write_Reg16Data8(OV2659_ID, 0x5031, 0xe0);
		sccb_write_Reg16Data8(OV2659_ID, 0x5032, 0xee);
		sccb_write_Reg16Data8(OV2659_ID, 0x5033, 0xf6);
		sccb_write_Reg16Data8(OV2659_ID, 0x5034, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x5070, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x5071, 0x5b);
		sccb_write_Reg16Data8(OV2659_ID, 0x5072, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x5073, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x5074, 0x94);
		sccb_write_Reg16Data8(OV2659_ID, 0x5075, 0xb4);
		sccb_write_Reg16Data8(OV2659_ID, 0x5076, 0xb4);
		sccb_write_Reg16Data8(OV2659_ID, 0x5077, 0xaf);
		sccb_write_Reg16Data8(OV2659_ID, 0x5078, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x5079, 0x98);
		sccb_write_Reg16Data8(OV2659_ID, 0x507a, 0x21);
		sccb_write_Reg16Data8(OV2659_ID, 0x5035, 0x6a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5036, 0x11);
		sccb_write_Reg16Data8(OV2659_ID, 0x5037, 0x92);
		sccb_write_Reg16Data8(OV2659_ID, 0x5038, 0x21);
		sccb_write_Reg16Data8(OV2659_ID, 0x5039, 0xe1);
		sccb_write_Reg16Data8(OV2659_ID, 0x503a, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x503c, 0x05);
		sccb_write_Reg16Data8(OV2659_ID, 0x503d, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x503e, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x503f, 0x64);
		sccb_write_Reg16Data8(OV2659_ID, 0x5040, 0x58);
		sccb_write_Reg16Data8(OV2659_ID, 0x5041, 0x2a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5042, 0xc5);
		sccb_write_Reg16Data8(OV2659_ID, 0x5043, 0x2e);
		sccb_write_Reg16Data8(OV2659_ID, 0x5044, 0x3a);
		sccb_write_Reg16Data8(OV2659_ID, 0x5045, 0x3c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5046, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x5047, 0xf8);
		sccb_write_Reg16Data8(OV2659_ID, 0x5048, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5049, 0x70);
		sccb_write_Reg16Data8(OV2659_ID, 0x504a, 0xf0);
		sccb_write_Reg16Data8(OV2659_ID, 0x504b, 0xf0);
		sccb_write_Reg16Data8(OV2659_ID, 0x500c, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x500d, 0x20);
		sccb_write_Reg16Data8(OV2659_ID, 0x500e, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x500f, 0x5c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5010, 0x48);
		sccb_write_Reg16Data8(OV2659_ID, 0x5011, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5012, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x5013, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x5014, 0x30);
		sccb_write_Reg16Data8(OV2659_ID, 0x5015, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x5016, 0x7c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5017, 0x40);
		sccb_write_Reg16Data8(OV2659_ID, 0x5018, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5019, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x501a, 0x03);
		sccb_write_Reg16Data8(OV2659_ID, 0x501b, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x501c, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x501d, 0x7c);
		sccb_write_Reg16Data8(OV2659_ID, 0x501e, 0x3a);
		sccb_write_Reg16Data8(OV2659_ID, 0x501f, 0x00);
		sccb_write_Reg16Data8(OV2659_ID, 0x5020, 0x66);
		sccb_write_Reg16Data8(OV2659_ID, 0x506e, 0x44);
		sccb_write_Reg16Data8(OV2659_ID, 0x5064, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5065, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x5066, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x5067, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x506c, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x506d, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x506f, 0xa6);
		sccb_write_Reg16Data8(OV2659_ID, 0x5068, 0x08);
		sccb_write_Reg16Data8(OV2659_ID, 0x5069, 0x10);
		sccb_write_Reg16Data8(OV2659_ID, 0x506a, 0x04);
		sccb_write_Reg16Data8(OV2659_ID, 0x506b, 0x12);
		sccb_write_Reg16Data8(OV2659_ID, 0x507e, 0x40);
		sccb_write_Reg16Data8(OV2659_ID, 0x507f, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x507b, 0x02);
		sccb_write_Reg16Data8(OV2659_ID, 0x507a, 0x01);
		sccb_write_Reg16Data8(OV2659_ID, 0x5084, 0x0c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5085, 0x7c);
		sccb_write_Reg16Data8(OV2659_ID, 0x5005, 0x80);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a0f, 0x34);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a10, 0x2c);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1b, 0x36);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1e, 0x2a);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a11, 0x70);
		sccb_write_Reg16Data8(OV2659_ID, 0x3a1f, 0x18);
		sccb_write_Reg16Data8(OV2659_ID, 0x5060, 0x69);
		sccb_write_Reg16Data8(OV2659_ID, 0x5061, 0x7d);
		sccb_write_Reg16Data8(OV2659_ID, 0x5062, 0x7d);
		sccb_write_Reg16Data8(OV2659_ID, 0x5063, 0x69);
		                                       
		sccb_write_Reg16Data8(OV2659_ID, 0x3004, 0x30);
	}                     
	      		          
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif        
}               
#endif
                
static INT32U gFrameBuffA;
static INT32U gFrameBuffB;
static INT16U gOidSensorChipVersion = OID_SENSOR_CHIP_VERSION_UNKNOWN;

#ifdef	__OID_SENSOR_DRV_C__
typedef struct
{
	INT8U reg;
	INT16U val;
} SensorRegVal_T;

// [TODO] A601 is not tested yet
static SensorRegVal_T A601_CMD[] = {
	{ 0x0d,	0x0001 },
	{ 0x0d,	0x0000 },	
	{ 0x01,	0x0002 },	
	{ 0x02,	0x0000 },
	{ 0x03,	0x0066 },	
	{ 0x04,	0x0080 },	
	{ 0x05,	0x0018 },
	{ 0x06,	0x0008 },	
	{ 0x07,	0x0002 },	
	{ 0x08,	0x005F },
	{ 0x09,	0x0030 },	// init val of 0x09 depends on frame rate and AE algorithm	
	{ 0x0A,	0x0001 },	// 260Hz	
	{ 0x20,	0x1305 },
	{ 0x26,	0x3C20 },	
	{ 0x27,	0x0008 },	
	{ 0x29,	0x0800 },
	{ 0x2A,	0x0400 },	
	{ 0x2F,	0x0004 },	
	{ 0x31,	0x0604 },
	{ 0x35,	0x0020 },	// gain
	{ 0x39,	0x000F },	
	{ 0x3C,	0x0007 },
	{ 0x3D,	0x0048 },	
	{ 0x3E,	0x0020 },	// Iref gain
	{ 0x56,	0x0000 },
	{ 0x57,	0x0440 },
};

static SensorRegVal_T A600_CMD[] = {
	{ 0x0d,	0x0001 },	// Reset
	{ 0x0d,	0x0000 },	// Reset
	{ 0x03,	0x00D3 },	// Write Row window size (180-1).
						// Change from 0xB3 to 0xD3 for 144x144 image size
	{ 0x04,	0x0105 },	// Write Cloumn window size (262-1).	
	{ 0x05,	0x008F },	// Write Column fill-in.
	{ 0x06,	0x000D },	// Write Vertical blank count. 	
	{ 0x07,	0x00AA },	// Write Control mode.	
	{ 0x09,	0x0020 },	// init val of 0x09 depends on frame rate and AE algorithm	
	{ 0x0A,	0x0000 },
	{ 0x20,	0x1200 },	// Pixel read control. Hsync continues during Vblank.
	{ 0x27,	0x002C },	// Write DAC control.
	{ 0x28,	0x000B },	// Write Voltage bias.	
	{ 0x2F,	0x0005 },	// Write address of the black column and row 
	{ 0x33,	0x0000 },	// Write digital post-gain offset value.
	{ 0x35,	0x0010 },	// Write Global gain.	
	{ 0x39,	0x0007 },	// Write Controls voltage offset added to signal before gain
	{ 0x3B,	0x000A },	// Write Voltage, Vref_HI (upper)
};

//====================================================================================================
//	Description:	OID sensor initialization
//	Syntax:			void OID_Sensor_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//					);
//	Return:			None
//====================================================================================================
INT16S OID_Sensor_Init(	INT16S nWidthH, INT16S nWidthV )
{
	INT16S ret, i;

	OID_Sensor_SetEnablePin();
	OID_Sensor_SetResetPin();

	// [TODO] Reset pin is not tested yet.
	OID_Sensor_Reset(TRUE);

	// Enable CLKO to read sensor's chip version
	R_CSI_TG_CTRL1 |= 0x0080;
	sccb_init();
	sccb_delay(200);
	OID_Sensor_Enable(TRUE);
	
	gOidSensorChipVersion = OID_Sensor_Read_Register(0x00);
	if(gOidSensorChipVersion == OID_SENSOR_CHIP_VERSION_A601)
	{
		R_CSI_TG_HLSTART = 0;	// Sensor horizontal start register.
		R_CSI_TG_VL0START = 0;	// Sensor field 0 vertical latch start register.
		R_CSI_TG_VL1START = 0;

		R_CSI_TG_CTRL1 =	CSI_NOSTOP | CLK_SEL48M | CLKOEN | CLKOINV;
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ		
		R_CSI_TG_CTRL0 = 	YUV_YUYV | VRST_RISE | VADD_RISE | HRST_RISE |	YUVIN | YUVOUT | CAP | CSIEN;
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_CSI_TG_CTRL0 = 	YUV_YUYV | VRST_RISE | VADD_RISE | HRST_RISE |	YUVIN | YUVOUT | CAP | CSIEN | (1 << 16);
	#endif
		for (i=0; i<(sizeof(A601_CMD)/sizeof(SensorRegVal_T)); i++)
		{
			OID_Sensor_Write_Register(A601_CMD[i].reg, A601_CMD[i].val);
		}
		ret = 0;
	}
	else if(gOidSensorChipVersion == OID_SENSOR_CHIP_VERSION_A600)
	{
		for (i=0; i<(sizeof(A600_CMD)/sizeof(SensorRegVal_T)); i++)
			OID_Sensor_Write_Register(A600_CMD[i].reg, A600_CMD[i].val);

		// Test only. Verify the results of writing. The delay does not seem enough!
		//sccb_delay (200);
		//for (i=2; i<(sizeof(A600_CMD)/sizeof(SensorRegVal_T)); i++)
		//{
		//	data = OID_Sensor_Read_Register(A600_CMD[i].reg);
		//	if (data != A600_CMD[i].val)
		//		while (1);
		//}

		// [TODO] There is a black column at the rightmost. Need to check 0x96 value later.
		if (nWidthH == 112 && nWidthV == 100)
		{
			R_CSI_TG_HLSTART = 0x96;	// Sensor horizontal start register.
			R_CSI_TG_VL0START = 0x50;	// Sensor field 0 vertical latch start register.
		}
		else if (nWidthH == 144 && nWidthV == 144)
		{
			// [TODO] Should match A600C's OQC testing condition.
			R_CSI_TG_HLSTART = 0x60;	// Sensor horizontal start register.
			R_CSI_TG_VL0START = 0x38;	// Sensor field 0 vertical latch start register.
		}

		R_CSI_TG_VL1START = 0;
		
		R_CSI_TG_CTRL1 =	CSI_NOSTOP | CLK_SEL27M | CLKOEN | CLKOINV;
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
		R_CSI_TG_CTRL0 = 	VRST_RISE | VADD_RISE | HRST_RISE |	YUVIN | YUVOUT | CAP | CSIEN | CLKIINV;
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_CSI_TG_CTRL0 = 	VRST_RISE | VADD_RISE | HRST_RISE |	YUVIN | YUVOUT | CAP | CSIEN | CLKIINV | (1 << 16);
	#endif
		ret = 0;
	}
	else
	{
		ret = (-1);
	}

	sccb_delay (200);
	return ret;
}
#endif

//=============================================================================
// Note:
//=============================================================================
void OID_Sensor_Write_Register(INT16U addr,	INT16U data)
{
	sccb_write_Reg8Data16(OID_SENSOR_ID, addr, data);
}

//=============================================================================
// Note:
//=============================================================================
INT16U OID_Sensor_Read_Register(INT16U addr)
{
	INT16U ret;
	INT16U data;

	ret = sccb_read_Reg8Data16(OID_SENSOR_ID, addr, &data);
	return data;
}

//=============================================================================
// Note:
//=============================================================================
INT16U OID_Sensor_GetChipVersion(void)
{
	return gOidSensorChipVersion;
}

//=============================================================================
// Note:
//=============================================================================
void OID_Sensor_SetEnablePin(void)
{
  	gpio_drving_init_io(SENSOR_ENABLE,(IO_DRV_LEVEL) CMOS_DRIVING);
    gpio_set_port_attribute(SENSOR_ENABLE,1);
  	gpio_init_io(SENSOR_ENABLE, GPIO_OUTPUT);

  	// ENABLE is LOW by default so that sensor is OFF
  	gpio_write_io(SENSOR_ENABLE, DATA_LOW);
}

//=============================================================================
// Note: Enable/disable sensor itself
//=============================================================================
void OID_Sensor_Enable(BOOLEAN en)
{
	if (en)
	  	gpio_write_io(SENSOR_ENABLE, DATA_HIGH);
	else
		gpio_write_io(SENSOR_ENABLE, DATA_LOW);
}

//=============================================================================
// Note:
//=============================================================================
void OID_Sensor_SetResetPin(void)
{
#if (SENSOR_RESET != 0xFF)

	gpio_drving_init_io(SENSOR_RESET,(IO_DRV_LEVEL) CMOS_DRIVING);
	gpio_set_port_attribute(SENSOR_RESET,1);
	gpio_init_io(SENSOR_RESET, GPIO_OUTPUT);

  	// RESET is HIGH by default so that sensor is at normal state
	gpio_write_io(SENSOR_RESET, DATA_HIGH);

#endif
}

//=============================================================================
// Note: Send sensor into reset state and then back to normal state
//=============================================================================
void OID_Sensor_Reset(BOOLEAN is_back_to_normal_state)
{
#if (SENSOR_RESET != 0xFF)

	// Enter reset state
	gpio_write_io(SENSOR_RESET, DATA_LOW);

	// [TODO] should rewrite it to delay with timer instead of cpu loop
	drv_msec_wait(100);

	// Back to normal state
	if (is_back_to_normal_state)
		gpio_write_io(SENSOR_RESET, DATA_HIGH);

#endif
}

//=============================================================================
// Note: Enable/disable CSI interface
//=============================================================================
void OID_Sensor_CSI_Enable(BOOLEAN en)
{
	// It is necessary to switch frame buffer to an invalid address because
	// another image will still come in even after we clear the flag and disable
	// CSI. The evidence is that GPID_BufFlag is 9 instead of after GPID_Decode() is
	// called. If we don't switch frame buffer, GPID_Decode() will be interferred by the
	// new image and fail.
	if (en)
	{
		R_CSI_TG_CTRL0 |= 0x0001;
		OID_Sensor_ChangeFrameBuff(TRUE);
	}
	else
	{
		R_CSI_TG_CTRL0 &= ~0x0001;
		OID_Sensor_ChangeFrameBuff(FALSE);
	}
}

//=============================================================================
// Note: Enable/disable both sensor and CSI interface
//=============================================================================
void OID_Sensor_And_CSI_Enable(BOOLEAN en)
{
	OID_Sensor_Enable(en);
	OID_Sensor_CSI_Enable(en);
}

//=============================================================================
// Note: Enable/disable both sensor and CSI interface
//=============================================================================
void OID_Sensor_SetFrameBuffB(INT32U buff)
{
	gFrameBuffB = buff;
}

//=============================================================================
// Note: Enable/disable both sensor and CSI interface
//=============================================================================
void OID_Sensor_ChangeFrameBuff(BOOLEAN is_valid)
{
	if (is_valid)
		*P_CSI_TG_FBSADDR = gFrameBuffA;
	else
		*P_CSI_TG_FBSADDR = gFrameBuffB;
}

#ifdef	__BF3710_DRV_C__
//====================================================================================================
//	Description:	BF3710 Initialization
//	Syntax:			void BF3710_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
static INT8U BF3710_reset_table[] = 
{
	0x12, 0x80,
	0x12, 0x00
};

static INT8U BF3710_init_table[] =
{	
	0x09, 0x15, // PDN_DR_CTR
	0x15, 0x02, // Output VSYNC active high + HSYNC active high
	0x1e, 0x07,
	0x13, 0x00, // AGC, AWB, AEC disable
	0x01, 0x11, // Blue gain
	0x02, 0x24, // Red gain
	0x8c, 0x02, // Integration time MSB
	0x8d, 0xee, // Integration time LSB
	0x87, 0xc8, // Global gain low 8 bits, bit[8] is in 0x96[7]
	0x86, 0xc8,
	0x9d, 0x20, // DIG gain max limit
	0x13, 0x87, // AGC, AWB, AEC enable
	0x27, 0x0a,
	0x28, 0x00,
	0x00, 0xaa,
	0x0d, 0x2c,
	0x0e, 0x2c,
	0x0f, 0x2a,
	0x28, 0x0f,
	0x93, 0x00, // Dummy line insert after active line high 8 bits
	0x92, 0x00, // Dummy line insert after active line low 8 bits
	0x2a, 0x0c, // Bit[7:4]: Dummy Pixel Insert MSB
	0x2b, 0x40, // Dummy Pixel Insert LSB, for dummy pixel insert in horizontal direction
	0x4a, 0x02, // Bit[2:0]=010: 720P=1288x728 subsample mode 
	0x03, 0x50, // Bit[6:4]:HSTOP[10:8]; Bit[2:0]:HSTART[10:8]
	0x17, 0x00, // HSTART[7:0]
	0x18, 0x00, // HSTOP[7:0]
	0x10, 0x20, // Bit[5:4]:VSTOP[9:8]; Bit[1:0]:VSTART[9:8]
	0x19, 0x00, // VSTART[7:0]
	0x1a, 0xd0, // VSTOP[7:0]
	0x11, 0x31, // PLL input/output divider
	0x1b, 0x30, // PLL feedback divider
	0x8a, 0x96, // 50HZ Banding Filter STEP low 8 bits, bit[8] is in 0x13[4]
	0x8b, 0x7d, // 60HZ Banding Filter STEP low 8 bits, bit[8] is in 0x13[5]
	0x16, 0xc3,
	0x3a, 0x00, // YUV422: YUYV 
	0x21, 0xc4, // VCLK
	0x08, 0xdb,
	0x1c, 0x77,
	0x1d, 0x00,
	0xe2, 0xc3,
	0x3e, 0x05, // Bit[3]: PAD SDA driver capacity: 0:1X 1:2X
	0x6c, 0x0f,
	0xbb, 0x80,
	0xed, 0xcb,
	0x35, 0x48, // Lens shading gain of R
	0x65, 0x50, // Lens shading gain of G
	0x66, 0x40, // Lens shading gain of B
	0x37, 0x98, // Center Y coordinate LSB for B
	0x38, 0x88, // Center X coordinate LSB for B
	0x9b, 0x98, // Center Y coordinate LSB for G
	0x9c, 0x88, // Center X coordinate LSB for G
	0xbd, 0x98, // Center Y coordinate LSB for R
	0xbe, 0x88, // Center X coordinate LSB for R
	0x36, 0x12, // Bit[5:4]:Center Y coordinate MSB for B; Bit[1:0]:Center X coordinate MSB for B
	0xbc, 0x12, // Bit[5:4]:Center Y coordinate MSB for R; Bit[1:0]:Center X coordinate MSB for R
	0xbf, 0x12, // Bit[5:4]:Center Y coordinate MSB for G; Bit[1:0]:Center X coordinate MSB for G
	0x24, 0x7a, // Y target value1
	0x13, 0x87, // AGC, AWB, AEC enable
	0x70, 0x07, // Bit[0]: Edge enhancement on
	0x78, 0xb7, // Bit[0]: Low pass filter Switch ON
	0x7a, 0x67,
	0x7d, 0x39,
	0x7e, 0x3d,
	0x80, 0x92, // Bit[1]=1:Choose 50HZ step
	0x82, 0x23, // GLB_MIN1 low 8 bits
	0x83, 0x4c, // GLB_MAX1 low 8 bits
	0x89, 0x2d, // Bit[7:3]: Integration time Maximum; Bit[2:0]: Integration time mean value
	0x98, 0x12, 
	0x39, 0xa0,
	0x3B, 0x60, // Y average threshold for auto offset adjust in low light scene
	0x3C, 0x24,
	0x3F, 0x20,
	0x40, 0x60, // K0: Gamma Correction Slop Coefficients
	0x41, 0x58, // K1: Gamma Correction Slop Coefficients
	0x42, 0x55, // K2: Gamma Correction Slop Coefficients
	0x43, 0x50, // K3: Gamma Correction Slop Coefficients
	0x44, 0x4a, // K4: Gamma Correction Slop Coefficients
	0x45, 0x46, // K5: Gamma Correction Slop Coefficients
	0x46, 0x41, // K6: Gamma Correction Slop Coefficients
	0x47, 0x3c, // K7: Gamma Correction Slop Coefficients
	0x48, 0x37, // K8: Gamma Correction Slop Coefficients
	0x49, 0x33, // K9: Gamma Correction Slop Coefficients
	0x4B, 0x30, // K10: Gamma Correction Slop Coefficients
	0x4C, 0x2d, // K11: Gamma Correction Slop Coefficients
	0x4D, 0x29, // Bit[4:0]: Edge threshold
	0x4E, 0x25, // K12: Gamma Correction Slop Coefficients
	0x4F, 0x21, // K13: Gamma Correction Slop Coefficients
	0x50, 0x20, // K14: Gamma Correction Slop Coefficients
	0x90, 0x20,
	0x91, 0x1C,
	0x5c, 0x8e, // Bit[5:4]=00 Color space select:BT601
	0x51, 0x22, // Low 8 bit of Color Correction Matrix Coefficients 1
	0x52, 0x00, // Low 8 bit of Color Correction Matrix Coefficients 2
	0x53, 0x96, // Low 8 bit of Color Correction Matrix Coefficients 3
	0x54, 0x8C, // Low 8 bit of Color Correction Matrix Coefficients 4
	0x57, 0x7F, // Low 8 bit of Color Correction Matrix Coefficients 5
	0x58, 0x0B, // Low 8 bit of Color Correction Matrix Coefficients 6
	0x5a, 0x14, // Bit[5:0]: Sign of Color Correction Matrix Coefficients 6:1
	0x59, 0x00, // Bit[5:0]: High bit of Color Correction Matrix Coefficients 6:1
	0x5c, 0x0E, // Bit[5:4]=00 Color space select:BT601
	0x51, 0x16, // Low 8 bit of Color Correction Matrix Coefficients 1
	0x52, 0x14, // Low 8 bit of Color Correction Matrix Coefficients 2
	0x53, 0x8F, // Low 8 bit of Color Correction Matrix Coefficients 3
	0x54, 0x78, // Low 8 bit of Color Correction Matrix Coefficients 4
	0x57, 0x93, // Low 8 bit of Color Correction Matrix Coefficients 5
	0x58, 0x25, // Low 8 bit of Color Correction Matrix Coefficients 6
	0x5a, 0x16, // Bit[5:0]: Sign of Color Correction Matrix Coefficients 6:1
	0x59, 0x00, // Bit[5:0]: High bit of Color Correction Matrix Coefficients 6:1
	0x5e, 0x38, // Bit[6:0]: Global gain threshold
	0xb0, 0x00, // Saturation control
	0xb3, 0x7a, // Bit[7:4]: Used as Y average value threshold for auto saturation.
	0xb4, 0x81,
	0xb1, 0xf0, // Cb Coefficient for Color Saturation
	0xb2, 0xe0, // Cr Coefficient for Color Saturation
	0xb4, 0x01,
	0xb1, 0xa0, // Cb Coefficient for Color Saturation
	0xb2, 0x88, // Cr Coefficient for Color Saturation
	0x6a, 0x01, // Bit[2:0]:G channel Gain
	0x23, 0x66, // GreenGain[2:0]:Bit[6:4]: For even column; Bit[2:0]: For odd column
	0xa2, 0x0f, // Bit[5:0]: The threshold of blue gain1
	0xa3, 0x22, // Bit[5:0]: The threshold of blue gain2
	0xa4, 0x0b, // Bit[5:0]: The threshold of red gain1
	0xa5, 0x24, // Bit[5:0]: The threshold of red gain2
	0xa7, 0x97, // Bit[4:0]: Base B gain
	0xa8, 0x16, // Bit[4:0]: Base R gain
	0xa9, 0x4e, // AWB CB limited
	0xaa, 0x58, // AWB CR limited
	0xab, 0x1b, // AWB CBCR limited
	0xac, 0x1c, // AWB Y limited1
	0xd4, 0x18, // AWB Y limited2
};

void BF3710_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT8U  reg, value;
	INT16U uCtrlReg1, uCtrlReg2;
	INT32U i;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	R_CSI_TG_HRATIO = 0;					
	R_CSI_TG_VRATIO = 0;					

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
	
	for(i=0; i<sizeof(BF3710_reset_table); i+=2) {
		reg = BF3710_reset_table[i];
		value = BF3710_reset_table[i+1];
		sccb_write_Reg8Data8(BF3710_ID, reg, value);
	}
	
	sccb_delay (200);

	for(i=0; i<sizeof(BF3710_init_table); i+=2) {
		reg = BF3710_init_table[i];
		value = BF3710_init_table[i+1];
		sccb_write_Reg8Data8(BF3710_ID, reg, value);
	}

	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif //__BF3710_DRV_C__

#ifdef	__GC0307_DRV_C_
#define GC0307_MIRROR_MODE	2	//0:NORMAL, 1:H_MIRROR , 2:V_MIRROR, 3.HV_MIRROR(180 degree)
//====================================================================================================
//	Description:	OV7670 Initialization
//	Syntax:			void OV7670_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void GC0307_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	INT16S nReso;
	
	// Enable CSI clock to let sensor initialize at first
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif	

	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}

	// Whether compression or not?
	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// VGA
	{
#ifdef	__TV_QVGA__
		R_CSI_TG_HRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_VRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_HWIDTH = nWidthH;					// Horizontal frame width
		R_CSI_TG_VHEIGHT = nWidthV*2;				// Vertical frame width
#endif	// __TV_QVGA__
	}
	else
	{
		R_CSI_TG_HRATIO = 0;
		R_CSI_TG_VRATIO = 0;
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init ();
	sccb_delay (200);
	
	sccb_write_Reg8Data8(GC0307_ID, 0x18, 0x80);   
#if 0
	//========= close output
	sccb_write_Reg8Data8(GC0307_ID, 0x43  ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x44  ,0xa2); 
	
	//======== close some functions
	//open thm after configure their parmameters
	sccb_write_Reg8Data8(GC0307_ID, 0x40  ,0x10);  
	sccb_write_Reg8Data8(GC0307_ID, 0x41  ,0x00);  			
	sccb_write_Reg8Data8(GC0307_ID, 0x42  ,0x10); 					  	
	sccb_write_Reg8Data8(GC0307_ID, 0x47  ,0x00);  //mode1,				  	
	sccb_write_Reg8Data8(GC0307_ID, 0x48  ,0xc3);  //mode2, 	
	sccb_write_Reg8Data8(GC0307_ID, 0x49  ,0x00);  //dither_mode 		
	sccb_write_Reg8Data8(GC0307_ID, 0x4a  ,0x00);  //clock_gating_en
	sccb_write_Reg8Data8(GC0307_ID, 0x4b  ,0x00);  //mode_reg3
	sccb_write_Reg8Data8(GC0307_ID, 0x4E  ,0x23);  //sync mode
	sccb_write_Reg8Data8(GC0307_ID, 0x4F  ,0x01);  //AWB, AEC, every N frame	
	
	//======== frame timing
	sccb_write_Reg8Data8(GC0307_ID, 0x01  ,0x74); //HB
	sccb_write_Reg8Data8(GC0307_ID, 0x02  ,0x70); //VB
	sccb_write_Reg8Data8(GC0307_ID, 0x1C  ,0x00); //Vs_st
	sccb_write_Reg8Data8(GC0307_ID, 0x1D  ,0x00); //Vs_et
	sccb_write_Reg8Data8(GC0307_ID, 0x10  ,0x00); //high 4 bits of VB, HB
	sccb_write_Reg8Data8(GC0307_ID, 0x11  ,0x05); //row_tail,  AD_pipe_number
	
	//======== windowing
	sccb_write_Reg8Data8(GC0307_ID, 0x05  ,0x00); //row_start
	sccb_write_Reg8Data8(GC0307_ID, 0x06  ,0x00);
	sccb_write_Reg8Data8(GC0307_ID, 0x07  ,0x00); //col start
	sccb_write_Reg8Data8(GC0307_ID, 0x08  ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x09  ,0x01); //win height
	sccb_write_Reg8Data8(GC0307_ID, 0x0A  ,0xE8);
	sccb_write_Reg8Data8(GC0307_ID, 0x0B  ,0x02); //win width, pixel array only 640
	sccb_write_Reg8Data8(GC0307_ID, 0x0C  ,0x80);
	
	//======== analog
	sccb_write_Reg8Data8(GC0307_ID, 0x0D ,0x22); //rsh_width
	
	sccb_write_Reg8Data8(GC0307_ID, 0x0E ,0x02); //CISCTL mode2,  
	sccb_write_Reg8Data8(GC0307_ID, 0x0F ,0x82); //CISCTL mode1
	
	sccb_write_Reg8Data8(GC0307_ID, 0x12 ,0x70); //7 hrst, 6_4 darsg,
	sccb_write_Reg8Data8(GC0307_ID, 0x13 ,0x00); //7 CISCTL_restart, 0 apwd
	sccb_write_Reg8Data8(GC0307_ID, 0x14 ,0x00); //NA
	sccb_write_Reg8Data8(GC0307_ID, 0x15 ,0xba); //7_4 vref
	sccb_write_Reg8Data8(GC0307_ID, 0x16 ,0x13); //5to4 _coln_r,  __1to0__da18
	sccb_write_Reg8Data8(GC0307_ID, 0x17 ,0x52); //opa_r, ref_r, sRef_r
	sccb_write_Reg8Data8(GC0307_ID, 0x18 ,0xc0); //analog_mode, best case for left band.
	
	sccb_write_Reg8Data8(GC0307_ID, 0x1E ,0x0d); //tsp_width 		   
	sccb_write_Reg8Data8(GC0307_ID, 0x1F ,0x32); //sh_delay
	
	//======== offset
	sccb_write_Reg8Data8(GC0307_ID, 0x47 ,0x00);  //7__test_image, __6__fixed_pga, __5__auto_DN, __4__CbCr_fix, 
	 											  //__to2__dark_sequence, __1__allow_pclk_vcync, __0__LSC_test_image
	sccb_write_Reg8Data8(GC0307_ID, 0x19 ,0x06);  //pga_o			 
	sccb_write_Reg8Data8(GC0307_ID, 0x1a ,0x06);  //pga_e			 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x31 ,0x00);  //4	//pga_oFFset ,	 high 8bits of 11bits
	sccb_write_Reg8Data8(GC0307_ID, 0x3B ,0x00);  //global_oFFset, low 8bits of 11bits
	
	sccb_write_Reg8Data8(GC0307_ID, 0x59 ,0x0f);  //offset_mode 	
	sccb_write_Reg8Data8(GC0307_ID, 0x58 ,0x88);  //DARK_VALUE_RATIO_G,  DARK_VALUE_RATIO_RB
	sccb_write_Reg8Data8(GC0307_ID, 0x57 ,0x08);  //DARK_CURRENT_RATE
	sccb_write_Reg8Data8(GC0307_ID, 0x56 ,0x77);  //PGA_OFFSET_EVEN_RATIO, PGA_OFFSET_ODD_RATIO
	
	//======== blk
	sccb_write_Reg8Data8(GC0307_ID, 0x35 ,0xd8);  //blk_mode
	
	sccb_write_Reg8Data8(GC0307_ID, 0x36 ,0x40);  
	
	sccb_write_Reg8Data8(GC0307_ID, 0x3C ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x3D ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x3E ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x3F ,0x00); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0xb5 ,0x70); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb6 ,0x40); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb7 ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb8 ,0x38); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb9 ,0xc3); 		  
	sccb_write_Reg8Data8(GC0307_ID, 0xba ,0x0f); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x7e ,0x35); 
	sccb_write_Reg8Data8(GC0307_ID, 0x7f ,0x86); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x5c ,0x68); //78
	sccb_write_Reg8Data8(GC0307_ID, 0x5d ,0x78); //88
	
	//======== manual_gain 
	sccb_write_Reg8Data8(GC0307_ID, 0x61 ,0x80); //manual_gain_g1	
	sccb_write_Reg8Data8(GC0307_ID, 0x63 ,0x80); //manual_gain_r
	sccb_write_Reg8Data8(GC0307_ID, 0x65 ,0x98); //manual_gai_b, 0xa0=1.25, 0x98=1.1875
	sccb_write_Reg8Data8(GC0307_ID, 0x67 ,0x80); //manual_gain_g2
	sccb_write_Reg8Data8(GC0307_ID, 0x68 ,0x18); //global_manual_gain	 2.4bits
	
	//========CC _R
	sccb_write_Reg8Data8(GC0307_ID, 0x69 ,0x58);  //54
	sccb_write_Reg8Data8(GC0307_ID, 0x6A ,0xf6);  //ff
	sccb_write_Reg8Data8(GC0307_ID, 0x6B ,0xfb);  //fe
	sccb_write_Reg8Data8(GC0307_ID, 0x6C ,0xf4);  //ff
	sccb_write_Reg8Data8(GC0307_ID, 0x6D ,0x5a);  //5f
	sccb_write_Reg8Data8(GC0307_ID, 0x6E ,0xe6);  //e1
	
	sccb_write_Reg8Data8(GC0307_ID, 0x6f  ,0x00); 	
	
	//========lsc							  
	sccb_write_Reg8Data8(GC0307_ID, 0x70 ,0x14); 
	sccb_write_Reg8Data8(GC0307_ID, 0x71 ,0x1c); 
	sccb_write_Reg8Data8(GC0307_ID, 0x72 ,0x20); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x73 ,0x10); 	
	sccb_write_Reg8Data8(GC0307_ID, 0x74 ,0x3c); 
	sccb_write_Reg8Data8(GC0307_ID, 0x75 ,0x52); 
	
	//========dn																			 
	sccb_write_Reg8Data8(GC0307_ID, 0x7d ,0x2f);  //dn_mode   	
	sccb_write_Reg8Data8(GC0307_ID, 0x80 ,0x0c); //when auto_dn, check 7e,7f
	sccb_write_Reg8Data8(GC0307_ID, 0x81 ,0x0c);
	sccb_write_Reg8Data8(GC0307_ID, 0x82 ,0x44);
															
	//dd																	   
	sccb_write_Reg8Data8(GC0307_ID, 0x83 ,0x18);  //DD_TH1 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x84 ,0x18);  //DD_TH2 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x85 ,0x04);  //DD_TH3 																							  
	sccb_write_Reg8Data8(GC0307_ID, 0x87 ,0x34);  //32 b DNDD_low_range X16,  DNDD_low_range_C_weight_center					
	
	//========intp-ee																		   
	sccb_write_Reg8Data8(GC0307_ID, 0x88 ,0x04);  													   
	sccb_write_Reg8Data8(GC0307_ID, 0x89 ,0x01);  										  
	sccb_write_Reg8Data8(GC0307_ID, 0x8a ,0x50);//60  										   
	sccb_write_Reg8Data8(GC0307_ID, 0x8b ,0x50);//60  										   
	sccb_write_Reg8Data8(GC0307_ID, 0x8c ,0x07);  												 				  
															  
	sccb_write_Reg8Data8(GC0307_ID, 0x50 ,0x0c);   						   		
	sccb_write_Reg8Data8(GC0307_ID, 0x5f ,0x3c); 																					 
															 
	sccb_write_Reg8Data8(GC0307_ID, 0x8e ,0x02);  															  
	sccb_write_Reg8Data8(GC0307_ID, 0x86 ,0x02);  																  
															
	sccb_write_Reg8Data8(GC0307_ID, 0x51 ,0x20);  																
	sccb_write_Reg8Data8(GC0307_ID, 0x52 ,0x08);  
	sccb_write_Reg8Data8(GC0307_ID, 0x53 ,0x00); 
	
	//======== YCP 
	//contrast center																			  
	sccb_write_Reg8Data8(GC0307_ID, 0x77 ,0x80); //contrast_center 																  
	sccb_write_Reg8Data8(GC0307_ID, 0x78 ,0x00); //fixed_Cb																		  
	sccb_write_Reg8Data8(GC0307_ID, 0x79 ,0x00); //fixed_Cr																		  
	sccb_write_Reg8Data8(GC0307_ID, 0x7a ,0x00); //luma_offset 																																							
	sccb_write_Reg8Data8(GC0307_ID, 0x7b ,0x40); //hue_cos 																		  
	sccb_write_Reg8Data8(GC0307_ID, 0x7c ,0x00); //hue_sin 																		  
																 
	//saturation																				  
	sccb_write_Reg8Data8(GC0307_ID, 0xa0 ,0x40); //40global_saturation
	sccb_write_Reg8Data8(GC0307_ID, 0xa1 ,0x40); //luma_contrast																	  
	sccb_write_Reg8Data8(GC0307_ID, 0xa2 ,0x34); //saturation_Cb																	  
	sccb_write_Reg8Data8(GC0307_ID, 0xa3 ,0x34); //saturation_Cr
												
	sccb_write_Reg8Data8(GC0307_ID, 0xa4 ,0xc8); 																  
	sccb_write_Reg8Data8(GC0307_ID, 0xa5 ,0x02); 
	sccb_write_Reg8Data8(GC0307_ID, 0xa6 ,0x28); 																			  
	sccb_write_Reg8Data8(GC0307_ID, 0xa7 ,0x02); 
	
	//skin																							  
	sccb_write_Reg8Data8(GC0307_ID, 0xa8 ,0xee); 															  
	sccb_write_Reg8Data8(GC0307_ID, 0xa9 ,0x12); 															  
	sccb_write_Reg8Data8(GC0307_ID, 0xaa ,0x01); 														  
	sccb_write_Reg8Data8(GC0307_ID, 0xab ,0x20); 													  
	sccb_write_Reg8Data8(GC0307_ID, 0xac ,0xf0); 														  
	sccb_write_Reg8Data8(GC0307_ID, 0xad ,0x10); 															  
	
	//======== ABS
	sccb_write_Reg8Data8(GC0307_ID, 0xae ,0x18); 
	sccb_write_Reg8Data8(GC0307_ID, 0xaf ,0x74); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb0 ,0xe0); 	  
	sccb_write_Reg8Data8(GC0307_ID, 0xb1 ,0x20); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb2 ,0x6c); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb3 ,0x40); 
	sccb_write_Reg8Data8(GC0307_ID, 0xb4 ,0x04); 
	
	//======== AWB 
	sccb_write_Reg8Data8(GC0307_ID, 0xbb ,0x42); 
	sccb_write_Reg8Data8(GC0307_ID, 0xbc ,0x60); 
	sccb_write_Reg8Data8(GC0307_ID, 0xbd ,0x50); 
	sccb_write_Reg8Data8(GC0307_ID, 0xbe ,0x50); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0xbf ,0x0c); 
	sccb_write_Reg8Data8(GC0307_ID, 0xc0 ,0x06); 
	sccb_write_Reg8Data8(GC0307_ID, 0xc1 ,0x70); 
	sccb_write_Reg8Data8(GC0307_ID, 0xc2 ,0xf1);  //f4
	sccb_write_Reg8Data8(GC0307_ID, 0xc3 ,0x40); 
	sccb_write_Reg8Data8(GC0307_ID, 0xc4 ,0x20); //18
	sccb_write_Reg8Data8(GC0307_ID, 0xc5 ,0x33); //33
	sccb_write_Reg8Data8(GC0307_ID, 0xc6 ,0x1d);   
	
	sccb_write_Reg8Data8(GC0307_ID, 0xca ,0x70);// 70  
	sccb_write_Reg8Data8(GC0307_ID, 0xcb ,0x70); // 70
	sccb_write_Reg8Data8(GC0307_ID, 0xcc ,0x78); // 78
	
	sccb_write_Reg8Data8(GC0307_ID, 0xcd ,0x80); //R_ratio 									 
	sccb_write_Reg8Data8(GC0307_ID, 0xce ,0x80); //G_ratio  , cold_white white 								   
	sccb_write_Reg8Data8(GC0307_ID, 0xcf ,0x80); //B_ratio  	
	
	//========  aecT  
	sccb_write_Reg8Data8(GC0307_ID, 0x20 ,0x02); 
	sccb_write_Reg8Data8(GC0307_ID, 0x21 ,0xc0); 
	sccb_write_Reg8Data8(GC0307_ID, 0x22 ,0x60);    
	sccb_write_Reg8Data8(GC0307_ID, 0x23 ,0x88); 
	sccb_write_Reg8Data8(GC0307_ID, 0x24 ,0x96); 
	sccb_write_Reg8Data8(GC0307_ID, 0x25 ,0x30); 
	sccb_write_Reg8Data8(GC0307_ID, 0x26 ,0xd0); 
	sccb_write_Reg8Data8(GC0307_ID, 0x27 ,0x00); 

	sccb_write_Reg8Data8(GC0307_ID, 0x28 ,0x02); //AEC_exp_level_1bit11to8   
	sccb_write_Reg8Data8(GC0307_ID, 0x29 ,0x58); //AEC_exp_level_1bit7to0	  
	sccb_write_Reg8Data8(GC0307_ID, 0x2a ,0x03); //AEC_exp_level_2bit11to8   
	sccb_write_Reg8Data8(GC0307_ID, 0x2b ,0x84); //AEC_exp_level_2bit7to0			 
	sccb_write_Reg8Data8(GC0307_ID, 0x2c ,0x09); //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //	 
	sccb_write_Reg8Data8(GC0307_ID, 0x2d ,0x60); //AEC_exp_level_3bit7to0			 
	sccb_write_Reg8Data8(GC0307_ID, 0x2e ,0x0a); //AEC_exp_level_4bit11to8   4FPS 
	sccb_write_Reg8Data8(GC0307_ID, 0x2f ,0x8c); //AEC_exp_level_4bit7to0	 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x30 ,0x20); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x31 ,0x00); 					   
	sccb_write_Reg8Data8(GC0307_ID, 0x32 ,0x1c); 
	sccb_write_Reg8Data8(GC0307_ID, 0x33 ,0x90); 			  
	sccb_write_Reg8Data8(GC0307_ID, 0x34 ,0x10);	
	
	sccb_write_Reg8Data8(GC0307_ID, 0xd0 ,0x34); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0xd1 ,0x50); //AEC_target_Y						   
	sccb_write_Reg8Data8(GC0307_ID, 0xd2 ,0xf2); 	  
	sccb_write_Reg8Data8(GC0307_ID, 0xd4 ,0x96); 
	sccb_write_Reg8Data8(GC0307_ID, 0xd5 ,0x10); 
	sccb_write_Reg8Data8(GC0307_ID, 0xd6 ,0x96); //antiflicker_step 					   
	sccb_write_Reg8Data8(GC0307_ID, 0xd7 ,0x10); //AEC_exp_time_min 			   
	sccb_write_Reg8Data8(GC0307_ID, 0xd8 ,0x02); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0xdd  ,0x12); 
											
	//========= measure window										
	sccb_write_Reg8Data8(GC0307_ID, 0xe0 ,0x03); 						 
	sccb_write_Reg8Data8(GC0307_ID, 0xe1 ,0x02); 							 
	sccb_write_Reg8Data8(GC0307_ID, 0xe2 ,0x27); 								 
	sccb_write_Reg8Data8(GC0307_ID, 0xe3 ,0x1e); 				 
	sccb_write_Reg8Data8(GC0307_ID, 0xe8 ,0x3b); 					 
	sccb_write_Reg8Data8(GC0307_ID, 0xe9 ,0x6e); 						 
	sccb_write_Reg8Data8(GC0307_ID, 0xea ,0x2c); 					 
	sccb_write_Reg8Data8(GC0307_ID, 0xeb ,0x50); 					 
	sccb_write_Reg8Data8(GC0307_ID, 0xec ,0x73); 		 
	
	//========= close_frame													
	sccb_write_Reg8Data8(GC0307_ID, 0xed ,0x00); //close_frame_num1 ,can be use to reduce FPS				 
	sccb_write_Reg8Data8(GC0307_ID, 0xee ,0x00); //close_frame_num2  
	sccb_write_Reg8Data8(GC0307_ID, 0xef ,0x00); //close_frame_num
	
	// page1  
	sccb_write_Reg8Data8(GC0307_ID, 0xf0 ,0x01); //select page1 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x00 ,0x20); 							  
	sccb_write_Reg8Data8(GC0307_ID, 0x01 ,0x20); 							  
	sccb_write_Reg8Data8(GC0307_ID, 0x02 ,0x20); 									
	sccb_write_Reg8Data8(GC0307_ID, 0x03 ,0x20); 							
	sccb_write_Reg8Data8(GC0307_ID, 0x04 ,0x78); 
	sccb_write_Reg8Data8(GC0307_ID, 0x05 ,0x78); 					 
	sccb_write_Reg8Data8(GC0307_ID, 0x06 ,0x78); 								  
	sccb_write_Reg8Data8(GC0307_ID, 0x07 ,0x78); 									 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x10 ,0x04); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x11 ,0x04);							  
	sccb_write_Reg8Data8(GC0307_ID, 0x12 ,0x04); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x13 ,0x04); 							  
	sccb_write_Reg8Data8(GC0307_ID, 0x14 ,0x01); 							  
	sccb_write_Reg8Data8(GC0307_ID, 0x15 ,0x01); 							  
	sccb_write_Reg8Data8(GC0307_ID, 0x16 ,0x01); 						 
	sccb_write_Reg8Data8(GC0307_ID, 0x17 ,0x01); 						 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x20 ,0x00); 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x21 ,0x00); 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x22 ,0x00); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x23 ,0x00); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x24 ,0x00); 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x25 ,0x00); 						  
	sccb_write_Reg8Data8(GC0307_ID, 0x26 ,0x00); 					  
	sccb_write_Reg8Data8(GC0307_ID, 0x27 ,0x00);  						  
	
	sccb_write_Reg8Data8(GC0307_ID, 0x40 ,0x11); 
	
	//============================lscP 
	sccb_write_Reg8Data8(GC0307_ID, 0x45 ,0x06); 	 
	sccb_write_Reg8Data8(GC0307_ID, 0x46 ,0x06); 			 
	sccb_write_Reg8Data8(GC0307_ID, 0x47 ,0x05); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x48 ,0x04); 	
	sccb_write_Reg8Data8(GC0307_ID, 0x49 ,0x03); 		 
	sccb_write_Reg8Data8(GC0307_ID, 0x4a ,0x03); 
	
	sccb_write_Reg8Data8(GC0307_ID, 0x62 ,0xd8); 
	sccb_write_Reg8Data8(GC0307_ID, 0x63 ,0x24); 
	sccb_write_Reg8Data8(GC0307_ID, 0x64 ,0x24); 
	sccb_write_Reg8Data8(GC0307_ID, 0x65 ,0x24); 
	sccb_write_Reg8Data8(GC0307_ID, 0x66 ,0xd8); 
	sccb_write_Reg8Data8(GC0307_ID, 0x67 ,0x24);
	
	sccb_write_Reg8Data8(GC0307_ID, 0x5a ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x5b ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x5c ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x5d ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x5e ,0x00); 
	sccb_write_Reg8Data8(GC0307_ID, 0x5f ,0x00); 
	
	//============================ ccP 	
	sccb_write_Reg8Data8(GC0307_ID, 0x69 ,0x03); //cc_mode
	
	//CC_G    
	sccb_write_Reg8Data8(GC0307_ID, 0x70 ,0x5d); 
	sccb_write_Reg8Data8(GC0307_ID, 0x71 ,0xed); 
	sccb_write_Reg8Data8(GC0307_ID, 0x72 ,0xff); 
	sccb_write_Reg8Data8(GC0307_ID, 0x73 ,0xe5); 
	sccb_write_Reg8Data8(GC0307_ID, 0x74 ,0x5f); 
	sccb_write_Reg8Data8(GC0307_ID, 0x75 ,0xe6); 
	
	//CC_
	sccb_write_Reg8Data8(GC0307_ID, 0x76 ,0x41); 
	sccb_write_Reg8Data8(GC0307_ID, 0x77 ,0xef); 
	sccb_write_Reg8Data8(GC0307_ID, 0x78 ,0xff); 
	sccb_write_Reg8Data8(GC0307_ID, 0x79 ,0xff); 
	sccb_write_Reg8Data8(GC0307_ID, 0x7a ,0x5f); 
	sccb_write_Reg8Data8(GC0307_ID, 0x7b ,0xfa); 	 

	//============================ AGP     
	sccb_write_Reg8Data8(GC0307_ID, 0x7e ,0x00);  
	sccb_write_Reg8Data8(GC0307_ID, 0x7f ,0x00);  
	sccb_write_Reg8Data8(GC0307_ID, 0x80 ,0xc8);  
	sccb_write_Reg8Data8(GC0307_ID, 0x81 ,0x06);  
	sccb_write_Reg8Data8(GC0307_ID, 0x82 ,0x08);  
	
	sccb_write_Reg8Data8(GC0307_ID, 0x83 ,0x23);  
	sccb_write_Reg8Data8(GC0307_ID, 0x84 ,0x38);  
	sccb_write_Reg8Data8(GC0307_ID, 0x85 ,0x4F);  
	sccb_write_Reg8Data8(GC0307_ID, 0x86 ,0x61);  
	sccb_write_Reg8Data8(GC0307_ID, 0x87 ,0x72);  
	sccb_write_Reg8Data8(GC0307_ID, 0x88 ,0x80);  
	sccb_write_Reg8Data8(GC0307_ID, 0x89 ,0x8D);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8a ,0xA2);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8b ,0xB2);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8c ,0xC0);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8d ,0xCA);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8e ,0xD3);  
	sccb_write_Reg8Data8(GC0307_ID, 0x8f ,0xDB);  
	sccb_write_Reg8Data8(GC0307_ID, 0x90 ,0xE2);  
	sccb_write_Reg8Data8(GC0307_ID, 0x91 ,0xED);  
	sccb_write_Reg8Data8(GC0307_ID, 0x92 ,0xF6);  
	sccb_write_Reg8Data8(GC0307_ID, 0x93 ,0xFD);  
	
	//about gama1 is hex r oct
	sccb_write_Reg8Data8(GC0307_ID, 0x94 ,0x04);  
	sccb_write_Reg8Data8(GC0307_ID, 0x95 ,0x0E);  
	sccb_write_Reg8Data8(GC0307_ID, 0x96 ,0x1B);  
	sccb_write_Reg8Data8(GC0307_ID, 0x97 ,0x28);  
	sccb_write_Reg8Data8(GC0307_ID, 0x98 ,0x35);  
	sccb_write_Reg8Data8(GC0307_ID, 0x99 ,0x41);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9a ,0x4E);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9b ,0x67);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9c ,0x7E);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9d ,0x94);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9e ,0xA7);  
	sccb_write_Reg8Data8(GC0307_ID, 0x9f ,0xBA);  
	sccb_write_Reg8Data8(GC0307_ID, 0xa0 ,0xC8);  
	sccb_write_Reg8Data8(GC0307_ID, 0xa1 ,0xD4);  
	sccb_write_Reg8Data8(GC0307_ID, 0xa2 ,0xE7);  
	sccb_write_Reg8Data8(GC0307_ID, 0xa3 ,0xF4);  
	sccb_write_Reg8Data8(GC0307_ID, 0xa4 ,0xFA); 
	
	//========= open functions	
	sccb_write_Reg8Data8(GC0307_ID, 0xf0 ,0x00); //set back to page0																		  
	sccb_write_Reg8Data8(GC0307_ID, 0x45 ,0x24);
	sccb_write_Reg8Data8(GC0307_ID, 0x40 ,0x7e); 
	sccb_write_Reg8Data8(GC0307_ID, 0x41 ,0x2F);
	sccb_write_Reg8Data8(GC0307_ID, 0x47 ,0x20); 	
	
#if (GC0307_MIRROR_MODE == 0)	// IMAGE_NORMAL:
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0xb2);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x27);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x2c);   
#elif (GC0307_MIRROR_MODE == 1) // IMAGE_H_MIRROR: ç”®      
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0xa2);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x26);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x28); 
#elif (GC0307_MIRROR_MODE == 2)	// IMAGE_V_MIRROR:   ç”®
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0x92);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x25);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x24);   
#elif (GC0307_MIRROR_MODE == 3) // IMAGE_HV_MIRROR:
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0x82);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x24);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x20); 
#endif
	
	//========= open output
	sccb_write_Reg8Data8(GC0307_ID, 0x43  ,0x40);
	sccb_write_Reg8Data8(GC0307_ID, 0x44  ,0xE2);
	
#else 
    //========= close output
    sccb_write_Reg8Data8(GC0307_ID, 0x43, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x44, 0xa2); 
    
    //========= close some functions
    // open them after configure their parmameters
    sccb_write_Reg8Data8(GC0307_ID, 0x40, 0x10); 
    sccb_write_Reg8Data8(GC0307_ID, 0x41, 0x00);             
    sccb_write_Reg8Data8(GC0307_ID, 0x42, 0x10);                          
  	//sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x00); //mode1,                      
    sccb_write_Reg8Data8(GC0307_ID, 0x48, 0xc3); //mode2,     
    sccb_write_Reg8Data8(GC0307_ID, 0x49, 0x00); //dither_mode         
    sccb_write_Reg8Data8(GC0307_ID, 0x4a, 0x00); //clock_gating_en
    sccb_write_Reg8Data8(GC0307_ID, 0x4b, 0x00); //mode_reg3
    sccb_write_Reg8Data8(GC0307_ID, 0x4E, 0x22); //sync mode
    sccb_write_Reg8Data8(GC0307_ID, 0x4F, 0x01); //AWB, AEC, every N frame    
    
    //========= frame timing
    sccb_write_Reg8Data8(GC0307_ID, 0x01, 0x6a); //HB
    sccb_write_Reg8Data8(GC0307_ID, 0x02, 0x25); //VB
    sccb_write_Reg8Data8(GC0307_ID, 0x1C, 0x00); //Vs_st
    sccb_write_Reg8Data8(GC0307_ID, 0x1D, 0x00); //Vs_et
    sccb_write_Reg8Data8(GC0307_ID, 0x10, 0x00); //high 4 bits of VB, HB
    sccb_write_Reg8Data8(GC0307_ID, 0x11, 0x05); //row_tail,  AD_pipe_number
    
    sccb_write_Reg8Data8(GC0307_ID, 0x03, 0x01); //row_start
    sccb_write_Reg8Data8(GC0307_ID, 0x04, 0x95);
    //========= windowing                 
    sccb_write_Reg8Data8(GC0307_ID, 0x05, 0x00); //row_start
    sccb_write_Reg8Data8(GC0307_ID, 0x06, 0x00);
    sccb_write_Reg8Data8(GC0307_ID, 0x07, 0x00); //col start
    sccb_write_Reg8Data8(GC0307_ID, 0x08, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x09, 0x01); //win height
    sccb_write_Reg8Data8(GC0307_ID, 0x0A, 0xE8);
    sccb_write_Reg8Data8(GC0307_ID, 0x0B, 0x02); //win width, pixel array only 640
    sccb_write_Reg8Data8(GC0307_ID, 0x0C, 0x80);
    
    //========= analog
    sccb_write_Reg8Data8(GC0307_ID, 0x0D, 0x22); //rsh_width
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x0E, 0x02); //CISCTL mode2,                                        
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x12, 0x70); //7 hrst, 6_4 darsg,
    sccb_write_Reg8Data8(GC0307_ID, 0x13, 0x00); //7 CISCTL_restart, 0 apwd
    sccb_write_Reg8Data8(GC0307_ID, 0x14, 0x00); //NA
    sccb_write_Reg8Data8(GC0307_ID, 0x15, 0xba); //7_4 vref
    sccb_write_Reg8Data8(GC0307_ID, 0x16, 0x13); //5to4 _coln_r,  __1to0__da18
    sccb_write_Reg8Data8(GC0307_ID, 0x17, 0x52); //opa_r, ref_r, sRef_r
    //sccb_write_Reg8Data8(GC0307_ID, 0x18, 0xc0); //analog_mode, best case for left band.
    
    sccb_write_Reg8Data8(GC0307_ID, 0x1E, 0x0d); //tsp_width            
    sccb_write_Reg8Data8(GC0307_ID, 0x1F, 0x32); //sh_delay
    
    //========= offset
    //sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x00);  //7__test_image, __6__fixed_pga, __5__auto_DN, __4__CbCr_fix, 
                									//__3to2__dark_sequence, __1__allow_pclk_vcync, __0__LSC_test_image
    sccb_write_Reg8Data8(GC0307_ID, 0x19, 0x06);  //pga_o             
    sccb_write_Reg8Data8(GC0307_ID, 0x1a, 0x06);  //pga_e             
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x31, 0x00);  //4    //pga_oFFset ,     high 8bits of 11bits
    sccb_write_Reg8Data8(GC0307_ID, 0x3B, 0x00);  //global_oFFset, low 8bits of 11bits
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x59, 0x0f);  //offset_mode     
    sccb_write_Reg8Data8(GC0307_ID, 0x58, 0x88);  //DARK_VALUE_RATIO_G,  DARK_VALUE_RATIO_RB
    sccb_write_Reg8Data8(GC0307_ID, 0x57, 0x08);  //DARK_CURRENT_RATE
    sccb_write_Reg8Data8(GC0307_ID, 0x56, 0x77);  //PGA_OFFSET_EVEN_RATIO, PGA_OFFSET_ODD_RATIO
                                          
    //========= blk                       
    sccb_write_Reg8Data8(GC0307_ID, 0x35, 0xd8);  //blk_mode
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x36, 0x40);  
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x3C, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x3D, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x3E, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x3F, 0x00); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xb5, 0x70); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb6, 0x40); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb7, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb8, 0x38); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb9, 0xc3);           
    sccb_write_Reg8Data8(GC0307_ID, 0xba, 0x0f); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x7e, 0x35); 
    sccb_write_Reg8Data8(GC0307_ID, 0x7f, 0x86); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x5c, 0x68); //78
    sccb_write_Reg8Data8(GC0307_ID, 0x5d, 0x78); //88
                                                                             
    //========= manual_gain               
    sccb_write_Reg8Data8(GC0307_ID, 0x61, 0x80); //manual_gain_g1    
    sccb_write_Reg8Data8(GC0307_ID, 0x63, 0x80); //manual_gain_r
    sccb_write_Reg8Data8(GC0307_ID, 0x65, 0x98); //manual_gai_b, 0xa0=1.25, 0x98=1.1875
    sccb_write_Reg8Data8(GC0307_ID, 0x67, 0x80); //manual_gain_g2
    sccb_write_Reg8Data8(GC0307_ID, 0x68, 0x18); //global_manual_gain     2.4bits
                                          
    //=========CC _R                      
    sccb_write_Reg8Data8(GC0307_ID, 0x69, 0x58);  //54
    sccb_write_Reg8Data8(GC0307_ID, 0x6A, 0xf6);  //ff
    sccb_write_Reg8Data8(GC0307_ID, 0x6B, 0xfb);  //fe
    sccb_write_Reg8Data8(GC0307_ID, 0x6C, 0xf4);  //ff
    sccb_write_Reg8Data8(GC0307_ID, 0x6D, 0x5a);  //5f
    sccb_write_Reg8Data8(GC0307_ID, 0x6E, 0xe6);  //e1
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x6f, 0x00);     
                                          
    //=========lsc                           
    sccb_write_Reg8Data8(GC0307_ID, 0x70, 0x14); 
    sccb_write_Reg8Data8(GC0307_ID, 0x71, 0x1c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x72, 0x20); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x73, 0x10);     
    sccb_write_Reg8Data8(GC0307_ID, 0x74, 0x3c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x75, 0x52); 
                                          
    //=========dn                                                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x7d, 0x2f);  //dn_mode       
    sccb_write_Reg8Data8(GC0307_ID, 0x80, 0x0c); //when auto_dn, check 7e,7f
    sccb_write_Reg8Data8(GC0307_ID, 0x81, 0x0c);
    sccb_write_Reg8Data8(GC0307_ID, 0x82, 0x44);
                                                                                     
    //dd                                                                        
    sccb_write_Reg8Data8(GC0307_ID, 0x83, 0x18);  //DD_TH1                       
    sccb_write_Reg8Data8(GC0307_ID, 0x84, 0x18);  //DD_TH2                       
    sccb_write_Reg8Data8(GC0307_ID, 0x85, 0x04);  //DD_TH3                                                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0x87, 0x34);  //32 b DNDD_low_range X16,  DNDD_low_range_C_weight_center                    
                                                                               
    //=========intp-ee                                                                        
    sccb_write_Reg8Data8(GC0307_ID, 0x88, 0x04);                                                         
    sccb_write_Reg8Data8(GC0307_ID, 0x89, 0x01);                                            
    sccb_write_Reg8Data8(GC0307_ID, 0x8a, 0x50);//60                                             
    sccb_write_Reg8Data8(GC0307_ID, 0x8b, 0x50);//60                                             
    sccb_write_Reg8Data8(GC0307_ID, 0x8c, 0x07);                                                                     
                                                                                   
    sccb_write_Reg8Data8(GC0307_ID, 0x50, 0x0c);                                      
    sccb_write_Reg8Data8(GC0307_ID, 0x5f, 0x3c);                                                                                      
                                                                                  
    sccb_write_Reg8Data8(GC0307_ID, 0x8e, 0x02);                                                                
    sccb_write_Reg8Data8(GC0307_ID, 0x86, 0x02);                                                                    
                                                                                 
    sccb_write_Reg8Data8(GC0307_ID, 0x51, 0x20);                                                                  
    sccb_write_Reg8Data8(GC0307_ID, 0x52, 0x08);  
    sccb_write_Reg8Data8(GC0307_ID, 0x53, 0x00); 
                                                                                
    //========= YCP                       
    //contrast_center                                                                           
    sccb_write_Reg8Data8(GC0307_ID, 0x77, 0x80); //contrast_center                                                                   
    sccb_write_Reg8Data8(GC0307_ID, 0x78, 0x00); //fixed_Cb                                                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x79, 0x00); //fixed_Cr                                                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x7a, 0x00); //luma_offset                                                                                                                                                             
    sccb_write_Reg8Data8(GC0307_ID, 0x7b, 0x40); //hue_cos                                                                           
    sccb_write_Reg8Data8(GC0307_ID, 0x7c, 0x00); //hue_sin                                                                           
                                                                                          
    //saturation                                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0xa0, 0x40); //global_saturation
    sccb_write_Reg8Data8(GC0307_ID, 0xa1, 0x40); //luma_contrast                                                                      
    sccb_write_Reg8Data8(GC0307_ID, 0xa2, 0x34); //saturation_Cb                                                                      
    sccb_write_Reg8Data8(GC0307_ID, 0xa3, 0x34); //saturation_Cr
                                                                             
    sccb_write_Reg8Data8(GC0307_ID, 0xa4, 0xc8);                                                                   
    sccb_write_Reg8Data8(GC0307_ID, 0xa5, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0xa6, 0x28);                                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0xa7, 0x02); 
                                          
    //skin                                                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0xa8, 0xee);                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0xa9, 0x12);                                                               
    sccb_write_Reg8Data8(GC0307_ID, 0xaa, 0x01);                                                           
    sccb_write_Reg8Data8(GC0307_ID, 0xab, 0x20);                                                       
    sccb_write_Reg8Data8(GC0307_ID, 0xac, 0xf0);                                                           
    sccb_write_Reg8Data8(GC0307_ID, 0xad, 0x10);                                                               
                                          
    //========= ABS                       
    sccb_write_Reg8Data8(GC0307_ID, 0xae, 0x18); 
    sccb_write_Reg8Data8(GC0307_ID, 0xaf, 0x74); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb0, 0xe0);       
    sccb_write_Reg8Data8(GC0307_ID, 0xb1, 0x20); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb2, 0x6c); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb3, 0xc0); 
    sccb_write_Reg8Data8(GC0307_ID, 0xb4, 0x04); 
                                          
    //========= AWB                       
    sccb_write_Reg8Data8(GC0307_ID, 0xbb, 0x42); 
    sccb_write_Reg8Data8(GC0307_ID, 0xbc, 0x60);
    sccb_write_Reg8Data8(GC0307_ID, 0xbd, 0x50);
    sccb_write_Reg8Data8(GC0307_ID, 0xbe, 0x50);
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xbf, 0x0c); 
    sccb_write_Reg8Data8(GC0307_ID, 0xc0, 0x06); 
    sccb_write_Reg8Data8(GC0307_ID, 0xc1, 0x60); 
    sccb_write_Reg8Data8(GC0307_ID, 0xc2, 0xf1);  //f1
    sccb_write_Reg8Data8(GC0307_ID, 0xc3, 0x40);
    sccb_write_Reg8Data8(GC0307_ID, 0xc4, 0x1c); //18//20
    sccb_write_Reg8Data8(GC0307_ID, 0xc5, 0x56);  //33
    sccb_write_Reg8Data8(GC0307_ID, 0xc6, 0x1d); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xca, 0x56); //70
    sccb_write_Reg8Data8(GC0307_ID, 0xcb, 0x52); //70
    sccb_write_Reg8Data8(GC0307_ID, 0xcc, 0x66); //78
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xcd, 0x80); //R_ratio                                      
    sccb_write_Reg8Data8(GC0307_ID, 0xce, 0x80); //G_ratio  , cold_white white                                    
    sccb_write_Reg8Data8(GC0307_ID, 0xcf, 0x80); //B_ratio      
                                          
    //=========  aecT                     
    sccb_write_Reg8Data8(GC0307_ID, 0x20, 0x06);//0x02 
    sccb_write_Reg8Data8(GC0307_ID, 0x21, 0xc0); 
    sccb_write_Reg8Data8(GC0307_ID, 0x22, 0x60);    
    sccb_write_Reg8Data8(GC0307_ID, 0x23, 0x88); 
    sccb_write_Reg8Data8(GC0307_ID, 0x24, 0x96); 
    sccb_write_Reg8Data8(GC0307_ID, 0x25, 0x30); 
    sccb_write_Reg8Data8(GC0307_ID, 0x26, 0xd0); 
    sccb_write_Reg8Data8(GC0307_ID, 0x27, 0x00); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x28, 0x02); //AEC_exp_level_1bit11to8   
    sccb_write_Reg8Data8(GC0307_ID, 0x29, 0x0d); //AEC_exp_level_1bit7to0      
    sccb_write_Reg8Data8(GC0307_ID, 0x2a, 0x02); //AEC_exp_level_2bit11to8   
    sccb_write_Reg8Data8(GC0307_ID, 0x2b, 0x0d); //AEC_exp_level_2bit7to0             
    sccb_write_Reg8Data8(GC0307_ID, 0x2c, 0x02); //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //     
    sccb_write_Reg8Data8(GC0307_ID, 0x2d, 0x0d); //AEC_exp_level_3bit7to0             
    sccb_write_Reg8Data8(GC0307_ID, 0x2e, 0x05); //AEC_exp_level_4bit11to8   4FPS 
    sccb_write_Reg8Data8(GC0307_ID, 0x2f, 0xdc); //AEC_exp_level_4bit7to0     
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x30, 0x20);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x31, 0x00);                        
    sccb_write_Reg8Data8(GC0307_ID, 0x32, 0x1c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x33, 0x90);               
    sccb_write_Reg8Data8(GC0307_ID, 0x34, 0x10);    
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xd0, 0x34); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xd1, 0x50); //AEC_target_Y                           
    sccb_write_Reg8Data8(GC0307_ID, 0xd2, 0x61);//0xf2       
    sccb_write_Reg8Data8(GC0307_ID, 0xd4, 0x96); 
    sccb_write_Reg8Data8(GC0307_ID, 0xd5, 0x01); // william 0318
    sccb_write_Reg8Data8(GC0307_ID, 0xd6, 0x4b); //antiflicker_step                        
    sccb_write_Reg8Data8(GC0307_ID, 0xd7, 0x03); //AEC_exp_time_min ,william 20090312               
    sccb_write_Reg8Data8(GC0307_ID, 0xd8, 0x02); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0xdd, 0x22);//0x12 
                                                               
    //========= measure window                                     
    sccb_write_Reg8Data8(GC0307_ID, 0xe0, 0x03);                          
    sccb_write_Reg8Data8(GC0307_ID, 0xe1, 0x02);                              
    sccb_write_Reg8Data8(GC0307_ID, 0xe2, 0x27);                                  
    sccb_write_Reg8Data8(GC0307_ID, 0xe3, 0x1e);                  
    sccb_write_Reg8Data8(GC0307_ID, 0xe8, 0x3b);                      
    sccb_write_Reg8Data8(GC0307_ID, 0xe9, 0x6e);                          
    sccb_write_Reg8Data8(GC0307_ID, 0xea, 0x2c);                      
    sccb_write_Reg8Data8(GC0307_ID, 0xeb, 0x50);                      
    sccb_write_Reg8Data8(GC0307_ID, 0xec, 0x73);          
                                          
    //========= close_frame                                                 
    sccb_write_Reg8Data8(GC0307_ID, 0xed, 0x00); //close_frame_num1 ,can be use to reduce FPS                 
    sccb_write_Reg8Data8(GC0307_ID, 0xee, 0x00); //close_frame_num2  
    sccb_write_Reg8Data8(GC0307_ID, 0xef, 0x00); //close_frame_num
                                          
    // page1                              
    sccb_write_Reg8Data8(GC0307_ID, 0xf0, 0x01); //select page1 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x00, 0x20);                               
    sccb_write_Reg8Data8(GC0307_ID, 0x01, 0x20);                               
    sccb_write_Reg8Data8(GC0307_ID, 0x02, 0x20);                                     
    sccb_write_Reg8Data8(GC0307_ID, 0x03, 0x20);                             
    sccb_write_Reg8Data8(GC0307_ID, 0x04, 0x78); 
    sccb_write_Reg8Data8(GC0307_ID, 0x05, 0x78);                      
    sccb_write_Reg8Data8(GC0307_ID, 0x06, 0x78);                                   
    sccb_write_Reg8Data8(GC0307_ID, 0x07, 0x78);                                      
                                                                                                                 
    sccb_write_Reg8Data8(GC0307_ID, 0x10, 0x04);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x11, 0x04);                              
    sccb_write_Reg8Data8(GC0307_ID, 0x12, 0x04);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x13, 0x04);                               
    sccb_write_Reg8Data8(GC0307_ID, 0x14, 0x01);                               
    sccb_write_Reg8Data8(GC0307_ID, 0x15, 0x01);                               
    sccb_write_Reg8Data8(GC0307_ID, 0x16, 0x01);                          
    sccb_write_Reg8Data8(GC0307_ID, 0x17, 0x01);                          
                                                                                     
    sccb_write_Reg8Data8(GC0307_ID, 0x20, 0x00);                       
    sccb_write_Reg8Data8(GC0307_ID, 0x21, 0x00);                       
    sccb_write_Reg8Data8(GC0307_ID, 0x22, 0x00);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x23, 0x00);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x24, 0x00);                       
    sccb_write_Reg8Data8(GC0307_ID, 0x25, 0x00);                           
    sccb_write_Reg8Data8(GC0307_ID, 0x26, 0x00);                       
    sccb_write_Reg8Data8(GC0307_ID, 0x27, 0x00);                            
    
    sccb_write_Reg8Data8(GC0307_ID, 0x40  ,0x11); 
    
    //=============================lscP 
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x06);      
    sccb_write_Reg8Data8(GC0307_ID, 0x46, 0x06);              
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x05); 
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x48, 0x04);     
    sccb_write_Reg8Data8(GC0307_ID, 0x49, 0x03);          
    sccb_write_Reg8Data8(GC0307_ID, 0x4a, 0x03); 
                                                                             
    sccb_write_Reg8Data8(GC0307_ID, 0x62, 0xd8); 
    sccb_write_Reg8Data8(GC0307_ID, 0x63, 0x24); 
    sccb_write_Reg8Data8(GC0307_ID, 0x64, 0x24);
    sccb_write_Reg8Data8(GC0307_ID, 0x65, 0x24); 
    sccb_write_Reg8Data8(GC0307_ID, 0x66, 0xd8); 
    sccb_write_Reg8Data8(GC0307_ID, 0x67, 0x24);
                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x5a, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5b, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5c, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5d, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5e, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5f, 0x00); 
    
    //============================= ccP 
    sccb_write_Reg8Data8(GC0307_ID, 0x69, 0x03); //cc_mode
                                          
    //CC_G                                
    sccb_write_Reg8Data8(GC0307_ID, 0x70, 0x5d); 
    sccb_write_Reg8Data8(GC0307_ID, 0x71, 0xed); 
    sccb_write_Reg8Data8(GC0307_ID, 0x72, 0xff); 
    sccb_write_Reg8Data8(GC0307_ID, 0x73, 0xe5); 
    sccb_write_Reg8Data8(GC0307_ID, 0x74, 0x5f); 
    sccb_write_Reg8Data8(GC0307_ID, 0x75, 0xe6); 
                                          
        //CC_B                            
    sccb_write_Reg8Data8(GC0307_ID, 0x76, 0x41); 
    sccb_write_Reg8Data8(GC0307_ID, 0x77, 0xef); 
    sccb_write_Reg8Data8(GC0307_ID, 0x78, 0xff); 
    sccb_write_Reg8Data8(GC0307_ID, 0x79, 0xff); 
    sccb_write_Reg8Data8(GC0307_ID, 0x7a, 0x5f); 
    sccb_write_Reg8Data8(GC0307_ID, 0x7b, 0xfa);      
    
    //============================= AGP
    sccb_write_Reg8Data8(GC0307_ID, 0x7e, 0x00);  
    sccb_write_Reg8Data8(GC0307_ID, 0x7f, 0x00);  
    sccb_write_Reg8Data8(GC0307_ID, 0x80, 0xc8);  
    sccb_write_Reg8Data8(GC0307_ID, 0x81, 0x06);  
    sccb_write_Reg8Data8(GC0307_ID, 0x82, 0x08);  

#if 0    
    sccb_write_Reg8Data8(GC0307_ID, 0x83, 0x23);  // è¿™æ˜¯0x00å¯¹å??„Gamma
    sccb_write_Reg8Data8(GC0307_ID, 0x84, 0x38);  
    sccb_write_Reg8Data8(GC0307_ID, 0x85, 0x4F);  
    sccb_write_Reg8Data8(GC0307_ID, 0x86, 0x61);  
    sccb_write_Reg8Data8(GC0307_ID, 0x87, 0x72);  
    sccb_write_Reg8Data8(GC0307_ID, 0x88, 0x80);  
    sccb_write_Reg8Data8(GC0307_ID, 0x89, 0x8D);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8a, 0xA2);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8b, 0xB2);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8c, 0xC0);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8d, 0xCA);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8e, 0xD3);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8f, 0xDB);  
    sccb_write_Reg8Data8(GC0307_ID, 0x90, 0xE2);  
    sccb_write_Reg8Data8(GC0307_ID, 0x91, 0xED);  
    sccb_write_Reg8Data8(GC0307_ID, 0x92, 0xF6);  
    sccb_write_Reg8Data8(GC0307_ID, 0x93, 0xFD);
#else                                          
    sccb_write_Reg8Data8(GC0307_ID, 0x83, 0x13);  // ?¸å?äº?x20å¯¹å??„Gamma
    sccb_write_Reg8Data8(GC0307_ID, 0x84, 0x23);  
    sccb_write_Reg8Data8(GC0307_ID, 0x85, 0x35);  
    sccb_write_Reg8Data8(GC0307_ID, 0x86, 0x44);  
    sccb_write_Reg8Data8(GC0307_ID, 0x87, 0x53);  
    sccb_write_Reg8Data8(GC0307_ID, 0x88, 0x60);  
    sccb_write_Reg8Data8(GC0307_ID, 0x89, 0x6D);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8a, 0x84);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8b, 0x98);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8c, 0xaa);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8d, 0xb8);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8e, 0xc6);  
    sccb_write_Reg8Data8(GC0307_ID, 0x8f, 0xd1);  
    sccb_write_Reg8Data8(GC0307_ID, 0x90, 0xdb);  
    sccb_write_Reg8Data8(GC0307_ID, 0x91, 0xea);  
    sccb_write_Reg8Data8(GC0307_ID, 0x92, 0xf5);  
    sccb_write_Reg8Data8(GC0307_ID, 0x93, 0xFb);
#endif 
                                         
    //about gamma1 is hex r oct           
    sccb_write_Reg8Data8(GC0307_ID, 0x94, 0x04);  //è¿™æ˜¯0x40å¯¹å??„Gamma
    sccb_write_Reg8Data8(GC0307_ID, 0x95, 0x0E);  
    sccb_write_Reg8Data8(GC0307_ID, 0x96, 0x1B);  
    sccb_write_Reg8Data8(GC0307_ID, 0x97, 0x28);  
    sccb_write_Reg8Data8(GC0307_ID, 0x98, 0x35);  
    sccb_write_Reg8Data8(GC0307_ID, 0x99, 0x41);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9a, 0x4E);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9b, 0x67);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9c, 0x7E);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9d, 0x94);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9e, 0xA7);  
    sccb_write_Reg8Data8(GC0307_ID, 0x9f, 0xBA);  
    sccb_write_Reg8Data8(GC0307_ID, 0xa0, 0xC8);  
    sccb_write_Reg8Data8(GC0307_ID, 0xa1, 0xD4);  
    sccb_write_Reg8Data8(GC0307_ID, 0xa2, 0xE7);  
    sccb_write_Reg8Data8(GC0307_ID, 0xa3, 0xF4);  
    sccb_write_Reg8Data8(GC0307_ID, 0xa4, 0xFA); 
    
    //========= open functions    
    sccb_write_Reg8Data8(GC0307_ID, 0xf0, 0x00); //set back to page0    
    sccb_write_Reg8Data8(GC0307_ID, 0x40, 0x7e); 
    sccb_write_Reg8Data8(GC0307_ID, 0x41, 0x27); //keep AEC close here
    
    //sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0xb2);
    //sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x27);
    //sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x2c);
 
    sccb_write_Reg8Data8(GC0307_ID, 0x43, 0x40);
    sccb_write_Reg8Data8(GC0307_ID, 0x44, 0xE2);
  
#if (GC0307_MIRROR_MODE == 0)	//  IMAGE_NORMAL:
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0xb2);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x27);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x2c);   
#elif (GC0307_MIRROR_MODE == 1)	// IMAGE_H_MIRROR: ç”®      
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0xa2);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x26);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x28); 
#elif (GC0307_MIRROR_MODE == 2)	// IMAGE_V_MIRROR:   ç”®
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0x92);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x25);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x24);   
#elif (GC0307_MIRROR_MODE == 3)	// IMAGE_HV_MIRROR:   // 180
    sccb_write_Reg8Data8(GC0307_ID, 0x0f, 0x82);
    sccb_write_Reg8Data8(GC0307_ID, 0x45, 0x24);
    sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x20); 
#endif
    // ?ã? 60 Hz
#if 0
    sccb_write_Reg8Data8(GC0307_ID, 0x01, 0x32);      //  50hz
    sccb_write_Reg8Data8(GC0307_ID, 0x02, 0x34); 
    sccb_write_Reg8Data8(GC0307_ID, 0x10, 0x01); 
    sccb_write_Reg8Data8(GC0307_ID, 0xd6, 0x87); 
    sccb_write_Reg8Data8(GC0307_ID, 0x28, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x29, 0x1c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2a, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2b, 0x1c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2c, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2d, 0x1c); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2e, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2f, 0x1c); 
#else
    sccb_write_Reg8Data8(GC0307_ID, 0x01, 0xce);      //  60hz
    sccb_write_Reg8Data8(GC0307_ID, 0x02, 0x89); 
    sccb_write_Reg8Data8(GC0307_ID, 0x10, 0x00); 
    sccb_write_Reg8Data8(GC0307_ID, 0xd6, 0x7d); 
    sccb_write_Reg8Data8(GC0307_ID, 0x28, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x29, 0x71); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2a, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2b, 0x71); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2c, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2d, 0x72); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2e, 0x02); 
    sccb_write_Reg8Data8(GC0307_ID, 0x2f, 0x71); 
#endif    
 
    sccb_write_Reg8Data8(GC0307_ID, 0x7e, 0x45); 
    sccb_write_Reg8Data8(GC0307_ID, 0x7f, 0x66); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5c, 0x68); 
    sccb_write_Reg8Data8(GC0307_ID, 0x5d, 0x78); 
 
    sccb_write_Reg8Data8(GC0307_ID, 0x41, 0x2f);            
    sccb_write_Reg8Data8(GC0307_ID, 0x40, 0x7e);
    sccb_write_Reg8Data8(GC0307_ID, 0x42, 0x10);
    //sccb_write_Reg8Data8(GC0307_ID, 0x47, 0x2c);
    sccb_write_Reg8Data8(GC0307_ID, 0x48, 0xc3);
    sccb_write_Reg8Data8(GC0307_ID, 0x8a, 0x50);//60
    sccb_write_Reg8Data8(GC0307_ID, 0x8b, 0x50);
    sccb_write_Reg8Data8(GC0307_ID, 0x8c, 0x07);
    sccb_write_Reg8Data8(GC0307_ID, 0x50, 0x0c);
    sccb_write_Reg8Data8(GC0307_ID, 0x77, 0x80);
    sccb_write_Reg8Data8(GC0307_ID, 0xa1, 0x40);
    sccb_write_Reg8Data8(GC0307_ID, 0x7a, 0x00);
    sccb_write_Reg8Data8(GC0307_ID, 0x78, 0x00);
    sccb_write_Reg8Data8(GC0307_ID, 0x79, 0x00);
    sccb_write_Reg8Data8(GC0307_ID, 0x7b, 0x40);
    sccb_write_Reg8Data8(GC0307_ID, 0x7c, 0x00);
#endif	
	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif

#ifdef	__GC0308_DRV_C_
//====================================================================================================
//	Description:	OV7670 Initialization
//	Syntax:			void OV7670_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
void GC0308_Init (
	INT16S nWidthH,			// Active H Width
	INT16S nWidthV,			// Active V Width
	INT16U uFlag				// Flag Type
) {
	INT16U uCtrlReg1, uCtrlReg2;
	INT16S nReso;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif	

	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;								// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)	{									// RGB1555
		uCtrlReg2 |= CSI_RGB1555;
	}
	
	if (uFlag & FT_CSI_CCIR656) {									// CCIR656?
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	} else {
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	
	if (uFlag & FT_CSI_YUVIN) {										// YUVIN?
		uCtrlReg1 |= YUVIN;
	}
	
	if (uFlag & FT_CSI_YUVOUT) {									// YUVOUT?
		uCtrlReg1 |= YUVOUT;
	}

	// Whether compression or not?
	nReso = ((nWidthH == 320) && (nWidthV == 240)) ? 1 : 0;
	if (nReso == 1)								// VGA
	{
#ifdef	__TV_QVGA__
		R_CSI_TG_HRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_VRATIO = 0x0102;					// Scale to 1/2
		R_CSI_TG_HWIDTH = nWidthH;					// Horizontal frame width
		R_CSI_TG_VHEIGHT = nWidthV*2;				// Vertical frame width
#endif	// __TV_QVGA__
	}
	else
	{
		R_CSI_TG_HRATIO = 0;
		R_CSI_TG_VRATIO = 0;
	}

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.

	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	drv_msec_wait(100); 							//wait 100ms for CLKO stable

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay (200);
	
	//640x480 init registers code.                                         
	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x80);  	
	
	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x00);  // set page0
	
	sccb_write_Reg8Data8(GC0308_ID, 0xd2, 0x10);  // close AEC
	sccb_write_Reg8Data8(GC0308_ID, 0x22, 0x55);  // close AWB
	
	sccb_write_Reg8Data8(GC0308_ID, 0x5a, 0x56);
	sccb_write_Reg8Data8(GC0308_ID, 0x5b, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x5c, 0x4a);			
	
	sccb_write_Reg8Data8(GC0308_ID, 0x22, 0x57); // Open AWB
	
#if 1
	sccb_write_Reg8Data8(GC0308_ID, 0x01, 0xce);
	sccb_write_Reg8Data8(GC0308_ID, 0x02, 0x70);
	sccb_write_Reg8Data8(GC0308_ID, 0x0f, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xe2, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xe3, 0x96);
	sccb_write_Reg8Data8(GC0308_ID, 0xe4, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe5, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xe6, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe7, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xe8, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe9, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xea, 0x0c);
	sccb_write_Reg8Data8(GC0308_ID, 0xeb, 0xbe);
	sccb_write_Reg8Data8(GC0308_ID, 0xec, 0x20);
#else
	sccb_write_Reg8Data8(GC0308_ID, 0x01, 0x6a);
	sccb_write_Reg8Data8(GC0308_ID, 0x02, 0x70);
	sccb_write_Reg8Data8(GC0308_ID, 0x0f, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xe2, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xe3, 0x96);
	sccb_write_Reg8Data8(GC0308_ID, 0xe4, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe5, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xe6, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe7, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xe8, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xe9, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0xea, 0x04);
	sccb_write_Reg8Data8(GC0308_ID, 0xeb, 0xb0);
	sccb_write_Reg8Data8(GC0308_ID, 0xec, 0x20);
#endif
	
	sccb_write_Reg8Data8(GC0308_ID, 0x05, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x06, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x07, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x08, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x09, 0x01);
	sccb_write_Reg8Data8(GC0308_ID, 0x0a, 0xe8);
	sccb_write_Reg8Data8(GC0308_ID, 0x0b, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0x0c, 0x88);
	sccb_write_Reg8Data8(GC0308_ID, 0x0d, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0x0e, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0x10, 0x22);
	sccb_write_Reg8Data8(GC0308_ID, 0x11, 0xfd);
	sccb_write_Reg8Data8(GC0308_ID, 0x12, 0x2a);
	sccb_write_Reg8Data8(GC0308_ID, 0x13, 0x00);

	sccb_write_Reg8Data8(GC0308_ID, 0x14, 0x12);// change direction  10:normal , 11:H SWITCH,12: V SWITCH, 13:H&V SWITCH

	sccb_write_Reg8Data8(GC0308_ID, 0x15, 0x0a);
	sccb_write_Reg8Data8(GC0308_ID, 0x16, 0x05);
	sccb_write_Reg8Data8(GC0308_ID, 0x17, 0x01);
	sccb_write_Reg8Data8(GC0308_ID, 0x18, 0x44);
	sccb_write_Reg8Data8(GC0308_ID, 0x19, 0x44);
	sccb_write_Reg8Data8(GC0308_ID, 0x1a, 0x1e);
	sccb_write_Reg8Data8(GC0308_ID, 0x1b, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x1c, 0xc1);
	sccb_write_Reg8Data8(GC0308_ID, 0x1d, 0x08);
	sccb_write_Reg8Data8(GC0308_ID, 0x1e, 0x60);
	sccb_write_Reg8Data8(GC0308_ID, 0x1f, 0x16); //pad drv ,00 03 13 1f 3f james remarked

	sccb_write_Reg8Data8(GC0308_ID, 0x20, 0xff);
	sccb_write_Reg8Data8(GC0308_ID, 0x21, 0xf8);
	sccb_write_Reg8Data8(GC0308_ID, 0x22, 0x57);
	sccb_write_Reg8Data8(GC0308_ID, 0x24, 0xa2); 	// YUV
	//sccb_write_Reg8Data8(GC0308_ID, 0x24, 0xa6);  // RGB
	sccb_write_Reg8Data8(GC0308_ID, 0x25, 0x0f);

	//output sync_mode       
	sccb_write_Reg8Data8(GC0308_ID, 0x26, 0x02);//vsync  maybe need changed, value is 0x02
	sccb_write_Reg8Data8(GC0308_ID, 0x2f, 0x01);
	sccb_write_Reg8Data8(GC0308_ID, 0x30, 0xf7);
	sccb_write_Reg8Data8(GC0308_ID, 0x31, 0x50);
	sccb_write_Reg8Data8(GC0308_ID, 0x32, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x39, 0x04);
	sccb_write_Reg8Data8(GC0308_ID, 0x3a, 0x18);
	sccb_write_Reg8Data8(GC0308_ID, 0x3b, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0x3c, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x3d, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x3e, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x3f, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x50, 0x10);
	sccb_write_Reg8Data8(GC0308_ID, 0x53, 0x82);
	sccb_write_Reg8Data8(GC0308_ID, 0x54, 0x80);
	sccb_write_Reg8Data8(GC0308_ID, 0x55, 0x80);
	sccb_write_Reg8Data8(GC0308_ID, 0x56, 0x82);

	sccb_write_Reg8Data8(GC0308_ID, 0x57, 0x80);  // R
	sccb_write_Reg8Data8(GC0308_ID, 0x58, 0x80);  // G
	sccb_write_Reg8Data8(GC0308_ID, 0x59, 0x80);  // B

	sccb_write_Reg8Data8(GC0308_ID, 0x8b, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x8c, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x8d, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x8e, 0x2e);
	sccb_write_Reg8Data8(GC0308_ID, 0x8f, 0x2e);
	sccb_write_Reg8Data8(GC0308_ID, 0x90, 0x2e);
	sccb_write_Reg8Data8(GC0308_ID, 0x91, 0x3c);
	sccb_write_Reg8Data8(GC0308_ID, 0x92, 0x50);
	sccb_write_Reg8Data8(GC0308_ID, 0x5d, 0x12);
	sccb_write_Reg8Data8(GC0308_ID, 0x5e, 0x1a);
	sccb_write_Reg8Data8(GC0308_ID, 0x5f, 0x24);
	sccb_write_Reg8Data8(GC0308_ID, 0x60, 0x07);
	sccb_write_Reg8Data8(GC0308_ID, 0x61, 0x15);
	sccb_write_Reg8Data8(GC0308_ID, 0x62, 0x08);
	sccb_write_Reg8Data8(GC0308_ID, 0x64, 0x03);
	sccb_write_Reg8Data8(GC0308_ID, 0x66, 0xe8);
	sccb_write_Reg8Data8(GC0308_ID, 0x67, 0x86);
	sccb_write_Reg8Data8(GC0308_ID, 0x68, 0xa2);
	sccb_write_Reg8Data8(GC0308_ID, 0x69, 0x18);
	sccb_write_Reg8Data8(GC0308_ID, 0x6a, 0x0f);
	sccb_write_Reg8Data8(GC0308_ID, 0x6b, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x6c, 0x5f);
	sccb_write_Reg8Data8(GC0308_ID, 0x6d, 0x8f);
	sccb_write_Reg8Data8(GC0308_ID, 0x6e, 0x55);
	sccb_write_Reg8Data8(GC0308_ID, 0x6f, 0x38);
	sccb_write_Reg8Data8(GC0308_ID, 0x70, 0x15);
	sccb_write_Reg8Data8(GC0308_ID, 0x71, 0x33);
	sccb_write_Reg8Data8(GC0308_ID, 0x72, 0xdc);
	sccb_write_Reg8Data8(GC0308_ID, 0x73, 0x80);
	sccb_write_Reg8Data8(GC0308_ID, 0x74, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0x75, 0x3f);
	sccb_write_Reg8Data8(GC0308_ID, 0x76, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0x77, 0x36);
	sccb_write_Reg8Data8(GC0308_ID, 0x78, 0x88);
	sccb_write_Reg8Data8(GC0308_ID, 0x79, 0x81);
	sccb_write_Reg8Data8(GC0308_ID, 0x7a, 0x81);
	sccb_write_Reg8Data8(GC0308_ID, 0x7b, 0x22);
	sccb_write_Reg8Data8(GC0308_ID, 0x7c, 0xff);
	sccb_write_Reg8Data8(GC0308_ID, 0x93, 0x48);
	sccb_write_Reg8Data8(GC0308_ID, 0x94, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0x95, 0x05);
	sccb_write_Reg8Data8(GC0308_ID, 0x96, 0xe8);
	sccb_write_Reg8Data8(GC0308_ID, 0x97, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x98, 0xf0);
	sccb_write_Reg8Data8(GC0308_ID, 0xb1, 0x38);
	sccb_write_Reg8Data8(GC0308_ID, 0xb2, 0x38);
	sccb_write_Reg8Data8(GC0308_ID, 0xbd, 0x38);
	sccb_write_Reg8Data8(GC0308_ID, 0xbe, 0x36);
	sccb_write_Reg8Data8(GC0308_ID, 0xd0, 0xc9);
	sccb_write_Reg8Data8(GC0308_ID, 0xd1, 0x10);
	//sccb_write_Reg8Data8(GC0308_ID, 0xd2, 0x90);
	//sccb_write_Reg8Data8(GC0308_ID, 0xd3, 0x80);
	sccb_write_Reg8Data8(GC0308_ID, 0xd3, 0xA0);		 // ?úö
	sccb_write_Reg8Data8(GC0308_ID, 0xd5, 0xf2);
	sccb_write_Reg8Data8(GC0308_ID, 0xd6, 0x16);
	sccb_write_Reg8Data8(GC0308_ID, 0xdb, 0x92);
	sccb_write_Reg8Data8(GC0308_ID, 0xdc, 0xa5);
	sccb_write_Reg8Data8(GC0308_ID, 0xdf, 0x23);
	sccb_write_Reg8Data8(GC0308_ID, 0xd9, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xda, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xe0, 0x09);
	sccb_write_Reg8Data8(GC0308_ID, 0xec, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0xed, 0x04);
	sccb_write_Reg8Data8(GC0308_ID, 0xee, 0xa0);
	sccb_write_Reg8Data8(GC0308_ID, 0xef, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x80, 0x03);
	sccb_write_Reg8Data8(GC0308_ID, 0x80, 0x03);
	
#if 1	//smallest gamma curve
	sccb_write_Reg8Data8(GC0308_ID, 0x9F, 0x0B); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA0, 0x16); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA1, 0x29); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA2, 0x3C); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA3, 0x4F); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA4, 0x5F); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA5, 0x6F); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA6, 0x8A); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA7, 0x9F); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA8, 0xB4); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA9, 0xC6); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAA, 0xD3); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAB, 0xDD);  
	sccb_write_Reg8Data8(GC0308_ID, 0xAC, 0xE5);  
	sccb_write_Reg8Data8(GC0308_ID, 0xAD, 0xF1); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAE, 0xFA); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAF, 0xFF); 	
#elif 0		
	sccb_write_Reg8Data8(GC0308_ID, 0x9F, 0x0E); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA0, 0x1C); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA1, 0x34); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA2, 0x48); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA3, 0x5A); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA4, 0x6B); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA5, 0x7B); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA6, 0x95); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA7, 0xAB); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA8, 0xBF);
	sccb_write_Reg8Data8(GC0308_ID, 0xA9, 0xCE); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAA, 0xD9); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAB, 0xE4);  
	sccb_write_Reg8Data8(GC0308_ID, 0xAC, 0xEC); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAD, 0xF7); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAE, 0xFD); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAF, 0xFF); 
#elif 0	
	sccb_write_Reg8Data8(GC0308_ID, 0x9F, 0x10);
	sccb_write_Reg8Data8(GC0308_ID, 0xA0, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0xA1, 0x38);
	sccb_write_Reg8Data8(GC0308_ID, 0xA2, 0x4E);
	sccb_write_Reg8Data8(GC0308_ID, 0xA3, 0x63);
	sccb_write_Reg8Data8(GC0308_ID, 0xA4, 0x76);
	sccb_write_Reg8Data8(GC0308_ID, 0xA5, 0x87);
	sccb_write_Reg8Data8(GC0308_ID, 0xA6, 0xA2);
	sccb_write_Reg8Data8(GC0308_ID, 0xA7, 0xB8);
	sccb_write_Reg8Data8(GC0308_ID, 0xA8, 0xCA);
	sccb_write_Reg8Data8(GC0308_ID, 0xA9, 0xD8);
	sccb_write_Reg8Data8(GC0308_ID, 0xAA, 0xE3);
	sccb_write_Reg8Data8(GC0308_ID, 0xAB, 0xEB);
	sccb_write_Reg8Data8(GC0308_ID, 0xAC, 0xF0);
	sccb_write_Reg8Data8(GC0308_ID, 0xAD, 0xF8);
	sccb_write_Reg8Data8(GC0308_ID, 0xAE, 0xFD);
	sccb_write_Reg8Data8(GC0308_ID, 0xAF, 0xFF);
#elif 0		
	sccb_write_Reg8Data8(GC0308_ID, 0x9F, 0x14); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA0, 0x28); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA1, 0x44); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA2, 0x5D); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA3, 0x72); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA4, 0x86); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA5, 0x95); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA6, 0xB1); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA7, 0xC6); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA8, 0xD5); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA9, 0xE1); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAA, 0xEA); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAB, 0xF1); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAC, 0xF5); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAD, 0xFB); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAE, 0xFE); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAF, 0xFF);
#else	// largest gamma curve
	sccb_write_Reg8Data8(GC0308_ID, 0x9F, 0x15); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA0, 0x2A); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA1, 0x4A); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA2, 0x67); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA3, 0x79); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA4, 0x8C); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA5, 0x9A); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA6, 0xB3); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA7, 0xC5); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA8, 0xD5); 
	sccb_write_Reg8Data8(GC0308_ID, 0xA9, 0xDF); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAA, 0xE8); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAB, 0xEE); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAC, 0xF3); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAD, 0xFA); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAE, 0xFD); 
	sccb_write_Reg8Data8(GC0308_ID, 0xAF, 0xFF);
#endif	
	
	sccb_write_Reg8Data8(GC0308_ID, 0xc0, 0x00);
	sccb_write_Reg8Data8(GC0308_ID, 0xc1, 0x10);
	sccb_write_Reg8Data8(GC0308_ID, 0xc2, 0x1C);
	sccb_write_Reg8Data8(GC0308_ID, 0xc3, 0x30);
	sccb_write_Reg8Data8(GC0308_ID, 0xc4, 0x43);
	sccb_write_Reg8Data8(GC0308_ID, 0xc5, 0x54);
	sccb_write_Reg8Data8(GC0308_ID, 0xc6, 0x65);
	sccb_write_Reg8Data8(GC0308_ID, 0xc7, 0x75);
	sccb_write_Reg8Data8(GC0308_ID, 0xc8, 0x93);
	sccb_write_Reg8Data8(GC0308_ID, 0xc9, 0xB0);
	sccb_write_Reg8Data8(GC0308_ID, 0xca, 0xCB);
	sccb_write_Reg8Data8(GC0308_ID, 0xcb, 0xE6);
	sccb_write_Reg8Data8(GC0308_ID, 0xcc, 0xFF);
	sccb_write_Reg8Data8(GC0308_ID, 0xf0, 0x02);
	sccb_write_Reg8Data8(GC0308_ID, 0xf1, 0x01);
	sccb_write_Reg8Data8(GC0308_ID, 0xf2, 0x01);
	sccb_write_Reg8Data8(GC0308_ID, 0xf3, 0x30);
	sccb_write_Reg8Data8(GC0308_ID, 0xf9, 0x9f);
	sccb_write_Reg8Data8(GC0308_ID, 0xfa, 0x78);

	//---------------------------------------------------------------
	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x01);  //set page 1

	sccb_write_Reg8Data8(GC0308_ID, 0x00, 0xf5);
	sccb_write_Reg8Data8(GC0308_ID, 0x02, 0x1a);
	sccb_write_Reg8Data8(GC0308_ID, 0x0a, 0xa0);
	sccb_write_Reg8Data8(GC0308_ID, 0x0b, 0x60);
	sccb_write_Reg8Data8(GC0308_ID, 0x0c, 0x08);
	sccb_write_Reg8Data8(GC0308_ID, 0x0e, 0x4c);
	sccb_write_Reg8Data8(GC0308_ID, 0x0f, 0x39);
	
	sccb_write_Reg8Data8(GC0308_ID, 0x11, 0x3f);
	//sccb_write_Reg8Data8(GC0308_ID, 0x11 ,0x37);  // bit3 = 0

	sccb_write_Reg8Data8(GC0308_ID, 0x12, 0x72);
	sccb_write_Reg8Data8(GC0308_ID, 0x13, 0x13);
	sccb_write_Reg8Data8(GC0308_ID, 0x14, 0x42);
	sccb_write_Reg8Data8(GC0308_ID, 0x15, 0x43);
	sccb_write_Reg8Data8(GC0308_ID, 0x16, 0xc2);
	sccb_write_Reg8Data8(GC0308_ID, 0x17, 0xa8);
	sccb_write_Reg8Data8(GC0308_ID, 0x18, 0x18);
	sccb_write_Reg8Data8(GC0308_ID, 0x19, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x1a, 0xd0);
	sccb_write_Reg8Data8(GC0308_ID, 0x1b, 0xf5);
	sccb_write_Reg8Data8(GC0308_ID, 0x70, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x71, 0x58);
	sccb_write_Reg8Data8(GC0308_ID, 0x72, 0x30);
	sccb_write_Reg8Data8(GC0308_ID, 0x73, 0x48);
	sccb_write_Reg8Data8(GC0308_ID, 0x74, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0x75, 0x60);
	sccb_write_Reg8Data8(GC0308_ID, 0x77, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0x78, 0x32);
	sccb_write_Reg8Data8(GC0308_ID, 0x30, 0x03);
	sccb_write_Reg8Data8(GC0308_ID, 0x31, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0x32, 0xe0);
	sccb_write_Reg8Data8(GC0308_ID, 0x33, 0xe0);
	sccb_write_Reg8Data8(GC0308_ID, 0x34, 0xe0);
	sccb_write_Reg8Data8(GC0308_ID, 0x35, 0xb0);
	sccb_write_Reg8Data8(GC0308_ID, 0x36, 0xc0);
	sccb_write_Reg8Data8(GC0308_ID, 0x37, 0xc0);
	sccb_write_Reg8Data8(GC0308_ID, 0x38, 0x04);
	sccb_write_Reg8Data8(GC0308_ID, 0x39, 0x09);
	sccb_write_Reg8Data8(GC0308_ID, 0x3a, 0x12);
	sccb_write_Reg8Data8(GC0308_ID, 0x3b, 0x1C);
	sccb_write_Reg8Data8(GC0308_ID, 0x3c, 0x28);
	sccb_write_Reg8Data8(GC0308_ID, 0x3d, 0x31);
	sccb_write_Reg8Data8(GC0308_ID, 0x3e, 0x44);
	sccb_write_Reg8Data8(GC0308_ID, 0x3f, 0x57);
	sccb_write_Reg8Data8(GC0308_ID, 0x40, 0x6C);
	sccb_write_Reg8Data8(GC0308_ID, 0x41, 0x81);
	sccb_write_Reg8Data8(GC0308_ID, 0x42, 0x94);
	sccb_write_Reg8Data8(GC0308_ID, 0x43, 0xA7);
	sccb_write_Reg8Data8(GC0308_ID, 0x44, 0xB8);
	sccb_write_Reg8Data8(GC0308_ID, 0x45, 0xD6);
	sccb_write_Reg8Data8(GC0308_ID, 0x46, 0xEE);
	sccb_write_Reg8Data8(GC0308_ID, 0x47, 0x0d);

	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x00);  

	sccb_write_Reg8Data8(GC0308_ID, 0xd2, 0x90); // Open AEC at last.  

	/////////////////////////////////////////////////////////
	//-----------Update the registers -------------//
	///////////////////////////////////////////////////////////

	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x00);//set Page0

	sccb_write_Reg8Data8(GC0308_ID, 0x10, 0x26);                               
	sccb_write_Reg8Data8(GC0308_ID, 0x11, 0x0d);// fd,modified by mormo 2010/07/06                               
	sccb_write_Reg8Data8(GC0308_ID, 0x1a, 0x2a);// 1e,modified by mormo 2010/07/06                                  

	sccb_write_Reg8Data8(GC0308_ID, 0x1c, 0x49); // c1,modified by mormo 2010/07/06                                 
	sccb_write_Reg8Data8(GC0308_ID, 0x1d, 0x9a); // 08,modified by mormo 2010/07/06                                 
	sccb_write_Reg8Data8(GC0308_ID, 0x1e, 0x61); // 60,modified by mormo 2010/07/06                                 
	//sccb_write_Reg8Data8(GC0308_ID, 0x1f, 0x16); // io driver current 
	
	sccb_write_Reg8Data8(GC0308_ID, 0x3a, 0x20);

	sccb_write_Reg8Data8(GC0308_ID, 0x50, 0x14);// 10,modified by mormo 2010/07/06                               
	sccb_write_Reg8Data8(GC0308_ID, 0x53, 0x80);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x56, 0x80);

	sccb_write_Reg8Data8(GC0308_ID, 0x8b, 0x20); //LSC                                 
	sccb_write_Reg8Data8(GC0308_ID, 0x8c, 0x20);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x8d, 0x20);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x8e, 0x14);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x8f, 0x10);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x90, 0x14);                                

	sccb_write_Reg8Data8(GC0308_ID, 0x94, 0x02);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x95, 0x07);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x96, 0xe0);                                

	sccb_write_Reg8Data8(GC0308_ID, 0xb1, 0x40); // YCPT                                 
	sccb_write_Reg8Data8(GC0308_ID, 0xb2, 0x40);                                
	sccb_write_Reg8Data8(GC0308_ID, 0xb3, 0x40);
	sccb_write_Reg8Data8(GC0308_ID, 0xb6, 0xe0);

	sccb_write_Reg8Data8(GC0308_ID, 0xd0, 0xcb); // AECT  c9,modifed by mormo 2010/07/06                                
	sccb_write_Reg8Data8(GC0308_ID, 0xd3, 0x48); // 80,modified by mormor 2010/07/06                           

	sccb_write_Reg8Data8(GC0308_ID, 0xf2, 0x02);                                
	sccb_write_Reg8Data8(GC0308_ID, 0xf7, 0x12);
	sccb_write_Reg8Data8(GC0308_ID, 0xf8, 0x0a);

	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x01);//set  Page1

	sccb_write_Reg8Data8(GC0308_ID, 0x02, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0x04, 0x10);
	sccb_write_Reg8Data8(GC0308_ID, 0x05, 0x08);
	sccb_write_Reg8Data8(GC0308_ID, 0x06, 0x20);
	sccb_write_Reg8Data8(GC0308_ID, 0x08, 0x0a);

	sccb_write_Reg8Data8(GC0308_ID, 0x0e, 0x44);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x0f, 0x32);
	sccb_write_Reg8Data8(GC0308_ID, 0x10, 0x41);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x11, 0x37);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x12, 0x22);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x13, 0x19);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x14, 0x44);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x15, 0x44);

	sccb_write_Reg8Data8(GC0308_ID, 0x19, 0x50);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x1a, 0xd8); 

	sccb_write_Reg8Data8(GC0308_ID, 0x32, 0x10); 

	sccb_write_Reg8Data8(GC0308_ID, 0x35, 0x00);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x36, 0x80);                                
	sccb_write_Reg8Data8(GC0308_ID, 0x37, 0x00); 
	//-----------Update the registers end---------//

	sccb_write_Reg8Data8(GC0308_ID, 0xfe, 0x00);// set back for page1

	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif
}
#endif

#ifdef	__OV3640_MIPI_DRV_C__
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)  
//====================================================================================================
//	Description:	OV3640 + mipi Initialization
//	Syntax:			void OV3640_MIPI_Init (
//						INT16S nWidthH,			// Active H Width
//						INT16S nWidthV,			// Active V Width
//						INT16U uFlag				// Flag Type
//					);
//	Return:			None
//====================================================================================================
//max resolution: 2048*1536
#define COLOR_BAR_EN	0
#define OV3640_YUYV		0x00
#define OV3640_UYVY		0x02
#define OV3640_BGGR		0x18
#define OV3640_GBRG		0x19
#define OV3640_GRBG		0x1A
#define OV3640_RGGB		0x1B
#define OV3640_FMT 		OV3640_YUYV	

mipi_config_t ov3640_mipi_param = {
	2048, 				//width
	1536,				//height
	DISABLE,			//low_power_en
	D_PHY_SAMPLE_POS,	//byte_clk_edge
	DISABLE, 			//data_from_mmr
	MIPI_YUV422,		//mipi_format
	ENABLE,				//ecc_check_en
	MIPI_ECC_ORDER3, 	//ecc_order
	0x08, 				//data_mask_time
	MIPI_CHECK_HS_SEQ,	//check_hs_seq 
};

void 
OV3640_MIPI_Init(
	INT16S width, 
	INT16S height,
	INT16U uFlag
)
{
	INT16U uCtrlReg1, uCtrlReg2;
	
	// Enable CSI clock to let sensor initialize at first
#if CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV2
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_27MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL &= ~0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_SYS_CLK_DIV4
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL48M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#elif CSI_CLOCK == CSI_CLOCK_13_5MHZ
	uCtrlReg2 = CLKOEN | CSI_RGB565 |CLK_SEL27M | CSI_HIGHPRI | CSI_NOSTOP;
	R_SYSTEM_CTRL |= 0x4000; 
#endif		
	
	uCtrlReg1 = CSIEN | YUV_YUYV | CAP;									// Default CSI Control Register 1
	if (uFlag & FT_CSI_RGB1555)											// RGB1555
	{
		uCtrlReg2 |= CSI_RGB1555;
	}
	if (uFlag & FT_CSI_CCIR656)										// CCIR656?
	{
		uCtrlReg1 |= CCIR656 | VADD_FALL | VRST_FALL | HRST_FALL;	// CCIR656
		uCtrlReg2 |= D_TYPE1;										// CCIR656
	}
	else
	{
		uCtrlReg1 |= VADD_RISE | VRST_FALL | HRST_RISE | HREF;		// NOT CCIR656
		uCtrlReg2 |= D_TYPE0;										// NOT CCIR656
	}
	if (uFlag & FT_CSI_YUVIN)										// YUVIN?
	{
		uCtrlReg1 |= YUVIN;
	}
	if (uFlag & FT_CSI_YUVOUT)										// YUVOUT?
	{
		uCtrlReg1 |= YUVOUT;
	}
	
	R_CSI_TG_HRATIO = 0;					
	R_CSI_TG_VRATIO = 0;					

	R_CSI_TG_VL0START = 0x0000;						// Sensor field 0 vertical latch start register.
	R_CSI_TG_VL1START = 0x0000;						//*P_Sensor_TG_V_L1Start = 0x0000;				
	R_CSI_TG_HSTART = 0x0000;						// Sensor horizontal start register.
	
	R_CSI_TG_CTRL0 = 0;								//reset control0
	R_CSI_TG_CTRL1 = CSI_NOSTOP|CLKOEN;				//enable CSI CLKO
	//gpio_drving_init_io(IO_H0,IOH_DRV_16mA);		//set mclk driving 
	
	/* mipi interface parameter set */
	mipi_init(MIPI_DATA_TO_CSI);	
	ov3640_mipi_param.width = width;
	ov3640_mipi_param.height = height;
	mipi_set_parameter(&ov3640_mipi_param);

	// CMOS Sensor Initialization Start...
	sccb_init();
	sccb_delay(200);
    sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x80);   //
    sccb_delay(2000);

#if 0 //FPGA
	sccb_write_Reg16Data8(OV3640_ID, 0x304d, 0x41);   // ;45 ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x16);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30aa, 0x45);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30b0, 0xff);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30b1, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x30b2, 0x10);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x10);   // ;Rev2A
	
	sccb_write_Reg16Data8(OV3640_ID, 0x309e, 0x00);   // ;terry
	sccb_write_Reg16Data8(OV3640_ID, 0x3602, 0x26);   // ;2a ;SOL/EOL on
	sccb_write_Reg16Data8(OV3640_ID, 0x3603, 0x4D);   // ;ecc 
	sccb_write_Reg16Data8(OV3640_ID, 0x364c, 0x04);   // ;ecc 
	sccb_write_Reg16Data8(OV3640_ID, 0x360c, 0x12);   // ;virtual channel 0
	sccb_write_Reg16Data8(OV3640_ID, 0x361e, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x361f, 0x11);   // ;pclk_period, terry
	sccb_write_Reg16Data8(OV3640_ID, 0x3633, 0x32);   // ;increase hs_prepare
	sccb_write_Reg16Data8(OV3640_ID, 0x3629, 0x3c);   // ;increase clk_prepare
	
	//sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x39);   // ;30fps terry
	//sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0xa1);   // ;terry
		
#if (OV3640_FMT == OV3640_UYVY) || (OV3640_FMT == OV3640_YUYV)
	sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x37);   // ;15fps terry
	sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0xa3);   // ;terry
#else 
	//bypass PLL
	sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x3b);   // ;15fps terry
	sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0xa8);   // ;terry
#endif
	
	sccb_write_Reg16Data8(OV3640_ID, 0x3010, 0x80);   // ;high mipi spd, 81 ;terry
	sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x81);   // ;Rev2A
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3018, 0x38);   // ;aec
	sccb_write_Reg16Data8(OV3640_ID, 0x3019, 0x30);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x301a, 0x61);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x307d, 0x00);   // ;aec isp 06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x02);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x3082, 0x20);   // ;06142007
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x303c, 0x08);   // ;aec weight
	sccb_write_Reg16Data8(OV3640_ID, 0x303d, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x303e, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x303f, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3030, 0x62);   //	
	sccb_write_Reg16Data8(OV3640_ID, 0x3031, 0x26);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3032, 0xe6);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3033, 0x6e);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3034, 0xea);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3035, 0xae);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3036, 0xa6);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3037, 0x6a);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3015, 0x12);   // ;07182007 8x gain, auto 1/2
	sccb_write_Reg16Data8(OV3640_ID, 0x3014, 0x04);   // ;06142007 auto frame off
	sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);   // ;07182007
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3104, 0x02);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3105, 0xfd);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3106, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3107, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3308, 0xa5);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3316, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3317, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x02);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3082, 0x20);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3300, 0x13);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3301, 0xde);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x30b8, 0x20);   // ;10
	sccb_write_Reg16Data8(OV3640_ID, 0x30b9, 0x17);   // ;18
	sccb_write_Reg16Data8(OV3640_ID, 0x30ba, 0x04);   // ;00
	sccb_write_Reg16Data8(OV3640_ID, 0x30bb, 0x08);   // ;1f          
	//Format
	sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x02);   // ;YUV
	sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x00);   //
#if OV3640_FMT == OV3640_YUYV
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_YUYV); // yuyv to csi  
#elif OV3640_FMT == OV3640_UYVY
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_UYVY); // uyvy     
#else
	//sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x22);
	//sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0x02); 
	//sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x03); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_GRBG); 
#endif
		
	//Size, 2048x1536
	sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);   // ;QXGA
	sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x0a);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x68);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x68);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0x04);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0x00);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3507, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x350a, 0x4f);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3600, 0xc4);   //
	
	if(width == 640 && height == 480)
	{// @@ OV3640 VGA key setting.		
		sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x06);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x24);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x90);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3069, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x34);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x0c);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x12);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x88);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0xe4);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);   //

		sccb_write_Reg16Data8(OV3640_ID, 0x302c, 0x0e);   //EXHTS
		sccb_write_Reg16Data8(OV3640_ID, 0x302d, 0x00);   //EXVTS[15:8]
		sccb_write_Reg16Data8(OV3640_ID, 0x302e, 0x10);   //EXVTS[7:0]

		sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x02);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x80);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0xe0);   //
	}
#else    
	sccb_write_Reg16Data8(OV3640_ID, 0x304d, 0x41);   // ;45 ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x16);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30aa, 0x45);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30b0, 0xff);   // ;Rev2A
	sccb_write_Reg16Data8(OV3640_ID, 0x30b1, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x30b2, 0x10);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x10);   // ;Rev2A
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x309e, 0x00);   // ;terry
	sccb_write_Reg16Data8(OV3640_ID, 0x3602, 0x26);   // ;2a ;SOL/EOL on
	sccb_write_Reg16Data8(OV3640_ID, 0x3603, 0x4D);   // ;ecc 
	sccb_write_Reg16Data8(OV3640_ID, 0x364c, 0x04);   // ;ecc 
	sccb_write_Reg16Data8(OV3640_ID, 0x360c, 0x12);   // ;virtual channel 0
	sccb_write_Reg16Data8(OV3640_ID, 0x361e, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x361f, 0x11);   // ;pclk_period, terry
	sccb_write_Reg16Data8(OV3640_ID, 0x3633, 0x32);   // ;increase hs_prepare
	sccb_write_Reg16Data8(OV3640_ID, 0x3629, 0x3c);   // ;increase clk_prepare
	
	sccb_write_Reg16Data8(OV3640_ID, 0x300e, 0x38);   // ;30fps terry
	sccb_write_Reg16Data8(OV3640_ID, 0x300f, 0xa2);   // ;terry //a1:1.5 , a2:2
#if 1	
	//1 lane
	sccb_write_Reg16Data8(OV3640_ID, 0x3010, 0x80);   // ;high mipi spd, 81 ;terry
#else
    //2 lanes
	sccb_write_Reg16Data8(OV3640_ID, 0x3010, 0xa0);   // ;high mipi spd, 81 ;terry
#endif
	sccb_write_Reg16Data8(OV3640_ID, 0x3011, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x304c, 0x81);   // ;Rev2A
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3018, 0x38);   // ;aec
	sccb_write_Reg16Data8(OV3640_ID, 0x3019, 0x30);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x301a, 0x61);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x307d, 0x00);   // ;aec isp 06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x02);   // ;06142007
	sccb_write_Reg16Data8(OV3640_ID, 0x3082, 0x20);   // ;06142007
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x303c, 0x08);   // ;aec weight
	sccb_write_Reg16Data8(OV3640_ID, 0x303d, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x303e, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x303f, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3030, 0x62);   //	
	sccb_write_Reg16Data8(OV3640_ID, 0x3031, 0x26);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3032, 0xe6);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3033, 0x6e);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3034, 0xea);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3035, 0xae);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3036, 0xa6);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3037, 0x6a);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3015, 0x12);   // ;07182007 8x gain, auto 1/2
	sccb_write_Reg16Data8(OV3640_ID, 0x3014, 0x04);   // ;06142007 auto frame off
	sccb_write_Reg16Data8(OV3640_ID, 0x3013, 0xf7);   // ;07182007
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3104, 0x02);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3105, 0xfd);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3106, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3107, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3308, 0xa5);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3316, 0xff);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3317, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3087, 0x02);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3082, 0x20);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3300, 0x13);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3301, 0xde);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x30b8, 0x20);   // ;10
	sccb_write_Reg16Data8(OV3640_ID, 0x30b9, 0x17);   // ;18
	sccb_write_Reg16Data8(OV3640_ID, 0x30ba, 0x04);   // ;00
	sccb_write_Reg16Data8(OV3640_ID, 0x30bb, 0x08);   // ;1f          
	//Format
#if OV3640_FMT == OV3640_YUYV
	sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x02);   // ;YUV
	sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_YUYV); // uyvy for cdsp mipi    
#elif OV3640_FMT == OV3640_UYVY
	sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x02);   // ;YUV
	sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_UYVY); // uyvy for cdsp mipi    
#else
	sccb_write_Reg16Data8(OV3640_ID, 0x3100, 0x22);
	sccb_write_Reg16Data8(OV3640_ID, 0x3304, 0x01); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3400, 0x03); 
	sccb_write_Reg16Data8(OV3640_ID, 0x3600, 0xC4);
	sccb_write_Reg16Data8(OV3640_ID, 0x3404, OV3640_RGGB);  
#endif
		
	//Size, 2048x1536
	sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);   // ;QXGA
	sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x0a);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x68);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x18);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x0c);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x68);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0x04);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x08);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x00);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0x00);   //
	//
	sccb_write_Reg16Data8(OV3640_ID, 0x3507, 0x06);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x350a, 0x4f);   //
	sccb_write_Reg16Data8(OV3640_ID, 0x3600, 0xc4);   //
	
	if(width == 640 && height == 480){
		// @@ OV3640 VGA key setting.		
		sccb_write_Reg16Data8(OV3640_ID, 0x3012, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3020, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3021, 0x1d);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3022, 0x00);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3023, 0x06);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3024, 0x08);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3025, 0x18);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3026, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3027, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302a, 0x03);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x302b, 0x10);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3075, 0x24);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x300d, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x30d7, 0x90);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3069, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3302, 0xef);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x335f, 0x34);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3360, 0x0c);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3361, 0x04);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3362, 0x12);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3363, 0x88);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3364, 0xe4);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3403, 0x42);   //

		sccb_write_Reg16Data8(OV3640_ID, 0x302c, 0x0e);   //EXHTS
		sccb_write_Reg16Data8(OV3640_ID, 0x302d, 0x00);   //EXVTS[15:8]
		sccb_write_Reg16Data8(OV3640_ID, 0x302e, 0x10);   //EXVTS[7:0]

		sccb_write_Reg16Data8(OV3640_ID, 0x3088, 0x02);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x3089, 0x80);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x308a, 0x01);   //
		sccb_write_Reg16Data8(OV3640_ID, 0x308b, 0xe0);   //
	}
#endif	
#if COLOR_BAR_EN == 1
	sccb_write_Reg16Data8(OV3640_ID, 0x307B, 0x4a);   //color bar[1:0]
	sccb_write_Reg16Data8(OV3640_ID, 0x307D, 0xa0);   //color bar[7]
	sccb_write_Reg16Data8(OV3640_ID, 0x306C, 0x00);   //color bar[4]
	sccb_write_Reg16Data8(OV3640_ID, 0x3080, 0x11);   //color bar[7] enable	
#endif

	R_CSI_TG_CTRL1 = uCtrlReg2;					//*P_Sensor_TG_Ctrl2 = uCtrlReg2;
#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ	
	R_CSI_TG_CTRL0 = uCtrlReg1;					//*P_Sensor_TG_Ctrl1 = uCtrlReg1;
#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (1 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (2 << 20) | (1 << 16);
#elif CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ
	R_CSI_TG_CTRL0 = uCtrlReg1 | (3 << 20) | (1 << 16);
#endif	
}
#endif //_DRV_L1_MIPI
#endif //__OV3640_MIPI_DRV_C__

#ifdef __MT9V112_DRV_C__
//====================================================================================================
// Description:	MT9V112 Initialization
// Syntax: void mt9v112_init(
//			INT16S nWidthH,			// Active H Width
//			INT16S nWidthV,			// Active V Width
//			INT16U uFlag			// Flag Type
//			);
// Return: None
//====================================================================================================
#include "mt9v112.c"
#endif

#ifdef __GC0308_DRV_C__
//====================================================================================================
// Description:	GC0308 Initialization
// Syntax: void gc0308_init(
//			INT16S nWidthH,			// Active H Width
//			INT16S nWidthV,			// Active V Width
//			INT16U uFlag			// Flag Type
//			);
// Return: None
//====================================================================================================
#include "gc0308.c"
#endif

//====================================================================================================
//	Description:	Initial CMOS sensor
//	Function:		CSI_Init (nWidthH, nWidthV, uFlag, uFrmBuf0, uFrmBuf1, uFrmBuf2)
//	Syntax:			void CSI_Init (INT16S nWidthH, INT16S nWidthV, INT16U uFlag, INT32U uFrmBuf0, INT32U uFrmBuf1, INT32U uFrmBuf2)
//	Input Paramter:	INT16S nWidthH: Active horizontal width
//					INT16S nWidthV: Active vertical width
//					INT16U uFlag: Flag type selection
//					INT32U uFrmBuf0: Frame buffer index 0
//	Return: 		none
//====================================================================================================
void CSI_Init (
	INT16S nWidthH, 
	INT16S nWidthV, 
	INT16U uFlag, 
	INT32U uFrmBuf0, 
	INT32U uFrmBuf1
){
	// Setup sensor frame buffer start address
	*P_CSI_TG_FBSADDR = uFrmBuf0;
#if (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ)
	*P_CSI_TG_FBSADDR_B = uFrmBuf1;	
#endif	
	// Setup sensor frame size
#ifdef	__OID_SENSOR_DRV_C__
	// In OID, one pixel is 1 byte.
	// In others, one pixel is 2 bytes
	R_CSI_TG_HWIDTH = nWidthH/2;			// Horizontal frame width
#else
	R_CSI_TG_HWIDTH = nWidthH;				// Horizontal frame width
#endif

	R_CSI_TG_VHEIGHT = nWidthV;				// Vertical frame width
	//
	//	CMOS Sensor Interface (CSI) H/W Initialization
	//

#ifdef	__HSB0357_DRV_C__
	HSB0357_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV6680_DRV_C__
	OV6680_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV7680_DRV_C__
	OV7680_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV7670_DRV_C__ 
	OV7670_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV9655_DRV_C__
	OV9655_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV9660_DRV_C__
	OV9660_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV2655_DRV_C__
	OV2655_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV3640_DRV_C__
	OV3640_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV5642_DRV_C__
	OV5642_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV7675_DRV_C__
	OV7675_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV2643_DRV_C__
	OV2643_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV2659_DRV_C__
	OV2659_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OID_SENSOR_DRV_C__
	gFrameBuffA = uFrmBuf0;
	OID_Sensor_Init (nWidthH, nWidthV);
#endif
#ifdef	__BF3710_DRV_C__
	BF3710_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef __GC0307_DRV_C_
	GC0307_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef __GC0308_DRV_C_
	GC0308_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__OV3640_MIPI_DRV_C__
	OV3640_MIPI_Init (nWidthH, nWidthV, uFlag);
#endif
#ifdef	__MT9V112_DRV_C__
	mt9v112_init(nWidthH, nWidthV, uFlag);
#endif
#ifdef	__GC0308_DRV_C__
	gc0308_init(nWidthH, nWidthV, uFlag);
#endif
}

//====================================================================================================
//	Description:	Wait capture delay complete
//	Function:		CSI_WaitCaptureComplete ()
//	Syntax:			void CSI_WaitCaptureComplete (void)
//	Input Paramter:	none
//	Return: 		none
//====================================================================================================
void CSI_WaitCaptureComplete(void)
{
	while ((R_PPU_IRQ_STATUS & 0x40) == 0);
	R_PPU_IRQ_STATUS = 0x40;
}

void Sensor_Bluescreen_Enable(void) 
{
	R_CSI_TG_CTRL0 |= 0x0080;
	R_CSI_TG_BSUPPER = 0x001f;
	R_CSI_TG_BSLOWER = 0x0000;
}
void Sensor_Bluescreen_Disable(void) 
{
	R_CSI_TG_CTRL0 &= 0xFF7F;
}

void Sensor_Cut_Enable(void) 
{			
	R_CSI_TG_CTRL1 |= 0x0100;	//Enable Cut function		
	R_CSI_TG_CUTSTART = 0x0808;	
	R_CSI_TG_CUTSIZE  = 0x0808;
}
void Sensor_Cut_Disable(void) 
{			
	R_CSI_TG_CTRL1 &= 0xFEFF;	
}

void Sensor_Black_Enable(void)
{
	R_CSI_TG_HSTART = 0x50;  			
	R_CSI_TG_VSTART = 0x28;
	R_CSI_TG_HEND = 0xf0;
	R_CSI_TG_VEND = 0xc8;
}

void Sensor_Black_Disable(void) 
{			
	R_CSI_TG_HSTART = 0x0000;			
	R_CSI_TG_VSTART = 0x0000;
	R_CSI_TG_HEND = 0x0140;
	R_CSI_TG_VEND = 0x00f0;
}

void Sensor_EnableFrameEndInterrupt(BOOLEAN en)
{
	if (en)
		R_PPU_IRQ_EN |= 0x00000040;
	else
		R_PPU_IRQ_EN &= ~0x00000040;
}

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L1_SENSOR) && (_DRV_L1_SENSOR == 1)        //
//================================================================//
